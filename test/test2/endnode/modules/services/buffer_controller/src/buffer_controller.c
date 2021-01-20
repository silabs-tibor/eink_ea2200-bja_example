/***************************************************************************//**
 * @file buffer_controller.c
 * @brief Handles the DTM1 and DTM2 related arrays
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
#include "buffer_controller.h"
#include "stdint.h"
#include "stdbool.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#define EINK_BUFFER_SIZE ((128*296)/8)
// amount of bytes will be sent in a round, 128byte, but 1 byte for the packet
// size
#define EINK_RF_PAYLOAD_SIZE 128
#define EINK_CHUNK_SIZE (EINK_RF_PAYLOAD_SIZE-1)

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

static bool buffers_ready_b = false;
static uint8_t bytestream_DTM1[EINK_BUFFER_SIZE];
static uint8_t bytestream_DTM2[EINK_BUFFER_SIZE];
static uint8_t amountOfFrames_u8 = 0;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/*******************************************************************************
 * eink_white_background()
 *
 ******************************************************************************/
void buffer_controller_white_background (void) {
  for (uint16_t i_u16 = 0; i_u16 < EINK_BUFFER_SIZE; i_u16++) {
    bytestream_DTM1[i_u16] = 0xFF;
    bytestream_DTM2[i_u16] = 0xFF;
  }
}

/*******************************************************************************
 * EINK_bytestreamDTMxValues()
 *
 ******************************************************************************/
uint8_t* buffer_controller_bytestreamDTMxValues (uint8_t stream,
                                                 uint16_t element) {

  if (stream == 1) {
    return (uint8_t*)&bytestream_DTM1[element];
  }
  else {
    return (uint8_t*)&bytestream_DTM2[element];
  }
}

/*******************************************************************************
 * EINK_setByteStreamDTMxValues()
 *
 ******************************************************************************/
void buffer_controller_setByteStreamDTMxValues (uint8_t stream,
                                                uint16_t element, uint8_t value) {

  if (stream == 1) {
    bytestream_DTM1[element] = value;
  }
  else {
    bytestream_DTM2[element] = value;
  }
}

/*******************************************************************************
 * EINK_getamountOfFrames()
 *
 ******************************************************************************/
uint8_t buffer_controller_getamountOfFrames (void) {

  return amountOfFrames_u8;
}

/*******************************************************************************
 * EINK_setamountOfFrames()
 *
 ******************************************************************************/
void buffer_controller_setamountOfFrames (uint8_t frames_u8) {
  amountOfFrames_u8 = frames_u8;
}

/*******************************************************************************
 * EINK_getBufferReady()
 *
 ******************************************************************************/
bool buffer_controller_getBufferReady (void) {
  return buffers_ready_b;
}

/*******************************************************************************
 * EINK_setBufferReady()
 *
 ******************************************************************************/
void buffer_controller_setBufferReady (bool ready_b) {
  buffers_ready_b = ready_b;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

