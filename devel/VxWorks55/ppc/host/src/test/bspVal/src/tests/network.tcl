#
# network.tcl - I/O tests for network
#
# Copyright 1997-2002 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01r,15jul02,pmr  fixed typo (SPR 78871).
# 01q,31oct01,sbs  docs update (SPR 32355)
# 01p,18sep01,sbs  merge from tor2_5.bsp_drv_facility branch for veloce
# 01o,29oct99,sbs  added markup for html docs generation.
# 01n,16oct99,sbs  added extra argument for checkAndLoadFile() call.
# 01m,20sep99,sbs  changed documentation to include INCLUDE_NET_SHOW as part
#                  of target configuartion (SPR #26420).
#                  changed script to fix udp echo test problem (SPR #27842).
# 01l,10jun99,sbs  merge from tor2_0_x
# 01k,23feb99,sbs  added changes to make VTS compatible with
#                  Tornado 2.0 (SPR #22425).
# 01j,26jul98,sbs  added comment on master and slave prefixes of variable
#                  names.( SPR #21423 and #21424)
# 01i,21apr98,p_b  Merge from ARM to Tornado 2_0_x
# 01h,21oct97,db   replaced taskDelete procedure to avoid the use of the
#                  target shell. set udpTMO to 200 msec(SPR #9512).
# 01g,04sep97,db   removed END_TEST in sample test output.
# 01f,29aug97,kbw  made man page fixes from jim
# 01e,25aug97,db   changed T1_TIP_DEVICE and T1_TIP_BAUD to T1_SER_DEVICE
#                  and T1_SER_BAUD respectively. minor bug fix.
# 01d,15aug97,kbw  made man page fixes from DAT and Dahmane
# 01c,10aug97,kbw  fixed minor man page problems, spell checked
# 01b,31jul97,db   doc tweaks for man page generation.
# 01a,30jun97,db   written.
#
#
# SYNOPSIS:
# 
# bspVal <options> network
# 
#
# DESCRIPTION:
# This test verifies basic network functionality, independent of network media.
# This script is written in Tcl and uses some wtx protocol functions. It also
# uses 'tclSerial' library functions for accessing the serial port(s).
# The entire test can take several minutes to run.
# See 'bspVal.sh' for an explanation of <options>.
#
# \ml
# \m 'NOTE:'
# This reference entry lists the most likely reasons for a test's failure.
# However, it does not list all possible reasons.
# \me
#
# This test requires two targets. The target being tested is the master. The
# second target, the slave, is used to run 'ping' and 'udpEchoTest'.
# The VxWorks image running on the slave must be built with the macro
# INCLUDE_PING defined. Both the targets require a VxWorks image built
# with INCLUDE_NET_SHOW defined. If using project facility, add components
# INCLUDE_PING and INCLUDE_NET_SHOW to respective VxWorks images. 
#
# The first test uses 'ping' to send approximately 50 ICMP
# packets (datagrams) to the ICMP server resident on the master.
# Each packet contains a request for a response from the server; the response
# should be received by the slave.  The round-trip time, from sending to
# receiving, is displayed for each packet.  As long as approximately 50
# responses are logged by the slave, this test passes.  If this test fails,
# check the network connection and make sure that the target IP address
# is correct.
#
# The first test requires that 'ping' be present on the slave. A
# check is done to make sure that it is in the path. If 'ping' is missing,
# the test aborts because the UDP test uses the 'ping' statistics.
#
# The second test spawns the pkUdpEchoTest(), a pkLib.c routine, on the slave
# to send a quantity of UDP packets to a UDP echo daemon running on the
# master, which is started by spawning pkUdpEchod(), also a pkLib.c
# routine. Each UDP packet is sent by the slave to the master, echoed,
# read back in by the slave, then checked for correct size
# and content.  Sixteen different size packets are sent and received.
# If pkUdpEchoTest() times out waiting for the echo, the packet is resent once.
# If the resent packet does not make it, the test is marked as a failure.
# The timeout value for individual UDP packets is derived from the statistics
# displayed in the 'ping' test.  This value depends on the network media
# being used. The actual packet round-trip time, however, should be considerably
# less than the calculated timeout value used.  If this test fails, make sure
# that a reliable host is being used and that the network is not being
# overloaded by other packet traffic.
#
# The UDP/IP networking protocol is used in the second test.  UDP/IP makes
# no guarantees with regard to packet delivery, thus the host and/or target
# might occasionally drop packets.  Although several dropped packets does not
# constitute a failed test, a large number of dropped packets (greater
# than one percent of the total sent) could indicate a problem with any of the
# following: network configuration, network device, device driver,
# networking software.
#
# Barring serious hardware or software limitations (such as an identifiable
# VxWorks problem), the target board must pass all tests for the BSP to be
# validated.
#
# A target that continually fails the second test might or might not be
# functioning properly.  If it can be proven that the host is responsible
# for dropping the packets (for example, by running 'etherfind'),
# then the target cannot be declared to have passed or failed this test.
# Hardware and network environment limitations need to be taken into
# consideration when evaluating the results of this test.
#
# CONFIGURATION PARAMETERS:
# \is
# \i 'T1_BOOT_TN' 20
# Master target name (required).
# \i 'T1_BOOT_E'
# Master target IP address (required).
# \i 'T1_BOOT_B'
# Master target backplane address (required).
# \i 'T2_BOOT_TN'
# Slave target name (required).
# \i 'T2_BOOT_E'
# Slave target IP address (required).
# \i 'T2_BOOT_B'
# Slave target backplane address (required).
# \i 'T2_UDP_TRANS'
# Number of packets used in UDP test.
# \ie
#
# EXAMPLE:
# Output consists of:
# \cs
#
#                          BSP VALIDATION TEST
#                          -------------------
#
#     Target server                     : t53-160
#     BSP                               : mv147
#     Second target server              : t214-2
#     Second BSP                        : mv147
#     Log file                          : /tmp/bspValidationLog.6425
#
#
#     network :
#     ping target test                                          : PASS
#     UDP packet echo test                                      : PASS
#
#                       Tests RUN           2
#                       Tests PASSED        2
#                       Tests FAILED        0
#                       Tests SKIPPED       0
# \ce
#
# SEE ALSO:
# 'bspVal.sh',
# networkLib.tcl,
# pkLib.c
#
# NOROUTINES
#
#

