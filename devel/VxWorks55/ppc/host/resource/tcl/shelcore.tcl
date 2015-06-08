# shelcore.tcl - Implementation of shell core routines in Tcl
#
# Copyright 1994-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 02e,07nov01,aeg  updated taskInfoGetVerbose, semInfoGet, and msgQInfoGet
#		   to return VxWorks events info.
# 02d,02oct01,tam  fixed memShow display (SPR 70538 & 70236)
# 02c,10may01,tpw  Add VX_ALTIVEC_TASK to tcbOptionBit array.
# 02b,15mar00,zl   merged SH support from T1
# 02a,05feb99,tcr  Added work-around for broken wtxEventPoll (& raised SPR24986)
# 01z,29jan99,jmp  fixed SPR# 24612: windsh can lose event unhandled events.
# 01y,03dec98,dbt  enabled initialization of instrumented classes (SPR #23643).
# 01x,28sep98,fle  fixed presence of 0x or not in disassembled address in
#                  nextAddrGet
# 01w,22sep98,fle  fixed a typo in nextAddrGet
# 01v,14sep98,fle  modified nextAddrGet since wtxMemDisassemble new output
# 01u,01jul98,c_s  WindView 2 -> Tornado 2 Merge
# 01t,02apr98,nps  removed redundant and troublesome code from rBuffInfoGet and
#                  trgInfoGet.
# 01s,12feb98,pr   Added hitCnt to trigger offsets.
# 02r,02dec97,nps  Support new rBuff statistic.
# 02q,20nov97,pr   added in trgInfoGet.
# 02p,25sep97,nps  added in rBuffInfoGet.
# 02o,25jul97,nps  added in new class [name] rBuff.
# 02t,09jun98,dbt  modified targetInitCheck() routine to return 0 (system not
#		   initialized) if the symbol activeQHead can not be found
#		   in the symbol table.
# 02s,03jun98,jmp  modified wtxEventPoll to use shAsyncEventGet{} if the
#                  async. event notification is started.
# 02s,20may98,drm  merged in 3rd party changes to add support for
#                  distributed objects
#                  - original changes were made to ver. 02n in 3rd party code
# 02r,05may98,rlp  adapted semContents syntax to Tcl 8.0.
# 02q,24mar98,dbt  added taskIdCurrent() proc.
# 02p,23feb98,dbt  improved targetInitCheck() routine to call it only once if
#                  the OS is initialized. Added targetInitCheck() call in
#		   some OS dependant routines.
#		   Added Copyright.
# 02o,12nov97,fle  moving dbgNextAddrGet from all dbgXxxxLib.tcl files here 
#                  (renamed nextAddrGet) since wtxMemDisassemble is OS
#                  agnostic
# 02n,12dec96,c_s  fix TSR 37398
# 02m,18oct96,pad  Added convenient routine shSymValue.
# 02l,17oct96,sjw  fix initClassNames{} to handle failure of gopher which can
#		   occur if kernel isn't initialized yet.
# 02l,04nov96,mem  added use of targetTool{}
# 02k,11oct96,pad  added targetInitCheck{} as a partial answer to SPR #7296.
#                  This routine is actually the exact clone of
#                  multitaskingCheck{} (Thanks Ben S.).
# 02j,29jun96,jank i2o class name initialization if i2o is defined
# 02i,27jun96,jank source i2o core file if exists; install procedure takes care
#                    of copying this file or not	
# 02h,30apr96,p_m  fixed SPR #6409 by getting sp and pc from the stack
# 		   for all tasks except the tWdbTask.
# 02g,02feb96,jco  fixed SPR #5952 (prevented querring className(0) in
#		    taskInfoGet routine).
# 02f,20dec95,c_s  fix SPR #5708
# 02e,20nov95,p_m  changed [] to {} in smObjInfoInit.
# 02d,13nov95,c_s  removed 'coff' from library path for 960 modules.
# 02c,30oct95,pad  Modified objectClassify{} to take care of communication loss
#		     with the license daemon.
#		   Reworked error handling in smObjInfoInit{}.
# 02b,27oct95,c_s  added routine members to classInfoGet (SPR #4567).
# 02a,19oct95,c_s  tweaked vxObjectLoad for i960.
# 01z,28sep95,pad  Now display an appropriate message when VxMP is not
#		     installed in VxWorks but smMemShow or fake shared object
#		     ids are used.
# 01y,19sep95,pad  added Shared Memory Object management for show routines.
#		   Added msgGet{} and memSummaryPrint{} routines.
# 01x,12sep95,pad  added semPx and mq class names (POSIX sem and POSIX msgQ)
#                    plus infoGet routines.
# 01w,08sep95,c_s  shSymAddr now resubmits errors not relating to missing
#                    symbols (relevant to SPR #4738); added an internal 
#                    routine, vxObjectLoad, to aid in test suite development.
# 01v,30aug95,jco  removed bindNamesToList (rewritten in C).
# 01u,08aug95,c_s  fixed code for stack check on "stack grows up" 
#                    architectures.
# 01t,26jul95,pad  Added code in objectClassify{} to handle better non-valid
#                    objects submitted to the Browser's Show facility. (SPR
#                    #4538).
# 01s,05jul95,c_s  reworked semInfo and msgQInfo so they just return the IDs
#                    of blocked tasks; then fixed blocked task info gatherers
#                    to fill in the blanks themselves.
# 01r,19jun95,c_s  removed wtxInterrupt calls.
# 01q,12jun95,c_s  fixed initClassNames so it handles nonexistent classes 
#                    correctly.
# 01p,31may95,c_s  added tcbOptionBit array; now uses wtxMemScan for stack
#                    checking.  Added taskStackInfo.
# 01o,30may95,c_s  added interrupt handling to wtxEventPoll.
# 01n,12may95,c_s  fixed shSymAddr for new wtxtcl; added wtxEventPoll.
# 01m,24apr95,c_s  moved shSymbolicAddr to shell.tcl; now uses wtxPath;
#                    removed obsolute cpu number table (see wtxcore.tcl).
# 01l,27mar95,c_s  added memPartInfoGetShort.
# 01k,22mar95,c_s  added activeQueue; reworked taskInfoGet.
# 01j,20mar95,c_s  removed event management material, taskIdDefault stuff
#                    to shell.tcl.
# 01i,16mar95,c_s  adjusted shFuncCall's syntax; now calls wtxFuncCall with
#                    a passed return type argument.  Now handles case where
#                    called function receives a breakpoint.
# 01h,15mar95,c_s  improved printouts in BREAKPOINT_Handler.
# 01g,08mar95,tpr  added EXCEPTION_Handler.
# 01f,06mar95,p_m  took care of 5.2 CPU naming.
# 01e,22feb95,c_s  added taskIdNameStatusList
# 01d,22jan95,c_s  replace inline initialization with shellCoreInit proc.
#                    removed some material to wtxcore.tcl, which we now
#                    source in.
# 01c,19jan95,c_s  added shSymbolicAddr from shell.tcl and improved.  Added
#                    taskIdDefault support from shell.tcl.  Added polling and
#                    event dispatch functions.
# 01b,02jan95,c_s  changed intel cpu family from "i386" to "i86".
# 01a,07dec94,c_s  extracted from "shell.tcl".
#*/

#|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
# 
# GLOBAL VARIABLES
#

# The list of known class names.

set classNameList {qJob class qPriListFromTail symTbl memPart \
		   sem semPx qPriBMap task qFifo wd qPriList hash   \
		   module msgQ mq fp rBuff trg}

# The map of task option bits.

set tcbOptionBit(VX_SUPERVISOR_MODE)    0x0001	
set tcbOptionBit(VX_UNBREAKABLE)	0x0002	
set tcbOptionBit(VX_DEALLOC_STACK)  	0x0004	
set tcbOptionBit(VX_FP_TASK)	   	0x0008	
set tcbOptionBit(VX_STDIO)	   	0x0010	
set tcbOptionBit(VX_ADA_DEBUG)	   	0x0020	
set tcbOptionBit(VX_FORTRAN)	   	0x0040	
set tcbOptionBit(VX_PRIVATE_ENV) 	0x0080
set tcbOptionBit(VX_NO_STACK_FILL)	0x0100	
set tcbOptionBit(VX_DSP_TASK)		0x0200	
set tcbOptionBit(VX_ALTIVEC_TASK)	0x0400	

# a memblock to hold "0xee", for stack checking.

set eeMemBlock ""

# The address minus one of the shared object pool

set smObjPoolMinusOne -1

