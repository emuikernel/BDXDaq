# sh-sh.tcl - SH-specific material for host shell Tcl implementation
#
# Copyright 1996-2002 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 02n,21feb02,h_k  fixed showing a0g and a1 registers' value and chenged the
#                  DSP registers order (SPR #73524).
# 02m,13nov01,aeg  added offset(WIND_TCB,events), offset(SEMPAHORE,events), &
#                  offset(MSG_Q,events); updated offset(WIND_TCB,pDspContext)
# 02l,02nov01,tam  undid SM_PARTITION offsets changes
# 02k,02nov01,tam  updated PARTITION and SM_PARTITION offsets
# 02j,19oct01,fmk  change offset for FD_ENTRY,inuse and sizeof FD_ENTRY
# 02i,12oct01,tcr  Correct RBUFF_TYPE offsets for change in size of semaphore
# 02h,02oct01,fmk  add offset for taskTicks
# 02g,20sep01,aeg  updated WIND_TCB offsets to match taskLib.h v04j changes.
# 02f,08nov00,zl   added shellShSetup.
# 02e,20oct00,rsh  adding dsp support to gdb for sh2 and sh3
# 02d,28sep00,zl   added gdbShSetup.
# 02c,25sep00,zl   rewrote DSP/FPU register support.
# 02b,20jul00,csi  Modification for Extended FP regs in host tool.
# 02a,09jun00,csi  Modification for vxfusion.
# 01z,18may00,rsh  fix taskRegSetStruct() and taskRegSetVec() to properly return cpu
#                  and fpp or dsp regs info for display in browser, consolidate 
#                  regSetGopher, remove fpp and dsp regs from regNList (mRegs() not 
#                  intelligent enough to use runtime info so we have to return only 
#                  cpu registers, remove fpp and dsp regs from availRegPuts() to comply
#                  with regNList change.
# 01y,14apr00,rsh  offset info for pPendQ and pFifoQ, also, RBUFF stuff for windview.
# 01x,04nov99,zl   display FP registers if task has FP context (breaks DSP)
# 01w,20jun99,zl   added SH7750 specific exception messages
# 01v,05mar99,st   added sh7718_ExcName (support SH7718 FPU). (merged by jmb,
#                  patch 01r,26nov97 from tor1_0_x.hitachiSH)
# 01u,22nov98,kab  fixed inclusion of dbgShLib.tcl by Browser.tcl.
# 01t,31aug98,kab  added mod reg for DSP.
# 01s,03aug98,kab  added DSP support.
# 01r,25nov97,hk   changed to use sequential interrupt vector number for SH7700.
# 01q,26jun97,hk   added offset(WIND_TCB,excInfo).
# 01p,30apr97,hk   added pcGetGopher, spGetGopher, regOffsetFromIndexGet,
#                  availRegPuts, mangledSymListGet, underscoreStrip.
#                  source dbgShLib.tcl. deleted needless global defs.
# 01o,17feb97,hk   changed excInfoShow.
# 01n,14feb97,hk   changed regSetPrint for ti.
# 01m,14feb97,hk   fixed offset(FREE_BLOCK,node) to 0x008 for memShow.
# 01l,10feb97,hk   updated sh3ExcName{}.
# 01k,05jan97,hk   added 8 to regs/pc/sp offsets in WIND_TCB.
# 01j,26sep96,wt   deleted the processes of bank registers.
#		   categorized SH registers.
#		   fixed excInfoShow.
# 01i,13sep96,wt   changed task register set structure.
# 01h,28aug96,wt   added support for SH family.
# 01a,02jul96,ja   written based on sh-i960.tcl 01g.
#

# DESCRIPTION
#
# This modules holds architecture dependant routines and globals
#
# RESOURCE FILES
# wpwr/host/resource/tcl/dbgShLib.tcl
#

