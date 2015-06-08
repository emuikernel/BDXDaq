# sh-arm.tcl - ARM-specific constants for host shell Tcl implementation 
#
# Copyright 1996-1997 Wind River Systems, Inc. 
#
# modification history
# -----------------------
# 01d,20jun97,cdp  ARM/Thumb common stuff moved to sh-armcore.tcl.
# 01c,17feb97,cdp  added mangledSymListGet.
# 01b,29jan97,elp  added underscoreStrip proc.
# 01a,28aug96,cdp  written.
#

# DESCRIPTION
# 
# This modules holds architecture dependant routines and globals
# 
# RESOURCE FILES
# wpwr/host/resource/tcl/dbgArmLib.tcl
# wpwr/host/resource/tcl/sh-armcore.tcl
#

source "[wtxPath host resource tcl]dbgArmLib.tcl"
source "[wtxPath host resource tcl]sh-armcore.tcl"