# The address of the Shared Memory Header

set smObjHdrAdrs 0

#|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
# 
# TASKS
#

##############################################################################
#
# taskInfoGetAll - return a taskInfoList for each running task
#
# SYNOPSIS:
#   taskInfoGetAll
#
# PARAMETERS:
#   NONE
#
# RETURNS:
#   The current value of vxTicks, followed by the following 10 items for
#   each running task:
#     taskId status priority pc sp errno timeout entry priNormal name
#   The resulting list will have 10*n+1 items, where n is the number
#   of running tasks.
#
# ERRORS:
#   NONE

proc taskInfoGetAll {} {
    global offset
    global iCommandGopher

    # Check that the target system has been correctely initialized before
    # proceeding

    if {! [targetInitCheck]} {
	error "The target system is not initialized yet. Can't proceed."
    }

    set aqHead [shSymAddr activeQHead]
    set vxTicks [shSymAddr vxTicks]
    return [wtxGopherEval "$vxTicks @
			   $aqHead * { < -$offset(WIND_TCB,activeNode) ! 
			       $iCommandGopher
			       >*}"]
}

##############################################################################
#
# taskInfoGet - get information about a particular task
#
# taskInfoGet gets enough information for one row of the "i" command, 
# but also performs the service of validating that the given object
# is in fact still a task, by checking the objCore pointer.
#
# SYNOPSIS:
#   taskInfoGet taskId
#
# PARAMETERS:
#   taskId: the task to fetch information about
#
# RETURNS:
#   A list of 11 items:
#     vxTicks taskId status priority pc sp errno timeout entry priNormal name
#
# ERRORS:
#   NONE

proc taskInfoGet {taskId} {
    global className
    global offset
    global iCommandGopher
    global __wtxCpuType
    global cpuFamily

    set tiList [wtxGopherEval "[shSymAddr vxTicks] @
                           $taskId !
                           < +$offset(WIND_TCB,objCore) @ >
			   $iCommandGopher"]

    # Pick out the objCore and compare it to taskClassId.

    set objCore [lindex $tiList 2]
    if {$objCore != 0} {
	if {$className($objCore) != "task"} {
	    error "not a task"
	}
    }

    # if this is the WDB task for the i960 then go get the sp value
    # from the TCB

    set name [lindex $tiList 11]

    if {($cpuFamily($__wtxCpuType) == "i960") && ($name == "tWdbTask")} {
       set pc [wtxGopherEval "$taskId <+$offset(WIND_TCB,pc) @"]
       set sp [wtxGopherEval "$taskId <+$offset(WIND_TCB,sp) @"]

       set tiList [lreplace $tiList 5 6 $pc $sp]
       }

    return [concat [lrange $tiList 0 1] [lrange $tiList 3 end]]
}

##############################################################################
#
# taskInfoGetVerbose - get a lot of information about a particular task
#
# SYNOPSIS:
#   taskInfoGetVerbose taskId
#
# PARAMETERS:
#   taskId: the task to fetch information about
#
# RETURNS:
#   First comes the same list of 11 items returned by taskInfoGet, followed by:
#     options pStackBase pStackEnd pStackLimit pStackMax events registers
#   ...where registers are prepared by evaluating regSetGopher, supplied 
#   in an architecture- specific module, and pStackMax is obtained from 
#   a memory scan.  pStackMax will be set 0 if the TCB options word contains
#   the VX_NO_STACK_FILL bit.
#
# ERRORS:
#   NONE

proc taskInfoGetVerbose {taskId} {
    global stackGrows
    global tcbOptionBit
    global iCommandGopher
    global regSetGopher
    global eeMemBlock
    global offset
    global __wtxCpuType
    global cpuFamily

    set list [wtxGopherEval "[shSymAddr vxTicks] @ 
                           $taskId ! 
			   $iCommandGopher
			   <+$offset(WIND_TCB,options) @>
			   <+$offset(WIND_TCB,pStackBase) @>
			   <+$offset(WIND_TCB,pStackEnd) @>
			   <+$offset(WIND_TCB,pStackLimit) @>
 			   <+$offset(WIND_TCB,events) @@@b@b>"]

    set options [lindex $list 11]
    set pStackBase [lindex $list 12]
    set pStackLimit [lindex $list 14]
    set name [lindex $list 10]

    if {! ($options & $tcbOptionBit(VX_NO_STACK_FILL))} {
	set stackMax [wtxMemScan -notmatch \
		$pStackLimit $pStackBase -memblk $eeMemBlock]
    } {
	set stackMax 0
    }
    set list [linsert $list 15 $stackMax]

    # if this is the WDB task for the i960 then go get the registers value
    # from the TCB

    if {($cpuFamily($__wtxCpuType) == "i960") && ($name == "tWdbTask")} {
	set list [concat $list [wtxGopherEval "$taskId <+$offset(WIND_TCB,regs) @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@>"]]
    } {
	set list [concat $list [wtxGopherEval "$taskId $regSetGopher"]]
    }

    return $list
}

##############################################################################
#
# taskStackInfo - get information about a task's stack usage.
#
# SYNOPSIS:
#   taskStackInfo taskId
#
# PARAMETERS:
#   taskId: the task to fetch information about
#
# RETURNS:
#   A vector with the following elements:
#      taskId
#      name
#      entry
#      sp
#      pStackBase
#      pStackLimit
#      pStackMax
#   pStackMax will be zero for those tasks that have the NO_STACK_FILL bit
#   set in their TCBs.

proc taskStackInfo {taskId} {
    global stackGrows
    global tcbOptionBit
    global eeMemBlock
    global offset
    global cpuFamily
    global __wtxCpuType
    global spGetGopher

    set list [wtxGopherEval "$taskId ! 
			     <+$offset(WIND_TCB,name) *$>
			     <+$offset(WIND_TCB,entry) @>
			     $spGetGopher
			     <+$offset(WIND_TCB,options) @>
			     <+$offset(WIND_TCB,pStackBase) @>
			     <+$offset(WIND_TCB,pStackLimit) @>"]

    bindNamesToList {dummy name entry sp options pStackBase pStackLimit} $list

    # if this is the WDB task for the i960 then go get the sp value
    # from the TCB

    if {($cpuFamily($__wtxCpuType) == "i960") && ($name == "tWdbTask")} {
       set sp [wtxGopherEval "$taskId <+$offset(WIND_TCB,sp) @"]
       }

    if {! ($options & $tcbOptionBit(VX_NO_STACK_FILL))} {
	set stackMax [wtxMemScan -notmatch \
		$pStackLimit $pStackBase -memblk $eeMemBlock]
    } {
	set stackMax 0
    }

    return [list $taskId $name $entry $sp $pStackBase $pStackLimit $stackMax]
}


##############################################################################
#
# taskStatus - return a string representation of a task's status code
#
# SYNOPSIS:
#   taskStatus stat inherit
#
# PARAMETERS:
#   status: the status word from the TCB of some task
#   inherit: whether the task has an inherited priority
#
# RETURNS:
#   The traditional name of the state given by the "i" command in VxWorks:
#   one of {READY,SUSPEND,PEND,DELAY,DEAD}, possibly followed by one or 
#   more of "+S", "+T", or "+I".  An unknown status code is represented as 
#   a hex number followed by "?".
#
# ERRORS:
#   NONE

proc taskStatus {stat inherit} { 
    case [expr $stat] {
        0	{set s READY}
        1	{set s SUSPEND}
	2	{set s PEND}
	3	{set s PEND+S}
	4	{set s DELAY}
	5	{set s DELAY+S}
        6	{set s PEND+T}
	7	{set s PEND+S+T}
	8	{set s DEAD}
	*	{set s [format "%#x?" $stat]}
    }
    if $inherit {set s $s+I}
    return $s
}

##############################################################################
#
# activeTaskNameMap - return a list of active task IDs and names
#
# SYNOPSIS:
#   activeTaskNameMap
#
# PARAMETERS:
#   NONE
#
# RETURNS:
#   A list with 2 elements for each running task.  Each task contributes
#   a hexadecimal ID and a string name to the list, e.g.
#     id1 name1 id2 name2 ...
#
# ERRORS:
#   NONE

proc activeTaskNameMap {} {
    global offset

    # Check that the target system has been correctely initialized before
    # proceeding

    if {! [targetInitCheck]} {
	error "The target system is not initialized yet. Can't proceed."
    }

    set aqHead [shSymAddr activeQHead]
    set taskNameMap [wtxGopherEval "$aqHead * 
                                        { < -$offset(WIND_TCB,activeNode) ! 
                                          <+$offset(WIND_TCB,name) *$>> 
                                        *}"]
    return $taskNameMap
}

##############################################################################
#
# activeQueue - return the active queue (task IDs only)
#
# SYNOPSIS:
#   activeQueue
#
# PARAMETERS:
#   priority - if true, fetch priorities along with task IDs
#
# RETURNS:
#   a list of task IDs (and priorities, if priority is true).
#
# ERRORS:
#   NONE

proc activeQueue {priority} {
    global offset

    # Check that the target system has been correctely initialized before
    # proceeding

    if {! [targetInitCheck]} {
	error "The target system is not initialized yet. Can't proceed."
    }

    set aqHead [shSymAddr activeQHead]
    if {$priority} {
	set aq [wtxGopherEval "$aqHead * 
	                       { <-$offset(WIND_TCB,activeNode) !
	                           <+$offset(WIND_TCB,priority) @>> * }"]
    } {
	set aq [wtxGopherEval "$aqHead * 
	                       { <-$offset(WIND_TCB,activeNode) !> * }"]
    }
    return $aq
}

##############################################################################
#
# taskIdNameStatusList -
#
# SYNOPSIS:
#   taskIdNameStatusList
#
# PARAMETERS:
#   NONE
#
# RETURNS:
#   A list with 5 elements for each running task.  Each task contributes
#   a hexadecimal ID, a string name, and a status word to the list and 
#   the current and normal priority, e.g.
#     id1 name1 stat1 p1 np1 id2 name2 stat2 p2 np2...
#
# ERRORS:
#   NONE

proc taskIdNameStatusList {} {
    global offset
    set aq [activeQueue 0]
    set list ""
    
    foreach tid $aq {
	set query "$tid <+$offset(WIND_TCB,name) *$> 
	                <+$offset(WIND_TCB,status) @> 
                        <+$offset(WIND_TCB,priority) @>
                        <+$offset(WIND_TCB,priNormal) @>"

	if {! [catch {wtxGopherEval $query} result]} {
	    lappend list $tid [lindex $result 0] [lindex $result 1] \
		    [lindex $result 2] [lindex $result 3]
	}
    }

    return $list
}

##############################################################################
#
# taskBlockInfo - get information about a blocked task
#
# This call is used to get the information printed about tasks blocked on 
# a sempahore or message queue.
#
# SYNOPSIS:
#   taskBlockInfo taskId
#
# PARAMETERS:
#   taskId - the ID of the task to get information about
#
# RETURNS:
#   A list with 4 elements:
#     timeout     the ticks remaining before the block times out
#     status      the task's status word
#     priority    the task's priority
#     name        the task's name
#
# ERRORS:
#   NONE

proc taskBlockInfo {taskId} {
    global offset
    
    set result [wtxGopherEval "$taskId <+$offset(WIND_TCB,timeout) @>
				       <+$offset(WIND_TCB,status) @>
				       <+$offset(WIND_TCB,priority) @>
				       <+$offset(WIND_TCB,name) *$>"]
    return $result
}

##############################################################################
#
# smTaskBlockInfo - get information about a task blocked on a shared object.
#
# This call is used to get the information printed about tasks blocked on 
# a shared sempahore or shared message queue.
#
# SYNOPSIS:
#   smTaskBlockInfo sharedTcbAddr
#
# PARAMETERS:
#   sharedTcbAddr - the address of the task's shared TCB to get information
#                   about
#
# RETURNS:
#   A list with 2 elements:
#     taskId      the ID of the blocked task.
#     cpuNum      the CPU number on which the task is blocked.
#
# ERRORS:
#   NONE

proc smTaskBlockInfo {sharedTcbAddr} {
    global offset
    
    set result [wtxGopherEval "$sharedTcbAddr
			       <+$offset(SM_OBJ_TCB,localTcb) @>
			       <+$offset(SM_OBJ_TCB,ownerCpu) @>"]
    return $result
}

##############################################################################
#
# taskIdToName - given a task ID, return the task's name
#
# This routine calls activeTaskNameMap to figure out the name of a task 
# given its ID.
#
# SYNOPSIS:
#   taskIdToName id
#
# PARAMETERS:
#   id: the ID of the task to look for 
#
# RETURNS:
#   the name of the task matching the given ID, or an empty string if none
#   is found
#
# ERRORS:
#   NONE

proc taskIdToName {id} {
    set taskNameMap [activeTaskNameMap]
    set listIx [lsearch $taskNameMap [format "0x%x" $id]]
    if {$listIx != -1} {
	return [lindex $taskNameMap [expr $listIx + 1]]
    }
    return ""
}

##############################################################################
#
# taskNameToId - given a task name, return its ID
#
# This routine calls activeTaskNameMap to figure out the ID of a task
# given its name.
#
# SYNOPSIS:
#   taskNameToId name
#
# PARAMETERS:
#   name: the name of the task
#
# RETURNS:
#   the ID of the task matching the given name, or 0 if none
#   is found
#
# ERRORS:
#   NONE

proc taskNameToId {name} {
    set taskNameMap [activeTaskNameMap]
    set listIx [lsearch $taskNameMap $name]
    # the item we hit must have an odd index if we've hit a task name.
    # But this can still be confused by a task with a numeric name XXX
    if {$listIx != -1 && $listIx & 1} {
	return [lindex $taskNameMap [expr $listIx - 1]]
    }
    return 0
}

# global variable defaultTaskId - used by the taskIdDefault function,
# which simulates the one in the traditional shell and is used by the
# shell convenience functions.

set defaultTaskId 0



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

proc taskIdDefault {args} {
    global defaultTaskId
    bindNamesToList id $args
    if {[expr $id] != 0} {
	set defaultTaskId [format "%#x" [expr $id]]
    }
    if {$defaultTaskId == 0} {
	error "No default task has been established."
    }
    return $defaultTaskId
}

##############################################################################
#
# taskIdCurrent - return the current task ID
#
# SYNOPSIS:
#   taskIdCurrent
#
# PARAMETERS:
#   NONE
#
# RETURNS:
#   The current task ID
#
# ERRORS:
#   "The target system is not initialized yet. Can't proceed."

proc taskIdCurrent {} {
    # Check that the target system has been correctely initialized before
    # proceeding

    if {! [targetInitCheck]} {
	error "The target system is not initialized yet. Can't proceed."
    }

    set taskIdCurrent [shSymValue taskIdCurrent]
    return $taskIdCurrent
}

#|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
# 
# SEMAPHORES
#

##############################################################################
#
# semType - return the string form of a semaphore's type semType field
#
# SYNOPSIS:
#   semType type
#
# PARAMETERS:
#   type: the value of the semType field of a semaphore
#
# RETURNS:
#   one of BINARY, MUTEX, COUNTING, OLD, SHARED_BINARY or SHARED_COUNTING,
#   or 0x%x? if the type is not expected.
#
# ERRORS:
#   NONE

proc semType type { 
    case [expr $type] {
        0	{set s BINARY}
	1	{set s MUTEX}
	2	{set s COUNTING}
	3	{set s OLD}
	4	{set s SHARED_BINARY}
	5	{set s SHARED_COUNTING}
	*	{set s [format "0x%x?" $type]}
	    }
    return $s
}

##############################################################################
#
# semQueueType - return the string form of a semaphore's queueing type
#
# SYNOPSIS:
#   semQueueType opt
#
# PARAMETERS:
#   opt: the value of the options field of a semaphore
#
# RETURNS:
#   FIFO or PRIORITY, or ??? if the type is not recognized.
#
# ERRORS:
#   NONE

proc semQueueType opt {
    case [expr {$opt & 0x3}] { 
        0	{set t FIFO}
        1	{set t PRIORITY}
        *       {set s {???}}
    }
}
         
##############################################################################
#
# semContents - return the string form of a semaphore's "contents"
#
# SYNOPSIS:
#   semContents type contents
#
# PARAMETERS:
#   type: 0, 1, or 2, according as the semaphore is BINARY, MUTEX, or COUNTING
#   contents: the value of the "state" union of a semaphore
#
# RETURNS:
#   For binary semaphores, EMPTY or FULL.
#   For mutex semaphores, "Owner=<taskId>" or "NotOwned".
#   For counting semaphores, the count in decimal.
#   For other semaphores, The value of "contents" in hex.
#
# ERRORS:
#   NONE

proc semContents {type contents} {
    case [expr $type] {
	0	{
		# BINARY.  Report FULL or EMPTY.
                if {$contents != 0} { return "EMPTY"
                } else { return "FULL" }
		}
	1	{
		# MUTEX.  Report Owner.
		if $contents \
			{ return [format "Owner=0x%x" $contents] } \
			{ return [format "NotOwned"] }
		# Unlike VxWorks, this code does not check to see if
		# the owning task has been deleted.
		}
	2	{
		# COUNTING.  Report Count.
		return [format "%d" $contents]
		}
	4	{
		# SHARED_BINARY
                if {$contents != 0} { return "FULL"
                } else { return "EMPTY" }
		}
	5	{
		# SHARED_COUNTING
		return [format "%d" $contents]
		}
	*	{
		# Some other kind.  Report 'state.'
		return [format "0x%x" $contents] 
		}
    }
}

##############################################################################
#
# semInfoGet - get a list of information about a semaphore, given its ID.
#
# SYNOPSIS:
#   semInfoGet semId
#
# PARAMETERS:
#   semId: ID of the semaphore to return information about
#
# RETURNS:
#   A list with the following elements:
#     vxClock      -- the value of vxTicks when the gopher was executed
#     semType      -- the semType field of the semaphore
#     options      -- the options field of the semaphore
#     count        -- the "count" field of the semaphore (actually a union)
#     evRegistered -- the events.registered field of the semaphore (only local)
#     evTaskId     -- the events.taskId field of the semaphore     (only local)
#     evOptions    -- the events.options field of the semaphore    (only local)
#   In addition, for each task blocked on the semaphore, if the semaphore is
#   local a task ID will be written, if the semaphore is shared, the shared
#   TCB address will be written instead.
#
#   Hence semInfoGet will return list with 7 + k elements for local semapahores
#   and 4 + k for shares semaphores, where k is the 
#   number of blocked tasks.
#
# ERRORS:
#   NONE

proc semInfoGet {semId} {
    global offset

    set vxTicks [shSymAddr vxTicks]

    # Determine whether the semaphore is local or shared.

    if {[objIsShared $semId]} {

	#
	# The format to hexidecimal is used such that unsigned arithmetic
	# is performed by the gopher parser for situations when shared
	# memory addresses are greater than 0x7fffffff.  This is required
	# since the 'expr' statement in smObjIdToAdrs returns a decimal
	# formated number preceded with a negative sign when an address ID
	# is greater than 0x7fffffff.
	# 

	set smSemId [format "0x%x" [smObjIdToAdrs $semId]]
	set smObjPoolAddr [smObjPoolAddrGet]
	set fifo 0
	
	set semGopherString "$vxTicks @
			     $smSemId <+$offset(SM_SEM,objType) @>
			     $fifo !
			     $smSemId <+$offset(SM_SEM,count) @>
			     +$offset(SM_SEM,smPendQ) *
			     {+$smObjPoolAddr ! *}"

    } else {

	set semGopherString "$vxTicks @
			     $semId <+$offset(SEMAPHORE,semType) @b>
				    <+$offset(SEMAPHORE,options) @b>
				    <+$offset(SEMAPHORE,count) @> 
				    <+$offset(SEMAPHORE,events) @@@b>
				    +$offset(SEMAPHORE,qHead) * { ! *}"
    }

    return [wtxGopherEval $semGopherString]
}

##############################################################################
#
# semPxInfoGet - get a list of info about a POSIX semaphore, given its ID.
#
# SYNOPSIS:
#   semPxInfoGet semId
#
# PARAMETERS:
#   semId: ID of the POSIX semaphore to return information about
#
# RETURNS:
#   A list with the following elements:
#     semId        -- the ID of the underlying WIND semaphore
#     refCnt       -- the number of sem_open operations done on this semaphore
#     sem_name     -- the semaphore name (null if it is an unnamed semaphore)
#     count        -- the "count" field of the semaphore (actually a union)
#   In addition, for each task blocked on the semaphore, a task ID will
#   be written.
#
#   Hence semPxInfoGet will return list with 4 + k elements, where k is the
#   number of blocked tasks.
#
# ERRORS:
#   NONE

proc semPxInfoGet {semId} {
    global offset

    return [wtxGopherEval "$semId <+$offset(POSIX_SEM,semId) @>
                                  <+$offset(POSIX_SEM,refCnt) @>
                                  <+$offset(POSIX_SEM,sem_name) @>
                                  +$offset(POSIX_SEM,semId) *
                                  <+$offset(SEMAPHORE,count) @>
                                  +$offset(SEMAPHORE,qHead) *
                                  { ! *}"]
}

#|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
# 
# WATCHDOGS
#


##############################################################################
#
# wdInfoGet - return a list of information about a watchdog, given its ID.
#
# SYNOPSIS:
#   wdInfoGet wdId
#
# PARAMETERS:
#   wdId: the ID of the watchdog to query
#
# RETURNS:
#   A list containing the following 5 elements:
#     the value of vxTicks;
#     the values of the following members of the WDOG structure:
#       timeout status routine parameter
#
# ERRORS:
#   NONE

proc wdInfoGet {wdId} {
    global offset
    set vxTicks [shSymAddr vxTicks]
    return [wtxGopherEval "$vxTicks @
			   $wdId 
			       <+$offset(WDOG,timeout) @> 
			       <+$offset(WDOG,status) @w>
			       <+$offset(WDOG,routine) @>
			       <+$offset(WDOG,parameter) @>"]
}

##############################################################################
#
# wdStateName - return the string form of the watchdog state
#
# SYNOPSIS:
#   wdStateName state
#
# PARAMETERS:
#   state: the value of the status member of a watchdog structure
#
# RETURNS:
#   OUT_OF_Q, IN_Q, DEAD, or 0x%x? if the state is not recognized.
#
# ERRORS:
#   NONE

proc wdStateName state { 
    case [expr $state] {
        0	{set s OUT_OF_Q}
	1	{set s IN_Q}
	2	{set s DEAD}
	*	{set s [format 0x%x?]}
    }
    return $s
}

#|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
# 
# MESSAGE QUEUES
#

##############################################################################
#
# msgQType - return the string form of a message queue's type
#
# SYNOPSIS:
#   msgQType opt
#
# PARAMETERS:
#   opt: the options word of the message queue structure
#
# RETURNS:
#   FIFO, PRIORITY or ??? if the queuing type is unexpected
#
# ERRORS:
#   NONE

proc msgQType opt { 
    case [expr {$opt & 0x1}] { 
	0	{set s FIFO}
	1	{set s PRIORITY}
	*	{set s {???}}
    }
    return $s
}


##############################################################################
#
# msgQInfoGet - return information about a message queue
#
# SYNOPSIS:
#   msgQInfoGet msgQId
#
# PARAMETERS:
#   msgQId: the ID of the message queue to query
#
# RETURNS:
#   A fairly involved list of data returned by a gopher expression.
#   The first several elements are static:
#     vxTicks -- the value of vxTicks at the time of the gopher evaluation
#     options -- this and the following are unpacked from the msgQ structure
#     maxMsgs
#     maxMsgLength
#     sendTimeouts
#     recvTimeouts
#     evRegistered -- the events.registered field of the msg Q (only local)
#     evTaskId     -- the events.taskId field of the msg Q     (only local)
#     evOptions    -- the events.options field of the msg Q    (only local)
#   If there are any receivers blocked on the message queue, for each 
#   of them a task ID will be present:
#     taskId...
#
#   Then follows the sentinal value "0xee":
#     0xee
#
#   If there are any senders blocked on the message queue, for each of them
#   there will be a task ID:
#     taskId...
#
#   There follows the sentinel value "0xee":
#     0xee
#
#   If there are any messages in the message queue, for each of them the 
#   following 2 pieces of information are placed on the list:
#     address
#     length
#
# To summarize the return value:
#
#   vxTicks options maxMsgs maxMsgLength sendTimeouts recvTimeouts events
#   taskId... 					# 0 or more of these sets
#   0xee
#   taskId... 					# 0 or more of these sets
#   0xee
#   address length				# 0 or more of these sets
#
# ERRORS:
#   NONE

proc msgQInfoGet {msgQId} {
    global offset

    set vxTicks [shSymAddr vxTicks]

    # Determine whether the message queue is local or shared.

    if {[objIsShared $msgQId]} {

	#
	# The format to hexidecimal is used such that unsigned arithmetic
	# is performed by the gopher parser for situations when shared
	# memory addresses are greater than 0x7fffffff.  This is required
	# since the 'expr' statement in smObjIdToAdrs returns a decimal
	# formated number preceded with a negative sign when an address ID
	# is greater than 0x7fffffff.
	# 

	set smMsgQId [format "0x%x" [smObjIdToAdrs $msgQId]]
	set smObjPoolAddr [smObjPoolAddrGet]

	set msgQGopher "$vxTicks @
			$smMsgQId <+$offset(SM_OBJ,objType) @>
			<+$offset(SM_MSG_Q,maxMsgs) @>
			<+$offset(SM_MSG_Q,maxMsgLength) @>
			<+$offset(SM_MSG_Q,sendTimeouts) @>
			<+$offset(SM_MSG_Q,recvTimeouts) @>
			<+$offset(SM_MSG_Q,freeQSem)
			    +$offset(SM_SEM,smPendQ) *
			    {+$smObjPoolAddr ! *} 0xee!>
			<+$offset(SM_MSG_Q,msgQSem)
			    +$offset(SM_SEM,smPendQ) *
			    {+$smObjPoolAddr ! *} 0xee!>
			<+$offset(SM_MSG_Q,msgQ) * { 
			    +$smObjPoolAddr
			    <+$offset(SM_MSG_NODE,message) !>
			    <+$offset(SM_MSG_NODE,msgLength) @>
			    *}>"
    } else {
	set msgQGopher "$vxTicks @
			$msgQId <+$offset(MSG_Q,options) @>
			<+$offset(MSG_Q,maxMsgs) @> 
			<+$offset(MSG_Q,maxMsgLength) @> 
			<+$offset(MSG_Q,sendTimeouts) @> 
			<+$offset(MSG_Q,recvTimeouts) @> 
		        <+$offset(MSG_Q,events) @@@b>
			<+$offset(MSG_Q,freeQ) 
			    +$offset(Q_JOB_HEAD,pendQ) * { ! * } 0xee! >
			<+$offset(MSG_Q,msgQ) 
			    +$offset(Q_JOB_HEAD,pendQ) * { ! * } 0xee! >
			<+$offset(MSG_Q,msgQ) * { 
			    <+$offset(MSG_NODE,message) !>
			    <+$offset(MSG_NODE,msgLength) @>
			    *} >"
    }

    return [wtxGopherEval $msgQGopher]
}

##############################################################################
#
# mqPxInfoGet - return information about a POSIX message queue
#
# SYNOPSIS:
#   mqPxInfoGet mqId
#
# PARAMETERS:
#   mqId: the ID of the POSIX message queue to query
#
# RETURNS:
#   A fairly involved list of data returned by a gopher expression.
#   First, the list of readers blocked on an empty message queue. For each of
#   them a task ID will be present. These task IDs are followed by a
#   sentinel (value 0xee).
#   Then, the list of writers blocked on a full message queue. For each of
#   them a task ID will be present. These task IDs are followed by a
#   sentinel (value 0xee).
#   Then several static elements:
#     name              -- the address where the message queue name is stored.
#     mq_curmsgs        -- current number of messages in the queue.
#     mq_maxmsg         -- maximum number of messages the queue can hold.
#     mq_msgsize        -- maximum size of a message.
#     mq_flags		-- the flags associated with the POSIX message queue.
#
# To summarize the return value:
#
#   taskId...                                   # 0 or more of these sets
#   0xee
#   taskId...                                   # 0 or more of these sets
#   0xee
#   name mq_curmsgs mq_maxmsg mq_msgsize mq_flags
#
# ERRORS:
#   NONE

proc mqPxInfoGet {mqId} {
    global offset

    return [wtxGopherEval "$mqId
                           +$offset(POSIX_MSG_Q_DES,f_data) *
                           <+$offset(POSIX_MSG_Q,msgq_cond_read) * { ! *} 0xee!>
                           <+$offset(POSIX_MSG_Q,msgq_cond_data) * { ! *} 0xee!>
                           <+$offset(POSIX_MSG_Q,msgq_sym)
                            +$offset(SYMBOL,name) @>
                           +$offset(POSIX_MSG_Q,msgq_attr)
                           <+$offset(POSIX_MSG_Q_ATTR,mq_curmsgs) @>
                           <+$offset(POSIX_MSG_Q_ATTR,mq_maxmsg) @>
                           <+$offset(POSIX_MSG_Q_ATTR,mq_msgsize) @>
                           <+$offset(POSIX_MSG_Q_ATTR,mq_flags) @>"]
}

##############################################################################
#
# classInfoGet - get information about an object class
#
# SYNOPSIS:
#   classInfoGet classId
#
# PARAMETERS:
#   classId: the ID of the class to gather information about
#
# RETURNS:
#   A list consisting of the following members of the CLASS structure:
#     objPartId objSize objAllocCnt objFreeCnt objInitCnt objTerminateCnt
#     createRtn initRtn destroyRtn showRtn helpRtn
#
# ERRORS:
#   NONE

proc classInfoGet {classId} {
    global offset
    return [wtxGopherEval \
		"$classId +$offset(OBJ_CLASS,objPartId) @@@@@@ +4 @@@@@"]
}

##############################################################################
#
# memPartInfoGet - get information about a memory partition, given its ID
#
# SYNOPSIS:
#   memPartInfoGet memPartId
#
# PARAMETERS:
#   memPartId: memory partition ID to return information about
#
# RETURNS:
#   A list with the following elements:
#     totalWords           -- this and the next 4 elements are from the 
#     curBlocksAllocated   -- PARTITION structure
#     curWordsAllocated
#     cumBlocksAllocated
#     cumWordsAllocated
#   In addition, for each element of the free list, the following two
#   items are appended:
#     address		   -- address of block
#     nWords               -- number of 16-bit words in it
#
# ERRORS:
#   

proc memPartInfoGet {memPartId} { 
    global offset
    return [wtxGopherEval "$memPartId 
                           <+$offset(PARTITION,totalWords) @>
                           <+$offset(PARTITION,curBlocksAllocated) @>
                           <+$offset(PARTITION,curWordsAllocated) @>
                           <+$offset(PARTITION,cumBlocksAllocated) @>
                           <+$offset(PARTITION,cumWordsAllocated) @>
                            +$offset(PARTITION,freeList) * {
			       <-$offset(FREE_BLOCK,node) !
			        +$offset(FREE_BLOCK,hdr.nWords) @> * }"]

}

##############################################################################
#
# memPartInfoGetShort - get information about a memory partition, given its ID
#
# This is like memPartInfoGet but lacks the free list information.
#
# SYNOPSIS:
#   memPartInfoGet memPartId
#
# PARAMETERS:
#   memPartId: memory partition ID to return information about
#
# RETURNS:
#   A list with the following elements:
#     totalWords           -- this and the next 4 elements are from the 
#     curBlocksAllocated   -- PARTITION structure
#     curWordsAllocated
#     cumBlocksAllocated
#     cumWordsAllocated
#
# ERRORS:
#   

proc memPartInfoGetShort {memPartId} { 
    global offset
    return [wtxGopherEval "$memPartId 
                           <+$offset(PARTITION,totalWords) @>
                           <+$offset(PARTITION,curBlocksAllocated) @>
                           <+$offset(PARTITION,curWordsAllocated) @>
                           <+$offset(PARTITION,cumBlocksAllocated) @>
                           <+$offset(PARTITION,cumWordsAllocated) @>"]
}

##############################################################################
#
# memSummaryPrint - print statistics about a memory partition
#
# SYNOPSIS:
#   memSummaryPrint freeBlkCount totalFreeBytes maxBlkBytes curBlkCount
#		    curAllocWords cumBlkCount cumAllocWords
#
# PARAMETERS:
#   freeBlkCount: number of free blocks
#   totalFreeBytes: amount of bytes in free blocks
#   maxBlkBytes: amount of bytes in biggest free block
#   curBlkCount: number of allocated current blocks
#   curAllocWords: amount of bytes in allocated current blocks
#   cumBlkCount: number of allocated cumulative blocks
#   cumAllocWords: amount of bytes in allocated cumulative blocks
#
# RETURNS:
#
# ERRORS:
#   

proc memSummaryPrint {freeBlkCount totalFreeBytes maxBlkBytes \
		      curBlkCount curAllocWords \
		      cumBlkCount cumAllocWords} {

    puts stdout "\nSUMMARY:\n"
    puts stdout " status    bytes     blocks   avg block  max block"
    puts stdout " ------ ---------- --------- ---------- ----------"
    puts stdout "current"
    if {$freeBlkCount > 0} {
	puts stdout [format "   free %10u %9u %10u %10u" \
	    $totalFreeBytes $freeBlkCount \
	    [expr {$totalFreeBytes / $freeBlkCount}] $maxBlkBytes]
    } else {
    	puts stdout " no free blocks"
    }
    if {$curBlkCount} {
	puts stdout [format "  alloc %10u %9u %10u          -"   \
	    [expr {2 * $curAllocWords}] \
	    $curBlkCount \
	    [expr {2 * $curAllocWords / $curBlkCount}]]
    } {
	puts stdout "   no allocated blocks"
    }
    puts stdout "cumulative"
    if {$cumBlkCount} {
	puts stdout [format "  alloc %10u %9u %10u          -"   \
	    [expr {2 * $cumAllocWords}] \
	    $cumBlkCount \
	    [expr {(2 * $cumAllocWords) / $cumBlkCount}]]
    } {
	puts stdout "   no allocated blocks"
    }
}

##############################################################################
#
# smMemPartInfoGet - get info about a shared memory partition, given its ID
#
# SYNOPSIS:
#   smMemPartInfoGet smMemPartId
#
# PARAMETERS:
#   smMemPartId: shared memory partition ID to return information about
#
# RETURNS:
#   A list with the following elements:
#     totalWords           -- this and the next 4 elements are from the 
#     curBlocksAllocated   -- PARTITION structure
#     curWordsAllocated
#     cumBlocksAllocated
#     cumWordsAllocated
#   In addition, for each element of the free list, the following two
#   items are appended:
#     address		   -- address of block
#     nWords               -- number of 16-bit words in it
#
# ERRORS:
#   

proc smMemPartInfoGet {smMemPartId} { 
    global offset

    set smObjPoolAddr [smObjPoolAddrGet]

    return [wtxGopherEval "$smMemPartId 
                           <+$offset(SM_PARTITION,totalWords) @>
                           <+$offset(SM_PARTITION,curBlocksAllocated) @>
                           <+$offset(SM_PARTITION,curWordsAllocated) @>
                           <+$offset(SM_PARTITION,cumBlocksAllocated) @>
                           <+$offset(SM_PARTITION,cumWordsAllocated) @>
                            +$offset(SM_PARTITION,freeList) * {
				+$smObjPoolAddr
			       <-$offset(SM_FREE_BLOCK,node) !
			        +$offset(SM_BLOCK_HDR,nWords) @
			       > * }"]
}

##############################################################################
#
# trgInfoGet - get information about a particular trigger
#
# trgInfoGet gets the information related to a trigger structure
# and also performs the service of validating that the given object
# is in fact still a trigger, by checking the objCore pointer.
#
# SYNOPSIS:
#   trgInfoGet trgId
#
# PARAMETERS:
#   trgId: the trigger id to fetch information about
#
# RETURNS:
#   A (big) list of 18 items:
#
# ERRORS:
#   NONE

proc trgInfoGet {trgId} {
    global offset
    global className

    set trgList [wtxGopherEval "$trgId !
                           <+$offset(TRIGGER,objCore) @>"]

    # Pick out the objCore and compare it to trgClassId.

    set objCore [lindex $trgList 1]

    if {$objCore != 0} {
	if {$className($objCore) != "trg"} {
	    error "not a trigger"
	}
    }

    return [wtxGopherEval "$trgId 
                           <+$offset(TRIGGER,objCore) @>
                           <+$offset(TRIGGER,eventId) @w>
                           <+$offset(TRIGGER,status) @w>
                           <+$offset(TRIGGER,disable) @>
                           <+$offset(TRIGGER,contexType) @>
                           <+$offset(TRIGGER,contextId) @>
                           <+$offset(TRIGGER,objId) @>
                           <+$offset(TRIGGER,chain) @>
                           <+$offset(TRIGGER,next) @>
                           <+$offset(TRIGGER,conditional) @>
                           <+$offset(TRIGGER,condType) @>
                           <+$offset(TRIGGER,condEx1) @>
                           <+$offset(TRIGGER,condOp) @>
                           <+$offset(TRIGGER,condEx2) @>
                           <+$offset(TRIGGER,actionType) @>
                           <+$offset(TRIGGER,actionFunc) @>
                           <+$offset(TRIGGER,actionArg) @>
                           <+$offset(TRIGGER,actionDef) @>
                           <+$offset(TRIGGER,hitCnt) @>>"]
}

##############################################################################
#
# rBuffInfoGet - get information about a particular rBuff
#
# rBuffInfoGet gets enough information for one row of the "i" command, 
# but also performs the service of validating that the given object
# is in fact still a rBuff, by checking the objCore pointer.
#
# SYNOPSIS:
#   rBuffInfoGet rBuff
#
# PARAMETERS:
#   rBuff: the rBuff to fetch information about
#
# RETURNS:
#   A (big) list of 16 items:
#     srcPart options buffSize currBuffs threshold maxBuffs maxBuffsActual
#     emptyBuffs dataContent writesSinceReset readsSinceReset timesExtended
#     timesXThreshold bytesWritten bytesRead bytesPeak
#
# ERRORS:
#   NONE

proc rBuffInfoGet {rBuff} {
    global className
    global offset

    set rbList [wtxGopherEval "$rBuff !
                           <+$offset(RBUFF_TYPE,objCore) @>"]

    # Pick out the objCore and compare it to rBuffClassId.

    set objCore [lindex $rbList 1]

    if {$objCore != 0} {
	if {$className($objCore) != "rBuff"} {
	    error "not an rBuff"
	}
    }

    return [wtxGopherEval "$rBuff 
                           <+$offset(RBUFF_TYPE,buffRead) @>
                           <+$offset(RBUFF_TYPE,buffWrite) @>
                           <+$offset(RBUFF_TYPE,srcPart) @>
                           <+$offset(RBUFF_TYPE,options) @>
                           <+$offset(RBUFF_TYPE,buffSize) @>
                           <+$offset(RBUFF_TYPE,currBuffs) @>
                           <+$offset(RBUFF_TYPE,threshold) @>
                           <+$offset(RBUFF_TYPE,minBuffs) @>
                           <+$offset(RBUFF_TYPE,maxBuffs) @>
                           <+$offset(RBUFF_TYPE,maxBuffsActual) @>
                           <+$offset(RBUFF_TYPE,emptyBuffs) @>
                           <+$offset(RBUFF_TYPE,dataContent) @>
                           <+$offset(RBUFF_TYPE,writesSinceReset) @>
                           <+$offset(RBUFF_TYPE,readsSinceReset) @>
                           <+$offset(RBUFF_TYPE,timesExtended) @>
                           <+$offset(RBUFF_TYPE,timesXThreshold) @>
                           <+$offset(RBUFF_TYPE,bytesWritten) @>
                           <+$offset(RBUFF_TYPE,bytesRead) @>
                           <+$offset(RBUFF_TYPE,bytesPeak) @>"]
}

##############################################################################
#
# initClassNames - initialize the global className associative array
#
# For each class name listed in the global variable classNameList, a symbol
# is sought on the target named "<classname>ClassId".  If such is found, 
# an entry is made in the global associative array className as follows:
#   $className(<class-id-in-hex>) == <class-name>
# This can be used to map the objCore field of an object to the class name
# of the object.
#
# SYNOPSIS:
#   initClassNames
#
# PARAMETERS:
#   NONE
#
# RETURNS:
#   NONE
#
# ERRORS:
#   NONE

proc initClassNames {} {
    global className
    global classNameList
    foreach class $classNameList {
	if {! [catch {wtxSymFind -name ${class}ClassId} result]} {
	    set addr [lindex $result 1]
	    if {! [catch { wtxGopherEval "$addr @" } id]} {
		if {$id != 0} {
		    set className([format "0x%x" $id]) $class
		}
	    }
	}
	if {! [catch {wtxSymFind -name ${class}InstClassId} result]} {
	    set addr [lindex $result 1]
	    if {! [catch { wtxGopherEval "$addr @" } id]} {
		if {$id != 0} {
		    set className([format "0x%x" $id]) $class
		}
	    }
	}
    }
}

##############################################################################
#
# checkClassId - test whether a given ID matches a class name
#
# SYNOPSIS:
#   checkClassId objId class
#
# PARAMETERS:
#   objId: the ID of the object to test
#   class: the name of the class to check membership in
#
# RETURNS:
#   0 if objId is not a member of the named class
#  !0 if objId is a member of the named class
#   
# ERRORS:
#   NONE

proc checkClassId {objId class} {
    global className
    if ![info exists className($objId)] {return 0}
    return [expr {$className($objId) == "$class"}]
}

#|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
# 
# OBJECT CLASSIFICATION
#

##############################################################################
#
# objectClassify - given an object ID, return the name of its class
#
# Management differs depending on whether the object is a Shared Memory
# Object or not.
#
# For a local, non shared, object:
# A gopher string is used to return the integer pointed to by the objectId,
# and also the integer pointed to by the objectId + the offset of the objCore
# member of the TCB.  First, the second of these numbers is compared against
# the class ID of task objects, and if these match "task" is returned.  
# Otherwise, the class name is searched for in the className associative
# array.  If this fails, "0" is returned.
#
# For a shared object:
# A gopher string is used to return the object type. This type is compared
# against the known shared object types. If it matches, the class name is
# returned. If this fails, "0" is returned.
#
# If the object ID is not valid, this routines also returns "0".
#
# SYNOPSIS:
#   objectClassify objId
#
# PARAMETERS:
#   objId: the ID of the object to classify
#
# RETURNS:
#   The classname of the object, or "0" if the object class is not known, or
#   the object ID is not valid
#
# ERRORS:
#   NONE

proc objectClassify objId { 
    global className
    global offset

    if {[objIsShared $objId]} {
	set smObjId [smObjIdToAdrs $objId]
	if {$smObjId == 0} {
	    return 0
	}

	if {! [smObjVerify $smObjId]} {
	    return 0
	}
	
	set gopherString "$smObjId +$offset(SM_OBJ,objType) @"
	if [catch {wtxGopherEval $gopherString} smObjType] {
	    if {[wtxErrorName $smObjType] =="SVR_CANT_CONNECT_LICENSE_DAEMON"} {
		error $smObjType
	    } else {
		return 0
	    }
	}

	return [classFromSmObjTypeGet $smObjType]

    } else {
	set gopherString "$objId <@> +$offset(WIND_TCB,objCore) @"
	if [catch {wtxGopherEval $gopherString} classIds] {
	    if {[wtxErrorName $classIds] =="SVR_CANT_CONNECT_LICENSE_DAEMON"} {
		error $classIds
	    } else {
		return 0
	    }
	}
    }

    if [checkClassId [lindex $classIds 1] task] {return task}
    if [info exists className([lindex $classIds 0])] {
	return $className([lindex $classIds 0])
    } else {
	return 0
    }
}

#||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
# 
# UTILITY FUNCTIONS
#

##############################################################################
#
# shSymAddr - map a symbol name to its address
#
# SYNOPSIS:
#   shSymAddr name
#
# PARAMETERS:
#   name: the name of the symbol to search for
#
# RETURNS:
#   the hexadecimal address of the symbol
#
# ERRORS:
#   "No symbol $name" if the symbol cannot be found.

proc shSymAddr {name} {
    if {[catch {set wtxSym [wtxSymFind -name $name]} result]} {
	if {[wtxErrorName $result] == "SYMTBL_SYMBOL_NOT_FOUND"} {
	    error "The shell cannot complete this request, because
the symbol $name cannot be found in the symbol table."
	} {
	    error $result
	}
    } {
	return [lindex $wtxSym 1]
    }
}

##############################################################################
#
# shSymValue - map a symbol name to its value
#
# SYNOPSIS:
#   shSymValue name
#
# PARAMETERS:
#   name: the name of the symbol to search for
#
# RETURNS:
#   the decimal value of the symbol
#
# ERRORS:
#   "No symbol $name" if the symbol cannot be found.

proc shSymValue {name} {

    set addr [shSymAddr $name]

    set value [expr [memBlockGet -l [wtxMemRead $addr 4]]]

    return $value
}

##############################################################################
#
# classFromSmObjTypeGet - determine a class name from the shared object type
#
# This routine returns standard class names that match shared memory objects
# that have no specific class.
#
# SYNOPSIS:
#   classFromSmObjTypeGet smObjType
#
# PARAMETERS:
#   smObjType: shared object type
#
# RETURNS:
#   a class name or 0 if no match could be done.
#
# ERRORS:
#

proc classFromSmObjTypeGet {smObjType} {

    switch [expr $smObjType] {
	4	{
		# FIFO shared semaphore
		return sem
		}
	5	{
		# counting shared semaphore
		return sem
		}
	10	{
		# FIFO shared message queue
		return msgQ
		}
	20	{
		# shared memory partition
		return memPart
		}
	default	{return 0}
    }
}

##############################################################################
#
# smObjInfoInit - initialize global variables related to Shared Object
#
# SYNOPSIS:
#   smObjInfoInit
#
# PARAMETERS:
#   N/A
#
# RETURNS:
#   0  - OK
#   -1 - ERROR (VxMP not installed).
#
# ERRORS:
#

proc smObjInfoInit {} {
    global smObjPoolMinusOne
    global smObjHdrAdrs

    # Get the address, minus one, of the Shared Memory Object pool.
    # This is done only once, since this address will never change afterward.

    if {$smObjPoolMinusOne == -1} {
	if [catch {lindex [wtxSymFind -name smObjPoolMinusOne] 1} symAddr] {
	    if {[wtxErrorName $symAddr] == "SYMTBL_SYMBOL_NOT_FOUND"} {
		return -1
	    } else {
		error $symAddr
	    }
	}

	set memBlock [wtxMemRead $symAddr 4]
	set smObjPoolMinusOne [memBlockGet -l $memBlock]
	}

    # Get the address of the Shared Memory Object Header.

    if {$smObjHdrAdrs == 0} {
	if [catch {lindex [wtxSymFind -name pSmObjHdr] 1} symAddr] {
	    if {[wtxErrorName $symAddr] == "SYMTBL_SYMBOL_NOT_FOUND"} {
		return -1
	    } else {
		error $symAddr
	    }
	}

	set memBlock [wtxMemRead $symAddr 4]
	set smObjHdrAdrs [memBlockGet -l $memBlock]
	}

    return 0
}

##############################################################################
#
# smObjPoolAddrGet - return the local address of the shared object pool
#
# SYNOPSIS:
#   smObjPoolAddrGet
#
# PARAMETERS:
#   N/A
#
# RETURNS:
#   smObjPoolAddr: the local address of the shared object pool
#
# ERRORS:
#

proc smObjPoolAddrGet {} {
    global smObjPoolMinusOne

    smObjInfoInit
    return [expr $smObjPoolMinusOne + 1]
}

##############################################################################
#
# smObjIdToAdrs - return the local address of the shared object from its ID
#
# SYNOPSIS:
#   smObjIdToAdrs objId
#
# PARAMETERS:
#   objId - the shared object ID
#
# RETURNS:
#   objAdrs - the local address of the shared object, or 0 if VxMP not
#             installed.
#
# ERRORS:
#

proc smObjIdToAdrs {objId} {
    global smObjPoolMinusOne

    if {[smObjInfoInit] == -1} {
	return 0
    }

    return [expr $objId + $smObjPoolMinusOne]
}

##############################################################################
#
# locToGlobAdrs - return a global address from a local address
#
# SYNOPSIS:
#   locToGlobAdrs localAdrs
#
# PARAMETERS:
#   localAdrs - a local address
#
# RETURNS:
#   globalAdrs - the corresponding global address
#
# ERRORS:
#

proc locToGlobAdrs {localAdrs} {
    return [expr $localAdrs - [smObjPoolAddrGet]]
}

##############################################################################
#
# smObjVerify - determine if the object is a valid shared memory object
#
# SYNOPSIS:
#   smObjVerify objId
#
# PARAMETERS:
#   objId - the shared object ID
#
# RETURNS:
#   0 - the shared memory object is not valid
#   1 - the object memory object is valid
#
# ERRORS:
#

proc smObjVerify {objId} {

    set objGlobalAdrs [locToGlobAdrs $objId]

    set verify [memBlockGet -l [wtxMemRead $objId 4]]

    if {[expr $objGlobalAdrs == $verify]} {
	return 1
    } else {
	return 0
    }
}

##############################################################################
#
# objIsShared - determine if the object is a shared memory object
#
# SYNOPSIS:
#   objIsShared objId
#
# PARAMETERS:
#   objId - the shared object ID
#
# RETURNS:
#   0 - the object is not a shared memory object
#   1 - the object is a shared memory object
#
# ERRORS:
#

proc objIsShared {objId} {
    if {[expr $objId & 1]} {
	return 1
    } else {
	return 0
    }
}

##############################################################################
#
# objIsDistributed - determine if the object is a distributed object
#
# SYNOPSIS:
#   objIsDistributed objId
#
# PARAMETERS:
#   objId - the distributed object ID
#
# RETURNS:
#   0 - the object is not a distributed object
#   1 - the object is a distributed object
#
# ERRORS:
#

proc objIsDistributed {objId} {
	if {[expr $objId & 3] == 3} {
	return 1
	} else {
	return 0
	}
}

##############################################################################
#
# msgGet - returns a message queue message contents
#
# SYNOPSIS:
#   msgGet msgAdrs msgLen
#
# PARAMETERS:
#   msgAdrs - the local address of the message
#   msgLen  - the length of the message
#
# RETURNS:
#   The first 14 bytes (or less if smaller) of the message in a processed
#   format: the hexadecimal value of each byte followed by the character
#   transcription (values outside the ASCII range are represented by dots).
#
# ERRORS:
#

proc msgGet {msgAdrs msgLen} {
    set line ""

    # Don't get more than 14 bytes

    if {$msgLen > 14} {
	set msgLen 14
    }

    # Get message contents

    set memBlock [wtxMemRead $msgAdrs $msgLen]
    set message [memBlockGet $memBlock]

    # Display in byte format

    foreach byte $message {
	set line "$line[format "%02x " $byte]"
    }
    for {set remain [expr 14 - $msgLen]} {$remain > 0} {incr remain -1} {
	set line "$line   "
    }

    # Display in character format

    set line "$line *"

    foreach byte $message {
	if {($byte < 32) || ($byte > 126)} {
	    set line "$line."
	} else {
	    set line "$line[format "%c" $byte]"
	}
    }
    for {set remain [expr 14 - $msgLen]} {$remain > 0} {incr remain -1} {
	set line "$line "
    }
    set line "$line*"

    return $line
}

##############################################################################
#
# nextAddrGet - Get the next instruction address after call or jmpl
#
# SYNOPSIS:
#   nextAddrGet addr
#
# PARAMETERS:
#   addr: address of the current instruction
#
# ERRORS: N/A
#
# RETURNS: next instruction address
#
 
proc nextAddrGet {addr} {
 
    # disassembled instruction second field is inst size, third field is
    # instruction address
 
    set nextInst [wtxMemDisassemble -address $addr 1]

    # get current address from disassembled instruction. All addresses are
    # in hexa format, but not all of them are 0x prepended. Verify it

    set curAddr [lindex $nextInst 2]
    if { ! [string match 0x* $curAddr] } {
        set curAddr 0x$curAddr
    }

    set nextAddr [expr $curAddr + [lindex $nextInst 1]]

    return [format "%#x" $nextAddr]
}

#||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
# 
# EVENT POLLING
#

##############################################################################
#
# wtxEventPoll - poll the current connection for an event
#
# SYNOPSIS:
#   wtxEventPoll msec [maxtries]
#
# PARAMETERS:
#   msec:     number of milliseconds to wait between polls
#   maxtries: maximum number of polls to make before giving up
#
# RETURNS:
#   An event string, or "" if maxtries attempts were made without resulting
#   in an event, if maxtries was specified.
#
# ERRORS:
#   

proc wtxEventPoll {msec {maxtries 0}} {

    set event ""
    global remainingEvents	;# remaining events list

    # first have a look to the remainingEvents list
    if [info exists remainingEvents] {
        set event [lindex $remainingEvents 0]

    	if {$event != ""} {
	
	    # remove $event from the remaining events list
	    set remainingEvents [lreplace $remainingEvents 0 0]
	    return $event
	}
    }

    # if the asynchronous event notification is started then
    # use shAsyncEventGet{} (WindSh proc) instead of wtxEventGet
    if {"[info commands shAsyncEventGet]" == "shAsyncEventGet"} {
    	set eventGetCmd shAsyncEventGet
    } else {
    	set eventGetCmd wtxEventGet
    }

    if {$maxtries > 0} {
	set ix 0
	while {[set event [$eventGetCmd]] == "" && $ix < $maxtries} {
	    msleep $msec
	    incr ix
	}
    } else {
	while {[set event [$eventGetCmd]] == ""} {
	    msleep $msec
	}
    }

    return $event
}

proc vxObjectLoad {type name} {
    global __wtxCpuType
    global cpuType
    global cpuFamily
    
    set omf ""
    set tool [targetTool]

    set opath [wtxPath target lib obj$cpuType($__wtxCpuType)$tool$omf$type]$name
    return [lindex [wtxObjModuleLoad $opath] 0]
}
    
##############################################################################
#
# targetInitCheck - check whether the target system is initialized
#
# This routine tries to determine whether or not the target system (i.e.
# VxWorks) is already initialized when the tools attached to the target
# server. This may be necessary to prevent a tool to fetch data within
# data structures not initialized yet.
#
# SYNOPSIS:
#   targetInitCheck
#
# PARAMETERS: N/A
#
# RETURNS:
#   1 - if the target system is initialized
#   0 - if the target system is not initialized yet (system mode debug)
#
# ERRORS:
#

set targetIsInitialized 0

proc targetInitCheck {} {
    global targetIsInitialized

    # First check if we have not already passed succesfully this test

    if $targetIsInitialized {
	return 1
    }

    # If we can't find the symbol activeQHead, vxWorks is certainly not
    # loaded on the target. In that case, we consider that the system
    # is not initialized.

    if {[catch {set wtxSym [wtxSymFind -name activeQHead]} result]} {
	if {[wtxErrorName $result] == "SYMTBL_SYMBOL_NOT_FOUND"} {
	    return 0
	} {
	    error $result
	}
    } 

    # get activeQHead address

    set activeQHead [lindex $wtxSym 1]

    # Read value stored in activeQHead's class ID

    set activeQClassId [wtxGopherEval "$activeQHead + 12 @"]

    # If activeQHead is initialized
    # its class ID will be qFifoClassId
    #
    # Note the subtle use of lindex so that qFifoClassId
    # will be treated as a number and not a string in the
    # comparison below!

    set memBlk         [wtxMemRead [shSymAddr qFifoClassId] 4]
    set qFifoClassId   [lindex [memBlockGet -l $memBlk] 0]
    memBlockDelete     $memBlk

    if {$activeQClassId == $qFifoClassId} {
	set targetIsInitialized 1
	return 1
    } else {
        return 0
    }
}

###############################################################################
# 
# MODULE INITIALIZATION
#

source [wtxPath host resource tcl]wtxcore.tcl


###############################################################################
# 
# source i2o core file if exists
#

if {![catch {file exists [wtxPath host resource tcl]i2oCore.tcl} result] \
	&& $result} {
	source [wtxPath host resource tcl]i2oCore.tcl
}


###############################################################################
# 

proc shellCoreInit {} {
    global eeMemBlock
    global __wtxTsInfo
    global __wtxCpuType
    global iCommandGopher
    global offset
    global cpuFamily
    global env
    global pcGetGopher
    global spGetGopher
	global i2oDefined

    # Call wtxTsInfoGet and load the corresponding architecture-specific
    # module.

    set __wtxTsInfo [wtxTsInfoGet]
    set __wtxCpuType [lindex [lindex $__wtxTsInfo 2] 0]

    # Read in the cpu-family specific material, based on the cpuType value
    # obtained from wtxTsInfoGet.

    uplevel #0 source \
	[wtxPath host resource tcl]sh-$cpuFamily($__wtxCpuType).tcl

    # A gopher that will get the data from the TCB necessary to print one
    # line of the i command (this is used in several places in the shell and
    # browser). This gopher fragment is meant to be executed when the
    # pointer contains the address of the TCB, and fetches the status,
    # normal and current priorities, pc and sp, and a few other
    # interesting entries from that structure.

    set iCommandGopher "
        <+$offset(WIND_TCB,status) @>
        <+$offset(WIND_TCB,priority) @>
        $pcGetGopher
        $spGetGopher
        <+$offset(WIND_TCB,errorStatus) @>
        <+$offset(WIND_TCB,timeout) @>
        <+$offset(WIND_TCB,entry) @>
        <+$offset(WIND_TCB,priNormal) @>
        <+$offset(WIND_TCB,name) *$>"

    # clear the associative array of class IDs to class names.

    if [info exists className] {
	unset className
    }

    # intialize eeMemBlock with 0xee.

    if {$eeMemBlock == ""} {set eeMemBlock [memBlockCreate 1 0xee]}
    
    # create the association of class ID to class name.

    initClassNames

	if {[info exists i2oDefined] && $i2oDefined} {
		addI2oClassNames
	}
}

