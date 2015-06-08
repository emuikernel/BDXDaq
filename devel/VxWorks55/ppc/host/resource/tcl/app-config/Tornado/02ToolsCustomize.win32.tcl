###############################################################################
#
# app-config/Tornado/02ToolsCustomize.win32.tcl -
# Tools and builds customization dialog
#
# Copyright (C) 1995-97 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01j,24mar99,jak  assign default helpid for custom tools
# 01i,15mar99,cjtc Changed menu text for removal of Standard BSP Builds
#		   menu item and seperator. Extension to SPR #25459
# 01h,26feb99,cjs  Expose bootrom targets always
# 01g,02nov98,cjs  disable default menu item 
# 01f,09Oct98,jak  moved macro handling to ExtCmdInterface for UNIX support
# 01f,05Oct98,jak  changed reg. entries to start from 1 (for bkwd compat.)
# 01e,22Sep98,jak  reworked to look and behave as Tor101 but with macro buttons 
# 01d,17Sep98,cjs  make appearance conditional on 'ShowOldProjectMenu'
#                  registry value
# 01c,09Sep98,jak  used ExtCmdInterface for cmd execution 
# 01b,30Aug98,jak  reworked for UITcl changes
# ???,???????,???  created
#

# DESCRIPTION
#   This script contains the routines to customize tools and builds 
#
# Registry entries:
# 	[section key value]
#
# 	[CustomTools Entries n]	 n = no. of custom tools
# 	[CustomTools-Entry-i MenuText string]	i is entry no. (1 - n)
# 	[CustomTools-Entry-i ToolCommand string]	
# 	[CustomTools-Entry-i WorkingDir string]	
# 	[CustomTools-Entry-i PromptForArgs int]	
# 	[CustomTools-Entry-i RunAsMDIChild int]
# 	[CustomTools-Entry-i CloseOnExit int]
#
# 	[CustomBuilds Entries n]	 n = no. of custom builds 
# 	[CustomBuilds-Entry-i MenuText string]	i is entry no. (1 - n)
# 	[CustomBuilds-Entry-i BuildCommand string]	
# 	[CustomBuilds-Entry-i WorkingDir string]	
#
#
#


##############################################################################
# GLOBAL VARIABLES
#
global g_custType customList lastListBoxSelection regSection \
	structOrderList structTypeList

# Which dialog is this -- customize tools or customize builds?
set g_custType ""
set g_custMode ""

# List of custom tools
set customList(tools) {} 
set customList(builds) {} 

# Source in the old project menu code
source [wtxPath host resource tcl app-config Tornado]BuildMenu.tcl

# Registry sections for custom tool or build data
set regSection(tools) "CustomTools"
set regSection(builds) "CustomBuilds"

# List describing structure and order of the structure of a custom
#	tool or build item (which is represented by a Tcl list)
set structOrderList(tools) {tools ToolCommand MenuText WorkingDir CloseOnExit \
	PromptForArgs RunAsMDIChild}
set structTypeList(tools) {none string string string int int int}
set structOrderList(builds) {builds BuildCommand MenuText WorkingDir}
set structTypeList(builds) {none string string string}

# lists to keep track of items added to a menu
# These must be preserve their state throughout the life of the script
set listMenuItems(tools) {}
set listMenuItems(builds) {}

set g_CustomBuildsVisible 0 

#
# initCustomList	- initializes the global list of custom tools
# 	
proc initCustomList {type} {
	global structOrderList structTypeList regSection customList

	set customList($type) {} 

    set entryCount \
		[appRegistryEntryRead -int -default -0 $regSection($type) Entries]

    set menuTextList {} 
    for {set i 0} {$i < $entryCount} {incr i} {
		set tupleList "" 
		set slNo [expr $i + 1]
        set retrieveString [format "%s-Entry-%d" $regSection($type) $slNo]
        for {set j 1} \
                {$j < [llength $structOrderList($type)]} \
                {incr j} {

				set key [lindex $structOrderList($type) $j]
                if {![string compare \
                    [lindex $structTypeList($type) $j] int]} {
                    # Retrieve an int value
                    if {[catch { \
                   			appRegistryEntryRead -int -default 0 \
									$retrieveString $key} regVal]} {
                        puts "Registry entry $retrieveString corrupted!"
                    }
                } else {
                    # Retrieve a string
                    if {[catch { \
							appRegistryEntryRead $retrieveString $key} regVal]} {
                        puts "Registry entry $retrieveString corrupted!"
                    }
                }


               # Create a list of tuples for makeCustomStruct()
               if {$tupleList == ""} {
                   set tupleList [list [list \
                       [lindex $structOrderList($type) $j] $regVal]]
               } else {
                   set tmp [list \
                       [lindex $structOrderList($type) $j] $regVal]
                   set tupleList "$tupleList [list $tmp]"
               }
            }

            # After we finish the above loop, we have variables
            # with names corresponding to the contents of
            # structOrderList.  We also have a format string ready to receive
            # the structure member values.  We string these together into
            # a toolItem, and append this toolItem to the customList

            set toolItem [eval makeCustomStruct $type $tupleList]
            lappend customList($type) $toolItem
	}

}


