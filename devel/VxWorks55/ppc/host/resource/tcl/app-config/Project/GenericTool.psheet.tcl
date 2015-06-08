# GenericTool.psheet.tcl - generic property sheet for build tools 
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 02v,10jan03,cjs  Fix for SPR 82266: invoke rule before building checkbox
#                  broken
# 02u,16may02,cjs  Fix update problem between rules and macros by decoupling
#                  the two (spr 75813)
# 02t,13may02,cjs  Fix spr 76269: backslash problems in include paths in build
#                  settings dialog
# 02s,02may02,cjs  Fix spr 76637 -- build macro length very limited; fix spr
#                  76702 -- using $(PRJ_DIR) in include path trashes CFLAGS
# 02r,13feb02,cjs  Adding Include path modification logic to Assembler pane
# 02q,05nov01,rbl  merge diab bolt-on changes here to support setting diab
#                  optimization level
# 02p,16oct01,cjs  Fix SPR 28327 -- improve UI for updating include
#                  directories
# 02o,02apr99,cjs  Fixing error in logic that saves macros before rules
# 02n,30mar99,jak  fix optimization combo 
#				   fix "invoke before building project"
# 02m,16mar99,cjs  Fixing spr 25030: Hands are tied...
# 				   Also changed interpretation of "-O" to be "-O1"
# 02l,11mar99,cjs  Changing validation logic for new build rules; empty list
#                  of commands is ok
# 02k,16feb99,cjs  fixed bug in module to src file mapping logic 
# 02j,11feb99,jak  remove spaces from control names
# 02i,01feb99,ren  Adding help button.
# 02h,27jan99,cjs  Fixed bug in macros pane of build prop sheet:
#                  macro list not updated propery on rule change, etc
#                  Also added debug/release options to compiler sheet
# 02g,15jan99,cjs  Add 'General' sheet
# 02f,15dec98,cjs  clear rule cache in dataIn() 
# 02e,01dec98,cjs  fixing 'Invoke this rule before building project' logic;
#                  added a dataDiscard() function to clear the various caches
# 02d,30nov98,cjs  fixed spr 23568 -- removed blank lines from object
#                  module order sheet
# 02c,25nov98,cjs  protect module order logic in dataOut() from case of having
#                  no files
# 02b,24nov98,ren  reduced how often we update the object list
# 02a,23nov98,cjs  Remove extra brace causing obscure bug
# 01z,12nov98,cjs  adding 'Link Order' page 
# 01y,10nov98,cjs  send a DerivedObjectsUpdate event when the build
#                  rule changes
# 01x,06nov98,cjs  made tool visible and editable 
# 01w,02nov98,cjs  made macro description vscroll 
# 01v,26oct98,cjs  Filter out source file related rules from rule list;
#                  update macros pane when current rule changes 
# 01u,23oct98,cjs  Add static word 'Properties:' to title 
# 01s,12oct98,ren  made dialog box look professional.  things line up like
#                  they should.  Logical tab order.  Hot keys.
# 01r,30sep98,cjs  fix new/delete logic in rule and macro panes 
# 01q,10sep98,cjs  fixed problem in use of buildRuleJoin()
# 01p,02sep98,cjs  use buildRuleSplit/Join() routines to fix parse problem
# 01o,02sep98,cjs  protect custom file rules and LDDEPS macro from deletion 
# 01n,31aug98,cjs  removed problematic controlFocusSet in 
#                  onSelChangeMacrosCombo 
# 01m,19aug98,cjs  reduced number of buttons; made rule editor into modal
#                  dialog 
# 01l,03aug98,jmp  replaced dependancies by dependencies.
# 01k,30jul98,cjs  upgraded to handle new build view server 
# 01j,27jul98,jak  changed title of Build Prop. sheet 
# 01i,29jun98,jmp  added a checkbox to the rule editor to be able to invoke a
#                  rule before building project using LDDEPS macro.
# 01h,18jun98,jmp  added rules editor and macros editor.
# 01g,15jun98,cjs  make prop sheet title more descriptive
# 01f,09jun98,cjs  added copyright, return value to dataOut()
# 01e,05jun98,jmp  added Macros tab. added Copyright. set indentation
#		   levels according to wrs coding convention.
# 01d,18apr98,cjs  added build type sheet
# 01c,10apr98,cjs  got rid of underbar collapse of tool names 
# 01b,09apr98,cjs  changed function signatures for better support
#		   of multiple sheets 
# 01a,08apr98,cjs  written.
#
# DESCRIPTION
# Given access to a handle to a build, this property sheet
# replicates itself once for each tool in the toolchain.
#

namespace eval ::GenericToolPropertySheet {

    # This array allows multiple objects in the same set of
    # sheets to recycle one sheet
    variable _sheetList
    variable _index 0

    variable _ctrlList

    variable _buildDlgDirtyFlag 0
    variable _updateObjectsFlag
    variable _moduleMap
    variable _noevent 0 ; # Disables propagation of build flag data
			  # this is needed because -noevent flag doesn't work
}

##############################################################################
#
# titleGet -
#
proc ::GenericToolPropertySheet::titleGet {handle sheet} {
    return $sheet
}

##############################################################################
#
# dateStampGet -
#
proc ::GenericToolPropertySheet::dateStampGet {handle} {
    return 980819
}

##############################################################################
#
# pageMatch -
#
proc ::GenericToolPropertySheet::pageMatch {handle} {
    variable _sheetList
    set retval 0
    if {[string match [::Object::evaluate $handle Type] \
    	buildObject]} {
    	set toolchain [::prjBuildTcGet [hProjGet] [buildSpecGet]]
    	if {[::Workspace::isToolchainInstalled $toolchain]} {
    	    lappend sheets "Macros"
	    lappend sheets "General"
	    foreach tool [${toolchain}::toolListGet] {
		set name [eval ${tool}::type]
		set _sheetList($name) $tool
		lappend sheets $name
    	    }
    	    lappend sheets "Rules"
    	    lappend sheets "Link Order"
    	    set retval [list 1 $sheets]
   	} else {
    	    set retval [list 1 "General"]
	}
    }
    return $retval
}

##############################################################################
#
# OnShow -
#
proc ::GenericToolPropertySheet::OnShow {windowName sheet handle} {
    variable _ctrlList
             
    set name [::Object::evaluate $handle DisplayName] 
    windowTitleSet $windowName "Properties: build specification '$name'"
    foreach ctrl $_ctrlList($sheet) {
	regsub -all { } [::flagValueGet $ctrl -name] {_} ctrlName
	controlHide [format "%s.%s" $windowName $ctrlName] 0
    }
}

##############################################################################
#
# OnHide -
#
proc ::GenericToolPropertySheet::OnHide {windowName sheet handle} {
    variable _ctrlList
    foreach ctrl $_ctrlList($sheet) {
	regsub -all { } [::flagValueGet $ctrl -name] {_} ctrlName
	controlHide [format "%s.%s" $windowName $ctrlName] 1
    }
}

