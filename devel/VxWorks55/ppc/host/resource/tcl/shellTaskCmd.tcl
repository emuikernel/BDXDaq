# shellTaskCmd.tcl - Tcl implementation of WindSh "task" commands
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01h,27mar02,dtr  Adding check for altivec availability.
# 01g,07oct99,myz  modified sp for CW4000_16 in system mode.
# 01f,25nov98,jmp  changed td safe from deletion error message.
# 01e,13oct98,jmp  modified string allocation method in WindSh.
# 01d,15jul98,dbt  completed previous modification for sps() command.
#		   modified td(), ts() and tr() to accept task names in
#		   parameters.
# 01c,09jul98,fle  SPR#21727 : prepended spawned task names with sshToolNum
# 01b,22jun98,dbt  added a test in tt() if it is used in system mode
#		   removed obsolete CAVEAT message in tt() header.
# 01a,01apr98,jmp  created based on routines moved out from shell.tcl
#

# DESCRIPTION
# This module holds the "task" shellproc of WindSh:
#
# sp - spawn a task with default spawn parameters.
# sps - spawn a task with default spawn parameters; leave it suspended.
# taskIdDefault - return (or set) the "default" task ID
# taskIdFigure - figure out task ID, given name or number
# td - alias for taskDelete
# ti - a wrapper for taskShow, with verbosity level 1.
# tr - alias for taskResume
# ts - alias for taskSuspend
# tt - print a stack trace of a task
#   
# This module also contains windsh private routines used by the
# "task" shellprocs
#


# globals

# sp* global variables - mimic their equivalents in the traditional shell.
# These values are used as default arguments to the taskSpawn call when
# the sp command is given.
set spTaskPriority	100
set spTaskOptions	0x19	;# VX_SUPERVISOR_MODE|VX_STDIO|VX_FP_TASK
set spTaskStackSize	20000


if ![info exists spTaskNum] {
    set spTaskNum 0
}

##############################################################################
#
# sp - spawn a task with default spawn parameters.
#

shellproc sp {args} {
    global spTaskNum			;# number of spawned tasks
    global shToolNum			;# shell tool number
    global __wtxCpuType
    global cpuType

    bindNamesToList {entry a0 a1 a2 a3 a4 a5 a6 a7 a8 a9} $args

    if {$entry == 0} {
	error "Sorry, won't spawn a task at address 0."
    }

    set spName [format "s%iu%d" $shToolNum $spTaskNum]
    incr spTaskNum

    if {[wtxAgentModeGet] == "AGENT_MODE_EXTERN"} {
	# system mode.  use a direct call to excJobAdd.

	set excJobAdd [shSymAddr excJobAdd]
	set spawnFunc [shSymAddr wdbSp]

	if {($cpuType($__wtxCpuType)) == "CW4000_16"} {
	    if {[lindex [wtxSymFind -value $excJobAdd] 2] & 0x80} {
		set excJobAdd [expr $excJobAdd | 0x1]
            }

	    if {[lindex [wtxSymFind -value $spawnFunc] 2] & 0x80} {
		set spawnFunc [expr $spawnFunc | 0x1]
            }
        }

	if [expr $a5 || $a6 || $a7 || $a8 || $a9] {
	    puts stdout "warning: tasks started from windsh in system mode"
	    puts stdout "receive only 5 arguments."
	}

	wtxDirectCall $excJobAdd $spawnFunc $entry $a0 $a1 $a2 $a3 $a4
	return 0
    }

    set newTaskId [shSpawn $spName $entry $a0 $a1 $a2 $a3 $a4 \
	    $a5 $a6 $a7 $a8 $a9]

    wtxContextResume CONTEXT_TASK $newTaskId

    puts stdout [format "task spawned: id = %x, name = %s" $newTaskId $spName]
    return $newTaskId
}

##############################################################################
#
# sps - spawn a task with default spawn parameters; leave it suspended.
#

shellproc sps {args} {
    global spTaskNum			;# number of spawned tasks
    global shToolNum			;# shell tool number

    bindNamesToList {entry a0 a1 a2 a3 a4 a5 a6 a7 a8 a9} $args

    if {$entry == 0} {
	error "Sorry, won't spawn a task at address 0."
    }

    set spName [format "s%iu%d" $shToolNum $spTaskNum]
    incr spTaskNum

    set newTaskId [shSpawn $spName $entry $a0 $a1 $a2 $a3 $a4 \
	    $a5 $a6 $a7 $a8 $a9]

    puts stdout [format "task spawned (suspended): id = %x, name = %s" \
	    $newTaskId $spName]
    return $newTaskId
}

