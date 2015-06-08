# shell.tcl - WindSh Tcl entry point
#
# Copyright 1994-1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 07l,15jan03,lcs  Integrate SPR#26657 this will fix SPR#76840 - replaces
#		   [gets stdin] by [stdinGet].
# 07k,09nov00,zl   added shellShSetup call for SuperH.
# 07k,20oct99,myz  undo last fixes.
# 07j,30sep99,myz  modified symFindByCName and others for CW4000_16 support.
# 07j,07may99,jmp  added a x86-win32 conditional sleep in
#		   CALL_RETURN_EventPoll{} (SPR# 26594).
# 07i,24feb99,jmp  fixed a bug in the remainingEvents list handling (SPR#
#                  25292).
# 07h,19feb99,jmp  added procedure targetIsAlive{} to check if target is
#                  alive before starting the clean-up phase (SPR# 25217).
# 07g,18feb99,jmp  reset the interrupted flag when entering in
#                  windShErrorHandler{} (SPR# 25176).
# 07f,16feb99,jmp  fixed interrupted_Handler{} when the interrupted function
#		   is not a target function (SPR# 25109).
# 07e,02feb99,dbt  fixed exception handler if multitasking is not started.
#		   (SPR #24849).
# 07d,29jan99,jmp  fixed SPR# 24612: windsh can lose event unhandled events.
# 07c,05jan99,jmp  modified redirection system to use the same channel/fd
#		   (SPR# 24360). Now IO redirection are initialized using
#		   wtxDirectCall instead of wtxFuncCall (See SPR# 9202),
#		   also IO are no longer redirected in AGENT_MODE_EXTERN.
# 07b,18nov98,jmp  added progress meter to attachment message. added message
#                  to indicate how to interrupt windsh attachment (SPR# 23337).
# 07a,09nov98,jmp  modified to source windHelpLib on first CTRL-W.
# 06z,03nov98,jmp  modified VIO_WRITE handler to support memory blocks.
# 06y,03nov98,jmp  modified eventCheck{} to speed-up the event processing in
#                  particular for VIO_WRITE events.
# 06x,28oct98,jmp  modified to only source windHelpLib.tcl when WindSh is
#                  attached.
# 06w,09oct98,jmp  added windHelpLib.tcl sourcing.
# 06v,21sep98,jmp  made funcCalls done during WindSh initialization and
#                  termination unbreakable.
# 06u,08sep98,jmp  fixed funcCallsIoUnset{} to behave correctly with emulateur
#		   backend on exit.
# 06t,07sep98,jmp  modified windSh error handler to simply exit if we get
#		   an error during shCleanup.
# 06s,28jul98,jmp  removed useless eventList variable.
# 06r,09jul98,fle  SPR#21727 : added shToolNum global variable
# 06q,09jul98,jmp  moved rBuffShow() and trgShow() to shellShowCmd.tcl.
#		   updated modification history since (WindView 2 Merge).
# 06p,01jul98,c_s  WindView 2 -> Tornado 2 Merge
# 06o,12feb98,pr   Added hitCnt to Trigger info.
# 06n,22jan98,nps  handle a NULL ptr passed to rBuffShow.
# 06m,02dec97,nps  Support new rBuff statistic.
# 06l,20nov97,pr   added trgShow.
# 06k,24sep97,nps  added rBuffShow.
# 06j,18jun98,jmp  canceled previous workaround: the asynchronous event
#                  notification works now on win32.
# 06i,12jun98,jmp  added a temporary workaround for the event notification
#		   problem on win32.
# 06h,10jun98,jmp  removed AGENT_UNSUPPORTED_REQUEST from windShErrorHandler.
# 06g,09jun98,jmp  modified shEventDispatch{} to not add null event string to
#                  the event list.
# 06f,03jun98,jmp  added TS_KILLED_Handler{}.
# 06e,20may98,jmp  modified to used the asynchronous notification.
# 06d,14may98,jmp  moved memBlockDelete called in VIO_WRITE_Handler{}.
# 06c,13may98,jmp  changed the order of sourcing resource files.
# 06b,12may98,jmp  speed up WindSh reboot()
# 06a,01apr98,jmp  reorganized and splited in shell*Cmd.tcl.
#		   modified to give I/O control to WindSh (SPR# 20002).
# 05k,24mar98,dbt  added system mode support for tt() command.
#		   added sysStatusShow() and agentModeShow() commands.
#		   improved system mode support in various routines.
# 05j,12mar98,dbt  updated help() routine.
# 05i,18mar98,jmb  HPSIM merge of Mark Mason patch 04nov96:  added use of
#                  targetTool proc.
# 05h,18mar98,jmb  HPSIM merge of Mark Mason patch 25oct96:  mRegs now skips
#                  registers named '-'.
# 05g,10mar98,dbt  added hardware breakpoint and breakpoint count support.
# 05f,26feb98,dbt  modified sp routine in order to fix a TCL8.0 compiler bug.
# 05e,23jan98,fle  Modified for using wtxMemDisassemble service in the l command
#                  + destroyed disassemble{} procedure
#                  + destroyed shDemangle{} procedure
#                  + modified so{} for calling wtxMemDisassemble{} instead of
#                    disassemble{}
# 05d,29jan98,pad  restored broken history after merge operations.
# 05c,04dec97,jmp  added intVecShow.
# 05b,05nov97,rlp  modified msgQShow for tracking messages sent.
# 05a,16oct97,elp  moved step event processing in step proc (SPR# 9491).
# 01a,21sep94,c_s  written, derived from VxGDB 2.0's "vxgdb.tcl".
#*/

