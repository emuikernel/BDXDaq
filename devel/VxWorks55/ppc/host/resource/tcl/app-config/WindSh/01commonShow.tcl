# 01commonShow.tcl - Implementation of convenience routines for show routines
#
# Copyright 1998-2001 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01c,26oct01,fmk  fix SPR 70632 - sentiListCut does not parse sentinels
#                  correctly
# 01b,02dec98,dbt  removed useless argsConvert().
# 01a,15jun98,f_l  written.

# DESCRIPTION
# This module is the Tcl code for some convenience routines that are called
# from Windsh show routines.
#
# RESOURCE FILES
#

##############################################################################
#
# ifAddrFormat - format an IP address
#
# This routine formats 4 bytes into an IP address.
#
# SYNOPSIS:
# ifAddrFormat octetList
# 
# PARAMETERS:
#   octetList: a list of four bytes
#
# RETURNS:
#   a string containing an IP address
# 
# ERROR: N/A
#

proc ifAddrFormat {octetList} {
    return [format "%d.%d.%d.%d" \
		[lindex $octetList 0] [lindex $octetList 1] \
		[lindex $octetList 2] [lindex $octetList 3]]
}


##############################################################################
#
# ethernetAddrFormat - format an ethernet address
# 
# This routine formats 6 bytes into an ethernet address.
#
# SYNOPSIS:
# ethernetAddrFormat octetList
# 
# PARAMETERS:
#   octetList: a list of four bytes
#
# RETURNS:
#   a string containing an ethernet address
# 
# ERROR: N/A
#

proc ethernetAddrFormat {octetList} {
    return [format "%02x:%02x:%02x:%02x:%02x:%02x" \
                [lindex $octetList 0] [lindex $octetList 1] \
                [lindex $octetList 2] [lindex $octetList 3] \
		[lindex $octetList 4] [lindex $octetList 5]]
}


##############################################################################
#
# plural - get the plural form of a word depending on a number
#
# SYNOPSIS: 
#   plural num
#
# PARAMETERS: N/A
#   num: a number
#
# RETURNS:
#   the "s" string if num >1
#   an empty string else
#
# ERRORS: N/A
#

proc plural {num} {
    if {$num > 1} {
	return s
    } else {
	return ""
    }
}


#############################################################################
#
# twoBytesPack - convert two words of one byte into a two bytes word
#
# This is a convenience routine to solve endianess conversions.
# EXAMPLE: twoBytesPack {0xDE 0xAD} returns "0xDEAD"
#
# SYNOPSIS:
#   twoBytesPack twoBytes 
#
# PARAMETERS:
#   twoBytes: a list of two bytes
#
# RETURNS: a two bytes word in hexa format
#

proc twoBytesPack {twoBytes} {
    bindNamesToList {byte1 byte2} $twoBytes
    return [format "0x%02x%02x" $byte1 $byte2]
}

##############################################################################
#
# symAddrGet - get the address of a symbol if it exists
#
# This routine gets the address of a symbol if it exists. Else, it raises an
# error with a specified error message.
# This routine can fail because of any wtx error (due to the wtxSymFind). 
# In such a case, the wtx error message is displayed instead of the specified
# error message.
#
# SYNOPSIS:
#   symAddrGet symbol msg
#
# PARAMETERS:
#   symbol : symbole name
#   msg : error message to display if the symbol doesn't exist
#
# RETURNS:
#   the symbol address
#
# ERRORS:
#   the specified error message or the wtx error message corresponding to 
#   an other error.
#

proc symAddrGet {symbol msg} {

    if [catch {wtxSymFind -name $symbol} result] {
	set errorMsg [lindex $result 3]
	if {$errorMsg == "(SYMTBL_SYMBOL_NOT_FOUND)"} {

	    # the symbol is not found.

	    error $msg
	} else {
	    error $result
	}
    }

    # get the symbol address

    return [lindex $result 1] 
}