##############################################################################
#
# controlsCreate -
#
proc ::GenericToolPropertySheet::controlsCreate {windowName sheet handle} {
    variable _idx
    variable _ctrlList

    if {[string match $sheet "General"]} {
	set _ctrlList($sheet) [list \
	    [list text -name toolchaintext \
	    	-readonly -x 60 -y 38 -w 150 -h 12] \
	    [list text -name bsptext \
	    	-readonly -x 60 -y 62 -w 150 -h 12] \
	    [list label -name lblToolChain -title "ToolChain:" \
		-x 10 -y 38 -w 50 -h 10] \
	    [list label -name lblBsp -title "BSP:" \
	    	-x 10 -y 62 -w 50 -h 10] \
	]

	foreach ctrl $_ctrlList($sheet) {
	    controlCreate $windowName $ctrl
	}

    } elseif {[string match $sheet "Rules"]} {
	set _ctrlList($sheet) [list \
	    [list combo -name rulecombo -x 65 -y 33 -w 162 -h 78 \
		-border \
		-callback [list \
		::GenericToolPropertySheet::onSelChangeRulesCombo \
		$windowName]] \
	    [list text -name ruleInfoText -multiline \
		-border \
	    	-readonly -x 10 -y 70 -w 217 -h 40] \
	    [list button -name addrulebutton -x 25 -y 115 -w 50 -h 14 \
		-title "New/Edit..." -callback [list \
		::GenericToolPropertySheet::onEditRule $windowName]] \
	    [list button -name delrulebutton -x 160 -y 115 -w 50 -h 14 \
		-title "Delete" -callback [list \
		::GenericToolPropertySheet::onRuleDelete $windowName]] \
	    [list label -name lblNameCombo -title "Rule" \
		-x 10 -y 33 -w 50 -h 10] \
	    [list label -name ruleInfoTitle -title "Rule Description:" \
	    	-x 10 -y 58 -w 100 -h 10] \
	]

	foreach ctrl $_ctrlList($sheet) {
	    controlCreate $windowName $ctrl
	}

    } elseif {[string match $sheet "Macros"]} {

	set _ctrlList($sheet) [list \
	    [list combo -name macroscombo -x 42 -y 33 -w 185 -h 100 \
		-callback \
		[list ::GenericToolPropertySheet::onSelChangeMacrosCombo \
		$windowName]] \
	    [list text -name macroName \
		-border -multiline \
		-x 42 -y 48 -w 185 -h 25 \
		-callback \
		[list ::GenericToolPropertySheet::macroDirtyFlagSet \
		$windowName 1]] \
	    [list text -name macroValue \
		-border -multiline \
		-x 42 -y 75 -w 185 -h 25 \
		-autovscroll \
		-callback \
		[list ::GenericToolPropertySheet::macroDirtyFlagSet \
		$windowName 1]] \
	    [list text -name macroComment -multiline \
		-border \
		-vscroll \
		-x 42 -y 102 -w 185 -h 12 -readonly] \
	    [list button -name addmacrobutton -x 25 -y 115 -w 50 -h 14 \
		-title "Add/Set" -callback [list \
		::GenericToolPropertySheet::onMacroAdd $windowName]] \
	    [list button -name delmacrobutton -x 160 -y 115 -w 50 -h 14 \
		-title "Delete" -callback [list \
		::GenericToolPropertySheet::onMacroDelete $windowName]] \
	    [list label -name macroNameLbl \
	        -x 10 -y 50 -w 30 -h 14 -title "Name"] \
	    [list label -name macroValueLbl \
		-x 10 -y 75 -w 30 -h 14 -title "Value"] \
	    [list label -name lblNameComboMacros -title "Macros" \
		-x 10 -y 35 -w 30 -h 10] \
	    [list label -name macroCommentLbl \
		-x 10 -y 103 -w 32 -h 10 -title "Comment"] \
	]

	foreach ctrl $_ctrlList($sheet) {
	    controlCreate $windowName $ctrl
	}

    } elseif {[string match "Link Order" $sheet]} {
	set _ctrlList($sheet) [list \
	    [list button -name downbutton -title &Down \
		-callback [list ::GenericToolPropertySheet::onMove \
		    $windowName $sheet $handle 0]\
		-x 177 -y 32 -w 50 -h 14\
	    ]\
	    [list button -name upbutton -title &Up \
		-callback [list ::GenericToolPropertySheet::onMove \
		    $windowName $sheet $handle 1]\
		-x 177 -y 49 -w 50 -h 14\
	    ]\
	    [list list -name modulelist -nointegralheight\
		-x 16 -y 62 -w 142 -h 54\
	    ]\
	    [list label -name instrlabel\
		-title "Static C++ constructors are called\
		in module link order."\
		-x 10 -y 32 -w 158 -h 30\
	    ]\
	    [list group -name modulegroup -title "Specify module link order"\
		-x 10 -y 53 -w 157 -h 70 \
	    ]\
	]
	foreach ctrl $_ctrlList($sheet) {
	    controlCreate $windowName $ctrl
	}

    } elseif {[string match "C/C++ compiler" $sheet]} {

	# Create unique control names for the sheets 
	regsub -all { } $sheet {_} ctrlName
	set lblName [format "lbl1%s" $ctrlName] 
	set txtName [format "txt%s" $ctrlName] 
	set tableName [format "table%s" $ctrlName] 
	set _ctrlList($sheet) [list \
	    [list boolean -name booldebug -title "Include debug info" \
		-x 10 -y 30 -width 90 -height 14 -auto \
		-callback [list ::GenericToolPropertySheet::onBuildModeSet \
		    $windowName]] \
	    [list combo -name combobuildoptimization \
		-x 186 -y 30 -w 40 -h 40 \
		-callback \
		    [list ::GenericToolPropertySheet::onOptimizationCombo \
		    $windowName]] \
	    [list text -name $tableName -x 10 -y 45 -width 217 \
		-border \
		-vscroll -height 66 -multiline \
		-callback ::PSViewer::dirtyFlagSet] \
	    [list text -name $txtName -x 35 -y 115 -width 100 -h 14 \
		-callback ::PSViewer::dirtyFlagSet] \
            [list button -name IDD_INCLUDE_PATHS -title "Include paths..." \
                -x 150 -y 115 -w 75 -h 14 \
                -callback \
                    [list ::GenericToolPropertySheet::onEditIncludePaths \
                    $windowName.$tableName]] \
	    [list label -name $lblName \
		-x 10 -y 115 -w 20 -h 14 -title "Tool"] \
	    [list label -name lbloptimization \
		-x 114 -y 33 -w 60 -h 12 -title "Optimization level"] \
	]

	incr ::GenericToolPropertySheet::_index
	foreach ctrl $_ctrlList($sheet) {
	    controlCreate $windowName $ctrl
	}

	# controlColumnHeadersSet ${windowName}.$tableName {Flag Value}
	# controlColumnWidthsSet ${windowName}.$tableName {119 118}
    } elseif {[string match "assembler" $sheet]} {

	# Create unique control names for the sheets 
	regsub -all { } $sheet {_} ctrlName
	set lblName [format "lbl1%s" $ctrlName] 
	set txtName [format "txt%s" $ctrlName] 
	set tableName [format "table%s" $ctrlName] 
	set _ctrlList($sheet) [list \
	    [list text -name $tableName -x 10 -y 38 -width 217 \
		-border \
		-vscroll -height 73 -multiline \
		-callback ::PSViewer::dirtyFlagSet] \
	    [list label -name $lblName \
		-x 10 -y 115 -w 20 -h 14 -title "Tool"] \
	    [list text -name $txtName -x 35 -y 115 -width 100 -h 14 \
		-callback ::PSViewer::dirtyFlagSet] \
            [list button -name IDD_INCLUDE_PATHS2 -title "Include paths..." \
                -x 150 -y 115 -w 75 -h 14 \
                -callback \
                    [list ::GenericToolPropertySheet::onEditIncludePaths \
                    $windowName.$tableName]] \
	]

	incr ::GenericToolPropertySheet::_index
	foreach ctrl $_ctrlList($sheet) {
	    controlCreate $windowName $ctrl
	}

	# controlColumnHeadersSet ${windowName}.$tableName {Flag Value}
	# controlColumnWidthsSet ${windowName}.$tableName {119 118}
    } else {

	# Create unique control names for the sheets 
	regsub -all { } $sheet {_} ctrlName
	set lblName [format "lbl1%s" $ctrlName] 
	set txtName [format "txt%s" $ctrlName] 
	set tableName [format "table%s" $ctrlName] 
	set _ctrlList($sheet) [list \
	    [list text -name $tableName -x 10 -y 38 -width 217 \
		-border \
		-vscroll -height 73 -multiline \
		-callback ::PSViewer::dirtyFlagSet] \
	    [list label -name $lblName \
		-x 10 -y 115 -w 20 -h 14 -title "Tool"] \
	    [list text -name $txtName -x 35 -y 115 -width 193 -h 14 \
		-callback ::PSViewer::dirtyFlagSet] \
	]

	incr ::GenericToolPropertySheet::_index
	foreach ctrl $_ctrlList($sheet) {
	    controlCreate $windowName $ctrl
	}

	# controlColumnHeadersSet ${windowName}.$tableName {Flag Value}
	# controlColumnWidthsSet ${windowName}.$tableName {119 118}
    }

    OnHide $windowName $sheet $handle
}

