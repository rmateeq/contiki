
import sys
import os
import time
import smtplib
from subprocess import call
from datetime import datetime
from time import sleep
from email.mime.text import MIMEText
from email.mime.image import MIMEImage
from email.mime.multipart import MIMEMultipart


PICS_PATH    = "/home/pi/pics/"

#Email constants

EMAIL_DIR        = "YOUR_GMAIL"
EMAIL_PSW        = "YOUR_PASSWORD"
EMAIL_TO_SEND    = "GMAIL_TO_SEND"
EMAIL_TO_SEND2   = "GMAIL_TO_SEND"

# Takes a still photo with the built-in Raspberry Camera
def take_photo():
    i = datetime.now()
    now = i.strftime('%Y%m%d-%H%M%S')
    photo_name = PICS_PATH + now + '.jpg'
    photo_name_jpg = now + '.jpg'
    cmd = 'raspistill -t 500 -w 1024 -h 768 -o ' + photo_name
    call ([cmd], shell=True)
    return (photo_name,photo_name_jpg)

#send email with IP address

def send_IP():

        f = os.popen ('ifconfig wlan0 | grep "inet\ addr" | cut -d: -f2 | cut -d " " -f1')
        ip = f.read()
        print ip
        server = smtplib.SMTP('smtp.gmail.com', 587)
        server.starttls()
        server.login( EMAIL_DIR, EMAIL_PSW)

        msg = str(ip)
        server.sendmail(EMAIL_DIR, EMAIL_TO_SEND, msg)
        server.sendmail(EMAIL_DIR, EMAIL_TO_SEND2, msg)
        server.quit()


def send_photo():
    msg = MIMEMultipart()
    msg['Subject'] = 'Init successful'
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


send_IP()
send_photo()
