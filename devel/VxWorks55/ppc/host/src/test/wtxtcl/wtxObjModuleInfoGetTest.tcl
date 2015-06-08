# wtxObjModuleInfoGetTest.tcl - Tcl script, test WTX_OBJ_MODULE_INFO_GET
#
# Copyright 1994-1996 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01v,27may98,p_b  WTX 2.0 new changes
# 01u,14may98,p_b  Replace test?.o by objSampleWtxtclTest?.o
# 01t,28jan98,p_b  Adapted for WTX 2.0
# 01s,11mar97,jmp  modified catch format when a filename is given in
#                  parameters to avoid failures on windows side (SPR #7868).
# 01r,01oct96,jmp  added wtxObjModuleInfoGetTest4 to cover SPR #7133.
# 01q,13mar96,jmp  added wtxObjModuleInfoGetTest3.
# 01p,08nov95,s_w  modify for SPR 5399 fix OBJ_LOADED event no longer received
# 01o,20oct95,jmp  added invalidAddr argument to wtxObjModuleInfoGetTest.
# 01n,09aug95,jmp  changed objGetPath to objPathGet
#		   added timeout to wtxToolAttachCall in wtxObjModuleInfoGetTest
#                  changed tests control method, now uses wtxErrorHandler.
# 01m,23jun95,f_v  added wtxTestRun call
# 01l,21jun95,f_v  clean up main procedure
# 01k,04may95,f_v  take care about new wtxObjModuleInfoGet format
#		   register only OBJ_LOADED event
# 01j,27apr95,c_s  now uses wtxErrorMatch.
# 01i,12apr95,f_v  changed call to formatTypes by objGetPath
# 01h,28mar95,f_v  added wtxRegisterForEvent
# 01g,17mar95,f_v  add lindex to get objModule Id
# 01f,14mar95,f_v  change file path
# 01e,07mar95,f_v  change error message
# 01d,15feb95,f_v  add wtxObjModuleInfoGetTest2
#		   now wtxObjModuleInfoGetTest returns 8 parameters
#		   delete ObjMsg
#		   add format,group and flags tests in wtxObjModuleInfoGetTest1
#		   call objGetPath with test like parameter
#		   replace wtxEventGet by wtxEventPoll through waitEvent
# 01c,06feb95,f_v  put comments in front of ObjMsg
#		   test wtxObjModuleInfoGet by Id
#                  add clean up section
#                  add event management and file exists test
# 01b,27Jan95,f_v  change access messages, use wtxToolDetachCall 
# 01a,17Jan95,f_v  written. 
#
#

#
# DESCRIPTION
#
# WTX_OBJ_MODULE_INFO_GET  - WTX provide information about a loaded module 
#
#
#

#*****************************************************************************
#
# wtxObjModuleInfoGetTest1 -
#
# Verify if wtxObjModuleInfoGetTest provides good information 
#

proc wtxObjModuleInfoGetTest1 {Msg} {

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
    set tModuleId [wtxObjModuleLoad $USE_TSFORLOAD [objPathGet test]objSampleWtxtclTest1.o]
    if {$tModuleId == ""} {
	return
	}

    # get ObjModule Id
    set tModuleId [lindex $tModuleId 0]

    # get the name of module - here test1
    set Info [wtxObjModuleFind $tModuleId]
    if {$Info == ""} {
	return
	}

    set shortInfo [lindex $Info 1]
   
    # get information by name and Id
    set LongInfo [wtxObjModuleInfoGet objSampleWtxtclTest1.o]
    if {$LongInfo == ""} {
	return
	}

    set LongInfo2 [wtxObjModuleInfoGet $tModuleId]
    if {$LongInfo2 == ""} {
	return
	}

    # returns moduleId,moduleName,format,group,loadFlag,nSegments,segment

    # length should be 8
    set length [llength $LongInfo]

    # get moduleId and moduleName
    set Check [lrange $LongInfo 0 1]	

    # call objGetPath procedure to get format types Id
    #set trueFormat [objGetPath test 2]
    set trueFormat [loaderFormatGet]
    set format [lindex $LongInfo 2]

    # get group
    set group [lindex $LongInfo 3]

    # get flags - should be 4 ( LOAD_GLOBAL_SYMBOLS by default )
    set flags [lindex $LongInfo 4]

    # clean up
    cleanUp o $tModuleId

    # Reload objSampleWtxtclTest1.o to test group field
    set tModuleId [wtxObjModuleLoad $USE_TSFORLOAD [objPathGet test]objSampleWtxtclTest1.o]
    if {$tModuleId == ""} {
	return
	}

    # get ObjModule Id
    set tModuleId [lindex $tModuleId 0]

    # get information by name 
    set tmpInfo [wtxObjModuleInfoGet objSampleWtxtclTest1.o]
    if {$tmpInfo == ""} {
	cleanUp o $tModuleId
	return
	}

    # get group
    set group2 [lindex $tmpInfo 3]

    # clean up again
    cleanUp o $tModuleId

    if { $LongInfo != $LongInfo2 || $Info != $Check\
	 || $shortInfo != "objSampleWtxtclTest1.o" || $trueFormat != $format \
	 || $flags != 4 || $group != $group2 } {
	    testFail $errObjMsg(3)
	    return
	}

    wtxTestPass
}


