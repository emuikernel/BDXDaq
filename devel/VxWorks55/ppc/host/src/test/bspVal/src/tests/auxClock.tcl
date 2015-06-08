#
# auxClock.test - auxiliary clock tests
#
# Copyright 1997-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01m,31oct01,sbs  docs update (SPR 32355)
# 01l,18sep01,sbs  merge from tor2_5.bsp_drv_facility branch for veloce
# 01k,29oct99,sbs  added markup for html docs generation.
# 01j,16oct99,sbs  added extra argument for checkAndLoadFile() call.
# 01i,17oct97,sbs  added code to skip tests if auxclock is not supported.
# 01h,04sep97,sbs  added target server attach and detach.
# 01g,29aug97,sbs  added looping for shParse in case of error and
#                  more log file info.
# 01f,29aug97,kbw  minor changes to man page
# 01f,10aug97,kbw  minor changes to man page, add tS/tE for synopsis
# 01e,10aug97,kbw  minor changes to man page, spell check
# 01d,10aug97,kbw  minor changes to man page, spell check
# 01c,31jul97,db   doc tweaks for man page generation.
# 01b,31jul97,sbs  doc added.
# 01a,15mar97,sbs  written.
#
# SYNOPSIS:
#
# bspVal <options> auxClock
# 
#
# DESCRIPTION:
# This test verifies the functionality of the auxiliary clock for the BSP.
# Its run-time is seven to ten minutes.  See 'bspVal.sh' for an explanation
# of <options>.
#
# The tests within this script connect a routine to the auxiliary clock
# interrupt handler using the sysAuxClkConnect() routine.
# This disconnects any routine previously connected to the auxiliary
# clock ISR by the BSP.
#
# In order to run this test, the target has to be configured with auxiliary
# clock. To do this, add 'INCLUDE_AUXCLK' to config.h file of the BSP or
# include the component 'INCLUDE_AUX_CLK', if using project facility. 
#
# \ml 
# \m 'NOTE:' 
# This reference entry lists the most likely reasons for a test's failure. 
# However, it does not list all possible reasons.
# \me 
#
# Tests 1 through 4 check the accuracy of the auxiliary clock at several
# frequencies: an optional extra rate, minimum, maximum, and the default rate,
# respectively.  To measure the rate of the auxiliary clock, a simple callback
# routine is connected to the auxiliary clock ISR using sysAuxClkConnect().
# This callback routine increments a counter on every clock tick.  The
# counter is then cleared, and the auxiliary clock is enabled at the rate being
# tested.  The counter is read after 10 seconds and again after 130 seconds.
# The counter values are used to calculate the average interrupt rate of the
# auxiliary clock.  Three measurements are taken to cancel the fixed
# portion of measurement latency error.  The computed clock rate error is
# reported with one percent resolution.  If the measured clock rate is more
# than 10 percent off the value being tested, the test for that rate fails.
#
# If any of these tests fail, check that the timer chip is properly initialized
# and is programmed with an appropriate scaling factor, if necessary.
# Interrupts should be enabled in sysAuxClkEnable() before the timer is
# started. Check that the sysAuxClkRoutine is getting connected by
# sysAuxClkConnect(), and that the sysAuxClkRoutine() is being called on every
# auxiliary clock interrupt.
#
# The fifth test verifies the operation of sysAuxClkDisable().  This is done
# by periodically checking the same counter incremented by the first four
# tests.  After sysAuxClkDisable() is called, this counter should not continue
# to increment.  If this test fails, check that the sysAuxClkDisable() routine
# is disabling timer interrupts, turning off the timer, and setting the
# running flag to FALSE.
#
# The sixth test performs parameter checking of the sysAuxClkRateSet() routine.
# This test checks that the proper return value is given for erroneous input
# parameters.  If this test fails, check that sysAuxClkRateSet() performs error
# checking based on the AUX_CLK_RATE_MIN and AUX_CLK_RATE_MAX macros.
#
# The seventh test checks the return values of the sysAuxClkRateSet()
# and sysAuxClkRateGet() routines when first passed valid rates followed by
# erroneous rates.  If this test fails, check that sysAuxClkRateSet() is
# setting the global rate variable, and that sysAuxClkRateGet() is reading
# the same variable.
#
# Barring serious hardware or software limitations (such as an identifiable
# VxWorks problem), the target board must pass all tests for the BSP to be
# validated.
#
# 
# CONFIGURATION PARAMETERS:
#
# \is
# \i 'T1_EXTRA_AUXCLK'  25
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
#     Auxiliary Clock Test :
#     auxClk at 4321 hertz (rate = 4325  error = 0%)            : PASS
#     auxClk at 3 hertz (rate = 3  error = 0%)                  : PASS
#     auxClk at 5000 hertz (rate = 5002  error = 0%)            : PASS
#     auxClk at 60 hertz (rate = 60  error = 0%)                : PASS
#     sysAuxClkDisable() disables auxClk                        : PASS
#     sysAuxClkRateSet() parameter checking                     : PASS
#     sysAuxClkRateGet() return value                           : PASS
#
#                       Tests RUN           7
#                       Tests PASSED        7
#                       Tests FAILED        0
#                       Tests SKIPPED       0
#
# \ce 
# 
# SEE ALSO:
# 'bspVal.sh', cloclkLib.tcl, bspPkCommonProc.tcl, envLib.tcl, pkLib.c
#
# NOROUTINES
#

