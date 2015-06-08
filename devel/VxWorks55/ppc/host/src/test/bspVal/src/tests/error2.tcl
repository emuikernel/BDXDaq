#
# error2.tcl - error handling tests
#
# Copyright 1997-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01q,20jan02,dat  Fixing the timeout
# 01p,18sep01,sbs  merge from tor2_5.bsp_drv_facility branch for veloce
# 01o,27jun01,agf  fix logic that tests for presence of auxClk
# 01r,29oct99,sbs  added markup for html docs generation.
# 01q,16oct99,sbs  added extra argument for checkAndLoadFile() call.
# 01p,07sep99,sbs  changed script to use specific error message for
#                  catastrophic error test (SPR #7804).
#                  corrected the sequence to skip the test if aux clk
#                  is not present (SPR #22642).
#                  changed script to fix catastrophic test (SPR #27822).
# 01o,10jun99,sbs  merge from 2_0_x
# 01n,23feb99,sbs  added changes to make VTS compatible to
#                  Tornado 2.0. (SPR #22425).
# 01m,06jul98,sbs  changed timeout values while waiting for target reboot and
#                  fixed minor documentaion errors.(SPR #21050)
# 01l,21apr98,p_b  Merge from ARM to Tornado 2_0_x
# 01k,17oct97,sbs  changed funcCall return value checking and check for
#                  auxClock availability added.
# 01j,04sep97,sbs  added timeout values and minor changes.
# 01i,29aug97,kbw  made a minor man page fix
# 01h,28aug97,sbs  more log file info added.
# 01g,15aug97,kbw  made man page fixes from DAT and Dahmane
# 01f,11aug97,sbs  changed serial macro names.
# 01e,10aug97,kbw  minor man page changes and spell check
# 01d,06jul97,sbs  doc changes.
# 01c,01aug97,db   doc tweaks for man page generation.
# 01b,31jul97,sbs  doc added.
# 01a,08jul97,sbs  written.
#
#
# SYNOPSIS:
# 
# bspVal <options> error2
# 
#
# DESCRIPTION:
# This test verifies that the target board does not hang while
# handling errors initiated from the VxWorks shell.
# The entire test can take several minutes to run.
# See 'bspVal.sh' for an explanation of <options>.
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
# The first test connects a routine that causes a bus error to the
# target's system clock interrupt.  This causes an exception at interrupt
# level, known as a "catastrophic" error.  This exception should result in
# a reboot of the board.  An appropriate error message should be displayed
# by the target.  If this test fails, check that the sysToMonitor() routine
# in sysLib.c is functioning properly.  Also check that the address
# of 'sysExcMsg' is set correctly in sysLib.c.
#
# The second test reboots the target from the shell while characters are sent to
# the serial console and the auxiliary clock is enabled at the maximum rate.
# This checks that pending interrupts do not cause problems while rebooting.
# If this test fails, check that CPU interrupts are properly disabled in
# romInit(), which is defined in romInit.s, and that all serial device
# interrupts are disabled and cleared in sysSerialHwInit().  Also, check that
# the auxiliary clock routine sysAuxClkRoutine() is initialized to NULL and,
# if it is NULL, that the auxiliary clock is not called in sysAuxClkInt().
#
# Make sure that all the boot-line parameters in the configuration file
# represent the actual target boot parameters. Failing to do so could leave
# target in a state where a power cycle reset would be necessary with change
# in boot parameters.
#
# Barring serious hardware or software limitations (such as an identifiable
# VxWorks problem), the target board must pass all tests for the BSP to be
# validated.
#
# CONFIGURATION PARAMETERS:
# \is
# \i 'T1_OFFBOARD_ERR_ADRS' 30
# Off-board address that causes a bus error. (required)
# \i 'T1_CATASTROPHIC_ERR_MSG'
# catastrophic error message keyword (required)
# \i 'T1_SER_DEVICE'
# Serial device that is used on host. (required)
# \i 'T1_SER_BAUD'
# Serial baud rate used. (required)
# \i 'T1_TMO_BOOT'
# Timeout value for booting vxWorks. (required)
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
#     Second Error Test :
#     catastrophic error test                                   : PASS
#     reboot with interrupts                                    : PASS
#
#                       Tests RUN           2
#                       Tests PASSED        2
#                       Tests FAILED        0
#                       Tests SKIPPED       0
#
# \ce 
#
# SEE ALSO:
# 'bspVal.sh',
# bspPkCommonProc.tcl,
# serLib.tcl,
# envLib.tcl
#
# NOROUTINES
#
#

# print header

printHeader     "Second Error Test"

# read environment variables

set obErrAdrs [readEnvRequired T1_OFFBOARD_ERR_ADRS]
set bootline [getBootline T1]
set tipDevice [readEnvRequired T1_SER_DEVICE]
set tipBaud [readEnvRequired T1_SER_BAUD]
set catastrophicError [readEnvRequired T1_CATASTROPHIC_ERR_MSG]
set bootTimeout [readEnvRequired T1_TMO_BOOT]

send_user " off-board error address is : $obErrAdrs \n\
           bootline is : $bootline \n\
           host serial device is : $tipDevice \n\
           host default serial baud is : $tipBaud \n " 0

# attach to target server

if {[catch {set handle1 [wtxToolAttach $firsttgtsvrname]} res1]} {
    fatalError "target server not found"
}

