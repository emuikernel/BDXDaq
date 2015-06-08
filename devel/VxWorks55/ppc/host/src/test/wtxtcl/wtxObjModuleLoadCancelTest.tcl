# wtxObjModuleLoadCancelTest.tcl - Tcl script, test WTX_OBJ_MODULE_LOAD_CANCEL
#
# Copyright 1994-1998 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01b,07jan99,p_b added 500ms sleep in a loop.
# 01a,24jun98,p_b written.
#
# DESCRIPTION
#
# WTX_OBJ_MODULE_LOAD_CANCEL  -  Cancel the Load of an object file,
#                                performed with wtxObjModuleLoadStart.
#
#
#

#*****************************************************************************
#
# wtxObjModuleLoadCancelTest1 - Local load of a file, Cancel the load
#                               and check if the module is not in the
#                               module list.
#

proc wtxObjModuleLoadCancelTest1 {ts tgtsvrName Msg} {
    
    global errObjMsg
    global env
    
    puts stdout "$Msg" nonewline
    
    if {[catch {wtxObjModuleList} objModuleIdList]} {
	testFail "wtxObjModuleList failed. ($objModuleIdList)."
	return 0
    }
    
    if { $env(WIND_HOST_TYPE) == "x86-win32" } {
	set nameTmpFile $env(TMP)\\objSampleWtxtclTest8.o
    } else {
	set nameTmpFile /tmp/objSampleWtxtclTest8.o
    }
    
    foreach id $objModuleIdList {
        set moduleInfo [wtxObjModuleInfoGet $id]
        if {[lindex $moduleInfo 1] == "objSampleWtxtclTest8.o"} {
            cleanUp o $id
        }
    }
    
    set tIdList [wtxObjModuleList]
    if {$tIdList == ""} {
        return 0
    }
    
    set NbModuleLoad [llength $tIdList]
    
    # verify whether objSampleWtxtclTest8.o exists in the right directory
    set name [objPathGet test]objSampleWtxtclTest8.o
    if { [file exists $name] == 0 } {
        testFail "File $name is missing for wtxObjModuleLoadCancel test"
        return 0
    }
    
    set theSourceFile $name
    file copy -force $theSourceFile $nameTmpFile
    
    # load local objSampleWtxtclTest8.o file
    if {[catch {wtxObjModuleLoadStart $ts LOAD_GLOBAL_SYMBOLS\
	    $nameTmpFile} res]} {
	testFail "wtxObjModuleLoadStart failed. ($res)."
	return 0
    }
    
    if { $res != "" } {
	testFail "wtxObjModuleLoadStart failed. ($res)."
	return 0
    }
    
    if {[catch {wtxObjModuleLoadProgressReport} goOn]} {
	testFail "wtxObjModuleLoadProgressReport failed. ($goOn)."
	return 0
    }

    # puts stdout $goOn
    set goOn [lindex $goOn 0]
    
    # Cancel while the module is not completely loaded.
    if { $goOn < "4" } {
	after 500
	if {[catch {wtxObjModuleLoadCancel} res]} {
	    testFail "wtxObjModuleLoadCancel failed.  ($res)."
	    return 0
	}
	if { $res != "" } {
	    testFail "wtxObjModuleLoadCancel failed. ($res)."
	    return 0
	}
    } else {
	puts stdout "Not the time to perform a Cancel."
	puts stdout "Test SKIPPED"
	# puts stdout $goOn
	return 0
    }
    
    # Check if module is not loaded
    set NbModuleLoad2 [llength [wtxObjModuleList]]
    set Check [expr ($NbModuleLoad + 1)]
    
    if { $NbModuleLoad2 == $Check } {
        testFail "The load has not been cancel."
        return 0
    }
    
    file delete $nameTmpFile
    wtxTestPass
    
}


#*****************************************************************************
#
# wtxObjModuleLoadCancelTest2 - Local load of a file. The cancel operation is
#                               done while the file is loaded. Check the 
#                               behaviour of such a cancel.
#