##############################################################################
# UTIL PROCS

#
# getCustomField - Extract a field from a list representing a build structure
#
proc getCustomField {struct fieldName} {
	global structOrderList

	if {[llength $struct] < 1} {
		error "getCustomField: bad structure, or fcn needs updating."
	}

	set type [lindex $struct 0]

	if { [llength $struct] != [llength $structOrderList($type)] } {
		error "getCustomField: bad structure, or fcn needs updating."
	}

	for {set i 1} {$i < [llength $structOrderList($type)]} {incr i} {
		if {![string compare [lindex $structOrderList($type) $i] $fieldName]} {
			return [lindex $struct $i]
		}
	}

	error "bad fieldName $fieldName for getCustomField"
}

#
# setCustomField - Replace the structure value identified by fieldName
#
proc setCustomField {struct fieldName value} {
	global structOrderList

	# Version test!
	if {[llength $struct] > [llength $structOrderList($g_custType)]} {
		error "setCustomField: bad structure"
	}

	set type [lindex $struct 0]

	# Find position of described field in list
	set fieldIndex [lsearch $structOrderList($type) $fieldName]

	# Replace the value at that position
	if {$fieldIndex != -1} {
		set struct [lreplace $struct $fieldIndex $fieldIndex $value]
	} else {
		error "setCustomField: field $fieldName not found."
	}
	
	return $struct
}

#
# makeCustomStruct - creates a struct for the tool info
# 	Take a list of <fieldName>, <value> pairs and assemble
# 	it into a list representing a custom structure.  Missing
# 	tuples are assigned a blank
proc makeCustomStruct {type args} {
	global structOrderList
	set struct [list $type] 
	for {set i 1} {$i < [llength $structOrderList($type)]} \
		{incr i} {

		set found 0
		for {set j 0} {$j < [llength $args]} {incr j} {
			set tuple [lindex $args $j]
			if {![string compare [lindex $tuple 0] \
				[lindex $structOrderList($type) $i]]} {
				lappend struct [lindex $tuple 1]
				set found 1
			}	
		}

		# Append a blank element if the tuple wasn't included
		if {$found == 0} {
			lappend struct ""
		}
	}

	return $struct
}



