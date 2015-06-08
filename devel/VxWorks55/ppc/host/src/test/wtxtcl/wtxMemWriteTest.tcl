# wtxMemWriteTest.tcl - Tcl script, test WTX_MEM_WRITE
#
# Copyright 1994-1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01t,28jan98,p_b  Adapted for WTX 2.0
# 01s,01oct96,jmp  removed -timeout option of wtxMemWrite.
# 01r,20oct95,jmp  added invalidAddr argument to wtxMemWriteTest.
# 01q,12sep95,jmp  added wtxMemWriteTest1 in external mode.
# 01p,17aug95,jmp  added timeout to wtxToolAttachCall in wtxMemWriteTest.
#                  changed tests control method, now uses wtxErrorHandler.
#                  now uses invalidAddrGet procedure.
# 01o,19jul95,p_m  changed test for write at invalid address to use 
#                  AGENT_MEM_ACCESS_ERROR.
# 01n,23jun95,f_v  added wtxTestRun call
# 01m,09jun95,f_v  added timeout to wtxMemWrite, clean up main procedure
# 01l,10may95,f_v  now uses 0xeeeeeeee like invalid address.
# 01k,27apr95,c_s  now uses wtxErrorMatch.
# 01j,07apr95,f_v  change few error messages
# 01i,30mar95,f_v  replace S_wdb_xx by agentErr
# 01h,17mar95,f_v  change index in wtxTsInfoGet
# 01g,03mar95,f_v  change S_tgtMemLib_XXX by tgtMemErr(XXX)
# 01f,16feb95,kpn  added wtxMemWriteTest4{}, added memBlockDelete call,
#		   Time the execution of  wtxMemWrite().
# 01e,27jan95,kpn  deplaced testcase units description in wtxTestMsg and
#                  testcase units errors in wtxTestErr.tcl,
#                  added global memWriteMsg in the main,
#                  added global errMemMsg in each testcase units,
#                  used memWriteMsg & errMemMsg to manage messages.
# 01d,26jan95,kpn  removed the calls to wtxToolAttachCall and wtxToolDetach
#                  made in each testcase.
#                  Added wtxToolAttachCall and wtxToolDetachCall in the main
#                  procedure.
#		   removed error codes initialization, added global variables.
# 01c,20jan95,kpn  added msge12 initialization in wtxMemWriteTest2.
# 01b,04jan95,kpn  removed parameter to wtxToolDetach (automatic now).
# 01a,15nov94,kpn  written.
#
#

#
# DESCRIPTION
#
# WTX_MEM_WRITE - Write a Block of Memory
#
# TESTS :
#
# This script tests the wtxMemWrite routine.
# 
# (1) wtxMemWriteTest1 - Verify the content of a target memory block
#	The first test allocates a block of memory of 100 bytes and, with
#	the routine wtxMemWrite(), writes 100 bytes at the address of this 
#	allocated memory block. Thanks the wtxMemRead() routine it reads at 
#	this address, verifies there is something written and verifies if what
#	has been writen is good. 
#	Time the execution of  wtxMemWrite().
#
# (2) wtxMemWriteTest2 - Verify wtxMemWrite() writes the rigth number of bytes
#	The second test verifies that when we allocate memory on the target
#	and write only  a part of this memory, the other bytes are not affect 
#	by this writing.
#
# (3) wtxMemWriteTest3 - Verify wtxMemWrite() with invalid <blockId> argument
#	The third test verifies that the result of wtxMemWrite() is "block not
#	found" when this routine is called with an invalid blockId.
#
# (4) wtxMemWriteTest4 - Verify wtxMemWrite() with invalid <addr> argument
#	Verify that the result of wtxMemWrite() is AGENT_MEM_ACCESS_ERROR,
#	when it's called with invalid <address> argument.
#


#*****************************************************************************
#
# wtxMemWriteTest1 - Verify that the memory block has been fill with the 
#		     right value
#
# Allocate a block of memory of 10000 bytes.
# Write 10000 bytes at the address of the allocated memory block.
# Read at this address to verify there is something written and verify 
# if what has been written is good.
# 

proc wtxMemWriteTest1 {Msg} {

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
    # time the execution of wtxMemWrite
    set timeMsr [time {set resMemWrite [wtxMemWrite $blockId $resMemAlloc]} 1]
    if {$resMemWrite == ""} {
        cleanUp mb $blockId ms $resMemAlloc
        return 0
    } 

    # get the average  amount  of  time  required  per iteration for\
    # the execution of wtxMemWrite
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
# wtxMemWriteTest2 - Verify that wtxMemWrite writes the right number of bytes
#
# This test verifes that when we allocate memory on the target and write
# a part of this memory, the other bytes are not affect by this writing. 
#  

proc wtxMemWriteTest2 {Msg} {

 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline

    # create memory block, initialized to 0x00 in each byte 
    set blockId [memBlockCreate -B 150 0x00]

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
    set resMemWrite [wtxMemWrite $blockId $resMemAlloc]
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
    set resMemWrite [wtxMemWrite $blockId2 [expr $resMemAlloc + 49]]
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

    # verify that wtxMemWrite has written the required bytes to the right
    # block of memory 

    for {set i 49} {$i<99} {incr i 1} {
        memBlockSet $blockId $i 0x40
    }

    # verify wtxMemWrite has not affected the bytes before the start 
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
# wtxMemWriteTest3 - test wtxMemWrite with an invalid blockId
#
# Verifies the result of wtxMemWrite when this routine is called with an
# invalid blockId.
#

proc wtxMemWriteTest3 {Msg} {

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
    if {![catch "wtxMemWrite blockIdInv $resMemAlloc" resMemWrite]} {
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
# wtxMemWriteTest4 - Verify wtxMemWrite with invalid <addr> argument
#
# This test calls wtxMemWrite() with the <addr> argument equals to an
# invalid address provided by invalidAddrGet.
# The test verifies that wtxMemWrite has returned an error code, and if
# the returned error code is AGENT_MEM_ACCESS_ERROR (To prevent from
# errors like TIMEOUT.
#

proc wtxMemWriteTest4 {Msg invalidAddr} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    if { $invalidAddr != 0 } {

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
    if {![catch "wtxMemWrite $blockId $invalidAdd" errCode]} {
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
# wtxMemWriteTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
# Furthermore it attaches itself to the target server (thru a wtxToolAttachCall
# command) and, when testcase units have been run, detaches itself from the
# target server.
#

proc wtxMemWriteTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxMemWriteTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    global memWriteMsg

    # connection to the target server
    wtxToolAttachCall $tgtsvrName 2 $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler
    
    # valids tests
    if {[set essai [wtxMemWriteTest1 $memWriteMsg(1)]]} {
	puts stdout "\t(Write 10000 bytes in $essai msec)"
    }
    wtxMemWriteTest2 $memWriteMsg(2)

    wtxErrorHandler [lindex [wtxHandle] 0] ""

    # invalids tests
    wtxMemWriteTest3 $memWriteMsg(3)
    wtxMemWriteTest4 $memWriteMsg(4) $invalidAddr

    # valid test in external mode :
    # if it's possible to change mode to external mode
    # wtxMemReadTest1 is run in external mode
    if {![catch "wtxAgentModeSet 2" agentSt]} {
        wtxErrorHandler [lindex [wtxHandle] 0] errorHandler
        puts stdout "\nSwitch to External Mode :"
        wtxMemWriteTest1 $memWriteMsg(1)
        wtxAgentModeSet 1
        }

    # detach from the target server
    wtxToolDetachCall "\nwtxMemWriteTest : "
}
