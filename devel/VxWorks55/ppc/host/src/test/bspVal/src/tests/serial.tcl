#
# serial.tcl - serial I/O tests
#
# Copyright 1997-2002 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01r,12jul02,pmr  rearranged calls to pkGetMacroValue (SPR 79653).
# 01q,18sep01,sbs  merge from tor2_5.bsp_drv_facility branch for veloce
# 01p,30apr01,agf  Fix \n from test string per SPRs 33071 27682 22425 64277
# 01o,10oct99,sbs  modified changes made for SPR #22425 fix. 
# 01n,29oct99,sbs  added markup for html docs generation.
# 01m,16oct99,sbs  added extra argument for checkAndLoadFile() call.
# 01l,10jun99,sbs  merge from 2_0_x
# 01k,23feb99,sbs  added changes to make VTS compatible to
#                  Tornado 2.0 (SPR #22425).
# 01j,07jul98,sbs  corrected multiple looping for multiple serial loopback
#                  test to avoid huge tomeout value. (SPR #21048)
# 01i,21apr98,p_b  Merge from ARM to Tornado 2_0_x
# 01h,20oct97,sbs  removed output task in console I/O test for windows,
#                  changed fatalError renaming and corrected fatalError
#                  message in finding max baud rate for console (SPR #9309).
# 01g,04sep97,sbs  changed printLogo test to console I/O test and
#                  fixes to all other tests.
# 01f,29aug97,kbw  making man page fixes from jim
# 01e,16aug97,kbw  making man page fixes from DAT and Dahmane
# 01d,06jul97,sbs  doc changes.
# 01c,31jul97,db   doc tweaks for man page generation.
# 01b,31jul97,sbs  doc added.
# 01a,16jun97,sbs  written.
#
#
# SYNOPSIS:
# 
# bspVal <options> serial
# 
#
# DESCRIPTION:
# This test verifies serial driver functionality under adverse
# conditions.  See 'bspVal.sh' for an explanation of <options>.
# The entire test can take several minutes to run.
#
# \ml
# \m 'NOTE:'
# This reference entry lists the most likely reasons for a test's failure.
# However, it does not list all possible reasons.
# \me
#
# The first test stresses the serial console port by having a target function
# repeatedly send output to the console, 'tty', while input is coming in from
# the host.  The test checks that the target does not hang and that all
# the input sent was received intact by the target.  This test is run at the
# highest baud rate supported by both host and target.  If this test fails,
# check the serial connection.  The serial driver's receive and transmit
# facilities must be stable and functioning properly.  Generally, the driver
# should handle the receive interrupts before servicing transmit interrupts
# if both events are handled by the same ISR.
#
# The second test also stresses the serial console port. The pkConsoleEcho()
# routine puts the console port in echo mode, where all characters sent by
# the host are immediately echoed.  The test makes sure that every character
# sent to the console is received back by the host in the correct order and
# quantity.  This test is run at the highest baud rate supported by both
# host and target.  If this test fails, check for any possible interrupt
# race conditions in the serial driver.
#
# The next two tests require that all free serial ports (other than
# the 'tty's used for the console and SLIP) be connected to run in
# loopback mode.  This can usually be accomplished by running a jumper
# wire from the transmit/output line to the receive/input line.  It might
# also be necessary to run a jumper from the RTS to the CTS line for ports
# with hardware handshaking.
#
# The third test spawns a single pkLoopbackTest() task for
# each serial port connected in loopback mode.  Each port sees a pattern
# of sequential characters sent and received at the maximum baud rate.  All
# characters must be received in the order sent.  If this test fails, check
# that all serial ports under test are set up correctly for loopback mode.
# Make sure that the serial driver is handling receive and transmit
# interrupts properly.  Make sure that all serial channels under test have
# been configured and initialized to the correct values.
#
# The fourth test spawns several pkLoopbackTest() tasks for
# each serial port connected in loopback mode.  Each task repeatedly sends a
# particular character to one of the ports.  There are several tasks for
# each serial port under test.  Each port should receive all the characters
# that were sent.  If this test fails, check the same problem areas suggested
# for the third test.
#
# The third and fourth tests spawn one or more pkLoopbackTest() tasks
# for each serial device connected in loopback mode.  These tasks run
# concurrently, sending and receiving characters at the highest baud rate
# supported by both host and target.  It is possible that target boards with
# slower CPUs and many serial devices might not possess the raw processing power
# required to keep up with the tests.  If hardware limitations are causing the
# loopback tests to fail, the tests can be configured to lighten the processor
# load. The 'serial.tcl' tests run at the highest baud specified in the 'bspVal'
# macro, BSPTEST_HOST_RATES (provided that the rate is supported by both the
# host and target).
#
# Modifying BSPTEST_HOST_RATES to exclude the higher common baud rates will
# cause the tests to run at a lower rate, thus avoiding possible hardware-
# related problems.  Additionally, the pkLib.c constant PK_LOOP_TASK_NUM
# can be changed from the default value of 4 to decrease the number of
# concurrent tasks running in the fourth test.  In this case, the object
# code file pkLib.o must be rebuilt from the changed pkLib.c source file.
#
# Target boards with several serial devices and a small amount of
# local RAM (1Mbyte or under) might run out of memory while executing the tests.
# In this case, either configure the target to use an external memory card
# or decrease the number of tasks running as described above.
#
# If the test script execution ends prematurely, the target board might be
# left set to a baud rate other than 'T1_TIP_BAUD'.  This situation could
# leave the board in such a state that a power cycle reset would be necessary
# to continue with other tests.
#
# Barring serious hardware or software limitations (such as an identifiable
# VxWorks problem), the target board must pass all tests for the BSP to be
# validated.
#
# CONFIGURATION PARAMETERS:
# \is
# \i 'BSPTEST_HOST_RATES' 25
# Baud rates supported on host. (required)
# \i 'T1_SER_DEVICE'
# Serial device to be used on host. (required)
# \i 'T1_SER_BAUD'
# Default serial baud rate to be used. (required)
# \ie
#
# EXAMPLE:
# Output consists of:
# \cs 
#                          BSP VALIDATION TEST
#                          -------------------
#
#     Target server                     : t53-160
#     BSP                               : mv147
#     Log file                          : /tmp/bspValidationLog.5219
#
#
#
#     Serial Test :
#     console I/O test                                          : PASS
#     console echo test                                         : PASS
#     sequential loopback test                                  : PASS
#     multiple loopback test                                    : PASS
#
#                       Tests RUN           4
#                       Tests PASSED        4
#                       Tests FAILED        0
#                       Tests SKIPPED       0
#
# \ce 
#
# SEE ALSO:
# 'bspVal.sh',
# bspPkCommonProc.tcl,
# envLib.tcl,
# serLib.tcl,
# pkLib.c
#
# NOROUTINES
#
#

