# wtxMemDisassembleTest.tcl - Tcl script, test wtxMemDisassemble
#
# Copyright 1997-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01d,06nov01,jhw  Changed invalid address calculation to use invalidAddrGet
# 01c,29sep98,p_b adapted to last wtxMemDisassemble changes. 
# 01b,28jan98,p_b adapted for WTX 2.0.
# 01a,05nov97,fle written.
#

#
# DESCRIPTION
#
# WTX_MEM_DISASSEMBLE - disassemble a part of memory.

################################################################################
#
# wtxMemDisassembleTest1 - valid address test
#
# Verify wtxMemDisassemble gives something back on a valid address
# As far as the <validAddr> parameter is the address of the <validName> symbol,
# we check that <validName> is part of the result.
#
# SYNOPSIS:
#   wtxMemDisassembleTest1 Msg validAddr validName
#
# PARAMETERS:
#   Msg : the message to display when entering this procedure
#   validAddr : a valid address to disassemble
#   validName : the symbol name associated to <validAddr>
#
# RETURNS: N/A
#
# ERRORS: N/A
#
# WARNINGS : If symbol name is mangled, this routine could FAIL
#

proc wtxMemDisassembleTest1 {Msg validAddr validName} {

    global errMemMsg

    puts stdout "$Msg" nonewline

    # get disassembled instructions list

    set disassembledList [wtxMemDisassemble $validAddr 1]

    if {$disassembledList == ""} {
        testFail $errMemMsg(33)
        return
        }

    # check if <validName> symbol name is part of the result

    if { ! [regsub $validName $disassembledList $validName disassembledList] } {
        testFail $errMemMsg(34)
        return
        }

    wtxTestPass
}

################################################################################
#
# wtxMemDisassembleTest2 - invalid address test
#
# Verify wtxMemDisassemble error status on invalid address request.
# Use WTX Test Library function invalidAddrGet to determine an invalid addr. 
#
# SYNOPSIS:
#   wtxMemDisassembleTest2 Msg validAddr invalidAddr
#
# PARAMETERS:
#   Msg : the message to display when entering this procedure
#   validAddr : a valid address to start with to find an invalid one
#   invalidAddr : a non-readable memory address
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc wtxMemDisassembleTest2 {Msg validAddr invalidAddr} {

    global errMemMsg
 
    # if the test suite has been run with the -SUPPRESS option

    if {$invalidAddr == "SUPPRESS"} {
        return
    }

    puts stdout "$Msg" nonewline

    # use the WTX Test Library to get an invalid address
    set invalidAddr [invalidAddrGet]

    # wtxMemDisassemble <startAddr> equal to [invalidAddr]
    # if no exception occured

    if {! [catch "wtxMemDisassemble $invalidAddr" status]} {
        testFail $errMemMsg(33)
        return
    }

    # if the caught exception is different than the expected error code

    if { [wtxErrorName $status] != "AGENT_MEM_ACCESS_ERROR" } {
        testFail $status
        return
    }

    # if the caught exception is the expected error code

    wtxTestPass
}

################################################################################
#
# wtxMemDisassembleTest3 - <-address> option test
#
# Verify the <-address> option of wtxMemDisassemble. For that, check for 12
# symbols address (wtxSymListGet returns a default list of 12 symbols with the
# <-value> option), that the address given in the disassembled list is the
# address of the symbol.
#
# SYNOPSIS:
#   wtxMemDisassembleTest3 Msg validAddr
#
# PARAMETERS:
#   Msg : the message to display when entering this procedure
#   validAddr : a valid address to disassemble
#
# RETURNS: N/A
#
# ERRORS: N/A
#
 
proc wtxMemDisassembleTest3 {Msg validAddr} {
 
    global errMemMsg
 
    puts stdout "$Msg" nonewline

    # get a list of addresses

    set symList [wtxSymListGet -value $validAddr]

    set addrList ""

    foreach oneSym $symList {
        lappend addrList [lindex $oneSym 1]
    }
 
    # call for wtxMemDiasseamble with the <-address> option

    foreach addr $addrList { 

        set disassembledInstList [wtxMemDisassemble -address $addr 1]

        if { $disassembledInstList == "" } {
            testFail errMemMsg(33)
            return
        }

        # get address of instruction 

        set disassembledAddr [lindex $disassembledInstList 2]

        # prepend 0x formater if necessary

        if {! [string match 0x* $disassembledAddr]} {
            set disassembledAddr 0x$disassembledAddr
        }

        # compare addresses

        set addr [string tolower $addr]
        set disassembledAddr [string tolower $disassembledAddr]

        if { [expr $addr - $disassembledAddr] != 0 } {
            testFail $errMemMsg(35)
            return
        }
    }

    wtxTestPass
}

