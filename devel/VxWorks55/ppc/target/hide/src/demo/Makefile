# Makefile - makefile for building demos
#
# modification history
# --------------------
# 01e,02dec97,jmp  added synopsis rules to avoid top level synopsis build
#                  failure.
# 01d,20feb97,jank use $(MAKE) -C color intead of (cd color; ...)
# 01c,06nov96,yp   added TGT_DIR support, brought default to top
# 01b,04nov96,p_m  added man rule to avoid top leval man build to stop.
# 01a,18jun96,yp   created from 01c of MakeSkel
#
# DESCRIPTION
# This file contains the makefile rules for building the demo objects
#
#*/

TGT_DIR= $(WIND_BASE)/target
 
CPU=MC68020
TOOL=gnu

default:
	$(MAKE) -C color CPU=$(CPU) TOOL=$(TOOL) TGT_DIR=$(TGT_DIR)
 
rclean:
	$(MAKE) -C color CPU=$(CPU) TOOL=$(TOOL) TGT_DIR=$(TGT_DIR) clean
 
man:

synopsis:

clean_synopsis:

release : default
