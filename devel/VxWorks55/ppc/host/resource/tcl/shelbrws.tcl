# shelbrws.tcl - shell browser Tcl procedures
#
# Copyright 1994-2001 Wind River Systems, Inc.
#
# modification history
# ----------------------
# 01s,08nov01,aeg  added VxWorks events awareness.
# 01r,17apr98,rlp  canceled msgQBrowse modification for backward
#                  compatibility. Adapted msgQBrowse syntax to Tcl 8.0.
# 01q,13feb98,pad  Adapted to Tcl 8.0 syntax.
# 01p,04nov97,rlp  modified msgQBrowse for tracking messages sent.
# 01o,29jun96,jank source i2oShelBrws.tcl file if exists
# 01n,16jan96,mem  Added use of targetEndian proc.
# 01m,30oct95,pad  Modified taskStruct{} and taskBrowse{} to display the entry
#		   routine (SPR #5165). Lined up fields in hierarchy structures.
#		   Reworked classBrowse{} for a display closer to WindSh.
# 01l,27oct95,c_s  add class browser functions (SPR #4567).
# 01k,20sep95,pad  added Shared Memory Object management.
# 01j,14sep95,pad  added management of POSIX semaphores and message queues.
# 01i,12sep95,jco  fixed free size computation in memPartBrowse, spr#4758.
# 01h,01aug95,c_s  fixed block size in memPart browser for little-endian 
#                    targets.
# 01g,05jul95,c_s  updated blockedTaskArrayConstruct to use the new format.
# 01f,15jun95,c_s  added globalBrowseListRemoveByName; added shellBrowseInit.
# 01e,25apr95,c_s  now uses wtxPath.
# 01d,03apr95,c_s  rearranged message queue structure so it has an 
#                    Attributes folder.
# 01c,23feb95,c_s  formatting.
# 01b,23jan95,c_s  added shellCoreInit call.
# 01a,06dec94,c_s  written.
#*/

# The following global variables build up a regular expression which 
# recognizes "WRS" tasks by name.

set __pat0 "tSl\[0-9\]+Wrt|tFtpdServ\[0-9\]+|tFtpdTask|tNetTask|tShell|"
set __pat1 "tRlogind|tRlogOutTask|tRlogInTask|tRlogChildTask|"
set __pat2 "tPortmapd|tTelnetd|tTelnetOutTask|tTelnetInTask|"
set __pat3 "tTftpTask|tTftpdTask|tTftpRRQ|tTftpWRQ|tExcTask|"
set __pat4 "tLogTask|tSpyTask|tRootTask|tRestart|tRdbTask|tWdbTask"

set wrsTaskPattern "$__pat0$__pat1$__pat2$__pat3$__pat4"

##############################################################################
#
# browseActiveTasks - return a hierarchy of active task names (and Ids).
#
# Return a hierarchy structure which contains two top level entries:
# "WRS Tasks" and "User Tasks".  Each of these entries is populated 
# with one item for each task that fits the category, as dictated by 
# the regular expression above.
#
# SYNOPSIS:
#   browseActiveTasks tasks
#
# PARAMETERS: 
#   tasks: a list of paired task ids, names and statuses, as produced by 
#     the routine "taskIdNameStatusList".
#
# RETURNS:
#   A hierarchy structure with the input tasks sorted into categories,
#   with their names and IDs in the hierarchy entries.
#
# ERRORS: 
#   NONE

proc browseActiveTasks {tasks} {
    global wrsTaskPattern
    set wrsTasks ""
    set userTasks ""
    
    while {[llength $tasks] > 1} {
	bindNamesToList {taskId taskName taskStatus pri priNormal} $tasks

	set string [format "%#10x %-12.12s %s" \
		$taskId $taskName [taskStatus $taskStatus \
		        [expr $pri > $priNormal]]]

	if [regexp $wrsTaskPattern $taskName] {
	    lappend wrsTasks $string
	} else {
	    lappend userTasks $string
	}

	set tasks [lrange $tasks 5 end]
    }

    return "{$wrsTasks} {$userTasks}"
}

##############################################################################
#
# taskStruct - return the hierarchy structure for a task browser
#
# SYNOPSIS:
#   taskStruct taskId
#
# PARAMETERS:
#   taskId - the ID of the task to be browsed (not used).
#
# RETURNS: 
#   A hierarchy structure for the task browser
#
# ERRORS: 
#   NONE

