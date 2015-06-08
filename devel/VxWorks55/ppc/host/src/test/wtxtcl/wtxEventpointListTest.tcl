# wtxEventpointListTest.tcl - Tcl script, test WTX_EVENTPOINT_LIST
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01p,14may98,p_b  Replace test?.o by objSampleWtxtclTest?.o
# 01o,28jan98,p_b  Adapted for WTX 2.0
# 01n,20oct95,jmp  added invalidAddr argument to wtxEventpointListTest.
# 01m,22aug95,jmp  changed test implementation, now the test doesn't set
#                  breakpoints on system calls. now uses cleanUp procedure.
#		   added timeout to wtxToolAttachCall in wtxEventpointListTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01l,23jun95,f_v  added wtxTestRun call
# 01k,19jun95,f_v  changed testTaskCreate call
# 01j,21may95,p_m  removed detection related stuff
# 01i,20apr95,f_v  use generic procedures
# 01h,10apr95,f_v  avoid memory leak
# 01g,03apr95,f_v  changed WDB_EVENT_TEXT_ACCESS to WTX_EVENT_TEXT_ACCESS
# 01f,30mar95,f_v  changed pomme to test1
# 01e,20mar95,f_v  changed printLogo by a user task in wtxEventpointListTest1
# 01d,09mar95,f_v  remove EvptMsg messages,replace BREAKPOINT by TEXT_ACCESS
# 01c,09feb95,f_v  change EvMsg to EvptMsg, vEvMsg to vEvptMsg
#		   change errEvMsg to errEvptMsg
#		   put comments in front of EvptMsg
# 01b,27jan95,f_v  change access messages, use wtxToolDetachCall
# 01a,10jan95,f_v  written
#
#

#
# DESCRIPTION
#
# WTX_EVENTPOINT_LIST  - WTX list breakpoints
#
#
#

#*****************************************************************************
#
# wtxEventpointListTest1 -
#
# Verify if wtxEventpointList list breakpoint in the CreateTest Task 
#

proc wtxEventpointListTest1 {Msg} {

    global errEvptMsg

    puts stdout "$Msg" nonewline

    # create a new context
    set taskDesc [testTaskCreate $Msg objSampleWtxtclTest1.o test1]
    if {$taskDesc == 1} {
	return
    }

    # get entry point
    if {![set entry [testSymFind test1]]} {
	cleanUp t $taskDesc
	return
	}

    set taskId [lindex $taskDesc 0]

    # set a breakpoint on user task - at the beginning of the task 
    set bkpId1 [testEventpointAdd $Msg $entry $taskId]
    if {$bkpId1 == ""} {
	return
	}

    # set a second breakpoint on user task at beginning of task + 0x10
    set entry  [ expr $entry + 0x10]
    set bkpId2 [testEventpointAdd $Msg $entry $taskId]
    if {$bkpId2 == ""} {
        return
        }


    # list all the breakpoints
    set listBreakpoint [wtxEventpointList]
    set bkpNb [llength $listBreakpoint]

    # get different parameters of bkp
    set param [lindex $listBreakpoint [expr $bkpNb-1]]

    # length must be 3
    set nbParam [llength $param]
    if { $nbParam != 3 } {
	testFail $errEvptMsg(3)
	cleanUp b $bkpId1 $bkpId2 t $taskDesc
	return
	}

    # Get the event_type and its adress
    set event [lindex $param 0]
    set eventTy [lindex $event 0]
    set eventAddr [lindex $event 1]

    if { "$eventTy" != {WTX_EVENT_TEXT_ACCESS}} {
	testFail $errEvptMsg(5)
	cleanUp b $bkpId1 $bkpId2 t $taskDesc
	return
	}

    if {$eventAddr != $entry} {
	testFail $errEvptMsg(6)
	cleanUp b $bkpId1 $bkpId2 t $taskDesc
	return
	}

    # Get the context_type and its adress
    set context [lindex $param 1]
    set contTy [lindex $context 0]
    set contAddr [lindex $context 1]
    
    if {"$contTy" != {CONTEXT_TASK}} {
	testFail $errEvptMsg(7)
	cleanUp b $bkpId1 $bkpId2 t $taskDesc
	return
        }

    if { $contAddr != $taskId} {
	testFail $errEvptMsg(8)
	cleanUp b $bkpId1 $bkpId2 t $taskDesc
	return
        }

    # Get the action_type must be ACTION_NOTIFY|ACTION_STOP
    set action [lindex [lindex $param 2] 0] 
    if {"$action" != {ACTION_NOTIFY|ACTION_STOP}} {
	testFail $errEvptMsg(9)
	cleanUp b $bkpId1 $bkpId2 t $taskDesc
	return
	}

    # Clean up
    cleanUp b $bkpId1 $bkpId2 t $taskDesc

    wtxTestPass
}

#*****************************************************************************
#
# wtxEventpointListTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxEventpointListTest {tgtsvrName timeout invalidAddr} {

    global vEvptMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxEventpointListTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # call the test procedures
    wtxEventpointListTest1 $vEvptMsg(3)

    # deconnection
    wtxToolDetachCall "wtxEventpointListTest"
}
