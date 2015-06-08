# wtxObjModuleLoadProgressReportTest.tcl -
# Test WTX_OBJ_MODULE_LOAD_PROGRESS_REPORT
#
# Copyright 1994-1998 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01b,20nov01,jmp  fixed test to avoid looping indefinitely on test failure.
# 01a,24jun98,p_b written.
#
 
# DESCRIPTION
#
# WTX_OBJ_MODULE_LOAD_PROGRESS_REPORT  -  Get a report of a load peformed with
#                                         wtxObjModuleLoadStart.
#
#
#
 
#*****************************************************************************
#
# wtxObjModuleLoadProgressReportTest1 - Local load of a file, run a function and
#                              check the result.
#
#
 
proc wtxObjModuleLoadProgressReportTest1 {ts tgtsvrName Msg} {
 
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
 
    # verify whether objSampleWtxtclTest2.o exists in the right directory
    set name [objPathGet test]objSampleWtxtclTest2.o
    if { [file exists $name] == 0 } {
        testFail "File $name is missing for\
                    wtxObjModuleLoadProgressReportTest1 test"
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

    if {[catch {wtxObjModuleLoadProgressReport} goOn ]} {
     testFail "wtxObjModuleLoadProgressReport failed."
     return 0
    }
    # puts stdout $goOn
    set goOn [lindex $goOn 0]
 
    # Wait until the module is loaded
    while { ![string match 0x* $goOn] } {
     if {[catch {wtxObjModuleLoadProgressReport} goOn ]} {
      testFail "wtxObjModuleLoadProgressReport failed."
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
    set symInfo [wtxSymFind -name "$sym"]
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
    wtxObjModuleUnload $name
 
    file delete $nameTmpFile
    wtxTestPass

}
 
 
#*****************************************************************************
#
# wtxObjModuleLoadProgressReportTest2 - test wtxObjModuleLoadProgressReport
#                                       without starting a load.
#
#
 
proc wtxObjModuleLoadProgressReportTest2 {ts tgtsvrName Msg} {
 
    global errObjMsg
    global env
 
    puts stdout "$Msg" nonewline
    catch { wtxObjModuleLoadProgressReport } goOn
    if [string match *SVR_EINVAL* $goOn] {
    } else {
     testFail "Bad returned error"
     return 1
    }
    wtxTestPass
}

#*****************************************************************************
#
# wtxObjModuleLoadProgressReportTest3 - Local load of a file,
#                              
#
#
 
proc wtxObjModuleLoadProgressReportTest3 {ts tgtsvrName Msg} {
 
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
 
    # verify whether objSampleWtxtclTest2.o exists in the right directory
    set name [objPathGet test]objSampleWtxtclTest2.o
    if { [file exists $name] == 0 } {
        testFail "File $name is missing for\
                wtxObjModuleLoadProgressReportTest1 test"
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

    if {[catch {wtxObjModuleLoadProgressReport} goOn ]} {
     testFail "wtxObjModuleLoadProgressReport failed."
    }
    # puts stdout $goOn
    set goOn [lindex $goOn 0]
 
    # Wait until the module is loaded
    while { ![string match 0x* $goOn] } {
     if {[catch {wtxObjModuleLoadProgressReport} goOn ]} {
        testFail "wtxObjModuleLoadProgressReport failed."
        return 0
     }
     # puts stdout $goOn
     set goOn [lindex $goOn 0]
    }
 
    catch { wtxObjModuleLoadProgressReport } goOn
    if [string match *SVR_EINVAL* $goOn] {
    } else {
     testFail "Bad returned error"
     return 1
    }
 
    # clean up
    wtxObjModuleUnload $name
 
    file delete $nameTmpFile
    wtxTestPass
 
}

#*****************************************************************************
#
# wtxObjModuleLoadProgressReportTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#
 
proc wtxObjModuleLoadProgressReportTest {tgtsvrName timeout invalidAddr} {
 
    global vObjMsg
 
    # variables initializations
    set serviceType 2
    # Use target server to load file
    set ts "-ts"
 
    # call valid tests
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    catch {wtxRegisterForEvent .*} st
 
    wtxObjModuleLoadProgressReportTest1 $ts $tgtsvrName $vObjMsg(136)

    wtxErrorHandler [lindex [wtxHandle] 0] ""

    wtxObjModuleLoadProgressReportTest2 $ts $tgtsvrName $vObjMsg(139)
    wtxObjModuleLoadProgressReportTest3 $ts $tgtsvrName $vObjMsg(140)
 
    wtxToolDetachCall "wtxObjModuleLoadProgressReport : "
 
}

