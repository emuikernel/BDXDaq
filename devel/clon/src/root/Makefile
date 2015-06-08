#
# Makefile for ROOT CMON presenter for CLON cluster
#

PLATFORM      = solaris

EXTRA_CXXFLAGS = -xO5

CXX           = CC
CXXFLAGS      = -KPIC -mt -I/usr/openwin/include -ptr$(ROOTSYS) \
				$(EXTRA_CXXFLAGS) \
				-D_CMON \
				-I$(ROOTSYS)/include \
				-I$(CODA)/common/include \
				-I$(CLON)/common/include

#CXXFLAGS      = -xO5 -mt -KPIC -D__SunOS_5_6 -library=iostream,no%Cstd \
#				-D_POSIX_PTHREAD_SEMANTICS -D_LANGUAGE_C -D_CMON \
#				-I$(ROOTSYS)/include \
#				-I$(CODA)/common/include \
#				-I$(CLON)/common/include

#OPT           = -O
NOOPT         = 
LD            = CC

#LDFLAGS       = -O
#or next line for CC5
#LDFLAGS       = -g -library=iostream,no%Cstd -o



SOFLAGS       = -G
SOGFLAGS      = -G
LIBS          = $(ROOTLIBS) -lm -lgen -ldl -lsocket -lsunmath
GLIBS         = $(ROOTLIBS) $(ROOTGLIBS)  -L/usr/openwin/lib \
                -L/usr/ccs/lib -lX11 -lm -lgen -ldl -lsocket -lsunmath

##### MACROS and TARGETS #####

include Make-macros


##### DEPENDENCIES #####

include Make-depend

# DO NOT DELETE
