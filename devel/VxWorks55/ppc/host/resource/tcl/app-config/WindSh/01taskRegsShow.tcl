# 01taskRegsShow.tcl - Implementation of shell taskRegsShow command in Tcl
#
# Copyright 1998-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01b,09nov01,aeg  updated taskRegsShow
# 01a,15jun98,f_l  written.
#
# DESCRIPTION
# This file holds the Tcl implementation of the windsh taskRegsShow command
#

##############################################################################
#
# taskRegsShow - display the contents of a task's registerS
#
#
# This routine displays the register contents of a specified task
# on standard output.
# It is equivalent to the taskShow routine with the moderate verbosity level 1
#
# SYNOPSIS:
#   taskRegsShow taskId/taskName
#
# PARAMETERS:
#   taskId/taskName: the task to fetch information about
#
# RETURNS:
#   A list of items depending on architecture.
#
# ERRORS: N/A
#

shellproc taskRegsShow {args} {
    global stackGrows

    set taskId [[shellName taskIdFigure] [lindex $args 0]]
    if {$taskId == -1} {
        puts stdout "Task not found."
        return -1
    }

    set taskInfoList [taskInfoGetVerbose [taskIdDefault $taskId]]

    regSetPrint [lrange $taskInfoList 20 end]

    puts stdout {}
    return 0
}
