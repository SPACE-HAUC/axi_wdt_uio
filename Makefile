CC=gcc
CXX=g++
RM= /bin/rm -vf
ARCH=UNDEFINED
PWD=$(shell pwd)
CDR=$(shell pwd)
ECHO=echo

UNAME_S := $(shell uname -s)

EDCFLAGS:=$(CFLAGS) -I ./ -I uio_dev/ -Wall -std=c11 -DUNIT_TEST
EDLDFLAGS:=$(LDFLAGS) -lpthread -lm

all: CFLAGS+= -O2

TARGET=wdt_test.out
COBJS=axi_wdt_uio.o uio_dev/uio_dev.o

all: $(TARGET)

$(TARGET): $(COBJS)
	$(CC) $(EDCFLAGS) -o $@ $(COBJS) $(EDLDFLAGS)
	$(ECHO) "Built for $(UNAME_S), execute sudo ./$(GUITARGET)"

%.o: %.c
	$(CC) $(EDCFLAGS) -o $@ -c $<

.PHONY: clean

clean:
	$(RM) $(TARGET)
	$(RM) $(COBJS)
