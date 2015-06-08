# wtxEventpointDeleteTest.tcl - Tcl script, test WTX_EVENTPOINT_DELETE
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01o,14may98,p_b  Replace test?.o by objSampleWtxtclTest?.o
# 01n,28jan98,p_b  Adapted for WTX 2.0
# 01m,12sep96,jmp  added wtxEventpointDeleteTest3 to cover SPR# 6233.
# 01l,20oct95,jmp  added wtxEventpointDeleteTest2.
#		   added invalidAddr argument to wtxEventpointDeleteTest.
# 01k,22aug95,jmp  changed objSampleWtxtclTest1.o entry point.
#		   added timeout to wtxToolAttachCall in wtxEventpointDeleteTest
#                  changed tests control method, now uses wtxErrorHandler.
# 01j,23jun95,f_v  added wtxTestRun call
# 01i,19jun95,f_v  changed testTaskCreate call
# 01h,20apr95,f_v  use generic procedure
# 01g,10apr95,f_v  avoid memory leak
# 01f,30mar95,f_v  changed pomme to test1
# 01e,20mar95,f_v  changed printLogo by a user task in wtxEventpointDeleteTest1
# 01d,09mar95,f_v  replace BREAKPOINT by TEXT_ACCESS
#		   remove EvptMsg messages
# 01c,09feb95,f_v  change EvMsg to EvptMsg, vEvMsg to vEvptMsg
#		   change errEvMsg to errEvptMsg
#		   put comments in front of EvptMsg
# 01b,27jan95,f_v  change access messages, use wtxToolDetachCall
# 01a,10jan95,f_v  written
#

#
# DESCRIPTION
#
# WTX_EVENTPOINT_DELETE  - WTX delete a breakpoint
#
#
#

#*****************************************************************************
#
# wtxEventpointDeleteTest1 -
#
# Verify if wtxEventpointdelete delete a breakpoint in the CreateTest Task 
#

proc wtxEventpointDeleteTest1 {Msg} {

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
    if {$bkpNb == ""} {
	return
	}

    # Get the number of breakpoint
    set bkpCount1 [llength [wtxEventpointList]]

    # Delete the previous breakpoint
    set status [wtxEventpointDelete $bkpNb]

    # Get the new number of breakpoint 
    set bkpCount2 [llength [wtxEventpointList]]

    set bkpCount2 [expr $bkpCount2+1] 

    # activate user task (taskActivate)
    cleanUp t $taskDesc

    if { $bkpCount1 != $bkpCount2 } {
	    testFail $errEvptMsg(2)
	    return
        }

    wtxTestPass
}


#*****************************************************************************
#
# wtxEventpointDeleteTest2 -
#
# Verify if wtxEventpointdelete delete a breakpoint among a list.
#

