#
# bspPkCommonProc.tcl - Tcl script with Common procedures for Bsp Validation.
#
# Copyright 1997-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01q,27oct01,sbs  fixing target reboot for vxWorks ROM image (SPR 30070)
# 01p,03oct01,pmr  SPR 69224: summary file in send_user proc.
# 01o,18sep01,sbs  merge from tor2_5.bsp_drv_facility branch for veloce
# 01n,16oct99,sbs  added optional argument for checkAndLoadFile().
# 01m,26jul98,sbs  modified printBanner() to log more information.
#                  (SPR #21750)
# 01l,07jul98,sbs  added an explicit "exit" command before closing pipe
#                  to windsh process.(SPR #20973).
# 01k,20apr98,p_b  Merge from ARM to Tornado 2_0_x
# 01j,20oct97,db   fixed bug in getWindshOutput() procedure.
# 01i,17oct97,sbs  changed date function, funcCall procedure for return
#                  value and error checking (SPR #9403).
# 01h,02sep97,db   added flush command in send_user procedure.
# 01g,25aug97,sbs  added getWindshOutput procedure and minor changes.
# 01f,20aug97,db   added taskDelete, vtsErrorHandler and getWindShRet
#                  procedures.
# 01e,31jul97,sbs  changed checkAndLoadFile.
# 01d,28jul97,db   doc fixes.
# 01c,21jul97,sbs  added numSort and trimSerOutput routines.
# 01b,14jul97,db   added procedures beforeColon, afterEquals, extract, symbol
# 		   and retcodeGet.
# 01a,28may97,sbs  written.
#
# DESCRIPTION
# This module contains all general routines required for BSP Validation
# test.
#*/

# globals

global env
set totalCount		0	;# total number of tests
set passCount		0	;# total number of tests passed
set failCount		0	;# total number of tests failed
set skipCount		0	;# total number of tests skipped
set logFile $env(TEST_LOG_FILE) ;# log file for the tests
set logFileId ""		;# log file ID


############################################################################
#
# checkAndLoadFile - check and load an object file on the target
#
# This routine loads an object module on the target given the path and
# file name. The user can provide an optional symbol entry argument for
# vxWorks images that might have the object module linked with them.
#
# SYNOPSIS:
#   checkAndLoadFile filePath filename symEntry
#
# PARAMETERS:
#   filePath: path of the object module to be loaded
#   filename: name of the object module
#
# RETURNS: never returns if it can't find the file
#
# ERRORS:
# "<filename> not loaded"
# "<filename> not found"
#

proc checkAndLoadFile {filePath filename {symEntry}} {

    if {$symEntry != ""} {
        if {[catch "wtxSymFind -name $symEntry" symCheck]} {
            if {[catch "wtxObjModuleFind $filename" objFindResult] } {
                send_user "$filename is being loaded" 0
                if {[catch {wtxObjModuleLoad $filePath/$filename} \
                    objLoadRes]} {
                fatalError "$filename not found"
                }
                msleep 2000
            }
        }
    } else {
        if {[catch "wtxObjModuleFind $filename" objFindResult] } {
            send_user "$filename is being loaded" 0
            if {[catch {wtxObjModuleLoad $filePath/$filename} \
                 objLoadRes]} {
                fatalError "$filename not found"
            }
            msleep 2000
        }
    }

}

###########################################################################
#
# eventPoll - polls for occurance of an event.
#
# This routine is used to poll for events occuring on the target server. We
# can specify the number of times to poll for events in case of any error.
#
# SYNOPSIS:
#   eventPoll msec [maxtries]
#
# PARAMETERS:
#   msec: number of seconds to sleep before trying for another event poll
#   maxtries: max number of trials
#
# RETURNS: the event type.
#
# ERRORS: N/A
#