proc taskStruct {taskId} {
    return "{Attributes {\"entry   \" \"status  \" priority \"options \"
                         \"error   \" \"delay   \"}}
            {Stack {\"base  \" \"end   \" \"size  \" \"high  \" margin}} \
            {\"VxWorks Events\" {\"wanted  \" received  \"options \"}} \
            [taskRegSetStruct]"
}

##############################################################################
#
# taskBrowserName - return the title for a task browser
#
# SYNOPSIS:
#   taskBrowserName
#
# PARAMETERS:
#   the ID of the task to be browsed
#
# RETURNS:
#   the name for the browser (which will appear in the window titlebar)
#
# ERRORS:
#   NONE

proc taskBrowserName {taskId} {
    return "[wtxTargetName]: [taskIdToName $taskId]"
}

##############################################################################
#
# taskBrowse - construct data to fill a task browser
#
# SYNOPSIS:
#   taskBrowse taskId
#
# PARAMETERS:
#   the ID of the task to be browsed
#
# RETURNS:
#   a hierarchy value list that matches the structure returned by taskStruct:
#   {entry status priority options errCode delayCount}
#   {stkBase stkEnd stkSize stkHigh stkMargin}
#   {eventsWanted eventsReceived eventsOptions}
#   {the task register set vector--preparedby taskRegSetVec}
#   
# ERRORS:
#   NONE

proc taskBrowse {taskId} {
    global stackGrows

    set taskInfoList [taskInfoGetVerbose $taskId]
    set taskName [taskIdToName $taskId]
    set options [lindex $taskInfoList 11]
    set errCode [lindex $taskInfoList 6]
    set status [lindex $taskInfoList 2]
    set timeout [lindex $taskInfoList 7]
    set vxClock [lindex $taskInfoList 0]
    set pri [lindex $taskInfoList 3]
    set priNormal [lindex $taskInfoList 9]
    set entry [lindex $taskInfoList 8]

    if {$pri == $priNormal} {
	set priString [format "%d" $pri]
    } {
	set priString [format "%d/%d" $pri $priNormal]
    }

    set delayCount [expr {$status & 0x4 ? $timeout - $vxClock : 0}]
    set stkBase [lindex $taskInfoList 12]
    set stkEnd [lindex $taskInfoList 13]
    set stkLimit [lindex $taskInfoList 14]
    set stkMax [lindex $taskInfoList 15]

    set eventsReceived  [lindex $taskInfoList 17]
    set eventsSysflags  [lindex $taskInfoList 19]

    if {($eventsSysflags & 1) == 0} {
	set eventsWanted    "N/A"
	set eventsOptions   "N/A"
    } {
	set eventsWanted    [lindex $taskInfoList 16]
	set eventsOptions   [lindex $taskInfoList 18]
    }

    if {$stackGrows == "down"} {
	set stkSize [expr $stkBase - $stkLimit]
	set stkMargin [expr $stkMax - $stkLimit]
	set stkHigh [expr $stkSize - $stkMargin]
    } {
	set stkSize [expr $stkLimit - $stkBase]
	set stkMargin [expr $stkLimit - $stkMax]
	set stkHigh [expr $stkSize - $stkMargin]
    }

    set statusString [taskStatus $status [expr $pri > $priNormal]]
    set entryFunc [lindex [wtxSymFind -value $entry] 0]
    
    return "{$entryFunc $statusString $priString $options $errCode $delayCount}
	    {$stkBase $stkEnd $stkSize $stkHigh $stkMargin}
	    {$eventsWanted $eventsReceived $eventsOptions}
	    [taskRegSetVec $taskInfoList]"
}

##############################################################################
#
# semBrowserName - return the name of a semaphore browser, given ID
#
# SYNOPSIS:
#   semBrowserName semId
#
# PARAMETERS:
#   semId: the ID of the semaphore being browsed
#
# RETURNS:
#   the name of the browser for the semaphore (may be used in window titlebar)
#
# ERRORS:
#   NONE

proc semBrowserName {semId} {
    if {[objIsShared $semId]} {
	return "[wtxTargetName]: SM SEM $semId"
    } else {
	return "[wtxTargetName]: SEM $semId"
    }
}

##############################################################################
#
# semStruct - return a hierarchy structure for a semaphore browser
#
# SYNOPSIS:
#   semStruct semId
#
# PARAMETERS:
#   semId: the ID of the semaphore being browsed
#
# RETURNS:
#   a hierarchy structure to present the details of a semaphore object
#
# ERRORS:
#   NONE

proc semStruct {semId} {
    if {[objIsShared $semId]} {
        return "{Attributes {\"type   \" \"queue  \" pending \"state  \"}}
                {\"Blocked Tasks\" {+ {\"task_id   \" \"cpu number\"
                                       \"shared_TCB\"}}}"
    } else {
        return "{Attributes {\"type   \" \"queue  \" pending \"state  \"}}
                {\"VxWorks Events\" {task_id  \"events \" options}} \

                {\"Blocked Tasks\" {+ {\"name   \" \"task_id\" \"pri    \"
                                       timeout}}}"
    }
}
    
##############################################################################
#
# semBrowse - construct data to fill a semaphore browser
#
# SYNOPSIS:
#   semBrowse semId
#
# PARAMETERS:
#   semId: the ID of the semaphore to browse
#
# RETURNS:
#   A hierarchy value list that matches the structure returned by semStruct:
#
#   {semType semQType nBlockedTasks semContents {{blockedTask}...}
#
# ERRORS:
#   NONE

proc semBrowse {semId} {
    set semInfoList [semInfoGet $semId]

    if {[objIsShared $semId]} {
	bindNamesToList {vxClock sType options contents} $semInfoList
    } {
	bindNamesToList {vxClock sType options contents evRegistered \
			 evTaskId evOptions } $semInfoList

	if {$evTaskId == 0} {
	    set evTaskId NONE
	    set evRegistered N/A
	    set evOptions N/A
	}
    }

    set semTypeString "[semType $sType]"
    set semQString [semQueueType $options]
    set semContentsString [semContents $sType $contents]

    if {[objIsShared $semId]} {
        set semInfoList [lrange $semInfoList 4 end]
	set btArray [smBlockedTaskArrayConstruct $semInfoList]
        set retval "{$semTypeString $semQString [llength $btArray] \
	        $semContentsString} {$btArray}"
    } else {
        set semInfoList [lrange $semInfoList 7 end]
	set btArray [blockedTaskArrayConstruct $vxClock $semInfoList]
        set retval "{$semTypeString $semQString [llength $btArray] \
	        $semContentsString} {$evTaskId $evRegistered $evOptions} \
		{$btArray}"
    }

}

##############################################################################
#
# semPxBrowserName - return the name of a POSIX semaphore browser, given ID
#
# SYNOPSIS:
#   semPxBrowserName semId
#
# PARAMETERS:
#   semId: the ID of the POSIX semaphore being browsed
#
# RETURNS:
#   the name of the browser for the semaphore (may be used in window titlebar)
#
# ERRORS:
#   NONE

proc semPxBrowserName {semId} {
    return "[wtxTargetName]: PX SEM $semId"
}

##############################################################################
#
# semPxStruct - return a hierarchy structure for a POSIX semaphore browser
#
# SYNOPSIS:
#   semPxStruct semPxId
#
# PARAMETERS:
#   semPxId - the ID of the POSIX semaphore to browse (not use).
#
# RETURNS:
#   a hierarchy structure to present the details of a POSIX semaphore object
#
# ERRORS:
#   NONE

proc semPxStruct {semPxId} {
    return "{Attributes {\"name    \" sem_open \"value   \" \"pending \"}} \
	    {\"Blocked Tasks\" {+ {\"name   \" \"task_id\" \"pri    \"
				   timeout}}}"
}
    
