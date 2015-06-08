# wtxAgentModeSetTest.tcl - Tcl script, test WTX_AGENT_MODE_SET
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01g,28jan98,p_b  Adapted for WTX 2.0
# 01f,01mar96,jmp  renamed wtxzAgentModeSetTest by wtxAgentModeSetTest.
# 01e,07nov95,jmp  replaced AGENT_INVALID_PARAM by AGENT_AGENT_MODE_ERROR.
# 01d,20oct95,jmp  added invalidAddr argument to wtxzAgentModeSetTest.
# 01c,09aug95,jmp  added timeout to wtxToolAttachCall in wtxzAgentModeSetTest.
# 01b,23jun95,f_v  added wtxTestRun call
# 01a,24may95,f_v  written. 
#
#

#
# DESCRIPTION
#
# WTX_AGENT_MODE_SET  - WTX change mode
#
#
#



#*****************************************************************************
#
# wtxAgentModeSetTest1 -
#
# Verify if wtxAgentModeSet change agent mode
#

proc wtxAgentModeSetTest1 {Msg} {

    global errCtxMsg
    global errEvptMsg
    global agentErrMsg

    puts stdout "$Msg" nonewline

    # set agent in extern mode
    catch "wtxAgentModeSet 2" agentSt
    if {[wtxErrorMatch $agentSt AGENT_AGENT_MODE_ERROR]} {
	puts stdout $agentErrMsg(12)
	return
    }

    # suspend the system
    if {[catch "wtxContextSuspend CONTEXT_SYSTEM -1" susSt]} {
	testFail $susSt
	return
    }

    # read vxTicks
    set addrs [lindex [wtxSymFind -name "vxTicks"] 1]
    set vxTicksIni [wtxGopherEval -errvar errCode "$addrs *!"]

    after 1000

    set vxTicksMid [wtxGopherEval -errvar errCode "$addrs *!"]

    # check if system was correctly suspended
    if {$vxTicksMid != $vxTicksIni} {
	testFail "ERROR : System wasn't suspended"
	return
	}

    # continue the system
    if {[catch "wtxContextCont system -1" contSt]} {
	    testFail $contSt
	    catch "wtxAgentModeSet 1" agentSt
	    return
        }

    after 1000

    # read vxTicks
    set vxTicksEnd [wtxGopherEval -errvar errCode "$addrs *!"]

    # check if vxTicksIni is correctly incremented by one
    if {$vxTicksEnd <= $vxTicksMid} {
	testFail "ERROR : Context in extern mode didn't continued correctly"
	catch "wtxAgentModeSet 1" agentSt
	return
	}

    # set agent in task mode
    catch "wtxAgentModeSet 1" agentSt

    if { $agentSt != 0} {
	    testFail $agentErrMsg(11)
	    return
        }

    wtxTestPass
}

#*****************************************************************************
#
# wtxAgentModeSetTest2 -
#
# Verify if wtxAgentModeSet change agent mode
#

proc wtxAgentModeSetTest2 {Msg} {

    global errCtxMsg
    global errEvptMsg
    global agentErrMsg

    puts stdout "$Msg" nonewline

    # set agent in extern mode
    catch "wtxAgentModeSet 2" agentSt
    if {[wtxErrorMatch $agentSt AGENT_INVALID_PARAMS]} {
	puts stdout $agentErrMsg(12)
	return
	}

    # get entry point
    if {![set usrInfo [testSymFind usrClock]]} {
	return
	}

    set entry [lindex $usrInfo 0]

    # add a breakpoint on usrClock system function
    # now all the system should be pended
    catch "wtxEventpointAdd TEXT_ACCESS $entry SYSTEM -1 STOP_ALL 0 0 0" bk
    if { $bk < 0} {
	    testFail $errEvptMsg(10)
	    catch "wtxAgentModeSet 1" agentSt
	    return
        }

    # read vxTicks
    set addrs [lindex [wtxSymFind -name "vxTicks"] 1]
    set vxTicksIni [wtxGopherEval -errvar errCode "$addrs *!"]

    # continue the system
    catch "wtxContextCont system -1" contSt
    if { $contSt != 0} {
	    testFail $errCtxMsg(0)
	    catch "wtxAgentModeSet 1" agentSt
	    catch "wtxEventpointDelete $bk" status
	    return
        }

    after 1000

    # read vxTicks
    set vxTicksEnd [wtxGopherEval -errvar errCode "$addrs *!"]

    # check if vxTicksIni is correctly incremented by one
    if {$vxTicksEnd != [expr $vxTicksIni+1]} {
	testFail "ERROR : Context in extern mode didn't continued correctly"
	return
	}

    # Delete breakpoint
    catch "wtxEventpointDelete $bk" status
    if { $status != 0} {
	    testFail $errEvptMsg(2)
	    catch "wtxAgentModeSet 1" agentSt
	    return
        }

    # continue the system
    catch "wtxContextCont system -1" contSt
    if { $contSt != 0} {
	    testFail $errCtxMsg(0)
	    catch "wtxAgentModeSet 1" agentSt
	    return
        }

    after 1000

    # read vxTicks
    set vxTicksEnd2 [wtxGopherEval -errvar errCode "$addrs *!"]

    # check if system was restarted
    if {$vxTicksEnd2 <= $vxTicksEnd} {
	testFail "ERROR : Context in extern mode didn't continued correctly"
	catch "wtxAgentModeSet 1" agentSt
	return
	}

    # set agent in task mode
    catch "wtxAgentModeSet 1" agentSt

    if { $agentSt != 0} {
	    testFail $Msg
	    puts stdout $agentErrMsg(11)
	    return
        }

    wtxTestPass
}

#*****************************************************************************
#
# wtxAgentModeSetTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxAgentModeSetTest {tgtsvrName timeout invalidAddr} {

    global agentModeSetMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxAgentModeSet targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout

    # call the test procedures
    # two procedures are incompatibles 
    wtxAgentModeSetTest1 $agentModeSetMsg(1)
    #wtxAgentModeSetTest2 $agentModeSetMsg(1)

    # deconnection
    wtxToolDetachCall "wtxAgentModeSet"
}
