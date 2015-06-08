# Makefile - for vxColor demo doc.
#
# modification history
# --------------------
# 01a,31aug95,jco written from host/src/launch.
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
#*/

include         $(WIND_BASE)/host/include/make/generic.mh
include         $(WIND_BASE)/host/include/make/$(HOST).mh

SUBDIRS         = color
DOCS		= doc

default:        $(SUBDIRS)

color::
	(cd color; $(MAKE) HOST=$(HOST) TOOLCHAIN=$(TOOLCHAIN) \
			EXETYPE=$(EXETYPE))

doc:

include         $(WIND_BASE)/host/include/make/generic2.mh
