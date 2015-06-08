# munch.tcl - utility to munch C++ modules for static constructors.
#
# Copyright 1994-2001 Wind River Systems, Inc.
#
# modification history
# -----------------------
# 01u,28jun02,mcm  Adding .init$00 and .fini$00 sections to ensure proper
#                  alignment (SPR 78646)
# 01t,24apr02,sn   SPR 75123 - added error trapping code
# 01s,25mar02,jab  made frameInfoEmit emit-language specific
# 01r,19feb02,r_s  Handle duplicate object modules
# 01q,21jan02,sn   Only skip declaration of _STD__needed_destruction_list if
#                  we generated it here
# 01p,11jan02,f_b  Added (de)registration of Diab exception tables and
#                  handling of static destructors for loaded modules
# 01o,12dec01,jab  updated deduceUnderscore; coldfire has leading underscores
# 01n,08nov01,jn   Add arm to the list of architectures not needing
#                  prepended underscores
# 01n,07nov01,sn   Support priority sorting of Diab ctors/dtors
# 01m,06nov01,sn   Match Diab STI/STD functions correctly
# 01l,06nov01,sn   Handle underscore correctly when emitting C
# 01k,03oct01,pad  Added pentium to the list of architectures not needing
#                  prepended underscores.
# 01j,22aug01,sn   only consider symbols with global linkage
# 01i,18oct00,zl   added .type directive in ASMArrayEmit for SuperH.
# 01o,01nov00,yvp  Added conditional for frv - emit a "type" directive in 
#                  ctdt.c without which static constructors would not execute.
# 01n,22sep00,ish  Change Coldfire to cf 
# 01m,04sep00,yvp  no underscores to prepend for arch frv
# 01l,05jul00,dh   deduce prepends-underscore from arch for Coldfire
# 01k,07jun99,dra  deduce prepends-underscore from arch for sparc64
# 01j,01jun99,tdl  deduce prepends-underscore from arch for ppce
# 01i,26may99,tdl  deduce prepends-underscore from arch for mipse
# 01h,13apr99,sn   deduce prepends-underscore from arch
# 01g,24mar99,sn   ensure that ctors, dtors data is 4 byte aligned
# 01f,24mar99,sn   corrected indentation: no code changes
# 01e,16mar99,sn   removed references to VOIDFUNCPTR (fixes SPR 25469)
# 01d,11mar99,sn   fixed typo that stopped -asm from having any effect
# 01c,22feb99,sn   rewrote to include postMunch.tcl
#                  produce __asm__ directives if -asm specified
# 01b,01may98,sn   fixed definition of procSmbl
# 01a,24feb98,jco  written, tcl translation of munch.cpp.
#
# DESCRIPTION
# This script is intended to be lauched from munch.sh on Unix and munch.bat
# on Windows. See documentation in munch.sh (hutils).
#

###############################################################################
# 
# prologEmit - Emit prologue in specified language
# 
# $lang must be C or ASM

proc prologEmit {lang} {
    ${lang}PrologEmit
}

###############################################################################
# 
# CPrologEmit - Emit prologue in C
#
proc CPrologEmit {} {
    puts "/* ctors and dtors arrays -- to be used by runtime system */"
    puts "/*   to call static constructors and destructors          */"
    puts "/*                                                        */"
    puts "/* NOTE: Use a C compiler to compile this file. If you    */"
    puts "/*       are using GNU C++, be sure to use compile this   */"
    puts "/*       file using a GNU compiler with the               */"
    puts "/*       -fdollars-in-identifiers flag.                   */"
    puts ""
}

###############################################################################
# 
# ASMPrologEmit - emit prologue as ASM directives
#

proc ASMPrologEmit {} {
    puts "/* ctors and dtors arrays -- to be used by runtime system */"
    puts "/*   to call static constructors and destructors          */"
    puts "/*                                                        */"
    puts "/* NOTE: Use a GNU C/C++ compiler to compile this file.   */"
    puts ""
    puts "__asm__ (\".data\");"
    }

###############################################################################
# 
# arrayEmit - emit an array in the specified language and order
# 
# $lang must be C or ASM
# $op is the C name of the array minus an obligatory leading underscore
# $order is "forward" or "reverse"
# $procList is the list of function pointers to put in the array

proc arrayEmit {lang op order procList} {
    listRemoveLeftDuplicates $procList outList
    if {$order == "reverse"} {
	listReverse $outList outList
	}
    ${lang}ArrayEmit $op $outList
}

