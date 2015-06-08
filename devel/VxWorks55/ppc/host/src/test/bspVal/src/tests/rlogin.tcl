#
# rlogin.tcl - testing 'rlogin'
#
# Copyright 1997-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01o,31oct01,sbs  docs update (SPR 32355)
# 01n,18sep01,sbs  merge from tor2_5.bsp_drv_facility branch for veloce
# 01m,29oct99,sbs  added markup for html docs generation.
# 01l,26jul98,sbs  made the test independent of the 2 target configuration
#                  if both the targets are on a common VME backplane.
#                  (SPR #21424)
# 01k,15oct97,db   call to smNetShow() is made only when the backplane
# 		   network is used for rlogin to slave(SPR #9469).
# 01j,09oct97,kbw  checked in (late) changes made in the last minute
# 01i,05sep97,kbw  again, changed master/slave terms for target/reference
# 01g,05sep97,kbw  changed master/slave terms for target/reference
# 01h,04sep97,db   removed END_TEST in sample test output.
# 01g,29aug97,kbw  made man page fixes from jim
# 01f,25aug97,db   changed script to use ethernet address or backplane
#                  address. updated doc.
# 01e,15aug97,kbw  made man page fixes from DAT and Dahmane
# 01d,11aug97,kbw  made editing changes from db, spell check
# 01c,11aug97,db   renamed T1_TIP_DEVICE and T1_TIP_BAUD to T1_SER_DEVICE and
#                  T1_SER_BAUD respectively.
# 01b,11aug97,kbw  fixed minor man page problems, did a spell check
# 01a,19jun97,db  written.
#
#
# SYNOPSIS:
# 
# bspVal <options> rlogin
# 
#
# DESCRIPTION:
# This test verifies the ability to 'rlogin' between two VxWorks target
# boards.  The entire test should not take more than
# a few minutes to run.  See 'bspVal.h' for an explanation of <options>.
#
# \ml
# \m 'NOTE:'
# This reference entry lists the most likely reasons for a test's failure.
# However, it does not list all possible reasons.
# \me
# 
# In order to run this test, the configuration file used to create the
# VxWorks image must have defined the following macros:
#  
# \ts 
# 'INCLUDE_SHELL'
# 'INCLUDE_RLOGIN' 
# 'INCLUDE_SHOW_ROUTINES' 
# 'INCLUDE_NET_SYM_TBL'
# 'INCLUDE_LOADER'
# \te
#
# If the project facility is used to create VxWorks image, add the following
# components to VxWorks configuration:
#
# \ts
# INCLUDE_RLOGIN
# INCLUDE_SHELL
# INCLUDE_NET_SYM_TBL
# INCLUDE_LOADER
# INCLUDE_SM_NET_SHOW
# INCLUDE_SYM_TBL_SHOW
# INCLUDE_SHELL_BANNER
# \te
#
# All tests run by this script are concerned with verifying the 'rlogin'
# process.  After issuing an 'rlogin' to the target on the reference board,
# the first test verifies that "IN USE" appears on the console of the target
# board.  The second test confirms the 'rlogin' session by calling version()
# from the VxWorks shell and attempting to match the address for
# the target board.  The third test exercises the 'pty' by spawning printLogo().
# The fourth test checks the operation of logout().  This process is then
# repeated for 'rlogin' from the target to the reference board and
# the 'rlogin' session is confirmed with tests five through eight.  If any of
# these tests fail, check that the IP address or backplane address for the
# target board ('T1_BOOT_E' or 'T1_BOOT_B') and reference board IP or backplane
# address ('T2_BOOT_E' or 'T2_BOOT_B') are set correctly. Check also that their
# respective target names ('T1_BOOT_TN' and 'T2_BOOT_TN') are set to their 
# proper values.
#
# Barring serious hardware or software limitations (such as an identifiable
# VxWorks problem), the target board must pass all tests for the BSP to be
# validated.
#
# \ml
# \m 'NOTE:'
# This test does not require a backplane.  It is enough if there is a
# network connection between the two targets.  If the boards reside on
# separate sub-nets, the routing tables for each must be set up appropriately.
# \me
# 
# CONFIGURATION PARAMETERS:
# \is
# \i 'T1_BOOT_TN' 20
# Target name (required).
# \i 'T1_BOOT_E'
# Target IP address (required).
# \i 'T2_BOOT_B'
# Target backplane address (required if 'T1_BOOT_E' is not set).
# \i 'T2_BOOT_TN'
# Reference target name (required).
# \i 'T2_BOOT_E'
# Reference target IP address (required if 'T2_BOOT_B' is not set).
# \i 'T2_BOOT_B'
# Reference target backplane address (required if 'T2_BOOT_E' is not set).
# Because the default is to use the Ethernet, if you set both 'T2_BOOT_E'
# and 'T2_BOOT_B', the test uses the reference target IP address.
# \ie
#
# EXAMPLE:
# Output consists of:
# \cs
#                          BSP VALIDATION TEST
#                          -------------------
#
#  Target server                     : t53-160
#  BSP                               : mv147
#  Second target server              : t214-2
#  Second BSP                        : mv147
#  Log file                          : /tmp/bspValidationLog.6425
#
#  rlogin :
#  IN USE message when rlogin to t53-160                        : PASS
#  reach shell of t53-160                                       : PASS
#  rlogin pty usage on t53-160                                  : PASS
#  logout from t53-160                                          : PASS
#  IN USE message when rlogin to t214-2                         : PASS
#  reach shell of t214-2                                        : PASS
#  rlogin pty usage on t214-2                                   : PASS
#  logout from t214-2                                           : PASS
#
#                        Tests RUN           8
#                        Tests PASSED        8
#                        Tests FAILED        0
#                        Tests SKIPPED       0
# \ce
#
# SEE ALSO:
# 'bspVal.sh',
# loginLib.tcl
#
# NOROUTINES
#
#

