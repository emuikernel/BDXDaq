# 01TgtSvrManage.win32.tcl - Target Server Management 
#
# Copyright (C) 1995-99 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01b,09feb99,jak  remove references to symbolic numeric id
# 01a,18jan99,jak created with code from 01TargetServer.win32.tcl and
#										 02Launch.win32.tcl
#
#
# DESCRIPTION
# This script contains procedures that create the target server Manage dialog.
# For proper menu creation, this should be sourced after 01TSConfigure.win32.tcl
#
#*/

##############################################################################
# GLOBAL VARIABLES
#

set tm_current_ts ""

################################################################################
#
#	targetServerManageInit()
#

proc targetServerManageInit {} {
    global tm_current_ts

	# select what's selected in IDE
    set tm_current_ts [selectedTargetGet]

	on_targetManage_refresh
}


################################################################################
#
#   on_tm_targets
#
#
proc on_tm_targets  {args} {
    global tm_current_ts

    set event [controlEventGet targetServerManage.targetManage_targets]
    if {$event == "selchange"} {
		if {[llength $args] !=0} {
			set tm_current_ts [lindex $args 0]
		} else {
			set tm_current_ts \
				[controlSelectionGet targetServerManage.targetManage_targets \
				-string]
		}
		on_targetManage_refresh
	}

}


################################################################################
#
#   on_targetManage_apply
#
#
proc on_targetManage_apply {args} {
	global tm_current_ts

    # get the target server selected
    set ts [controlSelectionGet targetServerManage.targetManage_targets -string]
    set index [controlSelectionGet targetServerManage.targetManage_targets]

    if {$ts == ""} {
		return
	}

	set tm_selectedAction \
	    [controlSelectionGet targetServerManage.targetManage_combo_action \
				-string ]

	if {$tm_selectedAction == "" } {
	    messageBox "No action selected to apply."
		return
	}

	catch {wtxToolAttach $ts -tgtManage} errString
	set args [controlEventGet targetServerManage.targetManage_targets]
	set errMsg "unknown action" 
	set bWtxFail 1

	switch $tm_selectedAction {

	    Reserve 	{

			if {![catch {wtxTsLock} errMsg]} {
				on_targetManage_refresh
				set bWtxFail 0
			}
	    }

	    Unreserve 	{ 

			if {![catch {wtxTsUnlock} errMsg]} {
				on_targetManage_refresh
				set bWtxFail 0
			}
	    }

	    Unregister	{

			if {![catch {wtxUnregister $ts} errMsg]} {
				set tm_current_ts ""
				on_targetManage_refresh
				tgtServerListUpdate
				set bWtxFail 0
			}
    
	    }

	    Kill	{

			set reply [messageBox -yesno \
						"Do you really want to kill the target server $ts?"]
			if {$reply == "yes"} {
		    	if {![catch {wtxTsKill "destroy"} errMsg]} {
		    		# refresh list, then remove tgt from list 
					# if it hasn't unregistered yet
					set dead_ts [controlSelectionGet \
						targetServerManage.targetManage_targets -string]
					set tm_current_ts ""
					on_targetManage_refresh
		    		set old_list \
					[controlValuesGet targetServerManage.targetManage_targets]
					set index [lsearch $old_list $dead_ts]
					if {$index != -1} {
		    			set new_list [lreplace $old_list $index $index]
		    			controlValuesSet \
							targetServerManage.targetManage_targets $new_list
					}
					tgtServerListUpdate
					set bWtxFail 0
				}
	        } else {
				set bWtxFail 0
			}
	    }

	    Reboot	{

			set reply [messageBox -yesno \
				"Do you really want to reboot the target connected to $ts?"]

			if {$reply == "yes"} {
		    	if {![catch {wtxTargetReset} errMsg]} {
					set bWtxFail 0
				}
	        } else {
				set bWtxFail 0
			}
	    }

	} ;# close switch

	if {$bWtxFail} { ;# unknown commands or failures
		messageBox "Action \"$tm_selectedAction\" failed!"
		puts "Target Manage error: $errMsg" 
	}

	catch {wtxToolDetach $ts} errString
}

################################################################################
#
#    on_targetManage_close
#

proc on_targetManage_close {args} {
	global tm_current_ts

	set tm_current_ts ""
    windowClose targetServerManage
}



