# wtxFuncCallTest.tcl - Tcl script, test WTX_FUNC_CALL 
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01t,27may98,p_b  WTX 2.0 new changes
# 01s,28jan98,p_b  wpwrLibPathTest calls removed, WTX 2.0 adapted.
# 01r,11mar97,jmp  modified catch format when a filename is given in
#                  parameters to avoid failures on windows side (SPR #7868).
# 01q,08nov95,s_w  modify for SPR 5399 fix. OBJ_LOADED event no longer received
# 01p,20oct95,jmp  added invalidAddr argument to wtxFuncCallTest.
# 01o,07sep95,jmp  added wtxFuncCallTest3, wtxFuncCallTest4, wtxFuncCallTest5
# 01n,22aug95,jmp  changed objGetPath to objPathGet
#		   added timeout to wtxToolAttachCall in wtxFuncCallTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01m,23jun95,f_v  added wtxTestRun call
# 01l,21jun95,f_v  clean up main procedure
# 01k,02jun95,f_v  replaced strict test by regular exp in wtxFuncCallTest2
# 01j,28apr95,c_s  now uses wtxErrorMatch.
# 01i,28mar95,f_v  added wtxRegisterForEvent
# 01h,17mar95,f_v  add lindex to get objModule Id
#		   implementation of wtxFuncCallTest2
# 01g,08mar95,f_v  replace S_loadLib_MODULE_NOT_FOUND by
#		   objModuleErr(WTX_ERR_LOADER_OBJ_MODULE_NOT_FOUND),
#		   S_tgtMemLib_NOT_ENOUGH_MEMORY by
#		   tgtMemErr(WTX_ERR_TGTMEM_NOT_ENOUGH_MEMORY)
# 01f,20feb95,kpn  added argument 'test' to objGetPath(), 
#		   added vxWorksErr in wtxFuncCallTest1.
# 01e,06feb95,kpn  added test : verify object module exists.
# 01d,27jan95,kpn  deplaced testcase units description and testcase units
#                  errors in wtxTestMsg.tcl,
#                  added global funcCallMsg in the main.
#		   added global errMiscMsg in each testcase units,
#                  used funcCallMsg & errMiscMsg to manage messages.
# 01c,26Jan95,kpn  removed the calls to wtxToolAttachCall() and wtxToolDetach()
#                  made in each testcase.
#                  added wtxToolAttachCall() and wtxToolDetachCall() in the main
#                  procedure.
#		   removed error codes initialization, added global variables.
# 01b,17Jan95,kpn  written wtxFuncCallTest1{} and wtxFuncCallTest2{}.
#		   documentation.
# 01a,09Jan95,kpn  written.
#
#

#
# DESCRIPTION
#
# WTX_FUNC_CALL - Call a Function on the Target
#
# TESTS :
#
# (1) wtxFuncCallTest1 - This script verifies the result of the wtxFuncCall()
#       routine when it's called with valid integer parameters.
#
# (2) wtxFuncCallTest2 - Verify wtxFuncCall() result when it calls a function 
#	with too many arguments. 
#
# (3) wtxFuncCallTest3 - This script verifies the result of the wtxFuncCall()
#       routine when it's called with valid floating parameters.
#
# (4) wtxFuncCallTest4 - This script verifies the result of the wtxFuncCall()
#       routine when it's called with valid double parameters.
#
# (5) wtxFuncCallTest5 - This script verifies that wtxFuncCall() manages
#	correctly file redirection.
#

#*************************************************************************
#
# wtxFuncCallTest1 - verify wtxFuncCall() with valid arguments.
#
# Load the object module of a function. This function needs ten arguments.
# The script tests wtxFuncCall() result when it calls this function on the
# target with ten arguments (maximum argument number for wtxFuncCall() routine).
#

