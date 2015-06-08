# sh-simnt.tcl - x86-specific constants for host shell Tcl implementation
#
# Copyright 1995-2002 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01l,13nov01,aeg  added offset(WIND_TCB,events), offset(SEMPAHORE,events), &
#                  offset(MSG_Q,events)
# 01k,02nov01,tam  undid SM_PARTITION offsets changes
# 01j,02nov01,tam  updated PARTITION and SM_PARTITION offsets
# 01i,19oct01,fmk  change offset for FD_ENTRY,inuse and sizeof FD_ENTRY
# 01h,12oct01,tcr  Correct RBUFF_TYPE offsets for change in size of semaphore
# 01g,02oct01,fmk  add offset for taskTicks
# 01f,19sep01,aeg  updated WIND_TCB offsets to match taskLib.h v04j changes.
# 01e,01oct98,nps  add in WV2.0 support (rBuff & trg offsets).
# 01d,29jul98,j_k  changed references of simpc back to simnt
# 01c,28jul98,fle  changed reference to simnt into simpc
# 01b,28may98,pdn  fixed the excInfoShow()
# 01a,06nov97,pdn  written based on sh-i86.tcl.
#

# DESCRIPTION
#
# This modules holds architecture dependant routines and globals
#
# RESOURCE FILES
# wpwr/host/resource/tcl/dbgI86Lib.tcl
#


source "[wtxPath host resource tcl]dbgI86Lib.tcl"

set offset(WIND_TCB,timeout)		0x018
set offset(WIND_TCB,activeNode) 	0x020
set offset(WIND_TCB,objCore)		0x030
set offset(WIND_TCB,name)		0x034
set offset(WIND_TCB,options)		0x038
set offset(WIND_TCB,status)		0x03c
set offset(WIND_TCB,priority)		0x040
set offset(WIND_TCB,priNormal)		0x044
set offset(WIND_TCB,pPendQ)             0x05c
set offset(WIND_TCB,safeCnt)		0x060
set offset(WIND_TCB,entry)		0x074
set offset(WIND_TCB,pStackBase)		0x078
set offset(WIND_TCB,pStackLimit)	0x07c
set offset(WIND_TCB,pStackEnd)		0x080
set offset(WIND_TCB,errorStatus)	0x084
set offset(WIND_TCB,taskTicks)		0x094
set offset(WIND_TCB,events)		0x0dc
set offset(WIND_TCB,regs)		0x11c
set offset(WIND_TCB,pc)			0x140
set offset(WIND_TCB,sp)			0x128

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

set offset(SYMBOL,name)                 0x004

set offset(WDOG,timeout)                0x00c
set offset(WDOG,status)                 0x014
set offset(WDOG,routine)                0x018
set offset(WDOG,parameter)              0x01c

set offset(MSG_NODE,msgLength)		0x004
set offset(MSG_NODE,message)		0x008

set offset(Q_JOB_HEAD,pendQ)		0x010
set offset(Q_FIFO_G_HEAD,pFifoQ)        0x008

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
set offset(FREE_BLOCK,node)		0x008
set offset(FREE_BLOCK,hdr.nWords)	0x004

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
set offset(TRIGGER,hitCnt)              0x044 

#
# Define a gopher that will fetch the CPU registers from the TCB of a 
# given task.  The gopher pointer should point to the TCB upon evaluation
# of this string.
#

set regSetGopher "
    <+$offset(WIND_TCB,regs) @@@@@@@@@@>"
#                            edi esi ebp esp ebx edx ecx eax eflags pc


# register name list (used by mRegs)

set regNList {edi esi ebp esp ebx edx ecx eax eflags pc}

# gophers that will get the PC value and the SP value

set pcGetGopher "<+$offset(WIND_TCB,pc) @>"
set spGetGopher "<+$offset(WIND_TCB,sp) @>"

##############################################################################
#
# regSetOfContext - get the system context registers
#
# A procedure to get the system context registers, in the same format
# as regSetGopher would return.
#
# SYNOPSIS: regSetOfContext [ctype cid]
#
# PARAMETERS:
#       ctype: context type
#       cid: context id
#
# RETURNS: N/A
#

proc regSetOfContext {ctype cid} {
    set regBlk [wtxRegsGet $ctype $cid REG_SET_IU 0 [expr 10*4]]
    set rset [memBlockGet -l $regBlk 0 10]
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
#
# availRegPuts - puts available registers
#
# SYNOPSIS: availRegPuts
#
# RETURNS: N/A
#

proc availRegPuts {} {
    puts stdout "available registers are:"
    puts stdout "  edi esi ebp esp ebx edx ecx eax eflags pc"
}

##############################################################################
#
# pcOfRegSet - get pc register
#
# SYNOPSIS: pcOfRegSet [regSet]
#
# PARAMETERS:
#       regSet: set of register values
#
# RETURNS: content of pc
#

proc pcOfRegSet {regSet} {
    return [lindex $regSet 9]
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
	"edi"   [lindex $regSet 0]  "esi"  [lindex $regSet 1] \
	"ebp"   [lindex $regSet 2]  "esp"  [lindex $regSet 3]]
    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
	"ebx"   [lindex $regSet 4]  "edx"  [lindex $regSet 5] \
	"ecx"   [lindex $regSet 6]  "eax"  [lindex $regSet 7]]
    puts stdout [format "%-6s = %8x   %-6s = %8x" \
	"eflags" [lindex $regSet 8]  "pc"  [lindex $regSet 9]]
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
    return "{\"x86 Registers\" {edi esi ebp esp ebx edx ecx eax eflags pc}}"
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
    set regRawVec [lrange $taskInfoList [expr $tiLen - 10] end]
    return [list $regRawVec]
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

set endianFlag "-L"

#
# The table of exception names.  These are indexed by exception number.
#

set i86ExcName {
    "Divide Error"
    "Debug"
    "Nonmaskable Interrupt"
    "Breakpoint"
    "Overflow"
    "Bound"
    "Invalid Opcode"
    "Device Not Available"
    "Double Fault"
    "Coprocessor Overrun"
    "Invalid TSS"
    "Segment Not Present"
    "Stack Fault"
    "General Protection Fault"
    "Page Fault"
    "Intel Reserved"
    "Coprocessor Error"
    "Alignment Check"
}

proc excInfoShow {vector esf} {
    global i86ExcName

    set esfInfo [wtxGopherEval "$esf @w @w +4 @l @l"]
    bindNamesToList {valid vec pc ps} $esfInfo

    set excMsg [lindex $i86ExcName $vec]
    if {$excMsg == ""} {
        set excMsg "Unknown exception: $vec"
    }

    puts stdout "\n$excMsg"
    puts stdout [format "Program Counter:            0x%08x" $pc]
    puts stdout [format "Status Register:            0x%08x" $ps]
    puts stdout ""
}


##############################################################################
#
# mangledSymListGet - get a list of the mangled symbols
#
# This procedure fetches the list of the overloaded function or method names.
#
# SYNOPSIS: mangledSymListGet
#
# PARAMETERS:
#    name: root name of symbol
#
# RETURNS: a list of mangled names or an empty list if none.
#

proc mangledSymListGet {name} {
    return [wtxSymListGet -name "^_${name}__.*"]
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

    if {[string index $symbol 0] == "_"} {
        return [string range $symbol 1 end]
    } else {
        return $symbol
    }
}
