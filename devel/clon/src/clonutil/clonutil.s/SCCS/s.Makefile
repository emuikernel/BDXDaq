h48200
s 00002/00002/00041
d D 1.33 03/12/18 12:22:37 boiarino 34 33
c use EPICS env var
e
s 00001/00001/00042
d D 1.32 00/11/20 13:11:36 wolin 33 32
c Added put_epics_ca
e
s 00001/00001/00042
d D 1.31 00/10/24 15:04:10 wolin 32 31
c Minor
e
s 00002/00003/00041
d D 1.30 00/10/24 14:16:13 wolin 31 30
c Typo
e
s 00005/00002/00039
d D 1.29 00/10/24 14:13:49 wolin 30 29
c Added get_epics functions
e
s 00007/00005/00034
d D 1.28 00/08/15 14:44:12 wolin 29 28
c Minor mods
e
s 00008/00004/00031
d D 1.27 00/07/25 15:01:43 wolin 28 27
c For release
e
s 00001/00001/00034
d D 1.26 00/07/25 12:44:58 wolin 27 26
c Do not need CLON_DEVxxx any more
e
s 00002/00002/00033
d D 1.25 00/07/10 13:09:11 wolin 26 25
c Mods for 5.0 compilers and ss55
e
s 00001/00001/00034
d D 1.24 00/01/12 14:55:32 wolin 25 24
c Added fputil
e
s 00002/00003/00033
d D 1.23 00/01/12 13:49:03 wolin 24 23
c Removed cmlog
e
s 00001/00001/00035
d D 1.22 00/01/05 16:17:41 wolin 23 22
c Release to devel area
e
s 00001/00001/00035
d D 1.21 99/06/01 15:43:34 wolin 22 21
c insert_msg now smartsockets version
c 
e
s 00001/00001/00035
d D 1.20 98/02/21 00:48:36 clasrun 21 20
c Added get_run_number_ (ejw)
c 
e
s 00001/00001/00035
d D 1.19 97/12/02 13:22:07 wolin 20 19
c Using common/include
c 
e
s 00003/00003/00033
d D 1.18 97/07/29 10:37:07 wolin 19 18
c Added fortran vsns of find_tag_line, get_next_line
c 
e
s 00001/00001/00035
d D 1.17 97/05/23 16:50:43 wolin 18 17
c Added get_next_line, find_tag_line
e
s 00001/00001/00035
d D 1.16 97/05/20 16:38:00 wolin 17 16
c Added get_run_config
e
s 00006/00002/00030
d D 1.15 97/05/20 10:35:21 wolin 16 15
c Uses new cmlog,cdev
e
s 00001/00001/00031
d D 1.14 97/05/19 15:01:23 wolin 15 14
c Added fork_and_wait
e
s 00002/00002/00030
d D 1.13 97/05/19 11:30:25 wolin 14 13
c Added get_run_number
e
s 00004/00006/00028
d D 1.12 97/05/07 10:10:44 wolin 13 12
c CLAS changed to CLON
e
s 00002/00002/00032
d D 1.11 97/04/08 12:21:04 wolin 12 11
c Added get_run_status
e
s 00001/00001/00033
d D 1.10 97/03/27 13:20:10 wolin 11 10
c Now using cmlog subdir only
e
s 00006/00002/00028
d D 1.9 97/03/25 09:55:29 wolin 10 9
c Added insert_msg
e
s 00001/00001/00029
d D 1.8 96/11/08 13:55:02 wolin 9 8
c Minor bug initializing char var structure
e
s 00001/00001/00029
d D 1.7 96/10/22 14:41:28 wolin 8 7
c Removed setc
e
s 00002/00002/00028
d D 1.6 96/10/22 11:50:30 wolin 7 6
c Added tcl_linkfvar, removed tcl_linkvar
e
s 00001/00001/00029
d D 1.5 96/10/21 15:08:40 wolin 6 5
c Added _ to fortran name
c 
e
s 00002/00001/00028
d D 1.4 96/10/16 16:32:11 wolin 5 4
c Added tcl_linkvar
e
s 00002/00002/00027
d D 1.3 96/10/14 15:01:54 wolin 4 3
c Added setc
e
s 00004/00001/00025
d D 1.2 96/09/26 17:37:02 wolin 3 1
c Added find_arg
e
s 00000/00000/00000
d R 1.2 96/09/11 09:54:04 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libutil/s/Makefile
c Name history : 1 0 s/Makefile
e
s 00026/00000/00000
d D 1.1 96/09/11 09:54:03 wolin 1 0
c Makes libutil library
e
u
U
f e 0
t
T
I 1
D 29
#
# builds the utility lib.
#
E 29
I 29

