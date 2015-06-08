# Makefile - for vxColor demo doc.
#
# modification history
# --------------------
# 01e,22sep98,fle  doc : changed doc files book belonging
# 01d,09sep98,fle  doc : changed refgen options since refgen.tcl
# 01c,03aug98,fle  doc : redirected output to DOCS_DIR
# 01b,27may98,fle  doc now uses refgen instead of mangen
# 01a,02aug95,jco copied from testSuite.
#
# DESCRIPTION
# This file contains the makefile rule for building the vxColor demo host side.
#
# SUBSTITUTION
#
# INCLUDES
#       $(WIND_BASE)/host/include/make/generic.mh
#       $(WIND_BASE)/host/include/make/$(HOST).mh
#       $(WIND_BASE)/host/include/make/generic2.mh
#

include         $(WIND_BASE)/host/include/make/generic.mh
include         $(WIND_BASE)/host/include/make/$(HOST).mh

VXCOLOR		= $(WIND_BIN)/vxColor

PROGRAMS        = $(VXCOLOR)
DOCS            = doc

default:        prog

$(VXCOLOR):	vxColor.sh
		$(CP) vxColor.sh $(VXCOLOR)
		$(CHMOD) $(CHMODFLAGS) $(VXCOLOR)

doc:		vxColor.sh 
		$(WIND_BIN)/refgen $(REFGEN_OPT) \
		    -out $(DOCS_ROOT)/tornado/tools \
		    -book Tornado_Reference \
		    -chapter Tornado_Tools vxColor.sh
		$(WIND_BIN)/htmlLink $(DOCS_ROOT)/tornado/tools

include         $(WIND_BASE)/host/include/make/generic2.mh
