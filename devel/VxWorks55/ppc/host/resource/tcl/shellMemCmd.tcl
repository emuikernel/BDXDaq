# shellMemCmd.tcl - Tcl implementation of WindSh "memory" commands
#
# Copyright 1998-2002 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01o,15jan03,lcs  Integrate SPR#26657 this will fix SPR#76840 replaces
#		   [gets stdin] by [stdinGet].
# 01n,22mar02,fmk  change default to reg32Write in mRegs  
# 01m,16jul01,sru  correct parenthesis on MIPS64 test
# 01l,05jan01,mem  Initial changes for MIPS32/MIPS64
# 01k,10sep99,myz  merged CW4000 ES FCS3 and modified shellproc l for CW4000_16
# 01k,20jan99,dra  Added VR4100, VR5000, VR5400 support.
# 01k,06jun99,dra  added SPARCV9 support.
# 01j,15dec98,fle  fix SPR#23722, turn off the SYMBOL + OFFSET disassembling
#                  mode
# 01i,03dec98,jmp  added shConfig LD_COMMON_MATCH_ALL (Workaround for
#                  SPR#23385).
# 01h,27nov98,fle  fixed a bug in error returned value
# 01g,28oct98,jmp  fixed taskIdDefault handling in mRegs{}.
# 01f,13oct98,jmp  modified string allocation method in WindSh.
# 01e,14sep98,fle  changed l command since new wtxMamDisassemble output format
# 01d,10jul98,fle  fixed a typo
# 01c,24jun98,fle  asynchronous load implementation
# 01b,26may98,pcn  Changed wtxObjModuleLoad_2 in wtxObjModuleLoad.
# 01a,01apr98,jmp  created based on routines moved out from shell.tcl
#

# DESCRIPTION
# This module holds the "memory" shellproc of WindSh:
#
# d - dump memory.  
# l - list (disassemble) memory command
# ld - load an object module
# lkAddr - look-up a symbol by address
# lkup - look-up a symbol by name
# m - modify memory command
# mRegs - modify registers
# unld - unload an object module
#
# This module also contains windsh private routines used by the
# "memory" shellprocs
#

##############################################################################
#
# d - dump memory.  
# 
# Takes three optional arguments: the base address,
# the number of "words" to show, and the size of each word (which may
# be 1, 2, or 4).  Parameters not specified have default values (which
# are updated when those parameters are specified).
#
set dSavedBase 0        ; # default address
set dSavedCount 128     ; # default nunits
set dSavedSize 2        ; # default unit size

shellproc d {args} {
    global dSavedBase
    global dSavedCount
    global dSavedSize

    bindNamesToList {base count size} $args

    if {$base == 0} {set base $dSavedBase}
    if {$count == 0} {set count $dSavedCount}
    if {$size == 0} {set size $dSavedSize}

    # Eliminate bogus sizes and counts.

    if {$size != 1 && $size != 2 && $size != 4} {
        set size 2
    }

    if {$count < 0} {
        set count 128
    }

    # build the dumped lines list

    set lineList [memoryDump $base $count $size]

    # save the new default parameters

    # set dSavedBase [lvarpop lineList end]
    set length [llength $lineList]
    set dSavedBase [lindex $lineList [expr $length - 1]]
    set lineList [lrange $lineList 0 [expr $length -2]]

    set dSavedSize $size
    set dSavedCount $count

    # display results

    foreach line $lineList {
        puts stdout $line
    }

    return 0
}

##############################################################################
#
# l - list (disassemble) memory command
#

set lSavedBase 0x0	; # default l address base
set lSavedNLines 10	; # default number of disassembled lines

