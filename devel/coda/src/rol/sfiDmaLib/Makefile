#
# Description:
#   	Makefile for FASTBUS Standard Routines Library for SFI
#
#

#Check Operating system we are using
ifndef OSTYPE
  OSTYPE := $(subst -,_,$(shell uname))
endif

ifndef ARCH
  ARCH = VXWORKS_ppc
endif


ifeq ($(OSTYPE),SunOS)
LIBDIR = $(CODA)/$(ARCH)/obj
CFLAGS = $(DEFS)


ifeq ($(ARCH),VXWORKS_ppc)
CC = ccppc
DEFS = -O2 -mcpu=604 -DCPU=PPC604 -DVXWORKS -D_GNU_TOOL -DVXWORKSPPC
VXINC = $(WIND_BASE)/target/h
INCS = -fno-for-scope -fno-builtin -fvolatile -fvolatile-global \
			-fstrength-reduce -mlongcall -I. -I$(VXINC) \
			-I../inc/ -I../sfi.s/ \
			-I$(CODA)/common/include
endif

endif



all: sfiIntLib.o dmaPLib.o sfiUserLib.o

sfiIntLib.o: sfi_intlib.c ../sfi.s/sfi.h ../inc/sfi_fb_macros.h
	$(CC) -c $(CFLAGS) $(INCS) -o $@ sfi_intlib.c

sfiUserLib.o: sfiUserLib.c sfiLib.h dmaPList.h
	$(CC) -c $(CFLAGS) $(INCS) -o $@ sfiUserLib.c

dmaPLib.o: dmaPList.c dmaPList.h
	$(CC) -c $(CLFAGS) $(INCS) -o $@ dmaPList.c

install:
	cp sfiIntLib.o $(LIBDIR)
	cp dmaPLib.o $(LIBDIR)
	cp sfiUserLib.o $(LIBDIR)

clean:
	rm -f *.o