#
# Indirect test that we've been sourced by Browser.tcl
# Browser needs nothing from dbg<Arch>Lib.tcl, and has not sourced
# shell.tcl, so it breaks on implementation of shellproc bha/bhb.
#
if {[info procs shellproc] != ""} {
    source "[wtxPath host resource tcl]dbgShLib.tcl"
}

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
set offset(WIND_TCB,pFpContext)		0x0a4
set offset(WIND_TCB,pDspContext)	0x178
set offset(WIND_TCB,excInfo)		0x108
set offset(WIND_TCB,regs)		0x11c
set offset(WIND_TCB,pc)			0x170
set offset(WIND_TCB,sp)			0x16c

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

set offset(DIST_OBJ_NODE,objNodeReside) 0x008
set offset(DIST_OBJ_NODE,objNodeId)     0x00c

set offset(WDOG,timeout)                0x00c	;# yet not checked
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

set offset(DEV_HDR,drvNum)		0x008	;# previously appeared

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

#
# Define a gopher that will fetch the CPU registers from the TCB of a 
# given task.  The gopher pointer should point to the TCB upon evaluation
# of this string.
#

# CPU Registers                vbr gbr pr  r0-r7  mach macl  r8-r15  pc sr
# Floating Point Registsers    fpul fpscr     fr0-fr15       xf0-xf15
# DSP Registers                rs re  dsr a0-1 a0-1g m0-1 x0-1 y0-1 mod
# options value set at end to indicate which registers are valid

set regSetGopher "
    <+$offset(WIND_TCB,regs)    @   @  @  @@@@@@@@ @    @   @@@@@@@@ @  @>
    <+$offset(WIND_TCB,pFpContext)
                           *   {@    @    @@@@@@@@@@@@@@@@ @@@@@@@@@@@@@@@@ 0}>
    <+$offset(WIND_TCB,pDspContext)
                           *   {@  @   @   @@    @@   @@   @@   @@   @   0}>
    <+$offset(WIND_TCB,pFpContext) * !>
    <+$offset(WIND_TCB,pDspContext) * !>"

#
# Define the name register list.
#

set regNList {vbr gbr pr r0 r1 r2 r3 r4 r5 r6 r7 mach macl \
    r8 r9 r10 r11 r12 r13 r14|fp r15|sp pc sr} 

# gophers that will get the PC value and the SP value

set pcGetGopher "<+$offset(WIND_TCB,pc) @>"
set spGetGopher "<+$offset(WIND_TCB,sp) @>"




#############################################################################
#
# gdbShSetup - initialize gdb for SuperH architecure
#
# SYNOPSIS: gdbShSetup
#
# RETURNS: N/A
#

