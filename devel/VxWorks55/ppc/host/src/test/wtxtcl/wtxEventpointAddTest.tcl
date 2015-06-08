# wtxEventpointAddTest.tcl - Tcl script, test WTX_EVENTPOINT_ADD
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01s,28apr99,p_b  added more robustness.
# 01r,14may98,p_b  replaced test?.o by objSampleWtxtclTest?.o
# 01q,05may98,p_b  WTX2.0
# 01p,28jan98,p_b  added wtxEventpointAddTest3 - wtxEventpointAdd with
#                  more than 1 argument returned in the event type
# 01o,17sep96,jmp  added wtxEventpointAddTest2 - wtxEventpointAdd with
#		   ACTION_CALL.
# 01n,20oct95,jmp  added invalidAddr argument to wtxEventpointAddTest.
# 01m,22aug95,jmp  changed objSampleWtxtclTest1.o entry point, now uses cleanUp.
#		   added timeout to wtxToolAttachCall in wtxEventpointAddTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01l,23jun95,f_v  added wtxTestRun call
# 01k,19jun95,f_v  changed testTaskCreate call
# 01j,24may95,f_v  added catch on wtxContextResume
# 01i,04may95,f_v  replaced BREAKPOINT by TEXT_ACCESS, replace event choice in
#		   wtxRegisterForEvent
# 01h,20apr95,f_v  used generic procedures
# 01g,10apr95,f_v  quit test more properly
# 01f,28mar95,f_v  added wtxRegisterForEvent
# 		   changed pomme to test1
# 01e,20mar95,f_v  changed printLogo by a user task in wtxEventpointAddTest1
# 01d,09mar95,f_v  replaced BREAKPOINT by TEXT_ACCESS
#		   delete EvptMsg messages
# 01c,09feb95,f_v  changed EvMsg to EvptMsg, vEvMsg to vEvptMsg
#		   changed errEvMsg to errEvptMsg
#  		   put comments in front of EvptMsg
# 01b,27jan95,f_v  changed access messages, use wtxToolDetachCall
# 01a,10jan95,f_v  written. 
#
#

#
# DESCRIPTION
#
# WTX_EVENTPOINT_ADD  - WTX add a breakpoint
#
#
#

#*****************************************************************************
#
# wtxEventpointAddTest1 -
#
# Verify if wtxEventpointAdd add a breakpoint in the taskId task 
#

proc wtxEventpointAddTest1 {Msg} {

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
    set bkpNb [testEventpointAdd $Msg $entry $taskId]
    if {$bkpNb ==""} {
	return
	}

    # activate user task (taskActivate)
    if {[catch {wtxContextResume CONTEXT_TASK $taskId} resSt]} {
        testFail "ERROR : Can not perform a wtxContextResume."
        return
    }
    if {$resSt == ""} {
	return
	}

    # get infomation about bkp
    if {[catch {wtxEventGet} eventInfo]} {
        testFail "ERROR : Can not perform a wtxEventGet."
        return
    }

    set cp 0
    while {([string compare [lindex $eventInfo 0] TEXT_ACCESS] && \
            [string compare [lindex $eventInfo 1] $taskId] && \
            $cp < 10)} {
        if {[catch {wtxEventGet} eventInfo]} {
            testFail "ERROR : Can not perform a wtxEventGet."
            return
        }
        after 500
        incr cp
      }

    if {$cp >= 10} {
        testFail "ERROR : Event never received."
        return
    }

    # Get info about bkp
    set name [lindex $eventInfo 0]
    set tId [lindex $eventInfo 1]

    # clean up
    cleanUp b $bkpNb t $taskDesc

    if { $bkpNb < 0 || "$name"!= {TEXT_ACCESS} || $tId != $taskId } {
	    testFail $errEvptMsg(1)
	    return
        }


    wtxTestPass
}

#*****************************************************************************
#
# wtxEventpointAddTest2 - wtxEventpointAdd with ACTION_CALL
#
# Verify that wtxEventpointAdd adds a breakpoint and calls the specified
# function when the breakpoint is hit.
#

