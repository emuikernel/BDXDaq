#
# bootline.tcl - functionality test for bootline
#
# Copyright 1997-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01o,29nov01,rec  update documentation
# 01n,18sep01,sbs  merge from tor2_5.bsp_drv_facility branch for veloce
# 01m,29oct99,sbs  added markup for html docs generation.
# 01l,16oct99,sbs  added extra argument for checkAndLoadFile() call.
# 01k,20sep99,sbs  changed script to use new variables for local and
#                  offboard error tests (SPR #7804).
# 01j,26jul98,sbs  changed serial timeout for ctrl-x test
#                  added more log file info for boot commands test.
#                  (SPR #21944)
# 01i,17oct97,sbs  added more error checking and corrected bootline change
#                  to for slow targets.
# 01h,29aug97,sbs  more log file info and minor changes.
# 01g,29aug97,kbw  made a minor man page fix
# 01f,11aug97,sbs  changed serial macro names.
# 01e,10aug97,kbw  fixed minor man page problems, did a spell check
# 01d,06jul97,sbs  doc changes.
# 01c,31jul97,db   doc tweaks for man page generation.
# 01b,31jul97,sbs  doc added.
# 01a,05may97,sbs  written.
#
#
# SYNOPSIS:
# 
# bspVal <options> bootline
#
#
# DESCRIPTION:
# This test verifies the commands executable from the VxWorks boot
# ROM prompt.  It has a run-time of up to a few minutes.
# See 'bspVal.sh' for an explanation of <options>.
#
# \ml
# \m 'NOTE:'
# This reference entry lists the most likely reasons for a test's failure.
# However, it does not list all possible reasons.
# \me
#
# The first test reboots the target while sending characters to the serial
# console, causing possible problems with pending serial interrupts.
# If this test fails, check that CPU interrupts are properly disabled in
# romInit(), a routine defined in romInit.s, and that all serial device
# interrupts are disabled and cleared in the 'sysLib'
# routine, sysSerialHwInit().
#
# The second and third tests check the bus error exception handling
# functionality (local and off-board addresses, respectively).  If these tests
# fail, check that the memory controller is initialized properly and that
# any memory watchdog timers are configured to reasonable values (if present).
#
# If there is not a local (on-board) memory address that can cause a bus
# error when accessed, the macro 'T1_BOOT_LOCAL_ERR_ADRS' should be set to an
# invalid off-board address.
#
# The fourth test checks that the bootline commands and booting mechanisms
# all act as expected.  This test can fail for a variety of reasons.  Check
# that the memory controller is set up properly and that the network
# connection is attached.
#
# This test script does not work properly with a VxWorks standalone image
# in ROM.  If the target can execute VxWorks only out of ROM, this test
# script should not be part of the BSP validation procedure.
#
# Make sure that all the boot line parameters in the configuration file
# represent the actual target boot parameters. Failing to do so can leave
# target in a state where a power cycle reset is necessary with change in
# boot parameters.
#
# Barring serious hardware or software limitations (such as an identifiable
# VxWorks problem), the target board must pass all tests for the BSP to be
# validated.
#
# CONFIGURATION PARAMETERS:
#
# \is
# \i 'T1_BOOT_LOCAL_ERR_ADRS' 30
# Local memory address that causes bus error. (required)
# \i 'T1_BOOT_OFFBOARD_ERR_ADRS'
# Off-board address that causes bus error. (required)
# \i 'T1_BOOT_LOCAL_BUS_ERR_MSG'
# Local bus error message keyword. (required)
# \i 'T1_BOOT_OFFBOARD_BUS_ERR_MSG'
# Off-board bus error message keyword. (required)
# \i 'T1_SER_DEVICE'
# Serial device to be used on host. (required)
# \i 'T1_SER_BAUD'
# Default serial baud rate. (required)
# \i 'T1_TMO_BOOT'
# Timeout value for booting vxWorks. (required)
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
#     Bootline Test :
#     Control X test                                            : PASS
#     Bus error test for local error address                    : PASS
#     Bus error test for off-board error address                : PASS
#     boot commands test                                        : PASS
#
#                       Tests RUN           4
#                       Tests PASSED        4
#                       Tests FAILED        0
#                       Tests SKIPPED       0
#
# \ce
#
# SEE ALSO:
#
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

