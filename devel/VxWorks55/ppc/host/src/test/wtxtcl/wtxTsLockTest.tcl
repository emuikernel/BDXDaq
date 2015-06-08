# wtxTsLockTest.tcl - Tcl script, test WTX_TS_LOCK
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
# WTX_TS_LOCK - lock the target server.
#


#****************************************************************************
#
# wtxTsLockTest1 - 
#
#

proc wtxTsLockTest1 {Msg} {

    global errMiscMsg

    puts stdout "$Msg" nonewline

    # get the actual mode to restore it at the end of the test
    set tsInfo [wtxTsInfoGet]

    set saveMode [lindex $tsInfo 11]
    if {$saveMode == ""} {
        return
    }

    set resLock [wtxTsLock]
    if {$resLock == ""} {
	return
    }

    set tsInfo [wtxTsInfoGet]
    if {$tsInfo == ""} {
	return
    }

    set lockStatus [lindex $tsInfo 11]

    if {$lockStatus != "reserved"} {
	testFail $errMiscMsg(32)
	return
	}

    # restore the previous mode if it's changed
    if {$saveMode == "unreserved"} {
        set resUnlock [wtxTsUnlock]
        if {$resUnlock == ""} {
                return
        }
    }


    wtxTestPass
}


#****************************************************************************
#
# wtxTsLockTest -
#
# Initialization of variables.
# Initialization of variables that contain the description
# of the tests.
# Call the test scripts.
#

proc wtxTsLockTest {tgtsvrName timeout invalidAddr} {

    global tsLockMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxTsLockTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    wtxTsLockTest1 $tsLockMsg(1)

    # deconnection
    wtxToolDetachCall "wtxTsLockTest"
}
