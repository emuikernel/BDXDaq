# busTas.tcl - bus test-and-set test
#
# Copyright 1997-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01p,31oct01,sbs  docs update (SPR 32355)
# 01o,18sep01,sbs  merge from tor2_5.bsp_drv_facility branch for veloce
# 01n,29oct99,sbs  added markup for html docs generation.
# 01m,16oct99,sbs   added extra argument for checkAndLoadFile() call.
# 01l,23feb99,sbs   added changes to make VTS compatible with
#                   Tornado 2.0 (SPR #22425).
# 01k,26jul98,sbs   added changes to get the test run with 'T1' target
#                   as slave on common VME backplane.(SPR #21423).
# 01j,04oct97,db    added call to serChanClose() for slave serial port.
#		    replaced _pkTasTest with pkTasTest(SPR #9512).
# 01i,04sep97,db    removed END_TEST in sample test output.
# 01h,30aug97,db    made minor man page fix
# 01g,25aug97,db    minor bug fix. added call to vtsErrorHandler.
# 01f,15aug97,kbw   edits to man pages from DAT and Dahmane
# 01e,14aug97,db    added log messages and did minor modifications.
# 01d,11aug97,db    input edits recommended by db, spell check
# 01c,11aug97,db    added a fix for cleanup of left-over tests tasks.
# 01b,10aug97,kbw   fixed minor man page problems, spell check
# 01a,16jun97,db    written.
#
# SYNOPSIS:
# 
# bspVal <options> busTas
# 
#
# DESCRIPTION
# This module is the Tcl code for the test-and-set test. It verifies the
# sysBusTas() operation between two VxWorks target boards in a common VME
# backplane. This module is implemented in Tcl and WTX protocols functions.
# The entire test takes approximately 10 minutes to run.  See 'bspVal.sh'
# for an explanation of <options>.
#
# If this test is run between two targets using shared memory, make sure
# the target is configured to include shared memory show routines. To do
# this, add 'INCLUDE_SHOW_ROUTINES' to config.h of the BSPs or include the
# component 'INCLUDE_SM_NET_SHOW', if using project facility.
#
# \ml
# \m 'NOTE:'
# This reference entry lists the most likely reasons for a test's failure.
# However, it does not list all possible reasons.
# \me
#
# The script does not reboot the targets. It assumes that the two targets
# and their respective target servers are up and running.
#
# By default the test assumes that 'T1' target is the system controller on
# the common VME backplane. An optional configuration parameter 'T1_BUS_MASTER'
# can be set to 'FALSE' to override this default configuration. In this case
# the 'T2' target is assumed to be the system controller. It is necessary to
# make sure that 'T1_BUS_MASTER' has the correct value in case targets have to
# be rebooted before running any test.
#
# The test starts by obtaining two cache-coherent memory locations by calls to
# cacheDmaMalloc() on the <master>. These two memory locations are used for
# the semaphore and the access counter. The 'pkTestRamByte' function checks
# that the addresses returned by cacheDmaMalloc() are readable and
# writable.
#
# Next, the local addresses are used to compute the bus addresses by calling
# the function sysLocalToBusAdrs() on the master. The bus addresses returned by
# sysLocalToBusAdrs() are passed to the slave, which uses the function
# sysBusToLocalAdrs() to get its own local addresses that it must apply to
# access the semaphore and counter locations on the master's shared memory.
# The 'smAdrsFind' procedure is called to verify that the counter and semaphore
# locations are seen by both the master and the slave. The memory locations are
# then initialized and the test is started by spawning the 'pkTasTest' task on
# both master and slave. The functions wtxContextCreate() and wtxContextResume()
# are used here to spawn and start the tasks. The function pkTasTest() uses the
# semaphore location for mutual exclusion. When the master gets the semaphore,
# it increments the counter, if it is odd, and runs a busy-wait loop until it
# finally clears the semaphore. The slave runs the same way except that it
# increments the counter if it is even. This scheme ensures that the two
# tasks take turns grabbing the semaphore and incrementing the counter.
#
# The counter is monitored from the script, which takes two readings of the
# counter one second apart. If the second reading is larger than the first one,
# the short-term test is successful. After a pause of ten minutes, the two
# readings are retaken. If the counter is still increasing, the long-term
# test is successful.
#
# If this test fails, check that the <master> and <slave> are both accessing
# the same two <master> memory locations.  If the <master> sysLocalToBusAdrs()
# or the <slave> sysBusToLocalAdrs() routines are not functioning properly,
# the test will not be able to find appropriate <slave> addresses that
# access the <master> memory locations.  The <master> memory locations must
# be cache-coherent and must be writable.  The VME controller chips must be
# initialized to proper values.  For fast target boards, it might be necessary
# to increase the busy-wait delays given by the 'T1_TAS_DELAY'
# ('T2_TAS_DELAY') macro.
#
# The two tests in this script require two unique cache-coherent memory
# locations on <master> that can be accessed by both <master> and <slave>.
# By default, these locations are obtained by calls to cacheDmaMalloc()
# on the <master>.  Alternatively, the macros 'T1_COUNT_ADRS'
# and/or 'T1_SEM_ADRS' can be changed from the default of "-1" to
# point to unique shared memory addresses on <master>.  In this case, the
# user must make sure that these are free cache-coherent memory locations.
# The ability to explicitly declare these locations is provided for debugging
# purposes and for boards with hardware limitations.  The final validation
# should leave the macros set to "-1", and allocate the memory dynamically
# by calling cacheDmaMalloc().
#
# Barring serious hardware or software limitations (such as an identifiable
# VxWorks problem), the target board must pass all tests for the BSP to be
# validated.  Of course, if the VME backplane is not supported by the target
# board, then this test is not a BSP validation requirement.
#
# CONFIGURATION PARAMETERS:
# \is
# \i 'T1_COUNT_ADRS' 25
# Address of master's shared counter (optional).
# \i 'T1_SEM_ADRS'
# Address of master's shared semaphore (optional).
# \i 'T1_TAS_DELAY'
# Busy delay during TAS on <master> (optional).
# \i 'T2_TAS_DELAY'
# Busy delay during TAS on <slave> (optional).
# \ie
#
# EXAMPLE:
# Output consists of:
# \cs
#                          BSP VALIDATION TEST
#                          -------------------
#
#  Target server                     : t53-160
#  BSP                               : mv147
#  Second target server              : t214-2
#  Second BSP                        : mv147
#  Log file                          : /tmp/bspValidationLog.6425
#
#  busTas:
#  short-term TAS test					: PASS
#  long-term TAS test					: PASS
#
#
# 			Tests RUN           2
#			Tests PASSED        2
#			Tests FAILED        0
#			Tests SKIPPED       0
# \ce
# 
# SEE ALSO:
# 'bspVal.sh',
# smLib.tcl,
# pkLib.c
#
# NOROUTINES
#


