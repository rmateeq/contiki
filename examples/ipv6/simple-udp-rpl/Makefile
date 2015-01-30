all: broadcast-example unicast-sender unicast-receiver
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
APPS = servreg-hack serial-feed
CONTIKI=../../..

CONTIKI_WITH_IPV6 = 1
include $(CONTIKI)/Makefile.include
