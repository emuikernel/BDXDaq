##########################################################
# VersionControlOptions.win32.tcl - Allows version control options to be
#                                   configured
# Copyright 1995-98 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01b,09sep98,jak  completed implementation 
# 01a,06sep98,aam  written
#

# DESCRIPTION
# This script defines the defaults for ver. ctrl. systems on WIN32. It then 
# sources the options UI for configuring the commands.

namespace eval VerCtrlDefaults {
	variable vcSystems 
	variable vcCmds 
	set vcSysList 	[list \
					"Visual Source Safe" \
					"PVCS" \
					"ClearCase" \
					] 
	set vcCmds(checkin) [list \
            			"ss Checkin \$filename -C\$comment -I-N" \
						"put -M\$comment -U -N \$filename" \
        				"cleartool checkin -c \$comment \$filename" \
						]

	set vcCmds(checkout) [list \
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
