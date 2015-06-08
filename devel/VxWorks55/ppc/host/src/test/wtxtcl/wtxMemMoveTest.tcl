# wtxMemMoveTest.tcl - Tcl script, test WTX_MEM_MOVE
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01i,28jan98,p_b  Adapted for WTX 2.0
# 01h,07nov95,jmp  removed wtxMemMoveTest2 with a huge argument.
# 01g,20oct95,jmp  added invalidAddr argument to wtxMemMoveTest.
# 01f,09aug95,jmp  added a test in wtxMemMoveTest1 to verify that
#		   there isn't erased bytes, added wtxMemMoveTest2.
#		   added timeout to wtxToolAttachCall in wtxMemMoveTest.
#		   changed tests control method, now uses wtxErrorHandler.
#                  now uses invalidAddrGet procedure.
# 01e,23jun95,f_v  added wtxTestRun call
# 01d,21jun95,f_v  clean up main procedure
# 01c,27apr95,c_s  now uses wtxErrorMatch.
# 01b,07apr95,f_v  change errMemMsg(40) to agentErrMsg.
#		   test mem Alloc individually
# 01a,29mar95,f_v  written.
#
#

#
# DESCRIPTION
#
# WTX_MEM_MOVE - Move a block of target memory
#
# TESTS :
#
# (1) wtxMemMoveTest1 - verify the value returned by wtxMemMove() when this 
#	routine move a block of memory previously allocated with
#	wtxMemAlloc and initialized at 0x45. So verify wtxMemMove() with a
#	valid memory block address by checking two block of memory.
#	Verify if wtxMemMove hasn't erased bytes before the start address and
#	after the end address of the moved block.
#
#
# (2) wtxMemMoveTest2 - verify wtxMemMove() with invalid <srcAddr> argument,
#	verify wtxMemMove() with invalid <dstAddr> argument,
#	Verify that the result of wtxMemMove() is AGENT_MEM_ACCESS_ERROR.
#
#



#*****************************************************************************
#
# wtxMemMoveTest1 - verify wtxMemMove() with a valid memory block address
#
# Verify wtxMemMove moves a block of memory previously allocated with
# wtxMemAlloc. wtxMemMove must return zero and initial block initialised at
# 0x45 and moved block should be egal.
# Verify if wtxMemMove hasn't erased bytes before the start address and
# after the end address of the moved block.
#

proc wtxMemMoveTest1 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    set blockSize   1000
    set sizeToMove  100
    set controlSize 10

    puts stdout "$Msg" nonewline

    # Allocate a fisrt block of memory on target
    set bAddrs1 [wtxMemAlloc $blockSize]
    if {$bAddrs1 == ""} {
	return
	}

    # Allocate a second block of memory on target
    set bAddrs2 [wtxMemAlloc $blockSize]
    if {$bAddrs2 == ""} {
	cleanUp ms $bAddrs1
        return
        }

    set resMemSet [wtxMemSet $bAddrs1 $blockSize 0x45454545]
    if {$resMemSet == ""} {
	cleanUp ms $bAddrs1 $bAddrs2
        return
        }

    set addToMove1 [format 0x%x [expr $bAddrs1 + 100]]
    set addToMove2 [format 0x%x [expr $bAddrs2 + 100]]
    set addToControl1 [format 0x%x [expr $addToMove2 - $controlSize]]
    set addToControl2 [format 0x%x [expr $addToMove2 + $sizeToMove]]

    # Read memory blocks to control before "wtxMemMove"
    set ctrl1Before [wtxMemRead $addToControl1 $controlSize]
    if {$ctrl1Before == ""} {
	cleanUp ms $bAddrs1 $bAddrs2
	return
	}

    set ctrl2Before [wtxMemRead $addToControl2 $controlSize]
    if {$ctrl2Before == ""} {
	cleanUp ms $bAddrs1 $bAddrs2
	return
	}

    # move a memory block from addToMove1 to addToMove2
    set st [wtxMemMove $addToMove1 $addToMove2 $sizeToMove]
    if {$st == ""} {
	cleanUp ms $bAddrs1 $bAddrs2
        return
        }

    # Read memory blocks
    set bId1 [wtxMemRead $addToMove1 $sizeToMove]
    if {$bId1 == ""} {
	cleanUp ms $bAddrs1 $bAddrs2
	return
	}
    set bId2 [wtxMemRead $addToMove2 $sizeToMove]
    if {$bId2 == ""} {
	cleanUp ms $bAddrs1 $bAddrs2
	return
	}
    set ctrl1After [wtxMemRead $addToControl1 $controlSize]
    if {$ctrl1After == ""} {
	cleanUp ms $bAddrs1 $bAddrs2
	return
	}
    set ctrl2After [wtxMemRead $addToControl2 $controlSize]

    if {[memBlockGet $bId1] != [memBlockGet $bId2] ||\
	[memBlockGet $ctrl1After] != [memBlockGet $ctrl1Before] ||\
	[memBlockGet $ctrl2After] != [memBlockGet $ctrl2Before]} { 
        cleanUp ml ms $bAddrs1 $bAddrs2
        testFail $errMemMsg(4)
        return
    }

    cleanUp ml ms $bAddrs1 $bAddrs2 
    wtxTestPass 
}


