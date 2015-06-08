# dbgPpcLib.tcl - ppc-specific Tcl implementation 
#
# Copyright 1996-1998 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01i,16may02,pch  Add support for PPC440
# 01h,01nov00,s_m  added support for PPC405
# 01g,07feb99,dbt  completed hardware breakpoints description.
# 01f,19oct98,elg  added hardware breakpoints for PPC603 and PPC604
# 01e,28jul98,elg  added hardware breakpoints
# 01d,12nov97,fle  put dbgNextAddrGet{} in shelcore.tcl as nextAddrGet{}
# 01c,25sep96,jmp  fixed a syntax error in wtxEnumFromString calls
# 01b,06jun96,elp  made dbgRetAdrsGet{} work in sytem mode (SPR# 6659).
# 01a,25mar96,elp  written.
#

# DESCRIPTION
# /target/src/arch/ppc/trcLib.c inspired
# /target/src/arch/ppc/dbgArchLib.c inspired

# globals

set defArgNb		0		; # default argument number
set spIndex		1		; # index of sp register
set lrIndex		33		; # index of lr register
set MAX_PROLOGUE_SIZE	[expr 20 * 4]	; # prologue size (20 instructions)
set MAX_SCAN_DEPTH	[expr 250 * 4]	; # search for proc start up to 250
					  # instructions
set INST_B		0x48000000	; # some ppc instructions
set INST_BCLR		0x4c000020
set INST_STWU_SP	0x94210000
set INST_MTLR		0x7c0803a6
set INST_LWZ_SP		0x80010000
set INST_LWZU_SP	0x84010000
set INST_ADDI_SP	0x38210000

# Set architecture help message for hardware breakpoints, and
# export hwBpTypeList to Crosswind.tcl and shellDbgCmd.tcl
# WindSH and CrossWind will support hardware breakpoints only
# on processors for which hwBpTypeList is set.

# Support here should be coordinated with _archHelp_msg and
# _dbgBrkDisplayHard in target/src/arch/ppc/dbgArchLib.c, and
# with the documentation in host/src/tgtsvr/server/wtx.pcl