# print header

printHeader    "Serial Test"

# set max trials to do in case of error

set maxTries 5

# read environment variables

set rates       [readEnvRequired BSPTEST_HOST_RATES]
set tipDevice   [readEnvRequired T1_SER_DEVICE]
set tipBaud     [readEnvRequired T1_SER_BAUD]

send_user " BSPTEST_HOST_RATES are $rates \n\
           T1_SER_DEVICE is $tipDevice \n\
           T1_SER_BAUD is $tipBaud \n\
           \n" 0

# attach to the target server

set targetHd [wtxToolAttach $firsttgtsvrname]
wtxRegisterForEvent .*

# load the test routine library

checkAndLoadFile $firstbspdir pkLib.o pkGetMacroValue

# set task's stack size to 10Kbytes

set cmd " spTaskStackSize=10000 "

set ret [getWindShRet $cmd $maxTries]

if {$ret == -1} {
    fatalError "task stack size setting failed"
}

# find the serial port used by the console

set cmd " pkGetMacroValue(\"CONSOLE_TTY\") "

set consoleTTY [getWindShRet $cmd $maxTries]

if {$consoleTTY == -1} {
    fatalError "CONSOLE_TTY macro not defined"
}

send_user "CONSOLE_TTY is $consoleTTY" 0

# open a serial channel

