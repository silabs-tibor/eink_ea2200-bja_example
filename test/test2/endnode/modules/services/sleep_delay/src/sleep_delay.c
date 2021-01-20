/***************************************************************************//**
 * @file sleep_delay.c
 * @brief Sends the MCU to EM2 during the EPD update related BUSY periods.
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

#include "sleep_delay.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "sl_sleeptimer.h"

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
 * my_timer_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
 *
 ******************************************************************************/
void my_timer_callback (sl_sleeptimer_timer_handle_t *handle, void *data) {
  (void) handle;
  (void) data;

  sl_sleeptimer_stop_timer (&timer);
}

/***************************************************************************//**
 * sleep_delay_ms()
 *
 ******************************************************************************/
void sleep_delay_ms (uint32_t dlyTicks) {
  status = SL_STATUS_FAIL;

  while (status != SL_STATUS_OK) {
    status = sl_sleeptimer_start_timer_ms (&timer, dlyTicks, my_timer_callback,
                                           (void*) NULL, 0, 0);
  }

  EMU_EnterEM2 (true);

}

/***************************************************************************//**
 * sleep_delay_init()
 *
 ******************************************************************************/
void sleep_delay_init () {
  CMU_ClockSelectSet (cmuClock_RTCCCLK, cmuSelect_LFRCO);
  CMU_ClockEnable (cmuClock_RTCC, true);

  status = SL_STATUS_FAIL;
  while (status != SL_STATUS_OK) {
    status = sl_sleeptimer_init ();
  }

}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

