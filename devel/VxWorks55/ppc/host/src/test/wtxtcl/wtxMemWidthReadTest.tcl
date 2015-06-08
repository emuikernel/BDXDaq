# wtxMemWidthReadTest.tcl - Tcl script, test WTX_MEM_WIDTH_READ
#
# Copyright 1998 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01c,27may98,p_b  WTX 2.0 new changes
# 01b,14may98,p_b  Replace test?.o by objSampleWtxtclTest?.o
# 01a,28jan98,p_b  Complete import from wtxMemReadTest, plus modif.
#

#
# DESCRIPTION
#
# WTX_MEM_WIDTH_READ - Read a Block of Memory with WIDTH = 1,2 & 4
#
# (1) wtxMemWidthReadTest1 - Verify that wtxMemWidthRead() reads the right block of
#       memory, so that it reads the required number of bytes and that
#       it returns the right block contents.
#	Time the execution of  wtxMemWidthRead().
#
# (2) wtxMemWidthReadTest2 - Verify wtxMemWidthRead() with invalid <addr> argument
#       Verify that the result of wtxMemWidthRead() is S_wdb_MEM_ACCES_ERROR.
#
# (3) wtxMemWidthReadTest3 - Verify wtxMemWidthRead after wtxMemWrite on a object module.
#	Verify if wtxMemWidthRead can read correctly modifications made by wtxMemWrite.
#
# (4) wtxMemWidthReadTest4 - Verify wtxMemWidthRead after wtxMemSet on a object module.
#	Verify if wtxMemWidthRead can read correctly modifications made by wtxMemSet.
#
# (5) wtxMemWidthReadTest5 - Verify wtxMemWidthRead after wtxMemMove on a object module.
#	Verify if wtxMemWidthRead can read correctly modifications made by wtxMemMove.
#
#


#*****************************************************************************
#
# wtxMemWidthReadTest1 - Verify that wtxMemWidthRead reads the right block of
#                   memory, so that it reads the required number of bytes
#                   and that it returns the right block contents
#
# Write 10000 bytes initialized to 0x55 to the target and read these 10000
# bytes. The test verifies that 1000 bytes have been read and that the 
# read block contains the value 0x55 in each byte.  This script times the
# execution of  wtxMemWidthRead as well.
#


