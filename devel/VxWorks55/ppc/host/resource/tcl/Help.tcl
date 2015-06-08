##########################################################
# Help.tcl - Adds windows-style help support to tornado project for UNIX
#
# Copyright 1999 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,12feb99,jak  written
#

# DESCRIPTION
#   This script contains the initialization for help :
# Initialization of the help id manager used for context-sensitive help 
# When the mapfile is set, the manager reads the contents and uses this
# according the the mode specified.
#
# NOTES
# 1)If this file is sourced multiple times within the same interpreter,
# the check for the mapfile will save the help id manager from getting reset.
# 2)To generate helpids, set the mode to write. At all other times, set mode
#	to read. In write mode, the help ids get written to the mapfile only
#	when uitcldll is unloaded.
#
# CAUTION
# 1) This script assumes UITcl has been loaded
# 2) Do not run the helpid manager in "write" mode on UNIX unless you
# are sure that only one instance of UITcl is available and are sure of its
# load and unload times.


# initializations for context-sensitive help (UNIX)
set mapfile [string trim [uitclHelpIdInfoGet -mapfile]]
if {$mapfile == ""} { ;# set the mapfile only if not set
	set mapfile [wtxPath host resource help]tornado2.hm
	uitclHelpIdInfoSet -mapfile $mapfile -mode read 
}
