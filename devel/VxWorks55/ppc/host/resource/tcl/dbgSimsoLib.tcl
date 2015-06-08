# dbgSparcLib.tcl - sparc-specific debug routines Tcl implementation
#
# Copyright 1996-2001 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01k,20nov01,hbh  Fixed dbgRetAdrsGet() return address.
# 01j,15jul98,dbt  added some missing "$" in trcFindIndirectCall() code.
# 01i,12nov97,fle  put dbgNextAddrGet{} in shelcore.tcl as nextAddrGet{}
# 01h,26aug97,elp  added simsol support (SPR# 9262).
# 01g,10mar97,elp  use wtxRegsGet rather than wtxGopherEval to get valid
#		   register even during exception.
# 01f,03oct96,elp  added some missing \, fix a string comparison problem.
# 01e,06jun96,elp  made dbgRetAdrsGet{} work in sytem mode (SPR# 6659).
# 01d,02apr96,elp  changed wtxFuncCall in wtxDirectCall to get intVecBase.
# 01c,08mar96,elp  in trcFindFuncStart{}, made pc be exact address of inst.
# 01b,06mar96,elp  added debug routines and renamed,
#                  caught SVR_INVALID_FIND_REQUEST errors,
#		   added expr before each memBlockGet returning one value.
# 01a,19feb96,elp  written.
#

# DESCRIPTION
# /target/src/arch/sparc/trcLib.c inspired
# /target/src/arch/sparc/dbgArchLib.c inspired

# globals

set defArgNb		6	; # default number of argument

set intVecBase		0	; 
set windowFlushAddr	0	;

# regNb = 40 (38 registers + 2 holes for sparc and simsp) 
#	  43 (38 + 5 for simso)

set regNb [expr ([string compare $cpuFamily($__wtxCpuType) "simso"] == 0) \
		? 43 : 40]

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
# In order to do the trace, a number of assumptions are made.
# In general, the trace will work for all non-leaf C language routines,
# and for assembly language routines that contain some variation of
# the standard prologue and epilogue:
# .CS
#     sethi     %hi (ROUTINE_FRAME_SIZE), %g1
#     add       %g1, %lo (ROUTINE_FRAME_SIZE), %g1
#     save      %sp, %g1, %sp
#     ...
#     ret
#     restore
# .CE
# However, routines written in other languages, strange entries in
# routines, or tasks with corrupted stacks, can confuse the trace.
# Also, all parameters are assumed to be 32-bit quantities.
#
# On SPARC systems, if the routine changes its values of registers i0-i5
# between the time it is called and the time it calls the next level down
# (or, at the lowest level, the time the task is suspended), trcStack() will
# report the changed values.  It has no way to locate the original values.
#
# Also on SPARC systems, if you try to do a trcStack() of a routine between
# the time the routine is called, and the time its initial 'save' is done,
# you will see strange results.  Floating-point parameters are not shown.
#
# SYNOPSIS:
#	trcStack tid
# 
# PARAMETERS:
#	tid: task to trace
#
# ERRORS: N/A
#
# RETURNS: a list of trace lines
#          each trace line is a list of 4 elements :
#               {addr callFunc argNb {argList}}
#

