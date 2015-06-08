# wtxMemChecksumTest.tcl  - Tcl script, test WTX_MEM_CHECKSUM
#
# Copyright 1994 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01r,28jan98,p_b  Adapted for WTX 2.0
# 01q,07nov95,jmp  removed wtxMemChecksumTest3.
# 01p,20oct95,jmp  added invalidAddr argument to wtxMemChecksumTest.
# 01o,17aug95,jmp  added wtxMemChecksumTest3
#		   added timeout to wtxToolAttachCall in wtxMemChecksumTest.
#                  changed tests control method, now uses wtxErrorHandler.
#                  now uses invalidAddrGet procedure.
# 01n,23jun95,f_v  added wtxTestRun call
# 01m,21jun95,f_v  clean up main procedure
# 01l,10may95,f_v  now uses 0xeeeeeeee like invalid address.
# 01k,27apr95,c_s  now uses wtxErrorMatch.
# 01j,07apr95,f_v  changed few error code
# 01i,30mar95,f_v  replace S_wdb_xx by agentErr
# 01h,17mar95,f_v  replace index in wtxTsInfoGet 
# 01g,03mar95,f_v  replace  S_tgtMemLib_XXX by tgtMemErr(XXX) 
# 01f,17feb95,kpn  rewritten wtxMemChecksumTest2{}, added memBlockDelete call.
# 01e,27jan95,kpn  deplaced testcase units description in wtxTestMsg and
#                  testcase units errors in wtxTestErr.tcl,
#                  added global memChecksumMsg in the main,
#                  added global errMemMsg in each testcase units,
#                  used memChecksumMsg & errMemMsg to manage messages.
# 01d,26jan95,kpn  removed the calls to wtxToolAttachCall and wtxToolDetach
#                  made in each testcase.
#                  Added wtxToolAttachCall and wtxToolDetachCall in the main
#                  procedure.
#		   removed error codes initialization, added global variables.
# 01c,24jan95,kpn  changed S_wdb_MEM_ACCESS_ERROR value.
# 01b,15jan95,kpn  written wtxMemChecksumTest1 and wtxMemChecksumTest2.
# 01a,28nov94,kpn  written.
#
#


#
# DESCRIPTION
#
# WTX_MEM_CHECKSUM - Perform a Checksum on a Block of Memory
#
# TESTS :
#
# (1) wtxMemChecksumTest1 - verify wtxMemChecksum() on a Block of Memory
#
# (2) wtxMemChecksumTest2 - verify wtxMemChecksum() with invalid <startAddr> 
#	argument. Verify that the result of wtxMemChecksum() is
#	S_wdb_MEM_ACCES_ERROR.
#


#*****************************************************************************
#
# wtxMemChecksumTest1 - verify wtxMemChecksum on a Block of Memory
#
# This script creates a block of memory 100 bytes long and initialized
# to 0x20 in each byte. Then it additions the bytes contains on this block 
# of memory and makes a calcul to obtain the equivalence of a checksum
# (The checksum is a 16 bits value).
# Then it allocates memory on target and store those 100 bytes to the target,
# make a checksum on the block of memory with wtxMemChecksum service.
# The test verifies that the two sums are equal.
#

proc wtxMemChecksumTest1 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline

    # create memory block
    set blockId [memBlockCreate -B 100 0x20]

    set tmpSum 0

    for {set i 0} {$i<50} {incr i 1} {
        set tmp [memBlockGet -w $blockId $i 1]
        set tmpSum [expr $tmpSum + $tmp]
    }

    #set chksum [expr [expr $tmpSum & 0xffff] + [expr $tmpSum >> 16]]

    # F_V
    set tmpSum [expr [expr ($tmpSum & 0xffff0000)>>16] + ($tmpSum & 0x0000ffff)]
    set chksum [expr ($tmpSum ^ 0xffff)]

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

    # make a checksum on the block of memory starting at $resMemAlloc 
    set resMemChecksum [wtxMemChecksum $resMemAlloc 100]
    if {$resMemChecksum == ""} {
	cleanUp mb $blockId ms $resMemAlloc
        return
    }

    # compare the result of the additions
    if {$resMemChecksum != $chksum} {
	cleanUp mb $blockId ms $resMemAlloc
	testFail $errMemMsg(21)
	return
    }

    # clean up
    cleanUp mb $blockId ms $resMemAlloc

    wtxTestPass
}


#*****************************************************************************
#
# wtxMemChecksumTest2 - verify wtxMemChecksum() with invalid <startAddr>
#			argument.
#
# This test calls wtxMemChecksum() with the <startAddr> argument equals to
# an invalid address provided by invalidAddrGet.
# The test verifies that wtxMemChecksum has returned an error code, and if
# the returned error code is AGENT_MEM_ACCESS_ERROR (To prevent from
# errors like TIMEOUT).
#

proc wtxMemChecksumTest2 {Msg invalidAddr} {


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


    # if no exception occurred
    if {![catch "wtxMemChecksum $invalidAdd 100" errCode]} {
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
# wtxMemChecksumTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.#

proc wtxMemChecksumTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxMemChecksumTest targetServerName\
				timeout invalidAddr"
        return
    }

    global memChecksumMsg


    # call valids tests
    wtxToolAttachCall $tgtsvrName 2 $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    wtxMemChecksumTest1 $memChecksumMsg(1)

    # call invalids tests
    wtxErrorHandler [lindex [wtxHandle] 0] ""

    wtxMemChecksumTest2 $memChecksumMsg(2) $invalidAddr

    # detach from the target server
    wtxToolDetachCall "\nwtxMemChecksumTest : "
}