# print header

printHeader "Network Test"

# read environment variables
# please note that the variable name prefixes master and slave doesn't
# reflect any configuration setup.

set hostType    	"$env(WIND_HOST_TYPE)"

set masterTN    	[readEnvRequired T1_BOOT_TN]
set masterBootE       	[readEnvRequired T1_BOOT_E]
set masterBootB       	[readEnvRequired T1_BOOT_B]
set masterLibPath 	$firstbspdir
set masterBaud  	[readEnvRequired T1_SER_BAUD]
set masterDev   	[readEnvRequired T1_SER_DEVICE]

set slaveTN    		[readEnvRequired T2_BOOT_TN]
set slaveBootE      	[readEnvRequired T2_BOOT_E]
set slaveBootB       	[readEnvRequired T2_BOOT_B]
set slaveLibPath 	$secondbspdir
set slaveBaud  		[readEnvRequired T2_SER_BAUD]
set slaveDev   		[readEnvRequired T2_SER_DEVICE]

# UDP timeout is defined here to a value of 200 msec.

set udpTMO   200

# attach to master target server

set masterHd [wtxToolAttach $firsttgtsvrname]
wtxRegisterForEvent .*

wtxTimeout 50

checkAndLoadFile $masterLibPath pkLib.o pkGetMacroValue

# attach to slave target server

set slaveHd [wtxToolAttach $secondtgtsvrname]
wtxRegisterForEvent .*

wtxTimeout 50

# set error handler

wtxErrorHandler $masterHd vtsErrorHandler
wtxErrorHandler $slaveHd vtsErrorHandler

checkAndLoadFile $slaveLibPath pkLib.o pkGetMacroValue

# get serial connection with master and slave targets

set masterFd [serChanOpen $masterDev $masterBaud]
set slaveFd  [serChanOpen $slaveDev $slaveBaud]

# call ifShow() so the network configuration will appear in the log file

serChanFlush $masterFd

# select master target server

wtxHandle $masterHd
wtxFuncCall -int [symbol ifShow]
set retcode [retcodeGet]

set ret1 [serChanRecv $masterFd 2048 30 "collisions" 1]
send_user [trimSerOutput $ret1] 0

if {[string match timeoutError* $ret1] ||  \
    [string match Error* $ret1]} {
    fatalError "Can't read from $masterDev port"
}

# if target ethernet address not defined, assume target
# is a backplane slave.

set masterAdrs [afterEquals [beforeColon $masterBootE]]

if {$masterAdrs == ""} {
    set masterAdrs [afterEquals [beforeColon $masterBootB]]
}

# break master address into 4 numbers from the xxx.xx.xx.xx notation

scan $masterAdrs {%[0-9].%[0-9].%[0-9].%[0-9]} adr1 adr2 adr3 adr4