proc gdbShSetup {} {
    global gdbRegs
    global cpuType
    global __wtxCpuType
    global __wtxTsInfo

    set gdbRegs(sh,numRegs)	71
    set gdbRegs(sh,regSetSizes)	{92 136 0 0 0 0 56}

    # data registers
    set gdbRegs(sh,0)  {0x0c 4 IU r0}
    set gdbRegs(sh,1)  {0x10 4 IU r1}
    set gdbRegs(sh,2)  {0x14 4 IU r2}
    set gdbRegs(sh,3)  {0x18 4 IU r3}
    set gdbRegs(sh,4)  {0x1c 4 IU r4}
    set gdbRegs(sh,5)  {0x20 4 IU r5}
    set gdbRegs(sh,6)  {0x24 4 IU r6}
    set gdbRegs(sh,7)  {0x28 4 IU r7}

    # address registers
    set gdbRegs(sh,8)  {0x34 4 IU r8}
    set gdbRegs(sh,9)  {0x38 4 IU r9}
    set gdbRegs(sh,10) {0x3c 4 IU r10}
    set gdbRegs(sh,11) {0x40 4 IU r11}
    set gdbRegs(sh,12) {0x44 4 IU r12}
    set gdbRegs(sh,13) {0x48 4 IU r13}
    set gdbRegs(sh,14) {0x4c 4 IU fp}
    set gdbRegs(sh,15) {0x50 4 IU sp}
    set gdbRegs(sh,16) {0x54 4 IU pc}
    set gdbRegs(sh,17) {0x08 4 IU pr}
    set gdbRegs(sh,18) {0x04 4 IU gbr}
    set gdbRegs(sh,19) {0x00 4 IU vbr}
    set gdbRegs(sh,20) {0x2c 4 IU mach}
    set gdbRegs(sh,21) {0x30 4 IU macl}
    set gdbRegs(sh,22) {0x58 4 IU sr}

    # floating point
    set gdbRegs(sh,23) {0x00 4 FPU fpul}
    set gdbRegs(sh,24) {0x04 4 FPU fpscr}
    set gdbRegs(sh,25) {0x08 4 FPU fr0}
    set gdbRegs(sh,26) {0x0c 4 FPU fr1}
    set gdbRegs(sh,27) {0x10 4 FPU fr2}
    set gdbRegs(sh,28) {0x14 4 FPU fr3}
    set gdbRegs(sh,29) {0x18 4 FPU fr4}
    set gdbRegs(sh,30) {0x1c 4 FPU fr5}
    set gdbRegs(sh,31) {0x20 4 FPU fr6}
    set gdbRegs(sh,32) {0x24 4 FPU fr7}
    set gdbRegs(sh,33) {0x28 4 FPU fr8}
    set gdbRegs(sh,34) {0x2c 4 FPU fr9}
    set gdbRegs(sh,35) {0x30 4 FPU fr10}
    set gdbRegs(sh,36) {0x34 4 FPU fr11}
    set gdbRegs(sh,37) {0x38 4 FPU fr12}
    set gdbRegs(sh,38) {0x3c 4 FPU fr13}
    set gdbRegs(sh,39) {0x40 4 FPU fr14}
    set gdbRegs(sh,40) {0x44 4 FPU fr15}

    # extended floating point
    set gdbRegs(sh,41) {0x48 4 FPU xf0}
    set gdbRegs(sh,42) {0x4c 4 FPU xf1}
    set gdbRegs(sh,43) {0x50 4 FPU xf2}
    set gdbRegs(sh,44) {0x54 4 FPU xf3}
    set gdbRegs(sh,45) {0x58 4 FPU xf4}
    set gdbRegs(sh,46) {0x5c 4 FPU xf5}
    set gdbRegs(sh,47) {0x60 4 FPU xf6}
    set gdbRegs(sh,48) {0x64 4 FPU xf7}
    set gdbRegs(sh,49) {0x68 4 FPU xf8}
    set gdbRegs(sh,50) {0x6c 4 FPU xf9}
    set gdbRegs(sh,51) {0x70 4 FPU xf10}
    set gdbRegs(sh,52) {0x74 4 FPU xf11}
    set gdbRegs(sh,53) {0x78 4 FPU xf12}
    set gdbRegs(sh,54) {0x7c 4 FPU xf13}
    set gdbRegs(sh,55) {0x80 4 FPU xf14}
    set gdbRegs(sh,56) {0x84 4 FPU xf15}

    # DSP control registers
    set gdbRegs(sh,57) {0x00 4 DSP rs}
    set gdbRegs(sh,58) {0x04 4 DSP re}
    set gdbRegs(sh,59) {0x08 4 DSP dsr}
    set gdbRegs(sh,60) {0x34 4 DSP mod}

    # DSP data registers
    set gdbRegs(sh,61) {0x0c 4 DSP a0g}
    set gdbRegs(sh,62) {0x10 4 DSP a0}
    set gdbRegs(sh,63) {0x14 4 DSP a1g}
    set gdbRegs(sh,64) {0x18 4 DSP a1}
    set gdbRegs(sh,65) {0x1c 4 DSP m0}
    set gdbRegs(sh,66) {0x20 4 DSP m1}
    set gdbRegs(sh,67) {0x24 4 DSP x0}
    set gdbRegs(sh,68) {0x28 4 DSP x1}
    set gdbRegs(sh,69) {0x2c 4 DSP y0}
    set gdbRegs(sh,70) {0x30 4 DSP y1}

    # turn off reg sets not in use for "this" arch
    
    set hasFpp [lindex [lindex $__wtxTsInfo 2] 1]
    
    switch -regexp $cpuType($__wtxCpuType) {
        SH7000 {
	    # sh
            set first 23
            set last 70
        }
	SH7600 {
	    # sh-dsp
            set first 23
            set last 56
        }
        SH7700 {
	    if { $hasFpp } {
		# sh3e
                set first 41
                set last 70
	    } else {
		# sh3-dsp
                set first 23
                set last 56
            }
	}
	SH7750 {
	    # sh4
            set first 57
            set last 70
	}
	default {
	    # sh
            set first 23
            set last 70
	}
    }

    for {set i $first} {$i <= $last} {incr i 1} {
        set gdbRegs(sh,$i) [lreplace $gdbRegs(sh,$i) 0 1 -1 -1]
    }
}

