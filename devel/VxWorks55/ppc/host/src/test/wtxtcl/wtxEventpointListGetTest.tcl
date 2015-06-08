# wtxEventpointListGetTest.tcl - Tcl script, test WTX_EVENTPOINT_LIST_GET
#
# Copyright 1998 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01b,28may98,p_b  New WTX2.0 changes
# 01a,27May98,p_b  Import from WTX_EVENTPOINT_LIST and changes.
#

#
# DESCRIPTION
#
# WTX_EVENTPOINT_LIST_GET  - WTX LIST GET Breakpoints
#
#
#

#*****************************************************************************
#
# wtxEventpointListGetTest1 -
#
# Verify if wtxEventpointListGet list breakpoint in the CreateTest Task 
# Verify if the three entry points are returned
#

proc wtxEventpointListGetTest1 {Msg} {

    global errEvptMsg

    puts stdout "$Msg" nonewline

    if {![file exists [objPathGet test]objSampleWtxtclTest7.o]} {
      testFail "File objSampleWtxtclTest7.o missing for\
            wtxEventpointListGet test"
      return 0
    }

    # create a new context
    set taskDesc [testTaskCreate $Msg objSampleWtxtclTest7.o test1]
    if {$taskDesc == 1} {
	return
    }

    # get entry point
    if {![set entry [testSymFind test1]]} {
	cleanUp t $taskDesc
	return
    }

    # get a second entry point
    if {![set entry2 [testSymFind test4]]} {
       cleanUp t $taskDesc
       return
    }

    # get a third entry point
    if {![set entry3 [testSymFind test5]]} {
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

    set entryArg "$entry $entry2 $entry3"

    set bkpId2 [testEventpointAdd $Msg $entryArg $taskId]
    if {$bkpId2 == ""} {
        return
    }

    # list all the breakpoints
    set listBreakpoint [wtxEventpointListGet]
    set bkpNb [llength $listBreakpoint]

    # get different parameters of bkp
    set param [lindex $listBreakpoint [expr $bkpNb-1]]

    # length must be 5
    set nbParam [llength $param]
    if { $nbParam != 5 } {
	testFail $errEvptMsg(3)
	cleanUp b $bkpId1 $bkpId2 t $taskDesc
	return
    }

    # Get the event_type and its addresses
    set event [lindex $param 0]

    # length must be 4
    set nbParam [llength $event]
    if { $nbParam != 4 } {
        testFail $errEvptMsg(3)
        cleanUp b $bkpId1 $bkpId2 t $taskDesc
        return
    }

    set eventTy [lindex $event 0]
    set eventAddr1 [lindex $event 1]
    set eventAddr2 [lindex $event 2]
    set eventAddr3 [lindex $event 3]

    if { "$eventTy" != {WTX_EVENT_TEXT_ACCESS}} {
	testFail $errEvptMsg(5)
	cleanUp b $bkpId1 $bkpId2 t $taskDesc
	return
    }

    if {$eventAddr1 != $entry} {
	testFail $errEvptMsg(6)
	cleanUp b $bkpId1 $bkpId2 t $taskDesc
	return
    }

    if {$eventAddr2 != $entry2} {
        testFail $errEvptMsg(6)
        cleanUp b $bkpId1 $bkpId2 t $taskDesc
        return
    }

    if {$eventAddr3 != $entry3} {
        testFail $errEvptMsg(6)
        cleanUp b $bkpId1 $bkpId2 t $taskDesc
        return
    }

    # Get the context_type and its address
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
# wtxEventpointListGetTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxEventpointListGetTest {tgtsvrName timeout invalidAddr} {

    global vEvptMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxEventpointListGetTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # call the test procedures
    wtxEventpointListGetTest1 $vEvptMsg(7)

    # deconnection
    wtxToolDetachCall "wtxEventpointListGetTest"
}
