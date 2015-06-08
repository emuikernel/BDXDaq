# timestamp.tcl - tests timestamp timer
#
# Copyright 1997-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01o,31oct01,sbs  docs update (SPR 32355)
# 01n,18sep01,sbs  merge from tor2_5.bsp_drv_facility branch for veloce
# 01m,29oct99,sbs  added markup for html docs generation.
# 01l,16oct99,sbs  added extra argument for checkAndLoadFile() call.
# 01k,02dec97,sbs timestamp disable test is skipped if timestamp timer
#                 cannot be stopped. (SPR #9872)
# 01j,20oct97,db  added check for sysTimestamp before start of test.
# 01i,23sep97,db  replaced fatal error with warning when timestamp timer
#                 cannot be stopped. removed check of return code for
#                 sysTimestampFreq() and sysTimestampPeriod()(SPR #9467).
# 01h,04sep97,db  removed END_TEST in sample test output.
# 01g,03sep97,db  added call to sysTimestampDisable() at end of script.
# 01f,29aug97,kbw made man page format fix from jim
# 01e,22aug97,db  replaced calls to shParse with calls to getWindShRet() for
#                 use of retrials.
# 01d,15aug97,kbw made man page format fix
# 01c,14aug97,db  renamed T1_TIP_DEVICE and T1_TIP_BAUD to T1_SER_DEVICE and
#                 T1_SER_BAUD respectively. added retrials on shParse calls.
# 01b,31jul97,db  doc tweaks for man page generation.
# 01a,11jul97,db  written.
#
#
# SYNOPSIS:
# 
# bspVal <options> timestamp
# 
#
# DESCRIPTION:
# This test verifies the BSP timestamp timer functionality.  This entire
# test takes approximately 5 minutes (default timeout value) to run.
# See 'bspVal.sh' for an explanation of <options>.
#
# In order to run this test, the target has to be configured with timestamp 
# timer. To do this, add 'INCLUDE_TIMESTAMP' to config.h file of the BSP or
# include the component 'INCLUDE_TIMESTAMP', if using project facility.
#
# \ml
# \m 'NOTE:'
# This reference entry lists the most likely reasons for a test's failure.
# However, it does not list all possible reasons.
# \me
# 
# The script starts by making sure that the timestamp timer is not running.
# If found running, the function sysTimestampDisable() is called and a
# verification that the timer stopped is made.
#
# The first test verifies that sysTimestampEnable() works. It calls the
# function and does a short term test to check that the timer is incrementing.
#
# The second and third tests verify the long-term timestamp operation using
# sysTimestamp() or sysTimestampLock(). At regular intervals, the tests take
# a number of readings of the timestamp timer.  The value of 'T1_TMO_TIMESTAMP'
# specifies the period of time over which the tests take their readings.
# The 'T1_TMO_SLICES' value specifies the number of readings. The
# number of timestamp ticks for each interval is measured against the
# calculated number. The test fails if the timer error exceeds 1%.
#
# The fourth test verifies that sysTimestampDisable() works. It calls the
# function and verifies that the timer stopped incrementing.
#
# The fifth test verifies that the timer can be reenabled after it was
# disabled. It calls sysTimestampEnable() and verifies that the timer starts
# incrementing.
#
# Barring serious hardware or software limitations (such as an identifiable
# VxWorks problem), the target board must pass all tests for the BSP to be
# validated.
#
# CONFIGURATION PARAMETERS:
# \is
# \i 'T1_BOOT_TN' 30
# Target name (required).
# \i 'T1_TMO_TIMESTAMP'
# Long-term test duration (optional).
# \i 'T1_TMO_SLICES'
# Long-term test time intervals (optional).
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
#     enable the timestamp timer                                : PASS
#     sysTimestamp() long-term test                             : PASS
#     sysTimestampLock() long-term test                         : PASS
#     disable the timestamp timer                               : PASS
#     re-enable after disable                                   : PASS
#
#			Tests RUN           5
#			Tests PASSED        5
#			Tests FAILED        0
#			Tests SKIPPED       0
# \ce
#
# SEE ALSO:
# 'bspVal.sh',
# timestampLib.tcl,
# pkLib.c
#
# NOROUTINES
#
#

# print header

printHeader     "timeStamp"

# read environment variables

set targetTN    [readEnvRequired T1_BOOT_TN]
set targetBaud  [readEnvRequired T1_SER_BAUD]
set targetDev   [readEnvRequired T1_SER_DEVICE]
set libPath 	$firstbspdir

set tmrTMO      [readEnvOptional T1_TMO_TIMESTAMP 300]
set nslices	[readEnvOptional T1_TMO_SLICES 4]

# set retrials on shParse()

set retrials 5

# attach to target server

set targetHd [wtxToolAttach $firsttgtsvrname]
wtxRegisterForEvent .*

# check if pkLib.o is loaded

