h00018
s 00001/00001/00051
d D 1.12 00/08/29 16:21:09 wolin 13 12
c Removed unneeded stuff
e
s 00005/00001/00047
d D 1.11 00/07/25 15:21:11 wolin 12 11
c Added ftest
e
s 00003/00003/00045
d D 1.10 00/07/25 15:13:25 wolin 11 10
c Typo
e
s 00006/00004/00042
d D 1.9 00/07/25 14:59:19 wolin 10 9
c For release
e
s 00001/00001/00045
d D 1.8 00/07/25 12:50:10 wolin 9 8
c Do not need CLON_DEVxxx any more
e
s 00001/00001/00045
d D 1.7 00/01/12 15:37:23 wolin 8 7
c Using DEVBIN
e
s 00007/00010/00039
d D 1.6 97/05/07 10:25:51 wolin 7 6
c CLAS changed to CLON
e
s 00011/00003/00038
d D 1.5 96/11/18 11:57:46 wolin 6 5
c Added warm_connection, other minor mods
e
s 00005/00000/00036
d D 1.4 96/11/15 16:26:11 muguira 5 4
c JAM - fix a bug.
e
s 00006/00003/00030
d D 1.3 96/11/01 15:25:14 muguira 4 3
c JAM - collate the various tt?.c files into 1 - pack_test.c
c add pack_test to build
e
s 00003/00000/00030
d D 1.2 96/10/11 09:59:02 wolin 3 1
c Minor mods
e
s 00000/00000/00000
d R 1.2 96/08/29 17:38:10 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libipc/test/Makefile
c Name history : 1 0 test/makefile
e
s 00030/00000/00000
d D 1.1 96/08/29 17:38:09 muguira 1 0
c JAM build test tools for libipc
e
u
U
f e 0
t
T
I 1
#
# build the ipc lib test programs..
#
D 7
#  this includes master.make from $(CLAS_ROOT)
#
E 7

D 7

E 7
DEBUG = -g
I 10
D 11
PROG = ipctest rcv tt1 tt2 tt3 tt4 pack_test rcv2 warm_connection
E 11
I 11
D 12
PROG = ipctest rcv1 tt1 tt2 tt3 tt4 pack_test rcv2 warm_connection
E 12
I 12
D 13
PROG = ipctest rcv1 tt1 tt2 tt3 tt4 pack_test rcv2 warm_connection ftest
E 13
I 13
PROG = warm_connection
E 13
E 12
E 11
E 10

D 4
all: rcv tt1
E 4
I 4
D 6
all: rcv tt1 pack_test rcv2
E 6
I 6
D 10
all: rcv tt1 pack_test rcv2 warm_connection
E 10
E 6
E 4

D 4
OBJS = rcv1.o tt1.o rcv2.o 
E 4
I 4
D 10
OBJS = rcv1.o tt1.o rcv2.o pack_test.o tt2.o
E 10
I 10
all: $(PROG)
E 10
E 4

I 3
ipctest:
D 7
	rtlink -g -o ipctest ipctest.c -I$(CLAS_INC) -L$(CLAS_LIB) -lipc -L$(CODA)/lib -ltcl
E 7
I 7
	rtlink -g -o ipctest ipctest.c -I$(CLON_INC) -L$(CLON_LIB) -lipc -L$(CODA)/lib -ltcl
E 7

E 3
D 11
rcv:
	rtlink -g -o rcv rcv1.c -L../s -lipc 
E 11
I 11
rcv1:
	rtlink -g -o rcv1 rcv1.c -L../s -lipc 
E 11

rcv2:
D 7
	rtlink -g -o rcv2 rcv2.c -I$(CLAS_ROOT)/include -L../s -lipc 
E 7
I 7
	rtlink -g -o rcv2 rcv2.c -I$(CLON_ROOT)/include -L../s -lipc 
E 7

tt1:
	rtlink -g -o tt1 tt1.c  -L../s -lipc 

tt2:
D 7
	rtlink -g -o tt2 tt2.c  -I$(CLAS_ROOT)/include -L../s -lipc 
E 7
I 7
	rtlink -g -o tt2 tt2.c  -I$(CLON_ROOT)/include -L../s -lipc 
E 7

I 5
tt3:
D 7
	rtlink -g -o tt3 tt3.c -I$(CLAS_ROOT)/include -L../s -lipc 
E 7
I 7
	rtlink -g -o tt3 tt3.c -I$(CLON_ROOT)/include -L../s -lipc 
E 7
tt4:
D 7
	rtlink -g -o tt4 tt4.c -I$(CLAS_ROOT)/include -L../s -lipc 
E 7
I 7
	rtlink -g -o tt4 tt4.c -I$(CLON_ROOT)/include -L../s -lipc 
E 7

E 5
I 4
pack_test:
D 7
	rtlink -g -o pack_test pack_test.c -I$(CLAS_ROOT)/include -L../s -lipc 
E 7
I 7
	rtlink -g -o pack_test pack_test.c -I$(CLON_ROOT)/include -L../s -lipc 
E 7

I 12

E 12
I 6
warm_connection:
	rtlink -g -o warm_connection warm_connection.c

I 12

ftest: ftest.f
	 CC=f77; export CC; rtlink -g -o ftest ftest.f -L$(CLON_LIB) -lipc -lutil
E 12

E 6
E 4
clean:
D 4
	rm -f *.o rcv tt2 tt1 rcv2
E 4
I 4
D 6
	rm -f *.o rcv tt2 tt1 rcv2 pack_test
E 6
I 6
D 10
	rm -f *.o
E 10
I 10
	rm -f *.o 

E 10
E 6
E 4

I 6
distclean:
D 10
	rm -f *.o rcv tt2 tt1 rcv2 pack_test warm_connection
E 10
I 10
	rm -f *.o $(PROG)
E 10


I 10
install: exports
E 10
E 6
exports:
D 6
	cp $(PROG) $(CLAS_LINK)
E 6
I 6
D 7
	cp warm_connection $(CLAS_BIN)
E 7
I 7
D 8
	cp warm_connection $(CLON_BIN)
E 8
I 8
D 9
	cp warm_connection $(CLON_DEVBIN)
E 9
I 9
	cp warm_connection $(CLON_BIN)
E 9
E 8
E 7
E 6
E 1
