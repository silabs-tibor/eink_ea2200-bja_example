/***************************************************************************//**
 * @file tx_handler.c
 * @brief Responsible to prepare the rail_handler related Tx buffer. The DTM
 * buffer related slicing happens here.
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
#include "tx_handler.h"
#include "rx_handler.h"
#include "rail_controller.h"
#include "buffer_controller.h"
#include "command_handler.h"
#include "recovery_handler.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

static void tx_handler_S_chunkcnt_update (void);
static void tx_handler_S_gotosleep_tasks (void);
static void tx_handler_S_dtm1_start_tasks (void);
static void tx_handler_S_dtm1_transfer_tasks (void);
static void tx_handler_S_dtm1_stop_tasks (void);
static void tx_handler_S_dtm2_start_tasks (void);
static void tx_handler_S_dtm2_transfer_tasks (void);
static void tx_handler_S_dtm2_stop_tasks (void);
static void tx_handler_S_clean_tasks (void);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

static uint8_t chunkCnt_u8;
static uint8_t byteToSend_u8 = 127;
static volatile bool tx_busy_b = false;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * tx_handler_setTxBusy()
 *****************************************************************************/
void tx_handler_setTxBusy (bool busy_b) {
  tx_busy_b = busy_b;
}

/******************************************************************************
 * tx_handler_getTxBusy()
 *****************************************************************************/
bool tx_handler_getTxBusy (void) {
  return tx_busy_b;
}

/******************************************************************************
 * tx_handler_get_byteToSend()
 *****************************************************************************/
uint8_t tx_handler_get_byteToSend (void) {
  return byteToSend_u8;
}

/******************************************************************************
 * tx_handler_main()
 *****************************************************************************/
void tx_handler_main (void) {

  switch (command_handler_get_tx_command ()) {

    case EINK_gotosleep: {
      /// Send "EINK_GotoSleep!"
      tx_handler_S_gotosleep_tasks ();
      break;
    }

    case EINK_clean: {
      /// Send "EINK_Clean!"
      tx_handler_S_clean_tasks ();
      break;
    }

    case DTM1_start:
      /// Send "DTM1_transfering_started!"
      tx_handler_S_dtm1_start_tasks ();
      break;

    case DTM1_transfer: {

      tx_handler_S_dtm1_transfer_tasks ();
      break;

    }
    case DTM1_stop: {
      /// Send "DTM1_sending_stoped!"
      tx_handler_S_dtm1_stop_tasks ();
      break;

    }
    case DTM2_start: {
      /// Send "DTM2_transfering_started!"
      tx_handler_S_dtm2_start_tasks ();
      break;

    }

    case DTM2_transfer: {

      tx_handler_S_dtm2_transfer_tasks ();
      break;
    }

    case DTM2_stop: {
      /// Send "DTM2_sending_stoped!"
      tx_handler_S_dtm2_stop_tasks ();
      break;
    }
    case NO_Transition: {
      ; // do nothing
      break;
    }

    default: {
      ; // do nothing
      break;
    }
      break;
  }

}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * tx_handler_S_gotosleep_tasks()
 *****************************************************************************/
static void tx_handler_S_chunkcnt_update (void) {

  if (rx_handler_get_ack_state () == rx_handler_ACK_received) {
    chunkCnt_u8++;
  }
  else if (rx_handler_get_ack_state () == rx_handler_NACK_received) {
    ; // do nothing; chunkCnt_u8 should be same as before
    // retransmission will happen
  }
  else {
    ; // do nothing; it shouldn't be happen
  }

}

/******************************************************************************
 * tx_handler_S_gotosleep_tasks()
 *****************************************************************************/
