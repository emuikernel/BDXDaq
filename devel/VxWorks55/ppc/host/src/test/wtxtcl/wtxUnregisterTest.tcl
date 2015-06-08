# wtxUnregisterTest.tcl - Tcl script, test WTX_UNREGISTER
#
# Copyright 1999 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,08jan99,fle  written.
#


#
# DESCRIPTION
#
# WTX_UNREGISTER - unregisters an element from the Tornado registry
#

################################################################################
#
# wtxUnregisterErrorLog - logs a wtxUnregisterTest error
#
# SYNOPSYS
#   wtxUnregisterErrorLog <msg> <errStatus>
#
# PARAMETERS
#   msg : the message to log
#   errStatus : the status error returned
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc wtxUnregisterErrorLog { msg errStatus } {

    set errorName [wtxErrorName $errStatus]

    if { errorName == "" } {
	set errorName $errStatus
    }

    testFail "$msg $errorName"
}

################################################################################
#
# wtxUnregisterTest1 - unregisters an element then checks it
#
# This procedure registers an item, checks it is here, then Unregisters it, and
# check that it is not in the database anymore.
#
# SYNOPSIS
#   wtxUnregisterTest1 <msg>
#
# PARAMETERS
#   msg : the message to print when entering this test
#
# RETURNS: N/A
#
# ERRORS:
#

proc wtxUnregisterTest1 { msg } {

    global errRegistryMsg

    puts stdout "$msg" nonewline

    # now register for item

    if { [catch "wtxRegister wtxUnregisterTestName wtxUnregisterTestType\
			     wtxUnregisterTestKey" regStatus] } {

	wtxUnregisterErrorLog $errRegistryMsg(0) $regStatus
	return
    }

    # now check that item has been well registered through a call to wtxInfoQ

    if { [catch "wtxInfoQ wtxUnregisterTestName" infoQStatus] } {
	testFail $errRegistryMsg(9)
	return
    }

    # unregister item

    if { [catch "wtxUnregister wtxUnregisterTestName" unRegStatus] } {
	wtxRegisterErrorLog $errRegistryMsg(6) unRegStatus
	return
    }

    catch [wtxInfoQ] infoQList 

    # check that the name is not in database anymore

    set infoQNameList {}
    foreach elt $infoQList {
	lappend infoQNameList [lindex $elt 0]
    }

    if { [lsearch $infoQNameList "wtxUnregisterTestName"] != -1 } {
	testFail errRegistryMsg(10)
    }

    wtxTestPass
}

################################################################################
#
# wtxUnregisterTest - test for the wtxRegister Tcl api
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxUnregisterTest { tgtsvrName timeout invalidAddr } {

    global registerMsg

    wtxUnregisterTest1 $registerMsg(2)
}