# builds the utility library

#  ejw, 15-aug-2000

E 29
D 13
#  this includes master.make from $(CLAS_ROOT)
#
E 13

D 13
include /usr/local/clas/master.make
E 13
I 13

E 13
D 31
DEBUG = -g
I 16
D 24
CMLOG = /usr/local/cmlog
E 24
D 19
CDEV  = /usr/local/cdev	
E 19
I 19
D 28
CDEV  = /usr/local/cdev
E 19

E 28
E 16
PROG = libutil.a
E 31
I 31
DEBUG      = -g
PROG       = libutil.a
E 31
I 30
D 34
EPICS_EXT  = /home/epics/R3.13.0.beta11/extensions/
EPICS_BASE = /home/epics/R3.13.0.beta11/base/
E 34
I 34
EPICS_EXT  = $(EPICS)/extensions/
EPICS_BASE = $(EPICS)/base/
E 34
E 30

I 16

E 16
all: $(PROG)

D 3
OBJS = getaddr.o strf.o
E 3
I 3
D 4
OBJS = getaddr.o strf.o find_arg.o
E 4
I 4
D 5
OBJS = getaddr.o strf.o find_arg.o setc.o
E 5
I 5
D 6
OBJS = getaddr.o strf.o find_arg.o setc.o tcl_linkvar.o
E 6
I 6
D 7
OBJS = getaddr.o strf.o find_arg.o setc.o tcl_linkvar_.o
E 7
I 7
D 8
OBJS = getaddr.o strf.o find_arg.o setc.o tcl_linkfvar_.o
E 8
I 8
D 10
OBJS = getaddr.o strf.o find_arg.o tcl_linkfvar_.o
E 10
I 10
D 12
OBJS = getaddr.o strf.o find_arg.o tcl_linkfvar_.o insert_msg.o insert_msg_.o
E 12
I 12
D 14
OBJS = getaddr.o strf.o find_arg.o tcl_linkfvar_.o insert_msg.o insert_msg_.o get_run_status.o
E 14
I 14
D 15
OBJS = getaddr.o strf.o find_arg.o tcl_linkfvar_.o insert_msg.o insert_msg_.o get_run_status.o get_run_number.o 
E 15
I 15
D 17
OBJS = getaddr.o strf.o find_arg.o tcl_linkfvar_.o insert_msg.o insert_msg_.o get_run_status.o get_run_number.o fork_and_wait.o
E 17
I 17
D 18
OBJS = getaddr.o strf.o find_arg.o tcl_linkfvar_.o insert_msg.o insert_msg_.o get_run_status.o get_run_number.o fork_and_wait.o get_run_config.o
E 18
I 18
D 19
OBJS = getaddr.o strf.o find_arg.o tcl_linkfvar_.o insert_msg.o insert_msg_.o get_run_status.o get_run_number.o fork_and_wait.o get_run_config.o get_next_line.o find_tag_line.o
E 19
I 19
D 21
OBJS = getaddr.o strf.o find_arg.o tcl_linkfvar_.o insert_msg.o insert_msg_.o get_run_status.o get_run_number.o fork_and_wait.o get_run_config.o get_next_line.o find_tag_line.o find_tag_line_.o get_next_line_.o
E 21
I 21
D 25
OBJS = getaddr.o strf.o find_arg.o tcl_linkfvar_.o insert_msg.o insert_msg_.o get_run_status.o get_run_number.o fork_and_wait.o get_run_config.o get_next_line.o find_tag_line.o find_tag_line_.o get_next_line_.o get_run_number_.o
E 25
I 25
D 30
OBJS = getaddr.o strf.o find_arg.o tcl_linkfvar_.o insert_msg.o insert_msg_.o get_run_status.o get_run_number.o fork_and_wait.o get_run_config.o get_next_line.o find_tag_line.o find_tag_line_.o get_next_line_.o get_run_number_.o fputil.o
E 30
I 30
D 32
OBJS = getaddr.o strf.o find_arg.o tcl_linkfvar_.o insert_msg.o insert_msg_.o get_run_status.o get_run_number.o fork_and_wait.o get_run_config.o get_next_line.o find_tag_line.o find_tag_line_.o get_next_line_.o get_run_number_.o fputil.o get_epics.o get_epics_ca.o
E 32
I 32
D 33
OBJS = get_epics.o get_epics_ca.o getaddr.o strf.o find_arg.o tcl_linkfvar_.o insert_msg.o insert_msg_.o get_run_status.o get_run_number.o fork_and_wait.o get_run_config.o get_next_line.o find_tag_line.o find_tag_line_.o get_next_line_.o get_run_number_.o fputil.o
E 33
I 33
OBJS = get_epics.o get_epics_ca.o put_epics_ca.o getaddr.o strf.o find_arg.o tcl_linkfvar_.o insert_msg.o insert_msg_.o get_run_status.o get_run_number.o fork_and_wait.o get_run_config.o get_next_line.o find_tag_line.o find_tag_line_.o get_next_line_.o get_run_number_.o fputil.o
E 33
E 32
E 30
E 25
E 21
E 19
E 18
E 17
E 15
E 14
E 12
E 10
E 8
E 7
E 6
E 5
E 4
E 3

