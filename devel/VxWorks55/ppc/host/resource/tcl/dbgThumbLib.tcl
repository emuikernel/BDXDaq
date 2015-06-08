# dbgThumbLib.tcl - Thumb-specific Tcl implementation
#
# Copyright 1997 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01c,27oct97,kkk  took out EOF line from end of file.
# 01b,10oct97,cdp  added backtrace code, replacing null functions.
# 01a,20jun97,cdp  derived from dbgArmLib.tcl 01b.
#

# DESCRIPTION
# /target/src/arch/arm/trcLib.c inspired
# /target/src/arch/arm/dbgArchLib.c inspired

# globals

# some Thumb instruction decoding stuff

set IMASK_T_BL0		0xf800
set IOP_T_BL0		0xf000

set IMASK_T_BL1		0xf800
set IOP_T_BL1		0xf800

set IMASK_T_MOV_LR_PC	0xffff
set IOP_T_MOV_LR_PC	0x46fe

set IMASK_T_POP_LO	0xff00
set IOP_T_POP_LO	0xbc00

set IMASK_T_MOV_FP_LO	0xffc7
set IOP_T_MOV_FP_LO	0x4683

set IMASK_T_MOV_SP_LO	0xffc7
set IOP_T_MOV_SP_LO	0x4685

set IMASK_T_BX_LO	0xffc0
set IOP_T_BX_LO		0x4700

set IMASK_T_MOV_LO_PC	0xfff8
set IOP_T_MOV_LO_PC	0x4678

set IMASK_T_MOV_LO_FP	0xfff8
set IOP_T_MOV_LO_FP	0x4658

set IMASK_T_PUSH_LO	0xff00
set IOP_T_PUSH_LO	0xb400

set IMASK_T_PUSH	0xff00
set IOP_T_PUSH		0xb500

set IMASK_T_SUB_SP_16	0xffff
set IOP_T_SUB_SP_16	0xb084

