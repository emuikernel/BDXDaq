h19967
s 00015/00001/00203
d D 1.12 05/02/25 15:02:03 boiarino 13 12
c *** empty log message ***
e
s 00003/00002/00201
d D 1.11 03/06/20 18:02:19 boiarino 12 11
c *** empty log message ***
e
s 00004/00002/00199
d D 1.10 03/04/17 16:44:05 boiarino 11 10
c *** empty log message ***
e
s 00009/00000/00192
d D 1.9 02/04/01 11:03:35 wolin 10 9
c Added make headers
e
s 00000/00006/00192
d D 1.8 01/11/19 23:14:44 boiarino 9 8
c minor
c 
e
s 00002/00002/00196
d D 1.7 01/10/30 14:19:11 boiarino 8 7
c -O -> -fast
e
s 00004/00004/00194
d D 1.6 01/05/15 13:51:06 boiarino 7 6
c comment out bostest
c 
e
s 00007/00002/00191
d D 1.5 00/11/28 17:07:04 wolin 6 5
c For release
e
s 00008/00005/00185
d D 1.4 00/08/30 16:50:19 boiarino 5 4
c *** empty log message ***
e
s 00007/00000/00183
d D 1.3 00/08/30 14:34:11 boiarino 4 3
c *** empty log message ***
e
s 00009/00001/00174
d D 1.2 00/08/29 15:45:18 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/Makefile
e
s 00175/00000/00000
d D 1.1 00/08/10 11:09:04 boiarino 1 0
c date and time created 00/08/10 11:09:04 by boiarino
e
u
U
f e 0
t
T
I 1
#---------------------------------------------------------------------------
# bosio Makefile
#
#=================== Begin of MODIFICATION Section =========================
#
I 5

SHELL = /bin/sh

E 5
MAIN    = bosio
TOP     = .
MYBIN   = $(CLON_BIN)
MYLIB   = $(CLON_LIB)
I 4
MYINC   = $(CLON_INC)
E 4
INC     = ./bosio.inc
OBJ      = ./$(shell uname)/obj
locallib = ./$(shell uname)/lib
localbin = ./$(shell uname)/run

I 13
machine=`uname -m | tr "/" "_"`
os=`uname -s | tr "/" "_"`
rev=`uname -r | tr "/" "_"`

#if echo "$machine" | grep "^sun4" > /dev/null
#then
#	machine="Sparc"
#fi

E 13
# Machine dependent compiler flags and CLAS definitions

ifeq ("$(shell uname)","HP-UX")
  FC = fort77
#  f77 -c +Z +ppu +O2 +Onolimit jetset74.f
#  f77 -c +Z +ppu +O2 pythia5707.f
#  ld -b -z -o libPythia.sl *.o
  FFLAGS      = +ppu +O2 
  CC = cc
  CFLAGS = -Aa -Ae -DHPUX
  CLAS_FFLAGS = -I$(INC)
  SHLD = CC -z
  SHLIBS = -lm
endif

I 11
# -> use ' -fast' for 32bit or '-xarch=v9' for 64bit (NOT TOGETHER !!!)
E 11
D 13
ifeq ("$(shell uname)","SunOS")
E 13
I 13
ifeq ("$(shell uname -s)","SunOS")
E 13
  FC = f77
D 8
  FFLAGS = -O -KPIC -mt
E 8
I 8
  FFLAGS = -fast -KPIC -mt
E 8
  CC = cc
D 8
  CFLAGS = -O -KPIC -mt -DSunOS
E 8
I 8
D 11
  CFLAGS = -fast -KPIC -mt -DSunOS
E 11
I 11
D 12
#  CFLAGS = -fast -KPIC -mt -DSunOS
  CFLAGS = -g -KPIC -mt -DSunOS
E 12
I 12
  CFLAGS = -fast -KPIC -mt -DSunOS
I 13
# for opteron-based Solaris
  ifeq ("$(shell uname -m)","i86pc")
    CFLAGS += -DSunOS_i86pc
  endif

E 13
#  CFLAGS = -g -KPIC -mt -DSunOS
E 12
E 11
E 8
  CLAS_FFLAGS = -I$(INC)
D 11
  SHLD = CC -G -O
E 11
I 11
  SHLD = CC -fast -G -O
E 11
endif

ifeq ("$(shell uname)","AIX")
  FC = xlf
  FFLAGS      = -O -qextname -WF,-I$(INC)
  CC = xlc
  CFLAGS = -g -DAIX -I$(INC)
  CLAS_FFLAGS = 
#  SHLD = xlC -bnoquiet
  SHLD = xlC -g
  SHLIBS = -lm -lPW -lld -lcurses
endif

ifeq ("$(shell uname)","Linux")
  FC = g77
  FFLAGS = -O2 -m486 -fno-automatic -finit-local-zero \
	-ffixed-line-length-none -fno-second-underscore -DLinux
  CC = gcc
  CFLAGS = -O2 -fwritable-strings -DLinux
  CLAS_FFLAGS = -I$(INC)
  SHLD = g++ -Wl,-soname,libBosio.so -shared -g
endif

#------------------------------
# Set up CERN & MATH libraries
#------------------------------

MATHLIB = /lib
CERNLIB = $(CERN_ROOT)/lib

#----------------------------------------------------------------
# List of subdirectories containing FORTRAN and/or C source code
#----------------------------------------------------------------

