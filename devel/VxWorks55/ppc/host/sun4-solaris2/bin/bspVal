#!/bin/sh
#
# bspVal - shell script to configure and run BSP Validation Test Suite
#
# Copyright 1997-2002 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01r,08apr02,sbs  adding serial ports as command line option (SPR 77221)
# 01q,31oct01,sbs  documentation update (SPR 32355).
# 01p,18sep01,sbs  merge from tor2_5.bsp_drv_facility branch for veloce
# 01o,29oct99,sbs  added markup for html docs generation.
# 01n,16oct99,sbs  added extra criteria for pkLib.o checking.
# 01m,20sep99,sbs  changed documentation to include INCLUDE_NET_SHOW as part
#                  target configuration and minor changes to man page
#                  (SPR #26420).
# 01n,17jul98,sbs  corrected sequence of reboot depending on T1_BUS_MASTER
#                  configuration parameter in a 2 target system on a common
#                  VME backplane. (SPR #21423)
# 01m,09jul98,sbs  changed default log file directory from /tmp to
#                  <bspdir>/vtsLogs. (SPR #21750)
# 01l,07jul98,sbs  added an explicit "exit" command before closing pipe
#                  to windsh process.(SPR #20973).
# 01k,06oct97,sbs  added more help info and TGT_DIR macro.
# 01j,19sep97,kbw  man page changes from Sarma
# 01i,04sep97,kbw  man page changes from DAT and Sarma
# 01h,01sep97,kbw  made minor man page format fixes
# 01g,28aug97,sbs  changed reboot option and added info for Windows.
# 01f,14aug97,kbw  more man page changes
# 01e,14aug97,kbw  input man page changes from DAT and Dahmane
# 01d,08jul97,sbs  moved serial devices to configuration files and minor
#                  changes.
# 01c,05jul97,sbs  added documentation, provision for user to specify log file
#                  as environment variable and suppression of END_TEST string.
# 01b,28jul97,db   suppressed echoing of source and bspValTest commands.
# 01a,25mar97,sbs  written.
#
# SYNOPSIS:
# .tS
# bspVal [-h] [-all] [-r] [-c] <serverID> -b <bspName>
#        [-s <secondServerID>] [-sb <secondBspname>] 
#        [-t1SerDev <T1SerialDevice>] [-t1SerBaud <T1SerialBaud>] 
#        [-t2SerDev <T2SerialiDevice>] [-t2SerBaud <T2SerialBaud>] 
#        [<testName> ... ] "
# .tE
#
# DESCRIPTION:
# This script provides the high-level user interface to the BSP Validation
# Test Suite (VTS). After initializing the environment required for the test,
# this script executes a Tcl script that runs the specified BSP test,
# \f2testName\fP, on the target. To run all possible tests, use the '-all'
# option.
#
# This VTS is designed to use the host-based 'windsh' to run the tests.
# Internally, these tests use a combination of 'windsh' commands and WTX
# Tcl API calls.  This allows the same scripts to run under UNIX and as well
# as Windows. Also referenced is a specialized set of functions that
# the tests use to access host serial ports for target/console interaction.
# See the \f2Tornado API Guide\fP, for information on the WTX protocol
# functions and WTX Tcl API.
#
# \ml
# \m 'NOTE:'
# The target has to be connected with a target server by some type of WDB
# connection to run VTS. Please refer to Tornado manuals for different types
# of WDB connection if the target does not support ethernet connectivity,
# the default connection.
# \me
#
# \ml
# \m 'NOTE:'
# VTS can also run on targets booting VxWorks ROM images but the target has
# to be connected to a target server. If the target is booting VxWorks ROM
# image, certain tests like 'bootline' that require VxWorks boot prompt will
# fail.  
# \me
#
# Under UNIX, this script can be directly invoked from the command line.
# Under Windows, you must first invoke 'bash':
#
# .IP 1. 3
#  Change directories to:
#
#  <windbase>\\host\\x86-win32\\bin
# 
#  Where <windbase> is the directory pointed to by the 'WIND_BASE' environment
#  variable.
#
# .IP 2.
# Edit 'bashrc.bspVal' to configure the Tornado environment.
# \tb 
# (The instructions are in the file.)
#
# .IP 3.
# Invoke the 'bash' shell:
# \tb 
#
# \tb 
# .tS
# bash -rcfile bashrc.bspVal
# .tE
# .LP
# After the 'bash' shell responds with a prompt (\f3bash$\fP), type in
# the 'bspVal' command exactly as you would on a UNIX host (described below).
# For example:
# .CS
# bash$ bspVal tgtSvr1 -b mv147 -s tgtSvr2 -sb mv147 -all
# .CE
#
# OPTIONS
# If you specify a two-target test, make sure that the second target
# (reference board) is a valid BSP. In addition, the required command-line
# arguments for 'bspVal' varies according to whether you select a
# single-target or a two-target test.
# For single-target tests, the required parameters are:
# .IP <serverID> 15
# Specifies the target server name.
# .IP "\f3-b\fP <bspName>"
# Specifies the name of the BSP being validated.
# .IP "\f2testName\fP"
# Specifies the name of the test to be performed.
# .LP
# For two-target tests, you must specify all the parameters shown above, as
# well as the following:
# .IP "\f3-s\fP <secondServerID>" 25
# Specifies the reference board target server name.
# .IP "\f3-sb\fP <secondBspName>"
# Specifies the reference board BSP name
# .LP
# Use the other 'bspVal' options as follows:
# .IP "\f3-t1SerDev\fP <T1SerialDevice>" 25
# Specifies the host serial device of the main target.
# .IP "\f3-t1SerBaud\fP <T1SerialBaud>" 25
# Specifies baud rate of the host serial device of the main target.
# .IP "\f3-t2SerDev\fP <T2SerialDevice>" 25
# Specifies the host serial device of the reference board target.
# .IP "\f3-t2SerBaud\fP <T2SerialBaud>" 25
# Specifies baud rate of the host serial device of the reference board target.
# .IP '-h'
# Print the usage information about VTS.
# .IP '-r'
# Cancel Reboot of target(s) before starting the test.
# .IP '-c'
# Delete the previous log files.
# .IP '-all'
# Run all tests.
#
# CONFIGURATION FILES
# The VTS uses a configuration file called <bspName>.T1.  Where <bspName> is
# the name of the BSP you want to validate. It must reside in the
# \f3$WIND_BASE/host/resource/test/bspVal\fP directory. If you want to perform
# tests involving two targets ('network', 'rlogin', 'busTas'), you must also
# specify an additional configuration file called <secondBspName>.T2.
# Where <secondBspName> is the name of the BSP running on the reference board
# used by the second target. All the configuration parameters for the first
# target should use a 'T1_' prefix and all parameters for the reference board
# should use a 'T2_' prefix.  See the sample set of configuration files
# provided in the '$WIND_BASE/host/resource/test/bspVal' directory.
#
# Parameters referred to as optional need not be defined because the BSP
# tests have default values associated with these macros.  All other parameters
# must be defined, even if they are set to "".
#
# If two target tests are run with the targets on a common VME backplane then
# the test assumes 'T1' target as the system controller by default. The user
# can set an optional configuration parameter 'T1_BUS_MASTER' to 'FALSE' for
# overriding this default configuration. This is particularly useful to run
# 'busTas' test with 'T2' target as a slave on the common VME backplane. This
# has an impact on the rebooting sequence of targets since the system
# controller is required to reboot before any slaves on the VME backplane
# are rebooted.
#
# The VTS configuration parameters 'T1_SER_DEVICE', 'T1_SER_BAUD', 
# 'T2_SER_DEVICE' and 'T2_SER_BAUD' can be optionally specified as command 
# line paremeters to the 'bspVal' command.  If any of these configuration
# parameters are specified as command line parameters to 'bspVal' command, 
# the default values specified in the configuration files will be overwritten.
#
# LOG FILES
# By default, the 'bspVal' script creates a directory named 'vtsLogs' in the
# BSP directory.  In this directory a log file of the form
# \f3bspValidationLog.\fP<PID>, where <PID> is the ID of the process
# associated with the 'bspVal' script execution, is created for each
# run of VTS. Make sure that the BSP directory has required permissions to
# allow creation of the 'vtsLogs' directory.
#
# Alternatively, you can use the 'TEST_LOG_FILE' environment variable to
# specify the full path name of a user-defined log file.  Note that as long
# as the 'TEST_LOG_FILE' environment variable remains defined, all subsequent
# runs of the 'bspVal' will append their log output to this file.  To return
# to the default log-file location, unset 'TEST_LOG_FILE'.
#
# VTS ORGANIZATION
# The organization of the VTS over the Tornado tree is as shown below:
#
# .ne 9
# \f3$WIND_BASE/
#     target/src/test/bspVal/\fP  (pklib.c, the target test library)
#     \f3host/
#         resource/test/bspVal/\fP  (configuration files)
#         \f3src/test/
#             tclserial/\fP  (all, required source files)
#             \f3bspVal/
#                 src/tests/\fP (all tests)
#                 \f3src/lib/\fP  (all library routines)
#
# The target test library pkLib.c is compiled for each BSP by copying the
# file into the BSP directory. The 'bspVal' script checks for the existence
# of pkLib.o, copies the source file to the BSP directory if necessary,
# generates pkLib.o, and loads it onto the target. If you make any changes
# to the files in \f3$WIND_BASE/target/src/test/bspVal\fP, you must delete
# pkLib.o and unload the previous object module from the target so that the
# latest code is used.
#
# The 'tclserial' directory contains the source files for the functions used
# to access the serial port on the host that is bound to Tcl. The
# 'serLib.tcl' file contains a set of library functions that are an
# extension of 'tclserial'.
#
# Every test has to be invoked using the 'bspVal' command with required
# options. Tests involving single target and no reboot are run first, followed
# by tests involving two targets, followed by clock tests, followed by tests
# that reboot the target.  Within these limitations, you can specify the order
# in which the tests are run.
#
# After parsing through the command line parameters, 'bspVal':
# .IP 1. 3
# Checks the validity of the target server(s) specified
# using a 'checkServerName' library function.
# .IP 2.
# Sources the configuration parameters from the resource directory.
# .IP 3.
# Reboots the target(s) if necessary (an option you can specify).
# .IP 4.
# Loads pkLib.o on the target.
# .IP 5.
# Opens a pipeline to 'windsh' to send
# commands to source the Tcl scripts and run the tests.
# .LP
# You can abort the test scripts at any time using an interrupt signal
# (normally ctrl-c).  However, the target board might continue to execute local
# tasks.  This situation could leave the board in such a state that a
# power-cycle reset is necessary.
#
# TEST LIST
# This section lists all the tests currently included in the BSP validation
# test suite.  Some tests run stand-alone.  Some require a reference board.
# The tests listed below run stand-alone.
# .IP 'auxClock' 15
# Tests auxiliary clock's functionality.
# .IP 'baudConsole'
# Tests console's ability to communicate at all
# supported serial baud rates.
# .IP 'bootline'
# Verifies commands executable from vxWorks boot prompt.
# .IP 'eprom'
# verifies ROM read operations.
# .IP 'error1'
# Verifies the target local bus access, off-board
# bus access and divide by zero exceptions initiated
# from the target shell.
# .IP 'error2'
# Another set of error tests involving testing of
# reboot and catastrophic errors.
# .IP 'model'
# Tests return value of sysModel() routine.
# .IP 'nvRam'
# Verifies non-volatile RAM manipulation routines functionality.
# .IP 'procNumGet'
# Verifies return value of sysProcNumGet() routine.
# .IP 'ram'
# Verifies RAM read operations.
# .IP 'scsi'
# Verifies scsi read/write operations.
# .IP 'serial'
# Tests serial driver functionality.
# .IP 'sysClock'
# Tests system clock's functionality.
# .IP 'timestamp'
# Verifies BSP timestamp timer functionality.
# .LP
# The following tests require a reference board, a valid BSP.
# .IP 'busTas' 15
# Verifies sysBusTas() operation between 2 vxWorks
# target boards in a common VME backplane.
# .IP 'network'
# Verifies basic network functionality.
# .IP 'rlogin'
# Verifies ability to 'rlogin'.
#
# TARGET CONFIGURATION USING BSP CONFIGURATION FILE
# Most 'bspVal' tests should run under the default VxWorks 5.4 configuration,
# but some tests require that the target include utilities not typically
# included in the default configuration.  The list below shows which tests
# require which macros.  Add these macros to target's config.h prior to
# building its VxWorks image.  These requirements are as follows:
# \ts 
# 'auxClock'    | INCLUDE_AUXCLK
# 'busTas'      | INCLUDE_SHOW_ROUTINES
# 'error2'	| INCLUDE_AUXCLK
# 'network'     | INCLUDE_PING
#               | INCLUDE_NET_SHOW
# 'rlogin'      | INCLUDE_SHOW_ROUTINES
#               | INCLUDE_RLOGIN
#               | INCLUDE_SHELL
#               | INCLUDE_NET_SYM_TBL
#               | INCLUDE_LOADER
# 'timestamp'   | INCLUDE_TIMESTAMP
# \te 
#
# TARGET CONFIGURATION USING PROJECT FACILITY
# If the Tornado project facility is used to configure the target the 
# components required for each test differ slightly from target configuration
# using config.h file in the bsp. The list below shows the components that are
# required to be configured for each test. 
# \ts
# 'auxClock'    | INCLUDE_AUX_CLK
# 'busTas'      | INCLUDE_SM_NET_SHOW
# 'error2'      | INCLUDE_AUX_CLK
# 'network'     | INCLUDE_PING
#               | INCLUDE_NET_SHOW
# 'rlogin'      | INCLUDE_RLOGIN
#               | INCLUDE_SHELL
#               | INCLUDE_NET_SYM_TBL
#               | INCLUDE_LOADER
#               | INCLUDE_SM_NET_SHOW
#               | INCLUDE_SYM_TBL_SHOW
#               | INCLUDE_SHELL_BANNER
# 'timestamp'   | INCLUDE_TIMESTAMP
# \te
#
# 
# Additionally, the 'scsi' test (not listed above) requires that
# the target be configured for SCSI and that a SCSI drive be connected.
#
# ERRORS
# The following is the list of errors the user might come across while running
# the VTS.
#
# .IP "\f3FATAL ERROR\fP" 30
# This error message is displayed whenever any important functionality of
# the test fails.
# .IP "\f3SKIP count\fP"
# Certain tests may be skipped if the test
# detects that the target is not configured
# for the test (for example, 'timestamp')
# .IP "\f3FAIL count\fP"
# Count indicating tests failed.
# .IP "\f3target server ambiguity\fP"
# This error occurs when target server with the same name is run on
# different machines.
# .IP 'networking'
# If there is highly active networking over
# the subnet then the user may get different
# kinds of wtx error messages due to network
# problems and the tests may fail.
# .IP "\f3file error messages\fP"
# If the test detects that required files are
# missing, these messages may come up.
# .IP "\f3abnormal termination(^C)\fP"
# The user is notified of the abnormal termination of test.
#
# NOROUTINES
#
#