proc eventPoll {msec {maxtries 0}} {
    set event ""
    if {$maxtries > 0} {
        set ix 0
        while {[set event [wtxEventGet]] == "" && $ix < $maxtries} {
            msleep $msec
            incr ix
        }
    } else {
        while {[set event [wtxEventGet]] == ""} {
            msleep $msec
        }
    }
    return $event
}


#############################################################################
#
# checkTgtSvr - checks for target server and if needed restarts it.
#
# This routine checks for target server by invoking an target function. If
# the target is restarted then it causes the target server to be restarted.
# Event polling is done to make sure that the server has restarted and
# running.
#
# SYNOPSIS:
#   checkTgtSvr tgtsvrName
#
# PARAMETERS:
#   tgtsvrName: name of the target server
#
# RETURNS: returns 1 if error on starting target server.
#
# NOTE:  This routine ends in an infinite loop until the target server
#        restarts.
#
# ERRORS: N/A
#

proc checkTgtSvr { tgtsvrName } {

    set done 0
    set handle [wtxToolAttach $tgtsvrName]
    wtxRegisterForEvent .*

    while {[set event [wtxEventGet]] != ""} {}

    set fcExpr "wtxFuncCall [lindex [wtxSymFind -name sysClkRateGet] 1]"

    while {! $done} {

        catch {set funcId [eval $fcExpr]} fcExprRes
        if { [lindex $fcExprRes 1] != "Error" } {
            set event [eventPoll 100]
            set done 1
        } else {
            set fcExprRes ""
            set done 0
            wtxToolDetach $handle
            while {[catch  {set handle [wtxToolAttach $tgtsvrName]} \
                                        attachRes]} {
                msleep 1000
                send_user "Connecting  targetserver.... " 0
            }
        wtxRegisterForEvent .*
        }
    }

    wtxToolDetach $handle

    if {[string match "CALL_RETURN" [lindex $event 0]]} {
        send_user "Targetserver reconnected" 0
        return 0
    } else {
        return 1
    }
}


##########################################################################
#
# fatalError - tcl script abort with proper message.
#
# RETURNS: never returns
#
# ERRORS:
# "<mesg>"
#

proc fatalError {mesg} {

    send_user "FATAL ERROR: $mesg" 1
    exit 1
  }

##########################################################################
#
# warning - display an warning message.
#
# RETURNS: N/A
#
# ERRORS:
# "<mesg>"
#

proc warning {mesg} {

    send_user "WARNING: $mesg" 1
  }


##########################################################################
#
# getBootPrompt - get the boot prompt.
#
# This routine reboots the target and gets to the boot prompt. If the 
# target is running VxWorks ROM image, it detects target reboot. 
#
# SYNOPSIS:
#   getBootPrompt serHandle timeout
#
# PARAMETERS:
#   serHandle: serial channel handle to be used
#   timeout: reboot timeout value for the target
#
# RETURNS: 
#   never returns if there is any error
#   returns 1 if it gets VxWorks boot prompt
#   returns 2 if it detects VxWorks ROM image boot 
#
# NOTE: If the test fails with the error in this routine then make sure
#       that the target rebooted properly before running any further tests.
#
# ERRORS:
# "Receive timeout while getting bootprompt"
# "Receiving error while getting bootprompt"
#

