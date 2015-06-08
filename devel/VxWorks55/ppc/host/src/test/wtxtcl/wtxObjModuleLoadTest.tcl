# wtxObjModuleLoadTest.tcl - Tcl script, test WTX_OBJ_MODULE_LOAD
#
# Copyright 1994-1996 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 02e,05apr02,jmp  changed search symbol for INCLUDE_CPLUS to cplusLibInit.
# 02d,24jun98,p_b  Added Cplus test. 
# 02c,04jun98,pcn  Added wtxObjModuleLoadTest10 (invalid address) in test
#                  suite.
# 02b,14may98,p_b  Replace test?.o by objSampleWtxtclTest?.o
# 02a,28jan98,p_b  Adapted for WTX 2.0
# 01z,11mar97,jmp  modified catch format when a filename is given in
#                  parameters to avoid failures on windows side (SPR #7868).
# 01y,25jul96,jmp  added wtxObjModuleLoadTest18, wtxObjModuleLoadTest19,
#		   wtxObjModuleLoadTest20 to test LOAD_COMMON_MATCH_* flags.
# 01x,13jun96,pad  re-instated wtxObjModuleLoadTest12 since SPR #6397 is fixed.
# 01w,15apr96,jmp  used objSampleWtxtclTest6.o instead of objSampleWtxtclTest1.o to fixe SPR #6342,
#		   improved wtxObjModuleLoadTest6 (C++ test),
#		   added wtxObjModuleLoadTest12, wtxObjModuleLoadTest13,
#		   wtxObjModuleLoadTest14, wtxObjModuleLoadTest15,
#		   wtxObjModuleLoadTest16, wtxObjModuleLoadTest17.
#		   wtxObjModuleLoadTest12 is not activate since SPR# 6397
#		   is not fixed.
# 01v,21feb96,jmp  reconnected wtxObjModuleLoadTest6 (C++ test).
# 01u,12jan96,p_m  suppressed test against SVR_EINVAL error now that SPR# 5424
#		   is fixed.
# 01t,10nov95,jmp  made temporary modifications in wtxObjModuleLoadTest10
#		   to cover SPR #5424.
# 01s,08nov95,s_w  modify for SPR 5399 fix. OBJ_LOADED event no longer received
# 01r,02nov95,jmp  temporarily removed wtxObjModuleLoadTest6, C++
#		   is not avaible on i960 architecture for the moment.
# 01p,20oct95,jmp  added wtxObjModuleLoadTest6, wtxObjModuleLoadTest7,
#		   wtxObjModuleLoadTest8, wtxObjModuleLoadTest9,
#		   wtxObjModuleLoadTest10.
#		   added invalidAddr argument to wtxObjModuleLoadTest.
# 01o,18sep95,jmp  use funcCall instead of wtxFuncCall.
# 01n,17aug95,jmp  changed objGetPath to objPathGet.
#		   added timeout to wtxToolAttachCall in wtxObjModuleLoadTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01m,24jul95,p_m  temporarily removed test no 5 since .ref files are not
#                  available for some architectures for the moment. 
# 01n,24jun95,c_s  fixed exec of loopback target server.  Now uses cmp instead
#                    of diff for reference file checking; prints output of 
#                    failed comparisons.
# 01m,23jun95,f_v  added wtxTestRun call
# 01l,08jun95,f_v  protected tgtsvr in loopback mode
# 01k,24may95,f_v  update attach and kill management, added timeout
# 01j,22may95,jcf  name revision.
# 01i,12may95,f_v  check wtxObjModuleLoad in wtxObjModuleLoadTest5
# 01h,28apr95,c_s  now uses wtxErrorMatch.
# 01g,12apr95,f_v  added wtxObjModuleLoadTest5
# 01f,28mar95,f_v  added wtxRegisterForEvent
# 		   changed pomme to test1
# 01e,17mar95,f_v  add lindex to get objModule Id
# 01d,15feb95,f_v  Add wtxObjModuleLoadTest4
#		   call objGetPath with test like parameter
#		   replace wtxEventGet by wtxEventPoll through waitEvent
# 01c,02feb95,f_v  Add wtxObjModuleLoadTest2,wtxObjModuleLoadTest3
#		   add event managment, put ObjMsg in comments
#		   add file exists test
#                  use milliseconds rather than seconds
# 01b,27jan95,f_v  change access messages, use wtxToolDetachCall
#                  error messages are now in wtxTestErr.tcl
#		   others messages are now in wtxTestMsg.tcl
# 01a,17jan95,f_v  written. 
#
#

#
# DESCRIPTION
#
# WTX_OBJ_MODULE_LOAD  -  Load an object file
#
#
#

#*****************************************************************************
#
# wtxObjModuleLoadTest0 - Local load of a file
#
#
 
proc wtxObjModuleLoadTest0 {ts tgtsvrName Msg} {
 
    global errObjMsg
    global env
 
    puts stdout "$Msg" nonewline
 
    set objModuleIdList [wtxObjModuleList]
    if { $env(WIND_HOST_TYPE) == "x86-win32" } {
       set nameTmpFile $env(TMP)\\objSampleWtxtclTest6.o
        } else {
       set nameTmpFile /tmp/objSampleWtxtclTest6.o
     }
 
    foreach id $objModuleIdList {
        set moduleInfo [wtxObjModuleInfoGet $id]
        if {[lindex $moduleInfo 1] == "objSampleWtxtclTest6.o"} {
            cleanUp o $id
        }
    }
 
    set tIdList [wtxObjModuleList]
    if {$tIdList == ""} {
        return 0
    }
 
    set NbModuleLoad [llength $tIdList]
 
    # verify if objSampleWtxtclTest6.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest6.o
    if { [file exists $name] == 0 } {
        testFail "File $name is missing for wtxObjModuleLoadTest1 test"
        return 0
    }

    set theSourceFile $name
    file copy -force $theSourceFile $nameTmpFile
 
    # load local objSampleWtxtclTest6.o file
    if [catch {wtxObjModuleLoad $ts LOAD_GLOBAL_SYMBOLS $nameTmpFile}] {
     # The catch failed :
     # Get Tool Host name
     set toolHost [lindex [lindex [wtxTsInfoGet] 15] 4]
     set toolHost [lindex [split $toolHost @] 1]
 
     # Get Target server host name
     set tgtsvrHost [lindex [split $tgtsvrName @] 1]
 
     # If (different hosts and "-ts") then OK, else ERROR
     if { ([string compare $ts "-ts"]) &&\
          (![string compare $tgtsvrHost $toolHost]) } {
        testFail $errObjMsg(6)
     } else {
        wtxTestPass
     }
     return
    }
 
    set NbModuleLoad2 [llength [wtxObjModuleList]]
    set Check [expr ($NbModuleLoad + 1)]
 
    if { $NbModuleLoad2 != $Check } {
        testFail $errObjMsg(6)
        return 0
    }
 
    # Verify that there is no local symbols loaded
 
    wtxErrorHandler [lindex [wtxHandle] 0] ""
 
    if ![catch {wtxSymFind -name staticVoidFunc}] {
        testFail $errObjMsg(21)
        return 0
    }
 
    if ![catch {wtxSymFind -name staticIntVarInit}] {
        testFail $errObjMsg(21)
        return 0
    }
 
    if ![catch {wtxSymFind -name staticGlobalVariable}] {
        testFail $errObjMsg(21)
        return 0
    }
 
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler
 
    # clean up
    foreach id $objModuleIdList {
        set moduleInfo [wtxObjModuleInfoGet $id]
        if {[lindex $moduleInfo 1] == "objSampleWtxtclTest6.o"} {
            cleanUp o $id
        }
    }

    file delete $nameTmpFile
    wtxTestPass

}

