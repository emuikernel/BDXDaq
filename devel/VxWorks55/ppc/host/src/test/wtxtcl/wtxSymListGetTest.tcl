# wtxSymListGetTest.tcl - Tcl script, test WTX_SYM_LIST_GET
#
# Copyright 1995-2001 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 02a,18oct01,pad  Adapted to new absolute symbol test code: all target
#                  architectures should work now.
# 01z,15may01,h_k  Fixed symbol type checking and problem with absolute symbol
#                  on ARM(thumb)
# 01y,21apr99,p_b  Fixed problem with absolute symbol on SIMPC architecture.
# 01y,03mar99,p_b  Fixed problem with absolute symbol on ARM architecture.
# 01x,27may98,p_b  WTX 2.0 new changes
# 01w,14may98,p_b  Replace test?.o by objSampleWtxtclTest?.o
# 01v,28mar98,p_b  Adapted for WTX 2.0
# 01u,12mar98,jmb  HPSIM merge, Kent Black patch from 04nov96: Fixed test 8,
#                  failed if pre-existing "symbol*" in sym table
# 01t,06aug97,jmp  Rename symbols symbolxxx with wtxSymListGetTest-symbolxxx
#                  to avoid confusion with 3rd party symbol names (SPR #9080).
# 01s,06aug97,jmp  remove absolute symbol test for SIMSO.
# 01r,15jan97,kkk  remove absolute symbol test for MIPS.
# 01q,30aug96,jmb  Remove absolute symbol test for HP.
# 01p,26jul96,jmp  modified a symbol type in wtxSymListGetTest12.
# 01o,13mar96,jmp  added wtxSymListGetTest12.
# 01n,26feb96,jmp  checked for symbols both with and without an underscore.
#	     +l_p  corrected English mistakes in a few messages.
# 01m,20oct95,jmp  added wtxSymListGetTest9, wtxSymListGetTest10,
#		   wtxSymListGetTest11.
#		   added invalidAddr argument to wtxSymListGetTest.
# 01l,12sep95,jmp  added wtxSymListGetTest8.
# 01k,09aug95,jmp  changed objGetPath to objPathGet, added wtxSymListGetTest7.
#		   added timeout to wtxToolAttachCall in wtxSymListGetTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01j,23jun95,f_v  added wtxTestRun call
# 01i,23jun95,f_v  changed test numbering, fix -module option call
# 01h,24may95,f_v  removed waitEvent in wtxSymListGetTest3 and 4
# 01g,19may95,f_v  added wtxSymListGetTest3 and 4
# 01f,12may95,c_s  updated for new wtxtcl version.
# 01e,10apr95,f_v  change wtxSymListGetTest4 structure 
#		   now symbol address are dynamic
# 01d,17mar95,f_v  change name symbol construction now use PID
# 01d,07mar95,f_v  change value in wtxSymListGetTest2
# 01c,02feb95,f_v  add wtxSymListGetTest3 and 4
#                  manage empty list
#		   put comments in front of SymMsg
# 01b,27jan95,f_v  change access messages, use wtxToolDetachCall
# 01a,17jan95,f_v  written. 
#
#

#
# DESCRIPTION
#
# WTX_SYM_LIST_GET  -  WTX get list of symbol

#*****************************************************************************
#
# wtxSymListGetTest1 -
#
# Verify if wtxSymListGet -name list symbols
#

proc wtxSymListGetTest1 {Msg} {

    global errSymMsg

    puts stdout "$Msg" nonewline

    # create a name which use wtxtcl pid
    set name symTest[pid]

    # alloc a block of memory to get dynamic symbol address
    set AllocAddrs [wtxMemAlloc 4]
    if {$AllocAddrs == ""} {
	return
	}

    # Add symTest symbol
    set addSt [wtxSymAdd $name $AllocAddrs 0x5]
    if {$addSt == ""} {
	return
	}

    # ListGet symbol symTest to the system symbol table
    set infoList [wtxSymListGet -name $name]

    # Get Info
    set sym [lindex $infoList 0]
    set symName [lindex $sym 0]
    set symValue [lindex $sym 1]
    set symType [lindex $sym 2]

    if { $symName != $name || \
	$symValue != $AllocAddrs || $symType != 0x5} {
	    testFail $errSymMsg(2)
            wtxSymRemove $name 5
	    cleanUp ms $AllocAddrs
	    return
	}

    # clean up symbol table
    set status [wtxSymRemove $name 5]
    cleanUp ms $AllocAddrs

    if { $status != 0 } {
       puts stdout "WARNING : symbol has not been removed in wtxSymListGetTest1"
       }

    wtxTestPass
}

