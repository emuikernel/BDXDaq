# dbgArmLib.tcl - ARM-specific Tcl implementation
#
# Copyright 1996-1997 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01d,27oct97,kkk  took out EOF line from end of file.
# 01c,10oct97,cdp  tidy.
# 01b,01may97,cdp  modified for alternative function prologues.
# 01a,28aug96,cdp  written.
#

# DESCRIPTION
# /target/src/arch/arm/trcLib.c inspired
# /target/src/arch/arm/dbgArchLib.c inspired

# globals

# some ARM instruction decoding stuff

set IMASK_BL			0x0f000000	; # BL xxx
set IOP_BL			0x0b000000

set IMASK_STMDB_SPP_FP_IP_LR_PC 0xfffff800	; # STMDB sp!,{..fp,ip,lr,pc}
set IOP_STMDB_SPP_FP_IP_LR_PC   0xe92dd800

set IMASK_SUB_FP_IP_4		0xffffffff	; # SUB fp,ip,#4
set IOP_SUB_FP_IP_4		0xe24cb004

set IMASK_SUB_FP_IP_4PLUS	0xfffff000      ; # SUB fp,ip,#4+
set IOP_SUB_FP_IP_4PLUS		0xe24cb000

set IMASK_MOV_IP_SP		0xffffffff	; # MOV ip,sp
set IOP_MOV_IP_SP		0xe1a0c00d

set IMASK_MOVXX_LR_PC		0x0fffffff	; # MOVxx lr,pc
set IOP_MOVXX_LR_PC		0x01a0e00f

set IMASK_STMDB_SPP_AREGS	0xfffffff0      ; # STMDB sp!,{a1-a4}
set IOP_STMDB_SPP_AREGS		0xe92d0000

set IMASK_SUB_SP_SP		0xfffff000      ; # SUB sp,sp,#n
set IOP_SUB_SP_SP		0xe24dd000


##############################################################################
#
# trcStack - print a trace of function calls from the stack
#
# This routine provides the low-level stack trace function.  A higher-level
# symbolic stack trace, built on top of trcStack{}, is provided by tt{} in
# shell.tcl
#
# This routine prints a list of the nested routine calls that are on the
# stack, showing each routine with its parameters.
#
# The stack being traced should be quiescent.  The caller should avoid
# tracing its own stack.
#
# CAVEAT
# In order to do the trace, a number of assumptions are made.  In general,
# the trace will work for all C language routines and for assembly language
# routines that start with a stack frame creation sequence. Many VxWorks
# assembly language routines start with this sequence for exactly this
# reason. However, routines written in other languages, strange entries into
# routines, or tasks with corrupted stacks can confuse the trace.  Also, all
# parameters are assumed to be 32-bit quantities, therefore structures
# passed as parameters will be displayed as a number of long integers.
#
# SYNOPSYS:
# 	trcStack tid
#
# PARAMETERS:
# 	tid: task identifier
#
# ERRORS: N/A
#
# RETURNS: a list of trace lines
#	   each trace line is a list of 4 elements :
#		{addr callFunc argNb {argList}}
#

