h47355
s 00003/00001/00048
d D 1.8 02/10/11 15:00:44 wolin 9 8
c Separate rtlink and CC
e
s 00004/00001/00045
d D 1.7 02/10/10 15:12:41 wolin 8 7
c Added scaler_statistics_monitor
e
s 00002/00002/00044
d D 1.6 00/07/25 15:09:06 wolin 7 6
c For release
e
s 00001/00001/00045
d D 1.5 00/07/25 12:46:15 wolin 6 5
c Do not need CLON_DEVxxx any more
e
s 00004/00001/00042
d D 1.4 00/04/25 09:54:48 gurjyan 5 4
c add lt_estimate
c 
e
s 00001/00001/00042
d D 1.3 00/01/18 12:58:42 wolin 4 3
c Added -lresolv
e
s 00003/00006/00040
d D 1.2 00/01/05 16:51:38 wolin 3 1
c Many typos, release to devel area
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 scalers/s/Makefile
e
s 00046/00000/00000
d D 1.1 00/01/05 16:44:57 wolin 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1

#  makefile for scaler programs

#  ejw, 8-jul-97

D 7
CDEV  = /usr/local/cdev
E 7

DEBUG = -g

D 5
PROG = scaler_readout scaler_analysis scaler_file_analysis scaler_server
E 5
I 5
D 8
PROG = lt_estimate scaler_readout scaler_analysis scaler_file_analysis scaler_server
E 8
I 8
PROG = lt_estimate scaler_readout scaler_analysis scaler_file_analysis scaler_server scaler_statistics_monitor
E 8
E 5

all: $(PROG)
 

scaler_readout:  scaler_readout.cc
D 7
	rtlink -cxx $(DEBUG) -o $@ $@.cc -I$(CLON_INC) -I$(CODA)/common/include -L$(CLON_LIB) -lipc -lutil -L$(CDEV)/lib/solaris -lcdev -L$(CODA)/SunOS/bin -ltcl -ldpS
E 7
I 7
	rtlink -cxx $(DEBUG) -o $@ $@.cc -I$(CLON_INC) -I$(CODA)/common/include -L$(CLON_LIB) -lipc -lutil -L$(CDEVROOt)/lib/solaris -lcdev -L$(CODA)/SunOS/bin -ltcl -ldpS
E 7

 
scaler_analysis:  scaler_analysis.cc
D 3
	rtlink -cxx $(DEBUG) -o $@ scaler_analysis.cc -L$(CLON_LIB) -lipc -lutil -L$(CODA)/SunOS/bin -ltcl
E 3
I 3
	rtlink -cxx $(DEBUG) -o $@ scaler_analysis.cc -I$(CLON_INC) -I$(CODA)/common/include -L$(CLON_LIB) -lipc -lutil -L$(CODA)/SunOS/bin -ltcl
E 3


scaler_file_analysis:  scaler_file_analysis.cc
	rtlink -cxx $(DEBUG) -o $@ $@.cc  -L$(CLON_LIB) -lutil

I 9

E 9
I 8
scaler_statistics_monitor:  scaler_statistics_monitor.cc
D 9
	rtlink -cxx $(DEBUG) -o $@ $@.cc  -I$(CLON_INC) -I$(CODA)/common/include -L$(CODA)/SunOS/lib -let -ltcl -L$(CLON_LIB) -lbosio -lipc -lutil -lposix4
E 9
I 9
	CC $(DEBUG) -c $@.cc -I$(CLON_INC) -I$(CODA)/common/include -I$(RTHOME)/include
	rtlink -cxx $(DEBUG) -o $@ $@.o -L$(CODA)/SunOS/lib -let -ltcl -L$(CLON_LIB) -lbosio -lipc -lutil -lposix4
E 9

E 8
I 5
lt_estimate:  lt_estimate.cc
	rtlink -cxx $(DEBUG) -o $@ $@.cc  -L$(CLON_LIB) -lutil

E 5
 
D 3
scaler_server: scaler_server.o
	rtlink -cxx $(DEBUG) -o $@ $(OBJS) -L$(CLON_LIB) -lipc -lutil -L$(CODA)/SunOS/lib -let -ltcl -ldpS -lposix4
E 3
I 3
scaler_server: scaler_server.cc
D 4
	rtlink -cxx $(DEBUG) -o $@ $@.cc -I$(CLON_INC) -I$(CODA)/common/include -L$(CLON_LIB) -lipc -lutil -L$(CODA)/SunOS/lib -let -ltcl -ldpS -lposix4
E 4
I 4
	rtlink -cxx $(DEBUG) -o $@ $@.cc -I$(CLON_INC) -I$(CODA)/common/include -L$(CLON_LIB) -lipc -lutil -L$(CODA)/SunOS/lib -let -ltcl -ldpS -lposix4 -lresolv
E 4
E 3

D 3
 
.cc.o:
	rtlink -cxx -c $(DEBUG) -I$(CLON_INC) -I$(CODA)/common/include $<
E 3
 


clean:
	rm -f *.o core $(PROG)


distclean:
	rm -f *.o core $(PROG)


I 7
install: exports
E 7
exports:
D 6
	cp $(PROG) $(CLON_DEVBIN)
E 6
I 6
	cp $(PROG) $(CLON_BIN)
E 6

E 1
