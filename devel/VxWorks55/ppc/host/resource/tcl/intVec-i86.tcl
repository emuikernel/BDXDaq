# intVec-i86.tcl - i86 specific routine for interrupt vector table display
#
# Copyright 1996-2002 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01b,14nov02,jmp  SPR#27883: updated intConnect code test, according to
#            +bpn  modifications in intConnect().
# 01a,25mar96,jmp  written.
#

# DESCRIPTION
# This module holds the Tcl code required by the vector table display
# applications.
#

################################################################################
#
# i86_intRtnGet - return informations about the i86 vector table.
#
# This routine returns the list of the associated addresses (or symbols if
# they can be found) for all the vectors in the i86 vector table if vector
# is equal to -1 or for the given interrupt vector. This routine is specific
# to i86 and is called by the main routine intRtnGet.
#
# SYNOPSIS:
#   i86_intRtnGet <vecBaseAddr> <vector>
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

proc i86_intRtnGet {vecBaseAddr vector} {

    set numVecMax 256   ;# maximum number of vectors

    # check input parameters

    if {$vector < -1 || $vector >= $numVecMax} {
        return -1
    }

    # get the vector table
    if [catch "wtxMemRead $vecBaseAddr [expr $numVecMax * 8]" vectorsBlock] {
	intVecTableReadError $vectorsBlock
    }

    set vectorTable [memBlockGet -w $vectorsBlock]
    memBlockDelete $vectorsBlock

    set intConnectCode "0x??e8 0x???? 0x50?? 0x5152 0x???? 0x???? 0x???? 0x???? 0x???? 0x??68 0x???? 0xe8?? *"

    # intConnectCode (intArchLib.c)
    # 00  e8 kk kk kk kk           call    _intEnt         * tell kernel
    # 05  50                       pushl   %eax            * save regs
    # 06  52                       pushl   %edx
    # 07  51                       pushl   %ecx
    # 08  68 pp pp pp pp           pushl   $_parameterBoi  * push BOI param
    # 13  e8 rr rr rr rr           call    _routineBoi     * call BOI routine
    # 18  68 pp pp pp pp           pushl   $_parameter     * push param
    # 23  e8 rr rr rr rr           call    _routine        * call C routine

    set i 0

    if {$vector >= 0} {
        set numVecMax [expr $vector + 1]
        set i $vector
    }


    # get one vector or all the vectors

    set funcAddr ""

    for {} {$i < $numVecMax} {incr i} {

        set cont 1

	set vectorInfo [lrange $vectorTable [expr $i*4] [expr ($i*4)+3]]

	set addr [expr ([lindex $vectorInfo 3] << 16) | [lindex $vectorInfo 0]]

	set funcName [rtnNameGet $addr]

	if {[string match "*excCallTbl" $funcName]} {

	    if [catch "wtxMemRead [expr $addr + 1] 4" callBlock] {
		lappend funcAddr "Corrupted Int"
		set cont 0
	    }

	    if {$cont} {
		set callCode [memBlockGet -l $callBlock]
		memBlockDelete $callBlock

		set callFuncAddr [expr $callCode + $addr + 5]

		set callFuncName [rtnNameGet $callFuncAddr]

		case $callFuncName in {

		    "*excIntStub" {lappend funcAddr "Uninit. Int"}
		    "*excStub"    {lappend funcAddr "Std Excep. Handler"}
		    "*"           {lappend funcAddr "Unknown"}
		}
	    }

	} else {

	    if [catch "wtxMemRead $addr 28" memBlock] {
		lappend funcAddr "Corrupted Int"
		set cont 0
	    }

	    if {$cont} {
		set code [memBlockGet -w $memBlock]
		memBlockDelete $memBlock

		# Test if the associated routine is a stub (intConnect)
		if {[string match $intConnectCode $code]} {

		    set callAddr [expr (([lindex $code 13]<<16) |\
				    [lindex $code 12]) + $addr + 28]

		    lappend funcAddr [rtnNameGet $callAddr]

		} else {

		    lappend funcAddr $funcName
		}
	    }
	}
    }

    return $funcAddr
}
