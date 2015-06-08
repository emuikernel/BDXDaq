#
# eprom.tcl - EPROM tests
#
# Copyright 1997-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01l,18sep01,sbs  merge from tor2_5.bsp_drv_facility branch for veloce
# 01k,29oct99,sbs  added markup for html docs generation.
# 01j,16oct99,sbs  added extra argument for checkAndLoadFile() call.
# 01i,04sep97,sbs  added target server attach and detach.
# 01h,29aug97,sbs  added looping for shParse in case of error.
# 01g,29aug97,kbw  made a minor fixes to the man page
# 01f,15aug97,kbw  fixes to man page from DAT and Dahmane
# 01e,10aug97,kbw  minor man page fixes, spell check
# 01d,06jul97,sbs  doc changes.
# 01c,31jul97,db   doc tweaks for man page generation.
# 01b,31jul97,sbs  doc added.
# 01a,04apr97,sbs  written.
#
# SYNOPSIS
# 
# bspVal <options> eprom
# 
#
# DESCRIPTION:
# This test verifies ROM read operations.
# The execution time of the tests run by this script depends on the
# CPU speed and the amount of ROM being tested.
# See 'bspVal.sh' for an explanation of <options>.
#
# \ml
# \m 'NOTE:'
# This reference entry lists the most likely reasons for a test's failure.
# However, it does not list all possible reasons.
# \me
#
# The three tests in this script call pkRead() to perform read operations
# of the ROM locations between ROM_BASE_ADRS and (ROM_BASE_ADRS + ROM_SIZE).
# The reads are done using 1-, 2-, or 4-byte accesses, depending on which
# test is being run.  If any of these tests fail, but the memory hardware
# is known to be functioning properly, check that the memory controller is
# initialized to the correct values.  Make sure that ROM_BASE_ADRS
# and ROM_SIZE are set to the correct values.  If an MMU is present, it
# might need to be configured so that the entire ROM space is accessible.
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
#                     BSP VALIDATION TEST
#                     -------------------
#
#     Target server                     : t53-160
#     BSP                               : mv147
#     Log file                          : /tmp/bspValidationLog.5219
#
#
#
#     EPROM Test :
#     1-byte read of EPROM                                      : PASS
#     2-byte read of EPROM                                      : PASS
#     4-byte read of EPROM                                      : PASS
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
# pkLib.c
#
# NOROUTINES
#
#

# print header

printHeader "EPROM Test"

# attach to the target server

set targetHd [wtxToolAttach $firsttgtsvrname]
wtxRegisterForEvent .*

# load the test routine

checkAndLoadFile $firstbspdir pkLib.o pkGetMacroValue

# set num of trials to make in case of error

set maxTries 5

# call pkLib.c routines to find ROM_BASE_ADRS and ROM_SIZE

set cmd " romBaseAddr = pkGetMacroValue(\"ROM_BASE_ADRS\") "

set result1 [getWindShRet $cmd $maxTries]

if { $result1 == -1 } {
    fatalError "ROM_BASE_ADRS macro not defined or pkGetMacroValue failed"
}

set cmd " romSize = pkGetMacroValue(\"ROM_SIZE\") "

set result2 [getWindShRet $cmd $maxTries]

if {$result2 == -1 } {
    fatalError "ROM_SIZE macro not defined or pkGetMacroValue failed"
}

# calculate ROM_BASE_ADRS + ROM_SIZE

set cmd " topAdrs = romBaseAddr + romSize "

set result3 [getWindShRet $cmd $maxTries]

if { $result3 == -1 } {
    fatalError "Expression evaluation failed during EPROM Test"
}

# run the memory check routines

set cmd " pkRead (romBaseAddr,topAdrs,1) "

set ret1 [getWindShRet $cmd $maxTries]

if {$ret1 == -1} {
    warning "pkRead failed for 1-byte read of EPROM"
}

testResult [expr {$ret1 == 0}] "1-byte read of EPROM"

set cmd " pkRead (romBaseAddr,topAdrs,2) "

set ret2 [getWindShRet $cmd $maxTries]

if {$ret2 == -1} {
    warning "pkRead failed for 2-byte read of EPROM"
}


testResult [expr {$ret2 == 0}] "2-byte read of EPROM"

set cmd " pkRead (romBaseAddr,topAdrs,4) "

set ret3 [getWindShRet $cmd $maxTries]

if {$ret3 == -1} {
    warning "pkRead failed for 4-byte read of EPROM"
}

testResult [expr {$ret3 == 0}] "4-byte read of EPROM"


# detach the target server

wtxToolDetach $targetHd