#############################################################################
#
# shellShSetup - initialize shell for SuperH architecure
#
# SYNOPSIS: shellShSetup
#
# RETURNS: N/A
#

proc shellShSetup {} {
    global gdbRegs
    global cpuType
    global __wtxCpuType
    global __wtxTsInfo
    global spTaskOptions
    global callTaskOptions
    global tcbOptionBit

    set hasFpp [lindex [lindex $__wtxTsInfo 2] 1]
    
    switch -regexp $cpuType($__wtxCpuType) {
        SH7000 {
	    # sh
        }
	SH7600 {
	    # sh-dsp

	    # include VX_DSP_TASK option
	    set spTaskOptions	[expr ($tcbOptionBit(VX_DSP_TASK) | \
				       $spTaskOptions)]
	    set callTaskOptions	[expr ($tcbOptionBit(VX_DSP_TASK) | \
				       $callTaskOptions)]
        }
        SH7700 {
	    if { $hasFpp } {
		# sh3e
	    } else {
		# sh3-dsp

		# include VX_DSP_TASK option
		set spTaskOptions   [expr ($tcbOptionBit(VX_DSP_TASK) | \
					   $spTaskOptions)]
		set callTaskOptions [expr ($tcbOptionBit(VX_DSP_TASK) | \
					   $callTaskOptions)]
            }
	}
	SH7750 {
	    # sh4
	}
	default {
	    # sh
	}
    }
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
#	ctype: context type
#	cid: context id
#
# RETURNS: N/A
#

proc regSetOfContext {ctype cid} {
    global cpuType
    global __wtxCpuType

    set regBlk [wtxRegsGet $ctype $cid REG_SET_IU 0 [expr 23*4]]
    set rset [memBlockGet -l $regBlk 0 23]
    memBlockDelete $regBlk

    if {$cpuType($__wtxCpuType) == "SH7750"} {
	set fpRsetSize [expr 34*4]
    } else {
	set fpRsetSize [expr 18*4]
    }

    # try getting FP registers
    if [catch {wtxRegsGet $ctype $cid REG_SET_FPU 0 $fpRsetSize} regBlk] {
	set pFpContext 0
    } else {
        set pFpContext 1
        set rset2 [memBlockGet -l $regBlk 0 34]
        memBlockDelete $regBlk

        set rset [concat $rset $rset2]
    }

    # try getting DSP registers
    if [catch {wtxRegsGet $ctype $cid REG_SET_DSP 0 [expr 14*4]} regBlk] {
	set pDspContext 0
    } else {
        set pDspContext 1
	set rset2 [memBlockGet -l $regBlk 0 14]
        memBlockDelete $regBlk

        set rset [concat $rset $rset2]
    }    

    set rset [concat $rset $pFpContext $pDspContext]
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
    puts stdout "  r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 r13 r14|fp r15|sp"
    puts stdout "  sr gbr vbr mach macl pr pc"
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
    global cpuType
    global __wtxCpuType

    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
	"r0" [lindex $regSet 3] "r1" [lindex $regSet 4] \
	"r2" [lindex $regSet 5] "r3" [lindex $regSet 6]]
    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
	"r4" [lindex $regSet 7] "r5" [lindex $regSet 8] \
	"r6" [lindex $regSet 9] "r7" [lindex $regSet 10]]
    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
	"r8" [lindex $regSet 13] "r9" [lindex $regSet 14] \
	"r10" [lindex $regSet 15] "r11" [lindex $regSet 16]]
    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
	"r12" [lindex $regSet 17] "r13" [lindex $regSet 18] \
	"r14" [lindex $regSet 19] "r15/sp" [lindex $regSet 20]]
    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
	"gbr" [lindex $regSet 1] "vbr" [lindex $regSet 0] \
	"mach" [lindex $regSet 11] "macl" [lindex $regSet 12]]
    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x" \
	"pr" [lindex $regSet 2] "sr" [lindex $regSet 22] \
	"pc" [lindex $regSet 21]]

    # Implementation sets two words after the end of the DSP/FP regSet to non-zero
    # if the pFp/DspContext is valid.
    set pDspContext [lindex $regSet [expr [llength $regSet] - 1]]
    set pFpContext  [lindex $regSet [expr [llength $regSet] - 2]]

    if {$pFpContext != 0} {
	puts ""
	puts stdout [format "%-6s = %8x   %-6s = %8x" \
		"fpul" [lindex $regSet 23] "fpscr" [lindex $regSet 24]]
	puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
		"fr0" [lindex $regSet 25] "fr1" [lindex $regSet 26] \
		"fr2" [lindex $regSet 27] "fr3" [lindex $regSet 28]]
	puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
		"fr4" [lindex $regSet 29] "fr5" [lindex $regSet 30] \
		"fr6" [lindex $regSet 31] "fr7" [lindex $regSet 32]]
	puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
		"fr8" [lindex $regSet 33] "fr9" [lindex $regSet 34] \
		"fr10" [lindex $regSet 35] "fr11" [lindex $regSet 36]]
	puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
		"fr12" [lindex $regSet 37] "fr13" [lindex $regSet 38] \
		"fr14" [lindex $regSet 39] "fr15" [lindex $regSet 40]]


	if {$cpuType($__wtxCpuType) == "SH7750"} {
	    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
		"xf0" [lindex $regSet 41] "xf1" [lindex $regSet 42] \
		"xf2" [lindex $regSet 43] "xf3" [lindex $regSet 44]]
	    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
		"xf4" [lindex $regSet 45] "xf5" [lindex $regSet 46] \
		"xf6" [lindex $regSet 47] "xf7" [lindex $regSet 48]]
	    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
		"xf8" [lindex $regSet 49] "xf9" [lindex $regSet 50] \
		"xf10" [lindex $regSet 51] "xf11" [lindex $regSet 52]]
	    puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
		"xf12" [lindex $regSet 53] "xf13" [lindex $regSet 54] \
		"xf14" [lindex $regSet 55] "xf15" [lindex $regSet 56]]
	}
    } elseif {$pDspContext != 0} {
	puts ""
	puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
		"rs" [lindex $regSet 23] "re" [lindex $regSet 24] \
		"dsr" [lindex $regSet 25] "mod" [lindex $regSet 36]]
	puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
		"a0g" [lindex $regSet 28] "a0" [lindex $regSet 26] \
		"a1g" [lindex $regSet 29] "a1" [lindex $regSet 27]]
	puts stdout [format "%-6s = %8x   %-6s = %8x   %-6s = %8x   %-6s = %8x" \
		"m0" [lindex $regSet 30] "m1" [lindex $regSet 31] \
		"x0" [lindex $regSet 32] "x1" [lindex $regSet 33]]
	puts stdout [format "%-6s = %8x   %-6s = %8x" \
		"y0" [lindex $regSet 34] "y1" [lindex $regSet 35]]
    }
}

