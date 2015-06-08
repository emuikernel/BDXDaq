# sh-coldfire.tcl - coldfire-specific constants for host shell Tcl implementation
#
# Copyright 1994-2002 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01l,26apr02,fmk  SPR 73890 - incorrect sp values from 'i' 
#                  SPR 76371 - iosDevShow does not work from windsh 
#                  - both problems due to incorrect offsets
# 01k,12dec01,jab  updated underscoreStrip; coldfire has leading underscores
# 01j,04dec01,sn   don't gratuitously remove underscores
# 01i,13nov01,aeg  added offset(WIND_TCB,events), offset(SEMPAHORE,events), &
#                  offset(MSG_Q,events)
# 01h,02nov01,tam  undid SM_PARTITION offsets changes
# 01g,02nov01,tam  updated PARTITION and SM_PARTITION offsets
# 01f,19oct01,fmk  change offset for FD_ENTRY,inuse and sizeof FD_ENTRY
# 01e,12oct01,tcr  Correct RBUFF_TYPE offsets for change in size of semaphore
# 01d,02oct01,fmk  add offset for taskTicks
# 01c,19sep01,aeg  updated WIND_TCB offsets to match taskLib.h v04j changes.
# 01b,08aug00,dh   Changed to support new REG_SET structure including MAC.
# 01a,17mar00,dra  Merged from T1.0.1 ColdFire and T2 MC68k ports.
#

# DESCRIPTION
#
# This modules holds architecture dependant routines and globals
#
# RESOURCE FILES
# wpwr/host/resource/tcl/dbgColdfireLib.tcl
#

source "[wtxPath host resource tcl]dbgColdfireLib.tcl"

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
set offset(WIND_TCB,regs)		0x11c
set offset(WIND_TCB,sp)			0x158
set offset(WIND_TCB,pc)			0x168

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

set offset(SM_OBJ,objType)		0x004

set offset(SM_OBJ_TCB,localTcb)		0x008
set offset(SM_OBJ_TCB,ownerCpu)		0x00c

set offset(SM_OBJ_MEM_HDR,smTcbPart)	0x00c
set offset(SM_OBJ_MEM_HDR,smSemPart)	0x030
set offset(SM_OBJ_MEM_HDR,smNamePart)	0x074
set offset(SM_OBJ_MEM_HDR,smMsgQPart)	0x0b8
set offset(SM_OBJ_MEM_HDR,smPartPart)	0x0fc
set offset(SM_OBJ_MEM_HDR,smSysPart)	0x140
set offset(SM_OBJ_MEM_HDR,nameDtb)	0x184
set offset(SM_OBJ_MEM_HDR,objCpuTbl)	0x1b0
set offset(SM_OBJ_MEM_HDR,maxSems)	0x1b4
set offset(SM_OBJ_MEM_HDR,maxMsgQueues)	0x1b8
set offset(SM_OBJ_MEM_HDR,maxTasks)	0x1bc
set offset(SM_OBJ_MEM_HDR,maxMemParts)	0x1c0
set offset(SM_OBJ_MEM_HDR,maxNames)	0x1c4
set offset(SM_OBJ_MEM_HDR,curNumSemB)	0x1c8
set offset(SM_OBJ_MEM_HDR,curNumSemC)	0x1cc
set offset(SM_OBJ_MEM_HDR,curNumMsgQ)	0x1d0
set offset(SM_OBJ_MEM_HDR,curNumTask)	0x1d4
set offset(SM_OBJ_MEM_HDR,curNumPart)	0x1d8
set offset(SM_OBJ_MEM_HDR,curNumName)	0x1dc

set offset(SM_SEM,objType)		0x004
set offset(SM_SEM,smPendQ)		0x00c
set offset(SM_SEM,count)		0x014