static inline void tx_handler_S_gotosleep_tasks (void) {

  // Tx fifo[0 is where signaling the amount of bytes
  // +1, becuase this number also sent
  byteToSend_u8 = (EINK_GOTOSLEEP_COMMAND_SIZE + 1);
  rail_controller_set_tx_fifo (0, byteToSend_u8);

  // Fill tx_buffer with string
  for (uint8_t i = 0; i < EINK_GOTOSLEEP_COMMAND_SIZE; i++) {
    rail_controller_set_tx_fifo (i + 1, EINK_GOTOSLEEP_COMMAND[i]);
  }

  // next turnaround will be "NO_Transition" state
  command_handler_set_tx_command (EINK_gotosleep);

  // stop tx until timer elapse
  rail_controller_set_tx_requested (false);

  // start timer to check whether DONE received in time
  recovery_handler_start_timer_ms (11000);

  // signaling to the FW: the endnode is sleeping, new command shouldn't be sent
  command_handler_set_endnodesleep (true);
}

/******************************************************************************
 * tx_handler_S_clean_tasks()
 *****************************************************************************/
static inline void tx_handler_S_clean_tasks (void) {
  // Tx fifo[0 is where signaling the amount of bytes
  // +1, becuase this number also sent
  byteToSend_u8 = (EINK_CLEAN_COMMAND_SIZE + 1);
  rail_controller_set_tx_fifo (0, byteToSend_u8);

  // Fill tx_buffer with string
  for (uint8_t i = 0; i < EINK_CLEAN_COMMAND_SIZE; i++) {
    //tx_fifo[i + 1] = EINK_CLEAN_COMMAND[i];
    rail_controller_set_tx_fifo (i + 1, EINK_CLEAN_COMMAND[i]);
  }

  // start timer to check whether DONE received in time
  recovery_handler_start_timer_ms (35000);

  // just one command, after it is sent: stop tx
  rail_controller_set_tx_requested (false);

  // next turnaround will be "NO_Transition" state
  command_handler_set_tx_command (EINK_gotosleep);

}

/*******************************************************************************
 * tx_handler_S_dtm1_start_tasks()
 *****************************************************************************/
static inline void tx_handler_S_dtm1_start_tasks (void) {
  // Init sending related variables
  buffer_controller_initSendingStuff ();

  // Tx fifo[0 is where signaling the amount of bytes
  // +1, becuase this number also sent
  // +1, amount of chunks
  byteToSend_u8 = (EINK_DTM1_START_COMMAND_SIZE + 1 + 1);
  rail_controller_set_tx_fifo (0, byteToSend_u8);

  // Fill tx_buffer with string
  for (uint8_t i = 0; i < EINK_DTM1_START_COMMAND_SIZE; i++) {
    rail_controller_set_tx_fifo (i + 1, EINK_DTM1_START_COMMAND[i]);
  }

  // Last element is the amount of chunks
  rail_controller_set_tx_fifo (EINK_DTM1_START_COMMAND_CHUNK_POSTION,
                               buffer_controller_getamountOfFrames ());

  // next turnaround will be DTM1_transfer state
  command_handler_set_tx_command (DTM1_transfer);

  // start timer to check whether DONE received in time
  recovery_handler_start_timer_ms (40000);

  //
  chunkCnt_u8 = 0;
}

/*******************************************************************************
 * tx_handler_S_dtm1_transfer_tasks()
 *****************************************************************************/
