# Makefile - make rules for Lmbench 
# Done by Anish
# Copyright 2002 Wind River Systems, Inc.
#
# This file has been developed or significantly modified by Anish Gupta of
# Force 
# modification history
# --------------------
# Ported to Everest by Anish
#
# DESCRIPTION
# This file contains rules for building VxWorks for the
# BCM1250.
#
# INCLUDES
#     makeTarget
#*/

CPU             = PPC604
TOOL            = gnu

TGT_DIR=$(WIND_BASE)/target
include $(TGT_DIR)/h/make/defs.bsp
include $(TGT_DIR)/h/make/make.$(CPU)$(TOOL)
include $(TGT_DIR)/h/make/defs.$(WIND_HOST_TYPE)

ADDED_CFLAGS= -I../main -I../../
OBJS = bench.o bw_mem.o lat_mem_rd.o lib_timing.o frclmbench.o
## Only redefine make definitions below this point, or your definitions will
## be overwritten by the makefile stubs above.

#$(OBJS): Makefile bench.h lib_timing.h names.h stats.h
$(OBJS): Makefile bench.h  names.h stats.h

CFLAGS = -O $(CC_INCLUDE) $(CC_DEFINES) $(CC_WARNING) -I../main
exe: $(OBJS)
	$(LD) -r -o $(OUT) $(OBJS)

clean: 
	$(RM) -f $(OBJS) 
