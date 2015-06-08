# taskHookShowCore.tcl - Implementation of taskHookShow core routines
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,15jun98,f_l  written.

# DESCRIPTION
# This module is the Tcl code for the taskHookShow core routines.
#
# RESOURCE FILES
#


##############################################################################
#
# taskHookGet - get the hooks in a hook table
#
# This routine gets the contents of a hook table symbolically.
# You must define the INCLUDE_SYM_TBL constant in the config.h file
# to synchronize host and target symbole tables, in order to use this routine.
#
# SYNOPSIS:
#   taskHookGet symHookTable tableSize errMsg
#
# PARAMETERS:
#   symHookTable: the table of hook symboles
#   tableSize : the max size of this table
#   errMsg : error message to display when the table of hook symboles is
#	     not found
#
# RETURNS:
#   a list of 2 items list:
#   hook symbole and hook id
#
# ERRORS:
#   <errMsg>
#

proc taskHookGet {symHookTable tableSize errMsg} {

    set symHookTableAddr [symAddrGet $symHookTable $errMsg]

    # tableSize
    # this is the max number of create hooks

    set hookIdList [wtxGopherEval "$symHookTableAddr\
				 ($tableSize @)"]
    set hookRtnList ""
    foreach hookId $hookIdList {
	if {!$hookId} {break}
	set result [wtxSymFind -value $hookId]
	if {[lindex $result 1] == $hookId} {

	     # the wtxSymFind -value routine returns the closest symbol
	     # this test verifies if the returned symbol id is the same as
	     # the hookId

	     lappend hookRtnList [lrange $result 0 1]
	}
    }
    return $hookRtnList
}

##############################################################################
#
# taskCreateHookGet - get the list of task create routines
#
# his routine gets all the task create routines installed in the task
# create hook table, in the order in which they were installed.
# You must define the INCLUDE_SYM_TBL constant in the config.h file
# to synchronize host and target symbole tables, in order to use this routine.
#
# SYNOPSIS: N/A
#
# PARAMETERS: N/A
#
# RETURNS:
#   a list of 2 items list:
#   hook symbole and hook id
#
# ERRORS:
# task create Hook table not found
#
  
proc taskCreateHookGet {} {

    set VX_MAX_TASK_CREATE_RTNS 16

    # VX_MAX_TASK_CREATE_RTNS = 16 defined in taskLib.h
    # this is the max number of create hooks

    # taskCreateTable is the global table containing create hook routines ids

    return [taskHookGet taskCreateTable $VX_MAX_TASK_CREATE_RTNS \
		"task create Hook table not found"]
}

##############################################################################
#
# taskDeleteHookGet - get the list of task delete routines
#
# his routine gets all the task delete routines installed in the task
# delete hook table, in the order in which they were installed.
# You must define the INCLUDE_SYM_TBL constant in the config.h file
# to synchronize host and target symbole tables, in order to use this routine.
#
# SYNOPSIS: N/A
#
# PARAMETERS: N/A
#
# RETURNS:
#   a list of 2 items list:
#   hook symbole and hook id
#
# ERRORS:
# task delete Hook table not found
#

proc taskDeleteHookGet {} {

    set VX_MAX_TASK_DELETE_RTNS 16

    # VX_MAX_TASK_DELETE_RTNS = 16 defined in taskLib.h
    # this is the max number of delete hooks

    # taskDeleteTable is the global table containing delete hook routines ids

    return [taskHookGet taskDeleteTable $VX_MAX_TASK_DELETE_RTNS \
		"task delete Hook table not found"]
}

##############################################################################
#
# taskSwitchHookGet - get the list of task switch routines
#
# his routine gets all the task switch routines installed in the task
# switch hook table, in the order in which they were installed.
# You must define the INCLUDE_SYM_TBL constant in the config.h file
# to synchronize host and target symbole tables, in order to use this routine.
#
# SYNOPSIS: N/A
#
# PARAMETERS: N/A
#
# RETURNS:
#   a list of 2 items list:
#   hook symbole and hook id
#
# ERRORS:
# task switch Hook table not found
#

proc taskSwitchHookGet {} {

    set VX_MAX_TASK_SWITCH_RTNS 16

    # VX_MAX_TASK_SWITCH_RTNS = 16 defined in taskLib.h
    # this is the max number of switch hooks

    # taskSwitchTable is the global table containing switch hook routines ids

    return [taskHookGet taskSwitchTable $VX_MAX_TASK_SWITCH_RTNS \
		"task switch Hook table not found"]
}
