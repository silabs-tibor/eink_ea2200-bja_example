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
 
 /***************************************************************************//**
 * @addtogroup Eink driver 
 * @brief Eink driver 
 *   AN0063 related source code
 * @{
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

/***************************************************************************//**
 * @brief
 *  Update the EPD display's content. 
 *
 * @note
 *  The functionality also performs the initialization of the used GPIO's, 
 *  perform the initialization process of the EPD related COG, updates its DTM 
 *  buffers via parameter 1 and parameter 2. After the image update, this 
 *  function performs the EPD related turn-off process, and de-initialize the 
 *  EPD used peripherals.
 *
 * @param[in] dtm1_ptr
 *   Pointer to the beginning of the host MCU related DTM_1 buffer
 *
 * @param[in] dtm2_ptr
 *   Pointer to the beginning of the host MCU related DTM_2 buffer
 *
 * @param[in] amountOfBytes_u16
 *   Amount of the bytes in the buffer. Most of the time equal with the EPD's
 *   pixel amount / 8
 *
 ******************************************************************************/
void eink_drv_update_image(uint8_t * dtm1_ptr,
                           uint8_t * dtm2_ptr,
                           uint16_t amountOfBytes_u16);
                           
/***************************************************************************//**
 * @brief
 *  Sets the EPD used GPIOs into Disable state
 *
 * @note
 *  Used for decreasing the current consumption as GPIO pins are used to power up 
 *  the EPD.
 *
 ******************************************************************************/                           
void eink_drv_board_gpio_deinit (void);

/***************************************************************************//**
 * @brief
 *  Interface for the application to store the actually running command. 
 *
 * @param[in] new_command
 *   Sets the actually running command.
 *
 ******************************************************************************/
void eink_drv_set_command (eink_drv_commands new_command);

/***************************************************************************//**
 * @brief
 *  Interface for the application to get the actually running command. 
 *
 * @return 
 *   Gets the actually running command.
 *
 ******************************************************************************/
eink_drv_commands eink_drv_get_command (void);

/** @} (end addtogroup Eink driver ) */

#endif /* EINK_DRV_EINK_DRV_H_ */
