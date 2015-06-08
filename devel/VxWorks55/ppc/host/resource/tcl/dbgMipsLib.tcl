# dbgMipsLib.tcl - mips-specific Tcl implementation 
#
# Copyright 1996-1998 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01p,22may02,pgh  Fix SPR 74002 and 73020.
# 01o,29jun01,mem  Rewrite to improve backtrace.
# 01n,28jun01,mem  Update backtracing for new toolchain.
# 01m,05jan01,mem  Initial changes for MIPS32/MIPS64
# 01l,15aug00,dra  Fix numerous MIPS16 merge problems.
# 01k,30sep99,myz  modified trcInfoGet and others for CW4000_16 support.
# 01j,23dra00,dra  Fixed spIndex and raIndex, added raOffset.
# 01i,20jan99,dra  Added CW4000, CW4011, VR4100, VR5000 and VR5400 support.
# 01h,07feb99,dbt  completed hardware breakpoints description.
# 01g,25mar98,dbt  fixed a problem in trcStackLvl.
# 01f,10mar98,dbt  added architecture specific help message and support for
#                  hardware breakpoints.
# 01e,13nov97,fle  moving dbgNextAddrGet{} from every dbgXxxxLib.tcl file to
#                  nextAddrGet{} in shelcore.tcl, since wtxMemDisassemble is
#                  CPU agnostic
# 01d,14jan97,kkk  added little endian, fixed trace debug.
# 01c,19nov96,kkk  added R4650, fixed cret() when encounter lw ra,x(sp).
# 01b,13aug96,elp  changed R4000 index values.
# 01a,25jun96,elp  written.
#

# DESCRIPTION
# /target/src/arch/mips/trcLib.c inspired
# /target/src/arch/mips/dbgArchLib.c inspired

# globals

set spIndex		35		; # index of sp register
set raIndex		37		; # index of lr register
set raOffset		0

switch -regexp $cpuType($__wtxCpuType) {
    MIPS32 {
	set _RTypeSize		4		; # 4 bytes (32 bits)
    }
    MIPS64 {
        switch [targetEndian] {
            BIG_ENDIAN {
		set raOffset	4		; # index of ra register
            }
	}
	set _RTypeSize		8		; # 8 bytes (64 bits)
    }
}

# Set architecture help message
# Hardware breakpoints are only supported by R4650
# processors.

set archHelpMsg		"bh        addr\[,access\[,task\[,count\]\]\]\
			Set hardware breakpoint\n\
			\t\taccess:\t0 - instruction\t1 - write\n\
			\t\t\t2 - read\t3 - read/write\n";

set hwBpTypeList {
    {"0"	"(hard-inst.)"		"Instruction"}
    {"1"	"(hard-write)"		"Data write"}
    {"2"	"(hard-read)"		"Data read"}
    {"3"	"(hard-read/write)"	"Data read/write"}}


set trcDebug		0		  ; # trace debugging on?
set defArgNb		4		; # default argument number

