# htmlLib.tcl - HTML files manipulation library
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01d,19oct98,fle  fixed a bug in htmlIndexCreate that was not taking upercase
#                  first letter into account anymore
#                  + removed any reference to WIND_BASE to use wtxPath instead
# 01c,30sep98,fle  modified htmlIndexCreate since new category field in help
#                  entries
# 01b,12sep98,fle  updated to new group and type help entry fields (see
#                    helpEntry.tcl)
# 01a,24aug98,fle  written, based on htmlLink.tcl 01a
#

#
# DESCRIPTION
# This library provides Tcl with several HTML files manipulation routines.
#
# INCLUDES
#   stringLib.tcl
#   helpEntryLib.tcl
#

#
# INTERNAL
#
# This Tcl source file contains the following procedures :
#
# htmlFormattedStringsGet	- gets all the specially formatted strings
# htmlIndexCreate		- create an HTML index file from <tocFile> and
#                                 <linkFile>
# htmlLinkExtract		- returns the html link from an entry link
# htmlNavIndexCreate		- puts local alphabetical linked index in
#                                 <fileId>

################################################################################
#
# htmlFormattedStringsGet - gets all the specially formatted strings
#
# This procedure gets all the <type> specified HTML formatted strings. It can
# retrieve bold, italic, head1 to head4 types.
#
# SYNOPSIS
#   htmlFormattedStringsGet htmlFile type
#
# PARAMETERS
#   htmlFile : the htmlFile in which to look for special strings
#   type : the type to retrieve [bold|italic|head1...|head4]
#
# RETURNS: A list of formatted strings.
#
# ERRORS: N/A
#

proc htmlFormattedStringsGet { htmlFile type } {

    global specialStringTypes		;# array of special strings types

    set startString [lindex $specialStringTypes($type) 0]
    set endString [lindex $specialStringTypes($type) 1]
    set specialStringList {}

    if { ! [file exists $htmlFile] || ! [file readable $htmlFile] } {
	return $specialStringList
    }

    # get the HTML file content

    set fileId [open $htmlFile r]
    set fileContent [read $fileId]
    close $fileId

    # isolate all the bold strings

    set remainingString [stringIsolate $startString $endString $fileContent]

    while { [lindex $remainingString 2] != "" } {

	set specialString [lindex $remainingString 1]
	set remainingString [lindex $remainingString 2]

	if { ([lsearch $specialStringList \
		       "$startString$specialString$endString"] == -1) && 
	     ([string trim $specialString] != "") } {

	    lappend specialStringList "$startString$specialString$endString"
	}

	set remainingString [stringIsolate "<b>" "</b>" $remainingString]
    }

    return [lsort -dictionary $specialStringList]
}

################################################################################
#
# htmlIndexCreate - create an HTML index file from <entryList>
#
# This routine creates an HTML index file, using the <entryList> index
# description and links.
#
# `entryListIndex' parameter is a list of the indexes that have to be generated.
# As several indexes can begenerated in the same file, entryList is in fact a
# list of index content descriptions, each element of the entryList should have
# the following format :
#
#   {localEntryList} {indexTitle} {parentFile} {[navBar]}
#
# where
#
# 'localEntryList' is the help entryList for the local index (as returned by
# the 'entryListInFileGet' Tcl procedure)
#
# `indexTitle' allows to set the html index file title
#
# 'parentFile' parameter should be a list of two elements : first
# giving a string that will describe the parent file. Second element is the
# name of the parent file. If empty, no link to parent file is generated
#
# `navBar' specifies wheter an alphabetical quick access bar has to be
#  generated at the top of the html index file (default set to 1).
#
# WARNING
# <entryListIndex> MUST BE a list of elements described above. The first element
# of <entryListIndex> should be an entry index, even if there is only one
# element in the list
#
# SYNOPSIS
#   htmlIndexCreate entryList inFile
#
# PARAMETERS
#   entryListIndex : index of help entry list
#   inFile : name of the file containing the html index
#
# RETURNS: N/A
#
# ERRORS: N/A
#
# EXAMPLE
# .CS
#   # get entry index for libraries
#
#   set libEntryList [entryListInFileGet [list "LIB" "LIB.SUB"]
#   set libEntryTitle "Libraries"
#   set libParentFile {}
#   set libNavBar 0
#   set indexFileName index.html
#   lappend entryListIndex [list $libEntryList $libEntryTitle $libParentFile \
# 				 $libNavBar]
#
#   # get entry index for routines
#
#   set rtnEntryList [entryListInFileGet [list "RTN" "RTN.SUB"]
#   set rtnEntryTitle "Routines"
#   set rtnparentFile [list $libEntryTitle $indexFileName]
#   set rtnNavBar 1
#   lappend entryListIndex [list $rtnEntryList $rtnEntryTitle $rtnparentFile \
#				 $rtnNavBar]
#
#   # generate the html index
#
#   htmlIndexCreate $entryListIndex $indexFileName
# .CE
#
# SEE ALSO
# `helpEntryLib', `htmlNavIndexCreate'
#