shellproc l {args} {
    global lSavedBase
    global lSavedNLines

    bindNamesToList {base nLines} $args

    if {$base == 0} {
        set base $lSavedBase
    }

    if {$base == 0} {
	puts stdout "supply an address to disassemble"
	return -1
    }

    if {$nLines == 0} {
        set nLines $lSavedNLines
    }

    # the returned value of wtxMemDisassemble is a string containing
    # disassembled instructions. All instructions are separated by "\n"
    # caracters.

    if { [lindex [shConfig DSM_HEX_MODE] 2] == "off" } {
	set dsmList [wtxMemDisassemble -address -opcodes $base $nLines]
    } else {
	set dsmList [wtxMemDisassemble -address -opcodes -hex $base $nLines]
    }

    set dsmList [split $dsmList \n]

    # get rid of last \n empty list member

    set length [llength $dsmList]
    set dsmList [lrange $dsmList 0 [expr $length - 2]]

    # get the last address in list to reach the next instruction's address

    set lastInst [lindex $dsmList end]
    set lastAddr [lindex $lastInst 2]
    set lastInstSize [lindex $lastInst 1]

    if { ! [string match 0x* $lastAddr] } {
	set lastAddr 0x$lastAddr
    }

    # set parameters for next call to l command

    set lSavedBase [expr $lastAddr + $lastInstSize]
    set lSavedNLines $nLines

    # format lines

    foreach line $dsmList {

	if { [set symbolName [lindex $line 0]] != {} } {
	    puts stdout $symbolName
	}

	set formattedLine [format "    %8.8s  %-24.24s %s" [lindex $line 2] \
							  [lindex $line 3] \
							  [lindex $line 4]]
	puts stdout $formattedLine
    }

    return 0
}

##############################################################################
#
# ld - load an object module
#
# This procedure loads an object module on the Target.
#
# SYNOPSIS:
# ld options filename
# or 
# ld < filename
#
# PARAMETER: 
# options:	0 - Add global symbols to the system symbol table.
#		1 - Add global and local symbols to the system symbol table.
#		-1 - Add no symbols to the system symbol table.
#
# filename: file containing object module
#
# RETURNS: module identifier or ERROR
#
# ERRORS: 
#

shellproc ld {args} {

    global shellConfig	;# Windsh environment table

    set ldOptions [lindex $args 0]
    set stringAddr [lindex $args 2]
    set ld_send ""
    set loadProgressing { "|" "/" "-" "\\" }

    # We ignore the second argument (noAbort) because we can't 
    # do anything with it presently.

    # map between the options that tradional VxWorks ld() uses and
    # the options to the WTX Tcl call.

    case [expr $ldOptions] {
	0	{set realLdOptions LOAD_GLOBAL_SYMBOLS}
	1	{set realLdOptions LOAD_ALL_SYMBOLS}
	-1	{set realLdOptions LOAD_NO_SYMBOLS}
	*	{set realLdOptions $ldOptions}
    }

    #
    # Use LOAD_CPLUS_XTOR flag according to shellConfig(LD_CALL_XTORS)
    #
    if {$shellConfig(LD_CALL_XTORS) != "target" } {
	if {$shellConfig(LD_CALL_XTORS) == "on"} {
	    set realLdOptions "$realLdOptions|LOAD_CPLUS_XTOR_AUTO"
	} elseif {$shellConfig(LD_CALL_XTORS) == "off"} {
	    set realLdOptions "$realLdOptions|LOAD_CPLUS_XTOR_MANUAL"
	}
    }

    #
    # Use COMMON_MATCH_ALL flag according to shellConfig(LD_COMMON_MATCH_ALL)
    #
    if {$shellConfig(LD_COMMON_MATCH_ALL) == "on"} {
	set realLdOptions "$realLdOptions|LOAD_COMMON_MATCH_ALL"
    }

    if {$stringAddr != 0} {
	# strip off braces from a path possibly containing Win32 backslashes
	set string [stringGet $stringAddr]

	# if string doesn't start with a /, prepend the windsh's
	# current working directory.

	if {! [string match "/*" $string]} {
	    if { ! [string match "\\*" $string]} {
		if {! [string match "*:*" $string]} {
		    if [file exists [pwd]/$string] {
		    	set string "[pwd]/$string"
		    }
		}
	    }
	}

	if {![file exists $string]} {
	    # Have a look to LD_PATH variable
	    set pathList [split $shellConfig(LD_PATH) ";"]

	    foreach pathName $pathList {
	    	if [file exists $pathName/$string] {
		    # module found in LD_PATH, break this loop and load it
		    set string "$pathName/$string"
		    break;
		}
	    }
	}

	# Have a look to LD_SEND_MODULES to determine if we must use -ts

	if {$shellConfig(LD_SEND_MODULES) == "off"} {
	    set ld_send "-ts"			;# file sent to target server
	} else {
	    set ld_send "-t"			;# file sent to target
	}
	
	# begin asynchronous load

	set realLdOptions [wtxEnumFromString LOAD_FLAG $realLdOptions]
	set loadProgressNum 0

	set isLoading 1
	puts -nonewline stdout "Loading $string  "

	if { [ldStart $ld_send $realLdOptions $string] == "OK" } {

	    while { $isLoading } {
		
		# make the cursor turn until first element of returned loadInfo
		# is DONE

		set loadInfo [ldStatusGet]		;# get load progress
		set loadStatus [lindex $loadInfo 0]	;# get first elt

		# in case of load error, just print error number and go out

		if { $loadStatus == "ERROR"} {
		    puts stdout "\n[lindex $loadInfo 1]"
		    return -1
		}

		if { $loadStatus == "DONE" } {

		    # load operation DONE, set load info to all elements but
		    # first which is the load status

		    puts stdout ""			;# new line
		    set loadInfo [lrange $loadInfo 1 end]
		    set isLoading 0
		}

		if { $loadStatus == "BREAK" } {		;# try to cancel load

		    puts -nonewline stdout [format "%c%c\nCancelling load\
						    operation ... " 8 8]

		    if { [ldCancel] == "OK" } {		;# load canceled

			puts stdout "succeeded"
			set isLoading 0

		    } else {				;# can not cancel

			puts stdout "failed"
			puts -nonewline stdout "Loading $string  "
		    }
		}

		if { $loadStatus == "PROGRESS" } {	;# still in progress

		    after 100
		    puts -nonewline stdout [format "%c%s" 8 \
						   [lindex $loadProgressing \
							   $loadProgressNum]]

		    set loadProgressNum [expr ($loadProgressNum + 1) % 4]
		}
	    }
	}

	# return lod info result

	if {[llength $loadInfo] > 4} {
	    puts stdout "Undefined symbols:"
	    for {set ix 4} {$ix < [llength $loadInfo]} {incr ix} {
		puts stdout [shDemangle [lindex $loadInfo $ix]]
	    }
	    puts stdout "Warning: object module may not be usable because of \
			 undefined symbols."
	}
	return [lindex $loadInfo 0]

    } else {
	puts stdout "Missing filename to load."
	return -1
    }
    return 0
}