# search for proc start up to 8000 instructions, the JVM intrepreter
# loop (and some other codes), can be quite large.
set MAX_SCAN_DEPTH	[expr 4 * 8000]

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
    global MAX_SCAN_DEPTH
    global spIndex
    global trcDebug

    set EXC_ACCESS_ADDR 0x0004
    set EXC_EPC		0x0002

    set blk [wtxMemRead [expr $tid + $offset(WIND_TCB,excInfo)] 0x20]
    set valid [expr [memBlockGet -w $blk 0 1]]
    bindNamesToList {epc badVa} [memBlockGet -l $blk 2 2]
    memBlockDelete $blk

    if {($valid & $EXC_ACCESS_ADDR) && 
	($valid & $EXC_EPC) &&
	($badVa == $epc)} {
	puts stdout "cannot trace : i-fetch caused exception"
	return -1
    }
    set blk [wtxMemRead [expr $tid + $offset(WIND_TCB,pStackBase)] 4]
    set stackBottom [expr [memBlockGet -l $blk]]
    memBlockDelete $blk

    #
    # must perform the trace by searching through code to determine stack
    # frames, and unwinding them to determine caller and parameters.
    # We substract 40 from stackBottom for the reserved task params that are 
    # tagged onto every task's main entry point.
    #

    set regSet [regSetOfContext CONTEXT_TASK $tid]
    set pc [pcOfRegSet $regSet]
    set sp [lindex $regSet $spIndex]

    if {$trcDebug > 0} {
        puts stdout [format "0: stackBot 0x%x, sp 0x%x, pc 0x%x" [expr $stackBottom - 40] $sp $pc]
    }

    # The end code "jr ra or jr a3" in the task main function(infinite loop)
    # may be removed due to compiler optimization. So we stop tracing at there
    # by subtracting the stack frame for the main routine from stackBottom

    # read task entry point  
    set blk [wtxMemRead [expr $tid + $offset(WIND_TCB,entry)] 4]
    set entryAddr [expr [memBlockGet -l $blk]]
    memBlockDelete $blk

    # read the first instruction of the task entry function
    # This supposes to be the instruction for stack frame allocation 

    set size 0
    set mips16Mode [expr $entryAddr & 0x1]
    set entryAddr [expr $entryAddr & (~0x1)]

    if {$mips16Mode} {
        set blk [wtxMemRead $entryAddr 2]
        set inst [expr [memBlockGet -w $blk]]

        if {($inst & 0xff00) == 0x6300} {
            # adjsp instruction

            set size [expr ($inst & 0xff) << 3]

            # size sign extend
            if {$size & 0x400} {
                set size [expr $size | ~0x7FF]
            }
        }
    } else {
        # for 32 bit instruction

        if [catch "set blk [wtxMemRead $entryAddr 4]"] {
            return -1
        }
        set inst [expr [memBlockGet -l $blk]]

        if {(($inst & 0xbfff0000) == 0x27bd0000)} {

            # match "[d]addiu sp,sp,+-offset" to determine stack frame size.

            set size [expr $inst & 0x0000ffff]

            # size sign extend
            if {$size & 0x8000} {
                set size [expr $size | ~0xffff]
            }
        }
    }

    memBlockDelete $blk
    set trc [trcStackLvl [expr $stackBottom - 40 + $size] $sp $pc $regSet]
    
    return $trc
}

#############################################################################
#
# trcInfoGet - get address from which function was called
#
# Determines specific info about the current function, such as the
# address where it was called from, the stack frame size (if any) and
# whether the stack frame has already been allocated.
#
# SYNOPSIS:
#	trcInfoGet pc sp ra
#
# PARAMETERS:
#	pc: current pc
#	sp: stack pointer
#	ra: content of ra register
#
# ERRORS: N/A
#
# RETURNS: 
#

