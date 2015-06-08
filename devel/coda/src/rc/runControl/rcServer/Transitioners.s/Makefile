###############################################################
#        Makefile for subSysTest
###############################################################
OBJS	= transitioner.o subSysTransitioner.o compTransitioner.o \
	booter.o activater.o configurer.o downloader.o \
	ender.o pauser.o prestarter.o resetter.o \
	terminater.o verifier.o transitionTimer.o \
	subSysBooter.o subSysActivater.o subSysAborter.o \
	subSysConfigurer.o subSysDownloader.o subSysEnder.o \
	subSysPauser.o subSysPrestarter.o subSysResetter.o \
	subSysTerminater.o subSysVerifier.o

EXTRA_INCLUDES = -I. -I../../Common -I../../Variables \
	-I../Constants -I../Components -I../Network

#-D_CODA_DEBUG -D_REPORT_LOCAL 

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
	rm -rf *.o $(BIN) core *~
