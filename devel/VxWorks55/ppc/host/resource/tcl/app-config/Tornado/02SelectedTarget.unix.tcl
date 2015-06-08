###########################################################################
# 01SelectedTarget.unix.tcl

# Modification history:
# 01d,02mar99,ren  Adding unload method
# 01c,02nov98,ren  Added "Target Toolbar" menu item
# 01b,30oct98,ren  Now, when targetSet is called, the box is also updated.

source [wtxPath host resource tcl app-config Tornado]Download.tcl
namespace eval Target {
    set ALIVE 1
    set DEAD 0

    proc isAlive {target} {
	variable DEAD
	variable ALIVE

	if {$target == ""} {
	    return $DEAD
	}

	beginWaitCursor

	set target_status $ALIVE
	if {[catch {wtxToolAttach $target TargetService} attachRetVal]} {

	    # Target Server not responding !!!

	    if { [regexp {SVR_IS_DEAD} $attachRetVal] } {
		# target server is dead and unregistered. No connection can be done.

		messageBox -ok "Target server $target is dead and Unregistered."
		set target_status $DEAD
	    } {
		# target server is too busy too answer

		messageBox -ok "Target Server $target is not responding but is still alive.\
			    It may simply be too busy to answer."
		set target_status $ALIVE
	    }
        } {
	    wtxToolDetach
	    set target_status $ALIVE
	}

	endWaitCursor

	return $target_status
    }
 
    proc instancesGet {} {
	set targetNameList {}
	set targetInfoList {}

	beginWaitCursor

	if {[catch {wtxInfoQ .* tgtsvr} targetInfo]} {

	    if [string match {*REGISTRY_NAME_NOT_FOUND*} $targetInfo] {
		set targetInfo {}
	    } else {
		endWaitCursor
		error $targetInfo
	    }
	}
	
	# extract target names from list of target information
	foreach target $targetInfo {
	    lappend targetNameList [lindex $target 0]
	}

	endWaitCursor

	return $targetNameList
    }

}

namespace eval SelectedTarget {
    set m_target ""

    proc targetGet {} {
        variable m_target
	return $m_target
    }

    proc targetSet {target} {
	variable m_target
	set m_target $target
	::TargetSelectionWindow::TargetCombo::targetListUpdate
    }

	proc unload {cpu filepaths} {
		::onUnloadFiles $filepaths $cpu
	}

    proc download {cpu filepaths {simStartImage ""}} {
		set status "ok"
		if {$simStartImage != ""} {
			set status [simulatorStart $simStartImage]
		}
		if {[string match "ok" $status]} {
			if {[lindex $filepaths 0] != ""} {
	    		::onDownloadFiles $filepaths $cpu
			}
		}
    }

	# True if simulator is up and its target server is current 
	proc isSimulatorActive {} {
		set retval 0
		return $retval
	}

	proc simulatorKill {} {
		set retval "ok"
		return $retval
	}

	proc isSimulatorTSActive {} {
		set retval 0
		return $retval
	}

	# Get the last launched, or failing that, default, path for the simulator
	proc defaultSimPathGet {hostType} {
		set path ""
		return $path
	}

	# (Re)start the simulator with the specified image
	proc simulatorStart {imagePath} {
		set retval ok
		return $retval
	}
}

namespace eval TargetSelectionWindow {
    set m_name TargetSelectionWindow
    set m_title {Target Selection}
    set m_width 200
    set m_height 50
    set DEAD ::Target::DEAD
    set ALIVE ::Target::ALIVE

    namespace eval TargetCombo {
	set m_self {}
	set m_name TargetCombo
	set m_tooltip {Target Selection}
	set m_xpos 10
	set m_ypos 0
	set m_width 100
	set m_height 80
	variable m_ignoreEvent 0

	set DEAD ::Target::DEAD
	set ALIVE ::Target::ALIVE

	proc onSelectionChange {} {
	    variable m_self
	    variable DEAD
	    variable ALIVE
	    variable m_ignoreEvent

	    # first find out the type of event that occured
	    set eventInfo [controlEventGet $m_self]

	    if {$m_ignoreEvent} {
		set m_ignoreEvent 0 
		return
	    }

	    # if it is dropdown, then update the target list
	    if [string match {*dropdown*} $eventInfo] {
		if {[catch {targetListUpdate} retValue]} {
		    set m_ignoreEvent 1

		    switch -glob "$retValue" {
			*API_REGISTRY_UNREACHABLE* {
			    global env
			    set message "ERROR unable to reach Tornado registry: \n \
WIND_REGISTRY = $env(WIND_REGISTRY)"
			}
			default { 
			    set message "Tornado Registry error reported:\n $retValue"
			}
		    }
		    messageBox $message
		}
		return
	    }

	    if [string match {*selchange*} $eventInfo] {
		set target [::SelectedTarget::targetGet]
		set selectedTarget [selectionGet]
		if { [string compare $target $selectedTarget] } {
		    # different target
		    ::SelectedTarget::targetSet $selectedTarget
		}
		return
	    }
	}
	
