/***************************************************************************//**
 * @file rx_handler.c
 * @brief Responsible to evaluate the rail_handler related Rx buffer
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
#include "rx_handler.h"
#include "command_handler.h"
#include "rail_controller.h"
#include "recovery_handler.h"
#include "stdbool.h"
#include "stdint.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

static uint16_t payload_length = 0;
static rx_handler_ack_states ack_state = rx_handler_ACK_default;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * txrx_controller_getAckReceived()
 *****************************************************************************/
rx_handler_ack_states rx_handler_get_ack_state (void) {
  return ack_state;
}

/******************************************************************************
 * txrx_controller_setAckReceived()
 *****************************************************************************/
void rx_handler_set_ack_state (rx_handler_ack_states ack) {
  ack_state = ack;
}

/******************************************************************************
 * rx_handler_main()
 *****************************************************************************/
void rx_handler_main () {

  bool new_command_set = false;

  payload_length = rail_controller_get_rx_fifo (0);

  // ACK check
  if (new_command_set == false) {
    uint8_t temp_arr[] = { 'A', 'C', 'K' };
    for (uint8_t i = 0; i < payload_length; i++) {
      if (temp_arr[i] == rail_controller_get_rx_fifo (i + 1)) {
        new_command_set = true;
        rx_handler_set_ack_state (rx_handler_ACK_received);
      }
      else {
        new_command_set = false;
        rx_handler_set_ack_state (rx_handler_ACK_default);
        break;
      }
    }

  }

  // NACK check
  if (new_command_set == false) {
    uint8_t temp_arr[] = { 'N', 'A', 'C', 'K' };
    for (uint8_t i = 0; i < payload_length; i++) {
      if (temp_arr[i] == rail_controller_get_rx_fifo (i + 1)) {
        new_command_set = true;
        rx_handler_set_ack_state (rx_handler_NACK_received);
      }
      else {
        new_command_set = false;
        rx_handler_set_ack_state (rx_handler_ACK_default);
        break;
      }
    }
  }

  // DONE check
  if (new_command_set == false) {
    uint8_t temp_arr[] = { 'D', 'O', 'N', 'E' };
    for (uint8_t i = 0; i < payload_length; i++) {
      if (temp_arr[i] == rail_controller_get_rx_fifo (i + 1)) {
        new_command_set = true;
        rx_handler_set_ack_state (rx_handler_DONE_received);
        command_handler_set_endnodesleep (false);
      }
      else {
        new_command_set = false;
        rx_handler_set_ack_state (rx_handler_ACK_default);
        command_handler_set_endnodesleep (true);
        break;
      }

      // possibly DONE received, so recovery should stop
      recovery_handler_stop_timer ();
    }
  }
  else {
    ; // do nothing
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

