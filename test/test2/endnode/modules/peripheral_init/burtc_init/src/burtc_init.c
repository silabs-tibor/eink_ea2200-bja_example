/***************************************************************************//**
 * @file burtc_init.c
 * @brief Initialization of BURTC peripheral
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
#include "stdio.h"
#include "stdbool.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_burtc.h"
#include "em_rmu.h"
#include "tx_controller.h"
#include "sl_simple_led_instances.h"
#include "burtc_init.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// Number of 1 KHz ULFRCO clocks between BURTC interrupts
#define BURTC_IRQ_PERIOD  10000

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

static bool burtc_init_reset_was_EM4 = false;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * burtc_init_get_was_em4_reset
 *
 *****************************************************************************/
bool burtc_init_get_was_em4_reset (void) {
  return burtc_init_reset_was_EM4;
}

/**************************************************************************//**
 * burtc_init_set_was_em4_reset
 *
 *****************************************************************************/
void burtc_init_set_was_em4_reset (bool new_value) {
  burtc_init_reset_was_EM4 = new_value;
}

/**************************************************************************//**
 * @brief  BURTC Handler
 *****************************************************************************/
void BURTC_IRQHandler (void) {
  BURTC_IntClear (BURTC_IF_COMP); // compare match
}

/**************************************************************************//**
 * burtc_init_init(void)
 *
 *****************************************************************************/
void burtc_init_init (void) {
  CMU_ClockSelectSet (cmuClock_EM4GRPACLK, cmuSelect_ULFRCO);
  CMU_ClockEnable (cmuClock_BURTC, true);
  CMU_ClockEnable (cmuClock_BURAM, true);

  BURTC_Init_TypeDef burtcInit = BURTC_INIT_DEFAULT;
  burtcInit.compare0Top = true; // reset counter when counter reaches compare value
  burtcInit.em4comp = true; // BURTC compare interrupt wakes from EM4 (causes reset)
  BURTC_Init (&burtcInit);

  BURTC_CounterReset ();
  BURTC_CompareSet (0, BURTC_IRQ_PERIOD);

  BURTC_IntEnable (BURTC_IEN_COMP);    // compare match
  NVIC_EnableIRQ (BURTC_IRQn);
  BURTC_Enable (true);
}

/**************************************************************************//**
 * burtc_init_checkResetCause()
 *
 *****************************************************************************/
void burtc_init_checkResetCause (void) {
  uint32_t cause = EMU->RSTCAUSE;
  RMU_ResetCauseClear ();

  if (cause & EMU_RSTCAUSE_PIN) {
    ; // do nothing
  }
  else if (cause & EMU_RSTCAUSE_EM4) {
    burtc_init_reset_was_EM4 = true;
  }

}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

