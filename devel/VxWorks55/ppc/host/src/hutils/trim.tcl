# trim.tcl - remove unreferenced symbols 
#
# modification history
# --------------------
# 01c,01dec98,yh   add more man page stuff.
# 01b,02nov98,yh   cleanup and make the code more robust.
# 01a,30oct98,yh   written.
#
# DESCRIPTION
# This tool removes the set of unreferenced symbols from an object file for a
# given list of precious symbols and a tool suffix. The object file has to be
# compiled with -ffunction-sections to make each function have its own section.
#
#
# USAGE
#        wtxtcl -f trim.tcl inFile outFile toolSuffix preciousSymbolList
#
# PARAMETERS
#   inFile : name of the obj file which contains multiple sections
#
#   outFile :  name of the stripped file produced
#
#   toolSuffix : used for objdump and objcopy
#
#   preciousSymbolList : the list of symbols to start with
#
# INTERNAL
# Existing issues:
#   * Among three OMFs, a.out doesn't support multiple segments. COFF limits 
#     the number of segments in one object file.
#   * PPC and MIPS use ELF, ccmips ignores -ffunction-sections. 
#   * In general, the tool works for Power PC with the following problems:
#     nmppc and gdbppc don't work with obj file with multiple text sections.
#     -ffunction-sections is unable to produce multiple sections for 
#     assemble files.


set usage "Uasge: wtxtcl -f trim.tcl inFile outFile toolSuffix \
preciousSymbolList"

if {$argc != 4} {
        puts "$usage"
        exit -1
        }

set inFile [lindex $argv 0]
set outFile [lindex $argv 1]
set arch [lindex $argv 2]
set symList [lindex $argv 3]

set sectionList {}
set unneededSet {}

###############################################################################
# 
# symbolTrim - find the set of unused symbols.
#
# Given a .o file, architecture type, and a list of symbols, find out the 
# set of function symbols unreferenced. The .o file has to be
# compiled with -ffuntion-sections to put each function to its own section.
#
# SYNOPSIS
#   symbolTrim filename arch symList
#
# PARAMETERS
#   filename : the obj file with multiple text sections
#   arch : architecture type (e.g. ppc)
#   symList : the precious list of symbols
#   
# RETURNS: N/A
#
# ERRORS:
 
proc symbolTrim {filename arch symList} {
    global sectionList
    global unneededSet
    global RELOCATION_RECORDS_FOR

    set fcontent [exec [wtxPath]/host/sun4-solaris2/bin/objdump${arch} -x $filename]
    set index [string first "RELOCATION" $fcontent]
    set fcontent [string range $fcontent $index end]

    regsub -all {RELOCATION RECORDS FOR \[} $fcontent {RELOCATION_RECORDS_FOR } fcontent
    regsub -all {\]:} $fcontent {} fcontent

    foreach line [split $fcontent \n] {
	if {[string match "RELOCATION_RECORDS_FOR*" $line] == 1} {
	    lappend sectionList [lindex $line 1]
	}	
    }
puts "There are [llength $sectionList] sections"

    foreach line [split $fcontent \n] {
	if {[string match "RELOCATION_RECORDS_FOR*" $line] == 1} {
	    set section [lindex $line 1]
	    set RELOCATION_RECORDS_FOR($section) {}
	    continue
	}

	if {[string match "OFFSET*" $line] == 1} {
	    continue
	}
	
	if {$line == ""} {
	    continue
	}

	set symbol [lindex $line 2]
	if {[lsearch $RELOCATION_RECORDS_FOR($section) $symbol] == -1 && [lsearch $sectionList $symbol] != -1} { 
	    lappend RELOCATION_RECORDS_FOR($section) [lindex $line 2]
	}
    }

    # recursively find out the set of sections needed

    set neededSet {}
    set workSet [lappend symList ".text" ".data"]

    # add sections in .data and .text to workSet
    foreach symbol $RELOCATION_RECORDS_FOR(.text) {
	if {[lsearch $workSet $symbol] == -1} {
	    lappend workSet $symbol
	}
    }

    foreach symbol $RELOCATION_RECORDS_FOR(.data) {
	if {[lsearch $workSet $symbol] == -1} {
	    lappend workSet $symbol
	}
    }
    
    while {$workSet != ""} {
	set addedSet [findSet $workSet]
	set neededSet [concat $neededSet $workSet]
	set workSet {}
	foreach symbol $addedSet {
	    if {[lsearch $neededSet $symbol] == -1} {
		lappend workSet $symbol
	    }
	}
    }
puts "[llength $neededSet] sections needed"

    set unneededSet {}
    foreach symbol $sectionList {
	if {[lsearch $neededSet $symbol] == -1} {
	    lappend unneededSet $symbol
	}
    }
puts "[llength $unneededSet] sections unneeded"
    
}

###############################################################################
# 
# symbolDelete - delete the set of unneeded symbols from inFile
#
# This procedure removes the set of unneeded symbols from inFile and the 
# output is written to outfile.                
#
# SYNOPSIS
#   infile : the object file with multiple text sections
#   outfile : the output file name
#   arch : the architecture type
#
# RETURNS : N/A
#
# ERRORS:
  
proc symbolDelete {infile outfile arch} {
    global unneededSet 
    set deletedSet {}

    set symbolList $unneededSet
    set remainSet {}
    set argList ""
    while {$symbolList != {}} {
	set elemList ""
	set usedSet [findSet $symbolList]
	set remainSet {}
	foreach elem $symbolList {
	    if {[lsearch $usedSet $elem] == -1} {
		append elemList "$elem "
		append argList "-R $elem "
		lappend deletedSet $elem
	    } else {
		lappend remainSet $elem
	    }
	}
	if {$elemList == ""} {
	    set remainSet {}
	    foreach elem $symbolList {
		append argList "-R $elem "
		lappend deletedSet $elem
	    }
	
	    regsub -all {\$} $argList {\\$} argList
	    set cmd "exec objcopy${arch} $argList $infile $outfile"
	    if [catch $cmd result] {
		puts "error on deleting $elemList: $result"
	    } 
	}
	set symbolList $remainSet
    }

puts "deleting [llength $deletedSet] sections"
}

###############################################################################
#
# findSet - Given a section list, return the set of sections referred 
#           
# SYNOPSIS
#   findSet symbolList
#
# PARAMETERS
#   symbolList : a list of sections
# 
# RETURNS
#   the set of sections referred by the list of sections
#
# ERRORS

proc findSet {symbolList} {
    global sectionList
    global RELOCATION_RECORDS_FOR
    set addedSet {}

    foreach symbol $symbolList {
	foreach sym $RELOCATION_RECORDS_FOR($symbol) {
	    if {[lsearch $addedSet $sym] == -1} {
	    lappend addedSet $sym
	    }
	}
    }		
    return $addedSet
}

symbolTrim $inFile $arch $symList
symbolDelete $inFile $outFile $arch

