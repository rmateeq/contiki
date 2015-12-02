
import sys
import os
import time
import serial
import smtplib
from subprocess import call
from datetime import datetime
from time import sleep
from email.mime.text import MIMEText
from email.mime.image import MIMEImage
from email.mime.multipart import MIMEMultipart

#Gmail constants
EMAIL_DIR        = "YOUR_GMAIL"
EMAIL_PSW        = "YOUR_PASSWORD"

#pySerial port
PORT_USB = "/dev/ZONGLE"

#pyCamera save photo folder 
PICS_PATH    = "/home/pi/pics/"

# Debug App
DEBUG_APP = 0

# Takes a still photo with the built-in Raspberry Camera
def take_photo():
    i = datetime.now()
    now = i.strftime('%Y%m%d-%H%M%S')  
    photo_name = PICS_PATH + now + '.jpg'  
    cmd = 'raspistill -t 500 -w 1024 -h 768 -o ' + photo_name   
    call ([cmd], shell=True)
    return (photo_name, i.strftime('%Y/%m/%d %H:%M:%S'))
    
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

def publish_to_gmail(ID):
    msg = MIMEMultipart()
    msg['Subject'] = 'Hello RE-Mote ID: '+ str(ID)
    msg['From'] = EMAIL_DIR
    msg['To'] = EMAIL_DIR

    photo_path, photo_timestamp = take_photo()
    img_data = open(photo_path, 'rb').read()
    text = MIMEText("")
    msg.attach(text)
    image = MIMEImage(img_data, name = os.path.basename(photo_timestamp))
    msg.attach(image)

    s = smtplib.SMTP('smtp.gmail.com', 587)
    s.ehlo()
    s.starttls()
    s.ehlo()
    s.login(EMAIL_DIR, EMAIL_PSW)
    s.sendmail(EMAIL_DIR, EMAIL_DIR, msg.as_string())
    s.quit()

#connect pySerial 
s = connectMote(PORT_USB)

while True:
    queue = s.inWaiting()
    if queue > 0:
      data = s.read(1000)
      if process_data(data) != -1:
        if DEBUG_APP:
          print ('Sending pic to Twitter')
	      publish_to_gmail(msg_id)
    # Check for data
    sleep(0.2)