##############################################################################
#
# controlsDestroy -
#
proc ::GenericToolPropertySheet::controlsDestroy {windowName sheet handle} {
    variable _ctrlList
    foreach ctrl $_ctrlList($sheet) {
	regsub -all { } [::flagValueGet $ctrl -name] {_} ctrlName
	controlDestroy ${windowName}.${ctrlName}
    }
}

##############################################################################
#
# dataIn -
#
proc ::GenericToolPropertySheet::dataIn {windowName handle sheet} {
    variable _sheetList
    variable _moduleMap

    set buildSpec [buildSpecGet]
    set hProj [hProjGet]
    ::beginWaitCursor

    if {[string match $sheet "General"]} {
	set toolchain [::prjBuildTcGet $hProj $buildSpec]
	regsub {(:*)tc_} $toolchain {} friendlyToolchain 
	if {![::Workspace::isToolchainInstalled $toolchain]} {
	    set friendlyToolchain "$friendlyToolchain (not installed!)"
	}
	set bsp [::Workspace::projectBspPathGet $hProj]
	if {$bsp == ""} {
	    set bsp "(None)"
	}
	controlValuesSet ${windowName}.toolchaintext $friendlyToolchain 
	controlValuesSet ${windowName}.bsptext $bsp 
    } elseif {[string match $sheet "Rules"]} {

	variable _rules
	catch {unset _rules} ; # force cache to be reloaded

	controlValuesSet ${windowName}.rulecombo [rulesGet]
	controlSelectionSet ${windowName}.rulecombo \
	    -string [::prjBuildRuleCurrentGet $hProj $buildSpec]

	# clear the need update flag.  the controlSelectionSet above
	# will cause a selchange event, causing the flag to be set, but
	# we really don't want it to be set.  -Ren
	variable _updateObjectsFlag
	set _updateObjectsFlag 0

    } elseif {[string match $sheet "Macros"]} {
        variable _macros
	catch {unset _macros}

	set macroList [lsort [macrosGet]]

        # Preserve the current selection if there is one
	set curSel [controlSelectionGet ${windowName}.macroscombo -string]
	set idx [lsearch -exact $macroList $curSel]
	if {$idx == -1} {
 	    set idx 0
	}	
	controlValuesSet ${windowName}.macroscombo $macroList
	controlSelectionSet ${windowName}.macroscombo $idx 

    } elseif {[string match $sheet "Link Order"]} {

	# Fetch the modules from the project and put them in the list
	if {![string match "nullObject" [::Object::evaluate $handle Type]]} {
	    set projectName [::Object::evaluate \
	        [::Object::evaluate $handle Project] Name]

	    set projName [::Object::evaluate \
		[::Object::evaluate $handle Project] Name]

	    # Add the derived objects 
	    set prjType [::prjTypeGet $hProj]
	    set hSpec [::prjBuildCurrentGet $hProj]
	    set buildRule \
		[::prjBuildRuleCurrentGet $hProj $hSpec]
            catch {unset _moduleMap}
	    set fileList [::prjFileListGet $hProj]
	    foreach file $fileList {
		set module [::prjFileInfoGet $hProj $file objects]
		if {$module != ""} {
		    set _moduleMap($module) $file 
		    lappend moduleList $module
		}
	    }
	    if {![info exists moduleList]} {
		set moduleList ""
	    }
	    controlValuesSet ${windowName}.modulelist $moduleList 
	}
    } elseif {[string match "C/C++ compiler" $sheet]} {

	# Set the tool
	set toolchain [::prjBuildTcGet $hProj $buildSpec]
	set toolns $_sheetList($sheet)
	set toolMacro [${toolns}::toolMacro]
	set macroValue [::prjBuildMacroGet $hProj $buildSpec $toolMacro]
	regsub -all { } $sheet {_} ctrlName
	controlTextSet [format "%s.txt%s" $windowName $ctrlName] $macroValue

	# Set tool's flags
	set flags [::prjBuildFlagsGet $hProj $buildSpec $_sheetList($sheet)]
	controlValuesSet [format "%s.table%s" $windowName $ctrlName] $flags 

	# Set Debug or Release radio button and optimization level combo
	initBuildModeStateSet $windowName $flags

    } else {

	# Set the tool
	set toolchain [::prjBuildTcGet $hProj $buildSpec]
	set toolns $_sheetList($sheet)
	set toolMacro [${toolns}::toolMacro]
	set macroValue [::prjBuildMacroGet $hProj $buildSpec $toolMacro]
	regsub -all { } $sheet {_} ctrlName
	controlTextSet [format "%s.txt%s" $windowName $ctrlName] $macroValue

	# Set tool's flags
	set flags [::prjBuildFlagsGet $hProj $buildSpec $_sheetList($sheet)]
	controlValuesSet [format "%s.table%s" $windowName $ctrlName] $flags 
    }
    ::endWaitCursor
}

proc ::GenericToolPropertySheet::onMove {window sheet handle {bUp 1}} {

    # Get the list, remove the item, and add it one element higher
    # in the list
    set selection [controlSelectionGet ${window}.modulelist -string]
    set idx [controlSelectionGet ${window}.modulelist]
    set contents [controlValuesGet ${window}.modulelist]

    if {($idx != -1) && ($idx != "")} {
	if {$bUp} {
  	    if {$idx > 0} {
		set contents [lreplace $contents $idx $idx]
		incr idx -1
		set contents [linsert $contents $idx $selection]
		controlValuesSet ${window}.modulelist $contents
		controlSelectionSet ${window}.modulelist $idx
	        ::PSViewer::dirtyFlagSet 1
	    } else {
		::messageBeep -ok
	    }
	} else {
	    if {$idx < [expr [llength $contents] - 1]} {
		set contents [lreplace $contents $idx $idx]
		incr idx 1
		set contents [linsert $contents $idx $selection]
		controlValuesSet ${window}.modulelist $contents
		controlSelectionSet ${window}.modulelist $idx
	        ::PSViewer::dirtyFlagSet 1
	    } else {
		::messageBeep -ok
	    }
	}
    }
}

