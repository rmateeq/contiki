/*
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
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
/**
 * \addtogroup cc2538-smartrf-sensors
 * @{
 *
 * Driver for the Re-Mote 2.4Ghz antenna switch, to enable either the internal
 * ceramic antenna or an external one connected to the uFL connector
 * @{
 *
 * \file
 * Driver for the Re-Mote 2.4Ghz antenna switch
 */
#include <stdio.h>
#include "contiki.h"
#include "dev/gpio.h"
#include "antenna-sw.h"

#define ANTENNA_2_4GHZ_SW_PORT_BASE  GPIO_PORT_TO_BASE(ANTENNA_2_4GHZ_SW_PORT)
#define ANTENNA_2_4GHZ_SW_PIN_MASK   GPIO_PIN_MASK(ANTENNA_2_4GHZ_SW_PIN)

static uint8_t initialized = 0;

/*---------------------------------------------------------------------------*/
/**
 * \brief Init function for the antenna switch
 *
 * The Re-Mote platform allows to programatically select between the 2.4Ghz
 * internal or external antenna.
 * The function is set to enable using the internal ceramic antenna as default,
 * it should be called from the contiki-main initialization process.
 *
 * \return ignored
 */
int
config_antenna_sw(void)
{
  /* Software controlled */
  GPIO_SOFTWARE_CONTROL(ANTENNA_2_4GHZ_SW_PORT_BASE, ANTENNA_2_4GHZ_SW_PIN_MASK);

  /* Set pin to output */
  GPIO_SET_OUTPUT(ANTENNA_2_4GHZ_SW_PORT_BASE, ANTENNA_2_4GHZ_SW_PIN_MASK);

  /* Set the antenna selector to a default position */
  GPIO_CLR_PIN(ANTENNA_2_4GHZ_SW_PORT_BASE, ANTENNA_2_4GHZ_SW_PIN_MASK);

  initialized = 1;

  return 1;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief Function to select between the internal or external 2.4Ghz antenna
 *
 * \param val Write 1 to enable the internal antenna, 0 for the external one
 * \return the selected antenna position, -1 if not previously configured
 */
int
select_antenna_sw(uint8_t val)
{

  if(!initialized) {
    return -1;
  }

  /* Set the antenna selector to a default position */
  GPIO_WRITE_PIN(ANTENNA_2_4GHZ_SW_PORT_BASE, ANTENNA_2_4GHZ_SW_PIN_MASK, val);

  return val;
}
/*---------------------------------------------------------------------------*/
