# Makefile - makefile skeleton for demo/vxColor
#
# Copyright 1984-1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01k,27nov98,p_m  Get rid of vxColor2.c since we don't need a buggy vxColor
#                  now that the whirlwind tour is gone
# 01j,20feb97,jank added subst / to \ using DIRCHAR in CP line for WIN32 builds
# 01i,12nov96,p_m  added TGT_DIR
# 01h,31oct96,yp   added clean and release rules
# 01g,15nov95,jcf  consolidated old and new versions of demo here.
# 01f,15nov95,jcf  changed name to vxColor.o.
# 01e,13nov95,p_m  used DEMO_DIR in order to build on Unix and WIN32.
# 01d,11nov95,p_m  changed mv to cp and added rm.
# 01c,11nov95,jeh  undoing last change.
# 01b,11nov95,jeh  changed 'mv' command to $(CP)
# 01a,21sep95,jco  written.
#	      +yp		
#
# DESCRIPTION
# This file contains rules for building the vxColor demo for the desired
# architecture.
#
# The file vxColor.c contains the actual demo, and builds its object into 
# the file named target/lib/.../vxColor.o.  The host portion of the demo, 
# vxColor, references this resultant object module.
#
# INCLUDES
#     makeTarget
#*/

TGT_DIR=$(WIND_BASE)/target

DOC_FILES =

LIBDIRNAME=obj$(CPU)$(TOOL)test

OBJS= vxColor.o

include $(TGT_DIR)/h/make/rules.library

CC_OPTIM = -g -fstrength-reduce -fno-builtin

# The normal end when using makeLibrary is to create a lib, but here, we
# don't need lib creation. The following settings prevent lib creation.

AR=echo
RANLIB=echo

