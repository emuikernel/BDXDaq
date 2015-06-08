# wtxMemReallocTest.tcl - Tcl script, test WTX_MEM_REALLOC 
#
# Copyright 1994-1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01i,28jan98,p_b  Adapted for WTX 2.0
# 01h,20oct95,jmp  added invalidAddr argument to wtxMemReallocTest.
# 01g,13sep95,jco  reconnected wtxMemReallocTest1 with smaller size.
# 01f,09aug95,jmp  added tests with valid size smaller than previous
#		   ,equal to previous and bigger to previous.
#		   !!! REMARK : wtxMemReallocTest1 with a smaller size
#		   than previous value doesn't pass for the moment.
#		   added timeout to wtxToolAttachCall in wtxMemReallocTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01e,23jun95,f_v  added wtxTestRun call
# 01d,21jun95,f_v  clean up main procedure
# 01c,28apr95,c_s  now uses wtxErrorMatch.
# 01b,07apr95,f_v  change errMemMsg.
# 01a,28mar95,f_v  written.
#
#


#
# DESCRIPTION
#
# WTX_MEM_REALLOC - re-allocates memory on target
#
# TESTS :
#
# (1) wtxMemReallocTest1 - Test wtxMemRealloc with valid parameter.
#	This script tests wtxMemRealloc() with a valid <size>and <address>
#	argument. This routine must return the address of the re-allocated
#	memory block.
#
#	To verify if wtxMemRealloc work correctly, the script tests if the
#	size of the free memory is the same than before the start of the test.
#	To verify if this address is valid, the script tests that wtxMemFree()
#	can de-allocates the memory block at this address.
#	This test times the execution of  wtxMemAlloc().
#
#	This test is called three times :
#		- with a valid <size> smaller than previous value.
#		- with a valid <size> equal to previous value.
#		- with a valid <size> bigger than previous value.
#
#
# (2) wtxMemReallocTest2 - Test wtxMemRealloc with invalid <size> parameter.
#	Verify that the result of wtxMemRealloc() is
#	tgtMemErr(WTX_ERR_TGTMEM_NOT_ENOUGH_MEMORY)
#
# (3) wtxMemReallocTest3 - Test wtxMemRealloc with invalid <address> parameter.
#	To obtain a invalid address this test allocates a block of memory, then
#	free this block with wtxMemFree() and try to re-allocates the previously
#	block. Verify that the result of wtxMemRealloc() is
#	tgtMemErr(WTX_ERR_TGTMEM_INVALID_BLOCK)
#
#



#*****************************************************************************
#
# wtxMemReallocTest1 - Test wtxMemRealloc with valid parameter
#
# This script tests wtxMemRealloc() with a valid <size> and <Address> argument.
# This routine must return the address of the allocated memory block.
#
# To verify if wtxMemRealloc work correctly, the script tests if the
# size of the free memory is the same than before the start of the test.
# To verify if this address is valid, the script tests that wtxMemFree()
# can de-allocates the memory block at this address.
# This test times the execution of  wtxMemAlloc().
#
# This test is called three times :
#	- with a valid <size> smaller than previous value.
#	- with a valid <size> equal to previous value.
#	- with a valid <size> bigger than previous value.
#