# DESCRIPTION
# This file is the Tcl entry point of WindSh. It contains all the
# initialization routines, events handler routines ([EVENT_NAME]_Handler),
# I/O controls routines plus the common routines used by the shellprocs
# declared in the resource files.
#
# RESOURCE FILES
# wpwr/host/resource/tcl/shellDbgCmd.tcl
# wpwr/host/resource/tcl/shellMemCmd.tcl
# wpwr/host/resource/tcl/shellShowCmd.tcl
# wpwr/host/resource/tcl/shellTaskCmd.tcl
# wpwr/host/resource/tcl/shellUtilCmd.tcl
# wpwr/host/resource/tcl/app-config/WindSh/*.tcl
# wpwr/host/resource/doctools/windHelpLib.tcl
#

# globals
set fileDesc		""	;# standard input/ouput file descriptor
set chanInOut		0	;# VIO channel for standard input and output
set funcCallId		0	;# Identifier of the last funcCall
set callTaskOptions	0x19	;# VX_SUPERVISOR_MODE | VX_FP_TASK | VX_STDIO
set callStack		""	;# stack of called function ids
set shellConfig()	0	;# WindSh Environment table for shConfig{}
set shPollInterval	200	;# event poll interval (msec)
set lastErrorValue	0	;# keep the last error value from func
set interrupted		"FALSE"	;# interrupted flag (See interrupted_Handler{})
set shToolNum		[wtxCommandSend wtxToolNumGet]	;# shell number
set exitNowFlag		0	;# set to 1 by shCleanup, so if the connection
				;# is lost: just exit.
set remainingEvents	""	;# remaining events list

# control key messages
if {[wtxHostType] == "x86-win32"} {
    set interruptMsg "(press CTRL+Break to stop)"
} else {
    set interruptMsg "(press CTRL+C to stop)"
}

#
# WIND SHELL PROCEDURE SUPPORT
#
if {[info var shellProcList] == ""} {
    set shellProcList ""
}


##############################################################################
#
# shellName - build a windsh routine name
#
# This routine prepends "__" to a name.
#

proc shellName {name} {
    return __$name
}

##############################################################################
#
# shellproc - define a windsh procedure
#
# This routine adds a procedure to the list of windsh procedures.
#

proc shellproc {name alist body} {
    global shellProcList

    # define the procedure, mangling the name.
    eval [list proc [shellName $name] $alist $body]

    # add the procedure to shellProcList.
    if {[lsearch $shellProcList $name] == -1} {
	lappend shellProcList $name
    }
}

##############################################################################
#
# describeTask - print task description
#

proc describeTask {taskId} {

    if {$taskId != -1} {
	return [format "%#x (%s)" $taskId [taskIdToName $taskId]]
    } else {
	return "SYSTEM"
    }
}

##############################################################################
#
# shAsmAddrPrint - print address symbolically
#
# SYNOPSIS:
# shAsmAddrPrint addr
#
# PARAMETERS:
# addr: address to print
#
# RETURNS:
# symbolical address or hexadecimal address
#

proc shAsmAddrPrint {addr} {
    return [shSymbolicAddr -inexact $addr]
}

##############################################################################
#
# shCleanup - cleanup routine invoked at shell exit time
#
# This routine is called when windsh is about to exit.  Currently, it removes
# all breakpoints that were set by this shell instance.
#
# SYNOPSIS:
#   shCleanup
#

proc shCleanup {} {

    global epTable
    global exitNowFlag

    # check if target is alive or not
    if ![targetIsAlive] {
    	return
    }

    # Remove all breakpoints we set.
    if [info exists epTable] {
	foreach bpnum [array names epTable] {
	    wtxEventpointDelete $bpnum
	    unset epTable($bpnum)
	}
    }

    # Cancel I/O redirection setting
    set exitNowFlag 1
    funcCallsIoUnset
    set exitNowFlag 0
}

