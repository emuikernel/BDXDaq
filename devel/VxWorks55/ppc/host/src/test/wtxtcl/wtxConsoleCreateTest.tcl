# wtxConsoleCreateTest.tcl - Tcl script, test WTX_CONSOLE_CREATE
#
# Copyright 1995-2001 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01l,23nov01,hbh  Removed DISPLAY test on :0 value.
# 01k,28jan98,p_b  Adapted for WTX 2.0
# 01j,20oct95,jmp  added invalidAddr argument to wtxConsoleCreateTest.
# 01i,21aug95,jmp  added timeout to wtxToolAttachCall in wtxConsoleCreateTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01h,23jun95,f_v  added wtxTestRun call
# 01g,09may95,f_v  added server can't start console test
#		   clean up main procedure
# 01f,12may95,c_s  updated error checking for new wtxtcl.
# 01e,16mar95,f_v  fix a bug now DISPLAY can be :0 or :0.0 
# 01d,27jan95,kpn  deplaced testcase units description in wtxTestMsg and
#                  testcase units errors in wtxTestErr.tcl,
#		   added global csleCreateMsg in the main,
#		   added global errMiscMsg in each testcase units,
#		   used csleCreateMsg & errMiscMsg to manage messages. 
# 01c,26jan95,kpn  removed the calls to wtxToolAttachCall and wtxToolDetach 
#		   made in each testcase.  
#		   Added wtxToolAttachCall and wtxToolDetachCall in the main 
#		   procedure. 
# 01b,24jan95,kpn  modified DISPLAY test.
# 01a,10jan95,kpn  written.
#
#

#
# DESCRIPTION
#
# WTX_CONSOLE_CREATE - Create a console tty
#
# TESTS :
#
# (1) verify wtxConsoleCreate result when it's called with valid display
#	parameter :
#	- verify the new console name
#	- verify the display value
#	- verify the console descriptor
#
# (2) verify that when the DISPLAY environment variable exists and its
#	value is valid, wtxConsoleCreate can be called without the <display>
#	parameter.
#
# (3) verify wtxConsoleCreate result when it's called with invalid display
#	parameter. The result must be : 0 0 0 it indicates the console has
#	not been created.
#
# REMARKS :
#
#   (1) wtxConsoleCreateTest3 calls wtxConsoleCreate with an invalid display
#	parameter and this call generates some <defunct> processes.  So if
#	this test is run several times the test will fail, and it is necessary
#	to restart the target server.
#   (2) when this test is executed it's normal to have the following message
#	on the target server : Error: Can't open display: unknownDisplay


#*************************************************************************
#
# wtxConsoleCreateTest1 - verify wtxConsoleCreate result when it's called 
#			  with valid display parameter.
#
# This test verifies wtxConsoleCreate result when it's called with valid
# display parameter. So it checks first that the DISPLAY environment
# variable exists, if not the test is stopped and a message is sent on the
# tcl shell to inform the user he must affect a value to the DISPLAY 
# environment variable. When this variable is available the test can test
# wtxConsoleCreate result : verify the console name is the same than the
# one given in parameter, verify the display value and verify the console 
# descriptor.
#

proc wtxConsoleCreateTest1 {Msg} {


    # array that contains the error codes
    global errMiscMsg

    global env

    puts stdout "$Msg" nonewline

    # check the DISPLAY environment variable exists
    set findDisplayVar [info exists env(DISPLAY)]
    
    # if DISPLAY environment variable doesn't exist : display a message
    if {$findDisplayVar == 0} {
	testFail $errMiscMsg(1)
	return
    }

    # get the value of the DISPLAY environment variable
    set display [set env(DISPLAY)]

    set pos [string first : $display] 

    if {$pos == -1} {
        testFail $errMiscMsg(2)
        return
    }

    # create a console
    set resConsoleCreate [wtxConsoleCreate consoleName $display]
    if {$resConsoleCreate == ""} {
        return
    }

    # verify the DISPLAY variable contains a valid value
    if {$resConsoleCreate == "0 0 0"} {
	testFail $errMiscMsg(2)
	return
    }

    # verify console name

    set consoleNameRet [lindex $resConsoleCreate 0]
    if {$consoleNameRet != "consoleName"} {
	testFail $errMiscMsg(3)
    }

    # verify the display value

    set displayValueRet [lindex $resConsoleCreate 1]
    if {$displayValueRet != $display} {
        testFail $errMiscMsg(4)
    }

    # verify console identificator
    # kill the console

    set consoleIdRet [lindex $resConsoleCreate 2] 
    set resConsoleKill [wtxConsoleKill $consoleIdRet]
    if {$resConsoleKill != 0} {
        testFail $errMiscMsg(5)
        return 
    }

    wtxTestPass
}