# Parse the build flags and set the appropriate build mode radio button
# and the optimization level
proc ::GenericToolPropertySheet::initBuildModeStateSet {window flags} {

    variable _noevent

    set buildSpec [buildSpecGet]
    set hProj [hProjGet]
    set toolchain [::prjBuildTcGet $hProj $buildSpec]

    # Populate optimzation combo, look for optimation level and set
    if {[${toolchain}::family] == "diab"} {
	controlValuesSet ${window}.combobuildoptimization {none 1 2}
    } else {
	controlValuesSet ${window}.combobuildoptimization {none 1 2 3}
    }

    # Set a selection in the optimization combo.  Don't trigger
    # an event yet, 'cause we don't have the proper radio buttons
    # checked
    set optResult [buildOptimizationGet $flags]

    # We don't want the following to generate an event
    set _noevent 1
    controlSelectionSet ${window}.combobuildoptimization \
	[lindex $optResult 0]
    set _noevent 0

    # Look for -g, indicating debug symbols
    set bDebug [lindex [debugBuildGet $flags] 0]

    set _noevent 1
    controlCheckSet ${window}.booldebug $bDebug
    set _noevent 0
}

# Determine if compiler flags contains -g.  Return list containing
# result (0 or 1) followed by position of -g in list, or -1
proc ::GenericToolPropertySheet::debugBuildGet {flags} {
    set found 0
    set pos [lsearch -exact $flags "-g"]
    if {$pos != -1} {
	set found 1
    }
    return [list $found $pos]
}

# Determine the optimization level; return level and index into flags as list
proc ::GenericToolPropertySheet::buildOptimizationGet {flags} {
    set opt 0
    set buildSpec [buildSpecGet]
    set hProj [hProjGet]
    set toolchain [::prjBuildTcGet $hProj $buildSpec]

    # Search for optimization level
    if {[${toolchain}::family] == "diab"} {
	set pos -1 
	if {[regexp -indices -- {-O|-XO} $flags charPos]} {
	    set opt [string range $flags \
		    [lindex $charPos 0] [lindex $charPos 1]]

	    # regexp -indices returns char pos in string, not list pos,
	    # so we need to do the lsearch to get the position value
	    set pos [lsearch -exact $flags $opt]
	    if {$opt == "-O"} {
		set opt 1
	    } elseif {$opt == "-XO"} {
		set opt 2
	    } else {
		set opt 0
	    }
	}
	return [list $opt $pos]
    } else {
	set pos -1 
	if {[regexp -indices -- {-O[0123]} $flags charPos]} {
	    set opt [string range $flags \
		    [lindex $charPos 0] [lindex $charPos 1]]

	    # regexp -indices returns char pos in string, not list pos,
	    # so we need to do the lsearch to get the position value
	    set pos [lsearch -exact $flags $opt]
	    scan $opt {-O%d} opt
	} elseif {[regexp -indices -- {-O} $flags charPos]} {
	    set opt 1
	}
	return [list $opt $pos]
    }
}

