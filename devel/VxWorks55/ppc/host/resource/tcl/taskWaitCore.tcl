# taskWaitCore.tcl - Implementation of taskWaitShow core routines in Tcl
#
# Copyright 1998-2001 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01c,09nov01,aeg  added VxWorks events awareness.
# 01b,05jun98,f_l  moved offsets to the appropriate sh-<arch>.tcl files
# 01a,29may98,f_l  written.
#

# DESCRIPTION
# This module holds the Tcl code of taskWaitShow core routines.


##############################################################################
#
# taskWaitInit - initialize global variables for taskWaitGet 
#
# This routine initializes global variables for the taskWaitGet routine.
# 

proc taskWaitInit {} {

    global taskWaitSemClass  #id of the semaphore class
    global taskWaitMsgQClass #id of the msgq class
    global taskWaitVxEventPendQ #address of VxWorks event pendQ

    # these global variables are accessible inside the taskWaitGet routine only

    if {[info exists taskWaitSemClass]} {return}
    set taskWaitSemClass [lindex [wtxSymFind -name semClassId] 1]
    set taskWaitSemClass [wtxGopherEval "$taskWaitSemClass *!"]
    set taskWaitMsgQClass [lindex [wtxSymFind -name msgQClassId] 1]
    set taskWaitMsgQClass [wtxGopherEval "$taskWaitMsgQClass *!"]
    set taskWaitVxEventPendQ [lindex [wtxSymFind -name vxEventPendQ] 1]
}

##############################################################################
#
# taskWaitGet - get information about the ojbect a task is pended on
#
# This routine gets information about the ojbect a task is pended on.
# This routine doesn't support POSIX semaphores and messages queues.
# This routine doesn't support pending signals.
# If the task is pended but not on a standard semaphore or a message queue,
# the taskWaitSmGet is run in order to test if the task is pended on a shared
# semaphore or a shared message queue.
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
#   taskWaitGet taskId
#
# PARAMETERS:
#   taskId : the task to fetch information about
#
# RETURNS:
#   A list of 11 items:
#	taskName entry taskId status delay objType objId objName
#
# ERRORS: N/A
#

