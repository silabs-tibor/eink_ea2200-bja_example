/***************************************************************************//**
 * @file spi_init.c
 * @brief Initializes USART peripheral in order to act as 9bit SPI interface
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

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "spi_init.h"
#include "eink_pinmapping.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

void spi_init_S_initUsart0(void);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * spi_init_init()
 *
 *****************************************************************************/
void spi_init_init(){
  spi_init_init_gpio();
  spi_init_S_initUsart0();
}


/**************************************************************************//**
 * @brief
 *    GPIO de-initialization
 *****************************************************************************/
void spi_init_deinit(void){
    
  CMU_ClockEnable (cmuClock_USART0, false);

  // Configure TX pin as an output
  GPIO_PinModeSet(WSTK6_SDA_COMBAINED, gpioModeDisabled, 0);

  // Configure CLK pin as an output low (CPOL = 0)
  GPIO_PinModeSet(WSTK8_SCL_COMBAINED, gpioModeDisabled, 0);

  // Configure CS pin as an output and drive inactive high
  GPIO_PinModeSet(WSTK10_CS_COMBAINED, gpioModeDisabled, 0);

}


/**************************************************************************//**
 * spi_init_init_gpio()
 *
 *****************************************************************************/
void spi_init_init_gpio(void)
{
  // Enable clock (not needed on xG21)
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure TX pin as an output
  GPIO_PinModeSet(WSTK6_SDA_COMBAINED, WSTK6_SDA_MODE, 0);

  // Configure CLK pin as an output low (CPOL = 0)
  GPIO_PinModeSet(WSTK8_SCL_COMBAINED, WSTK8_SCL_MODE, 0);

  // Configure CS pin as an output and drive inactive high
  GPIO_PinModeSet(WSTK10_CS_COMBAINED, WSTK10_CS_MODE, 1);
}


/**************************************************************************//**
 * USART_SPI_9bit_Tx()
 *
 *****************************************************************************/
void USART_SPI_9bit_Tx(USART_TypeDef *usart, uint16_t data)
{
  /* Check that transmit buffer is empty */
  while (!(usart->STATUS & USART_STATUS_TXBL)) {
  }

  usart->CTRL = (usart->CTRL)&~_USART_CTRL_BIT8DV_MASK;
  if (data&(1<<8)){
    usart->CTRL |= USART_CTRL_BIT8DV;
  }


  usart->TXDATA = 0xFF & data;

  while (!(usart->STATUS & USART_STATUS_TXC));

}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * spi_init_S_initUsart0()
 *
 *****************************************************************************/
void spi_init_S_initUsart0(void)
{
  // Default asynchronous initializer (master mode, 1 Mbps, 8-bit data)
  USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;

  // Enable clock (not needed on xG21)
  CMU_ClockEnable(cmuClock_USART0, true);
  init.msbf = true;   // MSB first transmission for SPI compatibility
  init.databits = usartDatabits9; // 9 bits frame format
  init.autoCsEnable = true;       //
  init.baudrate = (115200);  // speeding up the transmission with 100
  /*
   * Route USART0 RX, TX, and CLK to the specified pins.  Note that CS is
   * not controlled by USART0 so there is no write to the corresponding
   * USARTROUTE register to do this.
   */
  GPIO->USARTROUTE[0].TXROUTE = (WSTK6_SDA_PORT << _GPIO_USART_TXROUTE_PORT_SHIFT)
      | (WSTK6_SDA_PIN << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].CLKROUTE = (WSTK8_SCL_PORT << _GPIO_USART_CLKROUTE_PORT_SHIFT)
      | (WSTK8_SCL_PIN << _GPIO_USART_CLKROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].CSROUTE = (WSTK10_CS_PORT << _GPIO_USART_CLKROUTE_PORT_SHIFT)
      | (WSTK10_CS_PIN << _GPIO_USART_CLKROUTE_PIN_SHIFT);

  // Enable USART interface pins
  GPIO->USARTROUTE[0].ROUTEEN = GPIO_USART_ROUTEEN_TXPEN |    // MOSI
                                GPIO_USART_ROUTEEN_CSPEN |
                                GPIO_USART_ROUTEEN_CLKPEN;


  // Configure and enable USART0
  USART_InitSync(USART0, &init);
}