proc trcStack {tid} {
    global endianFlag
    global IMASK_MOV_IP_SP
    global IOP_MOV_IP_SP
    global IMASK_STMDB_SPP_FP_IP_LR_PC
    global IOP_STMDB_SPP_FP_IP_LR_PC
    global IMASK_SUB_FP_IP_4
    global IOP_SUB_FP_IP_4
    global IMASK_SUB_FP_IP_4PLUS
    global IOP_SUB_FP_IP_4PLUS
    global IMASK_STMDB_SPP_AREGS
    global IOP_STMDB_SPP_AREGS
    global IMASK_SUB_SP_SP
    global IOP_SUB_SP_SP

    # get the register set of the task
    # ***MUST*** do this even if we can't provide a back-trace because
    # it forces the target agent to copy the task's registers at the time
    # of the exception into the TCB. If this isn't done, 'ti' will show
    # the values of the registers at the time the task was suspended i.e.
    # the PC will point to somewhere in taskSuspend().

    set regSet [regSetOfContext CONTEXT_TASK $tid]

    # If the current routine doesn't have a stack frame, then we fake one
    # by putting the old one on the stack and making fp point to that;
    # we KNOW we don't have a stack frame in a few restricted but useful
    # cases:
    #  1) we are in the entry sequence of a routine which establishes the
    #     stack frame
    #  2) we are the start if a routine which doesn't create a stack frame.
    # Note that, if we are in a routine which does not create a stack
    # frame, we cannot readily distinguish between this and the body of a
    # routine that does create a stack frame so the results are not
    # reliable.
    #
    # So far, we have identified 3 types of function prologue (we could
    # not get a useful statement from Cygnus about others):
    #
    #  normal:
    #          mov     ip,sp
    #          stmdb   sp!,{v_regs,fp,ip,lr,pc}
    #          sub     fp,ip,#4
    #
    #  varargs:
    #          mov     ip,sp
    #          stmdb   sp!,{a_regs}
    #          stmdb   sp!,{v_regs,fp,ip,lr,pc}
    #          sub     fp,ip,#4+n*4
    #
    #  structure arg passed by value:
    #          mov     ip,sp
    #          sub     sp,sp,#n
    #          stmdb   sp!,{fp,ip,lr,pc}
    #          sub     fp,ip,#4+n
    #
    # Combining 'varargs' and 'structure arg passed by value' does not
    # seem to generate a different prologue.

    set pc [pcOfRegSet $regSet]

    # first instruction of 3/4 instruction entry sequence can be up to
    # 2/3 instructions before the current pc so read 7 instructions
    # starting 3 instructions before the current pc

    set blk [wtxMemRead [expr $pc - 12] 28]
    set in_entry 0

    for {set i 12} {$i >= 0} {incr i -4} {
	set inst0 [expr [memBlockGet -l $blk $i 1]]
	if {($inst0 & $IMASK_MOV_IP_SP) == $IOP_MOV_IP_SP} {
	    break
	}
    }

    if {$i >= 0} {
	# found mov ip,sp so examine next three instructions
	incr i 4
	bindNamesToList {inst1 inst2 inst3} [memBlockGet -l $blk $i 3]
	if {((($inst1 & $IMASK_STMDB_SPP_FP_IP_LR_PC) ==
				    $IOP_STMDB_SPP_FP_IP_LR_PC) &&
	    (($inst2 & $IMASK_SUB_FP_IP_4) == $IOP_SUB_FP_IP_4) &&
	    ($i > 4))
	    ||
	    ((($inst2 & $IMASK_STMDB_SPP_FP_IP_LR_PC) ==
					$IOP_STMDB_SPP_FP_IP_LR_PC) &&
	     (($inst3 & $IMASK_SUB_FP_IP_4PLUS) == $IOP_SUB_FP_IP_4PLUS) &&
	     ((($inst1 & $IMASK_STMDB_SPP_AREGS) == $IOP_STMDB_SPP_AREGS) ||
	      (($inst1 & $IMASK_SUB_SP_SP) == $IOP_SUB_SP_SP)))} {
	    set in_entry 1
	}
    }

    memBlockDelete $blk


    # if we're in the entry sequence or, failing that, at an address
    # that's in the symbol table, fake a stack frame

    set fp [lindex $regSet 11]

    if {$in_entry || ([lindex [wtxSymFind -value $pc] 1] == $pc)} {

	set sp [lindex $regSet 13]

	# save 4 words on top of stack
	set savedStack [wtxGopherEval "$sp -16 @@@@"]

	# create a stack frame and write it to target
	set blk [memBlockCreate $endianFlag 16]
	memBlockSet -l $blk 0 $fp $sp [lindex $regSet 14] $pc
	wtxMemWrite $blk [expr $sp - 16]
	memBlockDelete $blk

	# do the backtrace
	set fp [expr $sp -4]
	set trc [trcStackLvl $fp $pc]

	# restore the saved words
	set blk [memBlockCreate $endianFlag 16]
	memBlockSet -l $blk 0  [lindex $savedStack 0]
	memBlockSet -l $blk 4  [lindex $savedStack 1]
	memBlockSet -l $blk 8  [lindex $savedStack 2]
	memBlockSet -l $blk 12 [lindex $savedStack 3]
	wtxMemWrite $blk [expr $sp - 16]
	memBlockDelete $blk
    } else {
	set trc [trcStackLvl $fp $pc]
    }
    return $trc
}

