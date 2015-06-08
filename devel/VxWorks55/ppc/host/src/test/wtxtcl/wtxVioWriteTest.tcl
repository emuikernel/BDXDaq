# wtxVioWriteTest.tcl - Tcl script, test WTX_VIO_WRITE
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01s,18jan99,p_b  rename wtxCommonProc.tcl in wtxtestlib.tcl
# 01r,28jan98,p_b  Adapted for WTX 2.0
# 01q,20oct95,jmp  added invalidAddr argument to wtxVioWriteTest.
# 01p,21aug95,jmp  changed call of readContextCreate
#		   added timeout to wtxToolAttachCall in wtxVioWriteTest.
#                  changed openVio to vioOpen, emptyVio to vioEmpty.
#                  changed tests control method, now uses wtxErrorHandler.
# 01o,23jun95,f_v  added wtxTestRun call
# 01n,30may95,f_v  update vio use
# 01m,28apr95,c_s  now uses wtxErrorMatch.
# 01l,27apr95,f_v  replace call to funcCall1 by funcCall
# 01k,13apr95,f_v  moved openVio,emptyVio,readContextCreate
#		   to wtxtestlib.tcl
# 01e,07apr95,f_v  change few errMemMsg
# 01d,30mar95,f_v  replace S_wdb_xx by agentErr
# 01c,28mar95,f_v  added wtxRegisterForEvent
# 01e,03mar95,f_v  change S_tgtMemLib_XXX by tgtMemErr(XXX)
# 01d,20feb95,kpn  changed wtxMemZero() -> wtxMemSet(),
#		   replaced wtxFuncCall() and wtxEventPoll by funcCall() or
#		   funcCall3().
#		   added readContextCreate{}.
# 01c,27jan95,kpn  deplaced testcase units description in wtxTestMsg and
#                  testcase units errors in wtxTestErr.tcl,
#                  added global vioWriteMsg in the main,
#                  added global errVioMsg in each testcase units,
#                  used vioWriteMsg & errVioMsg to manage messages.
# 01b,26Jan95,kpn  removed the calls to wtxToolAttachCall and wtxToolDetach
#                  made in each testcase.
#                  Added wtxToolAttachCall and wtxToolDetachCall in the main
#                  procedure.
#		   removed error codes initialization, added global variables.
# 01a,17Jan95,kpn  written.
#
#

#
# DESCRIPTION
#
# WTX_VIO_WRITE - write to a virtual I/O channel
#
# TESTS :
#
# (1) wtxVioWriteTest1 - verify that wtxVioWrite routine writes the right 
#	content and the right bytes number of a memory block to a Target
#	virtual I/O channel.
#
# (2) wtxVioWriteTest2 - verify that wtxVioWrite routine writes the right 
#	content and the right bytes number of a string to a Target virtual
#	I/O channel
#
# (3) wtxVioWriteTest3 - verify that wtxVioWrite returns the error code
#	agentErr(WTX_ERR_AGENT_INVALID_VIO_CHANNEL) when it's called with
#	an invalid <channel> argument.(not implemented)
#


#*************************************************************************
#
# wtxVioWriteTest1 - verify wtxVioWrite routine writes the right content and
#		     the right bytes number of a memory block to a Target
#		     virtual I/O channel
#
# This test opens a virtual I/O file associated with the virtual I/O channel, 
# then it calls the function read() that waits until to be able to read bytes 
# from this virtual I/O. So to unblock it a block of 100 bytes, 
# initialized to 0x20 in each byte, is written to the virtual I/O channel. 
# The function read() is executed : reads bytes from the virtual I/O and 
# places this read bytes in a buffer.
# The test verifies that this buffer that has received the bytes from the 
# virtual I/O contains exactly 100 bytes initialized to 0x20 in each byte.
#

proc wtxVioWriteTest1 {Msg} {


 # GLOBAL VARIABLES

    # array that contain the testcase error codes
    global errVioMsg

    puts stdout "$Msg" nonewline

 # LOCAL VARIABLES

    # channel id for redirection
    set chanId 0
    # mode value for open
    set mode 0
    # max size for buffer that receives read bytes on a virtual I/O
    set readBytesMaxNum 512
    # maximum number of bytes that can be read in one time on a virtual I/O
    set maxBytesRead 512

 # OPEN VIO

    set resVioOpen [vioOpen]

    if {[lindex $resVioOpen 0] == 0} {
        puts stdout $errVioMsg(20)
        return
    }

    # get the file descriptor of the file associated with VIO channel,
    # necessary to can close the file at the end of the test
    set fileDescriptor  [lindex $resVioOpen 0]
    set channelNb  [lindex $resVioOpen 1]

 # EMPTY THE VIO

    set resVioEmpty [vioEmpty $fileDescriptor $readBytesMaxNum $maxBytesRead]
    if {[lindex $resVioEmpty 0] == 0} {
        puts stdout $errVioMsg(19)
	cleanUp fc $fileDescriptor
        return
    }

    set roomDataRead [lindex $resVioEmpty 0]


 # READ DATA FROM VIO
    
    set targetSystemAssignedId [readContextCreate $fileDescriptor $roomDataRead]
    if {$targetSystemAssignedId == 0} {
        puts stdout $errVioMsg(24)
	cleanUp fc $fileDescriptor f $fileIdent
        return
    }


 # WRITE block in VIO

    # create memory block, initialized to 0x20 in each byte, this block will 
    # be written to the virtual I/O channel 
    set memBlockId [memBlockCreate -B 100 0x20]

    # get the contents of this new memory block in a buffer, to can compare
    # it with the bytes that will be written on the target in the allocated
    # room for data read from virtual I/O channel
    set buffer [memBlockGet $memBlockId]

    # write the just created memory block in the virtual I/O channel 
    set numBytesWritten [wtxVioWrite $channelNb -memblk $memBlockId]
    if {$numBytesWritten == ""} {
	cleanUp c $targetSystemAssignedId mb $memBlockId ms $roomDataRead\
		fc $fileDescriptor
        return
    }
    if {$numBytesWritten != 100} {
        testFail $errVioMsg(22)
	cleanUp mb $memBlockId ms $roomDataRead fc $fileDescriptor
        return
    }

    after 1000


 # VERIFY the validity of the data received from VIO

    # read the 100 bytes written at the address $roomDataRead on the target
    set blockRead [wtxMemRead $roomDataRead 100]
    if {$blockRead == ""} {
	cleanUp mb $memBlockId ms $roomDataRead fc $fileDescriptor
        return
    }

    # get the contents of this block
    set blockReadCont [memBlockGet $blockRead]


    # compare the content of this block and the buffer that contains the 
    # initial bytes
    if { $blockReadCont != $buffer } {
	cleanUp mb $memBlockId $blockRead ms $roomDataRead fc $fileDescriptor
        testFail $errVioMsg(7)
        return
    }


 # clean up

    cleanUp mb $memBlockId $blockRead ms $roomDataRead fc $fileDescriptor

    wtxTestPass
}

