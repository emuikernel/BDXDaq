# wtxRegsSetTest.tcl - Tcl script, test WTX_REGS_SET
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01r,27may98,p_b  WTX 2.0 new changes
# 01q,14may98,p_b  Replace test?.o by objSampleWtxtclTest?.o
# 01p,28jan98,p_b  Adapted for WTX 2.0
# 01o,11mar97,jmp  modified catch format when a filename is given in
#                  parameters to avoid failures on windows side (SPR #7868).
# 01n,08nov95,s_w  modify for SPR 5399 fix. OBJ_LOADED event no longer received
# 01m,20oct95,jmp  added invalidAddr argument to wtxRegsSetTest.
# 01l,13sep95,jmp  added wtxRegsSetTest3 (external mode test).
# 01k,21aug95,jmp  changed objGetPath to objPathGet.
#		   now uses cleanUp procedure.
#                  changed tests control method, now uses wtxErrorHandler.
# 01j,23jun95,f_v  added wtxTestRun call
# 01i,19jun95,f_v  changed testTaskCreate call
# 01h,23may95,f_v  in wtxRegsSetTest2 replace AGENT_INVALID_CONTEXT by
#		   AGENT_INVALID_PARAMS 
# 01g,27apr95,c_s  now uses wtxErrorMatch.
# 01f,20apr95,f_v  use generic procedures
# 01e,10apr95,f_v  avoid memory leak
# 01d,30mar95,f_v  replace S_wdb_xx by agentErr
# 01c,28mar95,f_v  added wtxRegisterForEvent
# 01g,17mar95,f_v  add lindex to get objModule Id
# 01b,16feb95,f_v  add wtxRegsSetTest2,
#		   call objGetPath with test like parameter
#		   use waitEvent procedure
# 01a,08feb95,f_v  written. 
#
#
#
# DESCRIPTION
#
# WTX_REGS_SET  -  WTX set registers
#
#
#

#*****************************************************************************
#
# wtxRegsSetTest1 -
#
# Verify if wtxRegsSet set register
#

proc wtxRegsSetTest1 {Msg} {

    global errRegsMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # initialisation of variables
    set isTask 1
    set moduleIsLoad 0

    # verify if objSampleWtxtclTest2.o exist in right directory
    set nameFile [objPathGet]/objSampleWtxtclTest2.o 
    if { [file exists $nameFile] == 0 } {
          testFail "File $nameFile is missing for wtxRegsSetTest1 test"
          return
       }

    # Get the number of loaded module
    set NbModuleLoad [llength [wtxObjModuleList]]

    set tIdList [wtxObjModuleList]
        
    # this loop scan modules already loaded to find objSampleWtxtclTest2.o
    while { $NbModuleLoad > 0 } {
                set tId [ lindex $tIdList [expr $NbModuleLoad-1]]
                set ModuleInfo [wtxObjModuleInfoGet $tId]
                set ModuleName [lindex $ModuleInfo 1]

                if { "$ModuleName" == {objSampleWtxtclTest2.o} } {
                        set moduleIsLoad 1
                        break
                    }

                incr NbModuleLoad -1
          }

    
    # load objSampleWtxtclTest2.o file if this module isn't already loaded 

    if { $moduleIsLoad == 0 } {
        set tModuleId [wtxObjModuleLoad $USE_TSFORLOAD [objPathGet]/objSampleWtxtclTest2.o]
	if {$tModuleId == ""} {
		return
		}

	# get object module Id
	set tModuleId [lindex $tModuleId 0]
        
     }
 

    # create a new context
    set taskDesc [testTaskCreate $Msg objSampleWtxtclTest2.o test2]
    if {$taskDesc == 1} {
	return
    }

    set taskId [lindex $taskDesc 0]

    # initialise 4 bytes to 0x21
    set blkId [memBlockCreate -B 4 0x21]

    # initialise 4 bytes to 0x51
    set blkId2 [memBlockCreate -B 4 0x51]

    # set the 4th bytes to 0x21
    set status [wtxRegsSet task $taskId iu 0 4 $blkId]
    if {$status == ""} {
	cleanUp mb $blkId $blkId2 t $taskDesc
	return
	}

    # set 4 others bytes to 0x51
    set status [wtxRegsSet task $taskId iu 4 4 $blkId2]
    if {$status == ""} {
	cleanUp mb $blkId $blkId2 t $taskDesc
	return
	}

    # get the 8th bytes of test2 task
    set nameBlk [wtxRegsGet task $taskId iu 0 8]
    set reg [memBlockGet $nameBlk]

    # arrange registers in list
    set list1 [lrange $reg 0 3]
    set list2 [lrange $reg 4 7]
    set err 0

    # scan list and check it
    foreach i $list1 {
      if { $i != 0x21 } {
	    set err 1
	    break
	}
     }

    # scan list and check it
    foreach i $list2 {
      if { $i != 0x51 } {
		set err 1
		break
	}
     }

    # clean up
    cleanUp mb $blkId $blkId2 t $taskDesc

    if { $err != 0 } { 
	 testFail $errRegsMsg(2)
	 return
     }

    wtxTestPass
}


#*****************************************************************************
#
# wtxRegsSetTest2 -
#
# Verify if wtxRegsSet manages bad context Id
#

