#ifndef MODULES_RAIL_CONTROLLER_HEADER_RAIL_CONTROLLER_H_
#define MODULES_RAIL_CONTROLLER_HEADER_RAIL_CONTROLLER_H_

/***************************************************************************//**
 * @file rail_controller.h
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include "rail.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// RAIL channel number
#define CHANNEL ((uint8_t) 20)

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

void rail_controller_main(RAIL_Handle_t rail_handle);
bool rail_controller_get_tx_requested(void);
void rail_controller_set_tx_requested(bool requested);
void rail_controller_set_txfifo_message_size(uint8_t new_size);
bool rail_controller_getAckSent(void);
void rail_controller_setAckSent(bool isAck_b);
void rail_controller_set_txfifo(uint8_t index, uint8_t content);


#endif /* MODULES_RAIL_CONTROLLER_HEADER_RAIL_CONTROLLER_H_ */
