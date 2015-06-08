h41328
s 00003/00002/00020
d D 1.7 02/09/23 10:12:06 gurjyan 8 7
c *** empty log message ***
e
s 00002/00002/00020
d D 1.6 02/03/29 09:48:57 wolin 7 6
c Typo
e
s 00003/00002/00019
d D 1.5 02/03/29 09:48:24 wolin 6 5
c Added psd
e
s 00003/00002/00018
d D 1.4 02/03/27 15:03:11 wolin 5 4
c Added stadis_wc
e
s 00002/00001/00018
d D 1.3 00/10/06 10:07:51 wolin 4 3
c Protections
e
s 00001/00001/00018
d D 1.2 00/08/09 16:01:30 gurjyan 3 1
c 
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 stadis/Makefile
e
s 00019/00000/00000
d D 1.1 00/08/09 15:59:52 gurjyan 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1

#  makefile for stadis
#  vhg, 7-aug-2000


all:
	cd s;    $(MAKE) all
 
clean:
	cd s;    $(MAKE) clean
 
distclean:
	cd s;    $(MAKE) distclean
 
install: exports
exports:
D 3
	cd s;    $(MAKE) exports
E 3
I 3
	cd s;    $(MAKE) install
E 3
	- cd gui; cp stadis $(CLON_BIN)
D 4

E 4
I 4
D 5
	-chmod ug+rwx $(CLON_BIN)/stadis
	-chmod a+x $(CLON_BIN)/stadis
E 5
I 5
	- cd gui; cp stadis_wc $(CLON_BIN)
D 6
	-chmod ug+rwx $(CLON_BIN)/stadis*
	-chmod a+x $(CLON_BIN)/stadis*
E 6
I 6
	- cd gui; cp psd $(CLON_BIN)
D 7
	-chmod ug+rwx $(CLON_BIN)/stadis* psd
	-chmod a+x $(CLON_BIN)/stadis* psd
E 7
I 7
D 8
	-chmod ug+rwx $(CLON_BIN)/stadis* $(CLON_BIN)/psd
	-chmod a+x $(CLON_BIN)/stadis* $(CLON_BIN)/psd
E 8
I 8
	- cd gui; cp dctimeline $(CLON_BIN)
	-chmod ug+rwx $(CLON_BIN)/stadis* $(CLON_BIN)/psd $(CLON_BIN)/dctimeline
	-chmod a+x $(CLON_BIN)/stadis* $(CLON_BIN)/psd $(CLON_BIN)/dctimeline
E 8
E 7
E 6
E 5
E 4
E 1
