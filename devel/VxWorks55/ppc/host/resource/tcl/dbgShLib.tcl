# dbgShLib.tcl - SH-specific debug routines Tcl implementation
#
# Copyright 1997-2000 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01m,05apr02,h_k  adjust reserved task params size (SPR #75153).
# 01l,06dec01,h_k  fixed trcInfoGet for the recursion depth is 0 and added
#                  Diab support (SPR #69837).
# 01k,14sep00,zl   updated WH breakpoint support for all SuperH CPUs.
# 01j,30may00,hk   revised trcStack/trcStackLvl/trcInfoGet for GCC 2.96.
# 01i,21apr00,rsh  properly implement dbgRetAdrsGet() a'la target shell
#                  implementation
# 01h,17apr00,rsh  fix a typo in trcStackLvl
# 01g,23jul99,zl   added HW breakpoint support for SH7750
# 01f,16jun99,zl   fixed dbgNextAddrGet to return address after the delay slot
#                  instruction
# 01e,12nov98,kab  added SH7410 hardware breakpoint support; H/W bp display.
# 01d,11nov98,jmb  implement automatic overwrite of existing HW BP.
# 01c,05oct98,mem  added hardware breakpoint support.
# 01b,26jun97,hk   improved, got basic idea from dbgMipsLib.tcl-01d.
# 01a,01may97,hk   written.
#

# DESCRIPTION
# /target/src/arch/sh/trcLib.c inspired
# /target/src/arch/sh/dbgArchLib.c inspired

# globals

set trcDebug		0		; # trace debugging on?

set prIndex		2		; # regNList (sh-sh.tcl)
set spIndex		20		;

set defArgNb		0		; # default argument number

set MAX_PROLOG_INSN	[expr 2 * 20]	;
set MAX_SCAN_DEPTH	[expr 2 * 2000]	; # search for proc start up to 2000
					  # instructions

set INST_PUSH_REG	0x2f06		; # mov.l  rm,@-r15
set MASK_PUSH_REG	0xff0f		; #

set INST_POP_REG	0x60f6		; # mov.l  @r15+,rm
set MASK_POP_REG	0xf0ff		; #

set INST_ADD_IMM_SP	0x7f00		; # add    #imm,r15
set MASK_ADD_IMM_SP	0xff00		; #

set INST_ADD_IMM_R14	0x7e00		; # add    #imm,r14
set MASK_ADD_IMM_R14	0xff00		; #

set INST_MOV_IMM16	0x9000		; # mov.w  @(disp,PC),rn
set MASK_MOV_IMM16	0xf000		; #

set INST_SUB_REG_SP	0x3f08		; # sub    rm,r15
set MASK_SUB_REG_SP	0xff0f		; #

set INST_ADD_REG_SP	0x3f0c		; # add    rm,r15
set MASK_ADD_REG_SP	0xff0f		; #

set INST_PUSH_FP	0x2fe6		; # mov.l  r14,@-r15
set INST_SET_FP		0x6ef3		; # mov    r15,r14
set INST_RESTORE_SP	0x6fe3		; # mov    r14,r15
set INST_POP_FP		0x6ef6		; # mov.l  @r15+,r14

set INST_PUSH_PR	0x4f22		; # sts.l  pr,@-r15
set INST_POP_PR		0x4f26		; # lds.l  @r15+,pr
set INST_RTS		0x000b		; # rts

# Instructions below are not used by trcInfoGet

set INST_JSR		0x400b		; # jsr    @rm
set MASK_JSR		0xf0ff		; #
set INST_BSRF		0x0003		; # bsrf   rm
set MASK_BSRF		0xf0ff		; #
set INST_BSR		0xb000		; # bsr    label
set MASK_BSR		0xf000		; #


# Set architecture help message

# Note. the help message limits the user to one, anything goes, 
# instruction or data type breakpoint (this corresponds to the first
# breakpoint type in hwBpTypeList below and is made available really
# only so that the list contains a type corresponding to when the
# user types "bh" in the windShell with no args i.e. default is 0x000)
# and three possible instruction
# type hardware breakpoints corresponding to "Inst read 16 cpu" using 
# the IBUS, XBUS or YBUS. These should be all that are needed. The user
# may, however, construct any number of data type hardware breakpoints.
# Thus, we present the individual bit pattern options. Furthermore, we
# don't allow the user to specifiy Instruction/Data (i.e. any access),
# although the hardware supports it, since it doesn't really confer any
# additional power and prohibiting it keeps the user interfaces significantly
# simpler and cleaner.

set archHelpMsg	"bh	  addr\[,access\[,task\[,count\]\]\]\
		Set hardware breakpoint\n\
		\t\taccess:\n\
		\t\t\t  - Break on instruction/data         0x000 \n\
		\t\t\t  - Break on instruction fetch        0x025 \n\
                \t\t\t  - Break on XBUS instruction fetch   0x125 \n\
                \t\t\t  - Break on YBUS instruction fetch   0x225 \n\
                \t\t\t                                            \n\
                \t\t\t  - Break on data access        (        10)\n\
                \t\t\t  - Bus cycle any               (      00  )\n\
                \t\t\t  - Bus cycle read              (      01  )\n\
                \t\t\t  - Bus cycle write             (      10  )\n\
                \t\t\t  - Operand size any            (    00    )\n\
                \t\t\t  - Operand size byte           (    01    )\n\
                \t\t\t  - Operand size word           (    10    )\n\
                \t\t\t  - Operand size long           (    11    )\n\
                \t\t\t  - CPU access                  (  00      )\n\
                \t\t\t  - DMAC access                 (  01      )\n\
                \t\t\t  - CPU or DMAC access          (  10      )\n\
                \t\t\t  - IBUS                        (00        )\n\
                \t\t\t  - XBUS                        (01        )\n\
                \t\t\t  - YBUS                        (10        )\n\
    \t*Not all access combinations are supported by all SuperH CPUs.\n\
    \t Use of an invalid combination is not always reported as an error.\n"