# start ping target test

set ret 0
set trans 0
set recv 0

# check initial icmpstatShow() values

set retcode [icmpStatsGet $masterFd]
if {$retcode == "ERROR"} {
    fatalError "could not get icmp stats"
} else {
    set iMsg [lindex $retcode 0]
    set oMsg [lindex $retcode 1]
    send_user  "\nInitial ICMP stats: iMsg=$iMsg oMsg=$oMsg\n" 0
}

# find out if ping is present
set buffSize 2048
set readTMO 20

serChanFlush $slaveFd
wtxHandle $slaveHd
wtxFuncCall -int [symbol pkPing] $adr1 $adr2 $adr3 $adr4 1 0
set retcode [retcodeGet]

set ret1 [serChanRecv $slaveFd $buffSize $readTMO "alive" 1]
send_user  [trimSerOutput $ret1] 0
if {[string match timeoutError* $ret1] || [string match Error* $ret1]}  {
   fatalError "target not responding to ping"
}

# delete any left-over test task

set ret [getWindshOutput $secondtgtsvrname "td pkPing"]

# start a task on the target that will execute the ping command 100 times

serChanFlush $slaveFd

set priority 100
set options  0x3
set stackBase 0
set stackSize 5000
set entry [symbol  pkPing]
set redirIn 0
set redirOut 0

set tid [wtxContextCreate CONTEXT_TASK pkPing $priority $options\
 	       $stackBase $stackSize $entry $redirIn $redirOut $adr1 $adr2 \
 	       $adr3 $adr4 100 0]
if {$tid == -1} {
    fatalError "Could not spawn ping task on master"
}

if {[wtxContextResume CONTEXT_TASK $tid] != 0} {
    fatalError "Could not start ping on master"
}

set line [serChanRecv $slaveFd $buffSize $readTMO $masterAdrs 1]
if {[string match timeoutError* $line] || [string match Error* $line]}  {
   fatalError "ping timed out"
}

set count 0
set nTimeouts 0
while {$count <= 50}  {

    set line [serChanRecv $slaveFd $buffSize $readTMO "ms" 1]
    if {[string match timeoutError* $line] || [string match Error* $line]}  {
        incr nTimeouts
	if {$nTimeouts == 10} {
            fatalError "ping timed out"
        } else {
    	    continue
        }
    } else {
        send_user  "[trimSerOutput $line]" 0
        incr count
    }
}

wtxContextKill CONTEXT_TASK $tid

set line [serChanRecv $slaveFd $buffSize $readTMO "*/*/*" 1]
set line [trimSerOutput $line]
send_user  "count=$count $line" 0

if {[string match *transmitted* $line] == 1} {
    set trans [extract $line packets\ transmitted -1 -1]
}
if {[string match *received* $line] == 1} {
    set recv [extract $line packets\ received -1 -1]
}
if {[string match  *min/avg/max* $line] == 1} {
    set stats [extract [extract $line "min/avg/max =" 1 -1] / 0 -1]
}

# extract and check ping stats

set pingTMO [extract $stats / -1 -1]

if {($trans == -1) || ($recv == -1) || ($pingTMO == -1) || ($ret == -1)} {
    fatalError "was not able to retrieve ping packet stats"
}

if {$trans < 50} {
    fatalError "host only sent $trans packets"
}

send_user  "PING: trans=$trans recv=$recv" 0

# warn user if there are any dropped packets

if  {$trans > $recv}  {
    warning "[expr $trans -$recv] ICMP packets dropped in ping test"
}

# check final icmpstatShow()

wtxHandle $masterHd
set in 0
set out 0
for {set status 0}  {$status < 3} {incr status}  {

    set retcode [icmpStatsGet $masterFd]
    if {$retcode != "ERROR"} {
        set in  [expr [lindex $retcode 0] - $iMsg]
    	set out [expr [lindex $retcode 1] - $oMsg]
    }

    if { ($in != 0) && ($out != 0) } {
	break;
    }

}

send_user  "\nFinal ICMP stats: Messages in=$in Messages out=$out\n" 0

# check the ping results

set ret 0
if {($in < $recv) || ($out < $recv)}  {
    set ret -1
}

testResult [expr {$ret == 0}] "ping target test"

# check initial udpstatShow() values

set retcode [udpStatsGet $masterFd]
if {$retcode == "ERROR"} {
    fatalError "could not get icmp stats"
} else {
    set bcast [lindex $retcode 0]
    set iPkt  [lindex $retcode 1]
    set oPkt  [lindex $retcode 2]
    send_user "\nInitial UDP stats: broadcasts=$bcast \
	       input pkts=$iPkt output pkts=$oPkt\n" 0
}

