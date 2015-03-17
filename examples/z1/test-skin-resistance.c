/*
 * Copyright (c) 2007, Swedish Institute of Computer Science.
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
 *         An example of how to use the button and light sensor on
 *         the Z1 platform.
 * \author
 *         Joakim Eriksson <joakime@sics.se>
 */

#include <stdio.h>
#include "contiki.h"
#include "dev/leds.h"
#include "dev/z1-phidgets.h"
#include "dev/button-sensor.h"
#include "serial-ubidots.h"

/*---------------------------------------------------------------------------*/
PROCESS(test_skin_resistance, "Test Skin Resistance Phidget");
AUTOSTART_PROCESSES(&test_skin_resistance);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(test_skin_resistance, ev, data)
{
  static struct etimer et;
  uint16_t skinr;
  PROCESS_BEGIN();
  SENSORS_ACTIVATE(phidgets);

  while(1) {
    etimer_set(&et, CLOCK_SECOND/8);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    skinr = phidgets.value(PHIDGET3V_2);
    // printf("Phidget 3V 2:%d\n", skinr);
    send_to_ubidots((const char *)"TEST", (const char *)"TEST", &skinr);
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