# Note. We initialize the hwBpTypeList here with the three possible 
# instruction type hardware breakpoints. The indicies 37, 293 and 549
# correspond to the hex values 0x025, 0x125 and 0x225 listed above,
# which, correspond to "Inst read 16 cpu" using the IBUS, XBUS or YBUS.
# This is to avoid presenting the user (Crosswind Breakpoint Type: GUI)
# with an overwhelming number of possible type options, most of which
# are redundant, meaningless or invalid. This reduces the list from 
# well over a thousand, to just over 100 options. Note that, we also
# do not present the user with Instruction/Data (any access) type options
# (see above note) or invalid options, thereby cleaning up the GUI
# interface.

set hwBpTypeList {{0   "hard-all"       "Inst/Data"}
                  {37  "hard-inst"      "Instruction"}
                  {293 "hard-inst xbus" "Instruction xbus"}
                  {549 "hard-inst ybus" "Instruction ybus"}}

# here, we step through all the possible types of breakpoints, instruction,
# data and invalid combinations, but are careful only to assemble a list
# of the "meaningful" data type breakpoints.

for {set i 0} {$i < 0x3ff} {incr i} {

    if {[expr $i & 0x03] == "2"} {
        set bpType "Data"
    } else {
        continue
    }

    set rwAccess [expr ($i & 0x0c) >> 2]
    switch $rwAccess {
	0	{ set rwAccess "r/w" }
	1	{ set rwAccess "rd" }
	2	{ set rwAccess "wr" }
        default	{ continue }
    }

    set size     [expr ($i & 0x30) >> 4]
    switch $size {
 	0	{ set size "8/16/32" }
	1	{ set size "8" }
	2	{ set size "16" }
	3	{ set size "32" }
    }

    set master   [expr ($i & 0xc0) >> 6]
    switch $master {
	0	{ set master "cpu" }
	1	{ set master "dma" }
	2	{ set master "c/d" }
        default	{ continue }
    }

    set bus      [expr ($i & 0x300) >> 8]
    switch $bus  {
	0	{ set bus "" }
	1	{ set bus "xbus" }
	2	{ set bus "ybus" }
        default	{ continue }
    }

    # set bpType "$rwAccess $size $master $bus"
    set bpType "$bus $master $size $rwAccess"

    set typeListElement  [list [format "%d" $i] \
                         [format "%s" "hard-data $bpType"] \
                         [format "%s" "Data $bpType"]]
    lappend hwBpTypeList $typeListElement
}

unset bpType rwAccess size master bus typeListElement


##############################################################################
#
# trcStack - print a trace of function calls from the stack
#
# This routine provides the low-level stack trace function.  A higher-level
# symbolic stack trace, built on top of trcStack(), is provided by tt() in
# dbgLib.
# 
# The routine prints a list of the nested routine calls that are on the stack,
# showing each routine call with its parameters.
#
# The stack being traced should be quiescent.  The caller should avoid tracing
# its own stack.
#
# SYNOPSYS:
#	trcStack tid
#
# PARAMETERS:
#	tid: task identifier
#
# ERRORS: N/A
#
# RETURNS: a list of trace line
#          each trace line is a list of 4 elements :
#               {addr callFunc argNb {argList}}
#

proc trcStack {tid} {
    global offset					; # sh-sh.tcl
    global spIndex

    # check TCB's EXC_INFO to see if we can trace the specified task.

    set EXC_PC		0x02				; # excShLib.h
    set EXC_ACCESS_ADDR	0x08				; # excShLib.h

    if [catch "wtxMemRead [expr $tid + $offset(WIND_TCB,excInfo)] 20" blk] {
	if {[wtxErrorName $blk] == "AGENT_MEM_ACCESS_ERROR"} {
	    puts "trcStack: failed to read EXC_INFO from TCB"
	    return -1
	} else {
	    error $blk
	}
    } else {
	set valid [expr [memBlockGet -w $blk  0 1]]
	set pc    [expr [memBlockGet -l $blk  4 1]]
	set info  [expr [memBlockGet -l $blk 12 1]]
	memBlockDelete $blk
    }

    if {($valid & $EXC_PC) && ($valid & $EXC_ACCESS_ADDR) && ($pc == $info)} {
	puts "cannot trace : i-fetch caused exception"
	return -1
    }

    # get the start address of stack (trace limit) to stackBottom.

    if [catch "wtxMemRead [expr $tid + $offset(WIND_TCB,pStackBase)] 4" tmp32] {
	if {[wtxErrorName $tmp32] == "AGENT_MEM_ACCESS_ERROR"} {
	    puts "trcStack: failed to read pStackBase from TCB"
	    return -1
	} else {
	    error $tmp32
	}
    } else {
	set stackBottom [expr [memBlockGet -l $tmp32]]
	memBlockDelete $tmp32
    }

    # must perform the trace by searching through code to determine stack
    # frames, and unwinding them to determine caller and parameters.
    # We subtract 40 from stackBottom for the ten (MAX_TASK_ARGS) task
    # params on stack that are tagged onto every task's main entry point.

    set regSet [regSetOfContext CONTEXT_TASK $tid]	; # sh-sh.tcl
    set pc     [pcOfRegSet $regSet]			; # sh-sh.tcl
    set sp     [lindex $regSet $spIndex]

    set trc [trcStackLvl [expr $stackBottom - 40] $sp $pc $regSet]

    return $trc
}

