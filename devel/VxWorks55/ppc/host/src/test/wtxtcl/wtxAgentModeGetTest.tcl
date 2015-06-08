# wtxAgentModeGetTest.tcl - Tcl script, test WTX_AGENT_MODE_GET
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01b,28jan98,p_b  Adapted for WTX 2.0
# 01a,13nov95,jmp  written. 
#
#

#
# DESCRIPTION
#
# WTX_AGENT_MODE_GET  - Get the WDB agent mode.
#
#
#



#*****************************************************************************
#
# wtxAgentModeGetTest1 - Verify that wtxAgentModeGet return correct value
#
#
#

proc wtxAgentModeGetTest1 {Msg mode} {

    global agentErrMsg

    puts stdout "$Msg" nonewline

    # get agent mode
    if {[catch "wtxAgentModeGet" agentMode]} {
	testFail $agentMode
	return
    }


    if {$agentMode != $mode} {
	testFail $agentErrMsg(13)
	return
    }


    wtxTestPass
}


#*****************************************************************************
#
# wtxAgentModeGetTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxAgentModeGetTest {tgtsvrName timeout invalidAddr} {

    global agentModeGetMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxAgentModeGet targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout

    # call the test procedures
    wtxAgentModeGetTest1 $agentModeGetMsg(1) AGENT_MODE_TASK

    # valid test in external mode :
    # if it's possible to change mode to external mode
    # wtxAgentModeGetTest1 is run in external mode

    if {![catch "wtxAgentModeSet AGENT_MODE_EXTERN" agentSt]} {
	puts stdout "\nSwitch to External Mode :"
	wtxAgentModeGetTest1 $agentModeGetMsg(1) AGENT_MODE_EXTERN
	wtxAgentModeSet AGENT_MODE_TASK
    }

    # deconnection
    wtxToolDetachCall "wtxAgentModeGet"
}
