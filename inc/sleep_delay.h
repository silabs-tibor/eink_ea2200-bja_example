#ifndef DELAY_SLEEP_DELAY_H_
#define DELAY_SLEEP_DELAY_H_
/***************************************************************************//**
 * @file sleep_delay.h
 * @brief -
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
 
  /***************************************************************************//**
 * @addtogroup Eink driver 
 * @brief Eink driver 
 *   AN0063 related source code
 * @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <stdint.h>

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * @brief
 *  ms based delay for the application
 *
 * @param[in] dlyTicks
 *   sleep time in ms
 *
 ******************************************************************************/ 
void sleep_delay_ms(uint32_t dlyTicks);

/***************************************************************************//**
 * @brief
 *  Initializes delay
 *
 * @note
 *  During the delay, the MCU goes to EM2 state
 *
 ******************************************************************************/ 
void sleep_delay_init(void);

/** @} (end addtogroup Eink driver ) */

#endif /* DELAY_SLEEP_DELAY_H_ */
