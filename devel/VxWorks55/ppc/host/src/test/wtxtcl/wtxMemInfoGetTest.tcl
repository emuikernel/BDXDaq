# wtxMemInfoGetTest.tcl - Tcl script, test WTX_MEM_INFO_GET
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01o,28jan98,p_b  Adapted for WTX 2.0
# 01n,19nov95,p_m  added test for number of blocks = 0.
# 01m,02nov95,jmp  definitively removed wtxMemInfoGetTest3 because now
#		   it's possible to allocate a block of memory which
#		   size is higher than the biggest free block size.
# 01l,20oct95,jmp  added invalidAddr argument to wtxMemInfoGetTest.
# 01k,17aug95,jmp  Fixed a bug in test of validity of the current number
#		   of free blocks in wtxMemInfoGetTest1 & wtxMemInfoGetTest2.
#		   added timeout to wtxToolAttachCall in wtxMemInfoGetTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01j,23jun95,f_v  added wtxTestRun call
# 01i,21jun95,f_v  clean up main procedure
# 01h,27apr95,c_s  now uses wtxErrorMatch.
# 01g,07apr95,f_v  changed errMemMsg, add loop to free memory, exit properly
# 01f,22mar95,jcf  changed to match overheadless memory manager.
# 01e,03mar95,f_v  change S_tgtMemLib_NOT_ENOUGH_MEMORY by
#  		   tgtMemErr(WTX_ERR_TGTMEM_NOT_ENOUGH_MEMORY)
#		   change S_tgtMemLib_BLOCK_ERROR by
#		   tgtMemErr(WTX_ERR_TGTMEM_INVALID_BLOCK)
# 01d,27jan95,kpn  deplaced testcase units description in wtxTestMsg and
#                  testcase units errors in wtxTestErr.tcl,
#                  added global memInfoMsg in the main.
#                  added global errMemMsg in each testcase units,
#                  used memInfoMsg & errMemMsg to manage messages.
# 01c,26Jan95,kpn  removed the calls to wtxToolAttachCall and wtxToolDetach
#                  made in each testcase.
#                  Added wtxToolAttachCall and wtxToolDetachCall in the main
#                  procedure.
#		   removed error codes initialization, added global variables.
# 01b,24Jan95,kpn  added allocSize and set it to 512 bytes in order to 
#                  accomodate all architectures alignement.
# 01a,12Jan95,kpn  written. 
#
#

#
# DESCRIPTION
#
# WTX_MEM_INFO_GET - get memory information
#
# This service returns information about the Target Server managed Target
# memory pool. The memory pool information is :
#
#        curBytesFree 		current number of free bytes
#        curBytesAllocated 	current number of allocated bytes
#        cumBytesAllocated 	cumulative number of allocated bytes
#        curBlocksFree		current number of free blocks
#        curBlocksAlloc		current number of allocated blocks
#        cumBlocksAlloc		cumulative number of allocated blocks
#        avgFreeBlockSize	average size of free blocks
#        avgAllocBlockSize	average size of allocated blocks
#        cumAvgAllocBlockSize	cumulative average size of allocated blocks
#        biggestBlockSize	size of the biggest free block
#        reserved1
#        reserved2
#        reserved3
#
#
# TESTS :
#
# (1) wtxMemInfoGetTest1 - verify wtxMemInfoGet result after a wtxMemAlloc
#
# (2) wtxMemInfoGetTest2 - verify wtxMemInfoGet result after a wtxMemFree
#
#


#*****************************************************************************
#
# wtxMemInfoGetTest1 - verify wtxMemInfoGet result after a wtxMemAlloc
#
# This script gets memory information before and after to have allocated
# memory on target to verify the validity of the memory information that
# wtxMemInfoGet returned.
# 

