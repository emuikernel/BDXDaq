#
# scsi.tcl - SCSI test
#
# Copyright 1997-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01t,31oct01,sbs  docs update (SPR 32355)
# 01s,18sep01,sbs  merge from tor2_5.bsp_drv_facility branch for veloce
# 01r,12oct99,sbs  modified changes made for SPR #22425 fix.
# 01q,29oct99,sbs  added markup for html docs generation.
# 01p,16oct99,sbs  added extra argument for checkAndLoadFile() call.
# 01o,10jun99,sbs  merge from 2_0_x
# 01n,23feb99,sbs  added changes to make VTS compatible with
#                  Tornado 2.0 (SPR #22425).
# 01m,17oct97,sbs  added check for pkTestOneUnit before skipping the test.
# 01l,17oct97,sbs  changed serial timeouts and error checking(SPR #9470).
# 01j,05sep97,kbw  made a minor man page fix
# 01j,04sep97,sbs  added target server attach and detach.
# 01i,29aug97,kbw  made man page fixes from jim
# 01h,28aug97,sbs  added looping for shParse in case of error.
# 01g,16aug97,kbw  made man page fixes from DAT and Dahmane
# 01f,aug1197,kbw  fixed minor man page problems, spell check
# 01e,11aug97,sbs  changed serial macro names.
# 01d,06jul97,sbs  doc changes.
# 01c,31jul97,db   doc tweaks for man page generation.
# 01b,31jul97,sbs  doc added.
# 01a,14apr97,sbs  written.
#
# SYNOPSIS:
# 
# bspVal <options> scsi
# 
#
# DESCRIPTION:
# This test verifies SCSI read and write operations.
# See 'bspVal.sh' for an explanation of <options>.
#
# In order to run this test, the target has to be configured for SCSI 
# support. The target has to be configured for scsi show routines either
# by defining 'INCLUDE_SCSI_SHOW' in config.h of the BSP or adding component
# 'INCLUDE_SCSI_SHOW', if using project facility. 
#
# \ml
# \m 'WARNING:'
# All data on the SCSI device under test is overwritten.
# \me
# 
# The execution time of the test run by this script depends on the
# size and speed of SCSI device being tested, the CPU speed, and the test
# buffer size ('T1_SCSI_BUFSIZE').  A small buffer slows the test.
# For particularly slow targets, slow devices, large SCSI devices, and/or small
# buffers, it might be necessary to increase the timeout value specified by the
# macro 'T1_TMO_SCSI'.  The entire script could take more than one hour to
# complete.
#
# For target boards with limited RAM, it might be necessary to reduce
# 'T1_SCSI_BUFSIZE' from its default of 64 kilobytes.  The buffer size
# must be a multiple of the block size of the device under test.
#
# \ml
# \m 'NOTE:'
# This reference entry lists the most likely reasons for a test's failure.
# However, it does not list all possible reasons.
# \me
#
# The first test verifies the return value of scsiShow() routine. If this test
# fails, it is assumed that SCSI is not configured properly and the next
# test (for read/write operations on the device) is skipped.
#
# The second test calls pkTestOneUnit() to test the SCSI device
# specified by 'T1_SCSI_ID' and 'T1_SCSI_LUN'. A pattern
# is written to every block of the device, then each block is read to
# verify the pattern.  If this test fails, check that the SCSI ID and LUN
# are properly configured and that the device appears in the output from
# the scsiShow() command.
#
# Barring serious hardware or software limitations (such as an identifiable
# VxWorks problem), the target board must pass all tests for the BSP to be
# validated.  Of course, if SCSI is not supported by the target board, then
# SCSI testing is not a BSP validation requirement.
#
# CONFIGURATION PARAMETERS:
# \is
# \i 'T1_SER_DEVICE' 20
# Serial device used for target.
# \i 'T1_SER_BAUD'
# Serial baud rate used for target.
# \i 'T1_SCSI_ID'
# SCSI ID of the device under test.
# \i 'T1_SCSI_LUN'
# SCSI LUN of the device under test.
# \i 'T1_SCSI_BUFSIZE'
# Buffer size, in bytes (optional).
# \i 'T1_SCSI_NBBUF'
# Number of buffers to write (optional).
# \i 'T1_SCSI_NBLOOP'
# Number of loops to perform (optional).
# \i 'T1_TMO_SCSI'
# Timeout value, in seconds (optional).
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
#     SCSI Test :
#     scsiShow() test                                           : PASS
#     SCSI write/read/verify device                             : PASS
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
# serLib.tcl,
# pkLib.c,
# bspPkCommonProc.tcl,
# envLib.tcl
#
# NOROUTINES
#
#

# print header

printHeader "SCSI Test"

# attach to the target server

set targetHd [wtxToolAttach $firsttgtsvrname]
wtxRegisterForEvent .*

# load test routine

checkAndLoadFile $firstbspdir pkLib.o pkGetMacroValue

# read environment variables

