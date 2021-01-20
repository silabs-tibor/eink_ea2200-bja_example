/***************************************************************************//**
 * @file recovery_handler.c
 * @brief Responsible to check that the end device sent back 'DONE' after the
 * sent commands performed.
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
#include "recovery_handler.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "sl_sleeptimer.h"
#include "sl_flex_assert.h"
#include "tx_handler.h"
#include "rx_handler.h"
#include "command_handler.h"
#include "rail_controller.h"

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

static sl_status_t status = SL_STATUS_OK;
static sl_sleeptimer_timer_handle_t timer;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * recovery_handler_stop_timer
 *
 ******************************************************************************/
void recovery_handler_stop_timer (void) {

  sl_sleeptimer_stop_timer (&timer);
}

/***************************************************************************//**
 * recovery_handler_timer_callback()
 *
 ******************************************************************************/
void recovery_handler_timer_callback (sl_sleeptimer_timer_handle_t *handle,
                                      void *data) {
  (void) data;
  (void) handle;

  sl_sleeptimer_stop_timer (&timer);

  // if ACK was not received in this period, then set timeout
  if (rx_handler_get_ack_state () == rx_handler_waiting_to_ACK) {
    rx_handler_set_ack_state (rx_handler_ACK_timeout);

  }
  APP_INFO("recovery_called!\n");

  command_handler_set_endnodesleep (false);

}

/***************************************************************************//**
 * recovery_handler_start_timer_ms()
 *
 ******************************************************************************/
void recovery_handler_start_timer_ms (uint32_t dlyTicks) {
  status = SL_STATUS_FAIL;

  while (status != SL_STATUS_OK) {
    status = sl_sleeptimer_start_timer_ms (&timer, dlyTicks,
                                           recovery_handler_timer_callback,
                                           (void*) NULL, 0, 0);
  }

  command_handler_set_endnodesleep (true);
}

/***************************************************************************//**
 * recovery_handler_init()
 *
 ******************************************************************************/
void recovery_handler_init () {
  CMU_ClockSelectSet (cmuClock_LFE, cmuSelect_LFRCO);
  CMU_ClockEnable (cmuClock_RTCC, true);

  status = SL_STATUS_FAIL;

  while (status != SL_STATUS_OK) {
    status = sl_sleeptimer_init ();
  }

}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

