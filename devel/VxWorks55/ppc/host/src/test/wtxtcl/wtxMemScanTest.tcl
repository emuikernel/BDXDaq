# wtxMemScanTest.tcl - Tcl script, test WTX_MEM_SCAN
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01e,28jan98,p_b  Adapted for WTX 2.0
# 01d,20oct95,jmp  added wtxMemScanTest4 et wtxMemScanTest5.
#		   added invalidAddr argument to wtxMemScanTest.
# 01c,16aug95,jmp  added wtxMemScanTest3, corrected wtxMemScanTest2.
#		   added timeout to wtxToolAttachCall in wtxMemScanTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01b,23jun95,f_v  added wtxTestRun call
# 01a,31may95,f_v  written.
#
#

#
# DESCRIPTION
#
# WTX_MEM_SCAN - Scan a part of memory between start and end address
#
# TESTS :
#
# (1) wtxMemScanTest1 - Scan a part of memory with -memblk option
#
# (2) wtxMemScanTest2 - Scan a part of memory with -string option
#
# (3) wtxMemScanTest3 - Verify wtxMemScan() with invalid parameters
#
# (4) wtxMemScanTest4 - Scan a part of memory with -memblk and
#			-nomatch options
#
# (5) wtxMemScanTest5 - Scan a part of memory with -string and
#			-nomatch options
#



#*****************************************************************************
#
# wtxMemScanTest1 - verify wtxMemScan() with -memblk option and a valid memory
# block address
#
# Create 2 blocks of memory which include a typical value then scan memory
# to find one of these blocks
#

proc wtxMemScanTest1 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline

    set blockSize   1000
    set lblockSize  10

    # Allocate memory on target (1000 bytes), return the address of the
    # allocated memory block
    set bAddrs1 [wtxMemAlloc $blockSize]
    if {$bAddrs1 == ""} {
        return
    }

    set bId1 [memBlockCreate -B $lblockSize 0x1]
    set bId2 [memBlockCreate -B $lblockSize 0x2]

    # store the 10 bytes initialized to 0x1 to the target starting at
    # $bAddrs1+10
    set status [wtxMemWrite $bId1 [expr $bAddrs1+10]]
    if {$status == ""} {
	cleanUp mb $bId1 $bId2 ms $bAddrs1
        return
    }

    # store the 10 bytes initialized to 0x2 to the target starting at
    # $bAddrs1+30
    set status [wtxMemWrite $bId2 [expr $bAddrs1+30]]
    if {$status == ""} {
	cleanUp mb $bId1 $bId2 ms $bAddrs1
        return
    }

    # scan memory to get the address of second memory block
    set scanSt [wtxMemScan $bAddrs1 [expr $bAddrs1+50] -memblk $bId2]
    if {$scanSt == ""} {
	cleanUp mb $bId1 $bId2 ms $bAddrs1
	return
	}

    if { $scanSt != [expr $bAddrs1+30]} { 
        testFail $errMemMsg(9)
	cleanUp mb $bId1 $bId2 ms $bAddrs1
        return
    }

    # clean up
    cleanUp mb $bId1 $bId2 ms $bAddrs1

    wtxTestPass
}



#*****************************************************************************
#
# wtxMemScanTest2 - verify wtxMemScan() with -string option and valid parameters
#
# Create a blocks of memory which include a typical string then scan memory
# to find this string
#


proc wtxMemScanTest2 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline

    set blockSize  1000

    # Allocate memory on target (1000 bytes), return the address of the
    # allocated memory block
    set bAddrs1 [wtxMemAlloc $blockSize]
    if {$bAddrs1 == ""} {
        return
    }

    set bId1 [memBlockCreate -B -string "test1"]

    # store the memory block initialized to "test1" (0x74,65,73,74,31 in hexa)
    # to the target starting at $bAddrs1+10
    set status [wtxMemWrite $bId1 [expr $bAddrs1+10]]
    if {$status == ""} {
	cleanUp mb $bId1 ms $bAddrs1
        return
    }

    set scanSt [wtxMemScan $bAddrs1 [expr $bAddrs1+50] -string "test1"]
    if {$scanSt == ""} {
	cleanUp mb $bId1 ms $bAddrs1
        return
    }

    # clean up
    cleanUp mb $bId1 ms $bAddrs1

    wtxTestPass
}


#*****************************************************************************
#
# wtxMemScanTest3 - Verify wtxMemScan() with invalid parameters 
#
# Create 1 block of memory, verify if wtxMemScan with -memblk option
# and with a nonexistant block return "block not found", verify if
# wtxMemScan with -string option and with a nonexistant string return 
# AGENT_PATTERN_NOT_FOUND.
#
#


