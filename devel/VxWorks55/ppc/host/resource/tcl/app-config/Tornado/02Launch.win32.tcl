##############################################################################
# 02Launch.win32.tcl - Tornado (Windows) Tool Launching
#
# Copyright (C) 1995-96 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01d,09feb99,jak  remove references to on_targetManage_help
# 01c,18jan99,jak  separate target manage code
# 01b,12jan99,jak  use true value of WDB Agent 
# 01a,24apr96,j_k  written. Diverged from 01Launch.win32.tcl.
#

# DESCRIPTION
#	This script contains procedures used for selecting a target and
# launching tools. 


# globals

# User selected tool to be launched
set launchDlgGlobals(tool)		""

# Whether Target server manager dialog chosen
set launchDlgGlobals(tsManage)	0



##############################################################################
#
# targetInfoGet - return the target server information
#
# returns Target Server Information given its name.  This procedure formats 
# the information such that it can be displayed in a Windows edit control.
#
# SYNOPSIS:
#   targetInfoGet
#
# RETURNS:
#   formatted text with "\r\n"s embedded in them for display purposes
#
# ERRORS:
#   - Tool attach failed.
#   - Target not Attached.
#   - Couldn't obtain Target Server Infomation
#
proc targetInfoGet {ts} {
	global cpuType

	# First try to attach to the tool without any errors.
	# If any, return the error to be displayed for the user.
	if {[catch {wtxToolAttach $ts -launcher} attachRetVal]} {
	    return "Tool attach failed.\r\n$attachRetVal"
	}

	# Next try to get the Target Server information.
	# If any errors, report to the user
	if [catch "wtxTsInfoGet" info] {
		if {[lindex $info 3] == "(SVR_TARGET_NOT_ATTACHED)"} {
			# wtxToolDetach -all
			wtxToolDetach
			return "Target not Attached"
		}
		return "Couldn't obtain Target Server Information"
	}
	# If we get here, looks pretty good and we can start formatting
	# the data for displaying
	set link			[lindex $info 0]
	set typeId			[lindex [lindex $info 2] 0]
	set tgtsvr_cpu		$cpuType($typeId)
	set tgtsvr_cpuid    $typeId
	set tgtsvr_rtvers	"VxWorks [lindex [lindex $info 3] 1]"
	set tgtsvr_bsp		[lindex $info 4]
	set tgtsvr_core		[lindex $info 5]
	set memSize			[lindex [lindex $info 6] 1]
	set tgtsvr_user		[lindex $info 8]
	set start			[lindex $info 9]
	set access			[lindex $info 10]
	set tgtsvr_lock		[lindex $info 11]
	set tgtsvr_ver      [lindex $info 13]
	set agentVersion	[lindex $info 14]	

	set infoString              "Name   : $ts"
	set infoString "$infoString\nVersion: $tgtsvr_ver"
	set infoString "$infoString\nStatus : $tgtsvr_lock"
	set infoString "$infoString\nRuntime: $tgtsvr_rtvers"
	set infoString "$infoString\nAgent  : $agentVersion"
	set infoString "$infoString\nCPU    : $tgtsvr_cpu"
	set infoString "$infoString\nBSP    : $tgtsvr_bsp"
	set infoString "$infoString\nMemory : [format "%#x" $memSize]"
	set infoString "$infoString\nLink   : $link"
	set infoString "$infoString\nUser   : $tgtsvr_user"
	set infoString "$infoString\nStart  : $start"
	set infoString "$infoString\nLast   : $access"

	set toolListString "Attached Tools :\n"
	set toolList [lrange $info 15 end]

    foreach tool $toolList {
	    set toolUser ""
		if {[lindex $tool 4] != ""} {
		    set toolUser "([lindex $tool 4])"
		}
		set toolListString "$toolListString	[lindex $tool 1] $toolUser\n"
    }

	set infoString "$infoString\n$toolListString"

	regsub -all "\n" $infoString "\r\n" infoString1
    set returnVal $infoString1
	
	# wtxToolDetach -all
	wtxToolDetach

	return $returnVal
}


