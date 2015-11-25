DEFINES+=PROJECT_CONF_H=\"project-conf.h\"
CONTIKI_PROJECT = demo-stand

all: $(CONTIKI_PROJECT)

ifdef TX
CFLAGS+=-DTX_NODE=1
endif

CONTIKI = ../../../..
CONTIKI_WITH_RIME = 1
include $(CONTIKI)/Makefile.include