################################################################################
#
#    on_targetManage_refresh
#
# args - name of tgtsvr to be selected
#
proc on_targetManage_refresh {args} {
    global tm_current_ts

	set tgtsvrList {}
    if {[catch {tgtServerListGet} tgtsvrList]} {
        messageBox \
            "Warning: unable to retrieve Target Server list - $tgtsvrList"
		set tgtsvrList {}
	}

    if {[llength $tgtsvrList] == 0} { ;# no targets, nothing to be done
		controlValuesSet targetServerManage.targetManage_targets {}
		controlValuesSet targetServerManage.targetManage_tsInfo ""
    	controlValuesSet targetServerManage.targetManage_combo_action {}
		controlEnable targetServerManage.targetManage_combo_action 0
		set tm_current_ts ""
		return
    }

	set tgtInfo ""
	set actionList {} 
	set index -1

	# determine which tgtsvr is to be selected:
	if {[llength $args] > 0} {
		set toSel [lindex $args 0]
	    set index [lsearch $tgtsvrList $toSel]
	} else {
		if {$tm_current_ts != ""} {
			# select current tgtsvr if present
	    	set index [lsearch $tgtsvrList $tm_current_ts]
	    	if {$index != -1} {
				# check on sanity of current tgtsvr
				# checkTarget will put up a message box if not responding
				if {[checkTarget $tm_current_ts] != 0} { 
					set tgtInfo "Unable to retrieve target information"
					set actionList {Unregister}
				} else { 
					set tgtInfo [targetInfoGet $tm_current_ts]
    				set actionList {Reserve Unreserve Unregister Kill Reboot}
				}
	    	} else { ;# current tgtsvr wasn't on new list
				messageBox \
					"Warning: \"$tm_current_ts\" is no longer registered." 
			}
		}
	} 

	# insert the tgtsvrs and actions into the GUI combos
	controlValuesSet targetServerManage.targetManage_targets $tgtsvrList
    controlValuesSet targetServerManage.targetManage_combo_action $actionList

	# select a tgtsvr if required (as determined above),
	# then get the info and display it
	if {($index >= 0) && ($index < [llength $tgtsvrList]) } {
		controlSelectionSet targetServerManage.targetManage_targets \
					-noevent $index 
		set tm_current_ts [controlSelectionGet \
							targetServerManage.targetManage_targets -string]
	    controlValuesSet targetServerManage.targetManage_tsInfo $tgtInfo 
		controlEnable targetServerManage.targetManage_combo_action 1
	} else { ;# select none
		set tm_current_ts "" 
		controlValuesSet targetServerManage.targetManage_tsInfo ""
		controlEnable targetServerManage.targetManage_combo_action 0
	}

}

##############################################################################
#
# targetServerManageDialogCreate
#
proc targetServerManageDialogCreate { } {

	dialogCreate -name targetServerManage \
				-title "Manage Target Servers" \
		  		-w 218 -height 235 -init targetServerManageInit \
			-controls {	\
			{label -title "&Targets:" -name lbl1 -x 9 -y 9 -w 32 -h 9}  \
			{combo -name targetManage_targets -callback on_tm_targets \
			  -x 40 -y 7 -w 148 -h 67}  \
			{label -name lbl2 -title "Target &Information" -x 9 -y 25 \
			-w 72 -h 9}  \
			{text -fixed -readonly -vscroll -multiline \
			  -name targetManage_tsInfo -x 9 -y 38 -w 200 -h 150}  \
			{label -title "Select &Action:" -name lbl1 -x 30 -y 197 -w 49 -h 9} \
			{combo -name targetManage_combo_action \
			  -x 80 -y 195 -w 100 -h 67} \
			{button -default -callback on_targetManage_apply \
			-title "Apply" -name applyButton \
			  -x 7 -y 215 -w 50 -h 14} \
			{button -callback on_targetManage_close -title "Close" \
				-name tm_closeButton	\
			  -x 64 -y 215 -w 50 -h 14} \
			{button -helpbutton \
				-title "&Help" -name tm_helpButtom -x 160 -y 215 \
				-w 50 -h 14} \
			{button -callback on_targetManage_refresh -title "!" \
				-name tm_bangRefresh -x 193 -y 6 -w 16 -h 14} \
		  }
}

menuItemInsert -bypath -statusmessage "Manage target servers" \
    	-after -callback targetServerManageDialogCreate \
    	{&Tools "&Target Server" &Configure...} {&Manage...}
