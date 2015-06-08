# Makefile - for datagram demo.
#
# modification history
# --------------------
# 01b,13nov95,p_m  adapted to Tornado.
# 01a,12feb93,jcf  written.
#
# DESCRIPTION
# This file contains the makefile rules for building the host and target
# side of the VxWorks demo dg.
#
# SUBSTITUTION
# The following constant will be substitued:
#    CPU              cpu/architecture
#    TOOL             tool chain used for build
#    WIND_HOST_TYPE   host type; controls building of host-specific tools
#*/


CPU		= MC68020
TOOL		= gnu

include $(WIND_BASE)/target/h/make/defs.bsp
include $(WIND_BASE)/target/h/make/make.$(CPU)$(TOOL)
include $(WIND_BASE)/target/h/make/defs.$(WIND_HOST_TYPE)

default		: $(DEMO_DIR)

$(DEMO_DIR)	: .neverbuilt
		$(CC) $(CFLAGS) -c dgTest.c
		$(CP) dgTest.o $@

exe		:

.neverbuilt	:

