# sh-mips.tcl - MIPS-specific constants for host shell Tcl implementation
#
# Copyright 1994-2002 Wind River Systems, Inc. */
#
# modification history
# -----------------------
# 02a,28jun02,tpw  Fix SPR 78782, GDB crash on MIPS64.
# 01z,13nov01,aeg  added offset(WIND_TCB,events), offset(SEMPAHORE,events), &
#                  offset(MSG_Q,events)
# 01y,02nov01,tam  undid SM_PARTITION offsets changes
# 01x,29oct01,tam  fixed FREE_BLOCK offsets. Updated PARTITION offsets
# 01w,19oct01,fmk  change offset for FD_ENTRY,inuse and sizeof FD_ENTRY
# 01v,12oct01,tcr  Correct RBUFF_TYPE offsets for change in size of semaphore
# 01u,02oct01,fmk  add offset for taskTicks
# 01t,19sep01,aeg  updated WIND_TCB offsets to match taskLib.h v04j changes.
# 01s,26jun01,tpw  Make IU register sizes depend on MIPS32/64.
# 01r,20apr01,roz  Fix MIPS64 FP register offsets.
# 01q,05jan01,mem  Initial changes for MIPS32/MIPS64
# 01p,10sep99,myz  merged CW4000 ES FCS3
# 01o,08may97,mem  merged VR5000 support with R4000, added VR4100.
# 01n,17arp97,mem  changed addrMsg in excShowInfo to use \t rather than
# 01q,30jun99,dra  Added VR5464 support for multimedia registers.
#                  Added offset declaration for pFpContext.
#		   Fixed regSetOfContext for 64-bit targets.
# 01p,20jan99,dra  Added CW4000, CW4011, VR4100, VR5000 and VR5400 support.
# 		   Changed addrMsg in excShowInfo to use \t rather than
#		   embedded tabs.
# 01m,03feb97,sru  added CW4000 & CW4010 support.
# 01l,30jan97,sub  added VR5000 support.
# 01o,05mar99,jmp  added DIST_OBJ_NODE offsets (SPR# 25458).
# 01n,01jul98,c_s  WindView 2 -> Tornado 2 Merge
# 01m,06may98,nps  inserted new rBuff field.
# 01l,02apr98,pr   Added rBuff and event triggering offsets.
# 01n,05jun98,f_l  added offsets for taskWait routines
# 01m,17apr98,rlp  removed numMaxMsgsSent for backward compatibility.
# 01l,05nov97,rlp  added numMaxMsgsSent for tracking messages sent.
# 01k,19dec96,tam  added Integer Overflow case to excInfoShow().
# 01j,17dec96,tam  incremented variable off by 4 in MipsSetup() (spr #7632).
# 01i,19nov96,kkk  added R4650 support.
# 01h,29oct96,pad  Added routine underscoreStrip (SPR #7159).
# 01g,14oct96,pad  Removed the leading underscore for determining C++ mangled
#                  names (SPR #7326)
# 01f,30sep96,pad  Added mangledSymListGet procedure
# 01e,15sep96,kkk  fixed offset(FREE_BLOCK,node) to be 0x10 (spr# 7037).
# 01d,13aug96,elp  adapted regSetOfContext{} and regOffsetFromIndexget{} to
#		   R4000 + added reg64Write{}, reg64VListGet{}.
# 01c,01jul96,kkk  changed $0 to zero. Cleanup.
# 01b,21jun96,elp  added pcGetGopher, spGetGopher, regSetOfContext{},
#		   availRegPuts{}, regOffsetFromIndexGet{}
#		   and source dbgMipsLib.tcl.
# 01a,09jan96,mem  written; derived from sh-sparc.tcl
#
# RESOURCE FILES
# wpwr/host/resource/tcl/dbgMipsLib.tcl
#

source "[wtxPath host resource tcl]dbgMipsLib.tcl"

set offset(WIND_TCB,timeout)            0x018
set offset(WIND_TCB,activeNode)         0x020
set offset(WIND_TCB,objCore)            0x030
set offset(WIND_TCB,name)               0x034
set offset(WIND_TCB,options)            0x038
set offset(WIND_TCB,status)             0x03c
set offset(WIND_TCB,priority)           0x040
set offset(WIND_TCB,priNormal)          0x044
set offset(WIND_TCB,pPendQ)		0x05c
set offset(WIND_TCB,safeCnt)		0x060
set offset(WIND_TCB,entry)              0x074
set offset(WIND_TCB,pStackBase)         0x078
set offset(WIND_TCB,pStackLimit)        0x07c
set offset(WIND_TCB,pStackEnd)          0x080
set offset(WIND_TCB,errorStatus)        0x084
set offset(WIND_TCB,taskTicks)		0x094
set offset(WIND_TCB,events)		0x0dc
set offset(WIND_TCB,pFpContext)         0x0a4
set offset(WIND_TCB,excInfo)		0x108 
set offset(WIND_TCB,regs)               0x128
set offset(WIND_TCB,pc)                 0x12c
# 64-bit register starting at 0x228
set offset(WIND_TCB,sp)                 0x22c

set offset(OBJ_CLASS,objPartId)		0x004

set offset(SEMAPHORE,semType)		0x004
set offset(SEMAPHORE,options)		0x005
set offset(SEMAPHORE,qHead)		0x008
set offset(SEMAPHORE,count)		0x018
set offset(SEMAPHORE,events)		0x01c

set offset(POSIX_SEM,semId)		0x004
set offset(POSIX_SEM,refCnt)		0x008
set offset(POSIX_SEM,sem_name)		0x00c

set offset(MSG_Q,msgQ)			0x004
set offset(MSG_Q,freeQ)			0x024
set offset(MSG_Q,options)		0x044
set offset(MSG_Q,maxMsgs)		0x048
set offset(MSG_Q,maxMsgLength)		0x04c
set offset(MSG_Q,sendTimeouts)		0x050
set offset(MSG_Q,recvTimeouts)		0x054
set offset(MSG_Q,events)		0x058

set offset(POSIX_MSG_Q_DES,f_flags)	0x004
set offset(POSIX_MSG_Q_DES,f_data)	0x008

set offset(POSIX_MSG_Q,msgq_cond_read)  0x000
set offset(POSIX_MSG_Q,msgq_cond_data)  0x010
set offset(POSIX_MSG_Q,msgq_sym)	0x020
set offset(POSIX_MSG_Q,msgq_attr)	0x0e4

set offset(POSIX_MSG_Q_ATTR,mq_maxmsg)	0x000
set offset(POSIX_MSG_Q_ATTR,mq_msgsize)	0x004
set offset(POSIX_MSG_Q_ATTR,mq_flags)	0x008
set offset(POSIX_MSG_Q_ATTR,mq_curmsgs)	0x00c

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

set offset(SYMBOL,name)			0x004

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

set offset(TRIGGER,objCore)             0x000
set offset(TRIGGER,eventId)             0x004
set offset(TRIGGER,status)              0x006
set offset(TRIGGER,disable)             0x008
set offset(TRIGGER,contexType)          0x00c
set offset(TRIGGER,contextId)           0x010
set offset(TRIGGER,objId)               0x014
set offset(TRIGGER,chain)               0x018
set offset(TRIGGER,next)                0x01c
set offset(TRIGGER,conditional)         0x020
set offset(TRIGGER,condType)            0x024
set offset(TRIGGER,condEx1)             0x028
set offset(TRIGGER,condOp)              0x02c
set offset(TRIGGER,condEx2)             0x030
set offset(TRIGGER,actionType)          0x034
set offset(TRIGGER,actionFunc)          0x038
set offset(TRIGGER,actionArg)           0x03c
set offset(TRIGGER,actionDef)           0x040
set offset(TRIGGER,hitCnt)              0x044

set offset(DIST_OBJ_NODE,objNodeReside)	0x008
set offset(DIST_OBJ_NODE,objNodeId)	0x00c

#
# Define a gopher that will fetch the CPU registers from the TCB of a 
# given task.  The gopher pointer should point to the TCB upon evaluation
# of this string.
#
# MIPS is ambi-endian, so the gopher string depends on both the endianness
# and register size of the target we are connected to.  Luckily we are
# already connected to the target when this file is sourced and we can
# set the gopher string based on the endianness of the target.
#
# In order to make some of the rest of the code easier to write, the
# result of the gopher string splits the upper and lower parts of the
# 64-bit registers on the MIPS64.
#
# The following is common to both the MIPS32 and MIPS64 (all values are 32 bits):
#     sr pc cause fpscr divlo divhi gpregLo[0] - gpregLo[31]
#
# The MIPS64 appends the high 32 bits of the following regs:
#     divlo divhi gpregHi[0] - gpregHi[31]
#
# We also setup offset(WIND_TCB, sp) here as well, as it has the same
# dependencies.

#
# Code all of this as a proc so we can use local variables w/o cluttering
# up the tcl namespace.
#
proc MipsSetup {} {
    global gdbRegs
    global offset
    global regSetGopher
    global cpuType
    global __wtxCpuType

    # size of a register (for those that differ between MIPS32 and MIPS64)
    set regSize 4

    # common part of the gopher string
    # get sr and pc
    set regSetGopher "<+$offset(WIND_TCB,regs) @@"
    switch -regexp $cpuType($__wtxCpuType) {
	MIPS32 {
	    # get cause and fpscr, then get the rest of the regs.
	    set regSetGopher "$regSetGopher <+[expr 34 * 4] @@> (34 @)"
	    set offset(WIND_TCB,sp) [expr $offset(WIND_TCB,pc) +4+(2 + 29)*4]

	}
	MIPS64 {
	    # get cause and fpscr
	    set regSize 8
	    set regSetGopher "$regSetGopher <+[expr 34 * 8] @@>"
	    switch [targetEndian] {
		BIG_ENDIAN {
		    # get the low 32 bits of each register
		    set regSetGopher "$regSetGopher (34 +4 @)"
		    # get the high 32 bits of each register
		    set regSetGopher "$regSetGopher (34 @ +4)"
		    set offset(WIND_TCB,sp) [expr $offset(WIND_TCB,pc) + 4 \
			    + (2 + 29)*8 + 4]
		}
		LITTLE_ENDIAN {
		    # get the low 32 bits of each register
		    set regSetGopher "$regSetGopher (34 @ +4)"
		    # get the high 32 bits of each register
		    set regSetGopher "$regSetGopher (34 +4 @)"
		    set offset(WIND_TCB,sp) [expr $offset(WIND_TCB,pc) + 4 \
			    + (2 + 29)*8 + 0]
		}
	    }
	}	
    }
    # Close off the gopher string
    set regSetGopher "$regSetGopher >"

    # setup the gdbRegs info for MIPS

#    set gdbRegs(mips,numRegs) 80
#    set gdbRegs(mips,numRegs) 124
# GDB no longer recognizes multimedia regs! Don't ask it to use them. The 124
# value can crash GDB because our new defensive code isn't enabled on MIPS yet.
    set gdbRegs(mips,numRegs) 80

    # Setup misc registers
    set gdbRegs(mips,32) {0x0000 4 IU sr}
    set off 8
    set gdbRegs(mips,33) [list $off $regSize IU lo]
    incr off $regSize
    set gdbRegs(mips,34) [list $off $regSize IU hi]

    set gdbRegs(mips,35) [list -1 -1 IU bad]
    set gdbRegs(mips,36) [list [expr 8 + 34*$regSize] 4 IU cause]
    set gdbRegs(mips,37) [list 4 4 IU pc]

    set gdbRegs(mips,70) [list [expr 12 + 34*$regSize] 4 IU fsr]
    set gdbRegs(mips,71) [list -1 -1 IU fir]
    set gdbRegs(mips,72) [list -1 -1 IU fp]
    set gdbRegs(mips,73) [list -1 -1 IU inx]
    set gdbRegs(mips,74) [list -1 -1 IU rand]
    set gdbRegs(mips,75) [list -1 -1 IU tlblo]
    set gdbRegs(mips,76) [list -1 -1 IU ctxt]
    set gdbRegs(mips,77) [list -1 -1 IU tlbhi]
    set gdbRegs(mips,78) [list -1 -1 IU epc]
    set gdbRegs(mips,79) [list -1 -1 IU prid]

    set gdbRegs(mips,80) [list -1 -1 IU foo1]
    set gdbRegs(mips,81) [list -1 -1 IU foo2]
    set gdbRegs(mips,82) [list -1 -1 IU foo3]
    set gdbRegs(mips,83) [list -1 -1 IU foo4]
    set gdbRegs(mips,84) [list -1 -1 IU foo5]
    set gdbRegs(mips,85) [list -1 -1 IU foo6]
    set gdbRegs(mips,86) [list -1 -1 IU foo7]
    set gdbRegs(mips,87) [list -1 -1 IU foo8]
    set gdbRegs(mips,88) [list -1 -1 IU foo9]
    set gdbRegs(mips,89) [list -1 -1 IU foo10]

    # Setup integer registers
    set names { zero at v0 v1 a0 a1 a2 a3 t0 t1 t2 t3 t4 t5 t6 t7 \
	    s0 s1 s2 s3 s4 s5 s6 s7 t8 t9 k0 k1 gp sp s8 ra }
    set off [expr 8 + 2 * $regSize]
    for {set i 0} {$i < 32} {incr i} {
	set gdbRegs(mips,$i) [list $off $regSize IU [lindex $names $i]]
	incr off $regSize
    }

    # Setup fp registers
    set off 0
    for {set i 0} {$i < 32} {incr i} {
	set gdbRegs(mips,[expr $i+38]) [list $off $regSize FPU f$i]
	incr off $regSize
    }

    # Setup mm registers (even then odd)
    # First map mm0-mm30 to appropriate register numbers, evens only
    set off 0
    for {set i 0} {$i < 32} {incr i 2} {
	set gdbRegs(mips,[expr $i + 91]) [list $off 8 FPU mm$i]
	incr off 8
    }

    # Second, map mm1-mm31 to appropriate register numbers, odds only
    set off 136
    for {set i 1} {$i < 32} {incr i 2} {
	set gdbRegs(mips,[expr $i + 91]) [list $off 8 FPU mm$i]
	incr off 8
    }

    # Third, map accumulator to appropriate register number
    # Value for "off" falls through
    set gdbRegs(mips,123) [list $off 24 FPU acc]

    # Fourth, map FR_FLAG from FP_CONTEXT to make gdb "think" it's
    # a register.
    set off 132
    set gdbRegs(mips,90) [list $off 4 FPU fr_flag]

    set gdbRegs(mips,regSetSizes) \
	    [list [expr 8 + 34*$regSize + 8] [expr 32 * 8] [expr 32 * 8] 4 24 0 0 0 0]
#    set gdbRegs(mips,regSetSizes) {164 412 0 0 0 0} 
}

# Now do the mips-specific setup
MipsSetup

#
# register names list. Registers are considered as consecutive if not then we
# add register names called "_" that are skipped by mRegs
#

set regNList { %sr %pc cause fpscr %divlo %divhi \
	zero at v0 v1 a0 a1 a2 a3 \
	t0 t1 t2 t3 t4 t5 t6 t7 \
	s0 s1 s2 s3 s4 s5 s6 s7 \
	t8 t9 k0 k1 gp sp s8 ra }

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
    puts stdout " %sr %pc cause fpscr %divlo %divhi"
    puts stdout " zero at v0 v1 a0 a1 a2 a3"
    puts stdout " t0 t1 t2 t3 t4 t5 t6 t7"
    puts stdout " s0 s1 s2 s3 s4 s5 s6 s7"
    puts stdout " t8 t9 k0 k1 gp sp s8 ra"
}

#############################################################################
#
# regSetOfContext - get the system context registers
#
# A procedure to get the system context registers, a list of the content of the 
# following registers is returned
#
# SYNOPSIS: regSetOfContext [ctype cid]
#
# PARAMETERS:
#       ctype: context typE
#       cid: context id
#
# RETURNS: list of registers content. cause and fpscr values are set to 0 since
#	   they are not returned by wtxRegsGet{}.
#

proc regSetOfContext {ctype cid} {
    global cpuType
    global __wtxCpuType
    
    switch -regexp $cpuType($__wtxCpuType) {
	MIPS32 {
	    set regSetLength [expr 36 * 4]
	    set regBlk [wtxRegsGet $ctype $cid REG_SET_IU 0 $regSetLength]
	    set rset [memBlockGet -l $regBlk 0 4]
	    append rset " 0 0 "
	    append rset [memBlockGet -l $regBlk 16 32]
	    memBlockDelete $regBlk
	}
	MIPS64 {
	    set regSetLength [expr 34 * 8 + 4 * 4]
	    set regBlk [wtxRegsGet $ctype $cid REG_SET_IU 0 $regSetLength]
	    
	    # put sr, pc, add 0 for cause and fpscr
	    set rset [memBlockGet -l $regBlk 0 2]

	    append rset " 0 0 "
	    set regs [memBlockGet -l $regBlk 8 68]
	    switch [targetEndian] {
		BIG_ENDIAN {
		    for {set i 0} {$i < 34} {incr i} {
			append rset " "
			append rset [lindex $regs [expr 2*$i + 1]]
		    }
		    for {set i 0} {$i < 34} {incr i} {
			append rset " "
			append rset [lindex $regs [expr 2*$i]]
		    }
		}
		LITTLE_ENDIAN {
		    for {set i 0} {$i < 34} {incr i} {
			append rset " "
			append rset [lindex $regs [expr 2*$i]]
		    }
		    for {set i 0} {$i < 34} {incr i} {
			append rset " "
			append rset [lindex $regs [expr 2*$i + 1]]
		    }
		}
	    }
	    memBlockDelete $regBlk
	}
    }
    return $rset
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
    return [lindex $regSet 1]
}

##############################################################################
# regPrint64 - print a 64 bit register
#
# SYNOPSIS: regPrint64 [name hbits lbits]
#
# PARAMETERS:
#       name: reg name
#       hbits: high order bits.
#       lbits: low order bits.
#
# RETURNS:
#

proc regPrint64 {name hbits lbits} {
    if {$hbits != 0x0} then {
	puts -nonewline stdout [format "%-5s= %8x%08x   " \
		$name $hbits $lbits]
    } else {
	puts -nonewline stdout [format "%-5s=         %8x   " \
		$name $lbits]
    }
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
    global regNList
    global cpuType
    global __wtxCpuType

    switch -regexp $cpuType($__wtxCpuType) {
	MIPS32 {
	    for {set i 0} {$i < 32} {incr i} {
		if {$i && ![expr $i % 4]} {
		    puts stdout ""
		}
		set n [expr $i + 6]
		puts -nonewline stdout [format "%-4s = %8x   " \
			[lindex $regNList $n] [lindex $regSet $n]]
	    }
	    puts stdout ""
	    puts -nonewline stdout [format "%-4s = %8x   %-4s = %8x" \
		    "sr"  [lindex $regSet 0]  "pc"  [lindex $regSet 1]]
	    puts stdout [format "   %-5s= %8x   %-5s= %8x" \
		    "divlo"  [lindex $regSet 4]  "divhi" [lindex $regSet 5]]
	}
	MIPS64 {
	    for {set i 0} {$i < 32} {incr i} {
		if {$i && ![expr $i % 3]} {
		    puts stdout ""
		}
		set n [expr $i + 6]
		regPrint64 [lindex $regNList $n] \
			[lindex $regSet [expr $n + 34]] [lindex $regSet $n]
	    }
	    puts stdout ""
	    regPrint64 divlo [lindex $regSet 38] [lindex $regSet 4]
	    regPrint64 divhi [lindex $regSet 39] [lindex $regSet 5]
	    puts stdout ""
	    puts stdout [format "%-5s=         %8x   %-5s=         %8x" \
		    "sr"  [lindex $regSet 0]  "pc"  [lindex $regSet 1]]
	}
    }
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
    return "{\"MIPS Registers\" 
	{
	{Control { sr pc cause fpscr divlo divhi } } \
	{General { zero at v0 v1 a0 a1 a2 a3 \
		t0 t1 t2 t3 t4 t5 t6 t7 \
		s0 s1 s2 s3 s4 s5 s6 s7 \
		t8 t9 k0 k1 gp sp s8 ra }}
	}}"
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
    global cpuType
    global __wtxCpuType

    set tiLen [llength $taskInfoList]
    switch -regexp $cpuType($__wtxCpuType) {
	MIPS32 {
	    set regRawVec [lrange $taskInfoList [expr $tiLen - 38] end]
	}
	MIPS64 {
	    set regRawVec [lrange $taskInfoList [expr $tiLen - 72] end]
	}
    }
    return "{ \
		[list [lrange $regRawVec  0 5]]
		[list [lrange $regRawVec  6 end]] }"
}