##############################################################################
#
# taskIdDefault - return (or set) the "default" task ID
#
# return (or set) the "default" task ID, as understood by the
# host-implemented functions that deal with tasks (ts, tr, etc.).  If
# the first argument is zero, the current default is returned if it is
# set.  If it is not set, an error is thrown.  If the first argument
# is nonzero, the default is established (and is also returned).
#
# SYNOPSIS:
#   taskIdDefault [taskId]
#
# PARAMETERS:
#   taskId (optional): new default taskId
#
# RETURNS:
#   default task ID
#
# ERRORS:
#   "No default task has been established."

shellproc taskIdDefault {args} {

    bindNamesToList id $args
    taskIdDefault $id
}

##############################################################################
#
# taskIdFigure - figure out task ID, given name or number
#
# If 0 is passed, the value of [taskIdDefault 0] is returned.  Otherwise,
# the list of active task IDs is searched for the given number; if found,
# it is returned.  Next, a task whose name is the number (in hex) it
# sought; if found, its ID is returned.  Otherwise, if the given address
# points to a string that matches an existing task name, that task's ID
# is returned.  If all this fails, -1 is returned.
#
# SYNOPSIS:
#   taskIdFigure nameOrId
#
# PARAMETERS:
#   nameOrId - string name, or task ID itself
#
# RETURNS:
#   a task ID, or -1
#
# ERRORS:
#   NONE

shellproc taskIdFigure {args} {
    set task [lindex $args 0]

    if {$task == 0} {return [taskIdDefault 0]}

    set atnMap [activeTaskNameMap]
    # first see if the numeric form matches a task ID.
    set ix [lsearch $atnMap $task]
    if {$ix >= 0 && !($ix & 1)} {
	# the number was found in an "even" (task id) slot of the
	# table.  So it's a task ID.

	return $task
    }

    # Perhaps the task has a numeric name.

    #set tnStr [format "%x" $task]
    #set ix [lsearch $atnMap $tnStr]
    set ix [lsearch $atnMap $task]

    if {$ix >= 0 && ($ix & 1)} {
	# The number was found in an  "odd" (task name) slot of the
	# table.  So it's a task with a numeric name.

	return [lindex $atnMap [expr $ix - 1]]
    }

    # Hopefully the user has passed the address of a legitimate
    # string on the target.  Let's upload it and compare it to
    # the list of known task names. XXX: we only allow 32 characters
    # in a task name stored this way.

    set str [stringGet $task]
    set ix [lsearch $atnMap $str]
    if {$ix >= 0 && ($ix & 1)} {
	# matched task name slot!
	return [lindex $atnMap [expr $ix - 1]]
    }

    # Utter failure.

    return -1
}

##############################################################################
#
# td - alias for taskDelete
#
# This procedure deletes a specified task.
#
# SYNOPSIS
#  td tid
#
# PARAMETERS
# tid: task id, or task name, of the task to delete
#
# RETURNS: N/A
#
# ERRORS
# "Cannot kill a task which is safe from deletion" if the task can't be
#     deleted (usage of mutex semaphore with option SEM_DELETE_SAFE, or call
#     to taskSafe()).
#

shellproc td {args} {
    global offset

    set tid [[shellName taskIdFigure] [lindex $args 0]]

    if {$tid == -1} {
	puts stdout "Task not found."
	return -1
    }

    # Look at the safeCnt value in the TCB
    set result [wtxGopherEval "$tid <+$offset(WIND_TCB,safeCnt) @>"]

    if {$result > 0} {
        error "Cannot kill task $tid which is currently safe from deletion"
    } else {
        return [shTaskOp wtxContextKill $tid]
    }

}


##############################################################################
#
# ti - a wrapper for taskShow, with verbosity level 1.
#

shellproc ti {args} {

    set tid [[shellName taskIdFigure] [lindex $args 0]]

    if {$tid == -1} {
	puts stdout "Task not found."
	return -1
    }

    return [[shellName taskShow] [taskIdDefault $tid] 1]
}


##############################################################################
#
# tr - alias for taskResume
#

shellproc tr {args} {
    set tid [[shellName taskIdFigure] [lindex $args 0]]

    if {$tid == -1} {
	puts stdout "Task not found."
	return -1
    }

    return [shTaskOp wtxContextResume $tid]
}

