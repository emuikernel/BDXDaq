# wtxTargetAttachTest.tcl - Tcl script, test WTX_TARGET_ATTACH
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01b,28jan98,p_b  Adapted for WTX 2.0
# 01a,20oct95,jmp  written.
#


#
# DESCRIPTION
#
# WTX_TARGET_ATTACH - attach a Target to the Target Server
#


#****************************************************************************
#
# wtxTargetAttachTest1 - 
#
#

proc wtxTargetAttachTest1 {Msg} {

    global errMiscMsg

    puts stdout "$Msg" nonewline

    set resAttach [wtxTargetAttach]

    if {$resAttach == ""} {
	return
    }

    if {$resAttach != 0} {
	testFail $errMiscMsg(31)
	return
    }

    wtxTestPass
}


#****************************************************************************
#
# wtxTargetAttachTest -
#
# Initialization of variables.
# Initialization of variables that contain the description
# of the tests.
# Call the test scripts.
#

proc wtxTargetAttachTest {tgtsvrName timeout invalidAddr} {

    global tgtAttachMsg


    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxTargetAttachTest targetServerName\
			timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    wtxTargetAttachTest1 $tgtAttachMsg(1)

    # deconnection
    wtxToolDetachCall "wtxTargetAttachTest"
}
