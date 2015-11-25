/*
 * Copyright (c) 2015, Zolertia - http://www.zolertia.com
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
 * \addtogroup remote-demo-stand
 * @{
 *
 * \file
 *     Example demonstrating a connected demo stand
 */
#include "contiki.h"
#include "cpu.h"
#include "sys/etimer.h"
#include "sys/rtimer.h"
#include "dev/leds.h"
#include "dev/uart.h"
#include "dev/watchdog.h"
#include "dev/serial-line.h"
#include "dev/sys-ctrl.h"
#include "net/rime/broadcast.h"

#include <stdio.h>
#include <stdint.h>
/*---------------------------------------------------------------------------*/
#define LOOP_PERIOD         1
#define LOOP_INTERVAL       (CLOCK_SECOND * LOOP_PERIOD)
#define LEDS_RADIO_RX       LEDS_BLUE
#define LEDS_PERIODIC       LEDS_GREEN
#define BROADCAST_CHANNEL   108
/*---------------------------------------------------------------------------*/
static struct etimer et;
/*---------------------------------------------------------------------------*/
PROCESS(stand_demo_process, "Stand demo process");
AUTOSTART_PROCESSES(&stand_demo_process);
/*---------------------------------------------------------------------------*/
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  char buf_ptr[64];
  int8_t recv_rssi;
  uint16_t recv_addr;

  recv_rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);
  recv_addr = (uint16_t)(from->u8[1] << 8) + from->u8[0];
  snprintf(buf_ptr, 32, "ID:0x%04X,RSSI:%d,DATA:%05u",
                         recv_addr, recv_rssi,
                         *(uint16_t *)packetbuf_dataptr());
  printf("%s\n", buf_ptr);
  leds_toggle(LEDS_RADIO_RX);
}
/*---------------------------------------------------------------------------*/
static const struct broadcast_callbacks bc_rx = { broadcast_recv };
static struct broadcast_conn bc;
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(stand_demo_process, ev, data)
{
  PROCESS_EXITHANDLER(broadcast_close(&bc))

  PROCESS_BEGIN();

#if TX_NODE
  static uint16_t counter;
#endif

  broadcast_open(&bc, BROADCAST_CHANNEL, &bc_rx);

  while(1) {

    etimer_set(&et, LOOP_INTERVAL);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
#if TX_NODE
    counter++;
    packetbuf_copyfrom(&counter, sizeof(counter));
    broadcast_send(&bc);
    leds_toggle(LEDS_PERIODIC);
#endif
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 */
