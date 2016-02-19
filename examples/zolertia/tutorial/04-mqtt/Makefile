DEFINES+=PROJECT_CONF_H=\"../02-ipv6/project-conf.h\"

all: mqtt-demo

APPS += mqtt

# Linker size optimization
SMALL = 1

# Override this due to RAM overflow
CFLAGS += -DNBR_TABLE_CONF_MAX_NEIGHBORS=3
CFLAGS += -DUIP_CONF_MAX_ROUTES=3

CONTIKI_WITH_IPV6 = 1

CONTIKI = ../../../..

include $(CONTIKI)/Makefile.include
