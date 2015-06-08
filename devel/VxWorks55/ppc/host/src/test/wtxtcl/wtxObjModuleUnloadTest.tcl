# wtxObjModuleUnloadTest.tcl - Tcl script, test WTX_OBJ_MODULE_UNLOAD
#
# Copyright 1994 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01p,14may98,p_b  Replace test?.o by objSampleWtxtclTest?.o
# 01o,28jan98,p_b  Adapted for WTX 2.0
# 01n,08nov95,s_w  modify for SPR 5399 fix. OBJ_LOADED event no longer received
# 01m,20oct95,jmp  added invalidAddr argument to wtxObjModuleUnloadTest.
# 01l,08sep95,jmp  added wtxObjModuleUnloadTest3, wtxObjModuleUnloadTest4.
# 01k,18aug95,jmp  changed objGetPath to objPathGet
#		   added timeout to wtxToolAttachCall in wtxObjModuleUnloadTest.#                  changed tests control method, now uses wtxErrorHandler.
# 01j,23jun95,f_v  added wtxTestRun call
# 01i,21jun95,f_v  clean up main procedure
# 01h,27apr95,c_s  now uses wtxErrorMatch.
# 01g,28mar95,f_v  added wtxRegisterForEvent
# 		   changed pomme to test1
# 01f,17mar95,f_v  add lindex to get objModule Id
# 01e,07mar95,f_v  change error code
# 01d,17feb95,f_v  add wtxObjModuleUnloadTest2
#		   call objGetPath with test like parameter
# 01c,03feb95,f_v  add event message managment
#                  add comments in front of ObjMsg
#                  add file exists test
# 01b,27jan95,f_v  change access messages, use wtxToolDetachCall
# 01a,17jan95,f_v  written. 
#
#

#
# DESCRIPTION
#
# WTX_OBJ_MODULE_UNLOAD  -  Unload object file
#
#
# TESTS :
#
# (1) wtxObjModuleUnloadTest1 - This test verifies that wtxObjModuleUnload
# 	correctly a module with only a text segment.
#
# (2) wtxObjModuleUnloadTest2 - This test verifies that wtxObjModuleUnload
#	manages bad file Id
#
# (3) wtxObjModuleUnloadTest3 - This test verifies that wtxObjModuleUnload
#       correctly a module with only a data segment.
#
# (4) wtxObjModuleUnloadTest4 - This test verifies that wtxObjModuleUnload
#       correctly a module with text and data segments.
#



#*****************************************************************************
#
# wtxObjModuleUnloadTest1 -
#
# Verify if wtxObjModuleUnload unload correctly objSampleWtxtclTest2.o file and frees
# memory allocated for the module.
#

proc wtxObjModuleUnloadTest1 {Msg} {

    global errObjMsg

    puts stdout "$Msg" nonewline

    # verify if objSampleWtxtclTest2.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest2.o
    if {[file exists $name] == 0 } {
	testFail "File $name is missing for wtxObjModuleUnloadTest1 test"
	return
	}

    # get the current agent free memory size
    set freeSizeBefore [lindex [wtxMemInfoGet] 0]

    # Load objSampleWtxtclTest2.o file
    set status [wtxObjModuleLoad [objPathGet test]objSampleWtxtclTest2.o]
    if {$status == ""} {
	return
	}

    # get object module Id
    set status [lindex $status 0]

    # Get the number of loaded module
    set NbModuleLoad [llength [wtxObjModuleList]]
    set i [expr $NbModuleLoad -1]

    set tIdList [wtxObjModuleList]

    while { $i > 0 } {
		set tId [lindex $tIdList $i]
		set ModuleInfo [wtxObjModuleInfoGet $tId]
		if {$ModuleInfo == ""} {
			return
			}
		set ModuleName [lindex $ModuleInfo 1]

		if { "$ModuleName" == {objSampleWtxtclTest2.o} } {
			break
			} else {
			incr i -1
			if { $i == 0 } {
			    testFail $errObjMsg(6)
			    return
			    }	
			}
    	        }

    # clean up
    cleanUp o $tId

   # get the new agent free memory size
   set freeSizeAfter [lindex [wtxMemInfoGet] 0]

   # verify that the memmory allocated by the module is correctly freed

   if {$freeSizeAfter != $freeSizeBefore} {
	testFail $errObjMsg(10)
	return
	}

    set NbModuleLoad2 [llength [wtxObjModuleList]]
    set Check [expr ($NbModuleLoad2 + 1)]

    if { $NbModuleLoad != $Check } {
	  testFail $errObjMsg(10)
	  return
     }

    wtxTestPass
}