##############################################################################
#
# ts - alias for taskSuspend
#

shellproc ts {args} {
    set tid [[shellName taskIdFigure] [lindex $args 0]]

    if {$tid == -1} {
	puts stdout "Task not found."
	return -1
    }

    return [shTaskOp wtxContextSuspend $tid]
}

##############################################################################
#
# tt - print a stack trace of a task
#
# SYNOPSIS:
#   tt [taskId]
#
# This routine prints a list of the nested routine calls that the specified
# task is in.  Each routine call and its parameters are shown.
#
# If <task> is not specified or zero, the last task referenced is
# assumed.  The tt() routine can only trace the stack of a task other than
# itself.  For instance, when tt() is called from the shell, it cannot trace
# the shell's stack.
#
# EXAMPLE
# .CS
#     -> tt "logTask"
#      3ab92 _vxTaskEntry   +10 : _logTask (0, 0, 0, 0, 0, 0, 0, 0, 0, 0)
#       ee6e _logTask       +12 : _read (5, 3f8a10, 20)
#       d460 _read          +10 : _iosRead (5, 3f8a10, 20)
#       e234 _iosRead       +9c : _pipeRead (3fce1c, 3f8a10, 20)
#      23978 _pipeRead      +24 : _semTake (3f8b78)
#     value = 0 = 0x0
# .CE
# This indicates that logTask() is currently in semTake() (with
# one parameter) and was called by pipeRead() (with three parameters),
# which was called by iosRead() (with three parameters), and so on.
#
# INTERNAL
# This higher-level symbolic stack trace is built on top of the
# lower-level routines provided by trcLib.
#
# CAVEAT
# In order to do the trace, some assumptions are made.  In general, the
# trace will work for all C language routines and for assembly language
# routines that start with a LINK instruction.  Some C compilers require
# specific flags to generate the LINK first.  Most VxWorks assembly language
# routines include LINK instructions for this reason.  The trace facility
# may produce inaccurate results or fail completely if the routine is
# written in a language other than C, the routine's entry point is
# non-standard, or the task's stack is corrupted.  Also, all parameters are
# assumed to be 32-bit quantities, so structures passed as parameters will
# be displayed as \f2long\fP integers.
#
# RETURNS:
# OK, or ERROR if the task does not exist.
#
# ERRORS:
# Task not found
# Cannot trace task: <tid>
# Cannot trace current task (<tid>) in system mode
# tt not supported in system mode
#
# SEE ALSO:
# .pG "Debugging"