##############################################################################
#
# trcStackLvl - stack trace routine
#
# Loop and get trace information for each level. Maximum number of level is 20
#
# SYNOPSYS:
#	trcStackLvl stackBottom sp pc regSet
#
# PARAMETERS:
#	stackBottom: effective base of task's stack
#	sp: stack pointer
#	pc: content of pc register
#	regSet: register set
#
# ERRORS: N/A
#
# RETURNS: a list of trace lines
#	   each trace line is a list of 4 elements :
#		{addr callFunc argNb {argList}}
#

proc trcStackLvl {stackBottom sp pc regSet} {
    global trcDebug
    global prIndex
    global defArgNb

    for {set depth 0; set frameSize 0; set trcStack {}} \
	{($depth < 20) && ($sp <= $stackBottom)} \
	{incr depth; incr sp $frameSize; set pc $callPc} {

	bindNamesToList {callPc startPc frameSize} \
			[trcInfoGet $pc $sp [lindex $regSet $prIndex] $depth]

	if {$trcDebug > 0} {
	    puts [format "sp + %d = %#x, bottom %#x, margin %d" \
		  $frameSize [expr $sp + $frameSize] $stackBottom \
		  [expr $stackBottom - ($sp + $frameSize)]]
	}

	# stop the unwind if we can't determine the caller of this function

	if {$callPc == 0} {break}

	# only continue unwinding if this is not the last stack frame

	if {($sp + $frameSize) < $stackBottom} {

	    if {$startPc == 0} {

		# Unable to determine entry point from code... Try the symbol
		# table. Note that the symbol table may yield the wrong entry
		# point if the function name is not in it. In this case we
		# get the starting address for the previous function in the
		# text section and the trace would yield in strange results.

		if [catch "wtxSymFind -value $pc" sym] {
		    if {([wtxErrorName $sym] == "SVR_INVALID_FIND_REQUEST") ||
			([wtxErrorName $sym] == "SYMTBL_SYMBOL_NOT_FOUND")} {
			return
		    } else {error $sym}
		} else { set startPc [lindex $sym 1] }
	    }

	} else {

	    # Since this is the last frame, we can only determine the entry
	    # point via the symbol table

	    if [catch "wtxSymFind -value $pc" sym] {
		if {([wtxErrorName $sym] == "SVR_INVALID_FIND_REQUEST") ||
		    ([wtxErrorName $sym] == "SYMTBL_SYMBOL_NOT_FOUND")} {
		    return
		} else {error $sym}
	    } else { set startPc [lindex $sym 1] }

	    set level [list [expr $callPc - 4] [trcFuncStartName $startPc] \
			$defArgNb [trcPrintArgs [expr $sp + $frameSize] -1]]
	    set trcStack [linsert $trcStack 0 $level]
	    break
	}
	set level [list [expr $callPc - 4] [trcFuncStartName $startPc] \
			$defArgNb [trcPrintArgs [expr $sp + $frameSize] -1]]
	set trcStack [linsert $trcStack 0 $level]
    }
    return $trcStack
}

##############################################################################
#
# trcInfoGet - get address from which function was called
#
# Determines specific info about the current function, such as the
# address where it was called from, the stack frame size (if any) and
# whether the stack frame has already been allocated.
#
# SYNOPSIS:
#	trcInfoGet pc sp pr depth
#
# PARAMETERS:
#	pc: current pc
#	sp: stack pointer
#	pr: content of pr register
#	depth: scan depth (start from 0)
#
# ERRORS: N/A
#
# RETURNS:
#

