/***************************************************************************//**
 * @file app_process.c
 * @brief app_process.c
 * @version 0.0.1
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

#include <modules/tx_handler/inc/tx_handler.h>
#include "rail.h"
#include "app_process.h"

#include "buffer_controller.h"
#include "command_handler.h"
#include "rail_controller.h"
#include "rx_handler.h"
#include "stdbool.h"

void app_process_action(RAIL_Handle_t rail_handle){


  // Process the command received during end device was slept
  if (command_handler_get_command_during_sleep() != Empty &&
      command_handler_get_endnodesleep() == false){

    //set the new command to send
    rail_controller_set_tx_requested(true);
    command_handler_set_tx_command(command_handler_get_command_during_sleep());

    // delete the previous command
    command_handler_set_command_during_sleep(Empty);
  }

  // Endless sleep task
  else if (command_handler_get_tx_command() == EINK_gotosleep &&
      command_handler_get_endnodesleep() == false){

    rail_controller_set_tx_requested(true);
    command_handler_set_tx_command(EINK_gotosleep);
  }

  // perform rx-tx things
  rail_controller_main(rail_handle);

}
