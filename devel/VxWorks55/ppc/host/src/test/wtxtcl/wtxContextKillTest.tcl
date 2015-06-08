# wtxContextKillTest.tcl - Tcl script, test WTX_CONTEXT_KILL
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01q,14may98,p_b  Replace test?.o by objSampleWtxtclTest?.o
# 01p,28jan98,p_b  Adapted for WTX 2.0
# 01o,30nov95,jmp  replaced taskInfoGetAll call by taskInfoGet.
# 01n,20oct95,jmp  added invalidAddr argument to wtxContextKillTest.
# 01m,22aug95,jmp  removed useless wtxMemFree.
#		   added timeout to wtxToolAttachCall in wtxContextKillTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01l,23jun95,f_v  added wtxTestRun call
# 01k,19jun95,f_v  changed testTaskCreate call
# 01j,31may95,f_v  changed few error messages
# 01i,27apr95,c_s  now uses wtxErrorMatch.
# 01h,20apr95,f_v  use generic procedures
# 01g,07apr95,f_v  added agentErr test, quit test more properly
# 01f,30mar95,f_v  changed pomme to test1
# 01e,20mar95,f_v  changed printLogo by a user task in wtxContextKillTest1
# 01c,09mar95,f_v  delete CtxMsg messages
# 01b,26Jan95,f_v  change access messages, use wtxToolDetachCall
# 01a,16Jan95,f_v  written. 
#
#

#
# DESCRIPTION
#
# WTX_CONTEXT_KILL  -  WTX destroy context service
# This service destroys a context on the remote target. 
#
# (1) wtxContextKillTest1 - test wtxContextKill with right argument
#

#*****************************************************************************
#
# wtxContextKillTest1 -
#
# Verify if wtxContextKill has killed the create context 
#

proc wtxContextKillTest1 {Msg} {

    global errCtxMsg

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
    set moduleId [lindex $taskDesc 1]

    # get task information
    set taskInfo [taskInfoGet $taskId]
    if {$taskInfo == ""} {
	return
    }

    set taskName [lindex $taskInfo 10]

    # kill the created context
    set resKill [wtxContextKill CONTEXT_TASK $taskId]
    if {$resKill == ""} {
	cleanUp o $moduleId
	return
    }

    # Verify that the context has been killed
    # taskNameToId must return 0 if task doesn't exist
    set id [taskNameToId $taskName]

    if {$id != 0} {
	testFail $errCtxMsg(2)
	cleanUp o $moduleId
	return
    }

    # cleanUp
    cleanUp o $moduleId

    wtxTestPass
}


#*****************************************************************************
#
# wtxContextKillTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxContextKillTest {tgtsvrName timeout invalidAddr} {

    global vCtxMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxContextKillTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # call the test procedures
    wtxContextKillTest1  $vCtxMsg(2)

    # deconnection
    wtxToolDetachCall "wtxContextKillTest"
}
