# shellDbgCmd.tcl - Tcl implementation of WindSh "debug" commands
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01p,23jan02,tlc  Revise proc so to step over a function AND it's
#                  branch-delay slot for MIPS. (SPR # 67693 fix).
# 01o,12oct00,sdk  Added VLIW support for frv on single stepping.
# 01n,18sep00,yvp  Added VLIW instruction boundary check on breakpoint
#                  addresses for FRV.
# 01m,28sep99,myz  modified proc so for CW4000_16
# 01l,10sep99,myz  modified shellproc b to support CW4000_16 cpu.
# 01k,18mar99,jmp  fixed shStepEventWait() to no longer dispatch event twice
#                  (SPR# 25818).
# 01j,25feb99,jmp  no longer need to add event in the remainingEvents list
#                  SPR# 25292).
# 01i,29jan99,jmp  fixed SPR# 24612: windsh can lose event unhandled events.
# 01h,05jan99,jmp  modified redirection system to use the same channel/fd
#		   (SPR# 21821).
# 01g,03nov98,jmp  changed eventCheck call.
# 01f,22sep98,dbt  changed shStepEventWait() routine to handle new
#                  wtxMemDisassemble output format.
# 01e,28jul98,jmp  removed useless eventList variable.
# 01d,17jul98,dbt  in b() routine, add count parameter to wtxEventpointAdd
#		   call routine only if it is different from 0 for backward
#		   compatibility.
# 01c,26may98,pcn  Changed wtxEventpointList_2 in wtxEventpointListGet.
# 01b,20may98,jmp  used eventCheck{} instead of wtxEventPoll{}.
# 01a,01apr98,jmp  created based on routines moved out from shell.tcl
#

# DESCRIPTION
# This module holds the "debug" shellproc of WindSh:
#
# b - set shell breakpoint
# bh - set shell hardware breakpoint
# bd - delete a breakpoint
# bdall - delete all breakpoints
# c - continue a task
# cret - continue until return
# e - set shell eventpoint
# s - step a task
# so - step over function call
# sysResume - enter system mode, if possible
# sysSuspend - enter system mode, if possible
#
# This module also contains windsh private routines used by the
# "debug" shellprocs
#

# globals

# set hardware breakpoint type list if it doesn't exist.
if {[info var hwBpTypeList] == ""} {
    set hwBpTypeList ""
}


##############################################################################
#
# b - set or display breakpoints
#
# SYNOPSIS:
#   b <addr>,<task>,<count>
#
# PARAMETERS:
#   addr:	where to set breakpoint, or 0 = display all breakpoints
#   task:	task for which to set breakpoint, 0 = set all tasks
#   count:	number of passes before hit
#
# RETURNS: N/A
#
# ERRORS: N/A
#

shellproc b {args} {
    global cpuType
    global __wtxCpuType

    bindNamesToList {addr task count} $args

    if {$addr == 0} {
	# Just plain "b": list existing breakpoints.
	brkptDisplay
	return 0
    }

    # set address's LSB if it is the mips 16 bit instruction. Target debug 
    # facility depends on this bit to set either the 16 bit breakpoint
    # or the 32 bit's. 

    if {($cpuType($__wtxCpuType) == "CW4000_16")} {
        set type [lindex [wtxSymFind -value $addr] 2]
        if {$type & 0x80} {
	    set addr [expr $addr | 0x1]
        }
    }

    # For FRV family CPU's breakpoint addresses must be validated first. 
    # A breakpoint must only be set on a VLIW instruction boundary.
    # The currVliwInst procedure is used to ensure this.

    if {($cpuType($__wtxCpuType) == "FR500")} {

	#determine the start address of the VLIW instruction.
	set vliwInstrStart [currVliwInst $addr]

	# if vliwInstrStart != addr, set breakpoint at vliwInstrStart.
	if {$vliwInstrStart != $addr} {
	    puts stdout [format "Address 0x%x is not the start of a VLIW instruction." $addr]
	    puts stdout [format "Breakpoint now set to 0x%x" $vliwInstrStart]
	    set addr $vliwInstrStart
	}
    }

    # an address was given.  If a task Id is given, constrain to that
    # task, unless we're in system mode, in which we'll set a system 
    # breakpoint.

    if {[wtxAgentModeGet] == "AGENT_MODE_EXTERN"} {
	set cType CONTEXT_SYSTEM
	set cArg 0
	if {$task != 0} {
	    puts stdout "warning: task parameter ignored in external mode"
	}
    } else {
	if {$task == 0} {
	    # XXX should this be CONTEXT_ANY_TASK?
	    set cType CONTEXT_TASK
	    set cArg 0
	} else {
	    set cType CONTEXT_TASK
	    set cArg $task
	}
    }

    # XXX we save the breakpoint information in an array, which we
    # probably shouldn't do, but currently wtxEventpointList doesn't
    # return the eventpoint numbers along with the events so we can't
    # query it that way.

    set action [expr [wtxEnumFromString ACTION_TYPE ACTION_STOP] | \
		     [wtxEnumFromString ACTION_TYPE ACTION_NOTIFY]]

    if {$count != 0} {
	set epNum [wtxEventpointAdd \
		WTX_EVENT_TEXT_ACCESS $addr $count $cType $cArg $action 0 0 0]
    } else {
	set epNum [wtxEventpointAdd \
		WTX_EVENT_TEXT_ACCESS $addr $cType $cArg $action 0 0 0]
    }

    # XXX every reference to epTable should probably be reconsidered
    # if the eventpoint interface changes

    global epTable

    set epTable($epNum) "$addr $cArg"
    return 0
}