##############################################################################
#
# trcStackLvl - stack trace routine
#
# Loop and get trace information for each level. Maximun number of level is 40
#
# SYNOPSYS:
#	trcStackLevel fp pc
#
# PARAMETERS:
#	fp: content of fp register
#	pc: content of pc register
#
# ERRORS: N/A
#	
# RETURNS: a list of trace lines
#	   each trace line is a list of 4 elements :
#		{addr callFunc argNb {argList}}
#

proc trcStackLvl {fp pc} {

    set trcStack {}
    if {$fp == 0} {
	return $trcStack	; # stack is untraceable
    }

    # get for each level values that should be read on the target
    # fp and retAddr

    for {set depth 0} {$depth < 40} \
	{incr depth; set fp $nextFp; set pc $retAddr} {
	set blk [wtxMemRead [expr $fp -12] 12]
	bindNamesToList {nextFp sp retAddr} [memBlockGet -l $blk]
	memBlockDelete $blk
	set nArg 0
	set callAddr [trcFindCall $retAddr]
	set level [list $callAddr [trcFindFuncStart $fp $pc $callAddr] \
			$nArg [trcPrintArgs [expr $fp + 8] $nArg]]
	set trcStack [linsert $trcStack 0 $level]
	if {$nextFp == 0} {
	    break
	}
    }

    return $trcStack
}

#############################################################################
#
# trcFindCall - get address from which function was called
#
# SYNOPSIS:
#	trcFindCall addr
#
# PARAMETERS:
#	addr: address where we returned
#
# ERRORS: N/A
#
# RETURNS: address from which current subroutine was called, or NULL.
#
proc trcFindCall {retAddr} {

    if {$retAddr == 0} {
	return 0	; # not found
    }

    # starting at the word preceding the return addr, search for an
    # instruction that changes the pc

    for {set addr [expr $retAddr - 4]} {$addr != 0} {incr addr -4} {
	
	set blk [wtxMemRead $addr 4]
	set inst [expr [memBlockGet -l $blk]]
	memBlockDelete $blk
	if {[armInstrChangesPc $inst]} {
	    return $addr	; # found it
	}

    }
    return 0	; # not found
}