proc wtxEventpointAddTest2 {Msg} {

    global errEvptMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # verify if objSampleWtxtclTest6.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest6.o
    if { [file exists $name] == 0 } {
    	testFail "File $name is missing for wtxEventpointAddTest2 test"
    	return 0
    }

    # load objSampleWtxtclTest6.o file
    set tModuleId [wtxObjModuleLoad $USE_TSFORLOAD\
		LOAD_ALL_SYMBOLS [objPathGet test]objSampleWtxtclTest6.o]
    if {$tModuleId == ""} {
	return 0
    }

    # get object module Id
    set tModuleId [lindex $tModuleId 0]

    # create a new context
    set taskDesc [testTaskCreate $Msg objSampleWtxtclTest1.o test1]
    if {$taskDesc == 1} {
	return
    }

    # get test1 entry point
    if {![set entry [testSymFind test1]]} {
	cleanUp t $taskDesc
	return
    }

    set taskId [lindex $taskDesc 0]

    # get test6 entry point
    if {![set test6Addr [testSymFind funct1]]} {
	cleanUp t $taskDesc
	return
    }

    # Add a breakpoint 
    set bkpNb [wtxEventpointAdd TEXT_ACCESS $entry CONTEXT_TASK \
	$taskId ACTION_NOTIFY|ACTION_STOP|ACTION_CALL 0 $test6Addr 0]
    if {$bkpNb == ""} {
	cleanUp t $taskDesc
	return
    }

    # Verify that funct1() has not already been called
    # funct1() set staticIntVarInit to 0xabcdef
    set symAddr [testSymFind staticIntVarInit]
    set symValue [wtxGopherEval "$symAddr *!"]

    if { $symValue != 0x0 } {
	testFail $errEvptMsg(12)
	cleanUp b $bkpNb t $taskDesc
	return
    }

    # activate user task (taskActivate)
    if {[catch {wtxContextResume CONTEXT_TASK $taskId} resSt]} {
        testFail "ERROR : Can not perform a wtxContextResume."
        return
    }
    if {$resSt == ""} {
	cleanUp b $bkpNb t $taskDesc
	return
    }

    # get infomation about bkp
    if {[catch {wtxEventGet} eventInfo]} {
        testFail "ERROR : Can not perform a wtxEventGet."
        return
    }

    set cp 0
    while {([string compare [lindex $eventInfo 0] TEXT_ACCESS] && \
		[string compare [lindex $eventInfo 1] $taskId] && \
                $cp < 10)} {
        if {[catch {wtxEventGet} eventInfo]} {
            testFail "ERROR : Can not perform a wtxEventGet."
            return
        }
        after 500
        incr cp
    }

    if {$cp >= 10} {
        testFail "ERROR : Event never received."
        return
    }

    # Get info about bkp
    set name [lindex $eventInfo 0]
    set tId [lindex $eventInfo 1]

    if { $bkpNb < 0 || "$name"!= {TEXT_ACCESS} || $tId != $taskId } {
	testFail $errEvptMsg(1)
	cleanUp b $bkpNb t $taskDesc
	return
    }

    # Verify that funct1() has been called
    # funct1() set staticIntVarInit to 0xabcdef
    set symValue [wtxGopherEval "$symAddr *!"]

    if { $symValue != 0xabcdef } {
	testFail $errEvptMsg(12)
	cleanUp b $bkpNb t $taskDesc
	return
    }

    cleanUp b $bkpNb t $taskDesc

    wtxTestPass
}


#*****************************************************************************
#
# wtxEventpointAddTest3 - Test wtxEventpointAdd which has been modified  
#
# wtxEventpointListGet returns  more than one argument in the event
# type. The test checks this new feature. 
#
 
proc wtxEventpointAddTest3 {Msg} {
 
    global errEvptMsg
 
    puts stdout "$Msg" nonewline
 
    if {![file exists [objPathGet test]objSampleWtxtclTest7.o]} {
      testFail "File objSampleWtxtclTest7.o missing for wtxEventpointAddTest test"
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

    set entryArg "$entry $entry2 $entry3" 

    # set a breakpoint on user task - at the beginning of the task
    set bkpId [testEventpointAdd $Msg $entryArg $taskId]
    if {$bkpId ==""} {
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
        cleanUp b $bkpId t $taskDesc
        return
    }
 
    # Get the event_type and its addresses
    set event [lindex $param 0]
 
    # length must be 4
    set nbParam [llength $event]
    if { $nbParam != 4 } {
        testFail $errEvptMsg(3)
        cleanUp b $bkpId t $taskDesc
        return
    }
 
    # Now we are expecting more than one argument in "event"
    set eventTy [lindex $event 0]
    set eventAddr1 [lindex $event 1]
    set eventAddr2 [lindex $event 2]
    set eventAddr3 [lindex $event 3]
 
    if { "$eventTy" != {WTX_EVENT_TEXT_ACCESS}} {
        testFail $errEvptMsg(5)
        cleanUp b $bkpId t $taskDesc
        return
    }
 
    if {$eventAddr1 != $entry} {
        testFail $errEvptMsg(6)
        cleanUp b $bkpId t $taskDesc
        return
    }
 
    if {$eventAddr2 != $entry2} {
        testFail $errEvptMsg(6)
        cleanUp b $bkpId t $taskDesc
        return
    }
 
    if {$eventAddr3 != $entry3} {
        testFail $errEvptMsg(6)
        cleanUp b $bkpId t $taskDesc
        return
    }
 
   cleanUp b $bkpId t $taskDesc

   wtxTestPass
}


#*****************************************************************************
#
# wtxEventpointAddTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxEventpointAddTest {tgtsvrName timeout invalidAddr} {

    global vEvptMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxEventpointAddTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    # enable all events
    catch {wtxRegisterForEvent ^TEXT_ACCESS} st

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # call the test procedures
    wtxEventpointAddTest1 $vEvptMsg(1)
    wtxEventpointAddTest2 $vEvptMsg(6)
    wtxEventpointAddTest3 $vEvptMsg(8)

    # deconnection
    wtxToolDetachCall "wtxEventpointAddTest"
}