	proc selectionGet {} {
	    variable m_self

	    return [controlSelectionGet $m_self -string]
	}

	proc selectionSet {target} {
	    variable m_self

	    controlSelectionSet $m_self -string $target
	}

	proc targetListUpdate {} {
	    variable m_self

	    beginWaitCursor

	    if [catch {::Target::instancesGet} retValue] {
		endWaitCursor
		controlValuesSet $m_self {}
		error $retValue
	    }

	    controlValuesSet $m_self $retValue
	    set target [::SelectedTarget::targetGet]

	    if [llength $target] {
		    selectionSet $target
	    }

	    endWaitCursor
	}
	
    }

    proc showAsToolbar {} {
	variable m_name
	variable m_title
	variable m_width
	variable m_height

	toolbarCreate $MainWindow::m_name -name $m_name \
	    -title $m_title -top

	set TargetCombo::m_self $m_name.$TargetCombo::m_name

	controlCreate $m_name \
	    [list label -name TargetLabel \
		 -title "Target" \
		 -x 10 -y 5 \
		 -w 35 -h 10 ]

	controlCreate $m_name \
	    [list combo -name $TargetCombo::m_name \
		 -sort -tooltip $TargetCombo::m_tooltip \
		 -callback {::TargetSelectionWindow::TargetCombo::onSelectionChange} \
		 -x 40 \
		 -w $TargetCombo::m_width -h $TargetCombo::m_height]


	# AAM - kludge, the combo list will not show up unless there
	# is also a toolbar button control too.
	#
	controlCreate $m_name \
	    {toolbarbutton -name TargetButtons }

	menuItemAppend \
	    -callback ::TargetSelectionWindow::onToggleTargetTB \
	    -cmduicallback ::TargetSelectionWindow::onCmdUITargetTB \
	    -bypath "&View" \
	    "&Target Toolbar"

	return
    }

    proc onToggleTargetTB { args } {
	variable m_name
	windowShow $m_name [expr { ![windowVisible $m_name] } ]	
    }

    proc onCmdUITargetTB { args } {
	variable m_name
	global checkflag
	set checkflag [windowVisible $m_name]
    }

    proc showAsDialog {} {
	variable m_name
	variable m_title
	variable m_width
	variable m_height
	
	set label_ypos 20
	set label_xpos 4
	set TargetCombo::m_ypos 20
	set TargetCombo::m_xpos 35
	set TargetCombo::m_self $m_name.$TargetCombo::m_name
	dialogCreate -name $m_name  \
	    -title $m_title \
	    -width $m_width -height $m_height \
	    -init ::TargetSelectionWindow::onInit \
	    -controls [list \
			  [list label -name lbActiveTarget \
			       -title "Active\nTarget" \
			       -xpos $label_xpos -ypos $label_ypos \
			       -width 30 -height 30] \
			  [list combo -name $TargetCombo::m_name \
			       -sort -tooltip $TargetCombo::m_tooltip \
			       -callback \
			       {::TargetSelectionWindow::TargetCombo::onSelectionChange} \
			       -x $TargetCombo::m_xpos -y $TargetCombo::m_ypos \
			       -w $TargetCombo::m_width -h $TargetCombo::m_height] \
			  [list button -name cbClose \
			       -title "Close" \
			       -callback {::TargetSelectionWindow::onClose} \
			       -x 150 -y 20 \
			       -w 45 -h 14 ]
		      ]

    }

    proc onClose {} {
	variable m_name

	windowClose $m_name
    }

    proc menusCreate {} {
	menuItemInsert -bypath -before -callback \
	    {::TargetSelectionWindow::showAsDialog} \
	    {&Tools Preferences...} {Select Target...}
    }

    proc onInit {} {
	TargetCombo::targetListUpdate
    }

}

# XXX
# This prevents us from having to define a host type test later
namespace eval :: {
	proc isDebuggerActive {} {
		return 0
	}
}

TargetSelectionWindow::showAsToolbar
# TargetSelectionWindow::menusCreate