wtxRegisterForEvent .*

# load test support code

checkAndLoadFile $firstbspdir pkLib.o pkGetMacroValue

# setup initial environment

set fd [serChanOpen $tipDevice $tipBaud]

set maxAuxClk $envArray(AUX_CLK_RATE_MAX)

send_user "maximum auxClk rate is $maxAuxClk" 0

# test catastrophic error handling

send_user "## starting catastrophic error test ## \n " 0

set res -1

set entry1 [symbol pkCatastrophicTest]
set entry2 [symbol pkCatastrophicClean]

if {[catch {wtxFuncCall $entry1 [format "%#x" $obErrAdrs]} catErrRes]} {
    if {[catch {wtxFuncCall $entry2} catCleanErr]} {
        send_user "Note: Clean-up of catastrophic error test failed\n\
                   Please wait until the test is complete\n" 0
    }
    warning "Call to function pkCatastrophicTest failed"
    send_user "The error is \n\n $catErrRes \n" 0
}

# make sure the target rebooted

set errResult [serChanRecv $fd 2048 $bootTimeout $catastrophicError 1]

send_user "Serial ouput of catastrophic error is :\n \
            [trimSerOutput $errResult] \n" 0

if {[string match "timeoutError*" $errResult]} {
    warning "Serial receive timeout during catastrophic error test"
    set res -1
} elseif {[string match "Error" $errResult]} {
    warning "Serial error during catastrophic error test"
    set res -1
} else {
    set res 0
}

serChanFlush $fd

set autoBoot "auto-boot.."

set autoResult [serChanRecv $fd 4096 $bootTimeout $autoBoot 1]

serChanSend $fd "\r"
serChanFlush $fd

send_user "Serial channel output waiting target reboot is: \n \
           [trimSerOutput $autoResult] \n" 0

if {[string match "timeoutError*" $autoResult]} {
    if {[string match "*\[VxWorks Boot]:*" $autoResult] == 0} {
         warning "Serial receive timeout during catastrophic error test \
                  waiting for target reboot"
         set res -1
    }
} elseif {[string match "Error" $autoResult]} {
    warning "Serial error during catastrophic error test"
    set res -1
}

serChanFlush $fd

testResult [expr {$res == 0}] "catastrophic error test"

if {[catch {wtxToolDetach $handle1} detRes]} {
     send_user "wtxToolDetach : target server restarting" 0
}

send_user "### Target reboot... ###" 0

rebootTarget $fd $bootline $bootTimeout

msleep 5000

checkTgtSvr $firsttgtsvrname

# flush the serial channel

serChanFlush $fd

# attach to the target server and register for events

if {[catch {set handle2 [wtxToolAttach $firsttgtsvrname]} res1]} {
    fatalError "cannot attach to the target server"
}
wtxRegisterForEvent .*

# test reboot using ctrl-x with aux clock and serial interrupts

send_user "## starting reboot test with auxClk and serial\
           interrupts ##" 0

set res1 0

# check for aux clock and AUX_CLK_RATE_MAX macro availability

if {[catch "wtxSymFind -name sysAuxClkRateGet" findRes]} {
    if {[string match "*Error*" $findRes]} {
        warning "auxClk not supported on this target"
        testResult 2 "reboot with interrupts"
        return
    }
} elseif {$maxAuxClk == -1} {
    warning "AUX_CLK_RATE_MAX macro not defined"
    testResult 0 "reboot with interrupts"
    return
}

# enable aux clock at max rate

if {[catch "wtxFuncCall -int [symbol sysAuxClkDisable]" callRes]} {
    warning "call to function sysAuxClkDisable failed"
    set res1 -1
}

if {$maxAuxClk > 0} {
    if {[catch "wtxFuncCall -int [symbol sysAuxClkRateSet] \
        $maxAuxClk" callRes]} {
        warning "call to function sysAuxClkRateSet failed"
        set res1 -1
    }
} else {
    warning "AUX_CLK_RATE_MAX macro has zero or negative value"
    set res1 -1
}

if {[catch "wtxFuncCall -int [symbol sysAuxClkEnable]" callRes]} {
    warning "call to function sysAuxClkEnable failed"
    set res1 -1
}

set garbage "abcdefgabcdefghijklmnopqrstuvwxyz1234567890|<>,\Cu\r"

serChanSend $fd "$garbage\n"

set bootResult [serChanRecv $fd 2048 $bootTimeout "auto-boot..." 1]

serChanSend $fd "\r"
serChanFlush $fd

send_user "reboot result is : \n [trimSerOutput $bootResult] \n" 0

if {[string match "timeoutError*" $bootResult]} {
    if {[string match "*\[VxWorks Boot]:*" $bootResult] == 0} {
        send_user "Serial receive timeout during reboot with interrupts" 0
        set res1 -1
    }
} elseif {[string match "Error" $bootResult]} {
    send_user "Serial error during reboot with interrupts" 0
    set res1 -1
}

testResult [expr {$res1 == 0}] "reboot with interrupts"

# clean up

if {[catch {wtxToolDetach $handle2} detRes]} {
    send_user "wtxToolDetach : target server restarting" 0
}

send_user "### Target reboot ### \n" 0

rebootTarget $fd $bootline $bootTimeout

msleep 5000

checkTgtSvr $firsttgtsvrname

serChanClose $fd


