#
# Makefile for SunOS 4.x "modloadable" version
#
# -DSTREAMS for SUN OS
#
# $Id: Makefile,v 1.3 1993/12/15 05:01:37 paulus Exp $
#

IFLAGS = -I..
DFLAGS = -D`arch -k` -DKERNEL -DINET -DPPP_VD -DNPPP=5 -Difr_mtu=ifr_metric
CFLAGS = $(DFLAGS) $(IFLAGS) -O

OBJ = slcompress.o ppp_if.o ppp_async.o ppp_init.o

all: ppp_driver.o

install: all
	install ppp_driver.o $(BINDIR)

ppp_driver.o: $(OBJ)
	ld -r -o ppp_driver.o $(OBJ)

clean:
	rm -f ppp_driver.o $(OBJ) *~ #* core make

slcompress.o: slcompress.c
ppp_if.o: ppp_if.c
ppp_async.o: ppp_async.c
ppp_init.o: ppp_init.c

