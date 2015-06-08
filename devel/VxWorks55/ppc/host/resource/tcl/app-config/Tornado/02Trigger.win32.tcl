# Copyright 1998-2002 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 04t,24jun02,cpd  Fix sourcing of shelcore and wtxcore files
# 04s,28jan02,ajd  Changes for Thumb mode
# 04r,07jan02,tcr  Change Copyright date
# 04q,18sep01,cpd  Initial Veloce mods
#                  Fix SPR#21699: Implement cancel function
#                  Part Fix SPR#27093 #26082 #21328 #27147: WindView buttons
#		   and viewgraph are updated on Start & Stop WindView trigger
#                  actions (only when look or timer is clicked) and trigger
#                  start/stop buttons.
# 04p,08oct01,fle  unified Tornado version String
# 04o,20dec99,jgn  fix for thumb mode (with help from Paola)
# 04n,15dec99,jgn  update win32 registry key info for 2.0.1
# 04m,28apr99,tcr  improve error trapping, fix error in wtxHandle name pattern
# 04l,23apr99,tcr  maintain single wtxToolAttach connection.
# 04k,16apr99,nps  changed trigger maintenance dialog to label 'chaining'
#                  section.
# 04j,14apr99,nps  disable 'Toolbar->Triggering Toolbar' when not needed.
# 04i,17mar99,nps  corrected minor but annoying dialog control alignment.
# 04h,02mar99,ren  Changing &Triggering to T&riggering to avoid menu short cut
#                  conflict.
# 04g,24feb99,tcr  Changes to Trigger Save Confirmation for SPR 25222 use 
#                  windowQueryClose callback
# 04f,18feb99,tcr  Change Trigger Save Confirmation dialog for SPR 25175
# 04e,08dec98,nps  allow alternate tgtsvr selection between invokations.
# 04d,02dec98,nps  fixed SPR 23588 (check if triggers need saving).
#                  fixed SPR 23589 (trigger does not append .trg when saving
#                  files.
# 04c,01dec98,nps  ensure wvGlobal(targetName) is declared before used.
# 04b,30nov98,nps  use target server selected from launcher if supplied.
# 04a,13nov98,nps  add menu item to toggle triggering toolbar.
# 03z,09nov98,nps  further WV2.0 -> T2 merge.
# 03y,03nov98,ren  Fixed problem with combo box focus
# 03x,22oct98,jp   User the Tornado 2.0 entry in Windows Registry
# 02w,20oct98,ren  fixed problem where trigger would not be deleted
#                  from list box if couldn't connect to target
#                  also fixed: trigger dialog box would not close when
#                  you click ok if there is already a copy of windview
#                  control window
# 03v,09oct98,ren  tor2_0_x --> tor2_0_0 merge
# 02u,09oct98,ren  fixed my screw up on PC side when I made change 02t
# 02t,09oct98,ren  modified to prevent target select dialog from demanding focus
# 02s,02oct98,nps  remove <ctrl>-Ms
# 02r,30sep98,nps  Toggle trigger toolbar button as necessary.
# 02q,28aug98,nps  UITcl II update.
# 02p,27aug98,nps  corrected representation of trigger chains in overview.
# 02o,27aug98,nps  allow a chained trigger to be initially enabled.
# 02n,19aug98,cjtc event buffer full handled more gracefully (SPR 22133)
# 02m,07aug98,nps  add offset to user event IDs (SPR 22022)
# 02l,06aug98,nps  temporarily remove conditional 'lib' option.
# 02k,24jul98,nps  fixed invokation from Launch menu (SPR 21455) by
#                  calling approriate callback.
# 02j,21jul98,cjtc in trigRefreshView when periodically activated, prevented
#                  creation of multiple "select target" windows when trigger
#		   action is Start WV (SPR 21373)
# 02i,18jun98,nps  fixed SPR 21467 & 21753.
# 02i,18jun98,nps  fixed SPR 21467 & 21753.
# 02h,09jun98,nps  fixed SPR 21376 & 21443.
# 02g,15may98,nps  fixed online help support for trigger dialog
# 02f,06may98,nps  made overview window resizable..
# 02e,23apr98,sks  fix the displaying of the statusbar to actually show the
#                  progress meter.
# 02d,21apr98,nps  intEnt takes account of MIN_INT_ID offset.
# 02c,15apr98,c_s  mainwin fixups
# 02b,13apr98,nps  increase frequency of look on timer.
#                  detect if condition argument is a float.
# 02a,02apr98,nps  don't use 'magic number' to collect hit count.
# 029,27mar98,nps  tidy up trigger event list.
#                  fixed trigger selection via yellow maintenance button.
# 028,06mar98,nps  reset wvEventSearchList each time we open triggering.
# 027,03mar98,nps  added periodic look button.
# 026,02mar98,nps  incorporate various fixes added in Alameda.
#                  trigger overview now uses table control.
#                  event list used reverted to search list.
#                  don't allow multiple windows to be created.
#                  don't involve WV when stopping triggering if not relevant.
#                  action deferred execution is now labeled 'Safe'.
# 025,16feb98,nps  Lose 'ui loaded' message.
# 024,13feb98,nps  Automagically open WV window if triggering refers to WV.
# 023,13feb98,nps  Fixed header typo.
# 022,13feb97,nps  Added facility to 'Look' at trigger status.
#                  Don't use new interpreter for overview window.
#                  Fixed bug which caused target-side corruption.
# 021,02feb97,nps  ongoing integration.
# 020,20jan97,nps  wvEventList now called wvAllEventsList
# 01f,19jan97,nps  launchTrigger should be LaunchTriggering, so much for last
#                  moment changes!
# 01e,19jan97,nps  Now integrated with WV2.0 configuration.
# 01d,12jan97,nps  further target integration.
# 01c,08jan97,nps  fixed 8 trigger overview problem
#                  trigger start now has progress meter
# 01b,23dec97,nps  improved gui
# 01a,23oct97,nps  written.
#*/

# globals

set basePath ""

if [catch {set basePath [wtxPath]}] {
        set basePath "$env(WIND_BASE)"
        regsub -all {\\} $basePath / basePath
}

set trigGlobal(bitmaps)		[format "%s%s" $basePath host/resource/bitmaps/Trigger]
set trigGlobal(events)		[format "%s%s" $basePath host/resource/tcl/app-config/WindView]
set trigGlobal(helpFilePath)	[format "%s%s" $basePath host/resource/help/wv4win.hlp]

lappend launcherGlobals(targetChangeNotifiers) triggerButtonUpdate

set trigGlobal(notSaved)        0

global trigID {}

global trigList
global trigTempList

global trigCxtList
set trigCxtList        {Any "Any Task" "Specific Task" "Any ISR" "System"}

global trigObjList
set trigObjList        {Any "Specific Object"}

global trigCondList
set trigCondList       {"(unconditionally)" "conditional upon"}

global trigFnVarList
set trigFnVarList      {"Fn" "Var"}

# Temporarily remove Cond Lib support until it can be supported more
# elegantly

#set trigFnVarList      {"Fn" "Var" "Lib"}

global trigRelOpList
set trigRelOpList      {== != < <= > >= | &}

global trigActionList
set trigActionList    {"(no action)" "Start WV" "Stop WV" "Call function"}

global trigChainList

global trigSelectedEna
global trigSelectedEvt
global trigSelectedEvtSpc
global trigSelectedCxt
global trigSelectedCxtSpc
global trigSelectedObj
global trigSelectedObjSpc
global trigSelectedCond
global trigSelectedFnVar
global trigSelectedFnVarSym
global trigSelectedRelOp
global trigSelectedArg
global trigSelectedAction
global trigSelectedActFn
global trigSelectedActFnArg
global trigSelectedActDefer
global trigSelectedDis
global trigSelectedChain

global trigTempSaveEna 
global trigTempSaveEvt 
global trigTempSaveEvtSpc 
global trigTempSaveCxt 
global trigTempSaveCxtSpc 
global trigTempSaveObj 
global trigTempSaveObjSpc 
global trigTempSaveCond 
global trigTempSaveFnVar 
global trigTempSaveFnVarSym 
global trigTempSaveRelOp 
global trigTempSaveAction 
global trigTempSaveActDefer 
global trigTempSaveDis 
global trigTempSaveChain 

global trigTempSaveActFnArg
global trigTempSaveActFn
global trigTempSaveArg

#timr
global trigOkToDiscard

global trigCancelAll

proc triggerButtonUpdate {} {
    global wvGlobal
    set tgtsvrSel [selectedTargetGet]
    if {$tgtsvrSel == ""} {
    	controlEnable $wvGlobal(launchTbar).trigger 0
    } {
    	controlEnable $wvGlobal(launchTbar).trigger 1
    }
}

proc onCmdUITrigger {} {
    global enableflag
	global wvGlobal

	if { $wvGlobal(standalone) } {
		set target [selectedTargetValue]
	} else {
		set target [selectedTargetGet]
	}
    if {$target == ""} {
        set enableflag $wvGlobal(standalone)
    } {
        set enableflag 1
    }
}


proc trigInitUI {} {
    global trigGlobal
    global wvGlobal

    controlCreate $wvGlobal(launchTbar) [list toolbarbutton \
		-cmduicallback onCmdUITrigger \
		-callback "toolSelectCallback LaunchTriggering" \
		-name trigger \
		-tooltip "Launch Triggering" \
		-bitmap "$trigGlobal(bitmaps)/flashYellow.bmp"]

    if $wvGlobal(standalone) {
	menuItemAppend &Tools T&riggering... \
	    {toolSelectCallback LaunchTriggering}
    } else {
	menuItemInsert -name trgMenu  \
	    -callback {toolSelectCallback LaunchTriggering} \
            -bypath -after {"&Tools" &WindView} "T&riggering..."
    }

    menuItemAppend -bypath \
            -callback onViewTriggeringTB \
            -cmduicallback onCmdUITrgTB \
            {"&View" "&Toolbar"} \
            "&Triggering Toolbar"
}

proc LaunchTriggering {args} {
    global wvGlobal

    if { !$wvGlobal(standalone) } {
        set wvGlobal(targetName) ""
    }

    wrapper_triggerLaunch $args
}

proc triggerLaunch {} {
    set tgtsvr_selected [selectedTargetGet]
    
    if {$tgtsvr_selected != "" &&
    	[trigCheckTarget $tgtsvr_selected] == "0"} {
    	wrapper_triggerLaunch $tgtsvr_selected
    }
}

proc trigCheckTarget {tgtsvr} {
    return 1
}

proc trigInit {target} {
    global trigList
    global trigChainList
    global trigGlobal
    global wvEventSearchList
#   global wvAllEventsList
    global wvGlobal

    set trigID {}
    set trigList {}
    set trigTempList {}
#   set trigList {New}
    set trigChainList {(none)}

    set trigGlobal(targetName) $target

    trigToolbarCreate
    trigInfoCreate

    uplevel #0 source [wtxPath host resource tcl app-config Tornado]01WvConfig.win32.tcl

    uplevel #0 source [wtxPath host resource tcl app-config WindView]database.tcl

    set wvEventSearchList {}

    trigEventFileRead vxworks

    # Perform some 'tidying' of this list

    # Remove 'value'
    set delIndex [lsearch -exact $wvEventSearchList "value"]
    set wvEventSearchList [lreplace $wvEventSearchList $delIndex $delIndex]

    set numEvents [llength $wvEventSearchList]

    # Add in Any and intEnt
    incr numEvents
    incr numEvents

    set trigGlobal(eventList) [lsort [linsert $wvEventSearchList 0 Any intEnt]]
    set trigGlobal(eventList) [linsert $trigGlobal(eventList) $numEvents "User Event"]

    set trigGlobal(allowMaint)         0
    set wvGlobal(uplinkActive)         0
}