proc trcInfoGet {pc sp pr depth} {
    global trcDebug
    global MAX_PROLOG_INSN MAX_SCAN_DEPTH
    global INST_PUSH_REG   MASK_PUSH_REG
    global INST_POP_REG    MASK_POP_REG
    global INST_ADD_IMM_SP MASK_ADD_IMM_SP
    global INST_ADD_IMM_R14 MASK_ADD_IMM_R14
    global INST_MOV_IMM16  MASK_MOV_IMM16
    global INST_SUB_REG_SP MASK_SUB_REG_SP
    global INST_ADD_REG_SP MASK_ADD_REG_SP
    global INST_PUSH_FP
    global INST_SET_FP
    global INST_RESTORE_SP
    global INST_POP_FP
    global INST_PUSH_PR
    global INST_POP_PR
    global INST_RTS

    set szSubFrameEpilog 0
    set prStoreCheck 0
    set inEpilog 0
    set checkDiab 0

    # If the recursion depth is 0, it is likely to be in a prolog code unless
    # it's in a epilog code. Scan forward up to MAX_PROLOG_INSN instructions,
    # or until finding an epilog instruction.

    if {$depth == 0} {
	if [catch "wtxMemRead $pc $MAX_PROLOG_INSN" scanBlk] {
	    if {[wtxErrorName $scanBlk] == "AGENT_MEM_ACCESS_ERROR"} {
		puts "trcInfoGet: forward memory block access error"
		return "0 0 0"
	    } else {
		error $scanBlk
	    }
	}

	set scanBlkEnd [expr $pc + $MAX_PROLOG_INSN]

	if {$trcDebug > 0} {
	    puts [format "<forward scan #%d> sp %#x, pc %#x, upto %#x" \
		  $depth $sp $pc [expr $scanBlkEnd - 2]]
	}

	for {set pscan $pc} {$pscan < $scanBlkEnd} {incr pscan 2} {

	    set ix [expr $pscan - $pc]
	    set insn [expr [memBlockGet -w $scanBlk $ix 1]]

	    if {($insn == $INST_PUSH_FP) || \
		($insn == $INST_PUSH_PR) || \
		($insn == $INST_SET_FP)} {

		if {$trcDebug > 0} {
		    puts -nonewline [format "%x  %04x    " $pscan $insn]

		    if     {$insn == $INST_PUSH_FP} {puts "mov.l r14,@-r15"} \
		    elseif {$insn == $INST_PUSH_PR} {puts "sts.l pr,@-r15"} \
		    elseif {$insn == $INST_SET_FP}  {puts "mov   r15,r14"}

		    puts [format "<forward scan #%d> pc %#x in prolog, pr %#x" \
			  $depth $pscan $pr]
		}

		set retAddr $pr

		# get the closest global function address from current pc

		if [catch "wtxSymFind -value $pc" sym] {
		    if {([wtxErrorName $sym] == "SVR_INVALID_FIND_REQUEST") || \
			([wtxErrorName $sym] == "SYMTBL_SYMBOL_NOT_FOUND")} {
			return "$retAddr 0 0"
		    } else { error $sym }
		} else {
		    set symName [lindex $sym 0]
		    set pSymAhead [lindex $sym 1]
		    set symType [lindex $sym 2]
		}

		set scanBlkStart [expr $pc - $MAX_SCAN_DEPTH]
		set scanBlkSize $MAX_SCAN_DEPTH

		if {$pSymAhead >= $scanBlkStart} {
		    set scanBlkStart $pSymAhead
		    set scanBlkSize [expr $pc - $pSymAhead]
		}

		if [catch "wtxMemRead $scanBlkStart $scanBlkSize" scanBlk] {
		    if {[wtxErrorName $scanBlk] == "AGENT_MEM_ACCESS_ERROR"} {
			puts "trcInfoGet: backward memory block access error"
			return "$retAddr 0 0"
		    } else {
			error $scanBlk
		    }
		}

		set szNvRegs 0
		set szFrame 0
		set frameReg -1

		# Scan back the prolog code until beginning of the function.

		for {set pscan [expr $pc - 2]} \
		    {$pscan >= $scanBlkStart} \
		    {incr pscan -2} {
		    set ix [expr $pscan - $scanBlkStart]
		    set insn [expr [memBlockGet -w $scanBlk $ix 1]]

		    if {($insn & ($MASK_ADD_IMM_SP | 0x80)) == \
			($INST_ADD_IMM_SP | 0x80)} {

			set imm8 [expr 0 - ((1 + ~[expr $insn & 0xff]) & 0xff)]

			if {$trcDebug > 0} {
			    puts [format "%x  %04x    add   #%d,r15" \
				$pscan $insn $imm8]
			}

			incr szFrame [expr 0 - $imm8]

		    } elseif {($insn & $MASK_SUB_REG_SP) == $INST_SUB_REG_SP} {

			set frameReg [expr ($insn & 0x00f0) >> 4]

			if {$trcDebug > 0} {
			    puts [format "%x  %04x    sub   r%d,r15"
				$pscan $insn $frameReg]
			    }

		    } elseif {($frameReg != -1) && \
			(($insn & $MASK_MOV_IMM16) == $INST_MOV_IMM16) && \
			(($insn & 0x0f00) == ($frameReg << 8))} {

			set disp [expr ($insn & 0x00ff) << 1]

			if [catch "wtxMemRead [expr $pscan + 4 + $disp] 2" \
				tmp16] {
			    if {[wtxErrorName $tmp16] == \
				"AGENT_MEM_ACCESS_ERROR"} {
				puts "trcInfoGet: failed to get imm16" \
				return "$retAddr 0 0"
			    } else {
				error $tmp16
			    }
			} else {
			    set imm16 [expr [memBlockGet -w $tmp16]]
			    memBlockDelete $tmp16
			}

			if {$trcDebug > 0} {
			    puts [format "%x  %04x    mov.w @(%#x,pc),r%d \
				(= %#x)" $pscan $insn $disp $frameReg $imm16]
			    puts [format "<backward scan #%d> frame size %d" \
				$depth $imm16]
			}

			set szFrame $imm16
			
		    } elseif {$insn == $INST_PUSH_PR} {

			if {$trcDebug > 0} {
			    puts [format "%x  %04x    sts.l pr,@-r15" \
				$pscan $insn]
			}

			set szNvRegs 4

		    } elseif {($insn & $MASK_PUSH_REG) == $INST_PUSH_REG} {

			set rm [expr ($insn & 0x00f0) >> 4]

			if {($rm >= 8) && ($rm <= 14)} {

			    if {$trcDebug > 0} {
				puts [format "%x  %04x    mov.l r%d,@-r15" \
				    $pscan $insn $rm]
			    }

			incr szNvRegs 4

			}

			if {$rm == 8} {
			    set entry $pscan
			    break
			}
		    }
		}

		return "$retAddr 0 [expr $szNvRegs + $szFrame]"

	    } elseif {($insn == $INST_RESTORE_SP) || \
		      ($insn == $INST_POP_PR) || \
		      ($insn == $INST_POP_FP) || \
		      ($insn == $INST_RTS)} {

		if {$trcDebug > 0} {
		    puts -nonewline [format "%x  %04x    " $pscan $insn]

		    if {$insn == $INST_RESTORE_SP} {puts "mov   r14,r15"} \
		    elseif {$insn == $INST_POP_PR} {puts "lds.l @r15+,pr"} \
		    elseif {$insn == $INST_POP_FP} {puts "mov.l @r15+,r14"} \
		    elseif {$insn == $INST_RTS}    {puts "rts"}

		    puts [format "<forward scan #%d> found epilog at %#x" \
			  $depth $pscan]
		}
		break
	    }
	}

	if {$trcDebug > 0} {
	    puts [format "<forward scan #%d> pc %#x not in prolog" $depth $pc]
	}

	memBlockDelete $scanBlk
    }

    # get the closest global function address from current pc

    if [catch "wtxSymFind -value $pc" sym] {
	if {([wtxErrorName $sym] == "SVR_INVALID_FIND_REQUEST") ||
	    ([wtxErrorName $sym] == "SYMTBL_SYMBOL_NOT_FOUND")} {
	    return "0 0 0"
	} else { error $sym }
    } else {
	set symName [lindex $sym 0]
	set pSymAhead [lindex $sym 1]
	set symType [lindex $sym 2]
    }

    set scanBlkStart [expr $pc - $MAX_SCAN_DEPTH]
    set scanBlkSize $MAX_SCAN_DEPTH

    if {$pSymAhead > $scanBlkStart} {
	set scanBlkStart $pSymAhead
	set scanBlkSize [expr $pc - $pSymAhead]
    }

    if [catch "wtxMemRead $scanBlkStart $scanBlkSize" scanBlk] {
	if {[wtxErrorName $scanBlk] == "AGENT_MEM_ACCESS_ERROR"} {
	    puts "trcInfoGet: backward memory block access error"
	    return "0 0 0"
	} else {
	    error $scanBlk
	}
    }

    # We now know that the pc is NOT in a function prolog. Scan backward and
    # determine whether it is a C or an assembly function. If this is GNU C,
    # we should first find 'mov r15,r14' at the end of prolog. If this is
    # Diab C, we should first find 'add -imm,r15', 'sts.l pr,@-r15' or
    # 'mov.l r14,@-r15'. Keep track of stack operations otherwise, to get the
    # called address of this assembly function.

    if {$trcDebug > 0} {
	puts [format "<backward scan #%d> sp %#x, pc %#x, upto %#x(%s:%#x)" \
	      $depth $sp $pc $pSymAhead $symName $symType]
    }

    for {set szFrame 0
	 set pscan [expr $pc - 2]} {$pscan >= $scanBlkStart} {incr pscan -2} {

	set ix [expr $pscan - $scanBlkStart]
	set insn [expr [memBlockGet -w $scanBlk $ix 1]]

	if {$insn == $INST_SET_FP} {

	    break;	# found 'mov r15,r14', this must be a GNU C code.

	} elseif {($insn & $MASK_PUSH_REG) == $INST_PUSH_REG} {

	    set rm [expr ($insn & 0x00f0) >> 4]

	    set ix [expr ($pscan -2) - $scanBlkStart]
	    set insnNext [expr [memBlockGet -w $scanBlk $ix 1]]

	    if {($rm == 14) && ($insnNext == ($INST_PUSH_REG | 0xd0))} {
		set checkDiab 1
		break;	# found 'mov r13,@-r15' 'mov r14,@-r15'
	    } else {

		if {$trcDebug > 0} {
		    puts [format "%x  %04x    mov.l r%d,@-r15" $pscan $insn $rm]
		}

		incr szFrame 4
	    }

	} elseif {$insn == $INST_PUSH_PR} {

	    set ix [expr ($pscan - 2) - $scanBlkStart]
	    set insnNext [expr [memBlockGet -w $scanBlk $ix 1]]

	    if {$insnNext == ($INST_PUSH_REG | 0xe0)} {
		set checkDiab 1
		break;	# found 'mov r14,@-r15' 'sts.l pr,@-r15'
	    } else {

		if {$trcDebug > 0} {
		    puts [format "%x  %04x    sts.l pr,@-r15" $pscan $insn]
		}

		if [catch "wtxMemRead [expr $sp + $szFrame] 4" tmp32] {
		    if {[wtxErrorName $tmp32] == "AGENT_MEM_ACCESS_ERROR"} {
			puts "trcInfoGet: failed to get pr on stack (asm)"
			return "0 0 0"
		    } else {
			error $tmp32
		    }
		} else {
		    set retAddr [expr [memBlockGet -l $tmp32]]
		    memBlockDelete $tmp32
		}

		return "$retAddr 0 [expr $szFrame + 4]"
	    }

	} elseif {($insn & $MASK_ADD_IMM_SP) == $INST_ADD_IMM_SP} {

	    set ix [expr ($pscan - 2) - $scanBlkStart]
	    set insnNext [expr [memBlockGet -w $scanBlk $ix 1]]

	    if {($insnNext == ($INST_PUSH_REG | 0xe0)) || \
		($insnNext == $INST_PUSH_PR)} {
		set checkDiab 1
		break;	# found 'add imm,r15'
			# 'mov r14,@-r15' or 'sts.l pr,@-r15'
	    }

	} elseif {($depth == 0) && ($insn == $INST_RESTORE_SP)} {

	    set inEpilog 1
	    break

	} elseif {($depth == 0) && ($insn == $INST_POP_PR)} {

	    set inEpilog 1

	} elseif {($depth == 0) && \
		(($insn & $MASK_POP_REG) == ($INST_POP_REG | 0xe00))} {

	    set inEpilog 1

	}
    }

    # Here we know that this is a C function, and the pc is NOT in prolog.
    # Scan backward again until finding the end of prolog (mov r15,r14 or
    # sts.l @r15+,pr) to see if a sub-stack frame is constructed after the
    # prolog code.

    set pscan [expr $pc - 2]

    if {$inEpilog == 1} {
	if {$trcDebug > 0} {
	    puts [format "<backward scan #%d> pc %#x in epilog" $depth $pc]
	}

	for {set foundAddFrame 0
	     set frameReg -1} {$pscan >= $scanBlkStart} {incr pscan -2} {

	    set ix [expr $pscan - $scanBlkStart]
	    set insn [expr [memBlockGet -w $scanBlk $ix 1]]

	    if {($insn & $MASK_ADD_IMM_SP) == $INST_ADD_IMM_SP} {

		set imm8 [expr $insn & 0xff]

		if {$trcDebug > 0} {
		    puts -nonewline [format "%x  %04x    add   #%d,r15" \
			$pscan $insn $imm8]
		}

		if {$foundAddFrame == 0} {
		    set foundAddFrame 1

		    if {$imm8 > 0} {

			if {$trcDebug > 0} {puts " (add)"}
			set szSubFrameEpilog [expr 0 - $imm8]

		    } else { if {$trcDebug > 0} {puts " (skip)"} }

		} else { if {$trcDebug > 0} {puts " (skip)"} }
	    } elseif {$insn == $INST_RESTORE_SP} {

		if {$trcDebug > 0} {
		    puts [format "%x  %04x    mov   r14,r15" $pscan $insn]
		}

		incr pscan -2
		break;		# found epilog start, proceed to next step
	    } elseif {($insn & $MASK_ADD_REG_SP) == $INST_ADD_REG_SP} {

		set frameReg [expr ($insn & 0x00f0) >> 4]

		if {$trcDebug > 0} {
		    puts [format "%x  %04x    add   r%d,r15" $pscan $insn \
				$frameReg]
		}

	    } elseif {($frameReg != -1) && \
		(($insn & $MASK_MOV_IMM16) == $INST_MOV_IMM16) && \
		(($insn & 0x0f00) == $frameReg << 8)} {

		set disp [expr ($insn & 0x00ff) << 1]

		if [catch "wtxMemRead [expr $pscan + 4 + $disp] 2" tmp16] {
		    if {[wtxErrorName $tmp16] == "AGENT_MEM_ACCESS_ERROR"} {
			puts "trcInfoGet: failed to get imm16"
			return "0 0 0"
		    } else {
			error $tmp16
		    }
		} else {
		    set imm16 [expr [memBlockGet -w $tmp16]]
		    memBlockDelete $tmp16
		}

		if {$trcDebug > 0} {
		    puts [format "%x  %04x    mov.w @(%#x,pc),r%d (= %#x)" \
			$pscan $insn $disp $frameReg $imm16]
		    puts [format "<backward scan #%d> frame size %d" $depth \
			$imm16]
		}

		set szSubFrameEpilog [expr 0 - $imm16]

	    } elseif {$insn == $INST_POP_PR} {

		if {$trcDebug > 0} {
		    puts [format "%x  %04x    lds.l @r15+,pr" $pscan $insn]
		}

		incr szSubFrameEpilog -4
		incr prStoreCheck -1

		if {$checkDiab == 1} {
		    incr pscan -2
		    break;
		}

	    } elseif {($insn & $MASK_POP_REG) == $INST_POP_REG} {

		set rm [expr ($insn & 0x0f00) >> 8]

		if {($rm >= 8) && ($rm <= 14)} {

		    if {$trcDebug > 0} {
			puts [format "%x  %04x    mov.l @r15+,r%d" $pscan \
				$insn $rm]
		    }

		    incr szSubFrameEpilog -4
		}

		if {($rm == 14) && ($checkDiab)} {

		    set ix [expr ($pscan - 2) - $scanBlkStart]
		    set insnNext [expr [memBlockGet -w $scanBlk $ix 1]]

		    if {$insnNext != $INST_POP_PR} {
			incr pscan -2
			break
		    }
		}
	    }
	}
    }

    # Scan back until the end of the prolog.

    for {set foundSubFrame 0; set foundAddFrame 0; set szSubFrame 0
	 } {$pscan >= $scanBlkStart} {incr pscan -2} {

	set ix [expr $pscan - $scanBlkStart]
	set insn [expr [memBlockGet -w $scanBlk $ix 1]]

	if {($insn & $MASK_ADD_IMM_SP) == $INST_ADD_IMM_SP} {

	    set imm8 [expr $insn & 0xff]

	    if {$imm8 & 0x80} {set imm8 [expr 0 - ((1 + ~$imm8) & 0xff)]}

	    if {$trcDebug > 0} {
		puts -nonewline [format "%x  %04x    add   #%d,r15" \
				 $pscan $insn $imm8]
	    }

	    # We are in a sub stack frame if this is the first 'add #imm,r15'
	    # instruction and #imm is negative.  Skip otherwise.

	    if {$foundSubFrame == 0} {
		set foundSubFrame 1

		if {$imm8 < 0} {

		    if {$trcDebug > 0} {puts " (sub)"}
		    set szSubFrame [expr 0 - $imm8]

		} else { if {$trcDebug > 0} {puts " (skip)"} }

	    } else { if {$trcDebug > 0} {puts " (skip)"} }

	} elseif {(($insn & $MASK_ADD_IMM_R14) == $INST_ADD_IMM_R14) && \
		($inEpilog == 1) && ($checkDiab == 0)} {

	    set imm8 [expr $insn & 0xff]

	    if {$trcDebug > 0} {
		puts -nonewline [format "%x  %04x    add   #%d,r14" \
			$pscan $insn $imm8]
	    }

	    # We are in a add stack frame if this is the first 'add #imm,r14'
	    # instruction and #imm is positive.  Skip otherwise.

	    if {$foundAddFrame == 0} {
		set foundAddFrame 1

		if {$imm8 > 0} {

		    if {$trcDebug > 0} {puts " (add)"}
		    incr szSubFrameEpilog [expr 0 - $imm8]

		} else { if {$trcDebug > 0} {puts " (skip)"} }

	    } else { if {$trcDebug > 0} {puts " (skip)"} }

	} elseif {(($insn & $MASK_PUSH_REG) == $INST_PUSH_REG) && \
			($checkDiab == 1)} {
	    set rm [expr ($insn & 0x00f0) >> 4]

	    if {($rm >=8) && ($rm <=14)} {
		if {$inEpilog == 1} {
		    set szSubFrame $szSubFrameEpilog
		    break
		}
	    }

	} elseif {($insn == $INST_PUSH_PR) && ($checkDiab == 1)} {

	    if {$inEpilog == 1} {
		set szSubFrame $szSubFrameEpilog
	    }

	    break

	} elseif {$insn == $INST_SET_FP} {

	    if {$trcDebug > 0} {
		puts [format "%x  %04x    mov   r15,r14" $pscan $insn]
	    }

	    if {$inEpilog == 1} {
		set szSubFrame $szSubFrameEpilog
	    }

	    incr pscan -2
	    break;		# found prolog, proceed to next step
	}
    }

    # Scan backward the rest of prolog code.

    for {set entry 0; set szNvRegs 0; set szFrame 0; set frameReg -1
	 } {$pscan >= $scanBlkStart} {incr pscan -2} {

	set ix [expr $pscan - $scanBlkStart]
	set insn [expr [memBlockGet -w $scanBlk $ix 1]]

	if {($insn & ($MASK_ADD_IMM_SP | 0x80)) == ($INST_ADD_IMM_SP | 0x80)} {

	    set imm8 [expr 0 - ((1 + ~[expr $insn & 0xff]) & 0xff)]

	    if {$trcDebug > 0} {
		puts [format "%x  %04x    add   #%d,r15" $pscan $insn $imm8]
	    }

	    incr szFrame [expr 0 - $imm8]

	} elseif {($insn & $MASK_SUB_REG_SP) == $INST_SUB_REG_SP} {

	    set frameReg [expr ($insn & 0x00f0) >> 4]

	    if {$trcDebug > 0} {
		puts [format "%x  %04x    sub   r%d,r15" $pscan $insn $frameReg]
	    }

	} elseif {($frameReg != -1) && \
		  (($insn & $MASK_MOV_IMM16) == $INST_MOV_IMM16) && \
		  (($insn & 0x0f00) == $frameReg << 8)} {

	    set disp [expr ($insn & 0x00ff) << 1]

	    if [catch "wtxMemRead [expr $pscan + 4 + $disp] 2" tmp16] {
		if {[wtxErrorName $tmp16] == "AGENT_MEM_ACCESS_ERROR"} {
		    puts "trcInfoGet: failed to get imm16"
		    return "0 0 0"
		} else {
		    error $tmp16
		}
	    } else {
		set imm16 [expr [memBlockGet -w $tmp16]]
		memBlockDelete $tmp16
	    }

	    if {$trcDebug > 0} {
		puts [format "%x  %04x    mov.w @(%#x,pc),r%d (= %#x)" \
		      $pscan $insn $disp $frameReg $imm16]
		puts [format "<backward scan #%d> frame size %d" $depth $imm16]
	    }

	    set szFrame $imm16

	} elseif {$insn == $INST_PUSH_PR} {

	    if {$trcDebug > 0} {
		puts [format "%x  %04x    sts.l pr,@-r15" $pscan $insn]
	    }

	    set szNvRegs 4
	    incr prStoreCheck 1

	} elseif {($insn & $MASK_PUSH_REG) == $INST_PUSH_REG} {

	    set rm [expr ($insn & 0x00f0) >> 4]

	    if {($rm >= 8) && ($rm <= 14)} {

		if {$trcDebug > 0} {
		    puts [format "%x  %04x    mov.l r%d,@-r15" $pscan $insn $rm]
		}

		incr szNvRegs 4
	    }

	    if {$rm == 8} {
		set entry $pscan
		break
	    }
	}
    }

    # get the return address on stack

    if [catch "wtxMemRead [expr $sp + $szFrame + $szSubFrame] 4" tmp32] {
	if {[wtxErrorName $tmp32] == "AGENT_MEM_ACCESS_ERROR"} {
	    puts "trcInfoGet: failed to get return address"
	    return "0 0 0"
	} else {
	    error $tmp32
	}
    } else {
	if {($depth == 0) && ($prStoreCheck == 0)} {
	    set retAddr $pr
	} else {
	    set retAddr [expr [memBlockGet -l $tmp32]]
	}

	memBlockDelete $tmp32
    }

    return "$retAddr $entry [expr $szNvRegs + $szFrame + $szSubFrame]"
}