proc getBootPrompt {serHandle timeout} {

    set rebootCommand "\n"
    set autoBootPattern "auto-boot..."
    set vxBootPrompt "\[VxWorks Boot]:"
    set done 0

    serChanFlush $serHandle

    for {set count 0} {$count < 4} {incr count} {

        serChanSend $serHandle $rebootCommand

        set ret1 [serChanRecv $serHandle 1024 $timeout $autoBootPattern 1]

        send_user "serial output waiting for auto boot pattern is :\
                   [trimSerOutput $ret1] \n " 0


        if {[string match "Error*" $ret1]} {
            send_user "Receiving error while getting bootprompt... \
                       retrying" 0

            serChanFlush $serHandle
            continue
        } elseif {[string match "*timeoutError*" $ret1]} {
            if {[string match "*VxWorks Boot\]*" $ret1]} {
                set done 1
                break
            }
            # check to see if the target has rebooted with any ROM image
            # with WDB connection. 
            if {[string match "*WDB\: Ready*" $ret1]} {
                set done 2
                break
            }
            serChanFlush $serHandle
            continue
        } else {
            set done 1
            break
        }
    }

    if {$done == 1} {
        serChanSend $serHandle "\r"

        set ret2 [serChanRecv $serHandle 1024 $timeout $vxBootPrompt 0]

        if {[string match "timeoutError*" $ret2]} {
            fatalError "Receive timeout while getting bootprompt"
        } elseif {[string match "Error" $ret2]} {
            fatalError "Receiving error while getting bootprompt"
        }
    } elseif {$done == 2} {
        send_user "Target is rebooted with a VxWorks ROM image or equivalent \
                   with no boot parameters\n" 0
    } else {
        fatalError "Target reboot failed"
    }

    return $done

}


##############################################################################
#
# rebootTarget - reboot the target.
#
# This routine reboots the target with the given bootline paremeters in the
# configuration file.
#
# SYNOPSIS:
#   rebootTarget serHandle bootline timeout
#
# PARAMETERS:
#   serHandle:
#   bootline:
#   timeout:
#
# RETURNS: never returns if there is any error
#
# NOTE: If this routine ends with error then make sure that the target is
#       rebooted properly before any further testing.
#
# ERRORS:
# "Receive timeout while rebooting target"
# "Receiving error while rebooting target"
#

proc rebootTarget {serHandle bootline timeout} {

    set bootCommand "@\n"
    set vxBootPrompt "\[VxWorks Boot]:"
    set rebootPattern "WDB: Ready."

    set bootRet [getBootPrompt $serHandle $timeout]

    if {$bootRet == 2} {
        return
    } elseif {$bootRet == 1} {

        serChanFlush $serHandle

        serChanSend $serHandle "c\n"

        foreach param $bootline {
            if { $param == "" } {
                serChanSend $serHandle ".\n"
            } else {
                serChanSend $serHandle "${param}\n"
            }
        }

        for {set i 0} {$i < 30} {incr i} {
            msleep 1000
        }

        set ret3 [serChanRecv $serHandle 4096 $timeout $vxBootPrompt 1]

        send_user "serial output after sending boot line parameters is :\
               [trimSerOutput $ret3] \n" 0

        if {[string match "timeoutError*" $ret3]} {
            fatalError "Receive timeout while rebooting target"
        } elseif {[string match "Error" $ret3]} {
            fatalError "Receiving error while rebooting target"
        }

        serChanSend $serHandle $bootCommand

        set ret4 [serChanRecv $serHandle 8192 $timeout $rebootPattern 1]

        send_user "reboot pattern received is : [trimSerOutput $ret4] \n" 0

        if {[string match "timeoutError*" $ret4]} {
            fatalError "Receive timeout while rebooting target"
        } elseif {[string match "Error" $ret4]} {
            fatalError "Receiving error while rebooting target"
        }
    }

}

#******************************************************************************
#
# funcCall - call a function on the target and wait for event until
#            the right event is gotten.
#
# This procedure calls the function <func> passed as argument with
# some parameters <args> (thru the wtxFuncCall() routine) and waits for event
# until it gets the right one (thru the wtxEventPoll() routine).
#
# If wtxFuncCall{} fails, this routine raises an error exception.
#
# SYNOPSIS:
#   funcCall func [arguments]
#
# PARAMETERS
#   func: name of the function
#   arguments: function arguments if any
#
# RETURNS: the value returned by the called function.
#
# ERRORS:
# "Can't register for event for <func>"
# "Can't call <func>"
# "Can't get event for <func>"
#


