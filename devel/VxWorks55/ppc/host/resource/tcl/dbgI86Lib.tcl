# dbgI86Lib.tcl - i86-specific debug routines Tcl implementation 
#
# Copyright 1996-2002 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01j,18sep02,pai  Modify tt() such that it can trace back to vxTaskEntry (SPR
#                  73375).
# 01i,07feb99,dbt  completed hardware breakpoints description.
# 01h,27may98,pdn  fixed trcStackLvl() so that tt works on simnt also.
# 01g,07may98,jmp  fixed syntax error related to Tcl8 upgrade.
# 01f,10mar98,dbt  added architecture specific help message and support for
#                  hardware breakpoints.
# 01e,17sep97,fle  changing call to disassemble in call to wtxMemDisassemble
# 01d,06jun96,elp  made dbgRetAdrsGet{} work in sytem mode (SPR# 6659).
# 01c,06mar96,elp  fix two tt bugs.
# 01b,06mar96,elp  added debug routines and renamed,
#                  fixed displacement computation in trcFollowJump.
# 01a,27feb96,elp  written.
#

# DESCRIPTION
# /target/src/arch/i86/trcLib.c inspired
# /target/src/arch/i86/dbgLib.c inspired

# globals

# Set architecture help message

set archHelpMsg       "bh        addr\[,access\[,task\[,count\]\]\]\
                      Set hardware breakpoint\n\
                      \t\taccess:\t0 - instruction\t\t1 - write 1 byte\n\
                      \t\t\t3 - read/write 1 byte\t5 - write 2 bytes\n\
                      \t\t\t7 - read/write 2 bytes\td - write 4 bytes\n\
                      \t\t\tf - read/write 4 bytes\n";
set hwBpTypeList {
    {"0"        "hard-inst."             "Instruction"}
    {"1"        "hard-write 1 byte"      "Data write 1 byte"}
    {"3"        "hard-r/w 1 byte"        "Data read/write 1 byte"}
    {"5"        "hard-write 2 bytes"     "Data write 2 bytes"}
    {"7"        "hard-r/w 2 bytes"       "Data read/write 2 bytes"}
    {"0xd"      "hard-write 4 bytes"     "Data write 4 bytes"}
    {"0xf"      "hard-r/w 4 bytes"       "Data read/write 4 bytes"}
};


set cpuName            [wtxCpuInfoGet -n [lindex [lindex [wtxTsInfoGet] 2] 0]]
set MAX_TRACE_DEPTH     40  ;# max number of levels of stack to trace

set defArgNb            5   ;# default number of argument

set fpIndex             2   ;# index of the fp register in the register set
set spIndex             3   ;# index of the sp register in the register set
set pcIndex             9   ;# index of the pc register in the register set

set ADDI08_0            0x83
set ADDI08_1            0xc4
set ADDI32_0            0x81
set ADDI32_1            0xc4
set LEAD08_0            0x8d
set LEAD08_1            0x64
set LEAD08_2            0x24
set LEAD32_0            0x8d
set LEAD32_1            0xa4
set LEAD32_2            0x24
set JMPD08              0xeb
set JMPD32              0xe9
set ENTER               0xc8
set PUSH_EBP            0x55
set MOV_ESP0            0x89
set MOV_ESP1            0xe5
set LEAVE               0xc9
set RET                 0xc3
set RETADD              0xc2
set CALL_DIR            0xe8
set CALL_INDIR0         0xff
set CALL_INDIR1         0x10
set CALL_INDIR_REG_EAX  0xd0
set CALL_INDIR_REG_ECX  0xd1
set CALL_INDIR_REG_EDX  0xd2
set CALL_INDIR_REG_EBX  0xd3