#*****************************************************************************
#
# wtxSymListGetTest2 -
#
# Verify if wtxSymListGet -value list symbols
#

proc wtxSymListGetTest2 {Msg} {

    global errSymMsg

    puts stdout "$Msg" nonewline

    set name symTest[pid]

    # alloc a block of memory to get dynamic symbol address
    set AllocAddrs [wtxMemAlloc 4]
    if {$AllocAddrs == ""} {
	return
	}

    # Add symTest symbol
    set addSt [wtxSymAdd $name $AllocAddrs 0x5]
    if {$addSt == ""} {
	return
	}
    
    # ListGet symbol symTest to the system symbol table
    set infoList [wtxSymListGet -value $AllocAddrs]

    set foundSym ""
    foreach sym $infoList {
	if {[lindex $sym 0] == $name} {set foundSym $sym}
    }

    set foundSym [lindex $foundSym 0]

    if { $foundSym != $name } {
	  testFail $errSymMsg(2)
          wtxSymRemove $name 5
	  cleanUp ms $AllocAddrs
	  return
	}

    # clean up symbol table
    set status [wtxSymRemove $name 5]
    cleanUp ms $AllocAddrs

    if { $status != 0 } {
       puts stdout "WARNING : symbol has not been removed in wtxSymListGetTest2"
       }

    wtxTestPass
}



#*****************************************************************************
#
# wtxSymListGetTest3 -
#
# Verify if wtxSymListGet -module option work properly 
#

proc wtxSymListGetTest3 {Msg} {

    global errSymMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # verify if symbol.o exist in right directory
    set name [objPathGet test]symbol.o 
    if { [file exists $name] == 0 } {
          testFail "File $name is missing "
          return
       }

    # load symbol.o file if this module isn't already loaded 
    set tModuleId [wtxObjModuleLoad $USE_TSFORLOAD LOAD_GLOBAL_SYMBOLS\
		[objPathGet test]symbol.o]
    if {$tModuleId == ""} {
	return
	}
	
    # get object module Id
    set tModuleId [lindex $tModuleId 0]

    # ListGet symbol 
    if [catch {wtxSymListGet -module symbol.o} infoList] {
            cleanUp o $tModuleId
            testFail $errSymMsg(12)
            return
    }

    if {$infoList == ""} { 
	    cleanUp o $tModuleId
	    testFail $errSymMsg(12)
	    return
	}


    # clean up 
    cleanUp o $tModuleId

    # Get Information
    set idx1     [lsearch -regexp $infoList i]
    set sym1     [lindex $infoList $idx1]
    set symName1 [lindex $sym1 0]
    set symType1 [lindex $sym1 2]

    set idx2     [lsearch -regexp $infoList symbolTest]
    set sym2     [lindex $infoList $idx2]
    set symName2 [lindex $sym2 0]
    set symType2 [lindex $sym2 2]

    # Compare symName1 with i or _i and with symbolTest or _symbolTest
    set cmp1Sym1 [string match *i $symName1]
    set cmp2Sym1 [string match *symbolTest $symName1]

    # Compare symName2 with i or _i and with symbolTest or _symbolTest
    set cmp1Sym2 [string match *i $symName2]
    set cmp2Sym2 [string match *symbolTest $symName2]

    # Verify that the symbols have been found
    if {![expr $cmp1Sym1 | $cmp2Sym1] || \
	![expr $cmp1Sym2 | $cmp2Sym2]} {
	testFail $errSymMsg(2)
	return
    }

    wtxTestPass
}