set IMASK_T_ADD_LO_SP	0xf800
set IOP_T_ADD_LO_SP	0xa800

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
# SYNOPSIS:
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
    global IMASK_T_POP_LO
    global IOP_T_POP_LO
    global IMASK_T_MOV_FP_LO
    global IOP_T_MOV_FP_LO
    global IMASK_T_MOV_SP_LO
    global IOP_T_MOV_SP_LO
    global IMASK_T_BX_LO
    global IOP_T_BX_LO
    global IMASK_T_MOV_LO_PC
    global IOP_T_MOV_LO_PC
    global IMASK_T_MOV_LO_FP
    global IOP_T_MOV_LO_FP	

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
    #  1) we are in the entry sequence (prologue) of a routine which
    #     establishes a stack frame;
    #  2) we are in the exit sequence (epilogue) of a routine and the stack
    #     frame is being collapsed;
    # Note that, if we are in a routine which does not create a stack
    # frame, we cannot readily distinguish between this and the body of a
    # routine that does create a stack frame so the results are not
    # reliable.
    #
    # We have identified the following prologues. They vary
    # substantially depending on the type of routine and, notably, do not
    # always start with the same instruction. The prologue may be changed
    # later to make it more efficient.
    #
    #		push	{a_regs}	/@ only present if required @/
    #		sub	sp,#16
    #		push	{v_regs}	/@ only present if required @/
    #					/@ includes lr if non-leaf routine @/
    #		add	rx,sp,#n
    #		str	rx,[sp,#]
    #		mov	rx,pc		/@ 1 @/
    #		str	rx,[sp,#]	/@ 2 @/
    #		mov	rx,fp		/@ 3 @/
    #		str	rx,[sp,#]	/@ 4 @/
    #		mov	rx,lr
    #		str	rx,[sp,#]
    #		add	rx,sp,#n
    #		mov	fp,rx
    #
    # The instructions labelled 1, 2, 3 and 4 can appear in the order 3,4,1,2
    # in some routines.
    #
    # For Thumb we also need to be aware of routine epilogues, as,
    # unlike the ARM epilogue, which is atomic (one instruction), the
    # Thumb epilogue takes several instructions, during which the frame
    # is collapsed. The epilogue is not currently "strictly-conforming"
    # in that fp does not always point to a valid frame: there is a
    # window between the popping of part of the frame and the changing
    # of fp to point to the previous frame.
    #
    # The routine epilogue does not appear to vary much:
    #
    # leaf:
    #		pop	{v_regs}	/@ only present if required @/
    #		pop	{rx, ry}
    #		mov	fp,rx
    #		mov	sp,ry
    #		bx	lr
    # non-leaf:
    #		pop	{v_regs}	/@ only present if required @/
    #		pop	{rx, ry, rz}
    #		mov	fp,ry
    #		mov	sp,rz
    #		bx	rx

    set in_prologue 0
    set in_epilogue 0

    set pc [pcOfRegSet $regSet]
    set fp [lindex $regSet 11]
    set lr [lindex $regSet 14]

    # First check if we are in the epilogue. Thumb code, unlike ARM
    # code, does not have an atomic epilogue. The epilogue is 4
    # instructions long and can be up to 3 instructions before the
    # current pc so fetch 7 instructions.

    set blk [wtxMemRead [expr $pc - 6] 14]

    for {set i 6} {$i >= 0} {incr i -2} {
	bindNamesToList {inst0 inst1 inst2 inst3} [memBlockGet -w $blk $i 4]
	if {(($inst0 &$IMASK_T_POP_LO) == $IOP_T_POP_LO) &&
	    (($inst1 & $IMASK_T_MOV_FP_LO) == $IOP_T_MOV_FP_LO) &&
	    (($inst2 & $IMASK_T_MOV_SP_LO) == $IOP_T_MOV_SP_LO) &&
	    (($inst3 & $IMASK_T_BX_LO) == $IOP_T_BX_LO)} {
	    set in_epilogue 1
	    break
	}
    }

    memBlockDelete $blk


    if {$in_epilogue} {

	if {$i == 6} {

	    # Have not yet executed POP instruction so it's the same as
	    # if we're not in the epilogue and fp still points to a valid
	    # backtrace structure. Note: we have also learnt that we are
	    # not in the prologue.

	    set in_epilogue 0

	} else {

	    # Have POPped previous fp,sp into low registers and may or
	    # may not have restored fp. Assume we have not and extract fp
	    # from the low register used in the "MOV fp,loreg"
	    # instruction and lr from the register used in the "BX reg"
	    # instruction.

	    set reg [expr ($inst1 & 0x38) >> 3]
	    set fp [lindex $regSet $reg]

	    set reg [expr ($inst3 & 0x38) >> 3]
	    set lr [lindex $regSet $reg]
	}

    } else {

	# We are not in the epilogue but may be in the prologue.
	# Look up the current PC in the symbol table. If we find it, we
	# must be at the start of a function and so have not yet created
	# a backtrace structure.

	if {[lindex [wtxSymFind -value $pc] 1] == $pc} {
	    set in_prologue 1

	} else {

	    # Check if in the prologue.
	    # Look for the last instruction of the prologue which can
	    # be up to 12 instructions after the current one. If we find
	    # it, we then need to check the instruction 5 before it so
	    # read 5+1+12 = 18 instructions

	    set blk [wtxMemRead [expr $pc - 10] 36]

	    for {set i 10} {$i <= 34} {incr i 2} {
		set inst0 [expr [memBlockGet -w $blk $i 1]]
		if {($inst0 & $IMASK_T_MOV_FP_LO) == $IOP_T_MOV_FP_LO} {
		    break
		}
	    }

	    if {$i <= 34} {

		# Found last instruction of prologue.
		# Check the instruction 5 before.

		set inst1 [expr [memBlockGet -w $blk [expr $i - 10] 1]]
		if {(($inst1 & $IMASK_T_MOV_LO_PC) == $IOP_T_MOV_LO_PC) ||
		    (($inst1 & $IMASK_T_MOV_LO_FP) == $IOP_T_MOV_LO_FP)} {
		    set in_prologue 1
		}
	    }

	    memBlockDelete $blk
	}
    }


    # if we don't have a valid backtrace structure, fake one

    if {$in_prologue || $in_epilogue} {

	set sp [lindex $regSet 13]

	# save 4 words on top of stack
	set savedStack [wtxGopherEval "$sp -16 @@@@"]

	# create a stack frame and write it to target
	set blk [memBlockCreate $endianFlag 16]
	memBlockSet -l $blk 0 $fp $sp $lr $pc
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
# SYNOPSIS:
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

    for {set addr [expr ($retAddr & (~1))  - 2]} {$addr != 0} {incr addr -2} {
	set blk [wtxMemRead $addr 2]
	set inst [expr [memBlockGet -w $blk]]
	memBlockDelete $blk
	if {[thumbInstrChangesPc $inst]} {
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
# routine entry sequence is found (see trcStack() above for description of
# function prologues).
#
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
    global IMASK_T_PUSH_LO
    global IOP_T_PUSH_LO
    global IMASK_T_PUSH
    global IOP_T_PUSH
    global IMASK_T_SUB_SP_16
    global IOP_T_SUB_SP_16
    global IMASK_T_ADD_LO_SP
    global IOP_T_ADD_LO_SP

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

    # Search backwards for function prologue.
    # Note: we fetch 3 extra instructions because the bit of the
    # prologue we are interested in is up to 4 instructions long and the
    # PC could be pointing to the first word of the prologue (for
    # possible prologues, see trcStack() above).

    set size [expr $pc - $minPc + 8]
    set blk [wtxMemRead $minPc $size]
    for {set i [expr $size - 8]} {$i >= 0} {incr i -4} {
	bindNamesToList {inst0 inst1 inst2 inst3} [memBlockGet -w $blk $i 4]
	if {((($inst0 & $IMASK_T_PUSH_LO) == $IOP_T_PUSH_LO) &&
	     (($inst1 & $IMASK_T_SUB_SP_16) == $IOP_T_SUB_SP_16) &&
	     (($inst2 & $IMASK_T_PUSH) == $IOP_T_PUSH) &&
	     (($inst3 & $IMASK_T_ADD_LO_SP) == $IOP_T_ADD_LO_SP)) ||
	    ((($inst0 & $IMASK_T_SUB_SP_16) == $IOP_T_SUB_SP_16) &&
	     (($inst1 & $IMASK_T_PUSH) == $IOP_T_PUSH) &&
	     (($inst2 & $IMASK_T_ADD_LO_SP) == $IOP_T_ADD_LO_SP)) ||
	    ((($inst0 & $IMASK_T_SUB_SP_16) == $IOP_T_SUB_SP_16) &&
	     (($inst1 & $IMASK_T_ADD_LO_SP) == $IOP_T_ADD_LO_SP))} {

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
    global IMASK_T_BL0
    global IMASK_T_BL1
    global IOP_T_BL0
    global IOP_T_BL1

    set blk [wtxMemRead $callAddr 4]
    bindNamesToList {inst inext} [memBlockGet -w $blk]
    memBlockDelete $blk

    # BL?

    if {(($inst & $IMASK_T_BL0) == $IOP_T_BL0) &&
	(($inext & $IMASK_T_BL1) == $IOP_T_BL1)} {

	# extract offset, sign extend it and add it to current PC,
	# adjusting it for the pipeline

	set offset [expr (($inst & 0x7ff) << 12) | (($inext & 0x7ff) << 1)]
	if {($offset & 0x00400000) != 0} {
	    set offset [expr $offset | 0xff800000]
	}
	return [expr $callAddr + 4 + $offset]
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

    # Thumb compiler does not provide any way to determine number of arguments

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
    global IMASK_T_POP_LO
    global IOP_T_POP_LO
    global IMASK_T_MOV_FP_LO
    global IOP_T_MOV_FP_LO
    global IMASK_T_MOV_SP_LO
    global IOP_T_MOV_SP_LO
    global IMASK_T_BX_LO
    global IOP_T_BX_LO
    global IMASK_T_MOV_LO_PC
    global IOP_T_MOV_LO_PC
    global IMASK_T_MOV_LO_FP
    global IOP_T_MOV_LO_FP	

    set regSet [regSetOfContext $ctype $cid]
    set fp [lindex $regSet 11]

    # if the frame pointer is zero, assume return address is in lr

    if {$fp == 0} {
	return [expr [lindex $regSet 14] & (~1)]
    }

    # Frame pointer is non-zero.
    # First check if we are in the epilogue. Thumb code, unlike ARM
    # code, does not have an atomic epilogue. The epilogue is 4
    # instructions long and can be up to 3 instructions before the
    # current pc so fetch 7 instructions.
    # (for possible prologues see trcStack() above)

    set pc [pcOfRegSet $regSet]
    set blk [wtxMemRead [expr $pc - 6] 14]

    for {set i 6} {$i >= 0} {incr i -2} {
	bindNamesToList {inst0 inst1 inst2 inst3} [memBlockGet -w $blk $i 4]
	if {(($inst0 &$IMASK_T_POP_LO) == $IOP_T_POP_LO) &&
	    (($inst1 & $IMASK_T_MOV_FP_LO) == $IOP_T_MOV_FP_LO) &&
	    (($inst2 & $IMASK_T_MOV_SP_LO) == $IOP_T_MOV_SP_LO) &&
	    (($inst3 & $IMASK_T_BX_LO) == $IOP_T_BX_LO)} {
	    break
	}
    }

    memBlockDelete $blk


    if {$i >= 0} {

	# in epilogue

	if {$i != 6} {

	    # Have executed the POP instruction so the return address will be
	    # in whichever register is used in the BX instruction at the end of
	    # the epilogue. Extract the return address from that register.

	    set reg [expr ($inst3 & 0x38) >> 3]
	    return [expr [lindex $regSet $reg] & (~1)]
	}

    } else {

	# We are not in the epilogue but may be in the prologue.
	# Look for the last instruction of the prologue which can
	# be up to 12 instructions after the current one. If we find
	# it, we then need to check the instruction 5 before it so
	# read 5+1+12 = 18 instructions

	set blk [wtxMemRead [expr $pc - 10] 36]

	for {set i 10} {$i <= 34} {incr i 2} {
	    set inst0 [expr [memBlockGet -w $blk $i 1]]
	    if {($inst0 & $IMASK_T_MOV_FP_LO) == $IOP_T_MOV_FP_LO} {
		break
	    }
	}

	if {$i <= 34} {

	    # Found last instruction of prologue.
	    # Check the instruction 5 before.

	    set inst1 [expr [memBlockGet -w $blk [expr $i - 10] 1]]
	    if {(($inst1 & $IMASK_T_MOV_LO_PC) == $IOP_T_MOV_LO_PC) ||
		(($inst1 & $IMASK_T_MOV_LO_FP) == $IOP_T_MOV_LO_FP)} {
		return [expr [lindex $regSet 14] & (~1)]
	    }
	}

	memBlockDelete $blk
    }

    # not in prologue or epilogue so the return address is in the backtrace
    # structure pointed to by fp (known to be !0).

    set blk [wtxMemRead [expr $fp -4] 4]
    set lr [expr [memBlockGet -l $blk]]
    memBlockDelete $blk
    return [expr $lr & (~1)]
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
    global IMASK_T_BL0
    global IMASK_T_BL1
    global IOP_T_BL0
    global IOP_T_BL1
    global IMASK_T_MOV_LR_PC
    global IOP_T_MOV_LR_PC
 
    # we define function call as either
    #    1. a BL or
    #    2. a MOV LR,PC/<PC changing instruction>

    set blk [wtxMemRead [expr $addr - 2] 6]
    bindNamesToList {prev inst next} [memBlockGet -w $blk]
    memBlockDelete $blk

    if {((($inst & $IMASK_T_BL0) == $IOP_T_BL0) &&
	 (($next & $IMASK_T_BL1) == $IOP_T_BL1))
	||
	((($prev & $IMASK_T_MOV_LR_PC) == $IOP_T_MOV_LR_PC) &&
	 [thumbInstrChangesPc $inst])} {
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
# thumbInstrChangesPc - determine if current instruction changes PC
#
# This function examines an instruction to determine whether it will
# change the value of the PC in a non-sequential fashion.
#
# SYNOPSIS:
#	thumbInstrChangesPc instr
#
# PARAMETERS:
#	instr: the instruction to examine
#
# ERRORS: N/A
#
# RETURNS: 1 if the instruction changes the PC, 0 otherwise
#

proc thumbInstrChangesPc {instr} {

    # Examine the instruction to determine whether it changes the PC
    # other than in the usual incremental fashion. Note that we don't have
    # the CPSR value so we just assume the instruction will be executed.
    #
    # The following code is a translation of the target code.

    switch [expr ($instr & 0xf000) >> 12] {
        0 -
        1 -
        2 -
        3 -
	5 -
	6 -
	7 -
	8 -
	9 -
	10 -
	12 {
	    # no effect on PC - next instruction executes
                return 0
	}

        4 {
	    if {(($instr >> 7) & 0x1f) == 0xe} {
		# BX
		return 1
	    } else {
		if {(($instr & 0x80) != 0) &&
		    (($instr & 0xc07) == 0x407) &&
		    (($instr & 0x300) != 0x100)} {
		    # does something to PC and is not CMP
		    return 1
		}
	    }
	}

        11 {
	    if {($instr & 0xf00) == 0xd} {
		# POP {rlist, pc}
		return 1
	    }
	}

        13 {
	    if {($instr & 0xf00) != 0xf00} {
		# conditional branch, not SWI
		return 1
	    }
	}

        14 {
	    if {($instr & 0x800) == 0} {
		# unconditional branch
                return 1
            }
	}

        15 {
	    # BL
	    if {($instr & 0x800) == 0} {
		# BL prefix
		return 1
	    } else {
		# BL suffix
		return 0
	    }
	}
    }
}
