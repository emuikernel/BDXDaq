# wtxToolAttachTest.tcl - Tcl script, test WTX_TOOL_ATTACH 
#
# Copyright 1994 Wind River Systems, Inc. 
#
#
# modification history
# --------------------
# 01k,28jan98,p_b  Adapted for WTX 2.0
# 01j,20oct95,jmp  added invalidAddr argument to wtxToolAttachTest. 
# 01i,21aug95,jmp  added timeout to wtxToolAttachTest procedure.
#                  changed tests control method, now uses wtxErrorHandler.
# 01h,23jun95,f_v  added wtxTestRun call
# 01g,14jun95,f_v  change wtxToolAttach name parameter
# 01f,12may95,c_s  upgraded for new wtxtcl version.  wtxToolAttachTest3 is 
#                    now gone, because wtxtcl supports multiple server
#                    attachments.
# 01e,13feb95,f_v  change tests in wtxToolAttachTest3 to fix a bug
# 01d,27jan95,kpn  deplaced testcase units description in wtxTestMsg and
#                  testcase units errors in wtxTestErr.tcl,
#                  added global toolAttachMsg in the main,
#                  added global errSessionMsg in each testcase units,
#                  used toolAttachMsg & errSessionMsg to manage messages.
# 01c,22jan95,kpn  changed test of target server name.
# 01b,04jan95,kpn  removed parameter to wtxToolDetach (automatic now). 
# 01a,07nov94,kpn  written.
#



#
# DESCRIPTION
#
# WTX_TOOL_ATTACH - Attach tool to target server
#
# This script tests the result of wtxToolAttach according
# to different call.
#    (1) Test wtxToolAttach with valid target server name.
#    (2) Test wtxToolAttach with invalid target server name.
#
#
# REMARK : 
#	This script tests what wtxtcl program already tests, so it tests 
#	the wtxtcl return.
#


#*************************************************************************
#
# wtxToolAttachTest1 - wtxToolAttach with valid argument
#
# Test wtxToolAttach with valid target server name.
#

proc wtxToolAttachTest1 {tgtsvrName serviceType Msg} {


    # array that contains the error codes
    global errSessionMsg

    puts stdout "$Msg" nonewline

    # connection to the target server
    if {[catch "wtxToolAttach $tgtsvrName $serviceType" resToolAttach]} {
        testFail "$resToolAttach\n$errSessionMsg(1) : $tgtsvrName"
        return
    }

    catch "wtxToolDetach" errmsg
    if {$errmsg != ""} {
        testFail "$errSessionMsg(2)"
        return
    }

    wtxTestPass
}



#************************************************************************* 
#
# wtxToolAttachTest2 - wtxToolAttach with invalid arg.
#
# Test wtxToolAttach with invalid target server name.
#

proc wtxToolAttachTest2 {serviceType Msg} {

    puts stdout "$Msg" nonewline

    if {![catch "wtxToolAttach nothing $serviceType" errmsg]} {
	testFail $errSessionMsg(5)
	return
	}

    if {![wtxErrorMatch $errmsg API_SERVER_NOT_FOUND]} {
	testFail $errmsg
	return
    }

    wtxTestPass
}

#**************************************************************************
#
# wtxToolAttachTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
# Furthermore it attaches itself to the target server (thru a wtxToolAttachCall
# command) and, when testcase units have been run, detaches itself from the
# target server.
#

proc wtxToolAttachTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
	puts stdout "usage : wtxToolAttachTest targetServerName\
				timeout invalidAddr"
	return
    }

    # tests description
    global toolAttachMsg

    # call the subtest  
    wtxToolAttachTest1 $tgtsvrName wtxtest $toolAttachMsg(1)
    wtxToolAttachTest2 wtxtest $toolAttachMsg(2)  
}
