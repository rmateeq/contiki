/*
 * Copyright (c) 2011, Swedish Institute of Computer Science.
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

#include "contiki.h"
#include "lib/random.h"
#include "sys/ctimer.h"
#include "sys/etimer.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ip/uip-debug.h"

#include "sys/node-id.h"
#include "dev/leds.h"
#include "simple-udp.h"
#include "test-example.h"
#include "servreg-hack.h"
#include "dev/cc2420/cc2420.h"

#include <stdio.h>
#include <string.h>

#include "dev/adxl345.h"
#include "dev/battery-sensor.h"
#include "dev/i2cmaster.h"
#include "dev/tmp102.h"

#define ID              8
#define SEND_INTERVAL	(5 * CLOCK_SECOND)
#define SEND_TIME	(random_rand() % (SEND_INTERVAL))

static struct my_msg_t msg;
static struct my_msg_t *msgPtr = &msg;
static struct simple_udp_connection unicast_connection;

/*---------------------------------------------------------------------------*/
PROCESS(unicast_sender_process, "Unicast sender example process");
AUTOSTART_PROCESSES(&unicast_sender_process);
/*---------------------------------------------------------------------------*/
static void
receiver(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen)
{
  leds_toggle(LEDS_GREEN);
  printf("\n***\nMessage from: ");
  uip_debug_ipaddr_print(sender_addr);
  printf("\nData received on port %d from port %d with length %d\n",
         receiver_port, sender_port, datalen);
  printf("CH: %u RSSI: %d LQI %u\n", cc2420_get_channel(), cc2420_last_rssi,
                                     cc2420_last_correlation);
  printf("Mesage: %s\n", data);
}
/*---------------------------------------------------------------------------*/
static void
set_global_address(void)
{
  uip_ipaddr_t ipaddr;
  int i;
  uint8_t state;

  uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);

  printf("IPv6 addresses: ");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      uip_debug_ipaddr_print(&uip_ds6_if.addr_list[i].ipaddr);
      printf("\n");
    }
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(unicast_sender_process, ev, data)
{
  static struct etimer periodic_timer;
  static struct etimer send_timer;

  uip_ipaddr_t *addr;

  PROCESS_BEGIN();

  /* Initialize the sensors */
  tmp102_init();
  accm_init();
  SENSORS_ACTIVATE(battery_sensor);

  /* Register and start sending */
  servreg_hack_init();

  set_global_address();

  simple_udp_register(&unicast_connection, UDP_PORT,
                      NULL, UDP_PORT, receiver);

  etimer_set(&periodic_timer, SEND_INTERVAL);

  while(1) {

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    etimer_reset(&periodic_timer);
    etimer_set(&send_timer, SEND_TIME);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&send_timer));
    addr = servreg_hack_lookup(SERVICE_ID);

    msg.id = ID;

    if(addr != NULL) {

      static uint16_t temp;
      static uint16_t batt;
      static uint8_t x_val;
      static uint8_t y_val;
      static uint8_t z_val;
      static unsigned int message_number;

      printf("\n***\nSending unicast to ");
      uip_debug_ipaddr_print(addr);

      /* Read from the sensors */
      batt = battery_sensor.value(0);
      x_val = accm_read_axis(X_AXIS);
      y_val = accm_read_axis(Y_AXIS);
      z_val = accm_read_axis(Z_AXIS);
      temp  = tmp102_read_temp_x100();
      message_number++;

      /* Pack and send */
      msg.counter = message_number;
      msg.temp = temp;
      msg.x_axis = x_val;
      msg.y_axis = y_val;
      msg.z_axis = z_val;
      msg.battery = batt;

      printf("\nData to sent:\n    temp: %u, x: %u, y: %u, z: %u, batt: %u, counter: %u\n",
              temp, x_val, y_val, z_val, batt, message_number);

      simple_udp_sendto(&unicast_connection, msgPtr, sizeof(msg), addr);
    } else {
      printf("Searching for service %d\n", SERVICE_ID);
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
