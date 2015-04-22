/*
 * Copyright (c) 2012, Thingsquare, http://www.thingsquare.com/.
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
 *
 */

#include "contiki.h"
#include "contiki-net.h"
#include "dev/leds.h"
#include "reg.h"
#include "spi-arch.h"
#include "dev/ioc.h"
#include "dev/sys-ctrl.h"
#include "dev/spi.h"
#include "dev/ssi.h"
#include "dev/gpio.h"

#include <stdio.h>

#define SPI_CLK_PORT_BASE         GPIO_PORT_TO_BASE(SPI_CLK_PORT)
#define SPI_CLK_PIN_MASK          GPIO_PIN_MASK(SPI_CLK_PIN)
#define SPI_MOSI_PORT_BASE        GPIO_PORT_TO_BASE(SPI_MOSI_PORT)
#define SPI_MOSI_PIN_MASK         GPIO_PIN_MASK(SPI_MOSI_PIN)
#define SPI_MISO_PORT_BASE        GPIO_PORT_TO_BASE(SPI_MISO_PORT)
#define SPI_MISO_PIN_MASK         GPIO_PIN_MASK(SPI_MISO_PIN)

#define CC1120_SPI_CSN_PORT_BASE  GPIO_PORT_TO_BASE(CC1120_SPI_CSN_PORT)
#define CC1120_SPI_CSN_PIN_MASK   GPIO_PIN_MASK(CC1120_SPI_CSN_PIN)
#define CC1120_GDO0_PORT_BASE     GPIO_PORT_TO_BASE(CC1120_GDO0_PORT)
#define CC1120_GDO0_PIN_MASK      GPIO_PIN_MASK(CC1120_GDO0_PIN)
#define CC1120_GDO2_PORT_BASE     GPIO_PORT_TO_BASE(CC1120_GDO2_PORT)
#define CC1120_GDO2_PIN_MASK      GPIO_PIN_MASK(CC1120_GDO2_PIN)
#define CC1120_RESET_PORT_BASE    GPIO_PORT_TO_BASE(CC1120_RESET_PORT)
#define CC1120_RESET_PIN_MASK     GPIO_PIN_MASK(CC1120_RESET_PIN)

#define BUSYWAIT_UNTIL(cond, max_time)                                  \
  do {                                                                  \
    rtimer_clock_t t0;                                                  \
    t0 = RTIMER_NOW();                                                  \
    while(!(cond) && RTIMER_CLOCK_LT(RTIMER_NOW(), t0 + (max_time)));   \
  } while(0)