proc taskWaitGet {args} {
    global offset
    global taskWaitSemClass 
    global taskWaitMsgQClass
    global taskWaitVxEventPendQ

    bindNamesToList tid $args
    set taskInfo [taskInfoGet $tid]
    set stat [lindex $taskInfo 2]
    if {![expr $stat & 0x2]} {

	# the task is not pended

        set vxClock [lindex $taskInfo 0]
        set timeout [lindex $taskInfo 7]
        set delayCount [expr {$stat & 0x4 ? $timeout - $vxClock : 0}]
	set priority [lindex $taskInfo 3]
	set entry [lindex $taskInfo 8]
	set entry [shSymbolicAddr -exact $entry]
	set priNormal [lindex $taskInfo 9]
	set inherited [expr $priority > $priNormal]
	set status [taskStatus $stat $inherited]
	set name [lindex $taskInfo 10]
	lappend result $name $entry $tid $status $delayCount "" 0 ""
	return $result
    }

    # the task is pended
    # These Gopher scripts are supposed to get the objClassID objId of the 
    # object the task is pended on. 

    # This script assumes the object is a semaphore. If the wtxGopherEval
    # command returns an error, this implies the object is not a semaphore.

    set gopherPrg "$tid +$offset(WIND_TCB,pPendQ) *\
		 -$offset(SEMAPHORE,qHead) ! <*!>\
		 +$offset(SEMAPHORE,semType) @b" 

    # get objId objClassID and semType

    if [catch {wtxGopherEval $gopherPrg} semTest] {
	set semTest [lindex $semTest 3] 
    }

    # This script assumes the object is a message queue. The task is waiting
    # for a message. It is waiting on a receive command. If the wtxGopherEval
    # command returns an error, this implies the object is not a message
    # queue.

    set gopherPrg "$tid +$offset(WIND_TCB,pPendQ) *\
		-$offset(Q_JOB_HEAD,pendQ)-$offset(MSG_Q,msgQ) !*!"

    # get objId objClassID 

    if [catch {wtxGopherEval $gopherPrg} msgQRecTest] {
	set msgQRecTest [lindex $msgQRecTest 3]
    }

    # This script assumes the object is a message queue. 
    # The task is waiting on a send command. If the wtxGopherEval
    # command returns an error, this implies the object is not a message
    # queue.

    set gopherPrg "$tid +$offset(WIND_TCB,pPendQ) *\
		-$offset(Q_JOB_HEAD,pendQ)-$offset(MSG_Q,freeQ) !*!"

    # get objId objClassID 

    if [catch {wtxGopherEval $gopherPrg} msgQSenTest] {
	    set msgQSenTest [lindex $msgQSenTest 3]
    } 
    set vxClock [lindex $taskInfo 0]
    set priority [lindex $taskInfo 3]
    set entry [lindex $taskInfo 8]
    set entry [shSymbolicAddr -exact $entry]
    set priNormal [lindex $taskInfo 9]
    set inherited [expr $priority > $priNormal]
    set status [taskStatus $stat $inherited]
    set name [lindex $taskInfo 10]
    set timeout [lindex $taskInfo 7]
    set delayCount [expr {$stat & 0x4 ? $timeout - $vxClock : 0}]

    # get semaphore and msg_q class ids
    # this routine sets the taskWaitSemClass and taskWaitMsgQClass global
    # variable if they have not been set yet.

    taskWaitInit

    # Test if the objClassId returned by the Gopher script is the id of the
    # semaphore class

    if {($semTest != "(AGENT_GOPHER_FAULT)") && \
	($taskWaitSemClass == [lindex $semTest 1])} {
	set semId [lindex $semTest 0]
	set objName [objWNameCatch $semId]
	set semType [semTypeTag [lindex $semTest 2]]
	lappend result $name $entry $tid $status $delayCount\
			 $semType $semId $objName
	return $result
    }

    # Test if the objClassId returned by the Gopher script is the id of the
    # message queue class

    if {($msgQRecTest != "(AGENT_GOPHER_FAULT)") &&\
	($taskWaitMsgQClass == [lindex $msgQRecTest 1])} {
	set msgQId [lindex $msgQRecTest 0]
	set objName [objWNameCatch $msgQId]
	lappend result $name $entry $tid $status $delayCount MSG_Q(R) $msgQId \
				$objName 
	return $result
    }

    # Test if the objClassId returned by the Gopher script is the id of the
    # message queue class

    if {($msgQSenTest != "(AGENT_GOPHER_FAULT)") &&\
	($taskWaitMsgQClass == [lindex $msgQSenTest 1])} {
	set msgQId [lindex $msgQSenTest  0]
	set objName [objWNameCatch $msgQId]
	lappend result $name $entry $tid $status $delayCount MSG_Q(S) $msgQId \
				$objName 
	return $result
    }
    
    # Test if task is pending on the global VxWorks event queue

    if {[wtxGopherEval "$tid +$offset(WIND_TCB,pPendQ) *!"] == \
						$taskWaitVxEventPendQ} {

	lappend result $name $entry $tid $status $delayCount VX_EVENT 0 ""
	return $result
    }

    # tests failed. Task is pended on
    #  an other object (shared object or unknown object).
    # run taskWaitSmGet to check for shared memory objects

    return [taskWaitSmGet $tid]
}

##############################################################################
#
# taskWaitSmGet - get information about the Sm ojbect a task is pended on
# 
# This routine gets information about the Sm ojbect a task is pended on.
# It is called by the taskWaitGet routine.
# Names of Shared semaphores and message queues are not available(N/A).
#
# SYNOPSIS:
#   taskWaitSmGet taskId
#
# PARAMETERS:
#   taskId : the task to fetch information about
#
# RETURNS:
#   A list of 11 items:
#	taskName entry taskId status delay objType objId objName
#
# ERRORS: N/A
#