##############################################################################
#
# symValueGet - get a 4 bytes value corresponding to a symbol
#
# This routine gets a 4 bytes value corresponding to a symbol if it exists.
# Else, it raises an error with a specified error message.
# This routine can fail because of any wtx error (due to the wtxSymFind).
# In such a case, the wtx error message is displayed instead of the specified
# error message.
#
# SYNOPSIS:
#   symValueGet symbol msg
#
# PARAMETERS:
#   symbol : symbole name
#   msg : error message to display if the symbol doesn't exist (see
#	  the symAddrGet routine)
#
# RETURNS:
#   the value
#
# ERRORS:
#   the specified error message or the wtx error message corresponding to 
#   an other error.
#

proc symValueGet {symbol msg} {

    # get the symbol address

    set symAddr [symAddrGet $symbol $msg]

    # get the value at symAddr 
    # This routine assumes it is a 4 bytes value
    # Actually, it is typically used to get a 32 bits address in a pointer

    set block [wtxMemRead $symAddr 4]
    return [memBlockGet -l $block]
}

##############################################################################
#
# oneBitOfOneByteGet - get the value of a specified bit inside a byte
#
# This routine gets the value of a specified bit inside a byte.
# 
# SYNOPSIS:
#   oneBitOfOneByteGet byte bitNumber
#
# PARAMETERS:
#   byte: a one byte word
#   bitNumber: index of the bit to get. The LSB index is 0, the MSB index is 7.
#
# RETURNS:
#   the value of the selected bit (0 or 1...)
#
# ERRORS: N/A
#

proc oneBitOfOneByteGet {byte bitNumber} {

    # note: there are no test to validate the parameters.

    set masksList {0x01 0x02 0x04 0x08 0x10 0x20 0x40 0x80}

    set mask [lindex $masksList $bitNumber]
    if {[expr $byte & $mask]} {
	return 1
    } else {
	return 0
    }
}


##############################################################################
#
# booleanOnOffTranslate - get the message associated to a boolean value
# 
# This routine gets the message associated to a boolean value.
# 
# SYNOPSIS:
#   booleanOnOffTranslate flag
#
# PARAMETERS:
#   flag: a boolean value
#
# RETURNS:
#   the "ON" string if flag is not null, else the "OFF" string.
#
# ERRORS: N/A
#

proc booleanOnOffTranslate {flag} {

    if {$flag} {
	return ON
    } else {
	return OFF
    }
}

##############################################################################
#
# sentiListCut - format a list of values returned by wtxGopherEval
#
# This routine formats a list of values returned by wtxGopherEval. When
# the Gopher script grabs info about nodes of a list, it is usefull to put
# a sentinel at the biginning of each node.
# Example: wtxGopherEval "$listHead *{ <0xEEEE!> @@@@ *}"
# For each node of the list, the result would be :
# 0xeeee val1 val2 val3...0xeeee val1 val2 val3...
# If the data structure of one node is very complex. The sentinel 0xEEEE is
# a way to easyly locate each node among the lists of returned values.
#
# SYNOPSIS:
#   sentiListCut oneList sentinel
#
# PARAMETERS:
#   oneList : list of wtxGopherEval returned values (The gopher script must
#		contain a sentinel.)
#   sentinel: "value" of the sentinel. It should be a word in hexa different
#		from the expected results of the wtxGopherEval. For instance,
#		do never user "0", as a sentinel.
#
# RETURNS:
#   A list of sub-lists: one sub-list contains the returned values for one node
#
# ERRORS: N/A
#

proc sentiListCut {oneList sentinel} {

    set oneList [lrange $oneList 1 end]
    set resList ""
    while {[set index [lsearch $oneList $sentinel]] > 0} {
	set indexMinusOne [expr $index -1]
	lappend resList [lrange $oneList 0 $indexMinusOne]
	set oneList [lrange $oneList $index end]
	if {[llength $oneList] >0} {
          set oneList [lrange $oneList 1 end]
	}
    }
    if {[llength $oneList] >0} {
	lappend resList [lrange $oneList 0 end]
    }
    return $resList
}	

##############################################################################
#
# bzero - fill a memory region with zeros
#
# SYNOPSIS:
#   bzero bufferId nbytes
#
# PARAMETERS:
#   bufferId: start adress of the region
#   nbytes: size in bytes of the region
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc bzero {bufferId nbytes} {
    wtxMemSet $bufferId $nbytes 0x00
}