#
# Define a gopher that will check the stack for the end of the 0xee sentinel
# area.  This is dependent on the stack sense of the architecture.
#

set stackGrows down

#
# Add a routine that will print exception stack frames.
#

##############################################################################
#
# excInfoShow - show exception info
#

proc excInfoShow {vector esf} {
    global cpuType
    global __wtxCpuType

    set esfInfo [wtxGopherEval "$esf (12 @)"]
    bindNamesToList {param1 param2 param3 param4 errno cause \
	    cntxt fpcsr badva pad sr pc } $esfInfo

    set addrMsg ""

    switch [expr $vector] {
	1 {
	    set excMsg "TLB mod"
	    set addrMsg [format "address:\t\t0x%08x" $badva]
	}
	2 {
	    set excMsg "TLB load"
	    set addrMsg [format "address:\t\t0x%08x" $badva]
	}
	3 {
	    set excMsg "TLB store"
	    set addrMsg [format "address:\t\t0x%08x" $badva]
	}
	4 {
	    set excMsg "Addr load"
	    set addrMsg [format "address:\t\t0x%08x" $badva]
	}
	5 {
	    set excMsg "Addr store"
	    set addrMsg [format "address:\t\t0x%08x" $badva]
	}
	6 {
	    if {$cpuType($__wtxCpuType) == "CW4011"} {
		set excMsg "Bus Error"
		# Bus error address is not available
	    } else {
		set excMsg "Insn Bus Error"
		set addrMsg [format "address:\t\t0x%08x" $badva]
	    }
	}
	7 {
	    if {$cpuType($__wtxCpuType) == "CW4011"} {
		set excMsg "Unknown Exception: $sig"
	    } else {
		set excMsg "Data Bus Error"
		set addrMsg [format "address:\t\t0x%08x" $badva]
	    }
	}
	8  { set excMsg "System call"}
	9  {
	    set insnInfo [wtxGopherEval "$pc @"]
	    bindNamesToList { insn } $insnInfo
	    if {$insn == 0x0007000d} {
		set excMsg "Integer divide by zero"
	    } else {
		if {$insn == 0x0006000d} {
		    set excMsg "Integer Overflow"
		} else {
		    set excMsg "Break point"
		}
	    }
	}
	10 { set excMsg "Reserved Insn"}
	11 { set excMsg "Coprocessor unusable"}
	12 { set excMsg "Overflow"}
	13 { set excMsg "Trap"}
	14 { set excMsg "Virt Coherency Inst"}
	15 { set excMsg "Floating Point Exception"}

	23 {set excMsg "Trap"}

	31 {set excMsg "Virt Coherency Data"}

	32 {set excMsg "SW Trap 0"}
	33 {set excMsg "SW Trap 1"}

	54 {
	    set excMsg "FPA unimplemented op"
	    set addrMsg [format "fpcsr:\t\t0x%08x" $fpcsr]
	}

	55 {
	    set excMsg "FPA invalid op"
	    set addrMsg [format "fpcsr:\t\t0x%08x" $fpcsr]
	}
	56 {
	    set excMsg "FPA div by zero"
	    set addrMsg [format "fpcsr:\t\t0x%08x" $fpcsr]
	}
	57 {
	    set excMsg "FPA overflow op"
	    set addrMsg [format "fpcsr:\t\t0x%08x" $fpcsr]
	}
	58 {
	    set excMsg "FPA underflow op"
	    set addrMsg [format "fpcsr:\t\t0x%08x" $fpcsr]
	}
	59 {
	    set excMsg "FPA inexact op"
	    set addrMsg [format "fpcsr:\t\t0x%08x" $fpcsr]
	}
	60 {
	    set excMsg "Bus Error"
	}
	*  {
	    set excMsg "Unknown Exception: $sig"
	}
    }

    puts stdout "\n$excMsg"
    puts stdout [format "program counter:\t0x%08x" $pc]
    puts stdout [format "processor status:\t0x%08x" $sr]
    puts stdout [format "cause register:\t\t0x%08x" $cause]
    puts stdout "$addrMsg"
}