proc funcCall {func args} {

    # register for (all) events before making a call.

    # Test if an exception occurred (case with no error handling procedure)
    if {[catch {wtxRegisterForEvent ".*"} resRegist]} {
        send_user "ERROR : Can't register for events on target server \
                   for $func" 0
        return "Error"
    }
    # Test if errorHandler has caught an exception
    # (case with use of an error handling procedure)
    if {$resRegist == ""} {
        send_user "ERROR : Can't register for events on target server \
                   for $func" 0
        return "Error"
    }


    set funcAddr [lindex [wtxSymFind -name $func] 1]

    # Make sure event queue is empty to be sure we will receive the event
    # generated by this function call and not a previous one.

    while {[wtxEventGet] != ""} {}

    # Test if an exception occurred (case with no error handling procedure)
    if {[catch "wtxFuncCall $funcAddr $args" funcId]} {
        send_user "ERROR : Can't call $func" 0
        return "Error"
    }
    # Test if errorHandler has caught an exception
    # (case with use of an error handling procedure)
    if {$funcId == ""} {
        send_user "ERROR : Can't call $func on target" 0
        return "Error"
    }

    # wait for event
    # Test if an exception occurred (case with no error handling procedure)
    if {[catch "wtxEventPoll 100 50" resEventPoll]} {
        send_user "ERROR : Can't get target server event for $func" 0
        return "Error"
    }
    # Test if errorHandler has caught an exception
    # (case with use of an error handling procedure)
    if {$resEventPoll == ""} {
        send_user "ERROR : Can't get target server event for $func" 0
        return "Error"
    }

    # wait for event until we get the right event
    while {([string compare [lindex $resEventPoll 0] CALL_RETURN] && \
            [string compare [lindex $resEventPoll 1]  $funcId])} {
        # Test if an exception occurred
        # (case with no error handling procedure)
        if {[catch "wtxEventPoll 100 50" resEventPoll]} {
            send_user "ERROR : Can't get target server event for $func" 0
            return "Error"
        }
        # Test if errorHandler has caught an exception
        # (case with use of an error handling procedure)
        if {$resEventPoll == ""} {
            send_user "ERROR : Can't get target server event for $func" 0
            return "Error"
        }
    }

    return [lindex $resEventPoll 2]
}

#############################################################################
#
# getBootline - get the bootline parameters.
#
# This routine is used to get the bootline parameters from the configuration
# file depending on which target (T1 | T2) is specified.
#
# SYNOPSIS:
#   getBootline tgt
#
# PARAMETERS:
#   tgt: name of the target (T1 | T2)
#
# RETURNS:
# string of bootline parameters
#
# ERRORS: N/A
#

proc getBootline {tgt} {

    global env
    set paramList "BOOT_DEVICE BOOT_PROCNUM BOOT_HOST BOOT_FILE BOOT_E \
                   BOOT_B BOOT_H BOOT_G BOOT_U BOOT_PW BOOT_F BOOT_TN \
                   BOOT_S BOOT_O"

    foreach param $paramList {
        lappend bootline $env(${tgt}_${param})
    }

    return $bootline
}

#############################################################################
#
# printHeader - display the header before start of test.
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc printHeader {testName} {

    send_user "Test started : [clock format [clock seconds]]" 0
    send_user "\n$testName :" 1

}

#############################################################################
#
# testResult - display the test result and update global counts.
#
# This routine displays the test result with the proper final condition and
# all the test counts as necessary.
#
# SYNOPSIS:
#   testResult passCondition message
#
# PARAMETERS:
#   passCondition: final condition of the test
#   message: message to be displayed with the final condition
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc testResult {passCondition message} {

    global passCount failCount skipCount totalCount

    incr totalCount

    if { $passCondition == 2 } {
        incr skipCount
        send_user [format "%-60s %s" "$message" ": SKIP"] 1
    } elseif { $passCondition == 1 } {
        incr passCount
        send_user [format "%-60s %s" "$message" ": PASS"] 1
    } else {
        incr failCount
        send_user [format "%-60s %s" "$message" ": FAIL"] 1
    }
}

