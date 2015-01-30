/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
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
 *         App to relay data over the serial port
 * \author
 *         Antonio Lignan <alinan@zolertia.com>
 */

#define SERIALFEED_MSG_KEYLEN 24
#define SERIALFEED_MSG_16B_NUM 4
#define SERIALFEED_MSG_LEN (SERIALFEED_MSG_KEYLEN + (SERIALFEED_MSG_16B_NUM*2) + 2)

// A sanity check
#ifndef SERIALFEED_MSG_16B_NUM
#error "At least one variable should be sent!"
#endif

struct serialfeed_msg_t {
  uint8_t id;
  uint8_t len;
  uint16_t value[SERIALFEED_MSG_16B_NUM];
#ifdef SERIALFEED_MSG_KEYLEN
  char var_key[SERIALFEED_MSG_KEYLEN];
#endif
};

/*---------------------------------------------------------------------------*/
void send_to_serial(const char *api, uint8_t *id, uint16_t *val);
/*---------------------------------------------------------------------------*/