###############################################################################
# 
# CarrayEmit - Emit array in C
#
# $op is the C name of the array minus an obligatory leading underscore
# $procList is the list of function pointers to put in the array

proc CArrayEmit {op procList} {
    # underscore is "_" if the compiler prepends an underscore
    global underscore
    # usually we replace a prepended underscore with <blank>.
    # For backwards compatibility, if no flags are specified on
    # the command line, we replace a leading __ with _ .
    global underscore_replacement
    global moduleDestructorNeeded

    # Declare each function in procList
    foreach pn $procList {
	# Don't declare self generated functions
	if [ regexp {__?ST[ID]__[0-9][0-9]_ctors} $pn ] {
	    continue
	}
	if { $moduleDestructorNeeded != 0 && [ regexp {__?STD__needed_destruction_list} $pn ] } {
	    continue
	}

	# Remove possible compiler prefixed leading underscore
	regsub "^$underscore" $pn "$underscore_replacement"  pn
	puts "\nvoid $pn\(\);"
    }
    # Declare an array named $op
    puts "\nextern void (*_$op\[\])();"
    puts "void (*_$op\[\])() =\n    {"
    # Print the elements of the array
    foreach pn $procList {
	# Remove possible compiler prefixed leading underscore
	regsub "^$underscore" $pn "$underscore_replacement"  pn
	puts "    $pn,"
    }
    # End the array with a null
    puts "    0\n    };"
}

###############################################################################
# 
# ASMArrayEmit - Emit array as ASM directives
#
# $op is the C name of the array minus an obligatory leading underscore
# $procList is the list of function pointers to put in the array

proc ASMArrayEmit {op procList} {
    # underscore is "_" if the compiler prepends an underscore
    global underscore
    global omf
    global arch

    # Declare an array named $op
    puts "__asm__ (\".global ${underscore}_${op}\");"
    case $arch in {
	{sh}  {puts "__asm__ (\".type   ${underscore}_${op},@object\");"}
        {frv} {puts "__asm__ (\".type   ${underscore}_${op}, object\");"}
    }

    # Make sure the label is 4 bytes aligned
    puts "__asm__ (\".align 4\");"
    puts "__asm__ (\"${underscore}_${op}:\");"
    # Print the elements of the array
    foreach pn $procList {
	${omf}FunctionPtrEmit $pn
    }
    # End the array with a null
    puts "__asm__ (\"    .long 0\");"
}

###############################################################################
# 
# SOMFunctionPtrEmit - print inline assembly for a function pointer
#                      appropriate for the SIMHP assembler.
#
proc SOMFunctionPtrEmit {pn} {
    puts "__asm__ (\"    .IMPORT ${pn},CODE\");"
    puts "__asm__ (\"    .word P%${pn}\");"
}

###############################################################################
# 
# FunctionPtrEmit - print inline assembly for a function pointer
#                   appropriate for a general assembler.
#
proc FunctionPtrEmit {pn} {
    puts "__asm__ (\"    .long ${pn}\");"
}

###############################################################################
# 
# frameInfoEmit - emit frame info (de)registration function
# 

proc frameInfoEmit {lang} {
    global frameInfoNeeded

    if { $frameInfoNeeded == 0 } {
	return
    }

    ${lang}FrameInfoEmit
}

###############################################################################
# 
# CFrameInfoEmit - emit frame info (de)registration function in C
# 

proc CFrameInfoEmit {} {
    global underscore
    global underscore_replacement

    puts ""
    puts "\#ifdef __DCC__"
    puts "asm(\"     	.text\");"
    puts "asm(\"     	.section        .init\$00,4,C\");"
    puts "asm(\"	.text\");"
    puts "asm(\"	.section        .fini\$00,4,C\");"
    puts "asm(\"	.section	.frame_info\$00,4,r\");"
    puts "asm(\"${underscore}_frame_info_start:\");"
    puts "asm(\"	.section	.frame_info\$99\");"
    puts "asm(\"${underscore}_frame_info_end:\");"
    puts "\#endif"
    puts ""
    puts "\#ifdef __GNUC__"
    puts "\#error Diab C++ code must be munched and linked using the Diab toolchain"
    puts "\#endif"
    puts ""
    puts "extern unsigned long ${underscore_replacement}_frame_info_start\[\];"
    puts "extern unsigned long ${underscore_replacement}_frame_info_end\[\];"
    puts ""
    puts "extern void __frameinfo_add(void *, void *);"
    puts "extern void __frameinfo_del(void *, void *);"
    puts ""
    puts "static void ${underscore_replacement}_STI__15_ctors()"
    puts "{"
    puts "	__frameinfo_add(${underscore_replacement}_frame_info_start, ${underscore_replacement}_frame_info_end);"
    puts "}"
    puts ""
    puts "static void ${underscore_replacement}_STD__15_ctors()"
    puts "{"
    puts "	__frameinfo_del(${underscore_replacement}_frame_info_start, ${underscore_replacement}_frame_info_end);"
    puts "}"
    puts ""
    ctorAppend ${underscore}_STI__15_ctors
    dtorAppend ${underscore}_STD__15_ctors
}