##############################################################################
#
# bh - set a hardware breakpoint
#
# SYNOPSIS:
#   bh <addr>,<access>,<task>,<count>
#
# PARAMETERS:
#   addr:	where to set breakpoint, or 0 = display all breakpoints
#   access:	access type (arch dependant)
#   task:	task for which to set breakpoint, 0 = set all tasks
#   count:	number of passes before hit
#
# RETURNS: N/A
#
# ERRORS: N/A
#

shellproc bh {args} {
    global hwBpTypeList

    bindNamesToList {addr access task count} $args

    if {$addr == 0} {
	# Just plain "b": list existing breakpoints.
	brkptDisplay
	return 0
    }

    if {$hwBpTypeList == ""} {
	puts stdout "hardware breakpoints are not supported on \
			this architecture."
	return -1
    }

    # an address was given.  If a task Id is given, constrain to that
    # task, unless we're in system mode, in which we'll set a system 
    # breakpoint.

    if {[wtxAgentModeGet] == "AGENT_MODE_EXTERN"} {
	set cType CONTEXT_SYSTEM
	set cArg 0
	if {$task != 0} {
	    puts stdout "warning: task parameter ignored in external mode"
	}
    } else {
	if {$task == 0} {
	    # XXX should this be CONTEXT_ANY_TASK?
	    set cType CONTEXT_TASK
	    set cArg 0
	} else {
	    set cType CONTEXT_TASK
	    set cArg $task
	}
    }

    # XXX we save the breakpoint information in an array, which we
    # probably shouldn't do, but currently wtxEventpointList doesn't
    # return the eventpoint numbers along with the events so we can't
    # query it that way.

    set action [expr [wtxEnumFromString ACTION_TYPE ACTION_STOP] | \
		     [wtxEnumFromString ACTION_TYPE ACTION_NOTIFY]]

    set epNum [wtxEventpointAdd \
	    WTX_EVENT_HW_BP $addr $count $access\
	    $cType $cArg $action 0 0 0]

    # XXX every reference to epTable should probably be reconsidered
    # if the eventpoint interface changes

    global epTable

    set epTable($epNum) "$addr $cArg"
    return 0
}

##############################################################################
#
# bd - delete a breakpoint
#
# SYNOPSIS:
#   bd <addr>,<task>
#
# PARAMETERS:
#   addr:	address of breakpoint to delete
#   task:	task for which to delete breakpoint, 0 = delete for all tasks
#
# RETURNS: N/A
#
# ERRORS: N/A
#

shellproc bd {args} {
    global epTable
    bindNamesToList {addr task} $args

    if [info exists epTable] {
	foreach name [array names epTable] {
	    if {$addr == [lindex $epTable($name) 0] && \
		    ([lindex $epTable($name) 1] == $task || $task == 0)} {
		wtxEventpointDelete $name
		unset epTable($name)
	    }
	}
    }
    return 0
}