##############################################################################
#
# trcFindFuncStart - find the starting address of a function
#
# This routine finds the starting address of a function by one of several ways.
#
# If the given frame pointer points to a legitimate frame pointer, then the
# long word preceding the frame pointer pointed to by the frame pointer should
# be the return address of the function call.  Then the instruction preceding
# the return address would be the function call, and the address can be gotten
# from there, provided that the call was to an absolute address.  If it was,
# use that address as the function address.  Note that a routine that is
# called by other than a BL (e.g., indirectly) will not meet these
# requirements.
#
# If the above check fails, we search backward from the given pc until a
# routine entry sequence is found i.e.
#    MOV   ip,sp
#    STMDB sp!,{..fp,ip,lr,pc}
#    SUB   fp,ip,#4
# If the compiler is inserting these sequences at the beginning of ALL
# subroutines, then this will reliably find the start of the routine.
# However, some compilers allow routines, especially "leaf" routines that
# don't call any other routines, NOT to have stack frames, which will cause
# this search to fail.
#
# In either of the above cases, the value is bounded by the nearest routine
# in the system symbol table, if there is one.  If neither method returns a
# legitimate value, then the value from the symbol table is used.  Note that
# the routine may not be in the symbol table if it is LOCAL, etc.
#
# Note that the start of a routine that is not called by BL and
# doesn't start with a frame creation sequence and isn't in the symbol table,
# may not be possible to locate.
#
# SYNOPSIS:
#	trcFindFuncStart fp pc addr
#
# PARAMETERS:
#	fp: content of fp register
#	pc: content of pc register
#	addr: address in the function
#
# ERRORS: N/A
#
# RETURNS: the name of the calling function
#
proc trcFindFuncStart {fp pc callerAddr} {
    global IMASK_MOV_IP_SP
    global IOP_MOV_IP_SP
    global IMASK_STMDB_SPP_FP_IP_LR_PC
    global IOP_STMDB_SPP_FP_IP_LR_PC
    global IMASK_SUB_FP_IP_4
    global IOP_SUB_FP_IP_4
    global IMASK_SUB_FP_IP_4PLUS
    global IOP_SUB_FP_IP_4PLUS
    global IMASK_STMDB_SPP_AREGS
    global IOP_STMDB_SPP_AREGS
    global IMASK_SUB_SP_SP
    global IOP_SUB_SP_SP

    set minPc 0
    set symName "????"

    # lookup current pc in symbol table and use that as lower bound for
    # start of current function.

    if [catch "wtxSymFind -value $pc" sym] {
	if {([wtxErrorName $sym] == "SYMTBL_SYMBOL_NOT_FOUND") ||
	    ([wtxErrorName $sym] == "SVR_INVALID_FIND_REQUEST")} {
	    set minPc 0
	} else {
	    error $sym
	}
    } else {
    	set minPc [lindex $sym 1]
	set symName [lindex $sym 0]
    }

    if {$fp != 0} {
	set ip $callerAddr
    	if {$ip != 0} {
	    set ip [trcFindDest $ip]
	    if {($ip != 0) && ($ip >= $minPc)} {
		return [trcFuncStartName $ip]
	    }
	}
    }

    # search backwards for function prologue
    # Note: we fetch 3 extra words because the prologue is 3/4 words
    # long and the PC could be pointing to the first word of the prologue
    # (for possible prologues see trcStack() above)

    set size [expr $pc - $minPc + 16]
    set blk [wtxMemRead $minPc $size]

    for {set i [expr $size -16]} {$i >= 0} {incr i -4} {
	bindNamesToList {inst0 inst1 inst2 inst3} [memBlockGet -l $blk $i 4]
	if {(($inst0 & $IMASK_MOV_IP_SP) == $IOP_MOV_IP_SP) &&
	    ((($inst1 & $IMASK_STMDB_SPP_FP_IP_LR_PC) ==
				    $IOP_STMDB_SPP_FP_IP_LR_PC) &&
	     (($inst2 & $IMASK_SUB_FP_IP_4) == $IOP_SUB_FP_IP_4)) ||
	    ((($inst2 & $IMASK_STMDB_SPP_FP_IP_LR_PC) ==
				    $IOP_STMDB_SPP_FP_IP_LR_PC) &&
	     (($inst3 & $IMASK_SUB_FP_IP_4PLUS) == $IOP_SUB_FP_IP_4PLUS) &&
	     ((($inst1 & $IMASK_STMDB_SPP_AREGS) == $IOP_STMDB_SPP_AREGS) ||
	      (($inst1 & $IMASK_SUB_SP_SP) == $IOP_SUB_SP_SP)))} {

	    memBlockDelete $blk
	    return [trcFuncStartName [expr $minPc + $i]]
	}
    }
    memBlockDelete $blk

    # return nearest symbol in sym tbl

    return $symName
}

##############################################################################
#
# trcFuncStartName - return the name of the calling function
#
# SYNOPSIS: trcFuncStartName addr
#
# PARAMETER:
# 	addr: address of the symbol about which we want the name
#
# ERRORS: N/A
#
# RETURNS:
#	the name of the symbol if <addr> is the address of a symbol
#       the address if a symbol is found but with a different address
#       ???? when symbol search fails
#

proc trcFuncStartName {addr} {

    if [catch "wtxSymFind -value $addr" sym] {
	if {([wtxErrorName $sym] != "SYMTBL_SYMBOL_NOT_FOUND") &&
	    ([wtxErrorName $sym] != "SVR_INVALID_FIND_REQUEST")} {
	    error $sym
	}
    } else {
	set symVal [lindex $sym 1]
	if {$symVal == $addr} {
	    return [lindex $sym 0]
	} else {
	    return [format "%x" $addr]
	}
    }
    return "????"
}

#############################################################################
#
# trcFindDest - find destination of call instruction
#
# SYNOPSIS:
#	trcFindDest addr
#
# PARAMETERS:
#	addr: address of the call instruction
#
# ERRORS: N/A
#
# RETURNS: address to which call instruction (BL) will branch, or NULL if
# 	   unknown
#