#*****************************************************************************
#
# wtxObjModuleInfoGetTest2 -
#
# Verify if wtxObjModuleInfoGetTest manages bad module name and bad module Id 
#

proc wtxObjModuleInfoGetTest2 {Msg} {

    global errObjMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # verify if objSampleWtxtclTest1.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest1.o 
    if {[file exists $name] == 0 } {
	testFail "File $name is missing for wtxObjModuleInfoGetTest2 test"
	return
	}
    
    # load objSampleWtxtclTest1.o
    if {[catch {wtxObjModuleLoad $USE_TSFORLOAD [objPathGet test]objSampleWtxtclTest1.o} tModuleId]} {
	testFail $tModuleId
	return
	}

    # get ObjModule Id
    set tModuleId [lindex $tModuleId 0]

    # substact 4 to ModuleId for corrumpt it
    set tModuleId2 [expr $tModuleId - 4]
    
    # call wtxObjModuleInfoGet with bad name like parameters

    # if no exception occurred
    if {![catch "wtxObjModuleInfoGet xxzzQ.o" LongInfo]} {
	testFail $errObjMsg(2)
	cleanUp o $tModuleId
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $LongInfo LOADER_OBJ_MODULE_NOT_FOUND]} {
	testFail $LongInfo
	cleanUp o $tModuleId
	return
	}

    # call wtxObjModuleInfoGet with bad Id like parameters

    # if no exception occurred
    if {![catch "wtxObjModuleInfoGet $tModuleId2" LongInfo2]} {
	testFail $errObjMsg(1)
	cleanUp o $tModuleId
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $LongInfo2 LOADER_OBJ_MODULE_NOT_FOUND]} {
	testFail $LongInfo2
	cleanUp o $tModuleId
	return
	}

    # clean up
    cleanUp o $tModuleId

    wtxTestPass
}

#*****************************************************************************
#
# wtxObjModuleInfoGetTest3 - Verify if wtxObjModuleInfoGet provides
#				good segment informations 
#
# When a segment does not exist in the object module, the segment
# information must be {0 0xffffffff 0} (by convention).
# This test load objSampleWtxtclTest5.o and objSampleWtxtclTest2.o. objSampleWtxtclTest5.o has only a data segment and
# objSampleWtxtclTest2.o has only a text segment.
#

proc wtxObjModuleInfoGetTest3 {Msg} {

    global errObjMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # verify if objSampleWtxtclTest5.o and objSampleWtxtclTest2.o exist in right directory
    set nameTest5 [objPathGet test]objSampleWtxtclTest5.o 
    if {[file exists $nameTest5] == 0 } {
	testFail "File $name is missing for wtxObjModuleInfoGetTest3 test"
	return
       }
    
    set nameTest2 [objPathGet test]objSampleWtxtclTest2.o 
    if {[file exists $nameTest2] == 0 } {
	testFail "File $name is missing for wtxObjModuleInfoGetTest3 test"
	return
       }
    
    # load objSampleWtxtclTest5.o
    set test5Id [wtxObjModuleLoad $USE_TSFORLOAD $nameTest5]
    if {$test5Id == ""} {
	return
	}

    # load objSampleWtxtclTest2.o
    set test2Id [wtxObjModuleLoad $USE_TSFORLOAD $nameTest2]
    if {$test2Id == ""} {
	return
	}

    # get Object Modules Ids
    set test5Id [lindex $test5Id 0]
    set test2Id [lindex $test2Id 0]

    # get informations on objSampleWtxtclTest5.o and objSampleWtxtclTest2.o
    set test5Info [wtxObjModuleInfoGet $test5Id]
    if {$test5Info == ""} {
	return
    }

    set test2Info [wtxObjModuleInfoGet $test2Id]
    if {$test2Info == ""} {
	return
    }

    set textSegmentTest5 [lindex $test5Info 5]
    set dataSegmentTest5 [lindex $test5Info 6]
    set bssSegmentTest5  [lindex $test5Info 7]
    set textSegmentTest2 [lindex $test2Info 5]
    set dataSegmentTest2 [lindex $test2Info 6]
    set bssSegmentTest2  [lindex $test2Info 7]


    # This test uses regular expression because the value 0 can be
    # returned decimal or hexadecimal format (0 or 0x0)
    if {![string match "0* 0xffffffff 0*" $textSegmentTest5] || \
	 [string match "0* 0xffffffff 0*" $dataSegmentTest5] || \
	![string match "0* 0xffffffff 0*" $bssSegmentTest5 ] || \
	 [string match "0* 0xffffffff 0*" $textSegmentTest2] || \
	![string match "0* 0xffffffff 0*" $dataSegmentTest2] || \
	![string match "0* 0xffffffff 0*" $bssSegmentTest2 ] } {

	cleanUp o $test5Id $test2Id
	testFail $errObjMsg(19)
	return
    }

    cleanUp o $test5Id $test2Id

    wtxTestPass
}

