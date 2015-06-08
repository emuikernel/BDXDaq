#
# Makefile for clon/cmroot/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = cmroot


#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include

include Make-depend


#-----------------------
# customize definitions
#-----------------------

#CLON_FLAGS += -I./cm.s

ifeq ("$(OSTYPE)","SunOS")
  CXXFLAGS = -O -KPIC -mt -I/usr/openwin/include
##CLON_FLAGS += R__SOLARIS
endif

CLON_FLAGS = -D_CMON \
				-I$(ROOTSYS)/include -I./cm.s -I./dvcs.src \
				-I$(CODA)/common/include -I$(CLON)/common/include

CLON_FLAGS += -I$(RTHOME)/include


#ROOTLIBS  = -L$(ROOTSYS)/lib -lBase -lCint -lClib -lCont -lFunc -lGraf \
#            -lGraf3d -lHist -lHtml -lMeta -lMinuit -lNet -lPostscript \
#            -lProof -lRint -lTree -lUnix -lZip -lEG -lMatrix -lNew
#ROOTGLIBS = -lGpad -lGX11 -lMotif -lWidgets -lX3d

# using script $ROOTSYS/bin/root-config to get the list of libraries needed;
# if need ROOT graphics, use flag '--glibs';
# (run 'root-config --help' to see all options)
LIBNAMES += $(shell root-config --glibs)

LIBNAMES += $(globallib)/libipc.a
LIBNAMES += $(globallib)/libclonutil.a
LIBNAMES += $(CODA_LIB)/libcodatt.a
LIBNAMES += $(CODA_LIB)/libbosio.a
LIBNAMES += $(CODA_LIB)/libet.a

LIBNAMES += -L$(RTHOME)/lib/$(OSTYPE_MACHINE) -lrtipc -lrtipcl -lrtutil -lrtcxxipc50

#solaris
LIBNAMES += -lresolv
LIBNAMES += -lrt

#-------------------
# customize targets
#-------------------

CMROOT_TMP1 = $(subst LinkDef.h,,$(NOTDIR_H_SOURCES))
CMROOT_TMP2 = $(subst CMCint.h,,$(CMROOT_TMP1))

#temporary
CMROOT_TMP2 = CMCalibMenu.h CMClient.h CMDaq.h CMDisplay.h CMEpics.h CMHbook.h \
	CMHist.h CMInputDialog.h CMMenu.h CMPolar.h CMRootServer.h CMSpars.h CMSparsMenu.h \
	CMSparsMenuCrate.h CMTestSetup.h CMTline.h CMViewer.h CMVirtualDisplay.h uthbook.h \
	uthbook_fun.h


clean:
	$(CLEAN)
	rm -f cm.s/uthbook.h
	rm -f cm.s/uthbook_fun.h
	ln -s $(CODA)/common/include/uthbook.h cm.s/uthbook.h
	ln -s $(CODA)/common/include/uthbook_fun.h cm.s/uthbook_fun.h
	rm -f cm.s/CMCint.h
	rm -f cm.s/CMCint.cc
	@echo "Generating dictionary ..."
	cd cm.s; rootcint -f CMCint.cc -c $(CMROOT_TMP2) LinkDef.h; cd ..
	rm -f Make-depend.bak
	cd cm.s; makedepend -f../Make-depend -- $(CLON_FLAGS) -- $(NOTDIR_CC_SOURCES); cd ..


install: install_lib install_bin
	rm -rf tmp
	mkdir tmp
	cp scripts/cmon.C tmp
	cp scripts/testsetup.C tmp
	chmod 664 tmp/*
	cp tmp/* $(ROOTSYS)/macros
	rm -rf tmp
	mkdir tmp
	cp cm.s/CMDaq.h tmp
	cp cm.s/CMHist.h tmp
	cp cm.s/CMClient.h tmp
	cp cm.s/CMHbook.h tmp
	cp cm.s/CMTline.h tmp
	cp cm.s/CMViewer.h tmp
	cp cm.s/CMMenu.h tmp
	cp cm.s/CMRootServer.h tmp
	cp cm.s/CMDisplay.h tmp
	cp cm.s/CMVirtualDisplay.h tmp
	chmod 664 tmp/*
	cp tmp/* $(CLON)/common/include
	rm -rf tmp





