# print header

printHeader "busTas"

# determine the master and slave boards. By default it T1 board

set shMaster		[readEnvOptional T1_BUS_MASTER "TRUE"]

if {$shMaster == "TRUE"} {
    set masHnd T1
    set slvHnd T2
} else {
    set masHnd T2
    set slvHnd  T1
}

# read master environment variables

set masterTasDelay 	[readEnvOptional ${masHnd}_TAS_DELAY	20]
set masterCountAdrs	[readEnvOptional ${masHnd}_COUNT_ADRS	-1]
set masterSemAdrs	[readEnvOptional ${masHnd}_SEM_ADRS	-1]
if {$masHnd == "T1"} {
    set masterLibPath 	$firstbspdir
    set masterTgtSvr    $firsttgtsvrname
} else {
    set masterLibPath   $secondbspdir
    set masterTgtSvr    $secondtgtsvrname
}
set masterBaud  	[readEnvRequired ${masHnd}_SER_BAUD]
set masterDev   	[readEnvRequired ${masHnd}_SER_DEVICE]
set masterTN    	[readEnvRequired ${masHnd}_BOOT_TN]

# read slave environment variables

set slaveTasDelay	[readEnvOptional ${slvHnd}_TAS_DELAY	20]
if {$slvHnd == "T2"} {
    set slaveLibPath 	$secondbspdir
    set slaveTgtSvr     $secondtgtsvrname
} else {
    set slaveLibPath    $firstbspdir
    set slaveTgtSvr     $firsttgtsvrname
}
set slaveBaud   	[readEnvRequired ${slvHnd}_SER_BAUD]
set slaveDev    	[readEnvRequired ${slvHnd}_SER_DEVICE]
set slaveTN    		[readEnvRequired ${slvHnd}_BOOT_TN]

# establish serial link to master

set masterFd [serChanOpen $masterDev $masterBaud]

# establish serial link to slave

set slaveFd [serChanOpen $slaveDev $slaveBaud]

# attach to master target server

set masterHd [wtxToolAttach $masterTgtSvr]
wtxRegisterForEvent .*
wtxTimeout 80

# attach to slave target server

set slaveHd [wtxToolAttach $slaveTgtSvr]
wtxRegisterForEvent .*
wtxTimeout 120

# set error handler

wtxErrorHandler $masterHd vtsErrorHandler
wtxErrorHandler $slaveHd vtsErrorHandler

# check if pkLib.o is loaded on slave

wtxHandle $slaveHd
set retcode [checkAndLoadFile $slaveLibPath pkLib.o pkGetMacroValue]
if {$retcode == 1} {
    fatalError "pkLib.o is not found on slave"
}

# check if pkLib.o is loaded on master

wtxHandle $masterHd
set retcode  [checkAndLoadFile $masterLibPath pkLib.o pkGetMacroValue]
if {$retcode == 1}  {
    fatalError "pkLib.o is not found on master"
}

serChanFlush $masterFd

if {[catch "wtxSymFind -name smNetShow" symFin]} {
    warning "smNetShow symbol not available on the master target"
}

