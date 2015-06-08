# wtxSymAddTest.tcl - Tcl script, test WTX_SYM_ADD
#
# Copyright 1994 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01m,28jan98,p_b  Adapted for WTX 2.0
# 01l,28nov96,elp  testing a greater symbol name length (SPR# 7449).
# 01k,26feb96,l_p  corrected English mistakes in a few messages.
# 01j,11nov95,pad  removed temporarily wtxSymAddTest3: it breaks the symbol
#		   table and make the target server to fail.
# 01i,20oct95,jmp  added invalidAddr argument to wtxSymAddTest,
#		   added wtxSymAddTest3.
# 01h,21aug95,jmp  added timeout to wtxToolAttachCall in wtxSymAddTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01g,23jun95,f_v  added wtxTestRun call
# 01f,21jun95,f_v  clean up main procedure
# 01e,17may95,f_v  reduce symbol from 299 to 200 characters
# 01d,16feb95,f_v  add wtxSymAddTest2
# 01c,06feb95,f_v  add comments in front of symMsg
#                  add warning messsage, catch wtxSymAdd
# 01b,27jan95,f_v  change access messages, use wtxToolDetachCall
# 01a,17jan95,f_v  written. 
#
#

#
# DESCRIPTION
#
# WTX_SYM_ADD  -  Add a symbol in the symbol table
#
#
#

#*****************************************************************************
#
# wtxSymAddTest1 -
#
# Verify if wtxSymAdd adds correctly test in vxWorks' symbol table
#

proc wtxSymAddTest1 {Msg} {

    global SymMsg
    global errSymMsg

    puts stdout "$Msg" nonewline

    set name symT[pid]

    # initialise the wtxSymAdd's parameters
    lappend param $name 0x40000 0x5

    # Add symbol symTest to the system symbol table
     set status [wtxSymAdd $name 0x40000 0x5]
     if {$status == ""} {
	return
	}

    # Check if the symbol has been added
    set symInfo [wtxSymFind -exact -name $name]

    # Get the name,value,type of symTest symbol
    set symInfo [lrange $symInfo 0 2]

    # clean up
    set status2 [wtxSymRemove $name 5]
    if { $status2 != 0 } {
          puts stdout "WARNING : symbol has not been removed in wtxSymAddTest1"
	}
    
    # check parameters
    if { $symInfo != $param || $status != 0 } {
	      testFail $errSymMsg(0)
	      return
	}

    wtxTestPass
}


#*****************************************************************************
#
# wtxSymAddTest2 -
#
# Verify if wtxSymAdd adds correctly a 200 characters symbol
# 
# Warning : 200 characters is a maximum
#

proc wtxSymAddTest2 {Msg} {

    global SymMsg
    global errSymMsg

    puts stdout "$Msg" nonewline

    # creation of name with 200 characters
    set name ""
    for { set i 0 } { $i < 500 } { incr i } {
     lappend name x 
     }
    set name [join $name {}]

    # initialise the wtxSymAdd's parameters
    lappend param $name 0x40000 0x5

    # Add symbol to the system symbol table
    set status [wtxSymAdd $name 0x40000 0x5]
    if {$status == ""} {
	return
	}

    # Check if the symbol has been added
    set symInfo [wtxSymFind -exact -name $name]

    # Get the name,value,type of 200 char symbol
    set symInfo [lrange $symInfo 0 2]

    # clean up
    set status2 [wtxSymRemove $name 5]
    if { $status2 != 0 } {
          puts stdout "WARNING : symbol has not been removed in wtxSymAddTest2"
	}

    # check parameters
    if { $symInfo != $param || $status != 0 } {
	      testFail $errSymMsg(8)
	      return
	}

    wtxTestPass
}



#*****************************************************************************
#
# wtxSymAddTest3 - Adds a 400 characters symbol and verify that the
#		   generated event has been truncated to 280 characters
#
# This test adds a 400 characters symbol and verify that it has been
# correctly added, and that the generated event has been truncated to
# 280 characters (TOOL_EVT_SIZE -20).
#

proc wtxSymAddTest3 {Msg} {

    global SymMsg
    global errSymMsg

    puts stdout "$Msg" nonewline

    set resRegist [wtxRegisterForEvent .*]
    if {$resRegist == ""} {
	return
    }

    # creation of name with 400 characters
    set name ""
    for { set i 0 } { $i < 399 } { incr i } {
     lappend name x 
     }
    set name [join $name {}]

    # creation of a comparaison name with 280 characters
    set cmpName ""
    for { set i 0 } { $i < 280 } { incr i } {
     lappend cmpName x 
     }
    set cmpName [join $cmpName {}]

    # initialise the wtxSymAdd's parameters
    lappend param $name 0x40000 0x5

    # Add symbol to the system symbol table
    set status [wtxSymAdd $name 0x40000 0x5]
    if {$status == ""} {
	return
	}

    # loop until the right event arrives
    set event [wtxEventGet]
    if {$event == ""} {
	return
    }

    while { [string compare [lindex $event 0] SYM_ADDED]} {

	set event [wtxEventGet]
	if {$event == ""} {
	    return
	}
    }

    set infoName [lindex $event 1]

    # Check if the symbol has been added
    set symInfo [wtxSymFind -exact -name $name]

    # Get the name,value,type of 200 char symbol
    set symInfo [lrange $symInfo 0 2]

    # clean up
    set status2 [wtxSymRemove $name 5]
    if { $status2 != 0 } {
          puts stdout "WARNING : symbol has not been removed in wtxSymAddTest2"
	}

    # check parameters
    if { $symInfo != $param || $status != 0 || $infoName != $cmpName } {
	      testFail $errSymMsg(15)
	      return
	}

    wtxTestPass
}



#*****************************************************************************
#
# wtxSymAddTest -
#
# Initialization of variables.
# Initialization of variables that contain the description of the tests.
# Call the test scripts.
#

proc wtxSymAddTest {tgtsvrName timeout invalidAddr} {

    global vSymMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxSymAddTest targetServerName\
			timeout invalidAddr"
        return
    }

    # variables initializations
    set serviceType 2

    # connection to the target server
    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler
	
    # call the test procedures
    wtxSymAddTest1 $vSymMsg(0)
    wtxSymAddTest2 $vSymMsg(12)
#    wtxSymAddTest3 $vSymMsg(21)

    # deconnection
    wtxToolDetachCall "wtxSymAddTest"
}
