# 01Module.tcl - Module browser
#
# Copyright 1994-1996 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01h,23jul96,pad  Adapted to new common symbol type (SPR #6934).
# 01g,17jan96,pad  The notice post is now discarded when a WTX error occurs
# 		   while transfering the symbols (SPR #5862).
#		   Browsing a module with all segments empty no longer raise
#		   a Tcl error.
#                  Make more conformable to WRS Tcl coding conventions.
# 01f,09oct95,pad  Added reference to Target Server in window title (SPR #5110).
#                  Lined up fields in hierarchy window.
# 01e,28jul95,pad  enhanced module browser in order to display selected objects.
# 01d,24may95,pad  updated for use with configuration panel.
# 01c,15may95,pad  replaced ctoken (extended Tcl) by scan command.
# 01b,15may95,c_s  updated for new wtxtcl.
# 01a,12may95,pad  created
#

# DESCRIPTION
# This file holds all the Tcl code required to display a hierarchy window with
# information about a module and the module's symbols.
# The module browser is called when a Btn1Down event occurs in the object
# modules window of the Tornado Browser.
#
# RESOURCE FILES: N/A
#

# Globals

set symSortByName 0	;# symbol sorting by address by default

################################################################################
#
# objModuleSelect - get data and build hierarchy window
#
# This routine is the core of the module browser. It analyses a string passed
# as argument to get the module name.
# Information related to the module and its symbols is fetched, then processed
# to build the required hierarchy window structure.
#
# SYNOPSIS:
#    objModuleSelect "<moduleId> <moduleName> <textSize> <dataSize> <bssSize>"
#
# PARAMETERS:
#    textLine: a string which have the following contents: <moduleId>
#              <moduleName> <textSize> <dataSize> <bssSize>
#
# RETURNS: N/A
#
# ERRORS: N/A
#