set tipDevice   [readEnvRequired T1_SER_DEVICE]
set tipBaud     [readEnvRequired T1_SER_BAUD]
set id 		[readEnvRequired T1_SCSI_ID]
set lun	 	[readEnvRequired T1_SCSI_LUN]
set bufSize	[readEnvOptional T1_SCSI_BUFSIZE 65536]
set nbBuf	[readEnvOptional T1_SCSI_NBBUF 0]
set nbLoop	[readEnvOptional T1_SCSI_NBLOOP 0]
set scsiTimeout [readEnvOptional T1_TMO_SCSI 3600]

send_user " host serial device is : $tipDevice \n\
           host serial baud rate is : $tipBaud \n " 0

send_user "Specified SCSI parameters : \n \
             SCSI ID           : $id \n \
             SCSI LUN          : $lun \n \
             BUFFER SIZE       : $bufSize \n \
             NUM OF BUFFERS    : $nbBuf \n \
             NUM OF LOOPS      : $nbLoop \n \
             Timeout           : $scsiTimeout \n " 0


# open serial channel and flush it

set fd [serChanOpen $tipDevice $tipBaud]

serChanFlush $fd

# set max trials to do in case of error

set maxTries 5

# check if target is configured for scsi else skip the test

set res 0

if {[catch {wtxSymFind -name scsiShow} showRes]} {
    if {[string match "*Error*" $showRes]} {
        send_user "target not configured for scsi...\
                   skipping the test" 0
        set res 1
    }
} elseif {[catch {wtxSymFind -name pkTestOneUnit} pkRes]} {
    if {[string match "*Error*" $pkRes]} {
        send_user "target is configured for scsi but pkLib.o has \
                   to be recompiled for scsi ...\
                   skipping the test" 0
        set res 1
    }
}

if {$res} {
    testResult 2 "scsiShow() test"
    testResult 2 "SCSI write/read/verify device"
} else {

    # call scsiShow() so the SCSI configuration

    set cmd "sp scsiShow "
    set ret1 [getWindshOutput $firsttgtsvrname $cmd]
    if {$ret1 == "ERROR"} {
        fatalError "command failed for scsiShow"
    }
 
    # check for the output pattern
   
    set ret3 0
    set ret2 [serChanRecv $fd 2048 10 "pScsiPhysDev" 1]
 
    if {[string match "timeoutError*" $ret2]} {
        send_user "serial timeout error during scsiShow" 0
        set ret3 1
    } elseif {[string match "Error" $ret2]} {
        send_user "serial error during scsiShow" 0
        set ret3 1
    }
 
    # display the result
 
    testResult [expr {($ret1 != "ERROR") && ($ret3 == 0)}] "scsiShow() test"

    # delay to flush the serial channel

    msleep 1000

    # flush the serial channel

    serChanFlush $fd

    # go and run read/write test if scsiShow test passes else skip

    if {($ret1 != "ERROR") && ($ret3 == 0) } {

    # run read/write test

    set funcSym [lindex [wtxSymFind -name pkTestOneUnit] 1]

    if {[catch {wtxFuncCall $funcSym $id $lun $bufSize $nbBuf $nbLoop} ret3]} {
        send_user $ret3 0
        fatalError "pkTestOneUnit function call failed"
    }

    # set result variable and also expected patterns from the console

    set p1 0

    set pat1 "pkTestUnit: beginning write loop"
    set pat2 "pkTestUnit: half way through write loop"
    set pat3 "pkTestUnit: beginning read loop"
    set pat4 "pkTestUnit: half way through read loop"

    # loop through the delay time to and check for results

    for {set i 0} {$i < $scsiTimeout} {incr i} {

        # check for final result string display

        set ret4 [serChanRecv $fd 2048 1 "PASS" 1]

        # on timeout check if there was any error statement displayed

        if {[string match "*ERROR*" $ret4]} {
            send_user "\nReceived Error is \n [trimSerOutput $ret4] \n" 0
            set p1 1
            break

        # if final result then break

        } elseif {[string match "*PASS*" $ret4]} {
            set p1 0
            break
        }

        # display of intervals for every 10 minutes

        if {[expr $i%10] == 0} {
        send_user "[expr 2*$i]  seconds through scsi test" 0
        }

    # check if any progress strings displayed

        if {[string match *$pat1* $ret4] || \
            [string match *$pat2* $ret4] || \
            [string match *$pat3* $ret4] || \
            [string match *$pat4* $ret4] } {

    # send progress statements to log file

            send_user [trimSerOutput $ret4] 0
        }

    # delay

    msleep 1000

    }

    serChanFlush $fd

    # display final result

    testResult [expr {$p1 == 0}] "SCSI write/read/verify device"

    } else {

    # if scsiShow failed skip the test with appropriate statement

        send_user "SCSI write/read/verify device test is skipped due to \
                   failure of scsiShow test" 0

       testResult 2 "SCSI write/read/verify device"
    }

}

# close serial channel

serChanFlush $fd
serChanClose $fd

# detach the target server

wtxToolDetach $targetHd


