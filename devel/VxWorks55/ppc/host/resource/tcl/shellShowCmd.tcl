# shellShowCmd.tcl - Tcl implementation of WindSh "show" commands
#
# Copyright 1998-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01p,27nov02,bwa  Added printout of EVENT_KEEP_UNWANTED options of VxWorks
#                  events.
# 01o,10dec01,aeg  fixed display of queuing type in msgQShow.
# 01n,06dec01,sn   autodeduce shDemangleStyle
# 01m,08nov01,aeg  added display of VxWorks events to taskShow/semShow/msgQShow.
# 01l,02nov01,tam  fixed memShow display (SPR 70538 & 70236)
# 01k,30oct01,fmk  fix problem with iosFdShow
# 01j,15oct01,fmk  change iosFdShow to display an arbitrary number of fds
# 01i,18apr01,kab  Added Altivec bit to display
# 01h,27sep00,zl   added VX_DSP_TASK to taskOptionNames
# 01g,18feb99,drm  Changing output of msgQShow() for distributed message
#                  queues.
# 01f,13oct98,jmp  modified string allocation method in WindSh.
# 01e,19aug98,drm  changed msgQShow to allow printing of dist. message Qs
# 01d,09jul98,jmp  imported rBuffShow() and trgShow() from shell.tcl.
# 01c,20jun98,fle  removed a debug string I inrtroduced
# 01b,15jun98,fle  reformated output layout of moduleShow
# 01a,01apr98,jmp  created based on routines moved out from shell.tcl
#

# DESCRIPTION
# This module holds the "show" shellproc of WindSh:
#
# agentModeShow - show agent mode 
# browse - send a message to the browser asking it to browse an address
# checkStack - print stack consumption information for active tasks.
# classShow - show information about a class of objects
# devs - an alias for iosDevShow
# i - summarize TCB information for all tasks in the active queue.
# intVecShow - show information about a given interrupt vector
# iStrict - summarize TCB information for all tasks in the active queue.
# iosDevShow - show all devices known to the I/O system: their names
# iosDrvShow - show the addresses of the I/O functions for each I/O
# iosFdShow - show all existing file descriptors
# memPartShow - show statistics about a target memory partition usage
# memShow - show statistics about target memory usage
# moduleIdFigure - figure out module ID, given name or number
# moduleShow - show information about one module (or all of them)
# mqPxShow - show information about a POSIX message queue
# mqShow - support routine to show information about a POSIX message queue
# msgQShow - show information about a message queue
# semPxShow - display POSIX semaphore internals
# semShow - display WIND semaphore internals
# show - main dispatch for shell "show" routines
# smMemPartShow - show statistics about a target shared memory partition usage
# smMemShow - show statistics about target shared memory usage
# sysStatusShow - show system context status
# taskShow - dispatched when vxshow is applied to a task object,
# version - print various version strings.
# wdShow - show information about a watchdog.  Can be dispatched by the
#
# This module also contains windsh private routines used by the
# "show" shellprocs
#

# globals

# Associative array mapping class Ids to object names.  Filled in by
# initClassNames.
set className() 0

#
# List of names of task options. The first entry corresponds to the
# least-significant bit of the task options word, and subsequent entries
# represent more significant bits.
#
set taskOptionNames {
	VX_SUPERVISOR_MODE
	VX_UNBREAKABLE
	VX_DEALLOC_STACK
	VX_FP_TASK
	VX_STDIO
	VX_ADA_DEBUG
        VX_FORTRAN
	VX_PRIVATE_ENV
	VX_NO_STACK_FILL
	VX_DSP_TASK
        VX_ALTIVEC_TASK
    }
set taskOptMax [llength $taskOptionNames]

# C++ SUPPORT: Initialize demangling style.
if {! [info exists shDemangleStyle]} {
    set tool [targetTool]
    
    if {[string first gnu $tool] != -1} {
	set shDemangleStyle gnu
    } elseif {[string first diab $tool] != -1} {
	set shDemangleStyle diab
    } else {
	# default to gnu
	set shDemangleStyle gnu
    }
}

# C++ SUPPORT: Initialize demangling mode to full information.
if {! [info exists shDemangleMode]} {
    set shDemangleMode 2
}

##############################################################################
#
# agentModeShow - show agent mode 
#
# This command shows the running mode of the agent.
#
# SYNOPSIS:
# agentModeShow
#
# RETURNS:
# OK always.
#

shellproc agentModeShow {args} {
    switch [wtxAgentModeGet] {
   	"AGENT_MODE_EXTERN" {
	    puts stdout "Agent is running in system mode"
	}
   	"AGENT_MODE_TASK" {
	    puts stdout "Agent is running in task mode"
	}
	default {
	    puts stdout "Unknown agent mode"
	}
    }
    return 0
}

##############################################################################
#
# browse - send a message to the browser asking it to browse an address
#
# SYNOPSIS:
#   browse objId
#
# PARAMETERS:
#   objId: the address of an object to browse
#
# RETURNS:
#   NONE
#
# ERRORS:
#   NONE

shellproc browse {args} {
    bindNamesToList {objId} $args
    wtxEventAdd "Browser browse $objId"
    return 0
}

##############################################################################
#
# checkStack - print stack consumption information for active tasks.
#
# This procedure displays a summary of stack usage for all tasks if no argument
# is given.
# The summary includes the total stack size (SIZE), the current number of stack
# bytes used (CUR), the maximum number of stack bytes used (HIGH), and the
# number of bytes never used at the top of the stack (MARGIN = SIZE - HIGH).
#
# SYNOPSIS:
#   checkStack tid
#
# PARAMETERS:
# tid: a task ID or 0 if all tasks have to be displayed.
#
# RETURNS: N/A
#
# ERRORS: N/A
#
# INTERNAL
# checkStack allows for more than one parameter (up to 10). This feature is
# undocumented for now.
#

shellproc checkStack {args} {
    global stackGrows

    puts stdout "  NAME        ENTRY        TID     SIZE   CUR  HIGH  MARGIN"
    puts stdout "------------ ------------ -------- ----- ----- ----- ------"

    # Get an empty list if checkStack is requested for all tasks or the list
    # of tasks for which checkStack is requested (list size <= 10)

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

    foreach taskCouple $aList {
        set task [lindex $taskCouple 0]

        # If a list of tasks has been specified and if the current task is not
        # in this list, jump to next task.

        if {([llength $taskList] > 0) && ([lsearch $taskList $task] == -1)} {
            continue
        }

        set stkInfo [taskStackInfo $task]

        bindNamesToList {tid name entry sp base limit max} $stkInfo
        set symaddr [shSymbolicAddr -exact $entry]

        if {$stackGrows == "down"} {
            set size [expr $base - $limit]
            set cur [expr $base - $sp]
            if {$max} {
                set high [format "%5d" [expr $base - $max]]
                set margin [format "%6d" [expr $max - $limit]]
                set msg ""
            } else {
                set high "  ???"
                set margin "   ???"
                set msg "VX_NO_STACK_FILL"
            }
        } else {
            set size [expr $limit - $base]
            set cur [expr $sp - $base]
            if {$max} {
                set high [format "%5d" [expr $max - $base]]
                set margin [format "%6d" [expr $limit - $max]]
                set msg ""
            } else {
                set high "  ???"
                set margin "   ???"
                set msg "VX_NO_STACK_FILL"
            }
        }

        puts stdout [format "%-12.12s %-12.12s %8x %5d %5d %5s %6s %s" \
                $name $symaddr $tid $size $cur $high $margin $msg]
    }
    return 0
}

##############################################################################
#
# classShow - show information about a class of objects
#

