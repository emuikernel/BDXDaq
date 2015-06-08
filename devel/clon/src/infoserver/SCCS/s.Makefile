h65318
s 00001/00000/00026
d D 1.3 02/04/05 16:16:04 wolin 4 3
c Added headers
e
s 00006/00002/00020
d D 1.2 00/09/06 10:40:18 wolin 3 1
c Added includes
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 info_server/Makefile
e
s 00022/00000/00000
d D 1.1 00/08/03 10:37:55 wolin 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1

# makefile for info server
#  ejw, 3-aug-2000


all:
D 3
	cd server; $(MAKE) all
	cd misc;   $(MAKE) all
E 3
I 3
	cd includes; $(MAKE) all
	cd server;   $(MAKE) all
	cd misc;     $(MAKE) all
E 3
 
clean:
I 3
	cd includes; $(MAKE) clean
E 3
	cd server; $(MAKE) clean
	cd misc;   $(MAKE) clean
 
distclean:
I 3
	cd includes; $(MAKE) distclean
E 3
	cd server; $(MAKE) distclean
	cd misc;   $(MAKE) distclean
 
install: exports
exports:
I 3
	cd includes; $(MAKE) install
E 3
	cd server; $(MAKE) install
	cd misc;   $(MAKE) install

I 4
headers:
E 4
E 1
