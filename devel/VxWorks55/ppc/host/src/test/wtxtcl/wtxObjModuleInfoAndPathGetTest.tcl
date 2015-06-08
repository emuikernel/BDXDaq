# wtxObjModuleInfoAndPathGetTest.tcl - Tcl script, test 
# WTX_OBJ_MODULE_INFO_AND_PATH_GET
#
# Copyright 1994-1998 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01a,23sep98,p_b  written.
#

# DESCRIPTION
#
# WTX_OBJ_MODULE_INFO_AND_PATH_GET - WTX provides information about a 
# loaded module, and in particular the module path. 
#
#*****************************************************************************
#
# wtxObjModuleInfoAndPathGetTest1 -
#
# Verify if wtxObjModuleInfoAndPathGetTest provides good information
#

proc wtxObjModuleInfoAndPathGetTest1 {Msg} {

    global errObjMsg
    global USE_TSFORLOAD
 
    puts stdout "$Msg" nonewline
 
    # verify if objSampleWtxtclTest1.o exists in right directory
    set name [objPathGet test]objSampleWtxtclTest1.o
    if {[file exists $name] == 0 } {
        testFail "File $name is missing for wtxObjModuleInfoGetTest1 test"
        return
       }
 
    # load objSampleWtxtclTest1.o
    set tModuleId [wtxObjModuleLoad $USE_TSFORLOAD $name]

    if {$tModuleId == ""} {
        return
        }
 
    # get ObjModule Id
    set tModuleId [lindex $tModuleId 0]
 
    # get the name of module - here test1
    set Info [wtxObjModuleFind $tModuleId]
    if {$Info == ""} {
        return 0
        }
 
    if {[catch {wtxObjModuleInfoAndPathGet $tModuleId} LongInfo ]} {
        testFail "Can not perform a wtxObjModuleInfoAndPathGet"
        return 0
    }

    if {$LongInfo == ""} {
        return 0
    }

    # returns moduleId,moduleName,format,group,loadFlag,nSegments,segment
 
    # length should be 8
    set length [llength $LongInfo]
 
    # get moduleId and moduleName
    set Check [lrange $LongInfo 0 1]
    set completePathName [lindex $LongInfo 1]
 
    # call objGetPath procedure to get format types Id
    set trueFormat [loaderFormatGet]
    set format [lindex $LongInfo 2]
 
    # get group
    set group [lindex $LongInfo 3]
 
    # get flags - should be 4 ( LOAD_GLOBAL_SYMBOLS by default )
    set flags [lindex $LongInfo 4]
 
    # clean up
    cleanUp o $tModuleId
 
    # Reload objSampleWtxtclTest1.o to test group field
    set tModuleId [wtxObjModuleLoad $USE_TSFORLOAD $name]

    if {$tModuleId == ""} {
        return
        }
 
    # get ObjModule Id
    set tModuleId [lindex $tModuleId 0]
 
    # get information by name
    if {[catch {wtxObjModuleInfoGet objSampleWtxtclTest1.o} tmpInfo ]} {
        testFail "Can not perform a wtxObjModuleInfoGet objSampleWtxtclTest1.o"
        return 0
    }

    if {$tmpInfo == ""} {
        cleanUp o $tModuleId
        return 0
        }
 
    # get group
    set group2 [lindex $tmpInfo 3]

    # clean up again
    cleanUp o $tModuleId

    # This split is done to avoid errors related to / or \ selectors
    # (In a path you can find the 2 selectors).

    set completePathName [file split $completePathName]
    set name [file split $name]

    if { $completePathName != "$name" \
         || $trueFormat != $format \
         || $flags != 4 \
         || $group != $group2 } {
      testFail $errObjMsg(3)
      return 0
    }

    wtxTestPass
 
}

#*****************************************************************************
#
# wtxObjModuleInfoAndPathGetTest2 -
#
# Verify if wtxObjModuleInfoAndPathGetTest provides good information
#

proc wtxObjModuleInfoAndPathGetTest2 {Msg} {
    global errObjMsg env
    global USE_TSFORLOAD
 
    puts stdout "$Msg" nonewline

    if {[catch {wtxObjModuleFind vxWorks} resObjFind]} {
        if {[catch {wtxObjModuleFind vxWorks.exe} resObjFind]} {
            if {[catch {wtxObjModuleFind vxworks.exe} resObjFind]} {
                if {[catch {wtxObjModuleFind vxworks} resObjFind]} {
                    testFail "The core file must be called vxWorks, vxworks, vxW
orks.exe or vxworks.exe to perform this test"
                    return 0
                }
            }
        }
    }

    set idVx [lindex $resObjFind 0]

    if {[catch {wtxObjModuleInfoAndPathGet $idVx} LongInfo ]} {
        testFail "Can not perform a wtxObjModuleInfoAndPathGet"
        return 0
    }

    if {$LongInfo == ""} {
        return 0
    }
    set pathVx1 [lindex $LongInfo 1]

    set pathVx2 [lindex [wtxTsInfoGet] 5]

    set splitRes [split $pathVx2 ":"]
    
    if {[string length [lindex $splitRes 0]] == 1 } {
    } else {
        if { [llength $splitRes] == 3 } {
	    
	    set pathVx2 [lindex $splitRes 1]:[lindex $splitRes 2]
	    
	} else {
	    if { [llength $splitRes] == 2 } {
		
		set pathVx2 [lindex $splitRes 1]
		
	    } else {
		
		set pathVx2 $splitRes
	    }
	}
    }
    # This split is done to avoid errors related to / or \ selectors.
    # (In a path you can find the 2 selectors).
 
    set pathVx1 [file split $pathVx1]
    set pathVx2 [file split $pathVx2]

    if { $pathVx1 != $pathVx2} {
        testFail $errObjMsg(3)
        puts stdout "$pathVx1 != $pathVx2"
        return
       }

    wtxTestPass

}

#*****************************************************************************
#
# wtxObjModuleInfoAndPathGetTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxObjModuleInfoAndPathGetTest {tgtsvrName timeout invalidAddr} {

    global vObjMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxObjModuleInfoAndPathGetTest targetServerName\
                                timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # call valid tests
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    catch {wtxRegisterForEvent *} st

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    wtxObjModuleInfoAndPathGetTest1 $vObjMsg(141)

    wtxErrorHandler [lindex [wtxHandle] 0] ""

    wtxObjModuleInfoAndPathGetTest2 $vObjMsg(142)

    # deconnection
    wtxToolDetachCall "wtxObjModuleInfoAndPathGetTest"
}