#*************************************************************************
#
# wtxVioWriteTest2 - verify wtxVioWrite routine writes the right content and
#                    the right bytes number of a string to a Target
#                    virtual I/O channel
#
#

proc wtxVioWriteTest2 {Msg} {

 # GLOBAL VARIABLES

    # array that contain the testcase error codes
    global errVioMsg

    puts stdout "$Msg" nonewline

 # LOCAL VARIABLES

    # channel id for redirection
    set chanId 0
    # mode value for open
    set mode 0
    # max size for buffer that receives read bytes on a virtual I/O
    set readBytesMaxNum 512
    # maximum number of bytes that can be read in one time on a virtual I/O
    set maxBytesRead 512
    # Get close() function address
    set closeAdd [lindex [wtxSymFind -name "close"] 1]


 # OPEN VIO

    set resVioOpen [vioOpen]

    if {[lindex $resVioOpen 0] == 0} {
        puts stdout $errVioMsg(20)
        return
    }

    # get the file descriptor of the file associated with VIO channel,
    # necessary to can close the file at the end of the test
    set fileDescriptor  [lindex $resVioOpen 0]
    set channelNb  [lindex $resVioOpen 1]



 # EMPTY THE VIO

    set resVioEmpty [vioEmpty $fileDescriptor $readBytesMaxNum $maxBytesRead]
    if {[lindex $resVioEmpty 0] == 0} {
        puts stdout $errVioMsg(19)
	cleanUp fc $fileDescriptor
        return
    }

    set roomDataRead [lindex $resVioEmpty 0]


 # READ DATA FROM VIO

    set targetSystemAssignedId [readContextCreate $fileDescriptor $roomDataRead]
    if {$targetSystemAssignedId == 0} {
        puts stdout $errVioMsg(24)
        cleanUp f $fileIdent fc $fileDescriptor
        return
    }


 # WRITE string in VIO

    set stringToWrite "This is a test for wtxVioWrite"
    set stringBlock [memBlockCreate -string $stringToWrite]
    set stringBlockLength [expr [string length $stringToWrite]]
    set stringBlockGet [memBlockGet $stringBlock 0\
    [expr $stringBlockLength - 1]]

    # free memBlock
    cleanUp mb $stringBlock

    # write the same string in the virtual I/O channel
    set numBytesWritten [wtxVioWrite $channelNb -string $stringToWrite]
    if {$numBytesWritten == ""} {
	cleanUp ms $roomDataRead c $targetSystemAssignedId fc $fileDescriptor
        return
    }
    if {$numBytesWritten != $stringBlockLength} {
        testFail $errVioMsg(22)
	cleanUp ms $roomDataRead fc $fileDescriptor
        return
    }

    after 1000


 # VERIFY the validity of the data received from VIO

    # read the bytes written at the address $roomDataRead on the target
    set blockRead [wtxMemRead $roomDataRead $stringBlockLength]
    if {$blockRead == ""} {
	cleanUp ms $roomDataRead fc $fileDescriptor
        return
    }

    # get the contents of this block
    set blockReadCont [memBlockGet $blockRead 0 [expr $stringBlockLength - 1]]

    # compare the content of this block and the buffer that contains the
    # initial bytes
    if {$blockReadCont != $stringBlockGet} {
        testFail $errVioMsg(7)
	cleanUp ms $roomDataRead fc $fileDescriptor
        return
    }


 # clean up

    cleanUp ms $roomDataRead fc $fileDescriptor

    wtxTestPass
}

#*************************************************************************
#
# wtxVioWriteTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
# Furthermore it attaches itself to the target server (thru a wtxToolAttachCall
# command) and, when testcase units have been run, detaches itself from the
# target server.
#

proc wtxVioWriteTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxVioWriteTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    global vioWriteMsg

    # connection to the target server
    wtxToolAttachCall $tgtsvrName 2 $timeout
    # enable all events
    catch {wtxRegisterForEvent .*} st

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler
    
    # call the testcase units
    wtxVioWriteTest1 $vioWriteMsg(1)
    wtxVioWriteTest2 $vioWriteMsg(2)

    # detach from the target server
    wtxToolDetachCall "\nwtxVioWriteTest : "
}
