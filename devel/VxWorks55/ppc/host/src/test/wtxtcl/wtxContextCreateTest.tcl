# wtxContextCreateTest.tcl - Tcl script, test WTX_CONTEXT_CREATE
#
# Copyright 1995-1997 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01q,14may98,p_b  Replace test?.o by objSampleWtxtclTest?.o
# 01p,28jan98,p_b  Adapted for WTX 2.0
# 01o,06aug97,jmp  Added a one second temporization between a call to
#                  wtxContextResume and the gopher evaluation that verifies
#                  results of ContextResume - SPR #8441.
# 01n,20oct95,jmp  added invalidAddr argument to wtxContextCreateTest.
# 01m,11sep95,jmp  added wtxContextCreateTest2.
# 01l,22aug95,jmp  remove useless malloc for wtxContextCreate,
#		   now the test uses testTaskCreate and verifies if
#		   called functions return correct values.
#		   uses cleanUp prodedure.
#		   added timeout to wtxToolAttachCall in wtxContextCreateTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01k,23jun95,f_v  added wtxTestRun call
# 01j,02jun95,f_v  updated wtxContextCreate call
# 01i,24may95,f_v  removed wtxContextCreateTest2
# 01h,27apr95,c_s  now uses wtxErrorMatch.
# 01g,07apr95,f_v  added wtxContextCreateTest2 and new agentErr tests
#		   quit test more properly
# 01f,30mar95,f_v  changed pomme to test1
# 01e,20mar95,f_v  changed printLogo by a user task in wtxContextCreateTest1
# 01c,09mar95,f_v  delete CtxMsg messages 
# 01b,26Jan95,f_v  change access messages, use wtxToolDetachCall
# 01a,16Jan95,f_v  written. 
#
#
#
# DESCRIPTION
#
# WTX_CONTEXT_CREATE  -  Create a context and leave suspended
#
# (1) wtxContextCreateTest1 - test wtxContextCreate with right arguments
#
# (2) wtxContextCreateTest2 - test wtxContextCreate with file redirection
#


#*****************************************************************************
#
# wtxContextCreateTest1 - test wtxContextCreate with right arguments
#
# Verify if wtxContextCreate create a new context, and if the called function
# return rights values.
#

proc wtxContextCreateTest1 {Msg} {

    global errCtxMsg
    puts stdout "$Msg" nonewline

    # create a new context
    set taskDesc2 [testTaskCreate $Msg objSampleWtxtclTest2.o test2]
    if {$taskDesc2 == 1} {
        return
    }
    set moduleId2 [lindex $taskDesc2 1]
    set taskId2 [lindex $taskDesc2 0]


    # create a new context
    set taskDesc3 [testTaskCreate $Msg objSampleWtxtclTest3.o test3]
    if {$taskDesc3 == 1} {
        return
    }
    set moduleId3 [lindex $taskDesc3 1]
    set taskId3 [lindex $taskDesc3 0]

    # create a new context
    set taskDesc1 [testTaskCreate $Msg objSampleWtxtclTest1.o test1]
    if {$taskDesc1 == 1} {
        return
    }

    set moduleId1 [lindex $taskDesc1 1]

    # get task Id
    set taskId [lindex $taskDesc1 0]

    # get all the tasks
    set taskInfoList [activeTaskNameMap]

    # get the position of the new task in the list 
     set IdPosition [lsearch $taskInfoList "Test[pid]"]
     if { $IdPosition < 0 } {
	 testFail $errCtxMsg(13)
	 cleanUp t $taskDesc1 $taskDesc2 $taskDesc3
	 return
     }

    # resume test1 task
    set resuSt [wtxContextResume CONTEXT_TASK $taskId]
    if {$resuSt == ""} {
	   cleanUp t $taskDesc1 $taskDesc2 $taskDesc3
           return
         }

    # Wait during 1 second, to make sure that contextResume is completed
    # before running the gopher evaluation that verifies results of the
    # function run by the contextResume.
    # This isn't a foolproof synchronization method, but ...
    after 1000

    # Verifie that test1 works correctly :
    # resTest2 is the result of test2 call, it should be equal to 10
    # resTest3 is the result of test3 call, it should be equal to 5
    set resulAddr [lindex [wtxSymFind -name "resTest2"] 1]
    set result [wtxGopherEval -errvar errCode "$resulAddr *!"]
    if {$result != 10} {
        testFail $errCtxMsg(16)
        cleanUp t $taskDesc2 $taskDesc3 o $moduleId1
        return
        }

    set resulAddr [lindex [wtxSymFind -name "resTest3"] 1]
    set result [wtxGopherEval -errvar errCode "$resulAddr *!"]
    if {$result != 5} {
        testFail $errCtxMsg(16)
        cleanUp t $taskDesc2 $taskDesc3 o $moduleId1
        return
        }

    # clean up
    cleanUp t $taskDesc2 $taskDesc3 o $moduleId1

    wtxTestPass
}