##############################################################################
#
# lkAddr - look-up a symbol by address
#

shellproc lkAddr {args} {
    set addr [lindex $args 0]
    set symList [wtxSymListGet -value $addr]

    foreach sym $symList {
	symPrint $sym
    }
}

##############################################################################
#
# lkup - look-up a symbol by name
#

shellproc lkup {args} {
    set stringAddr [lindex $args 0]
    set string [stringGet $stringAddr]

    # wtxGopherEval might have braced the string if it contained 
    # spaces or other characters significant to Tcl.  We must rip
    # off the outer layer of bracing to let original string through.

    set symList [wtxSymListGet -name [lindex $string 0]]

    foreach sym $symList {
	symPrint $sym
    }
    return 0
}

##############################################################################
#
# m - modify memory command
#

set mSavedBase 0x1000
set mSavedSize 2

shellproc m {args} {
    global mSavedBase
    global mSavedSize

    bindNamesToList {base size} $args

    if {$base == 0} {set base $mSavedBase}
    if {$size == 0} {set size $mSavedSize}

    # Eliminate bogus sizes and counts.

    if {$size != 1 && $size != 2 && $size != 4} {
	set size 2
    }

    # round off the base to the appropriate boundary, downward.

    switch [expr $size] {
	1 {
	    set rbase [expr $base]
	    set sizeFlag "-b"
	    set format "%02x"
	}
	2 {
	    set rbase [expr $base & ~ 0x1]
	    set sizeFlag "-w"
	    set format "%04x"
	}
	4 {
	    set rbase [expr $base & ~ 0x3]
	    set sizeFlag "-l"
	    set format "%08x"
	}
    }

    set done 0
    set addr $rbase

    while {! $done} {

	# Fetch a little memory block with the old value in it.

	set memBlock [wtxMemRead $addr $size]
	set value [memBlockGet $sizeFlag $memBlock 0]

	# Prompt with the address and value in the proper format.

	puts stdout [format "%08x:  $format-" $addr $value] nonewline

	set newValue [stdinGet]

	if {$newValue == "."} {
	    # dot means bail out
	    set done 1
	    memBlockDelete $memBlock
	    continue
	}

	if {$newValue == ""} {
	    # nothing means don't modify the target memory
	    # and step to the new address

	    incr addr $size
	    memBlockDelete $memBlock
	    continue
	}

	# try to scan an number.  If we get one, store it
	# to the target.  If we don't, assume the user wants
	# to quit.

	if {[scan $newValue "0x%x" val] != 1} {
	    if {[scan $newValue "%x" val] != 1} {
		set done 1
		memBlockDelete $memBlock
		continue
	    }
	}

	# We got a number, one way or another, in "val".  Store it.

	memBlockSet $sizeFlag $memBlock 0 $val
	wtxMemWrite $memBlock $addr
	memBlockDelete $memBlock

	incr addr $size
    }

    # Save parameters for the next time through

    set mSavedBase $addr
    set mSavedSize $size
    return 0
}

