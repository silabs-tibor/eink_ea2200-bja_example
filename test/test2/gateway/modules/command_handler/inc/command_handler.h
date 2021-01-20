#ifndef MODULES_COMMAND_HANDLER_INC_COMMAND_HANDLER_H_
#define MODULES_COMMAND_HANDLER_INC_COMMAND_HANDLER_H_

/***************************************************************************//**
 * @file command_handler.h
 * @brief
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

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

typedef enum {
  EINK_gotosleep,
  EINK_clean,
  DTM1_start,
  DTM1_transfer,
  DTM1_stop,
  DTM2_start,
  DTM2_transfer,
  DTM2_stop,
  NO_Transition,
  Empty
} eink_commands;

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

eink_commands command_handler_get_tx_command(void);
eink_commands command_handler_get_command_during_sleep(void);
void command_handler_set_tx_command(eink_commands task);
void command_handler_set_command_during_sleep(eink_commands task);
bool command_handler_get_endnodesleep(void);
void command_handler_set_endnodesleep(bool endsleep);

#endif /* MODULES_COMMAND_HANDLER_INC_COMMAND_HANDLER_H_ */
