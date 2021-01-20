/***************************************************************************//**
 * @file buffer_controller.c
 * @brief Responsible to handle the DTM1 and DTM2 buffers
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

#define BUFFER_SIZE ((128*296)/8)
#define RF_PAYLOAD_SIZE 128
// amount of bytes will be sent in a round, 128byte, but 1 byte for the packet
// size
#define CHUNK_SIZE (RF_PAYLOAD_SIZE-1)

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// buffers
static uint8_t byte_buffer_DTM1[BUFFER_SIZE];
static uint8_t byte_buffer_DTM2[BUFFER_SIZE];

// amount of whole frames
static uint16_t EINK_amountOfBytes = BUFFER_SIZE;
static uint8_t amountOfEntireChunks = 0;
static uint8_t amountBytesInLastFrame = 0;
static uint8_t amountOfFrames_u8 = 0;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/*******************************************************************************
 * eink_white_background()
 *
 ******************************************************************************/
void buffer_controller_white_background (void) {
  for (uint16_t i_u16 = 0; i_u16 < BUFFER_SIZE; i_u16++) {
    byte_buffer_DTM1[i_u16] = 0xFF; //FF
    byte_buffer_DTM2[i_u16] = 0xFF; //FF
  }
}

/*******************************************************************************
 * Init control variable
 * The whole byte stream will be sent in 128byte chunks
 *
 *
 ******************************************************************************/
void buffer_controller_initSendingStuff () {
  amountOfEntireChunks = EINK_amountOfBytes / CHUNK_SIZE;
  amountBytesInLastFrame = EINK_amountOfBytes % CHUNK_SIZE;

  amountOfFrames_u8 = amountOfEntireChunks;
  if (amountBytesInLastFrame != 0) {
    amountOfFrames_u8++;
  }

}

/*******************************************************************************
 * EINK_getBytestreamDTMxValues()
 *
 ******************************************************************************/
uint8_t* buffer_controller_getBytestreamDTMxValues (uint8_t stream,
                                                    uint16_t element) {

  if (stream == 1) {
    return &byte_buffer_DTM1[element];
  }
  else {
    return &byte_buffer_DTM2[element];
  }
}

/*******************************************************************************
 * EINK_setByteStreamDTMxValues()
 *
 ******************************************************************************/
void buffer_controller_setByteStreamDTMxValues (uint8_t stream,
                                                uint16_t element, uint8_t value) {

  if (stream == 1) {
    byte_buffer_DTM1[element] = value;
  }
  else {
    byte_buffer_DTM2[element] = value;
  }
}

/*******************************************************************************
 * EINK_getAmountOfChunks()
 *
 ******************************************************************************/
uint8_t buffer_controller_getamountOfEntireChunks () {
  return amountOfEntireChunks;
}

/*******************************************************************************
 * EINK_getamountOfFrames()
 *
 ******************************************************************************/
uint8_t buffer_controller_getamountOfFrames (void) {
  return amountOfFrames_u8;
}

/*******************************************************************************
 * EINK_getRemainedBytesInLastTurn()
 *
 ******************************************************************************/
uint8_t buffer_controller_getRemainedBytesInLastTurn () {
  return amountBytesInLastFrame;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

