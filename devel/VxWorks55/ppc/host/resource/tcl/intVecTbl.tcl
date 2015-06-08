# intVecTbl.tcl - core routines for interrupt vector table display
#
# Copyright 1996 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01c,12may98,jmp  use wtxCpuInGet instead of tcl resources arrays.
# 01b,09apr98,jmp  splited by arch: intVec-<arch>.tcl.
# 01a,25mar96,jmp  written.
#

# DESCRIPTION
# This module holds the Tcl code required by the vector table display
# applications.
#

################################################################################
#
# intRtnGet - return the list of the associated addresses to the vector table.
#
# This routine returns the list of the associated addresses (or symbols if
# they can be found) for all the vectors in the vector table if vector is
# equal to -1 or for the given interrupt vector. This routine calls an
# architechture specific routine <$arch>_intRtnGet.
#
# SYNOPSIS:
#   intRtnGet <vecBaseAddr> <vector>
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

proc intRtnGet {vecBaseAddr vector} {

    # if vector table address is not given go get it

    if {$vecBaseAddr == 0} {
	if [catch "wtxSymFind -name intVecBaseGet" intVecBaseGetInfo] {
	    error "This request cannot be completed because intVecBaseGet cannot be found in the symbol table"
	}

	set intVecBaseGetFunc [lindex $intVecBaseGetInfo 1]

	if [catch "wtxDirectCall $intVecBaseGetFunc" vecBaseAddr] {
	    error $vecBaseAddr
	}
    }

    # get and call architecture specific routine
    set family [wtxCpuInfoGet -a [lindex [lindex [wtxTsInfoGet] 2] 0]]
    set intFile [wtxPath host resource tcl]intVec-$family.tcl

    if {[file exists $intFile]} {
	source [wtxPath host resource tcl]intVec-$family.tcl
	set rtnGet "_intRtnGet"
	set funcAddrList [$family$rtnGet $vecBaseAddr $vector]
    } else {
        error "This command is not available on $family architecture"
    }

    return $funcAddrList
}

################################################################################
#
# rtnNameGet - return the name of the nearest symbol
#
# This routine returns the name of the nearest symbol to the given address,
# if no symbol is found, the given address is returned.
#
# PARAMETERS:
#	address: the address to look around.
#
# RETURNS:
#	The name of the nearest symbol to the given address.
# or
#	The given address if no symbol is found.
#
# ERRORS: N/A
#

proc rtnNameGet {address} {

    # if no symbol is found near $address the address is returned
    if [catch "wtxSymFind -value $address" rtnName] {

	return [format 0x%x $address]

    } else {

	# return the nearest symbol name found
	return [lindex $rtnName 0]
    }
}

################################################################################
#
# intVecTableReadError - 
#
# This routine must be called when an error is raised while reading
# the interrupt vector table.
#
proc intVecTableReadError {error} {

    error "$error\nInterrupt Vector Table may be corrupted."
}
