#
# File:
#    Makefile
#
# Description:
#    Makefile for the JLAB extensions to the GEFANUC Controller API
#
#
# $Date$
# $Rev$
#

#ifndef GEFANUC_LIB
#	GEFANUC_LIB	= $CODA/extensions/gefanuc/libs
#endif
#ifndef GEFANUC_INC
#	GEFANUC_INC	= $CODA/extensions/gefanuc/include
#endif


CROSS_COMPILE		=
CC			= $(CROSS_COMPILE)gcc
AR                      = ar
RANLIB                  = ranlib
###CFLAGS			= -Wall -O2 -I${GEFANUC_INC} -I. -I/usr/include -L/usr/lib/gef -L.
CFLAGS			= -Wall -O2 -I. -I/usr/include -L/usr/lib/gef -L.

OBJS			= jlabgef.o

LIBS			= libjlabgef.a

all: $(LIBS)

libjlabgef.a: jlabgef.o 
	$(CC) -fpic -shared $(CFLAGS) -o libjlabgef.so  jlabgef.c
	$(AR) ruv libjlabgef.a jlabgef.o
	$(RANLIB) libjlabgef.a

clean distclean:
	@rm -f $(OBJS) $(LIBS) *~ *.so

links: libjlabgef.a
	ln -sf $(PWD)/libjlabgef.a $(GEFANUC_LIB)/libjlabgef.a
	ln -sf $(PWD)/libjlabgef.so $(GEFANUC_LIB)/libjlabgef.so
	ln -sf $(PWD)/jlabgef.h $(GEFANUC_INC)/jlabgef.h

%: %.c
	$(CC) $(CFLAGS) -o $@ $(@:%=%.c) $(LIBS_$@) -lrt -lvme

.PHONY: all clean distclean
