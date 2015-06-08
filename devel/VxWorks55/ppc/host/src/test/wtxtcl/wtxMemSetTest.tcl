# wtxMemSetTest.tcl - Tcl script, test WTX_MEM_SET
#
# Copyright 1994-1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01l,28jan98,p_b  Adapted for WTX 2.0
# 01k,01oct96,jmp  removed -timeout option of wtxMemWrite.
# 01j,26feb96,l_p  corrected English mistakes in a few messages.
# 01i,20oct95,jmp  removed wtxMemSetTest4.
#		   added invalidAddr argument to wtxMemSetTest.
# 01h.12sep95,jmp  added cleanUp call.
# 01g,16aug95,jmp  added wtxMemSetTest4
#		   added timeout to wtxToolAttachCall in wtxMemSetTest.
#                  changed tests control method, now uses wtxErrorHandler.
#                  now uses invalidAddrGet procedure.
# 01f,23jun95,f_v  changed error message in wtxMemSetTest3,changed invalid addr
# 		   added wtxTestRun call
# 01p,21jun95,f_v  clean up main procedure
# 01o,12jun95,pad  added some timeouts.
# 01n,27apr95,c_s  now uses wtxErrorMatch.
# 01m,07apr95,f_v  added vxWorksErr(EACCES) in wtxMemSetTest3 test
# 01l,30mar95,f_v  replace S_wdb_xx by agentErr
# 01k,17mar95,f_v  change index in wtxTsInfoGet
# 01j,03mar95,f_v  change S_tgtMemLib_XXX by tgtMemErr(XXX)
# 01i,20feb95,kpn  changed wtxMemZero -> wtxMemSet
# 01h,17feb95,kpn  added wtxMemZeroTest3{}, added memBlockDelete call
#		   Time the execution of  wtxMemZero().
# 01g,01feb95,kpn  corrected a test error in wtxMemZeroTest2.
# 01f,27jan95,kpn  deplaced testcase units description in wtxTestMsg and
#                  testcase units errors in wtxTestErr.tcl,
#                  added global memZeroMsg in the main.
#                  added global errMemMsg in each testcase units,
#                  used memZeroMsg & errMemMsg to manage messages.
# 01e,26jan95,kpn  removed the calls to wtxToolAttachCall and wtxToolDetach
#                  made in each testcase.
#                  Added wtxToolAttachCall and wtxToolDetachCall in the main
#                  procedure.
#		   removed error codes initialization, added global variables.
# 01d,24jan95,kpn  changed S_wdb_MEM_ACCESS_ERROR value.
# 01c,20jan95,kpn  replaced wtxToolAttach by wtxToolAttachCall.
#		   removed msge(4) and msge(5).
# 01b,04jan95,kpn  removed parameter to wtxToolDetach (automatic now).
# 01a,21nov94,kpn  written.
#


#
# DESCRIPTION
#
# WTX_MEM_SET - Clear a Block of Memory
#
#
# TESTS :
#
#
# (1) wtxMemSetTest1 - verify that wtxMemSet() initializes a memory block 
#	of 10000 bytes on the target to 0x00 in each byte. 
#
# (2) wtxMemSetTest2 - verify that wtxMemSet() clears only the required
#	block of memory. So verify that it doesn't clear the bytes before 
#	the start address and after the end address.
#
# (3) wtxMemSetTest3 - verify wtxMemSet() with invalid <startAddr> argument.
#     Verify that the result of wtxMemSet() is WTX_ERR_TGTSVR_EINVAL
#     or EACCES 
#


#*****************************************************************************
#
# wtxMemSetTest1 - Verify that wtxMemSet() initializes to 0x00 a memory block
#		   of 10000 bytes on the target.
#
# Verify that the wtxMemSet() routine can clear a block of memory.  Each 
# byte of this block of memory is initialised to 0x45 with wtxMemWrite().
# This testcase verifies that wtxMemSet() initializes to 0x00 each byte of 
# the block.
#  

proc wtxMemSetTest1 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline


    # create a big-endian one 10000 bytes long, initialized to 0x45 in
    # each byte
    set blockId1 [memBlockCreate -B 10000 0x45]

    # create a big-endian one 10000 bytes long, initialized to 0x00 in 
    # each byte
    set blockId2 [memBlockCreate -B 10000 0x00]

    # get the contents of the new memory block in a buffer, to can verify 
    # the allocated memory on the target contains only 0x00 in each byte 
    # after a wtxMemSet on it 
    set buffer [memBlockGet $blockId2]

    # Allocate memory on target (10000 bytes), return the address of the
    # allocated memory block
    set resMemAlloc [wtxMemAlloc 10000]
    if {$resMemAlloc == ""} {
	cleanUp mb $blockId1 $blockId2
        return 0
	}

    # store the 10000 bytes initialized to 0x45 to the target starting at
    # $resMemAlloc
    set resMemWrite [wtxMemWrite $blockId1 $resMemAlloc]
    if {$resMemWrite == ""} {
	cleanUp mb $blockId1 $blockId2 ms $resMemAlloc
        return 0
    }

    # clear the block of memory
    # time the execution of wtxMemSet
    set timeMsr [time {set resMemSet [wtxMemSet $resMemAlloc 10000 0x00]} 1]
    if {$resMemSet == ""} {
	cleanUp mb $blockId1 $blockId2 ms $resMemAlloc
        return 0
    }

    # get the average  amount  of  time  required  per iteration for\
    # the execution of wtxMemSet
    set timeMsr [lindex $timeMsr 0]
    # make a conversion between micro seconds and milli seconds
    set timeMsrSec [ expr $timeMsr/1000]


    # read the 10000 bytes written at the address $resMemAlloc
    # normally each byte of this block must contains now 0x00
    set mblock [wtxMemRead $resMemAlloc 10000]
    if {$mblock == ""} {
	cleanUp mb $blockId1 $blockId2 ms $resMemAlloc
        return 0
    }

    # verify that the block contains now 0x00 in each byte 
    if {[memBlockGet $mblock] != [memBlockGet $blockId2]} {
        cleanUp mb $blockId1 $blockId2 ms $resMemAlloc
        testFail $errMemMsg(12)
        return 0
    }
 
    # clean up
    cleanUp mb $blockId1 $blockId2 ms $resMemAlloc

    wtxTestPass

    return $timeMsrSec 
}