proc trcInfoGet {depth pc sp ra} {
    global raOffset
    global MAX_SCAN_DEPTH
    global cpuType
    global __wtxCpuType
    global trcDebug

    # set default values

    set retAddr 0
    set frameSize 0
    set mips16Mode 0

    if {$trcDebug > 0} {
        puts stdout [format " trcInfoGet depth=%d pc=%08x sp=%08x ra=%08x" $depth $pc $sp $ra]
    }
    #
    # scan instructions forward until the end of function or the max
    # scan count is reached. If we find a "sw ra,x(sp)" or a "jr ra"
    # then the return address is already in register "ra".  If we find
    # a "lw ra,x(sp)" then the return address is saved in offset "x"
    # on the stack. If the instruction space is corrupted, could get
    # a bus error eventually or could find a return address for a
    # neighboring subprogram. The calling pc is determined by subtracting
    # 2 instructions from the return address. The stack frame size, if
    # found, and whether it is allocated, are passed out.
    #

    # Handle mips16 functions for cpu type CW4000_16.

    if {($cpuType($__wtxCpuType) == "CW4000_16")} {
        set type [lindex [wtxSymFind -value $pc] 2]
        if {$type & 0x80} {
            set mips16Mode 1
            set pc [expr $pc & (~0x1)]
        }
    }

    set scanDepth $MAX_SCAN_DEPTH
    set lastPc [expr $pc-$scanDepth]
    if [catch "set scanBlk [wtxMemRead $lastPc [expr $MAX_SCAN_DEPTH+4]]"] {
	return "0 0"
    }

    if {$mips16Mode == 1} {
        for {set i 0} {$i < $scanDepth} {} {
        set inst [expr [memBlockGet -w $scanBlk $i 1]]

            if {($inst & 0xff00) == 0x6200} {

                # sw ra,x(sp) 

                set retAddr $ra

            } elseif {$inst == 0xe820} {
                # jr ra,  means retAddr in ra & end of function reached 

                if {! $retAddr} {
                    set retAddr $ra
                }

                # check branch delay slot and the stop the scan

                set scanDepth [expr $i + 4]

                if {$scanDepth > $MAX_SCAN_DEPTH} {
                    memBlockDelete $scanBlk
                    set scanBlk [wtxMemRead [expr $pc + $MAX_SCAN_DEPTH] 2]
                    set i -2        ; # set to 0 by the loop
                    set scanDepth 2
                }

            } elseif {$inst == 0xef00} {
                
                # XXX  jr a3, end of function, should set to a3 

                if {! $retAddr} {
                    set retAddr $ra
                }

                # check branch delay slot and the stop the scan

                set scanDepth [expr $i + 4]

                if {$scanDepth > $MAX_SCAN_DEPTH} {
                    memBlockDelete $scanBlk
                    set scanBlk [wtxMemRead [expr $pc + $MAX_SCAN_DEPTH] 2]
                    set i -2        ; # set to 0 by the loop
                    set scanDepth 2
                }

            } elseif {($inst & 0xff00) == 0x9700} {
                
                # lw a3,x(sp) 
                set blk [wtxMemRead [expr $sp + $frameSize + \
                                     (($inst & 0xff) << 2)] 4]
                set retAddr [expr [memBlockGet -l $blk]]
                memBlockDelete $blk
            } elseif {($inst & 0xff00) == 0x6300} {

                # adjsp 

                set offset [expr ($inst & 0xff) << 3]

                # offset sign extend
                if {$offset & 0x400} {
                    set offset [expr $offset | ~0x7FF]
                }

                set frameSize [expr $frameSize + $offset]
            }

            # jal(x) or extend instructions are 4 byte long, all other
            # mips16 instructions are 2 byte 

            if {(($inst & 0xf800) == 0x1800) && (($inst & 0xf800) == 0xf000)} { 
                set i [expr $i + 4]
            } else {
                set i [expr $i + 2]
            }
        }  ; # end for loop

    memBlockDelete $scanBlk
    set retAddr [expr $retAddr & (~0x1)]
    set callAddr [expr ($retAddr) ? $retAddr - 6 : 0]

    return "$callAddr $frameSize"

    }  ; # end if {$mips16Mode == 1}

    # non mips16 functions
 
    if {$depth == 0} {
	# Check insn we're sitting on to see if we're at the first
	# instruction of the function
	set inst [expr [memBlockGet -l $scanBlk $scanDepth 1]]
	# match "[d]addiu sp,sp,-offset"
	if {(($inst & 0xbfff8000) == 0x27bd8000)} {
	    # First instruction of function, innermost frame, don't scan
	    set scanDepth -4
	    set retAddr $ra
	} else {
	    # If in innermost frame, don't disassemble instruction we're
	    # sitting on, as we haven't executed it yet.
	    incr scanDepth -4
	}
    }
    for {set i $scanDepth} {$i >= 0} {incr i -4} {
	set inst [expr [memBlockGet -l $scanBlk $i 1]]

	if {(($inst & 0xffe0003f) == 0x03e00008)} {
	    # match "jr ra" means ran off the top and missed the
	    # beginning of the function
	    if {$trcDebug > 0} {
		puts stdout [format "%x: %s" [expr $lastPc + $i] \
			"match: jr ra"]
	    }
	    # If in innermost frame, assume return in ra.
	    if {$depth == 0} {
		set retAddr $ra
	    }
	    break
	} elseif {(($inst & 0xbfff0000) == 0x27bd0000)} {
	    # match "[d]addiu sp,sp,+-offset" to determine stack frame size.
	    if {$trcDebug > 0} {
		puts stdout [format "%x: %s" [expr $lastPc + $i] \
			"match: daddiu/addiu  sp,sp,+-offset"]
	    }
	    # offset sign extend 
	    set offset [expr $inst & 0x0000ffff]
	    if {$offset & 0x8000} {
		set offset [expr $offset | ~0xffff]
	    }
	    if {$offset > 0x0} {
		# stack frame de-allocation implies we may have
		# run off the top of the function.  Stop searching.
		if {$trcDebug > 0} {
		    puts stdout [format "%x: %s" [expr $lastPc + $i] \
			    "ran off top of function"]
		}
		if {$retAddr == 0} {
		    set retAddr $ra
		    break
		}
	    }
	    # If we hit this before 'sw/sd ra,x(sp)', this means we're
	    # stopped between the allocation of the stack frame and the
	    # save of RA on the stack.  If this happens anywhere other
	    # than the innermost frame, it's illegal code and we're probabaly
	    # actually lost
	    set frameSize [expr -($frameSize + $offset)]
	    if {$trcDebug > 0} {
		puts stdout [format "%x: frameSize %d" [expr $lastPc + $i] \
			$frameSize]
	    }
	    if {($depth == 0) && ($retAddr == 0)} {
		set retAddr $ra
		break
	    }
	    if {$retAddr != 0} {
		# Have frameSize and retAddr, done with this frame
		break
	    }
	} elseif {(($inst & 0xffff0000) == 0xffbf0000)} {
	    # match "sd ra,x(sp)" means return address is on the stack
	    if {$trcDebug > 0} {
		puts stdout [format "%x: %s" [expr $lastPc + $i] \
			"match: sd ra,x(sp)"]
	    }
	    # offset sign extend 
	    set offset [expr $inst & 0x0000ffff]
	    if {$offset & 0x8000} {
		set offset [expr $offset | ~0xffff]
	    }
	    set addr [expr $sp + $offset + $raOffset]
	    set blk [wtxMemRead $addr 4]
	    set retAddr [expr [memBlockGet -l $blk]]
	    memBlockDelete $blk
	    if {$trcDebug > 0} {
		puts stdout [format "%x: addr %x retAddr %x" [expr $lastPc + $i] \
			$addr $retAddr]
	    }
	} elseif {(($inst & 0xffff0000) == 0xafbf0000)} {
	    # match "sw ra,x(sp)" means return address is on the stack
	    if {$trcDebug > 0} {
		puts stdout [format "%x: %s" [expr $lastPc + $i] \
			"match: sw ra,x(sp)"]
	    }
	    # offset sign extend 
	    set offset [expr $inst & 0x0000ffff]
	    if {$offset & 0x8000} {
		set offset [expr $offset | ~0xffff]
	    }
	    set addr [expr $sp + $offset]
	    set blk [wtxMemRead $addr 4]
	    set retAddr [expr [memBlockGet -l $blk]]
	    memBlockDelete $blk
	    if {$trcDebug > 0} {
		puts stdout [format "%x: addr %x retAddr %x" [expr $lastPc + $i] \
			$addr $retAddr]
	    }
	}
    }
    memBlockDelete $scanBlk

    # See if it comes from a mips16 function. If so, subtract 2 instructions:
    # one 32 bit, jal(x) and one 16 bit delay slot.

    if {($cpuType($__wtxCpuType) == "CW4000_16")} {
        set type [lindex [wtxSymFind -value $retAddr] 2]
        if {$type & 0x80} {
            set callAddr [expr ($retAddr) ? $retAddr - 6 : 0] 
            return "$callAddr $frameSize"
        }
    }

    set callAddr [expr ($retAddr) ? $retAddr - 8 : 0]
    return "$callAddr $frameSize"
}

