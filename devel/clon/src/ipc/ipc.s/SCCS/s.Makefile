h20535
s 00005/00000/00030
d D 1.18 00/07/25 14:57:21 wolin 19 18
c For release
e
s 00001/00001/00029
d D 1.17 00/07/25 12:44:34 wolin 18 17
c Do not need CLON_DEVxxx any more
e
s 00002/00002/00028
d D 1.16 00/07/10 13:11:33 wolin 17 16
c Minor mods for ss55 and 5.0 compilers
e
s 00001/00001/00029
d D 1.15 00/01/05 16:15:05 wolin 16 15
c Release to devel area
e
s 00001/00001/00029
d D 1.14 99/11/15 14:10:24 wolin 15 14
c Added ipc_output_dummy
e
s 00001/00001/00029
d D 1.13 99/06/04 09:43:11 wolin 14 13
c Changed name to ipc_msg_
c 
e
s 00004/00001/00026
d D 1.12 99/06/03 17:09:25 wolin 13 12
c Added info_server_msg_
c 
e
s 00001/00001/00026
d D 1.11 97/12/02 13:21:40 wolin 12 11
c Using common/include
c 
e
s 00001/00001/00026
d D 1.10 97/05/21 10:28:26 wolin 11 10
c CLON_LIB, not CLON_LINK
e
s 00000/00001/00027
d D 1.9 97/05/07 10:18:53 wolin 10 9
c CLAS changed to CLON
e
s 00005/00006/00023
d D 1.8 97/05/07 10:09:17 wolin 9 8
c CLAS changed to CLON
e
s 00001/00001/00028
d D 1.7 97/03/25 12:17:09 wolin 8 7
c Added init_msg_types
e
s 00001/00001/00028
d D 1.6 96/12/11 09:45:57 wolin 7 6
c Added dbr_init
e
s 00002/00002/00027
d D 1.5 96/10/07 17:16:12 wolin 6 5
c Added tclipc_init
e
s 00001/00001/00028
d D 1.4 96/10/02 16:31:08 wolin 5 4
c Added ipc_init_
e
s 00004/00001/00025
d D 1.3 96/09/30 17:09:09 muguira 4 3
c JAM - fix dependence.
e
s 00002/00002/00024
d D 1.2 96/08/29 17:39:21 muguira 3 1
c JAM - add more functions
e
s 00000/00000/00000
d R 1.2 96/07/30 17:28:36 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libipc/s/Makefile
c Name history : 1 0 s/makefile
e
s 00026/00000/00000
d D 1.1 96/07/30 17:28:35 muguira 1 0
c 
e
u
U
f e 0
t
T
I 1
#
# build the ipc lib.
#
D 10
#  this includes master.make from $(CLAS_ROOT)
E 10
#

D 9
include /usr/local/clas/master.make
E 9
D 3

E 3
I 3
DEBUG = -g
E 3
PROG = libipc.a
I 4
D 9
INCLUDE_DEPENDS = $(CLAS_INC)/clas_ipc.h \
	$(CLAS_INC)/clas_ipc_mt.h \
	$(CLAS_INC)/clas_ipc_prototypes.h
E 9
I 9
INCLUDE_DEPENDS = $(CLON_INC)/clas_ipc.h \
	$(CLON_INC)/clas_ipc_mt.h \
	$(CLON_INC)/clas_ipc_prototypes.h
E 9
E 4

all: $(PROG)

D 5
OBJS = ipc_init.o ipc_pack.o ipc_perror.o
E 5
I 5
D 6
OBJS = ipc_init.o ipc_pack.o ipc_perror.o ipc_init_.o
E 6
I 6
D 7
OBJS = ipc_init.o ipc_pack.o ipc_perror.o ipc_init_.o tclipc_init.o tclipc_init_.o
E 7
I 7
D 8
OBJS = ipc_init.o ipc_pack.o ipc_perror.o ipc_init_.o tclipc_init.o tclipc_init_.o dbr_init.o
E 8
I 8
D 13
OBJS = ipc_init.o ipc_pack.o ipc_perror.o ipc_init_.o tclipc_init.o tclipc_init_.o dbr_init.o init_msg_types.o
E 13
I 13
D 14
OBJS = ipc_init.o ipc_pack.o ipc_perror.o ipc_init_.o tclipc_init.o tclipc_init_.o dbr_init.o init_msg_types.o info_server_msg_.o
E 14
I 14
D 15
OBJS = ipc_init.o ipc_pack.o ipc_perror.o ipc_init_.o tclipc_init.o tclipc_init_.o dbr_init.o init_msg_types.o ipc_msg_.o
E 15
I 15
OBJS = ipc_init.o ipc_pack.o ipc_perror.o ipc_init_.o tclipc_init.o tclipc_init_.o dbr_init.o init_msg_types.o ipc_msg_.o ipc_output_dummy.o
E 15
E 14
E 13
E 8
E 7
E 6
E 5

D 4
$(PROG): $(OBJS)
E 4
I 4
$(PROG): $(OBJS) $(INCLUDE_DEPENDS)
E 4
	rm -f $(PROG)
	ar r $(PROG) $(OBJS)

.c.o:
D 3
	cc -c -I$(CLAS_INC) -I$(SMART_INC) $<
E 3
I 3
D 6
	cc -c $(DEBUG) -I$(CLAS_INC) -I$(SMART_INC) $<
E 6
I 6
D 9
	cc -c $(DEBUG) -I$(CLAS_INC) -I$(SMART_INC) -I$(CODA)/SunOS/include  $<
E 9
I 9
D 12
	rtlink -c $(DEBUG) -I$(CLON_INC) -I$(CODA)/SunOS/include  $<
E 12
I 12
D 17
	rtlink -c $(DEBUG) -I$(CLON_INC) -I$(CODA)/common/include  $<
E 17
I 17
	cc -c $(DEBUG) -I$(CLON_INC) -I$(CODA)/common/include -I$(RTHOME)/include  $<
E 17
I 13

.cc.o:
D 17
	rtlink -cxx -c $(DEBUG) -I$(CLON_INC) -I$(CODA)/common/include  $<
E 17
I 17
	CC -c $(DEBUG) -I$(CLON_INC) -I$(CODA)/common/include -I$(RTHOME)/include  $<
E 17
E 13
E 12
E 9
E 6
E 3

clean:
	rm -f *.o $(PROG)

I 19
distclean:
	rm -f *.o $(PROG)


install: exports
E 19
exports:
D 9
	cp $(PROG) $(CLAS_LINK)
E 9
I 9
D 11
	cp $(PROG) $(CLON_LINK)
E 11
I 11
D 16
	cp $(PROG) $(CLON_LIB)
E 16
I 16
D 18
	cp $(PROG) $(CLON_DEVLIB)
E 18
I 18
	cp $(PROG) $(CLON_LIB)
E 18
E 16
E 11
E 9
E 1
