h31864
s 00001/00001/00082
d D 1.20 03/06/24 10:05:51 wolin 21 20
c New java
e
s 00005/00001/00078
d D 1.19 02/10/02 15:06:04 wolin 20 19
c Minor
e
s 00005/00001/00074
d D 1.18 01/09/28 14:53:28 wolin 19 18
c Added ipc_check
e
s 00001/00001/00074
d D 1.17 00/09/07 14:48:38 wolin 18 17
c New CC
e
s 00010/00007/00065
d D 1.16 00/07/25 14:54:55 wolin 17 16
c For release
e
s 00002/00002/00070
d D 1.15 00/07/25 12:43:36 wolin 16 15
c Do not need CLON_DEVxxx any more
e
s 00001/00001/00071
d D 1.14 00/01/12 13:48:07 wolin 15 14
c Cleaned cmlog stuff
e
s 00002/00002/00070
d D 1.13 00/01/05 16:15:35 wolin 14 13
c Release to devel area
e
s 00021/00001/00051
d D 1.12 00/01/04 16:06:49 wolin 13 12
c Combined many workspaces
e
s 00006/00002/00046
d D 1.11 00/01/04 15:36:45 wolin 12 11
c Added error_msg
e
s 00001/00000/00047
d D 1.10 99/11/22 14:09:47 wolin 11 10
c Added other ipc_monitor classes to export
e
s 00004/00001/00043
d D 1.9 99/11/22 12:38:09 wolin 10 9
c Added ipc_monitor
e
s 00005/00001/00039
d D 1.8 99/06/29 14:22:51 wolin 9 8
c Added ipc_cmlog_browser
c 
e
s 00001/00001/00039
d D 1.7 99/06/22 12:08:46 wolin 8 7
c Needed libipc in ipc_msg
c 
e
s 00005/00001/00035
d D 1.6 99/06/22 11:50:00 wolin 7 6
c Added ipc_msg
c 
e
s 00001/00001/00035
d D 1.5 99/06/01 15:56:14 wolin 6 5
c Added posix4
c 
e
s 00002/00002/00034
d D 1.4 99/05/18 14:57:10 wolin 5 4
c Seems to be working
c 
e
s 00006/00002/00030
d D 1.3 99/05/18 10:05:51 wolin 4 3
c Added cmlog2ipc and ipc2cmlog
c 
e
s 00004/00001/00028
d D 1.2 98/11/11 15:19:57 wolin 3 1
c Added ipc2cmlog
c 
e
s 00000/00000/00000
d R 1.2 98/06/08 15:48:37 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 ipcutil/s/Makefile
e
s 00029/00000/00000
d D 1.1 98/06/08 15:48:36 wolin 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
#
# builds ipc utilities
#

D 18
DEBUG = -g
E 18
I 18
###DEBUG = -g
E 18
D 17
CMLOG = /usr/local/cmlog
CDEV  = /usr/local/cdev	
E 17


D 3
PROG = ipc_connections
E 3
I 3
D 4
PROG = ipc_connections ipc2cmlog
E 4
I 4
D 7
PROG = ipc_connections ipc2cmlog cmlog2ipc
E 7
I 7
D 9
PROG = ipc_connections ipc2cmlog cmlog2ipc ipc_msg
E 9
I 9
D 10
PROG = ipc_connections ipc2cmlog cmlog2ipc ipc_msg ipc_cmlog_browser
E 10
I 10
D 12
PROG = ipc_connections ipc2cmlog cmlog2ipc ipc_msg ipc_cmlog_browser ipc_monitor.class
E 12
I 12
D 13
PROG = ipc_connections ipc2cmlog cmlog2ipc ipc_msg ipc_cmlog_browser ipc_monitor.class error_msg
E 13
I 13
D 19
PROG = ipc_connections ipc2cmlog cmlog2ipc ipc_msg ipc_cmlog_browser ipc_monitor.class error_msg ipc_control ipc_testmon tclipc ipc_info rc_transition
E 19
I 19
PROG = ipc_connections ipc2cmlog cmlog2ipc ipc_msg ipc_cmlog_browser ipc_monitor.class error_msg ipc_control ipc_testmon tclipc ipc_info rc_transition ipc_check
E 19
E 13
E 12
E 10
E 9
E 7
E 4
E 3


