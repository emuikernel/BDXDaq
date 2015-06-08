h23314
s 00003/00002/00026
d D 1.11 02/08/09 19:03:51 boiarino 14 13
c minor
c 
e
s 00002/00001/00026
d D 1.10 00/10/17 11:56:36 wolin 13 10
i 12
c Accepted child's version in workspace "/usr/local/clas/devel/source".
c 
e
s 00001/00001/00027
d D 1.8.1.2 00/10/17 11:56:09 wolin 12 11
c New name
e
s 00002/00001/00026
d D 1.8.1.1 00/10/06 10:10:04 wolin 11 9
c Protections
e
s 00001/00001/00026
d D 1.9 00/10/05 15:06:00 gurjyan 10 9
c change the name of pedman to pedman_action
c 
e
s 00001/00001/00026
d D 1.8 00/08/08 14:40:02 gurjyan 9 8
c 
e
s 00001/00001/00026
d D 1.7 00/08/07 16:13:21 gurjyan 8 7
c 
e
s 00001/00001/00026
d D 1.6 00/08/07 16:13:11 gurjyan 7 6
c 
e
s 00001/00001/00026
d D 1.5 00/08/07 16:13:00 gurjyan 6 5
c 
e
s 00006/00002/00021
d D 1.4 00/08/07 16:05:25 gurjyan 5 4
c 
e
s 00001/00001/00022
d D 1.3 00/08/07 16:02:01 gurjyan 4 3
c 
e
s 00007/00003/00016
d D 1.2 00/08/07 16:01:13 gurjyan 3 1
c 
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 pedman/Makefile
e
s 00019/00000/00000
d D 1.1 00/08/07 15:40:57 gurjyan 1 0
c 
e
u
U
f e 0
t
T
I 1

D 9
#  makefile for dchv
E 9
I 9
#  makefile for pedman
E 9
#  vhg, 7-aug-2000


all:
	cd spardb;    $(MAKE) all
I 3
	- cd s; $(MAKE) all
	- cd spardb/unix; $(MAKE) all
E 3
 
D 3
clean:
	cd spardb;    $(MAKE) clean
 
E 3
I 3
D 5
clean: distclean
E 3
distclean:
E 5
I 5
clean: 
E 5
	cd spardb;    $(MAKE) clean
I 3
D 4
	- cd s: $(MAKE) clean
E 4
I 4
	- cd s; $(MAKE) clean
E 4
D 6
	- cd spardb; $(MAKE) clean
E 6
I 6
	- cd spardb/unix; $(MAKE) clean
E 6
I 5

distclean:
	cd spardb;    $(MAKE) distclean
	- cd s; $(MAKE) distclean
D 7
	- cd spardb; $(MAKE) distclean
E 7
I 7
	- cd spardb/unix; $(MAKE) distclean
E 7
E 5
E 3
 
install: exports
exports:
D 14
	cd spardb;    $(MAKE) exports
D 10
D 12
	- cd gui; cp pedman $(CLON_BIN)
E 10
I 10
	- cd gui; cp pedman_action $(CLON_BIN)
E 10
E 12
I 12
	- cd gui; cp pedman_action $(CLON_BIN)
E 14
I 14
	cd spardb; $(MAKE) exports
	- cd gui; $(MAKE) exports
E 14
E 12
I 3
	- cd s; $(MAKE) exports
D 8
	- cd spardb; $(MAKE) exports
E 8
I 8
	- cd spardb/unix; $(MAKE) exports
I 14
	- cd spardb/scripts; $(MAKE) exports
E 14
E 8
E 3
D 11
D 13

E 13
I 13
	-chmod ug+rwx $(CLON_BIN)/*
	-chmod a+x $(CLON_BIN)/*
E 13
E 11
I 11
	-chmod ug+rwx $(CLON_BIN)/*
	-chmod a+x $(CLON_BIN)/*
E 11
E 1
