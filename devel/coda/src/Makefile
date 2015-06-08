#
# Makefile for coda/src/ directory
#

MAKE = gmake

all:
	-mkdir -p $(CODA)/common/include
	cd images;                           $(MAKE) install
	- cd scripts;                        $(MAKE) install
	- cd rcscripts;                      $(MAKE) install
	- cd et;                             $(MAKE)
	- cd et;                             $(MAKE) install
	- cd evio;                           $(MAKE)
	- cd evio;                           $(MAKE) install
	- cd bosio;                          $(MAKE)
	- cd bosio;                          $(MAKE) install
	- cd tcl7.4;                         $(MAKE)
	- cd tcl7.4;                         $(MAKE) install
	- cd itcl2.0;                        $(MAKE)
	- cd itcl2.0;                        $(MAKE) install
	- cd mysql4.1.20;                    $(MAKE)
	- cd mysql4.1.20;                    $(MAKE) install
	- cd tclmysql1.0;                    $(MAKE)
	- cd tclmysql1.0;                    $(MAKE) install
	- cd creg;                           $(MAKE)
	- cd creg;                           $(MAKE) install
	- cd tklite;                         $(MAKE)
	- cd tklite;                         $(MAKE) install
	- cd codatcl1.0;                     $(MAKE)
	- cd codatcl1.0;                     $(MAKE) install
	- cd codaedit;                       $(MAKE)
	- cd codaedit;                       $(MAKE) install
	- cd codatt;                         $(MAKE)
	- cd codatt;                         $(MAKE) install
###	- cd rol;                            $(MAKE) uincs
	- cd rol;                            $(MAKE)
	- cd rol;                            $(MAKE) install
	- cd dac;                            $(MAKE)
	- cd dac;                            $(MAKE) install
	- cd camac;                          $(MAKE)
	- cd camac;                          $(MAKE) install
	- cd rc/cg;                          $(MAKE)
	- cd rc/sace3.1;                     $(MAKE)
	- cd rc/jpeg6.1;                     $(MAKE)
	- cd rc/XmHTML-1.1.4;                $(MAKE)
	- cd rc/libhtmlw-2.7;                $(MAKE)
	- cd rc/motif;                       $(MAKE)
	- cd rc/runControl/Xui;              $(MAKE)
	- cd rc/runControl/Xui;              $(MAKE) install
	- cd rc/runControl/rcServer;         $(MAKE)
	- cd rc/runControl/rcServer;         $(MAKE) install

ppc:
	cd tcl7.4;                           $(MAKE) obj     VXWORKS
	- cd tcl7.4;                         $(MAKE) lib     VXWORKS
	- cd tcl7.4;                         $(MAKE) shlib   VXWORKS
	- cd tcl7.4;                         $(MAKE) bin     VXWORKS
	- cd tcl7.4;                         $(MAKE) install VXWORKS
	- cd itcl2.0;                        $(MAKE) obj     VXWORKS
	- cd itcl2.0;                        $(MAKE) lib     VXWORKS
	- cd itcl2.0;                        $(MAKE) shlib   VXWORKS
	- cd itcl2.0;                        $(MAKE) bin     VXWORKS
	- cd itcl2.0;                        $(MAKE) install VXWORKS
	- cd mysql4.1.20;                    $(MAKE) obj     VXWORKS
	- cd mysql4.1.20;                    $(MAKE) lib     VXWORKS
	- cd mysql4.1.20;                    $(MAKE) shlib   VXWORKS
	- cd mysql4.1.20;                    $(MAKE) bin     VXWORKS
	- cd mysql4.1.20;                    $(MAKE) install VXWORKS
	- cd tclmysql1.0;                    $(MAKE) obj     VXWORKS
	- cd tclmysql1.0;                    $(MAKE) lib     VXWORKS
	- cd tclmysql1.0;                    $(MAKE) shlib   VXWORKS
	- cd tclmysql1.0;                    $(MAKE) bin     VXWORKS
	- cd tclmysql1.0;                    $(MAKE) install VXWORKS
	- cd tklite;                         $(MAKE) obj     VXWORKS
	- cd tklite;                         $(MAKE) lib     VXWORKS
	- cd tklite;                         $(MAKE) shlib   VXWORKS