proc trcStack {tid} {
    global endianFlag
    global windowFlushAddr
    global offset
    global regNb
    global cpuFamily
    global __wtxCpuType

    if {$windowFlushAddr != 0} {
	wtxFuncCall $windowFlushAddr	; # flush SPARC windows to stack
    }

    #
    # The most recent stack frame is normally left blank because the TCB
    # got a copy on context switch (a SPARC optimization).  trcStackLvl()
    # assumes that ALL stack frames are in the stack.  We need to
    # copy the input parameters (%i0-%i5), the frame pointer (%i6) and
    # the return address (%i7) to the stack from the TCB just in case an
    # overflow never occurred forced them to the stack.
    #
    
    # get pStackBase sp and pc

    set rBlk [wtxMemRead [expr $tid + $offset(WIND_TCB,pStackBase)] 4]
    set pStackBase [expr [memBlockGet -l $rBlk]]

    set regBlk [wtxRegsGet CONTEXT_TASK $tid REG_SET_IU 0 [expr $regNb * 4]]

    set pc [expr [memBlockGet -l $regBlk 0 1]]
    set sp [expr [memBlockGet -l $regBlk [expr ($regNb - 18) * 4] 1]]

    #
    # write the most recent frame
    # i0 offset in regBlk = (32 registers * 4 bytes)
    #

    set i0Pos 32
    set i0BlkOffset [expr (($regNb - 8) * 4)]
    wtxMemWrite $regBlk [expr $sp + $i0Pos] $i0BlkOffset $i0Pos
    memBlockDelete $regBlk

    return [trcStackLvl $sp $pc $pStackBase]
}

##############################################################################
#
# trcStackLvl - stack trace routine
#
# The C version indicates that at least 4 values are needed to find the trace.
# To increase performance we get these value with only one gopher string per
# level.
# The maximum recursion is limited to 40 to prevent garbage stacks from causing
# this routine to continue unbounded.
#
# SYNOPSYS:
#	trcStackLvl sp addr pStackBase
#
# PARAMETERS:
#	sp: content of sp register (%i6)
#	addr: address in the function
#	pStackBase: stack base
# 
# ERRORS: N/A
#
# RETURNS:
#

proc trcStackLvl {sp addrCalledFrom pStackBase} {
    global defArgNb	; # default arguments number
    global __wtxCpuType
    global cpuFamily

    set trcStack {}
    for {set depth 0} {($depth < 40) && (([expr $pStackBase] - 0x70) > $sp)} \
	{incr depth; set sp $nextSp; set addrCalledFrom $ip} {
	bindNamesToList {nextSp ip ipContents} \
		[wtxGopherEval "$sp +56 {@ 0} +4 {* ! {@ 0} 0}"]
	set funcName [trcFindFuncStart $sp $addrCalledFrom $ip $ipContents]
	if {($cpuFamily($__wtxCpuType) == "simsp") ||	\
	    ($cpuFamily($__wtxCpuType) == "simso") ||	\
	    (($cpuFamily($__wtxCpuType) == "sparc") && 	\
	     ([string compare $funcName "_vxTaskEntry"] != 0))} {
	    set level [list $ip  $funcName $defArgNb \
		       [trcPrintArgs [expr $sp + 32] -1]]
	    set trcStack [linsert $trcStack 0 $level]
	}
    }
    return $trcStack
}
    
##############################################################################
#
# trcFindFuncStart - find the starting address of a function
#
# This routine finds the starting address of a function by one of two ways.
# If the given frame pointer points to a legitimate frame pointer, then the
# long word following the frame pointer pointed to by the frame pointer should
# be the address the function call was called from.  Then the instruction at
# the address called from would be the function call, and the address can be
# gotten from there, provided that the CALL or JMPL was to an absolute address.
# If it was, use that address as the function address.  Otherwise, search 
# backward from the given pc until the begining of the function is found.
# Any questions?
#
# SYNOPSIS: trcFindFuncStart sp ip addr addrCalledFrom
#
# PARAMETERS:
#	sp: stack pointer (i6 register)
#	addrCalledFrom: address called from
#	ip: *(sp + I7_OFFSET)
#	ipContents: *(*(sp + I7_OFFSET))
#
# ERRORS: N/A
#
# RETURNS: the name of the calling function
#

