# wtxContextStepTest.tcl - Tcl script, test WTX_CONTEXT_STEP
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01s,14may98,p_b  Replace test?.o by objSampleWtxtclTest?.o
# 01r,28jan98,p_b  Adapted for WTX 2.0
# 01q,22may96,jmp  modified wtxContextStepTest2 to set breakpoint on
#		   an existing function instead of using an address
#		   that may point to the middle of an instruction.
# 01p,30nov95,jmp  replaced taskInfoGetAll call by taskInfoGet.
# 01o,27nov95,p_m  used objSampleWtxtclTest3.o instead of objSampleWtxtclTest1.o in wtxContextStepTest1
#                  to remove unresolved references (SPR# 5542).
# 01n,20oct95,jmp  added invalidAddr argument to wtxContextStepTest.
# 01m,06sep95,jmp  used objSampleWtxtclTest3.o instead of objSampleWtxtclTest4.o in wtxContextStepTest2
#	 	   to avoid external references in object module.
# 01l,22aug95,jmp  added timeout to wtxToolAttachCall in wtxGopherEvalTest.
#                  changed test implementation, now the test doesn't set
#                  breakpoints on system calls (like printf in previous
#                  implementation), now the test verifies if called functions
#                  return correct values.
#                  changed tests control method, now uses wtxErrorHandler.
# 01k,23jun95,f_v  added wtxTestRun call
# 01j,19jun95,f_v  changed testTaskCreate call
# 01i,24may95,f_v  changed error message in wtxContextStepTest3
# 01h,16may95,f_v  remove extended tcl functions
# 01g,03may95,f_v  added wtxContextStepTest3
# 01f,19apr95,f_v  added wtxContextStepTest2
#		   remove variable in main procedure
#		   use the generic procedure testEventpointAdd
# 01e,30mar95,f_v  changed pomme to test1,quit test more properly
#		   use testTaskCreate, and testTaskCleanup
# 01d,20mar95,f_v  changed printLogo by a user task in wtxContextStepTest1
# 01c,09mar95,f_v  delete CtxMsg messages
#		   replace BREAKPOINT by TEXT_ACCESS
# 01b,26Jan95,f_v  change access messages, use wtxToolDetachCall
# 01a,25Jan95,f_v  written. 
#
#

#
# DESCRIPTION
#
# WTX_CONTEXT_STEP  - WTX steps context service
#   This service steps execution of a context on the remote Target.
#   The context to be steped is specified by contextId.
#
#
#

#*****************************************************************************
#
# wtxContextStepTest1 -
#
# Verify if wtxContextStep steps instruction by instruction
# execution of CreateTest context 
#

proc wtxContextStepTest1 {Msg} {

    global errCtxMsg

    puts stdout "$Msg" nonewline

    # initialisations
    set startAddr 0
    set endAddr 0

    # create a new context
    set taskDesc [testTaskCreate $Msg objSampleWtxtclTest3.o test3]
    if {$taskDesc == 1} {
	return
    }

    # get entry point
    if {![set entry [testSymFind test3]]} {
	cleanUp t $taskDesc
	return
	}

    set taskId [lindex $taskDesc 0]
       
    # set a breakpoint on user task - at the beginning of the task 
    set bkpNb [testEventpointAdd $Msg $entry $taskId]
    if {$bkpNb == ""} {
	cleanUp t $taskDesc
	return
	}

    # activate user task - taskActivate
    set contSt [wtxContextCont CONTEXT_TASK $taskId]
    if {$contSt == ""} {
	cleanUp b $bkpNb t $taskDesc
	return
	}


    after 1000
    # step user task
    set stepSt [wtxContextStep CONTEXT_TASK $taskId $startAddr $endAddr]
    if {$stepSt == ""} {
	cleanUp b $bkpNb t $taskDesc
	return
	}

    after 1000

    # Get task information
    set taskInfo [taskInfoGet $taskId]

    # Get PC - Program Counter of the task
    set tPC1 [lindex $taskInfo 4]

    set stepSt [wtxContextStep CONTEXT_TASK $taskId $startAddr $endAddr]
    if {$stepSt == ""} {
	cleanUp b $bkpNb t $taskDesc
	return
	}
    
    after 1000

    # Get task information
    set taskInfo [taskInfoGet $taskId]

    # Get PC - Program Counter of the task
    set tPC2 [lindex $taskInfo 4]

    # Clean up
    cleanUp b $bkpNb t $taskDesc

    if {! ($tPC2 > $tPC1 && $tPC1 > $entry)} { 
	testFail $errCtxMsg(6)
	return
    }

    wtxTestPass
}



