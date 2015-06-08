#
# Makefile for clon/level1/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = level1

#------------------
# generic Makefile
#------------------
include $(CLON)/src/Makefile.include

#-----------------------
# customize definitions
#-----------------------
CLON_FLAGS += -I$(MAIN).s -I./tigris.s -I./hallbtriggergui.s \
			-I$(CODA)/common/include -I$(CLON)/common/include \
			-I/apps/Trolltech/Qt/include \
			-I/apps/Trolltech/Qt/include/QtGui \
			-D_POSIX_PTHREAD_SEMANTICS

ifeq ("$(OSTYPE)","VXWORKS")
  LIBNAMES +=
else
  LIBNAMES += $(QTLIBS)
  LIBNAMES += $(globallib)/libHv.a
  LIBNAMES += $(X11LIBS)
  LIBNAMES += $(SYSLIBS)
endif

moc:
	$(MOC) hallbtriggergui.s/mainwindow.h -o hallbtriggergui.s/moc_mainwindow.cc


#-------------------
# customize targets
#-------------------
install: install_bin
