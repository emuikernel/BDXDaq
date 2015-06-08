# wtxMemReadTest.tcl - Tcl script, test WTX_MEM_READ
#
# Copyright 1994-1997 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01y,27may98,p_b  WTX 2.0 new changes
# 01x,14may98,p_b  Replace test?.o by objSampleWtxtclTest?.o
# 01w,28jan98,p_b  Adapted for WTX 2.0
# 01v,06aug97,jmp  Replaced objSampleWtxtclTest1.o with objSampleWtxtclTest6.o to avoid problems with
#                  undefined symbols.
# 01u,01oct96,jmp  removed -timeout option of wtxMemWrite.
# 01t,26feb96,l_p  corrected English mistakes in a few messages.
# 01s,08nov95,s_w  Modify for SPR 5399 fix. OBJ_LOADED event no longer received
# 01r,07nov95,jmp  fixed a printing error.
# 01q,20oct95,jmp  added invalidAddr argument to wtxMemReadTest.
# 01p,12sep95,jmp  added wtxMemReadTest1 in external mode.
# 01o,09aug95,jmp  changed objGetPath to objPathGet
# 		   added wtxMemReadTest3, wtxMemReadTest4, wtxMemReadTest5.
#		   added timeout to wtxToolAttachCall in wtxMemReadTest.
#                  changed tests control method, now uses wtxErrorHandler.
#                  now uses invalidAddrGet procedure.
# 01n,23jun95,f_v  added wtxTestRun call
# 01m,09jun95,f_v  added timeout for wtxMemWrite,clean up main procedure
# 01l,12may95,c_s  upgraded for new wtxtcl version.
# 01k,10may95,f_v  now uses 0xeeeeeeee like invalid address
# 01j,27apr95,c_s  now uses wtxErrorMatch.
# 01i,07apr95,f_v  replace errMemMsg(33) by agentErr
# 01h,30mar95,f_v  replace S_wdb_xx by agentErr
# 01g,17mar95,f_v  change index in wtxTsInfoGet
# 01f,28feb95,f_v  change test in wtxMemReadTest2 because wtxMemReadTest doesn't
#		   returns message
#		   change S_tgtMemLib_XXX by tgtMemErr(XXX)
# 01e,16feb95,kpn  added wtxMemReadTest2{}, added memBlockDelete call,
#		   Time the execution of  wtxMemRead().
# 01d,27jan95,kpn  deplaced testcase units description in wtxTestMsg and
#                  testcase units errors in wtxTestErr.tcl,
#                  added global memReadMsg in the main,
#                  added global errMemMsg in each testcase units,
#                  used memReadMsg & errMemMsg to manage messages.
# 01c,26jan95,kpn  removed the calls to wtxToolAttachCall and wtxToolDetach
#                  made in each testcase.
#                  Added wtxToolAttachCall and wtxToolDetachCall in the main
#                  procedure.
#		   removed error codes initialization, added global variables.
# 01b,04jan95,kpn  removed parameter to wtxToolDetach (automatic now).
# 01a,18nov94,kpn  written.
#

#
# DESCRIPTION
#
# WTX_MEM_READ - Read a Block of Memory
#
# (1) wtxMemReadTest1 - Verify that wtxMemRead() reads the right block of
#       memory, so that it reads the required number of bytes and that
#       it returns the right block contents.
#	Time the execution of  wtxMemRead().
#
# (2) wtxMemReadTest2 - Verify wtxMemRead() with invalid <addr> argument
#       Verify that the result of wtxMemRead() is S_wdb_MEM_ACCES_ERROR.
#
# (3) wtxMemReadTest3 - Verify wtxMemRead after wtxMemWrite on a object module.
#	Verify if wtxMemRead can read correctly modifications made by wtxMemWrite.
#
# (4) wtxMemReadTest4 - Verify wtxMemRead after wtxMemSet on a object module.
#	Verify if wtxMemRead can read correctly modifications made by wtxMemSet.
#
# (5) wtxMemReadTest5 - Verify wtxMemRead after wtxMemMove on a object module.
#	Verify if wtxMemRead can read correctly modifications made by wtxMemMove.
#
#


