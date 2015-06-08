h07890
s 00002/00002/00036
d D 1.10 00/10/05 12:57:05 wolin 11 10
c Minor
e
s 00010/00009/00028
d D 1.9 00/07/25 15:03:19 wolin 10 9
c For release
e
s 00001/00001/00036
d D 1.8 00/01/12 15:38:12 wolin 9 8
c Using DEVBIN
e
s 00003/00003/00034
d D 1.7 99/01/26 10:49:03 wolin 8 7
c Added posix4
c 
e
s 00005/00001/00032
d D 1.6 97/11/25 13:37:27 wolin 7 6
c Added run_number
c 
e
s 00001/00001/00032
d D 1.5 97/06/23 13:39:08 wolin 6 5
c run_status also prints run and config
e
s 00007/00004/00026
d D 1.4 97/05/20 17:07:13 wolin 5 4
c Added run_config
e
s 00005/00005/00025
d D 1.3 97/05/20 10:35:41 wolin 4 3
c Uses new cmlog,cdev; other minor mods
e
s 00004/00005/00026
d D 1.2 97/05/07 10:11:56 wolin 3 1
c CLAS changed to CLON
e
s 00000/00000/00000
d R 1.2 97/04/09 09:45:26 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 libutil/test/Makefile
e
s 00031/00000/00000
d D 1.1 97/04/09 09:45:25 wolin 1 0
c Links test routines
e
u
U
f e 0
t
T
I 1
#
D 3
# builds test routined
E 3
I 3
# builds test routines
E 3
#

D 3
include /usr/local/clas/master.make
E 3
DEBUG = -g
I 4
D 10
CMLOG = /usr/local/cmlog
CDEV  = /usr/local/cdev	
E 10
E 4
PROG = run_status
D 10

all: $(PROG)

E 10
D 5
OBJS = run_status.o
PROG = run_status
E 5
I 5
D 7
PROG = run_status run_config
E 7
I 7
PROG = run_status run_config run_number
E 7
E 5

I 10

E 10
all: $(PROG)
D 10
 
E 10
I 10

E 10
I 4

E 4
D 5
$(PROG): $(OBJS)
D 3
	CC $(DEBUG) -o $@ $(OBJS) -L$(CLAS_LIB) -lutil -L$(CODA)/SunOS/lib -lcdev -lsocket -lnsl
E 3
I 3
D 4
	CC $(DEBUG) -o $@ $(OBJS) -L$(CLON_LIB) -lutil -L$(CODA)/SunOS/lib -lcdev -lsocket -lnsl
E 4
I 4
	CC $(DEBUG) -o $@ $(OBJS) -L$(CLON_LIB) -lutil -L$(CDEV)/lib -lcdev -L$(CMLOG)/lib -lcmlog -lsocket -lnsl
E 5
I 5
run_status: run_status.o
D 6
	CC $(DEBUG) -o run_status run_status.o -L$(CLON_LIB) -lutil -L$(CDEV)/lib -lcdev -L$(CMLOG)/lib -lcmlog -lsocket -lnsl
E 6
I 6
D 8
	CC $(DEBUG) -o run_status run_status.o -L$(CLON_LIB) -lutil -L$(CDEV)/lib -lcdev -L$(CMLOG)/lib -lcmlog -L$(CODA)/SunOS/lib -lmsql -lsocket -lnsl
E 8
I 8
D 10
	CC $(DEBUG) -o run_status run_status.o -L$(CLON_LIB) -lutil -L$(CDEV)/lib -lcdev -L$(CODA)/SunOS/lib -lmsql -lsocket -lnsl -lposix4
E 10
I 10
	CC $(DEBUG) -o run_status run_status.o ../s/get_run_status.o ../s/get_run_config.o -L$(CDEVROOT)/lib -lcdev -L$(CODA)/SunOS/lib -lmsql -lsocket -lnsl -lposix4
E 10
E 8
E 6
E 5
E 4
E 3


I 5
run_config: run_config.o
D 8
	CC $(DEBUG) -o run_config run_config.o -L$(CLON_LIB) -lutil -L$(CODA)/SunOS/lib -lmsql -lsocket -lnsl
E 8
I 8
D 11
	CC $(DEBUG) -o run_config run_config.o -L$(CLON_LIB) -lutil -L$(CODA)/SunOS/lib -lmsql -lsocket -lnsl  -lposix4
E 11
I 11
	CC $(DEBUG) -o run_config run_config.o -L../s -lutil -L$(CODA)/SunOS/lib -lmsql -lsocket -lnsl  -lposix4
E 11
E 8


I 7
run_number: run_number.o
D 8
	CC $(DEBUG) -o run_number run_number.o -L$(CLON_LIB) -lutil -L$(CODA)/SunOS/lib -lmsql -lsocket -lnsl
E 8
I 8
D 11
	CC $(DEBUG) -o run_number run_number.o -L$(CLON_LIB) -lutil -L$(CODA)/SunOS/lib -lmsql -lsocket -lnsl  -lposix4
E 11
I 11
	CC $(DEBUG) -o run_number run_number.o -L../s -lutil -L$(CODA)/SunOS/lib -lmsql -lsocket -lnsl  -lposix4
E 11
E 8


E 7
E 5
.cc.o:
D 4
	CC -c $(DEBUG) -D_CMLOG_BUILD_CLIENT -I$(CODA)/common/include/cmlog -I$(CODA)/common/include/cdev $<
E 4
I 4
D 10
	CC -c $(DEBUG) -D_CMLOG_BUILD_CLIENT $<
E 10
I 10
	CC -c $(DEBUG) $<
E 10
E 4

D 4
.c.o:
D 3
	cc -c $(DEBUG) -I$(CLAS_INC) -I$(SMART_INC) -I$(CODA)/SunOS/include -I$(CODA)/source/TclS/tcl7.4/ $<
E 3
I 3
	rtlink -c $(DEBUG) -I$(CLON_INC) -I$(CODA)/SunOS/include -I$(CODA)/source/TclS/tcl7.4/ $<
E 4
E 3

D 4

E 4
clean:
I 10
	rm -f *.o

distclean:
E 10
	rm -f *.o $(PROG)

I 10

install: exports
E 10
exports:
D 3
	cp $(PROG) $(CLAS_BIN)
E 3
I 3
D 9
	cp $(PROG) $(CLON_BIN)
E 9
I 9
D 10
	cp $(PROG) $(CLON_DEVBIN)
E 10
I 10
	cp $(PROG) $(CLON_BIN)
E 10
E 9
E 3
E 1
