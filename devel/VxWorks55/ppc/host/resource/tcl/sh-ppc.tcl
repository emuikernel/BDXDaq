# sh-ppc.tcl - PowerPC-specific constants for host shell Tcl implementation 
#
# Copyright 1996-2002 Wind River Systems, Inc. 
#
# modification history
# -----------------------
# 01y,22may02,kab  SPR 74372: Add PPC440 support
# 01x,08nov01,aeg  added offset(WIND_TCB,events), offset(SEMPAHORE,events), &
#		   offset(MSG_Q,events)
# 01w,02nov01,tam  undid SM_PARTITION offsets changes
# 01v,02nov01,tam  updated PARTITION and SM_PARTITION offsets
# 01u,19oct01,fmk  change offset for FD_ENTRY,inuse and sizeof FD_ENTRY
# 01t,12oct01,tcr  Correct RBUFF_TYPE offsets for change in size of semaphore
# 01s,10oct01,fmk  add offset for WIND_TCB,taskTicks
# 01r,05oct01,yvp  Changed TCB register offsets, after TCB changed.
# 01q,19sep01,aeg  updated WIND_TCB offsets to match taskLib.h v04j changes.
# 01p,03nov00,s_m  added PPC405 support
# 01o,13oct98,jmp  added availRegPuts{} routine (SPR# 22669).
# 01n,24auf98,tpr  added PowerPC EC 603 support.
# 01t,19aug98,drm  added offset information for distributed objects
# 01s,24jul98,dbt  fixed "mq" register print.
# 01r,01jul98,c_s  WindView 2 -> Tornado 2 Merge
# 01q,06may98,nps  inserted new rBuff field.
# 01p,12feb98,pr   Added hitCnt to trigger offsets.
# 01o,10feb98,pr   Added rBuff offsets.
# 01n,08dec97,pr   Added event triggering offsets.
# 01p,05jun98,f_l  added offset information for taskWait routines, for
#		   Shared Memory objects and for POSIX semaphores
#		   and msg queues.
# 01o,17apr98,rlp  removed numMaxMsgsSent for backward compatibility.
# 01n,05nov97,rlp  added numMaxMsgsSent for tracking messages sent.
# 01m,26feb97,tam  fixed typo error for FP exception tests.
# 01l,12feb97,tam  Added handling of floating point exceptions (SPR #7840).
# 01k,29oct96,pad  Added routine underscoreStrip (SPR #7159).
# 01j,14oct96,pad  Removed the leading underscore for determining C++ mangled
#                  names (SPR #7327)
# 01i,30sep96,pad  Added mangledSymListGet procedure
# 01h,29may96,elp  added mq in regSetPrint{} (fix SPR# 6242).
# 01g,29apr96,p_m  added pcGetGopher and spGetGopher to fix SPR#6409.
# 01f,01apr96,tam  added _EXC_OFF_PROT case in excInfoShow() for PPC403. 
# 01e,20mar96,elp  fixed regNList and pcOfRegSet{} (pc index is 35 not 36)
#		   + source dbgPpcLib.tcl.
# 01d,05mar96,ms   added excInfoShow.
# 01c,04mar96,kkk  added regSetOfContext(), regOffsetFromIndexGet(), and
#  		   regNList variable so that mRegs works. Clean up.
# 01b,29jan96,tpr  added set offset(WIND_TCB,safeCnt) 0x060
# 01a,15nov95,tpr  written.
#

# DESCRIPTION
# 
# This modules holds architecture dependant routines and globals
# 
# RESOURCE FILES
# wpwr/host/resource/tcl/dbgPpcLib.tcl
#

source "[wtxPath host resource tcl]dbgPpcLib.tcl"

# globals