set fd [serChanOpen $tipDevice $tipBaud]
serChanFlush $fd

# check if SLIP is defined

set cmd " pkGetMacroValue(\"SLIP_TTY\") "

set slipTTY [getWindShRet $cmd $maxTries]

if {$slipTTY != -1} {

#   check if SLIP is running by checking the network configuration

    set cmd "ifShow"
    set ret1 [getWindshOutput $firsttgtsvrname $cmd]

    if {$ret1 == -1} {
        fatalError "ifShow command failed"
    }

    send_user "ifShow() output is : \n\n [trimSerOutput $ret1]\n" 0

    if {[string match "*sl (unit number*" $ret1] == 0} {
        set slipTTY -1
    }
}

# get the number of tasks to stress each port for multiple serial
# loopback test.

set cmd " pkGetMacroValue(\"PK_LOOP_TASK_NUM\") "
set numTasks [getWindShRet $cmd $maxTries]

if {$numTasks == -1} {
    fatalError "pkGetMacroValue() failed for PK_LOOP_TASK_NUM"
}

send_user "PK_LOOP_TASK_NUM for Multiple Serial loopback test is $numTasks" 0


# get a list of all tty's not used for the console and SLIP

set ret2 [getWindshOutput $firsttgtsvrname "devs"]

set loopDevs ""

# extract the serial port numbers out of the devices list

while {[set count [string first tyCo $ret2]] != -1} {

    set ret2 [string range $ret2 [expr $count+5] end]
    set device [lindex $ret2 0]

#   skip over console and SLIP tty's

    if {$device == $consoleTTY} {continue}
    if {$device == $slipTTY} {continue}
    if {$device == ""} {continue}

    lappend loopDevs $device
}

#   find the maximum supported baud rate for each serial ports

set loopDevs_new ""
set devMaxBaud ""
set rates [numSort $rates -1]

foreach device $loopDevs {
#   open a file descriptor for the serial port

    set cmd " open(\"/tyCo/${device}\",2,0) "

    set fddLoopback [getWindShRet $cmd $maxTries]

    if {$fddLoopback == -1} {
        fatalError "open for /tyCo/${device} failed"
    }

#   call ioctl() to change baud rate of port - break when ioctl() works

    foreach rate $rates {

        set cmd " ioctl($fddLoopback,4,$rate) "
        set baudCk [getWindShRet $cmd $maxTries]

        if {$baudCk == 0} {
            break
        }
    }

#   make a list of the max baud rates for each port

    if {$baudCk == 0} {
        lappend loopDevs_new $device
        lappend devMaxBaud $rate
    }

    if {$fddLoopback != -1} {
        set cmd " close($fddLoopback) "
        set ret [getWindShRet $cmd $maxTries]

        if {$ret == -1} {
            fatalError "close for /tyCo/${device} failed"
        }
    }
}

set loopDevs $loopDevs_new
set baudCk -1

if {$loopDevs != ""} {
    send_user "Devices being tested for loopBack tests are : \n" 0
    foreach param $loopDevs {
        send_user "/tyCo/${param}" 0
    }
}

# find the max baud rate for the console

set cmd " open(\"/tyCo/${consoleTTY}\",2,0) "
set fdConsole [getWindShRet $cmd $maxTries]

if {$fdConsole == -1} {
    fatalError "open failed for consoleTTY"
}

