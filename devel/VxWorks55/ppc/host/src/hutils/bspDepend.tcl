# bspDepend.tcl - tcl script fix the depend.<bsp> on windows
# 
# modification history
# --------------------
# 01b,08feb01,t_m  Adding search/replace for //<drive>/<tornado_dir> for ARM
#                  gcc 2.97 stdinc paths
# 01a,21Oct98,ms   written
#
# DESCRIPTION
# Strip ^M's at the end of line.
# Substiture WIND_BASE as needed.

set usage "Uasge: wtxtcl -f bspDepend.tcl WIND_BASE file"

if {$argc != 1} {
	puts "$usage"
	exit -1
	}

set file [lindex $argv 0]
regsub -all {(\\|/)} [wtxPath] {(\\\\|/)} windBase
#
# GCC 2.97 ARM compiler on windows puts stdinc depends out in
# the form "//e/tornado/host/..."
# this addition converts that to $WIND_BASE 
regsub -all {(:)} [wtxPath] "" windBase2
set windBase2 "//$windBase2"

set fd [open $file r]
fconfigure $fd -translation lf
set info [read $fd]
regsub -all -nocase $windBase $info {$(WIND_BASE)/} info

# the replacement of the above path to WIND_BASE
regsub -all -nocase $windBase2 $info {$(WIND_BASE)/} info

close $fd

set fd [open $file w+]
fconfigure $fd -translation lf
puts $fd $info
close $fd