proc trcFindDest {callAddr} {
    global IMASK_BL
    global IOP_BL

    set blk [wtxMemRead $callAddr 4]
    set inst [expr [memBlockGet -l $blk 0 1]]
    memBlockDelete $blk

    # BL?

    if {($inst & $IMASK_BL) == $IOP_BL} {

	# extract offset, sign extend it and add it to current PC,
	# adjusting it for the pipeline

	set offset [expr $inst & 0x00ffffff]
	if {($offset & 0x00800000) != 0} {
	    set offset [expr $offset | 0xFF000000]
	}
	set offset [expr $offset << 2]
	return [expr $callAddr + 8 + $offset]
    }
    return 0
}

############################################################################
#
# trcCountArgs - find number of arguments to function
#
# This routine finds the number of arguments passed to the called function
# by examining the stack-pop at the return address.  Many compilers offer
# optimization that defeats this (e.g., by coalescing stack-pops), so a return
# value of 0 may mean "don't know".
#
# SYNOPSIS:
#	trcCountArgs addr
#
# PARAMETERS:
#	addr: return address
#
# ERRORS: N/A
#
# RETURNS: number of arguments of function or default number
#
proc trcCountArgs {retAddr} {

    # ARM compiler does not provide any way to determine number of arguments

    return 0
}

##############################################################################
#
# dbgRetAdrsGet - get return address for current routine
#
# SYNOPSIS:
#	dbgRetAdrsGet ctype cid
#
# PARAMETERS:
#       ctype: context type
#       cid: context id
#
# ERRORS: N/A
#
# RETURNS: return address for current routine
#

proc dbgRetAdrsGet {ctype cid} {
    global IMASK_MOV_IP_SP
    global IOP_MOV_IP_SP
    global IMASK_STMDB_SPP_FP_IP_LR_PC
    global IOP_STMDB_SPP_FP_IP_LR_PC
    global IMASK_SUB_FP_IP_4
    global IOP_SUB_FP_IP_4
    global IMASK_SUB_FP_IP_4PLUS
    global IOP_SUB_FP_IP_4PLUS
    global IMASK_STMDB_SPP_AREGS
    global IOP_STMDB_SPP_AREGS
    global IMASK_SUB_SP_SP
    global IOP_SUB_SP_SP

    set regSet [regSetOfContext $ctype $cid]
    set fp [lindex $regSet 11]

    # if the frame pointer is zero, assume return address is in lr

    if {$fp == 0} {
	return [lindex $regSet 14]
    }

    # Frame pointer is non-zero - are we in the entry sequence?
    # First instruction of 3/4 instruction entry sequence can be up to
    # 2/3 instructions before the current pc so read 7 instructions
    # starting 3 instructions before the current pc.
    # (for possible prologues see trcStack() above)

    set pc [pcOfRegSet $regSet]
    set blk [wtxMemRead [expr $pc - 12] 28]

    # look for entry sequence

    for {set i 12} {$i >= 0} {incr i -4} {
	set inst0 [expr [memBlockGet -l $blk $i 1]]
	if {($inst0 & $IMASK_MOV_IP_SP) == $IOP_MOV_IP_SP} {
	    break
	}
    }

    if {$i >= 0} {

	# found mov ip,sp so examine next three instructions

	incr i 4
	bindNamesToList {inst1 inst2 inst3} [memBlockGet -l $blk $i 3]
	if {((($inst1 & $IMASK_STMDB_SPP_FP_IP_LR_PC) ==
				    $IOP_STMDB_SPP_FP_IP_LR_PC) &&
	    (($inst2 & $IMASK_SUB_FP_IP_4) == $IOP_SUB_FP_IP_4) &&
	    ($i > 4))
	    ||
	    ((($inst2 & $IMASK_STMDB_SPP_FP_IP_LR_PC) ==
					$IOP_STMDB_SPP_FP_IP_LR_PC) &&
	     (($inst3 & $IMASK_SUB_FP_IP_4PLUS) == $IOP_SUB_FP_IP_4PLUS) &&
	     ((($inst1 & $IMASK_STMDB_SPP_AREGS) == $IOP_STMDB_SPP_AREGS) ||
	      (($inst1 & $IMASK_SUB_SP_SP) == $IOP_SUB_SP_SP)))} {
	    memBlockDelete $blk
	    return [lindex $regSet 14]
	}
    }

    memBlockDelete $blk

    # frame pointer is non-zero and we're NOT in an entry sequence so
    # extract return address from stack frame

    set blk [wtxMemRead [expr $fp - 4] 4]
    set lr [expr [memBlockGet -l $blk]]
    memBlockDelete $blk
    return $lr
}

