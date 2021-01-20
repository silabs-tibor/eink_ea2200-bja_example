#ifndef EINK_SENDER_DISPLAY_DATA_H_
#define EINK_SENDER_DISPLAY_DATA_H_

/***************************************************************************//**
 * @file buffer_controller.h
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

#include "stdint.h"
#include "stdbool.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#define EINK_DTM1_STREAM 1
#define EINK_DTM2_STREAM 2


#define EINK_GOTOSLEEP_COMMAND ("EINK_GotoSleep!")
#define EINK_GOTOSLEEP_COMMAND_SIZE sizeof(EINK_GOTOSLEEP_COMMAND)

#define EINK_CLEAN_COMMAND ("EINK_Clean!")
#define EINK_CLEAN_COMMAND_SIZE sizeof(EINK_CLEAN_COMMAND)

#define EINK_DTM1_START_COMMAND ("DTM1_sending_started!")
#define EINK_DTM1_START_COMMAND_SIZE sizeof( EINK_DTM1_START_COMMAND )
#define EINK_DTM1_START_COMMAND_CHUNK_POSTION (EINK_DTM1_START_COMMAND_SIZE + 2 -1)

#define EINK_DTM1_STOP_COMMAND ("DTM1_sending_stoped!")
#define EINK_DTM1_STOP_COMMAND_SIZE sizeof(EINK_DTM1_STOP_COMMAND)

#define EINK_DTM2_START_COMMAND ("DTM2_sending_started!")
#define EINK_DTM2_START_COMMAND_SIZE sizeof(EINK_DTM2_START_COMMAND)
#define EINK_DTM2_START_COMMAND_CHUNK_POSTION (EINK_DTM2_START_COMMAND_SIZE + 2 -1)

#define EINK_DTM2_STOP_COMMAND ("DTM2_sending_stoped!")
#define EINK_DTM2_STOP_COMMAND_SIZE sizeof(EINK_DTM2_STOP_COMMAND)


typedef enum {
  Command_interpreting,
  EINK_gotosleep,
  EINK_clean,
  DTM1_start,
  DTM1_transfer,
  DTM1_stop,
  DTM2_start,
  DTM2_transfer,
  DTM2_stop,
  NO_Transition
} statemachine_DTM_update;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

uint8_t * buffer_controller_bytestreamDTMxValues(uint8_t,
                                    uint16_t );
void buffer_controller_setByteStreamDTMxValues(uint8_t stream,
                                  uint16_t element,
                                  uint8_t value);
void buffer_controller_setBufferReady(bool ready_b);
bool buffer_controller_getBufferReady(void);
uint8_t buffer_controller_getamountOfFrames(void);
void buffer_controller_setamountOfFrames(uint8_t frames_u8);
void buffer_controller_white_background(void);


#endif /* EINK_SENDER_DISPLAY_DATA_H_ */
