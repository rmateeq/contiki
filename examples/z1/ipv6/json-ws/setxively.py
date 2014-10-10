#!/usr/bin/python

# python set time code
import httplib,sys

# edit the key and feed parameters to match your Xively account and feed
# https://xively.com/dev/docs/api/communicating/

key  = "uD91Ot0O95Kn6GHfK4Gl1ICguHE7TnptAYrSSou80bmLBiv4"
feed = "1868998277"
addr = "[2001:470:1f10:333::2]"

host   = "[2001:470:1f0b:6bc:c30c::3]"
stream = "Test"

print "JSON-WS XIVELY configuration utility\n   Currently set to feed: %s Key: '%s'" % (feed, key)


print "Setting Xively config at:", host, " feed:", feed, " stream:",stream

conn = httplib.HTTPConnection(host)
# NAT64 address =
#conn.request("POST","", '{"host":"[2001:778:0:ffff:64:0:d834:e97a]","port":80,"path":"/v2/feeds/55180/datastreams/1","interval":120}')

requestData = '{"host":"%s","port":80,"path":"/v2/feeds/%s/datastreams/%s","appdata":"%s","interval":120,"proto":"xively"}' % (addr, feed, stream, key)
print "Posting to node: ", requestData
conn.request("POST","", requestData)

res = conn.getresponse()
print res.status, res.reason
