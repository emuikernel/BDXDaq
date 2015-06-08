#
# ram.tcl - RAM tests
#
# Copyright 1997-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01k,18sep01,sbs  merge from tor2_5.bsp_drv_facility branch for veloce
# 01j,29oct99,sbs  added markup for html docs generation.
# 01i,16oct99,sbs  added extra argument for checkAndLoadFile() call.
# 01h,04sep97,sbs  added target server attach and detach.
# 01g,29aug97,kbw  made minor man page fix from jim
# 01f,28aug97,sbs  added looping for shParse in case of error.
# 01e,15aug97,kbw  made man page fixes from DAT and Dahmane
# 01d,06jul97,sbs  doc changes.
# 01c,31jul97,db   doc tweaks for man page generation.
# 01b,31jul97,sbs  doc added.
# 01a,07apr97,sbs  written.
#
# SYNOPSIS:
# 
# bspVal <options> ram
# 
#
# DESCRIPTION:
# This test verifies RAM read operations.
# See 'bspVal.sh' for an explanation of <options>.
#
# \ml
# \m 'NOTE:'
# This reference entry lists the most likely reasons for a test's failure.
# However, it does not list all possible reasons.
# \me
#
# The three tests in this script call pkRead() to perform read operations
# of the RAM locations between LOCAL_MEM_LOCAL_ADRS and sysMemTop().  The
# reads are done using 1-, 2-, and 4-byte accesses, depending on which
# test is being run.  If any of these tests fail, but the memory hardware
# is known to be functioning properly, check that the memory controller
# is initialized to the correct values.  Make sure that LOCAL_MEM_LOCAL_ADRS
# and LOCAL_MEM_SIZE are set to the correct values.  If an MMU is present,
# it might need to be configured so that the entire RAM space is accessible.
# Check that sysMemTop() returns the correct values.
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
#     RAM Test :
#     1-byte read of RAM                                        : PASS
#     2-byte read of RAM                                        : PASS
#     4-byte read of RAM                                        : PASS
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

printHeader "RAM Test"

# attach to the target server

#set targetHd [wtxToolAttach $firsttgtsvrname]
wtxRegisterForEvent .*

# load test routine

checkAndLoadFile $firstbspdir pkLib.o pkGetMacroValue

# set max trials to do in case of error

set maxTries 5

# read environment variables

set cmd " lmla = pkGetMacroValue(\"LOCAL_MEM_LOCAL_ADRS\") "

set result1 [getWindShRet $cmd $maxTries]

if {$result1 == -1} {
    fatalError "LOCAL_MEM_LOCAL_ADRS macro not defined or\
                pkGetMacroValue failed "
}

# run the memory check routines using 1, 2, and 4 byte reads

set cmd " pkRead (lmla,sysMemTop(),1) "

set ret1 [getWindShRet $cmd $maxTries]

if {$ret1 == -1} {
    warning "command failed for 1-byte read of RAM"
}

testResult [expr {$ret1 == 0}] "1-byte read of RAM"

set cmd " pkRead (lmla,sysMemTop(),2) "

set ret2 [getWindShRet $cmd $maxTries]

if {$ret2 == -1} {
    warning "command failed for 2-byte read of RAM"
}

testResult [expr {$ret2 == 0}] "2-byte read of RAM"

set cmd " pkRead (lmla,sysMemTop(),4) "

set ret3 [getWindShRet $cmd $maxTries]

if {$ret3 == -1} {
    warning "command failed for 4-byte read of RAM"
}

testResult [expr {$ret3 == 0}] "4-byte read of RAM"

# detach the target server

#wtxToolDetach $targetHd