##############################################################################
# UI CONTROL LISTS 
#

	# lists of controls for each dialog: 

	set ctrlList(mainCtrls) { \
		{ label -title "Menu &Contents:" -name IDC_MENU_CONTENTS_LABEL \
			-xpos 6 -ypos 6 -width 63 -height 9 \
		} \
		{ list -name IDC_CUST_MENULIST  \
				-xpos 6 -ypos 18 -width 243 -height 73 \
				-callback IDD_CUSTOMIZEOnListBox \
		} \
		{ button -title "&Add..." -name IDB_CUST_ADD  \
				-xpos 259 -ypos 18 -width 50 -height 14  \
				-callback {IDD_CUSTOMIZEOnButton add} \
		} \
		{ button -title "&Remove" -name IDB_CUST_REMOVE  \
				-xpos 259 -ypos 35 -width 50 -height 14  \
				-callback {IDD_CUSTOMIZEOnButton remove} \
		} \
		{ button -title "Move &Up" -name IDB_CUST_MOVEUP  \
				-xpos 259 -ypos 52 -width 50 -height 14  \
				-callback {IDD_CUSTOMIZEOnButton moveup} \
		} \
		{ button -title "Move Dow&n" -name IDB_CUST_MOVEDOWN  \
				-xpos 259 -ypos 69 -width 50 -height 14  \
				-callback {IDD_CUSTOMIZEOnButton movedown} \
		} \
	}

	set ctrlList(common) {
		{ label -title "&Menu Text:" -name IDC_MENU_TEXT_LABEL  \
				-xpos 6 -ypos 93 -width 63 -height 9  \
		} \
		{ text -name IDC_CUST_MENUTEXT  \
				-xpos 74 -ypos 91 -width 236 -height 12  \
		} \
		{ text -name IDC_CUST_CMDSTRING  \
				-xpos 74 -ypos 109 -width 221 -height 12  \
		} \
		{ text -name IDC_CUST_WORKDIR  \
				-xpos 74 -ypos 127 -width 221 -height 12  \
		} \
		{ button -title " > " -name IDC_BUTTONFILES  \
				-menubutton \
				-xpos 299 -ypos 109 -width 12 -height 12  \
				-callback {IDD_CUSTOMIZEOnSelectionButton IDC_BUTTONFILES} \
		} \
		{ label -title "Working &Directory:" -name IDC_WORK_DIR_LABEL  \
				-xpos 6 -ypos 129 -width 63 -height 9  \
		} \
		{ button -title " > " -name IDC_BUTTONDIR  \
				-menubutton \
				-xpos 299 -ypos 127 -width 12 -height 12  \
				-callback {IDD_CUSTOMIZEOnSelectionButton IDC_BUTTONDIR} \
		} \
	}

	set ctrlList(tools) {
		{ button -title "OK" -name IDOK -default \
				-xpos 6 -ypos 165 -width 50 -height 14  \
				-callback IDD_CUSTOMIZEOnIDOK \
		} \
		{ button -title "Cancel" -name IDCANCEL  \
				-xpos 65 -ypos 165 -width 50 -height 14  \
				-callback IDD_CUSTOMIZEOnIDCANCEL \
		} \
		{ button -title "&Help" -name IDCANCEL  \
				-helpbutton \
				-xpos 259 -ypos 165 -width 50 -height 14  \
		} \
		{ label -title "&Tool Command:" -name IDC_TOOL_CMD_LABEL  \
				-xpos 6 -ypos 111 -width 63 -height 9  \
		} \
		{ boolean -title "&Prompt for Arguments" -name IDC_PROMPT_FOR_ARGS \
				-auto -xpos 6 -ypos 147 -width 84 -height 10  \
		} \
		{ boolean -title "Redirect to Child &Window" \
				-name IDC_RUN_AS_MDI_CHILD -auto \
				-xpos 110 -ypos 147 -width 100 -height 10  \
				-callback {IDD_CUSTOMIZEOnBoolean IDC_RUN_AS_MDI_CHILD} \
		} \
		{ boolean -title "Close Window On &Exit" \
				-name IDC_CLOSE_ON_EXIT -auto \
				-xpos 226 -ypos 147 -width 84 -height 10  \
				-callback {IDD_CUSTOMIZEOnBoolean IDC_CLOSE_ON_EXIT} \
		} \
	}

	set ctrlList(builds) { \
		{ button -title "OK" -name IDOK -default \
				-xpos 6 -ypos 147 -width 50 -height 14  \
				-callback IDD_CUSTOMIZEOnIDOK \
		} \
		{ button -title "Cancel" -name IDCANCEL  \
				-xpos 65 -ypos 147 -width 50 -height 14  \
				-callback IDD_CUSTOMIZEOnIDCANCEL \
		} \
		{ button -title "&Help" -name IDCANCEL  \
				-helpbutton \
				-xpos 259 -ypos 147 -width 50 -height 14  \
		} \
		{ label -title "&Build Target:" -name IDC_TOOL_CMD_LABEL  \
				-xpos 6 -ypos 111 -width 63 -height 9  \
		} \
	}



	#
	# showCustomizeUI - UI creation & call back for the menu item "Customize..."
	#
	proc showCustomizeUI {type} {

		proc IDD_CUSTOMIZEOnInit {type} {
			global ctrlList g_custType g_custMode g_iSel

			set g_custMode undefined 
			set g_custType $type
			set g_iSel -1

			set title "Customize "
			append title $g_custType
			windowTitleSet IDD_CUSTOMIZE $title
		
			# Create the mainCtrls controls
			foreach control $ctrlList(mainCtrls) {
				controlCreate IDD_CUSTOMIZE $control 
			} 
	
			initCustomList $g_custType	
			custUpdateListBox 

			switch $type { 
				tools {
					windowSizeSet IDD_CUSTOMIZE 315 186 
				}
				builds {
					windowSizeSet IDD_CUSTOMIZE 315 166 
				}
			}

			# Create the common controls
			foreach control $ctrlList(common) {
				controlCreate IDD_CUSTOMIZE $control 
			} 
			# Create the extra controls
			foreach control $ctrlList($type) {
				controlCreate IDD_CUSTOMIZE $control 
			} 
					
			custUpdateCtrls 0 ;# disable ctrls initially

		}

		dialogCreate \
			-name IDD_CUSTOMIZE \
			-title "Customize" \
			-width 315 -height 125 \
			-init "IDD_CUSTOMIZEOnInit $type"
	}



