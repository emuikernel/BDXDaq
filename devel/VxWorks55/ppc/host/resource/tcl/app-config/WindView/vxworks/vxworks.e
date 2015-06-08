#
# vxworks.e - VxWorks WindView event table - note that this is
# executable TCL code, although the definition of wvEvent in
# database.tcl is required before this file will execute correctly.
#
# Copyright 1995-2001 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01n,24sep01,tcr  Fix SPR 24971 - add PPC interrupt instrumentation
# 01n,18sep01,cpd  Initial Veloce mods:
#                  Add -icon tag for event icons
#                  Add -trigger tag for trigger firing events
#                  Add -helpid tag for the help id
#                  Fix SPR#65198: Update 'pTcb' labels to 'taskId'
#                  Fix Error in definition of EVENT_SAFE_PEND
# 01m,29jun98,dbs  fix param-names for EVENT_OBJ_SIGPAUSE (SPR 21344)
# 01l,23jun98,dbs  change class of msgQ events to lower-case-only msgq
# 01k,22jun98,dbs  change display-name of EVENT_TASKDESTROY to taskDelete
# 01j,11may98,pr   modified definition of EVENT_SIGWRAPPER
# 01i,05may98,dbs  fix tick-event names
# 01h,15apr98,dbs  implement object-ID
# 01g,08apr98,pr   Reordered the definition of EVENT_SEMFLUSH and EVENT_SEMDELETE
# 01f,18mar98,pr   Modified definition of EVENT_SEMFLUSH and EVENT_SEMDELETE
# 01e,20jan98,dbs  Change args to wvEvent to allow better parsing.
# 01d,17dec97,c_s  adjust EVENT_VALUE's declaration
# 01c,27nov97,c_s  add class names and color definitions
# 01b,24nov97,dbs  Converted to wvEvent statements
# 01a,12aug97,dbs  written.
#

#
# The format is:-
#
#  wvEvent EVENT_TITLE eventId|eventIdRange eventFlags eventParams
#
# where eventId is a single numeric ID (matching that in eventP.h),
# eventIdRange is the lower and upper event-IDs separated by a dash,
# eventParams is a Tcl-list, where each element is a
# <type name> pair (type is UINT32 or STRING and name is the
# parameter's textual name), and eventFlags is a Tcl-list of zero or 
# more option-selections, which can be:-
#
#  -name=niceName
#  -nosearch
#  -notimestamp
#  -class=className
#
# and the result is processed as an event-definition.
#

wvEvent EVENT_PPC_DEC_INT_ENT 599 { -nosearch -name=intEntDec -class=int } {}

wvEvent EVENT_INT_ENT 102-365 {
    -nosearch
    -name=intEnt-%d
    -class=int
    -helpid=8224
} {}

wvEvent EVENT_INT_EXIT 101 {
    -name=intExit
    -class=int
    -helpid=8240
    -trigger=true
} {}

wvEvent EVENT_INT_EXIT_K 100 {
    -name=intExitK
    -class=int
    -trigger=true
} {}

wvEvent EVENT_BEGIN 0 { -notimestamp -nosearch  } {
    UINT32 CPU 
    UINT32 bspSize 
    STRING bspName
    UINT32 taskIdCurrent 
    UINT32 collectionMode 
    UINT32 revision 
}

wvEvent EVENT_END 1 { -nosearch } {}

wvEvent EVENT_TIMER_ROLLOVER 2 { -nosearch } {}

wvEvent EVENT_TASKNAME 3 { -notimestamp -nosearch } {
    UINT32 status 
    UINT32 priority 
    UINT32 taskLockCount
    UINT32 taskId 
    UINT32 nameSize 
    STRING name
}

wvEvent EVENT_CONFIG 4 { -notimestamp -nosearch } {
    UINT32 revision
    UINT32 timestampFreq 
    UINT32 timestampPeriod 
    UINT32 autoRollover 
    UINT32 clkRate
    UINT32 collectionMode
    UINT32 processorNum
}

wvEvent EVENT_BUFFER 5  { -notimestamp -nosearch } {
    UINT32 taskIdCurrent
}

wvEvent EVENT_TIMER_SYNC 6 { -nosearch } {
    UINT32 spare
}