printHeader "Bootline Test"

# attach to the target server to run the test

if {[catch {set handle [wtxToolAttach $firsttgtsvrname]} res1]} {
    fatalError "target server not found"
}
wtxRegisterForEvent .*

# load the test routine

checkAndLoadFile $firstbspdir pkLib.o pkGetMacroValue

# get the required configuration parameters

set modAdrs $envArray(EXC_MSG_ADRS)

if {$modAdrs == -1} {
    fatalError "EXC_MSG_ADRS macro not defined"
}

# read the required environment variables

set locErrAdrs [readEnvRequired T1_BOOT_LOCAL_ERR_ADRS]
set obErrAdrs [readEnvRequired T1_BOOT_OFFBOARD_ERR_ADRS]
set locBusError [readEnvRequired T1_BOOT_LOCAL_BUS_ERR_MSG]
set obBusError [readEnvRequired T1_BOOT_OFFBOARD_BUS_ERR_MSG]
set bootline [getBootline T1]
set tipDevice [readEnvRequired T1_SER_DEVICE]
set tipBaud [readEnvRequired T1_SER_BAUD]
set bootTimeout [readEnvRequired T1_TMO_BOOT]

send_user "value of \n local error address is : $locErrAdrs \n\
           off-board error address is : $obErrAdrs \n" 0

send_user " local bus error message is : $locBusError \n\
           off-board bus error message is : $obBusError \n\
           bootline is : $bootline \n\
           host serial device is : $tipDevice \n\
           host default serial baud is : $tipBaud \n\
           timeout value for reboot is : $bootTimeout \n " 0

# open serial channel

set fd [serChanOpen $tipDevice $tipBaud]
serChanFlush $fd

# set the required variables

set return "\r"
set controlX "\n"
set garbage "a\nb\nc\nd\nefdhsjgfhd"

# test reboot using ctrl-x while sending input characters from the bootline

serChanSend $fd $controlX
serChanSend $fd $garbage

set autoResult [serChanRecv $fd 2096 $bootTimeout "auto-boot..." 1]

serChanSend $fd $return
serChanFlush $fd

send_user "serial output for control-X test is :\
           [trimSerOutput $autoResult] \n " 0

set done 0

if {[string match "timeoutError*" $autoResult]} {
    if {[string match "*\[VxWorks Boot]:*" $autoResult] == 0} {
        warning "Receive timeout during control X test"
        set done 1
    }
} elseif {[string match "Error" $autoResult]} {
    set done 1
}

testResult [expr {$done != 1}] "Control X test"

# get the boot prompt

send_user "### Trying to get boot prompt......  ### \n" 0

getBootPrompt $fd $bootTimeout

# test bus-error exception from the boot line - local address

send_user "## starting local bus error test ## \n" 0

serChanFlush $fd
serChanSend $fd "d $locErrAdrs\n"
set locErrRet [serChanRecv $fd 4096 10 "$locBusError" 1]

send_user "serial output for local bus error is :\
           [trimSerOutput $locErrRet]" 0

# check the result

if {([string match "timeoutError*" $locErrRet]) || \
    ([string match "Error" $locErrRet]) || \
    ([string match "*$locBusError*" $locErrRet] ==0) } {
    warning "T1_BOOT_LOCAL_ERR_ADRS did not cause a bus error"
    testResult 0 "Bus error test for local error address"
} else {
    testResult 1 "Bus error test for local error address"
}


# timeout to get the boot prompt

for {set i 0} {$i < 30} {incr i} {
    msleep 1000
}

# test bus-error exception from the boot line - offboard address

send_user "## starting off-board bus error test ## \n" 0

serChanFlush $fd
serChanSend $fd "d $obErrAdrs \n"

set obErrRet [serChanRecv $fd 4096 10 "$obBusError" 1]

send_user "serial output for off-board bus error is :\
           [trimSerOutput $obErrRet] \n" 0

# check the result

if {([string match "timeoutError*" $obErrRet]) || \
    ([string match "Error" $obErrRet]) || \
    ([string match "*$obBusError*" $obErrRet] == 0) } {
    warning "T1_BOOT_OFFBOARD_ERR_ADRS did not cause a bus error"
    testResult 0 "Bus error test for off-board error address"
} else {
    testResult 1 "Bus error test for off-board error address"
}