##############################################################################
#
# trcPrintArgsMips - read function arguments on the target
#
# This routine is the generic function that return the list of argument values
#
# SYNOPSIS:
#	trcPrintArgsMips pArg nArg
#
# PARAMETERS:
#	pArg: address where argument table starts
#	nArg: number of arguments (4 bytes)
#
# RETURNS: the list of argument values or an empty list if nArg is null
#
# ERRORS: N/A
#

proc trcPrintArgsMips {pArg nArg} {

    global defArgNb
    global _RTypeSize

    if {$nArg == -1} {
	set nArg $defArgNb
    }
    if {$nArg != 0} {
	if [catch "set blk [wtxMemRead $pArg [expr $nArg * $_RTypeSize]]"] {
	    return {}
	}
	if {$_RTypeSize == 4} {
	    set argValues [memBlockGet -l $blk]
	} else {
	    set argVec [memBlockGet -l $blk]
	    # Big-endian? If so, drop first long word
	    if {[targetEndian] == "BIG_ENDIAN"} {
		set argVec [lrange $argVec 1 end]
	    }
	    set argValues {}
	    for {set i 0} {$i < $nArg} {incr i} {
		lappend argValues [lindex $argVec [expr $i*2]]
	    }
	}
	memBlockDelete $blk
	return $argValues
    }
    return {}
}