wvEvent EVENT_LOGCOMMENT 7 { -notimestamp -nosearch } {
    UINT32 commentLength
    STRING comment
}

wvEvent EVENT_TASKSPAWN 10000 {
    -name=taskSpawn
    -class=task
    -helpid=8528
    -trigger=true
} {
    UINT32 options 
    UINT32 entryPt 
    UINT32 stackSize
    UINT32 priority 
    UINT32 taskId *
}

wvEvent EVENT_TASKDESTROY 10001 {
    -name=taskDelete
    -class=task
    -helpid=8608
    -trigger=true
} {
    UINT32 safeCnt 
    UINT32 taskId *
}

wvEvent EVENT_TASKDELAY	10002 {
    -name=taskDelay
    -class=task
    -helpid=8592
    -trigger=true
} {
    UINT32 ticks
}

wvEvent EVENT_TASKPRIORITYSET 10003 {
    -name=taskPrioritySet
    -class=task
    -helpid=8576
    -trigger=true
} {
    UINT32 oldPri 
    UINT32 newPri 
    UINT32 taskId *
}

wvEvent EVENT_TASKSUSPEND 10004 {
    -name=taskSuspend
    -class=task
    -helpid=8544
    -trigger=true
} {
    UINT32 taskId *
}

wvEvent EVENT_TASKRESUME 10005 {
    -name=taskResume
    -class=task
    -helpid=8560
    -trigger=true
} {
    UINT32 priority 
    UINT32 taskId *
}

wvEvent EVENT_TASKSAFE 10006 {
    -name=taskSafe
    -class=task
    -helpid=8624
    -trigger=true
} {
    UINT32 safeCnt 
    UINT32 taskId *
}

wvEvent EVENT_TASKUNSAFE 10007 {
    -name=taskUnsafe
    -class=task
    -helpid=8640
    -trigger=true
} {
    UINT32 safeCnt 
    UINT32 taskId *
}

wvEvent EVENT_SEMBCREATE 10008 {
    -name=semBCreate
    -class=sem
    -helpid=8320
    -trigger=true
} {
    UINT32 semOwner 
    UINT32 options 
    UINT32 semId *
}

wvEvent EVENT_SEMCCREATE 10009 {
    -name=semCCreate
    -class=sem
    -helpid=8336
    -trigger=true
} {
    UINT32 initialCount 
    UINT32 options 
    UINT32 semId *
}

wvEvent EVENT_SEMDELETE 10010 {
    -name=semDelete
    -class=sem
    -helpid=8432
    -trigger=true
} {
    UINT32 qHead
    UINT32 recurse
    UINT32 state
    UINT32 semId *
}

wvEvent EVENT_SEMFLUSH 10011 {
    -name=semFlush
    -class=sem
    -helpid=8416
    -trigger=true
} {
    UINT32 qHead
    UINT32 recurse
    UINT32 state
    UINT32 semId *
}

wvEvent EVENT_SEMGIVE 10012 {
   -name=semGive
   -class=sem
   -helpid=8368
    -trigger=true
} {
   UINT32 recurse 
   UINT32 semOwner 
   UINT32 semId *
}

wvEvent EVENT_SEMMCREATE 10013 {
    -name=semMCreate
    -class=sem
    -helpid=8352
    -trigger=true
} {
    UINT32 semOwner 
    UINT32 options 
    UINT32 semId *
}

wvEvent EVENT_SEMMGIVEFORCE 10014 {
    -name=semMGiveForce
    -class=sem
    -helpid=8384
    -trigger=true
} {
    UINT32 semOwner 
    UINT32 options 
    UINT32 semId *
}

wvEvent EVENT_SEMTAKE 10015 {
    -name=semTake
    -class=sem
    -helpid=8400
    -trigger=true
} {
    UINT32 recurse 
    UINT32 semOwner 
    UINT32 semId *
}

wvEvent EVENT_WDCREATE 10016 {
    -name=wdCreate
    -class=wd
    -helpid=8768
    -trigger=true
} {
    UINT32 wdId *
}

wvEvent EVENT_WDDELETE 10017 {
    -name=wdDelete
    -class=wd
    -helpid=8816
    -trigger=true
} {
    UINT32 wdId * 
}