##############################################################################
#
# mRegs - modify registers
#
# This command modifies the specified register for the specified task.
# If <taskNameOrId> is omitted or zero, the last task referenced is assumed.
# If the specified register is not found, it prints out the valid register list
# and returns ERROR.  If no register is specified, it sequentially prompts
# the user for new values for a task's registers.  It displays each register
# and the current contents of that register, in turn.  The user can respond
# in one of several ways:
#       RETURN
#         Do not change this register, but continue, prompting at the next
#         register.
#       <number>
#         Set this register to <number>.
#       ". (dot)"
#         Do not change this register, and quit.
#       EOF
#         Do not change this register, and quit.
#
# All numbers are entered and displayed in hexadecimal.
#       input like 12 or 0x12 equals 18 in decimal
#       input like -12 or -0x12 equals -18 in decimal
# Flotting point registers are not supported
#
# SYNOPSIS:
# mRegs [register] [taskNameOrId]
#

shellproc mRegs {args} {
    global cpuType
    global __wtxCpuType
    global regNList
    global defaultTaskId
    global cpuFamily
    set tid -1

    # mRegs sub-routine
    proc mRegsUsagePrint {} {
	puts stdout "usage: mRegs \[register\] \[tid\]"
	availRegPuts
    }

    # examine whether 1st and 2nd arguments are task id or register name

    # get arguments (max is 2, so arg2 must be = 0
    set arg0 [stringGet [lindex $args 0]]
    set arg1 [stringGet [lindex $args 1]]
    set arg2 [stringGet [lindex $args 2]]

    # test if we got more than 2 arguments
    if {$arg2 != 0} {
	mRegsUsagePrint
	return -1
    }

    # arg1 will always contain the taskName
    if {$arg1 != 0} {
	if {[set tid [[shellName taskIdFigure] $arg1]] == -1} {
	    mRegsUsagePrint
	    return -1
	}
    }

    if {$arg0 != "0"} {
    	
	if {$tid != -1} {
	    # we already found the taskName so we are looking for
	    # the registerName
	    if {[lsearch -regexp $regNList $arg0] != -1} {
	    	set regName $arg0
	    } else {
		mRegsUsagePrint
		return -1
	    }

	} else {

	    # arg0 can be either taskName or registerName
	    set tid 0
	    set res [catch {set tid [[shellName taskIdFigure] $arg0]}]
	    if {$res == 1 || $tid == -1} {

	    	# arg0 is probably the registerName
	    	if {[lsearch -regexp $regNList $arg0] != -1} {
		    set regName $arg0
		    set tid [taskIdDefault]
		} else {
		    mRegsUsagePrint
		    return -1
		}
	    }
	}
    }

    if {$tid == -1} {
    	set tid [taskIdDefault]
    } else {
	set defaultTaskId $tid
    }

    if {![info exists regName]} {
	set regName $regNList
    }

    # get the register values in a list

    if {($cpuType($__wtxCpuType) == "MIPS64") ||
        ($cpuType($__wtxCpuType) == "ULTRASPARC")} {
        set regVList [reg64VListGet $tid]
        set regFormat "%-6s : %16s - "
    } else {
        set regVList [regSetOfContext CONTEXT_TASK $tid]
        set regFormat "%-6s : %08x - "
    }

    foreach reg $regName {
        if {([set regInd [lsearch -exact $regNList $reg]] < 0) &&
            ([set regInd [lsearch -regexp $regNList $reg]] < 0) &&
            ([set regInd [lsearch -glob $regNList $reg]] < 0)} {
            availRegPuts
            return -1
        }

	# skip any register named '-'
	if {$reg == "-"} {
	    continue
	}

        # cause and fpscr ignored - not returned by wtxRegsGet{}.
        if {($cpuFamily($__wtxCpuType) == "mips") &&
            (([string compare $reg "cause"] == 0) ||
             ([string compare $reg "fpscr"] == 0))} {
            continue
        }

	# COLDFIRE FAMILY - hasmac always ignored (internal info only!)
        if {($cpuFamily($__wtxCpuType) == "coldfire") &&
	    ([string compare $reg "hasmac"] == 0)} {
            continue
        }

	# MCF5200 CPU - mac, macsr and mask ignored unless hasmac != 0
	if {($cpuType($__wtxCpuType) == "MCF5200") &&
	    ([lindex $regVList 19] == 0) &&
            (([string compare $reg "acc"] == 0) ||
             ([string compare $reg "macsr"] == 0) ||
             ([string compare $reg "mask"] == 0))} {
            continue
        }

        # display the current value of the register

        if {($cpuType($__wtxCpuType) == "MIPS64")||
            ($cpuType($__wtxCpuType) == "ULTRASPARC")} {
	    puts -nonewline stdout [format $regFormat $reg \
			[string range [lindex $regVList $regInd] 2 end]]
	} else {
	    puts -nonewline stdout [format $regFormat $reg \
				    [lindex $regVList $regInd]]
	}

        # get the new value and interpret it

        set newValue [stdinGet]
        if {$newValue == "."} {
            break
        } elseif {$newValue == ""} {
            continue
        }

        # remove possible -0x

        if {[string index $newValue 0] == "-"} {set neg 1} else { set neg 0}
        set start [string range $newValue $neg [expr $neg + 1]]
        if {[string compare $start "0x"] == 0} {
            set hexaValue [string range $newValue [expr $neg + 2] end]
        } else {
            set hexaValue [string range $newValue $neg end]
        }

        # split the value in 2 32 bits words

        if {[string length $hexaValue] <= 8} {
            set wordL $hexaValue
            set wordH 0
        } elseif {[set len [string length $hexaValue]] <= 16} {
            set wordL [string range $hexaValue [expr $len - 8] end]
            set wordH [string range $hexaValue 0 [expr $len - 9]]
        } else {
            return -1
        }

        # check each word is valid

        set wordV {}
        foreach word {$wordH $wordL} {
            set val [format [expr $neg ? {"-0x%s"} : {"0x%s"}] $word]
            if [catch "expr $val" val] {
                return -1
            }
            lappend wordV $val
        }

	switch $cpuType($__wtxCpuType) {
	    MCF5200 -
	    MCF5400 {
		reg32Write $tid $regInd [lindex $wordV 1]
	    }
	    MIPS64 {
		if {$regInd < 2} {
		    reg32Write $tid $regInd [lindex $wordV 1]
		} else {
		    reg64Write $tid $regInd [lindex $wordV 0] [lindex $wordV 1]
		}
	    }
	    ULTRASPARC {
		if {$regInd < 8} {
		    reg32Write $tid $regInd [lindex $wordV 1]
		} else {
		    reg64Write $tid $regInd [lindex $wordV 0] [lindex $wordV 1]
		}
	    }
	    default {
		reg32Write $tid $regInd [lindex $wordV 1]
	    }
	}
    }
    return 0
}

