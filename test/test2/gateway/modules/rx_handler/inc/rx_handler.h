#ifndef MODULES_RX_HANDLER_INC_RX_HANDLER_H_
#define MODULES_RX_HANDLER_INC_RX_HANDLER_H_

/***************************************************************************//**
 * @file rx_handler.h
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

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

typedef enum {
  rx_handler_ACK_default,
  rx_handler_waiting_to_ACK,
  rx_handler_ACK_received,
  rx_handler_NACK_received,
  rx_handler_DONE_received,
  rx_handler_ACK_timeout
} rx_handler_ack_states;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

rx_handler_ack_states rx_handler_get_ack_state(void);
void rx_handler_set_ack_state(rx_handler_ack_states ack);
void rx_handler_main(void);

#endif /* MODULES_RX_HANDLER_INC_RX_HANDLER_H_ */
