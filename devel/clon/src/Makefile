#
# Makefile for clas/src/ directory
#

MAKE = gmake

all:
	-mkdir -p $(CLON)/common/include
	cd clonutil;                         $(MAKE)
	- cd clonutil;                       $(MAKE) java
	- cd clonutil;                       $(MAKE) install
	- cd minuit;                         $(MAKE)
	- cd minuit;                         $(MAKE) install
	- cd cmon;                           $(MAKE)
	- cd cmon;                           $(MAKE) install
	- cd Hv;                             $(MAKE)
	- cd Hv;                             $(MAKE) install
	- cd ced;                            $(MAKE)
	- cd ced;                            $(MAKE) install
	- cd ipc;                            $(MAKE)
	- cd ipc;                            $(MAKE) java
	- cd ipc;                            $(MAKE) install
	- cd alarm;                          $(MAKE)
	- cd alarm;                          $(MAKE) java
	- cd alarm;                          $(MAKE) install
	- cd timeline;                       $(MAKE)
	- cd timeline;                       $(MAKE) install
	- cd dbutil;                         $(MAKE)
	- cd dbutil;                         $(MAKE) java
	- cd dbutil;                         $(MAKE) install
	- cd epicsutil;                      $(MAKE)
	- cd epicsutil;                      $(MAKE) install
	- cd evmon;                          $(MAKE)
	- cd evmon;                          $(MAKE) install
	- cd infoserver;                     $(MAKE)
	- cd infoserver;                     $(MAKE) install
	- cd laser;                          $(MAKE)
	- cd laser;                          $(MAKE) install
	- cd level1;                         $(MAKE)
	- cd level1;                         $(MAKE) install
	- cd level2;                         $(MAKE)
#euphrates
	- cd level2;                         $(MAKE) java
#euphrates
	- cd level2;                         $(MAKE) install
	- cd pedman;                         $(MAKE)
	- cd pedman;                         $(MAKE) install
	- cd prtr;                           $(MAKE)
	- cd prtr;                           $(MAKE) install
	- cd cmroot;                         $(MAKE)
	- cd cmroot;                         $(MAKE) install
	- cd runlog;                         $(MAKE)
	- cd runlog;                         $(MAKE) install
	- cd scalers;                        $(MAKE)
	- cd scalers;                        $(MAKE) install
	- cd scripts;                        $(MAKE) install
	- cd stadis;                         $(MAKE) install
#
# sparc-only !!!!!!!!!!!!!!!!!!!!
#
###	- cd dchv;                           $(MAKE)
###	- cd dchv;                           $(MAKE) install

#ppc:
#	cd tcl7.4;                           $(MAKE) obj     VXWORKS
#	- cd tcl7.4;                         $(MAKE) lib     VXWORKS
#	- cd tcl7.4;                         $(MAKE) shlib   VXWORKS
#	- cd tcl7.4;                         $(MAKE) bin     VXWORKS
#	- cd tcl7.4;                         $(MAKE) install VXWORKS

#cleanppc:
#	cd tcl7.4;                           $(MAKE) clean   VXWORKS
#	- cd itcl2.0;                        $(MAKE) clean   VXWORKS

clean:
	cd clonutil;                         $(MAKE) clean
	- cd minuit;                         $(MAKE) clean
	- cd cmon;                           $(MAKE) clean
	- cd Hv;                             $(MAKE) clean
	- cd ced;                            $(MAKE) clean
	- cd ipc;                            $(MAKE) clean
	- cd alarm;                          $(MAKE) clean
	- cd timeline;                       $(MAKE) clean
	- cd dbutil;                         $(MAKE) clean
	- cd epicsutil;                      $(MAKE) clean
	- cd evmon;                          $(MAKE) clean
	- cd infoserver;                     $(MAKE) clean
	- cd laser;                          $(MAKE) clean
	- cd level1;                         $(MAKE) clean
	- cd level2;                         $(MAKE) clean
	- cd pedman;                         $(MAKE) clean
	- cd prtr;                           $(MAKE) clean
	- cd cmroot;                         $(MAKE) clean
	- cd runlog;                         $(MAKE) clean
	- cd scalers;                        $(MAKE) clean
	- cd scripts;                        $(MAKE) clean
	- cd stadis;                         $(MAKE) clean
#	rm -rf $(CLAS)/common/*
#	-mkdir -p $(CLAS)/common/include
##	rm -rf $(CLAS)/$(OSTYPE_MACHINE)/bin/*
##	rm -rf $(CLAS)/$(OSTYPE_MACHINE)/lib/*

###########################################



