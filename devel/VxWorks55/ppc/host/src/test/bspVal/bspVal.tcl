# bspVal.tcl - Bsp Validation test tcl script
#
# Copyright 1997-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01i,18sep01,sbs  merge from tor2_5.bsp_drv_facility branch for veloce
# 01h,16jul98,sbs  changed procedure call to printBanner().(SPR #21750)
# 01g,13jul98,sbs  added conversion of EXC_MSG_ADRS from decimal to hex for
#                  bootline test. (SPR #21422 & #21049)  
# 01f,20apr98,p_b  Merge from ARM to Tornado 2_0_x
# 01e,06oct97,sbs  added warning for invalid test names, new date format
#                  and error checking for setInitEnv.
# 01d,04sep97,sbs  moved clock tests to seperate test list.
# 01c,05jul97,sbs  added documentation and correted some minor errors. 
# 01b,28jul97,db   added source statements for smLib.tcl,networkLib.tcl,
#		   loginLib.tcl and timestampLib.tcl
# 01a,20apr97,sbs  written.
#
# DESCRIPTION
# This is the main tcl script which has routines to initialize the target 
# for the test and run the test with parameters passed from bspVal.sh 
#
# bspValTest is the main procedure which is called from bspVal.sh  to run the
# test considering all the user specified parameters. This routine has 3 test
# lists which it scans to find the test it has to run depending on options 
# specified. 
#
# bspValInit is used to reboot the target and restart the target server if 
# the user specifies reboot option.
#
# initEnv is used to initialize all the environment variables which may be
# required by tests involved in rebooting the target in which case the windsh
# restarts. 
#


################################################################################
#
# bspValTest - run the validation test
#
# This is the main routine called from bspVal.sh to run the test. It runs all 
# the tests with no reboot first, followed by tests involving 2 targets if 
# specified and then tests involving reboot. 3 different test lists are used 
# to follow this sequence of operation.
#
# SYNOPSIS:
#   bspValTest <firsttgtsvrname> <firstbspname> <secondtgtsvrname> 
#              <secondbspname> <alltests> <secondtarget> [testlist]
#
# PARAMETERS:
#   firsttgtsvrname: target server name of master target
#   firstbspname: bsp of the master target
#   secondtgtsvrname: target server name of slave target
#   secondbspname: bsp of the slave target
#   alltests: used to detect whether to consider testlist
#   secondtarget: used to detect to run tests involving second target
#   testlist: testlist to run
#
# RETURNS: N/A
#
# ERRORS: N/A 
#

proc bspValTest {firsttgtsvrname firstbspname secondtgtsvrname \
                 secondbspname alltests secondtarget args} {

#   import all global variables

    global totalCount passCount failCount skipCount logFile logFileId \
           envArray

#   set all required test lists

    set testlist1 "baudConsole eprom model nvRam \
    procNumGet ram scsi serial timestamp" 
    set testlist2 "busTas rlogin network"
    set testlist3 "auxClock sysClock"
    set testlist4 "bootline error1 error2" 

#   initialize all paths

    set testdir [wtxPath host src test bspVal] 
    set firstbspdir [wtxPath target config $firstbspname]
    if {$secondtarget == "true"} {
        set secondbspdir [wtxPath target config $secondbspname] 
    }

#   source all library files 

    source $testdir/src/lib/envLib.tcl
    source $testdir/src/lib/clockLib.tcl
    source $testdir/src/lib/bspPkCommonProc.tcl
    source $testdir/src/lib/serLib.tcl
    source $testdir/src/lib/smLib.tcl
    source $testdir/src/lib/networkLib.tcl
    source $testdir/src/lib/loginLib.tcl
    source $testdir/src/lib/timestampLib.tcl 

#   initialize serial library

    serLibInit

#   print the banner

    printBanner $firsttgtsvrname $firstbspname $secondtgtsvrname \
                $secondbspname

#   set the initial environment

    

    setInitEnv $testdir

#   find all invalid tests to notify the user 
   
    set tests [concat $testlist1 $testlist2 $testlist3 $testlist4]


    if {[string compare "" $args] != 0} {
        foreach param $args {
            if {[lsearch -glob $tests $param] == -1 } {
                warning "\"$param\" is an invalid test" 
            }
        }
    }


#   run  the tests 

    for {set listCount 1} {$listCount < 5} {incr listCount} { 

#       run tests with [-all] options

        if {[string compare "" $args] == 0 && \
            [string compare true $alltests] == 0 } {
            if { ! ( $listCount == 2 && \
                   [string compare false $secondtarget] == 0 ) } {
                foreach testparam [subst \$testlist$listCount] { 
                    source $testdir/src/tests/${testparam}.tcl
                    send_user "\nEnd of test :\
                               [clock format [clock seconds]]\n" 0
                }
            } 
        } else { 

#           run tests if specific tests are specified 

            if { ! ( $listCount == 2 && \
                   [string compare false $secondtarget] == 0 ) } {
                foreach testparam $args {
                    set list [subst \$testlist$listCount]
                    set ix [lsearch -exact $list $testparam]
                    if {$ix >= 0} {
                        source $testdir/src/tests/${testparam}.tcl
                        send_user "\nEnd of test :\
                                   [clock format [clock seconds]]\n" 0 
                    } 
                }
            }
        }   
    }

#   print results

    printResults
    puts stdout "END_TEST" 
}