foreach rate $rates {
#   call ioctl() to change baud rate of console

    set cmd " ioctl($fdConsole,4,$rate) "
    set ret3 [getWindShRet $cmd $maxTries]

    if {$ret3 == -1 } {
        send_user "Console deos not support baud rate $rate" 0
        continue
    }

#   close serial session and reopen at new baud rate

    serChanFlush $fd
    serChanClose $fd

    set fd [serChanOpen $tipDevice $rate]
    serChanFlush $fd

#   now repeat the ioctl() call and verify the return value

    set cmd " pkPrintTest1($fdConsole, \"abcdefghijklmnopqrstuvwxyz\\n\", 200) "

    set ret [getWindShRet $cmd $maxTries]

    if {$ret == -1} {
        warning "pkPrintTest command failed"
        set baudCk -1
    }

    set ret4 [serChanRecv $fd 1024 10 "abcdefghijklmnopqrstuvwxyz" 1]

    if {[string match "timeoutError*" $ret4] || \
        [string match "Error" $ret4]} {
        set baudCk -1
    } else {
        set baudCk 0
        break
    }
}

# check that a maximum rate for the console was found

if {$baudCk == -1} {
    fatalError "could not find maximum baud rate for the console"
}

set maxRate $rate

send_user "maximum baud rate at which console is tested is $maxRate" 0

serChanFlush $fd
serChanClose $fd

# compute timeout multiplier

set timeMult [expr (2400/$rate)+1]

# redefine a new fatalError procedure so that console baud rate is reset
# before exit

rename fatalError fatalError_old

proc fatalError {message} {

    # set max trials to do in case of error

    set maxTrials 5

    # get default serial baud Rate and if not found set default to 9600

    set baudRate [readEnvOptional T1_SER_BAUD 9600]

    # get console tty and open it

    set cmd " pkGetMacroValue(\"CONSOLE_TTY\") "

    set consoleHd [getWindShRet $cmd $maxTrials]
    if {$consoleHd == -1} {
        fatalError_old "could not get value of CONSOLE_TTY"
    }

    set cmd " open(\"/tyCo/$consoleHd\",2,0) "

    set consoleFd [getWindShRet $cmd $maxTrials]
    if {$consoleFd == -1} {
        fatalError_old "could not open file desc for console"
    }

    # reset console to original baud rate

    set cmd " ioctl($consoleFd,4,$baudRate) "
    set ret [getWindShRet $cmd $maxTrials]

    if {$ret == -1} {
        fatalError_old "ioctl command failed while resetting the console"
    }

    set cmd " close($consoleFd) "
    set ret [getWindShRet $cmd $maxTrials]

    if {$ret == -1} {
        fatalError_old "command failed while closing console file descriptor"
    }

#   call original fatalError routine

    fatalError_old $message
}


# start console I/O test - sending chars while console has continuous output

send_user " ## Starting console I/O test ## " 0

set tShellId ""

# open a serial channel and clear it

set fd [serChanOpen $tipDevice $maxRate]
serChanFlush $fd

# find if shell is running on the target and if so suspend it

set ret [getWindshOutput $firsttgtsvrname "i"]

send_user "tasks running on the target are : \n $ret" 0

if {[string match "*tShell*" $ret]} {

    set cmd " taskNameToId(\"tShell\") "
    set tShellId [getWindShRet $cmd $maxTries]

    if {$tShellId == -1} {
        fatalError "cannot get target shell ID"
    }

    set cmd " taskSuspend($tShellId) "
    set ret [getWindShRet $cmd $maxTries]

    if {$ret == -1} {
        fatalError "could not suspend target shell"
    }

    # initial setup

    set cmd " pkSerialTestInit "
    set ret [getWindShRet $cmd $maxTries]

    if {$ret == -1} {
        fatalError "console initialization failed"
    }

}

# strings sent to target

serChanFlush $fd

set vars "alameda bandundu krasnoyarsk leipzig pacasmayo \
          peterborough saskatoon zhaotong"