# check for environment setup

wb_message="this environment must be set to the root location \
of the Tornado tree. "

host_message="WIND_HOST_TYPE must be set to the host type of the host \
machine. "

: ${WIND_BASE?$wb_message}

: ${WIND_HOST_TYPE?$host_message}

# specify tcl resources

TCL_LIBRARY=${WIND_BASE}/host/tcl/tcl
export TCL_LIBRARY

TCLX_LIBRARY=${WIND_BASE}/host/tcl/tclX
export TCLX_LIBRARY

if [ "$TGT_DIR" = "" ]
then
    TGT_DIR=$WIND_BASE/target
    export TGT_DIR
fi

# initialize all variables

testDir=$WIND_BASE/host/src/test/bspVal
resDir=$WIND_BASE/host/resource/test/bspVal
tgtSrcDir=$TGT_DIR/src/test/bspVal
paramList=""
alltests=false
testlist=""
tgtsvr1="undefined"
bspname1="undefined"
tgtsvr2="undefined"
bspname2="undefined"
t1SerDev="undefined"
t1SerBaud="undefined"
t2SerDev="undefined"
t2SerBaud="undefined"
target2=false
cleanup=false
reboot=true

# all messages

tests="baudConsole eprom model nvRam
procNumGet ram scsi serial timestamp
busTas network rlogin auxClock
sysClock bootline error1 error2"

