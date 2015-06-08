# wtxMemWidthWriteTest.tcl - Tcl script, test WTX_MEM_WIDTH_WRITE
#
# Copyright 1998 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01a,28jan98,p_b  Complete import from wtxMemWriteTest, plus modif.

#
# DESCRIPTION
#
# WTX_MEM_WIDTH_WRITE - Write a Block of Memory with WIDTH = 1,2 & 4
#
# TESTS :
#
# This script tests the wtxMemWidthWrite routine.
# 
# (1) wtxMemWidthWriteTest1 - Verify the content of a target memory block
#	The first test allocates a block of memory of 100 bytes and, with
#	the routine wtxMemWidthWrite(), writes 100 bytes at the address of this 
#	allocated memory block. Thanks the wtxMemRead() routine it reads at 
#	this address, verifies there is something written and verifies if what
#	has been writen is good. 
#	Time the execution of  wtxMemWidthWrite().
#
# (2) wtxMemWidthWriteTest2 - Verify wtxMemWidthWrite() writes the rigth number of bytes
#	The second test verifies that when we allocate memory on the target
#	and write only  a part of this memory, the other bytes are not affect 
#	by this writing.
#
# (3) wtxMemWidthWriteTest3 - Verify wtxMemWidthWrite() with invalid <blockId> argument
#	The third test verifies that the result of wtxMemWidthWrite() is "block not
#	found" when this routine is called with an invalid blockId.
#
# (4) wtxMemWidthWriteTest4 - Verify wtxMemWidthWrite() with invalid <addr> argument
#	Verify that the result of wtxMemWidthWrite() is AGENT_MEM_ACCESS_ERROR,
#	when it's called with invalid <address> argument.
#


#*****************************************************************************
#
# wtxMemWidthWriteTest1 - Verify whether the memory block has been filled with the 
#		     right values
#
# Allocate a block of memory of 10000 bytes.
# Write 10000 bytes at the address of the allocated memory block.
# Read at this address to verify there is something written and verify 
# if what has been written is good.
# 

proc wtxMemWidthWriteTest1 {nBytes Msg} {

 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline

    # create memory block
    set blockId [memBlockCreate -B 10000 0x20]

    # Allocate memory on target, return the address of the allocated
    # memory block
    set resMemAlloc [wtxMemAlloc 10000]
    if {$resMemAlloc == ""} {
        cleanUp mb $blockId
        return 0
    }

    # store those 10000 bytes to the target starting at $resMemAlloc
    # time the execution of wtxMemWidthWrite
    set timeMsr [time {set resMemWrite [wtxMemWidthWrite $blockId $resMemAlloc $nBytes]} 1]

    if {$resMemWrite == ""} {
        cleanUp mb $blockId ms $resMemAlloc
        return 0
    } 

    # get the average  amount  of  time  required  per iteration for\
    # the execution of wtxMemWidthWrite
    set timeMsr [lindex $timeMsr 0]
    # make a conversion between micro seconds and milli seconds
    set timeMsrSec [ expr $timeMsr/1000]

    # verify if what we have written at $resMemAlloc in the target is right 

    # read the 10000 bytes written at the address $resMemAlloc
    set mblock [wtxMemRead $resMemAlloc 10000]
    if {$mblock == ""} {
        cleanUp mb $blockId ms $resMemAlloc
        return 0
    } 

    # compare the block contents and the buffer that contains the initial
    # bytes

    if {[memBlockGet $mblock] != [memBlockGet $blockId]} {
        cleanUp mb $blockId $mblock ms $resMemAlloc
	testFail $errMemMsg(14)
	return 0
    } 

    cleanUp mb $blockId $mblock ms $resMemAlloc

    wtxTestPass

    return $timeMsrSec 
}

#*****************************************************************************
#
# wtxMemWidthWriteTest2 - Verify that wtxMemWidthWrite writes the right number of bytes
#
# This test verifes that when we allocate memory on the target and write
# a part of this memory, the other bytes are not affect by this writing. 
#  