#*****************************************************************************
#
# wtxSymListGetTest4 -
#
# Verify if wtxSymListGet -unknown option work properly 
#

proc wtxSymListGetTest4 {Msg} {

    global SymMsg
    global errSymMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # verify if symbol.o exist in right directory
    set name [objPathGet test]symbol.o
    if { [file exists $name] == 0 } {
          testFail "File $name is missing "
          return
       }

    # load symbol.o file if this module isn't already loaded 
    set tModuleId [wtxObjModuleLoad $USE_TSFORLOAD LOAD_GLOBAL_SYMBOLS\
	[objPathGet test]symbol.o]
    if {$tModuleId == ""} {
	testFail $tModuleId
	}
	
    # get object module Id
    set tModuleId [lindex $tModuleId 0]

    # ListGet unknown symbol 
    set infoList [wtxSymListGet -module symbol.o -unknown]
    if {$infoList == ""} {
	    cleanUp o $tModuleId
	    testFail $errSymMsg(12)
	    return
	}

    # clean up 
    cleanUp o $tModuleId

    # Get Information
    set sym1     [lindex $infoList 0]
    set sym2     [lindex $infoList 1]
    set sym3     [lindex $infoList 2]
    set symNbr   [llength $infoList]

    set cmp1 [string match *symbol_unknown1 $sym1]
    set cmp2 [string match *symbol_unknown1 $sym2]
    set cmp3 [string match *symbol_unknown1 $sym3]


    if {![expr $cmp1 | $cmp2 | $cmp3] || $symNbr != 3 } {
	testFail $errSymMsg(2)
	return
    }

    wtxTestPass
}

#*****************************************************************************
#
# wtxSymListGetTest5 -
#
# Verify if wtxSymListGet manage correctly empty list 
#

proc wtxSymListGetTest5 {Msg} {

    global errSymMsg

    puts stdout "$Msg" nonewline

    set name WTXsymxxx
    
    # ListGet symbol symTest to the system symbol table
    set infoList [wtxSymListGet -name $name]

    if { $infoList != "" } {
	  testFail $errSymMsg(5)
	  return
	}

    wtxTestPass
}



#*****************************************************************************
#
# wtxSymListGetTest6 -
#
# Verify if wtxSymListGet -name list symbols with regular expressions
#  few notes about regular expressions
#  -----------------------------------
#      ?  0 or 1  occurrences
#      *  0 or +  occurrences
#      +  1 or +  occurrences
#      ^ begin  $ end
#
#      \{m\}      exactly  m      occurrences
#      \{m,\}     m or +          occurrences
#      \{m,n\}    between m and n occurrences
#
#        A  one-character  RE  followed  by  \{m\},  \{m,\},  or
#       \{m,n\} is an RE that matches a range of occurrences of
#       the one-character RE. The values of m  and  n  must  be
#       nonnegative  integers  less  than  256;  \{m\}  matches
#       exactly  m  occurrences;  \{m,\}  matches  at  least  m
#       occurrences;  \{m,n\} matches any number of occurrences
#       between  m  and  n,  inclusively.   Whenever  a  choice
#       exists, the RE matches as many occurrences as possible.
#
#