set offset(WIND_TCB,timeout)		0x018
set offset(WIND_TCB,activeNode) 	0x020
set offset(WIND_TCB,objCore)		0x030
set offset(WIND_TCB,name)		0x034
set offset(WIND_TCB,options)		0x038
set offset(WIND_TCB,status)		0x03c
set offset(WIND_TCB,priority)		0x040
set offset(WIND_TCB,priNormal)		0x044
set offset(WIND_TCB,pPendQ)		0x05c
set offset(WIND_TCB,safeCnt)		0x060
set offset(WIND_TCB,entry)		0x074
set offset(WIND_TCB,pStackBase)		0x078
set offset(WIND_TCB,pStackLimit)	0x07c
set offset(WIND_TCB,pStackEnd)		0x080
set offset(WIND_TCB,errorStatus)	0x084
set offset(WIND_TCB,taskTicks)		0x094
set offset(WIND_TCB,events)		0x0dc
set offset(WIND_TCB,regs)		0x130
set offset(WIND_TCB,sp)			0x134
set offset(WIND_TCB,pc)			0x1bc

set offset(OBJ_CLASS,objPartId)		0x004

set offset(SEMAPHORE,semType)		0x004
set offset(SEMAPHORE,options)		0x005
set offset(SEMAPHORE,qHead)		0x008
set offset(SEMAPHORE,count)		0x018
set offset(SEMAPHORE,events)		0x01c

set offset(POSIX_SEM,semId)             0x004
set offset(POSIX_SEM,refCnt)            0x008
set offset(POSIX_SEM,sem_name)          0x00c

set offset(MSG_Q,msgQ)			0x004
set offset(MSG_Q,freeQ)			0x024
set offset(MSG_Q,options)		0x044
set offset(MSG_Q,maxMsgs)		0x048
set offset(MSG_Q,maxMsgLength)		0x04c
set offset(MSG_Q,sendTimeouts)		0x050
set offset(MSG_Q,recvTimeouts)		0x054
set offset(MSG_Q,events)		0x058

set offset(POSIX_MSG_Q_DES,f_flags)     0x004
set offset(POSIX_MSG_Q_DES,f_data)      0x008

set offset(POSIX_MSG_Q,msgq_cond_read)  0x000
set offset(POSIX_MSG_Q,msgq_cond_data)  0x010
set offset(POSIX_MSG_Q,msgq_sym)        0x020
set offset(POSIX_MSG_Q,msgq_attr)       0x0e4

set offset(POSIX_MSG_Q_ATTR,mq_maxmsg)  0x000
set offset(POSIX_MSG_Q_ATTR,mq_msgsize) 0x004
set offset(POSIX_MSG_Q_ATTR,mq_flags)   0x008
set offset(POSIX_MSG_Q_ATTR,mq_curmsgs) 0x00c

set offset(SM_OBJ,objType)              0x004

set offset(SM_OBJ_TCB,localTcb)         0x008
set offset(SM_OBJ_TCB,ownerCpu)         0x00c

set offset(SM_OBJ_MEM_HDR,smTcbPart)    0x00c
set offset(SM_OBJ_MEM_HDR,smSemPart)    0x030
set offset(SM_OBJ_MEM_HDR,smNamePart)   0x074
set offset(SM_OBJ_MEM_HDR,smMsgQPart)   0x0b8
set offset(SM_OBJ_MEM_HDR,smPartPart)   0x0fc
set offset(SM_OBJ_MEM_HDR,smSysPart)    0x140
set offset(SM_OBJ_MEM_HDR,nameDtb)      0x184
set offset(SM_OBJ_MEM_HDR,objCpuTbl)    0x1b0
set offset(SM_OBJ_MEM_HDR,maxSems)      0x1b4
set offset(SM_OBJ_MEM_HDR,maxMsgQueues) 0x1b8
set offset(SM_OBJ_MEM_HDR,maxTasks)     0x1bc
set offset(SM_OBJ_MEM_HDR,maxMemParts)  0x1c0
set offset(SM_OBJ_MEM_HDR,maxNames)     0x1c4
set offset(SM_OBJ_MEM_HDR,curNumSemB)   0x1c8
set offset(SM_OBJ_MEM_HDR,curNumSemC)   0x1cc
set offset(SM_OBJ_MEM_HDR,curNumMsgQ)   0x1d0
set offset(SM_OBJ_MEM_HDR,curNumTask)   0x1d4
set offset(SM_OBJ_MEM_HDR,curNumPart)   0x1d8
set offset(SM_OBJ_MEM_HDR,curNumName)   0x1dc