SOURCE_FILES = $(wildcard *.s/*.[cF])
SUB_DIRECTORIES = $(sort $(dir $(SOURCE_FILES)))

#--------------------------------------------------------------------------
# Prepare include file search path. All include files should be referenced
# with respect to.
#--------------------------------------------------------------------------

#--------------------
# BOSIO object modules
#--------------------

REQUIRED_OBJS  = $(locallib)/bosio_main.o
REQUIRED_OBJS +=

#-------------------------------------
# BOSIO, CLAS and CERN object libraries
#-------------------------------------
 
LIBNAMES   = $(MAIN)
# LIBNAMES  += fputil bos fpack
# LIBNAMES  += bosio
LIBNAMES  += grafX11 packlib
LIBNAMES  += X11 m
#
#===================== End of MODIFICATION Section =========================
#

#--------------------
# Set up the targets
#--------------------

EXE = $(localbin)/$(MAIN)
LIBRARY = $(locallib)/lib$(MAIN).a

#-----------------------------------------
# Prepare libraries and their search path
#-----------------------------------------

LIBS = $(patsubst %,-l%,$(LIBNAMES))

VPATH_DIR = $(locallib):$(MATHLIB):$(CERNLIB)        

LIBS_DIR  = -L$(subst :, -L,$(VPATH_DIR))

vpath %.a $(strip $(VPATH_DIR)):/usr/local/lib:/usr/lib
VPATH =.:$(patsubst %/,:%,$(SUB_DIRECTORIES))

#----------------------------------------------
# build up the list of source and object files
#----------------------------------------------

FORTRAN_SEARCH  = $(patsubst %,%*.F, . $(SUB_DIRECTORIES))
FORTRAN_SOURCES = $(wildcard $(FORTRAN_SEARCH))
NOTDIR_F_SOURCES = $(notdir $(FORTRAN_SOURCES))

C_SEARCH  = $(patsubst %,%*.c, . $(SUB_DIRECTORIES))
C_SOURCES = $(wildcard $(C_SEARCH))

NOTDIR_C_SOURCES = $(notdir $(C_SOURCES))

OBJS  = $(NOTDIR_F_SOURCES:%.F=$(LIBRARY)(%.o)) \
	$(NOTDIR_C_SOURCES:%.c=$(LIBRARY)(%.o))

D 3
all: lib shlib
E 3
I 3
D 6
all: dirs lib shlib
E 6
I 6
D 7
all: dirs lib shlib bostest
E 7
I 7
all: dirs lib shlib
E 7
E 6
E 3

D 6
exe: lib $(EXE)
E 6
I 6
D 7
exe: lib $(EXE) bostest
E 7
I 7
exe: lib $(EXE)
E 7
E 6

lib: $(LIBRARY)

I 3
dirs:
D 5
	rm -rf $(shell uname)
	mkdir $(shell uname)
	mkdir $(shell uname)/lib
	mkdir $(shell uname)/obj
	mkdir $(shell uname)/run
E 5
I 5
	if (test ! -d $(shell uname)); then \
		mkdir $(shell uname); \
		mkdir $(shell uname)/lib; \
		mkdir $(shell uname)/obj; \
	fi
E 5

I 12
distclean: clean
E 12
E 3
clean:
	rm -f $(locallib)/libbosio.a $(localbin)/bosio $(locallib)/libBosio.so $(OBJ)/*.o
I 3
D 9
	cd   bostest;  gmake clean
E 9
E 3

shlib:
	rm -f $(OBJ)/bosio_*.o
	$(SHLD) $(OBJ)/*.o $(SHLIBS) -o $(locallib)/libBosio.so

install:
	cp $(locallib)/libbosio.a $(MYLIB)
	cp $(locallib)/libBosio.so $(MYLIB)
I 4
	rm -rf tmp
	mkdir tmp
	cp bosio.s/*.h tmp
	chmod 664 tmp/*
	cp tmp/* $(MYINC)
	rm -rf tmp
I 6
D 7
	cd   bostest;  gmake install
E 7
I 7
D 9
#	cd   bostest;  gmake install
E 9
E 7
E 6
E 4

I 10
headers:
	rm -rf tmp
	mkdir tmp
	cp bosio.s/*.h tmp
	chmod 664 tmp/*
	cp tmp/* $(MYINC)
	rm -rf tmp


E 10
$(LIBRARY): $(OBJS)
	@$(AR) t $(LIBRARY)
#	ar -x $(locallib)/libbosio.a bosio_main.o
#	ar -d $(locallib)/libbosio.a bosio_main.o
#	mv bosio_main.o $(locallib)
I 6


D 7
bostest:
E 7
I 7
D 9
bostest: bostest/coda_l3
E 7
	cd   bostest;  gmake
E 6


E 9
#$(EXE): $(REQUIRED_OBJS) $(patsubst %, lib%.a,$(LIBNAMES))
#	$(FC) $(REQUIRED_OBJS) \
#		$(FFLAGS) $(CLAS_FFLAGS) \
#		$(LIBS_DIR) $(LIBS) \
#		-o $(EXE)

.F.a:
	$(FC) $(FFLAGS) $(CLAS_FFLAGS) -c $<
	@$(AR) r $@ $%
	@$(RM) $%

.c.a:
	$(CC) $(CFLAGS) $(CLAS_FFLAGS) -c $<
	@$(AR) r $@ $%
#	@$(RM) $%
	mv $% $(OBJ)

E 1