##############################################################################
#
# trcStack - print a trace of function calls from the stack
#
# This routine provides the low-level stack trace function.  A higher-level
# symbolic stack trace, built on top of trcStack(), is provided by tt() in
# dbgLib.
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
# routines that start with an PUSH %EBP MOV %ESP %EBP instruction.  Most
# VxWorks assembly language routines include PUSH %EBP MOV %ESP %EBP
# instructions for exactly this reason.
# However, routines written in other languages, strange entries into
# routines, or tasks with corrupted stacks can confuse the trace.  Also,
# all parameters are assumed to be 32-bit quantities, therefore structures
# passed as parameters will be displayed as a number of long integers.
#
# SYNOPSIS:
#        trcStack tid
# 
# PARAMETERS:
#        tid: task to trace
#
# ERRORS: N/A
#
# RETURNS: a list of trace lines
#           each trace line is a list of 4 elements :
#                {addr callFunc argNb {argList}}
#

proc trcStack {tid} {
    global fpIndex
    global spIndex
    global pcIndex
    global endianFlag
    global PUSH_EBP
    global MOV_ESP0
    global MOV_ESP1
    global ENTER
    global RET
    global RETADD

    # get the register set of the task

    set regList [regSetOfContext CONTEXT_TASK $tid]

    #
    # if the current routine doesn't have a stack frame, then we fake one
    # by putting the old one on the stack and making fp point to that;
    # we KNOW we don't have a stack frame in a few restricted but useful
    # cases:
    #  1) we are at a PUSH %EBP MOV %ESP %EBP or RET or ENTER instruction,
    #  2) we are the first instruction of a subroutine (this may NOT be
    #     a PUSH %EBP MOV %ESP %EBP instruction with some compilers)
    #

    set pc [lindex $regList $pcIndex]

    bindNamesToList {addr blk} [trcFollowJmp $pc]

    # blk holds 8 bytes of memory starting at addr-1

    bindNamesToList {prevInst inst nextInst next2Inst} [memBlockGet -b $blk]
    memBlockDelete $blk

    set fp [lindex $regList $fpIndex]
    set sp [lindex $regList $spIndex]

    set noStackFrame [expr (($inst == $PUSH_EBP) && \
                       ($nextInst == $MOV_ESP0)  && \
                       ($next2Inst == $MOV_ESP1))]

    set noStackFrame [expr $noStackFrame         || \
                       ($inst == $ENTER)         || \
                       ($inst == $RET)           || \
                       ($inst == $RETADD)]

    if ![catch {set pcSym [wtxSymFind -value $pc]}] {

        set noStackFrame [expr $noStackFrame     || \
                        ([lindex $pcSym 1] == $pc)]
    }


    if {$noStackFrame} {

        # no stack frame - fake one

        set savedStack [wtxGopherEval "$sp -4 @"]
        set blk [memBlockCreate $endianFlag 4]
        memBlockSet -l $blk 0 $fp
        wtxMemWrite $blk [expr $sp - 4]
        memBlockDelete $blk

        set fp [expr $sp - 4]
        set trc [trcStackLvl $fp $pc 0]

        # restore stack

        set blk [memBlockCreate $endianFlag 4]
        memBlockSet -l $blk 0 $savedStack
        wtxMemWrite $blk [expr $sp - 4]
        memBlockDelete $blk
    } elseif {($prevInst == $PUSH_EBP) && ($inst == $MOV_ESP0) && \
               ($nextInst == $MOV_ESP1)} {
        set fp $sp
        set trc [trcStackLvl $fp $pc 0]
    } else {
        set trc [trcStackLvl $fp $pc 0]
    }

    return $trc
}