###############################################################################
# 
# ASMFrameInfoEmit - emit frame info (de)registration function as asm
# 

proc ASMFrameInfoEmit {} {
    puts sterr "Diab C++ code cannot be munched using -asm. Use -c instead"
    exit 1
}

###############################################################################
# 
# moduleDestructionEmit - emit module destruction code
# 

proc moduleDestructionEmit {} {
    global underscore
    global underscore_replacement
    global moduleDestructorNeeded
    if { $moduleDestructorNeeded == 0 } {
	return
    }
    puts "static void *needed_destruction_head;"
    puts ""
    puts "extern void __record_needed_destruction_on_list(void *, void **);"
    puts "extern void __process_needed_destructions_on_list(void **);"
    puts ""
    puts "void __record_needed_destruction(void *ndp)"
    puts "{"
    puts "	__record_needed_destruction_on_list(ndp, &needed_destruction_head);"
    puts "}"
    puts ""
    puts "static void ${underscore_replacement}_STD__needed_destruction_list()"
    puts "{"
    puts "	__process_needed_destructions_on_list(&needed_destruction_head);"
    puts "}"
    puts ""
    dtorAppend ${underscore}_STD__needed_destruction_list
}

###############################################################################
# 
# deduceUnderscore - returns "_" if the compiler prepends an underscore
#

proc deduceUnderscore {arch} {
    # for the moment we default to prepending an underscore
    global underscore
    case $arch in {
	{ppc | ppce | simso | mips | mipse | \
	 hppa | sparc64 | frv | pentium | arm} {set underscore ""}
	default {set underscore "_"}
    }	
}

# Regular expression matching Diab ctors/dtors
set diabXtor {^__?ST[ID]__}

###############################################################################
#
# ctorAppend - append a ctor to the ctors list, in priority order if appropriate
#

proc ctorAppend {ctor} {
    global ctorList
    global diabXtor
    if [regexp $diabXtor $ctor] {
	diabXtorAppend $ctor ctorList
    } else {
	lappend ctorList $ctor
    }
}

###############################################################################
#
# ctorAppend - append a ctor to the dtors list, in priority order if appropriate
#

proc dtorAppend {dtor} {
    global dtorList
    global diabXtor
    if [regexp $diabXtor $dtor] {
	diabXtorAppend $dtor dtorList
    } else {
	lappend dtorList $dtor
    }
}

###############################################################################
#
# diabXtorPriorityGet - compute the priority of a Diab ctor/dtor
#

proc diabXtorPriorityGet {xtor} {
    # if a priority is explicitly encoded use that ...
    if { ! [regexp {__?ST[ID]__([0-9][0-9])} $xtor dummy priority] } {
	# otherwise use the default
	set priority 50
    }
    return $priority
}

###############################################################################
#
# diabXtorAppend - append a Diab ctor/dtor to a list in priority order
#

proc diabXtorAppend {xtor xtorListName} {
    upvar $xtorListName xtorList
    set priority [diabXtorPriorityGet $xtor]
    set xtorAdded 0
    for {set i 0} {$i < [llength $xtorList]} {incr i} {
	set current_xtor [lindex $xtorList $i]
	set current_priority [diabXtorPriorityGet $current_xtor]
	if { $current_priority > $priority } {
	    # this is a little much ... can we do an in place insertion?
	    set xtorList [linsert $xtorList $i $xtor]
	    set xtorAdded 1
	    break
	}
    }
    # if we haven't already inserted it then add it to the end of the list
    if { ! $xtorAdded } {
	set xtorList [linsert $xtorList $i $xtor]
    }
}

###############################################################################
# 
# listReverse - reverse a list
#
# Reverse $list and put the result in the variable named $name.
#

proc listReverse {inList name} {
    upvar $name outputList
    set outputList ""
    set index [llength $inList]
    while {$index != 0} {
        incr index -1
        lappend outputList [lindex $inList $index]
    }
}