##############################################################################
#
# taskRegSetStruct - return task registers for Browser
#
# This returns the register set to the Browser.  
#
# SYNOPSIS: taskRegSetStruct
#
# RETURNS:
#

proc taskRegSetStruct {} {
    global cpuType
    global __wtxCpuType

    if {$cpuType($__wtxCpuType) == "SH7750"} {
	# SH-4 has extra FP registers and no DSP registers
        return "{\"SH Registers\" \
                       {{General {r0 r1 r2 r3 r4 r5 r6 r7 \
                                  r8 r9 r10 r11 r12 r13 fp sp}} \
                        {System {mach macl pr pc}} \
                        {Control {sr gbr vbr}}}} \
        	{\"Floating Point Registers\" \
                       {{Control {fpul fpscr}} \
                        {Data {fr0 fr1 fr2 fr3 fr4 fr5 fr6 fr7 \
                               fr8 fr9 fr10 fr11 fr12 fr13 fr14 fr15 \
			       xf0 xf1 xf2 xf3 xf4 xf5 xf6 xf7 \
			       xf8 xf9 xf10 xf11 xf12 xf13 xf14 xf15}}}}"
    } else {
	# SH-2 and SH-3  could have either DSP or FP registers
        return "{\"SH Registers\" \
                       {{General {r0 r1 r2 r3 r4 r5 r6 r7 \
                                  r8 r9 r10 r11 r12 r13 fp sp}} \
                        {System {mach macl pr pc}} \
                        {Control {sr gbr vbr}}}} \
        	{\"Floating Point Registers\" \
                       {{Control {fpul fpscr}} \
                        {Data {fr0 fr1 fr2 fr3 fr4 fr5 fr6 fr7 \
                               fr8 fr9 fr10 fr11 fr12 fr13 fr14 fr15}}}} \
		{\"DSP Registers\" \
                       {{Control {rs re dsr mod}} \
                        {Data {a0g a0 a1g a1 m0 m1 x0 x1 y0 y1}}}}"
    }
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
    global cpuType
    global __wtxCpuType

    # Implementation sets two words after the end of the DSP/FP regSet to 
    # non-zero if the pFp/DspContext is valid.
    set pDspContext [lindex $taskInfoList [expr [llength $taskInfoList] - 1]]
    set pFpContext  [lindex $taskInfoList [expr [llength $taskInfoList] - 2]]

    if {$pDspContext != 0} {
	set regCount 37
    } elseif {$pFpContext != 0} {
	if {$cpuType($__wtxCpuType) == "SH7750"} {
	    set regCount 57
	} else {
	    set regCount 41
	}
    } else {
	set regCount 23
    }


    set tiLen [llength $taskInfoList]
    set regRawVec [lrange $taskInfoList [expr $tiLen - $regCount - 2] end]

    set list "{ \
        [list [concat [lrange $regRawVec 3 10] \
                      [lrange $regRawVec 13 20]]] \
        [list [concat [lrange $regRawVec 11 12] \
                      [lindex $regRawVec 2] \
                      [lindex $regRawVec 21]]] \
        [list [concat [lindex $regRawVec 22] \
                      [lindex $regRawVec 1] \
                      [lindex $regRawVec 0]]] \
        }" 

    if {$cpuType($__wtxCpuType) == "SH7750"} {
	# SH-4 has extra FP registers and no DSP registers
	if {$pFpContext != 0} {
	    lappend list " \
		[list [concat [lindex $regRawVec 23] \
			      [lindex $regRawVec 24]]] \
		[list [lrange $regRawVec 25 56]]"
	} else {
	    lappend list " \
		[list [concat "N/A" "N/A"]] \
		[list [concat "N/A" "N/A" "N/A" "N/A" "N/A" "N/A" "N/A" "N/A" \
	     		      "N/A" "N/A" "N/A" "N/A" "N/A" "N/A" "N/A" "N/A" \
			      "N/A" "N/A" "N/A" "N/A" "N/A" "N/A" "N/A" "N/A" \
			      "N/A" "N/A" "N/A" "N/A" "N/A" "N/A" "N/A" "N/A"]]"
	}
    } else {
	# SH-2 and SH-3  could have either DSP of FP registers
	if {$pFpContext != 0} {
	    lappend list " \
		[list [concat [lindex $regRawVec 23] \
			      [lindex $regRawVec 24]]] \
		[list [lrange $regRawVec 25 40]]"
	} else {
	    lappend list " \
		[list [concat "N/A" "N/A"]] \
		[list [concat "N/A" "N/A" "N/A" "N/A" "N/A" "N/A" "N/A" "N/A" \
			      "N/A" "N/A" "N/A" "N/A" "N/A" "N/A" "N/A" "N/A"]]"
	}

	if {$pDspContext != 0} {
	    lappend list " \
		[list [concat [lrange $regRawVec 23 25] \
			      [lindex $regRawVec 36]]] \
		[list [concat [lindex $regRawVec 28] \
			      [lindex $regRawVec 26] \
			      [lindex $regRawVec 29] \
			      [lindex $regRawVec 27] \
			      [lrange $regRawVec 30 35]]]"
	} else {
	    lappend list " \
		[list [concat "N/A" "N/A" "N/A" "N/A"]] \
		[list [concat "N/A" "N/A" "N/A" "N/A" "N/A" "N/A" "N/A" "N/A" \
			      "N/A" "N/A"]]" 
	}
    }
    return $list
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
# The table of exception names, keyed by exception number (as list index).
#

set sh7600_ExcName {
    ""
    ""
    ""
    ""
    "Reserved Instruction Code"
    ""
    "Illegal Slot Instruction"
    ""
    ""
    "CPU Address Error"
    "DMA Address Error"
    "Non Maskable Interrupt"
    "User Break Interrupt"
                   "" "" ""
    "" "" "" "" "" "" "" ""
    "" "" "" "" "" "" "" ""
    "" "" "" "" "" "" "" ""
    "" "" "" "" "" "" "" ""
    "" "" "" "" "" "" "" ""
    "" "" "" "" "" ""
    "Zero Divide"
    "(VxWorks Software Breakpoint)"
}

set sh7700_ExcName {
    "(VxWorks Software Breakpoint)"
    "Zero Divide"
    "TLB Miss/Invalid (Load)"
    "TLB Miss/Invalid (Store)"
    "Initial Page Write"
    "TLB Protection Violation (Load)"
    "TLB Protection Violation (Store)"
    "Address Error (Load)"
    "Address Error (Store)"
    "FPU Exception"
    "TLB Multiple Hit"
    "Unconditional Trap"
    "Reserved Instruction Code"
    "Illegal Slot Instruction"
    "Non Maskable Interrupt"
    "User Breakpoint Trap"
}

#
# Add a routine that will parse the exception stack frame.
#
proc excInfoShow {vec esf} {
    global cpuType
    global __wtxCpuType
    global sh7700_ExcName
    global sh7600_ExcName

    if {($cpuType($__wtxCpuType) == "SH7700") ||
	($cpuType($__wtxCpuType) == "SH7750")} {
	#
	# Gopher a sh7700 exception stack frame, then unpack it.
	# Format:      pc sr evt info
	#
	set excGopher "@  @   @   @"
	set excInfo [wtxGopherEval "$esf $excGopher"]
	bindNamesToList {pc sr evt info} $excInfo
	#
	# Print exception name 
	#
	set excName [lindex $sh7700_ExcName $vec]
	if {$excName == ""} {
	    set excName "Trap to uninitialized vector number $vec."
	}
	puts stdout "\n$excName"
	#
	# Print exception information
	#
	if {$evt == 0x160} {
	    puts stdout [format "TRA Register   : 0x%08x  (TRAPA #%d)" \
						$info [expr "$info >> 2"]]
	} else {
	    puts stdout [format "EXPEVT Register: 0x%08x" $evt]
	}
	puts stdout [format "Program Counter: 0x%08x" $pc]
	puts stdout [format "Status Register: 0x%08x" $sr]
	if {$evt >= 0x40 && $evt <= 0x100} {
	    puts stdout [format "Access  Address: 0x%08x" $info]
	}
	if {$evt == 0x120} {
	    puts stdout [format "FPSCR  Register: 0x%08x" $info]
	    # [FIX ME] FPU Exception
	    # print fpscr register information
	    # invalid operation or zero divide
	}
    } else {
	#
	# Gopher a sh7600 exception stack frame, then unpack it.
	# Format:      pc sr
	#
	set excGopher "@  @"
	set excInfo [wtxGopherEval "$esf $excGopher"]
	bindNamesToList {pc sr} $excInfo
	#
	# Print exception name 
	#
	set excName [lindex $sh7600_ExcName $vec]
	if {$excName == ""} {
	    set excName "Trap to uninitialized vector number $vec."
	}
	#
	# Print exception information
	#
	puts stdout "\n$excName"
	puts stdout [format "Program Counter: 0x%08x" $pc]
	puts stdout [format "Status Register: 0x%08x" $sr]
    }
    puts stdout [format ""]
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

