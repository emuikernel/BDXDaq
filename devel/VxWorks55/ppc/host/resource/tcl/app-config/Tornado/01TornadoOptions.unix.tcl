##############################################################################
# 01TornadoOptions.unix.tcl - Tornado for UNIX options dialog implementation
#
# Copyright 1995-98 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01b,04mar99,jak  rewrote based on win32 implementation, to add helpbutton
# 01a,06sep98,aam  written
#

# Note: The name of this dialog must be the same as that of the win32 version, 
# for correct helpid generation

set gOptionsPages {}

proc onTabChange {} {
	global gOptionsPages
	global gCurrentPage

	# first call the previous page exit function
	set retValue [eval "[lindex [lindex $gOptionsPages $gCurrentPage] 3]"]
	if {$retValue == 0} {
		controlSelectionSet tornadoOptionsDlg.mainTabCtrl $gCurrentPage
		eval "[lindex [lindex $gOptionsPages $gCurrentPage] 2]"
		return
	}

	# get the current selection page name
	set gCurrentPage [controlSelectionGet tornadoOptionsDlg.mainTabCtrl]

	# call the current page enter function
	eval "[lindex [lindex $gOptionsPages $gCurrentPage] 2]"
}

proc optionsDlgInit {} {
	global gOptionsPages
	global gCurrentPage

	set optionsDlgCtrls [list \
		[list tab -callback onTabChange -name mainTabCtrl \
			-xpos 5 -ypos 5 -width 300 -height 190] \
		[list button -default -title "OK" -callback {
				global gCurrentPage
				global gOptionsPages

				set retValue [eval "[lindex [lindex $gOptionsPages $gCurrentPage] 3]"]
				if {$retValue == 0} {
					controlSelectionSet tornadoOptionsDlg.mainTabCtrl $gCurrentPage
					eval "[lindex [lindex $gOptionsPages $gCurrentPage] 2]"
					return
				}
				
				# call the save proc to save to the registry
				foreach pageInfo $gOptionsPages {
					eval "[lindex $pageInfo 5]"
				}

				windowClose tornadoOptionsDlg
			} \
			-xpos 145 -ypos 200 -width 50 -height 14] \
		[list button -title "Cancel" -callback "windowClose tornadoOptionsDlg" \
			-xpos 200 -ypos 200 -width 50 -height 14] \
		[list button -title "&Help" -helpbutton \
			-xpos 255 -ypos 200 -width 50 -height 14] \
	]

	# call the restore proc to restore from the registry
	foreach pageInfo $gOptionsPages {
		eval "[lindex $pageInfo 4]"
	}

	foreach ctrl $optionsDlgCtrls {
		controlCreate tornadoOptionsDlg $ctrl
	}

	foreach pageInfo $gOptionsPages {
		controlTabsAdd tornadoOptionsDlg.mainTabCtrl [list [lindex $pageInfo 0]]
		
		eval "[lindex $pageInfo 1]"
	}
	set gCurrentPage 0
	eval "[lindex [lindex $gOptionsPages $gCurrentPage] 2]"
}

proc OptionsDialogCreate {} {
	dialogCreate -name tornadoOptionsDlg -title "Options" \
		-parent mainwindow -width 310 -height 220 \
		-init optionsDlgInit
}

proc OptionsPageAdd {title initProc enterProc exitProc restoreProc saveProc} {
	global gOptionsPages

	lappend gOptionsPages [list $title $initProc $enterProc $exitProc $restoreProc $saveProc]
}


menuItemAppend \
	-separator -bypath {&Tools} 

menuItemAppend \
	-callback OptionsDialogCreate \
	-bypath {&Tools} {&Options...}