#*****************************************************************************
#
# wtxMemSetTest2 - Verify that wtxMemSet clears only the required bytes 
#		    of a block of memory.
#
# Verify wtxMemSet clears only the required block of memory. So that it
# doesn't clear the bytes before the start address and after the end address.
#

proc wtxMemSetTest2 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline


    # create a big-endian one 150 bytes long, initialized to 0x00 in
    # each byte
    set blockId [memBlockCreate -B 150 0x00]

    # give a different value for each byte of the block (0x10 -> 0xa5)
    for {set i 0} {$i<150} {incr i 1} {
	memBlockSet $blockId $i [expr 0x10 + $i]
    }

    # Allocate memory on target, return the address of the allocated
    # memory block
    set resMemAlloc [wtxMemAlloc 150]
    if {$resMemAlloc == ""} {
        cleanUp ml
	return
	}

    # store those 150 bytes to the target starting at $resMemAlloc
    set resMemWrite [wtxMemWrite $blockId $resMemAlloc]
    if {$resMemWrite == ""} {
        cleanUp ml ms $resMemAlloc
        return
	}

    #
    # verify if what we have written at $resMemAlloc in the target is right

    # read the 150 bytes written at the address $resMemAlloc
    set mblock [wtxMemRead $resMemAlloc 150]
    if {[wtxErrorMatch $mblock AGENT_MEM_ACCESS_ERROR]} {
        cleanUp ml ms $resMemAlloc
        return
    }

    # get the contents of this block
    set blkCont [memBlockGet $mblock]

    # compare the block contents and the buffer that contains the initial
    # bytes

    if {[memBlockGet $mblock] != [memBlockGet $blockId]} {
        testFail $errMemMsg(12)
        cleanUp ml ms $resMemAlloc
        return
    }

    # clear a part of the block of memory
    set resMemSet [wtxMemSet [expr $resMemAlloc + 49] 50 0x00]
    if {$resMemSet == ""} {
        cleanUp ml ms $resMemAlloc
        return
    }

    # read the 150 bytes written at the address $resMemAlloc
    set mblock [wtxMemRead $resMemAlloc 150]
    if {$mblock == ""} {
        cleanUp ml ms $resMemAlloc
        return
    }

    # get the contents of this block
    set freeBlk [memBlockGet $mblock]

    # verify that wtxMemSet has cleared the right bytes of the block of 
    # memory and only these. 
    
    for {set i 49} {$i<99} {incr i 1} {
        memBlockSet $blockId $i 0x00 
    }

    if {[memBlockGet $blockId] != [memBlockGet $mblock]} {
        testFail $errMemMsg(13)
        cleanUp ml ms $resMemAlloc
        return
    }

    # clean up
    cleanUp ml ms $resMemAlloc

    wtxTestPass
}


#*****************************************************************************
#
# wtxMemSetTest3 - verify wtxMemSet() with invalid <startAddr> argument
#
# This test calls wtxMemSet() with the <startAddr> argument equals to an
# invalid address provided by invalidAddrGet.
# The test verifies that wtxMemSet has returned an error code, and if
# the returned error code is AGENT_MEM_ACCESS_ERROR (To prevent from
# errors like TIMEOUT.
#

proc wtxMemSetTest3 {Msg invalidAddr} {


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

    # initialize 100 bytes to the target starting at invalidAdd

    # if no exception occurred
    if {![catch "wtxMemSet $invalidAdd 100 0x55" errCode]} {
	testFail $errMemMsg(6)
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $errCode AGENT_MEM_ACCESS_ERROR]} {
        testFail $errCode
        return
    }

    # if the caught exception is the expected error code
    wtxTestPass
} 


#*****************************************************************************
#
# wtxMemSetTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
# Furthermore it attaches itself to the target server (thru a wtxToolAttachCall
# command) and, when testcase units have been run, detaches itself from the
# target server.
#

proc wtxMemSetTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxMemSetTest targetServerName\
				timeout invalidAddr"
        return
    }

    global memZeroMsg

    # connection to the target server
    wtxToolAttachCall $tgtsvrName 2 $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # valids tests
    if {[set essai [wtxMemSetTest1 $memZeroMsg(1)]]} {
        puts stdout "\t(Clear a 10000 bytes memory block in\
	 $essai msec)"
    }
    wtxMemSetTest2 $memZeroMsg(2)

    wtxErrorHandler [lindex [wtxHandle] 0] ""


    # invalids tests
    wtxMemSetTest3 $memZeroMsg(3) $invalidAddr

    # detach from the target server
    wtxToolDetachCall "\nwtxMemSetTest : "
}
