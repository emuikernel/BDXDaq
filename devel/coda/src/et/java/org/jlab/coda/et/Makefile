# Generated automatically from Makefile.in by configure.
#
CC  = gcc
CXX = g++
LD  = ld
AR  = ar
RANLIB = ranlib
#CODA_BIN = ${prefix}/bin
#CODA_LIB = ${prefix}/lib
#CODA_INC = /usr/local/include
#CODA_JAR = /usr/local/lib/java
JAVAHOME=$(JAVA_HOME)

# find location of "java" and back up 1 dir to find java's home dir
ifeq ($(JAVAHOME),)
JAVAHOME = $(shell /bin/csh -c "set j = `which java`; set k = \$$j:h; echo \$$k:h")
endif
JAVA_INC = $(JAVAHOME)/include
JAVA_INC_LOCAL = $(JAVA_INC)/linux
INSTALL  = ../../../../../install-sh -c

AC_FLAGS  = -D_REENTRANT
CFLAGS    = -O2 -fPIC -I../src -I$(JAVA_INC) -I$(JAVA_INC_LOCAL) $(AC_FLAGS)
CXXFLAGS  = $(CFLAGS)
SHLIB_LD  = ld -G
LIBS      = -lieee -lm -lpthread  -lnsl -lresolv -ldl

JAVAC = $(JAVAHOME)/bin/javac
JAVAH = $(JAVAHOME)/bin/javah
JAR   = $(JAVAHOME)/bin/jar
JAVAC_FLAGS = -classpath ../../../../:$(CLASSPATH)

JAVA_OBJS = \
	SystemUse.class \
	SystemCreate.class \
	Constants.class \
	EtException.class \
	EtBusyException.class \
	EtEmptyException.class \
	EtExistsException.class \
	EtReadException.class \
	EtTimeoutException.class \
	EtTooManyException.class \
	EtWakeUpException.class \
	EtWriteException.class \
	AllData.class \
	ProcessData.class \
	StationData.class \
	SystemData.class \
	AttachmentData.class \
	Event.class \
	EventList.class \
	EventSelectable.class \
	Attachment.class \
	AttachmentLocal.class \
	StationConfig.class \
	Station.class \
	StationLocal.class \
	StationSelection.class \
	SystemConfig.class \
	SystemOpenConfig.class \
	SystemOpen.class \
	SystemTcpServer.class \
	SystemUdpServer.class \
	EtMonitor.class

all: $(JAVA_OBJS)

install: install-jar

install-jar: $(JAVA_OBJS)
	@echo "Jarring .class files"
	$(JAR) cf Et.jar *.class
	@if [ ! -d $(CODA_JAR) ]; then \
	  echo "Making directory $(CODA_JAR)"; \
	  mkdir $(CODA_JAR); \
	fi;
	@echo "Installing Et.jar in $(CODA_JAR)"
	@$(INSTALL) Et.jar $(CODA_JAR)/Et.jar

clean: 
	rm -f core *~ *.o *.so *.class Et.jar

# SUFFIX RULES

.SUFFIXES: .java .class

.java.class:
	$(JAVAC) $< $(JAVAC_FLAGS)
.c:
	$(CC) -o $* $(CFLAGS) $< -L../src -let $(LIBS)
.cc:
	$(CXX) -o $* $(CFLAGS) $< -L../src -let $(LIBS)
.c.o:
	$(CC) -c $(CFLAGS) $<
