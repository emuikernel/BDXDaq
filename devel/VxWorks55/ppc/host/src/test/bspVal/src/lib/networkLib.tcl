# networkLib.tcl - VxWorks network library
#
# Copyright 1997 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,29jun97,db  written.
#
#
# DESCRIPTION
# This is a library of procedures used by the network test script.
#
# The procedures icmpStatsGet and udpStatsGet return the statistics given by
# the vxWorks functions icmpstatShow() and udpstatShow() respectively.
#
#*/

################################################################################
#
# icmpStatsGet - get ICMP stats  
#
# This routine sends the command icmpstatShow to the target and reads the
# return message. It extracts the numbers of messages received/sent and 
# retuns them in a string. 
#
# SYNOPSIS:
#    icmpStatsGet targetFd
#
# PARAMETERS:
#    targetFd: handle returned by the tclSerial open function.  
#
# RETURNS: 
#    "ERROR" if failure or string composed of number of input 
#    packets and output packets.
#
# ERRORS: N/A 
#
proc icmpStatsGet  {targetFd} { 

    set readTMO 20

    serChanFlush $targetFd
    wtxFuncCall -int [symbol icmpstatShow]
    set retcode [retcodeGet]

    set ret1 [serChanRecv $targetFd 2048 $readTMO "ICMP" 1]
    if {[string match timeoutError* $ret1] || [string match Error* $ret1]} {
     	return "ERROR"	
    } else {
    	set ret2 [serChanRecv $targetFd 2048 $readTMO "response" 1]
        if {[string match timeoutError* $ret2] || [string match Error* $ret2]} {
    		return "ERROR"	
	} else {

    		# save the number of ICMP packets already sent/received 

		set inMsg [extract $ret2 echo: 0 0]
		set outMsg [extract $ret2 echo\ reply: 0 0]
		lappend res $inMsg $outMsg
		return $res 
	}

    }
}

################################################################################
#
# udpStatsGet - get UDP stats  
#
# This routine sends the command udpstatShow to the target and reads the
# return message. It extracts the numbers of messages received/sent and 
# broadcasts and returns them in a string. 
#
# SYNOPSIS:
#   udpStatsGet <targetFd>
#
# PARAMETERS:
#    targetFd: handle returned by the tclSerial open function.  
#
# RETURNS: 
#    "ERROR" if failure or string composed of number of broadcasts, 
#    input packets and output packets.
#
# ERRORS: N/A
#
proc udpStatsGet  {targetFd} { 

    set readTMO 20
    serChanFlush $targetFd
    wtxFuncCall -int [symbol udpstatShow]
    set retcode [retcodeGet]

    set ret1 [serChanRecv $targetFd 2048 $readTMO "UDP" 1]
    if {[string match timeoutError* $ret1] || [string match Error* $ret1]} {
     	return "ERROR"	
    } else {
    	set ret2 [serChanRecv $targetFd 2048 $readTMO "socket" 1]
        if {[string match timeoutError* $ret2] || [string match Error* $ret2]} {
    		return "ERROR"	
	} else {

    		# extract the number of UDP packets sent/received 

		set bcast [extract $ret2 broadcasts\ received -1 0]
		set iPkt  [extract $ret2 input\ packets -1 0]
		set oPkt  [extract $ret2 output\ packets -1 0]

		lappend res $bcast $iPkt $oPkt
		return $res 
	}

    }
}
