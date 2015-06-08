# wtxVioReadTest.tcl - Tcl script, test WTX_VIO_READ
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01n,18jan99,p_b  rename wtxCommonProc.tcl in wtxtestlib.tcl
# 01m,28jan98,p_b  Adapted for WTX 2.0
# 01l,20oct95,jmp  added invalidAddr argument to wtxVioReadTest.
# 01k,21aug95,jmp  added timeout to wtxToolAttachCall in wtxVioReadTest.
#                  changed openVio to vioOpen, emptyVio to vioEmpty.
#                  changed tests control method, now uses wtxErrorHandler.
# 01j,23jun95,f_v  added wtxTestRun call
# 01i,21jun95,f_v  clean up main procedure
# 01h,27apr95,c_s  now uses wtxErrorMatch.
# 01g,27apr95,f_v  replace call to funcCall1 by funcCall
# 01f,13apr95,f_v  moved openVio,emptyVio to wtxtestlib.tcl
# 01e,07apr95,f_v  change few errMemMsg
# 01d,30mar95,f_v  replace S_wdb_xx by agentErr
# 01c,28mar95,f_v  added wtxRegisterForEvent
# 01b,20feb95,kpn  changed wtxMemZero -> wtxMemSet
#		   replaced wtxFuncCall() and wtxEventPoll by funcCall() or
#                  funcCall().
# 01a,16feb95,kpn  written.
#
#

#
# DESCRIPTION
#
# WTX_VIO_READ - read from a virtual I/O channel
#
# This routine is not implemented.
#
#  In order to read data from virtual I/O channels  one  should
#  use  the wtxEventGet() routine which will return all virtual
#  I/O data that are not redirected to any file.
#
#
# TESTS :
#
# (1) wtxVioReadTest1 - verify that wtxEventGet() routine allows to
#	get all virtual I/O data.
#
# REMARK : this test will have to be completed when wtxEventGet() will
#	   manage enterly this kind of envent.
#
#

#*************************************************************************
#
# wtxVioReadTest1 - verify that wtxEventGet() routine allows to
#       	    get all virtual I/O data.
#
# First the test allocates a memory block on target and initializes it 
# to 0x20 in each byte. Then it opens a VIO file associated with the
# virtual I/O channel 0 and empties it to make the test robustness.
# The test writes then bytes to the VIO channel 0 with the write() routine
# and reads this data by using the wtxEventGet() routine. The test
# verify that wtxEventGet() routine gets all virtual I/O data.
#

proc wtxVioReadTest1 {Msg} {


 # GLOBAL VARIABLES

    # array that contain the testcase error codes
    global errVioMsg

    puts stdout "$Msg" nonewline

 # LOCAL VARIABLES

    # max size for buffer that receives read bytes on a virtual I/O
    set readBytesMaxNum 512
    # maximum number of bytes that can be read in one time on a virtual I/O
    set maxBytesRead 512


    # create memory block
    set blockId [memBlockCreate -B 100 0x20]

    # get the contents of the new memory block in a buffer, to can compare
    # it with the bytes that will be read from the VIO 
    set buffer [memBlockGet $blockId]

    # Allocate memory on target, return the address of the allocated
    # memory block
    set resMemAlloc [wtxMemAlloc 100]
    if {$resMemAlloc == ""} {
	cleanUp mb $blockId
        return
    }

    # store those 100 bytes to the target starting at $resMemAlloc
    # return the number of bytes actually written.
    set resMemWrite [wtxMemWrite $blockId $resMemAlloc]
    if {$resMemWrite == ""} {
	cleanUp mb $blockId ms $resMemAlloc
        return
    }

    # free all memBlocks
    cleanUp mb $blockId

 # OPEN VIO

    set resVioOpen [vioOpen]
    if {[lindex $resVioOpen 0] == 0} {
        puts stdout $errVioMsg(20)
	cleanUp ms $resMemAlloc
        return
    }

    # get the file descriptor of the file associated with VIO channel 0,
    # necessary to can close the file at the end of the test
    set fileDescriptor  [lindex $resVioOpen 0]


 # EMPTY THE VIO

    set resVioEmpty [vioEmpty $fileDescriptor $readBytesMaxNum $maxBytesRead]
    if {[lindex $resVioEmpty 0] == 0} {
        puts stdout $errVioMsg(19)
	cleanUp ms $resMemAlloc fc $fileDescriptor
        return
    }

    set roomDataRead [lindex $resVioEmpty 0]


 # WRITE bytes to the VIO channel 0

    set numBytesWritten [funcCall write $fileDescriptor $resMemAlloc 100]

    # verify the number of bytes written
    if {$numBytesWritten != 100} {
        testFail $errVioMsg(23)
	cleanUp ms $resMemAlloc $roomDataRead fc $fileDescriptor
	return
    }


 # TO COMPLETE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



 # READ bytes written from VIO channel 0

    set vioBytesRead [wtxEventGet]

    while {[string compare [lindex $vioBytesRead  0] VIO_WRITE]} {
	set vioBytesRead [wtxEventGet]
    }


 #  clean up

    cleanUp ms $resMemAlloc $roomDataRead fc $fileDescriptor

    wtxTestPass
}

#*************************************************************************
#
# wtxVioReadTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
# Furthermore it attaches itself to the target server (thru a wtxToolAttachCall
# command) and, when testcase units have been run, detaches itself from the
# target server.
#

proc wtxVioReadTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxVioReadTest targetServerName\
				timeout invalidAddr"
        return
    }

    global vioReadMsg

    # connection to the target server
    wtxToolAttachCall $tgtsvrName 2 $timeout
    # enable all events
    catch {wtxRegisterForEvent .*} st

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # call the testcase units
    #wtxVioReadTest1 $vioReadMsg(1)
    puts stdout "\tTest not available for the moment"

    # detach from the target server
    wtxToolDetachCall "\nwtxVioReadTest : "
}
