# Makefile - makefile for demo/1
#
# Copyright 1984-1995 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,11nov95,p_m  written.
#
# DESCRIPTION
# This file contains rules for building a simple VxWorks demo (demo.o) 
# for the desired architecture and then moving the object in its final 
# location.
#
# It also contains rules to build the client side (running on VxWorks) of
# a simple client/server application.
#
# To build VxWorks objects for a given target do:
#
# % make CPU=<CPU_TYPE>
#
# Eg: to build the programs for a MC68060 target use:
#
# % make  CPU=MC68060
#
# INCLUDES
#     makeTarget
#*/

CPU             = MC68020
TOOL            = gnu

include $(WIND_BASE)/target/h/make/defs.bsp
include $(WIND_BASE)/target/h/make/make.$(CPU)$(TOOL)
include $(WIND_BASE)/target/h/make/defs.$(WIND_HOST_TYPE)

default		: $(DEMO_DIR)

$(DEMO_DIR)     : .neverbuilt
		$(CC) $(CFLAGS) -c demo.c
		$(CP) demo.o $@
		$(CC) $(CFLAGS) -c client.c
		$(CP) client.o $@

exe             :

.neverbuilt     :