all: $(PROG)
 

ipc_connections:  ipc_connections.cc
D 6
	rtlink -cxx $(DEBUG) -o $@ $@.cc -I$(CLON_INC) -L$(CLON_LIB) -lutil -L$(CDEV)/lib -lcdev -L$(CMLOG)/lib -lcmlog -L$(CODA)/SunOS/lib
E 6
I 6
D 15
	rtlink -cxx $(DEBUG) -o $@ $@.cc -I$(CLON_INC) -L$(CLON_LIB) -lutil -L$(CDEV)/lib -lcdev -L$(CMLOG)/lib -lcmlog -L$(CODA)/SunOS/lib -lposix4
E 15
I 15
D 17
	rtlink -cxx $(DEBUG) -o $@ $@.cc -I$(CLON_INC) -L$(CLON_LIB) -lutil -L$(CDEV)/lib -lcdev -L$(CODA)/SunOS/lib -lposix4
E 17
I 17
	rtlink -cxx $(DEBUG) -o $@ $@.cc -I$(CLON_INC) -L$(CLON_LIB) -lutil -L$(CDEVROOT)/lib -lcdev -L$(CODA)/SunOS/lib -lposix4
E 17
E 15
E 6

I 3

ipc2cmlog:  ipc2cmlog.cc
D 4
	rtlink -cxx $(DEBUG) -o $@ $@.cc -I$(CLON_INC) -I$(CODA)/common/include -L$(CLON_LIB) -lipc -lutil -L$(CMLOG)/lib -lcmlog -L$(CODA)/SunOS/lib -ltcl -lposix4
E 4
I 4
D 5
	rtlink -cxx $(DEBUG) -D_CMLOG_BUILD_CLIENT -o $@ $@.cc -I$(CLON_INC) -I$(CODA)/common/include -I$(CMLOG)/include -I$(CDEV)/include -L$(CLON_LIB) -lipc -lutil -L$(CMLOG)/lib -lcmlog -L$(CODA)/SunOS/lib -ltcl -lposix4
E 5
I 5
D 17
	rtlink -cxx $(DEBUG) -D_CMLOG_BUILD_CLIENT -o $@ $@.cc -I$(CLON_INC) -I$(CODA)/common/include -I$(CMLOG)/include -L$(CLON_LIB) -lipc -lutil -L$(CMLOG)/lib -lcmlog -L$(CODA)/SunOS/lib -ltcl -lposix4
E 17
I 17
	rtlink -cxx $(DEBUG) -D_CMLOG_BUILD_CLIENT -o $@ $@.cc -I$(CLON_INC) -I$(CODA)/common/include -I$(CMLOGROOT)/include -L$(CLON_LIB) -lipc -lutil -L$(CMLOGROOT)/lib -lcmlog -L$(CODA)/SunOS/lib -ltcl -lposix4
E 17
E 5


cmlog2ipc:  cmlog2ipc.cc
D 5
	rtlink -cxx $(DEBUG) -D_CMLOG_BUILD_CLIENT -o $@ $@.cc -I$(CLON_INC) -I$(CODA)/common/include -I$(CMLOG)/include -I$(CDEV)/include -L$(CLON_LIB) -lipc -lutil -L$(CMLOG)/lib -lcmlog -L$(CODA)/SunOS/lib -ltcl -lposix4
E 5
I 5
D 17
	rtlink -cxx $(DEBUG) -D_CMLOG_BUILD_CLIENT -o $@ $@.cc -I$(CLON_INC) -I$(CODA)/common/include -I$(CMLOG)/include -I$(CDEV)/include -L$(CLON_LIB) -lipc -lutil -L$(CMLOG)/lib -lcmlogb -lcmlog -L$(CDEV) -lcdev -L$(CODA)/SunOS/lib -ltcl -lposix4