# Callback for the build mode (debug, release) radio buttons and for
# optimization level combo box
proc ::GenericToolPropertySheet::onBuildModeSet {windowName} {
    variable _noevent

    if {! $_noevent} {
	set ctrlName "tableC/C++ compiler"
	regsub -all { } $ctrlName {_} ctrlName

	# Fetch the flags from the edit control
	set flags [controlTextGet ${windowName}.$ctrlName]

	# Determine the current optimization level from the combo box
	set opt [controlTextGet ${windowName}.combobuildoptimization] 
	if {[string match $opt "none"] || $opt == ""} {
	   set opt 0
	}

	# Find out which button was clicked (0 = debug, 1 = release)
	# and use it to:
	# 1) set debug flag
	# 2) set the optimization level
	# 3) enable/disable opt. combo box
	# 4) update the optimization combo
	if {[controlChecked ${windowName}.booldebug]} {
	    set bDebug 1
	    set opt 0
	} else {
	    set bDebug 0
	    if {$opt == 0} {
	    	set opt 2 ; # default optimization for release build!!!
	    }
	}
	set flags [debugStateSet $flags $bDebug]
	set flags [optimizationStateSet $flags $opt]
	set _noevent 1
	controlSelectionSet ${windowName}.combobuildoptimization $opt 
	set _noevent 0

	# Adjust the flags with the correct state for optimization
	# level and whether or not debug

	# Write the updated flags to the correct window
	controlTextSet ${windowName}.$ctrlName $flags

	# Mark the sheet as dirty, so we can save it
	::PSViewer::dirtyFlagSet 1
    }
}

    namespace eval ::IncludeOrder {
        variable _paths
	variable commit 0

        #######################################
        #
        # Fetch the include paths specified by -I
        # and return as a list
        # Returns: a list of include paths, no -Is
        #
        #######################################
        proc includePathsExtract {cmdLine} {
            # Fix those pesky backslashes before they bite us
            ::filePathFix cmdLine
            foreach word $cmdLine {
                if {[string match -I* $word]} {
                    scan $word "-I%s" path
		    lappend includePaths $path
                }
            }
            if {[info exists includePaths]} {
                return $includePaths
            } else { 
                return "" 
            }
        }

  
        #######################################
        #
        # Inject a list of include paths into 
        # a build command line.  Algorithm:
        # Locate first include directive, mark position
        # Delete all includes 
        # Insert all includes at marked position 
        #
        # PARAM: cmdLine -- the command line containing  -I include
        #        directives
        # PARAM: paths -- list of include paths to be added
        # RETURNS: the command line with include directives replaced 
        #        by items from the list 
        #
        #######################################
        proc includePathsInsert {cmdLine paths} {
            # Find insertion point
            for {set i 0} {$i < [llength $cmdLine]} {incr i} {
                set word [lindex $cmdLine $i]
                if {[string match -I* $word]} {
                    break
                }
            }
            if {$i == [llength $cmdLine]} {
                set i [expr [llength $cmdLine] - 2]
                if {$i < 0} {
                    set i 0
                }
            }

            # Remove old includes
	    regsub -all {\-I[^ ]*} $cmdLine "" cmdLine

	    # Remove empty elements
            foreach elem [split $cmdLine] {
                if {$elem != ""} {
                    lappend tmp $elem 
                } 
            }
            set cmdLine $tmp 

            # Insert new includes
            set expandedIncludes ""
            foreach include $paths {
                lappend expandedIncludes -I$include 
            }
            # Get rid of '{}' quoting Tcl attempts to supply if we insert '$'
            # (e.g, "$(PRJ_DIR)")
            set cmdLine [linsert $cmdLine $i $expandedIncludes]
            regsub -all {[\{\}]} $cmdLine "" cmdLine 
            return $cmdLine
        }

        proc getInitialDir {} {
            set hProj [::GenericToolPropertySheet::hProjGet]
            set curSel [file nativename [controlSelectionGet \
                IncludeOrder::IDD_INCLUDE_PATH.includeList -string]]
            if {$curSel == ""} {
                set curSel [file dirname [prjInfoGet $hProj fileName]]
            }
            return $curSel
        }

        proc init {paths} {
            set IncludeOrder::_paths $paths
            set controls {
                {button -title "OK" -name ID_OK \
                    -callback ::IncludeOrder::onOk \
                    -xpos 250 -ypos 10 -width 50 -h 14}
                {button -title "Cancel" -name ID_CANCEL \
                    -callback {windowClose IncludeOrder::IDD_INCLUDE_PATH} \
 	                -xpos 250 -ypos 30 -width 50 -height 14}
                {button -title "Add..." -name ID_ADD \
                    -callback {::UpdateDialog::onUpdate \
                        IncludeOrder::IDD_INCLUDE_PATH \
                        [::IncludeOrder::getInitialDir] \
                        ::IncludeOrder::onUpdateCallback 0} \
     	                -xpos 10 -ypos 110 -width 50 -height 14}
                {button -title "Remove" -name ID_REMOVE \
                    -callback IncludeOrder::onRemove \
                    -xpos 70 -ypos 110 -width 50 -height 14}
                {button -title "Edit..." -name ID_UPDATE \
                    -callback {::UpdateDialog::onUpdate \
                        IncludeOrder::IDD_INCLUDE_PATH \
                        [::IncludeOrder::getInitialDir] \
                        ::IncludeOrder::onUpdateCallback 1} \
     	            -xpos 130 -ypos 110 -width 50 -height 14}
                {button -title "Move Up" -name ID_MOVE_UP \
                    -callback {IncludeOrder::onMove 1} \
                    -xpos 190 -ypos 110 -width 50 -height 14}
                {button -title "Move Down" -name ID_MOVE_DOWN \
                    -callback {IncludeOrder::onMove 0} \
                    -xpos 250 -ypos 110 -width 50 -height 14}
                {list -name includeList -xpos 5 -ypos 10 \
                    -width 240 -height 100}
            }
            foreach control $controls {
                controlCreate IncludeOrder::IDD_INCLUDE_PATH $control
            }

            controlValuesSet IncludeOrder::IDD_INCLUDE_PATH.includeList \
                $IncludeOrder::_paths
            newSelectionSet 
        }

        proc onUpdateCallback {selection} {
            set oldSelection [controlSelectionGet \
                IncludeOrder::IDD_INCLUDE_PATH.includeList -string]
            set iSelection [controlSelectionGet \
                IncludeOrder::IDD_INCLUDE_PATH.includeList]
            set values [controlValuesGet \
                IncludeOrder::IDD_INCLUDE_PATH.includeList]
    
            if {[::UpdateDialog::isUpdate]} {
                set values [lreplace $values $iSelection $iSelection \
                    $selection]
            } else {
                lappend values $selection
            }
            controlValuesSet IncludeOrder::IDD_INCLUDE_PATH.includeList \
                $values
            controlSelectionSet IncludeOrder::IDD_INCLUDE_PATH.includeList\
                -string $selection 
        }

        proc onOk {} {
            set IncludeOrder::commit 1
	    set IncludeOrder::_paths \
                [controlValuesGet IncludeOrder::IDD_INCLUDE_PATH.includeList]
            windowClose IncludeOrder::IDD_INCLUDE_PATH
	}

        proc onRemove {} {
            controlValuesSet IncludeOrder::IDD_INCLUDE_PATH.includeList \
                -string -delete \
                [controlSelectionGet IncludeOrder::IDD_INCLUDE_PATH.includeList\
                    -string]
            newSelectionSet
            buttonsStatesUpdate 
        }

        proc onMove {bUp} {
            set iSelection [controlSelectionGet \
                IncludeOrder::IDD_INCLUDE_PATH.includeList]
            set selection [controlSelectionGet \
                IncludeOrder::IDD_INCLUDE_PATH.includeList -string]
            set includes [controlValuesGet \
                IncludeOrder::IDD_INCLUDE_PATH.includeList]

            if {$iSelection == 0 && $bUp == 1} {
                messageBeep
                return 
            } elseif {$iSelection == [expr [llength $includes] - 1] \
	        && $bUp == 0} {
                messageBeep
                return
            } 

            if {$bUp == 1} {
                # Swap this with its predecessor
                set iPredecessor [expr $iSelection - 1]
                set predecessor [lindex $includes $iPredecessor] 
                set includes [lreplace $includes $iPredecessor \
                    $iPredecessor $selection]
                set includes [lreplace $includes $iSelection \
                    $iSelection $predecessor]
            } else {
                # Swap this with its successor 
                set iSuccessor [expr $iSelection + 1]
                set successor [lindex $includes $iSuccessor] 
                set includes [lreplace $includes $iSuccessor \
                    $iSuccessor $selection]
                set includes [lreplace $includes $iSelection \
                    $iSelection $successor]
            } 
            controlValuesSet IncludeOrder::IDD_INCLUDE_PATH.includeList \
                $includes
            controlSelectionSet IncludeOrder::IDD_INCLUDE_PATH.includeList\
                -string $selection 
        }

        proc buttonsStatesUpdate {} {
            set selection [controlSelectionGet \
                IncludeOrder::IDD_INCLUDE_PATH.includeList -string]
            if {$selection != ""} {
                controlEnable IncludeOrder::IDD_INCLUDE_PATH.ID_REMOVE 1
                controlEnable IncludeOrder::IDD_INCLUDE_PATH.ID_UPDATE 1
            } else {
                controlEnable IncludeOrder::IDD_INCLUDE_PATH.ID_REMOVE 0
                controlEnable IncludeOrder::IDD_INCLUDE_PATH.ID_UPDATE 0
            }
        } 

	proc newSelectionSet {} {
            controlSelectionSet IncludeOrder::IDD_INCLUDE_PATH.includeList 0
	}

        proc onIncludes {cmdLine} {
            set paths [::IncludeOrder::includePathsExtract $cmdLine]
            dialogCreate -name IncludeOrder::IDD_INCLUDE_PATH \
                -parent ::PSViewer::psWindow \
                -title "Add/Remove/Update Header Include Paths" \
                -width 310 -height 130 -init {init $paths}
            set cmdLine [::IncludeOrder::includePathsInsert \
                $cmdLine $::IncludeOrder::_paths]
            return $cmdLine
        }
    }

# Callback to launch include path editor 
proc ::GenericToolPropertySheet::onEditIncludePaths {sourceText} {

    # Get tool's flags
    set flags [controlTextGet $sourceText]

    # Show include path editor
    set flags [IncludeOrder::onIncludes $flags]
    if {$::IncludeOrder::commit == 1} {
       controlTextSet $sourceText $flags
       ::PSViewer::dirtyFlagSet 1
    } 
}

# Callback for the optimization level combo box
proc ::GenericToolPropertySheet::onOptimizationCombo {windowName} {
    variable _noevent
    if {!$_noevent && [string match "selchange" \
	[controlEventGet ${windowName}.combobuildoptimization]]} {
	set ctrlName "tableC/C++ compiler"
	regsub -all { } $ctrlName {_} ctrlName

	# Get the optimization level from the combo
	set opt [controlTextGet ${windowName}.combobuildoptimization]
	if {[string match $opt "none"] || $opt == ""} {
	   set opt 0
	}

	# Fetch the flags from the edit control
	set flags [controlTextGet ${windowName}.$ctrlName]

	# Determine if flags contain -g
	set bDebug [lindex [debugBuildGet $flags] 0]

	# Adjust the flags with the correct state for optimization level
	set flags [optimizationStateSet $flags $opt]
	if {[expr $opt > 0]} {

		if {[expr $bDebug]} {
	    	# Warn user about optimizing debug code
	    	set result [messageBox -yesno -exclamationicon \
			"Warning: compiling with both debug info (-g)\
			\nand optimization will cause source stepping in the debugger\
			\nto produce unpredictable results.  Continue anyway?"]

	    	if {[string match $result "no"]} {
				return
			}
		}
	}

	# Set the flags text
	controlTextSet ${windowName}.$ctrlName $flags 

	# Mark the sheet as dirty, so we can save it
	::PSViewer::dirtyFlagSet 1
    }
}

# Determine if radio button has been set to debug, and if so,
# and if it's not already there, put -g in the flags window
proc ::GenericToolPropertySheet::debugStateSet {flags bDebug} {
    set debugResult [::GenericToolPropertySheet::debugBuildGet $flags]
    if {$bDebug} {

    	# Debug mode
    	if {[lindex $debugResult 1] == -1} {
	    set flags "-g $flags" ; # not found, prepend it
    	}
    } else {

    	# Release mode
	set idx [lindex $debugResult 1]
	if {$idx != -1} {
	    set flags [lreplace $flags $idx $idx]
	}
    }
    return $flags
}

