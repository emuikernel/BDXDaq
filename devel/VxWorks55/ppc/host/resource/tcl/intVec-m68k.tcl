# intVec-m68k.tcl - mc68k specific routine for interrupt vector table display
#
# Copyright 1996 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01b,28jul98,jmp  used wtxCpuInfoGet instead of wtxCpuType array.
# 01a,25mar96,jmp  written.
#

# DESCRIPTION
# This module holds the Tcl code required by the vector table display
# applications.
#

################################################################################
#
# m68k_intRtnGet - return informations about the m68k vector table.
#
# This routine returns the list of the associated addresses (or symbols if
# they can be found) for all the vectors in the m68k vector table if vector
# is equal to -1 or for the given interrupt vector. This routine is specific
# to m68k and is called by the main routine intRtnGet.
#
# SYNOPSIS:
#   m68k_intRtnGet <vecBaseAddr> <vector>
#
# PARAMETERS:
#   vecBaseAddr: The vector table address.
#   vector: The number of the interrupt vector or 
#	    -1 to get the whole vector table.
#
# RETURNS:
#   routineName
# or
#   vec0Rtn vec1Rtn vec2Rtn ... vec<n>Rtn
# or
#   ERROR
#

proc m68k_intRtnGet {vecBaseAddr vector} {

    set numVecMax 256	;# maximum number of vectors

    case [wtxCpuInfoGet -n [lindex [lindex [wtxTsInfoGet] 2] 0]] in {
	
	"MC68000" {	;# (CPU==MC68000)
	    set excStubCode    "0x9ffc 0x0000 0x0044 0x48d7 0xffff 0x06af *"
	    set excIntStubCode "0x52b9 0x???? 0x???? 0x9ffc 0x0000 0x0044 *"
	}

	"MC680*40" {	;# (CPU==MC68040 || CPU==MC68LC040)
	    set excStubCode    "0x48f9 0x0707 0x0007 0xd21c 0x9ffc 0x0000 *"
	    set excIntStubCode "0x52b9 0x???? 0x???? 0x9ffc 0x0000 0x0048 *"
	}

	"*" {		;# (CPU!=MC68000 && CPU!=MC68040 && CPU!=MC68LC040)
	    set excStubCode    "0x9ffc 0x0000 0x0048 0x48d7 0xffff 0x06af *"
	    set excIntStubCode "0x52b9 0x???? 0x???? 0x9ffc 0x0000 0x0048 *"
	}
    }

    set intConnectCode "0x4eb9 0x???? 0x???? 0x48e7 0xe0c0 0x2f3c\
			0x???? 0x???? 0x4eb9 *"

    # intConnectCode (intArchLib.c)
    # 0x00  4EB9 kkkk kkkk	jsr	_intEnt			tell kernel
    # 0x06  48E7 E0C0		movem.l	d0-d2/a0-a1,-(a7)	save regs
    # 0x0a  2F3C pppp pppp	move.l	#parameter,-(a7)	push param
    # 0x10  4EB9 rrrr rrrr	jsr	routine			call C routine
    # 0x16  588F		addq.l	#4,a7			pop param
    # 0x18  4CDF 0307		movem.l	(a7)+,d0-d2/a0-a1	restore regs
    # 0x1c  4EF9 kkkk kkkk	jmp	_intExit		exit via kernel


    # check input parameters

    if {$vector < -1 || $vector >= $numVecMax} {
	return -1
    }

    # get the vector table
    if [catch {wtxMemRead $vecBaseAddr [expr $numVecMax * 4]} vectorBlock] {
	intVecTableReadError $vectorBlock
    }
    set vectorTable [memBlockGet -l $vectorBlock]
    memBlockDelete $vectorBlock

    set i 0

    if {$vector >= 0} {
	set numVecMax [expr $vector + 1]
	set i $vector
    } 


    # get one vector or all the vectors

    set funcAddr ""

    for {} {$i < $numVecMax} {incr i} {

    	set cont 1

	set stubAddr [lindex $vectorTable $i]

	if [catch "wtxMemRead $stubAddr 22" blockId] {
	    lappend funcAddr "Corrupted Int"
	    set cont 0
	}

	if {$cont} {
	    set memBlock [memBlockGet -w $blockId]
	    memBlockDelete $blockId

	    # Test if the associated routine is a stub
	    if {[string match $intConnectCode $memBlock]} {
		set rtnAddr [expr ([lindex $memBlock 9] << 16) + \
			    [lindex $memBlock 10]]
		lappend funcAddr [rtnNameGet $rtnAddr]
	    } else {
		    
		# Test if the associated routine is an interrupt handler
		if {[string match $excIntStubCode $memBlock]} {
		    lappend funcAddr "Uninit. Int"

		} else {

		    # Test if it is an uninitialized exception handler
		    if {[string match $excStubCode  $memBlock]} {
			lappend funcAddr "Std Excep. Handler"

		    } else {
			lappend funcAddr [rtnNameGet $stubAddr]
		    }
		}
	    }
	}
    }

    return $funcAddr
}