#*************************************************************************
#
# wtxConsoleCreateTest2 - verify wtxConsoleCreate called without the 
#			  display parameter.
#
# This test verifies that when the DISPLAY environment variable exists and
# its value is valid, wtxConsoleCreate can be called without the <display>
# parameter. 
# 

proc wtxConsoleCreateTest2 {Msg} {


    # array that contains the error codes
    global errMiscMsg

    global env

    puts stdout "$Msg" nonewline

    # check the DISPLAY environment variable exists
    catch "info exists env(DISPLAY)" findDisplayVar

    # DISPLAY environment variable doesn't exist : display a message
    if {$findDisplayVar == 0} {
        testFail $errMiscMsg(1)
        return
    }

    # get the value of the DISPLAY environment variable
    catch {set env(DISPLAY)} display

    set pos [string first : $display] 

    if {$pos == -1} {
        testFail $errMiscMsg(2)
        return
    }

    # create a console : call wtxConsoleCreate routine without the 
    # display parameter
    catch {wtxConsoleCreate consoleName} resConsoleCreate
    if {$resConsoleCreate == ""} {
        return
    }

    # verify the DISPLAY variable contains a valid value
    if {$resConsoleCreate == "0 0 0"} {
        testFail $errMiscMsg(2)
        return
    }

    # verify console name

    set consoleNameRet [lindex $resConsoleCreate 0]
    if {$consoleNameRet != "consoleName"} {
        testFail $errMiscMsg(3)
    }

    # verify the display value : get the value of the DISPLAY environment 
    # variable and compare it with the display value returns by 
    # wtxConsoleCreate 

    catch {set env(DISPLAY)} display

    set displayValueRet [lindex $resConsoleCreate 1]
    if {$displayValueRet != $display} {
        testFail $errMiscMsg(4)
    }

    # verify console identificator / kill the console

    set consoleIdRet [lindex $resConsoleCreate 2]
    catch {wtxConsoleKill $consoleIdRet} resConsoleKill
    if {$resConsoleKill != 0} {
        testFail $errMiscMsg(5)
        return
    }

    wtxTestPass
}


#*************************************************************************
#
# wtxConsoleCreateTest3 - verify wtxConsoleCreate result when it's called 
#			  with invalid display parameter.
#
#	
	
proc wtxConsoleCreateTest3 {Msg} {

    global errMiscMsg

    puts stdout "$Msg"  nonewline


    # create a console with an invalid display parameter (unknownDisplay)

    # if no exception occurred
    if {![catch {wtxConsoleCreate consoleName unknownDisplay}\
	resConsoleCreate]} {
	testFail $errMiscMsg(22)
	return
	}

    # if the caught exception is different than the expected error code
    if {![wtxErrorMatch $resConsoleCreate SVR_CANT_START_CONSOLE]} {
        testFail $resConsoleCreate
        return
    }

    # if the caught exception is the expected error code
    wtxTestPass
}


#*************************************************************************
#
# wtxConsoleCreateTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the 
# testcase units and calls each testcase procedure contained in the test script.
#

proc wtxConsoleCreateTest {tgtsvrName timeout invalidAddr} {


    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxConsoleCreateTest targetServerName\
					timeout invalidAddr"
        return
    }

    # variables initializations
    global csleCreateMsg

    # call tests with valid parameters
    wtxToolAttachCall $tgtsvrName 2 $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler
 
    wtxConsoleCreateTest1 $csleCreateMsg(1)
    wtxConsoleCreateTest2 $csleCreateMsg(2)


    # call tests with invalid parameters
    wtxErrorHandler [lindex [wtxHandle] 0] ""
    wtxConsoleCreateTest3 $csleCreateMsg(3)

    # detach from the target server
    wtxToolDetachCall "\nwtxConsoleCreateTest : "
}