switch -regexp $cpuType($__wtxCpuType) {
    PPC603 -
    PPCEC603 {
        set archHelpMsg	"bh	  addr\[,access\[,task\[,count\]\]\]\
			Set hardware breakpoint\n\
			\t\taccess:\t0 - instruction\n";
	set hwBpTypeList {
	    {"0"	"(hard-inst.)"	"Instruction"}
	};
    }
    PPC604 -
    PPC440 -
    PPC860 {
        set archHelpMsg	"bh	  addr\[,access\[,task\[,count\]\]\]\
			Set hardware breakpoint\n\
			\t\taccess:\t0 - instruction\t\t1 - read/write data\n\
			\t\t\t2 - read data\t\t3 - write data\n";
	set hwBpTypeList {
	    {"0"	"(hard-inst.)"		"Instruction"}
	    {"1"	"(hard-r/w data)"	"Data read/write"}
	    {"2"	"(hard-read data)"	"Data read"}
	    {"3"	"(hard-write data)"	"Data write"}
	};
    }
    PPC403 {
        set archHelpMsg	"bh	  addr\[,access\[,task\[,count\]\]\]\
			Set hardware breakpoint\n\
			\t\taccess:\t0 - instruction\t\t1 - write byte\n\
			\t\t\t2 - read byte\t\t3 - read/write byte\n\
			\t\t\t5 - write half-word\t6 - read half-word\n\
			\t\t\t7 - r/w half-word\t9 - write word\n\
			\t\t\t10 - read word\t\t11 - read/write word\n\
			\t\t\t13 - write quad-word\t14 - read quad-word\n\
			\t\t\t15 - read/write quad-word\n";
	set hwBpTypeList {
	    {"0"	"(hard-inst.)"		"Instruction"}
	    {"1"	"(hard-write byte)"	"Data write byte"}
	    {"2"	"(hard-read byte)"	"Data read byte"}
	    {"3"	"(hard-r/w byte)"	"Data read/write byte"}
	    {"5"	"(hard-write half)"	"Data write half-word"}
	    {"6"	"(hard-read half)"	"Data read half-word"}
	    {"7"	"(hard-r/w half)"	"Data read/write half-word"}
	    {"9"	"(hard-write word)"	"Data write word"}
	    {"10"	"(hard-read word)"	"Data read word"}
	    {"11"	"(hard-r/w word)"	"Data read/write word"}
	    {"13"	"(hard-write 4-word)"	"Data write quad-word"}
	    {"14"	"(hard-read 4-word)"	"Data read quad-word"}
	    {"15"	"(hard-r/w 4-word)"	"Data read/write quad-word"}
	};
    }
    PPC405 -
    PPC405F {
        set archHelpMsg	"bh	  addr\[,access\[,task\[,count\]\]\]\
			Set hardware breakpoint\n\
			\t\taccess:\t0 - instruction\t\t1 - write byte\n\
			\t\t\t2 - read byte\t\t3 - read/write byte\n\
			\t\t\t4 - write half-word\t5 - read half-word\n\
			\t\t\t6 - r/w half-word\t7 - write word\n\
			\t\t\t8 - read word\t\t9 - read/write word\n\
			\t\t\t10 - write cache-line\t11 - read cache-line\n\
			\t\t\t12 - read/write cache-line\n";
	set hwBpTypeList {
	    {"0"	"(hard-inst.)"		"Instruction"}
	    {"1"	"(hard-write byte)"	"Data write byte"}
	    {"2"	"(hard-read byte)"	"Data read byte"}
	    {"3"	"(hard-r/w byte)"	"Data read/write byte"}
	    {"4"	"(hard-write half)"	"Data write half-word"}
	    {"5"	"(hard-read half)"	"Data read half-word"}
	    {"6"	"(hard-r/w half)"	"Data read/write half-word"}
	    {"7"	"(hard-write word)"	"Data write word"}
	    {"8"	"(hard-read word)"	"Data read word"}
	    {"9"	"(hard-r/w word)"	"Data read/write word"}
	    {"10"	"(hard-write cache-line)"	"Data write 8-word (cache-line)"}
	    {"11"	"(hard-read cache-line)"	"Data read 8-word (cache-line)"}
	    {"12"	"(hard-r/w cache-line)"	"Data read/write 8-word (cache-line)"}
	};
    }
}

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
# This routine scans forward and backward from current pc to find out return
# address. The maximum number of inst scanned is (MAX_SCAN_DEPTH/4). If a 
# routine has more than (MAX_SCAN_DEPTH/4) instructions, then trace may be 
# wrong.
# There is no way to determine the function arguments unless the
# code is compiled with debugging (e.g., "-gdwarf).
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
    global offset
    global spIndex
    global lrIndex
    global MAX_SCAN_DEPTH

    set blk [wtxMemRead [expr $tid + $offset(WIND_TCB,pStackBase)] 12]
    bindNamesToList {pStackBase pStackLimit pStackEnd} [memBlockGet -l $blk]
    memBlockDelete $blk

    set regSet [regSetOfContext CONTEXT_TASK $tid]
    set pc [pcOfRegSet $regSet]
    set sp [lindex $regSet $spIndex]

    if {(($sp % 8) || ($sp == 0) || ($sp > $pStackBase) || \
	 ($sp < $pStackEnd))} {
	puts stdout "trcStack aborted: error in top frame (sp)"
	return -1
    }

    if [catch "wtxMemRead $pc [expr $MAX_SCAN_DEPTH + 4]" scanBlk] {
	if {[wtxErrorName $scanBlk] == "AGENT_MEM_ACCESS_ERROR"} {
	    set frameAlloc 1
	    set returnPcOnStack 1
	} else {
	    error $inst
	}
    } else {
	bindNamesToList {frameAlloc returnPcOnStack returnPcInRx rx} \
			[trcInfoGet $scanBlk $pc $regSet]
    }
	
    if {$frameAlloc} {
	set blk [wtxMemRead $sp 4]
	set fp [expr [memBlockGet -l $blk]]
	memBlockDelete $blk
	if {($fp % 8) || ($fp < $sp) || ($fp > $pStackBase) || ($fp == 0)} {
	    puts stdout "trcStack aborted: error in top frame (fp)"
	    return -1
	}
    } else {
	set fp $sp
	set sp [expr $sp - 8]
    }

    if {$returnPcOnStack} {
	set blk [wtxMemRead [expr $fp + 4] 4]
	set returnPc [expr [memBlockGet -l $blk]]
	memBlockDelete $blk
    } elseif {$returnPcInRx} {
	set returnPc [lindex $regSet $rx]
    } else {
	set returnPc [lindex $regSet $lrIndex]
    }
    if [catch "wtxMemRead $returnPc 4" blk] {
	if {[wtxErrorName $blk] == "AGENT_MEM_ACCESS_ERROR"} {
	    puts stdout "trcStack aborted: error on top frame (lr)"
	    return -1
	} else {
	    error $blk
	}
    } else {
	memBlockDelete $blk
    }

    # do the stack trace
    
    set trc [trcStackLvl $pc $returnPc $sp $fp $pStackBase]
    
    return $trc
}