#*****************************************************************************
#
# wtxObjModuleInfoGetTest4 - Verify the module group number management
#
# This test cover SPR# 7133.
# Here is the description of SPR #7133 solution (by Pad):
#
#   - First step: since it is not very often that a user wants 65534 modules
#     loaded all together, I added a piece of code that resets the group
#     number counter to the value of the highest group value actually available
#     each time a module is loaded. This is very fast, because we just read
#     the group number of the last module in the module list and set the
#     counter with this value plus one. While the counter is lower than
#     65535 we have the following behavior:
#
#          ld a.o     ld b.o    ld c.o      ld d.o    unld c.o   ld e.o
#                                                     unld d.o
#
#            V          V          V          V          V          V
#            V          V          V          V          V          V
#
#          a.o  1     a.o  1     a.o  1     a.o  1     a.o  1     a.o  1
#                     b.o  2     b.o  2     b.o  2     b.o  2     b.o  2
#                                c.o  3     c.o  3                e.o  3
#                                           d.o  4
#
#     You see that the module e.o gets now the group number 3 (instead of 5
#     with the previous algorithm). Of course this works only if the last
#     modules are unloaded sooner or later, but I think this covers 99% of
#     the cases. So what about the remaining 1% ? This is the second step.
#
#
#   - Second step: for whatever reason, the last loaded module reaches the
#     group number 65534. Then, an other piece of code walks through the module
#     list and tries to find a hole in the group numbering. These holes are
#     created by the unloading of previous modules while the last module
#     remained loaded. If no hole is found then there are 65534 modules actually
#     loaded and, well, we are fucked (then an error message is issued, etc...).
#     Whenever a hole is found, the new module is inserted in the module list
#     at this very location and gets the available number as group number. We
#     have then the following behavior:
#
#     ld a.o     ld b.o     ld c.o    ...   ld y.o     ld y.o      ld z.o
#                unld a.o   unld b.o
#
#       V          V          V                V          V           V
#       V          V          V                V          V           V
#
#     a.o  1     <hole>     <hole>    ...   <holes>     y.o  1      y.o  1
#                b.o  2     <hole>    ...   v.o 65532   <holes>     z.o  2
#                           c.o  3    ...   w.o 65533   v.o 65532   <holes>
#                                           x.o 65534   w.o 65533   v.o 65532
#                                                       x.o 65534   w.o 65533
#                                                                   x.o 65534
#

