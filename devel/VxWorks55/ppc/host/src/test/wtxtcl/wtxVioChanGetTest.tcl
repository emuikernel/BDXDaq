# wtxVioChanGetTest.tcl - Tcl script, test WTX_VIO_CHAN_GET
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01e,28jan98,p_b  Adapted for WTX 2.0
# 01d,20oct95,jmp  added invalidAddr argument to wtxVioChanGetTest.
# 01c,21aug95,jmp  added timeout to wtxToolAttachCall in wtxVioChanGetTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01b,23jun95,f_v  added wtxTestRun call
# 01a,30may95,f_v  written.
#
#

#
# DESCRIPTION
#
# WTX_VIO_CHAN_GET - get a virtual I/O channel number
#
# TESTS :
#
# (1) wtxVioChanGetTest1 - wtxVioChanGet with valid parameters.
#
#
#	


#*************************************************************************
#
# wtxVioChanGetTest1 - wtxVioChanGet with valid parameters.
#
#

proc wtxVioChanGetTest1 {Msg} {

    global errVioMsg

    puts stdout "$Msg" nonewline

    set chanId [wtxVioChanGet]
    if {$chanId == ""} {
	return
	}

    if {$chanId <= 0 || $chanId > 0xffffff} {
        testFail $errVioMsg(3)
        return
    }

    # clean up
    wtxVioChanRelease $chanId

    wtxTestPass
}

#*************************************************************************
#
# wtxVioChanGetTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
# Furthermore it attaches itself to the target server (thru a wtxToolAttachCall
# command) and, when testcase units have been run, detaches itself from the
# target server.
#

proc wtxVioChanGetTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxVioChanGetTest targetServerName\
				timeout invalidAddr"
        return
    }

    # tests description
    global vioChanGetMsg

    # connection to the target server
    wtxToolAttachCall $tgtsvrName 2 $timeout
    # enable all events
    catch {wtxRegisterForEvent .*} st

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # call the testcase units 
    wtxVioChanGetTest1 $vioChanGetMsg(1)

    # detach from the target server
    wtxToolDetachCall "\nwtxVioChanGetTest : "
}
