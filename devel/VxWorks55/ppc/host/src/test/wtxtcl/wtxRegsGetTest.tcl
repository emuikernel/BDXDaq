# wtxRegsGetTest.tcl - Tcl script, test WTX_REGS_GET
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01o,27may98,p_b  WTX 2.0 new changes
# 01n,14may98,p_b  Replace test?.o by objSampleWtxtclTest?.o
# 01m,28jan98,p_b  Adapted for WTX 2.0
# 01l,08nov95,s_w  modify for SPR 5399 fix. OBJ_LOADED event no longer received
# 01k,20oct95,jmp  added invalidAddr argument to wtxRegsGetTest.
# 01j,13sep95,jmp  added wtxRegsGetTest2 (external mode test).
# 01i,21aug95,jmp  changed objGetPath to objPathGet.
#		   added timeout to wtxToolAttachCall in wtxRegsGetTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01h,23jun95,f_v  added wtxTestRun call
# 01g,19jun95,f_v  changed testTaskCreate call
# 01f,10may95,f_v  put PC part in comment to get compatible with different
#		   architecture
# 01e,20apr95,f_v  use generic procedures
# 01d,10apr95,f_v  avoid memory leak
# 01c,28mar95,f_v  added wtxRegisterForEvent
# 01b,17mar95,f_v  add lindex to get objModule Id
# 01a,07feb95,f_v  written. 
#
#
#
# DESCRIPTION
#
# WTX_REGS_GET  -  WTX get registers
#
#  Note : Just iu flag is tested for moment
#
#

#*****************************************************************************
#
# wtxRegsGetTest1 -
#
# Verify if wtxRegsGet gets register for this this test load objSampleWtxtclTest2.o,
# creates a context, and get the 8th registers of this context. Then
# the test check PC and nPC value.
#

proc wtxRegsGetTest1 {Msg} {

    global errRegsMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # initialisation of variables
    set moduleIsLoad 0


    # verify if objSampleWtxtclTest2.o exist in right directory
    set name [objPathGet]/objSampleWtxtclTest2.o 
    if { [file exists $name] == 0 } {
          testFail "File $name is missing for wtxRegsGetTest1 test"
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
        set tModuleId [wtxObjModuleLoad $USE_TSFORLOAD\
			[objPathGet]/objSampleWtxtclTest2.o]

	# get object module Id
	set tModuleId [lindex $tModuleId 0]
     }
 

    # create a new context
    set taskDesc [testTaskCreate $Msg objSampleWtxtclTest2.o test2]
    if {$taskDesc == 1} {
	return
    }

    # get entry point
    if {![set entry [testSymFind test2]]} {
	cleanUp t $taskDesc
	return
	}

    set taskId [lindex $taskDesc 0]

#    set nameBlk [wtxRegsGet task $taskId iu 0 4]
#
#    # In your case 8 - 4 pour PC 4 pour nPC
#    set memBlockGet [memBlockGet $nameBlk]
#
#    # create a name which use the wtxtcl pid
#    set name Test[pid]
#
#    # get a list of active task
#    set list [taskInfoGetAll]
#
#    # get a list of right parameters even if a task with same entry name exists
#    foreach i $list {
#      if { $i == $name } {
#	    set pos [lsearch $list $name]
#	    incr pos
#	    if { [llength $list] > $pos } {
#		set list [lrange $list $pos end]
#               } {
#		set list [lrange $list [expr $pos-10] end]
# 	       }	
#          }
#    }
#
#
#    # set PC [lindex $list [expr [lsearch $list $name]-6] ]
#    set PC [lindex $list 3]
#
#    set PC2 [format "%02x%02x%02x%02x" [lindex $reg 0] [lindex $reg 1]\
#		[lindex $reg 2] [lindex $reg 3]]
#
#    set PC [format %08x $PC] 
#
#    if {[string compare $PC $PC2] != 0 } {
#	    testFail $errRegsMsg(0)
#	    foreach block [memBlockList] {memBlockDelete $block}
#	    testTaskCleanup $taskDesc
#	    return
#	}
#
    # initialise 4 bytes to 0x21
    set blkId [memBlockCreate -B 4 0x21]

    # initialise 4 bytes to 0x51
    set blkId2 [memBlockCreate -B 4 0x51]

    # set the 4st bytes to 0x21
    wtxRegsSet task $taskId iu 0 4 $blkId
    # set 4 others bytes to 0x51
    wtxRegsSet task $taskId iu 4 4 $blkId2

    # get the 8th register of taskId context
    set nameBlk [wtxRegsGet task $taskId iu 0 8]
    set reg [memBlockGet $nameBlk]

    set list1 [lrange $reg 0 3]
    set list2 [lrange $reg 4 7]
    set err 0

    # scan list of <registers> and check it
    foreach i $list1 {
      if { $i != 0x21 } {
	    set err 1
	    break
	}
     }

    foreach i $list2 {
      if { $i != 0x51 } {
		set err 1
		break
	}
     }

    # clean up
    cleanUp mb $blkId $blkId2 t $taskDesc

    if { $err != 0 } { 
	 testFail $errRegsMsg(1)
	 return
     }

    wtxTestPass
}


