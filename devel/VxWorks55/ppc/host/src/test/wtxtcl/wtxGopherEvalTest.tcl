# wtxGopherEvalTest.tcl - Tcl script, test WTX_GOPHER_EVAL
#
# Copyright 1995-1997 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01p,27may98,p_b  WTX 2.0 new changes
# 01o,28jan98,p_b  Adapted for WTX 2.0
# 01n,22sep97,jmp  Threat errors set while using -errvar option.
# 01m,06aug97,jmp  Replaced a wtxFuncCall by a funcCall to fixe SPR #8435.
# 01l,09dec96,elp  added null length string test in wtxGopherEvalTest1.
# 01k,11jul96,jmp  modified description messages in wtxGopherEvalTest.
# 01j,26apr96,jmp  added wtxGopherEvalTest[3->5] to cover SPR #6403.
# 01i,20oct95,jmp  added invalidAddr argument to wtxGopherEvalTest.
# 01h,22aug95,jmp  added timeout to wtxToolAttachCall in wtxGopherEvalTest,
#		   changed objGetPath to objPathGet.
# 01g,26jun95,f_v  added gfree call
# 01f,23jun95,f_v  added wtxTestRun call
# 01e,21jun95,f_v  clean up main procedure
# 01d,12may95,c_s  upgraded for new wtxtcl version.
# 01c,10may95,f_v  now uses -1 instead of 0 in wtxGopherEvalTest2
# 01b,27apr95,c_s  now uses wtxErrorMatch.
# 01a,10apr95,f_v  written. 
#
#
#
# DESCRIPTION
#
# WTX_GOPHER_EVAL  -  Evaluate a gopher expression
#
# (1) wtxGopherEvalTest1 Verify if wtxGopherEval ruturns right information
#     when gopher expression is valid 
#
# (2) wtxGopherEvalTest2 Verify if wtxGopherEval ruturns right error code
#     when gopher expression is wrong
#     error code are :
#	WTX_ERR_AGENT_GOPHER_FAULT and WTX_ERR_AGENT_GOPHER_TRUNCATED 
#

#*****************************************************************************
#
# wtxGopherEvalTest1 -
#
# Verify if wtxGopherEval evaluate gopher expression with valid parameters
#

proc wtxGopherEvalTest1 {Msg} {

    global errMiscMsg
    global agentErrMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # verify if gopher.o exist in right directory
    set name [objPathGet test]gopher.o
    if { [file exists $name] == 0 } {
          testFail "File $name is missing for wtxGopherEvalTest1 test"
          return
       }
    # load gopher.o file
    if {[catch {wtxObjModuleLoad $USE_TSFORLOAD [objPathGet test]gopher.o} tModuleId]} {
	testFail $tModuleId
	return
	}

    set tModuleId [lindex $tModuleId 0]

    # create a task
    funcCall gdemo

    # search pointer to get its address 
    set sym nodeList
    set nodelist [lindex [wtxSymFind -name "$sym"] 1]

    # get info pointed to by nodeList
    catch {wtxGopherEval -errvar errCode "$nodelist * { @ < +8 *$ > * }"} tape
    catch {wtxGopherEval -errvar errCode2 \
	    "$nodelist * { +4 < +4 * {@@ 0} > * }"} tape2

    if {[wtxErrorMatch $errCode AGENT_GOPHER_FAULT] ||\
        [wtxErrorMatch $errCode2 AGENT_GOPHER_FAULT]} {
	funcCall gfree
        testFail $agentErrMsg(9)
        return

    } else {
	
	if {$errCode != ""} {
	    testFail $errCode
	    return
	}
	if {$errCode2  != ""} {
	    testFail $errCode2
	    return
	}
    }

    set Id1   [lindex $tape 0]
    set Id2   [lindex $tape 2]
    set Id3   [lindex $tape 4]
    set Name1 [lindex $tape 1]
    set Name2 [lindex $tape 3]
    set Name3 [lindex $tape 5]

    set x11   [lindex $tape2 0]
    set x21   [lindex $tape2 1]
    set x12   [lindex $tape2 2]
    set x22   [lindex $tape2 3]

    # clean up
    funcCall gfree
    cleanUp o $tModuleId

    if {$Id1 != 0x991 || $Id2 != 0x765 || $Id3 != 0x341 ||\
	$Name1 != "banana" || $Name2 != "cherry" || $Name3 != "lemon"||\
	$x11 != 0xccc || $x21 != 0xddd || $x12 != 0xaaa || $x22 != 0xbbb} {

        testFail $errMiscMsg(17)
	return
    } 

    # test gopher on a null length string
    
    # create a 10 bytes block
    set bAddrs [funcCall malloc 10]

    # create a block (endianness is not a matter)
    set blkId [memBlockCreate -B 10 0x00]
    wtxMemWrite $blkId $bAddrs
    memBlockDelete $blkId

    if [catch {wtxGopherEval "$bAddrs $"} result] {
	testFail $errMiscMsg(17)
	return
    } else {
	if {[string compare $result "{}"] != 0} {
	    testFail $errMiscMsg(17)
	    return
	}
    }

    cleanUp mt $bAddrs

    wtxTestPass
}