set offset(SM_MSG_Q,msgQSem)		0x008
set offset(SM_MSG_Q,msgQ)		0x024
set offset(SM_MSG_Q,freeQSem)		0x02c
set offset(SM_MSG_Q,freeQ)		0x048
set offset(SM_MSG_Q,options)		0x050
set offset(SM_MSG_Q,maxMsgs)		0x054
set offset(SM_MSG_Q,maxMsgLength)	0x058
set offset(SM_MSG_Q,sendTimeouts)	0x05c
set offset(SM_MSG_Q,recvTimeouts)	0x060

set offset(SM_MSG_NODE,msgLength)	0x008
set offset(SM_MSG_NODE,message)		0x00c

set offset(SM_PARTITION,freeList)		0x008
set offset(SM_PARTITION,totalWords)		0x028
set offset(SM_PARTITION,curBlocksAllocated)	0x034
set offset(SM_PARTITION,curWordsAllocated)	0x038
set offset(SM_PARTITION,cumBlocksAllocated)	0x03c
set offset(SM_PARTITION,cumWordsAllocated)	0x040

set offset(SM_FREE_BLOCK,node)		0x010

set offset(SM_BLOCK_HDR,nWords)		0x004

set offset(DIST_OBJ_NODE,objNodeReside)	0x008
set offset(DIST_OBJ_NODE,objNodeId)		0x00c

set offset(TBL_NODE,tblMsgQId)		    0x000
set sizeof(TBL_NODE)                    0x006

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

set offset(TRIGGER,objCore)          0x000 
set offset(TRIGGER,eventId)          0x004 
set offset(TRIGGER,status)           0x006 
set offset(TRIGGER,disable)          0x008 
set offset(TRIGGER,contexType)       0x00c 
set offset(TRIGGER,contextId)        0x010 
set offset(TRIGGER,objId)            0x014 
set offset(TRIGGER,chain)            0x018 
set offset(TRIGGER,next)             0x01c 
set offset(TRIGGER,conditional)      0x020 
set offset(TRIGGER,condType)         0x024 
set offset(TRIGGER,condEx1)          0x028 
set offset(TRIGGER,condOp)           0x02c 
set offset(TRIGGER,condEx2)          0x030 
set offset(TRIGGER,actionType)       0x034 
set offset(TRIGGER,actionFunc)       0x038 
set offset(TRIGGER,actionArg)        0x03c 
set offset(TRIGGER,actionDef)        0x040 
set offset(TRIGGER,hitCnt)           0x044 

#
# Define a gopher that will fetch the CPU registers from the TCB of a 
# given task.  The gopher pointer should point to the TCB upon evaluation
# of this string.
#

set regSetGopher "
    <+$offset(WIND_TCB,regs) @@@@@@@@ @@@@@@@@ @   @w    @w   @w     @w @>"
#                            d0-d7    a0-a7    mac macsr mask hasmac sr pc

#
# Define the name register list.
#

set regNList {d0 d1 d2 d3 d4 d5 d6 d7 a0 a1 a2 a3 a4 a5 a6|fp a7|sp acc macsr mask hasmac sr pc}

# gophers that will get the PC value and the SP value

set pcGetGopher "<+$offset(WIND_TCB,pc) @>"
set spGetGopher "<+$offset(WIND_TCB,sp) @>"

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
#	ctype: context type
#	cid: context id
#
# RETURNS: N/A
#

proc regSetOfContext {ctype cid} {
    set regBlk [wtxRegsGet $ctype $cid REG_SET_IU 0 80]
    set rset [memBlockGet -l $regBlk 0 16]	;# d0-d7/a0-a7
    append rset [memBlockGet -l $regBlk 64 1]	;# mac
    append rset [memBlockGet -w $regBlk 68 1]	;# macsr
    append rset [memBlockGet -w $regBlk 70 1]	;# mask
    append rset [memBlockGet -w $regBlk 72 1]	;# hasmac
    append rset [memBlockGet -w $regBlk 74 1]	;# sr
    append rset [memBlockGet -l $regBlk 76 1]	;# pc
    memBlockDelete $regBlk
    return $rset
}

