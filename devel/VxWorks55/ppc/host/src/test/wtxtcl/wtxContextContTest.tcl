# wtxContextContTest.tcl - Tcl script, test WTX_CONTEXT_CONT
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01t,20nov01,jmp  fixed wtxContextContTest1{} to wait for task completion
#                  before testing its return value.
# 01s,14may98,p_b  Replace test?.o by objSampleWtxtclTest?.o
# 01r,28jan98,p_b  Adapted for WTX 2.0
# 01q,25jul96,jmp  removed a test case since wtxContextCont no longer returned
#		   AGENT_AGENT_MODE_ERROR if attempting to continue context
#		   system in task mode.
# 01p,20oct95,jmp  added invalidAddr argument to wtxContextContTest.
# 01o,12sep95,jmp  fixed a syntax error in call of testTaskCreate.
# 01n,22aug95,jmp  replaced sleep time by a timeout mechanism.
#		   changed test implementation, now the test doesn't set
#		   breakpoints on system calls (like printf in previous 
#		   implementation), now the test verifies if called functions
#		   return correct values. Now uses cleanUp procedure.
#		   added timeout to wtxToolAttachCall in wtxContextContTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01m,27jul95,p_m  changed sleep time as an attempt to fix SPR# 4551.
#                  changed address of second breakpoint since it was
#                  out of the main routine on 386. This is a temporary fix
#                  the test should be implemented differently.
# 01l,23jun95,f_v  added wtxTestRun call
# 01k,19jun95,f_v  changed testTaskCreate call
# 01j,23may95,f_v  change error message in wtxContextContTest2
# 01i,28apr95,c_s  now uses wtxErrorMatch.
# 01h,20apr95,f_v  use generic procedures 
# 01g,04apr95,f_v  added wtxContextContTest2, quit test more properly
# 01f,30mar95,f_v  changed pomme to test1
# 01c,28mar95,f_v  added wtxRegisterForEvent
# 01e,20mar95,f_v  changed printLogo by a user task in wtxContextContTest1
# 01d,20feb95,f_v  replace wtxEventGet by wtxEventPoll
# 01c,07feb95,f_v  add event managment, add warning messages
#		   replace wtxEventPoll by wtxEventGet
# 01b,26Jan95,f_v  change access messages, use wtxToolDetachCall
# 01a,16Jan95,f_v  written. 
#
#

#
# DESCRIPTION
#
# WTX_CONTEXT_CONT  - WTX continue context service
#   This service continues execution of a context on the remote Target.  The
#   context to be continue is specified by taskId.
#
#
#

#*****************************************************************************
#
# wtxContextContTest1 -
#
# Verify if wtxContextCont continues correctly execution of taskId context
#
# This test loads objSampleWtxtclTest1.o, objSampleWtxtclTest2.o and objSampleWtxtclTest3.o, it sets breakpoints on the
# entry point of test1 and on function test3 called by test1, the test verifies
# if wtxContextCont return correct values and tests if test2 and test3 return
# rights values.