proc trcFindFuncStart {sp addrCalledFrom ip ipContents} {
    global intVecBase

    set OP		0xc0000000
    set RD 		0x3e000000
    set RD_o7		0x1e000000
    set RD_g0		0x00000000
    set TRAP_NUMBER	0x7f
    set DISP30		0x3fffffff
    set DISP30_SHIFT_CT	2

    if {$ip} {
	
	# another valid address called from

	switch [format "0x%x" [expr $ipContents & $OP]] {
	    0x80000000 {
		# general opcode: arithmetic et. al.

		switch [format "0x%x" [expr $ipContents & 0x01f80000]] {
		    0x1c00000 {
			# jmpl %o7, aka call (reg or imm)

			if {($ipContents & $RD) == $RD_o7} {
			    set possibleAddr [trcFindIndirectCall \
					      $ipContents $sp]
			    if {$possibleAddr != -1} {
				return [trcFuncStartName $possibleAddr]
			    }
			} 
		    }
		    0x1d00000 {
			# ticc
			set possibleAddr [trcFindIndirectCall $ipContents $sp]
			return [trcFuncStartName \
				[expr (($intVecBase & 0xfffff000) | \
					   ((($possibleAddr & $TRAP_NUMBER) | 0x80) << 2))]]
		    }
		}
	    }
	    0x40000000 {
		# general opcode: call (disp30)

		return [trcFuncStartName [expr $ip + \
				(($ipContents & $DISP30) << $DISP30_SHIFT_CT)]]
	    }
	}
    }
    
    #
    # if there is a symbol table, use value from table
    # that's <= address called from
    # as a lower bound in search for function start
    #

    set minPc 0
    set symName "????"

    if [catch "wtxSymFind -value $addrCalledFrom" sym] {
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

    # set upper bound for loop count by adjusting minPc if unreasonable
    
    if {($minPc < ($addrCalledFrom - 0x2000)) || ($minPc == 0)} {
	set minPc [expr $addrCalledFrom - 0x2000]
	set symName "????"
    }

    # start backtracking, looking for a C-flavor prologue

    # get the instructions between minPc and addrCalledFrom + 8

    set size [expr ($addrCalledFrom + 8) - $minPc + 4]
    set blk [wtxMemRead $minPc $size]
    for {set i [expr $size - 12]} {$i >= 0} {incr i -4} {
	bindNamesToList {prev2Inst prevInst inst} [memBlockGet -l $blk $i 3]]
	
	# address of inst
	set pc [expr $addrCalledFrom + 8 - ($size - $i - 12)]

	if {($inst & ($OP + 0x01F80000)) == (0x80000000 + 0x01E00000)} {

	    #
	    # inst == SAVE
	    # to get an exact answer, look for one of three sequences:
	    # pc - 1: sethi    %hi (x), %ry
	    # pc    : save     %sp, %ry, %sp
	    #                  -- or --
	    # pc - 2: sethi    %hi (x), %ry
	    # pc - 1: add      %ry, %lo (x), %ry
	    # pc    : save     %sp, %ry, %sp
	    #                  -- or --
	    # pc - 2: sethi    %hi (x), %ry
	    # pc - 1: or       %ry, %lo (x), %ry
	    # pc    : save     %sp, %ry, %sp
	    # ...but in no case can the sethi be to %g0.
	    # {"SETHI", OP_0 + OP2_4,  OP + OP2,       itSethi},
	    # {"NOP",   OP_0 + RD_g0 + OP2_4,  OP + RD + OP2,  itNop},
	    # {"ADD",   OP_2 + OP3_00, OP + OP3,       itArithOrSet},
	    # {"OR",    OP_2 + OP3_02, OP + OP3,       itArithOrSet},
	    #
	    
	    if {(($prevInst & ($OP + 0x01c00000)) == 0x01000000) &&
		 (($prevInst & $RD) != $RD_g0)} {
		# return address of prevInst

		return [trcFuncStartName [expr $pc - 4]]
	    } else {
		if {(($prev2Inst & ($OP + 0x01c00000)) == 0x01000000) &&
		     (($prev2Inst & $RD) != $RD_g0)} {
		    if {(($prevInst & ($OP + 0xc0000000)) == 0x80000000) ||
		        (($prevInst & ($OP + 0xc0000000)) == 0x80100000)} {
		    	# return address of prev2Inst

		    	return [trcFuncStartName [expr $pc - 8]]
		    } else {
			# return address of instr

			return [trcFuncStartName $pc]
		    }
		} else {
		    # return address of instr

		    return [trcFuncStartName $pc]
		}
	    }
	}
    }
    memBlockDelete $blk

    #
    # found neither an address called from nor a SAVE intruction between
    # minPc and addrCalledFrom
    # return the nearest symbol or ????
    #

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
# trcFindIndirectCall  - attemps to figure out where we were called at
#
# This routine does some of the work to figure out where we were called
# from, given that the call used register indirection of some kind.
# Since we have the %i and %l registers in the stack, we'll go see
# if they were all that was involved.
#
# SYNOPSIS:
#
# PARAMETERS:
#
# ERRORS: N/A
#
# RETURNS: address that we were called at, or ERROR
#

proc trcFindIndirectCall {instr sp} {
    set RS1_SHIFT_CT	14

    # get rs1 contribution if any

    set trcFICtmp [expr $instr & 0x0007C000]
    if {$trcFICtmp == 0x0} {
	set calledAddr 0
    } else {
	# do the match involved

	switch [format "0x%x" $trcFICtmp] {
	    0x5c000 -
	    0x58000 -
	    0x54000 -
	    0x50000 -
	    0x4c000 -
	    0x48000 -
	    0x44000 -
	    0x40000 {
		set trcFICptr [expr $sp + 4 * \
			       (($trcFICtmp & 0x0001c000) >> $RS1_SHIFT_CT)]
		set blk [wtxMemRead $trcFICptr 4]
		set calledAddr [expr [memBlockGet -l $blk]]
		memBlockDelete $blk
	    }
	    0x7c000 -
	    0x78000 -
	    0x74000 -
	    0x70000 -
	    0x6c000 -
	    0x68000 -
	    0x64000 -
	    0x60000 {
		set trcFICptr [expr $sp + 0x20 + 4 * \
			       (($trcFICtmp & 0x0001c000) >> $RS1_SHIFT_CT)]
		set blk [wtxMemRead $trcFICptr 4]
		set calledAddr [expr [memBlockGet -l $blk]]
		memBlockDelete $blk
	    }
	    0x3c000 -
	    0x38000 -
	    0x34000 -
	    0x30000 -
	    0x2c000 -
	    0x28000 -
	    0x24000 -
	    0x20000 -
	    0x1c000 -
	    0x18000 -
	    0x14000 -
	    0x10000 -
	    0xc000 -
	    0x8000 -
	    0x4000 {
		return -1
	    }
	}
    }

    # get immediate value contribution 

    if {($instr & 0x00002000) == 0x00002000} {
	if {($instr & 0x00001fff) == 0x00001fff} {
	    incr calledAddr [expr ($instr & 0x00001fff) | 0xffffe000] 
	} else {
	    incr calledAddr [expr $instr & 0x00001fff]
	}
    } else {
	
	# get rs2 contribution if any

	if {($instr & 0x0000001f) == 0} {
	    # do the match involved

	    switch [format "0x%x" $trcFICtmp] {
		0x17 -
		0x16 -
		0x15 -
		0x14 -
		0x13 -
		0x12 -
		0x11 -
		0x10 {
		    set trcFICptr [expr $sp + (4 * ($trcFICtmp & 0x7))]
		    set blk [wtxMemRead $trcFICptr 4]
		    incr calledAddr [expr [memBlockGet -l $blk]]
		    memBlockDelete $blk
		}
		0x1f -
		0x1e -
		0x1d -
		0x1c -
		0x1b -
		0x1a -
		0x19 -
		0x18 {
		    set trcFICptr [expr $sp + 0x20 + (4 * ($trcFICtmp & 0x7))]
		    set blk [wtxMemRead $trcFICptr 4]
		    incr calledAddr [expr [memBlockGet -l $blk]]
		    memBlockDelete $blk
		}
		0xf -
		0xe -
		0xd -
		0xc -
		0xb -
		0xa -
		0x9 -
		0x8 -
		0x7 -
		0x6 -
		0x5 -
		0x4 -
		0x3 -
		0x2 -
		0x1 {
		    return -1
		}
	    }
	}
    }
    return $calledAddr
}

##############################################################################
# 
# trcSparcLib.tcl initialization code
#
# This code is executed when the file is sourced. So symbol researches needed
# to get stack trace are done only once.
# 

if [catch "wtxSymFind -name intVecBaseGet" msg] {
    if {[wtxErrorName $msg] == "SYMTBL_SYMBOL_NOT_FOUND"} {
	puts stdout "WARNING : can not get intVecBase"
    } else {
	error $msg
    }
} else {
    set intVecBase [wtxDirectCall [lindex $msg 1]]
}

if [catch "wtxSymFind -name windowFlush" msg] {
    if {[wtxErrorName $msg] != "SYMTBL_SYMBOL_NOT_FOUND"} {
	error $msg
    }
} else {
    set windowFlushAddr [lindex $msg 1]
}

##############################################################################
#
# dbgRetAdrsGet - get return address for current routine
#
# SYNOPSIS:
#       dbgRetAdrsGet ctype cid
#
# PARAMETERS:
#	ctype: context type
#       cid: context id
#
# ERRORS: N/A
#
# RETURNS: return address for current routine or -1 if it can not find one
#

proc dbgRetAdrsGet {ctype cid} {
    global offset
    global regNb

    set INST_CALL	0x40000000
    set JMPL_o7		0x9fc00000
    set INST_SAV        0x9de3b000

    set regSet [regSetOfContext $ctype $cid]
    set sp [lindex $regSet [expr $regNb - 20]]

    # get instruction in pc 
    
    set pc [lindex $regSet 0]
    set blk [wtxMemRead $pc 4]
    set inst [memBlockGet -l $blk]
    memBlockDelete $blk

    # if the instruction is like :
    # 9de3bxxx		save	%sp, 0xffffffxx, %sp
    # then return address is in o7 register not in i7
    
    if {($inst & 0xffeff000) == $INST_SAV } {
	set o7 [lindex $regSet [expr $regNb - 19]]
	return [expr $o7 + 8]
    }

    set blk [wtxMemRead [expr $sp + (0x0f * 4)] 4]
    set i7Contents [memBlockGet -l $blk]
    set retAddr [expr $i7Contents + 8]
    memBlockDelete $blk

    if {[catch {wtxMemRead $i7Contents 4} blk]} {
        puts stdout "cannot get return address for task [format "%#x" $cid]"
        return "done"
    }
    set inst [expr [memBlockGet -l $blk]]
    memBlockDelete $blk

    if {(($inst & 0xc0000000) == $INST_CALL) || 
	(($inst & 0xfff80000) == $JMPL_o7)} {
	return $retAddr
    } else {
        puts stdout "cannot get return address for task [format "%#x" $cid]"
        return "done"
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
#       dbgFuncCallCheck addr
#
# PARAMETERS:
#       addr: where to find instruction
#
# ERRORS: N/A
#
# RETURNS:
#       1 if next instruction is JSR or BSR, or 0 otherwise.
#

proc dbgFuncCallCheck {addr} {
    set INST_CALL	0x40000000
    set JMPL_o7		0x9fc00000
    
    set blk [wtxMemRead $addr 4]
    set inst [expr [memBlockGet -l $blk]]
    memBlockDelete $blk
    return [expr (($inst & 0xc0000000) == $INST_CALL) || \
		 (($inst & 0xfff80000) == $JMPL_o7)]
}
