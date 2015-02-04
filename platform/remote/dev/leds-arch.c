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
/**
 * \addtogroup cc2538-smartrf
 * @{
 *
 * \defgroup cc2538dk-leds Re-Mote LED driver
 *
 * LED driver implementation for the Re-Mote platform
 * @{
 *
 * \file
 * LED driver implementation for the Re-Mote platform
 */
#include "contiki.h"
#include "reg.h"
#include "dev/leds.h"
#include "dev/gpio.h"

/*---------------------------------------------------------------------------*/
void
leds_arch_init(void)
{
  /* Initialize LED1 (Red) and LED3 (Green) */
  GPIO_SET_OUTPUT(GPIO_D_BASE, LEDS_CONF_PDx);
  /* Initialize LED2 - Blue */
  GPIO_SET_OUTPUT(GPIO_C_BASE, LEDS_CONF_PCx);
}
/*---------------------------------------------------------------------------*/
unsigned char
leds_arch_get(void)
{
  /* As LEDS_RED plus LEDS_YELLOW equals LEDS_GREEN, this is inconvenient,
   * fix this */
  uint8_t mask_leds;
  if (GPIO_READ_PIN(GPIO_C_BASE, LEDS_CONF_PCx)){
    mask_leds = LEDS_YELLOW;
  }
  mask_leds += GPIO_READ_PIN(GPIO_D_BASE, LEDS_CONF_PDx);
  return mask_leds;
}
/*---------------------------------------------------------------------------*/
void
leds_arch_set(unsigned char leds)
{
  /* Possible combinations for leds value, not optimized...
   * 1 -> LEDS_YELLOW
   * 2 -> LEDS_RED
   * 3 -> LEDS_YELLOW + LEDS_RED
   * 5 -> LEDS_GREEN
   * 6 -> LEDS_GREEN + LEDS_YELLOW
   * 7 -> LEDS_GREEN + LEDS_RED
   * 8 -> LEDS_ALL
   */

  uint8_t mask_leds_d;

  if ((leds == 1) || (leds == 3) || (leds == 6) || (leds == LEDS_CONF_ALL)){
    GPIO_WRITE_PIN(GPIO_C_BASE, LEDS_CONF_PCx, LEDS_CONF_PCx);
  }

  if ((leds == 2) || (leds == 3) || (leds == 7) || (leds == LEDS_CONF_ALL)){
    GPIO_WRITE_PIN(GPIO_D_BASE, LEDS_RED, LEDS_RED);
  }

  if ((leds == 5) || (leds == 6) || (leds == 7) || (leds == LEDS_CONF_ALL)){
    GPIO_WRITE_PIN(GPIO_D_BASE, LEDS_GREEN, LEDS_GREEN);
  }
}
/*---------------------------------------------------------------------------*/

/**
 * @}
 * @}
 */