#*****************************************************************************
#
# wtxObjModuleLoadTest1 -
#
# Verify if wtxObjModuleLoad load correctly objSampleWtxtclTest6.o file with 
# the flag LOAD_GLOBAL_SYMBOLS.
#

proc wtxObjModuleLoadTest1 {ts Msg} {

    global errObjMsg

    puts stdout "$Msg" nonewline

    set objModuleIdList [wtxObjModuleList]
    
    foreach id $objModuleIdList {
	set moduleInfo [wtxObjModuleInfoGet $id]
	if {[lindex $moduleInfo 1] == "objSampleWtxtclTest6.o"} {
	    cleanUp o $id
	}
    }

    set tIdList [wtxObjModuleList]
    if {$tIdList == ""} {
        return 0
    }

    set NbModuleLoad [llength $tIdList]
    
    # verify if objSampleWtxtclTest6.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest6.o 
    if { [file exists $name] == 0 } {
	testFail "File $name is missing for wtxObjModuleLoadTest1 test"
	return 0
    }

    # load objSampleWtxtclTest6.o file and give a idea of loading time
    set timeMsr [time {set tModuleId [wtxObjModuleLoad $ts LOAD_GLOBAL_SYMBOLS\
		[objPathGet test]objSampleWtxtclTest6.o]} 1] 
    if {$tModuleId == ""} {
	return 0
    }

    # get object module Id
    set tModuleId [lindex $tModuleId 0]

    # format time result
    set timeMsr [lindex $timeMsr 0]

    # make a conversion from micro seconds to milli seconds

    set timeMsrSec [ expr $timeMsr/1000]

    set NbModuleLoad2 [llength [wtxObjModuleList]]
    set Check [expr ($NbModuleLoad + 1)]

    if { $NbModuleLoad2 != $Check } {
	testFail $errObjMsg(6)
	return 0
    }


    # Verify that there is no local symbols loaded

    wtxErrorHandler [lindex [wtxHandle] 0] ""

    if ![catch {wtxSymFind -name staticVoidFunc}] {
	testFail $errObjMsg(21)
	return 0
    }

    if ![catch {wtxSymFind -name staticIntVarInit}] {
	testFail $errObjMsg(21)
	return 0
    }

    if ![catch {wtxSymFind -name staticGlobalVariable}] {
	testFail $errObjMsg(21)
	return 0
    }

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # clean up
    cleanUp o $tModuleId

    wtxTestPass
    return $timeMsrSec
}



#*****************************************************************************
#
# wtxObjModuleLoadTest2 -
#
# Verify if wtxObjModuleLoad load correctly objSampleWtxtclTest2.o file and 
# get back 10 from the task test2
#

proc wtxObjModuleLoadTest2 {ts Msg} {

    global errObjMsg

    puts stdout "$Msg" nonewline

    set moduleIsLoad 0

    # verify if objSampleWtxtclTest2.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest2.o 
    if { [file exists $name] == 0 } {
	testFail "File $name is missing for wtxObjModuleLoadTest2 test"
	return
	}

    # Get the number of loaded module
    set NbModuleLoad [llength [wtxObjModuleList]]

    catch "wtxObjModuleList" tIdList	
	
    while { $NbModuleLoad > 0 } {
		set tId [lindex $tIdList [expr $NbModuleLoad-1]]
		catch " wtxObjModuleInfoGet $tId" ModuleInfo
		set ModuleName [lindex $ModuleInfo 1]

		if { "$ModuleName" == {objSampleWtxtclTest2.o} } {
                        set moduleIsLoad 1
                        break
	       	    }

		incr NbModuleLoad -1
	  }

    set NbModuleLoad [llength [wtxObjModuleList]]

    # load objSampleWtxtclTest2.o file if this module isn't already loaded 
    if { $moduleIsLoad == 0 } {
	if {[catch {wtxObjModuleLoad $ts LOAD_GLOBAL_SYMBOLS\
		[objPathGet test]objSampleWtxtclTest2.o} tModuleId]} {
		return
		}
	    
        # get object module Id
        set tModuleId [lindex $tModuleId 0]

     } else {
	# readjust NbModuleLoad if objSampleWtxtclTest2.o is already loaded
        set NbModuleLoad [expr $NbModuleLoad-1]
     }

    set NbModuleLoad2 [llength [wtxObjModuleList]]
    set Check [expr ($NbModuleLoad + 1)]

    # test if objSampleWtxtclTest2.o is correctly loaded
    if { $NbModuleLoad2 == $Check } {

	    # get the task address
	    set sym test2
	    set symInfo [wtxSymFind -name "$sym"]
	    if {$symInfo == ""} {
		return
	    }

	    set symAdrs [lindex $symInfo 1]

	    # Call the test2 task
	    wtxFuncCall $symAdrs

	    # Loop until call-return event arrive
	    set info ""
	    while { $info != "CALL_RETURN" } {
		 catch "wtxEventGet" infoi
		 set info [lindex $infoi 0]
	     }

	    # task test2 should return 10 
	    if {[lindex $infoi 2] != 10 } {
		testFail $errObjMsg(7)    
		return
	     }


     } else {
	  testFail $errObjMsg(8)   
	  return
     }

    # unload module only if it wasn't loaded before
    if { $moduleIsLoad == 0} {
	cleanUp o $tModuleId
	}

    wtxTestPass
 }

#*****************************************************************************
#
# wtxObjModuleLoadTest3 -
#
# Verify if wtxObjModuleLoad load correctly objSampleWtxtclTest3.o,
# objSampleWtxtclTest4.o files and if test4 can call test3 with two integers 
# like parameters. test3 should return 0xf e.g the difference between the 
# two parameters
#

proc wtxObjModuleLoadTest3 {ts Msg} {

    global errObjMsg

    puts stdout "$Msg" nonewline

    # verify if objSampleWtxtclTest3.o and objSampleWtxtclTest4.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest3.o 
    if {[file exists $name] == 0 } {
	testFail "File $name is missing for wtxObjModuleLoadTest3 test"
	return
	}

    set name [objPathGet test]objSampleWtxtclTest4.o 
    if {[file exists $name] == 0 } {
	testFail "File $name is missing for wtxObjModuleLoadTest3 test"
	return
	}

    # load objSampleWtxtclTest3.o file  
    set tModuleId3 [wtxObjModuleLoad $ts [objPathGet test]objSampleWtxtclTest3.o]
    if {$tModuleId3 == ""} {
	return
	}

    # get object module Id
    set tModuleId3 [lindex $tModuleId3 0]

    # load objSampleWtxtclTest4.o file  
    set tModuleId4 [wtxObjModuleLoad $ts [objPathGet test]objSampleWtxtclTest4.o]
    if {$tModuleId4 == ""} {
	return
	}

    # get object module Id
    set tModuleId4 [lindex $tModuleId4 0]
    
    set resTest4 [funcCall test4]

    # Call the test4 task
    # test4 sould return 0xf
    if {$resTest4 != 0xf } {
	testFail $errObjMsg(7)    
	return
	}


     # clean up
     cleanUp o $tModuleId3 $tModuleId4

     wtxTestPass
}