proc wtxMemInfoGetTest1 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline

 # LOCAL VARIABLES

    # allocated block size
    set allocSize	512
    # allocated block overhead
    set allocOvh	0
    set list {1 2 3}

    for {set i 1} {$i < 4} {incr i 1} {

    # adjust list 
    set listLoop  [lrange $list 0 [expr $i-1]]
    set listLoop2 [lrange $list 0 [expr $i-2]]

    # get memory information
    set resMemInfoGet [wtxMemInfoGet]

    # get each memory pool information
    set curBytesFree1		[lindex $resMemInfoGet 0]
    set curBytesAllocated1	[lindex $resMemInfoGet 1]
    set cumBytesAllocated1	[lindex $resMemInfoGet 2]
    set curBlocksFree1		[lindex $resMemInfoGet 3]
    set curBlocksAlloc1		[lindex $resMemInfoGet 4]
    set cumBlocksAlloc1		[lindex $resMemInfoGet 5]
    set avgFreeBlockSize1	[lindex $resMemInfoGet 6]
    set avgAllocBlockSize1	[lindex $resMemInfoGet 7]
    set cumAvgAllocBlockSize1	[lindex $resMemInfoGet 8]
    set biggestBlockSize1	[lindex $resMemInfoGet 9]

    # allocate memory on target ($allocSize bytes), return the address of the
    # allocated memory block
    
    set resMemAlloc($i) [wtxMemAlloc $allocSize]
    if {$resMemAlloc($i) == ""} {
	foreach i $listLoop2 {
	   cleanUp ms $resMemAlloc($i)
	   }
        return
	}

    # get memory information
    set resMemInfoGet [wtxMemInfoGet]


    # get each new memory pool information
    set curBytesFree2           [lindex $resMemInfoGet 0]
    set curBytesAllocated2      [lindex $resMemInfoGet 1]
    set cumBytesAllocated2      [lindex $resMemInfoGet 2]
    set curBlocksFree2          [lindex $resMemInfoGet 3]
    set curBlocksAlloc2         [lindex $resMemInfoGet 4]
    set cumBlocksAlloc2         [lindex $resMemInfoGet 5]
    set avgFreeBlockSize2       [lindex $resMemInfoGet 6]
    set avgAllocBlockSize2      [lindex $resMemInfoGet 7]
    set cumAvgAllocBlockSize2   [lindex $resMemInfoGet 8]
    set biggestBlockSize2       [lindex $resMemInfoGet 9]

    # verify the informations

    # Test validity of the current number of free bytes
    if {$curBytesFree2 != [expr $curBytesFree1 - $allocSize - $allocOvh]} {
        testFail $errMemMsg(23) 
	foreach i $listLoop {
	    cleanUp ms $resMemAlloc($i)
	 }
        return
    }

    # Test validity of the current number of allocated bytes
    if {$curBytesAllocated2 != [expr $curBytesAllocated1 + $allocSize\
    + $allocOvh]} {
        testFail $errMemMsg(24) 
	foreach i $listLoop {
	    cleanUp ms $resMemAlloc($i)
	 }
        return
    }

    # Test validity of the cumulative number of allocated bytes
    if {$cumBytesAllocated2 != [expr $cumBytesAllocated1 + $allocSize\
    + $allocOvh]} {
        testFail $errMemMsg(25)
	foreach i $listLoop {
	    cleanUp ms $resMemAlloc($i)
	 }
        return
    }

    # Test validity of the current number of allocated blocks
    if {$curBlocksAlloc2 != [expr $curBlocksAlloc1 + 1]} {
        testFail $errMemMsg(27)
	foreach i $listLoop {
	    cleanUp ms $resMemAlloc($i)
	 }
        return
    }

    # Test validity of the cumulative number of allocated blocks
    if {$cumBlocksAlloc2 != [expr $cumBlocksAlloc1 + 1]} {
        testFail $errMemMsg(28)
	foreach i $listLoop {
	    cleanUp ms $resMemAlloc($i)
	 }
        return
    }

    # Test validity of the average size of free blocks

    if {$curBlocksFree2 != 0} {
	if {$avgFreeBlockSize2 != [expr $curBytesFree2 / $curBlocksFree2]} {
	    testFail $errMemMsg(29)
	    foreach i $listLoop {
		cleanUp ms $resMemAlloc($i)
	    }
	    return
	}
    }

    # Test validity of the average size of allocated blocks

    if {$curBlocksAlloc2 != 0} {
	if {$avgAllocBlockSize2 != [expr $curBytesAllocated2 /\
				   	 $curBlocksAlloc2]} {
	    testFail $errMemMsg(30)
	    foreach i $listLoop {
		cleanUp ms $resMemAlloc($i)
	     }
	    return
	}
    }

    # Test validity of the cumulative average size of allocated blocks

    if {$cumBlocksAlloc2 != 0} {
	if {$cumAvgAllocBlockSize2 != [expr $cumBytesAllocated2 / \
					    $cumBlocksAlloc2]} {
	    testFail $errMemMsg(31)
	    foreach i $listLoop {
		cleanUp ms $resMemAlloc($i)
	     }
	    return
	}
    }

    # end for 
    }

    # get memory information
    set resMemInfoGet [wtxMemInfoGet]

    # get current number of free blocks and size of the biggest free block
    set curBlocksFreeBef	[lindex $resMemInfoGet 3]
    set biggestBlockSize	[lindex $resMemInfoGet 9]

    # allocate memory on target with size equal to the size of the biggest free
    # block ,return the address of the allocated memory block

    set resBigMemAlloc [wtxMemAlloc $biggestBlockSize]
    if {$resBigMemAlloc == ""} {
        foreach i $listLoop {
           cleanUp ms $resMemAlloc($i)
           }
        return
    }

    # get memory information
    set resMemInfoGet [wtxMemInfoGet]

    # get current number of free blocks 
    set curBlocksFreeAft [lindex $resMemInfoGet 3]

    # verify if the number of free blocks is the same than before
    # the wtxMemAlloc

    if {$curBlocksFreeAft != [expr $curBlocksFreeBef - 1]} {
        testFail $errMemMsg(26)
	cleanUp ms $resBigMemAlloc
        foreach i $listLoop {
            cleanUp ms $resMemAlloc($i)
         }
        return
    }


    # clean up
    cleanUp ms $resBigMemAlloc
    foreach i $listLoop {
	    cleanUp ms $resMemAlloc($i)
     }

    wtxTestPass
}