proc wtxObjModuleInfoGetTest4 {Msg} {

    global errObjMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    #
    # verify that the required modules for this test exist in right directory
    #
    set file_a [objPathGet test]objSampleWtxtclTest5.o
    set file_b [objPathGet test]objSampleWtxtclTest3.o
    set file_c [objPathGet test]objSampleWtxtclTest2.o
    set file_d [objPathGet test]objSampleWtxtclTest1.o

    if {[file exists $file_a] == 0 } {
	testFail "File $file_a is missing for wtxObjModuleInfoGetTest4 test"
	return
    }

    if {[file exists $file_b] == 0 } {
	testFail "File $file_b is missing for wtxObjModuleInfoGetTest4 test"
	return
    }

    if {[file exists $file_c] == 0 } {
	testFail "File $file_c is missing for wtxObjModuleInfoGetTest4 test"
	return
    }

    if {[file exists $file_d] == 0 } {
	testFail "File $file_d is missing for wtxObjModuleInfoGetTest4 test"
	return
    }


    #
    # Test First step of the algorithm:
    # ---------------------------------

    #
    # load file_a	>>	file_a  1
    #
    set idFile_a [lindex [wtxObjModuleLoad $USE_TSFORLOAD $file_a] 0]
    if {$idFile_a == ""} {
	return
    }

    # get file_a module group number
    set grpFile_a [lindex [wtxObjModuleInfoGet $idFile_a] 3]


    #
    # load file_b	>>	file_a  1
    #			>>	file_b  2
    #
    set idFile_b [lindex [wtxObjModuleLoad $USE_TSFORLOAD $file_b] 0]
    if {$idFile_b == ""} {
	cleanUp o $idFile_a
	return
    }

    # get file_b module group number
    set grpFile_b [lindex [wtxObjModuleInfoGet $idFile_b] 3]


    #
    # verify module group numbers
    #
    if {$grpFile_b != [expr $grpFile_a + 1]} {
	testFail $errObjMsg(31)
	cleanUp o $idFile_a $idFile_b
	return
    }

    #
    # unload file_b	>>	file_a  1
    # load   file_c	>>	file_c  2
    #
    cleanUp o $idFile_b
    set idFile_c [lindex [wtxObjModuleLoad $USE_TSFORLOAD $file_c] 0]
    if {$idFile_c == ""} {
	cleanUp o $idFile_a
	return
    }

    # get file_c module group number
    set grpFile_c [lindex [wtxObjModuleInfoGet $idFile_c] 3]


    #
    # verify module group numbers
    #
    if {$grpFile_c != [expr $grpFile_a + 1]} {
	testFail $errObjMsg(31)
	cleanUp o $idFile_a $idFile_c
	return
    }

    cleanUp o $idFile_a $idFile_c



    #
    # Test Second step of the algorithm:
    # ----------------------------------

    #
    # load file_a	>>	file_a  1
    #
    set idFile_a [lindex [wtxObjModuleLoad $USE_TSFORLOAD $file_a] 0]
    if {$idFile_a == ""} {
	return
    }

    # get file_a module group number
    set grpFile_a [lindex [wtxObjModuleInfoGet $idFile_a] 3]


    #
    # load file_b	>>	file_a  1
    #			>>	file_b  2
    #
    set idFile_b [lindex [wtxObjModuleLoad $USE_TSFORLOAD $file_b] 0]
    if {$idFile_b == ""} {
	cleanUp o $idFile_a
	return
    }

    # get file_b module group number
    set grpFile_b [lindex [wtxObjModuleInfoGet $idFile_b] 3]


    #
    # verify module group numbers
    #
    if {$grpFile_b != [expr $grpFile_a + 1]} {
	testFail $errObjMsg(31)
	cleanUp o $idFile_a $idFile_b
	return
    }

    #
    # unload file_a	>>	file_b  2
    # load   file_c	>>	file_c  3
    #
    cleanUp o $idFile_a
    set idFile_c [lindex [wtxObjModuleLoad $USE_TSFORLOAD $file_c] 0]
    if {$idFile_c == ""} {
	cleanUp o $idFile_b
	return
    }

    # get file_c module group number
    set grpFile_c [lindex [wtxObjModuleInfoGet $idFile_c] 3]


    #
    # verify module group numbers
    #
    if {$grpFile_c != [expr $grpFile_b + 1]} {
	testFail $errObjMsg(31)
	cleanUp o $idFile_a $idFile_c
	return
    }

    # clean up
    cleanUp o $idFile_b $idFile_c

    wtxTestPass
}

#*****************************************************************************
#
# wtxObjModuleInfoGetTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxObjModuleInfoGetTest {tgtsvrName timeout invalidAddr} {

    global vObjMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxObjModuleInfoGetTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # call valid tests
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    catch {wtxRegisterForEvent ^OBJ_LOADED} st

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    wtxObjModuleInfoGetTest1 $vObjMsg(1)
    wtxObjModuleInfoGetTest3 $vObjMsg(22)
    wtxObjModuleInfoGetTest4 $vObjMsg(32)

    # call invalid tests
    wtxErrorHandler [lindex [wtxHandle] 0] ""
    catch {wtxRegisterForEvent ^OBJ_LOADED} st
    wtxObjModuleInfoGetTest2 $vObjMsg(9)

    # deconnection
    wtxToolDetachCall "wtxObjModuleInfoGetTest"
}
