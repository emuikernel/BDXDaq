# wtxEventGetTest.tcl - Tcl script, test WTX_EVENT_GET
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01w,05mar02,fle  made it exec on its own executable name
# 01v,14may98,p_b  Replace test?.o by objSampleWtxtclTest?.o
# 01u,28jan98,p_b  added tclPathGet & initEventsFile.
# 01t,02nov95,jmp  temporary removed wtxEventGetTest3.
# 01s,20oct95,jmp  added wtxEventGetTest3.
#		   added invalidAddr argument to wtxEventGetTest.
# 01r,12sep95.jmp  added cleanUp call.
# 01q,22aug95,jmp  changed objSampleWtxtclTest1.o entry point, now uses cleanUp.
#		   added timeout to wtxToolAttachCall in wtxEventGetTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01p,09jul95,jcf  resolved permission problem.
# 01o,23jun95,f_v  added wtxTestRun call
# 01n,19jun95,f_v  changed testTaskCreate call
# 01m,22may95,jcf  name revision.
# 01l,17may95,f_v  runs genEvent in the foreground but redirect stderr
# 01k,16may95,f_v  runs genEvent in the background
# 01j,15may95,c_s  now runs genEvent in the foreground, so that it will
#                    be synchronous with the test.
# 01i,04may95,f_v  replace BREAKPOINT by TEXT_ACCESS
# 01h,20apr95,f_v  use generic procedure
# 01g,10apr95,f_v  quit test more properly
# 01f,28mar95,f_v  added wtxRegisterForEvent, added clean up section
# 		   changed pomme to test1
# 01e,20mar95,f_v  changed printLogo by a user task in wtxEventGetTest1
# 01d,14mar95,f_v  changed file path
# 01c,27jan95,f_v  changed access messages, use wtxToolDetachCall
# 01b,25jan95,f_v  renamed wtxEventGetTest.tcl
# 01a,10jan95,f_v  written. 
#
#

#
# DESCRIPTION
#
# WTX_EVENT_GET  - WTX get a event
#
#
#

#*****************************************************************************
#
# wtxEventGetTest1 -
#
# Verify if wtxEventGet get a event breakpoint in the taskId Task 
#

proc wtxEventGetTest1 {Msg} {

    global EvMsg
    global errEvMsg
    global errMemMsg

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
    set bkPt [testEventpointAdd $Msg $entry $taskId]
    if {$bkPt == ""} {
	return
	}

    set resSt [wtxContextResume CONTEXT_TASK $taskId]
    if {$resSt == ""} {
	return
	}

    after 1000

    # get infomation about bkp
    set info [wtxEventGet]
    while {([string compare [lindex $info 0] TEXT_ACCESS] && \
            [string compare [lindex $info 1] $taskId])} {
        set info [wtxEventGet]
      }

    set event [lindex $info 0]

    # get the tId so tId = $taskId
    set contextId [lindex $info 1]

    # clean up
    cleanUp b $bkPt t $taskDesc 
    
    if { ("$event" != {TEXT_ACCESS}) || ($contextId != $taskId)} {
	testFail $errEvMsg(0)
	return
        }


    wtxTestPass
}


#*****************************************************************************
#
# wtxEventGetTest2 -
#
# Verify if wtxEventGet get event 
#

proc wtxEventGetTest2 {name Msg} {

    global env
    global EvMsg
    global errEvMsg

    puts stdout "$Msg" nonewline

    set err 0
    set nameFile [tclPathGet]genEvent2.tcl

    # verify if genEvent2.tcl exists in right directory
    set nameTmpFile [initEventsFile $nameFile "wtxEventGetTest2" $name]

    # run an other wtxtcl, which run genEvent2 script
    # the genEvent2 script attach to target server and then detach
    
    exec [wtxtclPathGet] $nameTmpFile ;# 2> /dev/null 

    # loop until EVENTTEST arrives
    set eventInfo [wtxEventGet]
    while { [string compare [lindex $eventInfo 0] EVENTTEST] } {
	set eventInfo [wtxEventGet]
	if { $eventInfo == "" } {
	   set err 1
	   break
	   }
	}

   # clean up
   file delete $nameTmpFile
   
   if { $err != 0 } { 
	    testFail $errEvMsg(6)
	    return
        }

    wtxTestPass
}


#*****************************************************************************
#
# wtxEventGetTest3 - Verfy that wtxEventPool get EXCEPTION event.
#
# This test generate a divide by zero Exception and verify that
# wtxEventPool get the EXCEPTION event.

proc wtxEventGetTest3 {Msg} {

    global errEvMsg
    global cpuZeroDivide

    puts stdout "$Msg" nonewline

    set nbCPU [lindex [lindex [wtxTsInfoGet] 2 ] 0]
    set excepNumber $cpuZeroDivide($nbCPU)

    set priority 100
    set option 0
    set arg1 1
    set arg2 0

    set taskDesc [testTaskCreate $Msg objSampleWtxtclTest6.o divide \
                        $priority $option $arg1 $arg2]
    if {$taskDesc == 1} {
        return
    }

    set taskId [lindex $taskDesc 0]

    set resResume [wtxContextResume CONTEXT_TASK $taskId]
    if {$resResume == ""} {
        cleanUp t $taskDesc
        return
    }

    set resEventPoll [wtxEventPoll 100]
    if {$resEventPoll == ""} {
        cleanUp t $taskDesc
        return
    }

    while {[string compare [lindex $resEventPoll 0] EXCEPTION]} {
        # wait for event
        set resEventPoll [wtxEventPoll 100]
        if {$resEventPoll == ""} {
            cleanUp t $taskDesc
            return
        }
    }


    set ctxType [lindex $resEventPoll 1]
    set ctxId [lindex $resEventPoll 2]
    set ctxVect [lindex $resEventPoll 3]

    if {$ctxType != 0x3 ||\
        $ctxId != $taskId ||\
        $ctxVect != $excepNumber} {
        testFail $errEvMsg(7)
        cleanUp t $taskDesc
        return
        }

    # clean up
    cleanUp t $taskDesc
    wtxTestPass
}


#*****************************************************************************
#
# wtxEventGetTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxEventGetTest {tgtsvrName timeout invalidAddr} {
    
    global vEvMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxEventGetTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set NBTESTS 2
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    # enable events
    catch {wtxRegisterForEvent .*} st

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # call the test procedures
    wtxEventGetTest1 $vEvMsg(0)
    wtxEventGetTest2 $tgtsvrName $vEvMsg(8)
    # wtxEventGetTest3 $vEvMsg(9)

    # deconnection
    wtxToolDetachCall "wtxEventGetTest"
}

################################################################################
#
# wtxtclPathGet - get path of wtxtcl executable currently used
#
# RETURNS: The path to this wtxtcl executable, without the extension
#

proc wtxtclPathGet {} {

    return [file rootname [info nameofexecutable]]
}
