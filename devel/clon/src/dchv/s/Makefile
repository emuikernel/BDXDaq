#
# Description:
#   	Makefile for PPC code
#

###include $(CLON)/src/Makefile.include
CC = ccppc
LD = ldppc
DEFINES = -DVXWORKS -DVXWORKSPPC -DCPU=PPC604 -D_GNU_TOOL 
INCS = -I$(WIND_BASE)/target/h -I$(CODA)/common/include/ -I$(CLON_INC) 
CFLAGS = -O2 -fno-builtin -mstrict-align -ansi -mlongcall -fvolatile \
        $(DEFINES) 


INCDIR = -I$(INCS) -I$(CODA)/src/camac

OBJS = v288com_ppc.o cvdr_ppc.o switch_ppc.o

all: $(OBJS)


cvdr_ppc.o: cvdr.c
	$(CC) -c $(CFLAGS) cvdr.c $(INCDIR) -o cvdr_ppc.o

switch_ppc.o: switch.c
	$(CC) -c $(CFLAGS) switch.c $(INCDIR) -o switch_ppc.o

v288com_ppc.o: v288com.c
	$(CC) -c $(CFLAGS) v288com.c $(INCDIR) -o v288com_ppc.o


clean:
	rm -f $(OBJS)


distclean:
	rm -f $(OBJS)


install: exports
exports:
	cp $(OBJS) $(CLON)/VXWORKS_ppc/obj/