proc wtxFuncCallTest1 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMiscMsg
    global errMemMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # get path of objects

    set funcCallTest [objPathGet test]funcCallTest.o

    # verify the object module exists
    if {[file exists $funcCallTest] == 0} {
        testFail "cannot find file: $funcCallTest"
        return
    }

    # load the object module of the file funcCallTest.c
    set resObjModuleLoad [wtxObjModuleLoad\
		$USE_TSFORLOAD LOAD_GLOBAL_SYMBOLS $funcCallTest]
    if {$resObjModuleLoad == ""} {
	return
    }

    # get ObjModule Id
    set resObjModuleLoad [lindex $resObjModuleLoad 0]

    # verify the load has succeeded
    set resObjModuleFind [wtxObjModuleFind $resObjModuleLoad]
    if {$resObjModuleFind == ""} {
	return
    }

    # search the symbol "funcCallIntTest" by name, get the address of\
    # the routine

    set resFind [wtxSymFind -name "funcCallIntTest"]
    if {$resFind == ""} {
	cleanUp o $resObjModuleLoad
	return
    }

    set taskAdd [lindex $resFind 1]

    # call this routine on the target, execute it in the context of the WDB
    # server, returns the context id of the routine.
    set resFuncCall [wtxFuncCall $taskAdd 1 2 3 4 5 6 7 8 9 10]
    if {$resFuncCall == ""} {
	cleanUp o $resObjModuleLoad
        return
    }

    # wait for event
    set resEventPoll2 [wtxEventPoll 100]
    while { [string compare [lindex $resEventPoll2 0] CALL_RETURN] && \
	    [string compare [lindex $resEventPoll2 1] $resFuncCall]} {
	       set resEventPoll2 [wtxEventPoll 100]
    }


    # get the routine result
    set result [lindex $resEventPoll2 2]
    # puts stdout $result

    # verify the result value of the executed function : the result must be 
    # the addition of the routine parameters, so must be equal to 55 
    if {$result != 55} {
        testFail $errMiscMsg(9)
	cleanUp o $resObjModuleLoad
        return
    }

    # unload the object module to free the allocated space 
    cleanUp o $resObjModuleLoad

    wtxTestPass
}

#*************************************************************************
#
# wtxFuncCallTest2 - Verify wtxFuncCall with too many arguments 
#
# Verify that a function is not called when wtxFuncCall() has too many function
# parameters 
#

proc wtxFuncCallTest2 { Msg } {

    puts stdout "$Msg" nonewline
 
 # GLOBAL VARIABLES
 
    # array that contains the testcase error codes
    global errMiscMsg
    global USE_TSFORLOAD
 
    # get path of objects
 
    set funcCallTest [objPathGet test]funcCallTest.o

    # verify the object module exists
    if {[file exists $funcCallTest] == 0} {
        testFail "cannot find file $funcCallTest"
        return
    }
 
    # load the object module of the file test.c
    catch {wtxObjModuleLoad $USE_TSFORLOAD LOAD_GLOBAL_SYMBOLS\
        $funcCallTest} resObjModuleLoad
    # verify the object module exists
    if {[file exists $funcCallTest] == 0} {
        testFail "cannot find file $funcCallTest"
        return
    }
 
    # load the object module of the file test.c
    catch {wtxObjModuleLoad $USE_TSFORLOAD LOAD_GLOBAL_SYMBOLS\
        $funcCallTest} resObjModuleLoad

    # get objModule Id
    set resObjModuleLoad [lindex $resObjModuleLoad 0]
 
    # verify the load has succeeded
    if {[catch "wtxObjModuleFind $resObjModuleLoad" resObjModuleFind]} {
        testFail $resObjModuleFind
        return
    }
 
    # search the symbol "funcCallTest" by name, get the address of
    # the routine
    set taskAdd [lindex [wtxSymFind -name "funcCallIntTest"] 1]

    
    # call this routine on the target with a number of parameters superior
    # to the number allowed by wtxFuncCall
    catch "wtxFuncCall $taskAdd 1 2 3 4 5 6 7 8 9 10 11 12" resFuncCall

    # unload the object module to free the sapce allocated
    catch "wtxObjModuleUnload $resObjModuleLoad" resObjModuleUnload

    if {$resObjModuleUnload != 0} {
        testFail "$errMiscMsg(11)"
    }

    # verify the function has not been called (too much parameter)
    if {[regexp {(^usage)} $resFuncCall] == 0} {
        testFail "$errMiscMsg(10)"
        return
    }
 
    wtxTestPass
}

