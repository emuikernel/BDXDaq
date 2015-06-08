# Project.unix.tcl - entry point for the UNIX version of the Project Tool
#
# modification history
# --------------------
# 02a,17sep98,cjs  unified with PC version 
# 01b,20aug98,jmp  added windHelpLib.tcl sourcing for the on-line help.
# 01a,30jul98,aam  written.
#
# DESCRIPTION
#
#############################################################

namespace eval :: source [wtxPath host resource tcl \
	app-config Tornado]02Project.win32.tcl