# spawn the output test task on the target

set priority 100
set options  0x3
set stackSize 5000
set serIterations 1000
set taskTimeout 60

set host_type [wtxHostType]

if {$host_type == "x86-win32"} {
    send_user "\n\nNOTE : \n There is no output task (pkPrintSerialOutput)\n\
               spawned on Windows. This is due to problems in controlling\n\
               serial devices using system calls under the Windows operating\n\
               system.\n\n" 0
} else {
    set cmd " taskSpawn(\"serTest\", $priority, $options, \
                        $stackSize, pkPrintSerialOutput, \
                        $serIterations) "
    set tid [getWindShRet $cmd $maxTries]

    if {$tid == -1} {
        fatalError "Can't spawn task pkPrintSerialOutput"
    }
}

msleep 200

serChanFlush $fd

# make sure that the global abort variable is reset

set cmd " pkSerChanAbort = 0 "
set ret [getWindShRet $cmd $maxTries]

if {$ret == -1} {
    fatalError "reset of pkSerChanAbort failed"
}

serChanFlush $fd

# spawn the input test task on the target

set priority 100
set options  0x3
set stackSize 5000

set cmd "taskSpawn(\"inputTest\", $priority, $options, \
                     $stackSize, pkSerialTest) "
set tid [getWindShRet $cmd $maxTries]

if {$tid == -1} {
    fatalError "Can't spawn task pkSerialTest"
}

msleep 500
serChanFlush $fd

# send serial input to console while pkPrintSerialOutput running

for {set count 0} {$count < 8} {incr count} {

    set varName [lindex $vars $count]

    if {$count == 7} {

        set cmd " pkSerChanAbort = 1 "
        set ret [getWindShRet $cmd $maxTries]

        if {$ret == -1} {
            fatalError "abort of pkPrintSerialOutput failed"
        }
    }

    send_user "string being sent is $varName" 0

    serChanFlush $fd
    serChanSend $fd "$varName\n"
    msleep 500
    serChanFlush $fd

}

serChanFlush $fd

for {set i 0} {$i < $taskTimeout} {incr i} {

#    msleep 100

    set ret1 [getWindshOutput $firsttgtsvrname "i"]

    if {[string match "*pkPrintSe*" $ret1]} {
        serChanFlush $fd
        continue
    } else {
        break
    }

}

serChanFlush $fd
msleep 1000

set p1 0

send_user "strings received are :\n" 0

for {set count 0} {$count < 8} {incr count} {

    wtxFuncCall [lindex [wtxSymFind -name pkSerChanOutput] 1] $count

    set recvPat [lindex $vars $count]

    set ret1 [serChanRecv $fd 1208 10 "$recvPat" 1]

    send_user "the string received is [trimSerOutput $ret1]" 0

    if {[string match "timeoutError*" $ret1]} {
        warning "string match failed for serial I/O test"
        set p1 1
        break
    }
    if {[string match "Error" $ret1]} {
        warning "string match error for serial I/O test"
        set p1 1
        break
    }

}

serChanFlush $fd
serChanClose $fd

testResult [expr {$p1 == 0}] "console I/O test"

# spawn pkConsoleEcho

send_user " ## Starting console echo test ## " 0

set priority 100
set options  0x3
set stackSize 5000
set taskTimeout 60

set cmd " pkConsoleEchoAbort = 0 "
set ret [getWindShRet $cmd $maxTries]

if {$ret == -1} {
    fatalError "reset of pkConsoleEchoAbort failed"
}

# open and clear the serial channel

set fd [serChanOpen $tipDevice $maxRate]
serChanFlush $fd

set cmd " taskSpawn(\"consoleTest\", $priority, $options, \
                    $stackSize, pkConsoleEcho, 0) "
set tid [getWindShRet $cmd $maxTries]

if {$tid == -1} {
    fatalError "Can't spawn task pkConsoleEcho"
}

set echoRes 0

