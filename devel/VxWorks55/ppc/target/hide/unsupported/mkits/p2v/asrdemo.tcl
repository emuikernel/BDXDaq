# asrdemo.tcl - Browser's configuration panel
#
# Copyright 1994-1997 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01a,11feb97,jmb  created
#
# DESCRIPTION
# This file is the GUI for running asrdemo.  It should be invoked by the
# following command:
#
# wtxtcl> dialog -resource asrdemo -geometry 500x320 <tgtsvr>
#
# RESOURCE FILES: N/A
#

set taskList ""
set Amplitude(0) 1
set Frequency(0) 1
set Status(0) 1
set ampTid(0) 1
set freqTid(0) 1
set focusTid ""
set asrdemoAddr ""
set taskNameAddr ""
set evsendAddr ""
set targetHandle ""

set callTaskOptions 0x19
set lastErrorValue 0

##############################################################################
#
# shSymAddr - map a symbol name to its address (taken from shelcore.tcl)
#
# SYNOPSIS:
#   shSymAddr name
#
# PARAMETERS:
#   name: the name of the symbol to search for
#
# RETURNS:
#   the hexadecimal address of the symbol
#
# ERRORS:
#   "No symbol $name" if the symbol cannot be found.

proc shSymAddr {name} {
    if {[catch {set wtxSym [wtxSymFind -name $name]} result]} {
	if {[wtxErrorName $result] == "SYMTBL_SYMBOL_NOT_FOUND"} {
	    error "The shell cannot complete this request, because
the symbol $name cannot be found in the symbol table."
	} {
	    error $result
	}
    } {
	return [lindex $wtxSym 1]
    }
}
##############################################################################
#
# wtxEventPoll - poll the current connection for event (taken from shelcore.tcl)
#
# SYNOPSIS:
#   wtxEventPoll msec [maxtries]
#
# PARAMETERS:
#   msec:     number of milliseconds to wait between polls
#   maxtries: maximum number of polls to make before giving up
#
# RETURNS:
#   An event string, or "" if maxtries attempts were made without resulting
#   in an event, if maxtries was specified.
#
# ERRORS:
#   

proc wtxEventPoll {msec {maxtries 0}} {
    set event ""
    if {$maxtries > 0} {
	set ix 0
	while {[set event [wtxEventGet]] == "" && $ix < $maxtries} {
	    msleep $msec
	    incr ix
	}
    } {
	while {[set event [wtxEventGet]] == ""} {
	    msleep $msec
	}
    }
    return $event
}
##############################################################################
#
# shFuncCall - call a function synchronously (poll for the return value)
#              This is a stripped down version that does not call
#              error handlers.  It was taken from shell.tcl.
#
# The function is called via wtxFuncCall.  Then, we poll the target server
# waiting for the event that contains the return value of the function to
# arrive.  When it does, the return value supplied by WTX is returned by
# this function.  
#
# SYNOPSIS:
#   shFuncCall [-int | -float | -double] [-nobreak] func [a0 ... a9]
#
# PARAMETERS:
#   -int, -float, -double: type of result
#   -nobreak: set the VX_UNBREAKABLE bit during the call
#   func: address of function to call
#   a0...a9: integer function arguments
#
# RETURNS:
#   The return value of the function called on the target.
#
# ERRORS: N/A
#
# CAVEATS:
#   This function discards events it receives that do not pertain to the
#   function call while waiting for the return-value event.
#
proc shFuncCall {args} {
    global callTaskOptions
    global lastErrorValue

    set func 0
    set alist ""
    set type "-int"
    set nobreak 0

    foreach arg $args {
	switch -glob -- $arg {
	    -i*		{set type "-int"}
	    -f*		{set type "-float"}
	    -d*		{set type "-double"}
	    -n*		{set nobreak 1}
	    default	{
		if {$func == 0} {
		    set func $arg
		} else {
		    lappend alist $arg
		}
	    }
	}
    }
    
    if {[llength $alist] > 10} {
	error "Too many arguments to function (limit: 10)"
    }

    set opt $callTaskOptions

    if {$type == "-float" || $type == "-double"} {
	# call the function with VX_FP_TASK set.
	set opt [expr $callTaskOptions | 0x8]
    }

    if {$nobreak} {
	# or in VX_UNBREAKABLE
	set opt [expr $opt | 0x2]
    }

    set fcExpr "wtxFuncCall $type -opt $opt $func $alist"
    set funcCallId [eval $fcExpr]

    while 1 {
	catch {set event [wtxEventPoll 100]} result

	if {[lindex $event 0] == "CALL_RETURN"
	    && [lindex $event 1] == $funcCallId} {
	    if {[lindex $event 3]} {
		set lastErrorValue [expr [lindex $event 3]]
	    }
	    return [lindex $event 2]
	} else {
		return 0
	    }
	}
    }