##############################################################################
#
# semPxBrowse - construct data to fill a POSIX semaphore browser
#
# SYNOPSIS:
#   semPxBrowse semId
#
# PARAMETERS:
#   semId: the ID of the POSIX semaphore to browse
#
# RETURNS:
#   A hierarchy value list that matches the structure returned by semPxStruct:
#
#   {semPxName refCnt semPxValue nBlockedTasks {{blockedTask}...}
#
# ERRORS:
#   NONE

proc semPxBrowse {semId} {

    # Get semaphore structure's field contents, plus pended tasks

    set semPxInfoList [semPxInfoGet $semId]
    bindNamesToList {semWindId refCnt sem_name count} $semPxInfoList

    if {$sem_name != 0} {
	set semPxName [wtxGopherEval "$sem_name $"]
    } else {
	set semPxName "<UNNAMED>"
	set refCnt 0
    }

    set semPxInfoList [lrange $semPxInfoList 4 end]
    set nBlockedTasks [llength $semPxInfoList]

    if {($count == 0) && ($nBlockedTasks != 0)} {
	set semPxValue 0
    } else {
	set semPxValue $count
    }

    set btArray [blockedTaskArrayConstruct 0 $semPxInfoList]

    set retval "{$semPxName $refCnt $semPxValue $nBlockedTasks} {$btArray}"
}

