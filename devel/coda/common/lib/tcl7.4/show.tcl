#	"@(#)tclStruct:library:show.tcl	1.2	95/09/08"
#
# Written by Matthew Costello
# (c) 1995 AT&T Global Information Solutions, Dayton Ohio USA
#
# See the file "license.terms" for information on usage and
# redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.

# Define a function to be used for showing the contents
# of any tclStruct object.

proc struct_show {objname} {
    # Parse $objname into the object name and index
    set o $objname
    set i {}
    regexp {^([^(]*)\((.*)\)$} $objname match o i
    #puts "o = $o, i = $i"
    upvar $o obj
    puts "Showing Structure $objname"
    struct_show_piece "    $o" obj $i 
}

 proc struct_show_piece {prefix objname elemname} {
    #puts "prefix = $prefix, obj = $objname, elemname = $elemname"
    upvar $objname obj
    set kind [struct_info object obj($elemname) type kind]
    #puts "kind = $kind"
    if {[struct_info object obj($elemname) type basic]} {
	# A simple type
	if {[catch {puts "${prefix}($elemname) = $obj($elemname)"} x]} {
	    puts "${prefix}($elemname) = $x"
	}
    } elseif {$kind == "struct"} {
	set elemprefix $elemname
	if {[string length $elemprefix] != 0} {append elemprefix .}
	foreach elem [struct_info object obj($elemname) type elemname] {
	    
	    struct_show_piece $prefix obj $elemprefix$elem
	}
    } elseif {$kind == "array"} {
	set elemprefix $elemname
	if {[string length $elemprefix] != 0} {append elemprefix .}
	set nelem [expr [struct_info object obj($elemname) size] / \
	                [struct_info object obj(${elemprefix}0) size]]
	for {set i 0} {$i < $nelem} {incr i} {
	    struct_show_piece $prefix obj $elemprefix$i
	}
    } else {
	# normally a pointer
	if {[catch {puts "${prefix}($elemname) = $obj($elemname)"} x]} {
	    puts "${prefix}($elemname) = $x"
	}
    }
}
