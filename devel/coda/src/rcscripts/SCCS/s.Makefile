h21033
s 00002/00005/00019
d D 1.8 00/10/30 14:02:36 wolin 22 21
c No chmod +x needed
e
s 00002/00002/00022
d D 1.7 00/10/30 11:38:31 wolin 21 20
c Minor
e
s 00006/00000/00018
d D 1.6 00/10/30 11:37:32 wolin 20 19
c Added distclean
e
s 00000/00002/00018
d D 1.5 00/10/06 10:20:24 wolin 19 18
c Protections
e
s 00003/00001/00017
d D 1.4 00/10/06 10:17:40 wolin 18 5
c Protections
e
s 00003/00019/00015
d D 1.2.1.2 00/09/06 14:04:16 wolin 17 16
c Minor mods
e
s 00020/00019/00014
d D 1.2.1.1 00/09/06 11:29:29 wolin 16 3
i 15
c Merged changes between child workspace "/usr/local/clas/release/ejwtest/source" and
c  parent workspace "/usr/local/clas/devel/source".
c 
e
s 00014/00008/00016
d D 1.1.1.8 00/09/06 11:23:03 wolin 15 14
c Still not working
e
s 00011/00008/00013
d D 1.1.1.7 00/01/26 13:57:38 gurjyan 14 13
c created new structure
e
s 00007/00005/00014
d D 1.1.1.6 00/01/17 12:05:02 gurjyan 13 12
c *** empty log message ***
e
s 00004/00000/00015
d D 1.1.1.5 99/03/10 18:11:02 gurjyan 12 11
c *** empty log message ***
e
s 00006/00002/00009
d D 1.1.1.4 98/11/10 10:16:24 wolin 11 10
c New scheme using prod, polar, minimal
c 
e
s 00000/00003/00011
d D 1.1.1.3 98/11/09 14:56:17 wolin 10 9
c Removed emp,pro,deb
c 
e
s 00007/00006/00007
d D 1.1.1.2 97/12/05 10:25:23 wolin 9 7
c No failure if scripts missing
c 
e
s 00000/00000/00000
d R 1.1.1.2 97/12/05 10:20:10 Codemgr 8 7
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 rcscripts/Makefile
e
s 00013/00000/00000
d D 1.1.1.1 97/12/05 10:20:09 wolin 7 1
c 
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:01 Codemgr 6 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 rcscripts/Makefile
c Conflict from ws : noforce /usr/local/clas/devel/source 1.2.1 1 1.2
e
s 00009/00010/00009
d D 1.3 00/10/04 16:34:20 wolin 5 3
c Updated for new release system
e
s 00000/00000/00000
d R 1.3 00/09/01 08:49:39 Codemgr 4 3
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 rcscripts/Makefile
e
s 00019/00000/00000
d D 1.2 00/09/01 08:49:38 gurjyan 3 1
c date and time created 00/09/01 08:49:38 by gurjyan
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:01 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 rcscripts/Makefile
e
s 00000/00000/00000
d D 1.1 70/01/01 00:00:00 Fake 1 0
c Fake CodeManager delta to combine two files with no common deltas
e
u
U
f e 0
t
T
I 16
#
#  copies rcscripts to $CLON_RCSCRIPTS directories
#

E 16
I 7
#
#  copies rcscripts to $CLON_RCSCRIPTS directories
#

I 15
SHELL = /bin/sh
I 16
all: 
clean:
E 16

I 16
exports: install
install:

all: exports
exports:
D 17
	cp run_dispatch       $(CLON_RCSCRIPTS); ./expr
E 16
E 15
all: exports