proc wtxSymListGetTest6 {Msg} {

    global errSymMsg

    puts stdout "$Msg" nonewline

    set errorMsg "WTX: RPC: Timed out"

    # create names
    set name1 AABBbb1122
    set name2 ABBabb122
    set name3 BBaabb22
    set name4 BBaabb22zz

    # alloc a block of memory to get dynamic symbol address
    set AllocAddrs1 [wtxMemAlloc 4]
    set AllocAddrs2 [wtxMemAlloc 4]
    set AllocAddrs3 [wtxMemAlloc 4]
    set AllocAddrs4 [wtxMemAlloc 4]
	
    # Add symTest symbol
    set addSt1 [wtxSymAdd $name1 $AllocAddrs1 0x5]
    set addSt2 [wtxSymAdd $name2 $AllocAddrs2 0x5]
    set addSt3 [wtxSymAdd $name3 $AllocAddrs3 0x5]
    set addSt4 [wtxSymAdd $name4 $AllocAddrs4 0x5]

    # ListGet symbol symTest to the system symbol table

    # this instruction should return name1 and name2
    set infoList [wtxSymListGet -name \[A-Z\]{3,}a?bb\[0-9\]{2,}]

    if { $infoList == $errorMsg } {
	testFail $errorMsg
        wtxSymRemove $name1 0x5
        wtxSymRemove $name2 0x5
        wtxSymRemove $name3 0x5
        wtxSymRemove $name4 0x5
	cleanUp ms $freeSt1 $freeSt2 $freeSt3 $freeSt4
	return

    } else {

	set test1 [lsearch -regexp $infoList "^$name1 "]
	set test2 [lsearch -regexp $infoList "^$name2 "]
	set test3 [lsearch -regexp $infoList "^$name3 "]
	set test4 [lsearch -regexp $infoList "^$name4 "]

	if { $test1 < 0 || $test2 < 0 || $test3 > 0 || $test4 > 0 } {
	      testFail $errSymMsg(6)
	      wtxSymRemove $name1 0x5
	      wtxSymRemove $name2 0x5
	      wtxSymRemove $name3 0x5
	      wtxSymRemove $name4 0x5
	      cleanUp ms $AllocAddrs1 $AllocAddrs2 $AllocAddrs3 $AllocAddrs4
	      return
	}
    }

    # this instruction should return name4
    set infoList [wtxSymListGet -name ^B.*z$ ]

    if { $infoList == $errorMsg } {
	testFail $errorMsg
	wtxSymRemove $name1 0x5
	wtxSymRemove $name2 0x5
	wtxSymRemove $name3 0x5
	wtxSymRemove $name4 0x5
	cleanUp ms $AllocAddrs1 $AllocAddrs2 $AllocAddrs3 $AllocAddrs4
	return

    } else {

	set test1 [lsearch -regexp $infoList "^$name1 "]
	set test2 [lsearch -regexp $infoList "^$name2 "]
	set test3 [lsearch -regexp $infoList "^$name3 "]
	set test4 [lsearch -regexp $infoList "^$name4 "]

	if { $test1 > 0 || $test2 > 0 || $test3 > 0 || $test4 < 0 } {
	      testFail $errSymMsg(7)
	      wtxSymRemove $name1 0x5
	      wtxSymRemove $name2 0x5
	      wtxSymRemove $name3 0x5
	      wtxSymRemove $name4 0x5
	      cleanUp ms $AllocAddrs1 $AllocAddrs2 $AllocAddrs3 $AllocAddrs4
              return
	}
    }
    # clean up symbol table
    set status1 [wtxSymRemove $name1 0x5]
    set status2 [wtxSymRemove $name2 0x5]
    set status3 [wtxSymRemove $name3 0x5]
    set status4 [wtxSymRemove $name4 0x5]
    cleanUp ms $AllocAddrs1 $AllocAddrs2 $AllocAddrs3 $AllocAddrs4

    if { $status1 != 0 || $status2 != 0 || $status3 != 0 || $status4 != 0} {
       puts stdout "WARNING : symbol has not been removed in wtxSymListGetTest4"
       }

    wtxTestPass
}

##########################################################################
#
# wtxSymListGetTest7 -
#
# This test load a file that contains 1050 symbols :
# symbolTest1 = 1  to  symbolTest1050 = 1050
#
# wtxSymListGetTest7 verifies that wtxSymListGet find all these
# symbols and tests if their values are correctly got.
#

