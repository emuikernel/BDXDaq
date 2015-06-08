##############################################################################
# 01TornadoOptions.win32.tcl - Tornado for Windows options dialog implementation
#
# Copyright 1995-98 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01f,16mar99,jak  add helpbutton
# 01e,19oct98,rbl  fixed bug where changing a color option would blank out the 
#			 fonts and colors window, and changed title to "Options"
# 01d,22sep98,j_k  changed the menu string to say options
# 01d,06sep98,aam  removed the call to updatePreferences on OK callback
# 01c,03sep98,j_k  broke this file into multiple files.
# 01b,07aug98,j_k  implemented debugger options page
# 01a,01jun98,j_k  written
#

#

set gOptionsPages {}
set bColorItemChanged 0
set bFontItemChanged 0

proc onColorsChange {bState} {
	global bColorItemChanged

	set bColorItemChanged $bState
}

proc onFontsChange {bState} {
	global bFontItemChanged

	set bFontItemChanged $bState
}

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
	global bColorItemChanged
	global bFontItemChanged

	set optionsDlgCtrls [list \
		[list tab -callback onTabChange -name mainTabCtrl \
			-xpos 5 -ypos 5 -width 300 -height 190] \
		[list button -default -title "OK" -callback {
				global bColorItemChanged
				global bFontItemChanged
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

				updatePreferences 1 $bColorItemChanged $bFontItemChanged
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
	global bColorItemChanged
	global bFontItemChanged

	dialogCreate -name tornadoOptionsDlg -title "Options" \
		-parent mainwindow -width 310 -height 220 \
		-init optionsDlgInit

	if {$bColorItemChanged == 1} {
		crosswind eval "onUpdate colors"
	}
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
