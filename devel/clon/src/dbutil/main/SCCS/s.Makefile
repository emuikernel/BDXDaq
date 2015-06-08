h46460
s 00001/00001/00027
d D 1.23 02/08/16 15:46:27 wolin 24 23
c -lelf
e
s 00001/00001/00027
d D 1.22 02/08/09 10:34:12 boiarino 23 22
c minor
c 
e
s 00005/00005/00023
d D 1.21 00/09/21 08:33:10 wolin 22 21
c Updated
e
s 00004/00002/00024
d D 1.20 00/07/25 15:25:33 wolin 21 20
c For release
e
s 00001/00000/00025
d D 1.19 00/07/25 14:38:59 wolin 20 19
c add install
e
s 00001/00001/00024
d D 1.18 00/07/25 12:38:34 wolin 19 18
c Do not need CLON_DEVxxx any more
e
s 00001/00002/00024
d D 1.17 00/01/12 13:43:49 wolin 18 17
c Removed cmlog
e
s 00001/00001/00025
d D 1.16 00/01/05 16:32:49 wolin 17 16
c Release to devel area
e
s 00001/00001/00025
d D 1.15 99/06/01 15:49:53 wolin 16 15
c Added posix4
c 
e
s 00001/00001/00025
d D 1.14 98/01/06 15:19:16 wolin 15 14
c Reordering link solved segv problem in TipcSrv::Instance()
c 
e
s 00001/00001/00025
d D 1.13 97/09/09 13:56:58 wolin 14 13
c Eliminated CODA_LIB
c 
e
s 00002/00001/00024
d D 1.12 97/05/23 12:03:56 wolin 13 12
c Using new cmlog
e
s 00005/00008/00020
d D 1.11 97/05/06 17:34:46 wolin 12 11
c CLAS changed to CLON
e
s 00001/00001/00027
d D 1.10 97/04/02 12:10:34 wolin 11 10
c Using new cmlog message system
e
s 00001/00001/00027
d D 1.9 97/02/18 09:35:20 wolin 10 9
c Added start/stop message to clasmsg database
e
s 00008/00006/00020
d D 1.8 97/01/14 15:40:28 wolin 9 8
c Added tcl capability
e
s 00001/00001/00025
d D 1.7 96/09/05 13:34:14 wolin 8 7
c New scheme for mt initialization 
e
s 00004/00004/00022
d D 1.6 96/08/09 11:31:43 wolin 7 6
c Now uses dbrsql.scc
e
s 00000/00000/00026
d D 1.5 96/08/02 13:45:01 wolin 6 5
c Minor mods
c 
e
s 00002/00002/00024
d D 1.4 96/07/31 17:21:42 wolin 5 4
c Now works with dbrsql.sc
e
s 00003/00003/00023
d D 1.3 96/07/30 14:23:22 wolin 4 3
c Now working on SUN with new Ingres installation
e
s 00003/00003/00023
d D 1.2 96/07/26 17:12:53 wolin 3 1
c Using esqlcc now
e
s 00000/00000/00000
d R 1.2 96/07/26 17:09:46 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 dbrouter/s/Makefile
c Name history : 1 0 s/Makefile
e
s 00026/00000/00000
d D 1.1 96/07/26 17:09:45 wolin 1 0
c Makefile for dbrouter
e
u
U
f e 0
t
T
I 1
#  makes dbrouter

I 13
D 18
CMLOG   = /usr/local/cmlog
E 18
E 13
D 9
LIB = /usr/local/clas/lib
BIN = /usr/local/clas/bin
INC = /usr/local/clas/include
E 9
I 9
D 12
LIB     = /usr/local/clas/lib
BIN     = /usr/local/clas/bin
INC     = /usr/local/clas/include
E 12
TCLINC  = $(CODA)/common/include
D 14
TCLLIB  = $(CODA_LIB)
E 14
I 14
TCLLIB  = $(TCL_LIBRARY)
E 14
E 9

all: dbrouter

D 12
dbrouter: dbrouter.o dbrsql.o $(LIB)/libipc.a
D 9
	rtlink -cxx -g -o dbrouter dbrouter.o dbrsql.o -L$(LIB) -lipc -L$(II_SYSTEM)/ingres/lib  -lingres -lm -lc
E 9
I 9
D 10
	rtlink -cxx -g -o dbrouter dbrouter.o dbrsql.o -L$(LIB) -lipc -L$(TCLLIB) -ltcl -L$(II_SYSTEM)/ingres/lib  -lingres -lm -lc
E 10
I 10
D 11
	rtlink -cxx -g -o dbrouter dbrouter.o dbrsql.o -L$(LIB) -lipc -L$(TCLLIB) -ltcl -L$(II_SYSTEM)/ingres/lib  -lingres -L/usr/local/coda/2.0dev/SunOS/lib -lcmsg -lmsql -lm -lc
E 11
I 11
	rtlink -cxx -g -o dbrouter dbrouter.o dbrsql.o -L$(LIB) -lipc -lutil -L$(TCLLIB) -ltcl -L$(II_SYSTEM)/ingres/lib  -lingres -L/usr/local/coda/2.0dev/SunOS/lib -lcmlog -lm -lc
