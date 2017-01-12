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
/*---------------------------------------------------------------------------*/
/**
 * \addtogroup zoul-hw-test
 * @{
 *
 * Test the Zoul hardware over AT API
 * @{
 *
 * \author
 *         Antonio Lignan <alinan@zolertia.com>
 */
#include "contiki.h"
#include "cpu.h"
#include "sys/etimer.h"
#include "sys/ctimer.h"
#include "sys/process.h"
#include "dev/adc.h"
#include "dev/leds.h"
#include "dev/watchdog.h"
#include "dev/sys-ctrl.h"
#include "dev/gpio.h"
#include "dev/ioc.h"
#include "net/rime/rime.h"
#include "net/netstack.h"
#include "dev/radio.h"
#include "dev/cc1200/cc1200-rf-cfg.h"
#include "dev/cc1200/cc1200-const.h"
#include "dev/cc1200/cc1200-arch.h"
#include "lib/list.h"
#include <stdio.h>
#include <stdint.h>
/*---------------------------------------------------------------------------*/
PROCESS(cc1200_rx_cont_process, "cc1200 tx cont mod process (low)");
AUTOSTART_PROCESSES(&cc1200_rx_cont_process);
/*---------------------------------------------------------------------------*/
#define CC1200_CONTINUOUS_RX_TRANSPARENT 1
/*---------------------------------------------------------------------------*/
/* Expected TX power (14dBm 0x7F) */
#define CC1200_EXPECTED_TXPOWER       0x7F
/* Expected RSSI offset */
#define CC1200_EXPECTED_RSSI_OFFSET  (-81)

/* Expected lowest frequency 863MHz */
#define CC1200_EXPECTED_FREQ0         0xCC
#define CC1200_EXPECTED_FREQ1         0x4C
#define CC1200_EXPECTED_FREQ2         0x56

/* CC1200 constants, expected during the test */
#define CC1200_EXPECTED_IDLE_STATE    0x00
#define CC1200_EXPECTED_RX_STATE      0x01
#define CC1200_EXPECTED_PARTNUMBER    0x20
#define CC1200_EXPECTED_PARTVERSION   0x11
#undef CC1200_RF_CFG
#define CC1200_RF_CFG                 cc1200_802154g_863_870_fsk_50kbps
/*---------------------------------------------------------------------------*/
#define ASSERTWAIT_UNTIL(cond, max_time)                                \
  do {                                                                  \
    rtimer_clock_t t0;                                                  \
    t0 = RTIMER_NOW();                                                  \
    while(!(cond) && RTIMER_CLOCK_LT(RTIMER_NOW(), t0 + (max_time))) {} \
  } while(0)