##############################################################################
# regOffsetFromIndexGet - return register offset from its index in the regset
#
# To write the register we need to know its offset in the regset. Indeed in
# some cases the offset is not always (index * 4) because of padding.
#
# SYNOPSIS:
#	regAddrGet index
#
# PARAMETERS:
#	index: index in the register set of the concerned register
#
# RETURN: the offset of the register
#
proc regOffsetFromIndexGet {regInd} {
    
    if {$regInd <= 17} {
	return [expr $regInd * 4]
    } else {
        return [expr ($regInd - 17) * 2 + 68]
    }
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
    puts stdout "  d0 d1 d2 d3 d4 d5 d6    d7"
    puts stdout "  a0 a1 a2 a3 a4 a5 a6|fp a7|sp"
    puts stdout " acc macsr mask (if present)"
    puts stdout "  sr pc"
}

##############################################################################
#
# pcOfRegSet - get pc register
#
# SYNOPSIS: pcOfRegSet [regSet]
#
# PARAMETERS:
#	regSet: set of register values
#
# RETURNS: content of pc
#

proc pcOfRegSet {regSet} {
    return [lindex $regSet 21]
}

##############################################################################
#
# regSetPrint - prints a register set
#
# SYNOPSIS: regSetPrint [regSet]
#
# PARAMETERS:
# 	regSet: set of register values
#
# RETURNS: N/A
#

proc regSetPrint {regSet} {
    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
	"d0" [lindex $regSet 0] "d1" [lindex $regSet 1] \
	"d2" [lindex $regSet 2] "d3" [lindex $regSet 3]]
    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
	"d4" [lindex $regSet 4] "d5" [lindex $regSet 5] \
	"d6" [lindex $regSet 6] "d7" [lindex $regSet 7]]
    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
	"a0" [lindex $regSet 8] "a1" [lindex $regSet 9] \
	"a2" [lindex $regSet 10] "a3" [lindex $regSet 11]]
    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
	"a4" [lindex $regSet 12] "a5" [lindex $regSet 13] \
	"a6/fp" [lindex $regSet 14] "a7/sp" [lindex $regSet 15]]
# Only print MAC registers if present.
    if {[lindex $regSet 19] != 0} {
	puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x" \
	    "acc" [lindex $regSet 16] "macsr" [lindex $regSet 17] \
	    "mask" [lindex $regSet 18]]
    }
    puts stdout [format "%-6s = %8x   %-6s = %8x" \
	"sr" [lindex $regSet 20] "pc" [lindex $regSet 21]]
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
    return "{\"Coldfire Registers\" {{Data {d0 d1 d2 d3 d4 d5 d6 d7}}
        {Address {a0 a1 a2 a3 a4 a5 a6 a7}}
        {Other {pc sr acc macsr mask}}}}"
}

##############################################################################
#
# taskRegSetVec -
# 
# SYNOPSIS: taskRegSetVec [taskInfoList]
#
# PARAMETERS:
#	taskInfoList:
#
# RETURNS:
#

