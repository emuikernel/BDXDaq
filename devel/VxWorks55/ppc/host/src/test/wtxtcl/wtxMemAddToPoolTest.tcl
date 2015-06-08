# wtxMemAddToPoolTest.tcl - Tcl script, test WTX_MEM_ADD_TO_POOL
#
# Copyright 1995 Wind River Systems, Inc.
#
#
# modification history
# --------------------
# 01i,28jan98,p_b  Adapted for WTX 2.0
# 01h,20oct95,jmp  added invalidAddr argument to wtxMemAddToPoolTest.
# 01g,17aug95,jmp  added timeout to wtxToolAttachCall in wtxMemAddToPoolTest.
#                  changed tests control method, now uses wtxErrorHandler.
# 01f,18jul95,jmp  removed "funcCall free" to prevent from errors to the
#		   other tests, there is 500 bytes on VxWorks not free.
# 01e,23jun95,f_v  added wtxTestRun call
# 01d,21jun95,f_v  clean up main procedure
# 01c,28apr95,c_s  now uses wtxErrorMatch.
# 01b,27apr95,f_v  replace call to funcCall1 by funcCall
# 01a,28mar95,f_v  written.
#
#

#
# DESCRIPTION
#
# WTX_MEM_ADD_TO_POOL - adds memory to the target memory pool
#
# TESTS :
#
# (1) wtxMemAddToPoolTest1 - Add memory to target memory pool previously
#     allocated on VxWorks. This test use valid parameters.
#



#*****************************************************************************
#
# wtxMemAddToPoolTest1 - adds memory to the target memory pool
#
# Verify wtxMemAddToPool add memory when it uses with valid parameters 
#
# REMARK : This test allocs 500 bytes on VxWorks to add memory to the target
# 	   memory pool, this 500 bytes aren't free at the end of the test.
#
 
proc wtxMemAddToPoolTest1 {Msg} {


 # GLOBAL VARIABLES

    # array that contains the testcase error codes
    global errMemMsg

    puts stdout "$Msg" nonewline

    set memSize 500
    set margInf 450
    set margSup 550

    # call malloc on VxWorks - create a block of "memSize" bytes
    set bAddrs [funcCall malloc $memSize]

    # get info about memory pool
    set memInfo1 [wtxMemInfoGet]
    set freeBytes1 [lindex $memInfo1 0]
    set freeBlock1 [lindex $memInfo1 3]

    # add memory to the target memory pool
    set status [wtxMemAddToPool $bAddrs $memSize]
    if {$status == "" } {
	return
	}

    # get info about memory pool
    set memInfo2 [wtxMemInfoGet]
    set freeBytes2 [lindex $memInfo2 0]
    set freeBlock2 [lindex $memInfo2 3]

    set freeBlock1 [expr $freeBlock1+1]

    # diff should be "memSize" by there is a overhead it's why we took a
    # 10% margin
    set diff [expr $freeBytes2-$freeBytes1]

    if {$freeBlock1!=$freeBlock2 || ($diff>$margSup) || ($diff<$margInf) } {
	 testFail $errMemMsg(17)
	 return
	 }
    wtxTestPass
}

#*****************************************************************************
#
# wtxMemAddToPoolTest - main procedure
#
# The main procedure initializes variables that will be used by the test's
# testcase units, it makes accessible the array with the description of the
# testcase units and calls each testcase procedure contained in the test script.
# Furthermore it attaches itself to the target server (thru a wtxToolAttachCall
# command) and, when testcase units have been run, detaches itself from the
# target server.
#

proc wtxMemAddToPoolTest {tgtsvrName timeout invalidAddr} {

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxMemAddToPoolTest targetServerName\
				timeout invalidAddr"
        return
    }

    # tests description
    global memAddToPoolMsg

    # connection to the target server
    wtxToolAttachCall $tgtsvrName 2 $timeout
    # enable events
    catch {wtxRegisterForEvent .*} st

    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # call the testcase units
    wtxMemAddToPoolTest1 $memAddToPoolMsg(1)

    # detach from the target server
    wtxToolDetachCall "wtxMemAddToPoolTest"
}