##############################################################################
#
# trcPrintArgs - read function arguments on the target
#
# This routine is the generic function that return the list of argument values
#
# SYNOPSIS:
#	trcPrintArgs pArg nArg
#
# PARAMETERS:
#	pArg: address where argument table starts
#	nArg: number of arguments (4 bytes)
#
# RETURNS:
#	the list of argument values or an empty list if nArg is null
#
proc trcPrintArgs {pArg nArg} {

    global defArgNb

    if {$nArg == -1} {
	set nArg $defArgNb
    }
    if {$nArg != 0} {
	set blk [wtxMemRead $pArg [expr $nArg * 4]]
	set argValues [memBlockGet -l $blk]
	memBlockDelete $blk
	return $argValues
    }
    return {}
}

##############################################################################
#
# shFuncCall - call a function synchronously (poll for the return value)
#
# The function is called via wtxFuncCall. Then, we call CALL_RETURN_EventPoll
# to wait for the CALL_RETURN event that contains the return value of the
# function.
#
# SYNOPSIS:
#   shFuncCall [-int | -float | -double] [-nobreak] func [a0 ... a9]
#
# PARAMETERS:
#   -int, -float, -double: type of result
#   -nobreak: set the VX_UNBREAKABLE bit during the call
#   func: address of function to call
#   a0...a9: integer function arguments
#
# RETURNS: N/A
#   The return value of the called function is returned
#   by CALL_RETURN_EventPoll{}.
#
# ERRORS:
#   Too many arguments to function (limit: 10)
#

proc shFuncCall {args} {
    global callTaskOptions	;# options for wtxFuncCall
    global fileDesc		;# standard input and output file descriptors
    global funcCallId		;# Identifier of the last funcCall
    global callStack		;# stack of called function ids
    global interrupted		;# interrupted flag (See interrupted_Handler{})
    global shellConfig		;# WindSh Environment table for shConfig{}


    # Parse shFuncCall arguments
    set func 0
    set alist ""
    set type "-int"
    set nobreak 0

    foreach arg $args {
	switch -glob -- $arg {
	    -i*		{set type "-int"}
	    -f*		{set type "-float"}
	    -d*		{set type "-double"}
	    -n*		{set nobreak 1}
	    default	{
		if {$func == 0} {
		    set func $arg
		} else {
		    lappend alist $arg
		}
	    }
	}
    }

    if {[llength $alist] > 10} {
	error "Too many arguments to function (limit: 10)"
    }

    set opt $callTaskOptions
    if {$type == "-float" || $type == "-double"} {
	# call the function with VX_FP_TASK set.
	set opt [expr $callTaskOptions | 0x8]
    }

    if {$nobreak} {
	# or in VX_UNBREAKABLE
	set opt [expr $opt | 0x2]
    }

    #
    # Set I/O according to SH_GET_TASK_IO
    #
    if {$shellConfig(SH_GET_TASK_IO) == "on"  && $fileDesc == ""} {
    	funcCallsIoSet
    }
    if {$shellConfig(SH_GET_TASK_IO) == "off" && $fileDesc != ""} {
    	funcCallsIoUnset
    }

    #
    # set rIn and rOut option for wtxFuncCall according to current setting
    #
    set redirOption ""
    if {$fileDesc != ""} {
    	set redirOption "-rIn $fileDesc -rOut $fileDesc"
    }

    # Clear interrupted flag
    set interrupted "FALSE"

    # Call set function
    set fcExpr "wtxFuncCall $type $redirOption -opt $opt $func $alist"
    set funcCallId [eval $fcExpr]

    # add it to the call stack
    lappend callStack "$funcCallId"

    # start CALL_RETURN polling
    CALL_RETURN_EventPoll
}

##############################################################################
#
# CALL_RETURN_EventPoll - Poll for CALL_RETURN event
#
# SYNOPSIS:
#   CALL_RETURN_EventPoll
#
# PARAMETERS: N/A
#
# RETURNS:
#   - The return value of the function called on the target.
#
# The function is called via wtxFuncCall.  Then, we poll the target server
# waiting for the event that contains the return value of the function to
# arrive.  When it does, the return value supplied by WTX is returned by
# this function.  Events not pertaining to the function call being waited
# for are dispatched to their handlers.
#
# CAVEATS:
#   This function discards events it receives that do not pertain to the
#   function call while waiting for the return-value event.

