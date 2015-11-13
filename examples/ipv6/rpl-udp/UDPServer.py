#! /usr/bin/env python

#------------------------------------------------------------#
# UDP example to forward data from a local IPv6 DODAG to an 
# IoT cloud platform (Ubidots)
# Antonio Lignan <alinna@zolertia.com>
#
# Requires Ubidots library: pip install ubidots==1.6.3
#------------------------------------------------------------#

import sys
from socket import*
from socket import error
from time import sleep
from ubidots import ApiClient

PORT        = 5678
BUFSIZE     = 1024
UBIDOTS_KEY = "XXX"
UBIDOTS_VAR = "XXX"

#------------------------------------------------------------#
# UDP server to forward data to Ubidots
#------------------------------------------------------------#
def server():
  try:
    s = socket(AF_INET6, SOCK_DGRAM)
    s.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)

    # Replace address below with "aaaa::1" if tunslip6 has
    # created a tun0 interface with this address
    s.bind(('', PORT))

  except Exception:
    print "ERROR: Server Port Binding Failed"
    return
  print 'UDP server ready: %s'% PORT

  while 1:
    data, addr = s.recvfrom(BUFSIZE)
    print "Received data from " + str(addr[0]) + ":" + str(addr[1])

    # The server expects bytes as data, in this case the same structure
    # my_msg_t as in "simple-udp-rpl" example.  Only keep the "temp" field
    val = (''.join(x.encode('hex') for x in data))[8:12]

    # Network byte ordering, reordering, converting to float
    val = (val[2:] + val[:2]).lstrip()

    try:
      new_val = int(val.lstrip("0"), 16)
    except:
      print "Invalid value received :'( "
      raise

    # Create a Ubidots client and get a pointer to the variable
    try:
      client = ApiClient(UBIDOTS_KEY)
    except Exception, e:
      print "Ubidots error: %s" % e
      return

    try:
      my_variable = client.get_variable(UBIDOTS_VAR)
    except Exception, e:
      print "Ubidots error: %s" % e
      return

    # Update the variable
    print "Updating Ubidots variable " + UBIDOTS_VAR + " with " + str(new_val)
    my_variable.save_value({'value':new_val})

    # Send a response to the origin
    reply = "Ubidots updated by your friendly server " + str(new_val)
    s.sendto(reply, addr)
    
#------------------------------------------------------------#
# MAIN APP
#------------------------------------------------------------#
server()
