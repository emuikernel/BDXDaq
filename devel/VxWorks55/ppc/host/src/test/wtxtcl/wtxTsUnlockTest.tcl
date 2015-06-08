# wtxTsUnlockTest.tcl - Tcl script, test WTX_TS_UNLOCK
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01c,28jan98,p_b  Adapted for WTX 2.0
# 01b,31jan96,jmp  corrected error message.
# 01a,09oct95,jmp  written.
#


#
# DESCRIPTION
#
# WTX_TS_UNLOCK - unlock the target server.
#


#****************************************************************************
#
# wtxTsUnlockTest1 - 
#
#

proc wtxTsUnlockTest1 {Msg} {

    global errMiscMsg

    puts stdout "$Msg" nonewline

    # get the actual mode to restore it at the end of the test
    set tsInfo [wtxTsInfoGet]

    set saveMode [lindex $tsInfo 11]
    if {$saveMode == ""} {
	return
    }

    set resUnlock [wtxTsUnlock]
    if {$resUnlock == ""} {
	return
    }

    set tsInfo [wtxTsInfoGet]
    set lockStatus [lindex $tsInfo 11]

    if {$lockStatus != "unreserved"} {
	testFail $errMiscMsg(32)
	return
	}

    # restore the previous mode if it's changed
    if {$saveMode == "reserved"} {
	set resLock [wtxTsLock]
	if {$resLock == ""} {
		return
	}
    }

    wtxTestPass
}


#****************************************************************************
#
# wtxTsUnlockTest -
#
# Initialization of variables.
# Initialization of variables that contain the description
# of the tests.
# Call the test scripts.
#

proc wtxTsUnlockTest {tgtsvrName timeout invalidAddr} {

    global tsLockMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxTsUnlockTest targetServerName\
			timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    wtxTsUnlockTest1 $tsLockMsg(2)

    # deconnection
    wtxToolDetachCall "wtxTsLockTest"
}