###############################################################################
#
# printResults - display of final results.
#
# This routine displays the final results of the test suite.
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc printResults {} {

    global passCount failCount skipCount totalCount logFileId logFile

    send_user "\n\nFinal Results are :  " 0

    send_user [format "\n\t\t\t Tests %-10s %4s \n\t\t\t Tests %-10s %4s \
                       \n\t\t\t Tests %-10s %4s \n\t\t\t Tests %-10s %4s "\
                       RUN $totalCount PASSED $passCount FAILED $failCount \
                       SKIPPED $skipCount ] 1

    send_user "\n\nEnd of BSP Validation Test : [clock format [clock seconds]] \n" 0
}

###############################################################################
#
# send_user - write the message to stdout or log file or both.
#
# This routine opens the log file and appends the given message to it. The
# message is sent to the stdout also depending on <outFlag>.
#
# SYNOPSIS:
#   send_user message outFlag
#
# PARAMETERS:
#   message: message to write
#   outFlag: if set to 1 the write to both log file or write to only log file
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc send_user {message outFlag} {

    global logFileId logFile 

    set sumFile ${logFile}_screenDump

    set logFileId [open $logFile a+]
    set sumFileId [open $sumFile a+]

    flush stdout
    if {$outFlag} {
        puts stdout $message
        puts $logFileId $message
        puts $sumFileId $message
    } else {
        puts $logFileId $message
    }
    flush stdout
    close $logFileId
    close $sumFileId
}

################################################################################
#
# printBanner - print the banner of the test.
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc printBanner {firstTgt firstBsp secndTgt secndBsp} {

    global logFile 
    
    set sumFile ${logFile}_screenDump
    set sumFileId [open $sumFile a+]
    flush stdout

    set VTSversion "BSP Validation Test Suite version 2.0"

    set banner "\t\t\t$VTSversion\n"
    append banner "\t\t\t-------------------------------------\n\n\n"
    append banner "Test parameters :\n"
    append banner "-----------------\n\n"
    append banner "Target server \t\t: $firstTgt\n"
    append banner "BSP \t\t\t: $firstBsp\n"
    if {$secndTgt != "undefined"} {
        append banner "Second target server \t: $secndTgt\n"
    }
    if {$secndBsp != "undefined"} {
        append banner "Second BSP \t\t: $secndBsp\n"
    }
    append banner "Log file \t\t: $logFile\n\n\n"
    append banner "BSP Validation Test Started : [clock format [clock seconds]] \n"

    send_user "$banner" 0
    puts $sumFileId $banner
    flush stdout 
    close $sumFileId

}


#################################################################################
# afterEquals - return the part of <inText> that is after "="
#
# This routine is useful for stripping prefixes like "b=" and "e=" off
# boot parameters.
#
# SYNOPSIS:
#   beforeColon inText
#
# PARAMETERS:
#   inText: input text
#
# RETURNS:
#   part of string after "=" or whole inText if "=" not found
#
# ERRORS: N/A
#
#
proc afterEquals {inText} {

    #if there is no "=" in <inText>, return <inText>

    set afterIndex [expr [string first = $inText]+1]
    return [string range $inText $afterIndex end]
}

#################################################################################
# beforeColon - return the part of <inText> that is before ":"
#
# This routine is useful for stripping suffixes like ":ffffff00" off
# boot parameters.
#
# SYNOPSIS:
#   beforeColon inText
#
# PARAMETERS:
#   inText: input text
#
# RETURNS:
#   part of string before ":" or whole inText if ":" not found
#
# ERRORS: N/A
#

proc beforeColon {inText} {

    # if there is no ":" in <inText>, return <inText>

    set beforeIndex [expr [string last : $inText]-1]
    if {$beforeIndex < 0} {
        set retVal $inText
    } else {
        set retVal [string range $inText 0 $beforeIndex]
    }
    return $retVal
}