##############################################################################
# UI CONTROL CALLBACK PROCS 
#


#
# IDD_CUSTOMIZEOnButton - event handler for add, moveup, movedown, remove
#
proc IDD_CUSTOMIZEOnButton {mode} {
	global g_custType customList g_custMode g_iSel

	switch $mode {

	  moveup {
			set iSel [controlSelectionGet \
				IDD_CUSTOMIZE.IDC_CUST_MENULIST]
			if {$iSel == ""} {return}
			set item [lindex $customList($g_custType) $iSel ]
			set customList($g_custType) \
		 		[lreplace $customList($g_custType) $iSel $iSel ]
			set iNew [expr $iSel - 1]
			set customList($g_custType) \
		 	[linsert $customList($g_custType) $iNew $item]
			set g_iSel $iNew
			custUpdateListBox $iNew 
	  }

	  movedown {
			set iSel [controlSelectionGet \
				IDD_CUSTOMIZE.IDC_CUST_MENULIST]
			if {$iSel == ""} {return}
			set item [lindex $customList($g_custType) $iSel ]
			set customList($g_custType) \
				[lreplace $customList($g_custType) $iSel $iSel]
			set iNew [expr $iSel + 1]
			set customList($g_custType) \
				[linsert $customList($g_custType) $iNew $item]
			set g_iSel $iNew
			custUpdateListBox $iNew 
	  }


	  remove {
			set iSel \
				[controlSelectionGet IDD_CUSTOMIZE.IDC_CUST_MENULIST]
			if {$iSel == ""} {return}
			set customList($g_custType) \
				[lreplace $customList($g_custType) $iSel $iSel]
			set g_iSel -1 
			custUpdateListBox
			controlValuesSet IDD_CUSTOMIZE.IDC_CUST_MENUTEXT "" 
			controlValuesSet IDD_CUSTOMIZE.IDC_CUST_CMDSTRING "" 
			controlValuesSet IDD_CUSTOMIZE.IDC_CUST_WORKDIR "" 
			if {![string compare $g_custType tools]} {
				controlCheckSet IDD_CUSTOMIZE.IDC_PROMPT_FOR_ARGS 0 
				controlCheckSet IDD_CUSTOMIZE.IDC_CLOSE_ON_EXIT 0 
				controlCheckSet IDD_CUSTOMIZE.IDC_RUN_AS_MDI_CHILD 0 
			}
	  }	

	  add {

			# save any changes and update the UI 
			custSaveState $g_custMode $g_custType $g_iSel
			custUpdateListBox

			custUpdateCtrls 1 ;# enable ctrls
			controlSelectionSet IDD_CUSTOMIZE.IDC_CUST_MENULIST -noevent -1 
			controlValuesSet IDD_CUSTOMIZE.IDC_CUST_WORKDIR "" 
			if {![string compare $g_custType tools]} {
				controlValuesSet IDD_CUSTOMIZE.IDC_CUST_CMDSTRING "" 
				controlValuesSet IDD_CUSTOMIZE.IDC_CUST_MENUTEXT "New Tool" 
				controlCheckSet IDD_CUSTOMIZE.IDC_PROMPT_FOR_ARGS 0 
				controlCheckSet IDD_CUSTOMIZE.IDC_CLOSE_ON_EXIT 0 
				controlCheckSet IDD_CUSTOMIZE.IDC_RUN_AS_MDI_CHILD 0 
			} else {
				controlValuesSet IDD_CUSTOMIZE.IDC_CUST_MENUTEXT "New Build" 
			}
			controlFocusSet IDD_CUSTOMIZE.IDC_CUST_MENUTEXT
			set g_custMode add 
	  }
	}
}