##############################################################################
#
# trcFuncStartName - return the name of the calling function
#
# SYNOPSIS: trcFuncStartName addr
#
# PARAMETER:
#	addr: address of the symbol about which we want the name
#
# ERRORS: N/A
#
# RETURNS:
#	the name of the symbol if <addr> is the address of a symbol
#	the address if a symbol is found but with a different address
#	???? when symbol search fails
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

##############################################################################
#
# dbgRetAdrsGet - get return address for current routine
#
# SYNOPSIS:
#	dbgRetAdrsGet ctype cid
#
# PARAMETERS:
#	ctype: context type
#	cid: context id
#
# ERRORS: N/A
#
# RETURNS: return address for current routine
#
# Note: This routine assumes the user is debugging c source and may fail
#       if debugging assembly code.


proc dbgRetAdrsGet {ctype cid} {
    global spIndex
    global prIndex
    global MAX_SCAN_DEPTH
    global INST_PUSH_PR
    global INST_POP_PR
    global INST_JSR
    global MASK_JSR
    global INST_RTS
    global INST_SET_FP
    global INST_ADD_IMM_SP
    global MASK_ADD_IMM_SP
 
    set regSet [regSetOfContext $ctype $cid]
    set pc [pcOfRegSet $regSet]

    #
    # scan instructions forward. If we find a "sts.l pr,@-sp" 
    # then the return address is already in register "pr".  If we find
    # a "lds.l @sp+,pr" then the return address is saved on the stack.
    # If we find "rts" without encountering the above instructions, it is
    # a leaf function and the return address is in register "pr".
    #
 
    set n 0
    set scanback 0
    while {($n < 10) && (! $scanback)} {
        set scanBlk [wtxMemRead [expr $pc + ($n * $MAX_SCAN_DEPTH)] \
                     $MAX_SCAN_DEPTH]
        for {set i 0} {$i < $MAX_SCAN_DEPTH} {incr i 2} {
            set inst [expr [memBlockGet -w $scanBlk $i 1]]
            if {($inst == $INST_PUSH_PR) ||
                ($inst == $INST_RTS) } {
 
                # match "sts.l pr,@-sp" means return address in pr,
                # also match "rts" means return address in pr (leaf procedure).
 
                set retAddr [lindex $regSet $prIndex]
                memBlockDelete $scanBlk
                return $retAddr
            } elseif {$inst == $INST_POP_PR} {
 
                # match "lds.l @sp+,pr" means return address is on the stack.

                set scanback 1
                break
            }
        }
        if {(! $scanback)} {
            memBlockDelete $scanBlk
            incr n
        }
    }

    # did we fail to find the INST_POP_PR? if so, just single step

    # if {(! $scanback) && ($n == 10)} { return [expr $pc + 2] }
    if {(! $scanback)} { 
        return [expr $pc + 2] 
    }

    # if we arrive here, we're somewhere inside the function body and
    # the current tcb's pr value may be invalid (i.e. we may have called
    # a subroutine within the current function body thereby causing the
    # pr to be modified. Consequently, we'll need to search backwards
    # looking for the frame pointer store instruction as well as any
    # framepointer adjustment instruction that may have been applied
    # before the frame pointer was stored. Since we are in the body of
    # the function, we use a similar approach to that above except that
    # we use negative offsets from $pc to calculate the address of the
    # memblock to read.

    set n 1
    set found 0
    while {($n <= 10) && (! $found)} {
        set scanBlk [wtxMemRead [expr $pc - ($n * $MAX_SCAN_DEPTH)] \
                     $MAX_SCAN_DEPTH]
        for {set i [expr $MAX_SCAN_DEPTH - 2]} {$i >= 0} {set i [expr $i - 2]} {
            set inst [expr [memBlockGet -w $scanBlk $i 1]]
            if {($inst == $INST_SET_FP)} {
                set found 1
                break 
            } 
        }
        if {(! $found)} {
            memBlockDelete $scanBlk
            incr n
        }
    }

    # did we fail to find the INST_SET_FP? if so, just single step

    if {(! $found)} { 
        return [expr $pc + 2] 
    }

    # we've found the INST_SET_FP. Now we only need to search for any
    # offset instruction (search until we find the INST_PUSH_PR, but
    # no further), retrieve any offset and add it to the pushed pr value.  
    # Note that i still points to the INST_SET_FP instruction and we are
    # reading from the same memblock as above. Note that the framepointer
    # value is stored in the task's register set. Consequently, we do not
    # need to retrieve it from the stack. We need only find the frame
    # pointer restore instruction so that we know where to begin looking
    # for an adjustment instruction and then retrieve, from the stack, 
    # any possible adjusment value as well as the return value itself,
    # once we've figured out where it actually is located on the stack. 
    
    set found 0
    set fp [lindex $regSet 19]

    set i [expr $i - 2]

    while {($n <= 10) && (! $found)} {
        for {} {$i >= 0} {set i [expr $i - 2]} {
            set inst [expr [memBlockGet -w $scanBlk $i 1]]
            if {(($inst & $MASK_ADD_IMM_SP) == $INST_ADD_IMM_SP)} {

                # extract the #immed offset from the instruction and add
                # it to the frame pointer before retrieving the frame pointer
                # value. But first! the 'add #imm,sp' instruction sign extends 
                # the #imm value before adding it to sp. Since the prologue 
                # decrements sp, #imm will be negative and we need to, likewise,
                # sign extend it to get it's proper negative value. Then we can
                # reverse its sign and add it to the frame pointer.
                  
                set immed [expr 0 - (0xffffff00 | ($inst & 0xff))]
                set fp [expr $fp + $immed]

                # now retrieve the return address.

                set stackBlk [wtxMemRead $fp 4]
                set retAddr [memBlockGet -l $stackBlk 0 1]

                return $retAddr

            } elseif {$inst == $INST_PUSH_PR} {
                set found 1
                break 
            } 
        }
        if {(! $found)} {
            memBlockDelete $scanBlk
            incr n
            set scanBlk [wtxMemRead [expr $pc - ($n * $MAX_SCAN_DEPTH)] \
                     $MAX_SCAN_DEPTH]
            set i [expr $MAX_SCAN_DEPTH - 2]
        }
    }

    # If we found INST_PUSH_PR, no adjustment was made to the frame pointer.
    # In this case, the frame pointer stored in the TCB's regSet points to
    # the location of the return address. Just retrieve the memory value 
    # pointed to by the regSet's frame pointer. Otherwise, just single step.

    if {($found)} { 
        set stackBlk [wtxMemRead $fp 4]
        set retAddr [memBlockGet -l $stackBlk 0 1]
        return $retAddr
    } else {
        return [expr $pc + 2]
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
    global INST_JSR
    global MASK_JSR
    global INST_BSRF
    global MASK_BSRF
    global INST_BSR
    global MASK_BSR

    set blk [wtxMemRead $addr 2]
    set inst [expr [memBlockGet -w $blk]]
    memBlockDelete $blk
    return [expr (($inst & $MASK_JSR)  == $INST_JSR)  || \
		 (($inst & $MASK_BSRF) == $INST_BSRF) || \
		 (($inst & $MASK_BSR)  == $INST_BSR)]
}