I 5

E 5
$(PROG): $(OBJS)
	rm -f $(PROG)
	ar r $(PROG) $(OBJS)

I 10
.cc.o:
D 11
	CC -c $(DEBUG) -D_CMLOG_BUILD_CLIENT -I$(CLAS_INC) -I$(SMART_INC) -I$(CODA)/common/include -I$(CODA)/common/include/cdev $<
E 11
I 11
D 12
	CC -c $(DEBUG) -D_CMLOG_BUILD_CLIENT -I$(CODA)/common/include/cmlog $<
E 12
I 12
D 14
	CC -c $(DEBUG) -D_CMLOG_BUILD_CLIENT -I$(CODA)/common/include/cmlog -I$(CODA)/common/include/cdev $<
E 14
I 14
D 16
	CC -c $(DEBUG) -D_CMLOG_BUILD_CLIENT -I$(CODA)/common/include -I$(CODA)/common/include/dev -I$(CODA)/common/include/cmlog $<
E 16
I 16
D 22
	CC -c $(DEBUG) -D_CMLOG_BUILD_CLIENT -I$(CDEV)/include -I$(CMLOG)/include -I$(CODA)/common/include $<
E 22
I 22
D 24
	rtlink -cxx -c $(DEBUG) -D_CMLOG_BUILD_CLIENT -I$(CDEV)/include -I$(CMLOG)/include -I$(CODA)/common/include $<
E 24
I 24
D 26
	rtlink -cxx -c $(DEBUG) -I$(CDEV)/include -I$(CODA)/common/include $<
E 26
I 26
D 28
	CC -c $(DEBUG) -I$(CDEV)/include -I$(CODA)/common/include -I$(RTHOME)/include $<
E 28
I 28
D 29
	CC -c $(DEBUG) -I$(CDEVROOt)/include -I$(CODA)/common/include -I$(RTHOME)/include $<
