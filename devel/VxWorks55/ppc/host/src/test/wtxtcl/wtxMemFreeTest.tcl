# wtxMemFreeTest.tcl - Tcl script, test WTX_MEM_FREE
#
# Copyright 1994-1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01m,28jan98,p_b  Adapted for WTX 2.0
# 01l,20oct95,jmp  added invalidAddr argument to wtxMemFreeTest.
# 01k,17aug95,jmp  added timeout to wtxToolAttachCall in wtxMemFreeTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01j,23jun95,f_v  added wtxTestRun call
# 01i,21jun95,f_v  clean up main procedure
# 01h,27apr95,c_s  now uses wtxErrorMatch
# 01g,07apr95,f_v  replace errMemMsg
# 01f,03mar95,f_v  replace S_tgtMemLib_XXX by tgtMemErr(XXX)
# 01e,17feb95,kpn  added wtxMemFreeTest2{}
# 01d,27jan95,kpn  deplaced testcase units description in wtxTestMsg and
#                  testcase units errors in wtxTestErr.tcl,
#                  added global memFreeMsg in the main,
#                  added global errMemMsg in each testcase units,
#	           used memFreeMsg & errMemMsg to manage messages.	
# 01c,26jan95,kpn  removed the calls to wtxToolAttachCall and wtxToolDetach
#                  made in each testcase.
#                  Added wtxToolAttachCall and wtxToolDetachCall in the main
#                  procedure.
#		   removed error codes initialization, added global variables.
# 01b,04jan95,kpn  removed parameter to wtxToolDetach (automatic now).
# 01a,21nov94,kpn  written.
#
#

#
# DESCRIPTION
#
# WTX_MEM_FREE - Free a block of target memory
#
# TESTS :
#
# (1) wtxMemFreeTest1 - verify the value returned by wtxMemFree() when this 
#	routine de-allocates a block of memory previously allocated with
#	wtxMemAlloc. So verify wtxMemFree() with a valid memory block 
#	address.
#
#
# REMARK :
#	This is the result of wtxMemFree with an invalid argument :
#	% wtxMemFree 10000000
#	0
#	%
#



#*****************************************************************************
#
# wtxMemFreeTest1 - verify wtxMemFree() with a valid memory block address
#
# Verify wtxMemfree de-allocates a block of memory previously allocated with
# wtxMemAlloc.
#

proc wtxMemFreeTest1 {Msg} {

 # GLOBAL VARIABLES
    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline

    # get current free memory size
    set freeBefore [lindex [wtxMemInfoGet] 0]

    # Allocate memory on target (100 bytes), return the address of the
    # allocated memory block
    set resMemAlloc [wtxMemAlloc 100]
    if {$resMemAlloc == ""} {
        return
	}

    set resMemFree [wtxMemFree $resMemAlloc]

    set freeAfter [lindex [wtxMemInfoGet] 0]

    if {$freeAfter != $freeBefore || $resMemFree != 0} {
	testFail $errMemMsg(22)
	return
	}

    wtxTestPass
}

#*****************************************************************************
#
# wtxMemFreeTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
# Furthermore it attaches itself to the target server (thru a wtxToolAttachCall
# command) and, when testcase units have been run, detaches itself from the
# target server.
#

proc wtxMemFreeTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxMemFreeTest targetServerName\
				timeout invalidAddr"
        return
    }

    global memFreeMsg

    # connection to the target server
    wtxToolAttachCall $tgtsvrName 2 $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # call the testcase units
    wtxMemFreeTest1 $memFreeMsg(1)

    # detach from the target server
    wtxToolDetachCall "\nwtxMemFreeTest : "
}