#############################################################################
#
# trcInfoGet - find out where is the returnPc and if frame is allocated
#
# SYNOPSIS:
#	trcInfoGet blk pc regSet
#
# PARAMETERS:
#	blk: memory block read from pc
#	pc: program counter
#	regSet: list of register values
#
# ERRORS: N/A
#
# RETURNS: a list a 3 booleans and a register used if returnPc is in a register
#

proc trcInfoGet {scanBlk pc regSet} {
    global lrIndex
    global MAX_SCAN_DEPTH
    global MAX_PROLOGUE_SIZE
    global INST_BCLR
    global INST_STWU_SP
    global INST_MTLR
    global INST_LWZ_SP
    global INST_LWZU_SP

    set INST_MFLR	0x7c0802a6
    set INST_STW_RX_dR1	0x90010000

    # set default values

    set frameAlloc 1
    set returnPcOnStack 1
    set returnPcInRx 0
    set rx 0
    
    # scan forward from current pc to see how the link register is used

    for {set i 0} {$i <= $MAX_SCAN_DEPTH} {incr i 4} {
	set inst [expr [memBlockGet -l $scanBlk $i 1]]

	# mfspr lr,rx => in proc prologue with return PC in link register

	if {($inst & 0xfc1fffff) == $INST_MFLR} {
	    set returnPcOnStack 0
	    set procAddr [trcFindFuncStart $pc [lindex $regSet $lrIndex]]
	    set frameAlloc [trcProlFrameAlloc $procAddr $MAX_PROLOGUE_SIZE \
			    fwd $pc 0xffff0000 $INST_STWU_SP]
	    break
	}

	# bclr => in proc epilogue with return PC in link register

	if {($inst & 0xfc00ffff) == $INST_BCLR} {
		
	    # look backward for a "stwu" if found we are in an
	    # epilogue and the frame is allocated otherwise we are in a 
	    # function where frame is not allocated

	    set returnPcOnStack 0
	    if {$i == 0} {
		# we are executing the last instruction
		set frameAlloc 0
		break
	    }
	    set procAddr [trcFindFuncStart $pc [lindex $regSet $lrIndex]]
	    set frameAlloc [trcProlFrameAlloc $pc $MAX_SCAN_DEPTH bwd \
			    $procAddr 0xffff0000 $INST_STWU_SP]
	    break
	}

	# mtspr lr, rx => return PC in link register
	# where we are (at pc) retPc is no more in lr 
	# it can be in the stack or in a register if we are in prologue.

	if {($inst & 0xfc1fffff) == $INST_MTLR} {
		
	    # try to scan backward to find "mfspr rx,LR"
	    # if between this inst and the pc we found "stw rx, d(r1)" then
	    # returnPc is on stack otherwise it is still in rx

	    if [catch "wtxSymFind -value $pc" sym] {
		if {([wtxErrorName $sym] == "SYMTBL_SYMBOL_NOT_FOUND") ||
		    ([wtxErrorName $sym] == "SVR_INVALID_FIND_REQUEST")} {
		    set procAddr [expr $pc - $MAX_SCAN_DEPTH]
		} else {
		    error $sym
		}
	    } else {
		set procAddr [lindex $sym 1]
		if {($pc - $procAddr) > $MAX_SCAN_DEPTH} {
		    set procAddr [expr $pc - $MAX_SCAN_DEPTH]
		}
	    }

	    set bwdBlk [wtxMemRead [expr $pc - $MAX_SCAN_DEPTH] \
	    		$MAX_SCAN_DEPTH]
	    for {set j [expr $MAX_SCAN_DEPTH - 4]} \
		{($j > 0) && (($pc + $MAX_SCAN_DEPTH - $j) >= $procAddr)} \
		{incr j -4} {
		set inst [expr [memBlockGet -l $bwdBlk $j 1]]
		if {($inst & 0xfc1fffff) == $INST_MFLR} {
		    set rx [expr $inst & 0x03e00000]
		    set frameAlloc 0
		    set returnPcOnStack 0
		    set returnPcInRx 1
		    for {set k $j} {$k < $MAX_SCAN_DEPTH} {incr k 4} {
			set inst [expr [memBlockGet -l $bwdBlk $k 1]]
			if {($inst & 0xffff0000) == ($INST_STW_RX_dR1 | $rx)} {
			    set returnPcOnStack 1
			    set returnPcInRx 0
			    break 
			}
		    }
		}
		if {($inst & 0xffff0000) == $INST_STWU_SP} {
		    set frameAlloc 1
		    break
		}
		if {($inst & 0xfc00ffff) == $INST_BCLR} {
		    break
		}
	    }
	    memBlockDelete $bwdBlk
	    break
	}

	# "stwu r1, rx" (but no mtspr lr, rx)

	if {($inst & 0xffff0000) == $INST_STWU_SP} {
	    set returnPcOnStack 0
	    set frameAlloc 0
	    break
	}
    }
    memBlockDelete $scanBlk
    return "$frameAlloc $returnPcOnStack $returnPcInRx $rx"
}