set offset(SM_SEM,objType)              0x004
set offset(SM_SEM,smPendQ)              0x00c
set offset(SM_SEM,count)                0x014

set offset(SM_MSG_Q,msgQSem)            0x008
set offset(SM_MSG_Q,msgQ)               0x024
set offset(SM_MSG_Q,freeQSem)           0x02c
set offset(SM_MSG_Q,freeQ)              0x048
set offset(SM_MSG_Q,options)            0x050
set offset(SM_MSG_Q,maxMsgs)            0x054
set offset(SM_MSG_Q,maxMsgLength)       0x058
set offset(SM_MSG_Q,sendTimeouts)       0x05c
set offset(SM_MSG_Q,recvTimeouts)       0x060

set offset(SM_MSG_NODE,msgLength)       0x008
set offset(SM_MSG_NODE,message)         0x00c

set offset(SM_PARTITION,freeList)               0x008
set offset(SM_PARTITION,totalWords)             0x028
set offset(SM_PARTITION,curBlocksAllocated)     0x034
set offset(SM_PARTITION,curWordsAllocated)      0x038
set offset(SM_PARTITION,cumBlocksAllocated)     0x03c
set offset(SM_PARTITION,cumWordsAllocated)      0x040

set offset(SM_FREE_BLOCK,node)          0x010

set offset(SM_BLOCK_HDR,nWords)         0x004

set offset(DIST_OBJ_NODE,objNodeReside) 0x008
set offset(DIST_OBJ_NODE,objNodeId)     0x00c

set offset(TBL_NODE,tblMsgQId)          0x000
set sizeof(TBL_NODE)                    0x008

set offset(SYMBOL,name)                 0x004

set offset(WDOG,timeout)                0x00c
set offset(WDOG,status)                 0x014
set offset(WDOG,routine)                0x018
set offset(WDOG,parameter)              0x01c

set offset(MSG_NODE,msgLength)		0x004
set offset(MSG_NODE,message)		0x008

set offset(Q_JOB_HEAD,pendQ)		0x010
set offset(Q_FIFO_G_HEAD,pFifoQ)	0x008

set offset(DEV_HDR,drvNum)		0x008
set offset(DEV_HDR,name)		0x00c

set offset(DL_NODE,next)		0x000
set offset(DL_LIST,head)		0x000

set offset(DEV_HDR,drvNum)		0x008

set offset(FD_ENTRY,pDevHdr)		0x000
set offset(FD_ENTRY,name)		0x008
set offset(FD_ENTRY,inuse)		0x010
set sizeof(FD_ENTRY)			0x020

set offset(PARTITION,totalWords)        0x034
set offset(PARTITION,curBlocksAllocated) 0x040
set offset(PARTITION,curWordsAllocated) 0x044
set offset(PARTITION,cumBlocksAllocated) 0x048
set offset(PARTITION,cumWordsAllocated) 0x04C
set offset(PARTITION,freeList)          0x004
set offset(FREE_BLOCK,node)             0x008
set offset(FREE_BLOCK,hdr.nWords)       0x004

set offset(RBUFF_TYPE,objCore)          0x000
set offset(RBUFF_TYPE,buffRead)         0x038
set offset(RBUFF_TYPE,buffWrite)        0x03c
set offset(RBUFF_TYPE,dataRead)         0x040
set offset(RBUFF_TYPE,dataWrite)        0x044

set offset(RBUFF_TYPE,srcPart)          0x0b0
set offset(RBUFF_TYPE,options)          0x0b4
set offset(RBUFF_TYPE,buffSize)         0x0b8
set offset(RBUFF_TYPE,currBuffs)        0x0bc
set offset(RBUFF_TYPE,threshold)        0x0c0
set offset(RBUFF_TYPE,minBuffs)         0x0c4
set offset(RBUFF_TYPE,maxBuffs)         0x0c8
set offset(RBUFF_TYPE,maxBuffsActual)   0x0cc
set offset(RBUFF_TYPE,emptyBuffs)       0x0d0
set offset(RBUFF_TYPE,dataContent)      0x0d4
set offset(RBUFF_TYPE,writesSinceReset) 0x0d8
set offset(RBUFF_TYPE,readsSinceReset)  0x0dc
set offset(RBUFF_TYPE,timesExtended)    0x0e0
set offset(RBUFF_TYPE,timesXThreshold)  0x0e4
set offset(RBUFF_TYPE,bytesWritten)     0x0e8
set offset(RBUFF_TYPE,bytesRead)        0x0ec
set offset(RBUFF_TYPE,bytesPeak)        0x0f0


