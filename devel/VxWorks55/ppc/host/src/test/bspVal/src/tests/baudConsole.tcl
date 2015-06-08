#
# baudConsole.tcl - console baud rate test
#
# Copyright 1997-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01j,18sep01,sbs  merge from tor2_5.bsp_drv_facility branch for veloce
# 01i,30apr01,agf  Fix \n from test string per SPRs 33071 27682 22425 64277
# 01m,29oct99,sbs  added markup for html docs generation.
# 01l,16oct99,sbs  added extra argument for checkAndLoadFile() call.
# 01k,27jan99,sbs  changed pkPrintTest to pkPrintTest1 for
#                  Tornado 2.0 compatibility (SPR #22425).
# 01j,21apr98,p_b  Merge from ARM to Tornado 2_0_x
# 01i,04sep97,sbs  added target server attach and detach.
# 01h,30aug97,kbw  made minor man page change
# 01g,28aug97,sbs  added looping for shParse in case of error.
# 01f,11aug97,sbs  changed serial macro names.
# 01e,10aug97,kbw  put tS/tE around synopsis
# 01d,10aug97,kbw  minor man page changes
# 01c,31jul97,db   doc tweaks for man page generation.
# 01b,31jul97,sbs  doc added.
# 01a,06jun97,sbs  written.
#
# SYNOPSIS:
# 
# bspVal <options> baudConsole
# 
#
# DESCRIPTION:
# This test verifies the target console's ability to communicate at
# all supported serial baud rates.  It has a run-time of several minutes.
# See 'bspVal.sh' for an explanation of <options>.
#
# \ml
# \m 'NOTE:'
# This reference entry lists the most likely reasons for a test's failure.
# However, it does not list all possible reasons.
# \me
#
# This script consists of one test for each serial baud rate supported
# by both the host and the target.  The host's supported rates are given
# in the macro 'BSPTEST_HOST_RATES'.  The target's supported rates are
# determined by the return value of an ioctl() call to the serial driver.
# A return of 'OK' (0) indicates that the requested baud rate is supported.
# A return of 'ERROR' (-1) indicates that the rate is unsupported.
# Baud rates supported by the target but not by the host cannot be tested.
# The actual communication verification amounts to simply being able to
# check the present baud rate from the shell, and calling ioctl() to set
# the console to the next baud rate.  If any of these tests fail, check
# that the serial device is capable of supporting the given rate.  The
# baud rate generator must be properly initialized and enabled, if present.
# Also check that the serial driver's ioctl() routine performs error checking
# consistent with intended baud rate support.
#
# If the test execution ends prematurely, the target board might be
# left set to a baud rate other than 'T1_SER_BAUD'.  This situation could
# leave the board in such a state that a power cycle reset would be
# necessary to continue with other tests.
#
# Barring serious hardware or software limitations (such as an identifiable
# VxWorks problem), the target board must pass all tests for the BSP to be
# validated.
#
# CONFIGURATION PARAMETERS:
#
# \is
# \i 'BSPTEST_HOST_RATES' 25
# Host supported baud rates (required).
# \i 'T1_SER_DEVICE'
# Serial device to be used on host (required).
# \i 'T1_SER_BAUD'
# Default serial baud rate (required).
# \ie
#
# EXAMPLE:
# Output consists of:
# \cs
#
#                    BSP VALIDATION TEST
#                    -------------------
#
#     Target server                     : t53-160
#     BSP                               : mv147
#     Log file                          : /tmp/bspValidationLog.5219
#
#
#
#     Baud Console Test :
#     console at 150 baud                                       : PASS
#     console at 300 baud                                       : PASS
#     console at 600 baud                                       : PASS
#     console at 1200 baud                                      : PASS
#     console at 1800 baud                                      : PASS
#     console at 2400 baud                                      : PASS
#     console at 4800 baud                                      : PASS
#     console at 9600 baud                                      : PASS
#     console at 19200 baud                                     : PASS
#     console at 38400 baud                                     : PASS
#
#                      Tests RUN           10
#                      Tests PASSED        10
#                      Tests FAILED         0
#                      Tests SKIPPED        0
#
# \ce 
#
# SEE ALSO:
# 'bspVal.sh',
# bspCommonProc.tcl,
# serLib.tcl,
# envLib.tcl,
# pkLib.c
#
# NOROUTINES
#