#############################################################################
#
# trcFollowJmp - resolve any JMP instructions to final destination
#
# This routine returns a pointer to the next non-JMP instruction to be
# executed if the pc were at the specified <adrs>.  That is, if the instruction
# at <adrs> is not a JMP, then <adrs> is returned.  Otherwise, if the
# instruction at <adrs> is a JMP, then the destination of the JMP is
# computed, which then becomes the new <adrs> which is tested as before.
# Thus we will eventually return the address of the first non-JMP instruction
# to be executed.
#
# The need for this arises because compilers may put JMPs to instructions
# that we are interested in, instead of the instruction itself.  For example,
# optimizers may replace a stack pop with a JMP to a stack pop.  Or in very
# UNoptimized code, the first instruction of a subroutine may be a JMP to
# a PUSH %EBP MOV %ESP %EBP, instead of a PUSH %EBP MOV %ESP %EBP (compiler
# may omit routine "post-amble" at end of parsing the routine!).  We call
# this routine anytime we are looking for a specific kind of instruction,
# to help handle such cases.
#
# SYNOPSIS:
#        trcFollowJmp adrs
#
# PARAMETERS:
#         adrs : current address
#
# ERRORS: N/A
#
# RETURNS: address that chain of branches points to and the 6-bytes block of 
#           memory read from address - 1.
#

proc trcFollowJmp {adrs} {
    global JMPD08
    global JMPD32
    
    #
    # read 6 bytes beginning at $addr - 1
    # these bytes can be interpreted as instructions or displacement
    #

    set blk [wtxMemRead [expr $adrs - 1] 6]
    set inst [memBlockGet -b $blk 1 1]

    while {($inst == $JMPD08) || ($inst == $JMPD32)} {
        if {$inst == $JMPD08} {
            set length 2
            set displacement [memBlockGet -b $blk 2 1]
        } elseif {$inst == $JMPD32} {
            set length 5
            set displacement [memBlockGet -l $blk 2 1]
        }
        set adrs [expr $adrs + $displacement + $length]
        memBlockDelete $blk
        set blk [wtxMemRead [expr $adrs - 1] 6]
        bindNamesToList {inst} [memBlockGet -b $blk 1 1]
        set displacement [memBlockGet -l $blk 2 1]
    }
    return "$adrs $blk"
}

##############################################################################
#
# trcStackLvl - stack trace routine
#
# SYNOPSIS:
#        trcStackLvl fp pc
#
# PARAMETERS:
#        fp: frame pointer
#        pc: program counter
# 
# ERRORS: N/A
#
# RETURNS: the list of trace lines, each line is composed of 4 arguments
#           {{addr funcName argNb {argList}} ....}
#

proc trcStackLvl {fp pc depth} {

    global MAX_TRACE_DEPTH
    global cpuName

    set trcStack {}

    if {$fp == 0} {
        return $trcStack     ;# stack is untraceable
    }

    set fpVal [wtxGopherEval "$fp *!"]

    # SIMNT vxTaskEntry does not force ebp to NULL

    if {$cpuName == "SIMNT"} {

        if {$fpVal == 0} {
            return $trcStack
        } else {
            set returnAdrs [wtxGopherEval "$fp + 4 *!"]
        }
    } else {

        set returnAdrs [wtxGopherEval "$fp + 4 *!"]
    }

    if {($fpVal != 0) && ($depth < $MAX_TRACE_DEPTH)} {
        set trcStack [trcStackLvl $fpVal $returnAdrs [expr $depth + 1]]
    }

    set callAddr [trcFindCall $returnAdrs]
    set nArg     [trcCountArgs $returnAdrs]
    set level    [list $callAddr [trcFindFuncStart $fp $pc $callAddr] \
                   $nArg [trcPrintArgs [expr $fp + 8] $nArg]]
    set trcStack [linsert $trcStack end $level]

    return $trcStack
}

#############################################################################
#
# trcFindCall - get address from which function was called
#
# SYNOPSIS:
#        trcFindCall returnAdrs
#
# PARAMETERS:
#         returnAdrs: current address
#
# ERRORS: N/A
#
# RETURNS: address from which current subroutine was called, or NULL.
#

