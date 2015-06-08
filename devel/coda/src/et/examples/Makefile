# Generated automatically from Makefile.in by configure.
#
CC  = $(PURE) gcc
CXX = g++
LD  = ld
AR  = ar
RANLIB = ranlib
#CODA_BIN = ${prefix}/bin
#CODA_LIB = ${prefix}/lib
#CODA_INC = /usr/local/include
INSTALL  = ../install-sh -c

AC_FLAGS = -D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS -DMUTEX_NOSHARE
CFLAGS   = -O2 -fPIC -I../src $(AC_FLAGS)
CXXFLAGS = $(CFLAGS)
SHLIB_LD = ld -G
# Use the following flag when using cmlog's messages with dalogmsg
#   AC_FLAGS = ... -DWITH_DALOGMSG

LIBNAM      = libet
LIB_STATIC  = $(LIBNAM).a
LIB_DYNAMIC = $(LIBNAM).so
LIB_USER    = $(LIBNAM)_user.so

LIBS = -lieee -lm -lpthread  -lnsl -lresolv -ldl -ldl
# Use the following terms when using cmlog's messages with dalogmsg
#   LIBS = ... -L$(CMLOGROOT)/lib/solaris -lcmlog

PROGS = et_start \
	et_client \
	et_client_simple \
	et_producer1 \
	et_producer2 \
	et_netclient \
	et_2_et

all: $(PROGS) $(LIB_USER)

install: install-bin install-lib

install-bin: $(PROGS)
	@echo "Installing executables in $(CODA_BIN)"
	@for i in $(PROGS); do \
	   echo "  installing $$i"; \
	   $(INSTALL) $$i $(CODA_BIN)/$$i; \
           done;

install-lib: $(LIB_USER)
	@echo "Installing $(LIB_USER) library in $(CODA_LIB)"
	@$(INSTALL) $(LIB_USER) $(CODA_LIB)/$(LIB_USER)

$(PROGS) : % : %.c  ../src/et.h
	$(CC) -o $@ $(CFLAGS) $< -L../src -let $(LIBS)

# example of user's event selection function in shared lib
$(LIB_USER): et_userfunction.o
	$(SHLIB_LD) -o $(LIB_USER) et_userfunction.o

clean: 
	rm -f core *~ *.o *.so *.a $(PROGS)

.c:
	$(CC) -o $* $(CFLAGS) $< -L../src -let $(LIBS)
.cc:
	$(CXX) -o $* $(CFLAGS) $< -L../src -let $(LIBS)
.c.o:
	$(CC) -c $(CFLAGS) $<
