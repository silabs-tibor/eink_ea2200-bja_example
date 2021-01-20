/***************************************************************************//**
 * @file rail_controller.c
 * @brief Responsible to control the radio, Tx - Rx - Idle and error handling.
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
#include "tx_controller.h"
#include <stdint.h>
#include <stdbool.h>
#include "sl_component_catalog.h"
#include "rail.h"
#include "rail.h"
#include "sl_simple_led_instances.h"
#include "rail_controller.h"
#include "le_controller.h"
#include "buffer_controller.h"
#include "eink_drv.h"
#include "burtc_init.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Size of RAIL RX/TX FIFO
#define RAIL_FIFO_SIZE (256u)

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
static void rail_controller_S_reset_DTM_counters (void);
static void rail_controller_S_command_interpreter (uint8_t *tx_fifo_u8p);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Flag, indicating transmit request ( CLI transmit request has occured)
static volatile bool tx_requested = false;

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
static uint8_t tx_fifo[RAIL_FIFO_SIZE] = { 0x03, 'A', 'C', 'K', };
static uint8_t tx_fifo_message_size = 0;

static statemachine_DTM_update receiving_state = Command_interpreting;
static uint8_t chunkCnt_u8 = 0;
static uint16_t receiveDTMxOffset_cnt = 0;
static bool ackSent_b = false;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/*******************************************************************************
 * rail_controller_main()
 *
 ******************************************************************************/
void rail_controller_main (RAIL_Handle_t rail_handle) {

  // packet details
  RAIL_RxPacketInfo_t packet_info;

  // Outer state machine, handling the main radio operation, like rx, tx,
  // errors. The content of the reception is handled in the inner state machine.
  switch (state) {
    case S_PACKET_RECEIVED:

      rx_packet_handle = RAIL_GetRxPacketInfo (
          rail_handle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE, &packet_info);
      RAIL_PeekRxPacket (rx_packet_handle, rx_packet_handle, rx_fifo, 150, 0);

      // Inner state machine: handling the received data basing on its content
      // and the actual state of the state machine. Required, because the DTM
      // updating requires more messages, it cannot be sent in one packet
      switch (receiving_state) {
        // process the rx_buffer content, checking that is it valid command
        case Command_interpreting: {
          rail_controller_S_command_interpreter (rx_fifo);
          break;
        }

          // Filling of the DTM1 buffer
        case DTM1_transfer: {

          // rx_fifo[0] contains the length of the received packet. Copy the
          // rx_fifo content to the dtm buffer
          for (uint8_t i = 0; i < rx_fifo[0]; i++) {
            buffer_controller_setByteStreamDTMxValues (EINK_DTM1_STREAM,
                                                       receiveDTMxOffset_cnt,
                                                       rx_fifo[i + 1]);
            receiveDTMxOffset_cnt++;
          }

          // after the coping all rx_buffer content, increment the received
          // chunk's amount
          chunkCnt_u8++;

          // if the last chunk has arrived, step out from the reception.
          if (chunkCnt_u8 == buffer_controller_getamountOfFrames ()) {
            receiving_state = Command_interpreting;
          }

          break;
        }

          // Filling of the DTM2 buffer
          // same as DTM1, the only difference is the buffer
        case DTM2_transfer: {
          for (uint8_t i = 0; i < rx_fifo[0]; i++) {
            buffer_controller_setByteStreamDTMxValues (EINK_DTM2_STREAM,
                                                       receiveDTMxOffset_cnt,
                                                       rx_fifo[i + 1]);
            receiveDTMxOffset_cnt++;
          }

          chunkCnt_u8++;
          if (chunkCnt_u8 == buffer_controller_getamountOfFrames ()) {
            receiving_state = Command_interpreting;
          }

          break;
        }

        default: {
          ; // do nothing
          break;
        }

      } // End of inner state machine

      RAIL_ReleaseRxPacket (rail_handle, rx_packet_handle);

      // Restart reception
      RAIL_StartRx (rail_handle, CHANNEL, NULL);

      // Set RX Fifo to its default state
      RAIL_ResetFifo (rail_handle, false, true); // 1.tx 2. rx fifo

      // Indicate the reception
      sl_led_toggle (&sl_led_led0);

      // next state - IDLE
      state = S_IDLE;

      // send an ACK, indicating to transciver that reciver is ready to new data
      tx_controller_send_response (ACK);

      break;

    case S_IDLE:
      if (rail_controller_get_tx_requested () == true
          || burtc_init_get_was_em4_reset () == true) {

        tx_controller_tx_tasks ();

        RAIL_ResetFifo (rail_handle, true, false); // 1.tx 2. rx fifo
        RAIL_SetTxFifo (rail_handle, tx_fifo, tx_fifo_message_size,
                        RAIL_FIFO_SIZE);
        RAIL_StartTx (rail_handle, CHANNEL, RAIL_TX_OPTIONS_DEFAULT, NULL);

      }
      break;
    case S_PACKET_SENT:
#if defined(SL_CATALOG_LED1_PRESENT)

      rail_controller_setAckSent (true);

      sl_led_toggle (&sl_led_led1);
#else
      sl_led_toggle(&sl_led_led0);
#endif
      state = S_IDLE;
      break;
    case S_RX_PACKET_ERROR:
      // Handle Rx error
      // send an NACK, indicating to transmission issue occured,
      // the last message should be resent
      tx_controller_send_response (NACK);

      // Restart reception
      RAIL_ReleaseRxPacket (rail_handle, rx_packet_handle);
      RAIL_ResetFifo (rail_handle, false, true);
      RAIL_StartRx (rail_handle, CHANNEL, NULL);

      state = S_IDLE;
      break;
    case S_TX_PACKET_ERROR:
      // Handle Tx error
      state = S_IDLE;
      break;
    case S_CALIBRATION_ERROR:
      state = S_IDLE;
      break;
    default:
      ; // Unexpected state, do nothing
      break;
  }

}