proc wtxRegsSetTest2 {Msg} {

    global errRegsMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # initialisation of variables
    set isTask 1
    set moduleIsLoad 0

    # verify if objSampleWtxtclTest2.o exist in right directory
    set nameFile [objPathGet test]objSampleWtxtclTest2.o 
    if { [file exists $nameFile] == 0 } {
          testFail "File $nameFile is missing for wtxRegsSetTest2 test"
          return
       }


    catch "wtxObjModuleList" tIdList    

    # Get the number of loaded module
    set NbModuleLoad [llength $tIdList]

    # this loop scan modules already loaded to find objSampleWtxtclTest2.o
    while { $NbModuleLoad > 0 } {
                set tId [ lindex $tIdList [expr $NbModuleLoad-1]]
                catch " wtxObjModuleInfoGet $tId" ModuleInfo
                set ModuleName [lindex $ModuleInfo 1]

                if { "$ModuleName" == {objSampleWtxtclTest2.o} } {
                        set moduleIsLoad 1
                        break
                    }

                incr NbModuleLoad -1
          }

    
    # load objSampleWtxtclTest2.o file if this module isn't already loaded 

    if { $moduleIsLoad == 0 } {
        if {[catch {wtxObjModuleLoad $USE_TSFORLOAD\
			[objPathGet test]objSampleWtxtclTest2.o} tModuleId]} {
		testFail $tModuleId
		return
		}

        
	# get object module Id
	set tModuleId [lindex $tModuleId 0]

     }
 

    # create a new context
    set taskDesc [testTaskCreate $Msg objSampleWtxtclTest2.o test2]
    if {$taskDesc == 1} {
	return
    }

    set taskId [lindex $taskDesc 0]

    # corrumpt taskId address
    set taskId2 [expr $taskId-4]

    # initialise 8 bytes to 0x30
    set blkId [memBlockCreate -B 8 0x30]

    # if no exception occurred
    if {![catch {wtxRegsSet task $taskId2 iu 0 8 $blkId} nameBlk]} {
	testFail $errRegsMsg(2)
	cleanUp ml t $taskDesc
	return
	}

    # clean up
    cleanUp ml t $taskDesc

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $nameBlk AGENT_INVALID_PARAMS] } { 
         testFail $nameBlk
         return
     }

    # if the caught exception is the expected error code
    wtxTestPass
}


#*****************************************************************************
#
# wtxRegsSetTest3 - Verify wtxRegsSet in external mode
#
# This test steps the system context, sets bytes of IU register and verify
# if the set bytes have correct values.
#

proc wtxRegsSetTest3 {Msg} {

    global errRegsMsg

    puts stdout "$Msg" nonewline

    # Create a memory block
    set testBlock [memBlockCreate -B 4 0xff]

    # step the system context
    set resStep [wtxContextStep CONTEXT_SYSTEM -1]
    if {$resStep == ""} {
        cleanUp mb $resGet1
        return
        }

    # get the 8th first bytes of IU register to restore
    # at the end of the test
    set saveGet [wtxRegsGet CONTEXT_SYSTEM -1 REG_SET_IU 4 4]
    if {$saveGet == ""} {
        return
        }

    # set the bytes 3 to 7 of IU register with $testBlock
    set resSet [wtxRegsSet CONTEXT_SYSTEM -1 REG_SET_IU 4 4 $testBlock]
    if {$resSet == ""} {
	cleanUp mb $testBlock $saveGet
        return
        }

    # get the 8th first bytes of IU register
    set resGet [wtxRegsGet CONTEXT_SYSTEM -1 REG_SET_IU 4 4]
    if {$resGet == ""} {
	cleanUp mb $testBlock $saveGet
        return
        }

    # verify that wtxRegsSet has set correctly IU register
    if {[memBlockGet $testBlock] != [memBlockGet $resGet]} {
	testFail $errRegsMsg(3)
	cleanUp mb $testBlock $resGet $saveGet
	return
	}

    # restore the previous value
    set resSet [wtxRegsSet CONTEXT_SYSTEM -1 REG_SET_IU 4 4 $saveGet]
    if {$resSet == ""} {
	cleanUp mb $testBlock $resGet $saveGet
	return
	}

    # continue the system context
    set resCont [wtxContextCont CONTEXT_SYSTEM -1]
    if {$resCont == ""} {
        cleanUp mb $resGet1 $resGet2
        return
        }

    wtxTestPass
}


#*****************************************************************************
#
# wtxRegsSetTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxRegsSetTest {tgtsvrName timeout invalidAddr} {

    global vRegsMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxRegsSetTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # call the valid tests
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    # enable all events
    catch {wtxRegisterForEvent .*} st

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    wtxRegsSetTest1 $vRegsMsg(1)

    wtxErrorHandler [lindex [wtxHandle] 0] ""


    # call the invalid tests
    wtxRegsSetTest2 $vRegsMsg(2)

    # if it's possible to change mode to external mode
    # wtxRegsSetTest3 must be run in external mode

    if {![catch "wtxAgentModeSet 2" agentSt]} {
        wtxErrorHandler [lindex [wtxHandle] 0] errorHandler
        puts stdout "\nSwitch to External Mode :"
        wtxRegsSetTest3 $vRegsMsg(1)
        wtxAgentModeSet 1
        }

    # deconnection
    wtxToolDetachCall "wtxRegsSetTest"
}
