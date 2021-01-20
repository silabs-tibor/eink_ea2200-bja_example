#ifndef EINK_DRV_EINK_PINMAPPING_H_
#define EINK_DRV_EINK_PINMAPPING_H_
/***************************************************************************//**
 * @file eink_pinmapping.h
 * @brief ToDo
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

#include "em_gpio.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/*
 * This header file primary role is describing the connection between the
 * BRD4182A-WSTK radio setup and the EINK related E-Tag TB7 display driver.
 *
 * Table:
 * ----------------------------------------------------------------------------
 * WSTK | WSTK  | Eink | Eink  | GPIO  | GPIO |
 * pin: | Role: | Pin: | Role: | PORT: | PIN: |
 * ----------------------------------------------------------------------------
 * (1)  | -     | -    | -     |   -   |  -   | Not connected
 * (2)  | -     | -    | -     |   -   |  -   | Not connected
 * (3)  | -     | -    | -     |   -   |  -   | Not connected
 * (4)  | -     | -    | -     |   -   |  -   | Not connected
 * (5)  | -     | -    | -     |   -   |  -   | Not connected
 * (6)  | SMISO | (14) | SDA   | C     | 01   | I
 * (7)  | BS1   | (2)  | BS1   | B     | 00   | I
 * (8)  | SCLK  | (12) | SCL   | C     | 02   | I
 * (9)  | RESET | (3)  | RES#  | B     | 01   | I
 * (10) | SCS   | (5)  | CS#   | C     | 03   | I
 * (11) | -     | -    | -     |   -   |  -   | Not connected
 * (12) | -     | -    | -     |   -   |  -   | Not connected
 * (13) | BUSY  | (4)  | BUSY  | D     | 03   | O
 * (14) | -     | -    | -     |   -   |  -   | Not connected
 * (15) | 3.3V  | (1)  | VMCU  | B     | 02   | Power
 * (16) | 0V    | (16) | GND   | B     | 03   | Power
 * (17) | -     | -    | -     |   -   |  -   | Not connected
 * (18) | -     | -    | -     |   -   |  -   | Not connected
 * (19) | -     | -    | -     |   -   |  -   | Not connected
 * (20) | -     | -    | -     |   -   |  -   | Not connected
 *
 *
 */

#define WSTK16_GND_COMBAINED WSTK16_GND_PORT,WSTK16_GND_PIN
#define WSTK16_GND_PORT gpioPortB
#define WSTK16_GND_PIN 3
#define WSTK16_GND_MODE gpioModePushPull

#define WSTK15_POWER_COMBAINED WSTK15_POWER_PORT,WSTK15_POWER_PIN
#define WSTK15_POWER_PORT gpioPortB
#define WSTK15_POWER_PIN 2
#define WSTK15_POWER_MODE gpioModePushPull

#define WSTK6_SDA_COMBAINED WSTK6_SDA_PORT,WSTK6_SDA_PIN
#define WSTK6_SDA_PORT gpioPortC
#define WSTK6_SDA_PIN 1
#define WSTK6_SDA_MODE gpioModePushPull

#define WSTK8_SCL_COMBAINED WSTK8_SCL_PORT,WSTK8_SCL_PIN
#define WSTK8_SCL_PORT gpioPortC
#define WSTK8_SCL_PIN 2
#define WSTK8_SCL_MODE gpioModePushPull

#define WSTK10_CS_COMBAINED WSTK10_CS_PORT,WSTK10_CS_PIN
#define WSTK10_CS_PORT gpioPortC
#define WSTK10_CS_PIN 3
#define WSTK10_CS_MODE gpioModePushPull

#define WSTK7_BS1_COMBAINED WSTK7_BS1_PORT,WSTK7_BS1_PIN
#define WSTK7_BS1_PORT gpioPortB
#define WSTK7_BS1_PIN 0
#define WSTK7_BS1_MODE gpioModePushPull

#define WSTK13_BUSY_COMBAINED WSTK13_BUSY_PORT,WSTK13_BUSY_PIN
#define WSTK13_BUSY_PORT gpioPortD
#define WSTK13_BUSY_PIN 3
#define WSTK13_BUSY_MODE gpioModePushPull

#define WSTK9_RESET_COMBAINED WSTK9_RESET_PORT,WSTK9_RESET_PIN
#define WSTK9_RESET_PORT gpioPortB
#define WSTK9_RESET_PIN 1
#define WSTK9_RESET_MODE gpioModePushPull

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

#endif /* EINK_DRV_EINK_PINMAPPING_H_ */
