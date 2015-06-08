# timestampLib.tcl - VxWorks timestamp test library
#
# Copyright 1997 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01c,29aug97,db  fix for PASS/FAIL return code.
# 01b,31jul97,db  doc tweaks for man page generation
# 01a,14jul97,db  written.
#
#
# DESCRIPTION
# This is a library of routines used with the main timestamp tcl script. 
#
# The procedure timestampResGet reads the progress messages and the final
# results messages sent by the task pkLib.c:pkTimestampTestLong to the 
# serial port.
#*/

################################################################################
#
# timestampResGet - Get timestamp test results 
#
# This procedure reads the messages sent by the timestamp test task to the 
# serial port. A number equal to <T1_TMO_SLICES> of messages are sent 
# to indicate the progress of the timestamp test. Then a message with a PASS
# or FAIL string is expected to terminate the test.
# 
# SYNOPSIS:
#   timestampResGet <targetFd> <nslices> <timeout>
#
# PARAMETERS:
#   targetFd: target serial handle
#   nslices: number of time slices in the total timeout
#   timeout: total timeout for long test
#
# RETURNS: 1 for success, 0 for failure
#
# ERRORS: N/A

proc timestampResGet {targetFd nslices timeout}  { 

    set sliceTimeout [expr ($timeout*2)/$nslices]

    # loop to get all progress status messages. there should be one
    # message per time slice.

    serChanFlush $targetFd
    for {set i 0} {$i < $nslices} {incr i} {
    	set ret [serChanRecv $targetFd 2048 $sliceTimeout "through\ test" 1]
    	send_user [trimSerOutput $ret] 0
    	if {[string match timeoutError* $ret] || [string match Error* $ret]}  {
    		return 0 
	}
    }

    # now look for the result messages. the message should start with
    # a PASS or FAIL string.

    set ret [serChanRecv $targetFd 2048 $sliceTimeout "long-term\ timestamp" 1]
    send_user [trimSerOutput $ret] 0
    if {[string match timeoutError* $ret] || [string match Error* $ret]}  {
    	return 0 
    }

    # return the result of the test

    if {[string match *PASS* $ret]} {
    	return 1
    } else {
    	return 0
    }
}