# Write the optimization level into the flags if it exists,
# or prepend it if not
proc ::GenericToolPropertySheet::optimizationStateSet {flags opt} {
    set optResult [::GenericToolPropertySheet::buildOptimizationGet $flags]
    set pos [lindex $optResult 1]
    set buildSpec [buildSpecGet]
    set hProj [hProjGet]
    set toolchain [::prjBuildTcGet $hProj $buildSpec]

    if {[${toolchain}::family] == "diab"} {
	# determine which Diab option to use
	if {$opt == 1} {
	    set opt "-O"
	} elseif {$opt == 2} {
	    set opt "-XO"
	} else {
	    set opt ""
	}

	if {$pos == -1} {
	    set flags "$opt $flags"
	} else {
	    if { $opt == "" } {
		# if we aren't optimizing, we use lrange to
		# remove the flag, avoiding the {} placehodler
		set flags "[lrange $flags 0 [expr $pos -1]] \
			[lrange $flags [expr $pos +1] end]"
	    } else {
		set flags [lreplace $flags $pos $pos "$opt"]
	    }
	}
    } else {
	if {$pos == -1} {
	    set flags "-O$opt $flags"
	} else {
	    set flags [lreplace $flags $pos $pos "-O$opt"]
	}
    }
}

proc ::GenericToolPropertySheet::ruleGet {ruleName} {
    variable _rules
    set retval ""
    if {[info exists _rules($ruleName)]} {
   	set retval $_rules($ruleName)
    }
    return $retval
}
 
proc ::GenericToolPropertySheet::rulesGet {} {
    variable _rules 
    set retval ""

    set hProj [hProjGet]
    set buildSpec [buildSpecGet]

    if {![info exists _rules]} {
	rulesRead $hProj $buildSpec
    }

    # Filter out rules related to source files
    set rules [array names _rules]
    set fileList [::prjFileListGet $hProj]
    foreach fileName $fileList {
	set rule [::prjFileInfoGet $hProj $fileName customRule]	
	if {$rule != ""} {
	    set idx [lsearch $rules $rule]
	    if {$idx != -1} {
		set rules [lreplace $rules $idx $idx]
	    }
	}
    }

    # Sort and return the remaining rules
    set rules [lsort $rules]
    return $rules
}

proc ::GenericToolPropertySheet::rulesRead {hProj buildSpec} {
    variable _rules

    set rules [::prjBuildRuleListGet $hProj $buildSpec]

    # Put the rules into an array
    foreach rule $rules {
	set _rules($rule) [::prjBuildRuleGet $hProj $buildSpec $rule]
    }
}

proc ::GenericToolPropertySheet::ruleAdd {target rule} {
    variable _rules
    set _rules($target) $rule 
}

proc ::GenericToolPropertySheet::ruleDelete {target} {
    variable _rules
    catch {unset _rules($target)}
}

proc ::GenericToolPropertySheet::rulesWrite {hProj buildSpec} {
    variable _rules

    # Delete all the original rules, then update with new ones
    set oldRules [::prjBuildRuleListGet $hProj $buildSpec]
    foreach oldRule $oldRules {
	::prjBuildRuleDelete $hProj $buildSpec $oldRule
    }
    if {[info exists _rules]} {
    	foreach rule [array names _rules] {
	    ::prjBuildRuleSet $hProj $buildSpec $rule $_rules($rule)
	}
    }
}

proc ::GenericToolPropertySheet::macroGet {macroName} {
    variable _macros
    set retval ""

    if {![info exists _macros]} {
        set hProj [hProjGet]
        set buildSpec [buildSpecGet]
        macrosRead $hProj $buildSpec
    }

    if {[info exists _macros($macroName)]} {
   	set retval $_macros($macroName)
    }
    return $retval
}

proc ::GenericToolPropertySheet::macrosGet {} {
    variable _macros

    if {![info exists _macros]} {
        set hProj [hProjGet]
        set buildSpec [buildSpecGet]
        macrosRead $hProj $buildSpec
    }
    set retval [lsort [array names _macros]]

    # Remove LDDEPS from return value 
    set idx [lsearch $retval "LDDEPS"]
    if {$idx != -1} {
        set retval [lreplace $retval $idx $idx]
    }
    return $retval
}

# Load the macro cache for a given project and build spec
# No need to call this; macroGet/macrosGet does it
proc ::GenericToolPropertySheet::macrosRead {hProj buildSpec} {
    variable _macros

    foreach macro [::prjBuildMacroListGet $hProj $buildSpec] {
        set _macros($macro) [::prjBuildMacroGet $hProj $buildSpec $macro]
    }

    # Fetch LDDEPS too, as prjBuildMacroListGet won't return it
    set _macros(LDDEPS) [::prjBuildMacroGet $hProj $buildSpec LDDEPS]
}

proc ::GenericToolPropertySheet::macroAdd {name value} {
    variable _macros
    set _macros($name) $value 
}

proc ::GenericToolPropertySheet::macroDelete {name} {
    variable _macros
    catch {unset _macros($name)}
}

proc ::GenericToolPropertySheet::macrosWrite {hProj buildSpec} {
    variable _macros

    # Delete all the original macros, then update with new ones
    set oldMacros [::prjBuildMacroListGet $hProj $buildSpec]
    foreach oldMacro $oldMacros {
	::prjBuildMacroRemove $hProj $buildSpec $oldMacro
    }

    if {[info exists _macros]} {
        foreach macro [array names _macros] {
            ::prjBuildMacroSet $hProj $buildSpec $macro $_macros($macro)
        }
    }
}

##############################################################################
#
# dataCommit
#   Author: Jeff Rennie
#  This function is called by PSManager's OnApply function.  It checks to
#  see if the derived objects need to be recalculated, and if so sends the
#  appropriate message

proc ::GenericToolPropertySheet::dataCommit { args } {
    variable _updateObjectsFlag
    if $_updateObjectsFlag {
	set _updateObjectsFlag 0
	::Workspace::eventSend ::GenericToolPropertySheet \
	    [list DerivedObjectsUpdate [::Workspace::projectNameGet \
	    [::GenericToolPropertySheet::hProjGet]]]
    }
}

##############################################################################
#
# dataDiscard -- drop data in caches so that next time, it's read afresh
# 

proc ::GenericToolPropertySheet::dataDiscard {args} {
    # Clear the cache, since we know the project is synch'd
    # That way, we'll get a clean copy of what's in the project next time
    variable _macros
    variable _rules
    catch {unset _macros}
    catch {unset _rules}
}

