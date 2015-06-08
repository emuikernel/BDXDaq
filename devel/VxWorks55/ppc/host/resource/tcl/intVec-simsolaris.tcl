# intVec-simsolaris.tcl - simsolaris specific routine for interrupt vector
#			  table display
#
# Copyright 1996 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01b,12may98,jmp  relpace simso by simsolaris.
# 01a,25mar96,jmp  written.
#

# DESCRIPTION
# This module holds the Tcl code required by the vector table display
# applications.
#

################################################################################
#
# simsolaris_intRtnGet - return informations about the simsolaris vector table.
#
# This routine returns the list of the associated addresses (or symbols if
# they can be found) for all the vectors in the simsolaris vector table if
# vector is equal to -1 or for the given interrupt vector. This routine is
# specific to simsolaris and is called by the main routine intRtnGet.
#
# SYNOPSIS:
#   simsolaris_intRtnGet <vecBaseAddr> <vector>
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

proc simsolaris_intRtnGet {vecBaseAddr vector} {

    set numVecMax 64	;# maximum number of vectors

    # check input parameters
    # ignore vecBaseAddr on sparc simulator

    if {$vector < -1 || $vector >= $numVecMax} {
	return -1
    }

    # get the pseudo vector table address

    if [catch "wtxSymFind -name intVecTable" intVecTableSym] {
	error $intVecTableSym
    }
    
    set intVecTableAddr [lindex $intVecTableSym 1]

    # Get the Vector Table
    if [catch {wtxGopherEval "$intVecTableAddr \
				($numVecMax < *! > +4)"} vecTbl] {
	intVecTableReadError $vecTbl
    }

    set i 0

    if {$vector >= 0} {
	set numVecMax [expr $vector +1]
	set i $vector
    }

    for {} {$i < $numVecMax} {incr i} {

	set cont 1

	if {[lindex $vecTbl $i] != 0 } {
	
	    if [catch {wtxSymFind -value [lindex $vecTbl $i]} symInfo] {
		lappend funcAddr "Corrupted Int"
		set cont 0
	    }

	    if {$cont} {

		if {[lindex $symInfo 1] == [lindex $vecTbl $i]} {
		    lappend funcAddr [rtnNameGet [lindex $symInfo 1]]

		} else {

		    if [catch {wtxGopherEval "[lindex $vecTbl $i] *!"} addr] {
			lappend funcAddr "Corrupted Int"
		    }

		    lappend funcAddr [rtnNameGet $addr]

		}
	    }
	} else {
	    
	    lappend funcAddr "Uninit. Int"
	}
    }

    return $funcAddr
}