set offset(TRIGGER,objCore)          	0x000 
set offset(TRIGGER,eventId)          	0x004 
set offset(TRIGGER,status)           	0x006 
set offset(TRIGGER,disable)          	0x008 
set offset(TRIGGER,contexType)       	0x00c 
set offset(TRIGGER,contextId)        	0x010 
set offset(TRIGGER,objId)            	0x014 
set offset(TRIGGER,chain)            	0x018 
set offset(TRIGGER,next)             	0x01c 
set offset(TRIGGER,conditional)      	0x020 
set offset(TRIGGER,condType)         	0x024 
set offset(TRIGGER,condEx1)          	0x028 
set offset(TRIGGER,condOp)           	0x02c 
set offset(TRIGGER,condEx2)          	0x030 
set offset(TRIGGER,actionType)       	0x034 
set offset(TRIGGER,actionFunc)       	0x038 
set offset(TRIGGER,actionArg)        	0x03c 
set offset(TRIGGER,actionDef)        	0x040 
set offset(TRIGGER,hitCnt)        	0x044 

#
# Define a gopher that will fetch the CPU registers from the TCB of a 
# given task.  The gopher pointer should point to the TCB upon evaluation
# of this string.
#

set regSetGopher "
    <+$offset(WIND_TCB,regs) @@@@@@@@ @@@@@@@@ @@@@@@@@ @@@@@@@@ @ @ @ @ @ @ @>"
#                            r0-r7    r8-r15   r16-r23  r24-31  MSR CTR  CR  MQ
#								   LR  PC XER

# Define the register names list

set regNList {r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 r13 r14 r15 r16 r17 \
	      r18 r19 r20 r21 r22 r23 r24 r25 r26 r27 r28 r29 r30 r31 msr lr \
	      ctr pc cr xer mq}

# gophers that will get the PC value and the SP value

set pcGetGopher "<+$offset(WIND_TCB,pc) @>"
set spGetGopher "<+$offset(WIND_TCB,sp) @>"

##############################################################################
#
# availRegPuts - puts available registers
#
# SYNOPSIS: availRegPuts
#
# RETURNS: N/A
#

proc availRegPuts {} {
    puts stdout "available registers are:"
    puts stdout "   r0  r1  r2  r3  r4  r5  r6  r7"
    puts stdout "   r8  r9 r10 r11 r12 r13 r14 r15"
    puts stdout "  r16 r17 r18 r19 r20 r21 r22 r23"
    puts stdout "  r24 r25 r26 r27 r28 r29 r30 r31"
    puts stdout "  msr  lr ctr  pc  cr xer  mq"
}

#############################################################################
#
# regSetOfContext - get the system context registers
#
# A procedure to get the system context registers, in the same format
# as regSetGopher would return.
#
# SYNOPSIS: regSetOfContext [ctype cid]
#
# PARAMETERS:
#       ctype: context typE
#       cid: context id
#
# RETURNS: N/A
#

proc regSetOfContext {ctype cid} {
    set regBlk [wtxRegsGet $ctype $cid REG_SET_IU 0 [expr 39*4]]
    set rset [memBlockGet -l $regBlk 0 39]
    memBlockDelete $regBlk
    return $rset
}

##############################################################################
#
# regOffsetFromIndexGet - return register offset from its index in the regset
#
# To write the register we need to know its offset in the regset. Indeed in
# some cases the offset is not always (index * 4) because of padding.
#
# SYNOPSIS:
#       regAddrGet index
#
# PARAMETERS:
#       index: index in the register set of the concerned register
#
# RETURN: the offset of the register
#
proc regOffsetFromIndexGet {regInd} {

       return [expr $regInd * 4]
       }

