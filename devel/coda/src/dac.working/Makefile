#
# Makefile for dac/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = dac


#------------------
# generic Makefile
#------------------
include $(CODA)/src/Makefile.include



# if want debugging mode, uncomment following lines
#ifeq ("$(OSTYPE)","SunOS")
#    CFLAGS = -g -KPIC -mt -DSunOS -DOSTYPE="SunOS"
#    LDFLAGS = -g -KPIC -mt
#endif

ifeq ("$(OSTYPE_MACHINE)","SunOS_i86pc")
    CFLAGS += -DSunOS_i86pc
endif
ifeq ("$(OSTYPE_MACHINE)","SunOS_sun4u")
    CFLAGS += -DSunOS_sun4u
endif



#-----------------------
# customize definitions
#-----------------------
CLON_FLAGS += -I./$(MAIN).s \
	-I$(CODA)/src/tcl7.4/tcl7.4.s \
	-I$(CODA)/src/itcl2.0/itcl2.0.s \
	-I$(CODA)/src/tklite/tklite.s \
	-I$(CODA)/common/include \
	-D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS \
	-DHAVE_GETCWD=1 -DHAVE_OPENDIR=1 -DHAVE_STRERROR=1 \
	-DHAVE_STRSTR=1 -DHAVE_STRTOL=1 -DHAVE_TMPNAM=1 \
	-DHAVE_WAITPID=1 -DHAVE_STRDUP=1 -DHAVE_STRERROR=1 \
	-DHAVE_STRDUP=1 -DHAVE_STRCASECMP=1 -DHAVE_UNISTD_H=1 \
	-DHAVE_ERRNO_H=1 -DHAVE_FCNTL_H=1 -DNO_STREAM_H=1 -DNO_UN_H=1 \
	-DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_TOLOWER=1 \
	-DSTDC_HEADERS=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 \
	-DHAVE_FLOAT_H=1 -DHAVE_MALLOC_H=1 -DHAVE_ERRNO_H=1 \
	-DHAVE_UNISTD_H=1 -DHAVE_MEMORY_H=1 -DHAVE_LIMITS_H=1 \
	-DHAVE_SYS_TIME_H=1 -DHAVE_SYS_WAIT_H=1 \
	-DTIME_WITH_SYS_TIME=1 -DNO_UNION_WAIT=1 -DNEED_MATHERR=1 \
	-DSVIPC_NO_UNION_SEMUN=1 -DTK_FILE_COUNT=_cnt \
		-D_REENTRANT \
		-D__EXTENSIONS__ \
		-DCODA_USER="\"${USER}\"" \
		-DVERSION="\"$(CODA_VERSION)\"" -D_LANGUAGE_C \
		-DDAYTIME="\"`date`\""
CLON_FLAGS += -DNO_X11

ifeq ("$(OSTYPE)","VXWORKS")
  LIBNAMES += ./VXWORKS_ppc/lib/libdac.so
  LIBNAMES += $(CODA)/VXWORKS_ppc/lib/libtklite.so
  LIBNAMES += $(CODA)/VXWORKS_ppc/lib/libitcl2.0.so
  LIBNAMES += $(CODA)/VXWORKS_ppc/lib/libmysql4.1.20.so
###  LIBNAMES += $(CODA)/VXWORKS_ppc/lib/libevio.so
  LIBNAMES += $(CODA)/VXWORKS_ppc/lib/libtcl7.4.so
else

#?????????? for TT_Go etc
###LIBNAMES += $(globallib)/libcodatt.a 
  LIBNAMES += $(CODA)/src/codatt/$(OSTYPE_MACHINE)/lib/libcodatt.a 

  LIBNAMES += $(globallib)/libbosio.a 
  LIBNAMES += $(globallib)/libtklite.a 
  LIBNAMES += $(globallib)/libcreg.a
  LIBNAMES += $(globallib)/libtcl7.4.a 
###  LIBNAMES += $(CODA)/src/tcl7.4/$(OSTYPE_MACHINE)/lib/libtcl7.4.a 
  LIBNAMES += $(globallib)/libitcl2.0.a  

#use system mysql, and then remaining calls from our mysql
  LIBNAMES += /usr/lib/mysql/libmysqlclient.a # system mysql
  LIBNAMES += /usr/lib/libz.so # compress/uncompress
  LIBNAMES += $(globallib)/libmysql4.1.20.a # the rest of mysql (our stuff)
  LIBNAMES += $(globallib)/libevio.a
  LIBNAMES += $(globallib)/libet.a
endif

#ifeq ("$(OSTYPE_MACHINE)","SunOS_sun4u")
#  LIBNAMES += /usr/sfw/lib/libgcc_s.so
#endif

ifeq ("$(OSTYPE)","SunOS")
  CLON_FLAGS += -DSOLARIS=1 -DOSTYPE="SunOS"
  LIBNAMES += -lpthread
  LIBNAMES += -lresolv
  LIBNAMES += $(X11LIBS)
  LIBNAMES += $(SYSLIBS)
endif

ifeq ("$(OSTYPE)","Linux")
  CLON_FLAGS += -DLINUX=1
  CLON_FLAGS += -D_GNU_SOURCE # added 2-may-2011 to satisfy 'tee'
  LIBNAMES += /usr/lib/libssl.so # for RHEL5 ..
  LIBNAMES += -lresolv
  LIBNAMES += $(X11LIBS)
  LIBNAMES += $(SYSLIBS)
  LIBNAMES += -lpthread
# Sergey: -rdynamic needed by Linux, it allows to search for global
#   symbols in main program by dlopen/dlsym
  LDFLAGS += -rdynamic
endif

ifeq ("$(OSTYPE)","VXWORKS")
#  CLON_FLAGS = -I./$(MAIN).s -I$(CODA)/common/include \
#		-DNO_X11 -DNEED_ACCESS -DNO_GETTOD -DNO_VALUES_H \
#		-DNO_SYS_ERRLIST -D__PROTOTYPE_5_0 -DTCL_VW
  CLON_FLAGS = -I./$(MAIN).s \
	-I$(CODA)/src/rol/inc \
	-I$(CODA)/common/include \
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
	cp $(MAIN).s/da.h tmp/
	cp $(MAIN).s/circbuf.h tmp/
	chmod 664 tmp/*
	cp tmp/* $(CODA)/common/include
	rm -rf tmp


tcl:
	echo "Converting 'tcl' into 'c'"
	$(CODA)/$(OSTYPE_MACHINE)/bin/tcl2c CODA_class.tcl > \
		main/cinclude/CODA_class.c
	$(CODA)/$(OSTYPE_MACHINE)/bin/tcl2c LINK_class.tcl > \
		main/cinclude/LINK_class.c
	$(CODA)/$(OSTYPE_MACHINE)/bin/tcl2c ER_class.tcl > \
		main/cinclude/ER_class.c
	$(CODA)/$(OSTYPE_MACHINE)/bin/tcl2c TS_class.tcl > \
		main/cinclude/TS_class.c
	$(CODA)/$(OSTYPE_MACHINE)/bin/tcl2c CDEB_class.tcl > \
		main/cinclude/CDEB_class.c
	$(CODA)/$(OSTYPE_MACHINE)/bin/tcl2c ROC_class.tcl > \
		main/cinclude/ROC_class.c
	$(CODA)/$(OSTYPE_MACHINE)/bin/tcl2c MON_class.tcl > \
		main/cinclude/MON_class.c
#
#
#



















