/*
 * Copyright (c) 2012, Texas Instruments Incorporated - http://www.ti.com/
 * Copyright (c) 2015, Zolertia
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
/** \addtogroup platform
 * @{
 *
 * \defgroup zongle Zongle USB platform based on the CC2538
 *
 * Defines related to the Zongle
 *
 * This file provides connectivity information on LEDs, Buttons, UART and
 * other peripherals
 *
 * @{
 *
 * \file
 * Header file with definitions related to the I/O connections on the Zolertia's
 * Zongle platform, cc2538-based
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
 * LEDs on the Zongle are connected as follows:
 * - LED1 (Green)  -> PC1
 * - LED2 (Blue)   -> PC2
 * - LED3 (Red)    -> PC3
 * @{
 */
/*---------------------------------------------------------------------------*/
/* Some files include leds.h before us, so we need to get rid of defaults in
 * leds.h before we provide correct definitions */
#undef LEDS_GREEN
#undef LEDS_YELLOW
#undef LEDS_BLUE
#undef LEDS_RED
#undef LEDS_CONF_ALL

/* In leds.h the LEDS_BLUE is defined by LED_YELLOW definition */
#define LEDS_GREEN    1  /**< LED1 (Green) -> PC1 */
#define LEDS_BLUE     2  /**< LED2 (Blue)  -> PC2 */
#define LEDS_RED      4  /**< LED3 (Red)   -> PC3 */

#define LEDS_CONF_ALL 7

#define LEDS_LIGHT_BLUE (LEDS_GREEN | LEDS_BLUE) /**< Green + Blue (3) */
#define LEDS_YELLOW     (LEDS_GREEN | LEDS_RED)  /**< Green + Red (5) */
#define LEDS_PURPLE     (LEDS_BLUE | LEDS_RED)   /**< Blue + Red (6) */
#define LEDS_WHITE      LEDS_ALL                 /**< Green + Blue + Red (7) */

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
/** \name Re-Mote Button configuration
 *
 * Buttons on the Zongle are connected as follows:
 * - BUTTON_USER  -> PA0
 * @{
 */
#define BUTTON_USER_PORT       GPIO_A_NUM
#define BUTTON_USER_PIN        0
#define BUTTON_USER_VECTOR     NVIC_INT_GPIO_PORT_A

/* Notify various examples that we have a button, pressing can trigger reboot */
#define PLATFORM_HAS_BUTTON              1
#define BUTTON_SENSOR_RESET_INTERVALS    3
/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name SPI and I2C configuration
 *
 * TODO: For the zongle these only exist to make the respective drivers build
 * @{
 */
#define SPI_CLK_PORT             GPIO_A_NUM /**< Shared with SCL */
#define SPI_CLK_PIN              0
#define SPI_MOSI_PORT            GPIO_A_NUM /**< Shared with SDA */
#define SPI_MOSI_PIN             0
#define SPI_MISO_PORT            GPIO_A_NUM /**< as GPIO for I2C */
#define SPI_MISO_PIN             0

#define I2C_SCL_PORT             SPI_CLK_PORT
#define I2C_SCL_PIN              SPI_CLK_PIN
#define I2C_SDA_PORT             SPI_MOSI_PORT
#define I2C_SDA_PIN              SPI_MOSI_PIN

/** @} */
/*---------------------------------------------------------------------------*/
/**
 * \name Device string used on startup
 * @{
 */
#define BOARD_STRING "Zongle"
/** @} */

#endif /* BOARD_H_ */

/**
 * @}
 * @}
 */