#*****************************************************************************
#
# wtxMemMoveTest2 - verify wtxMemMove() with invalid arguments.
#
# This test calls wtxMemMove() with 
#		- the <srcAddr> equal to an invalid address provided
#		  by invalidAddrGet.
#		- the <dstAddr> equal to an invalid address provided
#                 by invalidAddrGet.
#
# In each of these case, the test verifies that wtxMemMove has returned an
# error code, and if the returned error code is AGENT_MEM_ACCESS_ERROR (To
# prevent from errors like TIMEOUT.
#
proc wtxMemMoveTest2 {Msg invalidAddr} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    set validSize   1000
    
    # set invalid arguments
    if {$invalidAddr != 0} {

	# if the test suite has been run with the -SUPPRESS option
        if {$invalidAddr == "SUPPRESS"} {
            return
        }
    
        set invalidAdd $invalidAddr

    } else {

        # get an invalid address
        set invalidAdd [invalidAddrGet]
    }

    set invalidSrcAddr  $invalidAdd
    set invalidDstAddr  $invalidAdd

    puts stdout "$Msg" nonewline

    # Allocate a fisrt block of memory on target
    if {[catch "wtxMemAlloc $validSize" validAddr1]} {
        testFail $validAddr1
        return
	}

    # Allocate a second block of memory on target
    if {[catch "wtxMemAlloc $validSize" validAddr2]} {
        cleanUp ms $validAddr1
        testFail $validAddr2
        return
	}

    # wtxMemMove with the <srcAddr> equal to [invalidAddrGet]
    # if no exception occurred
    if {![catch "wtxMemMove $invalidSrcAddr $validAddr2 $validSize" status]} {
	testFail $errMemMsg(1)
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $status AGENT_MEM_ACCESS_ERROR]} {
        cleanUp ms $validAddr1 $validAddr2
        testFail $status
        return
	}

    # wtxMemMove with the <dstAddr> equal to [invalidAddrGet]
    # if no exception occurred
    if {![catch "wtxMemMove $validAddr1 $invalidDstAddr $validSize" status]} {
	cleanUp ms $validAddr1 $validAddr2
	testFail $errMemMsg(2)
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $status AGENT_MEM_ACCESS_ERROR]} {
        cleanUp ms $validAddr1 $validAddr2
        testFail $status
        return
    }

    cleanUp ms $validAddr1 $validAddr2
    wtxTestPass 
}


#*****************************************************************************
#
# wtxMemMoveTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
# Furthermore it attaches itself to the target server (thru a wtxToolAttachCall
# command) and, when testcase units have been run, detaches itself from the
# target server.
#

proc wtxMemMoveTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxMemMoveTest targetServerName\
				timeout invalidAddr"
        return
    }

    # tests description
    global memMoveMsg


    # valids tests
    wtxToolAttachCall $tgtsvrName 2 $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler
    wtxMemMoveTest1 $memMoveMsg(1) 

    # invalids tests
    wtxErrorHandler [lindex [wtxHandle] 0] ""
    wtxMemMoveTest2 $memMoveMsg(2) $invalidAddr
    wtxToolDetachCall "\nwtxMemMoveTest : "
}
