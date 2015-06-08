##############################################################################
# 03ExtEditorOptions.win32.tcl - Tornado for Windows options dlg implementation
#
# Copyright 1995-98 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01b,10sep98,jak  reworked for more features 
# 01a,01jun98,j_k  written
#

#
# Do not source this file if the platform is not win32
#
if {![string match *win32* [wtxHostType]]} return

set lExtEditorRegEntries {}

proc updateEditorPrefs {} {
	global extEditorGlobal
	global lExtEditorRegEntries

	set extEditorGlobal(fromFileMenu) [lindex $lExtEditorRegEntries 1] 
	set extEditorGlobal(fromProject) [lindex $lExtEditorRegEntries 2] 
	set extEditorGlobal(fromOutputWindow) [lindex $lExtEditorRegEntries 3]
}

namespace eval ExtEditorDefaults {
    variable eeList 
    variable eeCmds

    set eeList   [list \
                    "vi" \
					"CodeWright" \
					"Visual SlickEdit" \
                    ]
    set eeCmds [list \
                    "vi +\$lineno \$filename" \
					"cw32 -G\$lineno \$filename" \
					"vs \$filename -#\$lineno" \
                    ]


    proc getEditorList {} {
        variable eeList
        return $eeList
    }

    proc getCmdDefault { iee} {
        variable eeCmds

        if {$iee < 0} { return "" }
        if {$iee >= [llength $eeCmds]} { return "" }
        return [lindex $eeCmds $iee]
    }
}

namespace eval ExtEditorPage {
	variable lOrigExtEditorRegEntries {}
	variable lExtEditorRegItems \
		[list Path FromFileMenu FromProject FromOutputWindow]

	variable gExtEditorPageCtrls [list extEditGrp extEditLbl \
			extEditCmd eeOptIDC_BUTTON1 eeOptIDC_STATIC2 fromFileMenu \
			fromOutputWindow fromProject eeOptIDC_COMBO1 eeOptIDC_STATIC3]

	proc init {} {
		global lExtEditorRegEntries
		variable gExtEditorPageCtrls

		set Controls [list \
			[list group -hidden -name extEditGrp -title "Command" \
				-xpos 12 -ypos 20 -width 286 -height 50] \
			[list label -hidden -name extEditLbl -title "Command line:" \
				-xpos 20 -ypos 34 -width 50 -height 8] \
			[list text -hidden -callback "ExtEditorPage::onPathChange" \
				-name extEditCmd \
				-xpos 75 -ypos 32 -width 160 -height 14] \
			[list button -hidden -name eeOptIDC_BUTTON1 -title " > " \
				-menubutton \
				-callback {ExtEditorPage::onButton eeOptIDC_BUTTON1} \
				-xpos 240 -ypos 32 -width 14 -height 14] \
            [list label -title "Defaults:" -name eeOptIDC_STATIC3 \
            	-xpos 151 -ypos 52 -width 45 -height 8  \
            	-hidden \
            ] \
            [list combo -name eeOptIDC_COMBO1 \
            	-xpos 186 -ypos 50 -width 70 -height 60  \
            	-hidden \
				-callback {ExtEditorPage::onCombo eeOptIDC_COMBO1} \
            ] \
			[list group -hidden -name eeOptIDC_STATIC2 -title "Invoke from" \
				-xpos 12 -ypos 75 -width 98 -height 60] \
			[list boolean -hidden -auto -name fromFileMenu \
				-title "File menu" \
				-xpos 20 -ypos 87 -width 55 -height 10] \
			[list boolean -hidden -auto -name fromProject \
				-title "Project" \
				-xpos 20 -ypos 101 -width 55 -height 10] \
			[list boolean -hidden -auto -name fromOutputWindow \
				-title "Build output window" \
				-xpos 20 -ypos 115 -width 80 -height 10] \
		]

		foreach ctrl $Controls {
			controlCreate tornadoOptionsDlg $ctrl
		}
	
	    # initialize the combo for editors:
        controlValuesSet tornadoOptionsDlg.eeOptIDC_COMBO1 \
            		[ExtEditorDefaults::getEditorList]	

		if {[llength $lExtEditorRegEntries] == 0} {
			controlCheckSet tornadoOptionsDlg.fromFileMenu 1
			controlEnable tornadoOptionsDlg.fromFileMenu 0
			controlEnable tornadoOptionsDlg.fromProject 0
			controlEnable tornadoOptionsDlg.fromOutputWindow 0
		} {
			controlTextSet tornadoOptionsDlg.extEditCmd \
				[lindex $lExtEditorRegEntries 0]
			controlCheckSet tornadoOptionsDlg.fromFileMenu \
				[lindex $lExtEditorRegEntries 1]
			controlCheckSet tornadoOptionsDlg.fromProject \
				[lindex $lExtEditorRegEntries 2]
			controlCheckSet tornadoOptionsDlg.fromOutputWindow \
				[lindex $lExtEditorRegEntries 3]
		}
	}

	proc onPathChange {} {
		if {[controlTextGet tornadoOptionsDlg.extEditCmd] == ""} {
			controlEnable tornadoOptionsDlg.fromFileMenu 0
			controlEnable tornadoOptionsDlg.fromProject 0
			controlEnable tornadoOptionsDlg.fromOutputWindow 0
		} {
			controlEnable tornadoOptionsDlg.fromFileMenu 1
			controlEnable tornadoOptionsDlg.fromProject 1
			controlEnable tornadoOptionsDlg.fromOutputWindow 1
			controlCheckSet tornadoOptionsDlg.fromFileMenu 1
		}
		controlFocusSet tornadoOptionsDlg.extEditCmd 1
	}