static inline void tx_handler_S_dtm1_transfer_tasks (void) {
  tx_handler_S_chunkcnt_update ();

  /* In case:
   *  1.) there are full frames
   */
  if ((chunkCnt_u8 <= buffer_controller_getamountOfEntireChunks ())) {
    // Full frame will be sent

    byteToSend_u8 = 127;
    rail_controller_set_tx_fifo (0, byteToSend_u8);
    for (uint8_t i_u8 = 1; i_u8 < (127 + 1); i_u8++) {
      uint8_t *p = buffer_controller_getBytestreamDTMxValues (
          EINK_DTM1_STREAM, ((127 * (chunkCnt_u8 - 1)) + (i_u8 - 1)));
      rail_controller_set_tx_fifo (i_u8, *p);

    }

    command_handler_set_tx_command (DTM1_transfer);

  }

  /* In case:
   *  Last turn - ONLY there were full frames, no partial frame
   */
  else if ((chunkCnt_u8 <= buffer_controller_getamountOfFrames ())
      && (buffer_controller_getRemainedBytesInLastTurn () == 0)) {

    byteToSend_u8 = 127;
    rail_controller_set_tx_fifo (0, byteToSend_u8);
    for (uint8_t i_u8 = 1; i_u8 < (127 + 1); i_u8++) {
      uint8_t *p = buffer_controller_getBytestreamDTMxValues (
          EINK_DTM1_STREAM, ((127 * (chunkCnt_u8 - 1)) + (i_u8 - 1)));
      rail_controller_set_tx_fifo (i_u8, *p);

    }

    if (chunkCnt_u8 == buffer_controller_getamountOfFrames ()) {
      command_handler_set_tx_command (DTM1_stop);
    }
  }

  /* In case:
   *  Last turn - there were full frames AND partial frame
   */
  else if ((chunkCnt_u8 <= buffer_controller_getamountOfFrames ())
      && (buffer_controller_getRemainedBytesInLastTurn () != 0)) {
    // send last few bytes
    // +1 is the length
    byteToSend_u8 = buffer_controller_getRemainedBytesInLastTurn ();
    rail_controller_set_tx_fifo (0, byteToSend_u8);

    // last turn, send just remained bytes
    for (uint8_t i_u8 = 1; i_u8 < (byteToSend_u8 + 1); i_u8++) {
      uint8_t *p = buffer_controller_getBytestreamDTMxValues (
          EINK_DTM1_STREAM, ((127 * (chunkCnt_u8 - 1)) + (i_u8 - 1)));
      rail_controller_set_tx_fifo (i_u8, *p);
    }

    if (chunkCnt_u8 == buffer_controller_getamountOfFrames ()) {
      command_handler_set_tx_command (DTM1_stop);
    }
  }

  else {
    ; // do nothing, should not happen
  }
}

/*******************************************************************************
 * tx_handler_S_dtm1_stop_tasks()
 *****************************************************************************/
static inline void tx_handler_S_dtm1_stop_tasks (void) {

  // Tx fifo[0 is where signaling the amount of bytes
  // +1, becuase this number also sent
  byteToSend_u8 = (EINK_DTM1_STOP_COMMAND_SIZE + 1);
  rail_controller_set_tx_fifo (0, byteToSend_u8);

  // Fill tx_buffer with string
  for (uint8_t i = 0; i < EINK_DTM1_STOP_COMMAND_SIZE; i++) {
    rail_controller_set_tx_fifo (i + 1, EINK_DTM1_STOP_COMMAND[i]);
  }

  // next turnaround will be DTM2_start state
  command_handler_set_tx_command (DTM2_start);
}

/*******************************************************************************
 * tx_handler_S_dtm2_start_tasks()
 *****************************************************************************/
static inline void tx_handler_S_dtm2_start_tasks (void) {
  // Tx fifo[0 is where signaling the amount of bytes
  // +1, becuase this number also sent
  // +1, amount of chunks
  byteToSend_u8 = (EINK_DTM2_START_COMMAND_SIZE + 1 + 1);
  rail_controller_set_tx_fifo (0, byteToSend_u8);

  // Fill tx_buffer with string
  for (uint8_t i = 0; i < EINK_DTM2_START_COMMAND_SIZE; i++) {
    rail_controller_set_tx_fifo (i + 1, EINK_DTM2_START_COMMAND[i]);
  }

  // Last element is the amount of chunks
  rail_controller_set_tx_fifo (EINK_DTM2_START_COMMAND_CHUNK_POSTION,
                               buffer_controller_getamountOfFrames ());

  // next turnaround will be DTM1_transfer state
  command_handler_set_tx_command (DTM2_transfer);

  // reset chunkcnt
  chunkCnt_u8 = 0;
}