proc trigInfoCreate {} {

    controlCreate trigOverview "table -name trigInfo \
                  -columns 10 -columnheader \
                  -callback trigInfoEvent \
                  -x 7 -y 7 -w 590 -h 100"

    controlPropertySet trigOverview.trigInfo -columnheaders {Trg "Trigger ID" Status Event Context Object Cond? Action Disable? Chain}
    controlPropertySet trigOverview.trigInfo -columnwidths {30 40 40 80 80 80 40 40 40 40}
}

proc trigRefreshView {} {

    global trigID
    global trigList
    global trigSelectedEna
    global trigSelectedEvt
    global trigSelectedCxt
    global trigSelectedObj
    global trigSelectedCond
    global trigSelectedAction
    global trigSelectedDis
    global trigSelectedChain
    global trigGlobal
    global offset

    global trigTimerButtonActive

    set handleList [wtxHandle]

    if { [llength $handleList] > 0 } {
        set currentHandle [lindex $handleList 0]
    }

    set trigInfoList {}

    if {[llength $trigList] == 0} { # This becomes 1 if restoring 'New'
        controlValuesSet trigOverview.trigInfo {{None " " " " " " " " " " " " " " " " " "}}
    } else {
        foreach trg $trigList {
            set newInfoLine [list ]

            if {$trg == "New"} {
                continue
            }
            
            if {$trigSelectedEna($trg) == 1} {
                set ena "Enabled"
            } else {
                set ena "Disabled"
            }

            if {$trigSelectedCond($trg) == 1} {
                set cond "Yes"
            } else {
                set cond "No"
            }

            if {$trigSelectedDis($trg) == 1} {
                set dis "Yes"
            } else {
                set dis "No"
            }

            if {$trigID($trg) != 0} {
                set id $trigID($trg)

                if { [trigBindToTarget $trigGlobal(targetName)] == 0 } {
		    set trigTimerButtonActive 0
                    return
		}

                set hitCnt [wtxGopherEval "$id <+$offset(TRIGGER,hitCnt) @>"]

                if {$trigSelectedDis($trg) == 1} {
                     if {$hitCnt == 0} {
                         set hitCount "Armed"
                     } else {
                         set hitCount "Fired"
                     }
                } else {
                     set hitCount [format "%-8d" $hitCnt]
                }
            } else {
                set id "(not set)"

                set hitCount "------"
            }

            switch $trigSelectedAction($trg) {
            0 {set action "(none)"}
            1 {
               set action "WV Start"

               if {![windowExists trigConfig]} {

                   if {![windowExists eventLogging] && ![windowExists selTarget]} {						
                       toolSelectCallback LaunchWindView
                   }
					
                   #
                   # Windview has been launched. So long as the event logging
                   # window is open, keep looking to see if the buffer is full by
                   # calling the windview function loggingStoppedCheck
                   #

                   if { [windowExists eventLogging] } {
    
                   # this operation could fail the windview
                   # control window was already open
    
                   catch { loggingStoppedCheck }
                   }
               }
            }

            2 {set action "WV Stop"}
            default -
            3 {set action "Call Fn"}
            }

            if {$trigSelectedChain($trg) == "(none)"} {
                set chain "--"
            } else {
                set chain $trigSelectedChain($trg)
            }
			
            lappend newInfoLine [format "#%2d" $trg]
            lappend newInfoLine [format "%-10.10s" $id]
            lappend newInfoLine [format "%-9.9s" $hitCount]
#           lappend newInfoLine [format "%-9.9s" $ena]
            lappend newInfoLine [format "%-16.15s" $trigSelectedEvt($trg)]
            lappend newInfoLine [format "%-20.20s" $trigSelectedCxt($trg)]
            lappend newInfoLine [format "%-20.20s" $trigSelectedObj($trg)]
            lappend newInfoLine [format "%-6.6s" $cond]
            lappend newInfoLine [format "%-8.8s" $action]
            lappend newInfoLine [format "%-8.8s" $dis]
            lappend newInfoLine [format "%-6.6s" $chain]
			
	    if { $action == "WV Stop" && $hitCount == "Fired" } {
		controlEnable wvTB.wvStopButt   0
		controlEnable wvTB.wvStartButt  1
    		onWvStopButt
	    }		
						
            lappend trigInfoList $newInfoLine
        }
		
        controlValuesSet trigOverview.trigInfo $trigInfoList
    }

    if { [llength $handleList] > 0 } {
        catch {wtxHandle $currentHandle}
    }

}

proc onTrigLookTimer {} {
    global trigTimerButtonActive

    set trigTimerButtonActive [controlChecked trigTB.trigLookTimerButt]
    if {$trigTimerButtonActive == 1} {
        autoTimerCallbackSet trigOverview 5 trigRefreshView
        trigRefreshView

    } else {
        autoTimerCallbackSet trigOverview 0
    }
}

