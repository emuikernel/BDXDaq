# 01WindView.win32.tcl - WindView Tcl initialization file
#
# Copyright 1995-2002 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 05a,28jun02,cpd  Fix setting of button states
# 05a,07jan02,tcr  Change Copyright date
# 04z,16nov01,tcr  add license management
# 04y,16oct01,cpd  Fix Solaris registry problem
# 04x,19sep01,cpd  Initial Veloce mods.
#                  Fix SPR#21800: Disable ALL buttons properly on init.
#                  Fix SPR#24751: Change Windview Config button into a toggle
#                  Fix SPR#26820: Add OnWvStopButt when event buffer is full.
#		   Re-visit error handling/action order for buttons.
#                  Fix SPR#27311, #23571: Fix behaviour of 'Track incoming 
#                  events' button
#                  Move helpId's into .e files (allows easy integration of 
#                  network events.
# 04w,08oct01,fle  unified Tornado version String
# 04v,27feb01,hk   update copyright notice to cover 2001
# 04u,23apr99,nps  maintain single wtxToolAttach connection.
# 04t,16apr99,nps  spr#26762 - fix size of control window to avoid scroll bars.
# 04s,15apr99,nps  spr#26714 - added 'About Box' support for Unix platforms.
# 04r,25mar99,tcr  Fix initial size of eventlog overview window
# 04q,15mar99,tcr  Make the EventLog Periodic Refresh button pop out to
#                  reflect loss of connection to the target
# 04p,10mar99,tcr  Fix SPR 25372. Implement error handler for wtx... functions
#                  in 01WvConfig.win32.tcl and ensure that callers here
#		   have catch { } where necessary
# 04o,22feb99,nps  remove 'tooltips' menu item.
# 04n,22feb99,nps  remove <ctrl>-Ms, again.
# 04m,10feb99,jak  change help menu insertion
# 04l,05feb99,cjtc fixing missed conversion to UITcl II in which changing the
#		   zoomFactor would be missed. SPR 23468
# 04k,08dec98,nps  allow alternate tgtsvr selection between invokations.
# 04j,02dec98,nps  disable upload info table event.
# 04i,01dec98,nps  ensure wvGlobal(targetName) is declared before used.
# 04h,30nov98,nps  Use target server selected from launcher if supplied.
#                  Up rev number to 2.0.1
#                  When using dialog to select target, only list target-
#                  servers (not the registry), and sort them.
#                  fixed 'Merge Log' menu item on Unix.
# 04g,27nov98,nps  fixed 'Analyze' menu item.
# 04f,23nov98,nps  remove double quotes from stand-alone help menu items.
# 04e,19nov98,pr   added -toolbar to mainWindowCreate (jrennie)
# 04d,16nov98,nps  further WV2.0 -> T2 merge.
# 04c,16nov98,nps  don't enable hideNonExecEvents button if log collected in
#                  'thin mode'.
# 04b,11nov98,nps  further WV2.0 -> T2 merge.
#                  fixed hideNonExecEvents & trackIncomingEvents buttons.
# 04a,22oct98,jp   Use Tornado 2.0 Registry
# 03z,09oct98,ren  tor2_0_x --> tor2_0_0 merge
# 03y,09oct98,ren  fixed my screw up on PC side when I made change 03u
# 03x,09oct98,ren  Fixed problem on PC.  Needed proc that was only being declared
#                  in standalone mode
# 03w,08oct98,ren  Mostly merged with T2 on solaris.
# 03v,03oct98,cjtc put a wtxToolAttach/wtxToolDetach pair in 
#		   loggingStoppedCheck(), since when called from triggering, an
#		   attachment may not be in place (SPR 22588)
# 03u,02oct98,nps  remove <ctrl>-Ms
# 03t,30sep98,nps  support enable/disable and toggling of WV menu items and buttons.
# 03s,29sep98,nps  Fixed typo of 'Legend' which had broken legend help.
# 03r,25sep98,c_s  restore load of uitclcontrols on unix
# 03q,28aug98,nps  UITcl II update.
# 03p,11sep98,nps  Added help support for new memLib related events.
# 03o,20aug98,dbs  Removed -allowpaintonly args to other calls to uiEventProcess
#                  so socket-read can operate (it is driven by Win32 msg-pump 
#                  and cannot operate when Tcl code blocks).
# 03n,19aug98,cjtc event buffer full handled more gracefully (SPR 22133)
# 03m,13aug98,ren  Temporary fix to SPR 21803.  Drop down zoom box no longer
#                  editable.
# 03l,21jul98,cjtc avoid blank WindView Control window if target selection
#                  is cancelled (SPR 21440)
# 03k,16jul98,cjtc better messages when upload fails (SPR 21461)
# 03k,13jul98,nps  fixed SPR 21753 - WV not listed by launcher.
# 03j,22jun98,dbs  fix windviewHelpShow to get correct event-name
#                  (part of SPR 21472)
# 03i,09jun98,nps  fixed some SPRs.
# 03h,15may98,nps  allow resizing of control window.
# 03g,14may98,nps  corrected typo referencing WV icon.
# 03f,01may98,wmd  Use WindView.icon instead of WindView.bmp in 
#                  wrapper_WindviewLaunch proc.
# 03e,28apr98,nps  Use appropriate icon in messageBoxes.
# 03d,23apr98,wmd  (& sks) add statusbar to UNIX main window and fix the
#                    displaying of the statusbar for both plats.
# 03c,15apr98,sks  fix menu flickering and toolbar slow creation problem
#                    on UNIX standlone startup.
# 03b,20apr98,c_s  more mainwin fixups
# 03a,15apr98,dbs  move analysis-packs into C++
# 03a,15apr98,c_s  mainwin fixups
# 02z,13apr98,nps  increase frequency of look on timer.
# 02y,30mar98,dbs  Fix wvFindUserDefinedIcons by moving it to
#                    database.tcl so it is available to per-doc
#                    interpreters.
# 02x,16mar98,nps  continued PM development.
# 02x,25mar98,c_s  mainwin changes
# 02w,13mar98,dbs  fix analysis button
# 02v,13mar98,dbs  remove stop-collection, lock-event and lock-ctx 
#                  buttons from toolbar
# 02u,10mar98,dbs  remove interval combo, add 100% zoom button
# 02t,06mar98,nps  added upload faux-progress meter.
#                  don't say upload complete if it fails!
# 02s,03mar98,nps  added periodic look button.
# 02r,02mar98,nps  incorporate various fixes added in Alameda.
#                  overview window now uses table control.
#                  don't allow multiple windows to be created.
# 02q,16feb98,nps  minor bug fixes.
# 02p,16feb98,dbs  fixup change 02l
# 02o,16feb98,nps  Rejigged Tools menu.
# 02n,13feb98,nps  Uploading during a run is now a shortcut to stop->upload.