##############################################################################
#
# trcStackLvl - stack trace routine
#
# Loop and get trace information for each level. Maximun number of level is 40
#
# SYNOPSYS:
#	trcStackLevel pc returnPc sp fp pStackBase
#
# PARAMETERS:
#	pc: content of pc register
#	returnPc: return address
#	sp: stack pointer
#	fp: frame pointer
#	pStackBase: effective base of task stack
# 
# ERRORS: N/A
#	
# RETURNS: a list of trace lines
#	   each trace line is a list of 4 elements :
#		{addr callFunc argNb {argList}}
#

proc trcStackLvl {pc returnPc sp fp pStackBase} {
    
    set trcStack {}
    set blk [wtxMemRead $fp 4]
    set nextFp [expr [memBlockGet -l $blk]]
    memBlockDelete $blk

    for {set depth 0} {$depth < 40} \
	{incr depth; 
	 set pc $returnPc;
	 set returnPc $nextReturnPc;
	 set sp $fp;
	 set fp $nextFp;
	 set nextFp $nNextFp} {

	set blk [wtxMemRead $nextFp 8]
	bindNamesToList {nNextFp nextReturnPc} [memBlockGet -l $blk]
	memBlockDelete $blk

	if {($nextFp %8) || ($nextFp < $fp) || ($nextFp > $pStackBase) ||
	    ($nextFp == 0)} {
	    break
	} 
	if [catch "wtxMemRead $nextReturnPc 4" blk] {
	    if {[wtxErrorName $blk] == "AGENT_MEM_ACCESS_ERROR"} {
		break
	    } else {
		error $blk
	    }
	} else {
	    memBlockDelete $blk
	}

	set procAddr [trcFindFuncStart $pc $returnPc]
	bindNamesToList {nArg argList} [trcGetArgs $procAddr $sp]

	set level [list $returnPc [trcFuncStartName $procAddr] \
			$nArg $argList]
	set trcStack [linsert $trcStack 0 $level]

#puts [format "pc=0x%x, returnPc=0x%x, sp=0x%x, fp=0x%x, \
	      pStackbase=0x%x" $pc $returnPc $sp $fp $pStackBase]
#puts [format "nextFp=0x%x, nextReturnPc=0x%x" $nextFp $nextReturnPc]

    }

    return $trcStack
}

##############################################################################
#
# trcProlFrameAlloc - check if stack frame is allocated
#
# SYNOPSIS:
#	trcProlFrameAlloc addr dir end instrMask instr
#
# PARAMETERS:
#	addr: where the search begins
#	depth: number of instruction we scan
#	dir: direction (fwd= forward, bwd= backward)
#	end: where to stop the search
#	instrMask: mask the current instruction before comparison
#	instr: instruction to find
#
# ERRORS: N/A
#
# RETURNS: TRUE if instruction is found false otherwise
#

proc trcProlFrameAlloc {addr depth dir end instrMask instr} {
    
    set blk [wtxMemRead [expr [string compare $dir fwd] ? \
			 ($addr - $depth) : $addr] \
	     $depth]
    set i [expr [string compare $dir fwd] ? ($depth - 4): 0]
    for {} {[string compare $dir fwd] ? ($i > 0) : ($i < $depth)}\
	{incr i [expr [string compare $dir fwd] ? -4 : 4]} {

	set curInst [expr [memBlockGet -l $blk $i 1]]
	if {($curInst & $instrMask) == $instr} {
	    memBlockDelete $blk
	    return 1
	}
	if [string compare $dir fwd] {
	    if {($addr - $depth + $i) <= $end} {
		break
	    }
	} else {
	    if {($addr + $i) <= $end} {
		break
	    }
	}
    }
    memBlockDelete $blk
    return 0
}

