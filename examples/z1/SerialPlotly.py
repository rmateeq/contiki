# -*- coding: utf-8 -*-

# ----------------------------------------------------------------------------#
# Simple application to relay data to elsewhere from a Contiki serial-based conn
# - Plotly: pip install plotly
# - Sources: https://plot.ly/python/streaming-line-tutorial/
# ----------------------------------------------------------------------------#

import sys
import getopt
import os
import json
import serial
import time
import datetime
import json
from time import sleep

import plotly.plotly as py  
import plotly.tools as tls   
from plotly.graph_objs import *

PORT     = "/dev/ttyUSB0"

# Plotly constants
PLOTLY_USER    = "alinanco"
PLOTLY_API_KEY = "ewobici0wh"
STREAM_KEY_A   = "5lcns6p2ft"
STREAM_KEY_B   = "x8j8hgh5ky"
STREAM_KEY_C   = "2jiexyvlpl"
STREAM_KEY_D   = "2jiexyvlpl"

# ----------------------------------------------------------------------------#
# Create a serial object and connect to mote over USB
# ----------------------------------------------------------------------------#
def connectMote(port):
  try:
    ser = serial.Serial(port, 115200,timeout=0, parity=serial.PARITY_NONE, 
                        stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS)
  except:
    sys.exit("Error connecting to the USB device, aborting")
  return ser

# ----------------------------------------------------------------------------#
# Parse the serial data and publish to Plotly, this assumes the following:
# \r\n API_KEY \t VAR_KEY \t VALUE \r\n
# ----------------------------------------------------------------------------#
def process_data(raw):
  # Search for start and end of frame and slice, discard incomplete
  if "\r\n" in raw:
    raw = raw[(raw.index("\r\n") + 2):]
    if "\r\n" in raw:
      raw = raw[:raw.index("\r\n")]
      # We should have a full frame, parse based on tab and create a list
      parsed_frame = raw.split("\t")
      if len(parsed_frame) == 3:
        client.write({'x': datetime.datetime.now(), 'y': int(parsed_frame[2])})

# ----------------------------------------------------------------------------#
# MAIN APP
# ----------------------------------------------------------------------------#
if __name__=='__main__':

  # Create the serial object and connect to the mode
  # Do not check the serial port object as the function already does it
  s = connectMote(PORT)

  py.sign_in(PLOTLY_USER, PLOTLY_API_KEY)
  tls.set_credentials_file(stream_ids=[
                                        STREAM_KEY_A,
                                        STREAM_KEY_B,
                                        STREAM_KEY_C,
                                        STREAM_KEY_D
                                      ])
  # Initialize the graphic
  trace1 = Scatter(x=[], y=[], stream=dict(token=STREAM_KEY_A, maxpoints=200))

  # Set up and open the streaming channel
  layout = Layout(title='Antonio stress level')
  fig = Figure(data=[trace1], layout=layout)
  print "Plotly URL: " + py.plot(fig, filename='Zolertia Z1 Sensor Data')
  client = py.Stream(STREAM_KEY_A)
  client.open()

  while True:
    queue = s.inWaiting()
    if queue > 0:
      data = s.read(1000)
      process_data(data)
    sleep(0.2)