proc wtxMemScanTest3 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline

    set blockSize  1000

    # Allocate memory on target (1000 bytes), return the address of the
    # allocated memory block
    if {[catch "wtxMemAlloc $blockSize" addr]} {
	testFail $addr
        return
    }

    # verify if wtxMemScan with -memblk option and with a nonexistant block 
    # return "block not found"

    # if no exception occurred
    if {![catch "wtxMemScan $addr [expr $addr+50]\
	-memblk invalidBlock" scanSt]} {
	testFail $errMemMsg(10)
	return
	}

    # if the caught exception is different than the expected error code
    if {$scanSt != "block not found"} {
        cleanUp ms $addr
        testFail "$scanSt"
        return
    } 



    # verify if wtxMemScan with -string option and with a nonexistant string
    # return AGENT_PATTERN_NOT_FOUND.

    # if no exception occurred
    if {![catch {wtxMemScan $addr [expr $addr+50]\
	-string "invalidString"} scanSt]} {
	testFail $errMemMsg(11)
	retur
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $scanSt AGENT_PATTERN_NOT_FOUND]} {
        cleanUp ms $addr
	testFail $scanSt
        return
    } 

    # clean up 
    cleanUp ms $addr

    wtxTestPass
}



#*****************************************************************************
#
# wtxMemScanTest4 - Scan memory with -memblk and -nomatch options
#
#

proc wtxMemScanTest4 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline

    set blockSize   1000
    set lblockSize  10

    # Allocate memory on target (1000 bytes), return the address of the
    # allocated memory block
    set bAddrs1 [wtxMemAlloc $blockSize]
    if {$bAddrs1 == ""} {
        return
    }

    set bId1 [memBlockCreate -B $lblockSize 0x1]
    set bId2 [memBlockCreate -B $lblockSize 0x2]
    set bId3 [memBlockCreate -B 1 0x1]

    # store the 10 bytes initialized to 0x1 to the target starting at
    # $bAddrs1
    set status [wtxMemWrite $bId1 $bAddrs1]
    if {$status == ""} {
	cleanUp mb $bId1 $bId2 $bId3 ms $bAddrs1
        return
    }

    # store the 10 bytes initialized to 0x1 to the target starting at
    # $bAddrs1 + 10
    set status [wtxMemWrite $bId2 [expr $bAddrs1 + 10]]
    if {$status == ""} {
	cleanUp mb $bId1 $bId2 $bId3 ms $bAddrs1
        return
    }


    # scan memory to get the address of second memory block
    set scanSt [wtxMemScan -notmatch $bAddrs1 [expr $bAddrs1+50] -memblk $bId3]

    if {$scanSt == ""} {
	cleanUp mb $bId1 $bId2 $bId3 ms $bAddrs1
	return
    }

    if { $scanSt != [expr $bAddrs1+10]} { 
        testFail $errMemMsg(9)
	cleanUp mb $bId1 $bId2 $bId3 ms $bAddrs1
        return
    }

    # clean up
    cleanUp mb $bId1 $bId2 $bId3 ms $bAddrs1

    wtxTestPass
}


#*****************************************************************************
#
# wtxMemScanTest5 - Scan memory with -memblk and -nomatch options
#
#

proc wtxMemScanTest5 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline

    set blockSize   1000
    set lblockSize  10

    # Allocate memory on target (1000 bytes), return the address of the
    # allocated memory block
    set bAddrs1 [wtxMemAlloc $blockSize]
    if {$bAddrs1 == ""} {
        return
    }

    set bId1 [memBlockCreate -B -string "aaaaaaaaaa"]
    set bId2 [memBlockCreate -B $lblockSize 0x2]

    # store the 10 bytes initialized to 0x1 to the target starting at
    # $bAddrs1
    set status [wtxMemWrite $bId1 $bAddrs1]
    if {$status == ""} {
	cleanUp mb $bId1 $bId2 ms $bAddrs1
        return
    }

    # store the 10 bytes initialized to 0x1 to the target starting at
    # $bAddrs1 + 10
    set status [wtxMemWrite $bId2 [expr $bAddrs1 + 10]]
    if {$status == ""} {
	cleanUp mb $bId1 $bId2 ms $bAddrs1
        return
    }


    # scan memory to get the address of second memory block
    set scanSt [wtxMemScan -notmatch $bAddrs1 [expr $bAddrs1+50] -string "a"]

    if {$scanSt == ""} {
	cleanUp mb $bId1 $bId2 ms $bAddrs1
	return
	}

    if {$scanSt != [expr $bAddrs1+10]} { 
        testFail $errMemMsg(9)
	cleanUp mb $bId1 $bId2 ms $bAddrs1
        return
    }

    # clean up
    cleanUp mb $bId1 $bId2 ms $bAddrs1

    wtxTestPass
}



#*****************************************************************************
#
# wtxMemScanTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
# Furthermore it attaches itself to the target server (thru a wtxToolAttachCall
# command) and, when testcase units have been run, detaches itself from the
# target server.
#

proc wtxMemScanTest {tgtsvrName timeout invalidAddr} {

    if {[llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxMemScanTest targetServerName\
				timeout invalidAddr"
        return
    }

    # tests description
    global memScanMsg

    # valids tests
    wtxToolAttachCall $tgtsvrName 2 $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    wtxMemScanTest1 $memScanMsg(1)
    wtxMemScanTest2 $memScanMsg(2)
    wtxMemScanTest4 $memScanMsg(4)
    wtxMemScanTest5 $memScanMsg(5)

    # invalids tests
    wtxErrorHandler [lindex [wtxHandle] 0] ""
    wtxMemScanTest3 $memScanMsg(3)
    wtxToolDetachCall "\nwtxMemScanTest : "
}
