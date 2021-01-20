/***************************************************************************//**
 * @file app_cli.c
 * @brief app_cli.c
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <modules/tx_handler/inc/tx_handler.h>
#include <stdint.h>
#include "em_chip.h"
#include "sl_flex_assert.h"
#include "sl_cli.h"

#include "buffer_controller.h"
#include "rail_controller.h"
#include "rx_handler.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static void cli_set_command_as_act_or_next(eink_commands command);

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
 * CLI - clean: sending the buffer content to the end node
 *****************************************************************************/
void cli_eink_gotosleep(sl_cli_command_arg_t *arguments)
{
  (void) arguments;

  cli_set_command_as_act_or_next(EINK_gotosleep);

  APP_INFO("Send eink to sleep state - for 10 sec \n");
}


/******************************************************************************
 * CLI - clean: sending the buffer content to the end node
 *****************************************************************************/
void cli_eink_clean(sl_cli_command_arg_t *arguments)
{
  (void) arguments;

  cli_set_command_as_act_or_next(EINK_clean);

  APP_INFO("Clean up the eink display ... \n");
}

/******************************************************************************
 * CLI - update: sending the buffer content to the end node
 *****************************************************************************/
void cli_eink_update(sl_cli_command_arg_t *arguments)
{
  (void) arguments;

  cli_set_command_as_act_or_next(DTM1_start);

  APP_INFO("Sending picture to endnode ... \n");
}

/******************************************************************************
 * CLI - DTM1 fill:
 *   Description:
 *     1st argument: uint8_t; offset * 8
 *     2nd argument: uint8_t[] ; hex array
 *   example:
 *     cli_dtm1_fill 0 {AA BB CC}`
 *
 *****************************************************************************/

void cli_dtmx_update_buffer(sl_cli_command_arg_t *arguments)
{

  command_handler_set_tx_command(NO_Transition);
  rail_controller_set_tx_requested(false);

  size_t recivedBytes_u16 = 0;

  // get 1st argument: offset
  uint16_t offset_u16 = sl_cli_get_argument_uint16(arguments, 0);

  // get 2nd argument: stream
  uint8_t stream_u8 = sl_cli_get_argument_uint8(arguments, 1);

  // get 3nd argument: bytes
  uint8_t *hex_array_ptr = sl_cli_get_argument_hex(arguments,
                                                   2,
                                                   &recivedBytes_u16);

  // when the first command arrives, delete the entire DTM buffers content
  if(offset_u16 == 0 && stream_u8 == 1){
      buffer_controller_white_background();
  }

  for (uint16_t i_u16 = 0; i_u16 < recivedBytes_u16; i_u16++) {

      if (stream_u8 == 1){
        buffer_controller_setByteStreamDTMxValues(EINK_DTM1_STREAM,
                                   (offset_u16*32) + i_u16,
                                   *hex_array_ptr);
      } else if (stream_u8 == 2) {
        buffer_controller_setByteStreamDTMxValues(EINK_DTM2_STREAM,
                                     (offset_u16*32) + i_u16,
                                     *hex_array_ptr);
      } else {
        APP_INFO("Not supported stream!\n");
      }

      // increment pointer
      hex_array_ptr++;
  }

}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * cli_set_command_as_act_or_next
 *****************************************************************************/
static void cli_set_command_as_act_or_next(eink_commands command){

  if (command_handler_get_endnodesleep() == false){
    // In case the end device not sleeping than the command can be issued
    // promptly
    command_handler_set_tx_command(command);
    rail_controller_set_tx_requested(true);
  } else {
    // If the end device is sleeping, the gateway have to wait for the wake up,
    // than send it after the waking up
    command_handler_set_command_during_sleep(command);
  }
}