#*****************************************************************************
#
# wtxMemReadTest1 - Verify that wtxMemRead reads the right block of
#                   memory, so that it reads the required number of bytes
#                   and that it returns the right block contents
#
# Write 10000 bytes initialized to 0x55 to the target and read these 10000
# bytes. The test verifies that 1000 bytes have been read and that the 
# read block contains the value 0x55 in each byte.  This script times the
# execution of  wtxMemRead as well.
#


proc wtxMemReadTest1 {Msg} {

 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline

    # create a big-endian one 10000 bytes long, initialized to 0x55 in 
    # each byte 
    set blockId [memBlockCreate -B 10000 0x55]

    # get the contents of the new memory block in a buffer, to can compare
    # it with the bytes that will be read on the target  
    set buffer [memBlockGet $blockId]

    # Allocate memory on target (10500 bytes), return the address of the 
    # allocated memory block
    set resMemAlloc [wtxMemAlloc 10500]
    if {$resMemAlloc == ""} {
	cleanUp mb $blockId
        return 0
    }

    # store the 10000 bytes initialized to 0x55 to the target starting at
    # $resMemAlloc 
    set resMemWrite [wtxMemWrite $blockId $resMemAlloc]
    if {$resMemWrite == ""} {
        cleanUp mb $blockId ms $resMemAlloc
        return 0
    }

    # read the 10000 bytes written at the address $resMemAlloc
    # time the execution of wtxMemRead()
    set timeMsr [time {set mblock [wtxMemRead $resMemAlloc 10000]} 1]
    if {$mblock == "" } {
	cleanUp mb $blockId ms $resMemAlloc
	return 0
    }

    # get the average  amount  of  time  required  per iteration for\
    # the execution of wtxMemRead()
    set timeMsr [lindex $timeMsr 0]
    # make a conversion between micro seconds and milli seconds
    set timeMsrSec [ expr $timeMsr/1000]

    # get the contents of the read block
    set blkCont [memBlockGet $mblock]

    # Verify if  wtxMemRead reads the right block of memory, if it reads 
    # the required number of bytes and if it returns the right contents 
    # of the block .

    # compare the read block contents and the buffer that contains the initial
    # bytes

    if {$blkCont != [memBlockGet $blockId]} {
        cleanUp mb $blockId ms $resMemAlloc
        testFail $errMemMsg(5)
        return 0
    }

    # clean up

    cleanUp mb $blockId ms $resMemAlloc

    wtxTestPass

    return $timeMsrSec
}

#*****************************************************************************
#
# wtxMemReadTest2 - Verify wtxMemRead() with invalid <addr> argument
#
# This test calls wtxMemRead() with the <addr> argument equals to an
# invalid address provided by invalidAddrGet.
# The test verifies that wtxMemRead has returned an error code, and if
# the returned error code is AGENT_MEM_ACCESS_ERROR (To prevent from
# errors like TIMEOUT.
#

