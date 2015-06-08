# 01taskWaitShow.tcl - Implementation of shell taskWaitShow commands in Tcl
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01b,05jun98,f_l  moved taskWaitPrint and taskWaitBannerPrint from
#                  /host/ressource/tcl/01taskWaitCore.tcl
# 01a,29may98,f_l  written.
#
#
#
# DESCRIPTION
# This file holds the Tcl implementation of the windsh taskWaitShow commands
#
# RESOURCE FILES
# wpwr/host/resource/tcl/taskWaitCore.tcl
#

# source the data extraction and formatting routines

source [wtxPath host resource tcl]taskWaitCore.tcl

##############################################################################
#
# tw - a wrapper for taskWaitShow (one task, verbose level).
#
# This command doesn't support POSIX semaphores and message queues.
# This command doesn't support pending signals.
#

shellproc tw {args} {

    set tid [[shellName taskIdFigure] [lindex $args 0]]

    if {$tid == -1} {
        puts stdout "Task not found."
        return -1
    }
    return [[shellName taskWaitShow] [taskIdDefault $tid] 1]
}

###############################################################################
#
# w - a wrapper for taskWaitShow (all active tasks).
#
# This command doesn't support POSIX semaphores and message queues.
# This command doesn't support pending signals.
#

shellproc w {args} {

    # get an empty list if w is requested for all tasks or the list
    # of tasks for which w is requested (list size <= 10)

    set taskList {}
    foreach value $args {
	if {$value != "0x0"} {lappend taskList $value}
    }

    # get a list of active task IDs, and priorities.

    set aQ [activeQueue 1]

    # Reorganize that linear list into a list of {taskId priority} couples.

    set aList ""
    while {[llength $aQ] >= 2} {
	lappend aList [lrange $aQ 0 1]
	set aQ [lrange $aQ 2 end]
    }
    set aList [lsort -command activeQOrder $aList]
    taskWaitBannerPrint
    if {[llength $taskList] > 0} {
	foreach taskCouple $aList {
	    set task [lindex $taskCouple 0]
	    if {[lsearch $taskList $task] != -1} {
		taskWaitPrint [taskWaitGet $task]
	    }
	}
    } {
	foreach taskCouple $aList {
	    set task [lindex $taskCouple 0]
	    taskWaitPrint [taskWaitGet $task]
	}
    }
    return 0
}



##############################################################################
#
# taskWaitShow - show information about the object a task is pended on
#
# This routine shows information about the ojbect a task is pended on.
# This routine doesn't support POSIX semaphores and message queues.
# This command doesn't support pending signals.
#
# List of object types:
# - semaphores -
#       SEM_B : binary
#       SEM_M : mutex
#       SEM_C : counting
#       SEM_O : old
#       SEM_SB : shared_binary
#       SEM_SC : shared counting
# - message queues -
#	MSG_Q(R) : task is pended on a msgQReceive command
#	MSG_Q(S) : task is pended on a msgQSend command
# - shared  message queues -
#	MSG_Q_S(R) : task is pended on a msgQReceive command
#	MSG_Q_S(S) : task is pended on a msgQSend command
#
# - unknown object -
# 	N/A : The task is pended on an unknown object (POSIX ?, signal ?).
#	      Sometime, a task changes of state during the taskWaitGet or
#	      taskWaitSmGet execution. So that at it is not possible to get
#	      the object on which the task was pended a few microseconds
#	      before. 
#
# SYNOPSIS:
#   taskWaitShow taskId 
#
# PARAMETERS:
#   taskId : the task to fetch information about
#
# RETURNS: N/A
#
# ERRORS: N/A
#
#

shellproc taskWaitShow {args} {

    bindNamesToList {tid verbose} $args
    set taskWaitInfoList [taskWaitGet $tid]
    taskWaitBannerPrint
    taskWaitPrint $taskWaitInfoList
    if {$verbose} {
	set objId [lindex $taskWaitInfoList 6]
	if {$objId} {
	   [shellName show] "$objId" 1
	}
    }
    return
}


###############################################################################
#
# taskWaitPrint - format and print information collected for one task
# in the format used by the "w" command.
#

proc taskWaitPrint {taskWaitInfoList} {

    bindNamesToList {name entry taskId status delayCount objType\
			 objId objName} $taskWaitInfoList
    puts stdout [format "%-10.10s %-10.10s %8x %-10.10s %5d %-10.10s %8x\
	%-10.10s"\
        $name\
	$entry\
        $taskId\
        $status\
        $delayCount\
	$objType\
	$objId\
	$objName]
}				


##############################################################################
#
# taskWaitBannerPrint - print the usual "w" command banner
#
# SYNOPSIS:
#   taskWaitBannerPrint
#

proc taskWaitBannerPrint {} {
    puts stdout {  NAME       ENTRY      TID      STATUS   DELAY  OBJ_TYPE   OBJ_ID   OBJ_NAME}
    puts stdout {---------- ---------- -------- ---------- ----- ---------- -------- ------------}
}