###############################################################################
# 
# listRemoveLeftDuplicates - remove left duplicates from a list
#
# Without changing the order remove all but the rightmost copy of each 
# element of $inList and put the result in the variable $name.
#

proc listRemoveLeftDuplicates {inList name} {
    upvar $name outList
    listReverse $inList reversedInList
    set maxIndex [expr [llength $inList] - 1]
    set outList ""
    # Traverse list removing all but the rightmost copy of each element
    set index 0
    foreach el $inList {
        # The leftmost occurence of this element in the reversed list ...
        set reverseRightIndex [lsearch -exact $reversedInList $el] 
        # is the rightmost occurence in the original list
        set rightIndex [expr $maxIndex - $reverseRightIndex]
        if {$index == $rightIndex} {
            lappend outList $el
	}
        incr index
        }
}


###############################################################################
#
# inputProcess - Scan the input and extract ctors/dtors
# 
proc inputProcess {} {
    # Each entry is the assembler name of a ctor or a dtor
    global ctorList dtorList
    global frameInfoNeeded
    global moduleDestructorNeeded

    set frameInfoNotNeeded 0
    set moduleDestructorNotNeeded 0
    set ctorList {}
    set dtorList {}

    # define tags to detect, respectively for diab and gnu compilers
    set ctorTags {"^__?STI__" "^__?GLOBAL_.I."}
    set dtorTags {"^__?STD__" "^__?GLOBAL_.D."}

    # scan special tags in standard input 
    while {![eof stdin]} {
	# read one line
	gets stdin line

	# check if module destructor function is defined
	if { [regexp {^.*[ 	]+T[ 	]+_?__record_needed_destruction$} $line] } {
	    set moduleDestructorNotNeeded 1
	    continue
	}

	# check if module destructor function is needed
	if { [regexp {^[ 	]+U[ 	]+_?__record_needed_destruction$} $line] } {
	    set moduleDestructorNeeded 1
	    continue
	}

	# check if frame-info (de)registration functions are defined
	if { [regexp {^.*[ 	]+T[ 	]+__frameinfo_used} $line] } {
	    set frameInfoNotNeeded 1
	    continue
	}

	# check if frame-info (de)registration functions are needed
	if { [regexp {^[ 	]+U[ 	]+__frameinfo_used} $line] } {
	    set frameInfoNeeded 1
	    continue
	}

	# only consider symbols with global linkage (these show
        # up with a 'T' rather than a 't' in the nm output)
	if {! [regexp {^.*[	]*T .*$} $line]} {
	    continue
	}
	
	# extract the procedure symbol (last position in list)
	
	set procSmbl [lindex [lrange $line end end] 0]
	
	# look for special tags, and populate ctorList and dtorList
	if {$procSmbl != ""} {
            # ctors
	    foreach ctag $ctorTags {
		if [regexp $ctag $procSmbl] {
		    ctorAppend $procSmbl
		}
	    }
            # dtors
	    foreach dtag $dtorTags {
		if [regexp $dtag $procSmbl] {
		    dtorAppend $procSmbl
		}
	    }
	}
    }

    if ($moduleDestructorNotNeeded) {
       set moduleDestructorNeeded 0
    }

    if ($frameInfoNotNeeded) {
       set frameInfoNeeded 0
    }

}

###############################################################################
#
# Main Section
#

# Process command line options

set lang ""
set frameInfoNeeded 0
set moduleDestructorNeeded 0

if {[lindex $argv 0] == "-asm" } {
    set lang "ASM"
    set arch [lindex $argv 1]
    deduceUnderscore $arch
} else {
    if {[lindex $argv 0] == "-c" } {
	set lang "C"
	set arch [lindex $argv 1]
	deduceUnderscore $arch
	set underscore_replacement ""
    } else {
	# For backwards compatibility
	set lang "C"
	set underscore "__"
	set underscore_replacement "_"
    }
}

# omf is SOM (SIMHP) or ""

set omf ""

if {$lang == "ASM"} {
    case $arch in {
	"hppa" {set omf "SOM"}
    }
}


# Scan input stream

inputProcess

# Emit prologue
prologEmit $lang

# Emit frame info (de)registration functions
frameInfoEmit $lang

# Emit module destruction code
moduleDestructionEmit

# Emit array of initializers
arrayEmit $lang ctors forward $ctorList

# Emit array of finalizers in reverse
arrayEmit $lang dtors reverse $dtorList