wvEvent EVENT_WDSTART 10018 {
    -name=wdStart
    -class=wd
    -helpid=8784
    -trigger=true
} {
    UINT32 delay 
    UINT32 wdId * 
}

wvEvent EVENT_WDCANCEL 10019 {
    -name=wdCancel
    -class=wd
    -helpid=8800
    -trigger=true
} {
    UINT32 wdId *
}

wvEvent EVENT_MSGQCREATE 10020 {
    -name=msgQCreate
    -class=msgq
    -helpid=8256
    -trigger=true
} {
    UINT32 options 
    UINT32 maxMsgLen 
    UINT32 maxMsg 
    UINT32 msgQId *
}

wvEvent EVENT_MSGQDELETE 10021 {
    -name=msgQDelete
    -class=msgq
    -helpid=8304
    -trigger=true
} {
    UINT32 msgQId *
}

wvEvent EVENT_MSGQRECEIVE 10022 {
    -name=msgQReceive
    -class=msgq
    -helpid=8272
    -trigger=true
} {
    UINT32 timeout 
    UINT32 bufSize 
    UINT32 buffer 
    UINT32 msgQId *
}

wvEvent EVENT_MSGQSEND 10023 {
    -name=msgQSend
    -class=msgq
    -helpid=8288
    -trigger=true
} {
    UINT32 priority 
    UINT32 timeout 
    UINT32 bufSize 
    UINT32 buffer 
    UINT32 msgQId *
}

wvEvent EVENT_SIGNAL 10024 {
    -name=signal
    -class=sig
    -helpid=8448
    -trigger=true
} {
    UINT32 handler 
    UINT32 signo *
}

wvEvent EVENT_SIGSUSPEND 10025 {
    -name=sigsuspend
    -class=sig
    -helpid=8480
    -trigger=true
} {
    UINT32 pSet
}

wvEvent EVENT_PAUSE 10026 {
    -name=pause
    -class=sig
    -helpid=8496
    -trigger=true
} {
    UINT32 taskId
}

wvEvent EVENT_KILL 10027 {
    -name=kill
    -class=sig
    -helpid=8512
    -trigger=true
} {
   UINT32 taskId 
   UINT32 signo
}

wvEvent EVENT_SAFE_PEND 10028 {
    -notimestamp
    -name=safePend
    -class=task
    -helpid=8656
} {
    UINT32 taskId *
}

wvEvent EVENT_SIGWRAPPER 10029 {
    -name=sigwrapper
    -class=sig
    -helpid=8464
    -trigger=true
} {
    UINT32 taskId *
    UINT32 signo 
}

wvEvent EVENT_WINDSPAWN 600 { -notimestamp -nosearch } {
   UINT32 taskId *
   UINT32 priority
}

wvEvent EVENT_WINDDELETE 601 { -notimestamp -nosearch } {
   UINT32 taskId *
}

wvEvent EVENT_WINDSUSPEND 602 { -notimestamp -nosearch } {
   UINT32 taskId *
}

wvEvent EVENT_WINDRESUME 603 { -notimestamp -nosearch } {
   UINT32 taskId *
}

wvEvent EVENT_WINDPRIORITYSETRAISE 604 { -notimestamp -nosearch } {
   UINT32 taskId *
   UINT32 oldPriority 
   UINT32 priority
}

wvEvent EVENT_WINDPRIORITYSETLOWER 605 { -notimestamp -nosearch } {
   UINT32 taskId *
   UINT32 oldPriority 
   UINT32 priority
}

wvEvent EVENT_WINDSEMDELETE 606 { -notimestamp -nosearch } {
   UINT32 semId *
}

wvEvent EVENT_WINDTICKANNOUNCETMRSLC 607 {
    -notimestamp
    -name=tick_timeslice
    -class=tick
    -trigger=true
} {}

wvEvent EVENT_WINDTICKANNOUNCETMRWD 608 {
    -notimestamp
    -name=tick_wd
    -class=tick
    -helpid=8688
    -trigger=true
} {
    UINT32 wdId *
}

wvEvent EVENT_WINDDELAY	609 { -notimestamp -nosearch } { 
    UINT32 ticks
}

wvEvent EVENT_WINDUNDELAY 610 { -notimestamp -nosearch } {
   UINT32 taskId *
}

