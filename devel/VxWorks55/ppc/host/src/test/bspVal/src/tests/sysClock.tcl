#
# sysClock.test - system clock tests
#
# Copyright 1997-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01l,18sep01,sbs  merge from tor2_5.bsp_drv_facility branch for veloce
# 01k,29oct99,sbs  added markup for html docs generation.
# 01j,16oct99,sbs  added extra argument for checkAndLoadFile() call.
# 01i,10jun99,sbs  merge from 2_0_x
# 01h,23feb99,sbs  added code to connect default usrClock() to system
#                  clock as part of clean up (SPR #23019)
# 01g,28aug97,sbs  added looping for shParse in case of error,target
#                  server attach and detach and cleanup.
# 01f,29aug97,kbw  made man page fixes from jim
# 01e,15aug97,kbw  made man page fixes from DAT and Dahmane
# 01d,11aug97,kbw  fixed minor man page problems, spell check
# 01c,31jul97,db   doc tweaks for man page generation.
# 01b,31jul97,sbs  doc added.
# 01a,28mar97,sbs  written.
#
# SYNOPSIS:
# 
# bspVal <options> sysClock
# 
#
# DESCRIPTION:
# This test verifies the functionality of the BSP's system clock.
# The entire test takes approximately 7-10 minutes to run.
# See 'bspVal.sh' for an explanation of <options>.
#
# The tests within this script connect a routine to the system clock interrupt
# handler using the sysClkConnect() routine, which uses the 'sysClkRoutine'
# function pointer variable.  This call to sysClkConnect() for 'sysClkRoutine'
# disconnects any routine the BSP might have previously connected as the system
# clock ISR.
#
# \ml
# \m 'NOTE:'
# This reference entry lists the most likely reasons for a test's failure.
# However, it does not list all possible reasons.
# \me
#
# Tests one through four check the accuracy of the system clock at several
# frequencies: an optional extra rate, minimum, maximum, and the default rate,
# respectively.  To measure the rate of the system clock, a simple callback
# routine is connected to the system clock ISR using sysClkConnect().
# This callback routine increments a counter on every clock tick.  The
# counter is then cleared, and the system clock is enabled at the rate being
# tested.  The counter is read after 10 seconds and again after 130 seconds.
# The counter values are used to calculate the average interrupt rate of the
# system clock.  Three measurements are taken to cancel the fixed
# portion of measurement latency error.  The computed clock rate error is
# reported with one percent resolution.  If the measured clock rate is more
# than 10 percent off the value being tested, the test for that rate fails.
#
# If any of these tests fail, check that the timer chip is properly initialized
# and, if appropriate, that it is programmed with an appropriate scaling
# factor. Interrupts should be enabled in sysClkEnable() before the timer is
# started.  Check that the sysClkRoutine is getting connected
# by sysClkConnect(), and that sysClkRoutine() is called on every system
# clock interrupt.
#
# The fifth test verifies the operation of sysClkDisable().  This is done
# by periodically checking the same counter incremented by the first four
# tests.  After sysClkDisable() is called, this counter should not continue
# to increment.  If this test fails, check that the sysClkDisable() routine
# is disabling timer interrupts, turning off the timer, and setting the
# running flag to FALSE.
#
# The sixth test performs parameter checking of the sysClkRateSet() routine.
# This test checks that the proper return value is given for erroneous input
# parameters.  If this test fails, check that sysClkRateSet() performs error
# checking based on the SYS_CLK_RATE_MIN and SYS_CLK_RATE_MAX macros.
#
# The seventh test checks the return values of the sysClkRateSet()
# and sysClkRateGet() routines when first passed valid rates followed by
# erroneous rates.  If this test fails, check that sysClkRateSet() is setting
# the global rate variable and that sysClkRateGet() is reading the same
# variable.
#
# Barring serious hardware or software limitations (such as an identifiable
# VxWorks problem), the target board must pass all tests for the BSP to be
# validated.
#
# CONFIGURATION PARAMETERS:
# \is
# \i 'T1_EXTRA_SYSCLK' 20
# Another rate to test (optional).
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
#     Log file                          : /tmp/bspValidationLog.5219
#
#
#
#     System Clock Test :
#     sysClk at 4321 hertz (rate = 4325  error = 0%)            : PASS
#     sysClk at 3 hertz (rate = 3  error = 0%)                  : PASS
#     sysClk at 5000 hertz (rate = 5002  error = 0%)            : PASS
#     sysClk at 60 hertz (rate = 60  error = 0%)                : PASS
#     sysClkDisable() disables sysClk                           : PASS
#     sysClkRateSet() parameter checking                        : PASS
#     sysClkRateGet() return value                              : PASS
#
#                       Tests RUN           7
#                       Tests PASSED        7
#                       Tests FAILED        0
#                       Tests SKIPPED       0
#
# \ce
#
# SEE ALSO:
# 'bspVal.sh',
# bspPkCommonProc.tcl,
# envLib.tcl,
# clockLib.tcl,
# pkLib.c
#
# NOROUTINES
#
#