usage="usage: bspVal [-h] [-all] [-r] [-c] <serverID> -b <bspName>
[-s <secondServerID>] [-sb <secondBspname>] [-t1SerDev <T1SerialDevice>]
[-t1SerBaud <T1SerialBaud>] [-t2SerDev <T2SerialiDevice>] 
[-t2SerBaud <T2SerialBaud>] [<testName> ... ] "

filemsg1="A parameter file with name of the form <bspname>.<T1|T2>
must be specified in $WIND_BASE/host/resource/test/bspVal directory
with all the required parameters. File with extension T2 is specified
only if tests involving second target are performed. A file with
extension T1 is compulsory."

filemsg2="File pkLib.c non-existent. The file should exist in either
$WIND_BASE/target/src/test/bspVal  or  bsp directory."

abort()         # -- abort calling process, with optional message
{
    if [ $# -gt 0 ]
    then
       echo "$*"
    fi

    exit 1
}

checkPkLib()    # -- check and load pkLib.o, compiling if necessary
{

    if [ $# -eq 4 ]
    then
#       check for pkLib.o module on target
        wtxtcl<<END_OBJ_MODULE_CHECK
            if {[catch {wtxToolAttach $1} attachRes]} {
                if {[string match "*Error*" \$attachRes]} {
                    exit 2
                }
            }
            if {[catch {wtxSymFind -name pkGetMacroValue} symCheck]} {
                if {[catch {wtxObjModuleFind pkLib.o} objFindResult]} {
                    if {[string match "*Error*" \$objFindResult]} {
                        exit 1
                    }
                }
            }
END_OBJ_MODULE_CHECK

#       check the result and if needed compile and load pkLib.o

        objfindres=$?

        if [ $objfindres -eq 2 ]
        then
            abort "FATAL ERROR : Target Server attach failed while checking \
pkLib.o"
        fi

        if [ $objfindres -eq 1 ]
        then
            if [ ! -r $2/pkLib.o ]
            then
                if [ -r $2/pkLib.c ]
                then
                    cd $2
                    echo "Compiling pkLib.o....."
                    echo ""
                    make pkLib.o
                    cd $3
                elif [ -r $4/pkLib.c ]
                then
                    cp $4/pkLib.c $2
                    cd $2
                    echo "Compiling pkLib.o....."
                    echo ""
                    make pkLib.o
                    cd $3
                else
                    abort "ERROR : $filemsg2"
                fi
            fi

        wtxtcl<<END_PKLIB_LOAD
            if {[catch {wtxToolAttach $1} attachRes]} {
                if {[string match "*Error*" \$attachRes]} {
                    exit 2
                }
            }

            if {[catch {wtxObjModuleLoad $2/pkLib.o} objLoadRes]} {
                if {[string match "*Error*" \$objLoadRes]} {
                    exit 1
                }
            msleep 2000
            }

END_PKLIB_LOAD

        objLoadResult=$?

        if [ $objLoadResult -eq 2 ]
        then
            abort "FATAL ERROR : Target Server attach failed while loading \
pkLib.o"
        fi

        if [ $objLoadResult -eq 1 ]
        then
            abort "ERROR : Cannot load pkLib.o"
        fi

        fi

    else
        echo "pkLib.o checking failed"
        exit 1
    fi
}

# reboot the target, check that it was successful and load pkLib.c

rebootTarget()
{
    wtxtcl<<END_REBOOT_TGT
        source $4/bspVal.tcl
        bspValInit $1 $2 $4
END_REBOOT_TGT

rebootRes=$?

    if [ $rebootRes -eq 1 ]
    then
        abort "ERROR: $1 target reboot failed"
    fi

    checkPkLib $2 $3 $4 $5
}


# scan and extract command line options

while(test $# -ne 0)
do
    case "$1" in
        "")
            abort "$usage" ;;
        -h)
            echo ""
            echo "This command executes one or more BSP Validation Test(s)"
            echo ""
            echo "$usage"
            echo ""
            echo "Options available are:
-h[elp]                print this message
-all                   run all the tests
-c[lean up]            clean up the log files
-r[eboot]              cancel reboot of target(s) before starting the test"
            echo ""
            echo "Available tests are :"
            echo "$tests"
            echo ""
            echo "You can specify an environement variable <TEST_LOG_FILE>
if you want to override the default log file"
            echo ""
            exit 0
            ;;
      -all)
          alltests=true
          ;;
      -s)
          shift
          target2=true
          tgtsvr2="$1"
          ;;
      -b)
          shift
          bspname1="$1"
          ;;
      -sb)
          shift
          bspname2="$1"
          ;;
      -t1SerDev)
          shift
          t1SerDev="$1"
          ;;
      -t1SerBaud)
          shift
          t1SerBaud="$1"
          ;;
      -t2SerDev)
          shift
          t2SerDev="$1"
          ;;
      -t2SerBaud)
          shift
          t2SerBaud="$1"
          ;;
      -c)
          cleanup=true
          ;;
      -r)
          reboot=false
          ;;
      -*)
          echo "invalid option : $usage"
          exit 1
          ;;
      *)
          paramList="$paramList $1"
          ;;
    esac

    shift