proc htmlIndexCreate { entryListIndex inFile } {

    set fileId [open $inFile w]
    set nIndex [llength $entryListIndex]	;# nombre d'index

    # if there are several indexes to be generated, put a separator
    # between them

    if { $nIndex > 1 } {
	set indexSep 1
    } else {
	set indexSep 0
    }

    puts $fileId "<html>"		;# begin the HTML file

    # put page title : take first list index title as the page title

    set indexTitle [lindex [lindex $entryListIndex 0] 1]
    puts $fileId "<head>"
    puts $fileId "<title>$indexTitle</title>"
    puts $fileId "</head>"

    # begin HTML body

    puts $fileId "<body bgcolor=\"#FFFFFF\" text=\"#000000\">"
    set curIndexEntry 0

    foreach entryIndex $entryListIndex {

	set entryList [lindex $entryIndex 0]
	set entryList [lsort -index 0 -dictionary $entryList]

	set indexTitle [lindex $entryIndex 1]
	if { [lindex $entryIndex 2] == {} } {
	    set parentFile {}
	} else {
	    set parentFile [lindex $entryIndex 2]
	}
	set navBar [lindex $entryIndex 3]

	# if the parentFile parameter has been specified, it is used to generate
	# a link to the parent file in a kind of quick navigation bar

	if { $parentFile != {} } {
	    puts $fileId "<p align=right><font size=\"5\">"
	    puts $fileId "<a href=\"[lindex $parentFile 1]\">[lindex \
			  $parentFile 0]</a><br>"
	    puts $fileId "</font></p>"
	}

	# put index file title

	if { $indexTitle != "" } {
	    puts $fileId "<h1>$indexTitle</h1>"
	}

	puts $fileId "<blockquote>"
	if { $navBar } {
	    htmlNavIndexCreate $fileId $entryList
	}

	# Assuming that, once sorted, table of content entries, and link entries
	# have the same order, write them in the HTML file

	set entryIndex 0
	set indexLetter ""
	set previousIndexLetter ""

	# get the different categories from entry list

	set haveSeveralCategories 0
	if { [llength [lsCategory $entryList]] > 1 } {
	    set haveSeveralCategories 1
	}

	puts $fileId "<table border=\"0\">"
	foreach entry $entryList {

	    # generate an alphabetical index from first letter of entry name

	    set indexLetter [string toupper [string range [lindex $entry 0] \
							  0 0]]

	    # insert index letter only if the navbar is generated

	    if { ($indexLetter != $previousIndexLetter) && ($navBar == 1) } {

		puts $fileId "<tr><td><h1><a name=\"[string toupper \
							       $indexLetter]\">\
			      [string toupper $indexLetter]</a></h1></td>\
			      <td></td><td></td></tr>"

		set previousIndexLetter $indexLetter
	    }

	    # put the entry name in table

	    puts -nonewline $fileId "<tr><td></td><td>"
	    if { ([lindex $entry 6] != {}) && ($haveSeveralCategories) } {
		set entryLink "[lindex $entry 3] \[[lindex $entry 6]\]"
	    } else {
		set entryLink [lindex $entry 3]
	    }

	    set entryLink [stringReplace "\\\&nbsp" "\&nbsp" $entryLink]
	    puts -nonewline $fileId "$entryLink</td><td>"

	    # write the table of content description in file

	    puts -nonewline $fileId "[string trim [lindex $entry 1]]"
	    puts $fileId "</td></tr>"
	    incr entryIndex
	}
	puts $fileId "</table></p>"
	puts $fileId "</blockquote>"
	incr curIndexEntry

	if { ($indexSep) && ($nIndex > $curIndexEntry) } {
	    puts $fileId "<p>&nbsp;</p>"
	}
    }

    # end the HTML file

    puts $fileId "</body>"
    puts $fileId "</html>"
    close $fileId
}

