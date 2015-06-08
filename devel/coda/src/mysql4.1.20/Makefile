#
# Makefile for mysql4.1.20/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = mysql4.1.20


#------------------
# generic Makefile
#------------------
include $(CODA)/src/Makefile.include


#-----------------------
# customize definitions
#-----------------------

#for debugging
#ifneq ("$(OSTYPE)","VXWORKS")
#  CFLAGS = -g -KPIC -mt -DSunOS -DOSTYPE="SunOS"
#endif

CLON_FLAGS += -I./src.s -I./src_full.s -I./src_short.s -DCODAMYSQL \
	-DSHAREDIR=\"$(CODA)/common/mysql\" \
	-DDEFAULT_CHARSET_HOME=\"$(CODA)/common/mysql\"


# compress/uncompress
ifneq ("$(OSTYPE)","VXWORKS")
  LIBNAMES += -lz
endif

LIBNAMES += $(SYSLIBS)


#-------------------
# customize targets
#-------------------
install: install_lib install_bin
	rm -rf tmp
	mkdir tmp
	cp browser/dbedit tmp
	chmod 775 tmp/*
	cp tmp/* $(CODA)/common/scripts
	rm -rf tmp
#
	rm -rf tmp
	mkdir tmp
	cp coda.s/libdb.h tmp
	cp coda.s/libtcp.h tmp
	chmod 664 tmp/*
	cp tmp/* $(CODA)/common/include
	rm -rf tmp
#
	-mkdir -p $(CODA)/common/include/mysql
	mkdir tmp
	cp src.s/*.h tmp
	chmod 664 tmp/*
	cp tmp/* $(CODA)/common/include/mysql
	rm -rf tmp