##############################################################################
#
# dbgFuncCallCheck - check next instruction
#
# This routine checks to see if the next instruction is a function call.
# If it is, it returns TRUE, otherwise, returns FALSE.
#
# SYNOPSIS:
#	dbgFuncCallCheck addr
#
# PARAMETERS:
#	addr: where to find instruction
#
# ERRORS: N/A
#
# RETURNS:
#	1 if next instruction calls a function, 0 otherwise
#

proc dbgFuncCallCheck {addr} {
    global IMASK_BL
    global IOP_BL
    global IMASK_MOVXX_LR_PC
    global IOP_MOVXX_LR_PC

    # we define function call as either
    #    1. a BL or
    #    2. a MOV LR,PC/<PC changing instruction>

    set blk [wtxMemRead [expr $addr - 4] 8]
    bindNamesToList {prev inst} [memBlockGet -l $blk]
    memBlockDelete $blk

    if {(($inst & $IMASK_BL) == $IOP_BL) ||
	    ((($prev & $IMASK_MOVXX_LR_PC) == $IOP_MOVXX_LR_PC) &&
		[armInstrChangesPc $inst])} {
	return 1
    } else {
	return 0
    }
}

##############################################################################
#
# dbgNextAddrGet - Get the next instruction address
#
# This function disassembles the current instruction in order to get the
# address of the next one.
#
# SYNOPSIS:
#	dbgNextAddrGet addr
#
# PARAMETERS:
#	addr: address of the current instruction
#
# ERRORS: N/A
#
# RETURNS: next instruction address
#

proc dbgNextAddrGet {addr} {

    # This is used by so() to determine where to place the next
    # breakpoint so it should not return the address of the next
    # instruction to be executed (e.g. by calculating the destination of
    # a BL) but just the address of the instruction that lies next in
    # memory after the current one.
    #
    # disassemble 1 instruction

    bindNamesToList {line nextAddr} [disassemble $addr 1 0 0]

    return $nextAddr
}

##############################################################################
#
# armInstrChangesPc - determine if current instruction changes PC
#
# This function examines an instruction to determine whether it will
# change the value of the PC in a non-sequential fashion.
#
# SYNOPSIS:
#	armInstrChangesPc instr
#
# PARAMETERS:
#	instr: the instruction to examine
#
# ERRORS: N/A
#
# RETURNS: 1 if the instruction changes the PC, 0 otherwise
#

proc armInstrChangesPc {instr} {

    # Examine the instruction to determine whether it changes the PC
    # other than in the usual incremental fashion. Note that we don't have
    # the CPSR value so we just assume the instruction will be executed.
    #
    # The following code is a translation of the target code.

    switch [expr ($instr & 0x0F000000) >> 24] {
        0 -
        1 -
        2 -
        3 {
	    # data processing
	    # includes signed byte/halfword loads
	    # includes MUL, BX
	    if {(($instr & 0x0000F000) >> 12) == 15} {	# Rd
                return 1
	    }
	}

        4 -
        5 -
        6 -
        7 {
	    # data transfer
	    if {((($instr >> 20) & 1) == 1) &&
			((($instr & 0x0000F000) >> 12) == 15) &&
			((($instr >> 22) & 1) == 0)} {
		# load, PC and not a byte load
                return 1
	    }
	}

        8 -
        9 {
	    # block transfer
	    if {((($instr >> 20) & 1) == 1) && ((($instr >> 15) & 1) == 1)} {
		# loading PC
                return 1
            }
	}

        10 -
        11 {
	    # branch and branch & link
            return 1
	}

        12 -
        13 -
        14 -
        15 {
	    # coproc ops
	    # SWI
	    return 0
	}
    }
    return 0
}