proc asrdemoDialog {} {

    # Main dialog

    dialogCreate "main" -size 500 320 {
	{ list "Task" -width 50 -height 140 -left .+100  -top . taskCb }
        { list "Amp" -width 50 -vspan -left .+200  -top . ampCb }
	{ list "Freq" -width 50 -vspan -left .+300  -top . freqCb }
	{ list "Status" -width 50 -height 70 -left .+400  -top . statCb }
	{ button "Create" -left .+5 -top .+5 -width 80  createCb }
        { button "Delete" -left .+5 -top @+10 -width 80  deleteCb }
	{ button "Quit" -left .+5 -top @+80 -width 80 quitCb }
    }

#
    wsTitleSet  "ASR Demo"

    dialogSetValue "main" "Amp" {1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16}
    dialogSetValue "main" "Freq" {1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16}
    dialogSetValue "main" "Status" { run stop }

    # This dialog pops up when "Create" is selected in "main" dialog

    dialogCreate "enterTid" -size 135 100 {
	{ text "Transmitter ID:" -left .+5 -top .+5 -width 125 }
	{ button "Done" -left 25 -bottom 95 newXmtCb }
    }

    dialogPost "main"
    #
    #  Jimmy, testing....Create a chart
    #
    #    chartCreate "chart" -scale -valuefmt "%d" -size 350 100  
    #    chartSetQuantities "chart"  {{"amp" 0 16} {"freq" 0 16}}
    #    chartSetValues "chart" {{4} {10}}
    #    chartTitleSet "chart" "Transmitter A"
    #    chartPost "chart"
}



##############################################################################
#
# ldelete - Delete an item from a list 
#
# RETURNS:
#   A new list without item "value"
#
# ERRORS: N/A
#
proc ldelete { list value } {
    set ix [lsearch -exact $list $value]
    if { $ix >= 0 } {
        return [ lreplace $list $ix $ix ]
    } else {
	return $list
    }
}

proc deleteCb {} {
    global taskList
    global Amplitude
    global Frequency
    global focusTid
    global evsendAddr
    global ampTid
    shFuncCall -nobreak -int $evsendAddr $ampTid($focusTid) -3
    unset Amplitude($focusTid)
    unset Frequency($focusTid)
    set taskList [ ldelete "$taskList" "$focusTid" ]
    dialogSetValue "main" "Task" $taskList
}

proc statCb { value } {
    global focusTid
    global evsendAddr
    global ampTid
    global Status
    if { $value == "stop" } {
        shFuncCall -nobreak -int $evsendAddr $ampTid($focusTid) -1
    } else {
        shFuncCall -nobreak -int $evsendAddr $ampTid($focusTid) -2
    }
    set Status($focusTid) $value
}

proc newXmtCb {} {
    global taskList
    global Amplitude
    global Frequency
    global Status
    global ampTid
    global freqTid
    global taskNameToIdAddr
    global taskNameAddr
    global asrdemoAddr

    set newTid [dialogGetValue "enterTid" "Transmitter ID:"]
    dialogSetValue "enterTid" "Transmitter ID:" ""

    set newTid [string trimleft $newTid]
    set newTid [string trimright $newTid]

    if { [string length $newTid] == 0 } {
	return
    }

    if { [string length $newTid] != 1 } {
	puts stdout "Invalid string length"
	MsgBox "Invalid ID" "Identifier must be a single character, please try again"
	return
    }
    if { [lsearch -exact $taskList $newTid] != -1 } {
        puts stdout "Duplicate Id"
	MsgBox "Duplicate ID" "That ID is already in use, please try another"
	return
    } else {
#  User input OK, let's spawn a new Transmitter "
        scan $newTid "%c" tmp
        set tmpTsk [ wtxContextCreate 1 demoTsk 100 0 0 2000 $asrdemoAddr \
	    0 0 [set tmp] ]
        wtxContextResume CONTEXT_TASK $tmpTsk
# Sleep for a bit so demo driver has time to spawn Transmitter
        msleep 250
# Get the task ID's of the amplitude and freqency controller
	set ampTaskName "AMP"
	append ampTaskName $newTid
	set blockId [ memBlockCreate -string $ampTaskName ] 
	wtxMemWrite $blockId $taskNameAddr
        set ampTid($newTid) [shFuncCall -nobreak -int $taskNameToIdAddr                      $taskNameAddr ]
	memBlockDelete $blockId
	set freqTaskName "FRQ"
	append freqTaskName $newTid
	set blockId [ memBlockCreate -string $freqTaskName ] 
	wtxMemWrite $blockId $taskNameAddr
        set freqTid($newTid) [shFuncCall -nobreak -int $taskNameToIdAddr                     $taskNameAddr ]
	memBlockDelete $blockId
	lappend taskList $newTid
	set taskList [lsort $taskList]
    }  
    dialogSetValue "main" "Task" $taskList
# set value of amplitude and frequency to 1
    set Amplitude($newTid) 1
    set Frequency($newTid) 1
    set Status($newTid) run
#set focus to newly created task
    dialogListSelect "main" "Task" $newTid
    taskCb $newTid
    dialogUnpost "enterTid"
    }