# 02m,13feb98,nps  Don't enable upload button if not in deferred mode.
#                  Don't use new interpreter for WV window.
# 02l,04feb98,nps  Fixed initialisation problem found by Lisa Stanley.
# 02k,03feb98,nps  reread config from registry before uploading
# 02j,02feb98,nps  now supports a configuration overview window.
# 02i,26jan98,nps  integration with WV2.0 upload paths.
# 02h,17dec97,c_s  initial WV2.0 work.

#*/

# globals

package require Wind

set wvGlobal(tRegRoot)		[Wind::key]
set wvGlobal(regRoot)		$wvGlobal(tRegRoot)\\WindView
set wvGlobal(WvVersion)		"2.2"
set wvGlobal(aPackList)		""
set basePath ""

if [catch {set basePath [wtxPath]}] {
    set basePath "$env(WIND_BASE)"
    regsub -all {\\} $basePath / basePath
    if {[string index $basePath [expr [string length $basePath] - 1]] != "/"} {
	append basePath /
    }
}

set wvGlobal(wvToolBarState)	1
set wvGlobal(bitmaps)		${basePath}host/resource/bitmaps/WindView
set wvGlobal(helpFilePath)	${basePath}host/resource/help/wv4win.hlp
set wvGlobal(tclRoot)		${basePath}host/resource/tcl/app-config/WindView
set wvGlobal(aPackPath)		$wvGlobal(tclRoot)/analyze
set wvGlobal(logLevel)          1
set wvGlobal(launchTbar)        launch
set wvGlobal(thinMode)          1
set wvGlobal(WVMainTB)		0
set wvGlobal(hideNonExecEvents) 0
set wvGlobal(trackEvents)       0
set wvGlobal(wvTrackButtEnable) 0
set wvGlobal(timerButtonActive) 0

set wvOldMainWindowInitCB       ""

# First check if the dll loaded successfully, otherwise
# return immediately because there is nothing to be done
# from this file

set dllName1 "WindView.dll"
set dllName2 "UITclControls.dll"

if {[uitclinfo debugmode] == 1} {
	set dllName1 "WindView-d.dll"
	set dllName2 "UITclControls-d.dll"
}

global tcl_platform
if {$tcl_platform(platform) == "unix"} {
    global wvGlobal
    set dllName1 libwindview
    set dllName2 libuitclcontrols
    set wvGlobal(standalone) 1
    set wvGlobal(launchTbar) windViewTB
	applicationTitleSet "Windview $wvGlobal(WvVersion)"
} else {
    set wvGlobal(standalone) 0
}

if [catch {dllLoad $dllName1 _Windview_Init} wvdll] {
	puts "$wvdll"
	return
}

# Do this only on Unix....

if {$tcl_platform(platform) == "unix" && [catch {dllLoad $dllName2 _UITclControls_Init} uitcdll]} {
    puts "$uitcdll"
    return
}

lappend launcherGlobals(targetChangeNotifiers) wvToolButtonUpdate

proc onCmdUIWindView {} {
    global checkflag
    global enableflag
    global wvGlobal

    if { $wvGlobal(standalone) } {
        set target [selectedTargetValue]
    } else {
        set target [selectedTargetGet]
    }

    if {$target == ""} {
        set enableflag $wvGlobal(standalone)
    } else {
        set enableflag 1
    }

    set checkflag 1

    if [catch { windowVisible eventLogging } ] {
        set checkflag 0
    }
}

proc wvInitUI {} {
    global wvGlobal

    set wvGlobal(timerButtonActive) 0

    wvInitToolBar

    controlCreate $wvGlobal(launchTbar) [list toolbarbutton \
        -toggle \
        -cmduicallback onCmdUIWindView \
        -callback "toolSelectCallback LaunchWindView" \
        -name windview \
        -tooltip "Launch WindView Config" \
        -bitmap "$wvGlobal(bitmaps)/WindView.bmp"]

    if $wvGlobal(standalone) { 
        menuItemInsert -bypath -popup -name wvMenuTools -after &View &Tools
        menuItemAppend wvMenuTools -callback onWvLogMerge -cmduicallback onCmdUIToggle2 "&Merge Log..."
        menuItemAppend wvMenuTools -popup -name wvMenuOptions &Options
        menuItemAppend wvMenuOptions \
            -callback [list wvDispAttrChange bgBlack] "&Black Background"
        menuItemAppend wvMenuOptions \
            -callback [list wvDispAttrChange bgWhite] "&White Background"
        menuItemAppend wvMenuOptions \
            -callback [list wvDispAttrChange mono] "&Monochrome"
    } else {
        menuItemInsert -bypath -popup -name wvMenu \
            -after {"&Tools" "&Debugger..."} "&WindView"

        menuItemAppend wvMenu -callback {targetServerSelect WindView} "&Launch..."

        if $wvGlobal(standalone) {
            menuItemAppend &Tools &Configuration... {wvConfigLaunch}
        } else {
            menuItemInsert -bypath -name wvConfigMenu \
                -callback wvConfigLaunch \
                -after {&Tools &WindView &Launch...} "&Configuration..."
        }

        menuItemAppend wvMenu -callback onWvLogMerge -cmduicallback onCmdUIToggle2 "&Merge Log..."

        menuItemInsert -bypath -popup -name wvMenuOptions \
            -after {"&Tools" "&WindView" "&Merge Log..."} "&Options"

        menuItemAppend wvMenuOptions \
            -statusmessage "Set WindView display background to black" \
            -callback {wvDispAttrChange bgBlack} \
            "&Black Background"

        menuItemAppend wvMenuOptions \
            -statusmessage "Set WindView display background to white" \
            -callback {wvDispAttrChange bgWhite} \
            "&White Background"

        menuItemAppend wvMenuOptions \
            -statusmessage "Set WindView display to monochrome" \
            -callback {wvDispAttrChange mono} \
            "&Monochrome"
    }
	
    menuItemAppend -bypath \
        -callback onViewWindViewTB \
        -cmduicallback onCmdUIWVToolBar \
        {"&View" "&Toolbar"} \
        "&WindView Toolbar"

    if {!$wvGlobal(standalone)} {
        global wvOldMainWindowInitCB
        set wvOldMainWindowInitCB [windowInitCallbackGet mainwindow]
        windowInitCallbackSet mainwindow wvMainWindowInit	
    } {
        wvMainWindowInit
    }
    wvInitHelp
    wvInitButtonsAndMenus
    windowShowCallbackSet windViewTB wvUpdateTbState
    wvToolButtonUpdate

    windowShow windViewTB 1 
}