proc wtxContextContTest1 {Msg} {

    global errCtxMsg
    global errMsg

    puts stdout "$Msg" nonewline

    # create new contexts
    set taskDesc2 [testTaskCreate $Msg objSampleWtxtclTest2.o test2]
    if {$taskDesc2 == 1} {
	return
    }

    set moduleId2 [lindex $taskDesc2 1]

    set taskDesc3 [testTaskCreate $Msg objSampleWtxtclTest3.o test3]
    if {$taskDesc3 == 1} {
	return
    }
    set moduleId3 [lindex $taskDesc3 1]

    set taskDesc1 [testTaskCreate $Msg objSampleWtxtclTest1.o test1]
    if {$taskDesc1 == 1} {
	return
    }

    set moduleId1 [lindex $taskDesc1 1]

    # get entry point
    if {![set entry [testSymFind test1]]} {
	cleanUp t $taskDesc1 $taskDesc2 $taskDesc3
	return
    }

    # get task Id
    set taskId [lindex $taskDesc1 0]
    # get test3 address
    if {![set test3Addr [testSymFind test3]]} {
	cleanUp t $taskDesc1 $taskDesc2 $taskDesc3
	return
	}

    # set a breakpoint on user task - at the beginning of the task 
    set bkpNb1 [testEventpointAdd $Msg $entry $taskId]
    if {$bkpNb1 == ""} {
	return
	}

    # set a breakpoint on user task at test3
    set bkpNb2 [testEventpointAdd $Msg $test3Addr $taskId]
    if {$bkpNb2 == ""} {
        return
        }

    # activate user task - taskActivate
    set resSt [wtxContextResume CONTEXT_TASK $taskId]
    if {$resSt == ""} {
	return
	}

    # wait for event TEXT_ACCESS , if the system get MAX_EVENT events the
    # test fails 
    set MAX_EVENT 1000
    set i 0

    set bpStatus1 [wtxEventPoll 100]
    while {([string compare [lindex $bpStatus1 0] TEXT_ACCESS] &&\
            [string compare [lindex $bpStatus1 1] $taskId])} {
        set bpStatus1 [wtxEventPoll 100]
        set i [expr $i + 1]
        if { $i >= $MAX_EVENT }  {
              testFail $errMsg(3)
	      cleanUp b $bkpNb1 $bkpNb2 t $taskDesc1 $taskDesc2 $taskDesc3
              return
          }
      }

    # continue user task
    set contSt [wtxContextCont CONTEXT_TASK $taskId]
    if {$contSt == ""} {
	cleanUp b $bkpNb1 $bkpNb2 t $taskDesc1 $taskDesc2 $taskDesc3
	return
	}

    # wait for event TEXT_ACCESS , if the system get MAX_EVENT events the
    # test fails 
    set MAX_EVENT 1000
    set i 0

    set bpStatus2 [wtxEventPoll 100]
    while {([string compare [lindex $bpStatus2 0] TEXT_ACCESS] &&\
            [string compare [lindex $bpStatus2 1] $taskId])} {
        set bpStatus2 [wtxEventPoll 100]
        set i [expr $i + 1]
        if { $i >= $MAX_EVENT }  {
              testFail $errMsg(3)
	      cleanUp b $bkpNb1 $bkpNb2 t $taskDesc1 $taskDesc2 $taskDesc3
              return
          }
      }

    # need notification of context exit

    wtxEventpointAdd WTX_EVENT_CTX_EXIT 0 CONTEXT_TASK $taskId \
    	ACTION_NOTIFY 0 0 0

    # continue user task

    set contSt [wtxContextCont CONTEXT_TASK $taskId]
    if {$contSt == ""} {
	testFail $errCtxMsg(10)
	cleanUp b $bkpNb1 $bkpNb2 t $taskDesc1 $taskDesc2 $taskDesc3
	return
	}

    # wait for task completion before checking return value

    set i 0
    set event [wtxEventPoll 100]
    while {([string compare [lindex $event 0] CTX_EXIT] && \
	[string compare [lindex $event 2] $taskId])} {
	set event [wtxEventPoll 100]
	set i [expr $i + 1]
	if { $i >= $MAX_EVENT }  {
	    testFail $errMsg(3)
	    cleanUp b $bkpNb1 $bkpNb2 t $taskDesc1 $taskDesc2 $taskDesc3
	    return
	}
    }

    # Verifie that test1 works correctly :
    # resTest2 is the result of test2 call, it should be equal to 10
    # resTest3 is the result of test3 call, it should be equal to 5
    set resulAddr [lindex [wtxSymFind -name "resTest2"] 1]
    set result [wtxGopherEval -errvar errCode "$resulAddr *!"]
    if {$result != 10} {
        testFail $errCtxMsg(16)
	cleanUp b $bkpNb1 $bkpNb2 o $moduleId1 t $taskDesc2 $taskDesc3
        return
        }

    set resulAddr [lindex [wtxSymFind -name "resTest3"] 1]
    set result [wtxGopherEval -errvar errCode "$resulAddr *!"]
    if {$result != 5} {
        testFail $errCtxMsg(16)
	cleanUp b $bkpNb1 $bkpNb2 o $moduleId1 t $taskDesc2 $taskDesc3
        return
        }

    # clean up
    cleanUp b $bkpNb1 $bkpNb2 o $moduleId1 t $taskDesc2 $taskDesc3
    wtxTestPass
}