################################################################################
#
# extract - extract an element from a string
#
# This routine finds a list type string element located using the input
# parameters.  The <out> string is searched for the <search> string.  If
# not found, <default> is returned.  If found, the character offset <rel>
# is used to find a string element, which is returned.
#
# SYNOPSIS:
#   extract string search rel default
#
# PARAMETERS:
#   string: string to be searched
#   search: pattern lo look for
#   rel: relative position to the pattern
#   default: default value to return in case of failure
#
# RETURNS:
#   a string located using the input parameters
#
# ERRORS: N/A
#

proc extract {out search rel default} {

    if {[set count [string first $search $out]] != -1} {

        if {$rel < 0}  {
    	    set begin [string first ":" $out]
            set front [string range $out $begin [expr $count+$rel]]
            return [lindex $front [expr [llength $front]-1]]
        }
        set back [expr $count+[string length $search]+$rel]
        return [lindex [string range $out $back end] 0]

    } else {
        return $default
    }
}

################################################################################
#
# symbol - find a symbol in the symbol table
#
# SYNOPSIS:
#   symbol  name
#
# PARAMETERS:
#   name: name of symbol to search
#
# RETURNS:
#   symbol address
#
# ERRORS: N/A
#
proc symbol {name}  {
    return [lindex [wtxSymFind -name $name] 1]
}

################################################################################
#
# retcodeGet - get a function return code value
#
# SYNOPSIS:
#   symbol  name
#
# PARAMETERS:
#   name: name of symbol to search
#
# RETURNS:
#   symbol address
#
# ERRORS: N/A
#
proc retcodeGet  {} {
    while {[set event [wtxEventGet]] == ""} {
       	msleep 100
    }
    return [lindex $event 2]
}

################################################################################
#
# numSort - sort list by numeric (decimal) value
#
# This routine will sort <list> by the elements' numeric (decimal) value
# in either increasing or decreasing order, depending on <inc>.  If
# <inc> is -1, sorting will be done in decreasing order, otherwise
# <list> will be returned in increasing order.  This routine differs from
# the Tcl command "lsort" in that "lsort" uses ASCII value.
#
# SYNOPSIS:
#   numSort list
#
# PARAMETERS:
#   list: list to be sorted
#
# RETURNS:
#   <list> sorted in numeric order
#
# ERRORS: N/A
#

proc numSort {list inc} \
    {
    set count -1
    while {[incr count] < [llength $list]} \
        {
#       skip first value

        if {$count == 0} {continue}

#       set up indexes

        set pre [lindex $list [expr $count-1]]
        set cur [lindex $list $count]

#       swap pre and cur if out of order, then back up counter

        if {(($cur<$pre) && ($inc!=-1)) || (($cur>$pre) && ($inc==-1))} \
            {
            set list [lreplace $list [expr $count-1] $count $cur $pre]
            incr count -2
            }
        }
#   return sorted list

    return $list
    }

################################################################################
#
# trimSerOutput - trim the output of serial channel
#
# This routine is mainly developed to trim the output of serial channel which
# was necessary to remove control characters for unix host and also to trim
# the prefix and suffix received from the serial channel.
#
# SYNOPSIS:
#   trimSerOutput <pattern>
#
# PARAMETERS:
#   pattern: string to be modified
#
# RETURNS:
#   string after modification
#
# ERRORS: N/A
#

proc trimSerOutput {pattern} {

set count [string length $pattern]
set done 0
set resPattern ""

set firstVal [string first "{" $pattern]
set lastVal [string last "}" $pattern]

for {set i 0} {$i < $count} {incr i} {
    if {($i <= $firstVal) || ($i == $lastVal)} {
       continue
    }
    set retVal [string index $pattern $i]
    scan $retVal %c ascVal

    if {($ascVal == 13) && ($done)} {
    set done 0
    } else {
    append resPattern [format %c $ascVal]
    set done 1
    }

}

return $resPattern

}

