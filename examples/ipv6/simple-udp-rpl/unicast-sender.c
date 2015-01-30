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
#include "dev/leds.h"
#include "sys/ctimer.h"
#include "sys/etimer.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ip/uip-debug.h"

#include "sys/node-id.h"

#include "simple-udp.h"
#include "servreg-hack.h"

#include "dev/i2cmaster.h"
#include "dev/tmp102.h"
#include "dev/adxl345.h"
#include "serial-feed.h"

#include <stdio.h>
#include <string.h>

#define DEBUG_APP 0

#define UDP_PORT 1234
#define SERVICE_ID 190

#define SEND_INTERVAL  CLOCK_SECOND/2
// #define SEND_TIME      (random_rand() % (SEND_INTERVAL))
#define SEND_TIME      CLOCK_SECOND/4

static struct simple_udp_connection unicast_connection;

static struct serialfeed_msg_t msg;
static struct serialfeed_msg_t *msgPtr = &msg;

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
  printf("Data received on port %d from port %d with length %d\n",
         receiver_port, sender_port, datalen);
}
/*---------------------------------------------------------------------------*/
static void
set_global_address(void)
{
  uip_ipaddr_t ipaddr;
  int i;
  uint8_t state;

  // Initialize the IPv6 address as below
  uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);

  // Set the last 64 bits of an IP address based on the MAC address
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);

  // Add to our list addresses
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
  int16_t x, y, z, temp = 0;
  uip_ipaddr_t *addr;

  PROCESS_BEGIN();

  // registering, disseminating, and looking up services. A service is identified 
  // by an 8-bit integer between 1 and 255. Integers below 128 are reserved for
  // system services.
  servreg_hack_init();

  // Set our global and local addresses
  set_global_address();

  // Added to get readings from the TMP102 sensor
  tmp102_init();

  // Added to get the acceleration values
  accm_init();

  // Accept messages from any address
  simple_udp_register(&unicast_connection, UDP_PORT,
                      NULL, UDP_PORT, receiver);

  // Fill once
  memcpy(msg.var_key, "545a202b76254223b5ffa65f", SERIALFEED_MSG_KEYLEN);
  printf("VAR %s\n", msg.var_key);

  etimer_set(&periodic_timer, SEND_INTERVAL);

  while(1) {

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    etimer_reset(&periodic_timer);
    etimer_set(&send_timer, SEND_TIME);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&send_timer));

    // This function returns the address of the node offering
    // a specific service. If the service is not known, the
    // function returns NULL. If there are more than one nodes
    // offering the service, this function returns the address
    // of the node that most recently announced its service.
    addr = servreg_hack_lookup(SERVICE_ID);

    if(addr != NULL) {

      temp = tmp102_read_temp_x100();

      x = accm_read_axis(X_AXIS);
      y = accm_read_axis(Y_AXIS);
      z = accm_read_axis(Z_AXIS);

      // Create the message
      msg.id  = 0xAB;
      msg.len = SERIALFEED_MSG_LEN;

      // Optimize this, I know...
      memcpy(&msg.value[0], &temp, 2);
      memcpy(&msg.value[1], &x, 2);
      memcpy(&msg.value[2], &y, 2);
      memcpy(&msg.value[3], &z, 2);

      leds_off(LEDS_RED);
      leds_toggle(LEDS_BLUE);
#if DEBUG_APP
      printf("Readings -> %d, (%d,%d,%d) to ", temp, x, y, z);
      uip_debug_ipaddr_print(addr);
      printf("\n");
#endif
      simple_udp_sendto(&unicast_connection, msgPtr, SERIALFEED_MSG_LEN, addr);
    } else {
      leds_off(LEDS_BLUE);
      leds_toggle(LEDS_RED);
      printf("Service %d not found\n", SERVICE_ID);
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
