# Browser.tcl - Browser Tcl implementation file
#
# Copyright 1994-1998 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 02q,19feb99,jmp  added startNetscapeFromUI variable to workaround (SPRs
#                  #24072, #24300).
# 02p,19nov98,jmp  no longer source windHelpLib.tcl on startup, source it on
#                  first windHelp invocation.
# 02o,04nov98,jmp  added Manuals index entry to the help menu.
# 02n,30sep98,jmp  modified help menus to use windHelpLib instead of windman.
# 02m,09jun98,dbt  reworked previous modification. Now, we can start the
#		   browser even if the OS is not initialized. Only some
#		   informations are not available.
# 02l,23feb98,dbt  The browser now aborts cleanly when the target system is
#		   not yet loaded.
# 02k,19dec97,jdi  changed windman call to use -m option (-k has bug).
# 02j,11oct96,pad  The browser now aborts cleanly when the target system in
#                  not yet initialized (SPR #7296)
# 02i,20jun96,elp  added wtxToolDetach{} call before restarting.
# 02h,29may96,elp  removed wtx attachment from browserAttach{} (SPR# 6276).
# 02g,06feb96,jco  added global scrBrPos and call to scrollBarSet (spr# 6082).
# 02f,01dec95,c_s  removed debug print statements.  Reenabled object traversal
#                    behavior for task entries (SPR #5563).
# 02e,09nov95,pad  Fixed a bug that forced the update routine to be evaluated
#                    twice when the broser's auto update started while spy was
#                    on (SPR #4612).
#		     Made cosmetic changes to follow the WRS Tcl coding
#		     conventions
# 02d,30oct95,jco  added About menu and sourced .browser.tcl from ~/.wind 
#		     directory,	moved the sourcing of .browser.tcl to the 
#		     end of the file.
# 02c,27oct95,pad  Modified browse{} to take care of communication loss with
#                    the license daemon.
# 02b,23oct95,jcf  updated memory chart labels.
# 02a,23oct95,c_s  removed topLevelErrorHandler, as this is now provided by 
#                    host.tcl.
# 01z,20sep95,pad  Adapted browserCreate{} to new <class>Struct{} interface.
# 01y,28jul95,c_s  added WTX error handler and top level error handler.  The 
#                    WTX error handler detects reboots and offers to reconnect.
#                    The top level error handler catches uncaught Tcl errors
#                    and puts up a dialog (SPR #4452).
# 01x,25jul95,pad  Fixed minor bug in browserCreate{}.
# 01w,20jun95,jcf  updated manual.
# 01v,19jun95,jco  fit to modified shwait.
# 01u,15jun95,jco  improve help support (use shwait).
# 01t,15jun95,c_s  Now notices when windows are destroyed, and quits browsing 
#                    them.  Now refuses to browse object at address 0.
#                    Attachment code is moved here.
# 01s,10jun95,p_m  added help support.
# 01r,31may95,pad  added alphabetical sort to glob command.
# 01q,23may95,pad  now spyStop really prevents the timer evaluation in update.
#                    Added necessary code to decouple the spy update and the
#                    general browser update. Moved spy related stuff into
#                    01Spy.tcl.
# 01p,23may95,c_s  fixed ! button bug.
# 01o,16may95,f_v  fixed spyStop bug
# 01n,12may95,c_s  fixed Tcl calls for new wtxtcl version.
# 01m,11may95,f_v  Removed spytask, call now spyTaskSetup in spyStart
#		   fix a bug in spy Call,move few line from spyTaskSetup to
#		   spyStart, now spyDialog close button destroy spyChart
# 01l,07may95,c_s  Fixed up spy integration.  Most spy stuff now lives in a
#                    the browser extension area (app-config/Browser). 
#                    browserAuxProcAdd is now used to add something to the 
#                    browse update chain.
# 01k,26apr95,f_v  create GUI for spy,spy body code and add delta toolbar icon
# 01j,25apr95,c_s  now uses wtxPath.
# 01i,10apr95,c_s  don't try to browse an empty string; read in app configs.
# 01h,03apr95,c_s  Browser now listens to events.  Calls exit rather than 
#                    quit.
# 01g,27mar95,c_s  added memory charts.
# 01f,22mar95,c_s  Fixed bug where multiple spy starts would cause cascading
#                    updates.  Changed titleSet to wsTitleSet.
# 01e,15mar95,c_s  removed debug material.
# 01d,05mar95,jcf  upgraded spy dialog.
# 01c,24feb95,c_s  added task status.
# 01b,08feb95,p_m  take care of loadFlags in wtxObjModuleInfoGet return string.
# 01a,06dec94,c_s  written.
#*/