proc wtxMemReadTest2 {Msg invalidAddr} {


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

    # read 150 bytes at the address invalidAdd 
    # if no exception occurred
    if {![catch "wtxMemRead $invalidAdd 150" errCode]} {
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
# wtxMemReadTest3 - Verify wtxMemRead after wtxMemWrite on a object module.
#
# This test loads an object module, and :
#
# - 1 -  it writes bytes on the end of the text segment of the object module
#	 and tries to read the written block,
# - 2 -  it writes bytes on the end of the text segment and the start of the
#	 data segment and tries to read the written block,
# - 3 -  it writes bytes on the start of the data segment and tries to read
#	 the written block. 
#
#
proc wtxMemReadTest3 {Msg} {

 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # enable all events
    set st [wtxRegisterForEvent .*]
    if {$st == ""} {    
        return
        }


    set stringToWrite "Hello world"
 
    # verify if objSampleWtxtclTest6.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest6.o
    if {[file exists $name] == 0 } {
          testFail "File $name is missing for wtxObjModuleLoadTest1 test"
          return
       }

    set tModuleId [wtxObjModuleLoad $USE_TSFORLOAD LOAD_GLOBAL_SYMBOLS $name]

    # get object module Id
    set tModuleId [lindex $tModuleId 0]

    set textInfo [lindex [wtxObjModuleInfoGet $name] 5]
    set dataInfo [lindex [wtxObjModuleInfoGet $name] 6]

    set textAddr [lindex $textInfo 1]
    set dataAddr [lindex $dataInfo 1]
   
    set stringBlock [memBlockCreate -string $stringToWrite]
    set stringSize [format 0x%x [lindex [memBlockInfo $stringBlock] 0]]

    # Write a string on the end of the text segment and compare the result
    # of wtxMemRead with the block string.
 
    set adToWrite1 [format 0x%x [expr $dataAddr - $stringSize]]
    set resWrite [wtxMemWrite $stringBlock $adToWrite1]
    if {$resWrite == ""} {
        cleanUp o $tModuleId
        return
        }

    set resRead [wtxMemRead $adToWrite1 $stringSize]
    if {$resRead == ""} {
	cleanUp o $tModuleId
	return
	}

    if {[memBlockGet $resRead] != [memBlockGet $stringBlock]} {
        testFail $errMemMsg(5)
        cleanUp o $tModuleId
        return
        }

    # Write a string on the end of the text segment and the start of data
    # segment and compare the result of wtxMemRead with the block string.

    set adToWrite2 [format 0x%x [expr $dataAddr - [expr $stringSize / 2]]]
    set resWrite [wtxMemWrite $stringBlock $adToWrite2]
    if {$resWrite == ""} {
        cleanUp o $tModuleId
        return
        }

    set resRead [wtxMemRead $adToWrite2 $stringSize]
    if {$resRead == ""} {
        cleanUp o $tModuleId
        return
        }

    if {[memBlockGet $resRead] != [memBlockGet $stringBlock]} {
        testFail $errMemMsg(5)
        cleanUp o $tModuleId
        return
        }


    # Write a string on the start of the data segment and compare the result
    # of wtxMemRead with the block string.
    
    set adToWrite3 $dataAddr

    set resWrite [wtxMemWrite $stringBlock $adToWrite3]
    if {$resWrite == ""} {
        cleanUp o $tModuleId
        return
        }

    set resRead [wtxMemRead $adToWrite3 $stringSize]
    if {$resRead == ""} {
        cleanUp o $tModuleId
        return
        }

    if {[memBlockGet $resRead] != [memBlockGet $stringBlock]} {
        testFail $errMemMsg(5)
        cleanUp o $tModuleId
        return
        }

    # clean up
    cleanUp o $tModuleId

    wtxTestPass
}



#*****************************************************************************
#
# wtxMemReadTest4 - Verify wtxMemRead after wtxMemSet on a object module.
#
# This test loads an object module, and :
#
# - 1 -  it sets bytes on the end of the text segment of the object module
#	 and tries to read the set block,
# - 2 -  it sets bytes on the end of the text segment and the start of the
#	 data segment and tries to read the set block,
# - 3 -  it sets bytes on the start of the data segment and tries to read
#	 the set block. 
#
#

proc wtxMemReadTest4 {Msg} {

 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # enable all events
    set st [wtxRegisterForEvent .*]
    if {$st == ""} {
	return
	}

    set sizeToSet 10

    # verify if objSampleWtxtclTest6.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest6.o
    if { [file exists $name] == 0 } {
          testFail "File $name is missing for wtxObjModuleLoadTest1 test"
          return
       }

    set tModuleId [wtxObjModuleLoad $USE_TSFORLOAD LOAD_GLOBAL_SYMBOLS $name]

    # get object module Id
    set tModuleId [lindex $tModuleId 0]

    set textInfo [lindex [wtxObjModuleInfoGet $name] 5]
    set dataInfo [lindex [wtxObjModuleInfoGet $name] 6]
    set textAddr [lindex $textInfo 1]
    set dataAddr [lindex $dataInfo 1]

    set setBlock [memBlockCreate -B $sizeToSet 0xaa]
 
    # Set bytes on the end of the text segment and compare the result
    # of wtxMemRead with $setBlock.
 
    set adToSet1 [format 0x%x [expr $dataAddr - $sizeToSet]]
    
    set resMemSet [wtxMemSet $adToSet1 $sizeToSet 0xaaaaaaaa]
    if {$resMemSet == ""} {
        cleanUp o $tModuleId
        return
	}

    set resRead [wtxMemRead $adToSet1 $sizeToSet]
    if {$resRead == ""} {
	cleanUp o $tModuleId
        return
        }

    if {[memBlockGet $resRead] != [memBlockGet $setBlock]} {
        testFail $errMemMsg(5)
	cleanUp o $tModuleId
        return
	}


    # Set bytes on the end of the text segment and the start of data
    # segment and compare the result of wtxMemRead with $setBlock. 

    set adToSet2 [format 0x%x [expr $dataAddr - [expr $sizeToSet / 2]]]
    set resMemSet [wtxMemSet $adToSet2 $sizeToSet 0xaaaaaaaa]
    if {$resMemSet == ""} {
	cleanUp o $tModuleId
        return
	}

    set resRead [wtxMemRead $adToSet2 $sizeToSet]
    if {$resRead == ""} {
        cleanUp o $tModuleId
        return
        }

    if {[memBlockGet $resRead] != [memBlockGet $setBlock]} {
        testFail $errMemMsg(5)
        cleanUp o $tModuleId
        return
        }


    # Set bytes on the start of the data segment and compare the result
    # of wtxMemRead with $setBlock.
    
    set adToSet3 $dataAddr
    set resMemSet [wtxMemSet $adToSet3 $sizeToSet 0xaaaaaaaa]
    if {$resMemSet == ""} {
        cleanUp o $tModuleId
        return
        }

    set resRead [wtxMemRead $adToSet3 $sizeToSet]
    if {$resRead == ""} {
        cleanUp o $tModuleId
        return
        }

    if {[memBlockGet $resRead] != [memBlockGet $setBlock]} {
        testFail $errMemMsg(5)
        cleanUp o $tModuleId
        return
        }

    # clean up
    cleanUp o $tModuleId
 
    wtxTestPass
}


#*****************************************************************************
#
# wtxMemReadTest5 - Verify wtxMemRead after wtxMemMove on a object module.
#
# This test loads an object module, and :
#
# - 1 -  it moves bytes on the end of the text segment of the object module
#	 and tries to read the moved block,
# - 2 -  it moves bytes on the end of the text segment and the start of the
#	 data segment and tries to read the moved block,
# - 3 -  it moves bytes on the start of the data segment and tries to read
#	 the moved block. 
#

proc wtxMemReadTest5 {Msg} {

 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # enable all events
    set st [wtxRegisterForEvent .*]
    if {$st == ""} {
	return
	}


    set sizeToMove 10

    # verify if objSampleWtxtclTest6.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest6.o
    if { [file exists $name] == 0 } {
          testFail "File $name is missing for wtxObjModuleLoadTest1 test"
          return
       }

    set tModuleId [wtxObjModuleLoad $USE_TSFORLOAD LOAD_GLOBAL_SYMBOLS $name]

    # get object module Id
    set tModuleId [lindex $tModuleId 0]

    set textInfo [lindex [wtxObjModuleInfoGet $name] 5]
    set dataInfo [lindex [wtxObjModuleInfoGet $name] 6]

    set textAddr [lindex $textInfo 1]
    set dataAddr [lindex $dataInfo 1]

    set moveBlock [memBlockCreate -B $sizeToMove 0xbb]

    set resMalloc [wtxMemAlloc $sizeToMove]
    if {$resMalloc == ""} {
        cleanUp o $tModuleId
	return
        }

    set resWrite [wtxMemWrite $moveBlock $resMalloc]
    if {$resWrite == ""} {
	cleanUp ms $resMalloc o $tModuleId
        return
        }

    # Moves bytes on the end of the text segment and compare the result
    # of wtxMemRead with $moveBlock.
 
    set adToMove1 [format 0x%x [expr $dataAddr - $sizeToMove]]
   
    set resMemMove [wtxMemMove $resMalloc $adToMove1 $sizeToMove]
    if {$resMemMove == ""} {
	cleanUp ms $resMalloc o $tModuleId
        return
	}

    set resRead [wtxMemRead $adToMove1 $sizeToMove]
    if {$resRead == ""} {
        cleanUp ms $resMalloc o $tModuleId
        return
        }

    if {[memBlockGet $resRead] != [memBlockGet $moveBlock]} {
	cleanUp ms $resMalloc o $tModuleId
        testFail $errMemMsg(5)
	return
	}


    # Move bytes on the end of the text segment and the start of data
    # segment and compare the result of wtxMemRead with $moveBlock. 

    set adToMove2 [format 0x%x [expr $dataAddr - [expr $sizeToMove / 2]]]

    set resMemMove [wtxMemMove $resMalloc $adToMove2 $sizeToMove]
    if {$resMemMove == ""} {
	cleanUp ms $resMalloc o $tModuleId
        return
	}

    set resRead [wtxMemRead $adToMove2 $sizeToMove]
    if {$resRead == ""} {
        cleanUp ms $resMalloc o $tModuleId
        return
        }

    if {[memBlockGet $resRead] != [memBlockGet $moveBlock]} {
        testFail $errMemMsg(5)
	cleanUp ms $resMalloc o $tModuleId
        return
        }


    # Move bytes on the start of the data segment and compare the result
    # of wtxMemRead with $setBlock.
    
    set adToMove3 $dataAddr

    set resMemMove [wtxMemMove $resMalloc $adToMove3 $sizeToMove]
    if {$resMemMove == ""} {
	cleanUp ms $resMalloc o $tModuleId
        return
        }

    set resRead [wtxMemRead $adToMove3 $sizeToMove]
    if {[memBlockGet $resRead] != [memBlockGet $moveBlock]} {
        testFail $errMemMsg(5)
	cleanUp ms $resMalloc o $tModuleId
        return
        }

    # clean up
    cleanUp ms $resMalloc o $tModuleId

    wtxTestPass
}

#*****************************************************************************
#
# wtxMemReadTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
# Furthermore it attaches itself to the target server (thru a wtxToolAttachCall
# command) and, when testcase units have been run, detaches itself from the
# target server.
#

proc wtxMemReadTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxMemReadTest targetServerName\
			timeout invalidAddr"
        return
    }

    # variables initializations
    global memReadMsg

    # connection to the target server
    wtxToolAttachCall $tgtsvrName 2 $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler


    # call the valids testcase units
    if {[set essai [wtxMemReadTest1 $memReadMsg(1)]]} {
	puts stdout "\t(Read a 10000 bytes memory block in\
	    $essai msec)"
    }

    wtxMemReadTest3  $memReadMsg(3)
    wtxMemReadTest4  $memReadMsg(4)
    wtxMemReadTest5  $memReadMsg(5)

    wtxErrorHandler [lindex [wtxHandle] 0] ""
 
    # invalids tests 
    wtxMemReadTest2  $memReadMsg(2) $invalidAddr


    # valid test in external mode :
    # if it's possible to change mode to external mode
    # wtxMemReadTest1 is run in external mode
    if {![catch "wtxAgentModeSet 2" agentSt]} {
	wtxErrorHandler [lindex [wtxHandle] 0] errorHandler
	puts stdout "\nSwitch to External Mode :"
	wtxMemReadTest1 $memReadMsg(1)
	wtxAgentModeSet 1
	}

    # detach from the target server
    wtxToolDetachCall "\nwtxMemReadTest : "
}