##############################################################################
#
# unld - unload an object module
#
# SYNOPSIS:
#   unld modNameOrId
#
# PARAMETERS:
#   modNameOrId:	either a module ID, or a string containing a mod name
#
# RETURNS:
#   0 if sucessful, -1 otherwise.
#

shellproc unld {args} {
    bindNamesToList {modNameOrId} $args

    set modId [[shellName moduleIdFigure] $modNameOrId]

    if {$modId} {
	wtxObjModuleUnload $modId
	return 0
    }
    return -1
}


##############################################################################
#
# Locals Procedures (Procedures used by the shellprocs defined in this file)
#

###############################################################################
#
# symPrint - support routine for lkAddr
#

proc symPrint {sym} {
    # format the name, value, type and symbol-table-of-origin.

    set rawType [lindex $sym 2]

    set type [expr {($rawType >> 4) & 0xf}]

    if {$type == 1} {
        set section "comm"
    } else {
        set type [expr {($rawType >> 1) & 0xf}]

        switch $type {
            1           {set section "abs"}
            2           {set section "text"}
            3           {set section "data"}
            4           {set section "bss"}
            default     {set section "????"}
        }
    }

    puts stdout [format "%-25s 0x%08x %-8s (%s)" \
	    [shDemangle [lindex $sym 0]] \
	    [lindex $sym 1] \
	    $section \
	    [lindex $sym 5]]
}

