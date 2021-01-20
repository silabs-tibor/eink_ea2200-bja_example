/***************************************************************************//**
 * @file rail_controller.c
 * @brief Controls the radio peripheral. Rx, Tx, Idle and error handling.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *******************************************************************************
 * # Evaluation Quality
 * This code has been minimally tested to ensure that it builds and is suitable
 * as a demonstration for evaluation purposes only. This code will be maintained
 * at the sole discretion of Silicon Labs.
 ******************************************************************************/
// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <modules/tx_handler/inc/tx_handler.h>
#include <stdint.h>
#include "sl_component_catalog.h"
#include "sl_flex_assert.h"
#include "rail.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_led_instances.h"

#include "rail_controller.h"
#include "buffer_controller.h"
#include "rx_handler.h"
#include "em_core.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// Size of RAIL RX/TX FIFO
#define RAIL_FIFO_SIZE (256u)
/// Transmit data length
#define ACK_PAYLOAD_LENGTH (4u)

/// State machine of simple_trx
typedef enum {
  S_PACKET_RECEIVED,
  S_PACKET_SENT,
  S_RX_PACKET_ERROR,
  S_TX_PACKET_ERROR,
  S_CALIBRATION_ERROR,
  S_IDLE,
} state_t;

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * The function printfs the received rx message.
 *
 * @param rx_buffer Msg buffer
 * @returns None
 *****************************************************************************/
static void printf_rx_packet (const uint8_t *const rx_buffer);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

/// Flag, indicating transmit request (button has pressed / CLI transmit request has occured)
static volatile bool tx_requested = false;

/// Flag, indicating received packet is forwarded on CLI or not
static volatile bool rx_requested = true;

/// The variable shows the actual state of the state machine
static volatile state_t state = S_IDLE;

/// Contains the last RAIL Rx/Tx error events
static volatile uint64_t current_rail_err = 0;

/// Contains the status of RAIL Calibration
static volatile RAIL_Status_t calibration_status = 0;

/// RAIL Rx packet handle
static volatile RAIL_RxPacketHandle_t rx_packet_handle;

/// Receive FIFO
static uint8_t rx_fifo[RAIL_FIFO_SIZE];

/// Transmit FIFO
static uint8_t tx_fifo[RAIL_FIFO_SIZE];

///  @param rail_handle
static uint16_t availableSpaceinFifo = 0;
static volatile uint16_t allocated_tx_fifo_size = 0;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void rail_controller_main (RAIL_Handle_t rail_handle) {
  RAIL_RxPacketInfo_t packet_info;
  // Status indicator of the RAIL API calls
  RAIL_Status_t rail_status;
  // RAIL FIFO size allocated by RAIL_SetTxFifo() call

  switch (state) {

    case S_PACKET_RECEIVED:
      // Packet received:
      //  - Check whether RAIL_HoldRxPacket() was successful, i.e. packet handle is valid
      //  - Copy it to the application FIFO
      //  - Free up the radio FIFO
      //  - Return to IDLE state i.e. RAIL Rx
      rx_packet_handle = RAIL_GetRxPacketInfo (
          rail_handle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE, &packet_info);
      RAIL_PeekRxPacket (rx_packet_handle, rx_packet_handle, rx_fifo, 50, 0);
      rail_status = RAIL_ReleaseRxPacket (rail_handle, rx_packet_handle);
      APP_WARNING(rail_status == RAIL_STATUS_NO_ERROR,
                  "RAIL_ReleaseRxPacket() result:%d", rail_status);
      rail_status = RAIL_StartRx (rail_handle, CHANNEL, NULL);
      APP_WARNING(rail_status == RAIL_STATUS_NO_ERROR,
                  "RAIL_StartRx() result:%d", rail_status);
      if (rx_requested) {
        printf_rx_packet (&rx_fifo[0]);
      }
      sl_led_toggle (&sl_led_led0);
      state = S_IDLE;

      // process the incoming data
      rx_handler_main ();

      RAIL_ResetFifo (rail_handle, false, true); // 1.tx 2. rx fifo

      break;

    case S_PACKET_SENT:
      APP_INFO("Packet has been sent\n");
#if defined(SL_CATALOG_LED1_PRESENT)
      sl_led_toggle (&sl_led_led1);
#else
      sl_led_toggle(&sl_led_led0);
#endif
      // tx done, so radio is ready for new tx
      tx_handler_setTxBusy (false);

      state = S_IDLE;

      break;

    case S_RX_PACKET_ERROR:
      // Handle Rx error
      APP_INFO("Radio RX Error occured\nEvents: %lld\n", current_rail_err);
      state = S_IDLE;
      break;

    case S_TX_PACKET_ERROR:
      // Handle Tx error
      APP_INFO("Radio TX Error occured\nEvents: %lld\n", current_rail_err);
      state = S_IDLE;
      break;
    case S_IDLE:
      if (tx_requested) {

        if ((rx_handler_get_ack_state () == rx_handler_ACK_received
            || rx_handler_get_ack_state () == rx_handler_ACK_default
            || rx_handler_get_ack_state () == rx_handler_ACK_timeout
            || rx_handler_get_ack_state () == rx_handler_NACK_received
            || rx_handler_get_ack_state () == rx_handler_DONE_received)
            && ((tx_handler_getTxBusy () == false))) {

          // prepare the tx buffer
          CORE_irqState_t interrupts = CORE_EnterAtomic ();
          tx_handler_main ();
          CORE_ExitAtomic (interrupts);

          // waiting to response
          rx_handler_set_ack_state (rx_handler_waiting_to_ACK);

          RAIL_ResetFifo (rail_handle, true, false); // 1.tx 2. rx fifo
          //RAIL_SetTxFifoThreshold(rail_handle,255);

          availableSpaceinFifo = RAIL_GetTxFifoSpaceAvailable (rail_handle);

          allocated_tx_fifo_size = RAIL_SetTxFifo (
              rail_handle, tx_fifo, (tx_handler_get_byteToSend () + 1),
              RAIL_FIFO_SIZE);

          availableSpaceinFifo = RAIL_GetTxFifoSpaceAvailable (rail_handle);

          APP_ASSERT(
              allocated_tx_fifo_size == RAIL_FIFO_SIZE,
              "RAIL_SetTxFifo() failed to allocate a large enough fifo (%d bytes instead of %d bytes)\n",
              allocated_tx_fifo_size, RAIL_FIFO_SIZE);

          rail_status = RAIL_StartTx (rail_handle, CHANNEL,
                                      RAIL_TX_OPTIONS_DEFAULT, NULL);

          APP_WARNING(rail_status == RAIL_STATUS_NO_ERROR,
                      "RAIL_StartTx() result:%d ", rail_status);

          // stop sending,
          // after TX complete event, it will be allowed again
          tx_handler_setTxBusy (true);

        }
      }
      break;
    case S_CALIBRATION_ERROR:
      APP_WARNING(
          true,
          "Radio Calibration Error occured\nEvents: %lld\nRAIL_Calibrate() result:%d\n",
          current_rail_err, calibration_status);
      state = S_IDLE;
      break;
    default:
      // Unexpected state
      APP_INFO("Unexpected Simple TRX state occured:%d\n", state);
      break;
  }
}

