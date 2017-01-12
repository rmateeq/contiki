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
#include "sys/ctimer.h"
#include "sys/process.h"
#include "dev/adc.h"
#include "dev/leds.h"
#include "dev/watchdog.h"
#include "dev/sys-ctrl.h"
#include "dev/gpio.h"
#include "dev/ioc.h"
#include "net/rime/rime.h"
#include "dev/cc1200/cc1200-rf-cfg.h"
#include "dev/cc1200/cc1200-const.h"
#include "dev/cc1200/cc1200-arch.h"
#include "lib/list.h"
#include <stdio.h>
#include <stdint.h>
/*---------------------------------------------------------------------------*/
PROCESS(cc1200_tx_cont_mod_process, "cc1200 tx cont mod process");
AUTOSTART_PROCESSES(&cc1200_tx_cont_mod_process);
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
cc1200_txmod_test(void)
{
  if(!cc1200_on) {
    cc1200_arch_init();
  }

  /* Put the CC1200 in idle */
  strobe(CC1200_SIDLE);
  ASSERTWAIT_UNTIL((strobe_test() == CC1200_EXPECTED_IDLE_STATE), 
                   RTIMER_SECOND);

  default_cfg(CC1200_RF_CFG.register_settings,
              CC1200_RF_CFG.size_of_register_settings);

  /* RSSI offset */
  write_reg(CC1200_AGC_GAIN_ADJUST, (int8_t)CC1200_EXPECTED_RSSI_OFFSET);

  /* Flush TX buffer */
  strobe(CC1200_SFTX);
  /* TX buffer config */
  write_reg(CC1200_TXFIRST, 0);
  write_reg(CC1200_TXLAST, 0xFF);
  /* TX power */
  write_reg(CC1200_PA_CFG1, CC1200_EXPECTED_TXPOWER);
  /* Packet config */
  write_reg(CC1200_PKT_CFG2, 0x02);
  /* Frequency config */
  write_reg(CC1200_FREQ0, CC1200_EXPECTED_FREQ0);
  write_reg(CC1200_FREQ1, CC1200_EXPECTED_FREQ1);
  write_reg(CC1200_FREQ2, CC1200_EXPECTED_FREQ2);
  /* Modulated TX with random P9N data */
  write_reg(CC1200_SYNC_CFG1, 0xE8);
  write_reg(CC1200_PREAMBLE_CFG1, 0x00);
  write_reg(CC1200_MDMCFG1, 0x46);
  write_reg(CC1200_PKT_CFG0, 0x40);
  write_reg(CC1200_FS_DIG1, 0x07);
  write_reg(CC1200_FS_DIG0, 0xAA);
  write_reg(CC1200_FS_DVC1, 0xFF);
  write_reg(CC1200_FS_DVC0, 0x17);

  /* Enter in TX mode */
  strobe(CC1200_STX);

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
PROCESS_THREAD(cc1200_tx_cont_mod_process, ev, data)
{
  PROCESS_BEGIN();

  printf("-------------------------------------------\n");
  printf("CC1200 Rf test, ");
  printf("Lowest RF channel:\n");
  printf("-------------------------------------------\n");
  printf("RF: Freq0 0x%02x\n",  (uint8_t) CC1200_EXPECTED_FREQ0);
  printf("RF: Freq1 0x%02x\n",  (uint8_t) CC1200_EXPECTED_FREQ1);
  printf("RF: Freq2 0x%02x\n",  (uint8_t) CC1200_EXPECTED_FREQ2);
 
  cc1200_spi_test();
  cc1200_txmod_test();

  while(1) {
    PROCESS_YIELD();
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 * @}
 */
