#
# Makefile for camac/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = camac


#------------------
# generic Makefile
#------------------
include $(CODA)/src/Makefile.include



#-----------------------
# customize definitions
#-----------------------
CLON_FLAGS += -I./$(MAIN).s -I$(CODA)/common/include \

ifeq ("$(OSTYPE)","VXWORKS")
  LIBNAMES +=
else
  LIBNAMES += $(globallib)/libmysql4.1.20.a
  LIBNAMES += -lrpcsvc
endif

ifeq ("$(OSTYPE)","SunOS")
  CLON_FLAGS += -DSOLARIS=1
  LIBNAMES += -lpthread
  LIBNAMES += -lresolv
#  LIBNAMES += $(X11LIBS)
  LIBNAMES += $(SYSLIBS)
endif

ifeq ("$(OSTYPE)","Linux")
  CLON_FLAGS += -DLINUX=1
  LIBNAMES += -lresolv
#  LIBNAMES += $(X11LIBS)
  LIBNAMES += $(SYSLIBS)
  LIBNAMES += -lpthread
# Sergey: -rdynamic needed by Linux, it allows to search for global
#   symbols in main program by dlopen/dlsym
  LDFLAGS += -rdynamic
endif







ifeq ("$(OSTYPE)","SunOS")
  CLON_FLAGS += -DSOLARIS=1
endif

ifeq ("$(OSTYPE)","Linux")
  CLON_FLAGS += -DLINUX=1
endif

ifeq ("$(OSTYPE)","VXWORKS")
  CLON_FLAGS = -I./$(MAIN).s -I$(CODA)/common/include \
	-DNO_X11 -D_GNU_TOOL -DTCL_VW -DCODA_USER="\"boiarino\"" \
	-DOSTYPE="VXWORKSPPC" -DVERSION="\"2.5\"" -DNO_GETWD=1 -DNO_WAIT3=1 \
	-DHAVE_UNISTD_H=1  -D_LANGUAGE_C -DDAYTIME="\"`date`\""
endif

#-------------------
# customize targets
#-------------------
install: install_lib
	cp $(localbin)/* $(CODA)/$(OSTYPE_MACHINE)/bin/

	rm -rf tmp
	mkdir tmp
	cp $(MAIN).s/ca.h tmp/
	chmod 664 tmp/*
	cp tmp/* $(CODA)/common/include
	rm -rf tmp

#
#
#



