# obtain cache safe memory on the master target

if {$masterCountAdrs == -1} {
    wtxFuncCall -int [symbol cacheDmaMalloc] 8
    set masterCountAdrs [retcodeGet]
    if { !$masterCountAdrs }  {
        fatalError "cacheDmaMalloc failed "
    }
}

# get master semaphore address.

if {$masterSemAdrs == -1}  {
    set masterSemAdrs [format "%#x" [expr $masterCountAdrs + 4]]
}

# check that addresses do not overlap

if { ($masterSemAdrs >= $masterCountAdrs) && \
    ($masterSemAdrs < [expr $masterCountAdrs+4]) } {
    fataError "COUNT_ADRS and SEM_ADRS addresses overlap"
}

# run a memory check on the master's test locations

wtxFuncCall -int [symbol pkTestRamByte] $masterCountAdrs
set retcode1 [retcodeGet]

wtxFuncCall -int [symbol pkTestRamByte] $masterSemAdrs
set retcode2 [retcodeGet]

if {($retcode1 != 0) || ($retcode2 != 0)}  {
    fatalError "shared memory address invalid"
}

# select slave target server

wtxHandle $slaveHd

# find the shared memory addresses on the slave

set slaveCountAdrs [smAdrsFind $masterCountAdrs $masterHd $slaveHd]
if {$slaveCountAdrs == -1}  {
    fatalError "slave memory address not found for $masterCountAdrs"
}

set slaveSemAdrs [smAdrsFind $masterSemAdrs $masterHd $slaveHd]
if {$slaveSemAdrs == -1}  {
    fatalError "slave memory address not found for $masterSemAdrs"
}

# start the test on the master and slave

# initialize the test locations

wtxHandle  $masterHd
if {[wtxMemSet $masterCountAdrs 4 0] != 0} {
    fatalError "Could not initialize memory on master"
}
if {[wtxMemSet $masterSemAdrs 4 0] != 0} {
    fatalError "Could not initialize memory on master"
}

# select slave target server

wtxHandle $slaveHd
if {[wtxMemSet $slaveSemAdrs 4 0] != 0} {
    fatalError "Could not initialize memory on slave"
}
if {[wtxMemSet $slaveCountAdrs 4 0] != 0} {
    fatalError "Could not initialize memory on slave"
}

# spawn the test task on the master

wtxHandle $masterHd

# delete any existing task left from previous tests

# This is not done for Tornado 2.0 currently.

set priority 100
set options  0x3
set stackBase 0
set stackSize 5000
set entry [symbol pkTasTest]
set redirIn 0
set redirOut 0

set masterTid [wtxContextCreate CONTEXT_TASK mule1 $priority $options\
		                $stackBase $stackSize $entry $redirIn \
   				$redirOut $masterSemAdrs $masterCountAdrs \
   				1 $masterTasDelay]
if {$masterTid == -1} {
    fatalError "Could not spawn test task on master"
}

if {[wtxContextResume CONTEXT_TASK $masterTid] != 0} {
    fatalError "Could not start task on master"
}
send_user "mule1 spawned on master with id=$masterTid" 0

# spawn the test task on the slave

wtxHandle $slaveHd

# delete any left over test task from previous tests

# This is not done for Tornado 2.0 currently.

set entry [symbol pkTasTest]
set slaveTid  [wtxContextCreate CONTEXT_TASK mule2 $priority $options\
   			 	$stackBase $stackSize $entry $redirIn \
   				$redirOut $slaveSemAdrs $slaveCountAdrs \
   				0 $slaveTasDelay]

if {$slaveTid == -1} {
    fatalError "Could not spawn test task on slave"
}

if {[wtxContextResume CONTEXT_TASK $slaveTid] != 0} {
    fatalError "Could not start task on slave"
}
send_user "mule2 spawned on slave with id=$slaveTid" 0

# send user progress

send_user "\n## Starting short term TAS test. ##\n" 0

# check if counter is running

wtxHandle $masterHd
msleep 1000

smResultsCheck $masterHd $slaveHd $masterCountAdrs \
	       $masterTid $slaveTid "short-term TAS test"

# wait 10 minutes then do long term TAS test

send_user "\n## Starting long term TAS test. ##\n\n" 0
send_user "This test will take 10 minutes to run.\n\n" 0

set retcode [time {msleep 60000} 10]

smResultsCheck $masterHd $slaveHd $masterCountAdrs \
	       $masterTid $slaveTid "long-term TAS test"

# close serial channels

serChanClose $masterFd
serChanClose $slaveFd

# kill the two tasks that were spawned on the master and the
# slave.

wtxContextKill CONTEXT_TASK $masterTid
wtxHandle $slaveHd
wtxContextKill CONTEXT_TASK $slaveTid

# call wtxToolDetach to disconnect from the target servers for both
# the master and slave.

wtxToolDetach $masterHd
wtxToolDetach $slaveHd