# DESCRIPTION
# This module holds the Tcl code required by the browser application. It
# creates the main window of the browser and initializes the various objects
# in this window such the task hierarchy, the memory charts and the object
# module window.
# 
# RESOURCE FILES:
# wpwr/host/resource/tcl/shelbrws.tcl
# wpwr/host/resource/tcl/app-config/Browser/*.tcl
# wpwr/host/resource/tcl/app-config/all/host.tcl
# <user's home directory path>/.wind/browser.tcl
#

# globals

 # Strings displayed in status bar

set symSortNumStr       "Numerical"	;# numerical symbol sort method
set symSortAlphaStr     "Alphabetical"	;# alphabetical symbol sort method
set spyCumul            "Cumulative"	;# spy cumulative mode
set spyDiffer           "Differential"	;# spy differential mode
set autoUpdateStr	"Update /"	;# auto update mode
set noAutoUpdateStr	""		;# manual update mode (no string)

 # Default mode strings

set symSortMode $symSortNumStr		;# symbols sorted numerically by default
set spyMode $spyCumul			;# spy in cumulative mode by default
set updateMode $noAutoUpdateStr		;# manual update mode by default

 # update mode

set browserUpdate	0		;# no auto update by default

 # Default interval (msec) for automatic update

set updateIntv		10000		;# default auto update interval is 10 s

 # Memory values

set maxPoolMem		0		;# Maximum size of agent memory pool
set maxVxMem		0		;# aximum size of VxWorks memory pool

 # initial module object scroll bar position

set scrBrPos            0

# On HPUX, netscape cannot be started from the Launcher, Browser, Crosswind
# (See SPR# 24072, 24300). We defined startNetscapeFromUI to FALSE, so
# when the HTML help is invoked on HPUX, windHelpDisplay{} check if the
# variable startNetscapeFromUI is defined to FALSE, if yes, we display
# an error message.

set startNetscapeFromUI FALSE


##############################################################################
#
# createForms - set up menus and buttons
#
# This procedure sets up the various menus and buttons of the browser.
# It also initializes the task hierarchy and the memory charts.
#
# SYNOPSIS
# createForms
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc createForms {} {

    # Creates menus and items inside.

    menuCreate		File	F 
    menuCreate -help	Help	H
    menuCreate		About	A

    menuButtonCreate	File	Quit			Q	exit

    menuButtonCreate	Help	"On Browser"		O	{browserHelp}
    menuButtonCreate	Help	"Manuals contents"	M	{tornadoHelp}
    menuButtonCreate	Help	"Manuals index"		i	{windmanHelp}
    menuButtonCreate	About	Tornado			T	{tornadoVersion}

    # Manual update button (!).

    toolBarItemCreate update		button	{update} \
				"dragReturnString !" \
				"update"

    # Creation of the AutoUpdate button that runs the browser's automatic
    # update (clock).

    toolBarItemCreate autoUpdate	button	{updateToggleProc}

    # Update all browsed objects.  Initially, this will simply fill the main
    # active tasks hierarchy.

    hierarchySetStructure "Active Tasks" "{{WRS Tasks} +} {{User Tasks} +}"

    # Initializes the memory charts

    chartSetQuantities memory {{"Tools" 0 100} {"Application" 0 100}}
}

##############################################################################
#
# activeTaskCallback - invoked when a task is clicked in the main task window
#
# This procedure calls browserCreate with the taskId corresponding to the
# task clicked on in the main task window.
#
# SYNOPSIS:
#   activeTaskCallback { taskId : taskName } 
#
# PARAMETERS:
#   provided by the entry clicked on in the main task hierarchy window;
#   this is a string containing the task Id, a colon, and the task name
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc activeTaskCallback {args} {
    set taskId [lindex [lindex [lindex $args 0] 2] 0]
    browserCreate $taskId
}

