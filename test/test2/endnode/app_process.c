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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "stdbool.h"
#include "app_process.h"
#include "rail.h"
#include "eink_drv.h"
#include "buffer_controller.h"
#include "rail_controller.h"
#include "le_controller.h"
#include "tx_controller.h"
#include "burtc_init.h"
#include "spi_init.h"

#define BUFFER_SIZE (296*128)/8

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle)
{
  // if reset casue was BRTC reset, then send DONE, signaling to the gateway
  // that ready to the new commend
  burtc_init_checkResetCause();
  rail_controller_main(rail_handle);

  // delete the display content
  if (eink_drv_get_command() == clean){
    spi_init_init();
    eink_drv_set_command(nothing);

    // fills both DTMx buffers with 0xFF, that will cause a white image
    for(uint16_t i = 0; i < BUFFER_SIZE; i++){
      buffer_controller_setByteStreamDTMxValues(EINK_DTM1_STREAM,
                                                i,
                                                0xFF);
      buffer_controller_setByteStreamDTMxValues(EINK_DTM2_STREAM,
                                                i,
                                                0xFF);
    }

    eink_drv_update_image(buffer_controller_bytestreamDTMxValues(EINK_DTM1_STREAM,0),
                          buffer_controller_bytestreamDTMxValues(EINK_DTM2_STREAM,0),
                          BUFFER_SIZE);

    spi_init_deinit();
    tx_controller_send_response(DONE);
  }

  // update the display content
  if (buffer_controller_getBufferReady() == true){
    spi_init_init();
    buffer_controller_setBufferReady(false);

    eink_drv_update_image(buffer_controller_bytestreamDTMxValues(EINK_DTM1_STREAM,0),
                          buffer_controller_bytestreamDTMxValues(EINK_DTM2_STREAM,0),
                          BUFFER_SIZE);
    spi_init_deinit();
    tx_controller_send_response(DONE);
  }

  // go to em4
  if ((le_controller_getReadyToSleep() == true) &&
      (rail_controller_getAckSent() == true)){
    rail_controller_setAckSent(false);
    le_controller_enterEM4();
  }

}