##############################################################################
#
# bdall - delete all breakpoints
#
# SYNOPSIS:
#   bdall
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

shellproc bdall {args} {
    global epTable
    if [info exists epTable] {
	foreach name [array names epTable] {
	    wtxEventpointDelete $name
	}
	unset epTable
    }
    return 0
}

##############################################################################
#
# c - continue a task
#
# SYNOPSIS:
#   c [taskId]
#
# PARAMETERS:
#   taskId - task to continue, default if 0
#
# RETURNS:
#   NONE
#
# ERRORS:
#   NONE

shellproc c {args} {

    bindNamesToList {taskId addr addr1} $args

    if {[wtxAgentModeGet] == "AGENT_MODE_EXTERN"} {
	wtxContextCont CONTEXT_SYSTEM 0
	return 0
    }

    set taskId [taskIdDefault $taskId]

    if {$addr != 0 || $addr1 != 0} {
	# XXX should set pc, npc in TCB (arch specific)
	puts stdout \
	  "Warning: addr and addr1 parameters to `c' are not implemented yet."
    }

    # Clear event list before continuing task
    wtxContextCont CONTEXT_TASK $taskId

    CALL_RETURN_EventPoll
}

##############################################################################
#
# cret - continue until return
#
# SYNOPSIS:
#   cret taskOrId
#
# PARAMETERS:
#	taskOrId: concerned task
#
# ERRORS:
#   "Task not found"
# 
# RETURNS:
#

shellproc cret {args} {
    
    # check validity of task locally

    if {[wtxAgentModeGet] == "AGENT_MODE_EXTERN"} {
	set context CONTEXT_SYSTEM
	set cid 0
    } else {
	set context CONTEXT_TASK
	set cid [[shellName taskIdFigure] [lindex $args 0]]

	if {$cid == -1} {
	    error "Task not found."
	}
    }

    set retAddr [dbgRetAdrsGet $context $cid]
    if {$retAddr == "done"} {		; # PPC case
	return 0
    }

    set action [expr [wtxEnumFromString ACTION_TYPE ACTION_STOP] | \
		     [wtxEnumFromString ACTION_TYPE ACTION_NOTIFY]]
    set epNum [wtxEventpointAdd WTX_EVENT_TEXT_ACCESS $retAddr \
				$context $cid $action 0 0 0]

    wtxContextCont $context $cid 
    shStepEventWait $cid $context $retAddr
    wtxEventpointDelete $epNum

    return 0
}

##############################################################################
#
# e - set or display eventpoints
#
# SYNOPSIS:
#   e <addr>,<eventId>,<taskNameOrId>,<evtRtn>,<arg>
#
# PARAMETERS:
#   addr:	where to set eventpoint, or 0 means display all eventpoints
#   eventId:	event ID
#   taskNameOrId:
#		task affected; 0 means all tasks
#   evtRtn:	function to be invoked, NULL means no function is invoked
#   arg:	argument to be passed to <evtRtn>
#
# RETURNS: N/A
#
# ERRORS: N/A
#

shellproc e {args} {

    bindNamesToList {addr eventId task condRtn condArg} $args

    # an address was given.  If a task Id is given, constrain to that
    # task, unless we're in system mode, in which we'll set a system
    # breakpoint.

    if {[wtxAgentModeGet] == "AGENT_MODE_EXTERN"} {
        set cType CONTEXT_SYSTEM
        set cArg 0
        if {$task != 0} {
            puts stdout "warning: task parameter ignored in external mode"
        }
    } else {
        if {$task == 0} {
            # XXX should this be CONTEXT_ANY_TASK?
            set cType CONTEXT_TASK
            set cArg 0
        } else {
            set cType CONTEXT_TASK
            set cArg $task
        }
    }

    # The runtime callout, wdbE, requires three parameters (an event number,
    # and a conditioning routine and argument). However the WDB eventpoints
    # only support passing one argument to the callout. So here we allocate
    # a structure and pass it's address to the callout.
    # XXX - this creates a memory leak of 12 bytes.
    # We need to store the address of the structure with the eventpoint,
    # and have the breakpoint delete routines check and free the memory.

    if [catch "wtxSymFind -name wdbE" status] {
        error "Can't find target support routine wdbE"
    }
    set rout [lindex $status 1]
    set arg [wtxMemAlloc 12]
    set endianFlag "-[string range [targetEndian] 0 0]"
    set smb [memBlockCreate $endianFlag 12]
    memBlockSet -l $smb 0 $condRtn
    memBlockSet -l $smb 4 $condArg
    memBlockSet -l $smb 8 $eventId
    wtxMemWrite $smb $arg
    memBlockDelete $smb

    # XXX we don't implement the count field here
    # XXX we save the breakpoint information in an array, which we
    # probably shouldn't do, but currently wtxEventpointList doesn't
    # return the eventpoint numbers along with the events so we can't
    # query it that way.

    set epNum [wtxEventpointAdd \
            WTX_EVENT_TEXT_ACCESS $addr \
            $cType $cArg \
            ACTION_CALL 0 $rout $arg]

    # XXX every reference to epTable should probably be reconsidered
    # if the eventpoint interface changes

    global epTable

    set epTable($epNum) "$addr $cArg"
    return 0
}