/*******************************************************************************
 * tx_handler_S_dtm2_transfer_tasks()
 *****************************************************************************/
static inline void tx_handler_S_dtm2_transfer_tasks (void) {
  tx_handler_S_chunkcnt_update ();

  /* In case:
   *  1.) there are full frames
   */
  if ((chunkCnt_u8 <= buffer_controller_getamountOfEntireChunks ())) {
    // Full frame will be sent
    byteToSend_u8 = 127;
    rail_controller_set_tx_fifo (0, byteToSend_u8);

    for (uint8_t i_u8 = 1; i_u8 < (127 + 1); i_u8++) {
      uint8_t *p = buffer_controller_getBytestreamDTMxValues (
          EINK_DTM2_STREAM, ((127 * (chunkCnt_u8 - 1)) + (i_u8 - 1)));
      rail_controller_set_tx_fifo (i_u8, *p);

    }

    command_handler_set_tx_command (DTM2_transfer);

  }

  /* In case:
   *  Last turn - ONLY there were full frames, no partial frame
   */
  else if ((chunkCnt_u8 <= buffer_controller_getamountOfFrames ())
      && (buffer_controller_getRemainedBytesInLastTurn () == 0)) {

    byteToSend_u8 = 127;
    rail_controller_set_tx_fifo (0, byteToSend_u8);
    for (uint8_t i_u8 = 1; i_u8 < (127 + 1); i_u8++) {
      uint8_t *p = buffer_controller_getBytestreamDTMxValues (
          EINK_DTM2_STREAM, ((127 * (chunkCnt_u8 - 1)) + (i_u8 - 1)));
      rail_controller_set_tx_fifo (i_u8, *p);

    }

    if (chunkCnt_u8 == buffer_controller_getamountOfFrames ()) {
      command_handler_set_tx_command (DTM2_stop);
    }
  }

  /* In case:
   *  Last turn - there were full frames AND partial frame
   */
  else if ((chunkCnt_u8 <= buffer_controller_getamountOfFrames ())
      && (buffer_controller_getRemainedBytesInLastTurn () != 0)) {
    // send last few bytes
    // +1 is the length
    byteToSend_u8 = buffer_controller_getRemainedBytesInLastTurn ();
    rail_controller_set_tx_fifo (0, byteToSend_u8);

    // last turn, send just remained bytes
    for (uint8_t i_u8 = 1; i_u8 < (byteToSend_u8 + 1); i_u8++) {
      uint8_t *p = buffer_controller_getBytestreamDTMxValues (
          EINK_DTM2_STREAM, ((127 * (chunkCnt_u8 - 1)) + (i_u8 - 1)));
      rail_controller_set_tx_fifo (i_u8, *p);
    }

    if (chunkCnt_u8 == buffer_controller_getamountOfFrames ()) {
      command_handler_set_tx_command (DTM2_stop);
    }
  }

  else {
    ; // do nothing, should not happen
  }

}

/*******************************************************************************
 * tx_handler_S_dtm2_stop_tasks()
 *****************************************************************************/
static inline void tx_handler_S_dtm2_stop_tasks (void) {
  // Tx fifo[0 where signaling the amount of bytes
  // +1, becuase this number also count in the total amount
  byteToSend_u8 = (EINK_DTM2_STOP_COMMAND_SIZE + 1);
  rail_controller_set_tx_fifo (0, byteToSend_u8);

  // Fill tx_buffer with string
  for (uint8_t i = 0; i < EINK_DTM2_STOP_COMMAND_SIZE; i++) {
    rail_controller_set_tx_fifo (i + 1, EINK_DTM2_STOP_COMMAND[i]);
  }

  // next turnaround will be "gotosleep" state
  command_handler_set_tx_command (EINK_gotosleep);

  // stop transition
  rail_controller_set_tx_requested (false);
}

