h45233
s 00001/00001/00041
d D 1.20 02/08/16 15:44:33 wolin 22 21
c -lelf
e
s 00001/00001/00041
d D 1.19 00/09/11 15:07:10 wolin 21 19
c No debug	
e
s 00001/00001/00041
d D 1.18.1.1 00/08/29 16:59:33 wolin 20 19
c No debug mode
e
s 00001/00000/00041
d D 1.18 00/07/25 14:37:08 wolin 19 18
c add install
e
s 00001/00001/00040
d D 1.17 00/07/25 13:14:37 wolin 18 17
c Cleanup
e
s 00003/00003/00038
d D 1.16 00/07/25 13:07:40 wolin 17 16
c Cleanup
e
s 00001/00001/00040
d D 1.15 00/07/25 12:35:48 wolin 16 15
c Do not need CLON_DEVxxx any more
e
s 00001/00001/00040
d D 1.14 00/01/05 16:34:56 wolin 15 14
c Typo
e
s 00001/00001/00040
d D 1.13 00/01/05 16:33:51 wolin 14 13
c Release to devel area
c 
e
s 00003/00006/00038
d D 1.12 99/05/19 16:04:24 wolin 13 12
c Updated for new ipc alarm system
c 
e
s 00001/00001/00043
d D 1.11 98/10/28 12:07:48 wolin 12 11
c Added posix4
c 
e
s 00001/00001/00043
d D 1.10 98/10/21 09:43:09 wolin 11 10
c Needed posix for alarm handler
c 
e
s 00003/00003/00041
d D 1.9 98/10/19 15:24:06 wolin 10 9
c Minor upgrade
c 
e
s 00001/00001/00043
d D 1.8 98/10/19 12:46:28 wolin 9 8
c Fixed alarm_monitor
c 
e
s 00005/00001/00039
d D 1.7 98/10/09 09:37:45 wolin 8 7
c Added alarm_mgr
c 
e
s 00010/00004/00030
d D 1.6 98/06/26 13:54:45 wolin 7 6
c Added alarm_server
c 
e
s 00001/00001/00033
d D 1.5 98/04/23 10:01:47 wolin 6 5
c Minor mods
c 
e
s 00005/00002/00029
d D 1.4 98/04/23 09:16:25 wolin 5 4
c Minor mods
c 
e
s 00003/00004/00028
d D 1.3 97/05/06 17:29:25 wolin 4 3
c CLAS changed to CLON
e
s 00002/00002/00030
d D 1.2 97/04/08 16:51:06 wolin 3 1
c Now cmlog and cmlogBrowser enabled
e
s 00000/00000/00000
d R 1.2 97/02/19 09:04:42 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 alarm_monitor/s/Makefile
c Name history : 1 0 s/Makefile
e
s 00032/00000/00000
d D 1.1 97/02/19 09:04:41 wolin 1 0
c Monitors alarm conditions and takes corrective action
e
u
U
f e 0
t
T
I 1

D 10
#  makefile for alarm_monitor
E 10
I 10
#  makefile for online alarm system tilities
E 10

#  ejw, 15-jan-97

I 5
D 13
CDEV = /usr/local/cdev
CMLOG = /usr/local/cmlog

E 13
E 5
D 4
include /usr/local/clas/master.make
E 4
D 7
OBJS = alarm_monitor.o alarm_callbacks.o
E 7
D 20
D 21
DEBUG = -g
E 20
I 20
###DEBUG = -g
E 20
E 21
I 21
###DEBUG = -g
E 21

D 7
PROG = alarm_monitor
E 7
I 7
D 8
PROG = alarm_monitor alarm_server
E 8
I 8
D 10
PROG = alarm_monitor alarm_server alarm_mgr.class
E 10
I 10
PROG = alarm_handler alarm_server alarm_mgr.class
E 10
E 8
E 7

I 7

E 7
all: $(PROG)
 
D 7
$(PROG): $(OBJS)
D 3
	rtlink -cxx $(DEBUG) -o $@ $(OBJS) -L$(CLAS_LIB) -lipc -lutil -L$(CODA)/SunOS/lib -lcmsg -lmsql -lcdev -ltcl
E 3
I 3
D 4
	rtlink -cxx $(DEBUG) -o $@ $(OBJS) -L$(CLAS_LIB) -lipc -lutil -L$(CODA)/SunOS/lib -lcdev -lcmlogb -lcmlog -ltcl
E 4
I 4
D 5
	rtlink -cxx $(DEBUG) -o $@ $(OBJS) -L$(CLON_LIB) -lipc -lutil -L$(CODA)/SunOS/lib -lcdev -lcmlogb -lcmlog -ltcl
E 5
I 5
	rtlink -cxx $(DEBUG) -o $@ $(OBJS) -L$(CLON_LIB) -lipc -lutil -L$(CMLOG)/lib/solaris -lcmlogb -lcmlog -L$(CDEV)/lib/solaris -lcdev -L$(CODA)/SunOS/lib -ltcl
E 7
E 5
E 4
E 3

I 7
alarm_server: alarm_server.scc
	esqlcc alarm_server.scc
D 12
	rtlink -cxx $(DEBUG) -o $@ -D_CMLOG_BUILD_CLIENT $@.cc -I$(CLON_INC) -I$(CMLOG)/include -I$(CDEV)/include -I$(CODA)/common/include -L$(CLON_LIB) -lipc -lutil -L$(CMLOG)/lib/solaris -lcmlogb -lcmlog -L$(CDEV)/lib/solaris -lcdev -L$(CODA)/SunOS/lib -ltcl -L$(II_SYSTEM)/ingres/lib/ -lingres