done

# parameter checking for first target

for param in $paramList
do
    if [ $tgtsvr1 = "undefined" ]
    then
        wtxtcl <<END_CHECK_SVR_NAME
            source $testDir/src/lib/checkServerName.tcl
            checkServerName $param
END_CHECK_SVR_NAME

        checkServerResult=$?

        if (test $checkServerResult -eq 3 )
        then
            exit
        fi

        if (test $checkServerResult -eq 1 )
        then
            testlist="$testlist $param"
        else
            #
            # Test if there is an ambiguity in target server name
            #
            if (test $checkServerResult -eq 2)
            then
                exit
            fi
            tgtsvr1=$param
        fi

    else
        testlist="$testlist $param"
    fi
done

# check for required parameters

if [ $tgtsvr1 = "undefined" ]
then
    echo "ERROR : Target server unspecified or unknown"
    abort "$usage"
fi

if [ $bspname1 = "undefined" ]
then
    echo "ERROR : A BSP name must be specified to run the Validation tests"
    abort "$usage"
fi

# Checking parameters specified for second target.

if [ $target2 = true ]
then
    if [ $tgtsvr2 = "undefined" ]
    then
        echo "ERROR : Target server for second target unspecified or unknown"
        abort "$usage"
    else
        wtxtcl<<END_CHECK_SLAVE_TGTSVR
            source $testDir/src/lib/checkServerName.tcl
            checkServerName $tgtsvr2
