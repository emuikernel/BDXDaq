# wtxObjModuleFindTest.tcl - Tcl script, test WTX_OBJ_MODULE_FIND
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01t,27may98,p_b  WTX 2.0 new changes
# 01s,14may98,p_b  Replace test?.o by objSampleWtxtclTest?.o
# 01r,28jan98,p_b  Adapted for WTX 2.0
# 01q,11mar97,jmp  modified catch format when a filename is given in
#                  parameters to avoid failures on windows side (SPR #7868).
# 01p,08nov95,s_w  Modify for SPR 5399 fix. OBJ_LOADED event no longer received
# 01o,20oct95,jmp  added invalidAddr argument to wtxObjModuleFindTest.
# 01n,17aug95,jmp  changed objGetPath to objPathGet,
#		   added timeout to wtxToolAttachCall in wtxObjModuleFindTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01k,23jun95,f_v  added wtxTestRun call
# 01j,21jun95,f_v  clean up main procedure
# 01i,27apr95,c_s  now uses wtxErrorMatch.
# 01h,28mar95,f_v  added wtxRegisterForEvent
# 01g,17mar95,f_v  add lindex to get objModule Id
# 01f,09mar95,f_v  remove ObjMsg messages
# 01e,03mar95,f_v  replace S_moduleLib_MODULE_NOT_FOUND by
#		   objModuleErr(WTX_ERR_LOADER_OBJ_MODULE_NOT_FOUND)
# 01d,16feb95,f_v  add wtxObjModuleFindTest2
# 	           call objGetPath with test like parameter
# 01c,06feb95,f_v  didn't search VxWorks module, but objSampleWtxtclTest5.o
#	           add wtxObjModuleFind by name	
#		   add file exists test, clean up section
# 01b,27Jan95,f_v  change access messages, use wtxToolDetachCall
# 01a,20Jan95,f_v  written. 
#
#

#
# DESCRIPTION
#
# WTX_OBJ_MODULE_FIND  -  Find a object file
#
#
#

#*****************************************************************************
#
# wtxObjModuleFindTest1 -
#
# Verify if wtxObjModuleFind find correctly objSampleWtxtclTest1.o file 
#

proc wtxObjModuleFindTest1 {Msg} {

    global errObjMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # verify if objSampleWtxtclTest1.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest1.o 
    if { [file exists $name] == 0 } {
	  testFail "File $name is missing for wtxObjModuleFindTest1 test"
	  return
       }
    
    set tModuleId [wtxObjModuleLoad $USE_TSFORLOAD [objPathGet test]objSampleWtxtclTest1.o]
    if {$tModuleId == ""} {
	return
    }

    # get object module Id
    set tModuleId [lindex $tModuleId 0]

    # get the name of module - here test1
    set testName [lindex [wtxObjModuleFind $tModuleId] 1]
    if {$testName == ""} {
	return
    }

    # get the Id of module
    set testId [lindex [wtxObjModuleFind objSampleWtxtclTest1.o] 0]
    if {$testId == ""} {
        return
    }

    # clean up
    cleanUp o $tModuleId

    wtxTestPass
}

#*****************************************************************************
#
# wtxObjModuleFindTest2 -
#
# Verify if wtxObjModuleFind manages correctly bad module Id and bad module name
#

proc wtxObjModuleFindTest2 {Msg} {

    global errObjMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # verify if objSampleWtxtclTest1.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest1.o 
    if {[file exists $name] == 0 } {
	testFail "File $name is missing for wtxObjModuleFindTest2 test"
	return
	}
    
    if {[catch {wtxObjModuleLoad $USE_TSFORLOAD\
		  [objPathGet test]objSampleWtxtclTest1.o} tModuleId]} {
	testFail $tModuleId
	return
	}

    # get object module Id
    set tModuleId [lindex $tModuleId 0]

    # corrumpt moduleId by substract 4 
    set tModuleId2 [expr $tModuleId - 4]

    # call wtxObjModuleFind with a bad module Id

    # if no exception occurred
    if {![catch {wtxObjModuleFind $tModuleId2} testName]} {
	testFail $errObjMsg(1)
	cleanUp o $tModuleId
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $testName LOADER_OBJ_MODULE_NOT_FOUND]} {
	testFail $testName
	cleanUp o $tModuleId
	return
	}


    # call wtxObjModuleFind with a bad name

    # if no exception occurred
    if {![catch {wtxObjModuleFind xxWWzz.o} testId ]} {
	testFail $errObjMsg(2)
	cleanUp o $tModuleId
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $testId LOADER_OBJ_MODULE_NOT_FOUND]} {
	testFail $testId
	cleanUp o $tModuleId
	return
        }

    # clean up
    cleanUp o $tModuleId

    wtxTestPass
}

#*****************************************************************************
#
# wtxObjModuleFindTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxObjModuleFindTest {tgtsvrName timeout invalidAddr} {

    global vObjMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxObjModuleFindTest targetServerName\
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
     
    # call valid testS
    wtxObjModuleFindTest1 $vObjMsg(0)
    
    # call invalid tests
    wtxErrorHandler [lindex [wtxHandle] 0] ""

    # enable all events
    catch {wtxRegisterForEvent .*} st

    wtxObjModuleFindTest2 $vObjMsg(10)

    # deconnection
    wtxToolDetachCall "wtxObjModuleFindTest"
}