exports:
D 9
	cp run_dispatch     $CLON_RCSCRIPTS
	cp rcdev/*.csh      $CLON_RCSCRIPTS/rcdev
	cp rcdeb/*.csh      $CLON_RCSCRIPTS/rcdeb
	cp rcpro/*.csh      $CLON_RCSCRIPTS/rcpro
	cp rcemp/*.csh      $CLON_RCSCRIPTS/rcemp
	cp rcmin/*.csh      $CLON_RCSCRIPTS/rcmin
E 9
I 9
D 14
	cp run_dispatch     $(CLON_RCSCRIPTS)
D 11
	-cp rcdev/*.csh      $(CLON_RCSCRIPTS)/rcdev
D 10
	-cp rcdeb/*.csh      $(CLON_RCSCRIPTS)/rcdeb
	-cp rcpro/*.csh      $(CLON_RCSCRIPTS)/rcpro
	-cp rcemp/*.csh      $(CLON_RCSCRIPTS)/rcemp
E 10
	-cp rcmin/*.csh      $(CLON_RCSCRIPTS)/rcmin
E 11
I 11
D 13
	-cp prod/*.csh      $(CLON_RCSCRIPTS)/prod
E 13
	-cp polar/*.csh     $(CLON_RCSCRIPTS)/polar
D 13
	-cp minimal/*.csh   $(CLON_RCSCRIPTS)/minimal
I 12
	-cp level1/*.csh   $(CLON_RCSCRIPTS)/level1
	-cp level2/*.csh   $(CLON_RCSCRIPTS)/level2
	-cp calib/*.csh   $(CLON_RCSCRIPTS)/calib
E 13
I 13
	-cp laser/*.csh     $(CLON_RCSCRIPTS)/laser
	-cp g6_prod/*.csh   $(CLON_RCSCRIPTS)/g6_prod
	-cp g6_norm/*.csh   $(CLON_RCSCRIPTS)/g6_norm
	-cp level1/*.csh    $(CLON_RCSCRIPTS)/level1
	-cp level2/*.csh    $(CLON_RCSCRIPTS)/level2
	-cp calib/*.csh     $(CLON_RCSCRIPTS)/calib
E 14
I 14
	cp run_dispatch       $(CLON_RCSCRIPTS)
I 15
	if (test ! -d $(CLON_RCSCRIPTS)/polar); then mkdir $(CLON_RCSCRIPTS)/polar; fi
I 16
	-cp polar/*.csh       $(CLON_RCSCRIPTS)/polar
E 16
E 15
	-cp polar/*.csh       $(CLON_RCSCRIPTS)/polar
I 15
	if (test ! -d $(CLON_RCSCRIPTS)/ec_laser); then mkdir $(CLON_RCSCRIPTS)/ec_laser; fi
I 16
	-cp ec_laser/*.csh    $(CLON_RCSCRIPTS)/ec_laser
E 16
E 15
	-cp ec_laser/*.csh    $(CLON_RCSCRIPTS)/ec_laser
I 15
	if (test ! -d $(CLON_RCSCRIPTS)/tof_laser); then mkdir $(CLON_RCSCRIPTS)/tof_laser; fi
I 16
	-cp tof_laser/*.csh   $(CLON_RCSCRIPTS)/tof_laser
E 16
E 15
	-cp tof_laser/*.csh   $(CLON_RCSCRIPTS)/tof_laser
I 15
	if (test ! -d $(CLON_RCSCRIPTS)/calib); then mkdir $(CLON_RCSCRIPTS)/calib; fi
I 16
	-cp calib/*.csh       $(CLON_RCSCRIPTS)/calib
E 16
E 15
	-cp calib/*.csh       $(CLON_RCSCRIPTS)/calib
I 15
	if (test ! -d $(CLON_RCSCRIPTS)/cc_calib); then mkdir $(CLON_RCSCRIPTS)/cc_calib; fi
I 16
	-cp cc_calib/*.csh    $(CLON_RCSCRIPTS)/cc_calib
E 16
E 15
	-cp cc_calib/*.csh    $(CLON_RCSCRIPTS)/cc_calib
I 15
	if (test ! -d $(CLON_RCSCRIPTS)/ec_calib); then mkdir $(CLON_RCSCRIPTS)/ec_calib; fi
	-cp ec_calib/*.csh    $(CLON_RCSCRIPTS)/ec_calib
	if (test ! -d $(CLON_RCSCRIPTS)/cosmic_all); then mkdir $(CLON_RCSCRIPTS)/cosmic_all; fi
I 16
	-cp cosmic_all/*.csh  $(CLON_RCSCRIPTS)/cosmic_all
E 16
E 15
	-cp cosmic_all/*.csh  $(CLON_RCSCRIPTS)/cosmic_all
I 15
	if (test ! -d $(CLON_RCSCRIPTS)/e1c_ec); then mkdir $(CLON_RCSCRIPTS)/e1c_ec; fi
I 16
	-cp e1c_ec/*.csh      $(CLON_RCSCRIPTS)/e1c_ec
E 16
E 15
	-cp e1c_ec/*.csh      $(CLON_RCSCRIPTS)/e1c_ec
I 15
	if (test ! -d $(CLON_RCSCRIPTS)/e1c_eccc); then mkdir $(CLON_RCSCRIPTS)/e1c_eccc; fi
I 16
	-cp e1c_eccc/*.csh    $(CLON_RCSCRIPTS)/e1c_eccc
E 17
I 17
	cp run_dispatch       $(CLON_RCSCRIPTS);
	-chmod +x expr
	./expr
E 17
E 16
E 15
	-cp e1c_eccc/*.csh    $(CLON_RCSCRIPTS)/e1c_eccc
E 14
E 13
E 12
D 15


I 14


E 14
I 13

E 13
I 12

E 12

E 11

E 15
I 15
	-chmod -R ug+rwx      $(CLON_RCSCRIPTS)
	-chmod -R +rx         $(CLON_RCSCRIPTS)
E 15
E 9
E 7
I 3
D 16
#
#  copies rcscripts to $CLON_RCSCRIPTS directories
#

I 5
SHELL = /bin/sh
I 20

E 20
E 5
all: 
I 22
	@echo Nothing to do for all in rcsripts
E 22
I 20

E 20
clean:
I 20
D 21
	echo Nothing to clean in rcsripts
E 21
I 21
	@echo Nothing to clean in rcsripts
E 21

distclean:
D 21
	echo Nothing to distclean in rcsripts
E 21
I 21
	@echo Nothing to distclean in rcsripts
E 21
E 20
I 5

E 5
exports: install
install:
D 5
	cp run_dispatch       $(CLON_RCSCRIPTS); ./expr









E 5
D 22

I 5
all: exports
exports:
E 22
D 18
	cp run_dispatch       $(CLON_RCSCRIPTS);
E 18
I 18
	cp run_dispatch       $(CLON_RCSCRIPTS)
	-chmod ug+rwx         $(CLON_RCSCRIPTS)/run_dispatch
	-chmod a+x            $(CLON_RCSCRIPTS)/run_dispatch
E 18
D 22
	-chmod +x expr
	./expr
E 22
I 22
	/bin/csh -s < expr
E 22
D 19
	-chmod -R ug+rwx      $(CLON_RCSCRIPTS)
	-chmod -R +rx         $(CLON_RCSCRIPTS)
E 19
E 5
E 16
E 3