##############################################################################
# pcOfRegSet - get pc register
#
# SYNOPSIS: pcOfRegSet [regSet]
#
# PARAMETERS:
#       regSet: set of register values
#
# RETURNS: content of pc

proc pcOfRegSet {regSet} {
    return [lindex $regSet 35]
}

##############################################################################
#
# regSetPrint - prints a register set
#
# SYNOPSIS: regSetPrint [regSet]
#
# PARAMETERS:
#       regSet: set of register values
#
# RETURNS: N/A
#

proc regSetPrint {regSet} {
    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
	"r0" [lindex $regSet 0] "r1/sp" [lindex $regSet 1] \
	"r2" [lindex $regSet 2] "r3" [lindex $regSet 3]]
    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
	"r4" [lindex $regSet 4] "r5" [lindex $regSet 5] \
	"r6" [lindex $regSet 6] "r7" [lindex $regSet 7]]
    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
	"r8" [lindex $regSet 8] "r9" [lindex $regSet 9] \
	"r10" [lindex $regSet 10] "r11" [lindex $regSet 11]]
    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
	"r12" [lindex $regSet 12] "r13" [lindex $regSet 13] \
	"r14" [lindex $regSet 14] "r15" [lindex $regSet 15]]
    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
	"r16" [lindex $regSet 16] "r17" [lindex $regSet 17] \
	"r18" [lindex $regSet 18] "r19" [lindex $regSet 19]]
    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
	"r20" [lindex $regSet 20] "r21" [lindex $regSet 21] \
	"r22" [lindex $regSet 22] "r23" [lindex $regSet 23]]
    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
	"r24" [lindex $regSet 24] "r25" [lindex $regSet 25] \
	"r26" [lindex $regSet 26] "r27" [lindex $regSet 27]]
    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
	"r28" [lindex $regSet 28] "r29" [lindex $regSet 29] \
	"r30" [lindex $regSet 30] "r31" [lindex $regSet 31]]
    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
	"msr" [lindex $regSet 32] "lr" [lindex $regSet 33] \
	"ctr" [lindex $regSet 34] "pc" [lindex $regSet 35]]
    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x" \
	"cr" [lindex $regSet 36] "xer" [lindex $regSet 37] \
    	"mq" [lindex $regSet 38]]
}

##############################################################################
#
# taskRegSetStruct -
#
# SYNOPSIS: taskRegSetStruct
#
# RETURNS:
#

proc taskRegSetStruct {} {
    return "{\"ppc Registers\" {{Register {r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 r13 r14 r15 r16 r17 r18 r19 r20 r21 r22 r23 r24 r25 r26 r27 r28 r29 r30 r31}}
        {Other {msr lr  ctr pc  cr  xer mq }}}}"
}

##############################################################################
#
# taskRegSetVec -
#
# SYNOPSIS: taskRegSetVec [taskInfoList]
#
# PARAMETERS:
#       taskInfoList:
#
# RETURNS:
#

proc taskRegSetVec {taskInfoList} {
    set tiLen [llength $taskInfoList]
    set regRawVec [lrange $taskInfoList [expr $tiLen - 39] end]
    return "{ \
                [list [lrange $regRawVec 0 31]] \
                [list [lrange $regRawVec 32 38]] \
            }"
}

#
# Define a gopher that will check the stack for the end of the 0xee sentinel
# area.  This is dependent on the stack sense of the architecture.
#

set stackGrows down

#
# Define the endianness flag that should be used when memory blocks are
# created.  XXX this should be based on data reported from the target 
# server; some architectures are ambi-endian.
#

set endianFlag "-B"


##############################################################################
#
# excInfoShow - show exception info
#