#*****************************************************************************
#
# wtxMemInfoGetTest2 - verify wtxMemInfoGet result after a wtxMemFree
#
# This script gets memory information before and after to have de-allocated 
# a block of memory on target to verify the validity of the memory information
# that wtxMemInfoGet returned.
#

proc wtxMemInfoGetTest2 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline

 # LOCAL VARIABLES

    # allocated block size
    set allocSize               512
    # allocated block overhead
    set allocOvh	0

    set list {1 2}

    # allocate memory to obtain the address of an allocated memory block that
    # will be used as parameter for the wtxMemFree routine.
    # Remark : allocate two blocks of memory to avoid a division by zero. 
 
    for {set i 1} {$i < 3} {incr i 1} {

    # adjust list 
    set listLoop  [lrange $list 0 [expr $i-1]]
    set listLoop2 [lrange $list 0 [expr $i-2]]

    # allocate memory on target ($allocSize bytes), return the address of the
    # allocated memory block

    set resMemAlloc($i) [wtxMemAlloc $allocSize]
    if {$resMemAlloc($i) == ""} {
	foreach i $listLoop2 {
		cleanUp ms $resMemAlloc($i)
        }
        return
    }

    # end for
    }


    # get memory information before wtxMemFree call
    set resMemInfoGet [wtxMemInfoGet]

    # get each memory pool information
    set curBytesFree1           [lindex $resMemInfoGet 0]
    set curBytesAllocated1      [lindex $resMemInfoGet 1]
    set cumBytesAllocated1      [lindex $resMemInfoGet 2]
    set curBlocksFree1          [lindex $resMemInfoGet 3]
    set curBlocksAlloc1         [lindex $resMemInfoGet 4]
    set cumBlocksAlloc1         [lindex $resMemInfoGet 5]
    set avgFreeBlockSize1       [lindex $resMemInfoGet 6]
    set avgAllocBlockSize1      [lindex $resMemInfoGet 7]
    set cumAvgAllocBlockSize1   [lindex $resMemInfoGet 8]
    set biggestBlockSize1       [lindex $resMemInfoGet 9]

    # de-allocates a block of memory ($allocSize bytes) previously allocated 
    # with wtxMemAlloc
    set resMemFree [wtxMemFree $resMemAlloc(1)]
    if {$resMemFree == ""} {
        testFail $errMemMsg(22)
	cleanUp ms $resMemAlloc(2)
	return
    }

    # get memory information after wtxMemFree call
    set resMemInfoGet [wtxMemInfoGet]

    # get each new memory pool information
    set curBytesFree2           [lindex $resMemInfoGet 0]
    set curBytesAllocated2      [lindex $resMemInfoGet 1]
    set cumBytesAllocated2      [lindex $resMemInfoGet 2]
    set curBlocksFree2          [lindex $resMemInfoGet 3]
    set curBlocksAlloc2         [lindex $resMemInfoGet 4]
    set cumBlocksAlloc2         [lindex $resMemInfoGet 5]
    set avgFreeBlockSize2       [lindex $resMemInfoGet 6]
    set avgAllocBlockSize2      [lindex $resMemInfoGet 7]
    set cumAvgAllocBlockSize2   [lindex $resMemInfoGet 8]
    set biggestBlockSize2       [lindex $resMemInfoGet 9]

    # verify information returned by wtxMemInfoGet routine 

    # Test validity of the current number of free bytes
    if {$curBytesFree2 != [expr $curBytesFree1 + $allocSize + $allocOvh]} {
        testFail $errMemMsg(23)
        cleanUp ms $resMemAlloc(2)
        return
    }

    # Test validity of the current number of allocated bytes
    if {$curBytesAllocated2 != [expr $curBytesAllocated1 - $allocSize\
    - $allocOvh]} {
        testFail $errMemMsg(24)
        cleanUp ms $resMemAlloc(2)
        return
    }

    # Test validity of the cumulative number of allocated bytes
    if {$cumBytesAllocated2 != $cumBytesAllocated1} {
        testFail $errMemMsg(25)
        cleanUp ms $resMemAlloc(2)
        return
    }

    # Test validity of the current number of allocated blocks
    if {$curBlocksAlloc2 != [expr $curBlocksAlloc1 - 1]} {
        testFail $errMemMsg(27)
        cleanUp ms $resMemAlloc(2)
        return
    }

    # Test validity of the cumulative number of allocated blocks
    if {$cumBlocksAlloc2 != $cumBlocksAlloc1} {
        testFail $errMemMsg(28)
        cleanUp ms $resMemAlloc(2)
        return
    }

    # Test validity of the average size of free blocks
    if {$curBlocksFree2 != 0} {
	if {$avgFreeBlockSize2 != [expr $curBytesFree2 / $curBlocksFree2]} {
	    testFail $errMemMsg(29)
	    cleanUp ms $resMemAlloc(2)
	    return
	}
    }

    # Test validity of the average size of allocated blocks
    if {$curBlocksAlloc2 != 0} {
	if {$avgAllocBlockSize2 != [expr $curBytesAllocated2 /\
					$curBlocksAlloc2]} {
	    testFail $errMemMsg(30)
	    cleanUp ms $resMemAlloc(2)
	    return
	}
    }

    # Test validity of the cumulative average size of allocated blocks
    if {$cumBlocksAlloc2 != 0} {
	if {$cumAvgAllocBlockSize2 != [expr $cumBytesAllocated2 / \
					    $cumBlocksAlloc2]} {
	    testFail $errMemMsg(31)
	    cleanUp ms $resMemAlloc(2)
	    return
	}
    }

    # get memory information
    set resMemInfoGet [wtxMemInfoGet]

    # get size of the biggest free block
    set biggestBlockSize [lindex $resMemInfoGet 9]
    
    # allocate memory on target with size equal to the size of the biggest free
    # block ,return the address of the allocated memory block
    set resBigMemAlloc [wtxMemAlloc $biggestBlockSize]
    if {$resBigMemAlloc == ""} {
        cleanUp ms $resMemAlloc(2)
        return
    }

    # get memory information
    set resMemInfoGet [wtxMemInfoGet]

    # get current number of free blocks
    set curBlocksFreeBef        [lindex $resMemInfoGet 3]

    # de-allocates block of memory ($biggestBlockSize bytes) previously
    # allocated with wtxMemAlloc
    set resMemFree [wtxMemFree $resBigMemAlloc]
    if {$resMemFree == ""} {
        cleanUp ms $resMemAlloc(2)
        return
    }

    # get memory information after wtxMemFree call
    set resMemInfoGet [wtxMemInfoGet]

    # get current number of free blocks
    set curBlocksFreeAft [lindex $resMemInfoGet 3]

    # verify if the number of free blocks is correct
    if {$curBlocksFreeAft != [expr $curBlocksFreeBef + 1]} {
        testFail $errMemMsg(26)
        cleanUp ms $resMemAlloc(2)
        return
    }


   # clean up
    cleanUp ms $resMemAlloc(2)

    wtxTestPass
}

#*****************************************************************************
#
# wtxMemInfoGetTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
# Furthermore it attaches itself to the target server (thru a wtxToolAttachCall
# command) and, when testcase units have been run, detaches itself from the
# target server.
#


proc wtxMemInfoGetTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxMemInfoGetTest targetServerName\
				timeout invalidAddr"
        return
    }

    global memInfoMsg

    # connection to the target server
    wtxToolAttachCall $tgtsvrName 2 $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # tests with valids parameters 
    wtxMemInfoGetTest1 $memInfoMsg(1)
    wtxMemInfoGetTest2 $memInfoMsg(2)
    
    # detach from the target server
    wtxToolDetachCall "\nwtxMemInfoGetTest : "
}