E 17
I 17
	rtlink -cxx $(DEBUG) -D_CMLOG_BUILD_CLIENT -o $@ $@.cc -I$(CLON_INC) -I$(CODA)/common/include -I$(CMLOGROOT)/include -I$(CDEVROOT)/include -L$(CLON_LIB) -lipc -lutil -L$(CMLOGROOT)/lib -lcmlogb -lcmlog -L$(CDEVROOT) -lcdev -L$(CODA)/SunOS/lib -ltcl -lposix4
E 17
I 7


ipc_msg: ipc_msg.cc
D 8
	rtlink -cxx $(DEBUG) -o $@ $@.cc
E 8
I 8
	rtlink -cxx $(DEBUG) -o $@ $@.cc -L$(CLON_LIB) -lipc
I 9


ipc_cmlog_browser:  ipc_cmlog_browser.cc
	rtlink -cxx $(DEBUG) -o $@ $@.cc -I$(CLON_INC) -L$(CLON_LIB) -lipc -lutil -lposix4
E 9
E 8
E 7
E 5
E 4
E 3

I 10

ipc_monitor.class: ipc_monitor.java
D 21
	javac ipc_monitor.java
E 21
I 21
	/apps/java/jdk1.4.0/bin/javac ipc_monitor.java
E 21
E 10


I 12
error_msg:  error_msg.cc
	rtlink -cxx $(DEBUG) -o $@ $@.cc -I$(CLON_INC) -L$(CLON_LIB) -lipc -lutil
I 13


ipc_control:  ipc_control.cc
	rtlink -cxx $(DEBUG) -o $@ $@.cc -I$(CLON_INC) -L$(CLON_LIB) -lipc -lutil


ipc_testmon: ipc_testmon.c
D 20
	rtlink $(DEBUG) -o $@ $@.c -I$(CLON_INC) -L$(CLON_LIB) -lipc
E 20
I 20
	cc -c $(DEBUG) -I$(CLON_INC) -I$(RTHOME)/include ipc_testmon.c
	rtlink $(DEBUG) -o $@ $@.o -L$(CLON_LIB) -lipc
E 20


tclipc: tclipc.cc
	rtlink -cxx $(DEBUG) -o $@ $@.cc -I$(CLON_INC) -L$(CLON_LIB) -lipc -lutil


ipc_info: ipc_info.cc
	rtlink -cxx $(DEBUG) -o $@ $@.cc -I$(CLON_INC) -L$(CLON_LIB) -lipc -lutil


rc_transition: rc_transition.cc
	rtlink -cxx $(DEBUG) -o $@ $@.cc -L$(CLON_LIB) -lipc -lutil -L$(CODA)/SunOS/lib -lmsql
I 19


ipc_check:  ipc_check.cc
	rtlink -cxx $(DEBUG) -o $@ $@.cc -I$(CLON_INC) -L$(CLON_LIB) -lipc -lutil
E 19
E 13


E 12
.cc.o:
D 12
	CC -c $(DEBUG) $<
E 12
I 12
D 17
	rtlink -c $(DEBUG) $<
E 17
I 17
	CC -c $(DEBUG) -I$(RTHOME)/include $<
E 17
E 12

I 20
.c.o:
	cc -c $(DEBUG) -I$(RTHOME)/include $<

E 20
clean:
D 17
	rm -f *.o $(PROG)
E 17
I 17
	rm -f *.o core
E 17

I 17
distclean:
	rm -f *.o core $(PROG) *.class


install: exports
E 17
exports:
D 14
	cp $(PROG) $(CLON_BIN)
I 11
	cp ipc_monitor*.class $(CLON_BIN)
E 14
I 14
D 16
	cp $(PROG) $(CLON_DEVBIN)
	cp ipc_monitor*.class $(CLON_DEVBIN)
E 16
I 16
	cp $(PROG)            $(CLON_BIN)
	cp ipc_monitor*.class $(CLON_BIN)
E 16
E 14
E 11
E 1