##############################################################################
#
# update - updates all browsed information
#
# This routine updates the "Active Tasks" hierarchy in the main
# window, and then consults the globalBrowseList to find the names and
# bindings of all other browser windows open.  For each entry in the
# globalBrowseList, we first check to see if the class of the object
# browsed is the same as it was: if not, the browser is destroyed (and
# will vanish from the screen) and removed from the globalBrowseList;
# if so, the information in the browser is refreshed by calling the
# object- specific browser function.
#
# We also update the object module list here.
#
# SYNOPSIS:
#   update
#
# PARAMETERS:
#   A boolean that indicates if update{} is called in a rescheduling context.
#
# RETURNS: N/A
#
# ERRORS:
#

proc update {{reschedule 0}} {
    global globalBrowseList
    global browserAuxProcList
    global spyInterval
    global spyRunning
    global browserUpdate
    global updateIntv

    # Check that the target system has been correctely initialized before
    # proceeding

    if {! [targetInitCheck]} {
	set activeTasks ""
    } else {
	set activeTasks [taskIdNameStatusList]
    }	

    hierarchySetValues "Active Tasks" [browseActiveTasks $activeTasks]
    
    # recompute inspectors; delete the ones whose objects don't exist
    # anymore.

    foreach item $globalBrowseList {
	bindNamesToList {class objId name} $item
	set browseProc ${class}Browse
	set nameProc ${class}BrowserName
	set hierName [$nameProc $objId]

	if [hierarchyIsDisplayed $hierName] {
	    # if objectClassify takes an RPC timeout error from WTX,
	    # this will cause the following test to kick the object
	    # out of the global browse list.  Reasonable? XXX

	    set classNow [objectClassify $objId]
	    
	    if {$classNow != $class \
		    || [catch { \
		    hierarchySetValues $hierName [$browseProc $objId] } \
		    rslt]} {
		hierarchyDestroy [$nameProc $objId]
		globalBrowseListRemove $item
	    }
	}
    }

    objectModuleUpdate
    memoryChartUpdate

    foreach auxproc $browserAuxProcList {
	$auxproc
    }

    browserCheckEvents

    # Reinitialize timers. If spy and browser update are set, the faster will
    # update. XXX PAD - if other features have an independant interval in the
    # future, we'll need a better unified interface to the update mechanism.

    if {$spyRunning && $browserUpdate} {
	if {$spyInterval <= $updateIntv} {
	    if {$reschedule} {
		delayedEval $spyInterval {update 1}
    	    }
	} else {
	    if {$reschedule} {
		delayedEval $updateIntv {update 1}
	    }
	}
    } else {
	if {$reschedule && $spyRunning} {
	    delayedEval $spyInterval {update 1}
	}

	if {$reschedule && $browserUpdate} {
	    delayedEval $updateIntv {update 1}
	}
    }
}

##############################################################################
#
# browse - browse an object, given its ID
#
# This routine is bound to the "Show" button, and is invoked when that button
# is clicked.  If the argument (the contents of the text in the text field)
# starts with a number, the id is considered the numeric address of the object
# to browse.  Otherwise, it is treated as a symbol and WTX is called to look
# up the symbol's value.  The object ID is the contents of the integer pointed
# to by the symbol's value (unless the user prefaced the symbol name with "&").
# If all goes well, browserCreate is called with the object id.  If the 
# argument can't be deciphered, a notice is posted indicating this.
#
# SYNOPSIS:
#   browse [objAddr | symbol | &symbol]
#
# PARAMETERS:
#   objAddr: the address of an object to brose
#   symbol: a symbolic address whose contents is the address of 
#     an object to browse
#   &symbol: a symbolic address that is the address of an object to browse
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc browse {args} {
    if {"$args" == ""} {return}
    if [regexp {^[0-9].+} $args] {
	set addr $args
    } else {
	if {[string index $args 0] == "&"} {
	    set args [string range $args 1 end]
	    set ampersand 1
	} else {
	    set ampersand 0
	}

	if ![catch {set sym [wtxSymFind -name $args]} result] {
	    set addr [lindex $result 1]

	    if {$ampersand == 0} {
		# If the symbol name didn't start with an &,
		# get the int pointed to by the value of the 
		# symbol.
		set addr [wtxGopherEval "$addr @"]
	    }
	} else {
	    if {[wtxErrorName $result] == "SVR_CANT_CONNECT_LICENSE_DAEMON"} {
		error $result
	    } else {
		noticePost error "I don't understand that."
	    }
	    return
	}
    }

    browserCreate $addr
}

##############################################################################
#
# browserCheckEvents -
#
#
#
# SYNOPSIS:
#   browserCheckEvents
#
# PARAMETERS: N/A
# 
# RETURNS: N/A
#
# ERRORS: N/A
#