##############################################################################
#
# trcStackLvl - stack trace routine
#
# Loop and get trace information for each level. Maximun number of level is 40
#
# SYNOPSYS:
#	trcStackLevel stackBottom sp pc regSet
#
# PARAMETERS:
#	stackBottom: effective base of task's stack
#	sp: stack pointer
#	pc: content of pc register
#	regSet:	register set
# 
# ERRORS: N/A
#	
# RETURNS: a list of trace lines
#	   each trace line is a list of 4 elements :
#		{addr callFunc argNb {argList}}
#

proc trcStackLvl {stackBottom sp pc regSet} {
    global raIndex
    global defArgNb
    global trcDebug
 
    set trcStack {}
    set frameSize 0

    if {$trcDebug > 0} {
	puts stdout [format "trcStackLvl %x %x %x" $stackBottom $sp $pc]
    }
    for {set depth 0} {($depth < 40) && ($sp <= $stackBottom)} \
	{incr depth;
	 set pc $callPc;
         incr sp $frameSize;
	 } {
	
	# Start off at zero
        set frameSize 0

	# stop the unwind if it is obvious we can't dereference pc
	
	if {$pc < 0x80000000} {
	    if {$trcDebug > 0} {
		puts stdout "can't dereference pc - stopping"
	    }
	    break
	}

        if {$trcDebug > 0} {
            puts stdout [format " stackBot 0x%x, sp 0x%x, pc 0x%x" $stackBottom $sp $pc]
        }

	bindNamesToList {callPc frameSize} \
			[trcInfoGet $depth $pc $sp [lindex $regSet $raIndex]] 

        if {$trcDebug > 0} {
            puts stdout [format "callPc 0x%x frameSize %d" $callPc $frameSize]
        }

	# stop the unwind if we can't determine the caller of this function

	if {! $callPc} {
	    break
	}

        # only continue unwinding if this is not the last stack frame 

	if {$trcDebug > 0} {
	    puts stdout [format "sp           = %x" $sp]
	    puts stdout [format "sp+frameSize = %x" [expr $sp + $frameSize]]
	    puts stdout [format "stackBottom  = %x" $stackBottom]
	}

	set startPc [trcFindFuncStart $callPc]
	if {($sp + $frameSize) <= $stackBottom} {
	    if {$trcDebug > 0} {
            puts stdout [format "trcStackLvl not last frame, pc=%x" $startPc]
	    }
	    if {! $startPc} {
		
		if {$trcDebug > 0} {
		    puts stdout "trcFindFuncStart failed"
		}
		#
		# Unable to determine entry point from code... Try the symbol
		# table. Note that the symbol table may yield the wrong entry
		# point if the function name is not in it. In this case we
		# get the starting address for the previous function in the 
		# text section and the trace would yield in strange results
		#

		if [catch "wtxSymFind -value $pc" sym] {
		    if {([wtxErrorName $sym] == "SVR_INVALID_FIND_REQUEST") ||
			([wtxErrorName $sym] == "SYMTBL_SYMBOL_NOT_FOUND")} {
			if {$trcDebug > 0} {
			    puts stdout "no symbol"
			}
			return 
		    } else {
			error $sym
		    }
		} else {
		    set startPc [lindex $sym 1]
		}
	    }
	} else {

	    if {$trcDebug > 0} {
            puts stdout [format "trcStackLvl last frame, pc=%x" $startPc]
	    }
	    # Since this is the last frame, we can only determine the entry
	    # point via the symbol table

	    if [catch "wtxSymFind -value $pc" sym] {
		if {([wtxErrorName $sym] == "SVR_INVALID_FIND_REQUEST") ||
		    ([wtxErrorName $sym] == "SYMTBL_SYMBOL_NOT_FOUND")} {
			if {$trcDebug > 0} {
			    puts stdout "no symbol"
			}
			return 
		    } else {
			error $sym
		    }
		} else {
		    set startPc [lindex $sym 1]
		}
	    set level [list $callPc [trcFuncStartName $startPc] \
			$defArgNb [trcPrintArgsMips [expr $sp + $frameSize] -1]]
	    set trcStack [linsert $trcStack 0 $level]
	    break
	}
	set level [list $callPc [trcFuncStartName $startPc] \
			$defArgNb [trcPrintArgsMips [expr $sp + $frameSize] -1]]
	set trcStack [linsert $trcStack 0 $level]
    }
    return $trcStack
}

