# wtxDirectCallTest.tcl - Tcl script, test WTX_DIRECT_CALL
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01d,27may98,p_b  WTX 2.0 new changes
# 01c,28jan98,p_b  Adapted for WTX 2.0
# 01b,11mar97,jmp  modified catch format when a filename is given in
#                  parameters to avoid failures on windows side (SPR #7868).
# 01a,13nov95,jmp  written.
#
#

#
# DESCRIPTION
#
# WTX_DIRECT_CALL - Call a function on the target within the agent
#
# TESTS :
#
# (1) wtxDirectCallTest1 - Call a function on the target with valid
#			number of arguments.
#
# (2) wtxDirectCallTest2 - Call a function on the target with more
#			than ten arguments.
#

#*************************************************************************
#
# wtxDirectCallTest1 - Call a function on the target with valid number
#			of arguments.
#
# Load the object module of a function. This function needs ten arguments.
# The script tests wtxDirectCall() result when it calls this function on the
# target with ten arguments (maximum argument number for wtxDirectCall()
# routine).
#

proc wtxDirectCallTest1 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMiscMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # get path of objects
    set directCallTest [objPathGet test]funcCallTest.o

    # verify the object module exists
    if {[file exists $directCallTest] == 0} {
        testFail "cannot find file: $directCallTest"
        return
    }

    # load the object module of the file funcCallTest.c
    set resObjModuleLoad [wtxObjModuleLoad $USE_TSFORLOAD\
		LOAD_GLOBAL_SYMBOLS $directCallTest]
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

    # call this routine on the target in the context of the target agent.
    set resDirectCall [wtxDirectCall $taskAdd 1 2 3 4 5 6 7 8 9 10]
    if {$resDirectCall == ""} {
	cleanUp o $resObjModuleLoad
        return
    }


    # verify the result value of the executed function : the result must be 
    # the addition of the routine parameters, so must be equal to 55 
    if {$resDirectCall != 55} {
        testFail $errMiscMsg(9)
	cleanUp o $resObjModuleLoad
        return
    }

    # unload the object module to free the allocated space 
  #  cleanUp o $resObjModuleLoad

    wtxTestPass
}


#*************************************************************************
#
# wtxDirectCallTest2 - Call a function on the target with more than 
#			ten arguments.
#
# This test call wtxDirectCall with more than ten arguments and verify
# that it returns "usage: ..."
#

proc wtxDirectCallTest2 {Msg} {

    # array that contains the testcase error codes
    global errMiscMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # get path of objects
    set directCallTest [objPathGet test]funcCallTest.o

    # verify the object module exists
    if {[file exists $directCallTest] == 0} {
        testFail "cannot find file: $directCallTest"
        return
    }

    # load the object module of the file test.c
    catch {wtxObjModuleLoad $USE_TSFORLOAD\
			LOAD_GLOBAL_SYMBOLS $directCallTest}\
    resObjModuleLoad

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
    # to the number allowed by wtxDirectCall
    catch "wtxFuncCall $taskAdd 1 2 3 4 5 6 7 8 9 10 11 12" resDirectCall

    # unload the object module to free the sapce allocated
    cleanUp o $resObjModuleLoad

    # verify the function has not been called (too much parameter)
    if {[regexp {(^usage)} $resDirectCall] == 0} {
        testFail "$errMiscMsg(34)"
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

proc wtxDirectCallTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxFuncCallTest targetServerName\
				timeout invalidAddr"
        return
    }

    global directCallMsg

    # call tests with valid parameters
    wtxToolAttachCall $tgtsvrName 2 $timeout

    catch {wtxRegisterForEvent .*} st

    # test with valid parameters
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler
    wtxDirectCallTest1 $directCallMsg(1)

    # test with invalid parameters
    wtxErrorHandler [lindex [wtxHandle] 0] ""
    wtxDirectCallTest2 $directCallMsg(2)

    # detach from the target server
    wtxToolDetachCall "\nwtxDirectCallTest : "
}