# print header

printHeader   "rlogin"

# read environment variables
# please note that the variable name prefixes master and slave doesn't
# reflect any configuration setup.

set masterTN    	[readEnvRequired T1_BOOT_TN]
set masterE		[readEnvRequired T1_BOOT_E]
set masterB     	[readEnvRequired T1_BOOT_B]
set masterBaud  	[readEnvRequired T1_SER_BAUD]
set masterDev   	[readEnvRequired T1_SER_DEVICE]
set masterLibPath 	$firstbspdir

set slaveTN     	[readEnvRequired T2_BOOT_TN]
set slaveE		[readEnvRequired T2_BOOT_E]
set slaveB      	[readEnvRequired T2_BOOT_B]
set slaveBaud   	[readEnvRequired T2_SER_BAUD]
set slaveDev    	[readEnvRequired T2_SER_DEVICE]
set slaveLibPath 	$firstbspdir

if {$masterE == ""} {
    if {$masterB == ""} {
        fatalError "Master ethernet and backplane IP addresses not defined"
    } else {
        set masterIPAdrs $masterB
    }
} else {
    set masterIPAdrs $masterE
}

if {$slaveE == ""} {
    if {$slaveB == ""} {
        fatalError "Slave ethernet and backplane IP addresses not defined"
    } else {
        set slaveIPAdrs $slaveB
    }
} else {
    set slaveIPAdrs $slaveE
}


# attach to master target server

set masterHd [wtxToolAttach $firsttgtsvrname]
wtxRegisterForEvent .*

# attach to slave target server

set slaveHd [wtxToolAttach $secondtgtsvrname]
wtxRegisterForEvent .*


# establish serial link to master and slave

set masterFd [serChanOpen $masterDev $masterBaud]
set slaveFd  [serChanOpen $slaveDev $slaveBaud]

# strip prefix and suffix from network name and address variables

set masterName [afterEquals $masterTN]
set slaveName [afterEquals $slaveTN]
set masterAdrs [afterEquals [beforeColon $masterIPAdrs]]
set slaveAdrs [afterEquals [beforeColon $slaveIPAdrs]]

# call smNetShow if we are rlogin through the backplane

if {$slaveE == ""} {

    # verify that smNetShow exits on the target

    wtxHandle $masterHd
    if {[catch {wtxSymFind -name smNetShow} result]} {
        fatalError "smNetShow not included"
    }

    # call smNetShow() on master so the backplane configuration will
    # appear in the log file.

    serChanFlush $masterFd
    wtxFuncCall -int [symbol smNetShow]
    set retcode [retcodeGet]

    set ret [serChanRecv $masterFd 2048 20 "collisions" 1]
    if {[string match timeoutError* $ret] || \
        [string match Error* $ret]} {
        fatalError "Can't read from $masterDev port"
    } else {
        send_user  "\n[trimSerOutput $ret]\n" 0
    }
}

# verify that rlogin was included in the vxWorks image
# that runs on the slave.

wtxHandle $slaveHd
if {[catch {wtxSymFind -name rlogin} result]} {
    fatalError "rlogin not included in slave"
}

# test rlogin from the slave to the master

testLogin $slaveFd $masterFd $masterAdrs $masterName

# sleep for 2 seconds

msleep 2000

# check that rlogin was included in the vxWorks image
# that runs on the master.

wtxHandle $masterHd
if {[catch {wtxSymFind -name rlogin} result]} {
    fatalError "rlogin not included in master"
}

# test rlogin from the master to the slave

testLogin $masterFd $slaveFd $slaveAdrs $slaveName

# close sessions with serial ports

serChanClose $slaveFd
serChanClose $masterFd

# disconnect from target servers

wtxToolDetach $masterHd
wtxToolDetach $slaveHd
