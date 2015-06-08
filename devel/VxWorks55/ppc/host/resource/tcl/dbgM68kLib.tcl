# dbgM68kLib.tcl - 68k-specific Tcl implementation 
#
# Copyright 1996 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01f,17sep97,fle  changing call to disassemble in call to wtxMemDisassemble
# 01e,06jun96,elp  made dbgRetAdrsGet{} work in sytem mode (SPR# 6659)
#		   + improve dbgRetAdrsGet{} behavior in function prologue.
# 01d,08mar96,elp  made stack frame faking be effective + improve tt behavior
#		   when routine begins with PEA (A6), MOVEA .L A7,A6.
# 01c,06mar96,elp  fix tt bug (memBlockCreate initial value is only a byte).
# 01b,06mar96,elp  renamed and added debug routines,
#                  caught SVR_INVALID_FIND_REQUEST errors,
#		   added expr before each memBlockGet returning one value.
# 01a,02feb96,elp  written.
#

# DESCRIPTION
# /target/src/arch/mc68k/trcLib.c inspired
# /target/src/arch/mc68k/dbgArchLib.c inspired

# globals

set defArgNb		6	; # default number of argument

set LINK_A6		0x4e56	; # LINK A6,...
set RTS			0x4e75	; # RTS
set JSR_ABS		0x4eb9	; # JSR abs
set ADD_W		0xdefc	; # ADD.W
set ADD_L		0xdffc	; # ADD.L
set ADDQ_W		0x504f	; # ADDQ.W A7
set ADDQ_L		0x508f	; # ADDQ.L A7
set LEA_A7		0x4fef	; # LEA $x(A7),A7
set MOVE_L_A7		0x2e80	; # MOVE.L xxx,(A7)
set MOVE_L_A6_A7	0x2e75	; # MOVE.L (xxx,A6),A7
set PEA_A6		0x4856	; # PEA (A6)
set MOVEA_A7_A6		0x2c4f	; # MOVEA .L A7,A6

set JSR			0x4e80	
set BSR			0x6100

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
# routines that start with a LINK instruction.  Most VxWorks assembly
# language routines include LINK instructions for exactly this reason.
# However, routines written in other languages, strange entries into
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
    global LINK_A6
    global RTS
    global MOVEA_A7_A6
    global endianFlag

    # get the register set of the task

    set regSet [regSetOfContext CONTEXT_TASK $tid]

    #
    # if the current routine doesn't have a stack frame, then we fake one
    # by putting the old one on the stack and making fp point to that;
    # we KNOW we don't have a stack frame in a few restricted but useful
    # cases:
    #  1) we are at a LINK or RTS instruction,
    #  2) we are the first instruction of a subroutine (this may NOT be
    #     a LINK instruction with some compilers)
    #  3) we are on the second instruction of a routine beginnig by PEA (A6);
    #	  MOVEA .L A7,A6
    #

    set pc [pcOfRegSet $regSet]

    set blk [wtxMemRead $pc 2]
    set inst [expr [memBlockGet -w $blk]]
    memBlockDelete $blk

    bindNamesToList {retAddr inst} [trcFollowBra $inst $pc]

    set fp [lindex $regSet 14]

    if {($inst == $LINK_A6) || ($inst == $RTS) ||
	([lindex [wtxSymFind -value $pc] 1] == $pc)} { 
	# no stack frame - fake one 
	# save value we are going to clobber 
	# make new frame pointer by sticking old on e on stack
	# and pointing to it

	set sp [lindex $regSet 15]
	set savedStack [wtxGopherEval "$sp -4 @"]
	set blk [memBlockCreate $endianFlag 4]
	memBlockSet -l $blk 0 $fp
	wtxMemWrite $blk [expr $sp - 4]
	memBlockDelete $blk

	set fp [expr $sp - 4]
	set trc [trcStackLvl $fp $pc]

	set blk [memBlockCreate $endianFlag 4]
	memBlockSet -l $blk 0 $savedStack
	wtxMemWrite $blk [expr $sp - 4]
	memBlockDelete $blk
    } elseif {(($inst == $MOVEA_A7_A6) &&
		([lindex [wtxSymFind -value $pc] 1] == ($pc - 2)))} {
	set sp [lindex $regSet 15]
	set fp [expr $sp]
	set trc [trcStackLvl $fp $pc]
    } else {
	set trc [trcStackLvl $fp $pc]
    }
    return $trc
}