extern int cc1120_rx_interrupt(int src);
/*---------------------------------------------------------------------------*/
void
cc1120_arch_spi_enable(void)
{
  /* Flush SSI first and wait for CS to go low */

  /* Set CSn to low (0) */
  GPIO_CLR_PIN(CC1120_SPI_CSN_PORT_BASE, CC1120_SPI_CSN_PIN_MASK);

  /* The MISO pin should go high before chip is fully enabled. */
  BUSYWAIT_UNTIL(
    GPIO_READ_PIN(SPI_MISO_PORT_BASE, SPI_MISO_PIN_MASK) == 0,
    RTIMER_SECOND/1000);
}
/*---------------------------------------------------------------------------*/
void
cc1120_arch_spi_disable(void)
{
  /* Wait for CS to go low */

  /* Set CSn to high (1) */
  GPIO_SET_PIN(CC1120_SPI_CSN_PORT_BASE, CC1120_SPI_CSN_PIN_MASK);
}
/*---------------------------------------------------------------------------*/
int
cc1120_arch_spi_rw_byte(unsigned char c)
{
  SPI_WAITFORTx_BEFORE();
  SPI_TXBUF = c;
  SPI_WAITFOREOTx();
  SPI_WAITFOREORx();
  c = SPI_RXBUF;

  return c;
}
/*---------------------------------------------------------------------------*/
int
cc1120_arch_spi_rw(unsigned char *inbuf, unsigned char *outbuf, int len)
{
  int i;
  uint8_t c;

  if((inbuf == NULL && outbuf == NULL) || len <= 0) {
    return 1;

  } else if(inbuf == NULL) {
    for(i = 0; i < len; i++) {
      SPI_WAITFORTx_BEFORE();
      SPI_TXBUF = outbuf[i];
      SPI_WAITFOREOTx();
      SPI_WAITFOREORx();
      c = SPI_RXBUF;
    }

  } else if(outbuf == NULL) {
    for(i = 0; i < len; i++) {
      SPI_WAITFORTx_BEFORE();
      SPI_TXBUF = 0;
      SPI_WAITFOREOTx();
      SPI_WAITFOREORx();
      inbuf[i] = SPI_RXBUF;
    }

  } else {
    for(i = 0; i < len; i++) {
      SPI_WAITFORTx_BEFORE();
      SPI_TXBUF = outbuf[i];
      SPI_WAITFOREOTx();
      SPI_WAITFOREORx();
      inbuf[i] = SPI_RXBUF;
    }
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
static void
cc1120_arch_irq_callback(uint8_t port, uint8_t pin)
{
  if(pin == CC1120_GDO0_PIN) {
    cc1120_rx_interrupt(1);
  }
}
/*---------------------------------------------------------------------------*/
void
cc1120_arch_init(void)
{
  /* First leave RESET high */
  ioc_set_over(CC1120_RESET_PORT, CC1120_RESET_PIN, IOC_OVERRIDE_OE);
  GPIO_SET_OUTPUT(CC1120_RESET_PORT_BASE, CC1120_RESET_PIN_MASK);

  /* Initialize SPI */
  spi_init();

  /* Initialize CSn */
  spi_cs_init(CC1120_SPI_CSN_PORT, CC1120_SPI_CSN_PIN);

  /* Unselect radio. */
  cc1120_arch_spi_disable();

  /* Configure GDOs */
  GPIO_SOFTWARE_CONTROL(CC1120_GDO0_PORT_BASE, CC1120_GDO0_PIN_MASK);
  GPIO_SET_INPUT(CC1120_GDO0_PORT_BASE, CC1120_GDO0_PIN_MASK);
  GPIO_SOFTWARE_CONTROL(CC1120_GDO2_PORT_BASE, CC1120_GDO2_PIN_MASK);
  GPIO_SET_INPUT(CC1120_GDO2_PORT_BASE, CC1120_GDO2_PIN_MASK);

  /* Reset procedure */
  GPIO_SET_PIN(SPI_CLK_PORT_BASE, SPI_CLK_PIN_MASK);
  GPIO_SET_PIN(SPI_MOSI_PORT_BASE, SPI_MOSI_PIN_MASK);
  GPIO_CLR_PIN(CC1120_SPI_CSN_PORT_BASE, CC1120_SPI_CSN_PIN_MASK);
  GPIO_SET_PIN(CC1120_SPI_CSN_PORT_BASE, CC1120_SPI_CSN_PIN_MASK);
  clock_delay_usec(400);

  cc1120_arch_interrupt_enable();  

  GPIO_CLR_PIN(CC1120_SPI_CSN_PORT_BASE, CC1120_SPI_CSN_PIN_MASK);

  while(GPIO_READ_PIN(SPI_MISO_PORT_BASE, SPI_MISO_PIN_MASK));

  GPIO_SET_PIN(CC1120_SPI_CSN_PORT_BASE, CC1120_SPI_CSN_PIN_MASK);
}
/*---------------------------------------------------------------------------*/
void
cc1120_arch_interrupt_enable(void)
{
  /* Rising edge interrupt */
  GPIO_SOFTWARE_CONTROL(CC1120_GDO0_PORT_BASE, CC1120_GDO0_PIN_MASK);
  GPIO_SET_INPUT(CC1120_GDO0_PORT_BASE, CC1120_GDO0_PIN_MASK);
  GPIO_DETECT_EDGE(CC1120_GDO0_PORT_BASE, CC1120_GDO0_PIN_MASK);
  GPIO_TRIGGER_SINGLE_EDGE(CC1120_GDO0_PORT_BASE, CC1120_GDO0_PIN_MASK);
  GPIO_DETECT_RISING(CC1120_GDO0_PORT_BASE, CC1120_GDO0_PIN_MASK);
  GPIO_ENABLE_INTERRUPT(CC1120_GDO0_PORT_BASE, CC1120_GDO0_PIN_MASK);

  ioc_set_over(CC1120_GDO0_PORT, CC1120_GDO0_PIN, IOC_OVERRIDE_PUE);
  nvic_interrupt_enable(CC1120_GPIO0_VECTOR);
  gpio_register_callback(cc1120_arch_irq_callback, CC1120_GDO0_PORT, 
                         CC1120_GDO0_PIN);
}
/*---------------------------------------------------------------------------*/