# print header

printHeader "Auxiliary Clock Test"

# attach to the target server

set targetHd [wtxToolAttach $firsttgtsvrname]
wtxRegisterForEvent .*

# check and load the test routine

checkAndLoadFile $firstbspdir pkLib.o pkGetMacroValue

# check if auxiliary clock is supported on the target

if {[catch {wtxSymFind -name sysAuxClkRateGet} findRes]} {
    if {[string match "*Error*" $findRes]} {
        warning "auxiliary clock not supported on the target"
        testResult 2 "auxClk at extra clock rate"
        testResult 2 "auxClk at minimum clock rate"
        testResult 2 "auxClk at maximum clock rate"
        testResult 2 "auxClk at default clock rate"
        testResult 2 "sysAuxClkDisable() disables auxClk"
        testResult 2 "sysAuxClkRateSet() parameter checking"
        testResult 2 "sysAuxClkRateGet() return value"
        return
    }
}

# read environment variables

set extraAuxClk [readEnvOptional T1_EXTRA_AUXCLK 25]

send_user "optional extra auxClk rate selected is $extraAuxClk" 0

# set maximum trials to make in case of error

set maxTries 5

# find the default rate of the system clock

set cmd "sysAuxClkRateGet()"
set defAuxClk [getWindShRet $cmd $maxTries]

if {$defAuxClk <= 0} {
    fatalError "sysAuxClkRateGet for defAuxClk failed"
}

send_user "dafault auxClk rate is $defAuxClk" 0

#  find the aux clock's min and max rates (defined in BSP)

set cmd " pkGetMacroValue (\"AUX_CLK_RATE_MIN\") "
set minAuxClk [getWindShRet $cmd $maxTries]

if { $minAuxClk <= 0} {
    fatalError "pkGetMacroValue() failed for AUX_CLK_RATE_MIN"
}

send_user "minimum auxClk rate is $minAuxClk" 0

set cmd " pkGetMacroValue (\"AUX_CLK_RATE_MAX\") "
set maxAuxClk [getWindShRet $cmd $maxTries]

if { $maxAuxClk  <= 0} {
    fatalError "pkGetMacroValue() failed for AUX_CLK_RATE_MAX"
}

send_user "maximum auxClk rate is $maxAuxClk" 0

#  connect the test routine and make sure the clock is running

set cmd " clockTicks = 0 "
set ret [getWindShRet $cmd $maxTries]

if { $ret == -1} {
    fatalError "variable initialization failed"
}

set cmd " sysAuxClkConnect (pkIncrement,&clockTicks) "
set ret [getWindShRet $cmd $maxTries]

if { $ret == -1} {
    fatalError "sysAuxClkConnect failed to connect pkIncrement"
}

set cmd " sysAuxClkEnable() "
set ret [getWindShRet $cmd $maxTries]

if { $ret == -1} {
    fatalError "sysAuxClkEnable failed"
}

# test the system clock at minimum, maximum, default and "extra" rates

if { $extraAuxClk > 0} {
    send_user "## Starting auxClk test at $extraAuxClk hertz rate ##" 0
    clockRate AuxClk $extraAuxClk "auxClk"
}

if {( $minAuxClk > 0) && ( $minAuxClk != $extraAuxClk)} {
    send_user "## Starting auxClk test at $minAuxClk hertz rate ##" 0
    clockRate AuxClk $minAuxClk "auxClk"
}

if {( $maxAuxClk > 0) && ( $maxAuxClk !=  $minAuxClk) && \
    ( $maxAuxClk != $extraAuxClk)} {

    send_user "## Starting auxClk test at $maxAuxClk hertz rate ##" 0
    clockRate AuxClk $maxAuxClk "auxClk"
}

if {( $defAuxClk > 0) && ( $defAuxClk != $maxAuxClk) && \
    ( $defAuxClk != $minAuxClk) && ( $defAuxClk != $extraAuxClk)} {

    send_user "## Starting auxClk test at $defAuxClk hertz rate ##" 0
    clockRate AuxClk $defAuxClk "auxClk"

}


# test clock disabling, parameter checking, and return values

clockDisable AuxClk "auxClk"
clockParm AuxClk $minAuxClk $maxAuxClk
clockSetGet AuxClk $defAuxClk

# clean up

set cmd " sysAuxClkDisable() "

set ret [getWindShRet $cmd $maxTries]

if {$ret == -1} {
    fatalError "sysAuxClkDisable() command failed"
}

set cmd " sysAuxClkRateSet($defAuxClk) "

set ret [getWindShRet $cmd $maxTries]

if {$ret == -1} {
    fatalError "sysAuxClkRateSet() command failed"
}

msleep 1000

set cmd " sysAuxClkRateGet "

set ret [getWindShRet $cmd $maxTries]

if {($ret == -1) || ($ret != $defAuxClk)} {
    fatalError "sysAuxClkRateGet() returns error value"
}

# detach the target

wtxToolDetach $targetHd



