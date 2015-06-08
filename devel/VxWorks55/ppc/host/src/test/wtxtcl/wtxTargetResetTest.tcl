# wtxTargetResetTest.tcl - Tcl script, test WTX_TARGET_RESET 
#
# Copyright 1994 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01k,28apr99,jrp  Removed comments to re-instate this test.
# 01j,28jan98,p_b  Adapted for WTX 2.0
# 01i,26feb96,l_p  corrected English mistakes in a few messages.
# 01h,20oct95,jmp  added invalidAddr argument to wtxTargetResetTest.
# 01g,21aug95,jmp  added timeout to wtxToolAttachCall in wtxTargetResetTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01f,23jun95,f_v  added wtxTestRun call
# 01e,21jun95,f_v  clean up main procedure
# 01d,27jan95,kpn  deplaced testcase units description in wtxTestMsg.tcl,
#                  added global targetResetMsg in the main.
# 01c,26Jan95,kpn  removed the calls to wtxToolAttachCall and wtxToolDetach
#                  made in each testcase.
#                  Added wtxToolAttachCall and wtxToolDetachCall in the main
#                  procedure.
# 01b,20Jan95,kpn  replaced wtxToolAttach by wtxToolAttachCall.
# 01a,  Nov94,kpn  written.
#


#
# DESCRIPTION
#
# WTX_TARGET_RESET - Reset target
#
# This script tests the result of wtxTargetReset.
# wtxTargetReset has no parameter, there is only one mean to call it,  
# so this script only verifies that wtxTargetReset result is right.
#
# CAREFUL :
#	When the target is reset we must do <startup in the target for 
#	the moment ... 
#	so this test is tricky for the moment...and it must not use it.
#
#	Furthermore wtxTargetReset doesn't return always the same value,
#	even if it has succeeded. 
#



#***********************************************************************
#
# wtxTargetResetTest1 -
#
# Test if the result of wtxTargetReset is right. It must be zero.
#

proc wtxTargetResetTest1 {Msg} {

    puts stdout "$Msg" nonewline

    # execute the routine wtxTargetReset
    set resTargetReset [wtxTargetReset]

    # verify the result
    if {$resTargetReset != 0} {
        testFail "ERROR : wtxTargetReset call failed\n" 
	return
    }


    wtxTestPass
}



#***********************************************************************
#
# wtxTargetResetTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
# Furthermore it attaches itself to the target server (thru a wtxToolAttachCall
# command) and, when testcase units have been run, detaches itself from the
# target server.
#

proc wtxTargetResetTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxTargetResetTest targetServerName\
				timeout invalidAddr"
        return
    }

    global targetResetMsg

    # connection to the target server
    wtxToolAttachCall $tgtsvrName 2 $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # call the testcase units
    wtxTargetResetTest1 $targetResetMsg(1)

    # detach from the target server
    wtxToolDetachCall "wtxTargetResetTest : "

    # puts stdout "\tTest unavailable for the moment\n"
}
