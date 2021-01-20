/***************************************************************************//**
 * @file eink_drv.c
 * @brief Initialize the EPD and realizes the image updates
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
#include "sleep_delay.h"
#include "em_gpio.h"
#include "em_cmu.h"
#include "eink_pinmapping.h"
#include "eink_drv.h"
#include "em_usart.h"
#include "spi_init.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#define BUSYN GPIO_PinInGet(WSTK13_BUSY_COMBAINED)
#define BS_H GPIO_PinOutSet(WSTK7_BS1_COMBAINED)
#define RSTN_L GPIO_PinOutClear(WSTK9_RESET_COMBAINED)
#define RSTN_H GPIO_PinOutSet(WSTK9_RESET_COMBAINED)
#define POWER_ON GPIO_PinOutSet(WSTK15_POWER_COMBAINED)
#define POWER_OFF GPIO_PinOutClear(WSTK15_POWER_COMBAINED)
#define GND_ON GPIO_PinOutSet(WSTK16_GND_COMBAINED)
#define GND_OFF GPIO_PinOutClear(WSTK16_GND_COMBAINED)

#define EINK_DRV_SEND_DATA 1<<8

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

static void eink_drv_S_board_gpio_init (void);
static void eink_drv_S_board_peripheral_clock_init (void);
static void eink_drv_S_display_init (void);
static void eink_drv_S_inithw (void);
static void eink_drv_S_check_busy_low (void);
static void eink_drv_S_check_busy_high (void);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

static eink_drv_commands command = nothing;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * eink_drv_update_image()
 *
 *****************************************************************************/
void eink_drv_update_image(uint8_t * dtm1_ptr,
                           uint8_t * dtm2_ptr,
                           uint16_t amountOfBytes_u16) {

  // init periherals
  eink_drv_S_inithw ();
  GND_OFF;
  POWER_ON;
  eink_drv_S_display_init ();

  uint32_t i;

  USART_SPI_9bit_Tx (USART0, PON);
  eink_drv_S_check_busy_high ();

  USART_SPI_9bit_Tx (USART0, DTM1);
  for (i = 0; i < amountOfBytes_u16; i++) {
    uint8_t p = * dtm1_ptr;
    dtm1_ptr++;
    USART_SPI_9bit_Tx (USART0, EINK_DRV_SEND_DATA | p);

  }
  USART_SPI_9bit_Tx (USART0, DTM2);

  for (i = 0; i < amountOfBytes_u16; i++) {

    uint8_t p = * dtm2_ptr;
    dtm2_ptr++;
    USART_SPI_9bit_Tx (USART0, EINK_DRV_SEND_DATA | p);
  }
  USART_SPI_9bit_Tx (USART0, DRF);
  eink_drv_S_check_busy_high ();

  USART_SPI_9bit_Tx (USART0, POF);
  eink_drv_S_check_busy_low ();

  // release GPIO - unpower the display
  POWER_OFF;
  eink_drv_board_gpio_deinit ();

}

/**************************************************************************//**
 * eink_drv_set_command
 *
 *****************************************************************************/
void eink_drv_set_command (eink_drv_commands new_command) {
  command = new_command;
}

/**************************************************************************//**
 * eink_drv_get_command
 *
 *****************************************************************************/
eink_drv_commands eink_drv_get_command (void) {
  return command;
}

/**************************************************************************//**
 * eink_drv_board_gpio_deinit
 *
 *****************************************************************************/
