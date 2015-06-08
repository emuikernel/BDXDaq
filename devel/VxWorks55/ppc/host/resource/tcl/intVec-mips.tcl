# intVec-mips.tcl - mips specific routine for interrupt vector table display
#
# Copyright 1996 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,25mar96,jmp  written.
#

# DESCRIPTION
# This module holds the Tcl code required by the vector table display
# applications.
#

################################################################################
#
# mips_intRtnGet - return informations about the mips vector table.
#
# This routine returns the list of the associated addresses (or symbols if
# they can be found) for all the vectors in the mips vector table if vector
# is equal to -1 or for the given interrupt vector. This routine is specific
# to mips and is called by the main routine intRtnGet.
#
# SYNOPSIS:
#   mips_intRtnGet <vecBaseAddr> <vector>
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

proc mips_intRtnGet {vecBaseAddr vector} {

    set numVecMax 256   ;# maximum number of vectors

    # check input parameters

    if {$vector < -1 || $vector >= $numVecMax} {
        return -1
    }

    # get the vector table
    if [catch "wtxMemRead $vecBaseAddr [expr $numVecMax * 4]" vectorsBlock] {
	intVecTableReadError $vectorsBlock
    }

    set vectorTable [memBlockGet -l $vectorsBlock]
    memBlockDelete $vectorsBlock

    set intConnectCode "0x3c08???? 0x3c04???? 0x3508???? 0x01000008 0x3484???? *"

    set i 0

    if {$vector >= 0} {
        set numVecMax [expr $vector + 1]
        set i $vector
    }


    # get one vector or all the vectors

    set funcAddr ""

    for {} {$i < $numVecMax} {incr i} {

	set cont 1

	set vectorInfo [lindex $vectorTable $i]

	set callFuncName [rtnNameGet $vectorInfo]

	case $callFuncName in {

	    "*excExcHandle" {
		if {$i <= 31} {
		    lappend funcAddr "Std Excep. Handler"
		} else {
	    	    lappend funcAddr "Uninit. Int"
		}
	    }

	    "*"	{
		if [catch "wtxMemRead $vectorInfo 20" codeBlock] {
		    lappend funcAddr "Corrupted Int"
		    set cont 0
		}

		if {$cont} {
		    set code [memBlockGet -l $codeBlock]
		    memBlockDelete $codeBlock

		    # Test if the associated routine is a stub

		    if {[string match $intConnectCode $code]} {
			
			set msh [expr [lindex $code 0] & 0xffff]
			set lsh [expr [lindex $code 2] & 0xffff]
			set calledFunc [format "0x%x" [expr [expr $msh << 16] + $lsh]]

			lappend funcAddr [rtnNameGet $calledFunc]

		    } else {

			lappend funcAddr [rtnNameGet [lindex $vectorTable $i]]
		    }
		}
	    }
	}
    }
    return $funcAddr
}