##############################################################################
#
# trcFindFuncStart - find the starting address of a function
#
# This routine finds the starting address of a function.
# For MIPS processors, to find the start of a function, we use the instruction
# where the function was called from to compute it. The call was one of
# a "jal", "jalr" or "BxAL[L]" instruction. For "jalr", it will be impossible
# to determine the target address since the contents of the target register
# have been lost along the way.
#
# SYNOPSIS:
#	trcFindFuncStart callPc 
#
# PARAMETERS:
#	callPc:	starting address of the current function
#
# ERRORS: N/A
#
# RETURNS: the starting address of the function
#

proc trcFindFuncStart {callPc} {
    global MAX_SCAN_DEPTH
    global trcDebug
    global cpuType
    global __wtxCpuType

    #
    # Compute the target of the call instruction which will be the
    # entry point (i.e., starting address) of the current function.
    # If the call was by a "jalr" instruction, then there is no way
    # to reconstruct the entry point since the register contents have
    # been lost.
    #

    # Handle mips16 functions for cpu type CW4000_16.

    set mips16Mode 0

    if {($cpuType($__wtxCpuType) == "CW4000_16")} {
        set type [lindex [wtxSymFind -value $callPc] 2]
        if {$type & 0x80} {
            set mips16Mode 1
        }
    }

    if {$mips16Mode} {
        set blk [wtxMemRead $callPc 4]
        set startInst [expr [memBlockGet -w $blk 0 1]]
        set nextInst  [expr [memBlockGet -w $blk 2 1]]
    } else { 
        if [catch "set blk [wtxMemRead $callPc 4]"] {
	    return 0
	}
        set startInst [expr [memBlockGet -l $blk]]
    }

    memBlockDelete $blk

    if {$mips16Mode} {
        if { ($startInst & 0xf800) == 0x1800} {
            set imm [expr ($startInst << 16) | $nextInst] 
            set imm [expr ((($imm & 0x3e00000)>> 5) | (($imm & 0x1f0000)<<5) | \
			($imm & 0xffff)) << 2]
            return [expr ($callPc & 0xf0000000) | $imm]
        } else {
            return 0
        }
    }                           
      
    if {(($startInst & 0xfc000000) == 0x0c000000)} {
	
	#
	# Jump And Link : target address specified by shifting the offset 
	# left 2 bits and the "or"ing it with the upper 4 bits of the pc.
	#

	return [expr (($startInst & 0x03ffffff) << 2) | \
		     ($callPc & 0xf0000000)]
    }

    if {(($startInst & 0xfc1f0000) == 0x04100000) ||
	(($startInst & 0xfc1f0000) == 0x04110000) ||
	(($startInst & 0xfc1f0000) == 0x04120000) ||
	(($startInst & 0xfc1f0000) == 0x04130000)} {
	
	#
	# Branch And Link : target address specified by shifting the offset
	# left 2 bits with sign extension and the adding it to the address of
	# the instruction in the delay slot.
	#

	set offset [expr $startInst & 0x0000ffff]
	if {$offset & 0x8000} {
	    set offset [expr $offset | ~0xffff]
	}

        if {$trcDebug > 0} {
            set start [expr $callPc + 1 + $offset]
            puts stdout [format "BAL callPc %x inst %x start %x" $callPc $startInst $start]
        }
	return [expr $callPc + 1 + $offset]
    }
    
    #
    # Either the call was via a "jalr" or there was a bad instruction where we
    # thought we were called from
    #

    return 0
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
# RETURNS:
#

proc dbgRetAdrsGet {ctype cid} {
    global spIndex
    global raIndex
    global MAX_SCAN_DEPTH

    set regSet [regSetOfContext $ctype $cid]
    set pc [pcOfRegSet $regSet]

    #
    # scan instructions forward. If we find a "sw ra,x(sp)" or a "jr ra"
    # then the return address in already in register "ra".  If we find
    # a "lw ra,x(sp)" then the return address is saved in offset "x"
    # on the stack. If the instruction space is corrupted, could get
    # a bus error eventually or could find a return address for a
    # neighboring subprogram.
    #

    set n 0
    set found 0
    while {($n < 10) && (! $found)} {
	set scanBlk [wtxMemRead [expr $pc + ($n * $MAX_SCAN_DEPTH)] \
		     $MAX_SCAN_DEPTH]
	for {set i 0} {$i < $MAX_SCAN_DEPTH} {incr i 4} {
	    set inst [expr [memBlockGet -l $scanBlk $i 1]]
	    if {(($inst & 0xffff0000) == 0xafbf0000) ||
		(($inst & 0xffe0003f) == 0x03e00008)} {

		# match "sw ra,x(sp)" or "jr ra" means return address in ra

		set retAddr [lindex $regSet $raIndex]
		set found 1
		break
	    } elseif {($inst & 0xffff0000) == 0x8fbf0000} {
		
		# match "lw ra, x(sp)" means return address is on the stack

		# offset sign extend

		set offset [expr $inst & 0x0000ffff]
		if {$offset & 0x8000} {
		    set offset [expr $offset | ~0xffff]
		}
		set stackAddr [expr [lindex $regSet $spIndex] + $offset]
		set stackBlk [wtxMemRead $stackAddr 16]
		set retAddr [memBlockGet -l $stackBlk 0 1]

		set found 1
		break
	    }
	}
	memBlockDelete $scanBlk
	incr n
    }
    if {! $found} {
	# return address not found then single step
	return [nextAddrGet $pc]
    } else {
	return $retAddr
    }
}

##############################################################################
#
# dbgFuncCallCheck - check next instruction
# 
# This routine checks to see if the next instruction is a JAL or BAL.
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
#	1 if next instruction is JAL or BAL, or 0 otherwise.
#

proc dbgFuncCallCheck {addr} {

    global cpuType
    global __wtxCpuType
    set mips16Mode 0

    # Handle mips16 functions for cpu type CW4000_16.

    if {($cpuType($__wtxCpuType) == "CW4000_16")} {
        set type [lindex [wtxSymFind -value $addr] 2]
        if {$type & 0x80} {
            set mips16Mode 1
            set addr [expr $addr & (~0x1)]
        }
    }

    if {$mips16Mode} {
        set blk [wtxMemRead $addr 2]
        set inst [expr [memBlockGet -w $blk 0 1]]
    } else {
        set blk [wtxMemRead $addr 4]
        set inst [expr [memBlockGet -l $blk]]
    }
    memBlockDelete $blk

    if {$mips16Mode} {
        # mips16 jal, jalr instructions
       
        return [expr (($inst & 0xf800) == 0x1800) || \
			(($inst & 0xf81f) == 0xe800)] 
        }
     
    # inst 0x74000000 is the CW4000 JALX instruction(JAL+change to 16bit mode). 

    return [expr (($inst & 0xfc000000) == 0x0c000000) || \
		 (($inst & 0xfc00003f) == 0x00000009) || \
		 (($inst & 0xfc1f0000) == 0x04100000) || \
		 (($inst & 0xfc1f0000) == 0x04110000) || \
		 (($inst & 0xfc1f0000) == 0x04120000) || \
		 (($inst & 0xfc000000) == 0x74000000) || \
		 (($inst & 0xfc1f0000) == 0x04130000)]
}
