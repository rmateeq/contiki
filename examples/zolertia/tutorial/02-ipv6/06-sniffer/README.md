# Placeholder

To change the 6LoWPAN context:

core/net/ipv6/sicslowpan.c

Also overridable by:

´´´´
#define SICSLOWPAN_CONF_ADDR_CONTEXT_0 { addr_contexts[0].prefix[0]=0xbb; addr_contexts[0].prefix[1]=0xbb; }
´´´´

Execute:
python sensniff.py --non-interactive -d /dev/ttyUSB0 -p test.pcap

Launch wireshark from command line as below:
sudo wireshark -i /tmp/sensniff