proc objModuleSelect {textLine} {
    global symSortByName

    set notice 0

    # Get info from target server

    set moduleId [lindex $textLine 0]
    set modInfo [wtxObjModuleInfoGet $moduleId]
    
    # Extract general information about the module

    set modName [lindex $modInfo 1]
    set modFlags [wtxEnumToString LOAD_FLAG [lindex $modInfo 4]]
    set modOmf [lindex $modInfo 2]
    set modGrp [lindex $modInfo 3]

    #  Extract information about text segment

    set textSeg [lindex $modInfo 5]
    set textType "text"
    set textAddr [lindex $textSeg 1]
    set textSize [lindex $textSeg 2]
    if {$textSize > 10000} {
	set notice 1
    }

    # Extract information about data segment

    set dataSeg [lindex $modInfo 6]
    set dataType "data"
    set dataAddr [lindex $dataSeg 1]
    set dataSize [lindex $dataSeg 2]
    if {$dataSize > 10000} {
	set notice 1
    }

    #  Extract information about bss segment

    set bssSeg [lindex $modInfo 7]
    set bssType "bss"
    set bssAddr [lindex $bssSeg 1]
    set bssSize [lindex $bssSeg 2]
    if {$bssSize > 10000} {
	set notice 1
    }

    # Display a notice post if the number of symbols is big enough.

    if {$notice} {
	noticePost delay "Working. Be patient, please..."
    }

    # Only valid segments will be displayed

    set segments ""

    if {$textAddr != 0xffffffff} {
	lappend segments [list $textType $textAddr $textSize]
    }
    if {$dataAddr != 0xffffffff} {
	lappend segments [list $dataType $dataAddr $dataSize]
    }
    if {$bssAddr != 0xffffffff} {
	lappend segments [list $bssType $bssAddr $bssSize]
    }

    # Prepare common data to put in hierarchy window

    set value "{$modName $modFlags $modOmf $modGrp} {$segments}"
    set hierarchyStruct "{Module {\"name \" flags \"OMF  \" group}} \
			{Segments {+ {\"type   \" address \"size   \"}}}"

    # List of unknown symbols. Only the symbol's names are considered.
    # Note that if an error occurs while we're getting the symbols, we
    # must discard the notice post, if it exists.

    set unknownSymList ""

    if [catch "wtxSymListGet -module $modName -unknown" rawUnknownSymList] {
	if {$notice} {
	    noticeUnpost delay
	}
	error $rawUnknownSymList
    }

    foreach unknownSym $rawUnknownSymList {
	lappend unknownSymList [lindex $unknownSym 0]
    }

    # Sort unknown symbols list and update hierarchy window structure

    if {[llength $unknownSymList] != 0} {
	set unknownSymListSorted [lsort $unknownSymList]
	append value " {$unknownSymListSorted}"
	append hierarchyStruct " {Unresolved_Symbols +}"
    }

    # List of defined symbols that belong to the module.
    # Note that if an error occurs while we're getting the symbols, we
    # must discard the notice post, if it exists.

    set textSymList ""
    set dataSymList ""
    set bssSymList ""
    set otherSymList ""

    if [catch "wtxSymListGet -module $modName" rawSymList] {
	if {$notice} {
	    noticeUnpost delay
	}
	error $rawSymList
    }

    foreach definedSym $rawSymList {
	if {$definedSym == ""} break
	set symInfo ""

        # Put symbol's name first to allow sorting by name

	if {$symSortByName} {
	    append symInfo [lindex $definedSym 0]
	    append symInfo " "
	}

	# Symbol's address

	append symInfo [format "%08x" [lindex $definedSym 1]]
	append symInfo " "

	# Symbol's type

	set symRawType [lindex $definedSym 2]
	set symGlobal [expr {$symRawType & 1}]

	set symType [expr {($symRawType >> 4) & 0xf}]
	if {$symType == 1} {
	    append symInfo "C "
	    append symInfo [lindex $definedSym 0]
	    lappend otherSymList $symInfo
	} else {

	    set symType [expr {($symRawType >> 1) & 0xf}]

	    switch $symType {
		1 {
		    if {$symGlobal} {
			append symInfo "A "
		    } else {
			append symInfo "a "
		    }
		    append symInfo [lindex $definedSym 0]
		    lappend otherSymList $symInfo
		}

		2 {
		    if {$symGlobal} {
			append symInfo "T "
		    } else {
			append symInfo "t "
		    }
		    append symInfo [lindex $definedSym 0]
		    lappend textSymList $symInfo
		}

		3 {
		    if {$symGlobal} {
			append symInfo "D "
		    } else {
			append symInfo "d "
		    }
		    append symInfo [lindex $definedSym 0]
		    lappend dataSymList $symInfo
		}

		4 {
		    if {$symGlobal} {
			append symInfo "B "
		    } else {
			append symInfo "b "
		    }
		    append symInfo [lindex $definedSym 0]
		    lappend bssSymList $symInfo
		}

		default {
		    append symInfo "? "
		    append symInfo [lindex $definedSym 0]
		    lappend otherSymList $symInfo
		}
	    }
	}
    }

    #  Sort symbol lists and update hierarchy window structure

    if {[llength $textSymList] != 0} {
	set textSymListSorted [lsort $textSymList]
	if {$symSortByName} {
	    set textSymListTrimed ""

	    # Remove the leading symbol name from the list

	    foreach elem $textSymListSorted {
		set newElem ""
		scan $elem "%s %s %s %s" dummy address type name
		append newElem $address " " $type " " $name
		lappend textSymListTrimed $newElem
	    }
	    append value " {$textSymListTrimed}"
	} else {
	    append value " {$textSymListSorted}"
	}
	append hierarchyStruct " {Text_Symbols +}" 
    }

    if {[llength $dataSymList] != 0} {
	set dataSymListSorted [lsort $dataSymList]
	if {$symSortByName} {
	    set dataSymListTrimed ""

	    # Remove the leading symbol name from the list

	    foreach elem $dataSymListSorted {
		set newElem ""
		scan $elem "%s %s %s %s" dummy address type name
		append newElem $address " " $type " " $name
		lappend dataSymListTrimed $newElem
	    }
	    append value " {$dataSymListTrimed}"
	} else {
	    append value " {$dataSymListSorted}"
	}
	append hierarchyStruct " {Data_Symbols +}" 
    }

    if {[llength $bssSymList] != 0} {
	set bssSymListSorted [lsort $bssSymList]
	if {$symSortByName} {
	    set bssSymListTrimed ""

	    # Remove the leading symbol name from the list

	    foreach elem $bssSymListSorted {
		set newElem ""
		scan $elem "%s %s %s %s" dummy address type name
		append newElem $address " " $type " " $name
		lappend bssSymListTrimed $newElem
	    }
	    append value " {$bssSymListTrimed}"
	} else {
	    append value " {$bssSymListSorted}"
	}
	append hierarchyStruct " {Bss_Symbols +}" 
    }

    if {[llength $otherSymList] != 0} {
	set otherSymListSorted [lsort $otherSymList]
	if {$symSortByName} {
	    set otherSymListTrimed ""

	    # Remove the leading symbol name from the list

	    foreach elem $otherSymListSorted {
		set newElem ""
		scan $elem "%s %s %s %s" dummy address type name
		append newElem $address " " $type " " $name
		lappend otherSymListTrimed $newElem
	    }
	    append value " {$otherSymListTrimed}"
	} else {
	    append value " {$otherSymListSorted}"
	}
	append hierarchyStruct " {Other_Symbols +}" 
    }

    # Create hierarchy window

    set browserTitle "[lindex [wtxHandle] 0]: [lindex $modInfo 1]"
    hierarchyCreate $browserTitle objModSelectionHandler
    hierarchySetStructure $browserTitle $hierarchyStruct
    hierarchyPost $browserTitle

    # Update the hierarchy window

    hierarchySetValues $browserTitle $value

    # Discard the notice post, if any.

    if {$notice} {
	noticeUnpost delay
    }
}

################################################################################
#
# objModSelectionHandler - manage selections inside module browser window
#
# This routine handles the selections done via mouse clicks inside the module
# browser window. The selection involves the whole line pointed to by the mouse
# pointer.
#
# For now only subhierarchy Data_Symbols, Bss_Symbols and Other_symbols are
# managed.
#
# SYNOPSIS:
#    objModSelectionHandler hierarchyDesc
#
# PARAMETERS:
#    hierarchyDesc : string describing the local point in the hierarchy. For
#                    instance: {145 = {000a4118 D _semClassId}} Data_Symbols
#                    This strings means that we are in the Data_Symbols"
#                    subset, in which the 145th element is the string
#                    "000a4118 D _semClassId".
#    
# RETURNS: 0 always
#

proc objModSelectionHandler {args} {

    # Get hierarchy subset

    set fromHier [lindex $args 1]

    # If the hierarchy level is related to data, bss or "other" (not text) 
    # symbols, get the symbol name and apply an object browsing on it.
    # If the symbol corresponds to a valid system object, a specific browser
    # window will appear, otherwise, an error popup is displayed.

    if {($fromHier == "Data_Symbols") ||
	($fromHier == "Bss_Symbols")  ||
	($fromHier == "Other_Symbols")} {
	set symbolName [lindex [lindex [lindex $args 0] 2] 2]
	browse $symbolName
	return 0
    }
}