proc wtxObjModuleLoadCancelTest2 {ts tgtsvrName Msg} {
    
    global errObjMsg 
    global env
    
    puts stdout "$Msg" nonewline
    
    if {[catch {wtxObjModuleList} objModuleIdList]} {
	testFail "wtxObjModuleList failed. ($objModuleIdList)."
	return 0
    }
    
    if { $env(WIND_HOST_TYPE) == "x86-win32" } {
	set nameTmpFile $env(TMP)\\objSampleWtxtclTest8.o
    } else {
	set nameTmpFile /tmp/objSampleWtxtclTest8.o
    }
    
    foreach id $objModuleIdList {
        set moduleInfo [wtxObjModuleInfoGet $id]
        if {[lindex $moduleInfo 1] == "objSampleWtxtclTest8.o"} {
            cleanUp o $id
        }
    }
    
    set tIdList [wtxObjModuleList]
    if {$tIdList == ""} {
        return 0
    }
    
    set NbModuleLoad [llength $tIdList]
    
    # verify whether objSampleWtxtclTest8.o exists in the right directory
    set name [objPathGet test]objSampleWtxtclTest8.o
    if { [file exists $name] == 0 } {
        testFail "File $name is missing for wtxObjModuleLoadCancel test."
        return 0
    }
    
    set theSourceFile $name
    file copy -force $theSourceFile $nameTmpFile
    
    # load local objSampleWtxtclTest8.o file
    if {[catch {wtxObjModuleLoadStart $ts LOAD_GLOBAL_SYMBOLS\
	    $nameTmpFile} res]} {
	testFail "wtxObjModuleLoadStart failed."
	return 0
    }
    
    if { $res != "" } {
	testFail "wtxObjModuleLoadStart failed. ($res)."
	return 0
    }
    
    if {[catch {wtxObjModuleLoadProgressReport} goOn]} {
	testFail "wtxObjModuleLoadProgressReport-1 failed."
	return 0
    }
    # puts stdout $goOn
    set goOn [lindex $goOn 0]
    
    # Wait until the module is loaded
    while { ![string match 0x* $goOn] } {
	if {[catch {wtxObjModuleLoadProgressReport} goOn]} {
	    testFail "wtxObjModuleLoadProgressReport-2 failed."
	    return 0
	}
	after 500
	# puts stdout $goOn
	set goOn [lindex $goOn 0]
    }
    
     after 500
    set modId $goOn
    # puts stdout "Module Id : $modId"
    
    # Cancel while the module has been loaded.
    if {![catch {wtxObjModuleLoadCancel} res]} {
	testFail "wtxObjModuleLoadCancel-1 failed."
	return 0
    }
    
    if { ![string match *SVR_EINVAL* $res] } {
	testFail "wtxObjModuleLoadCancel-1 failed."
	return 0
    }
    
    # Check if module is still there
    set NbModuleLoad2 [llength [wtxObjModuleList]]
    set Check [expr ($NbModuleLoad + 1)]
    
    if { $NbModuleLoad2 != $Check } {
        testFail "wtxObjModuleLoadCancel removes the loaded object."
        return 0
    }
    
    # clean up
    catch {wtxObjModuleUnload "objSampleWtxtclTest8.o"}
    
    file delete $nameTmpFile
    wtxTestPass
}

#*****************************************************************************
#
# wtxObjModuleLoadCancelTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxObjModuleLoadCancelTest {tgtsvrName timeout invalidAddr} {
    
    global vObjMsg
    
    # variables initializations
    set serviceType 2
    # Use target server to load file
    set ts "-ts"
    
    # call valid tests
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    catch {wtxRegisterForEvent .*} st
    
    # Set a big timeout for these tests
    set oldTimeout [wtxTimeout]
    wtxTimeout 120000
    wtxObjModuleLoadCancelTest1 $ts $tgtsvrName $vObjMsg(137)
    wtxObjModuleLoadCancelTest2 $ts $tgtsvrName $vObjMsg(138)
    wtxTimeout $oldTimeout
 
    wtxToolDetachCall "wtxObjModuleLoadCancel : "
    
}