##############################################################################
#
# msgQBrowserName - return the name of a message queue browser, given ID
#
# SYNOPSIS:
#   msgQBrowserName msgQId
#
# PARAMETERS:
#   msgQId: the ID of the message queue being browsed
#
# RETURNS:
#   the name of the browser for the message queue (may be used in titlebar)
#
# ERRORS:
#   NONE


proc msgQBrowserName {msgQId} {
    if {[objIsShared $msgQId]} {
	return "[wtxTargetName]: SM MSGQ $msgQId"
    } else {
	return "[wtxTargetName]: MSGQ $msgQId"
    }
}

##############################################################################
#
# msgQStruct - return a hierarchy structure for a message queue browser
#
# SYNOPSIS:
#   msgQStruct msgQId
#
# PARAMETERS:
#   msgQId: the ID of the message queue being browsed (not used).
#
# RETURNS:
#   a hierarchy structure to present the details of a message queue object
#
# ERRORS:
#   NONE


proc msgQStruct {msgQId} {
    if {[objIsShared $msgQId]} {
        return "
            {Attributes {\"options     \" \"maxMsgs    \" \"maxLength  \"
                         sendTimeouts recvTimeouts}}
            {\"Receivers Blocked\" {+ {\"task_id   \" \"cpu number\"
                                       \"shared_TCB\"}}}
            {\"Senders Blocked\" {+ {\"task_id   \" \"cpu number\"
                                     \"shared_TCB\"}}}
            {\"Messages Queued\"   {+ {address \"length \" \"value  \"}}}"
    } else {
        return "
            {Attributes {\"options     \" \"maxMsgs   \" \"maxLength  \"
                         sendTimeouts recvTimeouts}}
            {\"VxWorks Events\" {task_id  \"events \" options}} \
            {\"Receivers Blocked\" {+ {\"name   \" \"task_id\" \"pri    \"
                                       timeout}}}
            {\"Senders Blocked\" {+ {\"name   \" \"task_id\" \"pri    \"
                                     timeout}}}
            {\"Messages Queued\"   {+ {address \"length \" \"value  \"}}}"
    }
}

##############################################################################
#
# msgQBrowse - construct data to fill a message queue browser
#
# SYNOPSIS:
#   msgQBrowse msgQId
#
# PARAMETERS:
#   msgQId: the ID of the message queue to browse
#
# RETURNS:
#   A hierarchy value list that matches the structure returned by msgQStruct:
#
#   state ticksLeft routine parameter
#
# ERRORS:
#   NONE

proc msgQBrowse {msgQId} {
    set msgQInfoList [msgQInfoGet $msgQId]

    # pick out the static members
    if {[objIsShared $msgQId]} {
	bindNamesToList {vxClock options maxMsgs maxMsgLength \
			 sendTimeouts recvTimeouts} \
	    		 $msgQInfoList
    } {
	bindNamesToList {vxClock options maxMsgs maxMsgLength \
			 sendTimeouts recvTimeouts evRegistered \
			 evTaskId evOptions} $msgQInfoList

	if {$evTaskId == 0} {
	    set evTaskId NONE
	    set evRegistered N/A
	    set evOptions N/A
	}
    }

    set typeString [msgQType $options]

    # throw away the part we just bound to 
    if {[objIsShared $msgQId]} {
	set msgQInfoList [lrange $msgQInfoList 6 end]

    } {
	set msgQInfoList [lrange $msgQInfoList 9 end]
    }

    # locate the first sentinel and extract the list of blocked senders
    set sentinelIx [lsearch $msgQInfoList 0xee]
    set senderList [lrange $msgQInfoList 0 [expr $sentinelIx - 1]]

    # throw away the senders.  Locate the next sentinel...
    set msgQInfoList [lrange $msgQInfoList [expr $sentinelIx + 1] end]
    set sentinelIx [lsearch $msgQInfoList 0xee]

    # and extract the list of receivers.  What's left over is the list
    # of messages, after skipping the second sentinel.
    set receiverList [lrange $msgQInfoList 0 [expr $sentinelIx - 1]]
    set msgQInfoList [lrange $msgQInfoList [expr $sentinelIx + 1] end]

    # for the list of messages, construct an array that has an element
    # for each message.  Each element is itself a list of 3 elements:
    # the address, length of the message, and its contents, in a processed
    # format e.g.:
    #  {{a0 l0 c0} {a1 l1 c1}...}

    set msgArray ""
    while {[llength $msgQInfoList] >= 2} {
	set msgAdrs [lindex $msgQInfoList 0]
	set msgLen [lindex $msgQInfoList 1]
	lappend msgArray [list $msgAdrs $msgLen [msgGet $msgAdrs $msgLen]]
	set msgQInfoList [lrange $msgQInfoList 2 end]
    }
    set msgArray "{$msgArray }"

    # To construct the final result, just copy in the static members, 
    # call blockedTaskArrayConstruct to process the blocked senders and
    # blocked receivers, and then append the message array we just 
    # constructed.

    if {[objIsShared $msgQId]} {
	set rcvArray [smBlockedTaskArrayConstruct $receiverList]
	set sndArray [smBlockedTaskArrayConstruct $senderList]

	return "{$typeString [format "%d" $maxMsgs] \
		 [format "%d" $maxMsgLength] [format "%d" $sendTimeouts] \
		 [format "%d" $recvTimeouts]} \
		 {$rcvArray} \
	    	 {$sndArray} \
		 $msgArray"
    } else {
	set rcvArray [blockedTaskArrayConstruct $vxClock $receiverList]
	set sndArray [blockedTaskArrayConstruct $vxClock $senderList]

	return "{$typeString [format "%d" $maxMsgs] \
		 [format "%d" $maxMsgLength] [format "%d" $sendTimeouts] \
		 [format "%d" $recvTimeouts]} \
		 {$evTaskId $evRegistered $evOptions} \
		 {$rcvArray} \
	    	 {$sndArray} \
		 $msgArray"
    }

}