	proc enter {} {
		variable gExtEditorPageCtrls

		foreach ctrl $gExtEditorPageCtrls {
			controlShow tornadoOptionsDlg.$ctrl 1
		}
	}

	proc exit {} {
		global launcherGlobals
		global lExtEditorRegEntries
		variable gExtEditorPageCtrls

		set lExtEditorRegEntries {}
		set filePath [controlTextGet tornadoOptionsDlg.extEditCmd]
		set filePath [string trim $filePath]
		lappend lExtEditorRegEntries $filePath

		# we need to verify that the path is a valid one.
		if {$filePath == ""} {
			set launcherGlobals(bExtEditorSetupOk) 0
			set launcherGlobals(extEditorCmd) ""
		} {
			set launcherGlobals(bExtEditorSetupOk) 1
			set launcherGlobals(extEditorCmd) $filePath
		}

		set check [controlChecked tornadoOptionsDlg.fromFileMenu]
		if {$check == ""} {
			set check 0
		}
		lappend lExtEditorRegEntries $check

		set check [controlChecked tornadoOptionsDlg.fromProject]
		if {$check == ""} {
			set check 0
		}
		lappend lExtEditorRegEntries $check

		set check [controlChecked tornadoOptionsDlg.fromOutputWindow]
		if {$check == ""} {
			set check 0
		}
		lappend lExtEditorRegEntries $check

		foreach ctrl $gExtEditorPageCtrls {
			controlShow tornadoOptionsDlg.$ctrl 0
		}
		
		updateEditorPrefs
		return 1
	}

	proc restore {} {
		global lExtEditorRegEntries
		variable lOrigExtEditorRegEntries
		variable lExtEditorRegItems

		set lOrigExtEditorRegEntries {}
		lappend lOrigExtEditorRegEntries [appRegistryEntryRead ExtEditor Path]
		lappend lOrigExtEditorRegEntries [appRegistryEntryRead -int -default 0 ExtEditor FromFileMenu]
		lappend lOrigExtEditorRegEntries [appRegistryEntryRead -int -default 0 ExtEditor FromProject]
		lappend lOrigExtEditorRegEntries [appRegistryEntryRead -int -default 0 ExtEditor FromOutputWindow]
		set lExtEditorRegEntries $lOrigExtEditorRegEntries
	}

	proc save {} {
		global lExtEditorRegEntries

		# write the external editor related entries
		appRegistryEntryWrite ExtEditor Path [lindex $lExtEditorRegEntries 0]
		appRegistryEntryWrite -int ExtEditor FromFileMenu [lindex $lExtEditorRegEntries 1]
		appRegistryEntryWrite -int ExtEditor FromProject [lindex $lExtEditorRegEntries 2]
		appRegistryEntryWrite -int ExtEditor FromOutputWindow [lindex $lExtEditorRegEntries 3]
	}



    #///////////////////////////////////
    # control callbacks

    proc onButton {bID} {
		global gMenuToShow

        switch $bID {
            eeOptIDC_BUTTON1 {
                set gMenuToShow eeOptIDM_MENU1 
                menuCreate -name eeOptIDM_MENU1 -context
                menuItemAppend eeOptIDM_MENU1 -callback \
                    {ExtEditorPage::onMenu eeOptIDM_MENU1 browse} \
                    "Browse..."
                menuItemAppend eeOptIDM_MENU1 -separator
                menuItemAppend eeOptIDM_MENU1 -callback \
                    {ExtEditorPage::onMenu eeOptIDM_MENU1 filename} \
                    "File name"
                menuItemAppend eeOptIDM_MENU1 -callback \
                    {ExtEditorPage::onMenu eeOptIDM_MENU1 lineno} \
                    "Line number"
                return 1
            }
		}
	}

    proc onMenu {mID mItemID} {
        switch $mItemID {
			browse {
				set pathName [lindex \
					[fileDialogCreate -title "Editor Path" -okbuttontitle "OK" \
					-filefilters \
					"Programs|*.exe;*.com;*.bat|All Files (*.*)|*.*||"] \
							0]
				if {$pathName != ""} {
        			controlTextInsert tornadoOptionsDlg.extEditCmd \
									-cursor $pathName
				}
				return 1
			}
            filename {
        		  controlTextInsert tornadoOptionsDlg.extEditCmd \
									-cursor "\$filename"
                  return 1
            }
            lineno {
        		  controlTextInsert tornadoOptionsDlg.extEditCmd \
									-cursor "\$lineno"
                  return 1
            }
        }
    }

    proc onCombo {cID} {
        set event [controlEventGet tornadoOptionsDlg.$cID]
        if {$event == "selchange"} {
            set iSel [controlSelectionGet tornadoOptionsDlg.$cID]
            if {$iSel < 0} {return}
            set cmd [ExtEditorDefaults::getCmdDefault $iSel]
            controlTextSet tornadoOptionsDlg.extEditCmd $cmd
        }
    }
}

OptionsPageAdd "External Editor" ExtEditorPage::init ExtEditorPage::enter \
	ExtEditorPage::exit ExtEditorPage::restore ExtEditorPage::save