/*---------------------------------------------------------------------------*/
static uint8_t cc1200_on = 0;
extern const cc1200_rf_cfg_t CC1200_RF_CFG;
/*---------------------------------------------------------------------------*/
static struct etimer et;
/*---------------------------------------------------------------------------*/
static uint8_t
read_reg(uint16_t addr)
{
  uint8_t ret;
  cc1200_arch_spi_select();
  if(CC1200_IS_EXTENDED_ADDR(addr)) {
    cc1200_arch_spi_rw_byte(CC1200_EXTENDED_READ_CMD);
    cc1200_arch_spi_rw_byte((uint8_t)addr);
  } else {
    cc1200_arch_spi_rw_byte(addr | CC1200_READ_BIT);
  }
  ret = cc1200_arch_spi_rw_byte(0);
  cc1200_arch_spi_deselect();
  return ret;
}
/*---------------------------------------------------------------------------*/
static uint8_t
write_reg(uint16_t addr, uint8_t val)
{
  uint8_t ret;
  cc1200_arch_spi_select();
  if(CC1200_IS_EXTENDED_ADDR(addr)) {
    cc1200_arch_spi_rw_byte(CC1200_EXTENDED_WRITE_CMD);
    cc1200_arch_spi_rw_byte((uint8_t)addr);
  } else {
    cc1200_arch_spi_rw_byte(addr | CC1200_WRITE_BIT);
  }
  ret = cc1200_arch_spi_rw_byte(val);
  cc1200_arch_spi_deselect();
  return ret;
}
/*---------------------------------------------------------------------------*/
static void
default_cfg(const registerSetting_t *reg_settings,
                   uint16_t sizeof_reg_settings)
{
  int i = sizeof_reg_settings / sizeof(registerSetting_t);
  if(reg_settings != NULL) {
    while(i--) {
      write_reg(reg_settings->addr, reg_settings->val);
      reg_settings++;
    }
  }
}
/*---------------------------------------------------------------------------*/
static uint8_t
strobe(uint8_t strobe)
{
  uint8_t ret;
  cc1200_arch_spi_select();
  ret = cc1200_arch_spi_rw_byte(strobe);
  cc1200_arch_spi_deselect();
  return ret;
}
/*---------------------------------------------------------------------------*/
static uint8_t
strobe_test(void)
{
  uint8_t status = strobe(CC1200_SNOP);
  status &= 0x70;
  status = status >> 4;
  return status;
}
/*---------------------------------------------------------------------------*/
static void
cc1200_rx_test(void)
{
  if(!cc1200_on) {
    cc1200_arch_init();
  }

  printf("CC1200: %s\n", CC1200_RF_CFG.cfg_descriptor);
  default_cfg(CC1200_RF_CFG.register_settings, CC1200_RF_CFG.size_of_register_settings);

#if CC1200_CONTINUOUS_RX_TRANSPARENT
  /* Serial clock (RX and TX mode). Synchronous to the data in synchronous
   * serial mode. Data is set up on the falling edge in RX and is captured on
   * the rising edge of the serial clock in TX
   */
  write_reg(CC1200_IOCFG2, 0x08);

  /* Serial data (RX mode). Used for both synchronous and transparent mode.
   * Transparent mode: No timing recovery (outputs just the hard limited
   * baseband signal)
   */
  write_reg(CC1200_IOCFG0, 0x09);

  /* No idea what this does, I might, but I'm lazy... */
  write_reg(CC1200_SYNC_CFG1, 0xFF);

  /* No preamble */
  write_reg(CC1200_PREAMBLE_CFG1, 0x00);
  write_reg(CC1200_PREAMBLE_CFG0, 0x8A);

  /* No FIFO */
  write_reg(CC1200_MDMCFG1, 0x00);
  write_reg(CC1200_MDMCFG0, 0x05);

  /* Keep computing and updating the RSSI */
  write_reg(CC1200_AGC_CFG3, 0x11);

  /* FIFO magic stuff too lazy to read trolololo */
  write_reg(CC1200_FIFO_CFG, 0x00);

  /* Transparent mode */
  write_reg(CC1200_PKT_CFG2, 0x01);
#endif /* CC1200_CONTINUOUS_RX_TRANSPARENT */

  /* CC1200 will be put in RX mode to allow measuring over network analyser probe */
  write_reg(CC1200_FREQ0, CC1200_EXPECTED_FREQ0);
  write_reg(CC1200_FREQ1, CC1200_EXPECTED_FREQ1);
  write_reg(CC1200_FREQ2, CC1200_EXPECTED_FREQ2);


  strobe(CC1200_SRX);
  ASSERTWAIT_UNTIL((strobe_test() == CC1200_EXPECTED_RX_STATE), RTIMER_SECOND);
  printf("CC1200: HW test finished, in RX mode now\n\n");

  cc1200_on = 1;
}
/*---------------------------------------------------------------------------*/
static uint8_t
cc1200_reg_assert(uint16_t reg, uint16_t cond, uint8_t wait_cycles)
{
  volatile uint8_t i;
  uint16_t expected;

  do {
    i++;
    expected = read_reg(reg);
    if(i >= wait_cycles) {
      return 0;
    }
    printf("REG 0x%04X -> 0x%04X vs 0x%04X (%u)\n", reg, expected, cond, i);
  } while(expected != cond);
  return 1;
}
/*---------------------------------------------------------------------------*/
static void
cc1200_spi_test(void)
{
  /** 
   * Configure SPI, CSn and GPIOs, at cc1200-remote-arch enable DEBUG_LEVEL to
   * bootstrap and check CSn/MISO.
   */

  if(!cc1200_on) {
    cc1200_arch_init();
  }

  /* Put the CC1200 in idle */
  strobe(CC1200_SIDLE);
  ASSERTWAIT_UNTIL((strobe_test() == CC1200_EXPECTED_IDLE_STATE),
                   RTIMER_SECOND);

  cc1200_reg_assert(CC1200_PARTNUMBER, CC1200_EXPECTED_PARTNUMBER, 10);
  cc1200_reg_assert(CC1200_PARTVERSION, CC1200_EXPECTED_PARTVERSION, 10);

  cc1200_on = 1;
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(cc1200_rx_cont_process, ev, data)
{
  PROCESS_BEGIN();

  static uint8_t rssi2compl, rssi_valid = 0;
  static int8_t rssi_converted;
  static uint8_t rssi_offset = 0;

  printf("-------------------------------------------\n");
  printf("CC1200 RF test RX, ");
  printf("Lowest RF channel:\n");
  printf("-------------------------------------------\n");
  printf("RF: Freq0 0x%02x\n",  (uint8_t) CC1200_EXPECTED_FREQ0);
  printf("RF: Freq1 0x%02x\n",  (uint8_t) CC1200_EXPECTED_FREQ1);
  printf("RF: Freq2 0x%02x\n",  (uint8_t) CC1200_EXPECTED_FREQ2);
 
  cc1200_spi_test();
  cc1200_rx_test();

  rssi_offset = read_reg(CC1200_AGC_GAIN_ADJUST);
  printf("RF: RSSI offset found: %d\n", (int8_t)rssi_offset);
  if((int8_t)rssi_offset != CC1200_EXPECTED_RSSI_OFFSET) {
    printf("RF: RSSI offset not as expected, writting...\n");
    write_reg(CC1200_AGC_GAIN_ADJUST, CC1200_EXPECTED_RSSI_OFFSET);
    cc1200_reg_assert(CC1200_AGC_GAIN_ADJUST, CC1200_EXPECTED_RSSI_OFFSET, 10);
  } else {
    printf("RF: RSSI offset match!\n");
  }


  while(1) {
    rssi_valid = read_reg(CC1200_RSSI0);
    if(rssi_valid & 0x01) {
      rssi2compl = read_reg(CC1200_RSSI1);
      rssi_converted = (int8_t)rssi2compl + (int8_t)rssi_offset;
      printf(rssi_converted < 0 ? "RSSI: %04d\n" : "RSSI: +%03d\n", rssi_converted);
    }

    etimer_set(&et, CLOCK_SECOND / 4);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 * @}
 */
