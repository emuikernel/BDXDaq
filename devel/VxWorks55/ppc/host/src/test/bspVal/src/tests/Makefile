# Makefile - for VTS tests document generation
#
# Copyright 1999-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01f,12jul02,wsl  rename refgen book and change refgen output directory
# 01e,12mar02,wsl  change BSP_Developer's_Kit to VxWorks_Developer\'s_Kit, and
#                  BSP_VTS to BSP_Validation_Test_Suite
# 01d,29oct99,sbs  changed HTMLLINK_EXE with absolute path.
# 01c,29oct99,sbs  added missing error2 in doc files list.
# 01b,19oct99,sbs  changed REFGEN_EXE with absolute path.
# 01a,16oct99,sbs  written.
#
# DESCRIPTION
# This file contains the makefile rules for making the VTS man pages for
# all VTS tests.
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

DOCS		= doc

DOC_FILES_TESTS	= auxClock.tcl baudConsole.tcl bootline.tcl \
		  busTas.tcl eprom.tcl error1.tcl error2.tcl model.tcl \
		  network.tcl nvRam.tcl procNumGet.tcl ram.tcl \
		  rlogin.tcl scsi.tcl  serial.tcl sysClock.tcl \
		  timestamp.tcl


default:

doc:	test-doc

test-doc:	$(DOC_FILES_TESTS)
		$(WIND_BIN)/refgen $(REFGEN_TCL_OPT) \
		-book VxWorks_BSP_Developer\'s_Reference \
		-chapter VxWorks_BSP_Validation_Test_Suite \
		-out $(DOCS_ROOT)/bspkit/valTest \
		$(DOC_FILES_TESTS)
		$(WIND_BIN)/htmlLink $(DOCS_ROOT)/bspkit/valTest

include         $(WIND_BASE)/host/include/make/generic2.mh

