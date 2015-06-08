#
# Description:  Makefile for ROCheaders
#   This Makefile simply makes softlinks of the headers contained
#   within this directory to the LINUXVME_INC directory.
#
# SVN: $Rev: 396 $
#

ifndef LINUXVME_LIB
	LINUXVME_LIB	= $CODA/extensions/linuxvme/libs
endif
ifndef LINUXVME_INC
	LINUXVME_INC	= $CODA/extensions/linuxvme/include
endif

HEADERS=GEN_source.h rol.h trigger_dispatch.h gen_list.c

links: $(HEADERS)
	ln -sf ${PWD}/*.{c,h} $(LINUXVME_INC)

clean:
	rm *~