#############################################################################
#	Makefile for the test rc
#############################################################################
include $(CODA)/src/Makefile.include

BIN   	= rcline

export WRAPPER_ROOT = $(shell cd ../../SACE-3.1; pwd)

OBJS	= codaIntHash.o codaSlist.o \
	codaStrHash.o rcMsg.o \
	rcServerLocater.o rcSvcLocaterRep.o \
	rcSvcInfoFinder.o \
	daqData.o daqDataManager.o \
	daqDataWriter.o daqDataTrigger.o daqNetData.o \
	codaRcCallback.o rccDaqData.o \
	daqArbStructFactory.o daqCompBootStruct.o daqMonitorStruct.o \
	daqArbStruct.o daqRunTypeStruct.o rcClient.o

LDFLAGS = -L../../SACE-3.1/lib/$(OSTYPE_MACHINE)


ifeq ($(OSTYPE),Linux)
CXX = g++
CXXFLAGS = -DLinux -DSYSV -DSVR4 -fwritable-strings -fno-for-scope -D_CODA_2_0_T \
	-I. -I../Variables -I../Common \
	-I$(CODA)/common/inc -I../../SACE-3.1/include

LDLIBS	= $(LDFLAGS) -lSACE \
		-L$(CODA)/$(OSTYPE_MACHINE)/lib $(globallib)/libmysql4.1.20.a -lsocket -lm
endif

ifeq ($(OSTYPE), SunOS)
CXX = CC 
CXXFLAGS = -DSunOS -DSVR4 -DSYSV -xO5 -KPIC -mt -D_CODA_2_0_T -D_TRACE_OBJECTS \
	-I. -I../Variables -I../Common \
	-I$(CODA)/common/inc -I../../SACE-3.1/include

LDLIBS = $(LDFLAGS) -lSACE \
		-L$(CODA)/$(OSTYPE_MACHINE)/lib $(globallib)/libmysql4.1.20.a -lsocket -lnsl
endif



.C.o:
	rm -f $@
	$(CXX) -o $@ -c $(CXXFLAGS) $<

.cc.o:
	rm -f $@
	$(CXX) -o $@ -c $(CXXFLAGS) $<

all: libcodaRc.a

libcodaRc.a: $(OBJS)
	rm -f $@
	ar ruv $@ $(OBJS)
	ranlib $@	

rcline: rcline.o libcodaRc.a
	rm -f $@
	$(CXX) -o $@ rcline.o  libcodaRc.a $(LDLIBS)

clean:
	rm -rf *.o *.a core *~ ptrepository $(BIN)

#############################################################################
codaIntHash.o: ../Common/codaIntHash.cc
	rm -f $@
	$(CXX) $(CXXFLAGS) -c ../Common/codaIntHash.cc -o $@

codaSlist.o: ../Common/codaSlist.cc
	rm -f $@
	$(CXX) $(CXXFLAGS) -c ../Common/codaSlist.cc -o $@

codaStrHash.o: ../Common/codaStrHash.cc
	rm -f $@
	$(CXX) $(CXXFLAGS) -c ../Common/codaStrHash.cc -o $@

rcMsg.o: ../Common/rcMsg.cc
	rm -f $@
	$(CXX) $(CXXFLAGS) -c ../Common/rcMsg.cc -o $@

rcServerLocater.o: ../Common/rcServerLocater.cc
	rm -f $@
	$(CXX) $(CXXFLAGS) -c ../Common/rcServerLocater.cc -o $@

rcSvcLocaterRep.o: ../Common/rcSvcLocaterRep.cc
	rm -f $@
	$(CXX) $(CXXFLAGS) -c ../Common/rcSvcLocaterRep.cc -o $@

rcSvcInfoFinder.o: ../Common/rcSvcInfoFinder.cc
	rm -f $@
	$(CXX) $(CXXFLAGS) -c ../Common/rcSvcInfoFinder.cc -o $@

daqData.o: ../Variables/daqData.cc
	rm -f $@
	$(CXX) $(CXXFLAGS) -c ../Variables/daqData.cc -o $@	

daqDataManager.o: ../Variables/daqDataManager.cc
	rm -f $@
	$(CXX) $(CXXFLAGS) -c ../Variables/daqDataManager.cc -o $@

daqDataWriter.o: ../Variables/daqDataWriter.cc
	rm -f $@
	$(CXX) $(CXXFLAGS) -c ../Variables/daqDataWriter.cc -o $@

daqDataTrigger.o: ../Variables/daqDataTrigger.cc
	rm -f $@
	$(CXX) $(CXXFLAGS) -c ../Variables/daqDataTrigger.cc -o $@

daqCompBootStruct.o: ../Variables/daqCompBootStruct.cc
	rm -f $@
	$(CXX) $(CXXFLAGS) -c ../Variables/daqCompBootStruct.cc -o $@

daqArbStructFactory.o: ../Variables/daqArbStructFactory.cc
	rm -f $@
	$(CXX) $(CXXFLAGS) -c ../Variables/daqArbStructFactory.cc -o $@

daqArbStruct.o: ../Variables/daqArbStruct.cc
	rm -f $@
	$(CXX) $(CXXFLAGS) -c ../Variables/daqArbStruct.cc -o $@

daqRunTypeStruct.o: ../Variables/daqRunTypeStruct.cc
	rm -f $@
	$(CXX) $(CXXFLAGS) -c ../Variables/daqRunTypeStruct.cc -o $@

daqMonitorStruct.o: ../Variables/daqMonitorStruct.cc
	rm -f $@
	$(CXX) $(CXXFLAGS) -c ../Variables/daqMonitorStruct.cc -o $@

daqNetData.o: ../Variables/daqNetData.cc
	rm -f $@
	$(CXX) $(CXXFLAGS) -c ../Variables/daqNetData.cc -o $@	