##############################################################################
#
# s - step a task
#
# SYNOPSIS:
#   s [taskId [addr0 [addr1]]]
#
# PARAMETERS:
#   taskId - task to step, zero for default
#   addr0 - lower bound of step range
#   addr1 - upper bound of step range
#
# RETURNS:
#   NONE
#
# ERRORS:
#   NONE

shellproc s {args} {
    bindNamesToList {taskId addr0 addr1} $args

    if {[wtxAgentModeGet] == "AGENT_MODE_EXTERN"} {
	wtxContextStep CONTEXT_SYSTEM 0 $addr0 $addr1 
	shStepEventWait -1 CONTEXT_SYSTEM 
	return 0
    }

    set taskId [taskIdDefault $taskId]

    if {$taskId} {
	wtxContextStep CONTEXT_TASK $taskId $addr0 $addr1
	shStepEventWait $taskId CONTEXT_TASK 
    }
    return 0
}

##############################################################################
#
# so - step over function call
#
# SYNOPSIS:
#   so <task>
#
# PARAMETERS:
#  task:	task to step; 0 = use default
#
# RETURNS: N/A
#
# ERRORS: N/A
#

shellproc so {args} {
    global cpuType
    global __wtxCpuType
    global vxCpuFamily

    set mips16Mode 0

    # check validity of task locally

    if {[wtxAgentModeGet] == "AGENT_MODE_EXTERN"} {
	set context CONTEXT_SYSTEM
	set cid 0
    } else {
	set context CONTEXT_TASK
	set cid [[shellName taskIdFigure] [lindex $args 0]]

	if {$cid == -1} {
	    error "Task not found."
	}
    }
    
    set regSet [regSetOfContext $context $cid]
    set pc [pcOfRegSet $regSet]

    # check if it is in mips16 mode

    if {($cpuType($__wtxCpuType)) == "CW4000_16"} {
        set type [lindex [wtxSymFind -value $pc] 2]
        if {($type & 0x80) || ($pc & 0x1)} {
            set mips16Mode 1 
        }
        if {$mips16Mode} {
            set pc [expr $pc & (~0x1)]
        }  
    }

    if [dbgFuncCallCheck $pc] {
	#
	# put a breakpoint and continue until it is hit
	# use disassemble {} to get the next addr where to put breakpoint
	#
	set nextAddr [nextAddrGet $pc]
        if {$vxCpuFamily($__wtxCpuType) == "MIPS"} {

            # go past the delay slot
            set nextAddr [nextAddrGet $nextAddr]
        }
	set action [expr [wtxEnumFromString ACTION_TYPE ACTION_STOP] | \
			 [wtxEnumFromString ACTION_TYPE ACTION_NOTIFY]]

        if {$mips16Mode} {
            # set the mips16 function indicator for target WDB agent

            set nextAddr [expr $nextAddr | 1]
        }

	set epNum [wtxEventpointAdd WTX_EVENT_TEXT_ACCESS $nextAddr \
				    $context $cid $action 0 0 0]

	wtxContextCont $context $cid
 
        if {$mips16Mode} {
	    set nextAddr [expr $nextAddr & (~0x1)] 
        }

        shStepEventWait $cid $context $nextAddr
	wtxEventpointDelete $epNum
    } else {
	wtxContextStep $context $cid
	shStepEventWait $cid $context
    }

    return 0
}    