##############################################################################
#
# dataOut -
#
proc ::GenericToolPropertySheet::dataOut {windowName handle sheet} {
    variable _sheetList
    variable _moduleMap

    set hProj [hProjGet]
    set buildSpec [buildSpecGet]

    # Special case "Rules"
    if {[string match $sheet "Rules"]} {

	set rule [controlSelectionGet ${windowName}.rulecombo -string]
	rulesWrite $hProj $buildSpec
	prjBuildRuleCurrentSet $hProj $buildSpec $rule

    } elseif {[string match $sheet "Macros"]} {

	# Write the new macro values
	macrosWrite $hProj $buildSpec

    } elseif {[string match $sheet "Link Order"]} {
	# Map the modules to source files, using the module names to
	# effectively re-order the source files.  Splice in the ones
	# that didn't map
	set prjFiles [prjFileListGet $hProj]

	foreach module [controlValuesGet ${windowName}.modulelist] {
	    if {[info exists _moduleMap($module)]} {
		lappend fileList $_moduleMap($module)
	    }
	}
	foreach file $prjFiles { 
	    if {[lsearch $fileList $file] == -1} {
		lappend fileList $file 
	    }			
	}

	# Set the project's filelist to this new order
	if {[info exists fileList]} {
	    ::prjTagDataSet $hProj PROJECT_FILES $fileList
	}

    } elseif {[string match $sheet "General"]} {
	# Do nothing
    } else {

	# Set the tool binary
	set toolns $_sheetList($sheet)
	regsub -all { } $sheet {_} ctrlName
	set ctrlName [format "txt%s" $ctrlName] 
	set toolExe [controlTextGet ${windowName}.$ctrlName] 
	set toolMacro [${toolns}::toolMacro]
	prjBuildMacroSet $hProj $buildSpec $toolMacro $toolExe

	# Set the tool's flags
	set toolchain [::prjBuildTcGet $hProj $buildSpec]
	regsub -all { } $sheet {_} ctrlName
	set flags [controlValuesGet [format "%s.table%s" $windowName $ctrlName]]
	::prjBuildFlagsSet $hProj $buildSpec $toolns $flags
    }
    # Return success
    return 1
}

##############################################################################
#
# onSelChangeRulesCombo -
#
proc ::GenericToolPropertySheet::onSelChangeRulesCombo {windowName} { 
    set hProj [hProjGet]
    set buildSpec [buildSpecGet]
    
    set selectedRule [controlSelectionGet ${windowName}.rulecombo -string]
    set ruleCommands [ruleGet $selectedRule]

    set data ""
    set targetName ""
    set dependencies ""

    if {"$ruleCommands" != ""} {
	set ruleDesc "Custom Rule"
    } else {
	set ruleDesc [prjBuildRuleDescGet $hProj $buildSpec $selectedRule]
    }
    controlValuesSet ${windowName}.ruleInfoText $ruleDesc

    set event [controlEventGet ${windowName}.rulecombo]
    if {[string match $event "selchange"]} {
	::PSViewer::dirtyFlagSet 1
		variable _updateObjectsFlag
		set _updateObjectsFlag 1
    }
}

##############################################################################
#
# onSelChangeMacrosCombo -
#
proc ::GenericToolPropertySheet::onSelChangeMacrosCombo {windowName} { 
    set hProj [hProjGet]
    set buildSpec [buildSpecGet]

    set curSel [controlSelectionGet ${windowName}.macroscombo -string]

    set macroValue [macroGet $curSel] 
    set macroDesc \
	[prjBuildMacroDescGet $hProj $buildSpec $curSel]

    controlValuesSet ${windowName}.macroName $curSel
    controlValuesSet ${windowName}.macroValue $macroValue
    controlValuesSet ${windowName}.macroComment $macroDesc
    
    macroDirtyFlagSet $windowName 0
}

##############################################################################
#
# onEditRule - Callback for "Edit/New" button in rule editor dialog
#
proc ::GenericToolPropertySheet::onEditRule {windowName} {
    set title "Create or Edit Rule"

    dialogCreate -name ruleEditDlg \
	-title $title -width 250 -height 150 \
	-init [list ::GenericToolPropertySheet::onEditRuleInit $windowName]
}

##############################################################################
#
# onEditRuleInit - initialization routine for rule editor dialog 
#
proc ::GenericToolPropertySheet::onEditRuleInit {parentWindow} {
    set ctrls [list \
	[list label -name tgtTxt -title "&Target" \
		-x 14 -y 38 -w 68 -h 10] \
	[list text -name ruleName -x 14 -y 48 -w 68 -h 14 \
	    -callback {::GenericToolPropertySheet::buildRuleEditDlgDirtyFlagSet 1}] \
	[list label -name depTxt -title "&Dependencies" \
		-x 86 -y 38 -w 145 -h 10] \
	[list text -name ruleDepend -x 86 -y 48 -w 145 -h 14 \
	    -callback {::GenericToolPropertySheet::buildRuleEditDlgDirtyFlagSet 1}] \
	[list label -name cmdTxt -title "Co&mmands" \
		-x 14 -y 69 -w 40 -h 10] \
	[list text -name ruleCommands -x 54 -y 64 -w 177 -h 36 \
	    -callback {::GenericToolPropertySheet::buildRuleEditDlgDirtyFlagSet 1} \
		-vscroll -multiline] \
	[list boolean -name buildFirstBool \
		-title "&Invoke this rule before building project" \
	    -callback {::GenericToolPropertySheet::buildRuleEditDlgDirtyFlagSet 1} \
		-auto -x 14 -y 102 -w 147 -h 10] \
   	[list button -name okbutton -x 86 -y 124 -w 50 -h 14 \
		-title "&OK" -callback [list \
		::GenericToolPropertySheet::onRuleEditOk $parentWindow]] \
   	[list button -name cancelbutton -x 140 -y 124 -w 50 -h 14 \
		-title "&Cancel" \
		-callback ::GenericToolPropertySheet::onRuleEditCancel] \
   	[list button -name helpbutton -x 194 -y 124 -w 50 -h 14 \
	    -title "&Help" -helpbutton] \
	[list group -title "Make Rule" -x 4 -y 4 -w 240 -h 112] \
	[list label -name lblRuleDesc -title \
		"Enter a target, dependencies, and commands to create\
 		\na GNU make rule." -x 14 -y 18 -w 225 -h 20] \
	[list label -name ruleSeparator -title ":" \
		-x 83 -y 51 -w 2 -h 10] \
    ]
    foreach ctrl $ctrls {
	controlCreate ruleEditDlg $ctrl
    }

    set hProj [hProjGet]
    set buildSpec [buildSpecGet]

    set selectedRule [controlSelectionGet ${parentWindow}.rulecombo -string]
    set ruleCommands [ruleGet $selectedRule]

    set data ""
    set targetName "<New Target>"
    set dependencies ""

    if {"$ruleCommands" != ""} {
   	set split [::buildRuleSplit $ruleCommands]
	set targetName [lindex $split 0]
	set dependencies [lindex $split 1]
	set data [lindex $split 2]
    } 
    set checkState 1
    controlValuesSet ruleEditDlg.ruleName $targetName
    controlValuesSet ruleEditDlg.ruleDepend $dependencies
    controlValuesSet ruleEditDlg.ruleCommands $data

    # Fetch the value of LDDEPS from the cache
    set rulesList [macroGet "LDDEPS"]
    if {$ruleCommands != ""} {
	if {[lsearch $rulesList $targetName] != -1} {
	    set checkState 1
	} else {
            set checkState 0
	}
    }
    controlCheckSet ruleEditDlg.buildFirstBool $checkState 

    ::GenericToolPropertySheet::buildRuleEditDlgDirtyFlagSet 0
}

##############################################################################
#
# buildRuleEditDlgDirtyFlagSet - 
#
proc ::GenericToolPropertySheet::buildRuleEditDlgDirtyFlagSet {value} {
    variable _buildDlgDirtyFlag

    controlEnable ruleEditDlg.okbutton $value 
    set _buildDlgDirtyFlag $value
}

##############################################################################
#
# macroDirtyFlagSet - 
#
proc ::GenericToolPropertySheet::macroDirtyFlagSet {windowName value} {
    controlEnable ${windowName}.delmacrobutton [expr !$value]
    controlEnable ${windowName}.addmacrobutton $value
}