#
# IDD_CUSTOMIZEOnListBox - event handler for the listbox
#
proc IDD_CUSTOMIZEOnListBox {} {
	global customList g_custType g_custMode g_iSel
	if {[controlEventGet IDD_CUSTOMIZE.IDC_CUST_MENULIST] == "selchange"} {

		# save any changes and update the UI 
		custSaveState $g_custMode $g_custType $g_iSel
		set iSel [controlSelectionGet IDD_CUSTOMIZE.IDC_CUST_MENULIST]
		if {$iSel == ""} { 
			set g_iSel -1
		} else {
			set g_iSel $iSel
		}
		custUpdateListBox $iSel
		custUpdateButtons
		custUpdateCtrls 1 ;# enable ctrls if unabled

		set g_custMode modify ;# it's a selchange, has to be modify mode

		set curItem [lindex $customList($g_custType) $iSel]
		controlValuesSet IDD_CUSTOMIZE.IDC_CUST_MENUTEXT \
					[getCustomField $curItem MenuText]
		controlValuesSet IDD_CUSTOMIZE.IDC_CUST_WORKDIR \
					[getCustomField $curItem WorkingDir]
		if {"$g_custType" == "tools"} {
			controlValuesSet IDD_CUSTOMIZE.IDC_CUST_CMDSTRING \
						[getCustomField $curItem ToolCommand]
			controlCheckSet IDD_CUSTOMIZE.IDC_PROMPT_FOR_ARGS \
						[getCustomField $curItem PromptForArgs]
			controlCheckSet IDD_CUSTOMIZE.IDC_CLOSE_ON_EXIT \
						[getCustomField $curItem CloseOnExit]
			controlCheckSet IDD_CUSTOMIZE.IDC_RUN_AS_MDI_CHILD \
						[getCustomField $curItem RunAsMDIChild]
		} else {
			controlValuesSet IDD_CUSTOMIZE.IDC_CUST_CMDSTRING \
						[getCustomField $curItem BuildCommand]
		}
		# return the focus to the list, that's where it all began
		controlFocusSet IDD_CUSTOMIZE.IDC_CUST_MENULIST
	}

}

#
# IDD_CUSTOMIZEOnBoolean - event handler for the booleans 
#
proc IDD_CUSTOMIZEOnBoolean {bID} {
	switch $bID {
		IDC_RUN_AS_MDI_CHILD {
			# if child, allow closing of window
			if {[controlChecked IDD_CUSTOMIZE.IDC_RUN_AS_MDI_CHILD]} {
				controlEnable IDD_CUSTOMIZE.IDC_CLOSE_ON_EXIT 1
			} else {
				controlCheckSet IDD_CUSTOMIZE.IDC_CLOSE_ON_EXIT 0
				controlEnable IDD_CUSTOMIZE.IDC_CLOSE_ON_EXIT 0 
			}
		}
	}
}

#
# IDD_CUSTOMIZEOnSelectionButton - event handler for the "selection" buttons 
#
proc IDD_CUSTOMIZEOnSelectionButton {bID} {
	global gMenuToShow

	set gMenuToShow custIDM_TOOLMACROS 
	menuCreate -name custIDM_TOOLMACROS -context

    menuItemAppend custIDM_TOOLMACROS -callback \
                    "IDD_CUSTOMIZEOnMenu $bID browse" \
                    "Browse..."

	# append categories of macros to menu:
	foreach macroType [MacroHandler::getMacroTypeList toolscustomize] {
    	menuItemAppend custIDM_TOOLMACROS -separator
		set listMacros [MacroHandler::getMacroList $macroType]
		foreach listItem $listMacros {
			set macro [lindex $listItem 0]
			set macroName [lindex $listItem 1]
			menuItemAppend custIDM_TOOLMACROS -callback \
				"IDD_CUSTOMIZEOnMenu $bID $macro" \
				"$macroName"
		}
	}

	return 1
}


#
# IDD_CUSTOMIZEOnMenu - inserts the required macros or browses from macromenu
#
proc IDD_CUSTOMIZEOnMenu {bID mItemID} {
	switch $bID {
		IDC_BUTTONFILES {
			set textField IDD_CUSTOMIZE.IDC_CUST_CMDSTRING
		}
		IDC_BUTTONDIR {
			set textField IDD_CUSTOMIZE.IDC_CUST_WORKDIR
		}
	}
	set strIns ""

	switch $mItemID {
    	browse {
			switch $bID {
				IDC_BUTTONFILES {
                	set strIns [lindex [fileDialogCreate -filemustexist \
							-title "Select Executable" \
							-okbuttontitle "Select" ] 0]
				}
				IDC_BUTTONDIR {
					set strIns [controlTextGet IDD_CUSTOMIZE.IDC_CUST_WORKDIR]
					if {$strIns == ""} {
						set strIns [pwd]
					}
					set strIns [dirBrowseDialogCreate -initialdir $strIns \
							-title "Working Directory"]
				}
			}
    	}
		default { ;# macros, just prepend $ and insert
			set strIns \$$mItemID
		}
	}
	if {$strIns != ""} {
		controlTextInsert $textField -cursor $strIns
    }
    return 1
}


