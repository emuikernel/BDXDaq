# wtxContextSuspendTest.tcl - Tcl script, test WTX_CONTEXT_SUSPEND
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01k,28jan98,p_b  Adapted for WTX 2.0
# 01j,30nov95,jmp  replaced taskInfoGetAll call by taskInfoGet.
# 01i,20oct95,jmp  added invalidAddr argument to wtxContextSuspendTest.
# 01h,22aug95,jmp  correct loss of memory problem,
#                  use wtxContextCreate instead of wtxFuncCall.
#		   added timeout to wtxToolAttachCall in wtxContextSuspendTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01g,23jun95,f_v  added wtxTestRun call
# 01f,22may95,f_v  replaced tCallTask by task Id
# 01e,28apr95,f_v  added wtxContextSuspendTest2
# 01d,20mar95,f_v  clean up main procedure
# 01c,09mar95,f_v  delete CtxMsg messages
# 01b,26Jan95,f_v  change access messages, use wtxToolDetachCall
# 01a,25Jan95,f_v  written. 
#
#

#
# DESCRIPTION
#
# WTX_CONTEXT_SUSPEND  - WTX suspend context service
#
# (1) wtxContextSuspendTest1 - test wtxContextSuspend with right parameters
#
# (2) wtxContextSuspendTest2 - test wtxContextSuspend with bad context type
#
#
#

#*****************************************************************************
#
# wtxContextSuspendTest1 -
#
# Verify if wtxContextSuspend has suspended the CreateTest Task 
#

proc wtxContextSuspendTest1 {Msg} {

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
        set delay 1000

    # Create a task and spawn it 
    set sym taskDelay
    set symAdrs [lindex [wtxSymFind -name "$sym"] 1]

    set tId [wtxContextCreate CONTEXT_TASK $name $priority $options\
	$stack $stackSize $symAdrs $redirIn $redirOut $delay]
    if {$tId == ""} {
	return
    }

    set stCont [wtxContextCont CONTEXT_TASK $tId]
    if {$stCont == ""} {
	return
    }

    # Suspend the task tCallTask 
    set stSus [wtxContextSuspend CONTEXT_TASK $tId]
    if {$stSus == ""} {
         return
    }

    # Get task informations
    catch {set taskInfo [taskInfoGet $tId]}

    # Get the state of the task
    catch {set taskState [lindex $taskInfo 2]}

    # verify that the task has been suspended
    if { $taskState != 1 && $taskState != 3 && $taskState != 5 \
	&& $taskState != 7} {
	   testFail $errCtxMsg(8)
	   return
    }

    # Kill the suspended task
    wtxContextKill CONTEXT_TASK $tId

    wtxTestPass
}



#*****************************************************************************
#
# wtxContextSuspendTest2 -
#
# Verify if wtxContextSuspend manage bad parameters
#

proc wtxContextSuspendTest2 {Msg} {

    global errCtxMsg
    global agentErrMsg

    puts stdout "$Msg" nonewline

    # initialisation of variables for wtxContextCreate
    set priority 100
    set options 0
    set stack 0
    set stackSize 0
    set redirIn 0
    set redirOut 0
    set delay 1000

    # Create a task and spawn it 
    set sym taskDelay
    set symAdrs [lindex [wtxSymFind -name "$sym"] 1]
    
    if {[catch {wtxContextCreate CONTEXT_TASK "tTest" $priority\
        $options $stack $stackSize $symAdrs $redirIn\
        $redirOut $delay} tId]} {

	testFail $tId
	return
	}
 
    # Suspend the task tCallTask 

    # if no exception occurred
    if {![catch "wtxContextSuspend 0 $tId" resuSt]} {
	testFail $agentErrMsg(10)
	wtxContextKill CONTEXT_TASK $tId
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $resuSt AGENT_INVALID_CONTEXT]} {
       testFail $resuSt
       wtxContextKill CONTEXT_TASK $tId
       return
     }

    # Kill the suspended task
    wtxContextKill CONTEXT_TASK $tId

    wtxTestPass
}


#*****************************************************************************
#
# wtxContextSuspendTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxContextSuspendTest {tgtsvrName timeout invalidAddr} {

    global vCtxMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxContextSuspendTest targetServerName"
        return
    }

    # variables initializations
    set serviceType 2

    # call tests with valid parameters
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler
    wtxContextSuspendTest1  $vCtxMsg(5)

    # call tests with invalid parameters
    wtxErrorHandler [lindex [wtxHandle] 0] ""
    wtxContextSuspendTest2  $vCtxMsg(9)

    # deconnection
    wtxToolDetachCall "wtxContextSuspendTest"
}