##############################################################################
#
# mqBrowserName - return the name of a POSIX message queue browser, given ID
#
# SYNOPSIS:
#   mqBrowserName mqPxId
#
# PARAMETERS:
#   mqPxId: the ID of the POSIX message queue being browsed
#
# RETURNS:
#   the name of the browser for the POSIX message queue (may be used in
#   titlebar)
#
# ERRORS:
#   NONE


proc mqBrowserName {mqPxId} {
    return "[wtxTargetName]: PX MSGQ $mqPxId"
}

##############################################################################
#
# mqStruct - return a hierarchy structure for a POSIX message queue browser
#
# SYNOPSIS:
#   mqStruct mqPxId
#
# PARAMETERS:
#   mqPxId: the ID of the POSIX message queue being browsed (not used)
#
# RETURNS:
#   a hierarchy structure to present the details of a POSIX message queue object
#
# ERRORS:
#   NONE


proc mqStruct {mqPxId} {
    return "{Attributes {\"name       \" curMessages maxMessages
                         \"maxSize    \"}}
            {\"Readers Blocked\" {+ {\"name   \" \"task_id\" \"pri    \"
                                     timeout}}}
            {\"Writers Blocked\" {+ {\"name   \" \"task_id\" \"pri    \"
                                     timeout}}}"
}

##############################################################################
#
# mqBrowse - construct data to fill a POSIX message queue browser
#
# SYNOPSIS:
#   mqBrowse mqPxId
#
# PARAMETERS:
#   mqPxId: the ID of the POSIX message queue to browse
#
# RETURNS:
#   A hierarchy value list that matches the structure returned by mqStruct:
#
#   {mqPxName currentMsg maxMsg msgSize {{taskId}...} {{taskId}...}
#
# ERRORS:
#   NONE

proc mqBrowse {mqPxId} {

    # Get message queue structure's field contents.

    set mqPxInfoList [mqPxInfoGet $mqPxId]

    # Isolate the list of senders and the list of receivers.

    set sentinelIx [lsearch $mqPxInfoList 0xee]
    set senderList [lrange $mqPxInfoList 0 [expr $sentinelIx - 1]]
    set mqPxInfoList [lrange $mqPxInfoList [expr $sentinelIx + 1] end]
    set sentinelIx [lsearch $mqPxInfoList 0xee]
    set receiverList [lrange $mqPxInfoList 0 [expr $sentinelIx - 1]]
    set mqPxInfoList [lrange $mqPxInfoList [expr $sentinelIx + 1] end]

    # pick out the static members

    bindNamesToList {msgNameAddr currentMsg maxMsg msgSize flags} $mqPxInfoList

    if {$msgNameAddr != 0} {
	set mqPxName [wtxGopherEval "$msgNameAddr $"]
    } else {
	set mqPxName "<NONE>"
    }

    # To construct the final result, just copy in the static members, 
    # call blockedTaskArrayConstruct to process the blocked senders and
    # blocked receivers.

    return "{$mqPxName [format "%d" $currentMsg] [format "%d" $maxMsg] \
	    [format "%d" $msgSize]} \
	    {[blockedTaskArrayConstruct 0 $receiverList]} \
	    {[blockedTaskArrayConstruct 0 $senderList]}"
}
    