###############################################################################
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
    global cpuType
    global __wtxCpuType

    switch -regexp $cpuType($__wtxCpuType) {
	MIPS32 {
	    return [expr $regInd * 4 - (($regInd > 5) ? 8 : 0)]
	}
    	MIPS64 {
	    if {$regInd <= 2} {
		return [expr $regInd * 4]
	    } else {
		return [expr 8 + (($regInd - 4) * 8)]
	    }
	}
    }
}

##############################################################################
#
# reg64VListGet - get register value list
#
# This routine concatenates the 32 bit values to get the 64 bit values that can
# be associated with the register name.
#
# SYNOPSYS:
#	reg64VListGet tid
#
# PARAMETERS:
#	tid: task identifier
#
# RETURNS:
#	The values of the register in the same order as in regNList.
#
# SEE ALSO:
# 	mRegs
#

proc reg64VListGet {tid} {

    # we concatenate 32 bit values to display a 64 bit value if needed

    set regSetLength [expr 36 * 8]
    set regBlk [wtxRegsGet CONTEXT_TASK $tid REG_SET_IU 0 $regSetLength]

    # put sr and pc in the list
    set vList [format "0x%-8x " [memBlockGet -l $regBlk 0 1]]
    append vList [format "0x%-8x " [memBlockGet -l $regBlk 4 1]]

    # add cause and fpscr
    append vList "0 0 "

    # other registers are 64 bit values
    for {set i 8} {$i < ($regSetLength - 8)} {incr i 8} {
	bindNamesToList {hi lo} [memBlockGet -l $regBlk $i 2]
 	append vList [format "0x%08x%08x " $hi $lo] 
    }
    memBlockDelete $regBlk
    return $vList
}

###############################################################################
#
# reg64Write - set value (that can be 64 bit long) into a register
#
# SYNOPSIS:
#	reg64Write ind hi lo
#
# PARAMETERS:
#	tid: task whose register is written
#	ind: index of the register in the register name
#	hi: high word
#	lo: low word
#
# RETURNS: N/A
#

proc reg64Write {tid ind hi lo} {

    # extend sign
    
    if {$hi == 0} {
	set hi [expr ($lo & 0x80000000) ? 0xffffffff : 0]
    }
    set endianFlag "-[string range [targetEndian] 0 0]"
    set blk [memBlockCreate $endianFlag]
    memBlockSet -l $blk 0 $hi
    memBlockSet -l $blk 4 $lo
    set offset [regOffsetFromIndexGet $ind]
    wtxRegsSet CONTEXT_TASK $tid REG_SET_IU $offset 8 $blk
    memBlockDelete $blk
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

    # For the Mips architecture, the compiler does not produce any
    # additional leading underscore, so we simply return the symbol name.

    return $symbol
}