################################################################################
#
# wtxMemDisassembleTest4 - <-opcode> option test
#
# Verify the <-opcodes> option of wtxMemDisassemble. For that, just check that
# the first digits of the opcodes are equal to the first digits of the
# memory block associated with the disassembled address.
#
# SYNOPSIS:
#   wtxMemDisassembleTest4 Msg validAddr
#
# PARAMETERS:
#   Msg : the message to display when entering this procedure
#   validAddr : a valid address to disassemble
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc wtxMemDisassembleTest4 {Msg validAddr} {

    global errMemMsg
    puts stdout "$Msg" nonewline

    # call for wtxMemDiasseamble with the -opcodes option

    set disassembledInstList [wtxMemDisassemble -address -opcodes $validAddr 1]
    if { $disassembledInstList == ""} {
        testFail #errMemMsg(33)
        return
    }

    # get rid of the symbol name then reach opcodes

    set opcodes [lindex $disassembledInstList 3]
    set nBytesToRead [expr [string length $opcodes] / 2]

    # look for a memory block

    set blk [wtxMemRead $validAddr $nBytesToRead]
    set memVal [memBlockGet -b $blk]

    # care for endianness

    set inverseMemVal ""

    if { [lindex [lindex [wtxTsInfoGet] 2] 4] == 4321 } {	;# endian = 4321
        foreach byte $memVal {
            set inverseMemVal [linsert $inverseMemVal 0 $byte]
        }

        set memVal $inverseMemVal
    }

    # get rid of the 0x prefix and of spaces

    regsub -all 0x $memVal {} memVal
    regsub -all { } $memVal {} memVal
    regsub -all { } $opcodes {} opcodes

    # get rid of the case problem

    set opcodes [string tolower $opcodes]
    set memVal [string tolower $memVal]

    if { $opcodes != $memVal } {
        testFail $errMemMsg(36)  
        puts stdout "$opcodes $memVal"
        return
        }

    wtxTestPass
}

################################################################################
#
# wtxMemDisassembleTest5 - nInst variable accuracy test
#
# Validate the accuracy of the nInst parameter for different values between 0
# and 1500.
#
# SYNOPSIS:
#   wtxMemDisassembleTest5 Msg validAddr
#
# PARAMETERS:
#   Msg : the message to display when entering this procedure
#   validAddr : a valid address to disassemble
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc wtxMemDisassembleTest5 {Msg validAddr} {
 
    global errMemMsg
 
    puts stdout "$Msg" nonewline

    set defaultNInst 10

    # get a bigger timeout

    set oldTimeout [wtxTimeout]
    wtxTimeout 1500

    set nInstList {0 1 2 3 4 5 7 10 20 40 70 100 250 300 300 300}

    foreach nInst $nInstList {
        set disassembledInstList [wtxMemDisassemble $validAddr $nInst]

        if { $disassembledInstList == "" } {
            testFail errMemMsg(33)
            wtxTimeout $oldTimeout
            return
        }

        # get rid of last \n that could cause problems
        # then get rid of symbol names by looking if instruction begins with
        # four spaces

        set disassembledInstList [split $disassembledInstList \n]
        set length [expr [llength $disassembledInstList] - 2]
        set disassembledInstList [lrange $disassembledInstList 0 $length]

        set nDisassembledInst 0

        foreach inst $disassembledInstList {
            set splitInst [split $inst " "]
            if { [lindex $splitInst 0] == "" &&
                 [lindex $splitInst 1] == "" &&
                 [lindex $splitInst 2] == "" &&
                 [lindex $splitInst 3] == "" } {
                incr nDisassembledInst
                }
        }

        # check if number of instructions are similar

        if { $nInst != $nDisassembledInst } {
            if { $nInst != 0 || $nDisassembledInst != $defaultNInst} {
                testFail $errMemMsg(37)
                wtxTimeout $oldTimeout
                return
            }
        }
    }

    wtxTimeout $oldTimeout

    wtxTestPass
}

################################################################################
#
# wtxMemDisassembleTest6 - <endAddr> variable test
#
# Validate use of the <endAddr> parameter, checking in the same time that if
# <endAddr> is specified, <nInst> doesn't matter.
#
# SYNOPSIS:
#   wtxMemDisassembleTest6 Msg validAddr
#
# PARAMETERS:
#   Msg : the message to display when entering this procedure
#   validAddr : a valid address to disassemble
#
# RETURNS: N/A
#
# ERRORS: N/A
#
 
proc wtxMemDisassembleTest6 {Msg validAddr} {
 
    global errMemMsg
 
    puts stdout "$Msg" nonewline

    # getting two valid addresses for <startAddr> and <endAddr>

    set symList [wtxSymListGet -value $validAddr]
    set startAddr [lindex [lindex $symList 0] 1]
    set endAddr [lindex [lindex $symList end] 1]
    # get disassembled instruction between <startAddr> and <endAddr>

    if { [catch {wtxMemDisassemble -address $startAddr 1 $endAddr} disassembledInstList] } {
        testFail errMemMsg(33)
        return 0
    }

    if { $disassembledInstList == "" } {
        testFail errMemMsg(33)
        return
    }

    # get the last address in list to reach the next instruction's address

    set length [expr [llength $disassembledInstList] / 5]
    set lastAddr [lindex $disassembledInstList\
        [expr [expr [expr $length - 1] * 5 ] + 2]]

    if { ! [string match 0x* $lastAddr] } {
        set lastAddr 0x$lastAddr
    }

    set nextAddr [nextAddrGet $lastAddr]

    if { $lastAddr > $endAddr ||
         $nextAddr < $endAddr } {

        testFail $errMemMsg(38)
        return
    }

    wtxTestPass
}

