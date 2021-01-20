#ifndef EINK_DRV_EINK_DRV_H_
#define EINK_DRV_EINK_DRV_H_

/***************************************************************************//**
 * @file eink_drv.h
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
  nothing,
  clean,
  update
} eink_drv_commands;

#define DATA_MASK   0x0100
#define DCX_CMD     0x0000
#define DCX_DATA    0x0001

#define PSR         0x0000
#define POF         0x0002
#define PFS         0x0003
#define PON         0x0004
#define BTST        0x0006
#define DTM1        0x0010
#define DRF         0x0012
#define DTM2        0x0013
#define LUTOPT      0x002A
#define TSE         0x0041
#define CDI         0x0050
#define TCON        0x0060

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

void eink_drv_update_image(uint8_t * dtm1_ptr,
                           uint8_t * dtm2_ptr,
                           uint16_t amountOfBytes_u16);
void eink_drv_board_gpio_deinit (void);
void eink_drv_set_command (eink_drv_commands new_command);
eink_drv_commands eink_drv_get_command (void);

#endif /* EINK_DRV_EINK_DRV_H_ */