##############################################################################
#
# trcFindFuncStart - find the starting address of a function
#
# Given a "pc" value, determine the address of the procedure
# containing that pc.
# We try several methods until one succeeds.
# 1) First check the returnPc to see if we got to the current proc
# via a branch instruction. If so we can determine the procedure
# address accuratly.
# 2) If there is no branch instruction found, scan backwards up to
# MAX_SCAN_DEPTH bytes looking for a "stwu sp, xxx(sp)" instruction,
# and assume that instruction is the proc entry.
#
# SYNOPSIS:
#	trcFindFuncStart pc returnPc
#
# PARAMETERS:
#	pc: content of pc register
#	returnPc: return address
#
# ERRORS: N/A
#
# RETURNS: the starting address of the function
#

proc trcFindFuncStart {pc returnPc} {
    global MAX_SCAN_DEPTH
    global MAX_PROLOGUE_SIZE
    global INST_STWU_SP
    global INST_B
    global INST_BCLR

    set INST_BC	0x40000000

    set blk [wtxMemRead [expr $returnPc - 4] 4]
    set inst [expr [memBlockGet -l $blk]]
    memBlockDelete $blk

    if {(($inst & 0xfc000000) == $INST_B) ||
	(($inst & 0xfc000000) == $INST_BC)} {
	
	# extract address from instruction and sign extend

	if {($inst & 0xfc000000) == $INST_B} {
	    set branchAddr [expr $inst & 0x03fffffc]
	    if {$branchAddr & 0x02000000} {
		set branchAddr [expr $branchAddr | 0xfc000000]
	    }
	} else {
	    set branchAddr [expr $inst & 0xfffc]
	    if {$branchAddr & 0x8000} {
		set branchAddr [expr $branchAddr | 0xffff0000]
	    }
	}

	# if branch is not absolute, add in relative address

	if {! (($inst & 0x2) >> 1)} {
	    set branchAddr [expr $branchAddr + $returnPc - 4]
	}
	return $branchAddr
    }
    
    # if that fails, try to scan backwards for "stwu sp, xxx(sp)"

    if [catch "wtxMemRead [expr $pc - $MAX_SCAN_DEPTH] \
			  [expr $MAX_SCAN_DEPTH + 4]" blk] {
	if {[wtxErrorName $blk] != "AGENT_MEM_ACCESS_ERROR"} {
	    error $blk
	}
    } else {
	for {set i $MAX_SCAN_DEPTH} {$i > 0} {incr i -4}  {
	    set curInst [expr [memBlockGet -l $blk $i 1]]
	    if {($curInst & 0xffff0000) == $INST_STWU_SP} {
		memBlockDelete $blk
		return [expr $pc - $MAX_SCAN_DEPTH + $i]
	    }
	}
	memBlockDelete $blk
    }

    # if all else fails, return pc as a guess
    # stack frame is not allocated since we did not find the "allocate frame"
    # instruction.

    return $pc
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

############################################################################
#
# trcGetArgs - find number of arguments to function
#
# XXX - No way to do this if the code is not compiled with "-g"
# because the power PC calling conventions do not require
# argumenst to be pushed on the stack.
# This routine is based on disassembled GNU code, which contains
# the following sequence near the proc prologue:
#
#     mfspr        r0, LR               # standard prologue
#     stw          r31, 0xfffc(r1)      # standard prologue
#     stw          r0, 0x4(r1)          # standard prologue
#     stwu         r1, 0xffc0(r1)       # standard prologue
#     or           r31, r1, r1          # r31 = stack pointer
#     stw          r3, 0x18(r31)        # save arg0 0x18 bytes from sp
#     stw          r4, 0x1c(r31)        # save arg1 0x1c bytes from sp
#     stw          r5, 0x20(r31)        # save arg2 0x1c bytes from sp
#       ...
#
# So we scan the function prologue looking for this pattern,
# and if we find them we copy the args from the stack.
# The ABI says nothing about this, so GNU or another compiler vendor
# can change this at any time and break this stack tracer.
#
# SYNOPSIS:
#	trcGetArgs addr
#
# PARAMETERS:
#	procAddr: function start address
#	sp: stack pointer
#
# ERRORS: N/A
#
# RETURNS: number of arguments of function or default number
#
proc trcGetArgs {procAddr sp} {
    global MAX_PROLOGUE_SIZE
    global INST_B
    global INST_BCLR
    
    set argList {}
    set nArgs 0

    set blk [wtxMemRead $procAddr $MAX_PROLOGUE_SIZE]
    for {set i 0} {$i < $MAX_PROLOGUE_SIZE} {incr i 4} {
	set inst [expr [memBlockGet -l $blk $i 1]]
	if {($inst & 0xffe00000) == ((36 << 26) | (($nArgs + 3) << 21))} {
	    incr nArgs
	    set argValBlk [wtxMemRead [expr $sp + ($inst & 0xffff)] 4]
	    append argList " [expr [memBlockGet -l $argValBlk]]"
	    memBlockDelete $argValBlk
	    if {$nArgs >= 10} {
		break
	    }
	}
	if {(($inst & 0xfc000000) == $INST_B) ||
	    (($inst & 0xfc00ffff) == $INST_BCLR)} {
	    break
	}
    }
    memBlockDelete $blk
    if {$nArgs == 0} {
	return "-1 {}"
    } else {
	return "$nArgs $argList"
    }
} 

##############################################################################
#
# dbgRetAdrsGet - get return address for current routine
#
# As it impossible to reliably determine return address from anywhere in a 
# routine, we assume this function is called only by cret{}. Thus, as the goal 
# is to jump over the current routine, we run until the end and the then get
# reliably the return address.
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
# RETURNS: return address for current routine or "done" if the "blr" instruction
#	   can not be hit
#

proc dbgRetAdrsGet {ctype cid} {
    global lrIndex
    global MAX_SCAN_DEPTH
    global INST_BCLR

    set regSet [regSetOfContext $ctype $cid]
    set pc [pcOfRegSet $regSet]

    # scan for "blr"

    set n 0
    set found 0
    while {($n < 10) && (! $found)} {
	set scanBlk [wtxMemRead [expr $pc + ($n * $MAX_SCAN_DEPTH)] \
		     $MAX_SCAN_DEPTH]
	for {set i 0} {$i < $MAX_SCAN_DEPTH} {incr i 4} {
	    set inst [expr [memBlockGet -l $scanBlk $i 1]]
	    if {($inst & 0xfc00ffff) == $INST_BCLR} {
		set addr [expr $pc + ($n * $MAX_SCAN_DEPTH) + $i]
		set found 1
		break
	    }
	}
	memBlockDelete $scanBlk
	incr n
    }
    if {! $found} {
	# "blr" not found then single step
	return [nextAddrGet $pc]
    } else {
	# put a breakpoint, wait for that breakpoint to be hit, then return
	# the address in lr which is the return address

	set action [expr [wtxEnumFromString ACTION_TYPE ACTION_STOP] | \
			 [wtxEnumFromString ACTION_TYPE ACTION_NOTIFY]]
	set epNum [wtxEventpointAdd WTX_EVENT_TEXT_ACCESS $addr \
		   $ctype $cid $action 0 0 0]
	
	wtxContextCont $ctype $cid
	while 1 {
	     set event [wtxEventPoll 300]
	     if {![string compare [lindex $event 0] "TEXT_ACCESS"]
		 && ([lindex $event 1] == $cid ||
		     [lindex $event 1] == 0xffffffff)
		 && [lindex $event 2] == 
		    [wtxEnumFromString CONTEXT_TYPE $ctype]} {

		if {([lindex $event 3] == $addr)} {
	 	    set regSet [regSetOfContext $ctype $cid]
		    wtxEventpointDelete $epNum
		    return [lindex $regSet $lrIndex]
		} else {
		    shEventDispatch $event
		    wtxEventpointDelete $epNum
		    return "done"
		}
		break
	     } else {
		shEventDispatch $event
	     }
	}
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

    set INST_BL		0x48000001
    set INST_BCL	0x40000001
    
    set blk [wtxMemRead $addr 4]
    set inst [expr [memBlockGet -l $blk]]
    memBlockDelete $blk
    return [expr (($inst & 0xfc000001) == $INST_BL) || \
		 (($inst & 0xfc000001) == $INST_BCL)]
}