proc browserCheckEvents {} {
    set event ""
    while {[set event [wtxEventGet]] != ""} {
	if {[lindex $event 0] == "Browser"} {
	    eval [lrange $event 1 end]
	}
    }
}

##############################################################################
#
# objDestroy - react when a hierarchy containing a browsed object is destroyed
#
# SYNOPSIS:
#   objDestroy <hierName>
#
# PARAMETERS:
#   hierName: name of hierarchy window destroyed
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc objDestroy {hName} {
    globalBrowseListRemoveByName $hName
    hierarchyDestroy $hName
}

##############################################################################
#
# browserCreate - create a browser for an object, given its ID
#
# The class of the object is determined with objectClassify.  If this succeeds,
# we then look for 3 Tcl procedures: <class>BrowserName, <class>Struct, and
# <class>Browse.  If any of these three procedures are absent, the object 
# cannot be browsed and a notice is posted saying so.  Otherwise the first
# of these procedures is called to get the name of the browser for the object.
# The list of existing hierarchy objects is queried to see if there is already
# one by this name; if not, it is created.  Then the second function, 
# <class>Struct, is called to set the structure of the newly-created hierarchy.
# The hiearchy is then posted if necessary.  Finally the third function,
# <class>Browse, is called, and its return value is presented to the 
# hierarchy for display.  A triple indicating the object class, the 
# object id, and the name of the browser is added to the globalBrowseList
# so that this process can be repeated when it is time to update all
# browsed objects.
#
# SYNOPSIS:
#   browserCreate objId
#
# PARAMETERS:
#   objId: the ID of the object to create a browser for
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc browserCreate {objId} {

    if {$objId == 0} {
	noticePost error "Can't browse an object at address 0."
	return
    }

    set class [objectClassify $objId]

    if {$class == 0} {
	noticePost error "I don't know what kind\nof object that is."
	return
    }

    set brNameFunc ${class}BrowserName
    set brStructFunc ${class}Struct
    set brBrowseFunc ${class}Browse

    if {[info proc $brNameFunc] == "" \
	    || [info proc $brBrowseFunc] == "" \
	    || [info exists $brStructFunc] == "" } {
	noticePost warning "I don't know how to\nbrowse $class objects."
	return
    }

    set hierName [$brNameFunc $objId]
    set needPost 0

    if ![hierarchyExists $hierName] {
	hierarchyCreate -destroy objDestroy -change $hierName objectTraverse
	hierarchySetStructure $hierName [$brStructFunc $objId]
	set needPost 1
    }

    hierarchySetValues $hierName [$brBrowseFunc $objId]
    if {$needPost || ![hierarchyIsDisplayed $hierName]} {
	hierarchyPost $hierName
    }
    globalBrowseListAdd [list $class $objId $hierName]
}

##############################################################################
#
# objectTraverse - traverse an object
#
# This routine is attached to the traversal callback of floating "object" 
# browsers.  This routine attempts to determine if the entry clicked on
# represents an object that can itself be displayed.  If so, we create
# a browser on that object.
#
# SYNOPSIS:
#   objectTraverse (any arguments--invoked from a hierarchy click)
#
# PARAMETERS:
#   any
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc objectTraverse {args} {
    set lastentry [lindex $args 0]
    if {[llength $lastentry] == 3} {
	if {[lindex $lastentry 1] == "="} {
	    bindNamesToList {field dummy value} $lastentry
	    if {$field == "id" || $field == "task_id"} {
		browse $value
		return
	    }
	    if {$field == "name"} {
		set taskId [taskNameToId $value]
		if {$taskId != 0} {
		    browse $taskId
		    return
		}
	    }
	}
    }
}
	
##############################################################################
#
# objectModuleUpdate - update object module information list
#
# Using WTX calls, prepare the text that should fill the object module 
# information box.
#
# SYNOPSIS:
#   objectModuleUpdate
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc objectModuleUpdate {} {
    global scrBrPos

    set omList [wtxObjModuleList]
    set totalText 0
    set totalData 0
    set totalBss 0

    set objModString ""
    foreach objMod $omList { 
	set objModInfo [wtxObjModuleInfoGet $objMod]
	set text [lindex [lindex $objModInfo 5] 2]
	set data [lindex [lindex $objModInfo 6] 2]
	set bss  [lindex [lindex $objModInfo 7] 2]

	set omString [format "%#10x %15s %8d %8d %8d" \
		$objMod [lindex $objModInfo 1] \
		$text $data $bss]
	set objModString "$objModString$omString\n"
	incr totalText $text
	incr totalData $data
	incr totalBss $bss
    }

    set objModString "$objModString\n"
    set totalString [format "                    Total: %8d %8d %8d" \
	    $totalText $totalData $totalBss]
    set objModString "$objModString$totalString\n"
    textWindowSet objects $objModString

    # XXX prototype the memory computations here.

    # scroll the module object list to its previous position

    scrollBarSet $scrBrPos
}