#*****************************************************************************
#
# wtxGopherEvalTest2 -
#
# Verify if wtxGopherEval returns right error code when gopher expression
# is wrong
#

proc wtxGopherEvalTest2 {Msg} {

    global agentErrMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # get info pointed to by nodeList
    catch {wtxGopherEval -errvar errCode "-1 @"} tape

    if {[wtxErrorMatch $errCode TIMEOUT]} {
	testFail $errCode
	return
    }

    if {![wtxErrorMatch $errCode AGENT_GOPHER_FAULT] } {
        testFail $agentErrMsg(8)
        return
    } 


    # verify if gopher.o exist in right directory
    set name [objPathGet test]gopher.o
    if { [file exists $name] == 0 } {
          testFail "File $name is missing for wtxGopherEvalTest1 test"
          return
       }

    # load gopher.o file
    if {[catch {wtxObjModuleLoad $USE_TSFORLOAD $name} tModuleId]} {
	testFail $tModuleId
	return
	}

    set tModuleId [lindex $tModuleId 0]

    # create a task
    funcCall gdemo

    # search pointer to get its address 
    set sym nodeList
    set nodelist [lindex [wtxSymFind -name "$sym"] 1]

    # get info pointed to by nodeList
    catch {wtxGopherEval -errvar errCode "$nodelist { @ }"} tape

    # clean up
    funcCall gfree
    cleanUp o $tModuleId

    if {[wtxErrorMatch $errCode TIMEOUT]} {
	testFail $errCode
	return
    }

    if {![wtxErrorMatch $errCode AGENT_GOPHER_TRUNCATED] } {
        testFail $agentErrMsg(7)
        return
    } 

    wtxTestPass
}


#*****************************************************************************
#
# wtxGopherEvalTest3 -
#
# Verify if wtxGopherEval returns AGENT_GOPHER_TRUNCATED if the size of
# the list to return is greater than the maximum size.
#
# This test contains three parts:
# 1 - Gets elementNumber of elements in the linked list with < +8 *!> syntax.
# 2 - Gets elementNumber of elements in the linked list with < +8 @ > syntax.
# 3 - Gets the string of elements in the linked list with <+12 *$> syntax.
#
# Each element of the linked list is a structure and contains:
#
#	NODE node;
#	UNINT32 elementNumber;
#	char * string;
#
# NODE is a structure that contains a pointer to next node in the list
# and a pointer to previous node in the list.
#

proc wtxGopherEvalTest3 {Msg} {

    global agentErrMsg
    global USE_TSFORLOAD

    set listLength 3000 ;# list length for a maximun size equal to 14000

    puts stdout "$Msg" nonewline

    # verify if listTest.o exist in right directory
    set name [objPathGet test]listTest.o
    if { [file exists $name] == 0 } {
          testFail "File $name is missing for wtxGopherEvalTest3 test"
          return
       }

    # load listTest.o file
    if [catch {wtxObjModuleLoad $USE_TSFORLOAD $name} tModuleId] {
	testFail $tModuleId
	return
    }

    set tModuleId [lindex $tModuleId 0]

    funcCall makeList $listLength

    if [catch {wtxSymFind -name pGopherTestList} pGopherTestListInfo] {
	testFail $pGopherTestListAddr
	funcCall deleteList
	cleanUp o $tModuleId
	return
    }

    set pGopherTestListAddr [lindex $pGopherTestListInfo 1]

    if [catch {wtxGopherEval "$pGopherTestListAddr *!"} gopherTestListAddr] {
	testFail $gopherTestListAddr
	funcCall deleteList
	cleanUp o $tModuleId
	return
    }

    # Get the list with <+8 *!> syntax and verify the result
    set list [wtxGopherEval -errvar errCode "$gopherTestListAddr * { <+8 *!> *}"]

    if {$errCode == ""} {
	testFail $agentErrMsg(7)
	funcCall deleteList
	cleanUp o $tModuleId
	return
    }

    if {![wtxErrorMatch $errCode AGENT_GOPHER_TRUNCATED] } {
        testFail $errCode
	funcCall deleteList
	cleanUp o $tModuleId
        return
    }


    # Get the list with <+8 @> syntax and verify the result
    set list [wtxGopherEval -errvar errCode "$gopherTestListAddr * { <+8 @> * }"]

    if {$errCode == ""} {
	testFail $agentErrMsg(7)
	funcCall deleteList
	cleanUp o $tModuleId
	return
    }

    if {![wtxErrorMatch $errCode AGENT_GOPHER_TRUNCATED] } {
	testFail $errCode
	funcCall deleteList
	cleanUp o $tModuleId
	return
    }

    # Get the string list with <+12 *$> syntax and verify the result
    set list [wtxGopherEval -errvar errCode "$gopherTestListAddr * { <+12 *$> * }"]

    if {$errCode == ""} {
	testFail $agentErrMsg(7)
	funcCall deleteList
	cleanUp o $tModuleId
	return
    }

    if {![wtxErrorMatch $errCode AGENT_GOPHER_TRUNCATED] } {
	testFail $errCode
	funcCall deleteList
	cleanUp o $tModuleId
	return
    }

    funcCall deleteList
    cleanUp o $tModuleId

    wtxTestPass
}

