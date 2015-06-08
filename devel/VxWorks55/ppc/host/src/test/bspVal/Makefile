# Makefile - for VTS tests
#
# Copyright 1997-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01k,12jul02,wsl  change refgen output directory
# 01j,12jul02,wsl  rename refgen book and change refgen output directory
# 01i,12mar02,wsl  change BSP_Developer's_Kit to VxWorks_BSP_Developer\'s_Kit,
#                  and BSP_VTS to BSP_Validation_Test_Suite
# 01h,31oct01,sbs  removed generation of bspVal executable. 
# 01g,18sep01,sbs  merge from tor2_5.bsp_drv_facility branch for veloce
# 01f,29oct99,sbs  changed HTMLLINK_EXE with absolute path.
# 01e,19oct99,sbs  changed REFGEN_EXE with absolute path.
# 01d,16oct99,sbs  added changes for html documents generation.
# 01c,10sep97,db   removed bspVal.tcl from list of files that require
#                  man page generation.
# 01b,02sep97,db   added rules for man page generation
# 01a,21apr97,sbs  written.
#
# DESCRIPTION
# This file contains the makefile rules for making the VTS man pages and
# building the bspVal executable.
#
# SUBSTITUTION
#
# INCLUDES
#	$(WIND_BASE)/host/include/make/generic.mh
#	$(WIND_BASE)/host/include/make/$(HOST).mh
#	$(WIND_BASE)/host/include/make/generic2.mh
#*/

include         $(WIND_BASE)/host/include/make/generic.mh
include         $(WIND_BASE)/host/include/make/$(HOST).mh

WIND_BIN        = $(WIND_BASE)/host/$(WIND_HOST_TYPE)/bin

BSPVALIDATION	= $(WIND_BIN)/bspVal

DOCS		= doc

BSPVAL_DOC      = bspVal.sh

default:	

doc:	bspVal-doc tests-doc

bspVal-doc:	$(BSPVAL_DOC)
		$(WIND_BIN)/refgen $(REFGEN_OPT) \
		-book VxWorks_BSP_Developer\'s_Reference \
		-chapter VxWorks_BSP_Validation_Test_Suite \
		 -out $(DOCS_ROOT)/bspkit/valTest bspVal.sh
		$(WIND_BIN)/htmlLink $(DOCS_ROOT)/bspkit/valTest

tests-doc::
		(cd src/tests; $(MAKE) HOST=$(HOST) \
		TOOLCHAIN=$(TOOLCHAIN) EXETYPE=$(EXETYPE) doc)

include         $(WIND_BASE)/host/include/make/generic2.mh


