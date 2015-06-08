# wtxInfoQTest.tcl - Tcl script, test WTX_UNREGISTER
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

global itemsToRegister

set itemsToRegister {
    { {wtxInfoQTestName1}	{wtxInfoQTestType1}	{wtxInfoQTestKey1} }
    { {wtxInfoQTestName2}	{wtxInfoQTestType1}	{wtxInfoQTestKey2} }
    { {wtxInfoQTestName3}	{wtxInfoQTestType2}	{wtxInfoQTestKey1} }
    { {wtxInfoQTestName4}	{wtxInfoQTestType2}	{wtxInfoQTestKey2} }
}

################################################################################
#
# wtxRegisterItems - register all items of itemsToRegister
#
# SYNOPSIS
#   wtxRegisterItems
#
# PARAMETERS: N/A
#
# RETURNS: OK or ERROR if impossible to register items
#
# ERRORS:
#

proc wtxRegisterItems {} {

    global itemsToRegister		;# list of items to register
    global errRegistryMsg		;# error messages array

    foreach item $itemsToRegister {
	if { [catch "wtxRegister $item" registerStatus] } {
	    wtxInfoQErrorLog $errRegistryMsg(0) $registerStatus
	    return "ERROR"
	}
    }
}

################################################################################
#
# wtxUnregisterItems - unregister items from itemsToRegister
#
# SYNOPSIS
#   wtxUnregisterItems
#
# PARAMETERS: N/A
#
# RETURNS: OK or ERROR if it was impossible to unregister an item
#
# ERRORS:
# .iP "Could not unregister item"
# One item name was not unregistrable. See given error code for more details.
#

proc wtxUnregisterItems {} {

    global itemsToRegister		;# list of items to register
    global errRegistryMsg		;# error messages array

    foreach item $itemsToRegister {
	if { [catch "wtxUnregister [lindex $item 0]" unregStatus] } {

	    # cannot unregister item. Notify error log, but continue with the
	    # other ones

	    wtxInfoQErrorLog $errRegistryMsg(6) $unregStatus
	}
    }
}

################################################################################
#
# wtxInfoQErrorLog - logs a wtxInfoQTest error
#
# SYNOPSYS
#   wtxInfoQErrorLog <msg> <errStatus>
#
# PARAMETERS
#   msg : the message to log
#   errStatus : the status error returned
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc wtxInfoQErrorLog { msg errStatus } {

    set errorName [wtxErrorName $errStatus]

    if { errorName == "" } {
	set errorName $errStatus
    }

    testFail "$msg $errorName"
}

################################################################################
#
# wtxInfoQTest1 - checks that registered items are in the list
#
# SYNOPSIS
#   wtxInfoQTest1 <msg>
#
# PARAMETERS
#   msg : the message to print when entering this test
#
# RETURNS: N/A
#
# ERRORS:
#

proc wtxInfoQTest1 { msg } {

    global errRegistryMsg		;# array of error messages
    global itemsToRegister		;# list of test registered items

    puts stdout "$msg" nonewline

    # first get the list of registered names from itemsToRegister

    set itemNameList {}
    foreach item $itemsToRegister {
	lappend itemNameList [lindex $item 0]
    }

    # now check that item has been well registered through a call to wtxInfoQ

    if { [catch "wtxInfoQ" infoQList] } {
	testFail $errRegistryMsg(9)
	return
    }

    # check that the names are in registry database

    set nItemMatch 0
    foreach elt $infoQList {

	if { [lsearch $itemNameList [lindex $elt 0]] != -1 } {

	    incr nItemMatch
	}
    }

    if { $nItemMatch != [llength $itemNameList] } {
	testFail $errRegistryMsg(2)
	return
    }

    wtxTestPass
}

################################################################################
#
# wtxInfoQTest2 - gets registered items through name pattern
#
# SYNOPSIS
#   wtxInfoQTest1 <msg>
#
# PARAMETERS
#   msg : the message to print when entering this test
#
# RETURNS: N/A
#
# ERRORS:
#

proc wtxInfoQTest2 { msg } {

    global errRegistryMsg		;# array of error messages
    global itemsToRegister		;# list of test registered items

    puts stdout "$msg" nonewline

    if { [catch "wtxInfoQ wtxInfoQTestName.*" infoQList] } {
	wtxInfoQErrorLog $errRegistryMsg(9) $infoQList
	return
    }

    # now check that the number of such registered items is really the one we
    # are looking for

    if { [llength $itemsToRegister] != [llength $infoQList] } {
	testFail errRegistryMsg(11)
    }

    wtxTestPass
}

################################################################################
#
# wtxInfoQTest3 - gets registered items through type pattern
#
# SYNOPSIS
#   wtxInfoQTest1 <msg>
#
# PARAMETERS
#   msg : the message to print when entering this test
#
# RETURNS: N/A
#
# ERRORS:
#

proc wtxInfoQTest3 { msg } {

    global errRegistryMsg		;# array of error messages
    global itemsToRegister		;# list of test registered items

    puts stdout "$msg" nonewline

    if { [catch "wtxInfoQ .* wtxInfoQTestType.*" infoQList] } {
	wtxInfoQErrorLog $errRegistryMsg(9) $infoQList
	return
    }

    # now check that the number of such registered items is really the one we
    # are looking for

    if { [llength $itemsToRegister] != [llength $infoQList] } {
	testFail errRegistryMsg(12)
    }

    wtxTestPass
}

################################################################################
#
# wtxInfoQTest4 - gets registered items through key pattern
#
# SYNOPSIS
#   wtxInfoQTest1 <msg>
#
# PARAMETERS
#   msg : the message to print when entering this test
#
# RETURNS: N/A
#
# ERRORS:
#

proc wtxInfoQTest4 { msg } {

    global errRegistryMsg		;# array of error messages
    global itemsToRegister		;# list of test registered items

    puts stdout "$msg" nonewline

    if { [catch "wtxInfoQ .* .* wtxInfoQTestKey.*" infoQList] } {
	wtxInfoQErrorLog $errRegistryMsg(9) $infoQList
	return
    }

    # now check that the number of such registered items is really the one we
    # are looking for

    if { [llength $itemsToRegister] != [llength $infoQList] } {
	testFail errRegistryMsg(13)
    }

    wtxTestPass
}

################################################################################
#
# wtxInfoQTest - test for the wtxRegister Tcl api
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxInfoQTest { tgtsvrName timeout invalidAddr } {

    global registerMsg

    wtxRegisterItems
    wtxInfoQTest1 $registerMsg(3)
    wtxInfoQTest2 $registerMsg(4)
    wtxInfoQTest3 $registerMsg(5)
    wtxInfoQTest4 $registerMsg(6)
    wtxUnregisterItems
}