proc wtxEventpointDeleteTest2 {Msg} {

    global errEvptMsg

    puts stdout "$Msg" nonewline


 # FUNCT1 - Eventpoint 1
 #######################

    # create a new context
    set taskDesc1 [testTaskCreate $Msg objSampleWtxtclTest6.o funct1]
    if {$taskDesc1 == 1} {
        return
    }

    set moduleId [lindex $taskDesc1 1]

    # get entry point
    if {![set entry1 [testSymFind funct1]]} {
    	cleanUp c $taskId1 \
		o $moduleId
        return
        }

    set taskId1 [lindex $taskDesc1 0]

    # set a breakpoint on user task - at the beginning of the task
    set bkpNb1 [testEventpointAdd $Msg $entry1 $taskId1]
    if {$bkpNb1 == ""} {
    	cleanUp c $taskId1 \
		o $moduleId
        return
        }

 # FUNCT2 - Eventpoint 2
 #######################

    # create a new context
    set taskDesc2 [testTaskCreate $Msg objSampleWtxtclTest6.o funct2]
    if {$taskDesc2 == 1} {
    	cleanUp c $taskId1 \
		b $bkpNb1
        return
    }

    set moduleId [lindex $taskDesc2 1]

    # get entry point
    if {![set entry2 [testSymFind funct2]]} {
    	cleanUp c $taskId1 $taskId2 \
		b $bkpNb1 \
		o $moduleId
        return
        }

    set taskId2 [lindex $taskDesc2 0]

    # set a breakpoint on user task - at the beginning of the task
    set bkpNb2 [testEventpointAdd $Msg $entry2 $taskId2]
    if {$bkpNb2 == ""} {
    	cleanUp c $taskId1 $taskId2 \
		b $bkpNb1 \
		o $moduleId
        return
        }

 # FUNCT3 - Eventpoint 3
 #######################

    # create a new context
    set taskDesc3 [testTaskCreate $Msg objSampleWtxtclTest6.o funct3]
    if {$taskDesc3 == 1} {
    	cleanUp c $taskId1 $taskId2 \
		b $bkpNb1 $bkpNb2 \
		o $moduleId
        return
    }

    set moduleId [lindex $taskDesc3 1]

    # get entry point
    if {![set entry3 [testSymFind funct3]]} {
    	cleanUp c $taskId1 $taskId2 $taskId3\
		b $bkpNb1 $bkpNb2 \
		o $moduleId
        return
        }

    set taskId3 [lindex $taskDesc3 0]

    # set a breakpoint on user task - at the beginning of the task
    set bkpNb3 [testEventpointAdd $Msg $entry3 $taskId3]
    if {$bkpNb3 == ""} {
    	cleanUp c $taskId1 $taskId2 $taskId3 \
		b $bkpNb1 $bkpNb2 \
		o $moduleId
        return
        }

 # FUNCT4 - Eventpoint 4
 #######################

    # create a new context
    set taskDesc4 [testTaskCreate $Msg objSampleWtxtclTest6.o funct4]
    if {$taskDesc4 == 1} {
    	cleanUp c $taskId1 $taskId2 $taskId3 \
		b $bkpNb1 $bkpNb2 $bkpNb3 \
		o $moduleId
        return
    }

    set moduleId [lindex $taskDesc4 1]

    # get entry point
    if {![set entry4 [testSymFind funct4]]} {
    	cleanUp c $taskId1 $taskId2 $taskId3 $taskId4 \
		b $bkpNb1 $bkpNb2 $bkpNb3 \
		o $moduleId
        return
        }

    set taskId4 [lindex $taskDesc4 0]

    # set a breakpoint on user task - at the beginning of the task
    set bkpNb4 [testEventpointAdd $Msg $entry4 $taskId4]
    if {$bkpNb4 == ""} {
    	cleanUp c $taskId1 $taskId2 $taskId3 $taskId4 \
		b $bkpNb1 $bkpNb2 $bkpNb3 \
		o $moduleId
        return
        }

 # FUNCT5 - Eventpoint 5
 #######################

    # create a new context
    set taskDesc5 [testTaskCreate $Msg objSampleWtxtclTest6.o funct5]
    if {$taskDesc5 == 1} {
    	cleanUp c $taskId1 $taskId2 $taskId3 $taskId4 \
		b $bkpNb1 $bkpNb2 $bkpNb3 $bkpNb4 \
		o $moduleId
        return
    }

    set moduleId [lindex $taskDesc5 1]

    # get entry point
    if {![set entry5 [testSymFind funct5]]} {
    	cleanUp c $taskId1 $taskId2 $taskId3 $taskId4 $taskId5 \
		b $bkpNb1 $bkpNb2 $bkpNb3 $bkpNb4 \
		o $moduleId
        return
        }

    set taskId5 [lindex $taskDesc5 0]

    # set a breakpoint on user task - at the beginning of the task
    set bkpNb5 [testEventpointAdd $Msg $entry5 $taskId5]
    if {$bkpNb5 == ""} {
    	cleanUp c $taskId1 $taskId2 $taskId3 $taskId4 $taskId5 \
		b $bkpNb1 $bkpNb2 $bkpNb3 $bkpNb4 \
		o $moduleId
        return
        }

 # FUNCT6 - Eventpoint 6
 #######################

    # create a new context
    set taskDesc6 [testTaskCreate $Msg objSampleWtxtclTest6.o funct6]
    if {$taskDesc6 == 1} {
    	cleanUp c $taskId1 $taskId2 $taskId3 $taskId4 $taskId5 \
		b $bkpNb1 $bkpNb2 $bkpNb3 $bkpNb4 $bkpNb5 \
		o $moduleId
        return
    }

    set moduleId [lindex $taskDesc6 1]

    # get entry point
    if {![set entry6 [testSymFind funct6]]} {
    	cleanUp c $taskId1 $taskId2 $taskId3 $taskId4 $taskId5 \
			$taskId6 \
		b $bkpNb1 $bkpNb2 $bkpNb3 $bkpNb4 $bkpNb5 \
		o $moduleId
        return
        }

    set taskId6 [lindex $taskDesc6 0]

    # set a breakpoint on user task - at the beginning of the task
    set bkpNb6 [testEventpointAdd $Msg $entry6 $taskId6]
    if {$bkpNb6 == ""} {
    	cleanUp c $taskId1 $taskId2 $taskId3 $taskId4 $taskId5 \
			$taskId6 \
		b $bkpNb1 $bkpNb2 $bkpNb3 $bkpNb4 $bkpNb5 \
		o $moduleId
        return
        }

 # FUNCT7 - Eventpoint 7
 #######################

    # create a new context
    set taskDesc7 [testTaskCreate $Msg objSampleWtxtclTest6.o funct7]
    if {$taskDesc7 == 1} {
    	cleanUp c $taskId1 $taskId2 $taskId3 $taskId4 $taskId5 \
			$taskId6 \
		b $bkpNb1 $bkpNb2 $bkpNb3 $bkpNb4 $bkpNb5 $bkpNb6 \
		o $moduleId
        return
    }

    set moduleId [lindex $taskDesc7 1]

    # get entry point
    if {![set entry7 [testSymFind funct7]]} {
    	cleanUp c $taskId1 $taskId2 $taskId3 $taskId4 $taskId5 \
			$taskId6 $taskId7 \
		b $bkpNb1 $bkpNb2 $bkpNb3 $bkpNb4 $bkpNb5 $bkpNb6 \
		o $moduleId
        return
        }

    set taskId7 [lindex $taskDesc7 0]

    # set a breakpoint on user task - at the beginning of the task
    set bkpNb7 [testEventpointAdd $Msg $entry7 $taskId7]
    if {$bkpNb7 == ""} {
    	cleanUp c $taskId1 $taskId2 $taskId3 $taskId4 $taskId5 \
			$taskId6 $taskId7 \
		b $bkpNb1 $bkpNb2 $bkpNb3 $bkpNb4 $bkpNb5 $bkpNb6 \
		o $moduleId
        return
        }

 # FUNCT8 - Eventpoint 8
 #######################

    # create a new context
    set taskDesc8 [testTaskCreate $Msg objSampleWtxtclTest6.o funct8]
    if {$taskDesc8 == 1} {
    	cleanUp c $taskId1 $taskId2 $taskId3 $taskId4 $taskId5 \
			$taskId6 $taskId7 \
		b $bkpNb1 $bkpNb2 $bkpNb3 $bkpNb4 $bkpNb5 \
			$bkpNb6 $bkpNb7 \
		o $moduleId
        return
    }

    set moduleId [lindex $taskDesc8 1]

    # get entry point
    if {![set entry8 [testSymFind funct8]]} {
    	cleanUp c $taskId1 $taskId2 $taskId3 $taskId4 $taskId5 \
			$taskId6 $taskId7 $taskId8 \
		b $bkpNb1 $bkpNb2 $bkpNb3 $bkpNb4 $bkpNb5 \
			$bkpNb6 $bkpNb7 \
		o $moduleId
        return
        }

    set taskId8 [lindex $taskDesc8 0]

    # set a breakpoint on user task - at the beginning of the task
    set bkpNb8 [testEventpointAdd $Msg $entry8 $taskId8]
    if {$bkpNb8 == ""} {
    	cleanUp c $taskId1 $taskId2 $taskId3 $taskId4 $taskId5 \
			$taskId6 $taskId7 $taskId8 \
		b $bkpNb1 $bkpNb2 $bkpNb3 $bkpNb4 $bkpNb5 \
			$bkpNb6 $bkpNb7 \
		o $moduleId
        return
        }

 # FUNCT9 - Eventpoint 9
 #######################

    # create a new context
    set taskDesc9 [testTaskCreate $Msg objSampleWtxtclTest6.o funct9]
    if {$taskDesc9 == 1} {
    	cleanUp c $taskId1 $taskId2 $taskId3 $taskId4 $taskId5 \
			$taskId6 $taskId7 $taskId8 \
		b $bkpNb1 $bkpNb2 $bkpNb3 $bkpNb4 $bkpNb5 \
			$bkpNb6 $bkpNb7 $bkpNb8 \
		o $moduleId
        return
    }

    set moduleId [lindex $taskDesc9 1]

    # get entry point
    if {![set entry9 [testSymFind funct9]]} {
    	cleanUp c $taskId1 $taskId2 $taskId3 $taskId4 $taskId5 \
			$taskId6 $taskId7 $taskId8 $taskId9 \
		b $bkpNb1 $bkpNb2 $bkpNb3 $bkpNb4 $bkpNb5 \
			$bkpNb6 $bkpNb7 $bkpNb8 \
		o $moduleId
        return
        }

    set taskId9 [lindex $taskDesc9 0]

    # set a breakpoint on user task - at the beginning of the task
    set bkpNb9 [testEventpointAdd $Msg $entry9 $taskId9]
    if {$bkpNb9 == ""} {
    	cleanUp c $taskId1 $taskId2 $taskId3 $taskId4 $taskId5\
			$taskId6 $taskId7 $taskId8 $taskId9 \
		b $bkpNb1 $bkpNb2 $bkpNb3 $bkpNb4 $bkpNb5 \
			$bkpNb6 $bkpNb7 $bkpNb8 \
		o $moduleId

        return
        }


    # Get the number of breakpoint
    set bkpCount1 [llength [wtxEventpointList]]

    # Delete one of the breakpoints (bkpNb5)
    set status [wtxEventpointDelete $bkpNb5]

    # Get the new number of breakpoint
    set bkpCount2 [llength [wtxEventpointList]]

    set bkpCount2 [expr $bkpCount2+1]

    # clean up
    cleanUp 	c $taskId1 $taskId2 $taskId3 $taskId4 $taskId5\
			$taskId6 $taskId7 $taskId8 $taskId9 \
		b $bkpNb1 $bkpNb2 $bkpNb3 $bkpNb4 $bkpNb6 $bkpNb7 \
			$bkpNb8 $bkpNb9 \
		o $moduleId

    if { $bkpCount1 != $bkpCount2 } {
            testFail $errEvptMsg(2)
            return
        }

    wtxTestPass
}