##############################################################################
#
# memoryChartUpdate - update memory bargraph
#
# Using WTX calls, compute the percentages of used memory in the agent pool
# and system memory partition.
#
# SYNOPSIS:
#   memoryChartUpdate
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc memoryChartUpdate {} {
    global maxPoolMem
    global maxVxMem
    set agentMem [wtxMemInfoGet]
    set aCurFree [lindex $agentMem 0]
    set aCurAlloc [lindex $agentMem 1]

    if {![catch {wtxSymFind -name memSysPartId} result]} {
	set memSysPartId [wtxGopherEval "[lindex $result 1] @"]
	set targetMem [memPartInfoGetShort $memSysPartId]
	set tCurAlloc [expr [lindex $targetMem 2] * 2]
	set tMem [expr [lindex $targetMem 0] * 2]
    } else {
	set tCurAlloc 0
	set tMem 0
    }

    if {$aCurAlloc >= $maxPoolMem} {set maxPoolMem $aCurAlloc}
    if {$tCurAlloc >= $maxVxMem} {set maxVxMem $tCurAlloc}

    chartSetValues memory [list \
	    [list $aCurAlloc 0 [expr $aCurAlloc+$aCurFree] $maxPoolMem] \
	    [list $tCurAlloc 0 $tMem $maxVxMem]]

}

##############################################################################
#
# browserAuxProcAdd -
#
# This procedure adds a procedure name to the list of procedures that need
# to be called when the update mechanism is on (either automatically or
# manually.
#
# SYNOPSIS
# browserAuxProcAdd procName
#
# PARAMETERS:
# auxproc - name of the procedure to call
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc browserAuxProcAdd {auxproc} {
    global browserAuxProcList

    if {[lsearch $browserAuxProcList $auxproc] == -1} {
	lappend browserAuxProcList $auxproc
    }
}

##############################################################################
#
# updateToggleProc - switch between automatic update and manual update
#
# This procedure switches the current update state: manual or automatic. It also
# updates the status bar
#
# This procedure is called when the browser's AutoUpdate button is pressed.
#
# SYNOPSIS:
# updateToggleProc
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc updateToggleProc {} {
    global browserUpdate
    global updateIntv
    global updateMode
    global autoUpdateStr
    global noAutoUpdateStr
    global spyRunning

    # If automatic update was on, turn it off

    if {$browserUpdate} {
	set browserUpdate 0
	set updateMode $noAutoUpdateStr
	browserStatusUpdate

    } else {

	# If automatic update was off, turn it on. Use current interval.
	# Note that we don't want to interfere with spy which already turned
	# on the update mechanism.

	if {! $spyRunning} {
	    delayedEval $updateIntv {update 1}
	}
	set browserUpdate 1
	set updateMode $autoUpdateStr
	browserStatusUpdate
    }
}

################################################################################
#
# browserHelp - display the browser manual page
#
# This procedure runs the windHelp tool to display the browser manual page.
#
# SYNOPSIS:
# browserHelp
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc browserHelp {} {

    if {[info proc windHelpDisplay] == ""} {
    	# Source windHelp library
	uplevel #0 source [wtxPath host resource doctools]windHelpLib.tcl
    }

    set htmlAddr [windHelpLinkGet -name browser]

    if {$htmlAddr != ""} {
    	windHelpDisplay $htmlAddr
    } else {
    	error "Tornado help files not installed"
    }
}

################################################################################
#
# tornadoHelp - launch the hyperhelp
#
# This procedure runs the windHelp tool
#
# SYNOPSIS:
# tornadoHelp
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc tornadoHelp {} {
    global docDir

    if {[info proc windHelpDisplay] == ""} {
    	# Source windHelp library
	uplevel #0 source [wtxPath host resource doctools]windHelpLib.tcl
    }

    windHelpDisplay $docDir/books.html
}

