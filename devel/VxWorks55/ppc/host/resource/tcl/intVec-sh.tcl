# intVec-sh.tcl - sh specific routine for interrupt vector table display
#
# Copyright 1996 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01b,17oct00,rsh  add support for SH2
# 01a,23may00,rsh  written.
#

# DESCRIPTION
# This module holds the Tcl code required by the vector table display
# applications.
#

################################################################################
#
# sh_intRtnGet - return informations about the sh vector table.
#
# This routine returns the list of the associated addresses (or symbols if
# they can be found) for all the vectors in the sh vector table if vector
# is equal to -1 or for the given interrupt vector. This routine is specific
# to sh and is called by the main routine intRtnGet.
#
# SYNOPSIS:
#   sh_intRtnGet <vecBaseAddr> <vector>
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

proc sh_intRtnGet {vecBaseAddr vector} {

    set cpuName [wtxCpuInfoGet -n [lindex [lindex [wtxTsInfoGet] 2] 0]]
    set numVecMax 256   ;# maximum number of vectors

    # check input parameters

    if {$vector < -1 || $vector >= $numVecMax} {
        return -1
    }

    if {$cpuName == "SH7600"} {
        set virtVecBaseAddr [expr $vecBaseAddr]
    } else {
        set virtVecBaseAddr [expr $vecBaseAddr + 0x800]
    }

    # get the vector table
    if [catch "wtxMemRead $virtVecBaseAddr [expr $numVecMax * 4]" vectorsBlock] {
	intVecTableReadError $vectorsBlock
    }

    set vectorTable [memBlockGet -l $vectorsBlock]
    memBlockDelete $vectorsBlock

    set intConnectCode "0x4f222f06 0xd004400b 0x2f16d005 0xd405400b 0x0009d002 0x402b61f6"

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

		if {$i <= 15} {
		    lappend funcAddr "Std Excep. Handler"
		} elseif {$i >=128} {
	    	    lappend funcAddr "Std Trap"
		}
	    }

            "*excIntStub" {
                    lappend funcAddr "Std Int"
            }
 
            "*excBsrTbl" {

                   # SH2 uses an exception table which (for default exception
                   # handlers installed at initialization time, contains an 
                   # sts.l instruction followed by a bsr to the exception 
                   # routine. So, here, we retrieve the four bytes, extract off 
                   # the bsr instruction and then extract the displacement from 
                   # the bsr instruction and reconstruct the offset needed to 
                   # calculate the address of the exception routine.

                   catch "wtxMemRead $vectorInfo 4" codeBlock
                   set code [memBlockGet -l $codeBlock]
                   memBlockDelete $codeBlock
                   set opDisp [expr $code & 0x0fff]

                   # calculate offset taking care to preserve negative values.

                   if {[expr $opDisp & 0x800]} {
                       set offset [expr [expr [expr $opDisp | 0xfffff000] * 2] + 6]
                   } else {
                       set offset [expr [expr $opDisp * 2] + 6]
                   }
                   set objective [expr $vectorInfo + $offset]
                   catch "wtxSymFind -value $objective" routineName 
                   lappend funcAddr [lindex $routineName 0]
            }

	    "*"	{
		if [catch "wtxMemRead $vectorInfo 36" codeBlock] {
		    lappend funcAddr "Corrupted Int"
		    set cont 0
		}


		if {$cont} {
		    set code [memBlockGet -l $codeBlock]
		    memBlockDelete $codeBlock

		    # Test if the associated routine is a stub

		    if {[string match $intConnectCode [lrange $code 0 5]]} {
			set routineAddr [lindex $code 8]
                    } else {
			set routineAddr [lindex $vectorTable $i]
                    }

                    if {$vectorInfo == 0x0c0000a0} {

                        lappend funcAddr "excBErrStub"

		    } else {

                        catch "wtxSymFind -value $routineAddr" rtnInfo
                        set routineName [lindex $rtnInfo 0]
                        set verifyAddr [lindex $rtnInfo 1]
                        if {$verifyAddr == $routineAddr} {
                            lappend funcAddr $routineName
                        } else {
                            lappend funcAddr $routineAddr
                        }

                        # lappend funcAddr [rtnNameGet [lindex $vectorTable $i]]
		    }
		}
	    }
	}
    }
    return $funcAddr
}