proc createCb {} {
    dialogPost "enterTid"
}


##############################################################################
#
proc quitCb {} {
    global taskList
    global focusTid

#   traverse the task list and "delete" all transmitters
    foreach task $taskList { 
	set focusTid $task
	deleteCb
    }
    dialogDestroy "main"
    wtxToolDetach -all
    exit 0
}

#############################################################################
#
#  MsgBox - displays a message in the named message box.
#
proc MsgBox {boxName msg} {

    dialogCreate $boxName -size 500 120 {
	{ multitext "" -left .+5 -top .+5 -height 65 -hspan }
	{ button "OK" -left 42 -top 60 -width 65 MsgBoxOK }
    }
    dialogSetValue $boxName "" $msg

    dialogPost $boxName
}


#############################################################################
#  MsgBoxOK - Turns off the MsgBox
#
#  Each invocation of MsgBox must have a unpost entry here.
#

proc MsgBoxOK {} {
    dialogUnpost "Invalid ID"
    dialogUnpost "Duplicate ID"
}

#############################################################################
#  taskCb - Set the focusTid and highlight its amplitude and frequency
#
#

proc taskCb {tid} {
    global Amplitude
    global Frequency
    global Status
    global focusTid

    dialogListSelect "main" "Amp" $Amplitude($tid)
    dialogListSelect "main" "Freq" $Frequency($tid)

    if { $Status($tid) == "run" } {
        dialogListSelect "main" "Status" "run"
    } else {
        dialogListSelect "main" "Status" "stop"
    }

    set focusTid $tid
}

#############################################################################
#  ampCb - Reset the amplitude of transmitter with focusTid
#
#

proc ampCb {amp} {
    global Amplitude
    global focusTid
    global ampTid
    global evsendAddr
    shFuncCall -nobreak -int $evsendAddr $ampTid($focusTid) $amp
    set Amplitude($focusTid) $amp
}
#############################################################################
#  freqCb - Reset the frequency of transmitter with focusTid
#
#

proc freqCb {freq} {
    global Frequency
    global focusTid
    global freqTid
    global evsendAddr
    shFuncCall -nobreak -int $evsendAddr $freqTid($focusTid) $freq
    set Frequency($focusTid) $freq
}

##############################################################################
#
# getTargetFunctions - 
#
# Download the application to the target and get addresses of function
# calls
# 

proc getTargetFunctions { } {
    global asrdemoAddr
    global evsendAddr
    global taskNameToIdAddr
    global taskNameAddr

#   Load object module if it's not already there

    if  [ catch {wtxObjModuleFind "asrdemo.o" } result]  {
        wtxObjModuleLoad /vobs/wpwr/target/config/sunos/asrdemo.o
    }

#   Get entry point of application

    set asrdemoAddr [shSymAddr asrdemo]

#   Transition kit ev_send routine

    set evsendAddr [shSymAddr ev_send]

#   VxWorks taskNameToId function

    set taskNameToIdAddr [shSymAddr taskNameToId]
    set taskNameAddr [ wtxMemAlloc 5 ]
}

##############################################################################
#
# connectTarget - 
#
# Connect to a target server.  The argument to this function should be
# a list whose last element is the target server name.  
# 
proc connectTarget {arg} {
    global targetHandle
    set tgtsvr [lindex $arg [expr [llength $arg] - 1]]
    puts stdout "Attaching to target server..."
    if  [ catch {wtxToolAttach $tgtsvr} targetHandle]  {
	puts "Unable to attach to target $tgtsvr"
	exit 1
    } else {
	puts stdout "Attach complete!"
    }
    wtxRegisterForEvent .*
}


##############################################################################
#
# 01ASRdemo.tcl - This is the "main program"
#
# 

connectTarget $argv

getTargetFunctions

asrdemoDialog