###	- cd tklite;                         $(MAKE) bin     VXWORKS
	- cd tklite;                         $(MAKE) install VXWORKS
	- cd codatt;                         $(MAKE) obj     VXWORKS
	- cd codatt;                         $(MAKE) lib     VXWORKS
	- cd codatt;                         $(MAKE) shlib   VXWORKS
###	- cd codatt;                         $(MAKE) bin     VXWORKS
	- cd codatt;                         $(MAKE) install VXWORKS
	- cd rol;                            $(MAKE) obj     VXWORKS
	- cd rol;                            $(MAKE) lib     VXWORKS
	- cd rol;                            $(MAKE) shlib   VXWORKS
	- cd rol;                            $(MAKE) bin     VXWORKS
	- cd rol;                            $(MAKE) rols
	- cd rol;                            $(MAKE) install VXWORKS
	- cd rol/sfiDmaLib;                  $(MAKE)
	- cd rol/sfiDmaLib;                  $(MAKE) install
	- cd rol/vmeIntLib;                  $(MAKE)
	- cd rol/vmeIntLib;                  $(MAKE) install
	- cd dac;                            $(MAKE) obj     VXWORKS
	- cd dac;                            $(MAKE) lib     VXWORKS
	- cd dac;                            $(MAKE) shlib   VXWORKS
	- cd dac;                            $(MAKE) bin     VXWORKS
	- cd dac;                            $(MAKE) install VXWORKS
	- cd camac;                          $(MAKE) obj     VXWORKS
	- cd camac;                          $(MAKE) lib     VXWORKS
	- cd camac;                          $(MAKE) shlib   VXWORKS
	- cd camac;                          $(MAKE) bin     VXWORKS
	- cd camac;                          $(MAKE) install VXWORKS

cleanppc:
	cd tcl7.4;                           $(MAKE) clean   VXWORKS
	- cd itcl2.0;                        $(MAKE) clean   VXWORKS
	- cd mysql4.1.20;                    $(MAKE) clean   VXWORKS
	- cd tclmysql1.0;                    $(MAKE) clean   VXWORKS
	- cd tklite;                         $(MAKE) clean   VXWORKS
	- cd codatt;                         $(MAKE) clean   VXWORKS
	- cd rol;                            $(MAKE) clean   VXWORKS
	- cd dac;                            $(MAKE) clean   VXWORKS
	- cd camac;                          $(MAKE) clean   VXWORKS

clean:
	cd images;                           $(MAKE) clean
	- cd et;                             $(MAKE) clean
	- cd evio;                           $(MAKE) clean
	- cd bosio;                          $(MAKE) clean
	- cd tcl7.4;                         $(MAKE) clean
	- cd itcl2.0;                        $(MAKE) clean
	- cd mysql4.1.20;                    $(MAKE) clean
	- cd tclmysql1.0;                    $(MAKE) clean
	- cd creg;                           $(MAKE) clean
	- cd tklite;                         $(MAKE) clean
	- cd codatcl1.0;                     $(MAKE) clean
	- cd codaedit;                       $(MAKE) clean
	- cd codatt;                         $(MAKE) clean
	- cd rol;                            $(MAKE) clean
	- cd rol/sfiDmaLib;                  $(MAKE) clean
	- cd rol/vmeIntLib;                  $(MAKE) clean
	- cd dac;                            $(MAKE) clean
	- cd camac;                          $(MAKE) clean
	- cd rc/cg;                          $(MAKE) clean
	- cd rc/sace3.1;                     $(MAKE) clean
	- cd rc/jpeg6.1;                     $(MAKE) clean
	- cd rc/XmHTML-1.1.4;                $(MAKE) clean
	- cd rc/libhtmlw-2.7;                $(MAKE) clean
	- cd rc/motif;                       $(MAKE) clean
	- cd rc/runControl/Xui;              $(MAKE) clean
	- cd rc/runControl/rcServer;         $(MAKE) clean
###	rm -rf $(CODA)/common/*
###	-mkdir -p $(CODA)/common/include
	rm -rf $(CODA)/$(OSTYPE_MACHINE)/bin/*
	rm -rf $(CODA)/$(OSTYPE_MACHINE)/lib/*

###########################################