proc taskWaitSmGet {args} {

    bindNamesToList tid $args
    set taskInfo [taskInfoGet $tid]
    set stat [lindex $taskInfo 2]
    if {![expr $stat & 0x2]} {
        set vxClock [lindex $taskInfo 0]
        set timeout [lindex $taskInfo 7]
        set delayCount [expr {$stat & 0x4 ? $timeout - $vxClock : 0}]
	set priority [lindex $taskInfo 3]
	set entry [lindex $taskInfo 8]
	set entry [shSymbolicAddr -exact $entry]
	set priNormal [lindex $taskInfo 9]
	set inherited [expr $priority > $priNormal]
	set status [taskStatus $stat $inherited]
	set name [lindex $taskInfo 10]
	lappend result $name $entry $tid $status 0 "" 0 ""
	return $result
    }
    if [catch {msgQSmSendTest $tid} testResult] {
	if [catch {msgQSmRecTest $tid} testResult] {
	    if [catch {semSmTest $tid} testResult] {

		# the three previous tests failed, the task is pended on an
		# unknown object (POSIX ?, signal ?) or no longer pended.

		set testResult "N/A 0"
	    }
	}
    }
    set vxClock [lindex $taskInfo 0]
    set priority [lindex $taskInfo 3]
    set entry [lindex $taskInfo 8]
    set entry [shSymbolicAddr -exact $entry]
    set priNormal [lindex $taskInfo 9]
    set inherited [expr $priority > $priNormal]
    set status [taskStatus $stat $inherited]
    set name [lindex $taskInfo 10]
    set timeout [lindex $taskInfo 7]
    set delayCount [expr {$stat & 0x4 ? $timeout - $vxClock : 0}]
    lappend result $name $entry $tid $status $delayCount
    return [concat $result $testResult]
}


##############################################################################
#
# semSmTest - test if a task is pended on a shared semaphore
# 
# This routine is called by the taskWaitSmGet routine. DO NOT RUN this test
# BEFORE a msgQSmSendTest or msgQSmRecTest. Indeed, if the object is actually
# a message queue, semSmTest will return the id of the semaphore included
# inside the message queue. So a task pended on a message queue would be
# considered like a task pended on a semaphore. By first testing if the task is 
# pended on a message queue, the problem is solved.
#
# SYNOPSIS:
#   semSmTest taskId
#
# PARAMETERS:
#   taskId: ID of the task
#
# RETURNS:
#   A list a three items:
#   semType semId objName	
#
# ERRORS: FAIL
#

proc semSmTest {tid} {

    global offset

    set gopherPrg "$tid +$offset(WIND_TCB,pPendQ)*\
		+$offset(Q_FIFO_G_HEAD,pFifoQ) *\
		-$offset(SM_SEM,smPendQ)! @@"

    # get smObjId verify and objType
    # the verify member is also the semId of the shared semaphore
    # but its LSB is set to zero.

    if [catch {wtxGopherEval $gopherPrg} semTest] {
        error FAIL
    }
    set smObjId [lindex $semTest 0]
    if [catch {smObjVerify $smObjId} verif] {
        error FAIL
    } elseif {! $verif} {
	error FAIL
    }

    # set the LSB of the semId to 1 in order to change it into a "real"
    # shared semaphore Id.

    set semId [format "0x%x" [expr [lindex $semTest 1] | 0x1]]

    # it is actually not possible to get the name of a Shared Semaphore

    set objName N/A
    set semType [semTypeTag [lindex $semTest 2]]
    set result "$semType $semId $objName"
    return $result
}

##############################################################################
#
# msgQSmSendTest - test if a task is pended on a shared message queue (send)
#
# This routine is called by the taskWaitSmGet routine.
#
# SYNOPSIS:
#   msgQSmSendTest taskId
#
# PARAMETERS:
#   taskId: ID of the task
#
# RETURNS:
#   A list a three items:
#   semType semId objName
#
# ERRORS: FAIL
#