#*****************************************************************************
#
# wtxGopherEvalTest4 -
#
# Verify if wtxGopherEval returns AGENT_GOPHER_TRUNCATED if the size of
# the list to return is greater than the maximum size in system mode.
#
# This test is similar to wtxGopherEvalTest3 except that the size of
# the linked list is smaller and the evaluation of the gopher programs
# are done is System Mode.
#

proc wtxGopherEvalTest4 {Msg} {

    global agentErrMsg
    global USE_TSFORLOAD

    set listLength 300;# list length for a maximun size equal to 1400

    puts stdout "$Msg" nonewline

    # verify if listTest.o exist in right directory
    set name [objPathGet test]listTest.o
    if { [file exists $name] == 0 } {
          testFail "File $name is missing for wtxGopherEvalTest4 test"
          return
       }

    # load listTest.o file
    if [catch {wtxObjModuleLoad $USE_TSFORLOAD $name} tModuleId] {
	testFail $tModuleId
	return
    }

    set tModuleId [lindex $tModuleId 0]

    funcCall makeList $listLength

    if [catch {wtxSymFind -name pGopherTestList} pGopherTestListInfo] {
	testFail $pGopherTestListAddr
	funcCall deleteList
	cleanUp o $tModuleId
	return
    }

    set pGopherTestListAddr [lindex $pGopherTestListInfo 1]

    if [catch {wtxGopherEval "$pGopherTestListAddr *!"} gopherTestListAddr] {
	testFail $gopherTestListAddr
	funcCall deleteList
	cleanUp o $tModuleId
	return
    }

    # switch to system mode to evaluate gopher program
    wtxAgentModeSet AGENT_MODE_EXTERN

    # Get the list with <+8 *!> syntax and verify the result
    set list [wtxGopherEval -errvar errCode "$gopherTestListAddr * { <+8 *!> * }"]

    wtxAgentModeSet AGENT_MODE_TASK

    if {$errCode == ""} {
	testFail $agentErrMsg(7)
	funcCall deleteList
	cleanUp o $tModuleId
	return
    }

    if {![wtxErrorMatch $errCode AGENT_GOPHER_TRUNCATED] } {
        testFail $errCode
	funcCall deleteList
	cleanUp o $tModuleId
        return
    }


    # switch to system mode to evaluate gopher program
    wtxAgentModeSet AGENT_MODE_EXTERN

    # Get the list with <+8 @> syntax and verify the result
    set list [wtxGopherEval -errvar errCode "$gopherTestListAddr * { <+8 @> * }"]

    wtxAgentModeSet AGENT_MODE_TASK


    if {$errCode == ""} {
	testFail $agentErrMsg(7)
	funcCall deleteList
	cleanUp o $tModuleId
	return
    }

    if {![wtxErrorMatch $errCode AGENT_GOPHER_TRUNCATED] } {
	testFail $errCode
	funcCall deleteList
	cleanUp o $tModuleId
	return
    }

    # switch to system mode to evaluate gopher program
    wtxAgentModeSet AGENT_MODE_EXTERN

    # Get the string list with <+12 *$> syntax and verify the result
    set list [wtxGopherEval -errvar errCode "$gopherTestListAddr * { <+12 *$> * }"]

    wtxAgentModeSet AGENT_MODE_TASK

    if {$errCode == ""} {
	testFail $agentErrMsg(7)
	funcCall deleteList
	cleanUp o $tModuleId
	return
    }

    if {![wtxErrorMatch $errCode AGENT_GOPHER_TRUNCATED] } {
	testFail $errCode
	funcCall deleteList
	cleanUp o $tModuleId
	return
    }

    funcCall deleteList
    cleanUp o $tModuleId
    wtxTestPass
}


