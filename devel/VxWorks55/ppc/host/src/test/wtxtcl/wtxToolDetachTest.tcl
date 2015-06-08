# wtxToolDetachTest.tcl - Tcl script, test WTX_TOOL_DETACH 
#
# Copyright 1994 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01k,28jan98,p_b  Adapted for WTX 2.0
# 01j,20oct95,jmp  added invalidAddr argument to wtxToolDetachTest.
# 01i,21aug95,jmp  added timeout to wtxToolDetachTest procedure.
#                  changed tests control method, now uses wtxErrorHandler.
# 01h,23jun95,f_v  added wtxTestRun call
# 01g,21jun95,f_v  clean up main procedure
# 01f,14jun95,f_v  changed wtxToolAttach name parameter
# 01e,12may95,c_s  upgraded for new wtxtcl version.
# 01d,27jan95,kpn  deplaced testcase units description in wtxTestMsg and
#                  testcase units errors in wtxTestErr.tcl,
#                  added global toolDetachMsg in the main,
#                  added global errSessionMsg in each testcase units,
#                  used toolDetachMsg & errSessionMsg to manage messages.
# 01c,22Jan95,kpn  changed test of target server name.
# 01b,04Jan95,kpn  rewritten the script : wtxToolDetach is became automatic.
# 01a,14Nov94,kpn  written.
#

#
# DESCRIPTION
#
# WTX_TOOL_DETACH - Detach tool from target server
#
# The procedure wtxToolDetach has no parameter so this script only tests
# its result. wtxToolDetach must return zero if a tool was attached to the
# target server. If no tool is attached when wtxToolDetach is called then 
# the procedure returns nothing.
# 
#



#*****************************************************************************
#
# wtxToolDetachTest1 - Test wtxToolDetach result after to have called 
#		       wtxToolAttach.
#
# This script executes the procedure wtxToolAttach that allows to
# attach a tool. Then it executes the procedure wtxToolDetach and tests the
# result. It must be equal to zero.
#

proc wtxToolDetachTest1 {tgtsvrName serviceType Msg} {


    # array that contains the error codes
    global errSessionMsg

    puts stdout "$Msg" nonewline

    if {[catch "wtxToolAttach $tgtsvrName $serviceType" resToolAttach]} {
        testFail "$errSessionMsg(1) : $tgtsvrName"
        return
    }

    catch "wtxToolDetach" errmsg
    if {$errmsg != ""} {
	testFail $errSessionMsg(2)
	return
    }

    wtxTestPass
}

#*****************************************************************************
#
# wtxToolDetachTest2 - verify wtxToolDetach when no tool is attached
#
# When no tool is attached wtxToolDetach routine must report that
# no handle is found.
#

proc wtxToolDetachTest2 {tgtsvrName Msg} {

    global errSessionMsg
    puts stdout "$Msg" nonewline

    if {![catch "wtxToolDetach" errmsg]} {
	testFail $errSessionMsg(6)
	return
	}

    if {$errmsg != "WTX handle not found"} {
        testFail $errmsg
	return
    }

    wtxTestPass
}


#*****************************************************************************
#
# wtxToolDetachTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
# Furthermore it attaches itself to the target server (thru a wtxToolAttachCall
# command) and, when testcase units have been run, detaches itself from the
# target server.
#

proc wtxToolDetachTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxToolDetachTest targetServerName\
				timeout invalidAddr"
        return
    }

    global toolDetachMsg

    # call the subtests 

    wtxToolDetachTest1 $tgtsvrName 2 $toolDetachMsg(1) 
    wtxToolDetachTest2 $tgtsvrName $toolDetachMsg(2)
}