for {set len 37} {$len < 123} {incr len} {
#   only echo printable characters

    if {$len == 92} {continue}
    set out [format %c $len]

    serChanSend $fd "$out$out\n"
    set ret6 [serChanRecv $fd 1024 3 "$out$out" 1]

    if {[string match "timeoutError*" $ret6]} {
        set cmd " pkConsoleEchoAbort=1 "
        set ret [getWindShRet $cmd $maxTries]

        if {$ret == -1} {
            send_user "set of pkConsoleEchoAbort failed" 0
        }

        set echoRes 1
        break
    } elseif {[string match "Error" $ret6]} {
        set cmd " pkConsoleEchoAbort=1 "
        set ret [getWindShRet $cmd $maxTries]

        if {$ret == -1} {
            send_user "set of pkConsoleEchoAbort failed" 0
        }

        set echoRes 1
        break
    }
}

# abort the task and make sure serial channel is cleared

set cmd " pkConsoleEchoAbort=1 "
set ret [getWindShRet $cmd $maxTries]

if {$ret == -1} {
    send_user "set of pkConsoleEchoAbort failed" 0
}

serChanSend $fd "\n"
serChanFlush $fd

testResult [expr {$echoRes == 0}] "console echo test"

serChanFlush $fd

# make sure that console is reset irrespective of the result

set cmd " pkSerialTestInit "
set ret [getWindShRet $cmd $maxTries]

if {$ret == -1} {
    fatalError "reset of console after console echo failed "
}

serChanFlush $fd

serChanClose $fd

# start sequential serial loopback test

send_user " ## Starting sequential serial loopback test ## " 0

set testTMO 120000
set res 0
set indx -1

set loopDevs_new ""
set devMaxBaud_new ""

# clear the global abort variable for loopback test

set cmd " pkLoopbackAbort=0 "
set ret [getWindShRet $cmd $maxTries]

if {$ret == -1} {
    fatalError "pkLoopbackAbort setting failed"
}

# open a serial channel and clear it

set fd [serChanOpen $tipDevice $maxRate]
serChanFlush $fd

# start the sequential loopback test on all available serial ports

foreach device $loopDevs {
    # recall the max baud rate for this port

    set maxBaud [lindex $devMaxBaud [incr indx]]

    # spawn the test task on the target

    set priority 100
    set options  0x3
    set stackSize 5000

    set cmd " taskSpawn(\"SeqSerTest\", $priority, $options, \
                         $stackSize, pkLoopbackTest, 0, \
                         $device,$maxBaud,0) "

    set tid [getWindShRet $cmd $maxTries]

    if {$tid == -1} {
        fatalError "Can't spawn task pkLoopbackTest"
    }

    set ret7 [serChanRecv $fd 1024 10 "pkLoopbackTest: Error" 1]

    if {[string match "timeoutError*" $ret7]} {
        # keep track of the ports that are connected for loopback

        lappend loopDevs_new $device
        lappend devMaxBaud_new $maxBaud
    } elseif {[string match "Error" $ret7]} {
        fatalError "error in receiving during loopback test"
    } else {
        warning "loopback not connected on serial port $device"
    }
}

set loopDevs $loopDevs_new
set devMaxBaud $devMaxBaud_new

# check that at least one port has loopback connected, else nothing to test

