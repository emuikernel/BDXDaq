##########################################################
# VersionControlOptions.unix.tcl - Allows version control options to be
#                                   configured
# Copyright 1995-98 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01d,09dec98,jak  uppercase system names
# 01c,08dec98,jak  add rcs and sccs defaults
# 01b,09sep98,jak  completed implementation. TODO- add support for others 
# 01a,06sep98,aam  written
#

# DESCRIPTION
# This file defines the defaults to be used for the ver. ctrl. commands
# on UNIX. It then sources the options UI for configuring these commands.
#

namespace eval VerCtrlDefaults {
	variable vcSystems 
	variable vcCmds 
	set vcSysList 	[list \
			"SCCS" \
			"RCS" \
			"Visual Source Safe" \
			"PVCS" \
			"ClearCase" \
					] 
	set vcCmds(checkin) [list \
				"sccs delget -y\$comment \$filename" \
				"ci -u -m\$comment \$filename" \
            			"ss Checkin \$filename -C\$comment -I-N" \
				"put -M\$comment -U -N \$filename" \
        			"cleartool checkin -c \$comment \$filename" \
				]

	set vcCmds(checkout) [list \
				"sccs get -e \$filename" \
				"co -l \$filename" \
            			"ss Checkout \$filename -C\$comment -I-N" \
				"get -L -W -N \$filename" \
        			"cleartool checkout -c \$comment \$filename" \
				]

	proc getSysList {} {
		variable vcSysList
		return $vcSysList
	}

	proc getCmdDefault { cmd iSys} {
		variable vcCmds
		
		if {![info exists vcCmds($cmd)]} { return "" }
		if {$iSys < 0} { return "" }
		if {$iSys >= [llength $vcCmds($cmd)]} { return "" }
		return [lindex $vcCmds($cmd) $iSys]
	}
}


# Now source the common UI:
set uiFileName \
	[wtxPath host resource tcl app-config Tornado]03VerCtrlOptionsUI.tcl

if {[catch {source $uiFileName} err]} {
	puts "Error in sourcing $uiFileName:\n$err"
}