#*****************************************************************************
#
# wtxRegsGetTest2 - Verify wtxRegsGet in external mode
#
# This test step the system context and verify that wtxRegsGet gives
# a different value than before the wtxContextStep.
# (The program counter must different after the step)
#

proc wtxRegsGetTest2 {Msg} {

    global errRegsMsg
    global offset

    puts stdout "$Msg" nonewline

    # Get the Program Counter Offset, it's depend on the architechture 
    set pcOffset [expr $offset(WIND_TCB,pc) - $offset(WIND_TCB,regs)]

    # get the 8th first bytes of IU register
    set resGet1 [wtxRegsGet CONTEXT_SYSTEM -1 REG_SET_IU $pcOffset 4]
    if {$resGet1 == ""} {
	return
	}

    # step the system context
    set resStep [wtxContextStep CONTEXT_SYSTEM -1]
    if {$resStep == ""} {
	cleanUp mb $resGet1
	return
	}

    # get the 8th first bytes of IU register
    set resGet2 [wtxRegsGet CONTEXT_SYSTEM -1 REG_SET_IU $pcOffset 4]
    if {$resGet2 == ""} {
	cleanUp mb $resGet1
	return
	}

    # continue the system context
    set resCont [wtxContextCont CONTEXT_SYSTEM -1]
    if {$resCont == ""} {
	cleanUp mb $resGet1 $resGet2
	return
	}

    set block1 [memBlockGet $resGet1]
    set block2 [memBlockGet $resGet2]

    # Verify that the blocks are differents
    if {$block1 == $block2} {
	testFail $errRegsMsg(0)
	cleanUp mb $resGet1 $resGet2
	return
	}

    wtxTestPass
}


#*****************************************************************************
#
# wtxRegsGetTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxRegsGetTest {tgtsvrName timeout invalidAddr} {

    global vRegsMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxRegsGetTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    # enable all events
    catch {wtxRegisterForEvent .*} st

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # call the test procedures
    wtxRegsGetTest1 $vRegsMsg(0)

    wtxErrorHandler [lindex [wtxHandle] 0] ""

    # if it's possible to change mode to external mode
    # wtxRegsGetTest2 must be run in external mode

    if {![catch "wtxAgentModeSet 2" agentSt]} {
        wtxErrorHandler [lindex [wtxHandle] 0] errorHandler
        puts stdout "\nSwitch to External Mode :"
        wtxRegsGetTest2 $vRegsMsg(0)
        wtxAgentModeSet 1
        }


    # deconnection
    wtxToolDetachCall "wtxRegsGetTest"
}