##############################################################################
#
# onRuleEditOk - Callback for "OK" button in rule editor dialog 
#
proc ::GenericToolPropertySheet::onRuleEditOk {parentWindow} {

    set hProj [hProjGet]
    set buildSpec [buildSpecGet]
    set ruleToAdd ""

    # Get rule from the edit box
    set ruleName [controlValuesGet ruleEditDlg.ruleName]
    set dependencies [controlValuesGet ruleEditDlg.ruleDepend]
    set commands [controlValuesGet ruleEditDlg.ruleCommands]

    if {$ruleName != ""} {
	if {$commands != "" || $dependencies != ""} {
	    set rule [::buildRuleJoin $ruleName $dependencies $commands]

	    # Add rule to project
	    ruleAdd $ruleName $rule

	    # Update the rule list and set the new selection
	    set ruleList [rulesGet] 
	    controlValuesSet ${parentWindow}.rulecombo $ruleList
	    controlSelectionSet ${parentWindow}.rulecombo -string $ruleName

	    # If requested, add the rule to load dependencies macro
    	    if {[controlChecked ruleEditDlg.buildFirstBool]} {
    		# add this rule to LDDEPS macro
		loadDepAdd $parentWindow $ruleName 1
    	    } else {
		loadDepAdd $parentWindow $ruleName 0
    	    }

	    ::PSViewer::dirtyFlagSet 1
    	    windowClose ruleEditDlg
	} else {
	    messageBox -ok -exclamationicon \
	    	"Cannot add rule \"$ruleName\": \
		\nrule must have commands or dependencies."
	    controlFocusSet ruleEditDlg.ruleCommands
	}
    }
}

##############################################################################
#
# onRuleEditCancel - Callback for "Cancel" button in rule editor dialog 
#
proc ::GenericToolPropertySheet::onRuleEditCancel {} {
    windowClose ruleEditDlg
}

##############################################################################
#
# onRuleDelete - Callback for "Delete" button in rule editor dialog
#
proc ::GenericToolPropertySheet::onRuleDelete {windowName} {

    set hProj [hProjGet]
    set buildSpec [buildSpecGet]
    set ruleName ""

    # Get the selected rule
    set ruleName [controlSelectionGet ${windowName}.rulecombo -string]

    set srcFile [file tail \
	[::FilePropertySheet::sourceFileRule $hProj $ruleName]]

    if {$srcFile != ""} {
	# Check to see that the rule doesn't build a source file
	messageBox -ok -exclamationicon \
	    "Rule '$ruleName' builds source file '$srcFile'. \
	    \nTo remove this rule, go to the file view, right mouse click \
	    \n'$srcFile', select 'Properties', and select the 'Build' \
	    \npane. Check the 'Use default build rule...' box to remove \
	    \nthe custom rule for this file" 
    } else {

	# Check to see that this isn't a default rule.  We can't delete
	# those
	set prjRules [[prjTypeGet $hProj]::buildRuleListGet $hProj] 
	if {[lsearch $prjRules $ruleName] == -1} {
	    ruleDelete $ruleName

	    # Update the rule sheet
	    set ruleList [rulesGet]
	    controlValuesSet ${windowName}.rulecombo $ruleList
	    controlSelectionSet ${windowName}.rulecombo \
	        -string [lindex $ruleList 0]

	    # Remove the associated item from the LDDEP macro
	    loadDepAdd $windowName $ruleName 0

	    ::PSViewer::dirtyFlagSet 1
	} else {
	    messageBox -ok -stopicon "Default rules can't be deleted!"
	}
    }
}

##############################################################################
#
# loadDepAdd - Routine to add or remove a rule from the LDDEPS macro 
#
proc ::GenericToolPropertySheet::loadDepAdd {windowName ruleName bAdd} {
    set hProj [hProjGet]
    set buildSpec [buildSpecGet]

    set rulesList [macroGet "LDDEPS"]

    if {$bAdd} {
    	# add this rule to LDDEPS macro
	if {[lsearch $rulesList $ruleName] == -1} {
	    set rulesList [concat $rulesList $ruleName]
	}
    } else {
    	# remove this rule from LDDEPS macro
	set idx [lsearch -exact $rulesList $ruleName]
	if {$idx != -1} {
	    set rulesList [lreplace $rulesList $idx $idx]
	}
    }

    macroAdd "LDDEPS" $rulesList
  
    # Repopulate the macros combo and refresh the macro page
    controlValuesSet ${windowName}.macroscombo [macrosGet] 
    controlSelectionSet ${windowName}.macroscombo -string "LDDEPS"
}

##############################################################################
#
# onMacroDelete - Callback for "Delete" button in Macro tab
#
proc ::GenericToolPropertySheet::onMacroDelete {windowName} {

    set hProj [hProjGet]
    set buildSpec [buildSpecGet]
    set macroName [controlSelectionGet ${windowName}.macroscombo -string]

    if {$macroName != ""} {
	# Check to see if this is LDDEPS, and if so, warn of the consequences 
	set status ok
	if {[string match $macroName LDDEPS]} {
	    set status [messageBox -okcancel -exclamationicon \
		"Macro LDDEPS is used to build custom rules. \
		\nIf you delete this macro, files with custom build \
		\nrules and any other custom build rules you have \
		\ndefined, will not be built.  Delete anyway?"]
	}
	
	if {[string match $status ok]} {
	    macroDelete $macroName

	    # reinitialize macro list
	    set macroList [macrosGet]
	    controlValuesSet ${windowName}.macroscombo $macroList

	    # display the first macro of the list
	    set firstMacro [lindex $macroList 0]
	    controlSelectionSet ${windowName}.macroscombo -string $firstMacro
	    set macroValue [macroGet $firstMacro]
	    set macroDesc [prjBuildMacroDescGet $hProj $buildSpec $firstMacro]
	    controlValuesSet ${windowName}.macroName $firstMacro
	    controlValuesSet ${windowName}.macroValue $macroValue
	    controlValuesSet ${windowName}.macroComment $macroDesc
	    macroDirtyFlagSet $windowName 0
	    ::PSViewer::dirtyFlagSet 1
	}
    }
}

##############################################################################
#
# onMacroAdd - Callback for "Add/Set" button in Macro tab
#
proc ::GenericToolPropertySheet::onMacroAdd {windowName} {

    set hProj [hProjGet]
    set buildSpec [buildSpecGet]

    # add new macro
    set macroName [controlTextGet ${windowName}.macroName]
    set macroValue [controlTextGet ${windowName}.macroValue]

    if {$macroName != ""} {
	macroAdd $macroName $macroValue 

	# reinitialize macro list
	set macroList [macrosGet]
	controlValuesSet ${windowName}.macroscombo $macroList

	# display the first macro of the list
	set firstMacro [lindex $macroList 0]
	controlSelectionSet ${windowName}.macroscombo -string $firstMacro
	set macroValue [macroGet $firstMacro]
	set macroDesc [prjBuildMacroDescGet $hProj $buildSpec $firstMacro]
	controlValuesSet ${windowName}.macroName $firstMacro
	controlValuesSet ${windowName}.macroValue $macroValue
	controlValuesSet ${windowName}.macroComment $macroDesc

	macroDirtyFlagSet $windowName 0
	::PSViewer::dirtyFlagSet 1
    }
}

##############################################################################
#
# buildSpecGet - Get current build spec
#
proc ::GenericToolPropertySheet::buildSpecGet {} {
    set curHandle [::Workspace::selectionGet]
    return [::Object::evaluate $curHandle Name]
}

##############################################################################
#
# hProjGet - Get current project handle
#
proc ::GenericToolPropertySheet::hProjGet {} {
    set curHandle [::Workspace::selectionGet]
    set prjObj [::Object::evaluate $curHandle Project]
    return [::Object::evaluate $prjObj Handle]
}

# This must be done last, to allow interface validation
::Workspace::propPageRegister ::GenericToolPropertySheet