shellproc classShow {args} {
    global className

    bindNamesToList {classId verbose} $args

    # Check object validity

    if {[objectClassify $classId] == 0} {
	error "$classId is not a known object"
    }

    # Get information about class

    set classInfoList [classInfoGet $classId]

    bindNamesToList \
	{partId size alloc free init term} \
	$classInfoList

    puts stdout [format "%-20s: %s" Class $className($classId)]
    puts stdout [format "%-20s: 0x%-10x" {Memory Partition Id} $partId]
    puts stdout [format "%-20s: %-10d" {Object Size} $size]
    puts stdout [format "%-20s: %-10d" {Objects Allocated} $alloc]
    puts stdout [format "%-20s: %-10d" {Objects Deallocated} $free]
    puts stdout [format "%-20s: %-10d" {Objects Initialized} $init]
    puts stdout [format "%-20s: %-10d" {Objects Terminated} $term]

    puts stdout ""

    # Set list of class methods

    set methods "Create Init Destroy Show Help"

    # Set offset in class information 

    set offset 6

    # Loop thru array of method

    foreach method $methods {
	set routineAdrs [lindex $classInfoList $offset]
	if [catch "wtxSymFind -value $routineAdrs" routineSym] {
	    if {[wtxErrorName $routineSym] == "SVR_INVALID_FIND_REQUEST"} {
		set routineName "No routine attached"
	    } else {
		set routineName "error: [wtxErrorName $routineSym]"
	    }
	} else {
	    set routineName [lindex $routineSym 0]
	}
	puts stdout [format "%-20s: %#-10x (%s)" "$method Routine" \
	    $routineAdrs $routineName]

	incr offset
    }

    return 0
}

##############################################################################
#
# devs - an alias for iosDevShow
#

shellproc devs {args} {
    return [[shellName iosDevShow]]
}

##############################################################################
#
# i - summarize TCB information for all tasks in the active queue.
#

shellproc i {args} {
    global offset
    global iCommandGopher

    # get an empty list if i is requested for all tasks or the list
    # of tasks for which i is requested (list size <= 10)

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

    taskPrintBanner

    if {[llength $taskList] > 0} {
	foreach taskCouple $aList {
	    set task [lindex $taskCouple 0]
	    if {[lsearch $taskList $task] != -1} {
		set taskInfo [taskInfoGet $task]
		set vxClock [lindex $taskInfo 0]
		# XXX validate it's really a task
		taskInfoPrint $vxClock [lrange $taskInfo 1 end]
	    }
	}
    } {
	foreach taskCouple $aList {
	    set task [lindex $taskCouple 0]
	    set taskInfo [taskInfoGet $task]
	    set vxClock [lindex $taskInfo 0]
	    # XXX validate it's really a task
	    taskInfoPrint $vxClock [lrange $taskInfo 1 end]
	}
    }

    if {[wtxAgentModeGet] == "AGENT_MODE_EXTERN"} {
	puts stdout "\nAgent mode     : Extern"
	if ![catch "wtxContextStatusGet CONTEXT_SYSTEM 0" result] {
	    if {$result == "CONTEXT_SUSPENDED"} {
		puts stdout "System context : Suspended"
	    } else {
		puts stdout "System context : Running"
	    }	
	}
    }
    return 0
}

##############################################################################
#
# intVecShow - show information about a given interrupt vector
#

shellproc intVecShow {args} {

    bindNamesToList {vector} $args

    source [wtxPath host resource tcl]intVecTbl.tcl

    set funcList [intRtnGet 0 $vector]
    if {$funcList == -1} {
    	error "$vector is not a valid vector number"
    }

    set vector [format "%d" $vector]

    if {[llength $funcList] > 1} {
	puts stdout "Vec Function    Vec Function    Vec Function    Vec Function    Vec Function"
	puts stdout "--- ----------- --- ----------- --- ----------- --- ----------- --- -----------"

	set vector 0
	while {[llength $funcList] > 0} {
	    set list [lrange $funcList 0 4]
	    set funcList [lrange $funcList 5 end]
	    foreach func $list {
	    	puts stdout "[format "%3d %-12.11s" $vector $func]" nonewline
		incr vector
	    }
	    puts stdout "\n" nonewline
	}

    } else {
	# There is only one vector to print
	puts stdout "Vec Function"
	puts stdout "--- -----------"
	puts stdout [format "%3d %s" $vector [lindex $funcList 0]]
    }
}

##############################################################################
#
# iStrict - summarize TCB information for all tasks in the active queue.
#
# This version gets all its information in one big gulp, and is inteded for
# situations where taking an absolutely accurate snapshot is essential.
#

shellproc iStrict {args} {
    global offset
    global iCommandGopher
    set taskInfoList [taskInfoGetAll]
    taskPrintBanner
    set vxClock [lindex $taskInfoList 0]
    set taskInfoList [lrange $taskInfoList 1 end]

    # Now the task info list contains groups of values, each representing
    # a task.  So that they may be sorted, reform this list into a list
    # of lists: each element of the outer list will be the list of values
    # for one task.  That is, we have "v0 v1 ... vn" and we want
    # "{v0 ... vk} {vk+1 ... v2k+1} ... {vn-k ... vn}" where k is the
    # number of items in taskInfoList belonging to each task, and
    # n+1 = (the number of tasks) * k.

    set taskList ""
    while {[llength $taskInfoList] >= 10} {
	set taskList [lappend taskList [lrange $taskInfoList 0 9]]
	set taskInfoList [lrange $taskInfoList 10 end]
    }

    set taskList [lsort -command taskInfoOrder $taskList]

    foreach tl $taskList {
	taskInfoPrint $vxClock $tl
    }
    return 0
}

##############################################################################
#
# iosDevShow - show all devices known to the I/O system: their names
# and driver numbers.
#

shellproc iosDevShow {args} {
    global offset
    set devList [wtxGopherEval "[shSymAddr iosDvList] * {
				<+$offset(DEV_HDR,drvNum) @w>
				<+$offset(DEV_HDR,name) *$>
			    * }"]

    puts stdout "drv name"
    while {[llength $devList] >= 2} {
	set drvNum [lindex $devList 0]
	puts stdout [format "%3d %s" \
	    $drvNum                  \
	    [lindex $devList 1]]
	set devList [lrange $devList 2 end]
    }
    return 0
}

##############################################################################
#
# iosDrvShow - show the addresses of the I/O functions for each I/O
# system driver in the kernel.
#

