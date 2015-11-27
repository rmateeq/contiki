
import sys
import os
import time
import tweepy
import smtplib
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
TWITTER_LINK = "http://ow.ly/JAzja"
TWITTER_MSG  = "Init Raspi now! " #+ TWITTER_HASH + " " + TWITTER_LINK

#Email constants

EMAIL_DIR        = "jgraspberry@gmail.com"
EMAIL_PSW        = "raspigarrido89"
EMAIL_TO_SEND    = "jsanchez@zolertia.com"
EMAIL_TO_SEND2   = "alinan@zolertia.com"

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
def publish_to_twitter(tweet):
    photo_path, photo_timestamp = take_photo()
    status = TWITTER_MSG + ' ' + photo_timestamp
    tweet.update_with_media(photo_path, status=status)
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



send_IP()

# OAuth process, using the keys and tokens  
auth = tweepy.OAuthHandler(CONSUMER_KEY, CONSUMER_SECRET)
auth.set_access_token(ACCESS_TOKEN, ACCESS_TOKEN_SECRET)

# Creation of the actual interface, using authentication
try:
    api = tweepy.API(auth)
except Exception, e:
    sys.exit("Tweepy exception %s" % e)

publish_to_twitter(api)