#*****************************************************************************
#
# wtxEventpointDeleteTest3 - wtxEventpointdelete with invalid eventpoint id
#
# Verify that wtxEventpointdelete returns INVALID_EVENTPOINT when an
# invalid eventpoint id is given to wtxEventpointDelete.
#

proc wtxEventpointDeleteTest3 {Msg} {

    global errEvptMsg

    puts stdout "$Msg" nonewline

    # Try to delete an invalid eventpoint
    # if no exception occurred
    if {![catch "wtxEventpointDelete 1234567890" errCode]} {
	testFail $errEvptMsg
	return
    }

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $errCode SVR_INVALID_EVENTPOINT]} {
	testFail $errCode
	return
    }

    # if the caught exception is the expected error code
    wtxTestPass
}


#*****************************************************************************
#
# wtxEventpointDeleteTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxEventpointDeleteTest {tgtsvrName timeout invalidAddr} {

    global vEvptMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxEventpointDeleteTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # call the test procedures
    wtxEventpointDeleteTest1 $vEvptMsg(2)
    wtxEventpointDeleteTest2 $vEvptMsg(4)

    wtxErrorHandler [lindex [wtxHandle] 0] ""
    wtxEventpointDeleteTest3 $vEvptMsg(5)

    # deconnection
    wtxToolDetachCall "wtxEventpointDeleteTest"
}
