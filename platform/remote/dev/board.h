/*
 * Copyright (c) 2012, Texas Instruments Incorporated - http://www.ti.com/
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** \addtogroup cc2538
 * @{
 *
 * \defgroup cc2538-smartrf Re-Mote Peripherals
 *
 * Defines related to the Re-Mote
 *
 * This file provides connectivity information on LEDs, Buttons, UART and
 * other SmartRF peripherals
 *
 * This file can be used as the basis to configure other platforms using the
 * cc2538 SoC.
 * @{
 *
 * \file
 * Header file with definitions related to the I/O connections on the Zolertia's
 * Re-Mote platform, cc2538-based
 *
 * \note   Do not include this file directly. It gets included by contiki-conf
 *         after all relevant directives have been set.
 */
#ifndef BOARD_H_
#define BOARD_H_

#include "dev/gpio.h"
#include "dev/nvic.h"
/*---------------------------------------------------------------------------*/
/** \name SmartRF LED configuration
 *
 * LEDs on the Re-mote are connected as follows:
 * - LED1 (Red)    -> PD2
 * - LED2 (Blue)   -> PC3
 * - LED3 (Green)  -> PD5
 *
 * LED1 routed also to JP5 connector
 * LED2 shares the same pin with Watchdog WDI pulse and routed to JP8 connector
 * LED3 routed also to JP5 connector
 * @{
 */
/*---------------------------------------------------------------------------*/
/* Some files include leds.h before us, so we need to get rid of defaults in
 * leds.h before we provide correct definitions */
#undef LEDS_GREEN
#undef LEDS_YELLOW
#undef LEDS_RED
#undef LEDS_CONF_ALL

/* In leds.h the LEDS_BLUE is defined by LED_YELLOW definition
   LEDS_YELLOW value choosen to avoid ambiguity with possible LEDs combinations,
   the proper GPIO mask is given by LEDS_CONF_PXx definition */
#define LEDS_GREEN    5 /**< LED3 (Green) -> PD5 */
#define LEDS_YELLOW   1 /**< LED2 (Blue)  -> PC3 */
#define LEDS_RED      2 /**< LED1 (Red)   -> PD2 */

/* Only groups PDx LEDs, PC3 have to be handled separately */
#define LEDS_CONF_PDx 7
#define LEDS_CONF_PCx 3
#define LEDS_CONF_ALL 8

/* Notify various examples that we have LEDs */
#define PLATFORM_HAS_LEDS        1
/** @} */
/*---------------------------------------------------------------------------*/
/** \name USB configuration
 *
 * The USB pullup is enabled by an external resistor, not mapped to a GPIO
 */
#ifdef USB_PULLUP_PORT
#undef USB_PULLUP_PORT
#endif
#ifdef USB_PULLUP_PIN
#undef USB_PULLUP_PIN
#endif
/** @} */
/*---------------------------------------------------------------------------*/
/** \name UART configuration
 *
 * On the Re-Mote, the UART is connected to the following ports/pins
 * - UART0:
 *   - RX:  PA0
 *   - TX:  PA1
 * - UART1:
 *   - RX:  PC6
 *   - TX:  PC5
 *   - CTS: 
 *   - RTS: 
 * We configure the port to use UART0 and UART1, CTS/RTS only for UART1,
 * both without a HW pull-up resistor
 * @{
 */
#define UART0_RX_PORT            GPIO_A_NUM
#define UART0_RX_PIN             0
#define UART0_TX_PORT            GPIO_A_NUM
#define UART0_TX_PIN             1