#
# IDD_CUSTOMIZEOnIDOK - event handler for the OK button 
#
proc IDD_CUSTOMIZEOnIDOK {} {
	global customList lastListBoxSelection structOrderList structTypeList \
		g_custType regSection g_iSel g_custMode


	# Save the changes, if any
	custSaveState $g_custMode $g_custType $g_iSel

	# remove the old custom tools from the registry, then
	# write the custom tool list back to the registry
    set entryCount \
		[appRegistryEntryRead -int -default -0 \
			$regSection($g_custType) Entries]
    for {set i 0} {$i < $entryCount} {incr i} {
		set slNo [expr $i + 1]
        set sectionName [format "%s-Entry-%d" $regSection($g_custType) $slNo]
		# TODO:
		# catch { appRegistryEntryDelete $sectionName }
	}

	set nTools 0 
	for {set i 0} {$i < [llength $customList($g_custType)]} {incr i} {
		
		set toolItem [lindex $customList($g_custType) $i]

		set slNo [expr $i + 1]
		set sectionName \
			[format "%s-Entry-%d" $regSection($g_custType) $slNo]
		incr nTools 1

		# Loop through structure list and write out elements
		for {set j 1} \
			{$j < [llength $structOrderList($g_custType)]} {incr j} {
			set keyName [lindex $structOrderList($g_custType) $j] 
			set data [getCustomField $toolItem $keyName]
			set dataType [lindex $structTypeList($g_custType) $j]
			if {$dataType == "int"} {
				appRegistryEntryWrite -int $sectionName $keyName $data
			} else {
				appRegistryEntryWrite $sectionName $keyName $data
			}
		}
		
	}

	# Write an entry that indicates how many entries there are
	appRegistryEntryWrite -int $regSection($g_custType) Entries $nTools 

	# add the items to the menu
	custUpdateMenuItems $g_custType

	# Close the window
	windowClose IDD_CUSTOMIZE
}


#
# IDD_CUSTOMIZEOnIDCANCEL - event handler for the CANCEL button 
#
proc IDD_CUSTOMIZEOnIDCANCEL {} {
	# Close the window
	windowClose IDD_CUSTOMIZE
}



#
# customLaunch - do the actual command associated with a menu item 
#
proc customLaunch {args} {

	set cmdInfoStruct $args
	set type [lindex $cmdInfoStruct 0]
	switch 	$type {
		tools {
			set title [getCustomField  $cmdInfoStruct MenuText]
			set cmdLine [getCustomField  $cmdInfoStruct ToolCommand]
			set cmdLine [MacroHandler::substAllMacros toollaunch $cmdLine]
    		set workingDir [getCustomField  $cmdInfoStruct WorkingDir] 
			set workingDir [string trim $workingDir]
			set workingDir [MacroHandler::substAllMacros toollaunch $workingDir]
			set closeOnExit [getCustomField  $cmdInfoStruct CloseOnExit]
			set promptForArgs [getCustomField  $cmdInfoStruct PromptForArgs]
			set runAsMDIChild [getCustomField  $cmdInfoStruct RunAsMDIChild]
			set captureOut $runAsMDIChild

    		if {[catch {ExtCmdInterface::runCmd tool \
                        $cmdLine $workingDir $title \
                        $runAsMDIChild $closeOnExit $promptForArgs } err]} {
                	 messageBox "error: '$err'"
    		}
		}
		builds {
			set title [getCustomField  $cmdInfoStruct MenuText]
			set cmdLine [getCustomField  $cmdInfoStruct BuildCommand]
			set cmdLine [MacroHandler::substAllMacros buildlaunch $cmdLine]
    		set workingDir [getCustomField  $cmdInfoStruct WorkingDir] 
			set workingDir [string trim $workingDir]
			set workingDir [MacroHandler::substAllMacros buildlaunch $workingDir]

			# launch an external process with output to Build output window
    		if {[catch {ExtCmdInterface::runCmd build \
                        $cmdLine $workingDir $title} err]} {
                	 messageBox "error: '$err'"
    		}
		}
		default {
			error "Unknown customized option"
		}
	}
}