void eink_drv_board_gpio_deinit () {
  /*
   * Inputs - from view point of WSTK
   */
  // BUSY
  GPIO_PinModeSet (WSTK13_BUSY_COMBAINED, gpioModeDisabled, 0);

  /*
   * Outputs - from view point of WSTK
   */
  // BS
  GPIO_PinModeSet (WSTK7_BS1_COMBAINED, gpioModeDisabled, 0);

  // RESET
  GPIO_PinModeSet (WSTK9_RESET_COMBAINED, gpioModeDisabled, 0);

  // + power source for eink display
  GPIO_PinModeSet (WSTK15_POWER_COMBAINED, gpioModeDisabled, 0);

  // gnd source for eink display
  GPIO_PinModeSet (WSTK16_GND_COMBAINED, gpioModeDisabled, 0);

  // Configure TX pin as an output
  GPIO_PinModeSet (WSTK6_SDA_COMBAINED, gpioModeDisabled, 0);

  // Configure CLK pin as an output low (CPOL = 0)
  GPIO_PinModeSet (WSTK8_SCL_COMBAINED, gpioModeDisabled, 0);

  // Configure CS pin as an output and drive inactive high
  GPIO_PinModeSet (WSTK10_CS_COMBAINED, gpioModeDisabled, 0);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * eink_drv_S_board_gpio_init
 *
 *****************************************************************************/
static void eink_drv_S_board_gpio_init () {
  /*
   * Inputs - from view point of WSTK
   */
  // BUSY
  GPIO_PinModeSet (WSTK13_BUSY_COMBAINED, gpioModeInput, 0);

  /*
   * Outputs - from view point of WSTK
   */
  // BS
  GPIO_PinModeSet (WSTK7_BS1_COMBAINED, gpioModePushPull, 0);
  // RESET
  GPIO_PinModeSet (WSTK9_RESET_COMBAINED, gpioModePushPull, 0);

  // + power source for eink display
  GPIO_PinModeSet (WSTK15_POWER_COMBAINED, gpioModePushPull, 0);

  // highest current to power source pin
  GPIO->P[gpioPortA].CTRL |= 0x7 << 4;

  // gnd source for eink display
  GPIO_PinModeSet (WSTK16_GND_COMBAINED, gpioModePushPull, 0);

  // Configure TX pin as an output
  GPIO_PinModeSet (WSTK6_SDA_COMBAINED, WSTK6_SDA_MODE, 0);

  // Configure CLK pin as an output low (CPOL = 0)
  GPIO_PinModeSet (WSTK8_SCL_COMBAINED, WSTK8_SCL_MODE, 0);

  // Configure CS pin as an output and drive inactive high
  GPIO_PinModeSet (WSTK10_CS_COMBAINED, WSTK10_CS_MODE, 1);

}

/**************************************************************************//**
 * eink_drv_S_display_init
 *
 *****************************************************************************/
static void eink_drv_S_display_init (void) {

  BS_H;
  sleep_delay_ms (20);
  RSTN_L; // Reset
  sleep_delay_ms (20);
  RSTN_H;
  sleep_delay_ms (20);

  /* Power ON */
  USART_SPI_9bit_Tx (USART0, PON);
  eink_drv_S_check_busy_high ();

  /* Panel Setting */
  /*    0b10001111
   *    D7-D6: 10 - 128*296
   *    D5   :  0 - LUT from OTP (default)
   *    D4   :  0 - Pixel with B/W/Red (default)
   *    D3   :  1 - Scan up
   *
   * */
  USART_SPI_9bit_Tx (USART0, PSR);
  USART_SPI_9bit_Tx (USART0, EINK_DRV_SEND_DATA | 0x8F);

  /* Power OFF Sequence */
  USART_SPI_9bit_Tx (USART0, PFS);
  USART_SPI_9bit_Tx (USART0, EINK_DRV_SEND_DATA | 0x00);

  /* Booster Soft Start */
  USART_SPI_9bit_Tx (USART0, BTST);
  USART_SPI_9bit_Tx (USART0, EINK_DRV_SEND_DATA | 0x17);
  USART_SPI_9bit_Tx (USART0, EINK_DRV_SEND_DATA | 0x17);
  USART_SPI_9bit_Tx (USART0, EINK_DRV_SEND_DATA | 0x17);

  USART_SPI_9bit_Tx (USART0, LUTOPT);
  USART_SPI_9bit_Tx (USART0, EINK_DRV_SEND_DATA | 0x80);
  USART_SPI_9bit_Tx (USART0, EINK_DRV_SEND_DATA | 0x00);

  /* Temperature Sensor selection */
  USART_SPI_9bit_Tx (USART0, TSE);
  USART_SPI_9bit_Tx (USART0, EINK_DRV_SEND_DATA | 0x00);

  /* VCom and data interval setting */
  USART_SPI_9bit_Tx (USART0, CDI);
  USART_SPI_9bit_Tx (USART0, EINK_DRV_SEND_DATA | 0x37);

  /* Gate and source related something */
  USART_SPI_9bit_Tx (USART0, TCON);
  USART_SPI_9bit_Tx (USART0, EINK_DRV_SEND_DATA | 0x22);

  /* Power OFF */
  USART_SPI_9bit_Tx (USART0, POF);
  eink_drv_S_check_busy_low ();
}

/**************************************************************************//**
 * eink_drv_S_check_busy_high
 *
 *****************************************************************************/
static void eink_drv_S_check_busy_high (void) // If BUSYN=0 then waiting
{

  while (!(BUSYN)) {
    sleep_delay_ms (400); // ~400msec
  }

}

/**************************************************************************//**
 * eink_drv_S_check_busy_low
 *
 *****************************************************************************/
static void eink_drv_S_check_busy_low (void) // If BUSYN=1 then waiting
{

  while (BUSYN) {
    sleep_delay_ms (400);
  }
}

/**************************************************************************//**
 * eink_drv_S_inithw
 *
 *****************************************************************************/
static void eink_drv_S_inithw () {
  eink_drv_S_board_peripheral_clock_init ();
  eink_drv_S_board_gpio_init ();

}

/**************************************************************************//**
 * eink_drv_S_board_peripheral_clock_init
 *
 *****************************************************************************/
static void eink_drv_S_board_peripheral_clock_init () {
  CMU_ClockEnable (cmuClock_GPIO, true);
}



