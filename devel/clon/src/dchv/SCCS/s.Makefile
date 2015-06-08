h07322
s 00003/00001/00028
d D 1.9 03/12/01 14:35:32 gurjyan 13 12
c added genfile4tg
e
s 00001/00001/00028
d D 1.8 02/03/29 09:47:13 wolin 12 11
c Added dchv_mon install
e
s 00003/00000/00026
d D 1.7 00/10/06 10:42:59 wolin 11 10
c Protections
e
s 00006/00002/00020
d D 1.6 00/10/04 15:43:51 wolin 10 7
i 9
c Accepted child's version in workspace "/usr/local/clas/release/ejwtest/source".
c 
e
s 00000/00001/00026
d D 1.4.1.3 00/10/04 15:43:09 wolin 9 8
c Typo
e
s 00002/00002/00025
d D 1.4.1.2 00/10/04 15:42:43 wolin 8 6
c Minor update
e
s 00001/00001/00022
d D 1.5 00/10/04 10:36:05 gurjyan 7 5
c add Franz dchv version
c 
e
s 00004/00000/00023
d D 1.4.1.1 00/09/07 11:11:57 wolin 6 5
c New release
e
s 00001/00001/00022
d D 1.4 00/08/23 12:34:52 gurjyan 5 4
c 
e
s 00001/00001/00022
d D 1.3 00/08/23 12:25:19 gurjyan 4 3
c 
e
s 00001/00001/00022
d D 1.2 00/08/09 15:41:42 gurjyan 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 dchv/Makefile
e
s 00023/00000/00000
d D 1.1 00/08/07 11:53:47 gurjyan 1 0
c date and time created 00/08/07 11:53:47 by gurjyan
e
u
U
f e 0
t
T
I 1
D 8
D 10

E 10
E 8
#  makefile for dchv
D 8
D 10
#  vhg, 7-aug-2000
E 10
I 10
#  vhg,ejw 4-oct-2000
E 10
E 8
I 8
#  vhg,ejw 4-oct-2000
E 8

I 10
SHELL = /bin/sh
E 10
I 6
SHELL = /bin/sh
E 6

all:
	cd vme/s/unix;    $(MAKE) all
	-cd vme/s; $(MAKE) all
 
clean:
	cd vme/s/unix;    $(MAKE) clean
	-cd vme/s; $(MAKE) clean
 
distclean:
	cd vme/s/unix;    $(MAKE) clean
	cd vme/s; $(MAKE) distclean
 
install: exports
exports:
I 10
	if (test ! -d $(CLON_BIN)/dchv_util); then \
		mkdir $(CLON_BIN)/dchv_util; \
	fi
E 10
I 6
	if (test ! -d $(CLON_BIN)/dchv_util); then \
		mkdir $(CLON_BIN)/dchv_util; \
	fi
E 6
	cd vme/s/unix;    $(MAKE) exports
I 10
D 12
	- cd vme/dchv; cp DcHv $(CLON_BIN)/dchv; cp dchvn $(CLON_BIN); cp *.tcl *.h $(CLON_BIN)/dchv_util/;
E 12
I 12
D 13
	- cd vme/dchv; cp DcHv $(CLON_BIN)/dchv; cp dchvn $(CLON_BIN); cp dchv_mon $(CLON_BIN); cp *.tcl *.h $(CLON_BIN)/dchv_util/;
E 13
I 13
	- cd vme/dchv; cp DcHv $(CLON_BIN)/dchv; cp dchvn $(CLON_BIN); cp dchv_mon $(CLON_BIN); cp *.tcl *.h $(CLON_BIN)/dchv_util/ cp genfile4tg $(CLON_BIN);
E 13
E 12
E 10
I 8
	- cd vme/dchv; cp DcHv $(CLON_BIN)/dchv; cp dchvn $(CLON_BIN); cp *.tcl *.h $(CLON_BIN)/dchv_util/;
E 8
D 3
	- cd vme/dchv; cp DcHv $(CLON_BIN)
E 3
I 3
D 4
	- cd vme/dchv; cp DcHv $(CLON_BIN)/dchv
E 4
I 4
D 5
	- cd vme/dchv; cp DcHv $(CLON_BIN)/dchv; cp *.tcl $(CLON_BIN)/dchv_util/;
E 5
I 5
D 7
D 9
	- cd vme/dchv; cp DcHv $(CLON_BIN)/dchv; cp *.tcl *.h $(CLON_BIN)/dchv_util/;
E 7
I 7
	- cd vme/dchv; cp DcHv $(CLON_BIN)/dchv; cp dchvn $(CLON_BIN); cp *.tcl *.h $(CLON_BIN)/dchv_util/;
E 7
E 9
E 5
E 4
E 3
	-cd vme/s; $(MAKE) exports
I 11
	-chmod ug+rwx $(CLON_BIN)/dchv*
	-chmod a+x $(CLON_BIN)/dchv*
I 13
	-chmod ug+rwx $(CLON_BIN)/genfile4tg
	-chmod a+x $(CLON_BIN)/genfile4tg
E 13
	-chmod ug+rw $(CLON_BIN)/dchv_util/*
E 11

E 1