END_CHECK_SLAVE_TGTSVR

    checkSecondServerResult=$?

        if (test $checkSecondServerResult -eq 3 )
        then
            echo "ERROR : Error in finding second target server"
            exit
        fi

        if (test $checkSecondServerResult -eq 1 )
        then
            abort "ERROR : Target server for second target does not exist"
        else

#           Test if there is an ambiguity in target server name

            if (test $checkSecondServerResult -eq 2)
            then
                echo "ERROR : Error in finding second target server"
                exit
            fi
        fi
    fi
    if [ $bspname2 = "undefined" ]
    then
        echo "ERROR : If tests are to be performed on second target then it's\
 BSP name must be specified"
        abort "$usage"
    fi
fi

# diagnostic check for options specified

if ( [ $target2 = true ] && [ $tgtsvr1 = $tgtsvr2 ] )
then
    echo "ERROR : target server names are same"
    abort "$usage"
fi

if ( [ "$testlist" = "" ] && [ $alltests = false ] )
then
    echo "ERROR : either [-all] option or testlist must be specified"
    abort "$usage"
fi

# specify bsp directories

bspdir1=${TGT_DIR}/config/$bspname1
bspdir2=${TGT_DIR}/config/$bspname2

# getting required configuration parameters for first target

if [ -r $resDir/$bspname1.T1 ]
then
    . $resDir/$bspname1.T1
