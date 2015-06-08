#
# error1.tcl - error handling tests
#
# Copyright 1997-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01r,12dec01,pch  Fix typo in logfile message
# 01q,18sep01,sbs  merge from tor2_5.bsp_drv_facility branch for veloce
# 01p,29oct99,sbs  added markup for html docs generation.
# 01o,16oct99,sbs  added extra argument for checkAndLoadFile() call.
# 01n,20sep99,sbs  changed script to use new variable defined in the
#                  configuration file (SPR #7804).
# 01m,06jul98,sbs  changed serial error messages (SPR #21050).
# 01l,17oct97,sbs  corrected serial timeoutError and Error string comparision.
# 01k,05sep97,kbw  minor changes to man page
# 01j,04sep97,sbs  added timeout values and minor changes.
# 01i,29aug97,kbw  made a minor man page fix
# 01h,28aug97,sbs  changed bus error tests, now uses only wtxtcl
#                  and reboots allowed.
# 01g,15aug97,kbw  made man page fixes from DAT and Dahmane
# 01f,11aug97,sbs  changed serial macro names.
# 01e,10aug97,kbw  minor man page fixes, spell check
# 01d,06jul97,sbs  doc changes.
# 01c,31jul97,db   doc tweaks for man page generation.
# 01b,31jul97,sbs  doc added.
# 01a,02jun97,sbs  written.
#
# SYNOPSIS:
# 
# bspVal <options> error1
# 
#
# DESCRIPTION:
# This script tests the target's local bus access.  The off-board bus access
# and divide by zero exceptions are initiated from the target shell.
# See 'bspVal.sh' for an explanation of <options>.
#
# \ml
# \m 'NOTE:'
# This reference entry lists the most likely reasons for a test's failure.
# However, it does not list all possible reasons.
# \me
#
# The first and second tests check the bus error exception handling
# functionality (local and off-board addresses, respectively).  If these tests
# fail, check that the memory controller is initialized properly and that
# any memory watchdog timers are configured to reasonable values (if present).
#
# If there is not a local (on-board) memory address that causes a bus
# error when accessed, set the 'T1_LOCAL_ERR_ADRS' macro to
# an invalid off-board address.
#
# The third test causes a divide by zero exception and checks for the return
# messages from the console.
#
# CONFIGURATION PARAMETERS:
# \is
# \i 'T1_LOCAL_ERR_ADRS' 30
# Local error address that causes a bus error (required).
# \i 'T1_OFFBOARD_ERR_ADRS'
# Off-board error address that causes a bus error (required).
# \i 'T1_DIV_ZERO_MSG'
# Bus error message keyword (required).
# \i 'T1_LOCAL_BUS_ERR_MSG'
# Local bus error message keyword (required).
# \i 'T1_OFFBOARD_BUS_ERR_MSG'
# Off-board bus error message keyword (required).
# \i 'T1_SER_DEVICE'
# Serial device on host (required).
# \i 'T1_SER_BAUD'
# Default serial baud rate (required).
# \i 'T1_TMO_BOOT'
# Timeout value for booting VxWorks (required).
# \ie
#
# EXAMPLE
# Output consists of:
# \cs 
#                     BSP VALIDATION TEST
#                     -------------------
#
#     Target server                     : t53-160
#     BSP                               : mv147
#     Log file                          : /tmp/bspValidationLog.5219
#
#
#
#     First Error Test :
#     bus-error test for local address                          : PASS
#     bus-error test for offboard address                       : PASS
#     Divide by zero test                                       : PASS
#
#                       Tests RUN           3
#                       Tests PASSED        3
#                       Tests FAILED        0
#                       Tests SKIPPED       0
#
# \ce
#
# SEE ALSO:
# 'bspVal.sh',
# bspPkCommonProc.tcl,
# serLib.tcl,
# envLib.tcl,
# pkLib.c
#
# NOROUTINES
#
#

# print header

printHeader "First Error Test"

# read the required environment variables