#*****************************************************************************
#
# wtxContextStepTest2 -
#
# Verify if wtxContextStep steps multiple instructions
#
#

proc wtxContextStepTest2 {Msg} {

    global errCtxMsg

    puts stdout "$Msg" nonewline

    # initialisations
    set startAddr 0
    set endAddr 0

    # create a new context
    set taskDesc [testTaskCreate $Msg objSampleWtxtclTest3.o test3]
    if {$taskDesc == 1} {
	return
    }
    
    # get entry point
    if {![set entry [testSymFind test3]]} {
	cleanUp t $taskDesc 
	return
	}

    set taskId [lindex $taskDesc 0]
       
    # set a breakpoint on user task - at the beginning of the task 
    set bkpNb [testEventpointAdd $Msg $entry $taskId]
    if {$bkpNb == ""} {
	cleanUp t $taskDesc 
        return
        }

    # activate user task - taskActivate
    set resSt [wtxContextResume CONTEXT_TASK $taskId]
    if {$resSt == ""} {
	cleanUp b $bkpNb t $taskDesc 
	return
	}

    after 1000
    # step user task
    set stepSt [wtxContextStep CONTEXT_TASK $taskId $startAddr $endAddr]
    if {$stepSt == ""} {
	cleanUp b $bkpNb t $taskDesc 
	return
	}
    
    after 1000

    # Get task information 
    set taskInfo [taskInfoGet $taskId]

    # Get PC - Program Counter of the task
    set tPC1 [lindex $taskInfo 4]

    # step multiples instructions
    set endAddr [lindex [wtxSymFind -name  malloc] 1]

    set stepSt [wtxContextStep CONTEXT_TASK $taskId $startAddr $endAddr]
    if {$stepSt == ""} {
	cleanUp b $bkpNb t $taskDesc 
	return
	}
    
    after 1000

    # Get task information
    set taskInfo [taskInfoGet $taskId]

    # Get PC - Program Counter of the task
    set tPC2 [lindex $taskInfo 4]

    # Clean up
    cleanUp b $bkpNb t $taskDesc 

    if {! ($tPC2 > $tPC1 && $tPC1 > $entry)} { 
	testFail $errCtxMsg(6)
	return
    }

    wtxTestPass
}

#*****************************************************************************
#
# wtxContextStepTest3 -
#
# Verify if wtxContextStep manage bad parameters 
# 
#

proc wtxContextStepTest3 {Msg} {

    global errCtxMsg

    puts stdout "$Msg" nonewline

    # initialisations
    set startAddr 0
    set endAddr 0

    # load objSampleWtxtclTest1.o and create a new context
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
	cleanUp t $taskDesc
        return
        }

    # activate user task - taskActivate
    if {[catch "wtxContextResume CONTEXT_TASK $taskId" resSt]} {
	testFail $resSt
	cleanUp b $bkpNb t $taskDesc
	return
	}

    after 1000

    # step user task

    # if no exception occurred
    if {![catch "wtxContextStep 0 $taskId $startAddr $endAddr" stepSt]} {
	testFail $errCtxMsg(14)
	return
	}
    
    # Clean up
    cleanUp b $bkpNb t $taskDesc

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $stepSt AGENT_AGENT_MODE_ERROR]} {
	testFail $stepSt
	return
    }

    # if the caught exception is the expected error code
    wtxTestPass
}


#*****************************************************************************
#
# wtxContextStepTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxContextStepTest {tgtsvrName timeout invalidAddr} {

    global vCtxMsg 

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxContextStepTest targetServerName\
			timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # call tests with valid parameters
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    wtxContextStepTest1  $vCtxMsg(4)
    wtxContextStepTest2  $vCtxMsg(8)

    # call tests with invalid parameters
    wtxErrorHandler [lindex [wtxHandle] 0] ""
    wtxContextStepTest3  $vCtxMsg(9)

    # deconnection
    wtxToolDetachCall "wtxContextStepTest"
}