##############################################################################
# UI UPDATE PROCS 

#
# custUpdateMenuItems - updates the menus with contents of the customList
#
proc custUpdateMenuItems {type} {
	global customList listMenuItems

	# remove all items previously added from the menu 
	foreach menuItem $listMenuItems($type) {
		menuItemDelete -bypath -string $menuItem
	}
	set listMenuItems($type) {}

    # add the tool to the menu
	if {$type == "tools"} {
		set menuBarItem {&Tools}
		set defMenuItem "No custom tools"
	}
	if {$type == "builds"} {
		set menuBarItem {&Build}
		set defMenuItem "No custom builds"
	}

    set len [llength $customList($type)]
    for {set i 0} {$i < $len} {incr i} {
		set toolItem [lindex $customList($type) $i]

        set menuItem [getCustomField $toolItem MenuText]
		set menuItem [string trim $menuItem]
		if {$menuItem == ""} { continue}

        if {![menuExists [list $menuBarItem $menuItem]]} {
			# note: 0x1fff is a reserved id that says there's no help
			# 		for this
            menuItemAppend \
				-helpid 0x1fff \
                -callback "customLaunch $toolItem" \
                -bypath $menuBarItem $menuItem
			lappend listMenuItems($type) [list $menuBarItem $menuItem]	
        }
    }

	# if there were no custom stuff, add the default to menu
    if {[llength $listMenuItems($type)] == 0} {
		menuItemAppend \
			-cmduicallback ::disableItem \
			-bypath $menuBarItem $defMenuItem 
		menuItemEnable -bypath [list $menuBarItem $defMenuItem] 0
		lappend listMenuItems($type) [list $menuBarItem $defMenuItem]	
	}
}

#
# A Command UI Update routine that always disables the menu item
#
proc disableItem {} {
	global enableflag
	global checkflag
	set enableflag 0
	set checkflag 0
}

#
# custUpdateListBox - change the listbox to reflect the contents of customList
# 	The parameter, if specified, sets a selection in the listbox.
#
proc custUpdateListBox {{iSel -1}} {
	global customList g_custType

	set menuTextList {} 
	# Obtain a list of the menu text items for all the custom tools
	for {set i 0} {$i < [llength $customList($g_custType)]} {incr i} {
		set toolItem [lindex $customList($g_custType) $i]		

		lappend menuTextList [getCustomField $toolItem MenuText]
	}

	# Put the text of the menu items into the listbox
	controlValuesSet IDD_CUSTOMIZE.IDC_CUST_MENULIST $menuTextList

	if {$iSel >= 0 } { 
    	controlSelectionSet IDD_CUSTOMIZE.IDC_CUST_MENULIST -noevent $iSel
	}
	custUpdateButtons
}


#
# custUpdateCtrls - enable or disable detail controls
#
proc custUpdateCtrls {bEnable} {
	global g_custType
	controlEnable IDD_CUSTOMIZE.IDC_CUST_MENUTEXT $bEnable 
	controlEnable IDD_CUSTOMIZE.IDC_CUST_CMDSTRING $bEnable 
	controlEnable IDD_CUSTOMIZE.IDC_CUST_WORKDIR $bEnable 
	controlEnable IDD_CUSTOMIZE.IDC_BUTTONFILES $bEnable 
	controlEnable IDD_CUSTOMIZE.IDC_BUTTONDIR $bEnable 
	if {![string compare $g_custType tools]} {
		controlEnable IDD_CUSTOMIZE.IDC_PROMPT_FOR_ARGS $bEnable 
		controlEnable IDD_CUSTOMIZE.IDC_CLOSE_ON_EXIT $bEnable 
		controlEnable IDD_CUSTOMIZE.IDC_RUN_AS_MDI_CHILD $bEnable 
	}
}