##############################################################################
#
# sysResume - reset the agent to tasking mode
#
# The agent is requested to enter tasking mode.
#
# SYNOPSIS:
#   sysResume
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

shellproc sysResume {args} {
    if [catch {wtxAgentModeSet AGENT_MODE_TASK} result] {
	puts stdout "Cannot enter tasking mode: $result"
	return -1
    }
    return 0
}    

##############################################################################
#
# sysSuspend - set the agent to external mode and suspend the system
#
# The agent is requested to enter system mode.  If this succeeds, the 
# system is suspended.
#
# SYNOPSIS:
#   sysSuspend
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

shellproc sysSuspend {args} {
    if [catch {wtxAgentModeSet AGENT_MODE_EXTERN} result] {
	puts stdout "Cannot enter system mode: $result"
	return -1
    }
    wtxContextSuspend CONTEXT_SYSTEM 0
    return 0
}

##############################################################################
#
# Locals Procedures (Procedures used by the shellprocs defined in this file)
#

##############################################################################
#
# brkptDisplay - display breakpoints
#
# This routine displays informations about breakpoints (software or hardware).
#
# SYNOPSIS: brkptDisplay
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc brkptDisplay {} {
    global hwBpTypeList

    foreach bpt [wtxEventpointListGet] {
	# This logic may not be very precise.  At the time of this writing,
	# breakpoints are the only supported eventpoint.  If we see a
	# breakpoint we assume that it should be printed in the following
	# way.  Non-breakpoint eventpoints are not handled by this routine.

	if {[lindex [lindex $bpt 0] 0] == "WTX_EVENT_TEXT_ACCESS" ||
	    [lindex [lindex $bpt 0] 0] == "WTX_EVENT_HW_BP"} {

	    set bpAddr [lindex [lindex $bpt 0] 1]

	    # test number of eventpoint parameters for backward 
	    # compatibiliy.

	    if {[llength [lindex $bpt 0]] > 2} {
		set bpCount [lindex [lindex $bpt 0] 2] 
	    } else {
		set bpCount 0
	    }
    
	    set bpSymAddr [shSymbolicAddr -inexact $bpAddr]
	    set bpTask [lindex [lindex $bpt 1] 1]
	    if {$bpTask == 0} {
		set bpTask "all"
	    }
	    if {[lindex [lindex $bpt 1] 0] == "CONTEXT_SYSTEM"} {
		set bpTask "SYSTEM"
	    }
	    if {[lindex [lindex $bpt 1] 0] == "CONTEXT_ANY_TASK"} {
		set bpTask "all"
	    }
	    if {[lindex [lindex $bpt 0] 0] == "WTX_EVENT_TEXT_ACCESS"} {
		puts stdout [format "0x%08x: %-18s Task: %10s Count: %2d" \
			$bpAddr $bpSymAddr $bpTask $bpCount]
	    } else {
		# this is a hardware breakpoint
		set access [lindex [lindex $bpt 0] 3]
		set bpTypeName "(hard-unknown)" 
		if {$hwBpTypeList != ""} {
		    foreach hwBp $hwBpTypeList {
			if {[lindex $hwBp 0] == $access} {
			    set bpTypeName [lindex $hwBp 1]
			    break
			}
		    }
		}
		puts stdout "[format "0x%08x: %-18s Task: %10s Count: %2d " \
				$bpAddr $bpSymAddr $bpTask $bpCount] \
				$bpTypeName"
	    }
	}
    }
}