E 29
I 29
	CC -c $(DEBUG) -I$(CDEVROOT)/include -I$(CODA)/common/include -I$(RTHOME)/include $<
E 29
E 28
E 26
E 24
E 22
E 16
E 14
E 12
E 11

E 10
.c.o:
D 4
	cc -c $(DEBUG) -I$(CLAS_INC) -I$(SMART_INC) $<
E 4
I 4
D 7
	cc -c $(DEBUG) -I$(CLAS_INC) -I$(SMART_INC) -I$(CODA)/SunOS/include $<
E 7
I 7
D 9
	cc -c $(DEBUG) -I$(CLAS_INC) -I$(SMART_INC) -I$(CODA)/SunOS/include -I$(CODA)/source/Tcl/src7.4/tcl7.4 $<
E 9
I 9
D 13
	cc -c $(DEBUG) -I$(CLAS_INC) -I$(SMART_INC) -I$(CODA)/SunOS/include -I$(CODA)/source/TclS/tcl7.4/ $<
E 13
I 13
D 16
	rtlink -c $(DEBUG) -I$(CLON_INC) -I$(CODA)/SunOS/include -I$(CODA)/source/TclS/tcl7.4/ $<
E 16
I 16
D 20
	rtlink -c $(DEBUG) -I$(CLON_INC)  -I$(CDEV)/include -I$(CMLOG)/include -I$(CODA)/SunOS/include -I$(CODA)/source/TclS/tcl7.4/ $<
E 20
I 20
D 24
	rtlink -c $(DEBUG) -I$(CLON_INC)  -I$(CDEV)/include -I$(CMLOG)/include -I$(CODA)/common/include -I$(CODA)/source/TclS/tcl7.4/ $<
E 24
I 24
D 26
	rtlink -c $(DEBUG) -I$(CLON_INC)  -I$(CDEV)/include -I$(CODA)/common/include -I$(CODA)/source/TclS/tcl7.4/ $<
E 26
I 26
D 28
	cc -c $(DEBUG) -I$(CLON_INC)  -I$(CDEV)/include -I$(CODA)/common/include -I$(CODA)/source/TclS/tcl7.4/ $<
E 28
I 28
D 29
	cc -c $(DEBUG) -I$(CLON_INC)  -I$(CDEVROOT)/include -I$(CODA)/common/include -I$(CODA)/source/TclS/tcl7.4/ $<
E 29
I 29
D 30
	cc -c $(DEBUG) -I$(CLON_INC)  -I$(CDEVROOT)/include -I$(CODA)/common/include -I$(CODA)/common/include  $<
E 30
I 30
D 31

E 31
	cc -c $(DEBUG) -I$(CLON_INC)  -I$(CDEVROOT)/include -I$(CODA)/common/include -I$(CODA)/common/include -I$(EPICS_BASE)/include -I$(EPICS_EXT)/include $<
E 30
E 29
E 28
E 26
E 24
E 20
E 16
E 13
E 9
E 7
E 4

I 3
.f.o:
D 13
	f77 -c $(DEBUG) -I$(CLAS_INC) -I$(SMART_INC) $<
E 13
I 13
D 19
	f77 -c $(DEBUG) -I$(CLON_INC) $<
E 19
I 19
	f77 -c -e $(DEBUG) -I$(CLON_INC) $<
E 19
E 13

I 10

E 10
E 3
clean:
I 28
	rm -f *.o


distclean:
E 28
	rm -f *.o $(PROG)

I 28

install: exports
E 28
exports:
D 10
	cp $(PROG) $(CLAS_LINK)
E 10
I 10
D 13
	cp $(PROG) $(CLAS_LIB)
E 13
I 13
D 23
	cp $(PROG) $(CLON_LIB)
E 23
I 23
D 27
	cp $(PROG) $(CLON_DEVLIB)
E 27
I 27
	cp $(PROG) $(CLON_LIB)
E 27
E 23
E 13
E 10
E 1