#*****************************************************************************
#
# wtxContextCreateTest2 - test wtxContextCreate with file redirection
#
# This test opens a file on VIO channel x , and creates a context with
# redirection to the opened file.
# To verify that the redirection works properly, the test checks the
# returned values of wtxEventPoll.
# wtxEventPoll should return {VIO_WRITE channelId mblk}
# 

proc wtxContextCreateTest2 {Msg} {

    # array that contains the testcase error codes
    global errVioMsg
    global errMiscMsg

    puts stdout "$Msg" nonewline

    # verify that printf symbol exist on vxWorks module
    # If printf does not exist, the test mustn't be run
    # It's necessary to remove the errorHanler procedure
    # to not generate an error message

    # save active handler to restore it after wtxSymFind call
    set oldHandler [wtxErrorHandler [lindex [wtxHandle] 0]]

    # remove active handler
    wtxErrorHandler [lindex [wtxHandle] 0] ""

    if {[catch "wtxSymFind -name printf" infoPrintf]} {
        puts stdout $errMiscMsg(25)
        return
        }

    # restore old handler
    wtxErrorHandler [lindex [wtxHandle] 0] $oldHandler

    set name test[pid]

 # OPEN VIO
    set resVioOpen [vioOpen]
    if {[lindex $resVioOpen 0] == 0} {
        puts stdout $errVioMsg(20)
        return
    }

    set fileDescriptor  [lindex $resVioOpen 0]
    set channelNb  [lindex $resVioOpen 1]


    # Create a block with the string to print and write it
    # at the address return by wtxMemAlloc
    set stringToPrint "\nHello to vxWorks users !\n"
    set stringBlck [memBlockCreate -string $stringToPrint]
    set stringAddr [wtxMemAlloc [lindex [memBlockInfo $stringBlck] 1]]
    if {$stringAddr == ""} {
	vioClose $fileDescriptor $channelNb
	cleanUp mb $stringBlck
	return
	}

    set numBytes [wtxMemWrite $stringBlck $stringAddr]
    if {$numBytes == ""} {
	vioClose $fileDescriptor $channelNb
	cleanUp ms $stringAddr mb $stringBlck
	return
	}

    set resRegist [wtxRegisterForEvent ".*"]
    if {$resRegist == ""} {
	vioClose $fileDescriptor $channelNb
	cleanUp ms $stringAddr mb $stringBlck
        return
        }

   # get printf address
    set printfAddr [lindex $infoPrintf 1]

    set resCreate [wtxContextCreate CONTEXT_TASK $name 100 0 0 0\
		   $printfAddr $fileDescriptor $fileDescriptor $stringAddr]
    if {$resCreate == ""} {
	vioClose $fileDescriptor $channelNb
	cleanUp ms $stringAddr mb $stringBlck
	return
	}

    set taskId [lindex $resCreate 0]
    set resResume [wtxContextResume CONTEXT_TASK $taskId]
    if {$resResume == ""} {
	vioClose $fileDescriptor $channelNb
	cleanUp ms $stringAddr mb $stringBlck
	return
	}

    # wait for VIO_WRITE event
    set resEventPoll [wtxEventPoll 100 50]
    if {$resEventPoll == ""} {
	vioClose $fileDescriptor $channelNb
	cleanUp ms $stringAddr mb $stringBlck
        return
        }

    # wait for event until we get VIO_WRITE event
    while {[string compare [lindex $resEventPoll 0] VIO_WRITE]} {
        # wait for event
        set resEventPoll [wtxEventPoll 100 50]
        if {$resEventPoll == ""} {
	    vioClose $fileDescriptor $channelNb
	    cleanUp ms $stringAddr mb $stringBlck
            return
            }
        }

    # the returned event is {VIO_WRITE channelId mblck}
    # It's necessary to verify if these values are corrects
    set returnChannel [lindex $resEventPoll 1]
    set returnMemBlock [lindex $resEventPoll 2]

    # Remove the end of string charactere of stringBlck
    # to compare with the returned memory block
    set length [llength [memBlockGet $stringBlck]]
    set list [lrange [memBlockGet $stringBlck] 0 [expr $length -2]]

    set list2 [lrange [memBlockGet $returnMemBlock] 0 [expr $length -1]]

    # Clean up
    vioClose $fileDescriptor $channelNb
    cleanUp ms $stringAddr mb $stringBlck $returnMemBlock

    if {$list2 != $list || $returnChannel != $channelNb} {
	testFail $errMiscMsg(24)
	return
	}

    wtxTestPass
}



#*****************************************************************************
#
# wtxContextCreateTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxContextCreateTest {tgtsvrName timeout invalidAddr} {

    global vCtxMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxContextCreateTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # call the test procedures
    wtxContextCreateTest1 $vCtxMsg(1)
    wtxContextCreateTest2 $vCtxMsg(10)

    # deconnection
    wtxToolDetachCall "wtxContextCreateTest"
}