##############################################################################
#
# bspValInit - initilise target 
#
# This routine initializes the target by rebooting it and loads the required
# object modules on the target required for testing. It then restarts the 
# target server to make sure that it reconnects to the target.
#
# SYNOPSIS:
#   bspValInit target tgtsvrname testdir 
#
# PARAMETERS:
#   target: target name
#   tgtsvrname: target server name
#   testdir: root of the testdir to use library routines required
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc bspValInit {target tgtsvrname testdir} {

#   import all global variables

    global env logFile logFileId

#   source all libraries

    source $testdir/src/lib/envLib.tcl
    source $testdir/src/lib/serLib.tcl
    source $testdir/src/lib/bspPkCommonProc.tcl

#   get required environment variables

    set bootline [getBootline $target]
    set tipDevice $env(${target}_SER_DEVICE)
    set tipBaud $env(${target}_SER_BAUD)
    set timeoutBoot $env(${target}_TMO_BOOT)

#   display rebooting message

    send_user "Rebooting target $tgtsvrname with the following boot\
                parameters : \n$bootline\n" 1

#   initialize serial library

    serLibInit

#   reboot the target and restart the target server 

    set fd [serChanOpen $tipDevice $tipBaud]
    rebootTarget $fd $bootline $timeoutBoot 

    msleep 5000

    checkTgtSvr $tgtsvrname 
    serChanClose $fd
}



##############################################################################
#
# setInitEnv - set initial environment
#
# This routine sets up the initial environment which may be necessary for the 
# test to run continuously even the target is rebooted.
#
# SYNOPSIS:
#   setInitEnv testDir
#
# RETURNS: N/A
#
# ERRORS: N/A
# 

proc setInitEnv {testDir} {
   
#   import all global variables
 
    global envArray env

#   set max trials to do in case of error

    set maxTries 5 

#   source the libraries

    source $testDir/src/lib/bspPkCommonProc.tcl

#   create an array of environment variables required

    set cmd " pkGetMacroValue(\"EXC_MSG_ADRS\") "
    set excMsgAdrs [getWindShRet $cmd $maxTries]

    if {$excMsgAdrs == -1} {
        warning "EXC_MSG_ADRS macro not defined or command failed" 
        set excMsgAdrs -1
    }

    set cmd " pkGetMacroValue(\"AUX_CLK_RATE_MAX\") "
    set auxClkRateMax [getWindShRet $cmd $maxTries] 

    if { $auxClkRateMax == -1} {
        warning "AUX_CLK_RATE_MAX macro not defined or command failed"
        set auxClkRateMax -1
    }
  
    set envArray(EXC_MSG_ADRS) [format "%#x" $excMsgAdrs] 
    set envArray(AUX_CLK_RATE_MAX) $auxClkRateMax

}
     