# print header

printHeader "System Clock Test"

# attach to the target server

set targetHd [wtxToolAttach $firsttgtsvrname]
wtxRegisterForEvent .*

# load test routine

checkAndLoadFile $firstbspdir pkLib.o pkGetMacroValue

# set the max trials to do in case of error

set maxTries 5

# read environment variables

set extraSysClk [readEnvOptional T1_EXTRA_SYSCLK 20]

send_user "optional extra sysClk rate selected is $extraSysClk" 0

# find the default rate of the system clock

set cmd " sysClkRateGet() "
set defSysClk [getWindShRet $cmd $maxTries]

if { $defSysClk == -1} {
    fatalError "sysClkRateGet() failed"
}

send_user "default sysClk rate is $defSysClk" 0

# find the system clock's min and max rates (defined in BSP)

set cmd " pkGetMacroValue (\"SYS_CLK_RATE_MIN\") "
set minSysClk [getWindShRet $cmd $maxTries]

if { $minSysClk == -1} {
    fatalError "pkGetMacroValue() failed for SYS_CLK_RATE_MIN"
}

send_user "minimum sysClk rate is $minSysClk" 0

set cmd " pkGetMacroValue (\"SYS_CLK_RATE_MAX\") "
set maxSysClk [getWindShRet $cmd $maxTries]

if { $maxSysClk == -1 } {
    fatalError "pkGetMacroValue() failed for SYS_CLK_RATE_MAX"
}

send_user "maximum sysClk rate is $maxSysClk" 0

# connect the test routine and make sure the clock is running

set cmd " clockTicks=0 "

set ret [getWindShRet $cmd $maxTries]

if {$ret == -1} {
    fatalError "clockTicks variable initialization failed"
}

set cmd " sysClkConnect (pkIncrement,&clockTicks) "
set ret [getWindShRet $cmd $maxTries]

if {$ret == -1} {
    fatalError "sysClkConnect for pkIncrement failed"
}

set cmd " sysClkEnable() "
set ret [getWindShRet $cmd $maxTries]

if {$ret == -1} {
    fatalError "function call to sysClkEnable failed"
}

# test the system clock at minimum, maximum, default and "extra" rates

if { $extraSysClk > 0} {
    send_user " ## starting sysClk test at $extraSysClk hertz rate ## " 0
    clockRate Clk $extraSysClk "sysClk"
}

if {( $minSysClk > 0) && ( $minSysClk != $extraSysClk)} {
    send_user " ## starting sysClk test at $minSysClk hertz rate ## " 0
    clockRate Clk $minSysClk "sysClk"
}

if {( $maxSysClk > 0) && ( $maxSysClk != $minSysClk) && \
    ( $maxSysClk != $extraSysClk)} {
    send_user " ## starting sysClk test at $maxSysClk hertz rate ## " 0
    clockRate Clk $maxSysClk "sysClk"
}

if {( $defSysClk > 0) && ( $defSysClk != $maxSysClk) && \
    ( $defSysClk != $minSysClk) && ( $defSysClk != $extraSysClk)} {
    send_user " ## starting sysClk test at $defSysClk hertz rate ## " 0
    clockRate Clk $defSysClk "sysClk"
}

# test clock disabling, parameter checking, and return values

clockDisable Clk "sysClk"
clockParm Clk $minSysClk $maxSysClk
clockSetGet Clk $defSysClk

# clean up

set cmd " sysClkDisable() "

set ret [getWindShRet $cmd $maxTries]

if {$ret == -1} {
    fatalError "sysClkDisable() command failed"
}

# attach default usrClock() to system clock

set cmd " sysClkConnect(usrClock, 0) "

set ret [getWindShRet $cmd $maxTries]

if {$ret == -1} {
    fatalError "sysClkConnect() to usrClock command failed"
}

# set the clock rate to default clk rate before the test

set cmd " sysClkRateSet($defSysClk) "

set ret [getWindShRet $cmd $maxTries]

if {$ret == -1} {
    fatalError "sysClkRateSet() command failed"
}

msleep 1000

set cmd " sysClkRateGet "

set ret [getWindShRet $cmd $maxTries]

if {($ret == -1) || ($ret != $defSysClk)} {
    fatalError "sysClkRateGet() returns error value"
}


set cmd " sysClkEnable() "

set ret [getWindShRet $cmd $maxTries]

if {$ret == -1} {
    fatalError "sysClkEnable() command failed"
}

# detach the target server

wtxToolDetach $targetHd




