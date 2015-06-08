#
# Description:
#   	Makefile for Standard Routines Library for TIR
#
#

#Check Operating system we are using
ifndef OSTYPE
  OSTYPE := $(subst -,_,$(shell uname))
endif

ifndef ARCH
  ARCH = VXWORKS_ppc
endif


#ifeq ($(OSTYPE),SunOS)
LIBDIR = $(CODA)/$(ARCH)/obj
CFLAGS = $(DEFS)


ifeq ($(ARCH),VXWORKS_ppc)
CC = ccppc
DEFS = -O2 -mcpu=604 -DCPU=PPC604 -DVXWORKS -D_GNU_TOOL -DVXWORKSPPC
VXINC = $(WIND_BASE)/target/h
INCS = -fno-for-scope -fno-builtin -fvolatile -fvolatile-global \
		-fstrength-reduce -mlongcall -I. -I$(VXINC) \
		-I./inc -I../code.s -I$(CODA)/common/include

endif

#endif



all: vmeIntLib.o vmeUserLib.o

vmeIntLib.o: vme_intlib.c
	$(CC) -c $(CFLAGS) $(INCS) -o $@ vme_intlib.c

vmeUserLib.o: vmeUserLib.c
	$(CC) -c $(CFLAGS) $(INCS) -o $@ vmeUserLib.c

install:
	cp vmeIntLib.o $(LIBDIR)
	cp vmeUserLib.o $(LIBDIR)

clean:
	rm -f *.o