wvEvent EVENT_WINDWDSTART 611 { -notimestamp -nosearch } {
   UINT32 wdId *
}

wvEvent EVENT_WINDWDCANCEL 612 { -notimestamp -nosearch } {
   UINT32 wdId *
}

wvEvent EVENT_WINDPENDQGET 613 { -notimestamp -nosearch } {
    UINT32 taskId *
}

wvEvent EVENT_WINDPENDQFLUSH 614 { -notimestamp -nosearch } {
   UINT32 taskId *
}

wvEvent EVENT_WINDPENDQPUT 615 { -notimestamp -nosearch } {}

wvEvent EVENT_WINDPENDQTERMINATE 617 { -notimestamp -nosearch } {
   UINT32 taskId *
}

wvEvent EVENT_WINDTICKUNDELAY 618 {
    -notimestamp
    -name=tick_undelay
    -class=tick
    -helpid=8704
    -trigger=true
} {
    UINT32 taskId *
}

wvEvent EVENT_OBJ_TASK 619 { -notimestamp -nosearch } {
   UINT32 taskId *
}

wvEvent EVENT_OBJ_SEMGIVE 620 { -notimestamp -nosearch } {
   UINT32 semId *
}

wvEvent EVENT_OBJ_SEMTAKE 621 { -notimestamp -nosearch } {
   UINT32 semId *
}

wvEvent EVENT_OBJ_SEMFLUSH 622 { -notimestamp -nosearch } {
    UINT32 semId *
}

wvEvent EVENT_OBJ_MSGSEND 623 { -notimestamp -nosearch } {
   UINT32 msgQId *
}

wvEvent EVENT_OBJ_MSGRECEIVE 624 { -notimestamp -nosearch } {
   UINT32 msgQId *
}

wvEvent EVENT_OBJ_MSGDELETE 625 { -notimestamp -nosearch } {
   UINT32 msgQId *
}

wvEvent EVENT_OBJ_SIGPAUSE 626 { -notimestamp -nosearch } {
   UINT32 taskId
}

wvEvent EVENT_OBJ_SIGSUSPEND 627 { -notimestamp -nosearch } {
   UINT32 sigset
}

wvEvent EVENT_OBJ_SIGKILL 628 { -notimestamp -nosearch } {
   UINT32 taskId
}

wvEvent EVENT_WINDTICKTIMEOUT 631 {
    -notimestamp
    -name=tick_timeout
    -class=tick
    -helpid=8720
    -trigger=true
} {
    UINT32 taskId
}

wvEvent EVENT_OBJ_SIGWAIT 632 { -notimestamp -nosearch } { UINT32 taskId }

wvEvent EVENT_WIND_EXIT_DISPATCH 52 { -nosearch } {
   UINT32 taskId
   UINT32 priority
}

wvEvent EVENT_WIND_EXIT_NODISPATCH 53 { -nosearch } { UINT32 priority }

wvEvent EVENT_WIND_EXIT_DISPATCH_PI 54 { -nosearch } {
   UINT32 taskId
   UINT32 priority
}

wvEvent EVENT_WIND_EXIT_NODISPATCH_PI 55 { -nosearch } { UINT32 priority }

wvEvent EVENT_WIND_EXIT_IDLE 56 { -nosearch } {}

wvEvent EVENT_TASKLOCK 57 {
    -name=taskLock
    -class=task
    -trigger=true
} {}

wvEvent EVENT_TASKUNLOCK 58 {
    -name=taskUnlock
    -class=task
    -trigger=true
} {}

wvEvent EVENT_TICKANNOUNCE 59 {
    -name=tickAnnounce
    -class=tick
    -trigger=true
} {}

wvEvent EVENT_EXCEPTION 60 {
    -name=exception
    -class=sig
    -helpid=8208
    -trigger=true
} {
    UINT32 exception
}

wvEvent EVENT_TASK_STATECHANGE 61 { -nosearch } {
    UINT32 taskId *
    UINT32 newState
}

wvEvent EVENT_STATECHANGE 70 { -nosearch } {}

wvEvent EVENT_VALUE 72 { -name=value } { UINT32 value }
   
#
# end of VxWorks events
#