proc wtxSymListGetTest7 {Msg} {

    global errSymMsg
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # verify if objSampleWtxtclTest5.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest5.o
    if {[file exists $name] == 0 } {
	testFail "File $name is missing for wtxObjModuleLoadTest7 test"
	return
	}

    set tModuleId [wtxObjModuleLoad $USE_TSFORLOAD\                                                 LOAD_GLOBAL_SYMBOLS [objPathGet test]objSampleWtxtclTest5.o]
    if {$tModuleId == ""} {
	return
	}
    set tModuleId [lindex $tModuleId 0]

    set infoList [wtxSymListGet -module objSampleWtxtclTest5.o]

    if {[llength $infoList] != 1050} {
	testFail $errSymMsg(14) 
	cleanUp o $tModuleId
	return
	}

    foreach symbolInfo $infoList {
	
	set symbolName [lindex $symbolInfo 0] ;# symbolTest...
	set symbolAddr [lindex $symbolInfo 1]

	# Get the symbol number
	set expectedValue1 0
	set expectedValue2 0

	if {[string index $symbolName 0] == "_"} {
	    # The symbol name begin by an underscore
	    set expectedValue1 [string range $symbolName 11 end]

	} else {
	    set expectedValue2 [string range $symbolName 10 end]
	}

	set symValue [wtxGopherEval -errvar errCode "$symbolAddr *!"]

	# The symbol value should be equal to the symbol number
	if {$symValue != $expectedValue1 && $symValue != $expectedValue2} {
	    testFail $errSymMsg(13)
	    cleanUp o $tModuleId
	    return
	}
    }


    # clean up
    cleanUp o $tModuleId
    wtxTestPass
}



##########################################################################
#
# wtxSymListGetTest8 - Verify that wtxSymListGet manages situations where
#	the number of symbol frames is a multiple of the symbol frame size
#
# This test allocates 260 symbols (symbol1 to symbol260), and removes
# symbol1 to symbol11 (260 symbols to 250 symbols), and verifies that
# wtxSymListGet works correctly in each of these cases.
#
# REMARK : The symbol frame size is defined in tswtx.c : TS_WTX_TOOL_MAX=255
#

proc wtxSymListGetTest8 {Msg} {

    global errSymMsg

    puts stdout "$Msg" nonewline

    # account for any "symbol*" that already exists
    set origLen [llength [wtxSymListGet -name symbol]]


    # add 260 symbols
    for {set x 1} {$x <= 260} {incr x 1} {
	
	# allocate memory for symbolx
	set addr($x) [wtxMemAlloc 4]
	if {$addr($x) == ""} {
		# clean up
		for {set i 1} {$i < $x} {incr i 1} {
			wtxSymRemove wtxSymListGetTest-symbol$i 0x5
			cleanUp ms $addr($i)
			}
		return
		}

	# add symbolx
	set st [wtxSymAdd wtxSymListGetTest-symbol$x $addr($x) 0x5]
	if {$st == ""} {
		# clean up
		cleanUp ms $addr($x)
		for {set i 1} {$i < $x} {incr i 1} {
			wtxSymRemove wtxSymListGetTest-symbol$i 0x5
			cleanUp ms $addr($i)
			}
		return
		}
	}


    for {set y 1} {$y <= 11} {incr y 1} {
	
	# get symbols list
	set symbolList [wtxSymListGet -name symbol]

	# get the number of symbols of the list
	set lengthList [llength $symbolList]
	
	set lengthList [expr $lengthList - $origLen]

	# verify if the number of symbols is correct
	if {$lengthList != [expr $x - $y]} {
		testFail $errSymMsg(14)
		
		#clean up
		for {set i $y} {$i < $x} {incr i 1} {
			wtxSymRemove wtxSymListGetTest-symbol$i 0x5
			cleanUp ms $addr($i)
			}
		return
		}

	# remove a symbol
	set st [wtxSymRemove wtxSymListGetTest-symbol$y 0x5]
	if {$st == ""} {
		# clean up
		for {set i [expr $y + 1]} {$i <= $x} {incr i 1} {
			wtxSymRemove wtxSymListGetTest-symbol$i 0x5
			cleanUp ms $addr($i)
			}
		return
		}

	# free memory used by the removed symbol
	cleanUp ms $addr($y)
	}


    # clean up
    for {set i 12} {$i <= 260} {incr i 1} {
	wtxSymRemove wtxSymListGetTest-symbol$i 0x5
	cleanUp ms $addr($i)
	}


    wtxTestPass
}



##########################################################################
#
# wtxSymListGetTest9 - wtxSymListGet with -module option and a
#		non-existent module
#

