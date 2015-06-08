# Makefile - makefile for regenerating GNU linker scripts from common template
#
# modification history
# --------------------
# 01a,05dec01,tpw  written
#
# DESCRIPTION
# This file contains the makefile rules to regenerate cross-target linker
# scripts using the template system in tool/common/ldscripts.
#
#*/

TGT_DIR=$(WIND_BASE)/target
GEN_DIR=$(TGT_DIR)/h/tool/common/ldscripts
REGEN_SH=$(GEN_DIR)/regen.sh
TOOL=gnu

regen all:
	sh $(REGEN_SH) $(TOOL)

update:
	sh $(REGEN_SH) -update $(TOOL)
