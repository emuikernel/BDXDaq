# wtxMemAllocTest.tcl - Tcl script, test WTX_MEM_ALLOC 

# Copyright 1994-1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01o,28jan98,p_b  Adapted for WTX 2.0
# 01n,26feb96,l_p  corrected English mistakes in a few messages.
# 01m,20oct95,jmp  added invalidAddr argument to wtxMemAllocTest.
# 01l,17aug95,jmp  added timeout to wtxToolAttachCall in wtxMemAllocTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01k,23jun95,f_v  added wtxTestRun call
# 01j,21jun95,f_v  clean up main procedure
# 01i,27apr95,c_s  now uses wtxErrorMatch.
# 01h,07apr95,f_v  replace errMemMsg
# 01g,03mar95,f_v  replace S_tgtMemLib_NOT_ENOUGH_MEMORY by
#		   tgtMemErr(WTX_ERR_TGTMEM_NOT_ENOUGH_MEMORY)
#		   replace S_tgtMemLib_BLOCK_ERROR by
#		   tgtMemErr(WTX_ERR_TGTMEM_INVALID_BLOCK)
# 01f,20feb95,kpn  Time the execution of  wtxMemAlloc().
# 01e,27jan95,kpn  deplaced testcase units description in wtxTestMsg and
#                  testcase units errors in wtxTestErr.tcl,
#                  added global memAllocMsg in the main,
#                  added global errMemMsg in each testcase units,
#		   used memAllocMsg & errMemMsg to manage messages.
# 01d,26jan95,kpn  removed the calls to wtxToolAttachCall and wtxToolDetach
#                  made in each testcase.
#                  Added wtxToolAttachCall and wtxToolDetachCall in the main
#                  procedure.
#		   removed error codes initialization, added global variable.
# 01c,20jan95,kpn  managed msge array.
# 01b,04jan95,kpn  removed parameter to wtxToolDetach (automatic now).
# 01a,15nov94,kpn  written.
#
#


#
# DESCRIPTION
#
# WTX_MEM_ALLOC - Allocate memory on target
#
# TESTS :
#
# (1) wtxMemAllocTest1 - Test wtxMemAlloc with valid parameter.
#	This script tests wtxMemAlloc() with a valid <size> argument. This 
#	routine must return the address of the allocated memory block. To
#	verify if this address is valid, the script tests that wtxMemFree()
#	can de-allocates the memory block at this address. wtxMemFree()
#	must return 0 if the memory block address is valid.
#	Time the execution of  wtxMemAlloc().
#
# (2) wtxMemAllocTest2 - Test wtxMemAlloc with invalid <size> parameter.
#	Verify that the result of wtxMemAlloc() is
#	tgtMemErr(WTX_ERR_TGTMEM_NOT_ENOUGH_MEMORY)
#
#



#*****************************************************************************
#
# wtxMemAllocTest1 - Test wtxMemAlloc with valid parameter
#
# This script tests wtxMemAlloc() with a valid <size> argument. This routine
# must return the address of the allocated memory block. To verify if this
# address is valid, the script tests that wtxMemFree() can de-allocates the 
# memory block at this address. wtxMemFree() must return 0 if the memory
# block address is valid.
#

proc wtxMemAllocTest1 {Msg} {

 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline

    # LOCAL VARIABLES
    set validSize 10000

    # Allocate memory on target (10000 bytes)
    # time the execution of wtxMemAlloc()
    set timeMsr [time {set resMemAlloc [wtxMemAlloc $validSize]} 1] 
    if {$resMemAlloc == ""} {
        return 0
    } 


    # get the average  amount  of  time  required  per iteration for\
    # the execution of wtxMemAlloc()
    set timeMsr [lindex $timeMsr 0]
    # make a conversion between micro seconds and milli seconds
    set timeMsrSec [ expr $timeMsr/1000]

    set freeMemBefore [lindex [wtxMemInfoGet] 0]

    # verify that the address returned by wtxMemAlloc() is valid
    set resMemFree [wtxMemFree $resMemAlloc]
    if {$resMemFree ==""} {
        return 0
    } 

    set freeMemAfter [lindex [wtxMemInfoGet] 0]

    if {$freeMemBefore == $freeMemAfter } {
        testFail $errMemMsg(20) 
        return 0
    }
 
    wtxTestPass
    return $timeMsrSec
}


#*****************************************************************************
#
# wtxMemAllocTest2 - Test wtxMemAlloc with invalid <size> parameter
#
# This procedure verifies that wtxMemAlloc() returns the error code 
# tgtMemErr(WTX_ERR_TGTMEM_NOT_ENOUGH_MEMORY) when it's called with an
# invalid <size> parameter. 
#

proc wtxMemAllocTest2 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline

    set invalidSize -400

    # if no exception occurred
    if {![catch "wtxMemAlloc $invalidSize" resMemAlloc]} {
	testFail $errMemMsg(3)
	return
	}

    # if the caught exception is different than the expected error code 
    if {![wtxErrorMatch $resMemAlloc TGTMEM_NOT_ENOUGH_MEMORY]} {
	testFail $resMemAlloc
	return
	}
	
	
    # if the caught exception is the expected error code
    wtxTestPass
}


#*****************************************************************************
#
# wtxMemAllocTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
#

proc wtxMemAllocTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxMemAllocTest targetServerName\
				timeout invalidAddr"
        return
    }

    global memAllocMsg

    # connection to the target server
    wtxToolAttachCall $tgtsvrName 2 $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # call the tests with valid parameters 
    if {[set essai [wtxMemAllocTest1 $memAllocMsg(1)]]} {
    	puts stdout "\t(Allocate a 10000 bytes memory block\
		in $essai msec)"
	}


    # call the tests with invalid parameters 
    wtxErrorHandler [lindex [wtxHandle] 0] ""
    wtxMemAllocTest2 $memAllocMsg(2) 

    # detach from the target server
    wtxToolDetachCall "\nwtxMemAllocTest : "
}