proc excInfoShow {vec pEsf} {
    global ppcExcName
    global __wtxCpuType
    global cpuType

    set cpu $cpuType($__wtxCpuType)
    
    # get important parts of ESF info via gopher

    set pcAddr	[expr $pEsf + 164]
    set msrAddr [expr $pEsf + 152]
    set darAddr	[expr $pEsf + 12]
    set	dsierAddr [expr $pEsf + 16]
    set fpcsrAddr [expr $pEsf + 20]

    set excInfo [wtxGopherEval "$pcAddr @ $msrAddr @ $darAddr @ $dsierAddr @ $fpcsrAddr @"]
    bindNamesToList {pc msr dar dsier fpcs} $excInfo

    # initialize list of floating point exception messages

    set fpExcIndLst {0x10000000 0x8000000 0x4000000 0x2000000 0x1000000 \
		     0x800000 0x400000 0x200000 0x100000 0x80000 0x400 0x200 \
		     0x100}

    set fpExcMsgLst {{"Floating point overflow"} {"Floating point underflow"} \
		   {"Floating point divide by zero"} {"Floating point inexact"}\
		   {"Floating point invalid operation for SNAN"} \
		   {"Floating point invalid operation for INF - INF"} \
		   {"Floating point invalid operation for INF / INF"} \
		   {"Floating point invalid operation for 0 / 0"} \
		   {"Floating point invalid operation for INF * 0"} \
		   {"Floating point invalid operation for invalid compare"} \
		   {"Floating point invalid operation for software request"} \
		   {"Floating point invalid operation for square root"} \
		   {"Floating point invalid operation for integer convert"}}

    # get the name of the exception

    set vec [format "%#x" $vec]
    switch $vec {
	# _EXC_OFF_RESET
	0x100		{set excMsg "system reset"}
	# _EXC_OFF_MACH
	0x200		{set excMsg "machine check"}
	# _EXC_OFF_DATA
	0x300		{set excMsg "data access"; set addr $dar}
	# _EXC_OFF_INST
	0x400		{set excMsg "instruction access"}
	# _EXC_OFF_INTR
	0x500		{set excMsg "external interrupt"}
	# _EXC_OFF_ALIGN
	0x600		{set excMsg "alignment"; set addr $dsier}
	# _EXC_OFF_PROG
	0x700		{
	    set excMsg ""
    	    set msrBits [expr $msr & 0x100000]

	    # check type of program exception 
		# for PPC405F, bit 11 is undefined in MSR.
	    
	    if {($msrBits != 0 && ($cpu == "PPC603" || $cpu == "PPC604")) ||
			($cpu == "PPC405F")} {
		# got a floating point exception: MSR[11] is set
    	    	set fpsExcBit [expr $fpcs & 0x1ff80700]
		set ix [llength $fpExcIndLst]
		foreach ix $fpExcIndLst {
    		    set bits [expr ($fpsExcBit & $ix)]
		    if {$bits != 0} {
    		        set ix [format "%#x" $ix]
		        set jx [lsearch $fpExcIndLst $ix]
		        set msg [lindex $fpExcMsgLst $jx]
			set excMsg [format "$excMsg\n$msg"]
		    }
		}
		if {$excMsg == ""} {
		    set excMsg "Unknown floating point exception"
		}
		set fpscr $fpcs
	    } else {
		set msrBits [expr $msr & 0x80000]
		if {$msrBits != 0} { 
		    set excMsg "illegal instruction program"
		} else {
		    set msrBits [expr $msr & 0x40000]
		    if {$msrBits != 0} { 
	    	        set excMsg "privileged instruction program"
		    } else {
		        set msrBits [expr $msr & 0x20000]
		        if {$msrBits != 0} { 
	    	            set excMsg "trap instruction program"
			} else {set excMsg "program"}
		    }
		}
	    }
	}
	# _EXC_OFF_FPU
	0x800		{set excMsg "floating point unavailable"}
	# _EXC_OFF_DECR
	0x900		{set excMsg "decrementer"}
	# _EXC_OFF_IOERR - 601 only
	0xa00		{set excMsg "I/O controller error"}
	# _EXC_OFF_SYSCALL
	0xc00		{set excMsg "system call"}
	# _EXC_OFF_TRACE
	0xd00		{set excMsg "trace"}
	# _EXC_OFF_DBG common to 604, BookE; SW_EMUL on 509
	0x1000		{set excMsg "debug"}
	# _EXC_OFF_FIT - 40x only
	0x1010		{set excMsg "fixed interval timer"}
	# _EXC_OFF_WD - 40x only
	0x1020		{set excMsg "watchdog timer"}
	# _EXC_OFF_LOAD_MISS - 603 and EC603 only
	0x1100		{set excMsg "data load translation miss"; set addr $dsier}
	# _EXC_OFF_STORE_MISS - 603 and EC603 only
	0x1200		{set excMsg "data store translation miss"; set addr $dsier}
	# _EXC_OFF_INST_BRK - 603, EC603 and 604 only
	0x1300		{set excMsg "instruction breakpoint exception"}
	# _EXC_OFF_SYS_MNG - 603, EC603 and 604 only
	0x1400		{set excMsg "system management exception"}
	# _EXC_OFF_RUN_TRACE - 601 only
	0x2000		{set excMsg "run-mode/trace exception"}
	# ???
	default		{set excMsg "Unknown Exception: $vec"}
    }

    if {$cpu == "PPC603"} {
	if {$vec == 0x1000} {set excMsg "instruction translation miss"}
    }

    # override some exception definitions for the 403

    if {$cpu == "PPC403" || $cpu == "PPC405" || $cpu == "PPC405F"} {
	# _EXC_OFF_CRTL
	if {$vec == 0x100} {set excMsg "critical interrupt"}
	# _EXC_OFF_PROT
	if {$vec == 0x300} {set excMsg "protection violation"}
	# _EXC_OFF_TRACE
	if {$vec == 0xd00} {set excMsg "Unknown Exception: $vec"}
	# _EXC_OFF_PIT
	if {$vec == 0x1000} {set excMsg "programmable interval timer"}
	# _EXC_OFF_DBG
	if {$vec == 0x2000} {set excMsg "debug exception"}
    }

    if {$cpu == "PPC440"} {
	# _EXC_OFF_CRTL
	if {$vec == 0x100} {set excMsg "critical interrupt"}
	# _EXC_OFF_CRTL
	if {$vec == 0x300} {set excMsg "my data access test"}
	# _EXC_OFF_DECR
	if {$vec == 0x900} {set excMsg "syscall"}
	# _EXC_OFF_SYSCALL
	if {$vec == 0xc00} {set excMsg "fixed interval timer"}
	# _EXC_OFF_TRACE
	if {$vec == 0xd00} {set excMsg "watchdog"}
	# _EXC_OFF_DATA_MISS
	if {$vec == 0xe00} {set excMsg "data TLB"}
	# _EXC_OFF_INST_MISS
	if {$vec == 0xf00} {set excMsg "instruction TLB"}
    }

    if {$cpu == "PPC604"} {
	# _EXC_OFF_ALTIVEC_UNAVAIL
	if {$vec == 0x0f20} {set excMsg "altivec unavailable exception"}
	# _EXC_OFF_ALTIVEC_ASSIST
	if {$vec == 0x1600} {set excMsg "altivec assist exception"}
	# _EXC_OFF_THERMAL
	if {$vec == 0x1700} {set excMsg "thermal exception"}
    }

    # print out exception information

    puts stdout "\n$excMsg"
    puts stdout [format "program counter:  %#x" $pc]
    if [info exists addr] {
	puts stdout [format "access address:   %#x" $addr]
    }
    puts stdout [format "machine status:   %#x" $msr]
    if [info exists fpscr] {
	puts stdout [format "FP status & ctrl: %#x" $fpscr]
    }

}

##############################################################################
#
# mangledSymListGet - get a list of the mangled symbols
#
# This procedure fetches the list of the overloaded function or method names.
#
# SYNOPSIS: mangledSymListGet name
#
# PARAMETERS:
#    name: root name of symbol
#
# RETURNS: a list of mangled names or an empty list if none.
#

proc mangledSymListGet {name} {
    return [wtxSymListGet -name "^${name}__.*"]
}

##############################################################################
#
# underscoreStrip - strip off the symbol's leading underscore
#
# This procedure removes the leading underscore in symbol names. This is
# required by some C++ demangler.
#
# SYNOPSIS: underscoreStrip symbol
#
# PARAMETERS:
#    symbol: name of symbol
#
# RETURNS: the symbol without its leading underscore.
#

proc underscoreStrip {symbol} {

    # For the PowerPc architecture, the compiler does not produce any
    # additional leading underscore, so we simply return the symbol name.

    return $symbol
}