else
    abort "ERROR : $filemsg1"
fi

# overwrite serial port information if specified as command line
# arguments

if [ $t1SerDev != "undefined" ]
then
    T1_SER_DEVICE=$t1SerDev
    export T1_SER_DEVICE
fi
if [ $t1SerBaud != "undefined" ]
then
    T1_SER_BAUD=$t1SerBaud
    export T1_SER_BAUD
fi

# getting required configuration parameters for second target

if [ $target2 = true ]
then
    if [ -r $resDir/$bspname2.T2 ]
    then
        . $resDir/$bspname2.T2
    else
        abort "ERROR : $filemsg1"
    fi

# overwrite serial port information if specified as command line
# arguments

    if [ $t2SerDev != "undefined" ]
    then
        T2_SER_DEVICE=$t2SerDev
        export T2_SER_DEVICE
    fi
    if [ $t2SerBaud != "undefined" ]
    then
        T2_SER_BAUD=$t2SerBaud
        export T2_SER_BAUD
    fi
fi

# optional log file creation and cleanup

if [ "$TEST_LOG_FILE" != "" ]
then
    if [ $cleanup = true ]
    then
        rm -f $TEST_LOG_FILE
    fi
else
    TEST_LOG_DIR=$bspdir1/vtsLogs

    if [ ! -d ${TEST_LOG_DIR} ]
    then
        mkdir $TEST_LOG_DIR
    fi

    if [ $cleanup = true ]
    then
        rm -f ${TEST_LOG_DIR}/bspValidationLog.*
    fi

    TEST_LOG_FILE="${TEST_LOG_DIR}/bspValidationLog.$$"
    export TEST_LOG_FILE