# timeout to get the boot prompt

for {set i 0} {$i < 30} {incr i} {
    msleep 1000
}

# test boot line commands

send_user "## starting  boot line commands test ## \n" 0

serChanFlush $fd
serChanSend $fd "? \n"
set ret1 [serChanRecv $fd 4096 20 "Boot flags" 1]

if {[string match "timeoutError*" $ret1] == 0 && \
    [string match "Error" $ret1] == 0} {
    send_user "serial output of \"?\" command was :\n \
               [trimSerOutput $ret1]\n " 0
    serChanFlush $fd
    serChanSend $fd "p \n"
    set ret1 [serChanRecv $fd 2048 20 "boot device" 1]
} else {
    send_user "Error during output of command \"?\" " 0
    send_user "serial output was :\n [trimSerOutput $ret1]\n " 0
}

if {[string match "timeoutError*" $ret1] == 0 && \
    [string match "Error" $ret1] == 0} {
    send_user "serial output of \"p\" command was :\n \
               [trimSerOutput $ret1]\n " 0
    serChanFlush $fd
    serChanSend $fd "e \n"
    set ret1 [serChanRecv $fd 8192 20 "\[VxWorks Boot]:" 1]
} else {
    send_user "Error during output of command \"p\" " 0
    send_user "serial output was :\n [trimSerOutput $ret1]\n " 0
}

if {[string match "timeoutError*" $ret1] == 0 && \
    [string match "Error" $ret1] == 0} {
    send_user "serial output of \"e\" command was :\n \
               [trimSerOutput $ret1]\n " 0
    serChanFlush $fd
    serChanSend $fd "f $modAdrs,1,0x5a\n"
    msleep 5000
    set ret1 [serChanRecv $fd 2048 20 "\[VxWorks Boot]:" 1]
} else {
    send_user "Error during output of command \"e\" " 0
    send_user "serial output was :\n [trimSerOutput $ret1]\n " 0
}

if {[string match "timeoutError*" $ret1] == 0 && \
    [string match "Error" $ret1] == 0} {
    send_user "serial output of command \"f $modAdrs,1,0x5a\"\
               was :\n [trimSerOutput $ret1]\n " 0
    serChanFlush $fd
    serChanSend $fd "f $modAdrs,1,0xa5\n"
    msleep 5000
    set ret1 [serChanRecv $fd 2048 20 "\[VxWorks Boot]:" 1]
} else {
    send_user "Error during output of command \
               \"f $modAdrs,1,0x5a\" " 0
    send_user "serial output was :\n [trimSerOutput $ret1]\n " 0
}

if {[string match "timeoutError*" $ret1] == 0 && \
    [string match "Error" $ret1] == 0} {
    send_user "serial output of command \"f $modAdrs,1,0xa5\" \
               was :\n [trimSerOutput $ret1]\n " 0
    serChanFlush $fd
    serChanSend $fd "d $modAdrs,1\n"
    set ret1 [serChanRecv $fd 2048 20 "a5" 1]
} else {
    send_user "Error during output of command \
               \"f $modAdrs,1,0xa5\" " 0
    send_user "serial output was :\n [trimSerOutput $ret1]\n " 0
}

if {[string match "timeoutError*" $ret1] == 0 && \
    [string match "Error" $ret1] == 0} {
    send_user "serial output of command \"d $modAdrs,1\"\
               was :\n [trimSerOutput $ret1]\n " 0
    serChanFlush $fd
    serChanSend $fd "m $modAdrs\n55aa\n.\n"
    msleep 5000
    set ret1 [serChanRecv $fd 2048 20 "\[VxWorks Boot]:" 1]
} else {
    send_user "Error during output of command \
               \"d $modAdrs,1\" " 0
    send_user "serial output was :\n [trimSerOutput $ret1]\n " 0
}