################################################################################
#
# windmanHelp - launch windman
#
# This procedure runs the windman tool
#
# SYNOPSIS:
#	windmanHelp
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc windmanHelp {} {
    exec windman &
}

##############################################################################
#
# browserStatusUpdate - update the status bar
#
# This procedure calls the function that set the browser's status bar with
# the correct information.
#
# SYNOPSIS:
# browserStatusUpdate
#
# PARAMETERS: N/A
#
# RETURNS: N/A
# 
# ERRORS: N/A

proc browserStatusUpdate {} {
    global updateMode
    global symSortMode
    global spyMode

    configStatusSet "$updateMode $symSortMode / $spyMode"
}

##############################################################################
#
# browserAttach - attach browser to given target server
#
# This procedure attaches the browser to a specified target server.
#
# SYNOPSIS:
# browserAttach ts
#
# PARAMETERS:
# ts - target server to attached to
#
# RETURNS: N/A
# 
# ERRORS: N/A
#

proc browserAttach {ts} {

    if {[catch {wtxTargetAttach} result]} {
	set errName [wtxErrorName $result]
	if {$errName == "EXCHANGE_TRANSPORT_DISCONNECT"} {
	    puts stderr "Target server locked."
	} else {
	    puts stderr "Couldn't attach to $ts.\nError: $result"
	}
	exit 1
    }

    # Install a WTX Tcl error handler, so we can see when the target 
    # reboots and offer to reconnect.

    wtxErrorHandler [wtxHandle] browserErrorHandler

    # register for browser events
    wtxRegisterForEvent "Browser .*"

    # Set the status bar
    browserStatusUpdate

    # make first update.
    shellBrowseInit
    wsTitleSet "Browser [wtxTargetName]"
    update
}

##############################################################################
#
# browserErrorHandler - error handler
#
# This procedure is the the error handler. It essentially reattaches the
# browser when the error is amog the following:
#   AGENT_COMMUNICATION_ERROR,
#   AGENT_COMMUNICATION_ERROR,
#   EXCHANGE_NO_TRANSPORT
#   AGENT_UNSUPPORTED_REQUEST
#   SVR_TARGET_NOT_ATTACHED
#   EXCHANGE_TRANSPORT_DISCONNECT
# If a different error is intercepted, the handler simply resubmit this error.
#
# SYNOPSIS:
# browserErrorHandler hwtx cmd err tag
#
# PARAMETERS:
# hwtx -
# cmd  -
# err  - WTX error string
# tag  -
#
# RETURNS: N/A
# 
# ERRORS: N/A
#

proc browserErrorHandler {hwtx cmd err tag} {

    set errName [wtxErrorName $err]
    if {$errName == "AGENT_COMMUNICATION_ERROR" \
	    || $errName == "EXCHANGE_NO_TRANSPORT" \
	    || $errName == "AGENT_UNSUPPORTED_REQUEST" \
	    || $errName == "SVR_TARGET_NOT_ATTACHED"  \
	    || $errName == "EXCHANGE_TRANSPORT_DISCONNECT" } {

	set reply [noticePost question \
		"Target connection has been lost.\nAttempt to reconnect?" \
		"Reconnect" "Quit"]

	if {$reply} {
	    # XXX try to restart with current window geometry
	    wtxToolDetach
	    wsRestart
	} else {
	    exit
	}

	# can't restart? resubmit the error.

	error $err
    }

    error $err
}

################################################################################
#
# Browser.tcl - Initialization
#
# This part of the file does all the necessary initialization and sourcing to
# get a runable browser application.
# The user's resource file, if any, is also sourced here.
#

# Create the menus and buttons

createForms

# load in the brower Tcl functions.

source [wtxPath host resource tcl]shelbrws.tcl

# set up browserAuxProcList

if {![info exists browserAuxProcList]} {
    global browserAuxProcList
    set browserAuxProcList ""
}

# Read in the app customizations.

set cfgdir [wtxPath host resource tcl app-config Browser]*.tcl
foreach file [lsort [glob -nocomplain $cfgdir]] {source $file}

# Read in the host configuration.

source [wtxPath host resource tcl app-config all]host.tcl

# Read in the user's home-directory Tcl initialization file.

if [file exists $env(HOME)/.wind/browser.tcl] {
    source $env(HOME)/.wind/browser.tcl
}
