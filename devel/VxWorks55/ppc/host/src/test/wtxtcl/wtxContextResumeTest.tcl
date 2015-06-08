# wtxContextResumeTest.tcl - Tcl script, test WTX_CONTEXT_RESUME
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01l,27apr99,p_b Added "after 1000" command (Vincent Hue's fix for SPR26854).
# 01k,28jan98,p_b Adapted for WTX 2.0
# 01j,30nov95,jmp replaced taskInfoGetAll call by taskInfoGet.
# 01i,20oct95,jmp added invalidAddr argument to wtxContextResumeTest.
# 01h,22aug95,jmp correct loss of memory problem,
#		  use wtxContextCreate instead of wtxFuncCall.
#		  added timeout to wtxToolAttachCall in wtxContextResumeTest.
#                 changed tests control method, now uses wtxErrorHandler.
# 01g,23jun95,f_v added wtxTestRun call
# 01f,27apr95,c_s now uses wtxErrorMatch.
# 01e,20apr95,f_v clean up main procedure
# 01d,07apr95,f_v uses Id rather than task name for lsearch
#		  added wtxContextResumeTest2
# 01c,09mar95,f_v delete CtxMsg messages
# 01b,26Jan95,f_v change access messages, use wtxToolDetachCall. 
# 01a,20Jan95,f_v written. 
#

#
# DESCRIPTION
#
# WTX_CONTEXT_RESUME  - WTX resume context service
#
# (1) wtxContextResumeTest1 - test wtxContextResume with right arguments
#
# (2) wtxContextResumeTest2 - test wtxContextResume with a wrong context type
#
#
#

#*****************************************************************************
#
# wtxContextResumeTest1 -
#
# Verify if wtxContextResume has resume the task CreateTest 
#

proc wtxContextResumeTest1 {Msg} {

    global errCtxMsg

    puts stdout "$Msg" nonewline

    # initialisation of variables for wtxContextCreate
	set name Test[pid]
	set priority 100
	set options 0
	set stack 0
	set stackSize 0
	set redirIn 0
	set redirOut 0
	set delay 10000			    
   
    # initialise taskState to 4 - DELAY -
    set taskState 4

    # Create tCallTask and suspend it
    set sym taskDelay
    set symAdrs [lindex [wtxSymFind -name "$sym"] 1]

    # create a new context
    set tId [wtxContextCreate CONTEXT_TASK $name $priority $options\
	$stack $stackSize $symAdrs $redirIn $redirOut $delay]
    if {$tId == ""} {
	return
	}

    set resuSt [wtxContextResume CONTEXT_TASK $tId]
    if {$resuSt == ""} {
           return
         }

    after 1000

    # get the created task information
    set taskInfo [taskInfoGet $tId]

    # get the state of the task
    set idState [lindex $taskInfo 2]

    # Verify that the state of the task is correct
    if {$idState != $taskState} {
	testFail $errCtxMsg(4)
	return
    }

    set resKill [wtxContextKill CONTEXT_TASK $tId]
    if {$resKill == ""} {
	return
    }

    wtxTestPass
}


#*****************************************************************************
#
# wtxContextResumeTest2 -
#
# Verify if wtxContextResume manage bad context type like parameter
#

proc wtxContextResumeTest2 {Msg} {

    global errMsg

    puts stdout "$Msg" nonewline


    # initialisation of variables for wtxContextCreate
    set priority 100
    set options 0
    set stack 0
    set stackSize 0
    set redirIn 0
    set redirOut 0
    set delay 10000
					    
					    
    # Create tCallTask and suspend it
    set sym taskDelay
    set symAdrs [lindex [wtxSymFind -name "$sym"] 1]

    # create a new context
    if {[catch {wtxContextCreate CONTEXT_TASK "tTest" $priority\
	$options $stack $stackSize $symAdrs $redirIn\
	$redirOut $delay} tId]} {

	testFail $tId
	return
	}

    if {[catch "wtxContextCont CONTEXT_TASK $tId" resSt]} {
	testFail $resSt
	return
	}

    if {[catch "wtxContextSuspend CONTEXT_TASK $tId" susSt]} {
	testFail $susSt
	return
	}

    # Resume the task CreateTest

    # if no exception occurred
    if {![catch "wtxContextResume 0 $tId" resuSt]} {
	testFail $errMsg(1)
	wtxContextKill CONTEXT_TASK $tId
	return
	}

    after 1000

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $resuSt AGENT_INVALID_CONTEXT]} {
	testFail $resuSt
	wtxContextKill CONTEXT_TASK $tId
	return
	}

    # clean up
	wtxContextKill CONTEXT_TASK $tId

    # if the caught exception is the expected error code
    wtxTestPass
}

#*****************************************************************************
#
# wtxContextResumeTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxContextResumeTest {tgtsvrName timeout invalidAddr} {

    global vCtxMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxContextResumeTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # call tests with valid parameters
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler
    wtxContextResumeTest1 $vCtxMsg(3)

    # call tests with invalid parameters
    wtxErrorHandler [lindex [wtxHandle] 0] ""
    wtxContextResumeTest2 $vCtxMsg(7)

    # deconnection
    wtxToolDetachCall "wtxContextResumeTest"
}
