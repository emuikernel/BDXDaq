# wtxSymTblInfoGetTest.tcl - Tcl script, test WTX_SYM_TBL_INFO_GET
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01g,28jan98,p_b  Adapted for WTX 2.0
# 01f,26feb96,l_p  corrected English mistakes in a few messages.
# 01e,20oct95,jmp  added invalidAddr argument to wtxSymTblInfoGetTest.
# 01d,21aug95,jmp  added timeout to wtxToolAttachCall in wtxSymTblInfoGetTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01c,23jun95,f_v  added wtxTestRun call
# 01b,21jun95,f_v  clean up main procedure
# 01a,01mar95,f_v  written. 
#
#

#
# DESCRIPTION
#
# WTX_SYM_TBL_INFO_GET  -  WTX get information about a symbol table
#
#
#

#*****************************************************************************
#
# wtxSymTblInfoGetTest1 -
#
# Verify if wtxSymTblInfoGet returns SymTable Id, number of symbols in table,
# and same name.
# This test uses wtxSymTblInfoGet without parameter, so it's the default symbol
# table which it uses.
# 
#

proc wtxSymTblInfoGetTest1 {Msg} {

    global SymMsg
    global errSymMsg

    puts stdout "$Msg" nonewline

    set name symT[pid]

    # get information about symbol table
    set tableInfo1 [wtxSymTblInfoGet]

    # Add symbol symTest to the system symbol table
    set status [wtxSymAdd $name 0x40000 0x5]
    if {$status == ""} {
	return
    }

    # get information about symbol table
    set tableInfo2 [wtxSymTblInfoGet]

    # get number of symbols in table
    set nbrSymb1 [lindex $tableInfo1 1]
    set nbrSymb1 [expr $nbrSymb1+1]
    set nbrSymb2 [lindex $tableInfo2 1]

    # get the length of wtxSymTblInfoGet result - it should be 3
    set leng [llength $tableInfo1]

    # get clash name flag it should be 1 (OK)
    set flag [lindex $tableInfo1 2]

    # clean up
    set status2 [wtxSymRemove $name 5]
    if { $status2 != 0 } {
          puts stdout "WARNING : symbol has not been removed in wtxSymTblInfoGetTest1"
	}
    
    # check parameters
    if {$nbrSymb1 != $nbrSymb2 || $leng != 3 || $flag != 1} {
	      testFail $errSymMsg(9)
	      return
	}

    wtxTestPass
}


#*****************************************************************************
#
# wtxSymTblInfoGetTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxSymTblInfoGetTest {tgtsvrName timeout invalidAddr} {

    global vSymMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxSymTblInfoGetTest targetServerName\
				timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # call the test procedures
    wtxSymTblInfoGetTest1 $vSymMsg(13)

    # deconnection
    wtxToolDetachCall "wtxSymTblInfoGetTest"
}