shellproc tt {args} {
    # globals
    global env
    global __wtxCpuType
    global cpuType
    global cpuFamily
    global offset

    # check validity of task locally

    set tid [[shellName taskIdFigure] [lindex $args 0]]

    if {$tid == -1} {
	error "Task not found."
    }

    # make sure task (or system) is suspended  before examining its stack

    set ttSuspended 0

    if {[wtxAgentModeGet] == "AGENT_MODE_EXTERN"} {
	if [catch "wtxContextStatusGet CONTEXT_SYSTEM 0" result] {
	    error "tt not supported in system mode"
	}

	if {$result == "CONTEXT_RUNNING"} {
	    wtxContextSuspend CONTEXT_SYSTEM 0
	    set ttSuspended 1
	}

	# it is not possible to trace current task since the TCB is not
	# up to date (no context switch). This should be fixed in 
	# the target agent.

	if {[taskIdCurrent] == $tid} {

	    # resume the context if necessary

	    if {$ttSuspended} {
		wtxContextResume CONTEXT_SYSTEM 0
	    }

	    # print error message and leave

	    error "Cannot trace current task ($tid) in system mode"
	}

	# examine task stack
    
	if [catch "trcStack $tid" trc] {

	    # We enter here if the system agent is not able to read
	    # tasks registers (eg emulators).

	    # resume the context if necessary

	    if {$ttSuspended} {
		wtxContextResume CONTEXT_SYSTEM 0
	    }

	    # print error message and leave

	    error "tt not supported in system mode"
	}

	# unsuspend if it is necessary
    
	if {$ttSuspended} {
	    wtxContextResume CONTEXT_SYSTEM $tid
	}
    } else {
	set blk [wtxMemRead [expr $tid + $offset(WIND_TCB,status)] 4]
	set status [expr [memBlockGet -l $blk]]
	memBlockDelete $blk
	if {!($status & 0x01)} {
	    if {([taskIdToName $tid] != "tWdbTask") && 
		([taskIdToName $tid] != "tNetTask")} {
		wtxContextSuspend CONTEXT_TASK $tid
		set ttSuspended 1
	    } else {
		error "Cannot trace task: $tid"
	    }
	}

	# examine task stack
    
	set trc [trcStack $tid]

	# unsuspend if it is necessary
    
	if {$ttSuspended} {
	    wtxContextResume CONTEXT_TASK $tid
	}
    }

    foreach line $trc {
	# start with en empty line

	set ttOutput ""

	# put current address

	set callAddr [lindex $line 0]
	append ttOutput [format "%-8x " $callAddr]

	# print the caller name and offset

	if [catch "wtxSymFind -value $callAddr" callerSym] {
	    append ttOutput "                   : "
	} else {
	    set callerAddr [lindex $callerSym 1]
	    set callerName [lindex $callerSym 0]
	    append ttOutput [format "%-15s+%-3x: " $callerName \
			    [expr $callAddr - $callerAddr]]
	}
	
	# put calling function name or address

	append ttOutput "[lindex $line 1] ("

	set usingDefault 0

	# if the number of arg is unknown print default number of args

	set numParam [lindex $line 2]
	if {$numParam == -1} {
	    set usingDefault 1
	    append ttOutput "\["
	}

	# process each param

	set argList [lindex $line 3]
	set numParam [llength $argList]
	
	for {set paramNum 0; set paramVal [lindex $argList 0]} \
	    {$paramNum != $numParam} \
	    {incr paramNum; set paramVal [lindex $argList $paramNum]} {

	    # try to get associated symbol

	    if [catch "wtxSymFind -value $paramVal" paramSym] {
		append ttOutput [format "%x" $paramVal]
	    } else {
		set paramName [lindex $paramSym 0]
		set paramFindVal [lindex $paramSym 1]

		# put the actual symbol or value

		if {$paramVal == $paramFindVal} {
		    append ttOutput [format "&%s" $paramName]
		} else {
		    append ttOutput [format "%x" $paramVal]
		}
	    }

	    # put comma where needed

	    if {$paramNum != [expr $numParam - 1]} {
		append ttOutput ", "
	    }
	}

	# close parenthesis

	if {$usingDefault == 1} {
	    append ttOutput "\]"
	}
	append ttOutput ")"

	# output the trace

	puts stdout $ttOutput
    }

    return 0
}


##############################################################################
#
# Locals Procedures (Procedures used by the shellprocs defined in this file)
#

##############################################################################
#
# shTaskOp - run $op on the given task
#

proc shTaskOp {op task} {
    set tid [[shellName taskIdFigure] $task]

    if {$tid == -1} {
	puts stdout "Task not found."
	return -1
    }

    $op CONTEXT_TASK $tid
    return 0
}

##############################################################################
#
# shSpawn - work routine for sp, sps
#

proc shSpawn {name entry a0 a1 a2 a3 a4 a5 a6 a7 a8 a9} {
    global spTaskPriority
    global spTaskOptions
    global spTaskStackSize
    global __wtxTsInfo
    global tcbOptionBit

    set cpuDesc [lindex $__wtxTsInfo 2]
    set hasAltivec [lindex $cpuDesc 5]
    set newTaskOptions $spTaskOptions

    if { $hasAltivec == 1 } {
	set newTaskOptions [expr {$spTaskOptions + $tcbOptionBit(VX_ALTIVEC_TASK)}]
	set newTaskOptions [format "%#x" $newTaskOptions] 
    }
  
    return [wtxContextCreate CONTEXT_TASK $name $spTaskPriority \
	    $newTaskOptions 0 $spTaskStackSize $entry 0 0 \
	    $a0 $a1 $a2 $a3 $a4 $a5 $a6 $a7 $a8 $a9]

}


proc taskIdGet {args} {
    set task [lindex $args 0]

    if {$task == 0} {return [taskIdDefault 0]}

    set atnMap [activeTaskNameMap]
    # first see if the numeric form matches a task ID.
    set ix [lsearch $atnMap $task]
    if {$ix >= 0 && !($ix & 1)} {
	# the number was found in an "even" (task id) slot of the
	# table.  So it's a task ID.

	return $task
    }

    # Perhaps the task has a numeric name.

    set ix [lsearch $atnMap $task]
    if {$ix >= 0 && ($ix & 1)} {
	# The number was found in an  "odd" (task name) slot of the
	# table.  So it's a task with a numeric name.

	return [lindex $atnMap [expr $ix - 1]]
    }
}
