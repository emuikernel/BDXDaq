#
# from Bryan Moffit
#

CODA = /u/site/coda/2.6

#ifndef GEFANUC_LIB
#	GEFANUC_LIB	= $(CODA)/extensions/gefanuc/libs
#endif
#ifndef GEFANUC_INC
#	GEFANUC_INC	= $(CODA)/extensions/gefanuc/include
#endif

CROSS_COMPILE		=
CC			= $(CROSS_COMPILE)gcc
AR                      = ar
RANLIB                  = ranlib
###CFLAGS			= -O2 -I. -I${GEFANUC_INC} -I/usr/include -L${GEFANUC_LIB} -L/usr/lib/gef -L.
CFLAGS			= -O2 -I. -I../jlabgef -I/usr/include -L/usr/lib/gef -L.
ifdef DEBUG
CFLAGS			+= -Wall -g
endif

OBJS			= tirLib.o

LIBS			= libtir.a

all: $(LIBS)

libtir.a: tirLib.o
	$(CC) -fpic -shared $(CFLAGS) -o libtir.so tirLib.c
	$(AR) ruv libtir.a tirLib.o
	$(RANLIB) libtir.a

clean distclean:
	@rm -f $(OBJS) $(LIBS) *.so *~

links: libtir.a
	ln -sf $(PWD)/libtir.a $(GEFANUC_LIB)/libtir.a
	ln -sf $(PWD)/libtir.so $(GEFANUC_LIB)/libtir.so
	ln -sf $(PWD)/tirLib.h $(GEFANUC_INC)/tirLib.h

%: %.c libtir.a 
	$(CC) $(CFLAGS) -o $@ $(@:%=%.c) $(LIBS_$@) -lrt -lvme -ltir 

.PHONY: all clean distclean