#
# custSaveState - gather values from UI into customList
#
proc custSaveState {mode type iSel} {
	global customList g_custMode

	if {$mode == "undefined"} {return}

	set menuText [controlValuesGet IDD_CUSTOMIZE.IDC_CUST_MENUTEXT]
	set menuText [string trim $menuText]
	# if menu text is empty, don't save
	if {$menuText == ""} {return}

	set cmdString \
		[controlValuesGet IDD_CUSTOMIZE.IDC_CUST_CMDSTRING]
	set cmdString [string trim $cmdString]
	
	set workingDir \
		[controlValuesGet IDD_CUSTOMIZE.IDC_CUST_WORKDIR]

	# Collect additional values and formulate item structure
	# based on which dialog we're working with
	switch $type {
	  tools {
		set closeOnExit \
			[controlChecked IDD_CUSTOMIZE.IDC_CLOSE_ON_EXIT]
		set promptForArgs \
			[controlChecked IDD_CUSTOMIZE.IDC_PROMPT_FOR_ARGS]
		set runAsMDIChild \
			[controlChecked IDD_CUSTOMIZE.IDC_RUN_AS_MDI_CHILD]

		set item [makeCustomStruct $type \
			[list ToolCommand $cmdString] \
			[list MenuText $menuText] [list WorkingDir $workingDir] \
			[list CloseOnExit $closeOnExit] \
			[list PromptForArgs $promptForArgs] \
			[list RunAsMDIChild $runAsMDIChild]]
	  }
	  builds {
		set item [makeCustomStruct $type \
			[list BuildCommand $cmdString] \
			[list MenuText $menuText] [list WorkingDir $workingDir]]
	  }
	}

	switch $mode {
		modify {
		  	set customList($type) \
				[lreplace $customList($type) $iSel $iSel $item]
		}
		add {
			lappend customList($type) $item
		}
	}
}

#
# custUpdateButtons - update all the buttons based on the system state
#
proc custUpdateButtons {} {
	global customList g_custType

	# Set variables hasContent and hasSelection to establish state
	# of menu text listbox
	set hasContent [expr [llength $customList($g_custType)] ? 1 : 0]
	set curSel [controlSelectionGet IDD_CUSTOMIZE.IDC_CUST_MENULIST]
	if {$curSel == ""} {
		set curSel -1
	}
	set hasSelection [expr $hasContent && ([expr $curSel != -1] ? 1 : 0)]

	# These should simply be disabled when there isn't a current
	# selection
	controlEnable IDD_CUSTOMIZE.IDB_CUST_REMOVE $hasSelection

	# These two should not be allowed off the end of the listbox list
	controlEnable IDD_CUSTOMIZE.IDB_CUST_MOVEUP \
		[expr $hasSelection && [expr $curSel > 0]]
	controlEnable IDD_CUSTOMIZE.IDB_CUST_MOVEDOWN \
		[expr $hasSelection && [expr $curSel < \
				[llength $customList($g_custType)] - 1]]
}


##############################################################################
# UI MENU INTEGRATION 


variable _buildMenuInsertionPoint [list {&Build} {&Dependencies...}]

# Builds menu additions - to be done only when old-style menu is requested
# Append "Customize..." to "Build" menu, then a separator 
proc addBuildsCustomizeMenu {} {
	global g_CustomBuildsVisible
	variable _buildMenuInsertionPoint

	if {$g_CustomBuildsVisible} { return }
	menuItemAppend \
	    	-callback {showCustomizeUI builds} \
			-bypath {&Build} {&Customize...}
	::OldBuildMenu::oldBuildMenuAdd $_buildMenuInsertionPoint 
	menuItemAppend \
			-separator -bypath {&Build}
	initCustomList builds 
	custUpdateMenuItems builds 
	set g_CustomBuildsVisible 1
}

proc removeBuildsCustomizeMenu {} {
	global g_CustomBuildsVisible listMenuItems

 	if {!$g_CustomBuildsVisible} { return }
 	menuItemDelete -separator \
		       -bypath [list {&Build} $::OldBuildMenu::buildMenuText]
 	menuItemDelete -bypath -string [list {&Build} {&Customize...}]
 	menuItemDelete -bypath \
		       -string [list {&Build} $::OldBuildMenu::buildMenuText]
	menuItemDelete -separator -bypath [lindex $listMenuItems(builds) 0]
  	foreach menuItem $listMenuItems(builds) {
  		menuItemDelete -bypath -string $menuItem
  	}
 	set listMenuItems(builds) {}
	set g_CustomBuildsVisible 0
}

::OldBuildMenu::bootromBuildMenuAdd $_buildMenuInsertionPoint 

if {[appRegistryEntryRead -default 0 Workspace ShowOldProjectMenu] != 0} {
	addBuildsCustomizeMenu
}


# Tools menu additions
# Append "Customize..." to "Tools" menu, then a separator 
menuItemAppend \
    	-callback {showCustomizeUI tools} \
		-bypath {&Tools} {&Customize...} 
menuItemAppend \
		-separator -bypath {&Tools}


initCustomList tools
custUpdateMenuItems tools

