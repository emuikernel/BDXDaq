#
# Makefile for codatt/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = codatt


#------------------
# generic Makefile
#------------------
include $(CODA)/src/Makefile.include

# if want debugging mode, uncomment following lines
#ifeq ("$(OSTYPE)","SunOS")
#    CFLAGS = -g -KPIC -mt -DSunOS -DOSTYPE="SunOS"
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
CLON_FLAGS += -I./$(MAIN).s -I$(CODA)/common/include \
	-D_REENTRANT -D_POSIX_PTHREAD_SEMANTICS -DHAVE_UNISTD_H=1 \
	-D__sun -DNO_X11 -DWITH_ET -D_INCtimeh \
	-D$(OSTYPE) -DOSTYPE=\"$(OSTYPE)\" -D_LANGUAGE_C \
	-DDAYTIME=\""`date`"\" -DCLON_PARMS=\"$(CLON_PARMS)\" \
	-DNO_UNION_WAIT=1 -DNEED_MATHERR=1 -Dvfork=fork -Dstrtod=fixstrtod \
	-DSTDC_HEADERS=1 \

LIBNAMES += $(globallib)/libbosio.a
LIBNAMES += -lresolv
LIBNAMES += $(SYSLIBS)

ifeq ("$(OSTYPE)","Linux")
  LIBNAMES += -lpthread
endif

#-------------------
# customize targets
#-------------------
install: install_lib

	rm -rf tmp
	mkdir tmp
	cp $(MAIN).s/tt.h tmp
	cp $(MAIN).s/coda.h tmp
	cp $(MAIN).s/ttbosio.h tmp
	cp $(MAIN).s/ttutils.h tmp
	cp $(MAIN).s/uthbook.h tmp
	cp $(MAIN).s/uthbook_fun.h tmp
	cp $(MAIN).s/sgutil.h tmp
	chmod 664 tmp/*
	cp tmp/* $(CODA)/common/include
	rm -rf tmp






























