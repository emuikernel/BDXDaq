# wtxObjModuleLoadStartTest.tcl - Tcl script, test WTX_OBJ_MODULE_LOAD_START
#
# Copyright 1994-1998 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01a,24jun98,p_b written.
#

# DESCRIPTION
#
# WTX_OBJ_MODULE_LOAD_START - Load an object file using wtxObjModuleLoadStart.
#
 
#*****************************************************************************
#
# wtxObjModuleLoadStartTest1 - Local load of a file, run a function and
#                              check the result.  
#
#

proc wtxObjModuleLoadStartTest1 {ts tgtsvrName Msg} {

    global errObjMsg
    global env
 
    puts stdout "$Msg" nonewline
 
    set objModuleIdList [wtxObjModuleList]
    if { $env(WIND_HOST_TYPE) == "x86-win32" } {
       set nameTmpFile $env(TMP)\\objSampleWtxtclTest2.o
        } else {
       set nameTmpFile /tmp/objSampleWtxtclTest2.o
    }

    foreach id $objModuleIdList {
        set moduleInfo [wtxObjModuleInfoGet $id]
        if {[lindex $moduleInfo 1] == "objSampleWtxtclTest2.o"} {
            cleanUp o $id
        }
    }
 
    set tIdList [wtxObjModuleList]
    if {$tIdList == ""} {
        return 0
    }
 
    set NbModuleLoad [llength $tIdList]
 
    # checks whether objSampleWtxtclTest2.o exists in the right directory
    set name [objPathGet test]objSampleWtxtclTest2.o
    if { [file exists $name] == 0 } {
        testFail "File $name is missing for wtxObjModuleLoadStartTest1 test"
        return 0
    }
 
    set theSourceFile $name
    file copy -force $theSourceFile $nameTmpFile

    # load local objSampleWtxtclTest2.o file
    if {[catch {wtxObjModuleLoadStart $ts LOAD_GLOBAL_SYMBOLS\
             $nameTmpFile} res]} {
     testFail "wtxObjModuleLoadStart failed."
     return 0
    }

    if { $res != "" } {
     testFail "wtxObjModuleLoadStart failed."
     return 0
    }

    if {[catch {wtxObjModuleLoadProgressReport} goOn]} {
     testFail "wtxObjModuleLoadProgressReport-0 failed."
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
     # puts stdout $goOn
     set goOn [lindex $goOn 0]
    }
 
    set modId $goOn
    # puts stdout "Module Id : $modId"


    # Check if module loaded
    set NbModuleLoad2 [llength [wtxObjModuleList]]
    set Check [expr ($NbModuleLoad + 1)]
 
    if { $NbModuleLoad2 != $Check } {
        testFail $errObjMsg(6)
        return 0
    }

    # Now run a function in the loaded module
    # get the task address

    set sym test2
    if {[catch {wtxSymFind -name "$sym"} symInfo]} {
      testFail "wtxSymFind failed."
      return 0
     }

    if {$symInfo == ""} {
     testFail "wtxSymFind failed."
     return 0
    }
 
    set symAdrs [lindex $symInfo 1]
    # Call the test2 task
    wtxFuncCall $symAdrs

    # Loop until call-return event arrive
    set info ""
    while { $info != "CALL_RETURN" } {
     # puts stdout "Waiting CALL_RETURN ..."
     catch "wtxEventGet" infoi
     set info [lindex $infoi 0]
    }

    # task test2 should return 10
    if {[lindex $infoi 2] != 10 } {
      testFail $errObjMsg(7)
      return 0
    }

    # clean up
    set objModuleIdList [wtxObjModuleList]
    foreach id $objModuleIdList {
        if {[catch {wtxObjModuleInfoGet $id} moduleInfo]} {
           testFail "wtxObjModuleInfoGet $id failed."
           return 0
        }
        if {[lindex $moduleInfo 1] == "objSampleWtxtclTest2.o"} {
            cleanUp o $id
        }
    }
 
    file delete $nameTmpFile
    wtxTestPass
 
}

#*****************************************************************************
#
# wtxObjModuleLoadStartTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#
 
proc wtxObjModuleLoadStartTest {tgtsvrName timeout invalidAddr} {
 
    global vObjMsg

    # variables initializations
    set serviceType 2

    # Use target server to load file
    set ts "-ts"
 
    # call valid tests
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    catch {wtxRegisterForEvent .*} st

    wtxObjModuleLoadStartTest1 $ts $tgtsvrName $vObjMsg(135)
    # wtxObjModuleLoadStartTest2 $ts $tgtsvrName $vObjMsg(135)
 
    wtxToolDetachCall "wtxObjModuleLoadStart : "

}
