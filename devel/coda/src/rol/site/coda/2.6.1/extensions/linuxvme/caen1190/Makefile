#
# Description:  Makefile for caen1190Lib.o
#   This driver is specific to VxWorks BSPs and must be compiled
#   with access to vxWorks headers.
#
# SVN: $Rev: 417 $
#
# Uncomment DEBUG line for debugging info ( -g and -Wall )
#DEBUG=1
#
ARCH=Linux


#Check Operating system we are using
ifndef OSNAME
  OSNAME := $(subst -,_,$(shell uname))
endif

ifndef ARCH
  ARCH = VXWORKSPPC
endif

ifndef WIND_BASE
  WIND_BASE = /site/vxworks/5.5/ppc
endif


ifeq ($(OSNAME),SunOS)
CODALIB = $(CODA)/$(OSNAME)/lib
CODAINC = $(CODA)/common/include
CFLAGS = -O $(DEFS)
endif

ifeq ($(OSNAME),Linux)

ifndef LINUXVME_LIB
	LINUXVME_LIB	= $CODA/extensions/linuxvme/libs
endif
ifndef LINUXVME_INC
	LINUXVME_INC	= $CODA/extensions/linuxvme/include
endif

CODALIB 	= $(CODA)/$(OSNAME)/lib
CODAINC 	= $(CODA)/common/include
CFLAGS 		= -O $(DEFS) -O2 \
		-I${LINUXVME_INC} -I. \
		-L${LINUXVME_LIB} -L. -DJLAB
ifdef DEBUG
CFLAGS		+= -Wall -g
endif
AR = ar
RANLIB = ranlib
endif

ifeq ($(ARCH),VXWORKSPPC)
INCDIR=$(WIND_BASE)/target/h -I../universeDma
CC = ccppc
LD = ldppc
DEFS = -mcpu=604 -DCPU=PPC604 -DVXWORKS -D_GNU_TOOL -DVXWORKSPPC
INCS = -fno-for-scope -fno-builtin -fvolatile -fstrength-reduce -mlongcall -I. -I$(CODAINC) -I$(INCDIR)
endif

ifeq ($(ARCH),VXWORKS68K51)
INCDIR=/site/vxworks/5.3/68k/target/h
CC = cc68k
DEFS = -DCPU=MC68040 -DVXWORKS -DVXWORKS68K51
INCS = -Wall -mc68020 -fvolatile -fstrength-reduce -nostdinc -I. -I$(INCDIR)
endif


ifeq ($(ARCH),Linux)
all: echoarch caen1190Lib.o libcaen1190.a links
else 
all: echoarch caen1190Lib.o
endif

caen1190Lib.o: caen1190Lib.c c1190Lib.h
	$(CC) -c $(CFLAGS) $(INCS) -o $@ caen1190Lib.c

libcaen1190.a: caen1190Lib.o
	$(CC) -fpic -shared $(CFLAGS) -o libcaen1190.so caen1190Lib.c
	$(AR) ruv libcaen1190.a caen1190Lib.o
	$(RANLIB) libcaen1190.a


clean:
	rm -f caen1190Lib*.o libcaen1190.{a,so} *~

links: libcaen1190.a
	ln -sf $(PWD)/libcaen1190.a $(LINUXVME_LIB)/libcaen1190.a
	ln -sf $(PWD)/libcaen1190.so $(LINUXVME_LIB)/libcaen1190.so
	ln -sf $(PWD)/c1190Lib.h $(LINUXVME_INC)/c1190Lib.h

echoarch:
	echo "Make for $(ARCH)"

rol:
	make -f Makefile-rol

rolclean:
	make -f Makefile-rol clean