/******************************************************************************
 * rail_controller_get_rx_fifo()
 *****************************************************************************/
uint8_t rail_controller_get_rx_fifo (uint16_t index_u16) {
  return rx_fifo[index_u16];
}

/******************************************************************************
 * rail_controller_set_tx_fifo()
 *****************************************************************************/
void rail_controller_set_tx_fifo (uint16_t index_u16, uint8_t value_u8) {
  tx_fifo[index_u16] = value_u8;
}

/******************************************************************************
 * rail_controller_set_tx_requested()
 *****************************************************************************/
void rail_controller_set_tx_requested (bool requested) {
  tx_requested = requested;
}

/******************************************************************************
 * RAIL callback, called if a RAIL event occurs.
 *****************************************************************************/
void sl_rail_app_on_event (RAIL_Handle_t rail_handle, RAIL_Events_t events) {

  // Handle Rx events
  if (events & RAIL_EVENTS_RX_COMPLETION) {
    if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
      // Keep the packet in the radio buffer, download it later at the state machine
      rx_packet_handle = RAIL_HoldRxPacket (rail_handle);
      state = S_PACKET_RECEIVED;

    }
    else {
      // Handle Rx error
      current_rail_err = (events & RAIL_EVENTS_RX_COMPLETION);
      state = S_RX_PACKET_ERROR;
    }

  }

  // Handle Tx events
  if (events & RAIL_EVENTS_TX_COMPLETION) {
    if (events & RAIL_EVENT_TX_PACKET_SENT) {
      state = S_PACKET_SENT;

    }
    else {
      // Handle Tx error
      current_rail_err = (events & RAIL_EVENTS_TX_COMPLETION);
      state = S_TX_PACKET_ERROR;
    }
  }

  // Perform all calibrations when needed
  if (events & RAIL_EVENT_CAL_NEEDED) {
    calibration_status = RAIL_Calibrate (rail_handle, NULL,
                                         RAIL_CAL_ALL_PENDING);
    if (calibration_status != RAIL_STATUS_NO_ERROR) {
      current_rail_err = (events & RAIL_EVENT_CAL_NEEDED);
    }
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * The API forwards the received rx packet on CLI
 *****************************************************************************/
static void printf_rx_packet (const uint8_t *const rx_buffer) {
  uint8_t i = 0;
  APP_INFO("Packet status has been received: ");
  for (i = 0; i < rx_buffer[0]; i++) {
    APP_INFO("%c", rx_buffer[i + 1]);
  }
  APP_INFO("\n");
}
