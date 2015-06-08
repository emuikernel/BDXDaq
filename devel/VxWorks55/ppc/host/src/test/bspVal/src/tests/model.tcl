# model.tcl - sysModel() test
#
# Copyright 1997-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01i,18sep01,sbs  merge from tor2_5.bsp_drv_facility branch for veloce
# 01h,29oct99,sbs  added markup for html docs generation.
# 01g,04sep97,sbs  added target server attach and detach.
# 01f,29aug97,sbs  more logfile info and minor changes.
# 01e,15aug97,kbw   made man page fixes from DAT and Dahmane
# 01d,10aug97,kbw   fixed minor man page problems, spell check
# 01c,31jul97,db   doc tweaks for man page generation.
# 01b,31jul97,sbs  doc added.
# 01a,10feb97,sbs  written.
#
# SYNOPSIS:
# 
# bspVal <options> model
# 
#
# DESCRIPTION:
# This test verifies that the target board matches the target configuration
# file.  To do this, it verifies the return value of the sysModel() routine. 
# This value is extracted from the target information stored at target 
# server. The entire test should not take more than a few seconds to run.  
# See 'bspVal.sh' for an explanation of <options>.
#
# This is the first test run if the '-all' option is specified for 'bspVal'.
#
# \ml
# \m 'NOTE:'
# This reference entry lists the most likely reason for this test's failure.
# However, it does not list all possible reasons.  
# \me
#
# The first (and only) test compares the return value of sysModel()
# to the string specified by 'T1_MODEL'.  If this test fails, check that the
# sysModel() routine in sysLib.c returns the correct target model information.
#
# Barring serious hardware or software limitations (such as an identifiable
# VxWorks problem), the target board must pass all tests for the BSP to be
# validated.
#
# CONFIGURATION PARAMETERS 
#
# \is
# \i 'T1_MODEL' 20
# Expected result of sysModel(). (required)
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
#     Model Test :
#     sysModel() return value                                   : PASS
#
#                        Tests RUN           1 
#                        Tests PASSED        1  
#                        Tests FAILED        0 
#                        Tests SKIPPED       0 
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

# print header 

printHeader "Model Test" 

# attach to the target server

set targetHd [wtxToolAttach $firsttgtsvrname]
wtxRegisterForEvent .*

# read environment variables
  
set model [readEnvRequired T1_MODEL]

send_user "sysModel() value specified is : $model \n" 0

# get the target information from target server and verify

set bspModel [lindex [wtxTsInfoGet] 4] 

send_user "sysModel() return value is : $bspModel \n" 0

testResult [expr {$bspModel == $model}] "sysModel() return value"
   
# detach the target server

wtxToolDetach $targetHd