#*************************************************************************
#
# wtxFuncCallTest3 - verify wtxFuncCall() with valid floating arguments.
#
# Load the object module of a function. This function needs ten floating
# arguments.
# The script tests wtxFuncCall() result when it calls this function on the
# target with ten arguments (maximum argument number for wtxFuncCall() routine).
#

proc wtxFuncCallTest3 {Msg} {

 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMiscMsg
    global errMemMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # test if floating point are supported
    if {[lindex [lindex [wtxTsInfoGet] 2] 1] == 0} {
	puts stdout $errMiscMsg(23)
	return
    }

    set funcCallTest [objPathGet test]funcCallTest.o

    # verify the object module exists
    if {[file exists $funcCallTest] == 0} {
        testFail "cannot find file: $funcCallTest"
        return
    }

    # load the object module of the file funcCallTest.c
    set resObjModuleLoad [wtxObjModuleLoad $USE_TSFORLOAD\
   				 LOAD_GLOBAL_SYMBOLS $funcCallTest]
    if {$resObjModuleLoad == ""} {
	return
    }

    # get ObjModule Id
    set resObjModuleLoad [lindex $resObjModuleLoad 0]

    # verify the load has succeeded
    set resObjModuleFind [wtxObjModuleFind $resObjModuleLoad]
    if {$resObjModuleFind == ""} {
	return
    }

    # search the symbol "funcCallFloatTest" by name, get the address of\
    # the routine
    set resFind [wtxSymFind -name "funcCallFloatTest"]
    if {$resFind == ""} {
	cleanUp o $resObjModuleLoad
	return
    }

    set taskAdd [lindex $resFind 1]

    # calls this routine on the target, executes it in the context of the WDB
    # server, returns the context id of the routine.
    if {[catch {wtxFuncCall -float $taskAdd 0 1 0 3 0 0 0 0 0 0}\
      resFuncCall]} {
       return	
    }

    if {$resFuncCall == ""} {
	cleanUp o $resObjModuleLoad
        return
    }

    # wait for event
    set resEventPoll2 [wtxEventPoll 100]
    while { [string compare [lindex $resEventPoll2 0] CALL_RETURN] && \
	    [string compare [lindex $resEventPoll2 1] $resFuncCall]} {
	       set resEventPoll2 [wtxEventPoll 100]
    }

    # get the routine result
    set result [lindex $resEventPoll2 2]

    # verify the result value of the executed function : the result must be 
    # the addition of the routine parameters, so must be equal to 0 
    # puts stdout $result
    if {$result != 0} {
        testFail $errMiscMsg(9)
	cleanUp o $resObjModuleLoad
        return
    }

    # unload the object module to free the allocated space 
    cleanUp o $resObjModuleLoad

    wtxTestPass
}


#*************************************************************************
#
# wtxFuncCallTest4 - verify wtxFuncCall() with valid double arguments.
#
# Load the object module of a function. This function needs ten double
# arguments.
# The script tests wtxFuncCall() result when it calls this function on the
# target with ten arguments (maximum argument number for wtxFuncCall() routine).
#