##############################################################################
#
# memoryDump - dump memory 
# 
# SYNOPSIS:
# memoryDump addr nunits width
#
# PARAMETERS:
# addr: address to display
# nunits: number of unit to display
# width of displaying unit
#
# RETURNS: a list of the lines to display followed by the next addr
#          { {line1} {line2} ... {addr} }
#
# ERROR:
# "Odd size"
#

proc memoryDump {base count size} {

    # round the base down to a multiple of the width

    switch [expr $size] {
        2 { set base [expr $base & ~1]}
        4 { set base [expr $base & ~3]}
    }

    # compute total number of bytes.

    set nbytes [expr $count * $size]

    # round off the base to a 16-byte boundary, downward

    set rbase [expr $base & ~ 0xf]
    set rnbytes [expr $nbytes + ($base - $rbase)]

    # compute the final address.  We must go at least as far as
    # base + count * size

    set end [expr $base + $nbytes]

    # precompute the number of items to appear on each line, and a
    # format string.

    case [expr $size] {
        1 {
            set perLine 16
            set sizeFlag "-b"
            set fmtString "%02x "
            set blank "   "
        }
        2 {
            set perLine 8
            set sizeFlag "-w"
            set fmtString "%04x "
            set blank "     "
        }
        4 {
            set perLine 4
            set sizeFlag "-l"
            set fmtString "%08x "
            set blank "         "
        }
        * {
            error "Odd size"
        }
    }
    # loop over 16-byte blocks.  Print those (bytes, words, or longs)
    # found in the intersection of the 16-byte block and the requested
    # memory range in hex and printable ASCII

    for {set offset 0} {$offset < $rnbytes} {incr offset 16} {
        set ptr [expr $offset + $rbase]

        # fetch the data to a memory block... up to 16 bytes.
        set memBlock [wtxMemRead $ptr \
                        [expr ($end - $ptr) > 16 ? 16 : ($end - $ptr)]]

        append line [format "%08x:  " $ptr]

        # loop over the data inside.
        for {set i 0} {$i < $perLine} {incr i} {
            set suboffset [expr $i * $size]
            set addr [expr $ptr + $suboffset]
            if {$addr + ($size - 1) >= $base && $addr < $end} {
                append line [format $fmtString \
                             [memBlockGet $sizeFlag $memBlock $suboffset 1]]
            } else {
                append line $blank
            }
	}

        # accumulate the ASCII part.
        append line "  *"
        for {set i 0} {$i < 16} {incr i} {

            # ASCII character set dependence here.
            if {$ptr + $i < $base || $ptr + $i >= $end} {

                # outside the range? change it to a space.
                set byte 0x20
            } else {
                set byte [expr [memBlockGet -b $memBlock $i 1]]
                if {$byte < 0x20 || $byte > 0x7e} {

                    # not printable? change it to a period.
                    set byte 0x2e
                }
            }
            append line [format "%c" $byte]
        }
        append line "*"
        memBlockDelete $memBlock
        lappend lineList $line
        set line ""
    }
    lappend lineList $end
    return $lineList
}