################################################################################
#
# taskDelete - delete an existing task
#
# This procedure is used to delete a left over task from a previous test. It
# calls the function i() to get the list of all the tasks running on the target
# and tries to match the specified one using the task name and entry point.
# If a match is found, the task is deleted.
#
# SYNOPSIS:
#   taskDelete targetFd taskName taskEntry
#
# PARAMETERS:
#   targetFd: serial port handle(returned by tclSerialChanOpen)
#   taskName: name of the task to be deleted
#   taskEntry: entry point of task to be deleted
#
# RETURNS: N/A
#
# ERRORS: N/A
#
proc  taskDelete {targetFd taskName taskEntry} {

    # if symbol does not exist, do a simple return.

    if {[catch {symbol $taskEntry} result]} {
    	return
    }
    serChanFlush $targetFd
    wtxFuncCall -int [symbol i]
    set ret [retcodeGet]

    set ret1 [serChanRecv $targetFd 2048 20 "value" 1]

    set ret2 [scan [symbol $taskEntry] {0x%x} entryVal]
    set entryVal [format "%x" $entryVal]

    if {[string match *$taskName* $ret1] && \
        [string match *$entryVal* $ret1]} {
       set tid [extract $ret1 $entryVal 0 -1]
       scan $tid "%x" tid
       wtxContextKill CONTEXT_TASK $tid
    }
}

################################################################################
#
# getWindShRet -  get return of windsh cmd
#
# This procedure is used to execute windsh commands through the use of
# shParse. In case of failure, the procedure can do retrials of the command
# up to the number specified in the second argument.
#
# SYNOPSIS:
#   getWindShRet command maxRetrials
#
# PARAMETERS:
#   command: command to be executed with shParse
#   maxRetrials: max number of retrials allowed.
#
# RETURNS:
#        return code of shParse() or -1 if out of retrials
#
# ERRORS: N/A
#
proc getWindShRet {command maxRetrials}  {

    for {set count 0} {$count < $maxRetrials} {incr count} {
        if {[set ret [shParse $command]] == -1 } {
            if {$count == [expr $maxRetrials-1]} {
                return -1
            }
            send_user "shParse for $command failed... retrying" 0
            continue
        } else {
            break
        }
    }
    return $ret
}

################################################################################
#
# vtsErrorHandler - wtx error handler
#
# This procedure gets called whenever a wtx error occurs. It should be
# attached first using the function wtxErrorHandler.
#
# SYNOPSIS:
#   vtsErrorHandler handle cmd err tag
#
# PARAMETERS:
#   handle: target server handle
#   cmd: wtx command that failed
#   err: error message
#   tag: supplied when the procedure is attached
#
# RETURNS: N/A
#
# ERRORS: N/A
#
proc vtsErrorHandler {handle cmd err tag} {

    fatalError "wtx command $cmd failed"
}

#############################################################################
#
# getWindshOutput - send windsh command by opening a seperate windsh and
#                   get return value
#
# SYNOPSIS:
#   getWindshOutput <tgtsvrName> <command>
#
# PARAMETERS:
#   tgtsvrName: target server name
#   command: command to send
#
# RETURNS:
#   return value from windsh or ERROR
#
# ERRORS: N/A
#

proc getWindshOutput {tgtsvrName command} {

set windId [open "|windsh $tgtsvrName -T" r+]
set retVal ""

set cmd1 "shParse $command"
set cmd2 "puts stdout END"
set endPattern "END"

puts $windId $cmd1
puts $windId $cmd2

set line1 ""

flush $windId

while {! [string match "END" $line1]} {

    catch {gets $windId line1} errRes

    if {$errRes == -1} {
    puts $windId "exit"
    flush $windId
    close $windId
    return "ERROR"
    }

    if {([string match $cmd1 $line1] == 0) && \
        ([string match $cmd2 $line1] == 0) && \
        ([string match $endPattern $line1] == 0)} {
        lappend retVal "$line1 "
    }

}

puts $windId "exit"
flush $windId
close $windId
return $retVal

}


