DEFINES+=PROJECT_CONF_H=\"project-conf.h\"
CONTIKI_PROJECT = tx-cont-mod-low tx-cont-no-mod-low tx-cont-mod-switching-low rx-cont-low \
                  tx-cont-mod-high tx-cont-no-mod-high tx-cont-mod-switching-high rx-cont-high

TARGET ?= zoul
BOARD  ?= firefly-reva

all: $(CONTIKI_PROJECT)

CONTIKI = ../../../../..
include $(CONTIKI)/Makefile.include
