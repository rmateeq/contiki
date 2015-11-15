/*
 * Copyright (c) 2015, Zolertia <http://www.zolertia.com>
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
 * This file is part of the Contiki operating system.
 *
 */
/**
 * \file
 *         A quick program for testing the Phidget rotation sensor (PN1109)
 * \author
 *         Antonio Lignan <alinan@zolertia.com>
 */
#include <stdio.h>
#include "contiki.h"
#include "dev/leds.h"
#include "dev/button-sensor.h"
#include "dev/z1-phidgets.h"
/*---------------------------------------------------------------------------*/
#define PERIOD  (CLOCK_SECOND/2)
/*---------------------------------------------------------------------------*/
PROCESS(test_rotation_process, "Test Rotation sensor (300º)");
AUTOSTART_PROCESSES(&test_rotation_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_rotation_process, ev, data)
{
  static uint32_t rotation;
  static struct etimer et;
  PROCESS_BEGIN();
  SENSORS_ACTIVATE(phidgets);

  while(1) {
    etimer_set(&et, PERIOD);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    rotation = phidgets.value(PHIDGET3V_2);
 
    /* Is a linear sensor, so 300/4095 = 0.07326, ADC*0.07326 = angle (º) */
    rotation *= 7326;
    rotation /= 100000;
    printf("Rotation (º):%lu\n", rotation);

    if(rotation <= 45) {
      leds_off(LEDS_ALL);
    }

    if((rotation > 45) && (rotation < 150)) {
      leds_on(LEDS_BLUE);
      leds_off(LEDS_GREEN | LEDS_RED);
    }

    if((rotation > 150) && (rotation < 250)) {
      leds_on(LEDS_GREEN | LEDS_BLUE);
      leds_off(LEDS_RED);
    }

    if(rotation >= 250) {
      leds_on(LEDS_ALL);
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