if {$loopDevs == ""} {
    warning "no serial devices to test - loopback not connected"
    set cmd " pkLoopbackAbort=1 "
    set ret [getWindShRet $cmd $maxTries]

    if {$ret == -1} {
        fatalError "pkLoopbackAbort setting failed"
    }
    testResult 2 "sequential loopback test"
    msleep 1000
} else {

#   send test status to user

    msleep [expr $testTMO/3]

    send_user "# 1/3 through sequential loopback test" 0
    msleep [expr $testTMO/3]

    send_user "# 2/3 through sequential loopback test" 0
    msleep [expr $testTMO/3]

#   check that loopback tasks are still operating -> pass

    foreach device $loopDevs {
        set lastChar 0

#       check each port twice to make sure still running

        for {set check 0} {$check < 2} {incr check} {
            msleep 3000

#           show the loopback count stats

            serChanFlush $fd

            set cmd " sp pkSeqLoopShow,$device "
            set ret [getWindshOutput $firsttgtsvrname $cmd]
 
            if {$ret == -1} {
                fatalError "pkSeqLoopShow func call failed"
            }
 
            set ret8 [serChanRecv $fd 1024 10 "OK" 1]
            send_user  "pkSeqLoopShow output for tyCo/${device} is :\n \
                        [trimSerOutput $ret8]" 0
 
            if {[string match "timeoutError*" $ret8] || \
                [string match "Error" $ret8]} {
                fatalError "pkSeqLoopShow did not return"
            }

            if {$res == -1} {continue}

            set ret9 [trimSerOutput $ret8]

            set chars [lindex $ret9 [expr [lsearch $ret9 \
                       characters]+2]]
            set errors [lindex $ret9 [expr [lsearch $ret9 \
                        errors]+2]]

#           make sure counters still going

            if {($chars == $lastChar) || ($errors > 0)} {
                set res -1
            }
            set lastChar $chars
        }
    }

    # stop loopback task(s)

    set cmd " pkLoopbackAbort=1 "
    set ret [getWindShRet $cmd $maxTries]

    if {$ret == -1} {
        fatalError "pkLoopbackAbort setting failed"
    }

    testResult [expr {$res == 0}] "sequential loopback test"
}

serChanFlush $fd
serChanClose $fd
msleep 2000

# reset pkLoopbackAbort variable

set cmd " pkLoopbackAbort = 0 "
set ret [getWindShRet $cmd $maxTries]

if {$ret == -1} {
    fatalError "reset of pkLoopbackAbort failed"
}

# start multiple serial loopback test

send_user " ## Starting multiple serial loopback test ## " 0

# initialize test parameters

set res 0
set timeSpent 0
set charsToSend 25000
set indx -1

set loopDevs_new ""
set devMaxBaud_new ""

# open a serial channel and clear it

set fd [serChanOpen $tipDevice $maxRate]
serChanFlush $fd

# start the multiple loopback test on all serial ports connected for loopback

foreach device $loopDevs {
#   recall the max baud rate for this port

    set maxBaud [lindex $devMaxBaud [incr indx]]

    # spawn the test task on the target

    set priority 100
    set options  0x3
    set stackSize 5000

    serChanFlush $fd

    set cmd " taskSpawn(\"multiSerTest\", $priority, $options, \
                                $stackSize, pkLoopbackTest, 1, \
                                $device,$maxBaud,$charsToSend) "
    set tid [getWindShRet $cmd $maxTries]

    if {$tid == -1} {
        fatalError "Can't spawn task pkLoopbackTest"
    }

    set ret10 [serChanRecv $fd 1024 10 "pkLoopbackTest: Error" 1]

    if {[string match "timeoutError*" $ret10]} {
#       keep track of the ports that are connected for loopback

        lappend loopDevs_new $device
        lappend devMaxBaud_new $maxBaud
    } elseif {[string match "Error" $ret10]} {
        fatalError "error in receiving during loopback test"
    } else {
        warning "loopback not connected on serial port $device"
    }
}

set loopDevs $loopDevs_new
set devMaxBaud $devMaxBaud_new

# check that at least one port has multiple loopback test running

