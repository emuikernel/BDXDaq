# wtxObjModuleChecksumTest.tcl - Tcl script, test WTX_OBJ_MODULE_CHECKSUM
#
# Copyright 1998 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01d,05jun98,pcn  Removed wtxObjModuleChecksum3.
# 01c,27may98,p_b  WTX 2.0 new changes
# 01b,14may98,p_b  Replace test?.o by objSampleWtxtclTest?.o
# 01a,28jan98,p_b  written
#

#
# DESCRIPTION
#
# WTX_OBJ_MODULE_CHECKSUM - Test program for wtxObjModuleChecksum
#
#



#*****************************************************************************
#
# wtxObjModuleChecksumTest1 - Test with "filename" as parameter
#
#

proc wtxObjModuleChecksumTest1 {Msg} {

    global USE_TSFORLOAD

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
        testFail "File $name is missing for wtxObjModuleChecksum test"
        return 0
    }
 
    # load objSampleWtxtclTest6.o file and give a idea of loading time
    set timeMsr [time {set tModuleId [wtxObjModuleLoad $USE_TSFORLOAD\
                LOAD_GLOBAL_SYMBOLS [objPathGet test]objSampleWtxtclTest6.o]} 1]
    if {$tModuleId == ""} {
        return 0
    }

    set result [wtxObjModuleChecksum [objPathGet test]objSampleWtxtclTest6.o]
    if {$result == ""} {
	return 0
    }

    wtxTestPass
}

#*****************************************************************************
#
# wtxObjModuleChecksumTest2 - Test with ID as parameter
#
#
 
proc wtxObjModuleChecksumTest2 {Msg} {
 
    global USE_TSFORLOAD

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
        testFail "File $name is missing for wtxObjModuleChecksum test"
        return 0
    }
 
    # load objSampleWtxtclTest6.o file and give a idea of loading time
    set timeMsr [time {set tModuleId [wtxObjModuleLoad\
         $USE_TSFORLOAD LOAD_GLOBAL_SYMBOLS [objPathGet test]objSampleWtxtclTest6.o]} 1]
    if {$tModuleId == ""} {
        return 0
    }
 
    set result [wtxObjModuleChecksum [lindex $tModuleId 0]]
    if {$result == ""} {
        return 0
    }
    wtxTestPass
}

#*****************************************************************************
#
# wtxObjModuleChecksumTest3 - Test with "1 as parameter
#
#

proc wtxObjModuleChecksumTest3 {Msg} {
 
    global USE_TSFORLOAD
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
        testFail "File $name is missing for wtxObjModuleChecksum test"
        return 0
    }
 
    # load objSampleWtxtclTest6.o file and give a idea of loading time
    set timeMsr [time {set tModuleId [wtxObjModuleLoad\
        $USE_TSFORLOAD LOAD_GLOBAL_SYMBOLS [objPathGet test]objSampleWtxtclTest6.o]} 1]
    if {$tModuleId == ""} {
        return 0
    }
 
    set result [wtxObjModuleChecksum 1]
    if {$result == ""} {
        return 0
    }
    wtxTestPass
}

#*****************************************************************************
#
# wtxObjModuleChecksumTest4 - Test with WTX_ALL_MODULE_CHECK as parameter
#
#

proc wtxObjModuleChecksumTest4 {Msg} {
 
    global USE_TSFORLOAD
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
        testFail "File $name is missing for wtxObjModuleChecksum test"
        return 0
    }
 
    # load objSampleWtxtclTest6.o file and give a idea of loading time
    set timeMsr [time {set tModuleId [wtxObjModuleLoad\
          $USE_TSFORLOAD LOAD_GLOBAL_SYMBOLS [objPathGet test]objSampleWtxtclTest6.o]} 1]
    if {$tModuleId == ""} {
        return 0
    }
 
    set result [wtxObjModuleChecksum WTX_ALL_MODULE_CHECK]

    if {$result == ""} {
        return 0
    }
    wtxTestPass
}


#*****************************************************************************
#
# wtxObjModuleChecksumTest5 - 	Boundary Test
# 				Module not loaded
#
 
proc wtxObjModuleChecksumTest5 {Msg} {
    puts stdout "$Msg" nonewline
 
    set objModuleIdList [wtxObjModuleList]
 
    # Unload all "objSampleWtxtclTest6.o" modules
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
 
    # verify if objSampleWtxtclTest6.o exists in the right directory
    set name [objPathGet test]objSampleWtxtclTest6.o

    if { [file exists $name] == 0 } {
        testFail "File $name is missing for wtxObjModuleChecksum test"
        return 0
    }
 
   # Run wtxObjModuleChecksum on objSampleWtxtclTest6.o
   if ![catch {wtxObjModuleChecksum [objPathGet test]objSampleWtxtclTest6.o}] {
	testFail "wtxObjModuleChecksum can not manage bad parameter ..."
	return 0
   }

   wtxTestPass
}

#*****************************************************************************
#
# wtxObjModuleChecksumTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
# Furthermore it attaches itself to the target server (thru a wtxToolAttachCall
# command) and, when testcase units have been run, detaches itself from the
# target server.
#

proc wtxObjModuleChecksumTest {tgtsvrName timeout invalidAddr} {

    global checksumMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxObjModuleChecksumTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # call the testcase units
    wtxObjModuleChecksumTest1 $checksumMsg(1)
    wtxObjModuleChecksumTest2 $checksumMsg(2)
    #wtxObjModuleChecksumTest3 $checksumMsg(3)
    wtxObjModuleChecksumTest4 $checksumMsg(4)

    wtxErrorHandler [lindex [wtxHandle] 0] ""
 
    # call invalid tests

    wtxObjModuleChecksumTest5 $checksumMsg(5)

    # detach from the target server
    wtxToolDetachCall "\nwtxObjModuleChecksumTest : "
}