#*****************************************************************************
#
# wtxContextContTest2 -
#
# Verify if wtxContextCont manages properly bad address and event like
# parameters
#

proc wtxContextContTest2 {Msg} {

    global errCtxMsg
    global errMsg

    puts stdout "$Msg" nonewline

    # create new contexts
    set taskDesc2 [testTaskCreate $Msg objSampleWtxtclTest2.o test2]
    if {$taskDesc2 == 1} {
        return
    }

    set taskDesc3 [testTaskCreate $Msg objSampleWtxtclTest3.o test3]
    if {$taskDesc3 == 1} {
        return
    }

    set taskDesc1 [testTaskCreate $Msg objSampleWtxtclTest1.o test1]
    if {$taskDesc1 == 1} {
        return
    }

    # get entry point
    if {![set entry [testSymFind test1]]} {
	cleanUp t $taskDesc1 $taskDesc2 $taskDesc3
	return
	}

    # get task Id
    set taskId [lindex $taskDesc1 0]

    # get test3 address
    if {![set test3Addr [testSymFind test3]]} {
	cleanUp t $taskDesc1 $taskDesc2 $taskDesc3
	return
	}

    # set a breakpoint on user task - at the beginning of the task
    catch {testEventpointAdd $Msg $entry $taskId} bkpNb1
    if {$bkpNb1 == ""} {
	return
	}

    # set a breakpoint on user task at test3
    catch {testEventpointAdd $Msg $test3Addr $taskId} bkpNb2
    if {$bkpNb2 == ""} {
        return
        }

    # activate user task - taskActivate
    catch "wtxContextResume CONTEXT_TASK $taskId" resSt


    # wait for event TEXT_ACCESS , if the system get MAX_EVENT events the
    # test fails 
    set MAX_EVENT 1000
    set i 0

    catch "wtxEventPoll 100" bpStatus1
    while {([string compare [lindex $bpStatus1 0] TEXT_ACCESS] &&\
            [string compare [lindex $bpStatus1 1] $taskId])} {
        catch "wtxEventPoll 100" bpStatus1
        set i [expr $i + 1]
        if { $i >= $MAX_EVENT }  {
              testFail $errMsg(3)
    	      cleanUp b $bkpNb1 $bkpNb2 t $taskDesc1 $taskDesc2 $taskDesc3
              return
          }
      }

    # continue task with a bad address
    # if no exception occurred
    if {![catch "wtxContextCont CONTEXT_TASK 0" contSt]} {
	testFail $errCtxMsg(10)
	cleanUp b $bkpNb1 $bkpNb2 t $taskDesc1 $taskDesc2 $taskDesc3
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $contSt AGENT_INVALID_CONTEXT]} {
	testFail $contSt
	cleanUp b $bkpNb1 $bkpNb2 t $taskDesc1 $taskDesc2 $taskDesc3
	return
	}

    # clean up
    cleanUp b $bkpNb1 $bkpNb2 t $taskDesc1 $taskDesc2 $taskDesc3

    wtxTestPass
}

#*****************************************************************************
#
# wtxContextContTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxContextContTest {tgtsvrName timeout invalidAddr} {

    global vCtxMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxContextContTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    catch {wtxRegisterForEvent .*} st
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    wtxContextContTest1  $vCtxMsg(0)

    wtxErrorHandler [lindex [wtxHandle] 0] ""

    # call tests with invalid parameters
    wtxContextContTest2  $vCtxMsg(6)

    # deconnection
    wtxToolDetachCall "wtxContextContTest"
}