##############################################################################
#
# memPartBrowserName - return the name of a mem partition browser, given ID
#
# SYNOPSIS:
#   memPartBrowserName memPartId
#
# PARAMETERS:
#   memPartId: the ID of the memPart being browsed
#
# RETURNS:
#   the name of the browser for the memPart (may be used in window titlebar)
#
# ERRORS:
#   NONE


proc memPartBrowserName {memPartId} {
    if {[objIsShared $memPartId]} {
	return "[wtxTargetName]: SM MEMPART $memPartId"
    } else {
	return "[wtxTargetName]: MEMPART $memPartId"
    }
}

##############################################################################
#
# memPartStruct - return a hierarchy structure for a memPart browser
#
# SYNOPSIS:
#   memPartStruct memPartId
#
# PARAMETERS:
#   memPartId: the ID of the memPart being browsed (not used).
#
# RETURNS:
#   a hierarchy structure to present the details of a memPart object
#
# ERRORS:
#   NONE


proc memPartStruct {memPartId} {
    return "{Total {bytes}}
            {Allocated {blocks \"bytes \"}}
	    {Free {blocks \"bytes \"}}
            {Cumulative {blocks \"bytes \"}} 
	    {\"Free List\" {+ {addr size}}}"
}

##############################################################################
#
# memPartBrowse - construct data to fill a memPart browser
#
# SYNOPSIS:
#   memPartBrowse memPartId
#
# PARAMETERS:
#   memPartId: the ID of the memPart to browse
#
# RETURNS:
#   A hierarchy value list that matches the structure returned by 
#   memPartStruct.
#
# ERRORS:
#   NONE

proc memPartBrowse {memPartId} {
    if {[objIsShared $memPartId]} {
	set smMemPartId [smObjIdToAdrs $memPartId]
	if {! [smObjVerify $smMemPartId]} {
	    noticePost error "Invalid memory partition id: $smMemPartId"
	}
	set memPartInfoList [smMemPartInfoGet $smMemPartId]
    } else {
	set memPartInfoList [memPartInfoGet $memPartId]
    }

    bindNamesToList {totWds curBlks curWds cumBlks cumWds} $memPartInfoList
    set memPartInfoList [lrange $memPartInfoList 5 end]

    if {! [objIsShared $memPartId]} {
	if {[targetEndian] == "BIG_ENDIAN"} {
	    set sizeBit 1
	    set wMult 1
	} {
	    set sizeBit 0x80000000
	    set wMult 2
	}
    }

    set nFreeBlks 0
    set nFreeBytes 0
    while {[llength $memPartInfoList] >= 2} { 

	if {! [objIsShared $memPartId]} {
	    # need to mask the flag bit.  On big-endian targets, it's the
	    # LSB; on little-endian targets, it's the MSB. After masking
	    # the flag bit on BE architectures, there's no need to
	    # multiply by 2; on LE architectures, we must make this
	    # adjustment here. The shell has to do this also.  XXX The
	    # routine memPartInfoGet should be fixed to take care of this
	    # for us.

	    set size [expr $wMult * (~$sizeBit & [lindex $memPartInfoList 1])]

	    lappend freeList [list [lindex $memPartInfoList 0] $size]
	} else {
	    set size [expr 2 * [lindex $memPartInfoList 1]]

	    lappend freeList [list [format "%#x" [locToGlobAdrs \
				[lindex $memPartInfoList 0]]] $size]
	}
	incr nFreeBlks
	incr nFreeBytes $size
	set memPartInfoList [lrange $memPartInfoList 2 end]
    }

    return "{ [expr 2*$totWds] } 
            { [expr $curBlks] [expr 2*$curWds] } 
	    { $nFreeBlks $nFreeBytes }
            { [expr $cumBlks] [expr 2*$cumWds]  } { $freeList }"
}
    
##############################################################################
#
# wdBrowserName - return the name of a watchdog browser, given ID
#
# SYNOPSIS:
#   wdBrowserName wdId
#
# PARAMETERS:
#   wdId: the ID of the wd being browsed
#
# RETURNS:
#   the name of the browser for the wd (may be used in window titlebar)
#
# ERRORS:
#   NONE


proc wdBrowserName {wdId} {
    return "[wtxTargetName]: WD $wdId"
}

##############################################################################
#
# wdStruct - return a hierarchy structure for a wd browser
#
# SYNOPSIS:
#   wdStruct wdId
#
# PARAMETERS:
#   wdId: the ID of the wd being browsed (not used).
#
# RETURNS:
#   a hierarchy structure to present the details of a wd object
#
# ERRORS:
#   NONE


proc wdStruct {wdId} {
    return "\"state    \" \"ticks    \" \"routine  \" parameter"
}

##############################################################################
#
# wdBrowse - construct data to fill a wd browser
#
# SYNOPSIS:
#   wdBrowse wdId
#
# PARAMETERS:
#   wdId: the ID of the wd to browse
#
# RETURNS:
#   A hierarchy value list that matches the structure returned by 
#   wdStruct.
#
# ERRORS:
#   NONE

proc wdBrowse {wdId} {
    set wdInfoList [wdInfoGet $wdId]

    bindNamesToList {vxClock timeout status routine parameter} $wdInfoList

    if {$status == 0x0} {
	set routine 0 
	set parameter 0 
	set ticksLeft 0
    } {
	set ticksLeft [expr $timeout - $vxClock]
    }

    

    return "[wdStateName $status] $ticksLeft $routine $parameter"
}
    
##############################################################################
#
# classBrowserName - return the name of a class browser, given ID
#
# SYNOPSIS:
#   classBrowserName classId
#
# PARAMETERS:
#   classId: the ID of the class being browsed
#
# RETURNS:
#   the name of the browser for the class (may be used in window titlebar)
#
# ERRORS:
#   NONE


proc classBrowserName {classId} {
    global className
    if [catch {set cName $className($classId)}] {
	# class name not in table
	set cName $classId
    }

    return "[wtxTargetName]: CLASS $cName"
}

##############################################################################
#
# classStruct - return a hierarchy structure for a class browser
#
# SYNOPSIS:
#   classStruct classId
#
# PARAMETERS:
#   classId: the ID of the class being browsed (not used).
#
# RETURNS:
#   a hierarchy structure to present the details of a class object
#
# ERRORS:
#   NONE


proc classStruct {classId} {
    return {"memPartId  "
	    "size       "
            "allocated  "
            "deallocated"
	    "initialized"
	    "terminated "
	{Routines {"create " "init   " "destroy" "show   " "help   "}}}
}

##############################################################################
#
# classBrowse - construct data to fill a class browser
#
# SYNOPSIS:
#   classBrowse classId
#
# PARAMETERS:
#   classId: the ID of the class to browse
#
# RETURNS:
#   A hierarchy value list that matches the structure returned by 
#   classStruct.
#
# ERRORS:
#   NONE

proc classBrowse {classId} {

    # Get information about class

    set classInfoList [classInfoGet $classId]
    bindNamesToList {objPartId objSize objAllocCnt objFreeCnt objInitCnt
		     objTerminateCnt createRtn initRtn destroyRtn
		     showRtn helpRtn} $classInfoList

    # Give more suitable format

    set objSize [format "%d" $objSize]
    set objAllocCnt [format "%d" $objAllocCnt]
    set objFreeCnt [format "%d" $objFreeCnt]
    set objInitCnt [format "%d" $objInitCnt]
    set objTerminateCnt [format "%d" $objTerminateCnt]

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
                error $routineSym
            }
        } else {
            set routineName [lindex $routineSym 0]
        }

	lappend routinesList [format "%-10s (%s)" $routineAdrs $routineName]
        incr offset
    }

    bindNamesToList {creatRtnInfo initRtnInfo destrRtnInfo showRtnInfo 
		    helpRtnInfo} $routinesList

    # Return a somewhat formated structure

    return "$objPartId $objSize $objAllocCnt $objFreeCnt
	    $objInitCnt $objTerminateCnt
	    {\"$creatRtnInfo\"
	     \"$initRtnInfo\"
	     \"$destrRtnInfo\"
	     \"$showRtnInfo\"
	     \"$helpRtnInfo\"}"
}

##############################################################################
#
# blockedTaskArrayConstruct - construct a hierarchy array of blocked tasks
#
# This routine takes a straight linear list of blocked task information (as
# might be returned from a gopher evaluation) and returns a structured list
# that can be used as the value of an array node in a hierarchy.  For each
# blocked task in the list, the delay count is computed based on the timeout
# and vxClock value.
#
# SYNOPSIS:
#   blockedTaskArrayConstruct vxClock btList
#
# PARAMETERS:
#   vxClock: the value of vxTicks at the time the sample was taken
#   btList: the list of blocked task IDs.
#
# RETURNS:
#   A list with as many elements as blocked tasks in the btList.  Each 
#   element of the list is itself a list with 4 elements:
#     taskName taskId priority delayCount
#   where delay count has been computed from the timeout and vxClock value,
#   if the status word has the DELAY bit set.
#
# ERRORS:
#   NONE

proc blockedTaskArrayConstruct {vxClock btList} {

    set array ""

    foreach bTask $btList {

	set blockedTaskInfo [taskBlockInfo $bTask]

	set stat [lindex $blockedTaskInfo 1]
	if {$stat & 0x4} {
	    set timeout [expr [lindex $blockedTaskInfo 0] - $vxClock]
	} {
	    set timeout 0
	}
	
	lappend array [format "%s 0x%x %d %d" \
		[lindex $blockedTaskInfo 3] \
		$bTask			    \
		[lindex $blockedTaskInfo 2] \
		$timeout]

    }

    return $array
}

##############################################################################
#
# smBlockedTaskArrayConstruct - construct a hierarchy array of tasks blocked
#                               on a shared object
#
# This routine takes a straight linear list of blocked task information (as
# might be returned from a gopher evaluation) and returns a structured list
# that can be used as the value of an array node in a hierarchy.
#
# SYNOPSIS:
#   smBlockedTaskArrayConstruct btList
#
# PARAMETERS:
#   btList: the list of blocked task IDs.
#
# RETURNS:
#   A list with as many elements as blocked tasks in the btList.  Each 
#   element of the list is itself a list with 3 elements:
#      taskId cpuNum sharedTcb
#
# ERRORS:
#   NONE

proc smBlockedTaskArrayConstruct {btList} {

    set array ""

    foreach bTask $btList {

	set blockedTaskInfo [smTaskBlockInfo $bTask]

	lappend array [format "%#x %d %#x" \
		[lindex $blockedTaskInfo 0] \
		[lindex $blockedTaskInfo 1] \
		$bTask]
    }

    return $array
}

##############################################################################
#
# globalBrowseListAdd - Add "item" to the globalBrowseList
#
# If item already exists, it is not added.  If it is not already present,
# it is appended to the list.  This routine is not concerned with the 
# structure of "item", but it is expected to be a list with three elements:
#   className objId browserName
#
# SYNOPSIS:
#   globalBrowseList item
#
# PARAMETERS:
#   item: the item to add 
#
# RETURNS:
#   NONE
#
# ERRORS:
#   NONE

proc globalBrowseListAdd {item} {
    global globalBrowseList
    
    if {[lsearch $globalBrowseList $item] == -1} {
	lappend globalBrowseList $item
    }
}

##############################################################################
#
# globalBrowseListRemove - remove "item" from the globalBrowseList
#
# The item is removed from the globalBrowseList, if it exists.
#
# SYNOPSIS:
#   globalBrowseListRemove item
#
# PARAMETERS:
#   item: the item to remove
#
# RETURNS:
#   NONE
#
# ERRORS:
#   NONE

proc globalBrowseListRemove {item} {
    global globalBrowseList

    set listIx [lsearch $globalBrowseList $item]
    if {$listIx != -1} {
	set globalBrowseList [lreplace $globalBrowseList $listIx $listIx]
    }
}

proc globalBrowseListRemoveByName {name} {
    global globalBrowseList

    set tmpBrowseList {}

    foreach item $globalBrowseList {
	if {[lindex $item 2] != $name} {
	    lappend tmpBrowseList $item
	}
    }

    set globalBrowseList $tmpBrowseList
}
    
# Load in the core Tcl functions.

source [wtxPath host resource tcl]shelcore.tcl

proc shellBrowseInit {} {
    global globalBrowseList
    shellCoreInit

    # Initialize a globalBrowseList variable, if one is not already present.

    if ![info exists globalBrowseList] {
	set globalBrowseList ""
    }
}

# 
# source i2o shell browser file if exists
#

if {![catch {file exists [wtxPath host resource tcl]i2oShelBrws.tcl} result] \
	&& $result} {
	source [wtxPath host resource tcl]i2oShelBrws.tcl
}
