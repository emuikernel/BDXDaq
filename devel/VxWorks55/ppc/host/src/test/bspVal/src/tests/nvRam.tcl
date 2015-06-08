#
# nvRam.tcl - non-volatile RAM tests
#
# Copyright 1997-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01m,18sep01,sbs  merge from tor2_5.bsp_drv_facility branch for veloce
# 01l,29oct99,sbs  added markup for html docs generation.
# 01k,16oct99,sbs  added extra argument for checkAndLoadFile() call.
# 01j,10jun99,sbs  merge from 2_0_x
# 01i,23feb99,sbs  tests 1-3 and 6-8 are skipped if nvRam is not present
#                  on the board (SPR #22659).
# 01h,17oct97,sbs  changed funcCall to getWindShRet and minor changes.
# 01f,05sep97,kbw  made a man page change from DAT
# 01f,04sep97,sbs  added target server attach and detach.
# 01d,29aug97,kbw  made man page fixe from jim
# 01e,28aug97,sbs  added looping for shParse in case of error.
# 01d,15aug97,kbw  made man page fixes from DAT and Dahmane
# 01c,31jul97,db   doc tweaks for man page generation.
# 01b,31jul97,sbs  doc added.
# 01a,10mar97,sbs  written.
#
# SYNOPSIS:
# 
# bspVal <options> nvRam
#
#
# DESCRIPTION:
# This test verifies sysNvRamGet() and sysNvRamSet(), the sysLib.c
# routines that manipulate non-volatile memory.
# See 'bspVal.sh' for an explanation of <options>.
#
# \ml
# \m 'NOTE:'
# This reference entry lists the most likely reasons for a test's failure.
# However, it does not list all possible reasons.
# \me
#
# On targets without non-volatile RAM, NV_RAM_SIZE should be set to NONE
# (-1) in the BSP.  On such boards, only parameter checking of sysNvRamSet()
# and sysNvRamGet() is performed.  If only a portion of NVRAM is writable,
# or some amount is reserved for hardware operations, be sure to
# specify the writable size with the NV_RAM_SIZE_WRITEABLE macro in config.h.
#
# The execution time of the tests run by this script is dependent on the
# size of NVRAM being tested and the CPU speed. The entire test script
# should not take more than a few minutes to run to completion.
#
# This script merely calls pkTestNvRam() eight times to perform
# different non-volatile RAM functionality tests.
#
# The first test verifies the operation of a sysNvRamGet() read of
# NV_RAM_SIZE bytes.  If this test fails, check that NV_RAM_SIZE is set
# to the size of NVRAM and that NV_BOOT_OFFSET is defined and used
# properly.  Also check that sysNvRamGet() is copying the entire length
# requested from NVRAM into the passed-in buffer.
#
# The second test reads, writes, and reads NV_RAM_SIZE bytes of NVRAM to check
# sysNvRamSet() functionality.  If this test fails, check the same problem
# areas as the first test.  Also check that sysNvRamSet() is copying the
# entire length requested from the passed-in buffer into NVRAM.
#
# The third test checks the operation of a sysNvRamGet() read of zero bytes.
# If this test fails, check that reads of zero length are allowed by
# sysNvRamGet().  The buffer should be properly terminated with EOS, and the
# routine should return OK.
#
# The fourth test performs parameter checking of sysNvRamSet().
# This test checks that the proper return value is given for erroneous input
# parameters.  If this test fails, check that error checking is provided
# by sysNvRamSet(), which should return ERROR for a negative offset or a length
# parameter.  Also, the value of the offset plus length parameters must be
# less than NV_RAM_SIZE.
#
# The fifth test performs parameter checking of sysNvRamGet().
# This test checks that the proper return value is given for erroneous input
# parameters.  If this test fails, check that error checking is provided
# by sysNvRamGet(), which should return ERROR for a negative offset or length
# parameter.  Also, the value of the offset plus length parameters must be
# less than NV_RAM_SIZE.
#
# The sixth test writes several bytes of data (0xff),  then reads back from
# the same NVRAM location. This operation checks that the same data is read
# back, and that the buffer is returned with a NULL termination.  If this test
# fails, check that writing 0xff to NVRAM does not cause a problem and that
# the passed-in buffer is properly terminated with EOS.
#
# The seventh test writes several bytes of data (0x00),  then reads back from
# the same NVRAM location. This operation checks that the same data is read
# back, and that the buffer is returned with a NULL termination.
# If this test fails, check that writing 0x00 to NVRAM does not cause a
# problem and that the passed-in buffer is properly terminated with EOS.
#
# The eighth test returns NV_RAM_SIZE bytes of NVRAM back to the values
# read in the first test.  If this test fails, check that NV_RAM_SIZE bytes
# of NVRAM can be written and read with sysNvRamSet() and sysNvRamGet().
#
# Barring serious hardware or software limitations (such as an identifiable
# VxWorks problem), the target board must pass all tests for the BSP to be
# validated.
#
# CONFIGURATION PARAMETERS: None.
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
#     Non-Volatile RAM Test :
#     sysNvRamGet() of boot line                                : PASS
#     sysNvRamSet() and sysNvRamGet() of complemented bootline  : PASS
#     sysNvRamGet() with length zero                            : PASS
#     sysNvRamSet() parameter checking                          : PASS
#     sysNvRamGet() parameter checking                          : PASS
#     sysNvRamSet() and sysNvRamGet() of 0xff data              : PASS
#     sysNvRamSet() and sysNvRamGet() of 0x00 data              : PASS
#     sysNvRamSet() and sysNvRamGet() of boot line              : PASS
#
#                       Tests RUN           8
#                       Tests PASSED        8
#                       Tests FAILED        0
#                       Tests SKIPPED       0
#
# \ce
#
# SEE ALSO:
# 'bspVal.sh',
# bspPkCommonProc.tcl,
# pkLib.c
#
# NOROUTINES
#
#