proc wtxMemWidthReadTest1 {nBytes Msg} {

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
    # time the execution of wtxMemWidthRead()
    set timeMsr [time {set mblock [wtxMemWidthRead $resMemAlloc 10000 $nBytes]} 1]
    if {$mblock == "" } {
	cleanUp mb $blockId ms $resMemAlloc
	return 0
    }

    # get the average  amount  of  time  required  per iteration for\
    # the execution of wtxMemWidthRead()
    set timeMsr [lindex $timeMsr 0]
    # make a conversion between micro seconds and milli seconds
    set timeMsrSec [ expr $timeMsr/1000]

    # get the contents of the read block
    set blkCont [memBlockGet $mblock]

    # Verify if  wtxMemWidthRead reads the right block of memory, if it reads 
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
# wtxMemWidthReadTest2 - Verify wtxMemWidthRead() with invalid <addr> argument
#
# This test calls wtxMemWidthRead() with the <addr> argument equals to an
# invalid address provided by invalidAddrGet.
# The test verifies that wtxMemWidthRead has returned an error code, and if
# the returned error code is AGENT_MEM_ACCESS_ERROR (To prevent from
# errors like TIMEOUT.
#

proc wtxMemWidthReadTest2 {nBytes Msg invalidAddr} {


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
    if {![catch "wtxMemWidthRead $invalidAdd 150 $nBytes" errCode]} {
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
# wtxMemWidthReadTest3 - Verify wtxMemWidthRead after wtxMemWrite on a object module.
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
proc wtxMemWidthReadTest3 {nBytes Msg} {

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
    # of wtxMemWidthRead with the block string.
 
    set adToWrite1 [format 0x%x [expr $dataAddr - $stringSize]]
    set resWrite [wtxMemWrite $stringBlock $adToWrite1]
    if {$resWrite == ""} {
        cleanUp o $tModuleId
        return
        }

    set resRead [wtxMemWidthRead $adToWrite1 $stringSize $nBytes]
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
    # segment and compare the result of wtxMemWidthRead with the block string.

    set adToWrite2 [format 0x%x [expr $dataAddr - [expr $stringSize / 2]]]
    set resWrite [wtxMemWrite $stringBlock $adToWrite2]
    if {$resWrite == ""} {
        cleanUp o $tModuleId
        return
        }

    set resRead [wtxMemWidthRead $adToWrite2 $stringSize $nBytes]
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
    # of wtxMemWidthRead with the block string.
    
    set adToWrite3 $dataAddr

    set resWrite [wtxMemWrite $stringBlock $adToWrite3]
    if {$resWrite == ""} {
        cleanUp o $tModuleId
        return
        }

    set resRead [wtxMemWidthRead $adToWrite3 $stringSize $nBytes]
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
# wtxMemWidthReadTest4 - Verify wtxMemWidthRead after wtxMemSet on a object module.
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

proc wtxMemWidthReadTest4 {nBytes Msg} {

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
    # of wtxMemWidthRead with $setBlock.
 
    set adToSet1 [format 0x%x [expr $dataAddr - $sizeToSet]]
    
    set resMemSet [wtxMemSet $adToSet1 $sizeToSet 0xaaaaaaaa]
    if {$resMemSet == ""} {
        cleanUp o $tModuleId
        return
	}

    set resRead [wtxMemWidthRead $adToSet1 $sizeToSet]
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
    # segment and compare the result of wtxMemWidthRead with $setBlock. 

    set adToSet2 [format 0x%x [expr $dataAddr - [expr $sizeToSet / 2]]]
    set resMemSet [wtxMemSet $adToSet2 $sizeToSet 0xaaaaaaaa]
    if {$resMemSet == ""} {
	cleanUp o $tModuleId
        return
	}

    set resRead [wtxMemWidthRead $adToSet2 $sizeToSet $nBytes]
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
    # of wtxMemWidthRead with $setBlock.
    
    set adToSet3 $dataAddr
    set resMemSet [wtxMemSet $adToSet3 $sizeToSet 0xaaaaaaaa]
    if {$resMemSet == ""} {
        cleanUp o $tModuleId
        return
        }

    set resRead [wtxMemWidthRead $adToSet3 $sizeToSet $nBytes]
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
# wtxMemWidthReadTest5 - Verify wtxMemWidthRead after wtxMemMove on a object module.
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

proc wtxMemWidthReadTest5 {nBytes Msg} {

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
    # of wtxMemWidthRead with $moveBlock.
 
    set adToMove1 [format 0x%x [expr $dataAddr - $sizeToMove]]
   
    set resMemMove [wtxMemMove $resMalloc $adToMove1 $sizeToMove]
    if {$resMemMove == ""} {
	cleanUp ms $resMalloc o $tModuleId
        return
	}

    set resRead [wtxMemWidthRead $adToMove1 $sizeToMove $nBytes]
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
    # segment and compare the result of wtxMemWidthRead with $moveBlock. 

    set adToMove2 [format 0x%x [expr $dataAddr - [expr $sizeToMove / 2]]]

    set resMemMove [wtxMemMove $resMalloc $adToMove2 $sizeToMove]
    if {$resMemMove == ""} {
	cleanUp ms $resMalloc o $tModuleId
        return
	}

    set resRead [wtxMemWidthRead $adToMove2 $sizeToMove $nBytes]
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
    # of wtxMemWidthRead with $setBlock.
    
    set adToMove3 $dataAddr

    set resMemMove [wtxMemMove $resMalloc $adToMove3 $sizeToMove]
    if {$resMemMove == ""} {
	cleanUp ms $resMalloc o $tModuleId
        return
        }

    set resRead [wtxMemWidthRead $adToMove3 $sizeToMove $nBytes]
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
# wtxMemWidthReadTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
# Furthermore it attaches itself to the target server (thru a wtxToolAttachCall
# command) and, when testcase units have been run, detaches itself from the
# target server.
#

proc wtxMemWidthReadTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxMemWidthReadTest targetServerName\
			timeout invalidAddr"
        return
    }

    # variables initializations
    global memWidthReadMsg

    # connection to the target server

    wtxToolAttachCall $tgtsvrName 2 $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    set NBTESTS 15
    foreach nBytes {1 2 4} {
     puts stdout "wtxMemWidthRead with <nBytes=$nBytes>"

     # call the valids testcase units
     if {[set essai [wtxMemWidthReadTest1 $nBytes $memWidthReadMsg(1)]]} {
	puts stdout "\t(Read a 10000 bytes memory block in\
	    $essai msec)"
     }

     wtxMemWidthReadTest3 $nBytes $memWidthReadMsg(3)
     wtxMemWidthReadTest4 $nBytes $memWidthReadMsg(4)
     wtxMemWidthReadTest5 $nBytes $memWidthReadMsg(5)

     wtxErrorHandler [lindex [wtxHandle] 0] ""
 
     # invalids tests 
     wtxMemWidthReadTest2 $nBytes $memWidthReadMsg(2) $invalidAddr


     # valid test in external mode :
     # if it's possible to change mode to external mode
     # wtxMemWidthReadTest1 is run in external mode
     if {![catch "wtxAgentModeSet 2" agentSt]} {
	wtxErrorHandler [lindex [wtxHandle] 0] errorHandler
	puts stdout "\nSwitch to External Mode :"
	wtxMemWidthReadTest1 $nBytes $memWidthReadMsg(1)
	wtxAgentModeSet 1
	}
     }
     # detach from the target server
     wtxToolDetachCall "\nwtxMemWidthReadTest : "
}
