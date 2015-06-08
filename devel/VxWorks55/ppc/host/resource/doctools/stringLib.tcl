# stringLib.tcl - Tcl library for special string manipulations
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01b,20may99,fle  fixed a typo in stringIsolate doc
#                  + added stringSplit procedure
# 01a,20aug98,fle  written
#

#
# DESCRIPTION
# This library exports the routines to manipulate strings. It implements some
# functions that are not provided by the tcl facilities.
#

#
# INTERNAL
#
# This library contains the following procedures :
#
# stringIsolate		- isolate a string between two given strings
# stringReplace		- replaces occurrences of string
#

################################################################################
#
# stringIsolate - isolate a string between two given strings
#
# This procedure isolates a string between two given <startString> and
# <endString>. It then returns three strings : the previous <startString>, the
# string between <startString> and <endString>, and the string after <endString>
#
# SYNOPSIS
# .tS
#   stringIsolate <startString> <endString> <inString>
# .tE
#
# PARAMETERS
#   startString : the first string to encounter
#   endString : the next string to encounter
#   inString : the string in which to look
#
# RETURNS: a list of three strings {prevString} {isolatedString} {remainString}
#
# ERRORS: N/A
#

proc stringIsolate { startString endString inString } {

    # variables initialization

    set prevString ""
    set isolatedString ""
    set remainString ""
    set startLen [string length $startString]
    set endLen [string length $endString]

    # looking for <startString>

    set strStartPos [string first $startString $inString]

    if { $strStartPos == -1 } {
	set prevString [string range $inString 0 end]
	set remainingString ""
    } else {
	set prevString [string range $inString 0 [expr $strStartPos - 1]]
	set remainingString [string range $inString \
				    [expr $strStartPos + $startLen] end]
    }

    # looking for <endString>

    set strEndPos [string first $endString $remainingString]

    if { $strEndPos == -1 } {
	set isolatedString [string range $remainingString 0 end]
	set remainingString ""
    } else {
	set isolatedString [string range $remainingString 0 \
				   [expr $strEndPos - 1]]

	set remainString [string range $remainingString \
				 [expr $strEndPos + $endLen] end]
    }

    return [list $prevString $isolatedString $remainString]
}

################################################################################
#
# stringReplace - replaces occurrences of string
#
# This routine replaces all occurences of <thisString> with <withString> in the
# string <inString>. This allow to not call to regular expressions that may
# contain specific characters. This replaces the exact occurrence of strings.
#
# SYNOPSIS
# .tS
#   stringReplace <thisString> <withString> <inString>
# .tE
#
# PARAMETERS
#   thisString : exact string to be replaced
#   withString : string to replace <thisString> with
#   inString : string containing (or not) <thisString> to be replaced with 
#              <withString>
#
# RETURNS: The modified string
#
# ERRORS: N/A
#

proc stringReplace { thisString withString inString } {

    # get the string to be replaced length and place in whole string

    set thisStringPlace [string first $thisString $inString]
    set thisStringLen [string length $thisString]

    # replace all occurrence of thisString with withString in inString

    while { $thisStringPlace != -1 } {

	# get string before thisString

	set beforePatternString [string range $inString 0 \
					[expr $thisStringPlace - 1]]

	# get string after thisString

	set afterPatternString [string range $inString \
				       [expr $thisStringPlace + $thisStringLen]\
				       end]

	# reformat output string with withString

	set inString "$beforePatternString$withString$afterPatternString"

	set thisStringPlace [string first $thisString $inString]
    }

    # return updated string

    return $inString
}

################################################################################
#
# stringSplit - splits a sting on a given string
#
# This procedure splits a string into a list on the given <onString>. This is
# diferent from the `split' Tcl call as it does not match one of the characters
# in the <onString>, but the string itself
#
# If the `-all' option is specified, all occurrences of <onString> will split
# <thisString>, while not giving the `-all' option only splits <thisString> on
# the first occurrence of <onString>
#
# NOTE
# Another difference with the `split' function is that this function does not
# return emply elements where the "split" occured
#
# EXAMPLE
# 
# Split a string on the "split" string
#
# \cs
#   stringSplit "please split this string" "split"
#   {please } { this string}
# \ce
#
# While calling for `split' would have done :
#
# \cs
#   split "please split this string" "split"
#   {} {} ea {e } {} {} {} {} { } h {} { } {} r ng
# \ce
#
# SYNOPSIS
# \ss
#   stringSplit <thisString> <onString> [-all]
# \se
#
# PARAMETERS
#   thisString : the string to be splitted
#   onString : the string on which to split
#   -all : specifies that all ocurrences of <onString> will split <thisString>
#
# RETURNS: A list of splitted strings. This list contains at list two elements.
# Second element may be empty if there was nothing to split. If the element to
# split was the very first, then the first element of the returned list is empty
#
# EXAMPLES
#
# Element to split comes first in <thisString> :
# \cs
#   stringSplit "split here" "split"
#   {} { here}
# \ce
#
# There is no element to split :
# \cs
#   stringSplit "cut here" "split"
#   {cut here} {}
# \ce
#
# Several occurrences of split, but we only want the very first :
# \cs
#   stringSplit "please split here, but also try to split here" "split"
#   {please } { here, but also try to split here}
# \ce
#
# And here there are several occurrences, and we want to get them all :
# \cs
#   stringSplit "please split here, but also try to split here" "split" "-all"
#   {please } { here, but also try to } { here}
# \ce
# 
# ERRORS: N/A
#

proc stringSplit { thisString onString {splitStyle "-notall"} } {

    set splitList {}
    set strLen [string length $onString]

    # check if there is at least one occurrence

    if { [set strIx [string first $onString $thisString]] == -1 } {
	set splitList [list $thisString {}]
	return $splitList
    }

    # there is at least one split to do

    set splitList [list [string range $thisString 0 [expr $strIx - 1]]]
    set thisString [string range $thisString [expr $strIx + $strLen] end]

    while { ($splitStyle == "-all") && \
	    ([set strIx [string first $onString $thisString]] != -1) } {

	lappend splitList [string range $thisString 0 [expr $strIx - 1]]
	set thisString [string range $thisString [expr $strIx + $strLen] end]
    }

    lappend splitList $thisString
    return $splitList
}