# start target UDP daemon

# spawn the test task on the target to be tested(master)

wtxHandle $masterHd

# delete any left-over test task

set ret [getWindshOutput $firsttgtsvrname "td udpEcho"]

set priority 100
set options  0x3
set stackBase 0
set stackSize 5000
set entry [symbol  pkUdpEchod]
set redirIn 0
set redirOut 0

set masterTid [wtxContextCreate CONTEXT_TASK udpEcho $priority $options\
 	       $stackBase $stackSize $entry $redirIn $redirOut]
if {$masterTid == -1} {
    fatalError "Could not spawn task on master"
}

if {[wtxContextResume CONTEXT_TASK $masterTid] != 0} {
    fatalError "Could not start task on master"
}

# get packet timeout and number of messages to send.

set pktTMO $udpTMO
set trans  [readEnvOptional T2_UDP_TRANS 5000]

# spawn slave-side test

wtxHandle $slaveHd

set entry [symbol pkUdpEchoT]
set param1 $adr1
set param2 $adr2
set param3 $adr3
set param4 $adr4
set param5 $trans
set param6 $pktTMO

# spawn the pkUdpEchoT function as a task

set cmd " sp pkUdpEchoT,$param1, $param2, $param3, $param4, $param5,\
             $param6 "

set ret [getWindshOutput $secondtgtsvrname $cmd]

# check for any errors

if {$ret == "ERROR"} {
    fatalError "Can't spawn task pkUdpEchoT task"
}

# get the task id from the pattern received from task spawning
# pattern received is "task spawned: id = <xxxx>, name = s<x>u<x>"

set ret1 [afterEquals $ret]
set ret2 [lindex $ret1 0]
set ret3 [string length ret1]
set ret4 [string range $ret1 0 [expr ($ret3 - 2)]]
set taskId "0x$ret4"

# flush the serial channel on the slave target

serChanFlush $slaveFd

set readTMO [expr 2 * ($pktTMO * $trans/10) / 1000]
if {$readTMO <= 0} {
    set readTMO 1    ;# one second minimum timeout
}

# we expect to receive 10 progress messages from the udpEchoTest
# task running on the master.

set count 0

while {$count < 10}  {

    set line [serChanRecv $slaveFd $buffSize $readTMO \
                          "through\ UDP\ echo\ test" 1]
    if {[string match timeoutError* $line] || [string match Error* $line]}  {
        if {[string match *Error:* $line] == 1} {
            send_user [trimSerOutput $line] 0
#           clean-up
            set ret [getWindshOutput $secondtgtsvrname "td $taskId" ]
            fatalError "udpEchoTest failed"
        }
    }
    send_user  [trimSerOutput $line] 0
    incr count
}

set passTest 0
set resent   0

set line [serChanRecv $slaveFd $buffSize $readTMO "END\ OF\ UDP\ TEST" 1]
send_user [trimSerOutput $line] 0
if {[string match timeoutError* $line] || [string match Error* $line]}  {
#   clean-up
    set ret [getWindshOutput $secondtgtsvrname "td $taskId" ]
    fatalError "udp timed out"
}
if {[string match *passed* $line] == 1} {
    set passTest 1
}
if {[string match *packets\ resent:* $line] == 1} {
    set resent [extract $line "packets\ resent:" 0 0]
}

# warn user of any dropped packets

if {$resent > 0}  {
    warning "$resent UDP packet(s) dropped in packet echo test"
}

if {$passTest == 0} {
    testResult $passTest "UDP packet echo test"
}


# check final udpstatShow() values

wtxHandle $masterHd
set retcode [udpStatsGet $masterFd]
if {$retcode == "ERROR"} {
    fatalError "could not get udp stats"
} else {
    set ret 0
    set inChange [expr [lindex $retcode 0] - $bcast + $iPkt]
    set in [expr [lindex $retcode 1] - $inChange]
    set out [expr [lindex $retcode 2] - $oPkt]
    send_user "\nFinal UDP stats: input pkts=$in output pkts=$out\n" 0

    if { ($in < $trans) || ($out < $trans)}  {
        set ret -1
    }

    testResult [expr {$ret == 0}] "UDP packet echo test"
}


# close serial channel

serChanClose $masterFd
serChanClose $slaveFd

# kill updEcho on master

wtxContextKill CONTEXT_TASK $masterTid

# detach from target servers

wtxToolDetach $masterHd
wtxToolDetach $slaveHd