/******************************************************************************
 * rail_controller_get_tx_requested()
 *
 *****************************************************************************/
bool rail_controller_get_tx_requested () {
  return tx_requested;
}

/******************************************************************************
 * rail_controller_set_tx_requested()
 *
 *****************************************************************************/
void rail_controller_set_tx_requested (bool requested) {
  tx_requested = requested;
}

/******************************************************************************
 * rail_controller_set_txfifo_message_size()
 *
 *****************************************************************************/
void rail_controller_set_txfifo_message_size (uint8_t new_size) {
  tx_fifo_message_size = new_size;
}

/******************************************************************************
 * rail_controller_set_txfifo()
 *
 *****************************************************************************/
void rail_controller_set_txfifo (uint8_t index, uint8_t content) {
  tx_fifo[index] = content;
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

/*******************************************************************************
 * rail_controller_getAckSent()
 *
 ******************************************************************************/
bool rail_controller_getAckSent () {
  return ackSent_b;
}

/*******************************************************************************
 * rail_controller_setAckSent()
 *
 ******************************************************************************/
void rail_controller_setAckSent (bool isAck_b) {
  ackSent_b = isAck_b;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * rail_controller_S_command_interpreter()
 *
 ******************************************************************************/
static void rail_controller_S_command_interpreter (uint8_t *tx_fifo_u8p) {

  statemachine_DTM_update receied_command = Command_interpreting;
  uint8_t packetbytes_u8 = 0;
  bool newCommandSet = false;

  // nullth byte is the packet length, including itself, so not valid content
  packetbytes_u8 = (tx_fifo_u8p[0] - 1);

  // EINK_GotoSleep command check
  if ((EINK_GOTOSLEEP_COMMAND_SIZE == packetbytes_u8)
      && (newCommandSet == false)) {
    for (uint8_t i = 0; i < packetbytes_u8 - 1; i++) {
      if (EINK_GOTOSLEEP_COMMAND[i] == tx_fifo_u8p[i + 1]) {
        receied_command = EINK_gotosleep;
        newCommandSet = true;
      }
      else {
        receied_command = Command_interpreting;
        newCommandSet = false;
        break;
      }
    }
  }

  // EINK_CLear command check
  if ((EINK_CLEAN_COMMAND_SIZE == packetbytes_u8) && (newCommandSet == false)) {
    for (uint8_t i = 0; i < packetbytes_u8 - 1; i++) {
      if (EINK_CLEAN_COMMAND[i] == tx_fifo_u8p[i + 1]) {
        receied_command = EINK_clean;
        newCommandSet = true;
      }
      else {
        receied_command = Command_interpreting;
        newCommandSet = false;
        break;
      }
    }
  }

  // STart DTM1 Command check
  // -1 because the frame count also sent
  if ((EINK_DTM1_START_COMMAND_SIZE == packetbytes_u8 - 1)
      && (newCommandSet == false)) {
    for (uint8_t i = 0; i < packetbytes_u8 - 1; i++) {
      if (EINK_DTM1_START_COMMAND[i] == tx_fifo_u8p[i + 1]) {
        receied_command = DTM1_start;
        newCommandSet = true;
      }
      else {
        receied_command = Command_interpreting;
        newCommandSet = false;
        break;
      }
    }
  }

  // Stop DTM1 Command check
  if ((EINK_DTM1_STOP_COMMAND_SIZE == packetbytes_u8)
      && (newCommandSet == false)) {
    for (uint8_t i = 0; i < packetbytes_u8 - 1; i++) {
      if (EINK_DTM1_STOP_COMMAND[i] == tx_fifo_u8p[i + 1]) {
        receied_command = Command_interpreting;
        newCommandSet = true;
      }
      else {
        receied_command = Command_interpreting;
        newCommandSet = false;
        break;
      }
    }
  }

  // Start DTM2 Command check
  // -1 because the frame count also sent
  if ((EINK_DTM2_START_COMMAND_SIZE == packetbytes_u8 - 1)
      && (newCommandSet == false)) {
    for (uint8_t i = 0; i < packetbytes_u8 - 1; i++) {
      if (EINK_DTM2_START_COMMAND[i] == tx_fifo_u8p[i + 1]) {
        receied_command = DTM2_start;
        newCommandSet = true;
      }
      else {
        receied_command = Command_interpreting;
        newCommandSet = false;
        break;
      }
    }
  }

  // Stop DTM2 Command check
  if ((EINK_DTM2_STOP_COMMAND_SIZE == packetbytes_u8)
      && (newCommandSet == false)) {
    for (uint8_t i = 0; i < packetbytes_u8 - 1; i++) {
      if (EINK_DTM2_STOP_COMMAND[i] == tx_fifo_u8p[i + 1]) {
        receied_command = DTM2_stop;
        newCommandSet = true;
      }
      else {
        receied_command = Command_interpreting;
        newCommandSet = false;
        break;
      }
    }
  }

  switch (receied_command) {
    case DTM1_start:
      buffer_controller_white_background ();
      rail_controller_S_reset_DTM_counters ();
      receiving_state = DTM1_transfer;
      buffer_controller_setamountOfFrames (
          tx_fifo_u8p[EINK_DTM1_START_COMMAND_CHUNK_POSTION]);
      break;
    case DTM2_start:
      rail_controller_S_reset_DTM_counters ();
      receiving_state = DTM2_transfer;
      buffer_controller_setamountOfFrames (
          tx_fifo_u8p[EINK_DTM2_START_COMMAND_CHUNK_POSTION]);
      break;
    case DTM2_stop:
      rail_controller_S_reset_DTM_counters ();
      buffer_controller_setBufferReady (true);
      break;
    case EINK_clean:
      receiving_state = Command_interpreting;
      eink_drv_set_command (clean);
      break;
    case EINK_gotosleep:
      rail_controller_setAckSent (false);
      le_controller_setReadyToSleep (true);
      break;

    default:
      ; // do nothing
      break;
  }

}

/*******************************************************************************
 * rail_controller_S_reset_DTM_counters()
 *
 ******************************************************************************/
static void rail_controller_S_reset_DTM_counters (void) {
  receiveDTMxOffset_cnt = 0;
  chunkCnt_u8 = 0;
}
