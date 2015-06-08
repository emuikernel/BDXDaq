# wtxSymRemoveTest.tcl - Tcl script, test WTX_SYM_REMOVE
#
# Copyright 1994 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01k,28jan98,p_b  Adapted for WTX 2.0
# 01j,20oct95,jmp  added invalidAddr argument to wtxSymRemoveTest.
# 01i,21aug95,jmp  added timeout to wtxToolAttachCall in wtxSymRemoveTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01h,23jun95,f_v  added wtxTestRun call
# 01g,21jun95,f_v  clean up main procedure
# 01f,12may95,c_s  upgraded for new wtxtcl version.
# 01e,27apr95,c_s  now uses wtxErrorMatch.
# 01d,07mar95,f_v  change error messages
# 01c,02feb95,f_v  manage WTXerror codes
#                  add wtxSymRemoveTest2, comments in front of SymMsg
# 01b,27jan95,f_v  change access messages, use wtxToolDetachCall
# 01a,17jan95,f_v  written. 
#
#

#
# DESCRIPTION
#
# WTX_SYM_REMOVE  -  Remove a symbol from the symbol table
#
#
#

#*****************************************************************************
#
# wtxSymRemoveTest1 -
#
# Verify if wtxSymRemove removes correctly symTest 
#

proc wtxSymRemoveTest1 {Msg} {

    global errSymMsg

    puts stdout "$Msg" nonewline

    # Add symbol symTest to the system symbol table
    set status [wtxSymAdd symTest 0x40000 0x5]
    if {$status == ""} {
	return
	}

    set symTestNb [llength [wtxSymListGet -name symTest]] 

    if { $symTestNb > 0 } {
	set status [wtxSymRemove symTest 0x5]
	}

    set symTestNb2 [llength [wtxSymListGet -name symTest]] 

    if { $symTestNb != $symTestNb2+1 || $status != 0 } {
	  testFail $errSymMsg(3)
	  return
	}

    wtxTestPass
}


#*****************************************************************************
#
# wtxSymRemoveTest2 -
#
# Verify if wtxSymRemove manages bad symbol name well
#

proc wtxSymRemoveTest2 {Msg} {

    global errSymMsg

    puts stdout "$Msg" nonewline

    set name WTXsymxxx 
    
    # if no exception occurred
    if {![catch "wtxSymRemove $name 0x5" status]} {
	testFail $errSymMsg(4)
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $status SYMTBL_SYMBOL_NOT_FOUND] } {
	testFail $status
	return
	}

    # if the caught exception is the expected error code
    wtxTestPass
}


#*****************************************************************************
#
# wtxSymRemoveTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxSymRemoveTest {tgtsvrName timeout invalidAddr} {

    global vSymMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxSymRemoveTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # call valid tests
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler
	
    wtxSymRemoveTest1 $vSymMsg(5)


    # call invalid tests
    wtxErrorHandler [lindex [wtxHandle] 0] ""
    wtxSymRemoveTest2 $vSymMsg(9)

    # deconnection
    wtxToolDetachCall "wtxSymRemoveTest"
}