fi

# indication of user abort

trap "echo                                              >> ${TEST_LOG_FILE}
      echo ""TESTING ABORTED BY USER""                  >> ${TEST_LOG_FILE}
      exit 1" 2 15


#
# all host specific parameters
#
host=$WIND_HOST_TYPE

case $host in
    "sun4-sunos4"|"sun4-solaris2"|"x86-win32")
        BSPTEST_HOST_RATES="150 300 600 1200 1800 2400 4800 9600 19200 \
38400";;
    "parisc-hpux9"|"parisc-hpux10"|"rs6000-aix4")
        BSPTEST_HOST_RATES="150 300 600 1200 2400 4800 9600 19200";;
esac

export BSPTEST_HOST_RATES


# rebooting the targets if necessary and checking for pkLib.o module


if [ $reboot = true ]
then
    if ( [ "$T1_BUS_MASTER" = "TRUE" ] || [ "$T1_BUS_MASTER" = "" ] )
    then
        rebootTarget T1 $tgtsvr1 $bspdir1 $testDir $tgtSrcDir

        if [ $target2 = true ]
        then
            rebootTarget T2 $tgtsvr2 $bspdir2 $testDir $tgtSrcDir
        fi
    elif [ "$T1_BUS_MASTER" = "FALSE" ]
    then
        if [ $target2 = true ]
        then
            rebootTarget T2 $tgtsvr2 $bspdir2 $testDir $tgtSrcDir
        fi

        rebootTarget T1 $tgtsvr1 $bspdir1 $testDir $tgtSrcDir
    else
        echo ""
        echo "WARNING : illegal parameter specified for T1_BUS_MASTER."
        echo "Please make sure <bspname>.T1 has correct value."
        echo "Reboot of target(s) cancelled."
        echo ""
    fi
else
    checkPkLib $tgtsvr1 $bspdir1 $testDir $tgtSrcDir

    if [ $target2 = true ]
    then
        checkPkLib $tgtsvr2 $bspdir2 $testDir $tgtSrcDir
    fi
fi

# print banner

echo ""
echo "                    BSP VALIDATION TEST"
echo "                    -------------------"
echo ""


echo "Target server                     : $tgtsvr1"
echo "BSP                               : $bspname1"
if [ $target2 = true ]
then
    echo "Second target server              : $tgtsvr2"
    echo "Second BSP                        : $bspname2"
fi

echo "Log file                          : $TEST_LOG_FILE"
echo ""
echo ""

# open windsh, source bspVal.tcl and run the test by calling bspValTest. The
# mechanism used for interaction with windsh is tcl "open" , "gets" , "puts"
# and "close" commands in conjuction with pipeline to windsh. This choice was
# made due to failure of I/O redirection with tcl on Windows hosts.

wtxtcl<<END_GET_ARCH_STR
    catch {set fd [open "|windsh $tgtsvr1 -T" r+ ]} openRes
    set endPattern "END_TEST"
    set cmd1 "source $testDir/bspVal.tcl"
    puts \$fd \$cmd1
    set cmd2 "bspValTest $tgtsvr1 $bspname1 $tgtsvr2 \
              $bspname2 $alltests $target2 $testlist"
    puts \$fd \$cmd2
    flush \$fd
    set line1 ""
    while {! [string match \$endPattern \$line1] } {
        catch {gets \$fd line1} errRes1
        if { \$errRes1 == -1 } {
            puts stdout "Error in receiving from windsh"
            exit 1
        }
 	if {([string match \$cmd1 \$line1] == 0) && \
 	    ([string match \$cmd2 \$line1] == 0) && \
            ([string match \$endPattern \$line1] == 0) } {
            puts stdout \$line1
 	}
    }
    puts \$fd "exit"
    flush \$fd
    close \$fd
END_GET_ARCH_STR