proc trigInfoEvent {} {

    global trigGlobal

    if {$trigGlobal(allowMaint) == 0} {
        return
    }

    set trigInfoList [controlValuesGet trigOverview.trigInfo]
    set eventInfo    [controlEventGet trigOverview.trigInfo]

    set trigGlobal(trigSel) [controlSelectionGet trigOverview.trigInfo]

    if {[string match {*dblclk*} [lindex $eventInfo 0]]} {
        if [catch {set selectedLine [lindex $trigInfoList [controlSelectionGet trigOverview.trigInfo]]}] {
	return
	}

        set selectedLine [string trim [lindex $selectedLine 0] \{\}]

        if {[string match *\#* $selectedLine]} {
            scan $selectedLine "#%2d" selectedTrg

            trigMaintLaunch [format "%2d" $selectedTrg]
        }
    }
}

proc wrapper_triggerLaunch {args} {
    global trigGlobal
    global trigID
    global trigList
    global trigChainList
    global wvGlobal

    if [windowExists trigOverview] {
        return
    }

    # wvGlobal(targetName) carries selected target (if supplied)

    # first check that it exists!

    if [catch { set test $wvGlobal(targetName)} ] {
         set wvGlobal(targetName) ""
    }

    if {$wvGlobal(targetName) == ""} {
        set wvGlobal(targetName) [selectedTargetGet]
    }

    if {$wvGlobal(targetName) == ""} {
        messageBox "No target selected."
        return
    }

    windowCreate -name trigOverview -width 530 -height 180  \
                 -nonewinterpreter \
                 -init "trigInit $wvGlobal(targetName)" \
                 -title "Triggering $wvGlobal(targetName)" \
                 -icon $trigGlobal(bitmaps)/trigger.ico \
                 -helpfile $trigGlobal(helpFilePath)

    # timr
    windowQueryCloseCallbackSet trigOverview trigSaveCheck

    adjustTrgOverview

    windowSizeCallbackSet trigOverview "adjustTrgOverview"
    windowExitCallbackSet trigOverview triggerExit

}


proc triggerExit {} { 
    global wvGlobal
    trigUnbindFromTarget $wvGlobal(targetName)    
}

proc trigCancelCheck {} {
    global trigCancelAll

    if [windowExists cancelCheck] {
        return
    }

    # this dialog now has 3 buttons
    dialogCreate -name cancelCheck \
        -title "Trigger Cancel Confirmation" \
        -w 165 -h 60 -nocontexthelp \
        -controls {
            { label -title "Latest changes on all triggers will be lost!" \
              -x 20 -y 07 -w 155 -h 25}
            { label -title "Are you sure?" \
              -x 60 -y 17 -w 125 -h 25}
            { button -default -name okButt
              -title "Yes" -callback {set trigCancelAll 1; windowClose cancelCheck}
              -x 15 -y 32 -w 30 -h 14}
            { button -name cancelButt -title "No"
              -callback {windowClose cancelCheck; set trigCancelAll 0}
              -x 115 -y 32 -w 30 -h 14}
        }

    return $trigCancelAll
}


# timr
# trigSaveCheck must return <> 0 if it's ok to close the window
#                              0 if not
#
# function is now called as windowQueryCloseCallback

proc trigSaveCheck {} {
    global trigGlobal
    global trigOkToDiscard

    if [windowExists saveCheck] {
        return
    }

    # timr
    # if already saved, don't ask the question
    if { $trigGlobal(notSaved) == 0 } {
        set trigOkToDiscard 1
        return $trigOkToDiscard
    }
    # timr
    # this dialog now has 3 buttons
    dialogCreate -name saveCheck \
        -title "Trigger Save Confirmation" \
        -w 165 -h 60 \
        -controls {
            { label -title "Do you want to save changes?" \
              -x 20 -y 07 -w 165 -h 25}
            { button -default -name yesButt
              -title "Yes" -callback {set trigOkToDiscard 0; trigSaveFile; windowClose saveCheck}
              -x 15 -y 32 -w 30 -h 14}
            { button -name noButt
              -title "No" -callback {windowClose saveCheck; set trigOkToDiscard 1 }
              -x 65 -y 32 -w 30 -h 14}
            { button -name cancelButt -title "Cancel"
              -callback {windowClose saveCheck; set trigOkToDiscard 0}
              -x 115 -y 32 -w 30 -h 14}
        }

    return $trigOkToDiscard
}

proc trigSaveCancel {} {
    windowClose saveCheck
}

proc trigMaintLaunch {args} {
    global trigGlobal

    dialogCreate -name trigConfig \
    		-title "Trigger Maintenance $trigGlobal(targetName)" \
    		-init "triggerDlgInit $args" \
    		-w 290 -h 280 \
                -helpfile $trigGlobal(helpFilePath) \
    		-controls [list \
			[list button -default -name okButt -title OK \
				-callback onTrigOK \
				-x 67 -y 255 -w 50 -h 14] \
\
			[list button -name cancelButt -title Cancel \
				-callback onTrigCancel \
				-x 177 -y 255 -w 50 -h 14] \
\
			[list label -name l1 -title "Trigger #" \
				-x 13 -y 15 -w 30 -h 8] \
			[list combo -name trigNum -callback {onTrigMaint read} \
				-x 50 -y 12 -w 48 -h 80] \
			[list label -name trigLEna -title "Initial trigger state is" \
				-x 17 -y 47 -w 90 -h 8] \
			[list boolean -name trigEna -callback onTrigEnable \
				-auto -title "Enabled" \
                                -x 84 -y 47 -w 42 -h 10] \
\
			[list label -name trigLEvt -title "Event Matches" \
				-x 17 -y 72 -w 48 -h 8] \
			[list combo -name trigEvt -callback onTrigEvt \
				-x 73 -y 69 -w 94 -h 80] \
			[list text -name trigEvtSpc -callback onTrigEvtSpc \
				-x 174 -y 69 -w 94 -h 14] \
\
			[list label -name trigLCxt -title "Context Matches" \
				-x 17 -y 94 -w 54 -h 8] \
			[list combo -name trigCxt -callback onTrigCxt \
				-x 73 -y 91 -w 94 -h 80] \
			[list text -name trigCxtSpc -callback onTrigCxtSpc \
				-x 174 -y 91 -w 94 -h 14] \
\
			[list label -name trigLObj -title "Object Matches" \
				-x 17 -y 116 -w 51 -h 8] \
			[list combo -name trigObj -callback onTrigObj \
				-x 73 -y 113 -w 94 -h 80] \
			[list text -name trigObjSpc -callback onTrigObjSpc \
				-x 174 -y 113 -w 94 -h 14] \
\
			[list combo -name trigCond -callback onTrigCond \
				-x 17 -y 135 -w 70 -h 80] \
\
                        [list combo -name trigFnVar -callback onTrigFnVar \
                                -x 17 -y 157 -w 30 -h 80] \
			[list text -name trigFnVarSym -callback onTrigFnVarSym \
				-x 55 -y 157 -w 70 -h 14] \
			[list combo -name trigRelOp -callback onTrigRelOp \
				-x 133 -y 157 -w 28 -h 80] \
			[list text -name trigArg -callback onTrigArg \
				-x 169 -y 157 -w 70 -h 14] \
                        [list group -title "Specification" \
                                -x 13 -y 30 -w 265 -h 150] \
                        [list group -title "Action / Chaining" \
                                -x 13 -y 185 -w 265 -h 62] \
			[list combo -name trigAction -callback onTrigAction \
				-x 17 -y 202 -w 70 -h 80] \
			[list text -name trigActFn -callback onTrigActFn \
				-x 87 -y 202 -w 70 -h 14] \
			[list text -name trigActFnArg -callback onTrigActFnArg \
				-x 157 -y 202 -w 70 -h 14] \
			[list boolean -name trigDefer -callback onTrigDefer \
				-auto -title "Safe" -x 231 -y 205 -w 42 -h 10] \
\
			[list label -name trigLDis -title "Disable current trigger" \
				-x 17 -y 227 -w 80 -h 8] \
			[list boolean -name trigDisable \
                                -auto -callback onTrigDisable \
				-x 90 -y 227 -w 10 -h 10] \
			[list label -name trigLChain -title "and enable trigger #" \
				-x 102 -y 227 -w 70 -h 8] \
			[list combo -name trigChain -callback onTrigChain \
				-x 172 -y 225 -w 48 -h 80] \
    		]
}

proc onCmdUItrigMaint {} {
    global enableflag
    global trigList
    global trigGlobal

    set enableflag [windowExists trigOverview]

    if { [controlSelectionGet trigOverview.trigInfo] == "" || [llength $trigList] == 0} {
	set enableflag 0
	set trigGlobal(trigSel) ""
    } else {
	set enableflag 1
    }
}

proc onCmdUITrgTB {} {
    global checkflag
    global enableflag

    set enableflag [windowExists trigOverview]

    if [windowExists trigTB] {
        set checkflag [windowVisible trigTB]
    }
}

proc onViewTriggeringTB {} {
    global trigGlobal

    if { ![windowExists trigTB] } {
        return
    }

    set TBState [windowVisible trigTB]

    if {$TBState == "1"} {
        windowShow trigTB 0
    } else {
        windowShow trigTB 1
    }

    set TBState [windowVisible trigTB]
}


set trigTimerButtonActive 0

proc onCmdUITrigTimerButt {} {
    global checkflag
    global trigTimerButtonActive

    set checkflag $trigTimerButtonActive
    if { $trigTimerButtonActive == 0 } {
        autoTimerCallbackSet trigOverview 0
    }
}


proc trigToolbarCreate {} {
    global trigGlobal 
    global trigTimerButtonActive

    toolbarCreate trigOverview -name trigTB -top -helpfile $trigGlobal(helpFilePath)

    controlCreate trigTB "toolbarbutton -name trigCreateButt \
 		-callback {trigMaintLaunch New} \
 		-tooltip \"Create Trigger\" \
 		-bitmap \"$trigGlobal(bitmaps)/trgCreate.bmp\" "

    controlCreate trigTB "toolbarbutton -name trigMaintButt \
                -cmduicallback onCmdUItrigMaint \
		-callback {trigMaintLaunch Look} \
 		-tooltip \"Maintain Triggers\" \
 		-bitmap \"$trigGlobal(bitmaps)/flashYellow.bmp\" "

    controlCreate trigTB "toolbarbutton -name trigDelButt \
                -cmduicallback onCmdUItrigQuickDel \
 		-callback trigQuickDel \
 		-tooltip \"Delete Trigger\" \
 		-bitmap \"$trigGlobal(bitmaps)/ms_del.bmp\" "

    controlCreate trigTB "toolbarbutton -name trigLoadButt \
                -separator \
 		-callback trigLoadFile \
 		-tooltip \"Load Trigger Configuration\" \
 		-bitmap \"$trigGlobal(bitmaps)/ms_open.bmp\" "

    controlCreate trigTB "toolbarbutton -name trigSaveButt \
 		-callback trigSaveFile \
 		-tooltip \"Save Trigger Configuration\" \
 		-bitmap \"$trigGlobal(bitmaps)/ms_save.bmp\" "

    controlCreate trigTB "toolbarbutton -name trigStartButt \
                -separator \
 		-callback trigStart \
 		-tooltip \"Start Triggering\" \
 		-bitmap \"$trigGlobal(bitmaps)/trgStart.bmp\" "

    controlCreate trigTB "toolbarbutton -name trigStopButt \
 		-callback trigStop \
 		-tooltip \"Stop Triggering\" \
 		-bitmap \"$trigGlobal(bitmaps)/trgStop.bmp\" "

    controlCreate trigTB "toolbarbutton -name trigLookButt \
 		-callback trigRefreshView \
 		-tooltip \"Look at Trigger state\" \
 		-bitmap \"$trigGlobal(bitmaps)/look.bmp\" "

    controlCreate trigTB "toolbarbutton -name trigLookTimerButt \
                -cmduicallback onCmdUITrigTimerButt \
                -toggle \
 		-callback onTrigLookTimer \
 		-tooltip \"Look Periodically at Trigger state\" \
 		-bitmap \"$trigGlobal(bitmaps)/lookTimer.bmp\" "

    set trigGlobal(allowMaint)         0

    controlEnable trigTB.trigMaintButt 0
    controlEnable trigTB.trigDelButt   0
    controlEnable trigTB.trigStartButt 0
    controlEnable trigTB.trigStopButt  0
    set trigTimerButtonActive 0
}

proc triggerDlgInit {args} {
    global trigGlobal
    global trigList
    global trigCxtList
    global trigObjList
    global trigCondList
    global trigFnVarList
    global trigRelOpList
    global trigActionList
    global trigChainList

    if {[string match $args Look]} {
	set args " $trigGlobal(trigSel)"
    }

    controlValuesSet trigConfig.trigNum $trigList

    controlCheckSet trigConfig.trigEna -noevent 1 
    controlEnable trigConfig.trigLEna 0 
    controlEnable trigConfig.trigEna 0 

    controlValuesSet trigConfig.trigEvt $trigGlobal(eventList)
    controlSelectionSet trigConfig.trigEvt -noevent 0
    controlEnable trigConfig.trigLEvt 0 
    controlEnable trigConfig.trigEvt 0 
    controlValuesSet trigConfig.trigEvtSpc ""
    controlEnable trigConfig.trigEvtSpc 0 

    controlValuesSet trigConfig.trigCxt $trigCxtList 
    controlSelectionSet trigConfig.trigCxt -noevent 0
    controlEnable trigConfig.trigLCxt 0 
    controlEnable trigConfig.trigCxt 0 
    controlValuesSet trigConfig.trigCxtSpc ""
    controlEnable trigConfig.trigCxtSpc 0 

    controlValuesSet trigConfig.trigObj $trigObjList 
    controlSelectionSet trigConfig.trigObj -noevent 0
    controlEnable trigConfig.trigLObj 0 
    controlEnable trigConfig.trigObj 0 
    controlValuesSet trigConfig.trigObjSpc ""
    controlEnable trigConfig.trigObjSpc 0 

    controlValuesSet trigConfig.trigCond $trigCondList 
    controlSelectionSet trigConfig.trigCond -noevent 0
    controlEnable trigConfig.trigCond 0 

    controlValuesSet trigConfig.trigFnVar $trigFnVarList 
    controlSelectionSet trigConfig.trigFnVar -noevent 0
    controlEnable trigConfig.trigFnVar 0 

    controlValuesSet trigConfig.trigFnVarSym ""
    controlEnable trigConfig.trigFnVarSym 0 

    controlValuesSet trigConfig.trigRelOp $trigRelOpList
    controlEnable trigConfig.trigRelOp 0 

    controlValuesSet trigConfig.trigArg ""
    controlEnable trigConfig.trigArg 0 

    controlValuesSet trigConfig.trigAction $trigActionList
    controlEnable trigConfig.trigAction 0 

    controlValuesSet trigConfig.trigActFn ""
    controlEnable trigConfig.trigActFn 0 

    controlValuesSet trigConfig.trigActFnArg ""
    controlEnable trigConfig.trigActFnArg 0 

    controlCheckSet trigConfig.trigDefer 1 
    controlEnable trigConfig.trigDefer 0 

    controlCheckSet trigConfig.trigDisable 0 
    controlEnable trigConfig.trigLDis 0 
    controlEnable trigConfig.trigDisable 0 
    controlValuesSet trigConfig.trigChain $trigChainList 
    controlEnable trigConfig.trigLChain 0 
    controlEnable trigConfig.trigChain 0 
    controlSelectionSet trigConfig.trigChain -noevent 0

    if {[string compare $args New] == 0} {
        #  puts "Creating new trigger"
	controlEnable trigConfig.trigNum 0
    } else {
        set args [string trim $args "{}"]
        #  puts "Selecting existing trigger #$args"
	controlEnable trigConfig.trigNum 1 
    }

    onTrigMaint $args
}

proc trigStart {} {
    global trigGlobal
    global trigList
    global trigID

    global trigCxtList
    global trigObjList

    global trigSelectedEna
    global trigSelectedEvt
    global trigSelectedEvtSpc
    global trigSelectedCxt
    global trigSelectedCxtSpc
    global trigSelectedObj
    global trigSelectedObjSpc
    global trigSelectedCond
    global trigSelectedFnVar
    global trigSelectedFnVarSym
    global trigSelectedRelOp
    global trigSelectedArgType
    global trigSelectedArg
    global trigSelectedAction
    global trigSelectedActFn
    global trigSelectedActDefer
    global trigSelectedActFnArg
    global trigSelectedDis
    global trigSelectedChain

    global wvEventInfo
    global wvGlobal

    global cpuFamily
    global __wtxCpuType

    beginWaitCursor

    # Connect to target

    if { [trigBindToTarget $trigGlobal(targetName)] == 0 } {
        endWaitCursor
        messageBox "Cannot attach to target server $trigGlobal(targetName)"
	return 
    }
	
    # OK, we attached. Now we need to make sure the kernel supports
    # triggering by checking for the tell tale symbols

    set symsNeeded "trgLibInit trgAddTcl trgDelete trgChainSet trgOn trgOff"

    foreach sym $symsNeeded {
        if [catch {wtxSymFind -name $sym} symInfo] {
            endWaitCursor
            messageBox "The target $trigGlobal(targetName) does not support Triggering\nSymbol: '$sym' is missing\n\nThe application will be closed."
	    trigUnbindFromTarget $trigGlobal(targetName)
            windowClose trigOverview
            return
        }

        set trigGlobal($sym) [lindex $symInfo 1]
    }

    # We have the symbols we need

    # Check that library is initialised

    if {[catch {wtxDirectCall $trigGlobal(trgLibInit)}] != 0} {
        endWaitCursor
        messageBox "Triggering support cannot be initialized on the target\n\nThe application will be closed."
	trigUnbindFromTarget $trigGlobal(targetName)
        windowClose trigOverview
        return
    }

    # Delete existing triggers

    set ix 0
    set progress 0.0
    set progUnits [llength $trigList]
    set progUnits [expr 20.0 / $progUnits]

    foreach trigger $trigList {

	incr ix 
        set progress [expr $progUnits * $ix]
	statusMessageSet -progress 20 [expr int($progress)] "Deleting existing triggers"
	uiEventProcess -allowpaintonly

        if {$trigger == "New"} {
            continue
        }

        if {$trigID($trigger) != 0} {
            onTrigDelete supplied target $trigger
        }
    }
    statusMessageSet -normal ""

    # Set the triggers

    set arg [wtxMemAlloc 64]

#messageBox $arg

    set endianFlag "-[string range [targetEndian] 0 0]"
    set argLocal [memBlockCreate $endianFlag]

    set ix 0
    set progress 0

    foreach trigger $trigList {
	incr ix 
        set progress [expr $progUnits * $ix]
	statusMessageSet -progress 20 [expr int($progress)] "Installing triggers"
	uiEventProcess -allowpaintonly

        if {$trigger == "New"} {
            continue
        }

#       messageBox "Setting trigger $trigger"

        # Event

        if {[string compare $trigSelectedEvt($trigger) Any] == 0} {
            # Magic number equivalent to EVENT_ANY_EVENT in eventP.h
            set eventID 48
        } elseif {[string compare $trigSelectedEvt($trigger) "User Event"] == 0} {
            # Add in MIN_USER_ID (40000 decimal)

            set eventID [expr $trigSelectedEvtSpc($trigger) + 40000]

        } elseif {[string compare $trigSelectedEvt($trigger) "intEnt"] == 0} {

            # Add in MIN_INT_ID (102 decimal)

            set eventID [expr $trigSelectedEvtSpc($trigger) + 102]
        } else {
            set eventID $wvEventInfo(id,$trigSelectedEvt($trigger))
        }

#       messageBox "Event ID $eventID"
        memBlockSet -l $argLocal  0 $eventID

        # Initial State
        memBlockSet -l $argLocal  4 $trigSelectedEna($trigger)

        # Context Type
        set context [lsearch -exact $trigCxtList $trigSelectedCxt($trigger)]

        # kludge start

        if {$context == 4} {

            # Modify value for 'System' now that 'Specific ISR'
            # has been removed.

            set context 5
        }

        # kludge end

        memBlockSet -l $argLocal  8 $context

        # Context ID

        if {[string first Specific $trigSelectedCxt($trigger)] == -1} {
            set contextID 0 
        } elseif [catch {expr $trigSelectedCxtSpc($trigger)} result] {
            if [catch "shSymAddr $trigSelectedCxtSpc($trigger)" sym] {
		set sym [taskNameToId $trigSelectedCxtSpc($trigger)]
		if {$sym == 0} {
                    statusMessageSet -normal ""
                    endWaitCursor
		    messageBox "Context ID \"$trigSelectedCxtSpc($trigger)\" not found on target"
                    wtxToolDetach
                    return
		} else {
#                   messageBox "found task id $sym"
		    set contextID $sym
                }
            } else {
#               messageBox "found symbol $sym"
                set contextID $sym
                set contextID [wtxGopherEval "$contextID @l"]
            }
        } else {
#           messageBox "found expr $result"
	    set contextID $result
        }

#messageBox "Context ID $contextID"
        memBlockSet -l $argLocal 12 $contextID

        # Object ID

        if {[string compare $trigSelectedObj($trigger) "Any"] == 0} {
            set obj 0
        } elseif [catch {expr $trigSelectedObjSpc($trigger)} result] {
            if [catch "shSymAddr $trigSelectedObjSpc($trigger)" sym] {
		set sym [taskNameToId $trigSelectedObjSpc($trigger)]
		if {$sym == 0} {
                    statusMessageSet -normal ""
                    endWaitCursor
		    messageBox "ObjectID \"$trigSelectedObjSpc($trigger)\" not found on target"
		    trigUnbindFromTarget $trigGlobal(targetName)
                    return
		} else {
#                   messageBox "found task id $sym"
		    set obj $sym
                }
            } else {
#               messageBox "found symbol $sym"
                set obj $sym
                set obj [wtxGopherEval "$obj @l"]
            }
        } else {
#           messageBox "found expr $result"
	    set obj $result
        }

#messageBox "Object ID $obj"
        memBlockSet -l $argLocal 16 $obj

        # Conditional Flag
#messageBox "Cond flag $trigSelectedCond($trigger)"
        memBlockSet -l $argLocal 20 $trigSelectedCond($trigger)

        # Operand 1 Type
#messageBox "Operand 1 Type $trigSelectedFnVar($trigger)"
        memBlockSet -l $argLocal 24 $trigSelectedFnVar($trigger)

        # Operand 1 Value
#messageBox "Operand 1 Value $trigSelectedFnVarSym($trigger)"

        if {$trigSelectedCond($trigger) == 0} {
            set fnVarSym 0
        } elseif [catch {expr $trigSelectedFnVarSym($trigger)} result] {
            if [catch "shSymAddr $trigSelectedFnVarSym($trigger)" sym] {
		set sym [taskNameToId $trigSelectedFnVarSym($trigger)]
	 	if {$sym == 0} {
		    set fnVarSym 0 
                    statusMessageSet -normal ""
                    endWaitCursor
		    messageBox "Condition Fn/Var Sym \"$trigSelectedFnVarSym($trigger)\" not found on target"
		    trigUnbindFromTarget $trigGlobal(targetName)
                    return
		} else {
#                   messageBox "found task id $sym"
		    set fnVarSym $sym
                }
            } else {
               if {($cpuFamily($__wtxCpuType) == "thumb")} {
	            if {$trigSelectedFnVar($trigger) == 0} {
	                set sym [expr $sym | 0x1]
		    }
               }
               #messageBox "found symbol $sym"
               set fnVarSym $sym

               # Target side will retrieve value if necessary
            }
        } else {
           #messageBox "found expr $result"
	    # in case we pass an address we just use it!

	    set fnVarSym $result
        }

#messageBox "Fn/Var $fnVarSym"
        memBlockSet -l $argLocal 28 $fnVarSym

        # Relative Operator

        if {$trigSelectedCond($trigger) == 0} {
            set relOp 0
        } else {
            set relOp $trigSelectedRelOp($trigger)
        }

#messageBox "Rel Op $relOp"
        memBlockSet -l $argLocal 32 $relOp

        # Constant

        if {$trigSelectedCond($trigger) == 0} {
            set condArg 0
        } else {
            if [catch {expr $trigSelectedArg($trigger)} result] {
                if [catch "shSymAddr $trigSelectedArg($trigger)" sym] {
                    set sym [taskNameToId $trigSelectedArg($trigger)]
                    if {$sym == 0} {
                        statusMessageSet -normal ""
                        endWaitCursor
                        messageBox "Cond Arg \"$trigSelectedArg($trigger)\" not found on target"
			trigUnbindFromTarget $trigGlobal(targetName)
                        return
                    } else {
#                       messageBox "found task id $sym"
                        set condArg $sym
                    }
                } else {
#                   messageBox "found symbol $sym"
                    set condArg $sym
                }
            } else {
#               messageBox "found expr $result"
                set condArg $result
            }
        }

#messageBox "Constant $condArg"

        # Watch out for dodgy values, such as floating point!

        if [catch {memBlockSet -l $argLocal 36 $condArg} result] {
             statusMessageSet -normal ""
             endWaitCursor
             messageBox "Conditional trigger arg : $result"
	     trigUnbindFromTarget $trigGlobal(targetName)
             return
        }

        # Disable Flag
#messageBox "Disable $trigSelectedDis($trigger)"
        memBlockSet -l $argLocal 40 $trigSelectedDis($trigger)

        # Chain (set dummy value here)
        memBlockSet -l $argLocal 44 0

        # ActionType
        memBlockSet -l $argLocal 48 $trigSelectedAction($trigger)

        switch $trigSelectedAction($trigger) {
        0 {
#           messageBox "No action"
            set actFn 0
            set actFnArg 0
          }

        1 {
            # messageBox "Start WV"

            if {[initStartWV $trigGlobal(targetName)] == 0} {
                endWaitCursor
                messageBox "WindView Initialization Failed"
                return
            }

	    set wvGlobal(wvTrackButtEnable) 1
	    controlEnable wvTB.wvStopButt   1
	    controlEnable wvTB.wvStartButt  0
            catch {uploadRefreshView}

            set actFn    $wvGlobal(wvEvtLogStart)
            set actFnArg 0
          }

        2 {
#           messageBox "Stop WV"

            # OK, We need to make sure the kernel supports WV by
            # checking for the tell tale symbols

            set actFn $wvGlobal(wvEvtLogStop)
            set actFnArg 0
          }

        3 {
            # Action Fn

            if [catch {expr $trigSelectedActFn($trigger)} result] {
                if [catch "shSymAddr $trigSelectedActFn($trigger)" sym] {
                    set sym [taskNameToId $trigSelectedActFn($trigger)]
                    if {$sym == 0} {
                        statusMessageSet -normal ""
                        endWaitCursor
                        messageBox "Action Fn Sym \"$trigSelectedActFn($trigger)\" not found on target"
			trigUnbindFromTarget $trigGlobal(targetName)
                        return
                    } else {
#                       messageBox "found task id $sym"
		        set actFn $sym
                    }
                } else {
#                   messageBox "found symbol $sym"
                    if {($cpuFamily($__wtxCpuType) == "thumb")} {
		        set sym [expr $sym | 0x1]
		    }
                    set actFn $sym
                }
            } else {
#               messageBox "found expr $result"
	        set actFn $result
            }

            if [catch {expr $trigSelectedActFnArg($trigger)} result] {
                if [catch "shSymAddr $trigSelectedActFnArg($trigger)" sym] {
	            set sym [taskNameToId $trigSelectedActFnArg($trigger)]
	            if {$sym == 0} {
                        statusMessageSet -normal ""
                        endWaitCursor
		        messageBox "Action Fn Arg \"$trigSelectedActFnArg($trigger)\" not found on target"
			trigUnbindFromTarget $trigGlobal(targetName)
                        return
		    } else {
#                       messageBox "found task id $sym"
		        set actFnArg $sym
                    }
                } else {
#                   messageBox "found symbol $sym"
                    set actFnArg $sym
                    set actFnArg [wtxGopherEval "$actFnArg @l"]
                }
            } else {
#               messageBox "found expr $result"
	        set actFnArg $result
            }
        }

        default {
             statusMessageSet -normal ""
             endWaitCursor
             messageBox "Action not recognized!"
	     trigUnbindFromTarget $trigGlobal(targetName)
             return
            }
        }

#messageBox "Action Fn $actFn"
#messageBox "Action Fn Arg $actFnArg"

        memBlockSet -l $argLocal 52 $actFn

        # Action Deferred Flag
#messageBox "Action Deferred $trigSelectedActDefer($trigger)"
        memBlockSet -l $argLocal 56 $trigSelectedActDefer($trigger)

        memBlockSet -l $argLocal 60 $actFnArg

        wtxMemWrite $argLocal $arg

        set trigID($trigger) [wtxDirectCall $trigGlobal(trgAddTcl) $arg]

        trigRefreshView
    }
    statusMessageSet -normal ""

    wtxMemFree $arg
    memBlockDelete $argLocal

    foreach trigger $trigList {
        if {$trigger == "New"} {
            continue
        }

        if {[string compare $trigSelectedChain($trigger) "(none)"] != 0} {
#messageBox "Setting Chain $trigger -> $trigSelectedChain($trigger)"
#messageBox "Setting Chain $trigID($trigger) -> $trigID($trigSelectedChain($trigger))"
            wtxDirectCall $trigGlobal(trgChainSet) $trigID($trigger) $trigID($trigSelectedChain($trigger))
        }
    }

    wtxDirectCall $trigGlobal(trgOn)

    set trigGlobal(allowMaint)          0

    controlEnable trigTB.trigCreateButt 0
    controlEnable trigTB.trigMaintButt  0
    controlEnable trigTB.trigDelButt    0
    controlEnable trigTB.trigSaveButt   0
    controlEnable trigTB.trigLoadButt   0
    controlEnable trigTB.trigStartButt  0
    controlEnable trigTB.trigStopButt   1
    controlEnable trigTB.trigLookButt   1
    controlEnable trigTB.trigLookTimerButt  1

    endWaitCursor
}

proc trigStop {} {
    global trigGlobal
    global wvGlobal

    beginWaitCursor

    # Connect to target
    if [catch {trigBindToTarget $trigGlobal(targetName)} result] {
        messageBox "Bind error $result"
    }

    # Turn off triggering
    catch {wtxDirectCall $trigGlobal(trgOff)} result

    # Close down an upload link if controlling WV
    if {$wvGlobal(uplinkActive) == 1} {
         wvCloseUpload
         catch {uploadRefreshView}
    }

    set trigGlobal(allowMaint)          1

    controlEnable trigTB.trigCreateButt 1
    controlEnable trigTB.trigMaintButt  1
    controlEnable trigTB.trigDelButt    1
    controlEnable trigTB.trigSaveButt   1
    controlEnable trigTB.trigLoadButt   1
    controlEnable trigTB.trigStartButt  1
    controlEnable trigTB.trigStopButt   0

    trigRefreshView

    if {[windowExists eventLogging] } {
        controlEnable wvTB.wvStopButt   0
        controlEnable wvTB.wvStartButt  1
    }

    endWaitCursor
}

proc onTrigCancel {} {
    global trigGlobal
    global trigList
    global trigTempList
    global trigCancelAll

    global trigSelectedEna
    global trigSelectedEvt
    global trigSelectedEvtSpc
    global trigSelectedCxt
    global trigSelectedCxtSpc
    global trigSelectedObj
    global trigSelectedObjSpc
    global trigSelectedCond
    global trigSelectedFnVar
    global trigSelectedFnVarSym
    global trigSelectedRelOp
    global trigSelectedArg
    global trigSelectedAction
    global trigSelectedActFn
    global trigSelectedActFnArg
    global trigSelectedActDefer
    global trigSelectedDis
    global trigSelectedChain

    global trigTempSaveEna 
    global trigTempSaveEvt 
    global trigTempSaveEvtSpc 
    global trigTempSaveCxt 
    global trigTempSaveCxtSpc 
    global trigTempSaveObj 
    global trigTempSaveObjSpc 
    global trigTempSaveCond 
    global trigTempSaveFnVar 
    global trigTempSaveFnVarSym 
    global trigTempSaveRelOp 
    global trigTempSaveAction 
    global trigTempSaveActDefer 
    global trigTempSaveDis 
    global trigTempSaveChain 
    global trigTempSaveActFnArg
    global trigTempSaveActFn
    global trigTempSaveArg

    ## Trawl through all the triggers and compare each parameter to the temporary
    ## store.  If we find a non-matching pair we need to check to see whether the
    ## user is sure he wants to cancel

    set needCancelCheck 0

    foreach trg $trigList {
    catch { \
	if { $trigSelectedEna($trg) != $trigTempSaveEna($trg) || \
	    $trigSelectedEvt($trg) != $trigTempSaveEvt($trg) || \
	    $trigSelectedEvtSpc($trg) != $trigTempSaveEvtSpc($trg) || \
	    $trigSelectedCxt($trg) != $trigTempSaveCxt($trg) || \
	    $trigSelectedCxtSpc($trg) != $trigTempSaveCxtSpc($trg) || \
	    $trigSelectedObj($trg) != $trigTempSaveObj($trg) || \
	    $trigSelectedObjSpc($trg) != $trigTempSaveObjSpc($trg) || \
	    $trigSelectedCond($trg) != $trigTempSaveCond($trg) || \
	    $trigSelectedFnVar($trg) != $trigTempSaveFnVar($trg) || \
	    $trigSelectedFnVarSym($trg) != $trigTempSaveFnVarSym($trg) || \
	    $trigSelectedRelOp($trg) != $trigTempSaveRelOp($trg) || \
	    $trigSelectedAction($trg) != $trigTempSaveAction($trg) || \
    	    $trigSelectedActDefer($trg) != $trigTempSaveActDefer($trg) || \
	    $trigSelectedDis($trg) != $trigTempSaveDis($trg) || \
	    $trigSelectedChain($trg) != $trigTempSaveChain($trg) || \
	    $trigSelectedActFn($trg) != $trigTempSaveActFn($trg) || \
	    $trigSelectedActFnArg($trg) != $trigTempSaveActFnArg($trg) || \
	    $trigSelectedArg($trg) != $trigTempSaveArg($trg)} {
	
		set needCancelCheck 1
		break;
	    }
	}
    }

    ## Only check the cancel if the info for the triggers has changed in any way
    if  {[llength $trigList] > 0 && $needCancelCheck == 1} {
	trigCancelCheck

	if {$trigCancelAll == 0} {
	    return
	    }

        foreach trg $trigList {
	    set trg [controlSelectionGet trigConfig.trigNum -string]
	    if { [catch {set trigSelectedEna($trg) $trigTempSaveEna($trg)}] == 0 } {
		set trigSelectedEvt($trg)      $trigTempSaveEvt($trg)
		set trigSelectedEvtSpc($trg)   $trigTempSaveEvtSpc($trg)
		set trigSelectedCxt($trg)      $trigTempSaveCxt($trg)
		set trigSelectedCxtSpc($trg)   $trigTempSaveCxtSpc($trg)
		set trigSelectedObj($trg)      $trigTempSaveObj($trg)
		set trigSelectedObjSpc($trg)   $trigTempSaveObjSpc($trg)
		set trigSelectedCond($trg)     $trigTempSaveCond($trg)
		set trigSelectedFnVar($trg)    $trigTempSaveFnVar($trg)
		set trigSelectedAction($trg)   $trigTempSaveAction($trg)
		set trigSelectedFnVarSym($trg) $trigTempSaveFnVarSym($trg)
		set trigSelectedRelOp($trg)    $trigTempSaveRelOp($trg)
		set trigSelectedActDefer($trg) $trigTempSaveActDefer($trg)
		set trigSelectedDis($trg)      $trigTempSaveDis($trg)
		set trigSelectedChain($trg)    $trigTempSaveChain($trg)
		set trigSelectedActFn($trg)    $trigTempSaveActFn($trg)
		set trigSelectedActFnArg($trg) $trigTempSaveActFnArg($trg)
		set trigSelectedArg($trg)      $trigTempSaveArg($trg)

		unset trigTempSaveEna($trg)
		unset trigTempSaveEvt($trg)
		unset trigTempSaveEvtSpc($trg)
		unset trigTempSaveCxt($trg)
		unset trigTempSaveCxtSpc($trg)
		unset trigTempSaveObj($trg)
		unset trigTempSaveObjSpc($trg)
		unset trigTempSaveCond($trg)
		unset trigTempSaveFnVar($trg)
		unset trigTempSaveFnVarSym($trg)
		unset trigTempSaveRelOp($trg)
		unset trigTempSaveArg($trg)
		unset trigTempSaveAction($trg)
		unset trigTempSaveActFn($trg)
		unset trigTempSaveActFnArg($trg)
		unset trigTempSaveActDefer($trg)
		unset trigTempSaveDis($trg)
		unset trigTempSaveChain($trg)
	    }
	}
    } 
    
    set trigTempList {}
    trigRefreshView

    windowClose trigConfig
}

proc onTrigOK {} {
    global trigGlobal
    global trigList
    global trigTempList
    global trigSelectedAction
    global wvGlobal

    set trigGlobal(notSaved) 1

    #set trigNum [controlSelectionGet trigConfig.trigNum -string]

    # search for the trigger in the list

    foreach trg $trigTempList {
    set currTrig [lsearch -exact $trigList $trg]
    if {$currTrig == -1} break
    }

    # The trigger is a New one, we need to put it in trigList
    if  {[llength $trigTempList] > 0} {
        set trigList [lsort $trigTempList]
    }

    trigRefreshView

    set trigGlobal(allowMaint)         1

    controlEnable trigTB.trigMaintButt 1
    controlEnable trigTB.trigDelButt   1
    controlEnable trigTB.trigStartButt 1

    foreach trg $trigList {
        if {($trigSelectedAction($trg) == 1) && ($wvGlobal(config.ok) == 0)} {
            if {![windowExists eventLogging] && ![windowExists selTarget]} {
		statusMessageSet -normal "Starting Windview"
						
                toolSelectCallback LaunchWindView
                
            } else {
            wvConfigDialog $trigGlobal(targetName) 
	    }

            if {$wvGlobal(config.ok) != 1} {
                messageBox "Please configure WV correctly or change action"
		# reset the trigger list by deleting the new trigger

                if  {[llength $trigTempList] > 0} {
                    set trigList [lreplace $trigList $trg $trg]
		}

                return
            } else {
	        break
	    }
        }
    }

    set trigTempList {}

    windowClose trigConfig
}

proc onCmdUItrigQuickDel {} {
    global enableflag
    global trigList
    global trigGlobal

    set enableflag [windowExists trigOverview]

    if { [controlSelectionGet trigOverview.trigInfo] == "" || [llength $trigList] == 0} {
	set enableflag 0
	set trigGlobal(trigSel) ""
    } else {
	set enableflag 1
    }

}


proc trigQuickDel {} {

    set trigInfoList [controlValuesGet trigOverview.trigInfo]
    set selectedLine [lindex $trigInfoList [controlSelectionGet trigOverview.trigInfo]]

    set selectedLine [string trim [lindex $selectedLine 0] \{\}]

    if {[string match {*\#*} $selectedLine]} {
        scan $selectedLine "#%2d" selectedTrg

        onTrigDelete supplied both [format "%2d" $selectedTrg]
        controlSelectionSet trigOverview.trigInfo 0
    }
}

proc onTrigDelete {which where trigToDelete} {

    global trigGlobal
    global trigID
    global trigList

    global trigSelectedEna
    global trigSelectedEvt
    global trigSelectedEvtSpc
    global trigSelectedCxt
    global trigSelectedCxtSpc
    global trigSelectedObj
    global trigSelectedObjSpc
    global trigSelectedCond
    global trigSelectedFnVar
    global trigSelectedFnVarSym
    global trigSelectedRelOp
    global trigSelectedArg
    global trigSelectedAction
    global trigSelectedActFn
    global trigSelectedActFnArg
    global trigSelectedActDefer
    global trigSelectedDis
    global trigSelectedChain

    global trigTimerButtonActive

    beginWaitCursor

#puts "Deleting Trigger ($which) #$trigToDelete on $where"

    # Connect to target

    if { [trigBindToTarget $trigGlobal(targetName)] == 0 } {
	endWaitCursor
        return
    }

    # OK, we attached. Now we need to make sure the kernel supports
    # triggering by checking for the tell tale symbols

    if [catch {wtxSymFind -name "trgDelete"} symInfo] {
        messageBox "The target $trigGlobal(targetName) does not support triggering"
	trigUnbindFromTarget $trigGlobal(targetName)
    }

    set trigGlobal(trgDelete) [lindex $symInfo 1]

    if {[string compare $which read] == 0} {
        set trigNum [controlSelectionGet trigConfig.trigNum]
    } else {
        set trigNum $trigToDelete
    }

    # Delete this trigger from the target

    if {$trigID($trigNum) != 0} {
        if {[wtxDirectCall $trigGlobal(trgDelete) $trigID($trigNum)] != 0} {
            messageBox "Trigger #$trigNum not found on target, assuming reboot"
            set trigID($trigNum) 0
        } else {
            set trigID($trigNum) 0
        } 
    }

    if {[string compare $where target] == 0} {
        trigRefreshView
        endWaitCursor

        # Bail out early if only deleting trigger on target
        return
    }

    # Delete this trigger from the list

    set currTrig [lsearch -exact $trigList $trigNum]
    set trigList [lreplace $trigList $currTrig $currTrig]

    if {[string compare $which read] == 0} {
        controlValuesSet trigConfig.trigNum $trigList
    }

    # ...and unset its associated attributes

    unset trigID($trigNum)
    unset trigSelectedEna($trigNum)
    unset trigSelectedEvt($trigNum)
    unset trigSelectedEvtSpc($trigNum)
    unset trigSelectedCxt($trigNum)
    unset trigSelectedCxtSpc($trigNum)
    unset trigSelectedObj($trigNum)
    unset trigSelectedObjSpc($trigNum)
    unset trigSelectedCond($trigNum)
    unset trigSelectedFnVar($trigNum)
    unset trigSelectedFnVarSym($trigNum)
    unset trigSelectedRelOp($trigNum)
    unset trigSelectedArg($trigNum)
    unset trigSelectedAction($trigNum)
    unset trigSelectedActFn($trigNum)
    unset trigSelectedActFnArg($trigNum)
    unset trigSelectedActDefer($trigNum)
    unset trigSelectedDis($trigNum)
    unset trigSelectedChain($trigNum)

    trigRefreshView

    if {[llength $trigList] == 0} {
        endWaitCursor

        # no triggers remaining

        set trigGlobal(allowMaint)         0

        controlEnable trigTB.trigMaintButt 0
        controlEnable trigTB.trigDelButt   0
        controlEnable trigTB.trigStartButt 0

        statusMessageSet -normal "All triggers deleted"

	set trigGlobal(notSaved) 0
	set trigOkToDiscard 1
        return
    } else {
         set newTrig [lindex $trigList 0]
    }

    if {[string compare $which read] == 0} {
        controlSelectionSet trigConfig.trigNum -string $newTrig
    }

    endWaitCursor
}

proc onTrigMaint {selection} {

    set event [controlEventGet trigConfig.trigNum]

    global trigID
    global trigTempList
    global trigList
    global trigChainList

    global trigSelected
    global trigSelectedEna
    global trigSelectedEvt
    global trigSelectedEvtSpc
    global trigSelectedCxt
    global trigSelectedCxtSpc
    global trigSelectedObj
    global trigSelectedObjSpc
    global trigSelectedCond
    global trigSelectedFnVar
    global trigSelectedFnVarSym
    global trigSelectedRelOp
    global trigSelectedArg
    global trigSelectedAction
    global trigSelectedActFn
    global trigSelectedActFnArg
    global trigSelectedActDefer
    global trigSelectedDis
    global trigSelectedChain

    global trigTempSaveEna 
    global trigTempSaveEvt 
    global trigTempSaveEvtSpc 
    global trigTempSaveCxt 
    global trigTempSaveCxtSpc 
    global trigTempSaveObj 
    global trigTempSaveObjSpc 
    global trigTempSaveCond 
    global trigTempSaveFnVar 
    global trigTempSaveFnVarSym 
    global trigTempSaveRelOp 
    global trigTempSaveArg
    global trigTempSaveAction 
    global trigTempSaveActFnArg
    global trigTempSaveActFn
    global trigTempSaveActDefer 
    global trigTempSaveDis 
    global trigTempSaveChain 


    global trigGlobal
    global wvGlobal

    ## Here we are checking the selection matches the trigger number.
    ## If not we set the selection to look at the correct trigger so that
    ## it appears correctly in the maintenance dialog

    if {[string compare $selection New] != 0 && [string compare $selection read] != 0 } {
	if { [lindex $trigList $selection] != $selection && [lindex $trigList $selection] !=""} {
	    set selection [lindex $trigList $selection]
	}
    }

    if {[string compare $selection read] == 0} {
        set trigNum [controlSelectionGet trigConfig.trigNum -string]
    } else {
        set trigNum $selection
        controlSelectionSet trigConfig.trigNum -string $selection
    }

    # Set all selections according to selected trigger

    if {[string compare $trigNum New] == 0} {

        # Allow this trigger to be configured

        controlEnable trigConfig.trigNum 1 
        controlEnable trigConfig.trigLEna 1 
        controlEnable trigConfig.trigEna 1
        controlEnable trigConfig.trigLEvt 1
        controlEnable trigConfig.trigEvt 1
        controlEnable trigConfig.trigLCxt 1
        controlEnable trigConfig.trigCxt 1
        controlEnable trigConfig.trigLObj 1
        controlEnable trigConfig.trigObj 1
        controlEnable trigConfig.trigCond 1 

        # Adding new trigger

        # Determine host trigger ID by looking for next free number

        set count 0

        while {[lsearch -exact $trigList [format "%2d" $count]] != -1} {
            incr count
        }

        set newTrig [format "%2d" $count]

	set trigTempList $trigList

	lappend trigTempList $newTrig

	set trigTempList [lsort $trigTempList]

        # Init values now so trigRefreshView will work

        # Set a dummy ID

        set trigID($newTrig) 0

        set trigSelectedEna($newTrig) 1
        set trigSelectedEvt($newTrig) "Any"
        set trigSelectedEvtSpc($newTrig) "<event ID>"
        set trigSelectedCxt($newTrig) "Any"
        set trigSelectedCxtSpc($newTrig) "<specific>"
        set trigSelectedObj($newTrig) "Any"
        set trigSelectedObjSpc($newTrig) "<specific>"
        set trigSelectedCond($newTrig)     0
        set trigSelectedFnVar($newTrig) 0
        set trigSelectedAction($newTrig)   0
        set trigSelectedFnVarSym($newTrig) "<operand1>"
        set trigSelectedRelOp($newTrig)    0
        set trigSelectedActDefer($newTrig) 1
        set trigSelectedDis($newTrig) 1
        set trigSelectedChain($newTrig) "(none)"

        # Don't allow chaining to current trigger

        # We need a temp list, since trigList is saved only in onTrigOK
        set trigChainList [linsert $trigTempList 0 (none)]

        set currTrig [lsearch -exact $trigChainList $newTrig]
        set trigChainList [lreplace $trigChainList $currTrig $currTrig]

        controlValuesSet trigConfig.trigNum $trigTempList

        controlSelectionSet trigConfig.trigNum -noevent -string $newTrig

        controlCheckSet trigConfig.trigEna 1 

        controlSelectionSet trigConfig.trigEvt -noevent -string "Any"
        controlValuesSet trigConfig.trigEvtSpc "<event ID>"
        controlEnable trigConfig.trigEvtSpc 0

        controlSelectionSet trigConfig.trigCxt -noevent -string "Any"
        controlValuesSet trigConfig.trigCxtSpc "<specific>"
        controlEnable trigConfig.trigCxtSpc 0

        controlSelectionSet trigConfig.trigObj -noevent -string "Any"
        controlValuesSet trigConfig.trigObjSpc "<specific>"
        controlEnable trigConfig.trigObjSpc 0

        controlSelectionSet trigConfig.trigCond -noevent 0
        controlSelectionSet trigConfig.trigFnVar -noevent 0
        controlValuesSet trigConfig.trigFnVarSym "<operand1>"
        controlSelectionSet trigConfig.trigRelOp -noevent 0
        controlValuesSet trigConfig.trigArg "<operand2>"

        controlSelectionSet trigConfig.trigAction -noevent 0
        controlEnable trigConfig.trigAction 1

        controlValuesSet trigConfig.trigActFn "<function>"
        controlEnable trigConfig.trigActFn 0
        controlValuesSet trigConfig.trigActFnArg "<fn argument>"
        controlEnable trigConfig.trigActFnArg 0
        controlCheckSet trigConfig.trigDefer 1 
        controlEnable trigConfig.trigDefer 0

        controlCheckSet trigConfig.trigDisable 1 
        controlEnable trigConfig.trigLDis 1 
        controlEnable trigConfig.trigDisable 1 

        controlEnable trigConfig.trigLChain 1 
        controlEnable trigConfig.trigChain 1 
        controlValuesSet trigConfig.trigChain $trigChainList
        controlSelectionSet trigConfig.trigChain -noevent -string "(none)"

    } elseif {[string compare $trigNum ""] != 0 && \
	      $event == "selchange"} {

        # Before making any change, let's save the selected trigger

        set trigTempSaveEna($trigNum) $trigSelectedEna($trigNum)
        set trigTempSaveEvt($trigNum) $trigSelectedEvt($trigNum)
        set trigTempSaveEvtSpc($trigNum) $trigSelectedEvtSpc($trigNum) 
        set trigTempSaveCxt($trigNum) $trigSelectedCxt($trigNum) 
        set trigTempSaveCxtSpc($trigNum) $trigSelectedCxtSpc($trigNum)
        set trigTempSaveObj($trigNum) $trigSelectedObj($trigNum) 
        set trigTempSaveObjSpc($trigNum) $trigSelectedObjSpc($trigNum)
        set trigTempSaveCond($trigNum) $trigSelectedCond($trigNum) 
        set trigTempSaveFnVar($trigNum) $trigSelectedFnVar($trigNum) 
        set trigTempSaveAction($trigNum) $trigSelectedAction($trigNum)   
        set trigTempSaveActFnArg($trigNum) $trigSelectedActFnArg($trigNum) 
        set trigTempSaveActFn($trigNum) $trigSelectedActFn($trigNum) 
        set trigTempSaveFnVarSym($trigNum) $trigSelectedFnVarSym($trigNum)
        set trigTempSaveRelOp($trigNum) $trigSelectedRelOp($trigNum)    
        set trigTempSaveArg($trigNum) $trigSelectedArg($trigNum) 
        set trigTempSaveActDefer($trigNum) $trigSelectedActDefer($trigNum)
        set trigTempSaveDis($trigNum) $trigSelectedDis($trigNum) 
        set trigTempSaveChain($trigNum) $trigSelectedChain($trigNum) 

        # Select existing trigger

        controlEnable trigConfig.trigLEna 1 
        controlEnable trigConfig.trigEna 1
        controlEnable trigConfig.trigLEvt 1
        controlEnable trigConfig.trigEvt 1
        controlEnable trigConfig.trigLCxt 1
        controlEnable trigConfig.trigCxt 1
        controlEnable trigConfig.trigLObj 1
        controlEnable trigConfig.trigObj 1
        controlEnable trigConfig.trigCond 1 
        controlEnable trigConfig.trigAction 1
        controlEnable trigConfig.trigLDis 1 
        controlEnable trigConfig.trigDisable 1 
        controlEnable trigConfig.trigLChain 1 
        controlEnable trigConfig.trigChain 1 

        controlCheckSet trigConfig.trigEna \
            $trigSelectedEna($trigNum)

        controlSelectionSet trigConfig.trigEvt \
            -noevent -string $trigSelectedEvt($trigNum)

        controlValuesSet trigConfig.trigEvtSpc $trigSelectedEvtSpc($trigNum)

        if {[string first User $trigSelectedEvt($trigNum)] == -1 &&
            [string first intEnt $trigSelectedEvt($trigNum)] == -1} {
            controlEnable trigConfig.trigEvtSpc 0
        } else {
            controlEnable trigConfig.trigEvtSpc 1
        }

        controlSelectionSet trigConfig.trigCxt \
            -noevent -string $trigSelectedCxt($trigNum)

        controlValuesSet trigConfig.trigCxtSpc $trigSelectedCxtSpc($trigNum)

        if {[string first Specific $trigSelectedCxt($trigNum)] == -1} {
            controlEnable trigConfig.trigCxtSpc 0
        } else {
            controlEnable trigConfig.trigCxtSpc 1
        }

        controlSelectionSet trigConfig.trigObj \
            -noevent -string $trigSelectedObj($trigNum)

        controlValuesSet trigConfig.trigObjSpc $trigSelectedObjSpc($trigNum)

        if {[string first Specific $trigSelectedObj($trigNum)] == -1} {
            controlEnable trigConfig.trigObjSpc 0
        } else {
            controlEnable trigConfig.trigObjSpc 1
        }

        controlSelectionSet trigConfig.trigCond \
            -noevent $trigSelectedCond($trigNum)

        controlSelectionSet trigConfig.trigFnVar \
            -noevent $trigSelectedFnVar($trigNum)

        controlValuesSet trigConfig.trigFnVarSym \
            $trigSelectedFnVarSym($trigNum)

        controlSelectionSet trigConfig.trigRelOp \
            -noevent $trigSelectedRelOp($trigNum)

        controlValuesSet trigConfig.trigArg \
            $trigSelectedArg($trigNum)

        if {$trigSelectedCond($trigNum) == 1} {
            controlEnable trigConfig.trigFnVar 1
            controlEnable trigConfig.trigFnVarSym 1
            controlEnable trigConfig.trigRelOp 1
            controlEnable trigConfig.trigArg 1
        } else {
            controlEnable trigConfig.trigFnVar 0
            controlEnable trigConfig.trigFnVarSym 0
            controlEnable trigConfig.trigRelOp 0
            controlEnable trigConfig.trigArg 0
        }

        controlSelectionSet trigConfig.trigAction \
            -noevent $trigSelectedAction($trigNum)

        controlValuesSet trigConfig.trigActFn \
             $trigSelectedActFn($trigNum)

        controlValuesSet trigConfig.trigActFnArg \
             $trigSelectedActFnArg($trigNum)

        set action [controlSelectionGet trigConfig.trigAction]

        switch $action {
            1 -
            2  {
	    ## action is Start/Stop WV
            controlEnable trigConfig.trigActFn 0
            controlEnable trigConfig.trigActFnArg 0
            controlEnable trigConfig.trigDefer 0

            }

            3 {
	    ## action is Call Function
            controlEnable trigConfig.trigActFn 1
            controlEnable trigConfig.trigActFnArg 1
            controlEnable trigConfig.trigDefer 1
            }

            default {
	    ## action is default
            controlEnable trigConfig.trigActFn 0
            controlEnable trigConfig.trigActFnArg 0
            controlEnable trigConfig.trigDefer 0
            }
        }

        # can only do CheckSets on enabled controls

        controlCheckSet trigConfig.trigDefer \
            $trigSelectedActDefer($trigNum)

        controlCheckSet trigConfig.trigDisable \
            $trigSelectedDis($trigNum)

        # Don't allow chaining to current trigger

        set trigChainList [linsert $trigList 0 (none)]
        set currTrig [lsearch -exact $trigChainList $trigNum]
        set trigChainList [lreplace $trigChainList $currTrig $currTrig]

        controlValuesSet trigConfig.trigChain $trigChainList

        controlSelectionSet trigConfig.trigChain \
            -string $trigSelectedChain($trigNum)
    }

}

proc onTrigEnable {} {
    global trigSelectedEna

    set v [controlChecked trigConfig.trigEna]

    set trigNum [controlSelectionGet trigConfig.trigNum -string]

    set trigSelectedEna($trigNum) $v
}

proc onTrigEvt {} {
    global trigSelectedEvt
    global trigSelectedEvtSpc

    set trigNum [controlSelectionGet trigConfig.trigNum -string]

    set selectedEvt [controlSelectionGet trigConfig.trigEvt -string]

    set trigSelectedEvt($trigNum) $selectedEvt

    if {[string first User $selectedEvt] == -1 &&
        [string first intEnt $selectedEvt] == -1} {
        controlEnable trigConfig.trigEvtSpc 0
    } else {
        controlEnable trigConfig.trigEvtSpc 1
    }


    if {[string first User $selectedEvt] != -1 ||
        [string first intEnt $selectedEvt] != -1} {

        if {[string first User $selectedEvt] == 0} {
           if {[string match $trigSelectedEvtSpc($trigNum) "<event ID>"] == 0} {

            controlValuesSet trigConfig.trigEvtSpc "<event ID>"
            set trigSelectedEvtSpc($trigNum) "<event ID>"
}
        }

        if {[string first intEnt $selectedEvt] == 0} {
            if {[string match $trigSelectedEvtSpc($trigNum) "<interrupt level>"] == 0} {
            controlValuesSet trigConfig.trigEvtSpc "<interrupt level>"
            set trigSelectedEvtSpc($trigNum) "<interrupt level>"
        }
}
    }
}

proc onTrigEvtSpc {} {
    global trigSelectedEvtSpc

    set trigNum [controlSelectionGet trigConfig.trigNum -string]

    set selectedEvtSpc [controlValuesGet trigConfig.trigEvtSpc]

    set trigSelectedEvtSpc($trigNum) $selectedEvtSpc
}

proc onTrigCxt {} {
    global trigSelectedCxt

    set trigNum [controlSelectionGet trigConfig.trigNum -string]

    set selectedCxt [controlSelectionGet trigConfig.trigCxt -string]

    set trigSelectedCxt($trigNum) $selectedCxt

    if {[string first Specific $selectedCxt] == -1} {
        controlEnable trigConfig.trigCxtSpc 0
    } else {
        controlEnable trigConfig.trigCxtSpc 1
    }
}

proc onTrigCxtSpc {} {
    global trigSelectedCxtSpc

    set trigNum [controlSelectionGet trigConfig.trigNum -string]

    set selectedCxtSpc [controlValuesGet trigConfig.trigCxtSpc]

    set trigSelectedCxtSpc($trigNum) $selectedCxtSpc
}

proc onTrigObj {} {
    global trigSelectedObj

    set trigNum [controlSelectionGet trigConfig.trigNum -string]

    set selectedObj [controlSelectionGet trigConfig.trigObj -string]

    set trigSelectedObj($trigNum) $selectedObj

    if {[string first Specific $selectedObj] == -1} {
        controlEnable trigConfig.trigObjSpc 0
    } else {
        controlEnable trigConfig.trigObjSpc 1
    }
}

proc onTrigObjSpc {} {
    global trigSelectedObjSpc

    set trigNum [controlSelectionGet trigConfig.trigNum -string]

    set selectedObjSpc [controlValuesGet trigConfig.trigObjSpc]

    set trigSelectedObjSpc($trigNum) $selectedObjSpc
}

proc onTrigCond {} {
    global trigSelectedCond

    set trigNum [controlSelectionGet trigConfig.trigNum -string]

    set selectedCond [controlSelectionGet trigConfig.trigCond]

    set trigSelectedCond($trigNum) $selectedCond

    if {$selectedCond == 1} {
        controlEnable trigConfig.trigFnVar 1
        controlEnable trigConfig.trigFnVarSym 1
        controlEnable trigConfig.trigRelOp 1
        controlEnable trigConfig.trigArg 1
    } else {
        controlEnable trigConfig.trigFnVar 0
        controlEnable trigConfig.trigFnVarSym 0
        controlEnable trigConfig.trigRelOp 0
        controlEnable trigConfig.trigArg 0
    }
}


proc onTrigFnVar {} {
    global trigSelectedFnVar

    set trigNum [controlSelectionGet trigConfig.trigNum -string]

    set selectedFnVar [controlSelectionGet trigConfig.trigFnVar]

    set trigSelectedFnVar($trigNum) $selectedFnVar
}

proc onTrigFnVarSym {} {
    global trigSelectedFnVarSym

    set trigNum [controlSelectionGet trigConfig.trigNum -string]

    set selectedFnVarSym [controlValuesGet trigConfig.trigFnVarSym]

    set trigSelectedFnVarSym($trigNum) $selectedFnVarSym
}

proc onTrigRelOp {} {
    global trigSelectedRelOp

    set trigNum [controlSelectionGet trigConfig.trigNum -string]

    set selectedRelOp [controlSelectionGet trigConfig.trigRelOp]

    set trigSelectedRelOp($trigNum) $selectedRelOp
}

proc onTrigArg {} {
    global trigSelectedArg

    set trigNum [controlSelectionGet trigConfig.trigNum -string]

    set selectedArg [controlValuesGet trigConfig.trigArg]

    set trigSelectedArg($trigNum) $selectedArg
}

proc onTrigAction {} {
    global trigSelectedAction

    set trigNum [controlSelectionGet trigConfig.trigNum -string]

    set selectedAction [controlSelectionGet trigConfig.trigAction]

    set trigSelectedAction($trigNum) $selectedAction

    set actionString [controlSelectionGet trigConfig.trigAction -string]

    if {[string compare $actionString "Call function"] == 0} {
        controlEnable trigConfig.trigActFn 1
        controlEnable trigConfig.trigActFnArg 1
        controlEnable trigConfig.trigDefer 1
    } else {
        controlEnable trigConfig.trigActFn 0
        controlEnable trigConfig.trigActFnArg 0
        controlEnable trigConfig.trigDefer 0
    }
}

proc onTrigActFn {} {
    global trigSelectedActFn

    set trigNum [controlSelectionGet trigConfig.trigNum -string]

    set selectedActFn [controlValuesGet trigConfig.trigActFn]

    set trigSelectedActFn($trigNum) $selectedActFn
}

proc onTrigActFnArg {} {
    global trigSelectedActFnArg

    set trigNum [controlSelectionGet trigConfig.trigNum -string]

    set selectedActFnArg [controlValuesGet trigConfig.trigActFnArg]

    set trigSelectedActFnArg($trigNum) $selectedActFnArg
}

proc onTrigDefer {} {
    global trigSelectedActDefer

    set v [controlChecked trigConfig.trigDefer]

    set trigNum [controlSelectionGet trigConfig.trigNum -string]

    set trigSelectedActDefer($trigNum) $v

    if {$v == 0} {
        messageBox "You must ensure that the action will not be invoked from\n an interrupt service routine or that it is safe to do so.";
       }
}

proc onTrigDisable {} {
    global trigSelectedDis

    set v [controlChecked trigConfig.trigDisable]

    set trigNum [controlSelectionGet trigConfig.trigNum -string]

    set trigSelectedDis($trigNum) $v
}

proc onTrigChain {} {
    global trigSelectedChain
    global trigSelectedEna

    set trigNum [controlSelectionGet trigConfig.trigNum -string]
    set selectedChain [controlSelectionGet trigConfig.trigChain -string]

    if {[string compare $selectedChain $trigSelectedChain($trigNum)] != 0} {

        if {$selectedChain != "(none)"} {
            if {$trigSelectedEna($selectedChain) == 1} {
                messageBox -exclamationicon "Note that trigger being chained is currently enabled"
            }
        }

        set trigSelectedChain($trigNum) $selectedChain
    }
}

proc trigSaveFile {} {

global trigGlobal
global trigList
global trigChainList
global trigSelectedEna
global trigSelectedEvt
global trigSelectedEvtSpc
global trigSelectedCxt
global trigSelectedCxtSpc
global trigSelectedObj
global trigSelectedObjSpc
global trigSelectedCond
global trigSelectedFnVar
global trigSelectedFnVarSym
global trigSelectedRelOp
global trigSelectedArg
global trigSelectedAction
global trigSelectedActFn
global trigSelectedActFnArg
global trigSelectedActDefer
global trigSelectedDis
global trigSelectedChain

global trigOkToDiscard

    set fname [fileDialogCreate -savefile -overwriteprompt -pathmustexist -filefilters "Trigger Config Files (*.trg)|*.trg||" -title "Save Trigger Configuration"]

    beginWaitCursor

    set fname [string trim $fname "{}"]

    # timr
    # if no name returned, user selected cancel, so do nothing more
    if { $fname == ""} {
        endWaitCursor
        return
    }

    if { [string match *.trg $fname] == 0} {
        set fname [format "%s.trg" $fname]
    }

    set fd [open $fname w]

    puts $fd "$trigList"
    puts $fd "$trigChainList"

    foreach trg $trigList {

        if {$trg == "New"} {
            continue
        }

        set trgSaveLine {}

        lappend trgSaveLine [format "%s" $trg]
        lappend trgSaveLine [format "%s" $trigSelectedEna($trg)]
        lappend trgSaveLine [format "%s" $trigSelectedEvt($trg)]
        lappend trgSaveLine [format "%s" $trigSelectedEvtSpc($trg)]
        lappend trgSaveLine [format "%s" $trigSelectedCxt($trg)]
        lappend trgSaveLine [format "%s" $trigSelectedCxtSpc($trg)]
        lappend trgSaveLine [format "%s" $trigSelectedObj($trg)]
        lappend trgSaveLine [format "%s" $trigSelectedObjSpc($trg)]
        lappend trgSaveLine [format "%s" $trigSelectedCond($trg)]
        lappend trgSaveLine [format "%s" $trigSelectedFnVar($trg)]
        lappend trgSaveLine [format "%s" $trigSelectedFnVarSym($trg)]
        lappend trgSaveLine [format "%s" $trigSelectedRelOp($trg)]
        lappend trgSaveLine [format "%s" $trigSelectedArg($trg)]
        lappend trgSaveLine [format "%s" $trigSelectedAction($trg)]
        lappend trgSaveLine [format "%s" $trigSelectedActFn($trg)]
        lappend trgSaveLine [format "%s" $trigSelectedActFnArg($trg)]
        lappend trgSaveLine [format "%s" $trigSelectedActDefer($trg)]
        lappend trgSaveLine [format "%s" $trigSelectedDis($trg)]
        lappend trgSaveLine [format "%s" $trigSelectedChain($trg)]

        puts $fd $trgSaveLine
        
    }            

    set trigGlobal(notSaved) 0

    #timr
    set trigOkToDiscard 1

    close $fd

    endWaitCursor
}

proc trigLoadFile {} {

global trigGlobal

global trigList
global trigTempList
global trigChainList
global trigSelectedEna
global trigSelectedEvt
global trigSelectedEvtSpc
global trigSelectedCxt
global trigSelectedCxtSpc
global trigSelectedObj
global trigSelectedObjSpc
global trigSelectedCond
global trigSelectedFnVar
global trigSelectedFnVarSym
global trigSelectedRelOp
global trigSelectedArg
global trigSelectedAction
global trigSelectedActFn
global trigSelectedActFnArg
global trigSelectedActDefer
global trigSelectedDis
global trigSelectedChain

global trigID

global wvGlobal

global trigOkToDiscard

    trigSaveCheck

    # timr
    if { $trigOkToDiscard == 0 } { 
        return 
    }

    set fname [fileDialogCreate -filemustexist -filefilters "Trigger Config Files (*.trg)|*.trg||" -title "Load Trigger Configuration"]

    if {$fname == ""} {
        return
    }

    set trigGlobal(notSaved) 0

    beginWaitCursor

    set fd [open [string trim $fname "{}"] r]

    gets $fd readLine
    set trigList $readLine
    set trigTempList $readLine

    gets $fd readLine
    set trigChainList $readLine

    while {[gets $fd readLine] != -1} {

        set trg [lindex $readLine 0]

        set trigSelectedEna($trg)      [lindex $readLine 1]
        set trigSelectedEvt($trg)      [lindex $readLine 2]
        set trigSelectedEvtSpc($trg)   [lindex $readLine 3]
        set trigSelectedCxt($trg)      [lindex $readLine 4]
        set trigSelectedCxtSpc($trg)   [lindex $readLine 5]
        set trigSelectedObj($trg)      [lindex $readLine 6]
        set trigSelectedObjSpc($trg)   [lindex $readLine 7]
        set trigSelectedCond($trg)     [lindex $readLine 8]
        set trigSelectedFnVar($trg)    [lindex $readLine 9]
        set trigSelectedFnVarSym($trg) [lindex $readLine 10]
        set trigSelectedRelOp($trg)    [lindex $readLine 11]
        set trigSelectedArg($trg)      [lindex $readLine 12]
        set trigSelectedAction($trg)   [lindex $readLine 13]
        set trigSelectedActFn($trg)    [lindex $readLine 14]
        set trigSelectedActFnArg($trg) [lindex $readLine 15]
        set trigSelectedActDefer($trg) [lindex $readLine 16]
        set trigSelectedDis($trg)      [lindex $readLine 17]
        set trigSelectedChain($trg)    [lindex $readLine 18]


        # Give the loaded trigger a NULL target trigger ID

        set trigID($trg) 0
    }            

    close $fd

    if  {[llength $trigList] > 0} { # This becomes 1 if restoring 'New'

        set trigGlobal(allowMaint)          1

        controlEnable trigTB.trigMaintButt  1
        controlEnable trigTB.trigDelButt    1
        controlEnable trigTB.trigStartButt  1
        controlEnable trigTB.trigStopButt   0

        foreach trg $trigList {
            if {$trigSelectedAction($trg) == 1 &&
                $wvGlobal(config.ok) == 0 } {
		statusMessageSet -normal "Starting Windview"
                toolSelectCallback LaunchWindView
            }
        }
    }

    trigRefreshView

    set trigGlobal(notSaved) 0
    set trigOkToDiscard 1

    endWaitCursor
}

proc adjustTrgOverview {} {

    if [controlExists trigOverview.trigInfo] {
        set clientSz [windowClientSizeGet trigOverview]

        set maxXExtent [expr [lindex $clientSz 0] - 14]
        set maxYExtent [expr [lindex $clientSz 1] - 14]
    }

    controlPositionSet trigOverview.trigInfo 7 7
    controlSizeSet     trigOverview.trigInfo $maxXExtent $maxYExtent
}

##############################################################################
#
# targetEndian - determine the endianness of the target.
#
# SYNOPSIS:
#   targetEndian
#
# PARAMETERS:
#   none
#
# RETURNS:
#   BIG_ENDIAN, LITTLE_ENDIAN or UNKNOWN_ENDIAN
#
# ERRORS:

proc targetEndian {} {
    set endian [lindex [lindex [wtxTsInfoGet] 2] 4]
    if {$endian == 1234} {
	return BIG_ENDIAN
    }
    if {$endian == 4321} {
	return LITTLE_ENDIAN
    }
    return UNKNOWN_ENDIAN
}




##############################################################################
#
proc trigBindToTarget {tname} {
    global trigTimerButtonActive

    # If there is a trigger attachment matching our spec, then use it
    # pattern is triggering ($tname)
    
    # Get the list of attachments
    set attachmentList [wtxHandle]
    
    set breakFlag 0
    set pattern "$tname\(-\[0-9\]+\)?"

    foreach handle $attachmentList {
        if [regexp $pattern $handle ] {
	    # Use that handle
	    wtxHandle $handle

	    # Does the toolname match?
	    # If it does, break, otherwise try some more
            # If the tgtsvr has restarted, wtxToolName causes an error on
            # that handle

            if { [catch {set toolName [wtxToolName]}] == 0 } {
                if { $toolName == "triggering" } {
		    set breakFlag 1
		    break
		}
            } else {
                wtxToolDetach $handle
            }

	    # Handle matches, tool doesn't
	}
    }

    # if breakFlag true then leave with the handle selected, otherwise try to create a new one
    if { !$breakFlag } {

        if [catch { wtxToolAttach $tname triggering }] {
	    set trigTimerButtonActive 0
	    autoTimerCallbackSet trigOverview 0
            messageBox "Cannot attach to target server $tname"
            return 0
        } else {
		if [catch {wtxRegisterForEvent .*}] {
		set trigTimerButtonActive 0
		autoTimerCallbackSet trigOverview 0
		uiEventProcess
                messageBox "cannot communicate with target server $tname"
                return 0
            }
	}

        uplevel #0 source [wtxPath host resource tcl]wtxcore.tcl
   	uplevel #0 source [wtxPath host resource tcl]shelcore.tcl

	shellCoreInit

    }

    if { [catch {wtxTargetAttach} strng] != 0 } {
        # Got an error
        messageBox "cannot communicate with target server $tname"
	trigUnbindFromTarget $tname
	return 0
    } else {
	return 1
    }

}

proc trigUnbindFromTarget {tname} {
    # Get the list of attachments
    set attachmentList [wtxHandle]
    
    set breakFlag 0
    set pattern "$tname\(-\[0-9\]+\)?"

    foreach handle $attachmentList {
        if [regexp $pattern $handle ] {
	    # Use that handle
	    wtxHandle $handle

	    # Does the toolname match?
	    # If it does, break, otherwise try some more
            # If the tgtsvr has restarted, wtxToolName causes an error on
            # that handle

	    if { [catch {set toolName [wtxToolName]}] == 0 } {
                if {$toolName == "triggering" } {
		    set breakFlag 1
		    break
		}
            } else {
                wtxToolDetach $handle
	    }

	    # Handle matches, tool doesn't
	}
    }

    # if breakFlag false then leave now
    if { !$breakFlag } {
        return
    }

    catch {wtxToolDetach}

}




##############################################################################
#
# 01Trigger.win32.tcl - Initialization
#
# In-line initialization of Trigger elements.  This code is executed when
# the Trigger GUI is added to the Tornado tool set at start time.
#

trigInitUI
