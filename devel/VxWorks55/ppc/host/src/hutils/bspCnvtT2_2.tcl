# bspCnvtT2_2.tcl - tcl script to convert a BSP to Tornado T2.2
# 
# Copyright 1984-2002, Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,16jan02,dat  written
#
# DESCRIPTION
# This script will update a T2.0 or T2.1 BSP to be compatible with T2.2.
#
# It does this by editting the BSP Makefile to remove include statements
# that are no longer used, and to flag a warning to the user about any 
# HEX_FLAG values encountered.
#

set bspBase .

if {$argc == 1} {
    set bspBase [wtxPath]target/config/[lindex $argv 0]
}

if {$argc > 1} {
    puts "Usage: wtxtcl bspCnvtT2_2.tcl \[bsp name\]"
    exit (-1)
}

# Find the Makefile

set fd [ open $bspBase/Makefile r]

# Open and read in the Makefile

set buf [ read $fd ]
close $fd

# Determine if conversion for T2.2 is needed.

if {[regexp "Update for T2.2" $buf] == 1} {
    return
}

# Save a copy of original Makefile

set fd [ open $bspBase/Makefile.orig w ]
puts $fd $buf
close $fd

# Add mod history entry.

regsub "\n# ------*" $buf "\\0\n# ---,01feb02,xxx  Update for T2.2 (bspCnvtT2_2.tcl)" buf

# Comment out old include lines

regsub "\n(\[ 	\]*include\[\^\n\]*make/make.\\$\\(CPU\\)\\$\\(TOOL\\))" $buf "\n#\\1" buf

regsub "\n(\[ 	\]*include\[\^\n\]*make/defs.\\$\\(WIND_HOST_TYPE\\))" $buf "\n#\\1" buf

regsub "\n(\[ 	\]*include\[\^\n\]*make/rules.\\$\\(WIND_HOST_TYPE\\))" $buf "\n#\\1" buf

#   Comment out old HEX_FLAGS, generate warning if needed

if {[regsub "\n(\[ 	\]*HEX_FLAGS\[\^\n\]*)" $buf "\n#\\1" buf] == 1} {
    puts "\nWarning: BSP Makefile defined HEX_FLAGS."
    puts "This script has commented this out. Please read the release notes"
    puts "and make your own determination if this is the correct action.\n\n"
}


#   Write out new Makefile

set fd [ open $bspBase/Makefile w]
puts $fd $buf
close $fd

#   Finished