shellproc iosDrvShow {args} {

    puts stdout [format "%3s %9s  %9s  %9s  %9s  %9s  %9s  %9s" \
        "drv" "create" "delete" "open" "close" "read" "write" "ioctl"]

    set maxDrivers [wtxGopherEval "[shSymAddr maxDrivers] @"]
    set maxDrvLess1 [expr $maxDrivers - 1]
    set drvTable [wtxGopherEval "[shSymAddr drvTable] * +0x20
                             ($maxDrvLess1 @@@@@@@@)"]
    set drvIx 1
    while {[llength $drvTable] >= 8} {
	if {[lindex $drvTable 7]} {
	    puts stdout [format "%3d %9x  %9x  %9x  %9x  %9x  %9x  %9x" \
		$drvIx			\
		[lindex $drvTable 0]	\
		[lindex $drvTable 1]	\
		[lindex $drvTable 2]	\
		[lindex $drvTable 3]	\
		[lindex $drvTable 4]	\
		[lindex $drvTable 5]	\
		[lindex $drvTable 6]]
	}
	set drvTable [lrange $drvTable 8 end]
	incr drvIx
    }
    return 0
}

##############################################################################
#
# iosFdShow - show all existing file descriptors
#
# Mutual exclusion note: This procedure is accomplished in two steps.
# First, a vector of booleans is obtained from the fdTable to find
# out which entries are valid.  Then data is collected for each fd marked
# as valid in the fdTable.
#
# Difference from the shell impelmentation: fds are not marked "in", "out"
# and "err" if they are bound to stdio devices as the target shell version
# of this command would do.
#

shellproc iosFdShow {args} {
    global __wtxTsInfo
    global offset
    global sizeof

    if {[lsearch -exact [lindex $__wtxTsInfo 0] RPC] != -1} {
	set mxGLength 1444
    } elseif {[lsearch -exact [lindex $__wtxTsInfo 0] serial] != -1} {
	set mxGLength 950
    } else {
	set mxGLength 444
    }
    
    #
    # maxGFiles is the maximum number of FD_ENTRY 'inuse' 
    # bool field values that 
    # can be retrieved from the target in one gopher request. 
    # maxGFiles > 2800 may result in a gopher truncation error
    #
    
    set maxGFiles 2700
    
    puts stdout [format "%3s %-20s %3s" "fd" "name" "drv"]
    
    set maxFiles [expr [wtxGopherEval "[shSymAddr maxFiles] @"]]
    set fdTable [wtxGopherEval "[shSymAddr fdTable] @"]

    set fdValid ""
    set fd 0
    
    #
    # Construct a gopher string to retrieve the value of 
    # the 'inuse' bool field from each FD_ENTRY.
    # 

    for {set fd 0} {$fd<$maxFiles} {incr fd $maxGFiles} {
       set fdEntry [expr $fdTable + $fd * $sizeof(FD_ENTRY)]
       set fdGEntry "[format " %#x" $fdEntry]"
       
       if { [expr $fd + $maxGFiles] > $maxFiles } {
           set maxGFiles [expr $maxFiles - $fd]
       }

       append fdValid " [wtxGopherEval "$fdGEntry ($maxGFiles
				<+$offset(FD_ENTRY,inuse) @>
				+$sizeof(FD_ENTRY)
			    )"]"
    }

    #
    # Construct a gopher string that will collect data for each valid
    # fd.  Iterate over the vector of validity bits collected above,
    # and append a gopher string that will pick out the information for
    # that fd.
    #
 
    set fragment \
    "<+$offset(FD_ENTRY,name) *$> <+$offset(FD_ENTRY,pDevHdr) *
     +$offset(DEV_HDR,drvNum) @w>"

    set fdGopher ""
    set fdInfo ""
    set fdGLength 0
    for {set fd 0} {$fd<$maxFiles} {incr fd} {
	if {[lindex $fdValid $fd] != 0} {
	    set fdEntry [expr $fdTable + $fd * $sizeof(FD_ENTRY)]
	    set fdGEntry "[format " %#x" $fdEntry] $fragment"
	    set fdGEntryLength [string length $fdGEntry]
	    if {[incr fdGLength $fdGEntryLength] >= $mxGLength} {
		append fdInfo " [wtxGopherEval $fdGopher]"
		set fdGopher $fdGEntry
		set fdGLength $fdGEntryLength
	    } else {
		append fdGopher " $fdGEntry"
	    }
        }
    }

    append fdInfo " [wtxGopherEval $fdGopher]"

    for {set fd 0} {$fd<$maxFiles} {incr fd} {
	if {[lindex $fdValid $fd] != 0} {
	    puts stdout [format "%3d %-20s %3d" \
		    [expr $fd+3] [lindex $fdInfo 0] [lindex $fdInfo 1]]
	    set fdInfo [lrange $fdInfo 2 end]
	}
    }
    return 0
}

##############################################################################
#
# memPartShow - show statistics about a target memory partition usage
#
# SYNOPSIS:
#   memPartShow memPartId flag
#
# PARAMETERS:
#   memPartId: the ID of the memory partition.
#   flag: either 0, for short form, or 1 for more details.
#
# RETURNS:
#
# ERRORS:
#

shellproc memPartShow {args} {

    if {[lindex $args 0] == "0x0"} {
	puts stdout "No partId specified."
	return -1
	}


    bindNamesToList {memPartId verbose} $args

    if {$memPartId == 0} {
	puts stdout "No partId specified."
	return -1
    }

    # if this is a shared memory partition, call the appropriate routine.

    if {[objIsShared $memPartId]} {
	[shellName smMemPartShow] [smObjIdToAdrs $memPartId] $verbose
	return 0
    }

    set memPartInfo [memPartInfoGet $memPartId]
    bindNamesToList {totWords curBlks curWds cumBlks cumWds} $memPartInfo

    if {$verbose >= 1} {
	puts stdout "\nFREE LIST:"
	puts stdout "  num     addr      size"
	puts stdout "  --- ---------- ----------"
    }

    set biggestBytes 0
    set totalBytes 0
    set ix 0

    set freeList [lrange $memPartInfo 5 end]

    # The size member of a block is a bit field: 31 bits for the size,
    # and 1 bit for the `free' status.  On big-endian machines the size
    # bit is the least significant; on little-endian machines, the most
    # signficant.  After the masking on LE architectures, we must multiply 
    # the residue by two to get the number of bytes in the block.

    if {[targetEndian] == "BIG_ENDIAN"} {
	set freeMask 1
	set wMult 1
    } {
	set freeMask 0x80000000
	set wMult 2
    }

    while {[llength $freeList] >= 2} {
	set nBytes [expr $wMult * ([lindex $freeList 1] & ~$freeMask)]
	set addr [lindex $freeList 0]

	incr totalBytes $nBytes
	if {$nBytes > $biggestBytes} {
	    set biggestBytes $nBytes
	}
	if {$verbose >= 1} {
	    puts stdout [format "  %3d %#10x %10u" \
		    $ix $addr $nBytes]
	    flush stdout
	}
	incr ix
	set freeList [lrange $freeList 2 end]
    }

    memSummaryPrint $ix $totalBytes $biggestBytes $curBlks $curWds $cumBlks \
		    $cumWds

    return 0
}

##############################################################################
#
# memShow - show statistics about target memory usage
#
# SYNOPSIS:
#   memShow flag
#
# PARAMETERS:
#   flag: either 0, for short form, or 1 for more details.
#
# RETURNS:
#
# ERRORS:
#

shellproc memShow {args} {
    bindNamesToList {verbose} $args

    return [[shellName memPartShow] \
	    [wtxGopherEval "[shSymAddr "memSysPartId"] @"] \
	    $verbose]
}

##############################################################################
#
# moduleIdFigure - figure out module ID, given name or number
#
# The list of module IDs known to the target server is searched for
# the given number; if found, it is returned.  Otherwise, if the given
# address points to a string that matches an existing module name,
# that module's ID is returned.  If all this fails, -1 is returned.
#
# SYNOPSIS:
#   moduleIdFigure modNameOrId
#
# PARAMETERS:
#   nameOrId - string name, or module ID itself
#
# RETURNS:
#   a module ID, or 0
#
# ERRORS:
#   NONE

shellproc moduleIdFigure {args} {
    set mod [lindex $args 0]

    set knownModList [wtxObjModuleList]

    if {[lsearch $knownModList $mod] != -1} {return $mod}

    set str [stringGet $mod]

    foreach knownMod $knownModList {
	set kModName [lindex [wtxObjModuleFind $knownMod] 1]
	if {$kModName == $str} {
	    return $knownMod
	}
    }

    # Utter failure.
    return 0
}

##############################################################################
#
# moduleShow - show information about one module (or all of them)
#
# SYNOPSIS:
#   moduleShow [modId]
#
# PARAMETERS:
#   modId:	ID of module to show information about.  If 0, all modules
#               will be used.
#

shellproc moduleShow {args} {
    bindNamesToList {modId} $args

    puts stdout \
"\nMODULE NAME              MODULE ID  GROUP #    TEXT START DATA START  BSS START"
    puts stdout \
"------------------------ ---------- ---------- ---------- ---------- ----------"

    if {$modId == 0} {
	set moduelList [wtxObjModuleList]
	foreach mod $moduelList {
	    modShow $mod
	}
    } else {
	set mod [[shellName moduleIdFigure] $modId]
	if {$mod} {
	    modShow $mod
	} else {
	    puts stdout "Module not found."
	    return -1
	}
    }
    return 0
}

##############################################################################
#
# mqPxShow - show information about a POSIX message queue
#
# Note, because the POSIX message queue class name is mq rather than mqPx, we
# must have mqShow{} as the support routine of mqPxShow{}. mqShow{} is called
# internally when show{} is used, whereas mqPxShow{} may be called directly by
# the user.
#

shellproc mqPxShow {args} {
    bindNamesToList {mqPxId verbose} $args
    [shellName mqShow] $mqPxId $verbose
}

##############################################################################
#
# mqShow - support routine to show information about a POSIX message queue
#

shellproc mqShow {args} {
    global offset

    # verbose mode is ignored for now.

    bindNamesToList {mqPxId verbose} $args

    # Get message queue structure's field contents.

    set mqPxInfoList [mqPxInfoGet $mqPxId]

    # Isolate the list of senders and the list of receivers. Note that these
    # list are not used here (we follow vxWorks' behavior).

    set sentinelIx [lsearch $mqPxInfoList 0xee]
    set mqPxInfoList [lrange $mqPxInfoList [expr $sentinelIx + 1] end]
    set senderList [lrange $mqPxInfoList 0 [expr $sentinelIx - 1]]
    set sentinelIx [lsearch $mqPxInfoList 0xee]
    set receiverList [lrange $mqPxInfoList 0 [expr $sentinelIx - 1]]
    set mqPxInfoList [lrange $mqPxInfoList [expr $sentinelIx + 1] end]

    # pick out the static members

    bindNamesToList {msgNameAddr currentMsg maxMsg msgSize flags} $mqPxInfoList

    if {$msgNameAddr != 0} {
	set name [wtxGopherEval "$msgNameAddr $"]
	puts stdout [format "%-32s: %s" {Message queue name} $name]
    } else {
	puts stdout [format "%-32s: %s" {Message queue name} "<NONE>"]
    }

    puts stdout [format "%-32s: %-10d" {No. of messages in queue} $currentMsg]
    puts stdout [format "%-32s: %-10d" {Maximum no. of messages} $maxMsg]
    puts stdout [format "%-32s: %-10d" {Maximum message size} $msgSize]

    return 0
}

##############################################################################
#
# msgQShow - show information about a message queue
#
# XXX use of 0xee as a sentinel here is suspect.
#

shellproc msgQShow {args} {
    global offset
    global sizeof
    bindNamesToList {msgQId verbose} $args

    # Check object validity if Shared Object.

    if {[objIsShared $msgQId]} {
	if {[objIsDistributed $msgQId]} {
		set pO [expr $msgQId & ~0x3]
		set dObjId [wtxGopherEval "$pO <+$offset(DIST_OBJ_NODE,objNodeId) @>"]
		puts stdout [format "Message Queue Id    : 0x%-10x" $msgQId]
		if {[expr $dObjId & 1]} {
			puts stdout [format "Global unique Id    : 0x%-10x" $dObjId]
			puts stdout [format "Type                : group"]
			return 0
		} else {
			set node [wtxGopherEval "$pO <+$offset(DIST_OBJ_NODE,objNodeReside) @>"]
			puts stdout [format "Global unique Id    : 0x%08x:%-10x" $node $dObjId]
			set id [expr $dObjId >> 1]
			set pThisNode [expr [lindex [wtxSymFind -name distNodeLocalId] 1]]
			set thisNode [wtxGopherEval "$pThisNode <@>"]
			if {$node != $thisNode} {
			puts stdout [format "Type                : remote queue"]
			puts stdout [format "Home Node           : 0x%08x" $node]
			return 0
			} else {
			puts stdout [format "Type                : queue"]
			puts stdout [format "Home Node           : 0x%08x" $node]
			set pTable [expr [lindex [wtxSymFind -name pMsgQDistTbl] 1]]
			set tblBase [wtxGopherEval "$pTable <@>"]
			set pNode [expr ($tblBase + ($sizeof(TBL_NODE) * $id))]
			set msgQId [wtxGopherEval "$pNode <+$offset(TBL_NODE,tblMsgQId) @>"]
			puts stdout [format "Mapped to           : 0x%-10x\n" $msgQId]
			}
		}
    } else {
	    set smObjId [smObjIdToAdrs $msgQId]
	    if {$smObjId == 0} {
	        error "$msgQId is not a known object"
	    }
	    if {! [smObjVerify $smObjId]} {
	        error "Invalid message queue id: $msgQId"
	    }
    }
    }

    set msgQInfoList [msgQInfoGet $msgQId]

    #
    # Remove the static parts of the msgQInfoList (the first nine elements
    # used to print the above information).
    #

    if {[objIsShared $msgQId]} {
	set msgBaseList [lrange $msgQInfoList 0 5]
	set msgQInfoList [lrange $msgQInfoList 6 end]
    } {
	set msgBaseList [lrange $msgQInfoList 0 8]
	set msgQInfoList [lrange $msgQInfoList 9 end]
    }

    #
    # After this will be three lists
    # (any of which may be empty, and delimited by the sentinel value "0xee":
    # the list of blocked receivers, the list of blocked senders, and the
    # list of messages.
    #

    set sentinelIx [lsearch $msgQInfoList 0xee]
    set senderList [lrange $msgQInfoList 0 [expr $sentinelIx - 1]]
    set msgQInfoList [lrange $msgQInfoList [expr $sentinelIx + 1] end]
    set sentinelIx [lsearch $msgQInfoList 0xee]
    set receiverList [lrange $msgQInfoList 0 [expr $sentinelIx - 1]]
    set msgQInfoList [lrange $msgQInfoList [expr $sentinelIx + 1] end]

    set vxClock [lindex $msgBaseList 0]
    set msgCount [expr [llength $msgQInfoList] / 2]
    set maxMsgs [lindex $msgBaseList 2]
    set blockType [expr {$msgCount == $maxMsgs ? "Senders" : "Receivers"}]

    if {"$blockType" == "Senders"} {
	set blockList $senderList
    } {
	set blockList $receiverList
    }

    set blockCount [llength $blockList]

    puts stdout [format "%-20s: 0x%-10x" {Message Queue Id} $msgQId]
    puts stdout [format "%-20s: %-10s"   {Task Queueing} \
					 [msgQType [lindex $msgBaseList 1]]]
    puts stdout [format "%-20s: %-10d"   {Message Byte Len} \
					 [lindex $msgBaseList 3]]
    puts stdout [format "%-20s: %-10d"   {Messages Max} \
					 $maxMsgs]
    puts stdout [format "%-20s: %-10d"   {Messages Queued} \
					 $msgCount]
    puts stdout [format "%-20s: %-10d"   "$blockType Blocked" \
					 $blockCount]
    puts stdout [format "%-20s: %-10d"   {Send Timeouts} \
					 [lindex $msgBaseList 4]]
    puts stdout [format "%-20s: %-10d"   {Receive Timeouts} \
					 [lindex $msgBaseList 5]]

    # generate and display options string only for local message queues

    if {![objIsShared $msgQId]} {

	set options [lindex $msgBaseList 1]

	if {($options & 0x1) == 0} {
	    set optionsString "MSG_Q_FIFO\n"
	} {
	    set optionsString "MSG_Q_PRIORITY\n"
	}

	if {($options & 0x2) == 0x2} {
	    set optionsString [format "%s%s" $optionsString \
			     "\t\t\t\tMSG_Q_EVENTSEND_ERR_NOTIFY\n"]
	}

	puts stdout [format "%-20s: 0x%x\t%s" {Options} $options $optionsString]

	# display VxWorks events information for local semaphores 

	eventRsrcShow [lindex $msgBaseList 6] [lindex $msgBaseList 7] \
		      [lindex $msgBaseList 8]
    } {
	puts stdout ""
    }

    # if not verbose return

    if {! $verbose} {return 0}

    if {[llength $senderList] > 0} {
	puts stdout "Senders Blocked:"
	puts stdout ""
	if {[objIsShared $msgQId]} {
	    taskPrintShortBanner shared

	    foreach bTask $senderList {
		smBlockedTaskInfoPrint $bTask [smTaskBlockInfo $bTask]
	    }
	} else {
	    taskPrintShortBanner local

	    foreach bTask $senderList {
		blockedTaskInfoPrint $vxClock $bTask [taskBlockInfo $bTask]
	    }
	}
	puts stdout ""
    }

    if {[llength $receiverList] > 0} {
	puts stdout "Receivers Blocked:"
	puts stdout ""
	if {[objIsShared $msgQId]} {
	    taskPrintShortBanner shared

	    foreach bTask $receiverList {
		smBlockedTaskInfoPrint $bTask [smTaskBlockInfo $bTask]
	    }
	} else {
	    taskPrintShortBanner local

	    foreach bTask $receiverList {
		blockedTaskInfoPrint $vxClock $bTask [taskBlockInfo $bTask]
	    }
	}
	puts stdout ""
    }

    if {[llength $msgQInfoList] > 0} {

	puts stdout "Messages queued:"
	if {[objIsShared $msgQId]} {
	    puts stdout "  # local adrs  len  value"
	} else {
	    puts stdout "  # address     len  value"
	}
	set msgIx 1
	while {[llength $msgQInfoList] >= 2} {
	    set msgAdrs [lindex $msgQInfoList 0]
	    set msgLen  [lindex $msgQInfoList 1]
	    puts stdout [format "%3d %#x %5d  %s" \
		$msgIx $msgAdrs $msgLen [msgGet $msgAdrs $msgLen]]
	    incr msgIx
	    set msgQInfoList [lrange $msgQInfoList 2 end]
	}
	puts stdout ""
    }
    return 0
}

##############################################################################
#
# rBuffShow - show information about an rBuff
#
# XXX use of 0xee as a sentinel here is suspect.
#

shellproc rBuffShow {args} {

    bindNamesToList {rBuff verbose} $args

    if {$rBuff == 0} {
        puts stdout [format "rBuffShow: NULL ptr supplied"]
        return 0
    }

    set rBuffInfoList [rBuffInfoGet $rBuff]

    bindNamesToList {buffRead buffWrite srcPart options buffSize currBuffs threshold minBuffs maxBuffs maxBuffsActual emptyBuffs dataContent writesSinceReset readsSinceReset timesExtended timesXThreshold bytesWritten bytesRead bytesPeak} $rBuffInfoList

    set percent [expr ($dataContent * 100) / ($buffSize * $currBuffs)]

    set aveW [expr {$writesSinceReset == 0 ? 0 : $bytesWritten / $writesSinceReset}]
    set aveR [expr {$readsSinceReset == 0 ? 0 : $bytesRead / $readsSinceReset}]

    puts stdout [format "\nSummary of rBuff id: 0x%-08x :" $rBuff]
    puts stdout [format "---------------------------------"]

    puts stdout [format "%-20s : %08x" "options" $options]
    puts stdout [format "%-20s : 0x%x" "buffer size" $buffSize]

    set msg [format "%-20s : %d" "curr no. of buffs" $currBuffs]

    if {$currBuffs == $minBuffs} {
        set msg [concat $msg {(min)}]
    } elseif {$currBuffs == $maxBuffs} {
        set msg [concat $msg {(max)}]
    }
    puts stdout $msg

    puts stdout [format "%-20s : %d / %d / %d" "min/actualMax/max" $minBuffs $maxBuffsActual $maxBuffs]
    puts stdout [format "%-20s : %d" "emptyBuffs" $emptyBuffs]
    puts stdout [format "%-20s : %d" "times extended" $timesExtended]
    puts stdout [format "%-20s : 0x%08x" "curr buff to read" $buffRead]
    puts stdout [format "%-20s : 0x%08x" "curr buff to write" $buffWrite]

    set msg [format "%-20s : 0x%x (0x%x)" "threshold (X'd)" $threshold $timesXThreshold]

    if {$dataContent > $threshold} {
        set msg [concat $msg {*}]
    }

    puts stdout $msg

    puts stdout [format "%-20s : 0x%x (%d%%)" "data content" $dataContent $percent]
    puts stdout [format "%-20s : 0x%x" "peak content" $bytesPeak]
    puts stdout [format "%-20s : %d / %d" "access count w / r" $writesSinceReset $readsSinceReset]
    puts stdout [format "%-20s : %d / %d" "bytes total  w / r" $bytesWritten $bytesRead]
    puts stdout [format "%-20s : %d / %d" "average      w / r" $aveW $aveR]


    if {$verbose == 1} {

        puts stdout [format "\n- traversing ring of buffers -"]

        set buffCount 1
        set backHome $buffRead

        set buffDetails [wtxGopherEval "$buffRead @@@@@"]
        bindNamesToList {next space dataStart dataEnd dataLen} $buffDetails

        set msg [format "%03d: addr: 0x%08x buff: 0x%08x dataLen: 0x%04x spaceAvail: 0x%04x R" \
            $buffCount $buffRead $dataStart $dataLen $space]

        if {$buffRead == $buffWrite} {
            set msg [concat $msg { W}]
        }

        puts stdout $msg

        set currBuff $next

        while {$currBuff != $backHome} {

            incr buffCount

            set buffDetails [wtxGopherEval "$currBuff @@@@@"]
            bindNamesToList {next space dataStart dataEnd dataLen} $buffDetails

            set msg  [format "%03d: addr: 0x%08x buff: 0x%08x dataLen: 0x%04x spaceAvail: 0x%04x" \
                $buffCount $currBuff $dataStart $dataLen $space]

            if {$currBuff == $buffWrite} {
                set msg [concat $msg { W}]
            }

            puts stdout $msg

            set currBuff $next
        }

        puts stdout [format "... and back to 0x%08x" $backHome]
    }

    return 0
}

##############################################################################
#
# semPxShow - display POSIX semaphore internals
#

shellproc semPxShow {semId args} {

    # Get semaphore structure's field contents, plus pended tasks

    set semPxInfoList [semPxInfoGet $semId]
    bindNamesToList {semWindId refCnt sem_name count} $semPxInfoList

    set bTaskList [lrange $semPxInfoList 4 end]
    set pendQCount [llength $bTaskList]

    # Display information

    if {$sem_name != 0} {
	set name [wtxGopherEval "$sem_name $"]
	puts stdout [format "%-32s: %-10s" {Semaphore name} $name]
	puts stdout [format "%-32s: %-10d" {sem_open() count} $refCnt]
    }

    if {($count == 0) && ($pendQCount != 0)} {
	puts stdout [format "%-32s: %-10d" {Semaphore value} 0]
	puts stdout [format "%-32s: %-10d" {No. of blocked tasks} $pendQCount]
    } else {
	puts stdout [format "%-32s: %-10d" {Semaphore value} $count]
    }

    return 0
}

##############################################################################
#
# semShow - display WIND semaphore internals
#

shellproc semShow {semId args} {

    bindNamesToList {verbose} $args

    # Check object validity if Shared Object.

    if {[objIsShared $semId]} {
	set smObjId [smObjIdToAdrs $semId]
	if {$smObjId == 0} {
	    error "$semId is not a known object"
	}
	if {! [smObjVerify $smObjId]} {
	    error "Invalid semaphore id: $semId"
	}
    }

    set semInfoList [semInfoGet $semId]

    if {[objIsShared $semId]} {
	bindNamesToList {vxClock sType options contents} $semInfoList

	set bTaskList [lrange $semInfoList 4 end]

    } {
	bindNamesToList {vxClock sType options contents evRegistered \
			 evTaskId evOptions} $semInfoList

	set bTaskList [lrange $semInfoList 7 end]
    }


    set pendQCount [llength $bTaskList]

    puts stdout [format "%-20s: 0x%-10x" {Semaphore Id} $semId]
    puts stdout [format "%-20s: %-10s"   {Semaphore Type} [semType $sType]]
    puts stdout [format "%-20s: %-10s"   {Task Queueing} \
	[semQueueType $options]]
    puts stdout [format "%-20s: %-10d"   {Pended Tasks} $pendQCount]
    puts stdout [format "%-20s: %-20s" {State} [semContents $sType $contents]]

    # generate semaphore options string (only for local semaphores)

    if {![objIsShared $semId]} {

	if {($options & 0x3) == 0} {
	    set optionsString "SEM_Q_FIFO\n"
	} {
	    set optionsString "SEM_Q_PRIORITY\n"
	}

	# decode SEM_DELETE_SAFE and SEM_INVERSION_SAFE options only for mutex

	if {$sType == 1} {
	    if {($options & 0x4) == 0x4} {
		set optionsString [format "%s%s" $optionsString \
					     "\t\t\t\tSEM_DELETE_SAFE\n"]
	    }

	    if {($options & 0x8) == 0x8} {
		set optionsString [format "%s%s" $optionsString \
					     "\t\t\t\tSEM_INVERSION_SAFE\n"]
	    }
	}

	if {($options & 0x10) == 0x10} {
	    set optionsString [format "%s%s" $optionsString \
			     "\t\t\t\tSEM_EVENTSEND_ERR_NOTIFY\n"]
	}

	puts stdout [format "%-20s: 0x%x\t%s" {Options} $options $optionsString]

	# display VxWorks events information for local semaphores 

	eventRsrcShow $evRegistered $evTaskId $evOptions
    } 

    if {$pendQCount <= 0} return

    # if not verbose return now

    if {! $verbose} return

    if {[objIsShared $semId]} {

	# Shared semaphores only

	puts stdout ""

	taskPrintShortBanner shared
	foreach bTask $bTaskList {
	    smBlockedTaskInfoPrint $bTask [smTaskBlockInfo $bTask]
	}
    } else {

	# Local semaphores only

	puts stdout "Pended Tasks\n------------"

	taskPrintShortBanner local
	foreach bTask $bTaskList {
	    blockedTaskInfoPrint $vxClock $bTask [taskBlockInfo $bTask]
	}
    }

    return 0
}

##############################################################################
#
# show - main dispatch for shell "show" routines
#
# The class of the object is determined.  If there is a routine in existence
# with the name <class>Show, this routine is called with the same arguments.
#
# SYNOPSIS:
#   show object [verbose]
#
# PARAMETERS:
#   object: the object ID to show
#   verbose: a nonnegative number, indicating the verbosity level
#
# RETURNS:
#   NONE
#
# ERRORS:
#   "$object is not a known object" if objectClassify cannot figure out class
#   "No show routine is implemented for $class objects" if there is no
#      procedure called <class>Show in the interpreter.

shellproc show {object args} {
    set verbose 0

    if {[llength $args] > 0} {
        set verbose [lindex $args 0]
    }

    set class [objectClassify $object]
    if {$class == 0} \
        {error "$object is not a known object"}

    set showRoutine ${class}Show
    if {[info procs [shellName $showRoutine]] == ""} \
        {error "No show routine is implemented for $class objects"}

    puts stdout ""
    set result [[shellName $showRoutine] $object $verbose]
    puts stdout ""
    return $result
}

##############################################################################
#
# smMemPartShow - show statistics about a target shared memory partition usage
#
# SYNOPSIS:
#   smMemPartShow smMemPartId flag
#
# PARAMETERS:
#   smMemPartId: the ID of the shared memory partition.
#   flag: either 0, for short form, or 1 for more details.
#
# RETURNS:
#
# ERRORS:
#

shellproc smMemPartShow {args} {

    bindNamesToList {smMemPartId verbose} $args

    if {! [smObjVerify $smMemPartId]} {
	error "Invalid memory partition id: $smMemPartId"
    }

    # XXX we don't lock the shared memory access. This can be dangerous...

    set smMemPartInfo [smMemPartInfoGet $smMemPartId]

    bindNamesToList {totWords curBlks curWds cumBlks cumWds} $smMemPartInfo

    if {$verbose >= 1} {
	puts stdout "\nFREE LIST:"
	puts stdout "  num     addr      size"
	puts stdout "  --- ---------- ----------"
    }

    set biggestBytes 0
    set totalBytes 0
    set ix 0

    set freeList [lrange $smMemPartInfo 5 end]

    while {[llength $freeList] >= 2} {
	set nBytes [expr 2 * [lindex $freeList 1]]
	set addr [lindex $freeList 0]

	incr totalBytes $nBytes
	if {$nBytes > $biggestBytes} {
	    set biggestBytes $nBytes
	}
	if {$verbose >= 1} {
	    puts stdout [format "  %3d %#10x %10d" \
		    $ix [locToGlobAdrs $addr] $nBytes]
	    flush stdout
	}
	incr ix
	set freeList [lrange $freeList 2 end]
    }

    memSummaryPrint $ix $totalBytes $biggestBytes $curBlks $curWds $cumBlks \
		    $cumWds

    return 0
}

##############################################################################
#
# smMemShow - show statistics about target shared memory usage
#
# SYNOPSIS:
#   smMemShow flag
#
# PARAMETERS:
#   flag: either 0, for short form, or 1 for more details.
#
# RETURNS:
#
# ERRORS:
#

shellproc smMemShow {args} {
    global smObjHdrAdrs
    global offset

    bindNamesToList {verbose} $args

    # Initialize Shared Object information.

    if {[smObjInfoInit] == -1} {
	error "VxMP component not installed"
    }

    # Get ID of system shared partition.

    set smSystemPartId [wtxGopherEval "$smObjHdrAdrs
					+$offset(SM_OBJ_MEM_HDR,smSysPart) !"]

    return [[shellName smMemPartShow] $smSystemPartId $verbose]
}

##############################################################################
#
# sysStatusShow - show system context status
#
# This command shows the status of the system context. The system context
# can be running or suspended. This command can be completed succesfully 
# only if the agent is running in system mode.
#
# SYNOPSIS:
# sysStatusShow
#
# RETURNS:
# OK or ERROR if agent is running in task mode or if wtxContextStatusGet
# WTX request fails.
#

shellproc sysStatusShow {args} {
    if {[wtxAgentModeGet] == "AGENT_MODE_TASK"} {
	error "Cannot get status of system context when agent is in task mode"
    }
    if [catch {wtxContextStatusGet CONTEXT_SYSTEM 0} result] {
	error "Cannot get status of system context: $result"
    }
    switch $result {
   	"CONTEXT_RUNNING" {
	    puts stdout "System context is running"
	}
   	"CONTEXT_SUSPENDED" {
	    puts stdout "System context is suspended"
	}
	default {
	    puts stdout "Unknown system context status : $result"
	}
    }
    return 0
}

##############################################################################
#
# taskShow - dispatched when vxshow is applied to a task object,
# and by the vxti command.  May also be called directly, with a
# taskId argument and a verbosity level.
#

shellproc taskShow {args} {
    global stackGrows

    bindNamesToList {taskId verbose} $args

    if {$verbose == 2} {
	#
	# high verbosity level: just do the i command
	#
	[shellName i]	
	return 0
    }

    if {$verbose == 0} {
	#
	# low verbosity level: just print one row of the "i" command.
	#
    	taskPrintBanner
	set taskInfoList [taskInfoGet $taskId]
	taskInfoPrint [lindex $taskInfoList 0] [lrange $taskInfoList 1 end]
	return 0
    }

    #
    # moderate verbosity: like level 0, but also print the registers,
    # stack, and option information
    #

    taskPrintBanner
    set taskInfoList [taskInfoGetVerbose $taskId]

    taskInfoPrint [lindex $taskInfoList 0] [lrange $taskInfoList 1 10]

    bindNamesToList {vxClock taskId status priority sp pc errno timeout	\
                     entry priNormal name options pStackBase pStackEnd	\
		     pStackLimit pStackMax eventsWanted eventsReceived	\
		     eventsOptions eventsSysflags}			\
        $taskInfoList

    if {$stackGrows == "down"} {
	set stackSize [expr $pStackBase - $pStackLimit]
    } {
	set stackSize [expr $pStackLimit - $pStackBase]
    }
	
    if {$pStackMax == 0} {
	# VX_NO_STACK_FILL must be set, so we can't calculate stack margin.

	puts stdout [format \
		"\nstack: base 0x%x  end 0x%x  size %d" \
		$pStackBase $pStackEnd $stackSize]
    } {
	if {$stackGrows == "down"} {
	    set stackMargin [expr $pStackMax - $pStackLimit]
	    set stackHigh [expr $stackSize - $stackMargin]
	} {
	    set stackMargin [expr $pStackLimit - $pStackMax]
	    set stackHigh [expr $stackSize - $stackMargin]
	}

	puts stdout [format \
		"\nstack: base 0x%x  end 0x%x  size %d   high %d   margin %d" \
		$pStackBase $pStackEnd $stackSize $stackHigh $stackMargin]
    }

    puts stdout [format "\noptions: 0x%x" $options]
    puts stdout [taskOptions $options]

    puts stdout "\nVxWorks Events\n--------------"

    if {($eventsSysflags & 1) == 0} {
	puts stdout [format "Events Pended on    : Not Pended\nReceived Events     : 0x%x\nOptions             : N/A\n" $eventsReceived]
    } {
	# generate options string displayed after hex representation 

	if {($eventsOptions & 1) == 0} {
	    set optionsString "EVENTS_WAIT_ALL\n"
	} {
	    set optionsString "EVENTS_WAIT_ANY\n"
	}

	if {($eventsOptions & 2) != 0} {
	    set optionsString [format "%s%s" $optionsString \
					"\t\t\t\tEVENTS_RETURN_ALL\n"]
	}
       
	if {($eventsOptions & 4) != 0} {
	    set optionsString [format "%s%s" $optionsString \
					"\t\t\t\tEVENTS_KEEP_UNWANTED\n"]
	}
       
	puts stdout [format "Events Pended on    : 0x%x\nReceived Events     : 0x%x\nOptions             : 0x%x\t%s" $eventsWanted $eventsReceived $eventsOptions \
			     $optionsString]
    }

    regSetPrint [lrange $taskInfoList 20 end]
    puts stdout {}
    return 0
}

##############################################################################
#
# trgShow - show information about a trigger
#
# With the option 1, it will show more detailed information.
#

shellproc trgShow {args} {

    bindNamesToList {trgId verbose} $args

    if {$trgId == 0} {
       return -1
    }

    set trgInfoList [trgInfoGet $trgId]

    bindNamesToList {objCore eventId status disable contextType contextId objId chain next conditional condType condEx1 condOp condEx2 actionType actionFunc actionArg actionDef hitCnt} $trgInfoList

    puts stdout [format "\nTrigger id: 0x%-08x " $trgId]
    puts stdout [format "---------------------------------"]

    puts stdout [format "%-20s : %hd" "event Id" $eventId]

    if {$status == 0} {
        set stat "disabled"
    } else {
        if {$status == 1} {
            set stat "enabled"
        } else {
            set stat "????"
        }
    }
    puts stdout [format "%-20s : %s" "current status" $stat]

    puts stdout [format "%-20s : %d" "number of hits" $hitCnt]

    if {$disable == 1} {
        set dis "YES"
    } else {
        if {$disable == 0} {
            set dis "NO"
        } else {
            set dis "????"
        }
    }

    puts stdout [format "%-20s : %s" "disable after use" $dis]

    switch $contextType {
        0           {set ctx "any"}
        0x1           {set ctx "any task"}
        0x2           {set ctx "task"}
        0x3           {set ctx "any ISR"}
        0x4           {set ctx "ISR"}
        0x5           {set ctx "system"}
        default     {set ctx "????"}
    }

    puts stdout [format "%-20s : %s" "context type" $ctx]
    if {$contextId == 0} {
        puts stdout [format "%-20s : -" "context id"]
    } else {
        puts stdout [format "%-20s : %s" "context id" $contextId]
    }

    if {$objId == 0} {
        puts stdout [format "%-20s : -" "obj id"]
    } else {
        puts stdout [format "%-20s : %s" "obj id" $objId]
    }

    if {$conditional == 1} {
        set cond "YES"
    } else {
        if {$conditional == 0} {
            set cond "NO"
        } else {
            set cond "????"
        }
    }
    puts stdout [format "%-20s : %s" "conditional" $cond]

    puts stdout [format "%-20s : %d" "action" $actionType]

    if {$chain == 0} {
        puts stdout [format "%-20s : -" "chained trigger"]
    } else {
        puts stdout [format "%-20s : %s" "chained trigger" $chain]
    }

    if {$verbose == 1} {

        puts stdout [format "\n- Condition info -\n"]

    if {$condType == 0} {
        set condt "function"
    } else {
        if {$condType == 1} {
            set condt "variable"
        } else {
            if {$condType == -1} {
                set condt "-"
            } else {
                set condt "????"
            }
        }
    }

    puts stdout [format "%-20s : %s" "type" $condt]

    if {$condEx1 == -1} {
        puts stdout [format "%-20s : - " "Ex1"]
    } else {
        puts stdout [format "%-20s : 0x%x" "Ex1" $condEx1]
    }

    switch $condOp {
        0             {set op "=="}
        0x1           {set op "!="}
        0x2           {set op "<"}
        0x3           {set op "<="}
        0x4           {set op ">"}
        0x5           {set op ">="}
        0x6           {set op "||"}
        0x7           {set op "&&"}
        0xffffffff  {set op "-"}
        default     {set op "????"}
    }

    puts stdout [format "%-20s : %s" "operator" $op]
    if {$condEx2 == -1} {
        puts stdout [format "%-20s : - \n" "Ex2"]
    } else {
        puts stdout [format "%-20s : %x\n" "Ex2" $condEx2]
    }

        puts stdout [format "- Action info -\n"]

    if {$actionFunc == 0} {
        puts stdout [format "%-20s : -" "func"]
    } else {
        puts stdout [format "%-20s : %s" "func" $actionFunc]
    }

    if {$actionArg == 0} {
        puts stdout [format "%-20s : -" "arg"]
    } else {
        puts stdout [format "%-20s : %s" "arg" $actionArg]
    }

    if {$actionDef == 1} {
       set def "YES"
    } else {
        if {$actionDef == 0} {
            set def "NO"
       } else {
           set def "????"
        }
    }
    puts stdout [format "%-20s : %s\n" "deferred" $def]

    }

    return 0
}

##############################################################################
#
# version - print various version strings.
#

shellproc version {args} {
    set sysModelFunc [targetFunctionGet sysModel]
    set sysModelAddr [shFuncCall -int -nobreak $sysModelFunc]
    set sysModel [wtxGopherEval "$sysModelAddr $"]
    set kernelVersionFunc [targetFunctionGet kernelVersion]
    set kvAddr [shFuncCall -int -nobreak $kernelVersionFunc]
    set kv [wtxGopherEval "$kvAddr $"]
    set vwVersion [wtxGopherEval "[shSymAddr vxWorksVersion] *$"]
    set creationDate [wtxGopherEval "[shSymAddr creationDate] *$"]
    set sysBootLine [wtxGopherEval "[shSymAddr sysBootLine] *$"]

    puts stdout [format "VxWorks (for %s) version %s." \
	    [string trim $sysModel \{\}\ ] [string trim $vwVersion]]
    puts stdout [format "Kernel: %s" [lindex $kv 0]]
    puts stdout [format "Made on %s." [string trim $creationDate \{\}\ ]]
    puts stdout [format "Boot line:\n%s." [string trim $sysBootLine \{\}\ ]]
    return 0
}

##############################################################################
#
# wdShow - show information about a watchdog.  Can be dispatched by the
# show command or called directly with a watchdog Id.
#

shellproc wdShow {wdId args} {
    set wdInfoList [wdInfoGet $wdId]
    bindNamesToList {vxClock timeout state routine parameter} $wdInfoList

    puts stdout [format "%-20s: 0x%-10x" {Watchdog ID} $wdId]
    puts stdout [format "%-20s: %-10s" {State} \
		    [wdStateName $state]]
    if {$state == 0x0} {
	set routine 0
	set parameter 0
	set ticksLeft 0
    } {
	set ticksLeft [expr $timeout - $vxClock]
    }

    puts stdout [format "%-20s: %-10s" {Ticks Remaining} $ticksLeft]
    puts stdout [format "%-20s: 0x%-10x" {Routine} $routine]
    puts stdout [format "%-20s: 0x%-10x" {Parameter} $parameter]
    return 0
}


##############################################################################
#
# Locals Procedures (Procedures used by the shellprocs defined in this file)
#

##############################################################################
#
# activeQOrder - A sort routine which sorts activeQ records by
# priority (used by the i command).
#

proc activeQOrder {t1 t2} {

    set p1 [lindex $t1 1]
    set p2 [lindex $t2 1]

    return [expr $p1 > $p2 ? 1 : ($p1 < $p2 ? -1 : 0)]
}

##############################################################################
#
# taskInfoOrder - A sort routine which sorts task info records by
# priority (used by the iStrict command).
#

proc taskInfoOrder {t1 t2} {

    set p1 [lindex $t1 2]
    set p2 [lindex $t2 2]

    return [expr $p1 > $p2 ? 1 : ($p1 < $p2 ? -1 : 0)]
}

##############################################################################
#
# taskInfoPrint - format and print information collected for one task
# in the format used by the "i" command.
#

proc taskInfoPrint {vxClock taskInfo} {

    bindNamesToList {taskId status priority pc sp \
                     errno timeout entry priNormal name} \
        $taskInfo


    # convert numeric entry to symbolic form if possible.

    set entry [shSymbolicAddr -exact $entry]

    set delayCount [expr {$status & 0x4 ? $timeout - $vxClock : 0}]
    set inherited [expr $priority > $priNormal]
    puts stdout [format "%-11.11s%-12.12s %8x %3d %-10s %8x %8x %7x %5d"\
        $name								\
        $entry								\
        $taskId								\
        $priority							\
        [taskStatus $status $inherited]					\
	$pc								\
	$sp								\
	$errno								\
	$delayCount]
}

##############################################################################
#
# taskPrintBanner - print the usual "i" command banner
#
# SYNOPSIS:
#   taskPrintBanner
#

proc taskPrintBanner {} {
    puts stdout {  NAME        ENTRY       TID    PRI   STATUS      PC       SP     ERRNO  DELAY}
    puts stdout {---------- ------------ -------- --- ---------- -------- -------- ------- -----}
}

##############################################################################
#
# taskPrintShortBanner - print a short task banner
#
# This routine prints the "short" task banner, which is an abbreviated banner
# used when printing lists of tasks blocked on semaphores or message queues.
# The banner itself depends on the range parameter, either 'local' or 'shared'.
#
# SYNOPSIS:
#   taskPrintShortBanner range
#

proc taskPrintShortBanner {range} {

    if {"$range" == "shared"} {
	puts stdout {    TID    CPU Number Shared TCB}
	puts stdout {---------- ---------- ----------}
    } else {
	puts stdout {   NAME      TID    PRI TIMEOUT}
	puts stdout {---------- -------- --- -------}
    }
}

##############################################################################
#
# blockedTaskInfoPrint - format and print information about a blocked task
#
# Formats and prints information about a blocked task.  The output is meant
# to fit nicely under the text printed by taskPrintShortBanner.
#
# SYNOPSIS:
#   blockedTaskInfoPrint vxClock taskId blockInfo
#
# PARAMETERS:
#   vxClock: the value of vxTicks when the sample was taken (for computing
#     delay counts)
#   taskId: id of blocked task
#   blockInfo: a list with the following 5 elements from the TCB of a task:
#     timeout
#     status
#     priority
#     name
#

proc blockedTaskInfoPrint {vxClock taskId blockInfo} {

    set stat [lindex $blockInfo 1]
    if {$stat & 0x4} {
	set timeout [expr [lindex $blockInfo 0] - $vxClock]
    } {
	set timeout 0
    }

    puts stdout [format "%-10s " [lindex $blockInfo 3]] nonewline
    puts stdout [format "%8x %3d %7d"	\
	$taskId				\
	[lindex $blockInfo 2]		\
	$timeout]
}

##############################################################################
#
# smBlockedTaskInfoPrint - format and print info about a task blocked on SM obj
#
# Formats and prints information about a task blocked on a shared memory
# object.  The output is meant to fit nicely under the text printed by
# taskPrintShortBanner.
#
# SYNOPSIS:
#   smBlockedTaskInfoPrint sharedTcb blockInfo
#
# PARAMETERS:
#   sharedTcb: shared TCB address of blocked task.
#   blockInfo: a list with the following 2 elements from the shared TCB of a
#              task:
#     taskId
#     cpuNum
#

proc smBlockedTaskInfoPrint {sharedTcb blockInfo} {

    set taskId [lindex $blockInfo 0]
    set cpuNum [lindex $blockInfo 1]

    puts stdout [format "%#-10x    %2d      %#-10x" $taskId $cpuNum $sharedTcb]
}

##############################################################################
#
# taskOptions - given an integer representing the options member of the TCB
# of some task, print an ascii representation of the task options.
#

proc taskOptions {options} {
    global taskOptionNames
    global taskOptMax
    set result {}
    for {set i 0} {$i <= $taskOptMax} {incr i} {
	if {$options & (1 << $i)} {
	    lappend result [lindex $taskOptionNames $i]
	}
    }
    return $result
}

##############################################################################
#
# shSymbolicAddr - return the symbolic form of a numerical address, if
#                  possible
#
# SYNOPSIS:
#   shSymbolicAddr flag addr
#
# PARAMETERS:
#   flag: either -exact, for exact matches, or -inexact
#   addr: the address to reformat
#
# RETURNS:
#   addr, if no symbol matches addr exactly, or else a symbol name,
#   possibly with an additive offset if -inexact was specified
#
# ERRORS:
#

proc shSymbolicAddr {flag addr} {
    if {[catch {set sym [wtxSymFind -value $addr]}] == 0} {
	set symAddr [lindex $sym 1]
	if {$flag == "-exact"} {
	    if {$symAddr == $addr} {
		return [shDemangle [lindex $sym 0]]
	    } else {
		return $addr
	    }
	} else {
	    # XXX we don't validate that the argument is -inexact
	    if {$addr >= $symAddr && $addr - $symAddr < 0x1000} {
		set diff [expr $addr - $symAddr]
		if {$diff > 0} {
		    return [format "[shDemangle [lindex $sym 0]] + 0x%x" $diff]
		} else {
		    return [shDemangle [lindex $sym 0]]
		}
	    } else {
		return $addr
	    }
	}
    }
    return $addr
}

###############################################################################
#
# reg32Write - set value into a 32 bits register
#
# SYNOPSIS:
#       reg32Write tid ind val
#
# PARAMETERS:
#       tid: task whose register is written
#       ind: index of the register in the register name
#       val: value to be written
#
# RETURNS: N/A
#

proc reg32Write {tid ind val} {

    set endianFlag "-[string range [targetEndian] 0 0]"
    set blk [memBlockCreate $endianFlag]
    memBlockSet -l $blk 0 $val
    set offset [regOffsetFromIndexGet $ind]
    wtxRegsSet CONTEXT_TASK $tid REG_SET_IU $offset 4 $blk
    memBlockDelete $blk
}

##############################################################################
#
# modShow - show information about one module
#
# SYNOPSIS:
#   modShow modId
#
# PARAMETERS:
#   modId:	ID of module to show information about
#

proc modShow {modId} {
    set modInfo [wtxObjModuleInfoGet $modId]

    set textStart [lindex [lindex $modInfo 5] 1]
    if {$textStart == "0xffffffff"} {
	set textStart "          "
    }
    set dataStart [lindex [lindex $modInfo 6] 1]
    if {$dataStart == "0xffffffff"} {
	set dataStart "          "
    }
    set bssStart [lindex [lindex $modInfo 7] 1]
    if {$bssStart == "0xffffffff"} {
	set bssStart "          "
    }

    puts stdout [format "%-24.24s %#10x %10d %#10s %#10s %#10s" \
	    [lindex $modInfo 1] \
	    [lindex $modInfo 0] \
	    [lindex $modInfo 3] \
	    $textStart \
	    $dataStart \
	    $bssStart]
    return 0
}

##############################################################################
#
# shDemangle - demangle C++ name
#
# Note: mode 0 and style "none" are redundant.
#

proc shDemangle {symbol} {
    global shDemangleStyle
    global shDemangleMode

    if {$shDemangleMode != "" && $shDemangleMode != 0} {

	if {$shDemangleStyle != "" && $shDemangleStyle != "none"} {

	    # strip off the leading underscore, if any.  It's unsightly in 
	    # demangled symbols and an extra underscore will confuse
	    # some demanglers.

	    set fixedSymbol [underscoreStrip $symbol]

	    if [catch \
		{demangle $shDemangleStyle $fixedSymbol $shDemangleMode} \
		result] {
		return $result
	    } else {
		if {$result != $fixedSymbol} {
		    return $result
		} else {
		    return $symbol
		}
	    }
	}
    }
    return $symbol
}

##############################################################################
#
# eventRsrcShow - show a resource's VxWorks events information
#
# SYNOPSIS:
#   eventRsrcShow evRegistered evTaskId evOptions
#
# PARAMETERS:
#   evRegistered: registered field of an EVENTS_RSRC struct
#   evTaskId    : taskId field of an EVENTS_RSRC struct
#   evOptions   : options field of an EVENTS_RSRC struct
#
# RETURNS:
#   N/A
#
# ERRORS:
#   NONE

proc eventRsrcShow {evRegistered evTaskId evOptions} {

    puts stdout "VxWorks Events\n--------------"

    if {$evTaskId == 0} {
	puts stdout "Registered Task     : NONE\nEvent(s) to Send    : N/A\nOptions             : N/A\n"

    } {
	# obtain name of registered task

	set taskName [taskIdToName $evTaskId]
	if {$taskName == ""} {
	    set taskName Deleted!
	}

	# generate options string

	if {$evOptions == 0} {
	    set optionsString "EVENTS_OPTIONS_NONE\n"
	} {
	    set optionsString ""

	    if {($evOptions & 0x1) == 0x1} {
		set optionsString "EVENTS_SEND_ONCE\n\t\t\t\t"
	    }
	
	    if {($evOptions & 0x2) == 0x2} {
		set optionsString [format "%s%s" $optionsString \
					     "EVENTS_ALLOW_OVERWRITE\n\t\t\t\t"]
	    }

	    if {($evOptions & 0x4) == 0x4} {
		set optionsString [format "%s%s" $optionsString \
					     "EVENTS_SEND_IF_FREE\n"]
	    }
	}

        puts stdout [format "Registered Task     : 0x%x\t(%s)\nEvent(s) to Send    : 0x%x\nOptions             : 0x%x\t%s" $evTaskId $taskName $evRegistered \
				   $evOptions $optionsString]
    }
}