# print header

printHeader "Non-Volatile RAM Test"

# attach to the target server

set targetHd [wtxToolAttach $firsttgtsvrname]
wtxRegisterForEvent .*

# load test routine

checkAndLoadFile $firstbspdir pkLib.o pkGetMacroValue

# set max trials to do in case of error

set maxTries 5

# determine if nvram present - only tests 4 and 5 should pass if no nvram

set nvPresent 0

set cmd " pkGetMacroValue(\"NV_RAM_SIZE\") "

set nvRamSize [getWindShRet $cmd $maxTries]

if {$nvRamSize == 987654321} {
    fatalError "NV_RAM_SIZE macro not defined "
}

if {$nvRamSize == -1} {
    warning "nvRam not present on this board"
}

send_user "NV_RAM_SIZE is  $nvRamSize \n" 0

# call pkTestNvRam() to run sub-tests

# skip tests 1-3 and 6-8 if nvRam is not present

if {$nvRamSize == -1} {
    testResult 2 "sysNvRamGet() of boot line"
    testResult 2 \
    "sysNvRamSet() and sysNvRamGet() of complemented bootline"
    testResult 2 "sysNvRamGet() with length zero"
} else {

# test 1

set cmd " pkTestNvRam 1 "
set return1 [getWindShRet $cmd $maxTries]
testResult [expr {$return1 == 0}] "sysNvRamGet() of boot line"

# test 2

set cmd " pkTestNvRam 2 "
set return2 [getWindShRet $cmd $maxTries]
testResult [expr {$return2 == 0}] \
           "sysNvRamSet() and sysNvRamGet() of complemented bootline"

# test 3

set cmd " pkTestNvRam 3 "
set return3 [getWindShRet $cmd $maxTries]
testResult [expr {$return3 == 0}] \
           "sysNvRamGet() with length zero"
}

# test 4

set cmd " pkTestNvRam 4 "
set return4 [getWindShRet $cmd $maxTries]
testResult [expr {$return4 == 0}] \
           "sysNvRamSet() parameter checking"

# test 5

set cmd " pkTestNvRam 5 "
set return5 [getWindShRet $cmd $maxTries]
testResult [expr {$return5 == 0}] \
           "sysNvRamGet() parameter checking"

if {$nvRamSize == -1} {
    testResult 2 "sysNvRamSet() and sysNvRamGet() of 0xff data"
    testResult 2 "sysNvRamSet() and sysNvRamGet() of 0x00 data"
    testResult 2 "sysNvRamSet() and sysNvRamGet() of boot line"
} else {
# test 6

set cmd " pkTestNvRam 6 "
set return6 [getWindShRet $cmd $maxTries]
testResult [expr {$return6 == 0}] \
           "sysNvRamSet() and sysNvRamGet() of 0xff data"

# test 7

set cmd " pkTestNvRam 7 "
set return7 [getWindShRet $cmd $maxTries]
testResult [expr {$return7 == 0}] \
           "sysNvRamSet() and sysNvRamGet() of 0x00 data"

# test 8

set cmd " pkTestNvRam 8 "
set return8 [getWindShRet $cmd $maxTries]
testResult [expr {$return8 == 0}] \
           "sysNvRamSet() and sysNvRamGet() of boot line"
}

# detach the target server

wtxToolDetach $targetHd