proc msgQSmSendTest {tid} {

    global offset

    set gopherPrg "$tid +$offset(WIND_TCB,pPendQ)*\
                +$offset(Q_FIFO_G_HEAD,pFifoQ) *\
                -$offset(SM_SEM,smPendQ) -$offset(SM_MSG_Q,freeQSem) ! @"

    if [catch {wtxGopherEval $gopherPrg} msgQSendTest] {
        error FAIL
    }
    set smObjId [lindex $msgQSendTest 0]
    if [catch {smObjVerify $smObjId} verif] {
        error FAIL
    } elseif {! $verif} {
	error FAIL
    }
    set msgQId [format "0x%x" [expr [lindex $msgQSendTest 1] | 0x1]]

    # it is actually not possible to get the name of a Shared Msg queue

    set objName N/A
    set msgQType MSG_Q_S(S)
    set result "$msgQType $msgQId $objName"
    return $result
}


##############################################################################
#
# msgQSmRecTest - test if a task is pended on a shared message queue (receive)
#
# This routine is called by the taskWaitSmGet routine.
#
# SYNOPSIS:
#   msgQSmRecTest taskId
#
# PARAMETERS:
#   taskId: ID of the task
#
# RETURNS:
#   A list a three items:
#   semType semId objName
#
# ERRORS: FAIL
#

proc msgQSmRecTest {tid} {

    global offset

    set gopherPrg "$tid +$offset(WIND_TCB,pPendQ)*\
                +$offset(Q_FIFO_G_HEAD,pFifoQ) *\
                -$offset(SM_SEM,smPendQ) -$offset(SM_MSG_Q,msgQSem) ! @"

    if [catch {wtxGopherEval $gopherPrg} msgQRecTest] {
        error FAIL
    }
    set smObjId [lindex $msgQRecTest 0]
    if [catch {smObjVerify $smObjId} verif] {
        error FAIL
    } elseif {! $verif} {
	error FAIL
    }
    set msgQId [format "0x%x" [expr [lindex $msgQRecTest 1] | 0x1]]

    # it is actually not possible to get the name of a Shared Msg queue

    set objName N/A
    set msgQType MSG_Q_S(R)
    set result "$msgQType $msgQId $objName"
    return $result
}


##############################################################################
#
# objWNameCatch - get the symbole name of an object 
#
# This routine doesn't support POSIX and shared memory objects 
#
# SYNOPSIS:
#   objWNameCatch objId
#
# PARAMETERS:
#   objId: ID of the object (semaphore or message queue)
#
# RETURNS:
#   objName or "N/A" if there is no symbole for the object
#
# ERRORS: N/A
#

proc objWNameCatch {objId} {

    if ![catch {wtxSymFind -value $objId} tmpResult] {
	if {[lindex $tmpResult 1] == $objId} {
		set objWName [lindex $tmpResult 0]
		
	        # filter the .bss symbol

		if {$objWName != ".bss"} {
		    return $objWName
		}
	}
    }
    return N/A
}
    

##############################################################################
#
# semTypeTag - return the tag string form of a semaphore type semType field
#
# SYNOPSIS:
#   semTypeTag type
#
# PARAMETERS:
#   type: the value of the semType field of a semaphore
#
# RETURNS:
#   one of :
#	SEM_B : binary
#	SEM_M : mutex
#	SEM_C : counting
#	SEM_O : old
#	SEM_SB : shared_binary
#	SEM_SC : shared counting
#   or 0x%x? if the type is not expected.
#
# ERRORS:
#   NONE
#

proc semTypeTag type { 
    case [expr $type] {
        0	{set s SEM_B}
	1	{set s SEM_M}
	2	{set s SEM_C}
	3	{set s SEM_O}
	4	{set s SEM_SB}
	5	{set s SEM_SC}
	*	{set s [format "0x%x?" $type]}
	    }
    return $s
}