proc wtxSymListGetTest9 {Msg} {

    global errSymMsg

    puts stdout "$Msg" nonewline

    # if no exception occurred
    if {![catch "wtxSymListGet -module qwertyuiopasdfghjkl" status]} {
	testFail $errSymMsg(10)
	return
    }

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $status SYMTBL_NO_SUCH_MODULE]} {
	testFail $status
	return
    }

    # if the caught exception is the expected error code

    wtxTestPass
}


##########################################################################
#
# wtxSymListGetTest10 - wtxSymListGet with -name option and
#		and a non-existent string
#

proc wtxSymListGetTest10 {Msg} {

    global errSymMsg

    puts stdout "$Msg" nonewline

    # wtxSymListGet should return an empty list
    set list [wtxSymListGet -name qwertyuiopasdfghjkl]

    if {$list != ""} {
	testFail $errSymMsg(4)
	return
	}

    wtxTestPass
}



##########################################################################
#
# wtxSymListGetTest11 - wtxSymListGet with -module and -name options
#		with non-existent strings
#

proc wtxSymListGetTest11 {Msg} {

    global errSymMsg

    puts stdout "$Msg" nonewline

    # if no exception occurred
    if {![catch "wtxSymListGet -name qwertyuiopasdfghjkl \
			-module qwertyuiopasdfghjkl" status]} {
	testFail $errSymMsg(10)
	return
    }

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $status SYMTBL_NO_SUCH_MODULE]} {
	testFail $status
	return
    }

    # if the caught exception is the expected error code

    wtxTestPass
}


##########################################################################
#
# wtxSymListGetTest12 - Verify if wtxSymListGet gets correct symbol types
#
# This test load an object file with ALL_SYMBOLS option,
# and verify that wtxSymListGet returns the correct types
# for the object file symbols.
#