if {$loopDevs == ""} {
    warning "no serial devices to test - loopback not connected"

    set cmd " pkLoopbackAbort=1 "
    set ret [getWindShRet $cmd $maxTries]

    if {$ret == -1} {
        fatalError "command to set pkLoopbackAbort failed"
    }

    testResult 2 "multiple loopback test"
    msleep 1000
} else {
#   calculate how long test *should* take to run, then x2 and +50 to be safe

    set minBaud [lindex [numSort $devMaxBaud_new 1] 0]
    set maxTimeout [expr 50+${numTasks}*${charsToSend}*20/${minBaud}]

#   wait until overall timeout and send status to user

    for {set i 0} {$i < $maxTimeout} {incr i} {
        msleep 1000
        if {[expr $i%20] == 0} {
            send_user "going through multiple loopback after \
                       [expr {$i+1}] seconds" 0
        }
    }

#   check results for each device tested

    foreach device $loopDevs {

        serChanFlush $fd

        set cmd " sp pkMultiLoopShow,$device "
        set ret [getWindshOutput $firsttgtsvrname $cmd]
 
        if {$ret == -1} {
            fatalError "command for pkMultiLoopShow failed"
        }
 
        set ret12 [serChanRecv $fd 2048 10 "OK" 1]
 
        if {[string match "timeoutError*" $ret12] || \
            [string match "Error" $ret12]} {
            fatalError "pkMultiLoopShow did not return"
        }
 
        set ret12 [trimSerOutput $ret12]
 
        send_user "return value of pkMultiLoopShow for tyCo/${device}\
                   is : \n $ret12" 0

#       check if error earlier

        if {$res == -1} {continue}

        while {[set index [lsearch $ret12 count]] != -1} {
#           check that all chars sent were received

            set count [lindex $ret12 [incr index 2]]
            set ret12 [lrange $ret12 $count end]

            if {$count != $charsToSend} {
                set res -1
            }
        }

    }

# abort pkLoopbackTest task anyway

    set cmd " pkLoopbackAbort=1 "
    set ret [getWindShRet $cmd $maxTries]

    if {$ret == -1} {
        fatalError "command to set pkLoopbackAbort failed"
    }


    testResult [expr {$res == 0}] "multiple loopback test"

}

# close the serial channel and reset pkLoopbackAbort variable value

serChanFlush $fd
serChanClose $fd

# reset the abort variable

set cmd " pkLoopbackAbort=0 "
set ret [getWindShRet $cmd $maxTries]

if {$ret == -1} {
    fatalError "command to set pkLoopbackAbort failed"
}

# reset console to original baud rate and test

set cmd " ioctl($fdConsole,4,$tipBaud) "
set ret13 [getWindShRet $cmd $maxTries]

if {$ret13 == -1} {
    fatalError "console reset for default baud rate failed"
}

msleep 1000

set fd [serChanOpen $tipDevice $tipBaud]
serChanFlush $fd

# test the console to make sure that the connection is reset

set cmd " pkPrintTest1($fdConsole, \"abcdefghijklmnopqrstuvwxyz\", 100) "
set ret14 [getWindShRet $cmd $maxTries]

if {$ret14 == -1} {
    fatalError "call to pkPrintTest failed"
}

set ret15 [serChanRecv $fd 1024 10 "abcdefghijklmnopqrstuvwxyz" 1]
send_user "final serial output from a-z is :\n \
           [trimSerOutput $ret15]" 0

if {[string match "timeoutError*" $ret15] || \
    [string match "Error" $ret15]} {

    fatalError "console not responding"
}

# close all file descriptors

set cmd " close($fdConsole) "
set ret16 [getWindShRet $cmd $maxTries]

if {$ret16 == -1} {
    fatalError "close of console file desc failed"
}

if {$tShellId != ""} {
    set cmd " taskResume($tShellId) "
    set ret17 [getWindShRet $cmd $maxTries]

    if {$ret17 == -1} {
        fatalError "taskResume shell task failed"
    }
}

serChanSend $fd "\n"

serChanFlush $fd
serChanClose $fd

msleep 2000

# rename the original fatalError procedure

rename fatalError ""
rename fatalError_old fatalError

# detach the target server

wtxToolDetach $targetHd