#*****************************************************************************
#
# wtxObjModuleLoadTest4 -
#
# Verify if wtxObjModuleLoad manage correctly a bad file name like parameter
#

proc wtxObjModuleLoadTest4 {ts Msg} {

    global errObjMsg

    puts stdout "$Msg" nonewline

    # Get the number of loaded module
    set NbModuleLoad [llength [wtxObjModuleList]]

    # load zzxxQ.o file - zzxxQ.o file mustn't exist 

    # if no exception occurred
    if {![catch {wtxObjModuleLoad $ts zzxxQ.o} tModuleId]} {
	testFail $errObjMsg(2)
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $tModuleId *API_FILE_NOT_ACCESSIBLE*]} {
	testFail $tModuleId
	return
	}

    set NbModuleLoad2 [llength [wtxObjModuleList]]

    if {$NbModuleLoad != $NbModuleLoad2} {
          testFail $errObjMsg(2)
          return
     }

    wtxTestPass
}


#*****************************************************************************
#
# wtxObjModuleLoadTest5 -
#
# Verify if wtxObjModuleLoad manage correctly .rms files
#

proc wtxObjModuleLoadTest5 {Msg {args 0} } { 

    global env
    global errObjMsg
    global wtxCmdTimeout

    puts stdout "$Msg" nonewline

    set flag 0
    set type test
    set tgtSvrName2 testLoop[pid]
    set wtxCmdTimeout(wtxObjModuleLoad) 100

    # set different path -
    # doesn't use objPathGet because loopBack will be used
    set tgtpath $env(WIND_BASE)/host/src/test/wtxtcl

    set nameFile $tgtpath/memAdrs.tcl

    if {[file exists $nameFile] != 1 } {
	testFail "ERROR : $nameFile doesn't exists"
	return
        }


    # read nameFile script
    source $nameFile

    # get cpu tool,adress of text,data,bss,and cpu number 
    # adress are in adrList array
    set info [memAdrs adrList]
    
    set cpu   [lindex $info 0]
    set tool  [lindex $info 1]
    set cpuNb [lindex $info 2]

    set refpath $env(WIND_BASE)/target/lib/obj$cpu$tool$type
    set fileName $refpath/vxWorks

    if {[file exists $fileName] != 1 } {
	testFail "ERROR : $fileName\ndoesn't exists"
	return
	}

    puts stdout "Test in progress ..." 

    # launch tgtsvr in loopBack mode by a subprocess
    exec tgtsvr $tgtSvrName2 -c $fileName -B loopback -cpu $cpuNb 2> /dev/null &

    # made a tempo 
    after 2000

    # attach tool to loopBack target server
    if {[catch "wtxToolAttach $tgtSvrName2 wtxtestLoopBack" attSt]} {
	testFail "ERROR : API server not found\n"
	return
	}

    # list of object modules - new module should be added at this list
    if { $args == 0 || $args == {}} {
	   set list [glob $refpath/*.o]
      } else { 
	   set list $args
      }

    # get file name without extension from list
    for { set i 0 } { $i < [llength $list] } { incr i } {
	set list [lreplace $list $i $i [file root [file tail\
			[lindex $list $i]]]]
       }

    # Load a module which generate a .rms file in current directory
    # and compare it to reference

    foreach i $list {

	# check if file exists in right directory
	if {[file exists $refpath/$i.o] == 1 } {

	    # load object module
	    catch "wtxObjModuleLoad $ts file $adrList(text)\
	    $adrList(data) $adrList(bss) $refpath/$i.o" tId

	    if {[wtxErrorMatch $tId "Permission denied"]} {
		testFail "ERROR : In current directory write permission\
		was denied"
		# kill target server in loopBack mode
		wtxTsKill DESTROY
		after 2000
		wtxToolDetach
		return
	    }
	    if {[wtxErrorMatch $tId EXCHANGE_TRANSPORT_DISCONNECT]} {
		testFail "ERROR : Target server disconnect"
		# detach target server in loopBack mode
		wtxToolDetach
		return
	    }

	    # make difference between new file and reference file
	    catch {exec cmp $i.rms $refpath/$i.ref} cmp

	    if {$cmp != ""} {
	        puts stdout $cmp
	        set flag 1
	    }

	    # clean up
	    if {[file exists $i.rms]} {file delete $i.rms}
	    set tId [lindex $tId 0]
	    cleanUp $tId
	}
     }

     # kill and detach target server in loopBack mode
     wtxTsKill DESTROY
     after 2000
     wtxToolDetach

     if { $flag == 1 } {
          testFail $errObjMsg(12)
          return
     }

    wtxTestPass
}


#*****************************************************************************
#
# wtxObjModuleLoadTest6 - Verify that wtxObjModuleLoad manages correctly
#			  C++ object modules.
#
# First, this test loads a C++ object module in MANUAL strategy and
# verifies that constructor hasn't been invoked.
#
# Then, the test reload the C++ object module in AUTOMATIC strategy and
# verifies that constructor has been invoked.
#
# After this the test execute a single C++ application.
#
# REMARKS :
# - Under the MANUAL strategy, a module's static constructors and
# destructors are called by cplusCtors() and cplusDtors(), which are
# themselves invoked manually.
#
# - Under the AUTOMATIC strategy, a module's static constructors are
# called as a side-effect of loading the module using the target server
# loader.  A module's static destructors are called as a side-effect of
# unloading the module.
#

proc wtxObjModuleLoadTest6 {ts Msg} { 
    
    global errObjMsg

    puts stdout "$Msg" nonewline

    # verify if cplus.o exist in right directory
    set name [objPathGet test]cplus.o
    if {[file exists $name] == 0 } {
        testFail "File $name is missing for wtxObjModuleLoadTest6 test"
        return
        }

    wtxErrorHandler [lindex [wtxHandle] 0] ""

    # Get the C++ constructor calling strategy
    # The strategy will be restored at the end of the test
    if {[catch {wtxSymFind -name cplusXtorStrategy} symInfo]} {
	puts stdout "\nWARNING : $symInfo\nWhile executing: \
		     wtxSymFind -name cplusXtorStrategy\t\t" nonewline

	wtxTestPass

	return
	}

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    set addrStrategy [lindex $symInfo 1]

    set previousStrategy [wtxGopherEval -errvar errCode "$addrStrategy *!"]
    if {$previousStrategy == ""} {
	return
	}



#####################
# MANUAL strategy
#####################

    # If strategy is set to AUTOMATIC, set it to MANUAL
    if {$previousStrategy != 0} {
	funcCall cplusXtorSet 0

	# Verify that strategy has been correctly set
	set strategy [wtxGopherEval -errvar errCode "$addrStrategy *!"]
	if {$strategy == ""} {
	    return
	    }

	if {$strategy != 0} {
	    testFail $errObjMsg(11)
	    return
	    }
	}

    # load cplus.o file
    set tModuleId [wtxObjModuleLoad $ts LOAD_GLOBAL_SYMBOLS\
	 [objPathGet test]cplus.o]
    if {$tModuleId == ""} {
        return
        }

    set tModuleId [lindex $tModuleId 0]

    # Get the real name of cplusTest function
    set cplusRealName [lindex [lindex [wtxSymListGet -name cplusTest] 0] 0]

    # Call cplusTest function:
    # cplusTest affect the returned value of the "what" function of
    # classA and classB to resTestClassA and resTestClassB

    funcCall $cplusRealName


    # Get resTestClassA address
    set resTestClassAInfo [wtxSymFind -name resTestClassA]
    if {$resTestClassAInfo == ""} {
	cleanUp o $tModuleId
	return
    }

    set addrA [lindex $resTestClassAInfo 1]

    # Get resTestClassB address
    set resTestClassBInfo [wtxSymFind -name resTestClassB]
    if {$resTestClassBInfo == ""} {
	cleanUp o $tModuleId
	return
	}

    set addrB [lindex $resTestClassBInfo 1]

    # Read resTestClassA value
    set resA [wtxGopherEval -errvar errCode "$addrA *!"]
    if {$resA == ""} {
	cleanUp o $tModuleId
	return
	}

    # Read resTestClassB value
    set resB [wtxGopherEval -errvar errCode "$addrB *!"]
    if {$resB == ""} {
	cleanUp o $tModuleId
	return
	}

    # Verify that the constructor hasn't been called
    if {$resA != 0 || $resB != 0} {
	testFail $errObjMsg(12)
	cleanUp o $tModuleId
	return
	}

    # Unload cplus.o module
    cleanUp o $tModuleId



#####################
# AUTOMATIC strategy
#####################

    # set strategy to AUTOMATIC
    funcCall cplusXtorSet 1

    # Verify that strategy has been correctly set
    set strategy [wtxGopherEval -errvar errCode "$addrStrategy *!"]
    if {$strategy == 0} {
	testFail $errObjMsg(11)
	return
	}

    # load cplus.o file
    set tModuleId [wtxObjModuleLoad $ts LOAD_GLOBAL_SYMBOLS\
	 [objPathGet test]cplus.o]
    if {$tModuleId == ""} {
        return
        }

    set tModuleId [lindex $tModuleId 0]

    # Call cplusTest function
    funcCall $cplusRealName


    # Get resTestClassA address
    set resTestClassAInfo [wtxSymFind -name resTestClassA]
    if {$resTestClassAInfo == ""} {
	cleanUp o $tModuleId
	return
	}

    set addrA [lindex $resTestClassAInfo 1]

    # Get resTestClassB address
    set resTestClassBInfo [wtxSymFind -name resTestClassB]
    if {$resTestClassBInfo == ""} {
	cleanUp o $tModuleId
	return
	}

    set addrB [lindex $resTestClassBInfo 1]

    # Read resTestClassA value
    set resA [wtxGopherEval -errvar errCode "$addrA *!"]
    if {$resA == ""} {
	cleanUp o $tModuleId
	return
	}

    # Read resTestClassB value
    set resB [wtxGopherEval -errvar errCode "$addrB *!"]
    if {$resB == ""} {
	cleanUp o $tModuleId
	return
	}

    # Verify that the constructor has been called
    if {$resA != 98765 || $resB != 43210} {
	testFail $errObjMsg(12)
	cleanUp o $tModuleId
	return
	}


    #
    # Run the C++ application and verify results
    #

    # Get the real name of family routine
    set familyRealName [lindex [lindex [wtxSymListGet -name family] 0] 0]

    set addr [funcCall $familyRealName]

    set resBlock [wtxMemRead $addr 50]
    if {$resBlock == ""} {
	cleanUp o $tModuleId
	return
    }

    funcCall free $addr

    set resulStr [memBlockGetString $resBlock]

    if {![string match "PadColinEricJmichelJclaude*" $resulStr]} {
	testFail $errObjMsg(20)
	cleanUp o $tModuleId mb $resBlock
	return
    }


    # Unload cplus.o module
    cleanUp o $tModuleId mb $resBlock

    # restore previous strategy
    funcCall cplusXtorSet $previousStrategy

    wtxTestPass
}


#*****************************************************************************
#
# wtxObjModuleLoadTest7 -
#
# Verify if wtxObjModuleLoad manage correctly empty object module
#

proc wtxObjModuleLoadTest7 {ts Msg} {

    global errObjMsg

    puts stdout "$Msg" nonewline

    set objModuleIdList [wtxObjModuleList]
    
    foreach id $objModuleIdList {
	set moduleInfo [wtxObjModuleInfoGet $id]
	if {[lindex $moduleInfo 1] == "nullSizeModule.o"} {
	    cleanUp o $id
	}
    }

    # Get the number of loaded module
    set NbModuleLoad [llength [wtxObjModuleList]]

    # verify if nullSizeModule.o exist in right directory
    set name [objPathGet test]nullSizeModule.o
    if { [file exists $name] == 0 } {
        testFail "File $name is missing for wtxObjModuleLoadTest7 test"
        return 0
        }

    # load nullSizeModule.o
    # if no exception occurred
    if {![catch {wtxObjModuleLoad $ts $name} tModuleId]} {
	testFail $errObjMsg(13)
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $tModuleId "API_NULL_SIZE_FILE"]} {
	testFail $tModuleId
	return
	}

    set NbModuleLoad2 [llength [wtxObjModuleList]]

    if {$NbModuleLoad != $NbModuleLoad2} {
          testFail $errObjMsg(13)
          return
     }

    wtxTestPass
}


#*****************************************************************************
#
# wtxObjModuleLoadTest8 -
#
# Verify if wtxObjModuleLoad manage correctly invalid object module format.
#

proc wtxObjModuleLoadTest8 {ts Msg} {

    global errObjMsg

    puts stdout "$Msg" nonewline

    set objModuleIdList [wtxObjModuleList]
    
    foreach id $objModuleIdList {
	set moduleInfo [wtxObjModuleInfoGet $id]
	if {[lindex $moduleInfo 1] == "unknownFormat.o"} {
	    cleanUp o $id
	}
    }

    # Get the number of loaded module
    set NbModuleLoad [llength [wtxObjModuleList]]

    # verify if unknownFormat.o exist in right directory
    set name [objPathGet test]unknownFormat.o
    if { [file exists $name] == 0 } {
        testFail "File $name is missing for wtxObjModuleLoadTest8 test"
        return
        }

    # load unknownFormat.o
    # if no exception occurred
    if {![catch {wtxObjModuleLoad $ts $name} tModuleId]} {
	testFail $errObjMsg(14)
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $tModuleId "LOADER_UNKNOWN_OBJ_MODULE_FORMAT"]} {
	testFail $tModuleId
	return
	}

    set NbModuleLoad2 [llength [wtxObjModuleList]]

    if {$NbModuleLoad != $NbModuleLoad2} {
          testFail $errObjMsg(14)
          return
     }

    wtxTestPass
}

#*****************************************************************************
#
# wtxObjModuleLoadTest9 -
#
# Verify if wtxObjModuleLoad manage correctly invalid flags conbination.
#

proc wtxObjModuleLoadTest9 {ts Msg} {

    global errObjMsg

    puts stdout "$Msg" nonewline

    set objModuleIdList [wtxObjModuleList]
    
    foreach id $objModuleIdList {
	set moduleInfo [wtxObjModuleInfoGet $id]
	if {[lindex $moduleInfo 1] == "objSampleWtxtclTest6.o"} {
	    cleanUp o $id
	}
    }

    # Get the number of loaded module
    set NbModuleLoad [llength [wtxObjModuleList]]

    # verify if objSampleWtxtclTest6.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest6.o
    if { [file exists $name] == 0 } {
        testFail "File $name is missing for wtxObjModuleLoadTest9 test"
        return
        }

    # load objSampleWtxtclTest6.o with LOAD_ALL_SYMBOLS(1) and LOAD_NO_SYMBOLS(2) Flags.
    # if no exception occurred
    if {![catch {wtxObjModuleLoad $ts 3 $name} tModuleId]} {
	testFail $errObjMsg(15)
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $tModuleId "LOADER_ILLEGAL_FLAGS_COMBINATION"]} {
	testFail $tModuleId
	return
	}

    set NbModuleLoad2 [llength [wtxObjModuleList]]

    if {$NbModuleLoad != $NbModuleLoad2} {
          testFail $errObjMsg(15)
          return
     }

    wtxTestPass
}

#*****************************************************************************
#
# wtxObjModuleLoadTest10 -
#
# Verify if wtxObjModuleLoad manage correctly invalid address
#

proc wtxObjModuleLoadTest10 {Msg invalidAddr} {

    global errObjMsg

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

    set objModuleIdList [wtxObjModuleList]
    
    foreach id $objModuleIdList {
	set moduleInfo [wtxObjModuleInfoGet $id]
	if {[lindex $moduleInfo 1] == "objSampleWtxtclTest6.o"} {
	    cleanUp o $id
	}
    }

    # Get the number of loaded module
    set NbModuleLoad [llength [wtxObjModuleList]]

    # verify if objSampleWtxtclTest6.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest6.o
    if { [file exists $name] == 0 } {
        testFail "File $name is missing for wtxObjModuleLoadTest10 test"
        return
    }

    if {[catch "wtxMemAlloc 10000" memAddr1]} {
	testFail $memAddr1
	return
    }

    if {[catch "wtxMemAlloc 10000" memAddr2]} {
	testFail $memAddr2
	cleanUp ms $memAddr1
	return
    }


 # 1 - load objSampleWtxtclTest6.o with an invalid address for text segment

    # if no exception occurred
    if {![catch {wtxObjModuleLoad $ts $invalidAdd \
		$memAddr1 0 $name} tModuleId]} {
	testFail $errObjMsg(16)
	cleanUp ms $memAddr1 $memAddr2
	return
    }


# if the caught exception is different than the expected error code
# report an error

    if {![wtxErrorMatch $tModuleId "AGENT_MEM_ACCESS_ERROR"]} {
	testFail $tModuleId
	cleanUp ms $memAddr1 $memAddr2
	return
    }

 # 2 - load objSampleWtxtclTest6.o with an invalid address for data segment

    # if no exception occurred
    if {![catch {wtxObjModuleLoad $ts $memAddr2 \
		$invalidAdd 0 $name} tModuleId]} {
	testFail $errObjMsg(16)
	cleanUp ms $memAddr1 $memAddr2
	return
    }


    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $tModuleId "AGENT_MEM_ACCESS_ERROR"]} {
	testFail $tModuleId
	cleanUp ms $memAddr1 $memAddr2
	return
    }

    set NbModuleLoad2 [llength [wtxObjModuleList]]

    if {$NbModuleLoad != $NbModuleLoad2} {
          testFail $errObjMsg(16)
	  cleanUp ms $memAddr1 $memAddr2
          return
     }

    # clean up
    cleanUp ms $memAddr1 $memAddr2

    wtxTestPass
}



#*****************************************************************************
#
# wtxObjModuleLoadTest11 -
#
# Verify that wtxObjModuleLoad manages correctly bss segment.
#

proc wtxObjModuleLoadTest11 {ts Msg} {

    global errObjMsg

    puts stdout "$Msg" nonewline

    # verify if objSampleWtxtclTest6.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest6.o 
    if { [file exists $name] == 0 } {
	testFail "File $name is missing for wtxObjModuleLoadTest11 test"
	return
    }

    set tModuleId [wtxObjModuleLoad $ts LOAD_GLOBAL_SYMBOLS $name]
    if {$tModuleId == ""} {
	return
    }

    set tModuleId [lindex $tModuleId 0]

    set testResult [funcCall initBssVariableTest]

    if {$testResult != 0} {
	testFail $errObjMsg(17)
	cleanUp o $tModuleId
	return
    }

    # clean up
    cleanUp o $tModuleId


    wtxTestPass
 }



#*****************************************************************************
#
# wtxObjModuleLoadTest12 -
#
# Verify if wtxObjModuleLoad load correctly objSampleWtxtclTest6.o file with 
# the flag LOAD_LOCAL_SYMBOLS.
#

proc wtxObjModuleLoadTest12 {ts Msg} {

    global errObjMsg

    puts stdout "$Msg" nonewline

    set objModuleIdList [wtxObjModuleList]
    
    foreach id $objModuleIdList {
	set moduleInfo [wtxObjModuleInfoGet $id]
	if {[lindex $moduleInfo 1] == "objSampleWtxtclTest6.o"} {
	    cleanUp o $id
	}
    }

    # verify if objSampleWtxtclTest6.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest6.o 
    if { [file exists $name] == 0 } {
	testFail "File $name is missing for wtxObjModuleLoadTest12 test"
	return 0
    }

    # load objSampleWtxtclTest6.o file with LOAD_LOCAL_SYMBOLS flag
    set tModuleId [wtxObjModuleLoad $ts LOAD_LOCAL_SYMBOLS $name]
    if {$tModuleId == ""} {
	return 0
	}

    # get object module Id
    set tModuleId [lindex $tModuleId 0]


    # Verify that local symbols are loaded
    
    wtxErrorHandler [lindex [wtxHandle] 0] ""

    if [catch {wtxSymFind -name staticVoidFunc}] {
	testFail $errObjMsg(22)
	return 0
    }

    if [catch {wtxSymFind -name staticIntVarInit}] {
	testFail $errObjMsg(22)
	return 0
    }

    if [catch {wtxSymFind -name staticGlobalVariable}] {
	testFail $errObjMsg(22)
	return 0
    }



    # Verify that there is no global symbols loaded

    if ![catch {wtxSymFind -name voidFunc}] {
	testFail $errObjMsg(22)
	return 0
    }

    if ![catch {wtxSymFind -name varIntInit}] {
	testFail $errObjMsg(22)
	return 0
    }

    if ![catch {wtxSymFind -name varIntNoInit}] {
	testFail $errObjMsg(22)
	return 0
    }

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # clean up
    cleanUp o $tModuleId

    wtxTestPass
}

#*****************************************************************************
#
# wtxObjModuleLoadTest13 -
#
# Verify if wtxObjModuleLoad load correctly objSampleWtxtclTest6.o file 
# with the flag LOAD_ALL_SYMBOLS.
#

proc wtxObjModuleLoadTest13 {ts Msg} {

    global errObjMsg

    puts stdout "$Msg" nonewline

    set objModuleIdList [wtxObjModuleList]
    
    foreach id $objModuleIdList {
	set moduleInfo [wtxObjModuleInfoGet $id]
	if {[lindex $moduleInfo 1] == "objSampleWtxtclTest6.o"} {
	    cleanUp o $id
	}
    }

    # verify if objSampleWtxtclTest6.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest6.o 
    if { [file exists $name] == 0 } {
	testFail "File $name is missing for wtxObjModuleLoadTest13 test"
	return
    }

    # load objSampleWtxtclTest6.o file with LOAD_LOCAL_SYMBOLS flag
    set tModuleId [wtxObjModuleLoad $ts LOAD_ALL_SYMBOLS\
		[objPathGet test]objSampleWtxtclTest6.o]
    if {$tModuleId == ""} {
	return
	}

    # get object module Id
    set tModuleId [lindex $tModuleId 0]


    # Verify that all symbols are loaded
    wtxErrorHandler [lindex [wtxHandle] 0] ""

    if [catch {wtxSymFind -name staticVoidFunc}] {
	testFail $errObjMsg(23)
	return
    }
    if [catch {wtxSymFind -name staticIntVarInit}] {
	testFail $errObjMsg(23)
	return
    }
    if [catch {wtxSymFind -name staticGlobalVariable}] {
	testFail $errObjMsg(23)
	return
    }
    if [catch {wtxSymFind -name voidFunc}] {
	testFail $errObjMsg(23)
	return
    }
    if [catch {wtxSymFind -name varIntInit}] {
	testFail $errObjMsg(23)
	return
    }
    if [catch {wtxSymFind -name varIntNoInit}] {
	testFail $errObjMsg(23)
	return
    }
    if [catch {wtxSymFind -name divide}] {
	testFail $errObjMsg(23)
	return
    }
    if [catch {wtxSymFind -name ctlBssVariableTest}] {
	testFail $errObjMsg(23)
	return
    }
    if [catch {wtxSymFind -name initBssVariableTest}] {
	testFail $errObjMsg(23)
	return
    }

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # clean up
    cleanUp o $tModuleId

    wtxTestPass
}

#*****************************************************************************
#
# wtxObjModuleLoadTest14 -
#
# Verify if wtxObjModuleLoad load correctly objSampleWtxtclTest6.o file 
# with the flag LOAD_NO_SYMBOLS.
#

proc wtxObjModuleLoadTest14 {ts Msg} {

    global errObjMsg

    puts stdout "$Msg" nonewline

    set objModuleIdList [wtxObjModuleList]
    
    foreach id $objModuleIdList {
	set moduleInfo [wtxObjModuleInfoGet $id]
	if {[lindex $moduleInfo 1] == "objSampleWtxtclTest6.o"} {
	    cleanUp o $id
	}
    }

    # verify if objSampleWtxtclTest6.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest6.o 
    if { [file exists $name] == 0 } {
	testFail "File $name is missing for wtxObjModuleLoadTest14 test"
	return
    }

    # load objSampleWtxtclTest6.o file with LOAD_NO_SYMBOLS flag
    set tModuleId [wtxObjModuleLoad $ts LOAD_NO_SYMBOLS\
		[objPathGet test]objSampleWtxtclTest6.o]
    if {$tModuleId == ""} {
	return
    }

    # get object module Id
    set tModuleId [lindex $tModuleId 0]

    wtxErrorHandler [lindex [wtxHandle] 0] ""

    # Verify that no symbols added to the target symbol table
    set symbolsList [wtxSymListGet -module objSampleWtxtclTest6.o]

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    if {$symbolsList != ""} {
	testFail $errObjMsg(24)
	return
    }

    # clean up
    cleanUp o $tModuleId

    wtxTestPass
}


#*****************************************************************************
#
# wtxObjModuleLoadTest15 -
#
# Verify if wtxObjModuleLoad load correctly objSampleWtxtclTest2.o file 
# with the flag LOAD_HIDDEN_MODULE.
#

proc wtxObjModuleLoadTest15 {ts Msg} {

    global errObjMsg

    puts stdout "$Msg" nonewline

    set objModuleIdList [wtxObjModuleList]
    
    foreach id $objModuleIdList {
	set moduleInfo [wtxObjModuleInfoGet $id]
	if {[lindex $moduleInfo 1] == "objSampleWtxtclTest2.o"} {
	    cleanUp o $id
	}
    }

    # Get the number of loaded modules
    set moduleNbr [llength [wtxObjModuleList]]

    # verify if objSampleWtxtclTest2.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest2.o 
    if { [file exists $name] == 0 } {
	testFail "File $name is missing for wtxObjModuleLoadTest15 test"
	return
    }

    # load objSampleWtxtclTest2.o file with LOAD_HIDDEN_MODULE flag
    set tModuleId [wtxObjModuleLoad $ts LOAD_GLOBAL_SYMBOLS|LOAD_HIDDEN_MODULE\
       $name]
    if {$tModuleId == ""} {
	return
    }

    # get object module Id
    set tModuleId [lindex $tModuleId 0]

    # Verify that module is hidden

    set moduleNbr2 [llength [wtxObjModuleList]]

    if {$moduleNbr2 != $moduleNbr} {
	testFail $errObjMsg(25)
	cleanUp o $tModuleId
	return
    }


    # clean up
    cleanUp o $tModuleId

    wtxTestPass
}



#*****************************************************************************
#
# wtxObjModuleLoadTest16 -
#
# Verify if wtxObjModuleLoad load correctly cplus.o file with the flag
# LOAD_CPLUS_XTOR_MANUAL.
#

proc wtxObjModuleLoadTest16 {ts Msg} {

    global errObjMsg

    puts stdout "$Msg" nonewline

    set objModuleIdList [wtxObjModuleList]
    
    foreach id $objModuleIdList {
	set moduleInfo [wtxObjModuleInfoGet $id]
	if {[lindex $moduleInfo 1] == "cplus.o"} {
	    cleanUp o $id
	}
    }

    # Get the number of loaded modules
    set moduleNbr [llength [wtxObjModuleList]]

    # verify if cplus.o exist in right directory
    set name [objPathGet test]cplus.o 
    if { [file exists $name] == 0 } {
	testFail "File $name is missing for wtxObjModuleLoadTest16 test"
	return
    }

    # load cplus.o file with LOAD_CPLUS_XTOR_MANUAL flag
    set tModuleId [wtxObjModuleLoad $ts\
       LOAD_GLOBAL_SYMBOLS|LOAD_CPLUS_XTOR_MANUAL $name]
    if {$tModuleId == ""} {
	return
    }

    # get object module Id
    set tModuleId [lindex $tModuleId 0]


    # Get the real name of cplusTest function
    set cplusRealName [lindex [lindex [wtxSymListGet -name cplusTest] 0] 0]

    # Call cplusTest function:
    # cplusTest affect the returned value of the "what" function of
    # classA and classB to resTestClassA and resTestClassB

    funcCall $cplusRealName


    # Get resTestClassA address
    set addrA [lindex [wtxSymFind -name resTestClassA] 1]
    if {$addrA == ""} {
        cleanUp o $tModuleId
        return
        }

    # Get resTestClassB address
    set addrB [lindex [wtxSymFind -name resTestClassB] 1]
    if {$addrB == ""} {
        cleanUp o $tModuleId
        return
        }

    # Read resTestClassA value
    set resA [wtxGopherEval -errvar errCode "$addrA *!"]
    if {$resA == ""} {
        cleanUp o $tModuleId
        return
        }

    # Read resTestClassB value
    set resB [wtxGopherEval -errvar errCode "$addrB *!"]
    if {$resB == ""} {
        cleanUp o $tModuleId
        return
        }

    # Verify that the constructor hasn't been called
    if {$resA != 0 || $resB != 0} {
	testFail $errObjMsg(26)
	cleanUp o $tModuleId
	return
    }


    # clean up
    cleanUp o $tModuleId

    wtxTestPass
}


#*****************************************************************************
#
# wtxObjModuleLoadTest17 -
#
# Verify if wtxObjModuleLoad load correctly cplus.o file with the flag
# LOAD_CPLUS_XTOR_AUTO.
#

proc wtxObjModuleLoadTest17 {ts Msg} {

    global errObjMsg

    puts stdout "$Msg" nonewline

    set objModuleIdList [wtxObjModuleList]
    
    foreach id $objModuleIdList {
	set moduleInfo [wtxObjModuleInfoGet $id]
	if {[lindex $moduleInfo 1] == "cplus.o"} {
	    cleanUp o $id
	}
    }

    # Get the number of loaded modules
    set moduleNbr [llength [wtxObjModuleList]]

    # verify if cplus.o exist in right directory
    set name [objPathGet test]cplus.o 
    if { [file exists $name] == 0 } {
	testFail "File $name is missing for wtxObjModuleLoadTest17 test"
	return
    }

    # load cplus.o file with LOAD_CPLUS_XTOR_AUTO flag
    set tModuleId [wtxObjModuleLoad $ts\
       LOAD_GLOBAL_SYMBOLS|LOAD_CPLUS_XTOR_AUTO $name]
    if {$tModuleId == ""} {
	return
    }

    # get object module Id
    set tModuleId [lindex $tModuleId 0]


    # Get the real name of cplusTest function
    set cplusRealName [lindex [lindex [wtxSymListGet -name cplusTest] 0] 0]

    # Call cplusTest function:
    # cplusTest affect the returned value of the "what" function of
    # classA and classB to resTestClassA and resTestClassB

    funcCall $cplusRealName


    # Get resTestClassA address
    set addrA [lindex [wtxSymFind -name resTestClassA] 1]
    if {$addrA == ""} {
        cleanUp o $tModuleId
        return
        }

    # Get resTestClassB address
    set addrB [lindex [wtxSymFind -name resTestClassB] 1]
    if {$addrB == ""} {
        cleanUp o $tModuleId
        return
        }

    # Read resTestClassA value
    set resA [wtxGopherEval -errvar errCode "$addrA *!"]
    if {$resA == ""} {
        cleanUp o $tModuleId
        return
        }

    # Read resTestClassB value
    set resB [wtxGopherEval -errvar errCode "$addrB *!"]
    if {$resB == ""} {
        cleanUp o $tModuleId
        return
        }

    # Verify that the constructor has been called
    if {$resA != 98765 || $resB != 43210} {
	testFail $errObjMsg(27)
	cleanUp o $tModuleId
	return
    }


    # clean up
    cleanUp o $tModuleId

    wtxTestPass
}


#*****************************************************************************
#
# wtxObjModuleLoadTest18 - wtxObjModuleLoad with LOAD_COMMON_MATCH_NONE
#
# This test loads objSampleWtxtclTest3.o which contains "dummy1" a common 
# symbol then it loads objSampleWtxtclTest6.o which also contains a common 
# symbol named "dummy1", by default the flag LOAD_COMMON_MATCH_NONE is used, 
# so "dummy1" should be allocated twice.
#

proc wtxObjModuleLoadTest18 {ts Msg} {

    global errObjMsg

    puts stdout "$Msg" nonewline

    set objModuleIdList [wtxObjModuleList]

    # if objSampleWtxtclTest3.o or objSampleWtxtclTest6.o are already
    # loaded unload them
    foreach id $objModuleIdList {
	set moduleInfo [wtxObjModuleInfoGet $id]
	if {[lindex $moduleInfo 1] == "objSampleWtxtclTest3.o" || \
	    [lindex $moduleInfo 1] == "objSampleWtxtclTest6.o" } {
	    cleanUp o $id
	}
    }

    # verify that objSampleWtxtclTest3.o exist in right directory
    set nameTest3 [objPathGet test]objSampleWtxtclTest3.o 
    if { [file exists $nameTest3] == 0 } {
	testFail "File $nameTest3 is missing for wtxObjModuleLoadTest18 test"
	return
    }

    # verify that objSampleWtxtclTest6.o exist in right directory
    set nameTest6 [objPathGet test]objSampleWtxtclTest6.o 
    if { [file exists $nameTest6] == 0 } {
	testFail "File $nameTest6 is missing for wtxObjModuleLoadTest18 test"
	return
    }

    # load objSampleWtxtclTest3.o (by default LOAD_COMMON_MATCH_NONE is used)
    set test3Id [wtxObjModuleLoad $ts $nameTest3]
    if {$test3Id == ""} {
	return
    }

    # get object module Id
    set test3Id [lindex $test3Id 0]

    # get the list of symbols which names matche "dummy1"
    set dummy1List [wtxSymListGet -name dummy1]
    set listLength [llength $dummy1List]

    # load objSampleWtxtclTest6.o file with LOAD_COMMON_MATCH_NONE
    set test6Id [wtxObjModuleLoad $ts\
		LOAD_COMMON_MATCH_NONE|LOAD_GLOBAL_SYMBOLS $nameTest6]
    if {$test6Id == ""} {
	return
    }

    # get object module Id
    set test6Id [lindex $test6Id 0]

    # get the new list of symbols which names matche "dummy1"
    set newDummy1List [wtxSymListGet -name dummy1]
    set newListLength [llength $newDummy1List]

    # clean up
    cleanUp o $test3Id $test6Id

    if {[expr $newListLength - $listLength] != 1} {
	testFail $errObjMsg(28)
	return
    }

    wtxTestPass
}


#*****************************************************************************
#
# wtxObjModuleLoadTest19 - wtxObjModuleLoad with LOAD_COMMON_MATCH_USER
#
# This test loads objSampleWtxtclTest3.o which contains "dummy1" a common 
# symbol then it loads objSampleWtxtclTest6.o which also contains a common 
# symbol named "dummy1" but objSampleWtxtclTest6.o is loaded with the flag 
# LOAD_COMMON_MATCH_USER, so "dummy1" should be allocated only once.
#

proc wtxObjModuleLoadTest19 {ts Msg} {

    global errObjMsg

    puts stdout "$Msg" nonewline

    set objModuleIdList [wtxObjModuleList]

    # if objSampleWtxtclTest3/6.o are already loaded unload them
    foreach id $objModuleIdList {
	set moduleInfo [wtxObjModuleInfoGet $id]
	if {[lindex $moduleInfo 1] == "objSampleWtxtclTest3.o" || \
	    [lindex $moduleInfo 1] == "objSampleWtxtclTest6.o" } {
	    cleanUp o $id
	}
    }

    # verify that objSampleWtxtclTest3.o exist in right directory
    set nameTest3 [objPathGet test]objSampleWtxtclTest3.o 
    if { [file exists $nameTest3] == 0 } {
	testFail "File $nameTest3 is missing for wtxObjModuleLoadTest19 test"
	return
    }

    # verify that objSampleWtxtclTest6.o exist in right directory
    set nameTest6 [objPathGet test]objSampleWtxtclTest6.o 
    if { [file exists $nameTest6] == 0 } {
	testFail "File $nameTest6 is missing for wtxObjModuleLoadTest19 test"
	return
    }

    # load objSampleWtxtclTest3.o (by default LOAD_COMMON_MATCH_NONE is used)
    set test3Id [wtxObjModuleLoad $ts $nameTest3]
    if {$test3Id == ""} {
	return
    }

    # get object module Id
    set test3Id [lindex $test3Id 0]

    # get the list of symbols which names matche "dummy1"
    set dummy1List [wtxSymListGet -name dummy1]
    set listLength [llength $dummy1List]

    # load objSampleWtxtclTest6.o file with LOAD_COMMON_MATCH_USER
    set test6Id [wtxObjModuleLoad $ts\
		LOAD_COMMON_MATCH_USER|LOAD_GLOBAL_SYMBOLS $nameTest6]
    if {$test6Id == ""} {
	return
    }

    # get object module Id
    set test6Id [lindex $test6Id 0]

    # get the new list of symbols which names matche "dummy1"
    set newDummy1List [wtxSymListGet -name dummy1]
    set newListLength [llength $newDummy1List]

    # clean up
    cleanUp o $test3Id $test6Id

    if {$newListLength != $listLength} {
	testFail $errObjMsg(29)
	return
    }

    wtxTestPass
}


#*****************************************************************************
#
# wtxObjModuleLoadTest20 - wtxObjModuleLoad with LOAD_COMMON_MATCH_ALL
#
# This test loads objSampleWtxtclTest6.o LOAD_COMMON_MATCH_ALL which contains
# "wdbIsNowExternal" a common symbol that also exist in vxWorks,
# so "wdbIsNowExternal" should not be allocated a second time.
#

proc wtxObjModuleLoadTest20 {ts Msg} {

    global errObjMsg

    puts stdout "$Msg" nonewline

    set objModuleIdList [wtxObjModuleList]

    # if objSampleWtxtclTest6.o is already loaded unload it
    foreach id $objModuleIdList {
	set moduleInfo [wtxObjModuleInfoGet $id]
	if {[lindex $moduleInfo 1] == "objSampleWtxtclTest3.o"} {
	    cleanUp o $id
	}
    }

    # verify that objSampleWtxtclTest6.o exist in right directory
    set nameTest6 [objPathGet test]objSampleWtxtclTest6.o 
    if { [file exists $nameTest6] == 0 } {
	testFail "File $nameTest6 is missing for wtxObjModuleLoadTest20 test"
	return
    }

    # get the list of symbols which names matche "wdbIsNowExternal"
    set symList [wtxSymListGet -name wdbIsNowExternal]
    set listLength [llength $symList]

    # load objSampleWtxtclTest6.o file with LOAD_COMMON_MATCH_ALL flag
    set test6Id [wtxObjModuleLoad $ts\
		LOAD_COMMON_MATCH_ALL|LOAD_GLOBAL_SYMBOLS $nameTest6]
    if {$test6Id == ""} {
	return
    }

    # get object module Id
    set test6Id [lindex $test6Id 0]

    # get the new list of symbols which names matche "wdbIsNowExternal"
    set newSymList [wtxSymListGet -name wdbIsNowExternal]
    set newListLength [llength $newSymList]

    # clean up
    cleanUp o $test6Id

    if {$newListLength != $listLength} {
	testFail $errObjMsg(30)
	return
    }

    wtxTestPass
}

#*****************************************************************************
#
# checkCplus - return 1 if cplusLibInit symbol is there.
#

proc checkCplus {} {

    set cplus 0
    set symbStruct ""
    catch {set symbStruct [wtxSymFind -name cplusLibInit]}

    if { $symbStruct != "" } {
	set symb [lindex $symbStruct 0]
	if {[string match "_cplusLibInit" $symb] ||\
	    [string match "cplusLibInit" $symb]} {
	    set cplus 1
	}
     }

     return $cplus
}

#*****************************************************************************
#
# wtxObjModuleLoadTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxObjModuleLoadTest {tgtsvrName timeout invalidAddr} {

    global vObjMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxObjModuleLoadTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # call valid tests
    wtxToolAttachCall $tgtsvrName $serviceType $timeout

    # Use target server to load file
    set ts "-ts"

    # set cplus value 
    set cplus [checkCplus]

    catch {wtxRegisterForEvent .*} st

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    wtxObjModuleLoadTest12 $ts $vObjMsg(23)
    wtxObjModuleLoadTest13 $ts $vObjMsg(24)
    wtxObjModuleLoadTest14 $ts $vObjMsg(25)
    wtxObjModuleLoadTest15 $ts $vObjMsg(26)
    wtxObjModuleLoadTest2 $ts $vObjMsg(5)
    wtxObjModuleLoadTest3 $ts $vObjMsg(7)
    if { $cplus } {
     wtxObjModuleLoadTest6 $ts $vObjMsg(15)
     wtxObjModuleLoadTest16 $ts $vObjMsg(27)
     wtxObjModuleLoadTest17 $ts $vObjMsg(28)
    }
    wtxObjModuleLoadTest18 $ts $vObjMsg(29)
    wtxObjModuleLoadTest19 $ts $vObjMsg(30)
    wtxObjModuleLoadTest20 $ts $vObjMsg(31)
    wtxObjModuleLoadTest11 $ts $vObjMsg(20)

    foreach ts {"" "-ts"} {
 
     wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

     puts stdout ""
     if { $ts == "-ts" } {
        puts stdout "\>\>Direct access by Target server."
     } else {
        puts stdout "\>\>Use a file transfer."
     }

     if {[set essai [wtxObjModuleLoadTest1 $ts $vObjMsg(3)]]} {
        puts stdout "\t(objSampleWtxtclTest6.o has been loaded in $essai msec)"
     }
 
     wtxObjModuleLoadTest0 $ts $tgtsvrName $vObjMsg(133)

     wtxErrorHandler [lindex [wtxHandle] 0] ""

     # call invalid tests

     wtxObjModuleLoadTest4 $ts $vObjMsg(8)
     # wtxObjModuleLoadTest5 $ts $vObjMsg(12)
     wtxObjModuleLoadTest7 $ts $vObjMsg(16)
     wtxObjModuleLoadTest8 $ts $vObjMsg(17)
     wtxObjModuleLoadTest9 $ts $vObjMsg(18)
     wtxObjModuleLoadTest10 $ts $vObjMsg(19) $invalidAddr
    }

    wtxToolDetachCall "wtxObjModuleLoad : "
}