#*****************************************************************************
#
# wtxGopherEvalTest5 -
#
#

proc wtxGopherEvalTest5 {Msg} {

    global errMiscMsg
    global USE_TSFORLOAD

    set listIntLength 2799	;# 2800*4 = 11200 (1400 < list length < 14000)
    set listStrLength 80	;# 80*170 = 13600 (1400 < list length < 14000)

    puts stdout "$Msg" nonewline

    # verify if listTest.o exist in right directory
    set name [objPathGet test]listTest.o
    if { [file exists $name] == 0 } {
          testFail "File $name is missing for wtxGopherEvalTest5 test"
          return
       }

    # load listTest.o file
    if [catch {wtxObjModuleLoad $USE_TSFORLOAD $name} tModuleId] {
	testFail $tModuleId
	return
    }

    set tModuleId [lindex $tModuleId 0]

    funcCall makeList $listIntLength

    if [catch {wtxSymFind -name pGopherTestList} pGopherTestListInfo] {
	testFail $pGopherTestListAddr
	funcCall deleteList
	cleanUp o $tModuleId
	return
    }

    set pGopherTestListAddr [lindex $pGopherTestListInfo 1]

    if [catch {wtxGopherEval "$pGopherTestListAddr *!"} gopherTestListAddr] {
	testFail $gopherTestListAddr
	funcCall deleteList
	cleanUp o $tModuleId
	return
    }

    # Get the list with <+8 *!> syntax and verify the result
    set list1 [wtxGopherEval -errvar errCode "$gopherTestListAddr * { <+8 *!> * }"]
    if {$errCode != ""} {
	testFail $errCode
	return
    }

    set llist1 [llength $list1]

    # Get the list with <+8 @> syntax and verify the result
    set list2 [wtxGopherEval -errvar errCode "$gopherTestListAddr * { <+8 @> * }"]
    if {$errCode != ""} {
	testFail $errCode
	return
    }

    set llist2 [llength $list2]

    if { $llist1 != 2800 || $llist2 != 2800 } {
	testFail $errMiscMsg(17)
	funcCall deleteList
	cleanUp o $tModuleId
	return
    }

    funcCall deleteList
    
    funcCall makeList $listStrLength

    if [catch {wtxSymFind -name pGopherTestList} pGopherTestListInfo] {
	testFail $pGopherTestListAddr
	funcCall deleteList
	cleanUp o $tModuleId
	return
    }

    set pGopherTestListAddr [lindex $pGopherTestListInfo 1]

    if [catch {wtxGopherEval "$pGopherTestListAddr *!"} gopherTestListAddr] {
	testFail $gopherTestListAddr
	funcCall deleteList
	cleanUp o $tModuleId
	return
    }
    
    # Get the string list with <+12 *$> syntax and verify the result
    set list3 [wtxGopherEval -errvar errCode "$gopherTestListAddr * { <+12 *$> * }"]

    if {$errCode != ""} {
	testFail $errCode
	return
    }

    set llist3 [llength $list3]

    if { $llist3 != 81 } {
	testFail $errMiscMsg(17)
	funcCall deleteList
	cleanUp o $tModuleId
	return
    }


    cleanUp o $tModuleId
    wtxTestPass
}

#*****************************************************************************
#
# wtxGopherEvalTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxGopherEvalTest {tgtsvrName timeout invalidAddr} {

    global gopherEvalMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxGopherEvalTest targetServerName\
			timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout

    # call the test procedures with valid parameters
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler
    wtxGopherEvalTest1 $gopherEvalMsg(1)

    # call the tests with invalid parameters
    wtxErrorHandler [lindex [wtxHandle] 0] ""
    # wtxGopherEvalTest2 $gopherEvalMsg(2)
    wtxGopherEvalTest3 $gopherEvalMsg(3)

    # valid test in external mode :
    # if serial mode is used then wtxGopherEvalTest4 is run
    # with a size greater to 1400 bytes else wtxGopherEvalTest5 
    # is run with a size greater to 14000 bytes.
    if {![catch "wtxAgentModeSet AGENT_MODE_EXTERN" agentSt]} {
	wtxAgentModeSet AGENT_MODE_TASK
	puts stdout "\nSwitch to External Mode :"
	wtxErrorHandler [lindex [wtxHandle] 0] ""
	wtxGopherEvalTest4 $gopherEvalMsg(3)

    } else {

	wtxGopherEvalTest5 $gopherEvalMsg(4) ;# This test must not be counted
    }

    # deconnection
    wtxToolDetachCall "wtxGopherEvalTest"
}