proc wtxMemReallocTest1 {newSize Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline

 # LOCAL VARIABLES
    set validSize 10000

    # Get the size of free memory before the test
    set memFreeSizeBefore [lindex [wtxMemInfoGet] 0]

    # Allocate memory on target (10000 bytes)
    # time the execution of wtxMemAlloc()
    set timeMsr [time {set bAddrs [wtxMemAlloc $validSize]} 1] 
    if {$bAddrs == ""} {
        return
    } 

    # get the average  amount  of  time  required  per iteration for\
    # the execution of wtxMemAlloc()
    set timeMsr [lindex $timeMsr 0]

    # make a conversion between micro seconds and milli seconds
    set timeMsrSec [ expr $timeMsr/1000]
    set memAllocMsgLocal(0) "Allocate a block of memory of 10000 bytes in\
    					$timeMsrSec milli-seconds"

    # re-allocate memory block with a new size
    set bAddrs [wtxMemRealloc $bAddrs $newSize]
    if {$bAddrs == ""} {
	cleanUp ms $bAddrs
	return
	}

    # verify that the address returned by wtxMemRealloc() is valid
    set resMemFree [wtxMemFree $bAddrs]
    if {$resMemFree == ""} {
	cleanUp ms $bAddrs
        return
    } 

 
    # Get the size of free memory after the test
    set memFreeSizeAfter [lindex [wtxMemInfoGet] 0]

    # Verify if the size of free memory is the same than before
    # the test.

    if {$memFreeSizeAfter != $memFreeSizeBefore} {
        testFail $errMemMsg(7)
        return
    }

    wtxTestPass 

    return $memAllocMsgLocal(0)
}


#*****************************************************************************
#
# wtxMemReallocTest2 - Test wtxMemRealloc with invalid <size> parameter
#
# This procedure verifies that wtxMemRealloc() returns the error code 
# tgtMemErr(WTX_ERR_TGTMEM_NOT_ENOUGH_MEMORY) when it's called with an
# invalid <size> parameter. 
#

proc wtxMemReallocTest2 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline

 # LOCAL VARIABLES

    set validSize   1000
    set invalidSize -400

    # allocate a block of memory
    if {[catch "wtxMemAlloc $validSize" bAddrs]} {
        testFail $bAddrs
        return
    }

    # try to re-allocates memory block with an invalid size
    # if no exception occurred
    if {![catch {wtxMemRealloc $bAddrs $invalidSize} bAddrs2]} {
	testFail $errMemMsg(3)
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $bAddrs2 TGTMEM_NOT_ENOUGH_MEMORY]} {
        testFail $bAddrs2
	cleanUp ms $bAddrs
        return
    } 

    cleanUp ms $bAddrs

    # if the caught exception is the expected error code
    wtxTestPass
}



#*****************************************************************************
#
# wtxMemReallocTest3 - Test wtxMemRealloc with invalid <Addrs> parameter
#
# This procedure verifies that wtxMemRealloc() returns the error code 
# tgtMemErr(WTX_ERR_TGTMEM_INVALID_BLOCK) when it's called with an
# invalid <Addrs> parameter. 
#

proc wtxMemReallocTest3 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline

 # LOCAL VARIABLES

    set validSize   1000

    # allocate a block of memory
    if {[catch "wtxMemAlloc $validSize" bAddrs]} {
        testFail $bAddrs
        return
    }

    # free the memory block
    if {[catch "wtxMemFree $bAddrs" status]} {
        testFail $status
        return
    }

    # try to re-allocates memory block with an invalid address
    # if no exception occurred
    if {![catch {wtxMemRealloc $bAddrs $validSize} bAddrs]} {
	testFail $errMemMsg(8)
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $bAddrs TGTMEM_INVALID_BLOCK]} {
        testFail $bAddrs
        return
       } 

    # if the caught exception is the expected error code
    wtxTestPass
}


#*****************************************************************************
#
# wtxMemReallocTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
#

proc wtxMemReallocTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxMemReallocTest targetServerName\
				timeout invalidAddr"
        return
    }

    global memReallocMsg

    set smallerSize	500
    set equalSize	10000
    set biggerSize	10500

    # valids tests 
    wtxToolAttachCall $tgtsvrName 2 $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    wtxMemReallocTest1 $smallerSize $memReallocMsg(1)

    wtxMemReallocTest1 $equalSize $memReallocMsg(2)
    wtxMemReallocTest1 $biggerSize $memReallocMsg(3) 

    
    # invalids tests

    wtxErrorHandler [lindex [wtxHandle] 0] ""
    wtxMemReallocTest2 $memReallocMsg(4) 
    wtxMemReallocTest3 $memReallocMsg(5) 

    # detach from the target server
    wtxToolDetachCall "\nwtxMemReallocTest : "
}