set locErrAdrs [readEnvRequired T1_LOCAL_ERR_ADRS]
set obErrAdrs [readEnvRequired T1_OFFBOARD_ERR_ADRS]
set divideZero [readEnvRequired T1_DIV_ZERO_MSG]
set locBusError [readEnvRequired T1_LOCAL_BUS_ERR_MSG]
set offbdBusError [readEnvRequired T1_OFFBOARD_BUS_ERR_MSG]
set tipDevice [readEnvRequired T1_SER_DEVICE]
set tipBaud [readEnvRequired T1_SER_BAUD]
set bootTimeout [readEnvRequired T1_TMO_BOOT]
set bootline [getBootline T1]

send_user "value of \n local error address is : $locErrAdrs \n\
           off-board error address is : $obErrAdrs \n" 0

send_user "local bus error message is : $locBusError \n\
           offboard bus error message is : $offbdBusError \n\
           divide by zero message is : $divideZero \n\
           bootline is : $bootline \n\
           host serial device is : $tipDevice \n\
           host default serial baud is : $tipBaud \n " 0


# open serial channel

set serId [serChanOpen $tipDevice $tipBaud]
serChanFlush $serId

# reboot

send_user "### Rebooting the target....  ###" 0

rebootTarget $serId $bootline $bootTimeout

msleep 5000

checkTgtSvr $firsttgtsvrname

if {[catch {wtxToolAttach $firsttgtsvrname} attachRes]} {
    if {[string match "*Error*" $attachRes]} {
        fatalError "Cannot attach to the target"
    }
}

wtxRegisterForEvent .*

set p1 0

# load the test routine

checkAndLoadFile $firstbspdir pkLib.o pkGetMacroValue

# test bus-error exception - local address

send_user "## starting local bus error test ##" 0

wtxFuncCall [lindex [wtxSymFind -name pkBusErrorTest] 1] $locErrAdrs
set ret1 [serChanRecv $serId 1024 10 $locBusError 1]

send_user "Serial output of local bus error test is : \n\
           [trimSerOutput $ret1]" 0

if {[string match "timeoutError*" $ret1]} {
    warning "Receive timeout during local bus error test or\
             value specified doesn't cause bus error"
    set p1 1
} elseif {[string match "Error" $ret1] } {
    warning "Receive error during local bus error test"
    set p1 1
}

testResult [expr {$p1==0}] "bus-error test for local error address"

set p1 0

# test bus-error exception - off-board address

send_user "## starting off-board bus error test ##" 0

wtxFuncCall [lindex [wtxSymFind -name pkBusErrorTest] 1] $obErrAdrs
set ret2 [serChanRecv $serId 1024 10 $offbdBusError 1]

send_user "Serial output of off-board error test is : \n\
           [trimSerOutput $ret2]" 0

if {[string match "timeoutError*" $ret2]} {
    warning "Receive timeout during off-board bus error test or\
             value specified doesn't cause bus error"
    set p1 1
} elseif {[string match "Error" $ret2] } {
    warning "Receive error during off-board bus error test"
    set p1 1
}

testResult [expr {$p1==0}] "bus-error test for off-board error address"

set p1 0

# test divide by zero exception

send_user "## starting divide by zero test ##" 0

wtxFuncCall [lindex [wtxSymFind -name pkDivideZeroTest] 1]

set ret4 [serChanRecv $serId 1024 10 $divideZero 1]

send_user "Serial output of divide by zero error is : \n\
           [trimSerOutput $ret4]" 0

if {[string match "timeoutError*" $ret4]} {
    warning "Receive timeout during divide by zero test"
    set p1 1
} elseif {[string match "Error" $ret4] } {
    warning "Receive error during divide by zero test"
    set p1 1
}

testResult [expr {$p1==0}] "Divide by zero test"

# clean up

send_user "### Rebooting the target ###" 0

rebootTarget $serId $bootline $bootTimeout

msleep 5000

checkTgtSvr $firsttgtsvrname

# close serial channel

serChanClose $serId