################################################################################
#
# wtxMemDisassembleTest7 - not-readable <endAddr> variable test
#
# test result for an <endAddr> value out of memory access. For the disassembler
# not to disassemble to many lines, the <invalidAddr> is changed until there is
# no more than 1000 between a <validAddr> and the <invalidAddr>.
#
# SYNOPSIS:
#   wtxMemDisassembleTest7 Msg validAddr invalidAddr
#
# PARAMETERS:
#   Msg : the message to display when entering this procedure
#   validAddr : a valid address to disassemble
#   invalidAddr : a non-readable memory address
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc wtxMemDisassembleTest7 { Msg validAddr invalidAddr} {

    global errMemMsg

    # if the test suite has been run with the -SUPPRESS option

    if {$invalidAddr == "SUPPRESS"} {
        return
    }

    # beginning test by displaying message
 
    puts stdout "$Msg" nonewline

    set stepList {100000 10000 1000}

    foreach step $stepList {

        set invalidAddr 0

        while { $invalidAddr == 0 } {
            set validAddr [expr $validAddr + $step]

            if { [catch "wtxMemRead $validAddr 10" status] } {
                if { [wtxErrorName $status] == "AGENT_MEM_ACCESS_ERROR" } {
                    set invalidAddr $validAddr
                }
            }
        }

        set validAddr [expr $invalidAddr - $step]
    }

    if {! [catch "wtxMemDisassemble $validAddr 0 $invalidAddr" status]} {
        testFail $errMemMsg(33)
        return
    }
 
    # if the caught exception is different than the expected error code
 
    if { [wtxErrorName $status] != "AGENT_MEM_ACCESS_ERROR" } {
        testFail $status
        return
    }
 
    # if the caught exception is the expected error code
 
    wtxTestPass
}

################################################################################
#
# wtxMemDisassembleTest8 - smaller than <startAddr> <endAddr> test
#
# Verify that wtxMemDisassemble really returns "" on reading instructions
# between <startAddr> and <endAddr> with <endAddr> smaller than <startAddr>
#
# SYNOPSIS:
#   wtxMemDisassembleTest8 Msg validAddr
#
# PARAMETERS:
#   Msg : the message to display when entering this procedure
#   validAddr : a valid address to disassemble
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc wtxMemDisassembleTest8 { Msg validAddr } {

    global errMemMsg

    puts stdout "$Msg" nonewline

    # getting endAddr < validAddr

    set endAddr [expr $validAddr - 1]

    set disassembledInstList [wtxMemDisassemble $validAddr 0 $endAddr]
    if { $disassembledInstList != "" } {
        testFail errMemMsg(33)
        return
    }

    wtxTestPass
}

################################################################################
#
# wtxMemDisassembleTest - calling procedure for several wtxMemDisassemble tests
#
# call the test scripts
#
# SYNOPSIS:
#   wtxMemDisassembleSmallerEndAddrTest tgtsvr timeout invalidAddr
#
# PARAMETERS:
#   Msg : the message to display when entering this procedure
#   validAddr : a valid address to disassemble
#   invalidAddr : equal SUPPRESS if invalid addresses are not supported
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc wtxMemDisassembleTest {tgtsvrName timeout invalidAddr} {

    global memDisassembleMsg

    if { [llength $tgtsvrName] == 0 } {
        puts stdout "usage : wtxMemDisassembleTest targetServerName\
        timeout invalidAddr"
        return
    }

    set serviceType 2

    # connection to the target server

    wtxToolAttachCall $tgtsvrName $serviceType $timeout
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    # look for a valid address, and its valide name, and an invalid address
    # We assume that sysClkConnect is always there, in vxWorks.

    set validName [lindex [lindex [wtxSymListGet -name sysClkConnect] 0] 0]
    set validAddr [lindex [lindex [wtxSymListGet -name sysClkConnect] 0] 1]

    # call for the test procedures
    wtxMemDisassembleTest1 $memDisassembleMsg(1) $validAddr $validName

    wtxErrorHandler [lindex [wtxHandle] 0] ""
    wtxMemDisassembleTest2 $memDisassembleMsg(2) $validAddr \
                                     $invalidAddr
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    wtxMemDisassembleTest3 $memDisassembleMsg(3) $validAddr
    wtxMemDisassembleTest4 $memDisassembleMsg(4) $validAddr
    # wtxMemDisassembleTest5 $memDisassembleMsg(5) $validAddr
    wtxMemDisassembleTest6 $memDisassembleMsg(6) $validAddr

    wtxErrorHandler [lindex [wtxHandle] 0] ""
    wtxMemDisassembleTest7 $memDisassembleMsg(7) $validAddr \
                                      $invalidAddr
    wtxErrorHandler [lindex [wtxHandle] 0] errorHandler

    wtxMemDisassembleTest8 $memDisassembleMsg(8) $validAddr

    # deconnection

    wtxToolDetachCall "wtxMemDisassembleTest"
}

