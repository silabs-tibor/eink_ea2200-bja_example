/***************************************************************************//**
 * @file tx_constroller.c
 * @brief Prepares the ral_controller related Tx buffer
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
#include "stdbool.h"
#include "stdint.h"
#include "sl_udelay.h"
#include "tx_controller.h"
#include "rail_controller.h"
#include "burtc_init.h"

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

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * tx_controller_send_response()
 *
 *****************************************************************************/
void tx_controller_send_response (tx_controller_responses resp) {

  switch (resp) {

    case ACK: {
      uint8_t tempArray[] = { 0x03, 'A', 'C', 'K', };
      for (uint8_t i = 0; i < 4; i++) {
        rail_controller_set_txfifo (i, tempArray[i]);
      }
      rail_controller_set_txfifo_message_size (4);
      break;
    }
    case NACK: {
      uint8_t tempArray[] = { 0x04, 'N', 'A', 'C', 'K', };
      for (uint8_t i = 0; i < 5; i++) {
        rail_controller_set_txfifo (i, tempArray[i]);
      }
      rail_controller_set_txfifo_message_size (5);
      break;
    }
    case DONE: {
      uint8_t tempArray[] = { 0x04, 'D', 'O', 'N', 'E', };
      for (uint8_t i = 0; i < 5; i++) {
        rail_controller_set_txfifo (i, tempArray[i]);
      }
      rail_controller_set_txfifo_message_size (5);
      break;
    }

  }

  rail_controller_set_tx_requested (true);
}

/******************************************************************************
 * tx_controller_tx_tasks()
 *
 *****************************************************************************/
void tx_controller_tx_tasks (void) {

  sl_udelay_wait (1000);

  if (burtc_init_get_was_em4_reset () == true) {
    tx_controller_send_response (DONE);
    burtc_init_set_was_em4_reset (false);
  }

  rail_controller_set_tx_requested (false);

}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