#############################################################################
#
# trcFollowBra - resolve any BRA instructions to final destination
#
# This routine returns a pointer to the next non-BRA instruction to be
# executed if the pc were at the specified <adrs>.  That is, if the instruction
# at <adrs> is not a BRA, then <adrs> is returned.  Otherwise, if the
# instruction at <adrs> is a BRA, then the destination of the BRA is
# computed, which then becomes the new <adrs> which is tested as before.
# Thus we will eventually return the address of the first non-BRA instruction
# to be executed.
#
# The need for this arises because compilers may put BRAs to instructions
# that we are interested in, instead of the instruction itself.  For example,
# optimizers may replace a stack pop with a BRA to a stack pop.  Or in very
# UNoptimized code, the first instruction of a subroutine may be a BRA to
# a LINK, instead of a LINK (compiler may omit routine "post-amble" at end
# of parsing the routine!).  We call this routine anytime we are looking
# for a specific kind of instruction, to help handle such cases.
#
# SYNOPSYS:
#	trcFollowBra inst addr
#
# PARAMETERS:
#	inst: current instruction
#	addr: address of the instruction
#
# ERRORS: N/A
#
# RETURNS: a two element list composed of address of the first 
#	   non-BRA instruction and coresponding instruction
#

proc trcFollowBra {inst adrs} {
    
    while [expr ($inst & 0xff00) == 0x6000] {
	incr adrs 2;	# points to following word

	switch [expr $inst & 0xff] {
	    0	{set disType {2 -w}	;# 16 bit displacement}
	    255	{set disType {4 -l}	;# 32 bit displacement}
	    default {set disType {1 -b}	;# 8 bit displacement}
	}
	set blk [wtxMemRead $adrs [lindex $disType 0]]
	set displacement [expr [memBlockGet [lindex $disType 1] $blk]]
	memBlockDelete $blk

	# check for branch to self, or to odd displacement

	if {(([lindex $disType 0] == 1) && \
	     (($displacement == 254) || ($displacement & 0x1)))} {
	    incr adrs -2	;# do not follow it
	    return $adrs
	}
	set adrs [expr $adrs + $displacement]
	set blk [wtxMemRead $adrs 2]
	set inst [expr [memBlockGet -w $blk]]
	memBlockDelete $blk
    }
    return "$adrs $inst"
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
    global portable
    
    set trcStack {}
    if {$fp == 0} {
	return $trcStack	; # stack is untraceable
    }

    #
    # get for each level values that should be read on the target
    # fp and retAddr
    #

    for {set depth 0} {$depth < 40} \
	{incr depth; set fp $nextFp; set pc $retAddr} {
	set blk [wtxMemRead $fp 8]
	bindNamesToList {nextFp retAddr} [memBlockGet -l $blk]
	memBlockDelete $blk
	set nArg [trcCountArgs $retAddr]
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
    
    # starting at the word preceding the return addr, search for jsr or bsr 

    for {set addr [expr $retAddr - 2]} {$addr != 0} {incr addr -2} {
	
	set blk [wtxMemRead $addr 2]
	set inst [expr [memBlockGet -w $blk]]
	memBlockDelete $blk
	if {(($inst & 0xffc0) == 0x4e80) || (($inst & 0xff00) == 0x6100)} {
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
# long word following the frame pointer pointed to by the frame pointer should
# be the return address of the function call.  Then the instruction preceding
# the return address would be the function call, and the address can be gotten
# from there, provided that the jsr was to an absolute address.  If it was,
# use that address as the function address.  Note that a routine that is
# called by other than a jsr-absolute (e.g., indirectly) will not meet these
# requirements.
#
# If the above check fails, we search backward from the given pc until a
# LINK instruction is found.  If the compiler is putting LINK instructions
# as the first instruction of ALL subroutines, then this will reliably find
# the start of the routine.  However, some compilers allow routines, especially
# "leaf" routines that don't call any other routines, to NOT have stack frames,
# which will cause this search to fail.
#
# In either of the above cases, the value is bounded by the nearest routine
# in the system symbol table, if there is one.  If neither method returns a
# legitimate value, then the value from the symbol table is used.  Note that
# the routine may not be in the symbol table if it is LOCAL, etc.
#
# Note that the start of a routine that is not called by jsr-absolute and
# doesn't start with a LINK and isn't in the symbol table, may not be possible
# to locate.
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
    global LINK_A6

    set minPc 0
    set symName "????"

    # try to find current function by looking up call

    # use value from symbol table that is <= pc as lower bound for function
    # start
    #

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

    #
    # search backward for LINK A6,#xxxx
    #

    set size [expr $pc - $minPc + 2]
    set blk [wtxMemRead $minPc $size]

    # get the instruction beginning by the end of the block

    for {set i [expr $size -2]} {$i >= 0} {incr i -2} {
 	set inst [expr [memBlockGet -w $blk $i 1]]
	switch {$inst} {
	    case 0x60?? {
		if {[lindex [trcFollowBra $inst $addr] 0] == $LINK_A6} {
		    memBlockDelete $blk
		    return [trcFuncStartName [expr $pc - ($size - $i - 2)]]
		}
	    }
	    case $LINK_A6 {
		memBlockDelete $blk
		return [trcFuncStartName [expr $pc - ($size - $i - 2)]]
	    }
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
# RETURNS: address to which call instruction (jsr) will branch, or NULL if
# 	   unknown
#

proc trcFindDest {callAddr} {
    global JSR_ABS

    set blk [wtxMemRead $callAddr 8]
    set inst [expr [memBlockGet -w $blk 0 1]]
    set dest [expr [memBlockGet -l $blk 4 1]]
    memBlockDelete $blk

    # jsr absolute long mode?

    if {$inst == $JSR_ABS} {
	return $dest
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
    global ADD_W
    global ADD_L
    global ADDQ_W
    global ADDQ_L
    global LEA_A7
    global MOVE_L_A7
    global MOVE_L_A6_A7
    
    set nArgs -1

    # if instruction is BRA use the target of the BRA as the return address

    set blk [wtxMemRead $retAddr 6]
    set inst [expr [memBlockGet -w $blk 0 1]]

    if {$inst & 0xff00 == 0x6000} {
	bindNamesToList {retAddr inst} [trcFollowBra $inst $retAddr]

	# we must read the word following the new return address

        memBlockDelete $blk
	set blk [wtxMemRead $retAddr 6]
    }

    if {$inst == $ADD_W} {
	set nArgs [expr [memBlockGet -w $blk 2 1] >> 2]
	memBlockDelete $blk
    } elseif {$inst == $ADD_L} {
	set nArgs [expr [memBlockGet -l $blk 2 1] >> 2]
	memBlockDelete $blk
    } elseif {$inst == $LEA_A7} {
	set nArgs [expr [memBlockGet -w $blk 2 1] >> 2]
	memBlockDelete $blk
    } elseif [expr ($inst & 0xffc0) == $MOVE_L_A7] {
	set nArgs 1
	memBlockDelete $blk
    } elseif {$inst == $MOVE_L_A6_A7} {
	set nArgs -1	; # number of args unknowable
	memBlockDelete $blk
    } else {
	memBlockDelete $blk

    	# there may be multiple addq's at return addr

	set argCount 0
	while {(($inst & 0xf1ff) == $ADDQ_W) || \
	       (($inst & 0xf1ff) == $ADDQ_L)} {

	    # get the number of bytes and div by 4 to get number of args

	    set tmpNArgs [expr ($inst & 0x0E00) >> 11]
	    if {$tmpNArgs == 0} {
		set tmpNArgs 2	; # 0 => 8 in quick mode 
	    }
	    incr argCount $tmpNArgs
	    incr retAddr 2;
	    set blk [wtxMemRead $retAddr 2]
	    set inst [expr [memBlockGet -w $blk]]
	    memBlockDelete $blk
	}
	if {$argCount != 0} {
	    set nArgs $argCount
	}
    }
    return $nArgs
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
    global RTS
    global LINK_A6

    set PEA_A6	0x4856
    set MOVEA_A7_A6 0x2c4f
    
    set regSet [regSetOfContext $ctype $cid]
    set pc [pcOfRegSet $regSet]
    set sp [lindex $regSet 15]
    set fp [lindex $regSet 14]
    bindNamesToList {prevInst inst inst2 retAddr1 retAddr2 retAddr3} \
		    [wtxGopherEval "$pc-2 {@w @w @w 0} $sp {@@0} $fp+4 {@ 0}"]
    
    #
    # if next instruction is a LINK (or PEA (A6); MOVEA A7,A6)or RTS, 
    # return address is on top of stack
    # otherwise it follows saved frame pointer
    #

    if {(($inst & 0xffff) == $RTS) || (($inst & 0xfff8) == $LINK_A6) ||
	((($inst & 0xffff) == $PEA_A6) && \
	 (($inst2 & 0xffff) == $MOVEA_A7_A6))} { 
	return $retAddr1
    } elseif { ((($prevInst & 0xffff) == $PEA_A6) && 
		(($inst & 0xffff) == $MOVEA_A7_A6))} {
	return $retAddr2
    } else {
	return $retAddr3
    }
}

##############################################################################
#
# dbgFuncCallCheck - check next instruction
# 
# This routine checks to see if the next instruction is a JSR or BSR.
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
#	1 if next instruction is JSR or BSR, or 0 otherwise.
#

proc dbgFuncCallCheck {addr} {
    global JSR
    global BSR
    
    set blk [wtxMemRead $addr 2]
    set inst [expr [memBlockGet -w $blk]]
    memBlockDelete $blk
    if {(($inst & 0xffc0) == $JSR) || (($inst & 0xff00) == $BSR)} {
	return 1
    } else {
	return 0
    }
}
