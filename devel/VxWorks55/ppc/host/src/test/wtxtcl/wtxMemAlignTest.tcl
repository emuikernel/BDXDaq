# wtxMemAlignTest.tcl - Tcl script, test WTX_MEM_ALIGN
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01h,28jan98,p_b  Adapted for WTX 2.0
# 01g,20oct95,jmp  added invalidAddr argument to wtxMemAlignTest.
# 01f,17aug95,jmp  added timeout to wtxToolAttachCall in wtxMemAlignTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01e,23jun95,f_v  added wtxTestRun call
# 01d,15may95,f_v  added wtxMemAlignTest2
# 01c,27apr95,c_s  now uses wtxErrorMatch.
# 01b,07apr95,f_v  change errMemMsg.
# 01a,28mar95,f_v  written.
#
#

#
# DESCRIPTION
#
# WTX_MEM_ALIGN - allocates memory on target aligned on a specified boundary
#
# TESTS :
#
# (1) wtxMemAlignTest1 - verify the value returned by wtxMemFree() when this 
#	routine de-allocates a block of memory previously allocated with
#	wtxMemAlloc. So verify wtxMemFree() with a valid memory block 
#	address.
#
# (2) wtxMemAlignTest1 - verify the error code returned are right this test 
#    	check bad alignment and memory size too big.
#



#*****************************************************************************
#
# wtxMemAlignTest1 - verify wtxMemAlign with a valid bytes number and alignment
#
# Verify wtxMemAlign return an adress which is multiple of alignment
#

proc wtxMemAlignTest1 {Msg} {

 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline

    # align a block of memory
    set bAdrs [wtxMemAlign 16 1000]
    if {$bAdrs == ""} {
        return
    }

    # test if bAdrs is a multiple of 16
    set result [expr $bAdrs % 16]

    if { $result != 0 } {
	testFail $errMemMsg(18)
	return
	}

    # clean up
    cleanUp ms $bAdrs

    wtxTestPass
}


#*****************************************************************************
#
# wtxMemAlignTest2 - verify wtxMemAlign with a invalid alignment
#
# Verify wtxMemAlign return the right error code for wrong alignment 
# and too big size
#

proc wtxMemAlignTest2 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline

    # SUBTEST 1 :
    # try to align a block of memory with invalid alignment

    # if no exception occurred
    if {![catch {wtxMemAlign -1 1000} bAdrs]} {
	testFail $errMemMsg(19)
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $bAdrs TGTMEM_NOT_ENOUGH_MEMORY]} {
	testFail $bAdrs
	return
	}


    # SUBTEST 2 :
    # try align a block of memory with invalid size

    # if no exception occurred
    if {![catch {wtxMemAlign 16 100000000} bAdrs]} {
	testFail $errMemMsg(3)
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $bAdrs TGTMEM_NOT_ENOUGH_MEMORY]} {
        testFail $bAdrs
        return
    }

    wtxTestPass
}

#*****************************************************************************
#
# wtxMemAlignTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
# Furthermore it attaches itself to the target server (thru a wtxToolAttachCall
# command) and, when testcase units have been run, detaches itself from the
# target server.
#

proc wtxMemAlignTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxMemAlignTest targetServerName\
				timeout invalidAddr"
        return
    }

    # tests description
    global memAlignMsg

    # connection to the target server
    wtxToolAttachCall $tgtsvrName 2 $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # call the valids tests 
    wtxMemAlignTest1 $memAlignMsg(1)

    # call the invalids tests
    wtxErrorHandler [lindex [wtxHandle] 0] ""
    wtxMemAlignTest2 $memAlignMsg(2)

    # detach from the target server
    wtxToolDetachCall "wtxMemAlignTest"
}