proc wtxMemWidthWriteTest2 {nBytes Msg} {

 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline

    # create memory block, initialized to  in each byte 
    set blockId [memBlockCreate -B 150 ]

    # give a different value for each byte of the block (0x10 -> 0xa5)
    for {set i 0} {$i<150} {incr i 1} {
        memBlockSet $blockId $i [expr 0x10 + $i]
    }

    # Allocate memory on target, return the address of the allocated
    # memory block
    set resMemAlloc [wtxMemAlloc 150]
    if {$resMemAlloc == ""} {
        cleanUp mb $blockId
        return
    }

    # store those 150 bytes to the target starting at $resMemAlloc
    # return the number of bytes actually written.
    set resMemWrite [wtxMemWidthWrite $blockId $resMemAlloc $nBytes]
    if {$resMemWrite == ""} {
        cleanUp mb $blockId ms $resMemAlloc
        return
    }

    # read the 150 bytes written at the address $resMemAlloc
    set mblock [wtxMemRead $resMemAlloc 150]
    if {$mblock == ""} {
        cleanUp mb $blockId ms $resMemAlloc
        return
    }

    # compare the block contents and the buffer that contains the initial
    # bytes
    if {[memBlockGet $mblock] != [memBlockGet $blockId]} {
        testFail $errMemMsg(14)
        cleanUp mb $blockId $mblock ms $resMemAlloc
        return
    }

    # create memory block, initialized to 0x40 in each byte
    set blockId2 [memBlockCreate -B 50 0x40]

    # store those 50 bytes to the target starting at $resMemAlloc + 49
    set resMemWrite [wtxMemWidthWrite $blockId2 [expr $resMemAlloc + 49]  4]
    if {$resMemWrite == ""} {
        cleanUp mb $blockId $mblock $blockId2 ms $resMemAlloc
        return
    }

    # read the 150 bytes written at the address $resMemAlloc
    set mblock [wtxMemRead $resMemAlloc 150]
    if {$mblock == ""} {
	cleanUp mb $blockId $mblock $blockId2 ms $resMemAlloc
        return
    }

    # verify that wtxMemWidthWrite has written the required bytes to the right
    # block of memory 

    for {set i 49} {$i<99} {incr i 1} {
        memBlockSet $blockId $i 0x40
    }

    # verify wtxMemWidthWrite has not affected the bytes before the start 
    # address and after the end address
    if {[memBlockGet $mblock] != [memBlockGet $blockId]} {
        testFail $errMemMsg(15)
	cleanUp mb $blockId $mblock $blockId2 ms $resMemAlloc
        return
    }

    # clean up
    cleanUp  mb $blockId $mblock $blockId2 ms $resMemAlloc

    wtxTestPass
}


#*****************************************************************************
#
# wtxMemWidthWriteTest3 - test wtxMemWidthWrite with an invalid blockId
#
# Verifies the result of wtxMemWidthWrite when this routine is called with an
# invalid blockId.
#

proc wtxMemWidthWriteTest3 {nBytes Msg} {

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline

    # Allocate memory on target, return the address of the allocated
    # memory block
    if {[catch "wtxMemAlloc 100" resMemAlloc]} {
        testFail $resMemAlloc
        return
    }

    # store an non-existant block to the target starting at $resMemAlloc

    # if no exception occurred
    if {![catch "wtxMemWidthWrite blockIdInv $resMemAlloc $nBytes" resMemWrite]} {
	testFail $errMemMsg(16)
	cleanUp ms $resMemAlloc
	return
	}

    # if the caught exception is different than the expected error code
    if {$resMemWrite != "block not found"} {
	cleanUp ms $resMemAlloc
	testFail $resMemWrite
        return
    } 

    # clean up
    cleanUp ms $resMemAlloc

    # if the caught exception is the expected error code
    wtxTestPass
}


#*****************************************************************************
#
# wtxMemWidthWriteTest4 - Verify wtxMemWidthWrite with invalid <addr> argument
#
# This test calls wtxMemWidthWrite() with the <addr> argument equals to an
# invalid address provided by invalidAddrGet.
# The test verifies that wtxMemWidthWrite has returned an error code, and if
# the returned error code is AGENT_MEM_ACCESS_ERROR (To prevent from
# errors like TIMEOUT.
#

proc wtxMemWidthWriteTest4 {nBytes Msg invalidAddr} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

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

    puts stdout "$Msg" nonewline

    # create memory block
    set blockId [memBlockCreate -B 100 0x20]

    # store those 100 bytes to the target starting at invalidAdd

    # if no exception occurred
    if {![catch "wtxMemWidthWrite $blockId $invalidAdd $nBytes" errCode]} {
	testFail $errMemMsg(8)
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $errCode AGENT_MEM_ACCESS_ERROR]} {
	testFail $errCode
	return
    }

    # clean up
    cleanUp mb $blockId 

    # if the caught exception is the expected error code
    wtxTestPass
}
 

#*****************************************************************************
#
# wtxMemWidthWriteTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
# Furthermore it attaches itself to the target server (thru a wtxToolAttachCall
# command) and, when testcase units have been run, detaches itself from the
# target server.
#

proc wtxMemWidthWriteTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxMemWidthWriteTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    global memWidthWriteMsg

    # connection to the target server
    wtxToolAttachCall $tgtsvrName 2 $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    set NBTESTS 12
    foreach nBytes {1 2 4} {
    puts stdout "wtxMemWidthWrite with <nBytes=$nBytes>"

    # valids tests
    if {[set essai [wtxMemWidthWriteTest1 $nBytes $memWidthWriteMsg(1)]]} {
    	puts stdout "\t(Write 10000 bytes in $essai msec)"
    }

    wtxMemWidthWriteTest2 $nBytes $memWidthWriteMsg(2)

    wtxErrorHandler [lindex [wtxHandle] 0] ""

    # invalids tests
    wtxMemWidthWriteTest3 $nBytes $memWidthWriteMsg(3)
    wtxMemWidthWriteTest4 $nBytes $memWidthWriteMsg(4) $invalidAddr

    # valid test in external mode :
    # if it's possible to change mode to external mode
    # wtxMemReadTest1 is run in external mode
    if {![catch "wtxAgentModeSet 2" agentSt]} {
       wtxErrorHandler [lindex [wtxHandle] 0] errorHandler
       puts stdout "\nSwitch to External Mode :"
       wtxMemWidthWriteTest1 $nBytes $memWidthWriteMsg(1)
       wtxAgentModeSet 1
       }
    }

    # detach from the target server
    wtxToolDetachCall "\nwtxMemWidthWriteTest : "
}
