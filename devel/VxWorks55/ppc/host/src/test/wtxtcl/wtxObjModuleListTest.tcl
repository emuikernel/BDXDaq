# wtxObjModuleListTest.tcl - Tcl script, test WTX_OBJ_MODULE_LIST
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
# 01k,20oct95,jmp  added invalidAddr argument to wtxObjModuleListTest.
# 01j,09aug95,jmp  changed objGetPath to objPathGet
#		   added timeout to wtxToolAttachCall in wtxObjModuleListTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01i,23jun95,f_v  added wtxTestRun call
# 01h,21jun95,f_v  clean up main procedure
# 01g,28mar95,f_v  added wtxRegisterForEvent
# 01f,17mar95,f_v  add lindex to get objModule Id
# 01e,10mar95,f_v  remove ObjMsg
# 01d,20feb95,f_v  call objGetPath with test like parameter
#		   replace wtxEventGet by wtxEventPoll
# 01c,06feb95,f_v  put comments in front of objMsg 
#		   add event management and file exists test
#                  works on objSampleWtxtclTest5.o rather than VxWorks module
# 		   add clean up section
# 01b,27jan95,f_v  change access messages, use wtxToolDetachCall
# 01a,17jan95,f_v  written. 
#
#

#
# DESCRIPTION
#
# WTX_OBJ_MODULE_LIST  -  
#    This routine returns a list of object modules previously
#    loaded on the Target via wtxObjModuleLoad().
#
#
#

#*****************************************************************************
#
# wtxObjModuleListTest1 -
#
# Verify if wtxObjModuleFind find correctly objSampleWtxtclTest1.o file 
#

proc wtxObjModuleListTest1 {Msg} {

    global errObjMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # get the number of module which are already loaded 
    set moduleNb1 [llength [wtxObjModuleList]]

    # verify if objSampleWtxtclTest1.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest1.o
    if {[file exists $name] == 0 } {
	testFail "File $name is missing for wtxObjModuleListTest1 test"
	return
	}

    # load module objSampleWtxtclTest1.o
    set tModuleId [wtxObjModuleLoad $USE_TSFORLOAD\
		[objPathGet test]objSampleWtxtclTest1.o]
    if {$tModuleId == ""} {
	return
	}

    # get ObjModule Id
    set tModuleId [lindex $tModuleId 0]

    # get the Id of objSampleWtxtclTest1.o module by wtxObjModuleList
    set list [wtxObjModuleList]
    set moduleNb2 [llength $list]
    set posModuleId [lsearch $list $tModuleId] 

    # clean up
    cleanUp o $tModuleId

    set moduleNb1 [expr $moduleNb1+1]
    if { $posModuleId < 0 || $moduleNb2 != $moduleNb1 } {
	    testFail $errObjMsg(4)
	    return
    }

    wtxTestPass
}


#*****************************************************************************
#
# wtxObjModuleListTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxObjModuleListTest {tgtsvrName timeout invalidAddr} {

    global vObjMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxObjModuleListTest targetServerName\
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
    wtxObjModuleListTest1 $vObjMsg(2)

    # deconnection
    wtxToolDetachCall "wtxObjModuleListTest"
}