proc CALL_RETURN_EventPoll {} {

    global funcCallId		;# Identifier of the last funcCall
    global callStack		;# stack of called function ids
    global chanInOut		;# VIO channel for standard input and output
    global shellConfig		;# WindSh Environment table for shConfig{}
    global shPollInterval	;# event poll interval (msec)
    global lastErrorValue	;# last error value
    global offset		;# offset for WIND_TCB query
    global interrupted		;# interrupted flag (See interrupted_Handler{})
    global remainingEvents	;# remaining events list

    #
    # If taskIdDefault is not in the callStack, this means that
    # this task has been spawned using sp or sps, and we do not have
    # to wait for the CALL_RETURN event.
    #
    set defTaskId $funcCallId
    if {[lsearch $callStack $defTaskId] == -1} {
    	return 0
    }

    while 1 {

	# Check the event list
	set eventList [eventCheck]
	foreach event $eventList {

	    # remove $event from the remaining events list
	    set evtIdx [lsearch $remainingEvents $event]
	    if {$evtIdx != -1} {
		set remainingEvents [lreplace $remainingEvents $evtIdx $evtIdx]
	    }

	    # ^C Handling (See interrupted_Handler{})
	    if {$interrupted == "TRUE"} {
		return 0
	    }

	    case [lindex $event 0] in {

		"EXCEPTION" {
		    if {[lindex $event 2] == $funcCallId} {
			return 0
		    }
		}

		"TEXT_ACCESS" {
		    if {[lindex $event 1] == $funcCallId} {

			puts stdout \
		       "Called function encountered a breakpoint (returning 0)."
			return 0
		    }
		}

		"DATA_ACCESS" {
		    if {[lindex $event 1] == $funcCallId} {
			puts stdout \
		       "Called function encountered a breakpoint (returning 0)."
			return 0
		    }
		}

		"CALL_RETURN" {

		    # check if the CALL_RETURN is the expected one, if no
		    # ignore it.
		    if {[lindex $event 1] == $funcCallId} {

			# if wtxFuncCall return an error,
			# save it for printErrno{}
			if {[lindex $event 3]} {
			    set lastErrorValue [expr [lindex $event 3]]
			}

			# remove the funcCall Id from the callStack
			set index [lsearch $callStack $funcCallId]
			set callStack [lreplace $callStack $index $index]

			# reset funcCallId
			set funcCallId 0

			# return the wtxFuncCall return value
			return [lindex $event 2]
		    }
		}
	    }
	}

	# Does WindSh set up to give I/O control to called functions ?
	# If yes we must send characteres typed on WindSh to the channel
	# associated with the called task Standard Input

	# polling windSh standard input is very slow on UNIX (about 200ms),
	# so on UNIX we only sleep $shPollInterval ms if we do not call
	# shInputToVio().

	if {$shellConfig(SH_GET_TASK_IO) == "on"} {
	    shInputToVio $chanInOut

	    # sleep only on Win32 hosts
	    if {[wtxHostType] == "x86-win32"} {
		msleep $shPollInterval
	    }
	} else {
	    msleep $shPollInterval
	}
    }
}

##############################################################################
#
# shEventPoll - process the event queue.
#
# This routine drains the WTX event queue in the attached target server.
# Each event is dispatched to a handler routine based on the event type.
#
# SYNOPSIS:
#   shEventPoll
#
# PARAMETERS:
#   NONE
#
# RETURNS:
#   NONE
#
# ERRORS:
#   NONE

proc shEventPoll {} {

    while {[set event [shAsyncEventGet]] != ""} {
	shEventDispatch $event
    }
}

##############################################################################
#
# quit - exit windsh
#

proc quit {args} {
    exit 0
}

##############################################################################
#
# symFindByCName - find symbol by name
#

proc symFindByCName {name} {
    global shellProcList

    if [regexp "@(.*)" $name dummy subname] {

	# The name starts with @.  Do not try to look the function up on
	# the host.  Remove the leading @ from the name.

	set name $subname
    } else {
	# See if the function has a Tcl binding.

	if {$name == "exit"} {return "HOST exit LHS TEXT"}
	if {[lsearch $shellProcList $name] != -1} {
	    return [list HOST [shellName $name] LHS TEXT]
	}
    }

    # See if it's a symbol on the target.

    if {[catch {set sym [wtxSymFind -name $name]}] == 0} {
	return [list TARGET [lindex $sym 1] LHS [lindex $sym 2]]
    }

    # Perhaps it's the name of a task.

    if {[set taskId [taskNameToId $name]] != 0} {
	return [list TARGET $taskId RHS DATA]
    }

    # Utter failure!

    error "symbol not found"
}

##############################################################################
#
# symFindByOtherName - perform symbol lookup for non-C languages
#
# This routine currently implements symbol searching for the C++ language.
# Other languages could be added.
#
# SYNOPSIS:
#   symFindByOtherName name
#
# PARAMETERS:
#   name: root name of symbol
#
# RETURNS:
#   A vector of four elements:
#     TARGET|HOST             TARGET if symbol found; HOST: Tcl implementation
#     symbol value            symbol value in hex, or string if Tcl
#     LHS|RHS                 whether sym can be assigned to (LHS) or not (RHS)
#     DATA|TEXT|type-code     type of symbol
#
# ERRORS:
#

