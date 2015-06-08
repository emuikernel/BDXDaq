#############################################################################
#	Makefile for the Master Name Server
#############################################################################
OBJS	= portHandler.o rccAcceptor.o rccIO.o rccCmdBuffer.o rccStdinPipe.o

#	-D_CODA_DEBUG -D_REPORT_LOCAL

EXTRA_INCLUDES = -I. -I../Components -I../Constants \
	-I../../Variables -I../../Common 

include ../Makefile.common

all: moveobjs copyheaders

moveobjs: $(OBJS)
	if test ! -d ../.objs; \
	then mkdir ../.objs; else echo; fi
	cp $(OBJS) ../.objs

copyheaders:
	if test ! -d ../include; \
	then mkdir ../include; else echo; fi
	cp *.h ../include

clean:
	rm -rf *.o *.a core *~ ptrepository 
