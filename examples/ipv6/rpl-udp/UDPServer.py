#! /usr/bin/env python
import sys
from socket import*
from socket import error
from time import sleep
from ubidots import ApiClient

PORT = 5678
BUFSIZE = 1024

#------------------------------------------------------------#
# Start a client or server application for testing
#------------------------------------------------------------#
def main():
  server()
#------------------------------------------------------------#
# Creates a server, echoes the message back to the client
#------------------------------------------------------------#
def server():
  try:
    s = socket(AF_INET6, SOCK_DGRAM)
    s.bind(('aaaa::1', PORT))
  except Exception:
    print "ERROR: Server Port Binding Failed"
    return
  print 'UDP server ready: %s'% PORT

  while 1:
    data, addr = s.recvfrom(BUFSIZE)
    raw = str(data)
    if "\r\n" in raw:
      raw = raw[(raw.index("\r\n") + 2):]
      if "\r\n" in raw:
        raw = raw[:raw.index("\r\n")]
        # We should have a full frame, parse based on tab and create a list
        ubidots_raw = raw.split("\t")

        # Sanity check: only use fixed length for api key and var key
        if len(ubidots_raw[0]) > 40:
          ubidots_raw[0] = ubidots_raw[0][:40]

        if len(ubidots_raw[1]) > 24:
          ubidots_raw[1] = ubidots_raw[1][:24]

        print "Recv: {0}:{1}".format(ubidots_raw[1], ubidots_raw[2])

        # Create a Ubidots client and get a pointer to the variable
        try:
          client = ApiClient(ubidots_raw[0])
        except Exception, e:
          print "Ubidots error: %s" % e
          print raw
          return

        try:
          my_variable = client.get_variable(ubidots_raw[1])
        except Exception, e:
          print "Ubidots error: %s" % e
          return

        # Update the variable
        my_variable.save_value({'value':int(ubidots_raw[2])})
    
#------------------------------------------------------------#
# MAIN APP
#------------------------------------------------------------#
main()