proc symFindByOtherName {name} {
    global shDemangleStyle

    # If we haven't been asked to demangle things this routine
    # has no work to do.

    if {$shDemangleStyle == "" || $shDemangleStyle == "none"} {
	error "symbol not found"
    }

    # See if it's an overloaded function or a method name.

    set symList [mangledSymListGet $name]

    if {$symList != ""} {
	set candidates ""
	set ix 0
	foreach sym $symList {

	    # Remove troublesome leading underscore, if any.

	    set symname [underscoreStrip [lindex $sym 0]]
	    lappend candidates [list $symname \
		    [lindex $sym 1] [lindex $sym 2]]
	    incr ix
	}

	set choice 0
	if {$ix > 1} {

	    set iy 0
	    foreach ctd $candidates {
		puts stdout [format "%4d: %s" $iy [shDemangle [lindex $ctd 0]]]
		incr iy
	    }

	    set ok 0
	    while {! $ok} {
		puts stdout "Choose the number of the symbol to use: " \
			nonewline
		set response [stdinGet]
		if {! [regexp {\-?[0-9]+} $response]} {
		    puts stdout "Please enter a number."
		} else {
		    if {$response < 0 || $response >= $ix} {
			error "symbol not found"
		    } else {
			set choice $response
			set ok 1
		    }
		}
	    }
	}

	set symChoice [lindex $candidates $choice]
	return [list TARGET [lindex $symChoice 1] LHS [lindex $symChoice 2]]
    }

    error "symbol not found"
}

##############################################################################
#
# targetFunctionGet - get target function address given its name
#

proc targetFunctionGet {name} {
    set func [shSymAddr $name]
    if {[expr $func] == 0} {
	error \
"This request cannot be completed,
because the target lacks the function $name."
    }
    return $func
}

##############################################################################
#
# targetSymbolGet - get target symbol value given its name
#

proc targetSymbolGet {name} {
    set sym [shSymAddr $name]
    if {[expr $sym] == 0} {
	error \
"This request cannot be completed,
because the target lacks the symbol $name."
    }
    return $sym
}

##############################################################################
#
# windShErrorHandler - handler procedure for WindSh
#
# An error handler procedure (attached with wtxErrorHandler) that tries
# to restart the shell should the target be rebooted.

proc windShErrorHandler {hwtx cmd err tag} {

    global exitNowFlag  ;# exit without calling shCleanup
    global interrupted	;# interrupted flag (See interrupted_Handler{}
    global interruptMsg	;# control key message

    set waitProgress { "|" "/" "-" "\\" }
    set loop 0

    # reset interrupted flag
    set interrupted FALSE

    #puts stdout "hwtx = $hwtx\n cmd = $cmd\n err = $err\n tag = $tag"
    #flush stdout

    set errName [wtxErrorName $err]
    if {$errName == "AGENT_COMMUNICATION_ERROR" \
	    || $errName == "EXCHANGE_NO_TRANSPORT" \
	    || $errName == "SVR_TARGET_NOT_ATTACHED" \
	    || $errName == "EXCHANGE_TRANSPORT_DISCONNECT" } {
	# Target server is about to go down, or is down.  There's no point in
	# trying to clean up; just restart the shell.

	if {$exitNowFlag} {
	    # We must exit here without calling shCleanup
	    exitNow
	}


	puts stdout "Target connection has been lost."
	puts -nonewline stdout \
	    "Waiting for target server to attach to target $interruptMsg  "

	flush stdout

	# stop windShErrorHandler
	wtxErrorHandler [wtxHandle] ""

	while {$errName == "AGENT_COMMUNICATION_ERROR"} {

	    # ^C Handling (See interrupted_Handler{})
	    if {$interrupted == "TRUE"} {
		exit
	    }

	    # display progress message
	    puts stdout [format "\b%s" \
	    	[lindex $waitProgress [expr $loop%4]]] nonewline

	    # try connection
	    catch {[wtxAgentModeGet]} err
	    set errName [wtxErrorName $err]
	    incr loop
	}

	# ^C Handling (See interrupted_Handler{})
	if {$interrupted == "TRUE"} {
	    exit
	}

	# restart WindSh process
	puts stdout "\nRestarting shell..."
	shRestart

	# can't restart? resubmit the error.
	wtxErrorHandler [wtxHandle] windShErrorHandler
	error $err
    }

    error $err
}

##############################################################################
#
# reinitialize - re-initialize shell
#

proc reinitialize {} {
    source [wtxPath host resource tcl]shell.tcl
}

##############################################################################
#
# shellInit - initialize shell
#

