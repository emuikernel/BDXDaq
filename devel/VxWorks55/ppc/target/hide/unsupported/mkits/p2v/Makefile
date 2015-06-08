# New Makefile Sirocco style 
#
# modification history
# --------------------
# 01a,11jan97,jmb  written.
# 
# This file defines the objects to build for p2vLib, the pSOS+(r)
# compatibility library.  These objects are added to the vxWorks runtime.
#
# SUBSTITUTION
# The following constant will be substituted:
#     <CPU>             cpu/architecture
#     <TOOL>            tool chain used for build
#
#*/

TGT_DIR=$(WIND_BASE)/target
LIBNAME		= lib$(CPU)$(TOOL)vx.a
LIBDIRNAME	= obj$(CPU)$(TOOL)vx

OBJS		= rn_Lib.o ev_Lib.o t_Lib.o sm_Lib.o q_Lib.o pt_Lib.o \
		  as_Lib.o tm_Lib.o p2vTblLib.o p2vShow.o

include $(TGT_DIR)/h/make/rules.library