E 12
I 12
D 13
	rtlink -cxx $(DEBUG) -o $@ -D_CMLOG_BUILD_CLIENT $@.cc -I$(CLON_INC) -I$(CMLOG)/include -I$(CDEV)/include -I$(CODA)/common/include -L$(CLON_LIB) -lipc -lutil -L$(CMLOG)/lib/solaris -lcmlogb -lcmlog -L$(CDEV)/lib/solaris -lcdev -L$(CODA)/SunOS/lib -ltcl -L$(II_SYSTEM)/ingres/lib/ -lingres -lposix4
E 13
I 13
D 22
	rtlink -cxx $(DEBUG) -o $@ $@.cc -I$(CLON_INC)  -I$(CODA)/common/include -L$(CLON_LIB) -lipc -lutil -L$(CODA)/SunOS/lib -ltcl -L$(II_SYSTEM)/ingres/lib/ -lingres -lposix4
E 22
I 22
	rtlink -cxx $(DEBUG) -o $@ $@.cc -I$(CLON_INC)  -I$(CODA)/common/include -L$(CLON_LIB) -lipc -lutil -L$(CODA)/SunOS/lib -ltcl -L$(II_SYSTEM)/ingres/lib/ -lingres -lposix4 -lelf
E 22
E 13
E 12


D 9
alarm_monitor: alarm_monitor.cc
E 9
I 9
D 10
alarm_monitor: alarm_monitor.o
E 10
I 10
alarm_handler: alarm_handler.o
E 10
E 9
D 11
	rtlink -cxx $(DEBUG) -o $@ $@.o -D_CMLOG_BUILD_CLIENT -L$(CLON_LIB) -lipc -lutil -L$(CMLOG)/lib/solaris -lcmlogb -lcmlog -L$(CDEV)/lib/solaris -lcdev -L$(CODA)/SunOS/lib -ltcl
E 11
I 11
D 13
	rtlink -cxx $(DEBUG) -o $@ $@.o -D_CMLOG_BUILD_CLIENT -L$(CLON_LIB) -lipc -lutil -L$(CMLOG)/lib/solaris -lcmlogb -lcmlog -L$(CDEV)/lib/solaris -lcdev -L$(CODA)/SunOS/lib -ltcl -lposix4
E 13
I 13
	rtlink -cxx $(DEBUG) -o $@ $@.o -L$(CLON_LIB) -lipc -lutil -L$(CODA)/SunOS/lib -ltcl -lposix4
E 13
E 11

E 7
 
I 8
alarm_mgr.class: alarm_mgr.java
	javac alarm_mgr.java


E 8
.cc.o:
D 3
	rtlink -cxx -c $(DEBUG) -I. -I$(CLAS_INC) -I$(CODA)/SunOS/include  -I$(CODA)/common/include/cdev -I$(CODA)/common/include $<
E 3
I 3
D 4
	rtlink -cxx -D_CMLOG_BUILD_CLIENT -c $(DEBUG) -I. -I$(CLAS_INC) -I$(CODA)/common/include/cmlog -I$(CODA)/common/include/cdev -I$(CODA)/common/include $<
E 4
I 4
D 5
	rtlink -cxx -D_CMLOG_BUILD_CLIENT -c $(DEBUG) -I. -I$(CLON_INC) -I$(CODA)/common/include/cmlog -I$(CODA)/common/include/cdev -I$(CODA)/common/include $<
E 5
I 5
D 6
	rtlink -cxx -D_CMLOG_BUILD_CLIENT -c $(DEBUG) -I. -I$(CLON_INC) -I$(CMLOG)/include -I$(CDEV)/include -I$(CODA)/common/include $<
E 6
I 6
D 13
	rtlink -cxx -D_CMLOG_BUILD_CLIENT -c $(DEBUG) -I$(CLON_INC) -I$(CMLOG)/include -I$(CDEV)/include -I$(CODA)/common/include $<
E 13
I 13
D 17
	rtlink -cxx -c $(DEBUG) -I$(CLON_INC) -I$(CODA)/common/include $<
E 17
I 17
D 18
	$(CC) -c $(DEBUG) -I$(CLON_INC) -I$(CODA)/common/include -I$(RTHOME)/include $<
E 18
I 18
	CC -c $(DEBUG) -I$(CLON_INC) -I$(CODA)/common/include -I$(RTHOME)/include $<
E 18
E 17
E 13
E 6
E 5
E 4
E 3
 
 
clean:
D 15
	rm -f *.o core
E 15
I 15
D 17
	rm -f *.o core $(PROG)
E 17
I 17
	rm -f *.o core alarm_server.cc $(PROG)
E 17
E 15


distclean:
D 17
	rm -f *.o core $(PROG)
E 17
I 17
	rm -f *.o core alarm_server.cc $(PROG)
E 17


I 19
install: exports
E 19
exports:
D 4
	cp $(PROG) $(CLAS_BIN)
E 4
I 4
D 14
	cp $(PROG) $(CLON_BIN)
E 14
I 14
D 16
	cp $(PROG) $(CLON_DEVBIN)
E 16
I 16
	cp $(PROG) $(CLON_BIN)
E 16
E 14
E 4

E 1
