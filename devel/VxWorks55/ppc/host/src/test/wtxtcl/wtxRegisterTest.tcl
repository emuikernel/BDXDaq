# wtxRegisterTest.tcl - Tcl script, test WTX_REGISTER
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
# WTX_REGISTER - registers an element to the Tornado registry
#

################################################################################
#
# wtxRegisterErrorLog - logs a wtxRegisterTest error
#
# SYNOPSYS
#   wtxRegisterErrorLog <msg> <errStatus>
#
# PARAMETERS
#   msg : the message to log
#   errStatus : the status error returned
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc wtxRegisterErrorLog { msg errStatus } {

    set errorName [wtxErrorName $errStatus]
    puts stdout "wtxRegisterErrorLog $msg $errStatus"
    testFail "$msg $errorName"
}

################################################################################
#
# wtxRegisterTest1 - registers an element then checks it
#
# This test case registers for 1 item, then check if it has been registered
# through a call to `wtxInfoQ'
#
# SYNOPSIS
#   wtxRegisterTest1 <msg>
#
# PARAMETERS
#   msg : the message to print when entering this test
#
# RETURNS: N/A
#
# ERRORS:
# .iP "Could not register item"
# wtxRegsiter received error
# .iP "Registry item duplicated"
# The registry was able to re-register an already registered item
# .iP "Registry item not in list"
# The registry can't find in registry infoQ the item it just registered
# .iP "Registry found and registered names differ"
# The registered name and the name found by looking at wtxInfoQ result differ
# .iP "Registry found and registered types differ"
# The registered type and the type found by looking at wtxInfoQ result differ
# .iP "Registry found and registered keys differ"
# The registered key and the key found by looking at wtxInfoQ result differ
# .iP "Could not unregister item"
# The registry is unable to unregister the item that has just been registered
#

proc wtxRegisterTest1 { msg } {

    global errRegistryMsg

    puts stdout "$msg" nonewline

    # now register for item

    if { [catch "wtxRegister wtxRegisterTestName wtxRegisterTestType\
			     wtxRegisterTestKey" regStatus] } {

	wtxRegisterErrorLog $errRegistryMsg(0) $regStatus
	return
    }

    # now check that item has been well registered through a call to wtxInfoQ

    set itemInfo [wtxInfoQ wtxRegisterTestName]

    if { [llength $itemInfo] > 1 } {

	testFail $errRegistryMsg(1)
	return

    } elseif { [llength $itemInfo] == 0} {

	testFail $errRegistryMsg(2)
	return

    } else {

	set foundItemName [lindex [lindex $itemInfo 0] 0]
	set foundItemType [lindex [lindex $itemInfo 0] 1]
	set foundItemKey [lindex [lindex $itemInfo 0] 2]

	# check if it is the right item name

	if { $foundItemName != "wtxRegisterTestName" } {
	    testFail $errRegistryMsg(3)
	    return
	}

	# check if it is the right item type

	if { $foundItemType != "wtxRegisterTestType" } {
	    testFail $errRegistryMsg(4)
	    return
	}

	# check if it is the right item key

	if { $foundItemKey != "wtxRegisterTestKey" } {
	    testFail $errRegistryMsg(5)
	    return
	}
    }

    # unregister item

    if { [catch "wtxUnregister wtxRegisterTestName" unRegStatus] } {
	wtxRegisterErrorLog $errRegistryMsg(6) unRegStatus
	return
    }

    wtxTestPass
}

################################################################################
#
# wtxRegisterTest2 - test wtxRegister NAME_CLASH error
#
# This test procedure is to test the WTX_ERR_REGISTRY_NAME_CLASH error code.
# The procedure tries to register twice for the same name, and test is
# considered passed if the first registration succeeded, and the second returned
# REGISTRY_NAME_CLASH error code
#
# SYNOPSIS
#   wtxRegisterTest2 <msg>
#
# PARAMETERS
#   msg : the message to print while entering this test
#
# RETURNS: N/A
#
# ERRORS:
# .iP "registry could overwrite a registered name"
# The registry was able to overwrite an already registered item
# .iP "Could not unregister item"
# The registry is unable to unregister the item that has just been registered
# .iP "REGISTRY_NAME_CLASH error awaited, but got :"
# The registry was waiting for the REGISTRY_NAME_CLASH error, but it got back
# another error.
#

proc wtxRegisterTest2 { msg } {

    global errRegistryMsg

    puts stdout "$msg" nonewline

    # register once for the item, and wait for successful registering

    if { [catch "wtxRegister wtxRegisterTestName wtxRegisterTestType \
		 wtxRegisterTestKey" regStatus] } {

	wtxRegisterErrorLog $errRegistryMsg(0) $regStatus
	return
    }

    # try to register a second time, and catch the error

    if { ! [catch "wtxRegister wtxRegisterTestName wtxRegisterTestType \
		   wtxRegisterTestKey" regStatus] } {

	testFail $errRegistryMsg(7)
	return
    }

    # see if the caught exception was the right one

    if { $regStatus != "REGISTRY_NAME_CLASH" } {

	wtxRegisterErrorLog $errRegistryMsg(8) $regStatus
	return
    }

    # unregister item

    if { [catch "wtxUnregister wtxRegisterTestName" unRegStatus] } {
	wtxRegisterErrorLog $errRegistryMsg(6) unRegStatus
	return
    }

    wtxTestPass
}

################################################################################
#
# wtxRegisterTest - test for the wtxRegister Tcl api
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxRegisterTest { tgtsvrName timeout invalidAddr } {

    global registerMsg

    wtxRegisterTest1 $registerMsg(0)
    wtxRegisterTest2 $registerMsg(1)
}