proc trcFindCall {returnAdrs} {
    global CALL_INDIR_REG_EAX
    global CALL_INDIR_REG_EDX
    global CALL_INDIR0
    global CALL_INDIR1
    global CALL_DIR
    
    # starting at the word preceding the return addr, search for jsr or bsr 

    for {set addr [expr $returnAdrs - 1]; set ix 0} {$addr != 0} \
        {incr addr -1; incr ix} {
        
        # read 2 instructions

        set blk [wtxMemRead $addr 2]
        bindNamesToList {inst nextInst} [memBlockGet -b $blk]
        memBlockDelete $blk

        if {(($inst == $CALL_INDIR0) && \
             ((($nextInst & 0x38) == $CALL_INDIR1) || \
             (($nextInst & 0xdf) == $CALL_INDIR_REG_EAX)   || \
             (($nextInst & 0xdf) == $CALL_INDIR_REG_EDX))) || \
            ($inst == $CALL_DIR)} {

            return $addr        ; # found it
        }

        # prevent infinite loop

        if {$ix > 128} {
            break
        }

    }
    return 0        ; # not found
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
# If the above check fails, we search backward from the given pc until 
# PUSH %EBP MOV %ESP %EBP instructions are found.  If the compiler is putting 
# PUSH %EBP MOV %ESP %EBP instructions
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
# doesn't start with a PUSH %EBP MOV %ESP %EBP and isn't in the symbol table,
# may not be possible to locate.
#
# SYNOPSIS:
#        trcFindFuncStart fp pc callerAddr
#
# PARAMETERS:
#        fp: frame pointer
#        pc: progrm counter
#        callerAddr: address in the function
#
# ERRORS: N/A
#
# RETURNS: the name of the calling function
#

proc trcFindFuncStart {fp pc callerAddr} {
    global PUSH_EBP
    global MOV_ESP0
    global MOV_ESP1
    global ENTER

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
            if {($ip != 0) && ($ip >= $minPc) && ($ip <= $pc)} {
                return [trcFuncStartName $ip]
            }
        }
    }

    #
    # search backward for PUSH %EBP MOV %ESP %EBP
    # read memory from minPc until pc+2 
    #

    set size [expr $pc + 2 - $minPc + 1]
    set blk [wtxMemRead $minPc $size]

    # get the instruction beginning by the end of the block

    for {set i [expr $size - 3]} {$i >= 0} {incr i -1} {
        bindNamesToList {inst inst2 inst3} [memBlockGet -b $blk $i 3]
        if {(($inst == $PUSH_EBP) && ($inst2 == $MOV_ESP0) && \
             ($inst3 == $MOV_ESP1)) || ($inst == $ENTER)} {
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
#         addr: address of the symbol about which we want the name
#
# ERRORS: N/A
#
# RETURNS:
#        the name of the symbol if <addr> is the address of a symbol
#       the address if a symbol is found but with a different address
#       ???? when symbol search fails
#

proc trcFuncStartName {addr} {
    
    if [catch "wtxSymFind -value $addr" sym] {
        if {([wtxErrorName $sym] != "SYMTBL_SYMBOL_NOT_FOUND") ||
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
#        trcFindDest addr
#
# PARAMETERS:
#        addr: call instruction address
#
# ERRORS: N/A
#
# RETURNS: address to which call instruction (jsr) will branch, or NULL if
#           unknown
#

proc trcFindDest {callAddr} {
    global CALL_DIR

    set blk [wtxMemRead $callAddr 5]
    set inst [expr [memBlockGet -b $blk 0 1]]
    set displ [memBlockGet -l $blk 1 1]
    memBlockDelete $blk

    if {$inst == $CALL_DIR} {
        return [expr $callAddr + 5 + $displ]
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
# value of -1 may mean "don't know".
# 
# SYNOPSIS:
#        trcCountArgs addr
#
# PARAMETERS:
#        addr: return address of function call
#
# ERRORS: N/A
#
# RETURNS: number of arguments of function or default number
#

proc trcCountArgs {addr} {
    global ADDI08_0
    global ADDI08_1
    global ADDI32_0
    global ADDI32_1
    global LEAD08_0
    global LEAD08_1
    global LEAD08_2
    global LEAD32_0
    global LEAD32_1
    global LEAD32_2
    
    set nArgs -1

    # if instruction is JMP use the target of the JMP as the return address

    bindNamesToList {returnAdrs blk} [trcFollowJmp $addr]

    # blk holds the 8 bytes starting at retaddr-1

    bindNamesToList {inst_0 inst_1 inst_2} [memBlockGet -b $blk 1 3]

    if {($inst_0 == $ADDI08_0) && ($inst_1 == $ADDI08_1)} {
        set nArgs [memBlockGet -b $blk 3 1]
    } elseif {($inst_0 == $ADDI32_0) && ($inst_1 == $ADDI32_1)} {
        set nArgs [memBlockGet -l $blk 3 1]
    } elseif {($inst_0 == $LEAD08_0) && ($inst_1 == $LEAD08_1) && \
              ($inst_2 == $LEAD08_2)} {
        set nArgs [memBlockGet -b $blk 4 1]
    } elseif {($inst_0 == $LEAD32_0) && ($inst_1 == $LEAD32_1) && \
              ($inst_2 == $LEAD32_2)} {
        set nArgs [memBlockGet -l $blk 4 1]
    } else {
        memBlockDelete $blk
        return $nArgs
    }
    memBlockDelete $blk
    if {$nArgs < 0} {
        set nArgs [expr 0 - $nArgs]
    }

    return [expr $nArgs >> 2]
}

##############################################################################
#
# dbgRetAdrsGet - address of the subroutine in which has hit a breakpoint
#
# SYNOPSIS:
#       dbgRetAdrsGet ctype cid
#
# PARAMETERS:
#       ctype: context type
#       cid: context id
#
# ERRORS: N/A
#
# RETURNS: Address of the next instruction to be executed upon return of
#           current routine
#

proc dbgRetAdrsGet {ctype cid} {
    global spIndex
    global fpIndex
    global PUSH_EBP
    global MOV_ESP1
    global MOV_ESP0
    global ENTER
    global RET
    global RETADD
    
    set regSet [regSetOfContext $ctype $cid]
    set pc [pcOfRegSet $regSet]

    set blk [wtxMemRead [expr $pc - 1] 4]
    bindNamesToList {inst1 inst2 inst3 inst4} [memBlockGet -b $blk]
    memBlockDelete $blk

    if {($inst2 == $PUSH_EBP) && ($inst3 == $MOV_ESP0) && 
        ($inst4 == $MOV_ESP1)} {
        set addr [lindex $regSet $spIndex]
    } elseif {($inst1 == $PUSH_EBP) && ($inst2 == $MOV_ESP0) && 
              ($inst3 == $MOV_ESP1)} {
        set sp [lindex $regSet $spIndex]
        set addr [expr $sp + 4]
    } elseif {($inst2 == $ENTER) || ($inst2 == $RET) ||
              ($inst2 == $RETADD)} {
        set addr [lindex $regSet $spIndex]
    } else {
        set fp [lindex $regSet $fpIndex]
        set addr [expr $fp + 4]
    }

    return [wtxGopherEval "$addr *!"]
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
    global CALL_INDIR_REG_EAX
    global CALL_INDIR_REG_EDX
    global CALL_INDIR0
    global CALL_INDIR1
    global CALL_DIR
    
    set blk [wtxMemRead $addr 2]
    bindNamesToList {inst1 inst2} [memBlockGet -b $blk]
    memBlockDelete $blk

    if {(($inst1 == $CALL_INDIR0) && \
         ((($inst2 & 0x38) == $CALL_INDIR1) || \
         (($inst2 & 0xdf) == $CALL_INDIR_REG_EAX)   || \
         (($inst2 & 0xdf) == $CALL_INDIR_REG_EDX))) || \
        ($inst1 == $CALL_DIR)} {

        return 1
    }

    return 0
}