proc wtxFuncCallTest4 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMiscMsg
    global errMemMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # test if floating point are supported
    if {[lindex [lindex [wtxTsInfoGet] 2] 1] == 0} {
	puts stdout $errMiscMsg(23)
	return
    }

    set funcCallTest [objPathGet test]funcCallTest.o

    # verify the object module exists
    if {[file exists $funcCallTest] == 0} {
        testFail "cannot find file: $funcCallTest"
        return
    }

    # load the object module of the file funcCallTest.c
    set resObjModuleLoad [wtxObjModuleLoad $USE_TSFORLOAD\
			LOAD_GLOBAL_SYMBOLS $funcCallTest]
    if {$resObjModuleLoad == ""} {
	return
	}

    # get ObjModule Id
    set resObjModuleLoad [lindex $resObjModuleLoad 0]

    # verify the load has succeeded
    set resObjModuleFind [wtxObjModuleFind $resObjModuleLoad]
    if {$resObjModuleFind == ""} {
	return
    }

    # search the symbol "funcCallDoubleTest" by name, get the address of\
    # the routine
    set resFind [wtxSymFind -name "funcCallDoubleTest"]
    if {$resFind == ""} {
	cleanUp o $resObjModuleLoad
        return
    }

    set taskAdd [lindex $resFind 1]

    # call this routine on the target, execute it in the context of the WDB
    # server, returns the context id of the routine.
    if {[catch {wtxFuncCall -double $taskAdd 1 2 3 4 5 6 7 8 9 10}\
	 resFuncCall]} { 
       return
    }

    if {$resFuncCall == ""} {
	cleanUp o $resObjModuleLoad
        return
    }

    # wait for event
    set resEventPoll2 [wtxEventPoll 100]
    while { [string compare [lindex $resEventPoll2 0] CALL_RETURN] && \
	    [string compare [lindex $resEventPoll2 1] $resFuncCall]} {
	       set resEventPoll2 [wtxEventPoll 100]
    }


    # get the routine result
    set result [lindex $resEventPoll2 2]
    # puts stdout $result

    # verify the result value of the executed function : the result must be 
    # the addition of the routine parameters, so must be equal to 55 
    if {$result != 55} {
        testFail $errMiscMsg(9)
	cleanUp o $resObjModuleLoad
        return
    }

    # unload the object module to free the allocated space 
    cleanUp o $resObjModuleLoad

    wtxTestPass
}


#*************************************************************************
#
# wtxFuncCallTest5 - verify wtxFuncCall() with file redirection
#
# This test opens a file on VIO channel x , and calls wtxFuncCall with
# "-redir" option set to the opened file.
# To verify that the redirection works properly, the test checks the
# returned values of wtxEventPoll.
# wtxEventPoll should return {VIO_WRITE channelId mblk}
#

proc wtxFuncCallTest5 {Msg} {

 # GLOBAL VARIABLES
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

    # call wtxFuncCall with the "-redir" option for redirection
    if [catch {wtxFuncCall -redir $fileDescriptor \
		       $printfAddr $stringAddr} funcId] {
       testFail "\tCan not manage -redir parameter"
       return
    }

    # wait for VIO_WRITE event
    set resEventPoll [wtxEventPoll 100 50]
    if {$resEventPoll == ""} {
	funcCall close $fileDescriptor
	wtxVioChanRelease $channelNb
	cleanUp ms $stringAddr mb $stringBlck
	return
    }

    while {[string compare [lindex $resEventPoll 0] VIO_WRITE]} {
	# wait for event
	set resEventPoll [wtxEventPoll 100 50]
	if {$resEventPoll == ""} {
	    funcCall close $fileDescriptor
	    wtxVioChanRelease $channelNb
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

    # clean up :
    funcCall close $fileDescriptor
    wtxVioChanRelease $channelNb
    cleanUp ms $stringAddr mb $returnMemBlock $stringBlck

    if {$list2 != $list || $returnChannel != $channelNb} {
	testFail $errMiscMsg(24)
	return
    }

    wtxTestPass
}

#*************************************************************************
#
# wtxFuncCallTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the 
# testcase units and calls each testcase procedure contained in the test script.
#

proc wtxFuncCallTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxFuncCallTest targetServerName\
				timeout invalidAddr"
        return
    }

    global funcCallMsg

    # call tests with valid parameters
    wtxToolAttachCall $tgtsvrName 2 $timeout

    catch {wtxRegisterForEvent .*} st

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    wtxFuncCallTest1 $funcCallMsg(1)
    # wtxFuncCallTest3 $funcCallMsg(3)
    # wtxFuncCallTest4 $funcCallMsg(4)
    # wtxFuncCallTest5 $funcCallMsg(5)

    wtxErrorHandler [lindex [wtxHandle] 0] ""

    # call tests with invalid parameters

    wtxFuncCallTest2 $funcCallMsg(2)

    # detach from the target server
    wtxToolDetachCall "\nwtxFuncCallTest : "
}
