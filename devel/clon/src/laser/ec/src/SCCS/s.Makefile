h38070
s 00000/00000/00000
d R 1.2 00/09/21 14:10:47 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/Makefile
e
s 00024/00000/00000
d D 1.1 00/09/21 14:10:46 wolin 1 0
c date and time created 00/09/21 14:10:46 by wolin
e
u
U
f e 0
t
T
I 1

#  makefile
#  vhg, aug-2000


all:
	cd c_cntl;    $(MAKE) all
 
clean:
	cd c_cntl;    $(MAKE) clean
 
distclean:
	cd c_cntl;    $(MAKE) distclean

 
install: exports
exports:
	cd c_cntl;    $(MAKE) exports

check:
	cd c_cntl;    $(MAKE) check

 

E 1