proc LaunchWindView {args} {
    global wvGlobal

    if [windowExists eventLogging] {
        windowClose eventLogging
	return
    }

    if { !$wvGlobal(standalone) } {
        set wvGlobal(targetName) ""
    }

    wrapper_windViewLaunch $args
}

proc onCmdUIWVToolBar {} {
    global checkflag

    set checkflag [windowVisible windViewTB]
}

proc onViewWindViewTB {} {
    global wvGlobal

    if {$wvGlobal(wvToolBarState) == "1"} {
        windowShow windViewTB 0
    } else {
        windowShow windViewTB 1
    }

    set wvGlobal(wvToolBarState) [windowVisible windViewTB]

}

proc windViewLaunch {} {
global wvGlobal

    set tgtsvr_selected [controlSelectionGet launch.tgtCombo -string]

    if {$tgtsvr_selected != "" &&
    	[checkTarget $tgtsvr_selected] == "0"} {
    	wrapper_windViewLaunch $tgtsvr_selected
    }
}


proc wrapper_windViewLaunch {args} {
    global wvGlobal

    if [windowExists eventLogging] {
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

   windowCreate -name eventLogging \
                -nonewinterpreter \
                -init "wvInit $wvGlobal(targetName)" \
    		-title "WindView Control ($wvGlobal(targetName))" \
    		-w 209 -h 145 \
		-icon "$wvGlobal(bitmaps)/WindView.ico"

    windowSizeCallbackSet eventLogging "adjustEvtOverview"
    windowExitCallbackSet eventLogging wvLoggingExit

    adjustEvtOverview
}

proc wvLoggingExit {} { 
    global wvGlobal
    windViewUnbindFromTarget $wvGlobal(targetName)    
}


proc adjustEvtOverview {} {

    if [controlExists eventLogging.uploadInfo] {
        set clientSz [windowClientSizeGet eventLogging]

        set maxXExtent [expr [lindex $clientSz 0] - 14]
        set maxYExtent [expr [lindex $clientSz 1] - 14]
    }

    controlPositionSet eventLogging.uploadInfo 7 7
    controlSizeSet     eventLogging.uploadInfo $maxXExtent $maxYExtent
}

proc wvInit {target} {
    global wvGlobal

    uplevel #0 source [wtxPath host resource tcl]wtxcore.tcl
    uplevel #0 source [wtxPath host resource tcl]shelcore.tcl
    uplevel #0 source [wtxPath host resource tcl app-config Tornado]01WvConfig.win32.tcl

    set wvGlobal(targetName) $target

    toolbarCreate eventLogging -name wvTB -top

    windowShow wvTB 0

    controlCreate wvTB "toolbarbutton -name wvConfigButt \
                -callback {wvConfigDialog $target} \
                -tooltip \"Configure WindView\" \
                -bitmap \"$wvGlobal(bitmaps)/wvConfig.bmp\" "

    controlCreate wvTB "toolbarbutton -name wvUploadButt \
                -separator \
                -callback onWvUploadButt \
                -tooltip \"Upload\" \
                -bitmap \"$wvGlobal(bitmaps)/Upload.bmp\" "

    controlCreate wvTB "toolbarbutton -name wvLookButt \
                -callback onViewButt \
                -tooltip \"Look for Event Log\" \
                -bitmap \"$wvGlobal(bitmaps)/look.bmp\" "

    controlCreate wvTB "toolbarbutton -name wvLookTimerButt \
		-toggle \
                -callback onViewTimerButt \
		-cmduicallback onUICmdViewTimerButt \
                -tooltip \"Look Periodically for Event Log\" \
                -bitmap \"$wvGlobal(bitmaps)/lookTimer.bmp\" "

    controlCreate wvTB "toolbarbutton -name wvStartButt \
                -separator \
                -callback onWvStartButt \
                -tooltip \"Start WV\" \
                -bitmap \"$wvGlobal(bitmaps)/wvStart.bmp\" "

    controlCreate wvTB "toolbarbutton -name wvStopButt \
                -callback onWvStopButt \
                -tooltip \"Stop WV\" \
                -bitmap \"$wvGlobal(bitmaps)/wvStop.bmp\" "

    controlCreate eventLogging "table -name uploadInfo \
                 -columns 2 -columnheader \
		 -x 7 -y 7 -w 194 -h 120"

    controlPropertySet eventLogging.uploadInfo -columnheaders {Attribute Value}
    controlPropertySet eventLogging.uploadInfo -columnwidths {60 130}

    controlEnable wvTB.wvStopButt   0
    controlEnable wvTB.wvStartButt  0
    controlEnable wvTB.wvConfigButt 1
    controlEnable wvTB.wvUploadButt 0
    controlEnable wvTB.wvLookTimerButt 0
    controlEnable wvTB.wvLookButt 0

    windowShow wvTB 1

    adjustEvtOverview

    onViewButt
}

proc wvTargetLongWrite { addr data } {
    set endianFlag "-[string range [targetEndian] 0 0]"
    set partLocal [memBlockCreate $endianFlag]
    memBlockSet -l $partLocal 0 $data
    wtxMemWrite $partLocal $addr
    memBlockDelete $partLocal
}

proc loggingStoppedCheck {} {
    global wvGlobal

    #
    # if, when this procedure is called, the event buffer full notify flag
    # is up on the target, put out a message to that effect and then
    # lower the flag on the target. First check to see that the symbol
    # exists on the target (!= 0)
    #

    if {$wvGlobal(wvEvtBufferFullNotify) != 0} {

	# the symbol exists on the target

        if {[windViewBindToTarget $wvGlobal(targetName)] == 0} {
            return
        }

	set buffFull [wtxGopherEval "$wvGlobal(wvEvtBufferFullNotify) @l"]

	if {$buffFull != 0} {

	    #
	    # the event buffer is full. Register that we've seen it and
	    # put out a message box
	    #

	    wvTargetLongWrite $wvGlobal(wvEvtBufferFullNotify) 0
	    messageBox "Event Buffer is Full\nLog collection has been stopped"

	    onWvStopButt
	}

        #wtxToolDetach

    }
}

proc onWvStartButt {} {
    global wvGlobal

    if {$wvGlobal(config.ok) == 0} {
        wvConfigDialog $wvGlobal(targetName)

        if {$wvGlobal(config.ok) == 0} {
            messageBox -stopicon "Unable to continue without WV configuration"
            return
        }
    }

    uplevel #0 source [wtxPath host resource tcl]wtxcore.tcl
    uplevel #0 source [wtxPath host resource tcl]shelcore.tcl

    set boundToTarget [windViewBindToTarget $wvGlobal(targetName)]

    if {$boundToTarget == 0} {
	## call function to set our button states
	setButtonsForNoTarget

        return
    }

    if {[initStartWV $wvGlobal(targetName)] == 0} {
        uploadRefreshView
        messageBox -stopicon "Windview Initialization Failed"
        return
    }

    uploadRefreshView
    
    if { ! [catch {wtxDirectCall $wvGlobal(wvEvtLogStart)}] } {
        #wtxToolDetach
    }

    if {$wvGlobal(reg.cont) == 0} {
        controlEnable wvTB.wvUploadButt 1
    } else {
        controlEnable wvTB.wvUploadButt 0
    }

    setStartStopButtons

    controlEnable wvTB.wvConfigButt 0

    set wvGlobal(wvTrackButtEnable) 1
}


proc onWvStopButt {} {
    global wvGlobal

    beginWaitCursor

    set boundToTarget [windViewBindToTarget $wvGlobal(targetName)]

    if {$boundToTarget == 0} {
	## call function to set our button states
	setButtonsForNoTarget
	endWaitCursor

        return
    }

    catch [loggingStoppedCheck]

    ## CPD: fix SPR#26820:  Do uploadRefreshView first.
    ## This allows us to check the status of the buffer and
    ## determine whether there is something valid to stop against
    ## (avoids target exceptions when stop is pressed after a reboot)

    uploadRefreshView

    if {$wvGlobal(rBuffID) != 0} {
	catch [wtxDirectCall $wvGlobal(wvEvtLogStop)]

	if {$wvGlobal(reg.cont) == 1} {
	    set uploadResult [wvCloseUpload]
	    if {$uploadResult != 0 } {
		messageBox "Upload terminated unexpectedly"
	    }
	}
    }

    setButtonsAfterStop

    endWaitCursor
}

##############################################################################
#
# setButtonsAfterStop - Reset control buttons after a 'stop' action
# 
# This function resets the buttons on the control dialog back to the default
# states (As if the stop button has been clicked) 
#
# RETURNS: n/a
# 

proc setButtonsAfterStop {} {
    global wvGlobal

    setStartStopButtons
    controlEnable wvTB.wvConfigButt 1

    set wvGlobal(wvTrackButtEnable) 0

    if {$wvGlobal(reg.cont) == 0} {
        controlEnable wvTB.wvUploadButt 1
    } else {
        controlEnable wvTB.wvUploadButt 0
    }

}

##############################################################################
#
# setStartStopButtons - Reset start and stop buttons to target state
# 
#
# RETURNS: n/a
# 

proc setStartStopButtons {} {
    global wvGlobal
    
    set logging [isWindViewLogging]

    if {$logging == 1} {
	controlEnable wvTB.wvStartButt  0
	controlEnable wvTB.wvStopButt  1
    } else { 
	controlEnable wvTB.wvStartButt  1
	controlEnable wvTB.wvStopButt  0
    }
}

proc onWvUploadButt {} {
    global wvGlobal

    beginWaitCursor

    set boundToTarget [windViewBindToTarget $wvGlobal(targetName)]

    if {$boundToTarget == 0} {
	## call function to set our button states
	setButtonsForNoTarget
	endWaitCursor

        return
    }

    # Check target to see if there's buffer holding a log
    # collected in deferred mode

    if {$wvGlobal(config.ok) == 0} {
        wvConfigDialog $wvGlobal(targetName)

        uploadRefreshView

        if {$wvGlobal(config.ok) == 0} {
	    endWaitCursor
            messageBox -stopicon "Unable to continue without WV configuration"
            return
        }

        if {[wvConfigGetSyms] == 0} {
	    endWaitCursor
            return
        }
    }

    if {$wvGlobal(reg.cont) == 1} {
        endWaitCursor
        messageBox -stopicon "Error - deferred upload not available in continuous mode" 
        return
    }

    if [catch {set wvGlobal(rBuffID) [wtxGopherEval "$wvGlobal(evtBufferId) @l"]} result] {
	controlEnable wvTB.wvUploadButt 0
	set wvGlobal(rBuffID) 0
    }

    if {$wvGlobal(rBuffID) == 0} {
	controlEnable wvTB.wvUploadButt 0
	uploadRefreshView
        endWaitCursor
        messageBox -stopicon "WV Buffer not available on target"
        return
    }

    set uploadResult 0

    if {[wvOpenUpload] == 1} {

        set wvGlobal(up.delay) 1

        statusMessageSet -progress 20 1 "Upload progress"
	uiEventProcess

        autoTimerCallbackSet eventLogging $wvGlobal(up.delay) uploadProgress

        set uploadResult [wvCloseUpload]
    } else {
        onViewTimerButt
        endWaitCursor
        return
    }

    autoTimerCallbackSet eventLogging 0

    while {$wvGlobal(up.delay) <= 20 && $uploadResult == 0} {
        statusMessageSet -progress 20 $wvGlobal(up.delay) "Upload progress"
	uiEventProcess
        incr wvGlobal(up.delay)
        msleep 50
    }

    statusMessageSet -normal ""

    onViewTimerButt

    #catch [wtxToolDetach]

    controlEnable wvTB.wvConfigButt 1
    controlEnable wvTB.wvUploadButt 0
    setStartStopButtons

    endWaitCursor

    if {$uploadResult == 0 } {
    	statusMessageSet -normal  "Upload complete"
    } else {
        messageBox "Upload terminated unexpectedly"
    }

}

proc uploadProgress {} {
    global wvGlobal

    incr wvGlobal(up.delay)
    statusMessageSet -progress 20 $wvGlobal(up.delay) "Upload progress"
    uiEventProcess -allowpaintonly

    if {$wvGlobal(up.delay) < 20} {
        autoTimerCallbackSet eventLogging $wvGlobal(up.delay) uploadProgress
    } else {
        autoTimerCallbackSet eventLogging 0
	set wvGlobal(timerButtonActive) 0
    }
}

proc onViewButt {} {
    global wvGlobal

    uploadRefreshView

    if {$wvGlobal(rBuffID) != 0 && $wvGlobal(reg.cont) == 0 && $wvGlobal(targetOK) == 1} {
        controlEnable wvTB.wvUploadButt 1
    } else {
        controlEnable wvTB.wvUploadButt 0
    }
}

proc onUICmdViewTimerButt {} {
    global checkflag
    global wvGlobal

    set checkflag $wvGlobal(timerButtonActive)
}

proc onViewTimerButt {} {
    global wvGlobal

    set autoRefresh [controlChecked wvTB.wvLookTimerButt]
    set wvGlobal(timerButtonActive) $autoRefresh
    if {$autoRefresh == 1} {
        autoTimerCallbackSet eventLogging 5 uploadRefreshView
    } else {
        autoTimerCallbackSet eventLogging 0
    }

    uploadRefreshView
}

proc evtLogDlgInit {args} {
    global wvGlobal

    controlValuesSet eventLogging.tgtNm $wvGlobal(targetName)
    controlValuesSet eventLogging.ctxSwitch 1
}

proc onCmdUIWVMain {} {
    global enableflag
    global wvGlobal

#puts $wvGlobal(WVMainTB)

    if {$wvGlobal(WVMainTB) == "1"} {
        set enableflag 1
    } else {
        set enableflag 0
    }
}

proc onCmdUIHideNonExec {} {
    global wvGlobal
    global checkflag
    global enableflag

    if {$wvGlobal(WVMainTB) == "1" && $wvGlobal(thinMode) != "1"} {
        set enableflag 1
    } else {
        set enableflag 0
    }

    if {$wvGlobal(hideNonExecEvents) == "1"} {
        set checkflag 1
    } else {
        set checkflag 0
    }
}

proc onCmdUITrackEvents {} {
    global wvGlobal
    global checkflag
    global enableflag

    if {$wvGlobal(WVMainTB) == "1" && $wvGlobal(wvTrackButtEnable) == 1} {
        set enableflag 1
    } else {
        set enableflag 0
    }

    if {$wvGlobal(trackEvents) == "1"} {
        set checkflag 1
    } else {
        set checkflag 0
    }
}

proc wvInitToolBar {} {
    global wvGlobal

    toolbarCreate mainwindow -name windViewTB \
		-title "WindView" -any \
		-helpfile $wvGlobal(helpFilePath)

    windowShow windViewTB 0

    controlCreate windViewTB "toolbarbutton \
		-name zoomInButton \
		-callback zoomIn \
		-cmduicallback onCmdUIWVMain \
		-tooltip \"Zoom in\" \
		-bitmap $wvGlobal(bitmaps)/ZoomIn.bmp"

    controlCreate windViewTB "toolbarbutton \
		-name zoomOutButton \
		-callback zoomOut \
		-cmduicallback onCmdUIWVMain \
		-tooltip \"Zoom out\" \
		-bitmap \"$wvGlobal(bitmaps)/ZoomOut.bmp\""
	
    controlCreate windViewTB "toolbarbutton \
		-name zoomMaxButton \
		-callback zoomMax \
		-cmduicallback onCmdUIWVMain \
		-tooltip \"Zoom 100\%\" \
		-bitmap \"$wvGlobal(bitmaps)/ZoomMax.bmp\""
	
    controlCreate windViewTB "combo \
		-fixed \
		-name zoomFactor \
		-callback onZoomFactor \
		-cmduicallback onCmdUIWVMain \
		-tooltip \"Zoom factor\" \
		-w 30 -h 50"

    controlCreate windViewTB "toolbarbutton \
                -separator \
		-name evntDispButton \
		-callback filterEvents \
		-cmduicallback onCmdUIWVMain \
		-tooltip \"Filter events\" \
		-bitmap \"$wvGlobal(bitmaps)/FilterEvents.bmp\""

    controlCreate windViewTB "toolbarbutton \
		-toggle \
		-name hideExecButton \
		-callback onHideNonExecEvents \
		-cmduicallback onCmdUIHideNonExec \
		-tooltip \"Hide nonexecuting states\" \
		-bitmap \"$wvGlobal(bitmaps)/HideNonExec.bmp\""

    controlCreate windViewTB "toolbarbutton \
                -separator \
		-toggle \
		-name trackEvents \
		-callback onTrackEvents \
		-cmduicallback onCmdUITrackEvents \
		-tooltip \"Track incoming events\"
		-bitmap \"$wvGlobal(bitmaps)/TrackEvent.bmp\""

    controlCreate windViewTB "toolbarbutton \
 		-separator \
    		-name analyze \
		-callback wvAnalyze \
		-cmduicallback onCmdUIWVMain \
		-tooltip Analyze \
		-bitmap $wvGlobal(bitmaps)/analyze.bmp"

    controlValuesSet windViewTB.zoomFactor {1.2 2 5 10 20}
    controlSelectionSet windViewTB.zoomFactor 1

    controlEnable windViewTB.zoomFactor 0
    controlEnable windViewTB.hideExecButton 0
    controlEnable windViewTB.zoomInButton 0
    controlEnable windViewTB.zoomOutButton 0
    controlEnable windViewTB.zoomMaxButton 0
    controlEnable windViewTB.hideExecButton 0
    controlEnable windViewTB.trackEvents 0
    controlEnable windViewTB.analyze 0
}

proc onZoomFactor {} {
    global wvGlobal

    set eventInfo [controlEventGet windViewTB.zoomFactor]
    set intvl ""

    # when the event is either "addstring" or "selchange"
    if {[string match {*addstring*} [lindex $eventInfo 0]] ||
        [string match {*selchange*} [lindex $eventInfo 0]]} {

        set ix [controlSelectionGet windViewTB.zoomFactor]
        set selString [controlSelectionGet windViewTB.zoomFactor -string]

        # first check that the data entered by the user is valid
        if {$selString != "" &&
            [regexp {^[0-9.]+$} $selString] &&
            [expr $selString] > 0} {
	    # set the currently selected string as the current zoom factor
            set wvGlobal(zoomFactor) $selString
        } {
	    # let's delete the invalid data and select the first item
            messageBox "Invalid value!  Please enter a positive number."
	    controlValuesSet windViewTB.zoomFactor -delete -string $selString
            controlSelectionSet windViewTB.zoomFactor 0
	}
    }
}
    
proc onWvLogMerge {} {
	set fname [fileDialogCreate -filemustexist \
		-filefilters "WindView Logs (*.wvr)|*.wvr||" \
		       -title "Merge WindView Log"]
	wvEventBaseMerge [lindex $fname 0]
}

proc onHideNonExecEvents {} {
    global wvGlobal

    if {$wvGlobal(thinMode) == 1} {
        set state 1
    } else {
        set state [expr [wvHideNonExecEvents] ? 0 : 1]
    }

    set wvGlobal(hideNonExecEvents) $state

    wvHideNonExecEvents $state
}

proc onTrackEvents {} {
    global wvGlobal

    set state [expr [wvTrackIncomingData] ? 0 : 1]

    set wvGlobal(trackEvents) $state

    wvTrackIncomingData $state
}

proc wvInitHelp {} {
    global helpIdMap

    #
    # Semi-automatically generated help topic ids for event help lookup 

    ## CPD: These are now defined in the .e files when the events are loaded

    ## There are some special cases however:
    set helpIdMap(unknown) 8736
    set helpIdMap(defaultUser) 8752
    set helpIdMap(Legend) 65536

##    set helpIdMap(Exception) 8208
##    set helpIdMap(IntEnt) 8224
##    set helpIdMap(intExit) 8240
##    set helpIdMap(msgQCreate) 8256
##    set helpIdMap(msgQReceive) 8272
##    set helpIdMap(msgQSend) 8288
##    set helpIdMap(msgQDelete) 8304
##    set helpIdMap(semBCreate) 8320
##    set helpIdMap(semCCreate) 8336
##    set helpIdMap(semMCreate) 8352
##    set helpIdMap(semGive) 8368
##    set helpIdMap(semGiveForce) 8384
##    set helpIdMap(semTake) 8400
##    set helpIdMap(semFlush) 8416
##    set helpIdMap(semDelete) 8432
##    set helpIdMap(signal) 8448
##    set helpIdMap(sigwrapper) 8464
##    set helpIdMap(sigsuspend) 8480
##    set helpIdMap(pause) 8496
##    set helpIdMap(kill) 8512
##    set helpIdMap(taskSpawn) 8528
##    set helpIdMap(taskSuspend) 8544
##    set helpIdMap(taskResume) 8560
##    set helpIdMap(taskPrioritySet) 8576
##    set helpIdMap(taskDelay) 8592
##    set helpIdMap(taskDelete) 8608
##    set helpIdMap(taskSafe) 8624
##    set helpIdMap(taskUnsafe) 8640
##    set helpIdMap(safePend) 8656
##    set helpIdMap(timeSlice) 8672
##    set helpIdMap(watchDog) 8688
##    set helpIdMap(undelay) 8704
##    set helpIdMap(timeout) 8720
##    set helpIdMap(wdCreate) 8768
##    set helpIdMap(wdStart) 8784
##    set helpIdMap(wdCancel) 8800
##    set helpIdMap(wdDelete) 8816
##    set helpIdMap(memPartAddToPool) 9008
##    set helpIdMap(memPartAlignedAlloc) 8944
##    set helpIdMap(memPartFree) 8960
##    set helpIdMap(memPartCreate) 8992
##    set helpIdMap(memPartRealloc) 8976
}

# This function shows either the help legend, or help for the current
# event. <helpId> .  The functon can deal with a help Id as a number or
# two special cases:  'Legend' for the legend and 'defaultUser' for user
# events 
proc windviewHelpShow {helpId} {
    global wvGlobal
    global helpIdMap

    if {$helpId == "Legend"} {
        set helpId $helpIdMap(Legend)
    }
    if {$helpId == "defaultUser"} {
        set helpId $helpIdMap(defaultUser)
    }

    helpLaunch context $wvGlobal(helpFilePath) $helpId event
}

proc onWindViewHelpTopics {} {
	global wvGlobal
	helpLaunch index $wvGlobal(helpFilePath) ""
}

proc onWindViewSearchForHelpOn {} {
	global wvGlobal
	helpLaunch search $wvGlobal(helpFilePath) ""
}

proc wvHelpFilePathGet {} {
	global wvGlobal
	return $wvGlobal(helpFilePath)
}

proc wvToolButtonUpdate {} {
    global wvGlobal
    if $wvGlobal(standalone) {
        return
    }
    set tgtsvrSel [selectedTargetGet]
    if {$tgtsvrSel == ""} {
    	controlEnable launch.windview 0
    } {
    	controlEnable launch.windview 1
    }
}

proc wvInitButtonsAndMenus {} {
    global wvGlobal

    menuItemAppend -bypath \
		-statusmessage "Zoom in" \
		-cmduicallback onCmdUIToggle2 \
		-callback zoomIn "&View" "Zoom &In"

    menuItemInsert -bypath -separator {"&View" "Zoom &In"}

    menuItemInsert -after -bypath \
		-statusmessage "Zoom out" \
		-cmduicallback onCmdUIToggle2 \
		-callback zoomOut {"&View" "Zoom &In"} "Zoom &Out"

    menuItemInsert -after -bypath \
		-cmduicallback onCmdUIToggle2 \
		-statusmessage "Zoom 100%" -callback zoomMax {"&View" "Zoom &Out"} "&Zoom 100\%"

    menuItemInsert -after -bypath \
		-cmduicallback onCmdUIToggle2 \
		-statusmessage "Analyze" -callback wvAnalyze {"&View" "&Zoom 100\%"} "&Analyze"

    menuItemInsert -after -bypath \
		-cmduicallback onCmdUIToggle2 \
		-statusmessage "Show Event properties" \
		-callback onShowEvent {"&View" "&Analyze"} "Show E&vent..."

    menuItemInsert -after -bypath \
		-cmduicallback onCmdUIToggle2 \
		-statusmessage "Filter events" \
		-callback filterEvents {"&View" "Show E&vent..."} "&Filter Events..."

    menuItemInsert -after -bypath \
		-statusmessage "Hide all states other than executing" \
		-cmduicallback "onCmdUIHideNonExec" \
		-callback onHideNonExecEvents \
		{"&View" "&Filter Events..."} "&Hide Nonexecuting States"

    menuItemInsert -after -bypath \
		-statusmessage "Track incoming events" \
		-cmduicallback "onCmdUITrackEvents" \
		-callback onTrackEvents \
		{"&View" "&Hide Nonexecuting States"} "Trac&k Incoming Events"

    menuItemEnable -bypath {&View "Zoom &In"} 0
    menuItemEnable -bypath {&View "Zoom &Out"} 0
    menuItemEnable -bypath {&View "&Analyze"} 0
    menuItemEnable -bypath {&View "Show E&vent..."} 0
    menuItemEnable -bypath {&View "&Filter Events..."} 0
    menuItemEnable -bypath {&View "&Hide Nonexecuting States"} 0
    menuItemEnable -bypath {&View "Trac&k Incoming Events"} 0

    #
    # Approach #1 to modifying Tornado help menu: leave help menu in
    # resource, add WindView stuff w/o modifying basic menu structure
    #
    if $wvGlobal(standalone) {
        menuItemAppend -bypath -statusmessage "View list of WindView help topics" \
                       -callback onWindViewHelpTopics \
                       &Help {&Help Topics}
        menuItemAppend -bypath -statusmessage "View WindView help index" \
                       -callback onWindViewSearchForHelpOn \
                       &Help {&Search for Help on...}
        menuItemAppend -bypath -statusmessage "View event and stipple legend" \
                       -callback {windviewHelpShow Legend} \
                       &Help &Legend

	# There might be a better way to do this...

	menuItemDelete -bypath {&Help &About...}
	menuItemAppend -bypath -callback wvAboutBox &Help "&About WindView..."

    } else {
        menuItemInsert -after -popup -name wvHelpMenu \
                       -statusmessage "View list of WindView help topics" \
                       -bypath {&Help "Manuals &Contents"} {&WindView Help}

        menuItemAppend wvHelpMenu \
                       -callback {onWindViewHelpTopics} \
                       {&Help Topics}

        menuItemAppend wvHelpMenu \
                       -statusmessage "View WindView help index" \
                       -callback onWindViewSearchForHelpOn \
                       {&Search for Help on...}

        menuItemAppend wvHelpMenu \
                       -statusmessage "View event and stipple legend" \
                       -callback {windviewHelpShow Legend} \
                       &Legend
    }
}

proc wvAboutBox {} {
    global wvGlobal
    messageBox -info "WindView Release $wvGlobal(WvVersion)\nTornado Release [Wind::version]\n\nCopyright Wind River Systems, Inc. 1995-2002\n\nPortions covered by U.S. Patent(s): 5,872,909."
}

proc onCmdUIToggle {item} {
    global wvGlobal
    global checkflag
    global enableflag

    if {$wvGlobal($item) == "1"} {
        set checkflag  0
    } else {
        set checkflag  1
    }
}

proc onCmdUIToggle2 {} {
    global wvGlobal
    global enableflag

    set enableflag $wvGlobal(WVMainTB)
}


#
# wvViewActive - callback from C++ code to give us the state of
# all the UI elements. The first arg <active> indicates whether 
# the view is active (1) or inactive (0). The 2nd arg <states> is
# a list of each button/menu-items actual state, as follows:-
#
# { <zoomIn> <zoomOut> <zoomMax> <nonEx> <track> <analysis> <thin mode>}
#

proc wvViewActive {args} {
    global wvGlobal

    set active			[lindex $args 0]
    set states			[lindex $args 1]

    set zIn			[lindex $states 0]
    set zOut			[lindex $states 1]
    set zMax			[lindex $states 2]
    set hideNonExecEvents	[lindex $states 3]
    set trackEvents	        [lindex $states 4]
    set analysis		[lindex $states 5]
    set thinMode		[lindex $states 6]

    set wvGlobal(WVMainTB)          $active
    set wvGlobal(hideNonExecEvents) $hideNonExecEvents
    set wvGlobal(trackEvents)       $trackEvents
    set wvGlobal(thinMode)          $thinMode

    if {$wvGlobal(thinMode) == 1} {
        controlEnable windViewTB.hideExecButton 0
    } else {
        controlEnable windViewTB.hideExecButton $active
    }

    if {$active == 1} {
	controlEnable windViewTB.zoomFactor 1
        controlEnable windViewTB.zoomInButton $zIn
        controlEnable windViewTB.zoomOutButton $zOut
        controlEnable windViewTB.zoomMaxButton $zMax
        controlEnable windViewTB.analyze $analysis
    } else {
	controlEnable windViewTB.zoomFactor 0
        controlEnable windViewTB.zoomInButton 0
        controlEnable windViewTB.zoomOutButton 0
        controlEnable windViewTB.zoomMaxButton 0
        controlEnable windViewTB.analyze 0
    }
}

proc wvMainWindowInit {} {
    global wvGlobal
    global wvOldMainWindowInitCB

    # first call the old procedure and let it finish what it needs to
    if {$wvOldMainWindowInitCB != ""} {
        $wvOldMainWindowInitCB
    }
    set wvGlobal(wvToolBarState) [windowVisible windViewTB]

}

proc wvUpdateTbState {args} {
    global wvGlobal

    set newState [lindex $args 0]
    set wvGlobal(wvToolBarState) $newState

}

proc wvDispAttrChange attr {
    global wvGlobal

    if 0 { 
	switch $attr {
	    bgBlack {
		menuItemCheck \
		    {"&Tools" "&Options" "&WindView..." "&Black Background"} 1
		menuItemCheck \
		    {"&Tools" "&Options" "&WindView..." "&White Background"} 0
		menuItemCheck \
		    {"&Tools" "&Options" "&WindView..." "&Monochrome"} 0
	    }
	    bgWhite {
		menuItemCheck \
		    {"&Tools" "&Options" "&WindView..." "&Black Background"} 0
		menuItemCheck \
		    {"&Tools" "&Options" "&WindView..." "&White Background"} 1
		menuItemCheck \
		    {"&Tools" "&Options" "&WindView..." "&Monochrome"} 0
	    }
	    mono {
		menuItemCheck \
		    {"&Tools" "&Options" "&WindView..." "&Black Background"} 0
		menuItemCheck \
		    {"&Tools" "&Options" "&WindView..." "&White Background"} 0
		menuItemCheck \
		    {"&Tools" "&Options" "&WindView..." "&Monochrome"} 1
	    }
	}
    }
    wvDisplaySet $attr
    sysRegistryValueWrite HKEY_CURRENT_USER $wvGlobal(regRoot) background $attr    
}

proc wvAnalysisPackAdd {shortName longName context procedure dest} {
    global wvGlobal

    if {[lsearch -exact $wvGlobal(aPackList) $shortName] == -1} {
        lappend wvGlobal(aPackList) $shortName
        set ix [expr [llength $wvGlobal(aPackList)] - 1]
        set wvGlobal(aPack,index,$ix) $shortName
    }

    set wvGlobal(aPack,$shortName,longName)	$longName
    set wvGlobal(aPack,$shortName,context)	$context
    set wvGlobal(aPack,$shortName,procedure)	$procedure
    set wvGlobal(aPack,$shortName,dest)         $dest
}

proc wvAnalysisInit {} {
    global wvGlobal
    set files [glob -nocomplain $wvGlobal(aPackPath)/*.tcl]
    foreach f $files {
        uplevel #0 source $f
    }
}


##############################################################################
#
# windViewBindToTarget:  moved from 01WvConfig.win32.tcl

proc windViewBindToTarget {tname} {

    global wvGlobal
    # If there is a windview attachment matching our spec, then use it
    # pattern is windview ($tname)
    
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
                if { $toolName == "windview" } {
                    set breakFlag 1
                    break
                }
            } else {
                wtxToolDetach $handle
            }

            # Handle matches, tool doesn't
        }
    }

    # if breakFlag true then continue, otherwise try to create a new handle
    if { !$breakFlag } {
        if [catch {wtxToolAttach $tname windview} strng] {
	    set wvGlobal(timerButtonActive) 0
	    autoTimerCallbackSet eventLogging 0
            messageBox "Cannot attach to target server $tname"
            return 0
        } else {
            if [catch {wtxRegisterForEvent .*}] {
		set wvGlobal(timerButtonActive) 0
		autoTimerCallbackSet eventLogging 0
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
        messageBox "Unable to attach to target server $tname"
        windViewUnbindFromTarget $tname
        return 0
    } else {
	return 1
    }
}

##############################################################################
#
# windViewUnbindFromTarget:  moved from 01WvConfig.win32.tcl

proc windViewUnbindFromTarget {tname} {
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
                if {$toolName == "windview" } {
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
# wvCPUNameGet - Retrieve the name of the CPU from the target.
#
# SYNOPSIS:
#   wvCPUNameGet
#
# RETURNS:
#   None
#
proc wvCPUNameGet {} {
    global wvGlobal

    if {$wvGlobal(targetName) != ""} {
	windViewBindToTarget $wvGlobal(targetName)
	set wvGlobal(cpuType) [wtxCpuInfoGet -n [lindex [lindex [wtxTsInfoGet] 2] 0]]
	windViewUnbindFromTarget $wvGlobal(targetName)
    }
}

##############################################################################
#
# 01WindView.win32.tcl - Initialization
#
# In-line initialization of WindView elements.  This code is executed when
# WindView is added to the Tornado tool set at start time.
#

if {[catch {sysRegistryValueRead HKEY_CURRENT_USER \
           $wvGlobal(tRegRoot)} result]} {
    # pre-populate registry

    catch {sysRegistryKeyCreate HKEY_CURRENT_USER "" Software}
    catch {sysRegistryKeyCreate HKEY_CURRENT_USER Software {Wind River Systems}}
    catch {sysRegistryKeyCreate HKEY_CURRENT_USER {Software\Wind River Systems} "Tornado [Wind::version]"}
    catch {sysRegistryKeyCreate HKEY_CURRENT_USER $wvGlobal(tRegRoot) {WindView}}
}

if $wvGlobal(standalone) {

    # Set selected target (if present) for later reference
    # (assumes target server name is last parameter supplied

    catch {set wvGlobal(targetName) [lindex $argv [expr $argc - 1]]}

    mainWindowCreate -title "WindView $wvGlobal(WvVersion)" \
		     -statusbar \
		     -menu \
		     -toolbar \
		     -icon "$wvGlobal(bitmaps)/WindView.ico"
    wvInitUI
}

wvAnalysisInit
if {!$wvGlobal(standalone)} {
    wvInitUI
}


if $wvGlobal(standalone) {
    global env
    set wp $env(WIND_BASE)/host/resource/tcl/app-config/Tornado
 
    catch {uplevel #0 source $wp/02Trigger.win32.tcl}
    catch {uplevel #0 source $wp/../../AppLicenseAuthorize.win32.tcl}

    proc toolSelectCallback {args} { 
        eval $args
    }

    proc selectedTargetValue {} {
        global wvGlobal
        set target {}
        catch { set target $wvGlobal(targetName) }
        set target
    }

    proc selectedTargetGet { } {   
        global wvGlobal

        set wvGlobal(selectedTarget) ""
        dialogCreate -name selTarget \
                     -title "Select Target" \
                     -init "selTargetInit" \
                     -w 130 -h 95 \
                     -helpfile $wvGlobal(helpFilePath) \
                     -controls {
			{ button -default -name okButt -disabled
                          -title "OK" -callback wvTargetSelect
                          -x 7 -y 74 -w 50 -h 14} 
                        { button -name cancelButt -title "Cancel" 
                          -callback wvTargetCancel
                          -x 73 -y 74 -w 50 -h 14 } 
                        { list -name targets 
			  -callback wvListSelected 
                          -x 7 -y 7 -w 116 -h 57 }
 		     }
 	return $wvGlobal(selectedTarget)
    }

    proc selTargetInit {} {
        foreach entry [wtxInfoQ .* tgtsvr] {
            lappend tlist [lindex $entry 0]
        }

        set tlist [lsort $tlist]

        controlValuesSet selTarget.targets $tlist
    }

    proc wvTargetSelect {} {
        global wvGlobal

        set wvGlobal(selectedTarget) [controlSelectionGet selTarget.targets -string]
	set wvGlobal(targetName) $wvGlobal(selectedTarget)

        windowClose selTarget
    }

    proc wvTargetCancel {} {
        global wvGlobal
        set wvGlobal(selectedTarget) ""
        windowClose selTarget
    }

    proc wvListSelected {} { 
        controlEnable selTarget.okButt 1
    }
}

#
# Establish a registry entry for the user's preference for the WindView
# background, if one does not exist.
#

if {[catch {sysRegistryValueRead HKEY_CURRENT_USER \
           $wvGlobal(regRoot) background} result]} {
    
    sysRegistryKeyCreate HKEY_CURRENT_USER $wvGlobal(tRegRoot) WindView
    sysRegistryValueWrite HKEY_CURRENT_USER $wvGlobal(regRoot) background bgBlack
    set result bgBlack
}

wvDispAttrChange $result
