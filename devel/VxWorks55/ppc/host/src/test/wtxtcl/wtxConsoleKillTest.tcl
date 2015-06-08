# wtxConsoleKillTest.tcl - Tcl script, test WTX_CONSOLE_KILL
#
# Copyright 1995-2001 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01m,23nov01,hbh  Removed DISPLAY test on :0 value.
# 01l,28jan98,p_b  Adapted for WTX 2.0
# 01k,20oct95,jmp  added invalidAddr argument to wtxConsoleKillTest.
# 01j,21aug95,jmp  added timeout to wtxToolAttachCall in wtxConsoleKillTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01i,23jun95,f_v  added wtxTestRun call
# 01h,09jun95,f_v  clean up main procedure, added test on wtxConsoleCreate
# 01g,27apr95,c_s  now uses wtxErrorMatch.
# 01f,17mar95,f_v  fix a bug now DISPLAY can be :0 or :0.0
# 01e,22feb95,kpn  changed S_wtx_INVALID_CONSOLE -> 
#		   WTX_ERR_TGTSVR_INVALID_CONSOLE
# 01d,27jan95,kpn  deplaced testcase units description in wtxTestMsg and
#                  testcase units errors in wtxTestErr.tcl,
#                  added global csleKillMsg in the main,
#		   added global errMiscMsg in each testcase units,
#                  used csleKillMsg & errMiscMsg to manage messages.
# 01c,26Jan95,kpn  removed the calls to wtxToolAttachCall and wtxToolDetach
#                  made in each testcase.
#                  Added wtxToolAttachCall and wtxToolDetachCall in the main 
#                  procedure.
#		   removed error codes initialization, added global variable.
# 01b,24Jan95,kpn  modified DISPLAY test.
# 01a,11Jan95,kpn  written.
#
#

#
# DESCRIPTION
#
# WTX_CONSOLE_KILL - Kill a console tty
#
# TESTS :
#
# (1) verify wtxConsoleKill with valid <console-pid> parameter.
#
# (2) verify wtxConsoleKill with invalid <console-pid> parameter. This call
#	must generate the error code : WTSERR_INVALID_CONSOLE
#
#
# REM : % wtxConsoleKill 0
#       WTX: RPC: Unable to receive; errno = Connection reset by peer
#
#


#*************************************************************************
#
# wtxConsoleKillTest1 - verify wtxConsoleKill with valid <console-pid>
#			parameter
#
# To do this test a console must be created. So this script creates a console
# with the routine wtxConsoleCreate that returns the console descriptor. This
# one is used to test wtxConsoleKill result called with valid <console-pid>
# parameter.
#

proc wtxConsoleKillTest1 {Msg} {

 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMiscMsg

    global env

    puts stdout "$Msg" nonewline

    # check the DISPLAY environment variable exists
    set findDisplayVar [info exists env(DISPLAY)]

    # DISPLAY environment variable doesn't exist : display a message
    if {$findDisplayVar == 0} {
        testFail $errMiscMsg(1)
        return
    }

    # get the value of the DISPLAY environment variable
    set display [set env(DISPLAY)]

    set pos [string first : $display]
    if {$pos == -1} {
        testFail $errMiscMsg(2)
        return
    }

    # create a console : call wtxConsoleCreate routine
    set resConsoleCreate [wtxConsoleCreate consoleName $display]
    if {$resConsoleCreate == ""} {
	 return
    }

    # verify the DISPLAY variable contains a valid value
    if {$resConsoleCreate == "0 0 0"} {
        testFail $errMiscMsg(2)
        return
    }

    # get the console identificator
    set consoleIdRet [lindex $resConsoleCreate 2]

    # kill the console
    set resConsoleKill [wtxConsoleKill $consoleIdRet]
    # verify wtxConsoleKill result is equal to zero
    if {$resConsoleKill != 0} {
        testFail $errMiscMsg(6)
        return
    }

    wtxTestPass
}

#*************************************************************************
#
# wtxConsoleKillTest2 - verify wtxConsoleKill with invalid <console-pid> 
#			parameter. 
#
# This test checks that wtxConsoleKill is called with an invalid
# <console-pid> parameter generates the error code : WTSERR_INVALID_CONSOLE
#

proc wtxConsoleKillTest2 {Msg} {

 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMiscMsg

    puts stdout "$Msg" nonewline

    # kill a console that doesn't exist
    set invalidConsoleId 20 

    # if no exception occurred
    if {![catch {wtxConsoleKill $invalidConsoleId} resConsoleKill]} {
	testFail $errMiscMsg(7)
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $resConsoleKill SVR_INVALID_CONSOLE]} {
	testFail $resConsoleKill
        return
    }

    # if the caught exception is the expected error code
    wtxTestPass
}

#*************************************************************************
#
# wtxConsoleKillTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the 
# testcase units and calls each testcase procedure contained in the test script
#

proc wtxConsoleKillTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxConsoleKillTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    global csleKillMsg

    # call tests with valid parameters
    wtxToolAttachCall $tgtsvrName 2 $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    wtxConsoleKillTest1 $csleKillMsg(1)

    # call tests with invalid parameters 
    wtxErrorHandler [lindex [wtxHandle] 0] ""
    wtxConsoleKillTest2 $csleKillMsg(2)

    # detach from the target server
    wtxToolDetachCall "\nwtxConsoleKill : "
}