proc wtxSymListGetTest12 {Msg} {

    global errSymMsg
    global cpuFamily
    global USE_TSFORLOAD

    puts stdout "$Msg" nonewline

    # verify if objSampleWtxtclTest6.o exist in right directory
    set name [objPathGet test]objSampleWtxtclTest6.o
    if {[file exists $name] == 0 } {
	testFail "File $name is missing for wtxObjModuleLoadTest12 test"
	return
    }

    set moduleId [wtxObjModuleLoad $USE_TSFORLOAD\
			LOAD_ALL_SYMBOLS [objPathGet test]objSampleWtxtclTest6.o]
    if {$moduleId == ""} {
	return
    }

    set moduleId [lindex $moduleId 0]

    wtxErrorHandler [lindex [wtxHandle] 0] ""

    if [catch {wtxSymListGet -name staticVoidFunc} staticVoidFuncInfo] {
	testFail $staticVoidFuncInfo
	return
    }
    if {$staticVoidFuncInfo == ""} {
	testFail $errSymMsg(17)
	cleanUp o $moduleId
	return
    }

    set staticVoidFuncType [lindex [lindex $staticVoidFuncInfo 0] 2]

    if [catch {wtxSymListGet -name voidFunc} voidFuncInfo] {
	testFail $voidFuncInfo
	return
    }
    if {$voidFuncInfo == ""} {
	testFail $errSymMsg(17)
	cleanUp o $moduleId
	return
    }

    set voidFuncType [lindex [lindex $voidFuncInfo 0] 2]

    if [catch {wtxSymListGet -name staticIntVarInit} staticIntVarInitInfo] {
	testFail $staticIntVarInitInfo
	return
    }
    if {$staticIntVarInitInfo == ""} {
	testFail $errSymMsg(17)
	cleanUp o $moduleId
	return
    }


    set staticIntVarInitType [lindex [lindex $staticIntVarInitInfo 0] 2]


    if [catch {wtxSymListGet -name varIntInit} varIntInitInfo] {
	testFail $varIntInitInfo
	return
    }
    if {$varIntInitInfo == ""} {
	testFail $errSymMsg(17)
	cleanUp o $moduleId
	return
    }

    set varIntInitType [lindex [lindex $varIntInitInfo 0] 2]

    if [catch {wtxSymListGet -name staticGlobalVariable} staticGlobalVariableInfo] {
	testFail $staticGlobalVariableInfo
	return
    }
    if {$staticGlobalVariableInfo == ""} {
	testFail $errSymMsg(17)
	cleanUp o $moduleId
	return
    }

    set staticGlobalVariableType [lindex [lindex $staticGlobalVariableInfo 0] 2]

    if [catch {wtxSymListGet -name varIntNoInit} varIntNoInitInfo] {
	testFail $varIntNoInitInfo
	return
    }
    if {$varIntNoInitInfo == ""} {
	testFail $errSymMsg(17)
	cleanUp o $moduleId
	return
    }


    set varIntNoInitType [lindex [lindex $varIntNoInitInfo 0] 2]

    set nbCPU [lindex [lindex [wtxTsInfoGet] 2 ] 0]
    set cpuFam $cpuFamily($nbCPU)

    if {$cpuFam == "thumb"} {
	if {$staticVoidFuncType	!= 0x44 || \
	    $voidFuncType	!= 0x45 || \
	    $staticIntVarInitType != 0x6 || \
	    $varIntInitType	!= 0x7 || \
	    $staticGlobalVariableType != 0x8 || \
	    $varIntNoInitType	!= 0x19 } {

	    testFail $errSymMsg(16)
	    cleanUp o $moduleId
	    return
	}
    } else {
	if {$staticVoidFuncType	!= 0x4 || \
	    $voidFuncType	!= 0x5 || \
	    $staticIntVarInitType != 0x6 || \
	    $varIntInitType	!= 0x7 || \
	    $staticGlobalVariableType != 0x8 || \
	    $varIntNoInitType	!= 0x19 } {

	    testFail $errSymMsg(16)
	    cleanUp o $moduleId
	    return
	}
    }

    cleanUp o $moduleId

    # The following test should now be able to run on all architectures.
    # If however an architecture proves to fail the test, its name could
    # replace "ok_for_all". Other architecture names can be added if needed.

    if {![string match "*$cpuFam*" " ok_for_all "]} {

	# check if absolute.o exist in right directory
	set name [objPathGet test]absolute.o
	if {[file exists $name] == 0 } {
	    testFail "File $name is missing for wtxObjModuleLoadTest12 test"
	    return
	}

	set moduleId [wtxObjModuleLoad $USE_TSFORLOAD LOAD_GLOBAL_SYMBOLS $name]
	if {$moduleId == ""} {
	    return
	}

	set moduleId [lindex $moduleId 0]

        set absoluteSymbolInfo ""

	set absoluteSymbolInfo [wtxSymListGet -name absoluteSymbol]

	if {$absoluteSymbolInfo == ""} {
            testFail "\twtxSymListGet -name absoluteSymbol has failed ($cpuFam)\n"
	    cleanUp o $moduleId
	    return
	}

	set absoluteSymbolType [lindex [lindex $absoluteSymbolInfo 0] 2]

	if {($absoluteSymbolType & 0x2) != 0x2} {
	    testFail $errSymMsg(18)
	    cleanUp o $moduleId
	    return
	}

	cleanUp o $moduleId
    }

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    wtxTestPass
}

#*****************************************************************************
#
# wtxSymListGetTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxSymListGetTest {tgtsvrName timeout invalidAddr} {

    global vSymMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxSymListGetTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # call the test procedures

    wtxSymListGetTest1 $vSymMsg(2)
    wtxSymListGetTest2 $vSymMsg(3)
    wtxSymListGetTest3 $vSymMsg(14)
    wtxSymListGetTest4 $vSymMsg(15)
    wtxSymListGetTest5 $vSymMsg(10)
    wtxSymListGetTest6 $vSymMsg(11)
    # wtxSymListGetTest7 $vSymMsg(16)
    wtxSymListGetTest8 $vSymMsg(17)
    wtxSymListGetTest12 $vSymMsg(22)

    wtxErrorHandler [lindex [wtxHandle] 0] ""

    wtxSymListGetTest9 $vSymMsg(18)
    wtxSymListGetTest10 $vSymMsg(19)
    wtxSymListGetTest11 $vSymMsg(20)
		
    # deconnection
    wtxToolDetachCall "wtxSymListGetTest"
}