proc shellInit {{interactive 0}} {
    global steppedTask
    global __logoPrinted
    global tcl_interactive
    global shellConfig		;# WindSh Environment table for shConfig{}
    global cpuFamily
    global __wtxCpuType

    set steppedTask 0

    if {$interactive} {
	wtxErrorHandler [wtxHandle] windShErrorHandler
    }

    # Read in the core shell support routines and initialize them to reflect
    # the current connection.
    uplevel #0 source [wtxPath host resource tcl]shelcore.tcl

    shellCoreInit
    wtxRegisterForEvent .*

    # Read in the shell resource files
    uplevel #0 source [wtxPath host resource tcl]shellDbgCmd.tcl
    uplevel #0 source [wtxPath host resource tcl]shellMemCmd.tcl
    uplevel #0 source [wtxPath host resource tcl]shellShowCmd.tcl
    uplevel #0 source [wtxPath host resource tcl]shellTaskCmd.tcl
    uplevel #0 source [wtxPath host resource tcl]shellUtilCmd.tcl

    # Read in the app customizations.
    set cfgdir [wtxPath host resource tcl app-config WindSh]*.tcl
    foreach file [lsort [glob -nocomplain $cfgdir]] {uplevel #0 source $file}

    # initialize Windsh environment table with default values
    shellConfigInit

    if { $cpuFamily($__wtxCpuType) == "sh" } {
	shellShSetup
    }

    if {[info exists tcl_interactive] && \
	    $tcl_interactive && \
	    ! [info exists __logoPrinted]} {
	[shellName printLogo]
	set __logoPrinted 1

	# Print C++ policy

        [shellName cplusStratShow]
    }
}

##############################################################################
#
# directCall - Call a function on the target using wtxDirectCall
#
# This routine call a function on the target and wait the function
# completion.
#
# This routine should only be called by windsh private routines.
#
# SYNOPSIS:
#    directCall <function> [arg0 ... arg9]
#
# PARAMETERS:
#    function: name of the function to call
#    arg[0-9]: arguments (10 max)
#
# RETURNS:
#    return value of the called function
#
# ERRORS:
#    wtxDirectCall errors
#
proc directCall {func args} {

    set funcAddr [lindex [wtxSymFind -name $func] 1]

    if {[catch "wtxDirectCall $funcAddr $args" funcRes]} {
    	error "\nERROR : Can't call $func\n"
    }

    return $funcRes
}

##############################################################################
#
# funcCallsIoSet - Activate Input/Output redirection
#
# This routine redirects Input/Output of called functions to WindSh,
# if the WindSh environment variable SH_GET_TASK_IO is set to on.
# See shConfig(SH_GET_TASK_IO) for more information.
#
# This routine should only be called by windsh private routines.
#
# SYNOPSIS:
#    funcCallsIoSet
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS:
#    Cannot open VIO Channel /vio/...
#
proc funcCallsIoSet {} {

    global chanInOut		;# VIO channel for standard input & output
    global fileDesc 		;# standard input & output file descriptor
    global shellConfig		;# WindSh Environment table for shConfig{}

    if {[wtxAgentModeGet] == "AGENT_MODE_EXTERN"} {
    	puts stdout "Warning: Target agent is in System mode,\
		cannot redirect Target I/O\n"
	return
    }

    set maxTries 10

    if {$shellConfig(SH_GET_TASK_IO) == "on"} {

	set numTries 0
	set fileDesc -1
	while {$fileDesc == -1} {

	    # Claim a new VIO channel for standard input and output
	    set chanInOut [wtxVioChanGet]

	    if [info exists vioInAddr] {
	    	wtxMemFree $vioInAddr
	    }
	    set vioInAddr [wtxMemAlloc [string length "/vio/$chanInOut"]]
	    set blockIn [memBlockCreate -string "/vio/$chanInOut"]
	    wtxMemWrite $blockIn $vioInAddr
	    set fileDesc [directCall open $vioInAddr 2 0]

	    if {$fileDesc == -1 && $numTries == $maxTries} {
	    	puts stdout "ERROR: Cannot open VIO Channel for standard I/O"
		break
	    }
	    incr numTries
	}

	wtxMemFree $vioInAddr
    }
}

##############################################################################
#
# funcCallsIoUnset - Cancel Input/Output redirection
#
# This routine cancel Input/Output redirection of called functions to
# WindSh.
# See shConfig(SH_GET_TASK_IO) for more information.
#
# This routine should only be called by windsh private routines.
#
# SYNOPSIS:
#    funcCallsIoUnset
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#
proc funcCallsIoUnset {} {

    global chanInOut		;# VIO channel for standard input & output
    global fileDesc 		;# standard input & output file descriptor
    set agentMode ""

    # check if target is alive or not
    if ![targetIsAlive] {
    	return
    }

    # if IO have been redirected ($fileDesc!="") without errors ($fileDesc!=-1)
    if {$fileDesc != "" && $fileDesc != -1} {
	# Close Input channels
	wtxErrorHandler [wtxHandle] ""
	catch {directCall close $fileDesc}
	catch {wtxVioChanRelease $chanInOut}
	wtxErrorHandler [wtxHandle] windShErrorHandler
	set fileDesc ""
    }
}

##############################################################################
#
# targetIsAlive - Is target alive ?
#
# This routine ping target to determine if it is alive or not
#
# SYNOPSIS:
#	targetIsAlive
#
# PARAMETERS: N/A
#
# RETURNS: 1 if alive, 0 otherwise
#
# ERRORS: N/A
#
proc targetIsAlive {} {

    # save current WTX Handler
    set prevHandler [wtxErrorHandler [wtxHandle]]

    # if a handler is installed, uninstall it
    if {$prevHandler != ""} {
    	wtxErrorHandler [wtxHandle] ""
    }

    # check if target is alive or not
    if [catch {set agentMode [wtxAgentModeGet]}] {
    	return 0
    }

    # reinstall WTX Handler
    if {$prevHandler != ""} {
	wtxErrorHandler [wtxHandle] $prevHandler
    }

    return 1
}

##############################################################################
#
#                               EVENT HANDLING
#
##############################################################################

##############################################################################
#
# shEventDispatch - dispatch an event, based on its name.
#
#   The Tcl interpreter is queried for a procedure named <event>_Handler.
#   If such a routine exists, it is called with the event as a parameter.
#
# SYNOPSIS:
#   shEventDispatch event
#
# PARAMETERS:
#   event: an event, returned by shAsyncEventGet (See eventCheck{}).
#
# RETURNS: N/A
#
# ERRORS: N/A
#

set showAllEvents 0		;# display all events for debug purpose
set showUnhandledEvents 0	;# display unhandled events for debug purpose

proc shEventDispatch {event} {

    global showAllEvents	;# display all events for debug purpose
    global showUnhandledEvents	;# display unhandled events for debug purpose


    set handlerProc "[lindex $event 0]_Handler"

    if {[info procs $handlerProc] != ""} {

	if {$showAllEvents} {
	    puts stdout $event
	}

	# call the associated hanlder
	$handlerProc $event

    } else {

	if {$showUnhandledEvents || $showAllEvents} {
	    if {$event != ""} {
		puts stdout "unhandled event: $event"
	    }
	}
    }
}

##############################################################################
#
# TEXT_ACCESS_Handler - handle breakpoint event
#
# We assume that a free-running task has hit a breakpoint, and print the
# breakpoint info in "short format".
#
# SYNOPSIS:
#   TEXT_ACCESS_Handler event
#
# PARAMETERS:
#   event:	a TEXT_ACCESS event received from the target server
#

proc TEXT_ACCESS_Handler {event} {

    global lSavedBase

    set bpAddr [lindex $event 3]
    set bpSymAddr [shSymbolicAddr -inexact $bpAddr]
    set bpTask [lindex $event 1]

    puts stdout [format "\nBreak at 0x%08x: %-23s Task: %s" \
		$bpAddr $bpSymAddr [describeTask $bpTask]]

    if {$bpTask != -1} {
	taskIdDefault $bpTask
    }

    # make disassembler start here by default.
    set lSavedBase $bpAddr
}

##############################################################################
#
# DATA_ACCESS_Handler - handle watchpoint event
#
# We assume that a free-running task has hit a watchpoint, and print the
# breakpoint info in "short format".
#
# SYNOPSIS:
#   DATA_ACCESS_Handler event
#
# PARAMETERS:
#   event:	a DATA_ACCESS event received from the target server
#

proc DATA_ACCESS_Handler {event} {

    global lSavedBase

    set bpAddr [lindex $event 6]
    set pc [lindex $event 3]
    set bpSymAddr [shSymbolicAddr -inexact $bpAddr]
    set bpTask [lindex $event 1]

    puts stdout [format "\nBreak at 0x%08x: %-23s Task: %s" \
		$bpAddr $bpSymAddr [describeTask $bpTask]]

    if {$bpTask != -1} {
	taskIdDefault $bpTask
    }

    # make disassembler start here by default.
    set lSavedBase $pc
}

##############################################################################
#
# EXCEPTION_Handler - handle EXCEPTION event
#
# Print a message describing the exception.
#
# SYNOPSIS:
#   EXCEPTION_Handler event
#
# PARAMETERS:
#   event:	EXCEPTION event received from target server
#

proc EXCEPTION_Handler {event} {

    if {! [targetInitCheck]} {
	set excTask -1
    } else {
	set excTask [lindex $event 2]
    }

    set excVector [lindex $event 3]
    set excEsfAddr [lindex $event 4]
    puts stdout [format "\n\007Exception number %d: Task: %s" \
	    $excVector [describeTask $excTask]]

    # Try to show the ESF, if support for ESF printing is configured in.

    catch {excInfoShow $excVector $excEsfAddr}

    # Trace the stack, if stack tracing is configured in.

    if {$excTask != -1} {
	catch {[shellName tt] $excTask}
	taskIdDefault $excTask
    }
}

##############################################################################
#
# TGT_RESET_Handler - handle TGT_RESET event
#
# Attempt to reattach to the target using wtxTargetAttach
#
# SYNOPSIS:
#   EXCEPTION_Handler event
#
# PARAMETERS:
#   event:	TGT_RESET event received from target server
#

proc TGT_RESET_Handler {event} {
    wtxTargetAttach
}

##############################################################################
#
# TS_KILLED_Handler - handle TS_KILLED event
#
# Attempt to reattach to the target using wtxTargetAttach.
# In this routine we should call funcCallsIoUnset{} to cancel Input/Output
# redirection, but since the Target Server has been killed, we can't
# close vio on Target.
#
# SYNOPSIS:
#   TS_KILLED_Handler event
#
# PARAMETERS:
#   event:	TS_KILLED event received from target server
#

proc TS_KILLED_Handler {event} {
    wtxTargetAttach
}

##############################################################################
#
# VIO_WRITE_Handler - handle a VIO_WRITE event
#
# SYNOPSIS:
#   VIO_WRITE_Handler event
#
# PARAMETERS:
#   event:	VIO_WRITE event received from target server
#

proc VIO_WRITE_Handler {event} {

    global chanInOut

    set vioChannel [lindex $event 1]
    set mblk [lindex $event 2]

    # verify that the memory block is valid
    if {[lsearch [memBlockList] $mblk] == -1} {
    	# mblk not valid, return
    	return
    }

    if {$vioChannel == $chanInOut || $vioChannel == 0} {

	# The data of the VIO is in the memory block returned with the
	# event.  Dump that block to stdout and free it.
	memBlockWriteFile $mblk -
	memBlockDelete $mblk
    }
}

##############################################################################
#
# interrupted_Handler - ^C handling
#
# The user ^C'd a function call. Kill the task (if it's stillaround).
#
# SYNOPSIS:
#	interrupted_Handler
#
# PARAMETERS: N/A
#
# ERRORS:
#    Function is safe from deletion: not interrupted
#

proc interrupted_Handler {} {

    global offset
    global funcCallId
    global interrupted

    # Turn On the interrupted flag
    set interrupted "TRUE"

    # If funcCallId is null, the interrupted function is shelproc
    # so no task to kill.
    if {$funcCallId == 0} {
	return
    }

    # Look at the safeCnt value in the TCB
    set result [wtxGopherEval "$funcCallId
				<+$offset(WIND_TCB,safeCnt) @>"]

    if {$result > 0} {
	puts "Function is safe from deletion: not interrupted"
    } else {
	catch {wtxContextKill $funcCallId}

	# task is killed, reset funcCallId
	set funcCallId 0
	return
    }
}

##############################################################################
#
# eventCheck - get events for asynchronous event list
#
# SYNOPSIS:
#    eventCheck [eventToSkip]
#
# ARGUMENTS:
#    eventToSkip: name of event to not dispatch, the event must be handled
#		  by the caller.
#
# DESCRIPTION:
#    This routine empties the Async event list, dispatch the events and
#    return the event list.
#    If this routine is called with eventToSkip != none, only one event
#    will be returned. And if this event is $eventToSkip, we only return
#    it without calling shEventDispatch()
#
# RETURNS:
#    An event list or nothing if there is no event to get.
#

proc eventCheck {{eventToSkip none}} {

    global remainingEvents	;# remaining events list

    # initialized the event list to return
    set eventList ""

    # get a first event
    set event [shAsyncEventGet]

    # are we in the eventToSkip Mode ?
    if {$eventToSkip == "none"} {

	# while the Async event list is not empty
	while {$event != ""} {

	    # add the event to the remaining event list if there is
	    # no handler for this event
	    set handlerProc "[lindex $event 0]_Handler"
	    if {[info procs $handlerProc] == ""} {
		lappend remainingEvents $event
	    }

	    # dispatch the event
	    shEventDispatch $event

	    # add event to the event list to return
	    lappend eventList $event

	    # get a new event
	    set event [shAsyncEventGet]
	}

    } else {

	if {$event != ""} {

	    # only dispatch the event if it is
	    # different than the event to skip
	    if {[lindex $event 0] != $eventToSkip} {
		shEventDispatch $event
	    }
	}
	set eventList $event
    }

    # return the event list
    return $eventList
}

##############################################################################
#
# asyncEvtHandle - get and dispatch events
#
# SYNOPSIS:
#    asyncEvtHandle
#
# ARGUMENTS:
#    N/A
#
# DESCRIPTION:
#    This routine empties the Async event list, and dispatch the events
#    using shEventDispatch.
#    This routine is called regularly (every 200ms) by the shell.
#    (See windsh/main.cpp: eventPoll())
#
# RETURNS:
#    N/A
#

proc asyncEvtHandle {} {

    global remainingEvents	;# remaining events list

    # empty the remaining events list
    set remainingEvents ""

    # then get a first event for the async event list
    set event [shAsyncEventGet]

    # while the Async event list is not empty
    while {$event != ""} {

    	# dispatch the event
    	shEventDispatch $event

	# get a new event
	set event [shAsyncEventGet]
    }
}