#*****************************************************************************
#
# wtxObjModuleUnloadTest2 -
#
# Verify if wtxObjModuleUnload manages bad file Id 
#

proc wtxObjModuleUnloadTest2 {Msg} {

    global errObjMsg

    puts stdout "$Msg" nonewline

    # verify if objSampleWtxtclTest1.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest1.o
    if { [file exists $name] == 0 } {
	testFail "File $name is missing for wtxObjModuleUnloadTest2 test"
	return
	}

    # Load objSampleWtxtclTest1.o file
    if {[catch {wtxObjModuleLoad [objPathGet test]objSampleWtxtclTest1.o} tId]} {
	testFail $tId
	return
	}

    # get object module Id
    set tId [lindex $tId 0]

    # try to UnLoad file with a bad file Id

    # if no exception occurred
    if {![catch {wtxObjModuleUnload [expr $tId-4]} status ]} {
	testFail $errObjMsg(1)
	return
	}


    # clean up
    cleanUp o $tId

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $status LOADER_OBJ_MODULE_NOT_FOUND] } {
          testFail $status
          return
     }

    # if the caught exception is the expected error code
    wtxTestPass
}



#*****************************************************************************
#
# wtxObjModuleUnloadTest3 -
#
# This test verify that wtxObjModuleUnload unloads a module with only a 
# data segment and frees memory allocated for the module. 
#

proc wtxObjModuleUnloadTest3 {Msg} {

    global errObjMsg

    puts stdout "$Msg" nonewline

    # verify if objSampleWtxtclTest5.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest5.o
    if {[file exists $name] == 0 } {
	testFail "File $name is missing for wtxObjModuleUnloadTest3 test"
	return
	}

    # get the current agent free memory size
    set freeSizeBefore [lindex [wtxMemInfoGet] 0]

    # Load objSampleWtxtclTest5.o file
    set status [wtxObjModuleLoad [objPathGet test]objSampleWtxtclTest5.o]
    if {$status == ""} {
	return
	}

    # get object module Id
    set objModId [lindex $status 0]

    # clean up
    cleanUp o $objModId

   # get the new agent free memory size
   set freeSizeAfter [lindex [wtxMemInfoGet] 0]

   # verify that the memmory allocated by the module is correctly freed

   if {$freeSizeAfter != $freeSizeBefore} {
	testFail $errObjMsg(10)
	return
	}

   wtxTestPass
}


#*****************************************************************************
#
# wtxObjModuleUnloadTest4 -
#
# This test verify that wtxObjModuleUnload unloads a module with text and
# data segments and frees memory allocated for the module. 
#

proc wtxObjModuleUnloadTest4 {Msg} {

    global errObjMsg

    puts stdout "$Msg" nonewline

    # verify if objSampleWtxtclTest1.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest1.o
    if {[file exists $name] == 0 } {
	testFail "File $name is missing for wtxObjModuleUnloadTest4 test"
	return
	}

    # get the current agent free memory size
    set freeSizeBefore [lindex [wtxMemInfoGet] 0]

    # Load objSampleWtxtclTest1.o file
    set status [wtxObjModuleLoad [objPathGet test]objSampleWtxtclTest1.o]
    if {$status == ""} {
	return
	}

    # get object module Id
    set objModId [lindex $status 0]

    # clean up
    cleanUp o $objModId

   # get the new agent free memory size
   set freeSizeAfter [lindex [wtxMemInfoGet] 0]

   # verify that the memmory allocated by the module is correctly freed

   if {$freeSizeAfter != $freeSizeBefore} {
	testFail $errObjMsg(10)
	return
	}

   wtxTestPass
}


#*****************************************************************************
#
# wtxObjModuleUnloadTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxObjModuleUnloadTest {tgtsvrName timeout invalidAddr} {

    global vObjMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxObjModuleUnloadTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # call valid tests
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    catch {wtxRegisterForEvent .*} st

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    wtxObjModuleUnloadTest1  $vObjMsg(4)
    wtxObjModuleUnloadTest3  $vObjMsg(13)
    wtxObjModuleUnloadTest4  $vObjMsg(14)

    wtxErrorHandler [lindex [wtxHandle] 0] ""


    # call invalid tests
    catch {wtxRegisterForEvent .*} st
    wtxObjModuleUnloadTest2  $vObjMsg(11)

    # deconnection
    wtxToolDetachCall "wtxObjModuleUnloadTest"
}