if {[checkAndLoadFile $libPath pkLib.o pkGetMacroValue] == 1} {
    fatalError "pkLib.o is not found"
}

# check if pkLib.o contains timestamp routines

set skipTest 2
if {[catch {wtxSymFind -name pkTimestamp} result] || \
    [catch {wtxSymFind -name sysTimestamp} result]} {
    warning "There is no timestamp support on the target or pkLib.o\
             is not re-compiled after adding timestamp support"
    testResult $skipTest "enable the timestamp timer"
    testResult $skipTest "sysTimestamp() long-term test"
    testResult $skipTest "sysTimestampLock() long-term test"
    testResult $skipTest "disable the timestamp timer"
    testResult $skipTest "re-enable after disable"
    return
}

# establish serial link to target

set targetFd [serChanOpen $targetDev $targetBaud]

# get the period and frequency of the timestamp timer and verify
# that they are non null.

set ret1 [getWindShRet sysTimestampPeriod() $retrials]
set ret2 [getWindShRet sysTimestampFreq() $retrials]

set tmrPeriod [format %u $ret1]
set tmrFreq [format %u $ret2]
if {($tmrPeriod == 0) || ($tmrFreq == 0)} {
    fatalError "could not compute timer period and/or frequency"
}

# connect the test routine

set ret [getWindShRet {tmrTicks = 0} $retrials]
if {$ret == -1} {
  fatalError "Could not create variable on target"
}

set pTmrTicks [getWindShRet &tmrTicks $retrials]
if {$pTmrTicks == -1} {
    set pTmrTicks 0
}

set tmrConnect [getWindShRet {sysTimestampConnect (pkIncrement,&tmrTicks)} \
                             $retrials]
if {$tmrConnect == -1} {
    set pTmrTicks 0
}

# flag used to check for failure to stop the timer

set TSDisableFail 0

# make sure the timer is not running

set ret [getWindShRet pkTimestampTestShort($pTmrTicks,0) $retrials]
if {$ret == 0} {
    set ret [getWindShRet sysTimestampDisable $retrials]
    if {$ret == -1} {
      warning "Can't disable timestamp timer"
    }
    if {[getWindShRet pkTimestampTestShort($pTmrTicks,0) $retrials] \
         == 0} {
        warning "timestamp timer is running and cannot be stopped"
        set TSDisableFail 1
    }
}

# sysTimestampEnable() test

set passVal 1
for {set ix 0} {($ix < 2) && ($passVal)} {incr ix} {
   if {[getWindShRet sysTimestampEnable $retrials] == -1} {
       set passVal 0
   } else {
       if {[getWindShRet pkTimestampTestShort($pTmrTicks,$ix) $retrials] \
           != 0} {
           set passVal 0
       }
   }
}

testResult $passVal "enable the timestamp timer"

# test long-term timestamp operation


# spawn the test task on the target

set priority 100
set options  0x3
set stackSize 5000

# do long term term timestamp test with call to sysTimestamp()

set tid [shParse taskSpawn("tstamp", $priority, $options, \
   		                $stackSize, pkTimestampTestLong, \
   		                $pTmrTicks, $tmrTMO, 1, $nslices)]

if {$tid == -1} {
    fatalError "Can't spawn task pkTimestampTestLong"
}

set ret [timestampResGet $targetFd $nslices $tmrTMO]

testResult $ret "sysTimestamp() long-term test"

msleep 1000

# do long term term timetstamp test with call to sysTimestampLock()

set tid [shParse taskSpawn("tstamp", $priority, $options, \
   		                $stackSize, pkTimestampTestLong, \
   		                $pTmrTicks, $tmrTMO, 0, $nslices)]

if {$tid == -1} {
    fatalError "Can't spawn task pkTimestampTestLong"
}

set $ret [timestampResGet $targetFd $nslices $tmrTMO]

testResult $ret "sysTimestampLock() long-term test"

# sysTimestampDisable() test

if {$TSDisableFail} {
    send_user "NOTE: sysTimestampDisable test is skipped because timestamp\
               timer cannot be stopped" 1
    testResult 2 "disable the timestamp timer"
} else {
    set ret [getWindShRet sysTimestampDisable $retrials]
    if {$ret != -1} {
        set ret [getWindShRet pkTimestampTestShort($pTmrTicks,0) $retrials]
    }
    testResult [expr {$ret != 0}] "disable the timestamp timer"
}

# re-enable after above disable

set ret [getWindShRet sysTimestampEnable $retrials]

set ret [getWindShRet pkTimestampTestShort($pTmrTicks,0) $retrials]
testResult [expr {$ret == 0}] "re-enable after disable"

# cleanup: disable timestamp timer

if {[getWindShRet sysTimestampDisable $retrials] == -1} {
   if {$TSDisableFail == 0} {
       fatalError "Can't disable timestamp timer"
   }
}

# close serial channel

serChanClose $targetFd

# disconnect from target server

wtxToolDetach $targetHd