################################################################################
#
# htmlLinkExtract - returns the html link from an entry link
#
# This routine extract the file link of an entry link. An entry link is
# beautified and refers to a href. This routine allows to get rid of all the
# html stuff, and only keep the file name and tag.
#
# SYNOPSIS
#   htmlLinkExtract entryLink
#
# PARAMETERS
#   entryLink : the entry link to extract link from
#
# RETURNS: The file name and tag link
#
# EXAMPLE
#
# .CS
#   htmlLinkExtract <b><i><a href="./exmpl.html#test">test</a></i>(\&nbsp;)</b>
#   exmpl.html#test
# .CE
#
# ERRORS: N/A
#

proc htmlLinkExtract { entryLink } {

    set linkPos [string first "href" $entryLink]
    set linkFile [string range $entryLink [expr $linkPos + 4] end]
    set linkPos [string first "\"" $linkFile]
    set linkFile [string range $linkFile [expr $linkPos + 1] end]
    set linkPos [string first "\"" $linkFile]
    set linkFile [string range $linkFile 0 [expr $linkPos - 1]]
    set linkFile [file split $linkFile]

    if { [lindex $linkFile 0] == "." } {
	set linkFile [lrange $linkFile 1 end]
    }

    set linkFile [join $linkFile /]
    return $linkFile
}

################################################################################
#
# htmlNavIndexCreate - puts local alphabetical linked index in <fileId>
#
# This procedure writes a table index in the <fileId> html file. It takes
# all first letters of <entryList>, then generate a flat table with links
# to each of the alphabetical letters found.
#
# SYNOPSIS
#   htmlNavIndexCreate fileId entryList
#
# PARAMETERS
#   fileId : the file ID in which to write HTML index
#   entryList : entries from which to get the index elements
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc htmlNavIndexCreate { fileId entryList } {

    set entryLetterList {}
    set indexLetters { "A" "B" "C" "D" "E" "F" "G" "H" "I" "J" "K" "L" "M" "N" \
		       "O" "P" "Q" "R" "S" "T" "U" "V" "W" "X" "Y" "Z"}

    # get the letter from entries

    foreach entry $entryList {
	set entryLetter [string range $entry 0 0]
	if { [lsearch $entryLetterList $entryLetter] == -1 } {
	    lappend entryLetterList [string toupper $entryLetter]
	}
    }

    # now create the HTML table and its links

    puts $fileId "<p>&nbsp;<p/>"
    puts $fileId "<center>"
    puts $fileId "<table><tr valign=middle>"

    foreach indexLetter $indexLetters {
	if { [lsearch $entryLetterList $indexLetter] != -1 } {
	    puts $fileId "<td valign=middle><h1> \
			  <a href=\"#$indexLetter\">$indexLetter</a></h1>"
	} else {
	    puts $fileId "<td valign=middle><h1>$indexLetter</a></h1>"
	}
    }

    puts $fileId "</tr></table>"
    puts $fileId "</center>"
    puts $fileId "<p>&nbsp;<p/>"
}

################################################################################
#
# lsCategory - retrieves the list of categories from <entryList>
#
# This procedure returns the list of different categories the <entryList>
# variable contains. It only returns non-empty categories.
#
# SYNOPSIS
#   lsCategory <entryList>
#
# PARAMETERS
#   entryList : the list of entries to get categories from
#
# RETURNS: The list of different categories from <entryList> or {} if no
# categories have been found.
#
# ERRORS: N/A
#

proc lsCategory { entryList } {

    set categoryList {}

    foreach entry $entryList {
	set entryCategory [lindex $entry 6]
	if { [lsearch $categoryList $entryCategory] == -1 } {
	    lappend categoryList $entryCategory
	}
    }

    return $categoryList
}

# global variables

global env				;# user's environment variables
global specialStringTypes		;# array of reachable special strings

array set specialStringTypes {
   {bold}	{"<b>" "</b>"}
   {italic}	{"<i>" "</i>"}
   {head1}	{"<h1>" "</h1>"}
   {head2}	{"<h2>" "</h2>"}
   {head3}	{"<h3>" "</h3>"}
   {head4}	{"<h4>" "</h4>"}
}

# check for the source directory

if { [catch "set htmlLibDir [wtxPath host resource]doctools"] } {
    set htmlLibDir "."
}
    
# source the Tcl libraries

source "$htmlLibDir/helpEntryLib.tcl"
source "$htmlLibDir/stringLib.tcl"
