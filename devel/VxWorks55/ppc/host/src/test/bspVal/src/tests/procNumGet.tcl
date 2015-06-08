#
# procNumGet.tcl - sysProcNumGet() test
#
# Copyright 1997-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01k,18sep01,sbs  merge from tor2_5.bsp_drv_facility branch for veloce
# 01j,29oct99,sbs  added markup for html docs generation.
# 01i,17oct97,sbs  changed funcCall to getWindShRet. 
# 01h,04sep97,sbs  added target server attach and detach.
# 01g,29aug97,kbw  made man page fixes from jim
# 01f,29aug97,sbs  more log file info and minor changes.
# 01e,15aug97,kbw  made man page fixes from DAT and dahmane
# 01d,10aug97,kbw  minor man page fixes, spell check
# 01c,31jul97,db   doc tweaks for man page generation.
# 01b,31jul97,sbs  doc added.
# 01a,14mar97,sbs  written.
#
# SYNOPSIS:
# 
# bspVal <options> procNumGet 
# 
#
# DESCRIPTION:
# This test verifies the return value of the sysProcNumGet() routine.
# The entire test should not take more than a few seconds to run. 
# See 'bspVal.sh' for an explanation of <options>.
#
# \ml
# \m 'NOTE:'
# This reference entry lists the most likely reasons for a test's failure.
# However, it does not list all possible reasons.  
# \me
# 
# The first (and only) test compares the return value of sysProcNumGet() 
# to the number specified by 'T1_BOOT_PROCNUM'.  If this test fails, check 
# that sysProcNumGet() returns the correct target processor number 
# information.
#
# Barring serious hardware or software limitations (such as an identifiable
# VxWorks problem), the target board must pass all tests for the BSP to be
# validated.
#
# CONFIGURATION PARAMETERS:
# \is
# \i 'T1_BOOT_PROCNUM' 25
# Expected result of sysProcNumGet(). (required)
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
#     sysProcNumGet Test :
#     sysProcNumGet() return value                              : PASS
#
#                       Tests RUN           1
#                       Tests PASSED        1
#                       Tests FAILED        0
#                       Tests SKIPPED       0
# 
# \ce
#
# SEE ALSO:
# 'bspVal.sh',
# bspPkCommonProc.tcl,
# envLib.tcl
#
# NOROUTINES
#
#

# print header

printHeader "sysProcNumGet Test" 

# attach to the target server

set targetHd [wtxToolAttach $firsttgtsvrname]
wtxRegisterForEvent .*

# set maximum trials to do in case of error

set maxTries 5

# read environment variables

set bootProc [readEnvRequired T1_BOOT_PROCNUM]

send_user "sysProcNumGet value specified is $bootProc \n" 0

# test sysProcNumGet()

set cmd " sysProcNumGet "
set bspModel [getWindShRet $cmd $maxTries]

if {$bspModel == -1} {
    warning "Command failed for sysProcNumGet"
}

send_user "sysProcNumGet return value is $bspModel \n" 0

testResult [expr {$bspModel == $bootProc}] "sysProcNumGet() return value"

# detach the target server

wtxToolDetach $targetHd