if {[string match "timeoutError*" $ret1] == 0 && \
    [string match "Error" $ret1] == 0} {
    send_user "serial output of command \"m $modAdrs 55aa\"\
               was :\n [trimSerOutput $ret1]\n " 0
    serChanFlush $fd
    serChanSend $fd "d $modAdrs,1\n"
    set ret1 [serChanRecv $fd 2048 20 "55aa" 1]
} else {
    send_user "Error during output of command \
               \"m $modAdrs\" " 0
    send_user "serial output was :\n [trimSerOutput $ret1]\n " 0
}

if {[string match "timeoutError*" $ret1] == 0 && \
    [string match "Error" $ret1] == 0} {
    send_user "serial output of command \"d $modAdrs,1\"\
               was :\n [trimSerOutput $ret1]\n " 0
    serChanFlush $fd
    serChanSend $fd "\n"
    msleep 1000
    serChanSend $fd "c\n"
    serChanFlush $fd

    foreach param $bootline {
        if { $param == "" } {
            serChanSend $fd ".\n"
        } else {
            serChanSend $fd "${param}\n"
        }
    }

    for {set i 0} {$i < 30} {incr i} {
        msleep 1000
    }

    set ret1 [serChanRecv $fd 8192 $bootTimeout "\[VxWorks Boot]:" 1]

    send_user "serial output after \"c\" command was :\
               \n [trimSerOutput $ret1]\n " 0

    if {[string match "timeoutError*" $ret1] || \
        [string match "Error" $ret1]} {
        send_user "Serial receive error while getting boot prompt \
                   after loading boot parameters" 0
        send_user "serial output was : \n [trimSerOutput $ret1]\n" 0
    }

    serChanFlush $fd
    serChanSend $fd "@\n"
    set ret1 [serChanRecv $fd 8192 $bootTimeout "WDB: Ready." 1]

    send_user "serial output after \"@\" command was :\
               \n [trimSerOutput $ret1]\n" 0

    if {[string match "timeoutError*" $ret1] || \
        [string match "Error" $ret1]} {
        send_user "Serial receive error while getting reboot pattern" 0
        send_user "serial output was : \n [trimSerOutput $ret1]\n" 0
    }

    getBootPrompt $fd $bootTimeout
} else {
    send_user "Error during output of command \
               \"d $modAdrs,1\" " 0
    send_user "serial output was :\n [trimSerOutput $ret1]\n " 0
}

if {[string match "timeoutError*" $ret1] == 0 && \
    [string match "Error" $ret1] == 0} {
    serChanFlush $fd
    serChanSend $fd "l\n"

    for {set i 0} {$i < 30} {incr i} {
        msleep 1000
    }

    set ret1 [serChanRecv $fd 4096 $bootTimeout "\[VxWorks Boot]:" 1]

    if {[string match "timeoutError*" $ret1] || \
        [string match "Error" $ret1]} {
        send_user "Error during output of command \"l\" " 0
        send_user "serial output was :\n [trimSerOutput $ret1]\n " 0
    } else {

        serChanFlush $fd

        send_user "serial output while loading vxWorks image was: \n \
                  [trimSerOutput $ret1]" 0

        # extract the value of entry
        set loadPat [trimSerOutput $ret1]

        while {[set res1 [lsearch $loadPat "entry"]] != -1} {
            if {[string match "=" [lindex $loadPat [expr {$res1+1}]]]} {
                set res2 [lindex $loadPat [expr {$res1+2}]]
                break
            } else {
                set loadPat [lrange $loadPat $res1 end]
                continue
            }
        }

        send_user "entry is : $res2" 0

        serChanSend $fd "g $res2\n"

        set ret1 [serChanRecv $fd 8192 $bootTimeout "WDB: Ready." 1]

        send_user "serial output while executing \"g $res2\" was: \n \
                   [trimSerOutput $ret1]" 0
    }
}

# check the result

testResult [expr { [string match "timeoutError*" $ret1] == 0 && \
                   [string match "Error" $ret1] == 0 }] \
           "boot commands test"

serChanFlush $fd

# detach from the target server

if {[catch {wtxToolDetach $handle} detRes]} {
    send_user "wtxToolDetach : target server restarting \n" 0
}

# clean up

# reboot target and restart target server

send_user "### Rebooting target ....  ### \n" 0

rebootTarget $fd $bootline $bootTimeout

msleep 5000

checkTgtSvr $firsttgtsvrname

serChanClose $fd

