# 01taskHookShow.tcl - Implementation of shell taskHookShow commands in Tcl
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,15jun98,f_l  written.

# DESCRIPTION
# This file holds the Tcl implementation of the windsh taskHookShow commands
#
# RESOURCE FILES
# wpwr/host/resource/tcl/taskHookShowCore.tcl
#


# source the data extraction and formatting routines

source [wtxPath host resource tcl]taskHookShowCore.tcl

##############################################################################
#
# taskCreateHookShow - show the list of task create routines
#
# This routine shows all the task create routines installed in the task
# create hook table, in the order in which they were installed.
# You must define the INCLUDE_SYM_TBL constant in the BSP config.h file
# to synchronize host and target symbole tables, in order to use this routine.
#
# SYNOPSIS: N/A
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

shellproc taskCreateHookShow {args} {
    set hookRtnList [taskCreateHookGet]
    foreach hookRtn $hookRtnList {
 	bindNamesToList {rtnName rtnId} $hookRtn
	puts stdout [format "%-15.15s %-10.10s" $rtnName $rtnId]
    }
}

##############################################################################
#
# taskDeleteHookShow - show the list of task delete routines
#
# This routine shows all the delete routines installed in the task delete
# hook table, in the order in which they were installed.  Note that the
# delete routines will be run in reverse of the order in which they were
# installed.
# You must define the INCLUDE_SYM_TBL constant in the BSP config.h file
# to synchronize host and target symbole tables, in order to use this routine.
# 
# SYNOPSIS: N/A
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

shellproc taskDeleteHookShow {args} {
    set hookRtnList [taskDeleteHookGet]
    foreach hookRtn $hookRtnList {
 	bindNamesToList {rtnName rtnId} $hookRtn
	puts stdout [format "%-15.15s %-10.10s" $rtnName $rtnId]
    }
}


##############################################################################
#
# taskSwitchHookShow - show the list of task switch routines
#
# This routine shows all the switch routines installed in the task
# switch hook table, in the order in which they were installed.
# You must define the INCLUDE_SYM_TBL constant in the BSP config.h file
# to synchronize host and target symbole tables, in order to use this routine.
#
# SYNOPSIS: N/A
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

shellproc taskSwitchHookShow {args} {
    set hookRtnList [taskSwitchHookGet]
    foreach hookRtn $hookRtnList {
 	bindNamesToList {rtnName rtnId} $hookRtn
	puts stdout [format "%-15.15s %-10.10s" $rtnName $rtnId]
    }
}