# print header

printHeader "Baud Console Test"

# attach to the target server

set targetHd [wtxToolAttach $firsttgtsvrname]
wtxRegisterForEvent .*

# load the test routine

checkAndLoadFile $firstbspdir pkLib.o pkGetMacroValue

# set maximum trials to do in case of error

set maxTries 5

# read environment variables

set rates [readEnvRequired BSPTEST_HOST_RATES]
set tipDevice [readEnvRequired T1_SER_DEVICE]
set tipBaud [readEnvRequired T1_SER_BAUD]

send_user "host supported serial baud rates are :  $rates \nhost serial device \
           used is : $tipDevice \ndefault baud rate setting on host is :\
           $tipBaud \n " 0

# call pkGetMacroValue to find CONSOLE_TTY

set cmd " pkGetMacroValue (\"CONSOLE_TTY\") "

set consoleTTY [getWindShRet $cmd $maxTries]

if { $consoleTTY == -1 } {
    fatalError "CONSOLE_TTY macro not defined"
}

send_user "CONSOLE_TTY value is $consoleTTY" 0

# open a file descriptor for the console's serial port

set cmd " open(\"/tyCo/${consoleTTY}\",2,0) "

set fdConsole [getWindShRet $cmd $maxTries]

if {$fdConsole == -1} {
    fatalError "open failed for consoleTTY"
}

# test every baud rate supported by the host

send_user "## starting baud console test ## \n" 0

foreach rate $rates {

    # call ioctl() to change baud rate of console

    set cmd " ioctl($fdConsole,4,$rate) "
    set ret1 [getWindShRet $cmd $maxTries]
    if {$ret1 == -1} {
        testResult 2 "console at $rate baud"
    } else {

        # open serial channel and call pkPrintTest to verify that
        # console is functional at that baud rate

        set fd [serChanOpen $tipDevice $rate]

        set cmd " pkPrintTest1($fdConsole, \"abcdefghijklmnopqrstuvwxyz\\n\", \
                               200) "

        set ret1 [getWindShRet $cmd $maxTries]

        if {$ret1 == -1 } {
            warning "call to run pkPrintTest failed"
            testResult 0 "console at $rate baud"
            continue
        }

        set ret2 [serChanRecv $fd 1024 10 "abcdefghijklmnopqrstuvwxyz" 1]

        send_user "ret value of print test is : [trimSerOutput $ret2]" 0

        # check for results

        if {[string match "timeoutError*" $ret2]} {
            warning "recv timeout during baud console test for rate $rate"
            testResult 0 "console at $rate baud"
        } elseif {[string match "Error" $ret2]} {
            warning "error during baud console test for rate $rate"
            testResult 0 "console at $rate baud"
        } else {
            testResult 1 "console at $rate baud"
        }

#   flush the serial channel and close it

       serChanFlush $fd
       serChanClose $fd
   }
}

# restore default baud rate on console and call pkPrintTest to check

send_user "### restoring default console baud rate ### \n" 0

set ret1 [shParse ioctl($fdConsole,4,$tipBaud)]
if {$ret1 == -1} {
    fatalError "ioctl() call re-setting console baud rate failed"
}

set fd [serChanOpen $tipDevice $tipBaud]

set cmd " pkPrintTest1($fdConsole, \"abcdefghijklmnopqrstuvwxyz\\n\", 200) "

set ret1 [getWindShRet $cmd $maxTries]

if {$ret1 == -1 } {
    fatalError "call to run pkPrintTest failed"
}

set ret2 [serChanRecv $fd 1024 10 "abcdefghijklmnopqrstuvwxyz" 1]

send_user "return value of final print test is [trimSerOutput $ret2]" 0

if {[string match "timeoutError*" $ret2]} {
    fatalError "default baud rate reset failed"
} elseif {[string match "Error" $ret2]} {
    fatalError "default baud rate reset failed"
}

serChanFlush $fd
serChanClose $fd

# detach the target server

wtxToolDetach $targetHd