##############################################################################
#
# shStepEventWait - wait for a given event
#
# Wait for a given event, others are dispatched
#
# SYNOPSIS:
#   shStepEventWait evtCtx evtCtxType [evtAddr]
#
# PARAMETERS:
#   evtCtx:	context of the waited event
#   evtCtxType:	context type of the waited event
#   evtAddr:	address of the waited event
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc shStepEventWait {evtCtx evtCtxType {evtAddr any}} {
    global lSavedBase 
    global chanInOut
    global shellConfig
    global shPollInterval
    global cpuType
    global __wtxCpuType

    while {1} {
    	# Check the event list every $shPollInterval ms
	set event [eventCheck TEXT_ACCESS]

	if {(![string compare [lindex $event 0] "TEXT_ACCESS"]) &&
	    ([lindex $event 1] == $evtCtx || [lindex $event 1] == 0xffffffff) &&
	    ([lindex $event 2] == 
	     [wtxEnumFromString CONTEXT_TYPE $evtCtxType])} {
	    #
	    # the task hit a breakpoint if it is the one expected
	    # display registers and disassembled instr as cret usually did,
	    # otherwise only display the breakpoint 
	    #
	    if {(![string compare $evtAddr "any"]) ||
		[lindex $event 3] == $evtAddr} {
		set regSet [regSetOfContext $evtCtxType $evtCtx]
		regSetPrint $regSet

		# disassemble 1 instruction 
		# (take care 1st line is not a symbol line)
		# symbols and instructions lines are separated by "\n" char

                if {($cpuType($__wtxCpuType)) == "CW4000_16"} {

		# mask off possible mips16 function indicator

		    set dsmLine [wtxMemDisassemble -address -opcodes \
			     [expr [lindex $event 3] & (~0x1)] 1]
                } elseif {($cpuType($__wtxCpuType)) == "FR500"} {

		# FR500 is a vliw processor, the max packet length is
		# 4. 
		# If instructions are like 
		# movsg lr, gr12
		# add.p gri, grj, grk
		# addi  grx, #4, gry
		# and if a break point is set on movsg instruction
		# on single stepping, disassembler should display
		# a complete VLIW instruction 
		# for ex, in this case it should display add.p & addi
		# together.
	
		set nextInstAddr [lindex $event 3]
		
		# Please do not change this format, tcl does not like
		# it.

		set vliwMask [format "%#08x" 0x80000000]
		
		# Leave  blank line, it just makes display looks
		# better.

		puts "\n"

		for {set i 0} {$i < 4} {incr i 1} {
	                set dsmLine [wtxMemDisassemble -address -opcodes \
				$nextInstAddr  1]
			set dsmLine [split $dsmLine \n]

			# get first (and unique) member

			set dsmLine [lindex $dsmLine 0]

			# format line

			set formattedLine [format "    %8.8s  %-24.24s %s" \
				    [lindex $dsmLine 2] [lindex $dsmLine 3] \
				    [lindex $dsmLine 4]]

			puts stdout $formattedLine
			set opCodePart [lindex $dsmLine 3]

			# Unfortunately tcl does not understand hex
			# no. To do this, first put 0x in the begining
			# and then convert it into hex
 
			set tmpVar "0x$opCodePart"
			set tmp1Var [format "%#08x" $tmpVar ]


			# Check for the packing flag

			set checkBit [expr $tmp1Var & $vliwMask]

			if {$checkBit == 0} {
				incr nextInstAddr 4
			} else {
				# End of vliw packet so break;
			 break
			}
				
		}
		set lSavedBase nextInstAddr
		break;
		} else {
                    set dsmLine [wtxMemDisassemble -address -opcodes \
				[lindex $event 3] 1]
                }
		set dsmLine [split $dsmLine \n]

		# get first (and unique) member

		set dsmLine [lindex $dsmLine 0]

		# format line

		set formattedLine [format "    %8.8s  %-24.24s %s" \
				    [lindex $dsmLine 2] [lindex $dsmLine 3] \
				    [lindex $dsmLine 4]]

		puts stdout $formattedLine

		# make disassembler start here by default.

		set lSavedBase [lindex $event 3]
	    } else {
		# This event is not the expected one, dispatch it
	    	shEventDispatch $event
	    }
	    break
	}

	# Does WindSh set up to give I/O control to called functions ?
	# If yes we must send characteres typed on WindSh to the channel
	# associated with the called task Standard Input

	# XXX - jmp
	# polling windSh standard input is very slow (about 200ms), so we
	# only sleep $shPollInterval ms if we do not call shInputToVio()

	if {$shellConfig(SH_GET_TASK_IO) == "on"} {
	    shInputToVio $chanInOut
	} else {
	    msleep $shPollInterval
	}
    }
}