E 12
I 12
D 22
dbrouter: dbrouter.o dbrsql.o $(CLON_LIB)/libipc.a
D 13
	rtlink -cxx -g -o dbrouter dbrouter.o dbrsql.o -L$(CLON_LIB) -lipc -lutil -L$(TCLLIB) -ltcl -L$(II_SYSTEM)/ingres/lib  -lingres -L/usr/local/coda/2.0dev/SunOS/lib -lcmlog -lm -lc
E 13
I 13
D 15
	rtlink -cxx -g -o dbrouter dbrouter.o dbrsql.o -L$(CLON_LIB) -lipc -lutil -L$(TCLLIB) -ltcl -L$(II_SYSTEM)/ingres/lib  -lingres -L$(CMLOG)/lib/solaris -lcmlog -lm -lc
E 15
I 15
D 16
	rtlink -cxx -g -o dbrouter dbrouter.o dbrsql.o -L$(CLON_LIB) -lipc -lutil -L$(TCLLIB) -ltcl -L$(CMLOG)/lib/solaris -lcmlog -L$(II_SYSTEM)/ingres/lib -lingres
E 16
I 16
D 18
	rtlink -cxx -g -o dbrouter dbrouter.o dbrsql.o -L$(CLON_LIB) -lipc -lutil -L$(TCLLIB) -ltcl -L$(CMLOG)/lib/solaris -lcmlog -L$(II_SYSTEM)/ingres/lib -lingres -lposix4
E 18
I 18
	rtlink -cxx -g -o dbrouter dbrouter.o dbrsql.o -L$(CLON_LIB) -lipc -lutil -L$(TCLLIB) -ltcl -L$(II_SYSTEM)/ingres/lib -lingres -lposix4
E 22
I 22
dbrouter: dbrouter.o dbrsql.o
D 24
	rtlink -cxx -o dbrouter dbrouter.o dbrsql.o -L$(CLON_LIB) -lipc -lutil -L$(CODA_LIB) -ltcl -L$(II_SYSTEM)/ingres/lib -lingres -lposix4
E 24
I 24
	rtlink -cxx -o dbrouter dbrouter.o dbrsql.o -L$(CLON_LIB) -lipc -lutil -L$(CODA_LIB) -ltcl -L$(II_SYSTEM)/ingres/lib -lingres -lposix4 -lelf
E 24
E 22
E 18
E 16
E 15
E 13
E 12
E 11
E 10
E 9


D 8
dbrouter.o: dbrouter.h dbrouter.cc $(INC)/clas_ipc.h $(INC)/clas_ipc_mt.h $(INC)/clas_ipc_prototypes.h
E 8
I 8
D 12
dbrouter.o: dbrouter.h dbrouter.cc $(INC)/clas_ipc.h $(INC)/clas_ipc_prototypes.h
E 8
D 9
	rtlink -cxx -c -g -I. -I$(INC) dbrouter.cc
E 9
I 9
	rtlink -cxx -c -g -I. -I$(INC) -I$(TCLINC) dbrouter.cc
E 12
I 12
D 22
dbrouter.o: dbrouter.h dbrouter.cc $(CLON_INC)/clas_ipc.h $(CLON_INC)/clas_ipc_prototypes.h
	rtlink -cxx -c -g -I. -I$(CLON_INC) -I$(TCLINC) dbrouter.cc
E 22
I 22
dbrouter.o: dbrouter.h dbrouter.cc
	CC -c -I. -I$(CLON_INC) -I$(TCLINC) -I$(RTHOME)/include dbrouter.cc
E 22
E 12
E 9


D 3
dbrsql.o: dbrouter.h dbrsql.sc
	esqlcc dbrsql.sc
E 3
I 3
D 5
dbrsql.o: dbrouter.h dbrsql.scc
D 4
	esqlcc dbrsql.scc
E 3
	rtlink -cxx -g -c -I. dbrsql.cc
E 4
I 4
	esqlc dbrsql.scc
E 5
I 5
D 7
dbrsql.o: dbrouter.h dbrsql.sc
	esqlc dbrsql.sc
E 5
	rtlink -Aa -g -c -I. dbrsql.c
E 7
I 7
dbrsql.o: dbrouter.h dbrsql.scc
	esqlcc dbrsql.scc
D 9
	rtlink -cxx -g -c -I. dbrsql.cc
E 9
I 9
D 22
	rtlink -cxx -g -c -I. -I$(TCLINC) dbrsql.cc
E 22
I 22
	CC -c -I. -I$(TCLINC) -I$(RTHOME)/include dbrsql.cc
E 22
E 9
E 7
E 4

clean:
D 21
	rm -f *.o
D 3
	rm -f dbrsql.c
E 3
I 3
D 4
	rm -f dbrsql.cc
E 4
I 4
D 7
	rm -f dbrsql.c
E 7
I 7
	rm -f dbrsql.cc
E 21
I 21
D 23
	rm -f *.o dbrsql.cc
E 23
I 23
	rm -f *.o dbrsql.cc dbrouter
E 23

distclean:
	rm -f *.o dbrsql.cc dbrouter
E 21
E 7
E 4
E 3

I 20
install: exports
E 20
exports:
D 12
	cp dbrouter $(BIN)
E 12
I 12
D 17
	cp dbrouter $(CLON_BIN)
E 17
I 17
D 19
	cp dbrouter $(CLON_DEVBIN)
E 19
I 19
	cp dbrouter $(CLON_BIN)
E 19
E 17
E 12
E 1
