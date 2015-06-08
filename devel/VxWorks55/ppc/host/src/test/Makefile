# Makefile - for tests
#
# modification history
# --------------------
# 01n,03jan02,sbs  added bspVal to docs build.
# 01m,31may99,fle  added IWtx to build
# 01l,11jan99,fle  added wtxregd sub-directory
# 01k,31aug98,p_b  removed share
# 01j,15may98,p_b  added tgtsvr
# 01i,06dec96,jmp  added gdb to SUBDIRS list.
# 01h,27aug96,jmp  added share and wtxC to SUBDIRS list.
# 01g,17jul96,jmp  added vxwRegression to SUBDIRS list.
# 01f,23may96,jmp  added windsh to SUBDIRS list.
# 01e,08jun95,p_m  added documentation building support.
# 01d,22may95,jcf  name revision.
# 01c,17may95,pad  added EXETYPE=$(EXETYPE) to make command lines.
# 01b,14apr95,p_m  updated for new tree.
# 01a,10mar95,jcf  created.
#
# DESCRIPTION
# This file contains the makefile rules for building the tests
#
# SUBSTITUTION
#
# INCLUDES
#	$(WIND_BASE)/host/include/make/generic.mh
#	$(WIND_BASE)/host/include/make/$(HOST).mh
#	$(WIND_BASE)/host/include/make/generic2.mh
#*/

include 	$(WIND_BASE)/host/include/make/generic.mh
include		$(WIND_BASE)/host/include/make/$(HOST).mh

SUBDIRS		= wtxtcl windsh vxwRegression wtxC gdb tgtsvr wtxregd IWtx
DOCS		= doc

default:	$(SUBDIRS)

tgtsvr::
		(cd tgtsvr; $(MAKE) HOST=$(HOST) TOOLCHAIN=$(TOOLCHAIN) \
				EXETYPE=$(EXETYPE))
windsh::
		(cd windsh; $(MAKE) HOST=$(HOST) TOOLCHAIN=$(TOOLCHAIN) \
				EXETYPE=$(EXETYPE))
wtxtcl::
		(cd wtxtcl; $(MAKE) HOST=$(HOST) TOOLCHAIN=$(TOOLCHAIN) \
				EXETYPE=$(EXETYPE))
vxwRegression::
		(cd vxwRegression; $(MAKE) HOST=$(HOST) TOOLCHAIN=$(TOOLCHAIN) \
				EXETYPE=$(EXETYPE))
wtxC::
		(cd wtxC; $(MAKE) HOST=$(HOST) TOOLCHAIN=$(TOOLCHAIN) \
				EXETYPE=$(EXETYPE))
gdb::
		(cd gdb; $(MAKE) HOST=$(HOST) TOOLCHAIN=$(TOOLCHAIN) \
				EXETYPE=$(EXETYPE))

IWtx::
		(cd IWtx; $(MAKE) HOST=$(HOST) TOOLCHAIN=$(TOOLCHAIN) \
				EXETYPE=$(EXETYPE))

doc:		bspVal-docs

bspVal-docs::
		(cd bspVal; $(MAKE) HOST=$(HOST) \
                TOOLCHAIN=$(TOOLCHAIN) EXETYPE=$(EXETYPE) doc)

include 	$(WIND_BASE)/host/include/make/generic2.mh
