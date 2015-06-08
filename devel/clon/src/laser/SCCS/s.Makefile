h45552
s 00016/00016/00014
d D 1.3 03/12/16 16:08:54 sergpozd 4 3
c *** empty log message ***
e
s 00008/00000/00022
d D 1.2 00/10/06 01:40:35 clasrun 3 1
c add the make files for the javagui for both ec and sc
c 
e
s 00000/00000/00000
d R 1.2 00/09/21 14:20:41 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/Makefile
e
s 00022/00000/00000
d D 1.1 00/09/21 14:20:40 wolin 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1

#  main makefile for laser
#  ejw,klg 21-sep-2000


all:
D 4
	cd ec/src;    $(MAKE) all
	cd sc/src;    $(MAKE) all
I 3
	cd ec/gui;    $(MAKE) all
	cd sc/gui;    $(MAKE) all
E 4
I 4
	cd ec/src;      $(MAKE) all
	cd ec/gui;      $(MAKE) all
	cd sc/s;        $(MAKE) all
	cd sc/scripts;  $(MAKE) all
E 4
E 3
 
clean:
D 4
	cd ec/src;    $(MAKE) clean
	cd sc/src;    $(MAKE) clean
I 3
	cd ec/gui;    $(MAKE) clean
	cd sc/gui;    $(MAKE) clean
E 4
I 4
	cd ec/src;      $(MAKE) clean
	cd ec/gui;      $(MAKE) clean
	cd sc/s;        $(MAKE) clean
	cd sc/scripts;  $(MAKE) clean
E 4
E 3
 
distclean:
D 4
	cd ec/src;    $(MAKE) distclean
	cd sc/src;    $(MAKE) distclean
I 3
	cd ec/gui;    $(MAKE) distclean
	cd sc/gui;    $(MAKE) distclean
E 4
I 4
	cd ec/src;      $(MAKE) distclean
	cd ec/gui;      $(MAKE) distclean
	cd sc/s;        $(MAKE) distclean
	cd sc/scripts;  $(MAKE) distclean
E 4
E 3
 
install: exports
exports:
D 4
	cd ec/src;    $(MAKE) install
	cd sc/src;    $(MAKE) install
I 3
	cd ec/gui;    $(MAKE) install
	cd sc/gui;    $(MAKE) install
E 4
I 4
	cd ec/src;      $(MAKE) install
	cd ec/gui;      $(MAKE) install
	cd sc/s;        $(MAKE) install
	cd sc/scripts;  $(MAKE) install
E 4
E 3

E 1
