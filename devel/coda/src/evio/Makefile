#
# Makefile for evio/
#

#-----------------------------
# mandatory definitions: MAIN
#-----------------------------
MAIN = evio

#------------------
# generic Makefile
#------------------
include $(CODA)/src/Makefile.include

#-----------------------
# customize definitions
#-----------------------
CLON_FLAGS += -I./src.s 

LIBNAMES += -lpthread -lexpat -lz

install: install_lib
	-mkdir -p $(globalbin)
	cp $(localbin)/evio2xml $(globalbin)
	cp $(localbin)/evio2xml4.0 $(globalbin)
#
	rm -rf tmp
	mkdir tmp
	cp src.s/evio.h tmp/
	chmod 664 tmp/*
	-mkdir -p $(CODA)/common/include
	cp tmp/* $(CODA)/common/include
	rm -rf tmp

