#############################################################################
#	Makefile for the Master Name Server
#############################################################################
OBJS	= daqActions.o daqState.o

#-D_CODA_DEBUG

EXTRA_INCLUDES = -I../../include -I../../Common -I. -D_CFRONT

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
