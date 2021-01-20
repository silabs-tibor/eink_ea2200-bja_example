#ifndef MODULES_PERIPHERAL_INIT_SPI_INIT_INC_SPI_INIT_H_
#define MODULES_PERIPHERAL_INIT_SPI_INIT_INC_SPI_INIT_H_
/***************************************************************************//**
 * @file spi_init.h
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

#include "stdint.h"
#include "em_usart.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * @brief
 *  Initialise USART0 peripheral as SPI peripheral 
 *
 * @note
 *  The SPI acts as 9bit SPI. 
 *
 ******************************************************************************/ 
void spi_init_init(void);

/***************************************************************************//**
 * @brief
 *  Turn off the SPI peripheral.
 *
 * @note
 *  Turns off the clock of the peripheral and the related GPIOs will be 
 *  disabled.
 *
 ******************************************************************************/ 
void spi_init_deinit(void);

/***************************************************************************//**
 * @brief
 *  Initializes the SPI related GPIOs
 *
 * @note
 *  CS, CLK and MOSI pins initialisation.
 *
 ******************************************************************************/ 
void spi_init_init_gpio(void);

/***************************************************************************//**
 * @brief
 *  Sends 9bit SPI command
 *
 * @param[in] *usart
 *   The USART peripheral related pointer. If the application note related 
 *   configuration is used, this value should be USART0
 *
 * @param[in] data
 *   Used for sending the 9bit SPI data.
 *
 ******************************************************************************/ 
void USART_SPI_9bit_Tx(USART_TypeDef *usart, uint16_t data);

/** @} (end addtogroup Eink driver ) */

#endif /* MODULES_PERIPHERAL_INIT_SPI_INIT_INC_SPI_INIT_H_ */
