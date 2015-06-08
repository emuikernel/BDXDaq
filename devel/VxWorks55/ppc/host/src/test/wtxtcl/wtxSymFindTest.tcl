# wtxSymFindTest.tcl - Tcl script, test WTX_SYM_FIND
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01m,28jan98,p_b  Adapted for WTX 2.0
# 01l,20oct95,jmp  added invalidAddr argument to wtxSymFindTest.
# 01k,21aug95,jmp  added timeout to wtxToolAttachCall in wtxSymFindTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01j,23jun95,f_v  added wtxTestRun call
# 01i,21jun95,f_v  clean up main procedure
# 01h,12may95,c_s  updated for new wtxtcl version.
# 01g,04may95,f_v  fix a mistake in wtxSymFindTest2 - now use wtxSymFind
# 01f,10apr95,f_v  change symAddress in wtxSymFindTest1 now is dynamic
# 01e,07mar95,f_v  reduce WTXsymList2 and WTXsymList3 list because their last
#		   field has change
# 01d,06feb95,f_v  add comments in front of SymMsg
#                  add test of wtxSymFind -value
# 01c,02Jan95,f_v  add wtxSymFindTest2
#                  test on WTXsym  symbol rather than printf symbol
# 01b,27Jan95,f_v  change access messages, use wtxToolDetachCall
# 01a,17Jan95,f_v  written. 
#
#

#
# DESCRIPTION
#
# WTX_SYM_FIND  -  Find a symbol table
# 
#         actually option -type isn't managed 
#
#

#*****************************************************************************
#
# wtxSymFindTest1 -
#
# Verify if wtxSymFind find correctly WTXsym in symbol table
#

proc wtxSymFindTest1 {Msg} {

    global SymMsg
    global errSymMsg

    puts stdout "$Msg" nonewline
    
    set name WTXsym[pid]

    # memory alloc to get a uniq symbol address
    set Addrs [wtxMemAlloc 4]
    if {$Addrs == ""} {
	return
	}
    
    # add an own symbol
    set status [wtxSymAdd $name $Addrs 0x5]
    if {$status == ""} {
	return
	}

    # get a list of symbol including WTXsym
    set WTXsymList [wtxSymListGet -name $name]

    # destroy the last parameter for moment because wtxSymFind doesn't
    # managed module name
    set WTXsymList [lrange $WTXsymList 0 2]
    
    if  {$WTXsymList == ""} {
	testFail "ERROR in wtxSymFindTest : symbol not found"
        wtxSymRemove $name 0x5
	cleanUp ms $Addrs
	return
	}
	
    # get the position of exact WTXsym string in the list
    set WTXsymPos [lsearch $WTXsymList $name]
	
    # create a list which contain only info about WTXsym
    set WTXsymList [lrange [lindex $WTXsymList 0] \
	    $WTXsymPos [expr $WTXsymPos+3]]
	
    # get the same result with wtxSymFind
    set WTXsymList2 [lrange [wtxSymFind -exact -name $name] 0 2]
    set WTXsymList3 [lrange [wtxSymFind -exact -value $Addrs] 0 2]

    # clean up
    wtxSymRemove $name 0x5
    cleanUp ms $Addrs

    if { $WTXsymList != $WTXsymList2 || $WTXsymList != $WTXsymList3 } {
	  testFail $errSymMsg(1)
	  return	  
	}

    wtxTestPass

}


#*****************************************************************************
#
# wtxSymFindTest2 -
#
# Verify if wtxSymFind manages non existing symbol well
#

proc wtxSymFindTest2 {Msg} {

    global SymMsg
    global errSymMsg

    puts stdout "$Msg" nonewline

    set name WTXxxx[pid]

    # get a list of symbol including WTXxxx

    # if no exception occurred
    if {![catch "wtxSymFind -name $name" WTXsymList]} {
	testFail $errSymMsg(4)
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $WTXsymList SYMTBL_SYMBOL_NOT_FOUND]} {
	  testFail $WTXsymList
	  return	  
	}

    # if the caught exception is the expected error code
    wtxTestPass
}

#*****************************************************************************
#
# wtxSymFindTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxSymFindTest {tgtsvrName timeout invalidAddr} {

    global vSymMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxSymFindTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # call the valid tests
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler
	
    wtxSymFindTest1 $vSymMsg(1)


    # call the invalid tests
    wtxErrorHandler [lindex [wtxHandle] 0] ""

    wtxSymFindTest2 $vSymMsg(7)

    # deconnection
    wtxToolDetachCall "wtxSymFindTest"
}