proc taskRegSetVec {taskInfoList} {
    set tiLen [llength $taskInfoList]
    set regRawVec [lrange $taskInfoList [expr $tiLen - 22] end]
    if {[lindex $regRawVec 19] == 0} {
	return "{ \
                [list [lrange $regRawVec 0 7]] \
                [list [lrange $regRawVec 8 15]] \
                { [lindex $regRawVec 21] [lindex $regRawVec 20] } }"
    } else {
        return "{ \
                [list [lrange $regRawVec 0 7]] \
                [list [lrange $regRawVec 8 15]] \
                { [lindex $regRawVec 21] [lindex $regRawVec 20] \
		  [lindex $regRawVec 16] [lindex $regRawVec 17] [lindex $regRawVec 18] } }"
    }
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

#
# Table of exception names, keyed by exception number (as list index).
#

set coldfireExcName {
    ""
    ""
    "Access Fault/Bus Error"
    "Address Error"
    "Illegal Instruction"
    "Zero Divide"
    "CHK Trap"
    "TRAPV Trap"
    "Privilege Violation"
    "Trace Exception"
    "Unimplemented Opcode 1010"
    "Unimplemented Opcode 1111"
    "Emulator Interrupt"
    "Coprocessor Protocol Violation"
    "Format Error"
    "Uninitialized Interrupt"
    ""  ""  ""  ""
    ""  ""  ""  "" 
    "Spurious Interrupt"
    ""  ""  ""  ""
    ""  ""  "" 
    ""  ""  ""  ""
    ""  ""  ""  "" 
    ""  ""  ""  "" 
    ""  ""  ""  "" 
    "FP Branch or Set on Unordered"
    "FP Inexact Result"
    "FP Divide by Zero"
    "FP Underflow"
    "FP Operand Error"
    "FP Overflow"
    "FP Signaling NAN"
    "FP Unimplemented Data Type"
    "MMU Configuration Error"
    "" ""
    "Unimplemented Effective Address"
    "Unimplemented Integer Instruction"
}

#
# excEsfFormatTable
#
# This is a list of lists.  Each element of the list has the following 
# three elements:
#   ESF-type gopher member-names
#
# The first element, the ESF-type, is used as the key to the list.  
# The excInfoShow routine selects one of these based on the vector
# number and the fixed fields of the ESF.
#
# The gopher string, written assuming the address of the ESF is initialized
# in the gopher pointer, pulls out several valid fields of the ESF.
#
# The member-names structure describe the data that are pulled from the ESF.
#

set excEsfFormatTable {
  # ESFCOLD: Any exception on MCF5200
    {ESFCOLD		{@w@w@}			{fmt sr pc}}
}

# 
# excEsfMemberName - maps the short ESF member name codes of the
# excEsfFormatTable to a nice name for printing.  Intentionally, there
# are fewer entries in this table than there are member codes in the
# Format Table; this table is meant to contain those ESF members that
# are "interesting."  The absence of an entry in this table corresponding
# to an element of the Format Table indicates that the member should
# not be printed.
#

set excEsfMemberName {
    {fn		"Function Code       "  }
    {aa		"Access Address      "	}
    {pc		"Program Counter     "	}
    {ir		"Instruction Register"	}
    {sr		"Status Register     "	}
    {ea		"Effective Address   "	}
    {fmt	"Format              "	}
}

#
# Add a routine that will parse the exception stack frame.
#

proc excInfoShow {vec esf} {
    global cpuType
    global __wtxCpuType
    global coldfireExcName
    global excEsfFormatTable
    global excEsfMemberName
    
    set cpu $cpuType($__wtxCpuType)
    set vec [expr $vec]

    set excFormat ESFCOLD

    # Look up the descriptor for the format.

    set ename [lindex $coldfireExcName $vec]
    if {$ename == ""} {set ename "Unknown Exception ($vec)"}
    puts stdout "\n$ename"

    set success 0 

    foreach esfFormat $excEsfFormatTable {
	if {[lindex $esfFormat 0] == $excFormat} {

	    incr success
	    # found a match.  Gopher out the members.

	    set info [wtxGopherEval "$esf [lindex $esfFormat 1]"]

	    # for each member, print it out.

	    foreach member [lindex $esfFormat 2] {
		
		# see if the member is in the list of members we want
		# to print.

		foreach name $excEsfMemberName {
		    if {$member == [lindex $name 0]} {
			puts stdout [format "%s: %#x" \
				[lindex $name 1] [lindex $info 0]]
		    }
		}

		set info [lrange $info 1 end]
	    }
	}
    }

    if {$success == 0} {
	puts stdout "Sorry, I don't know how to print that kind of exception."
    }
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