#define UART1_RX_PORT            GPIO_C_NUM
#define UART1_RX_PIN             6
#define UART1_TX_PORT            GPIO_C_NUM
#define UART1_TX_PIN             5
#define UART1_CTS_PORT           GPIO_C_NUM
#define UART1_CTS_PIN            1
#define UART1_RTS_PORT           GPIO_C_NUM
#define UART1_RTS_PIN            2
/** @} */
/*---------------------------------------------------------------------------*/
/** \name Re-Mote Button configuration
 *
 * Buttons on the Re-Mote are connected as follows:
 * - BUTTON_USER  -> PA3, S1 user button, shared with bootloader
 * - BUTTON_RESET -> RESET_N line, S2 reset both CC2538 and CoP
 * - BUTTON_VBAT  -> Power switch, not mounted by default
 * @{
 */
/** BUTTON_USER -> PA3 */
#define BUTTON_USER_PORT       GPIO_A_NUM
#define BUTTON_USER_PIN        3
#define BUTTON_USER_VECTOR     NVIC_INT_GPIO_PORT_A

/* Notify various examples that we have Buttons */
#define PLATFORM_HAS_BUTTON      1
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name ADC configuration
 *
 * These values configure which CC2538 pins and ADC channels to use for the ADC
 * inputs. By default the Re-Mote allows two out-of-the-box ADC ports with a 
 * phidget-like 3-pin connector (GND/3V3/ADC)
 *
 * ADC inputs can only be on port A.
 * @{
 */
#define ADC_PHIDGET_PORT         GPIO_A_NUM /**< Phidget GPIO control port */
#define ADC_PHIDGET_ADC2_PIN     6 /**< ADC2 to PA6, 3V3 */
#define ADC_PHIDGET_ADC3_PIN     7 /**< ADC3 to PA7, 3V3 */
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name SPI configuration
 *
 * These values configure which CC2538 pins to use for the SPI lines, shared 
 * with the I2C lines
 * TODO: rename better...
 * @{
 */
#define SPI_CLK_PORT             GPIO_B_NUM /**< Shared with SCL */
#define SPI_CLK_PIN              1
#define SPI_MOSI_PORT            GPIO_B_NUM /**< Shared with SDA */
#define SPI_MOSI_PIN             0
#define SPI_MISO_PORT            GPIO_C_NUM /**< as GPIO for I2C */
#define SPI_MISO_PIN             1
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name Antenna switch configuration
 *
 * These values configure the required pin to drive the antenna switch, to 
 * use either the built-in ceramic antenna or an external one over the uFL
 * connector
 * - Internal antenna: LOW
 * - External antenna: HIGH
 * @{
 */
#define ANTENNA_2_4GHZ_SW_PORT GPIO_D_NUM
#define ANTENNA_2_4GHZ_SW_PIN  4
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name CC1120/CC1200 configuration
 *
 * These values configure the required pins to drive the CC1120/CC1200
 * @{
 */
#define CC1120_CLK_PORT             GPIO_D_NUM
#define CC1120_CLK_PIN              1
#define CC1120_MOSI_PORT            GPIO_D_NUM
#define CC1120_MOSI_PIN             0
#define CC1120_MISO_PORT            GPIO_C_NUM
#define CC1120_MISO_PIN             4
#define CC1120_CS_PORT              GPIO_D_NUM
#define CC1120_CS_PIN               3
#define CC1120_G0_PORT              GPIO_B_NUM
#define CC1120_G0_PIN               4
#define CC1120_G2_PORT              GPIO_B_NUM
#define CC1120_G2_PIN               3
#define CC1120_RESET_PORT           GPIO_B_NUM
#define CC1120_RESET_PIN            2
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name microSD configuration
 *
 * These values configure the required pins to drive the built-in microSD 
 * external module
 * @{
 */
#define USD_CLK_PORT             GPIO_B_NUM
#define USD_CLK_PIN              5
#define USD_MOSI_PORT            GPIO_C_NUM
#define USD_MOSI_PIN             7
#define USD_MISO_PORT            GPIO_A_NUM
#define USD_MISO_PIN             4
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name Device string used on startup
 * @{
 */
#define BOARD_STRING "Zolertia Re-Mote platform"
/** @} */

#endif /* BOARD_H_ */

/**
 * @}
 * @}
 */