##############################################################################
#
# selectTarget - called as a callback when the target server select dialog
# is being closed.
#
# Closes the target server select dialog and resets the 
# launchDlgGlobals(tool) variable to an empty string for later use.
#
# loads the previously selected target server name from the Windows Registry 
# as part of initialization of Tornado.
#
#
proc selectTarget {} {
	global launchDlgGlobals
	
	set savedTool $launchDlgGlobals(tool)
	set tgt_sel [controlSelectionGet targetServerSelect.targets -string]
	tgtSvrDlgClose

	# Depending on the tool launched, launch it
	if {$savedTool != "" && $tgt_sel != ""} {
		Launch$savedTool $tgt_sel
	}
}


##############################################################################
#
# tgtSvrDlgClose - called as a callback when the target server select dialog
# is being closed.
#
# Closes the target server select dialog and resets the 
# launchDlgGlobals(tool) variable to an empty string for later use.
#
proc tgtSvrDlgClose {} {
	global launchDlgGlobals

	windowClose targetServerSelect
	set launchDlgGlobals(tool) ""
}


##############################################################################
#
# targetServerSelect - shows up a dialog box for the user to select a target
# server against which to run the selected tool.
# is being closed.
#
proc targetServerSelect {args} {
	global launchDlgGlobals
	# Creation of the Configuration panel
	set launchDlgGlobals(tool) [lindex $args 0]

	dialogCreate -name targetServerSelect \
			-title "Launch $launchDlgGlobals(tool)" \
			-init targetServerSelectDlgInit -w 218 -height 207 \
		    -controls { \
		    {label -title "&Targets:" -name lbl1 -x 7 -y 7 -w 32 -h 9} \
			{combo -name targets -callback onTargetSelChange -x 46 -y 7 \
			-w 145 -h 67} \
			{button -callback tgtSvrListRefresh -title "!" \
				-name targetServerSelectDlgInit -x 195 -y 6 -w 16 -h 14} \
			{label -name lbl2 -title "Target &Information" -x 7 -y 25 \
				-w 72 -h 9} \
			{text -fixed -readonly -vscroll -multiline -name tsInfo \
				-x 7 -y 38 -w 204 -h 142} \
			{button  -default -callback selectTarget -title "OK" -name okButt \
				-x 7 -y 186 -w 50 -h 14} \
			{button -callback tgtSvrDlgClose -title "Cancel" -name cancelButt \
				-x 64 -y 186 -w 50 -h 14} \
			{button -helpbutton \
				-title "&Help" -name helpButt -x 161 -y 186 -w 50 -h 14} \
		    }
}


##############################################################################
#
# targetServerSelectDlgInit - callback routine when the target server is
# being initialized on the desktop.
#
# Initializes values in the necessary controls and sets focus to the ok button.
#
proc targetServerSelectDlgInit {} {
	set tgts [tgtServerListGet]
	controlValuesSet targetServerSelect.targets $tgts
	set defTarget [selectedTargetGet]
	if {$defTarget != "" && [lsearch $tgts $defTarget] != -1 } {
		controlSelectionSet targetServerSelect.targets -string $defTarget
	} {
		controlSelectionSet targetServerSelect.targets 0
	}

	controlFocusSet targetServerSelect.okButt
}


##############################################################################
#
# tgtSvrListRefresh - sets the values of the combo box in the target server
# select dialog with the names of the target servers that have been regis-
# tered with the Tornado Registry.
#
proc tgtSvrListRefresh {} {
	controlValuesSet targetServerSelect.targets [tgtServerListGet]
	controlSelectionSet targetServerSelect.targets 0
}


##############################################################################
#
# onTargetSelChange - callback routine for the combo box in the target select
# dialog.  Invoked when the selection changes in that combo box.
#
proc onTargetSelChange {} {

	set eventInfo [eventInfoGet launch]
	if ![string match {*dropdown*} [lindex $eventInfo 0]] {
		set tgt_sel [controlSelectionGet targetServerSelect.targets -string]
		if {$tgt_sel != ""} {
			controlValuesSet targetServerSelect.tsInfo [targetInfoGet $tgt_sel]
		} {
			controlEnable targetServerSelect.okButt 0
		}
	}
}
