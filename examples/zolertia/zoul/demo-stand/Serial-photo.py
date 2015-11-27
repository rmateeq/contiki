import sys
import os
import time
import tweepy
import serial
from subprocess import call
from datetime import datetime
from time import sleep


CONSUMER_KEY        = 'DJtf4O9vlpVQQTwzv1dCRl2H5'
CONSUMER_SECRET     = 'OOeqjgbNOcXoFX0MtoC76kNqf9gEvEWW3c6cU8qi4J0VK7G15D'
ACCESS_TOKEN        = '221817527-scQTX5yoLd4KaN160IsV0JvJVdtNyHG3bCUvVwug'
ACCESS_TOKEN_SECRET = 'qIfVk47aYwW2VxbfKiZ0ehsiFS5H8w02mu1UniJsPLngG'

# Other application's constants
TWITTER_HASH = "#IoTmeetupBCN"
PICS_PATH    = "/home/pi/pics/"
TWITTER_LINK = "http://bit.ly/1ThmLKd"
TWITTER_MSG  = "Hello"  #+ TWITTER_HASH + " " + TWITTER_LINK
TWITTER_NODE_ID = "RE-Mote node "
#pySerial port
PORT_USB = "/dev/ZOUL010"

# Debug App
DEBUG_APP = 1

#correct message from node
CORRECT_MSG = 4242

# Takes a still photo with the built-in Raspberry Camera
def take_photo():
    i = datetime.now()
    now = i.strftime('%Y%m%d-%H%M%S')
    photo_name = PICS_PATH + now + '.jpg'
    cmd = 'raspistill -t 500 -w 1024 -h 768 -o ' + photo_name
    call ([cmd], shell=True)
    return (photo_name, i.strftime('%Y/%m/%d %H:%M:%S'))

# Takes a photo and publishes to twitter with a given message and hashtag
def publish_to_twitter(tweet, id):
    photo_path, photo_timestamp = take_photo()
    status = TWITTER_MSG + ' ' + TWITTER_NODE_ID + id + ' ' + photo_timestamp
    tweet.update_with_media(photo_path, status=status)

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
# This function searches for the head frame delimiter through all the buffer
# ----------------------------------------------------------------------------#

def searchFrameStart(my_string):
  for i, char in enumerate(my_string):
    if char == "I" and my_string[i+1] == "D":
      return i
  return -1

# ----------------------------------------------------------------------------#
# Process packet from mote
# Data format: ID:0x420F,RSSI:-043,DATA:0049\n = leng 30 digits
# ----------------------------------------------------------------------------#

def process_data(raw):
        global msg_id
        if DEBUG_APP:
                print raw

        a = searchFrameStart(raw)

        if a == -1:
                if DEBUG_APP:
                        print "No valid frame found, discarding"
                return -1

        if len(raw) != 31:
                if DEBUG_APP:
                        print "Incomplete frame, discarding"
                return -1
        if "ID" in raw:
        # Sanity check: verify this is not a system print or a debug statement
        # Data format: ID:0x420F,RSSI:-043,DATA:0049\n = leng 30 digits
                msg_id = raw[(a+3):(a+9)]
                print msg_id
                msg_rssi = raw[(a+15):(a+19)]
                print msg_rssi
                msg_data = raw[(a+25):(a+30)]
                print msg_data
                return 0


# OAuth process, using the keys and tokens  
auth = tweepy.OAuthHandler(CONSUMER_KEY, CONSUMER_SECRET)
auth.set_access_token(ACCESS_TOKEN, ACCESS_TOKEN_SECRET)

# Creation of the actual interface, using authentication
try:
    api = tweepy.API(auth)
except Exception, e:
    sys.exit("Tweepy exception %s" % e)

#connect pySerial 
s = connectMote(PORT_USB)

while True:
    queue = s.inWaiting()
    if queue > 0:
      data = s.read(1000)
      if process_data(data) != -1:
        if DEBUG_APP:
          print ('Sending pic to Twitter')
        publish_to_twitter(api, msg_id)
    # Check for data
    sleep(0.2)



