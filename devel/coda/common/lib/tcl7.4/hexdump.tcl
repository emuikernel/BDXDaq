#	"@(#)tclStruct:library:hexdump.tcl	1.1	95/09/08"
#
# Written by Matthew Costello
# (c) 1995 AT&T Global Information Solutions, Dayton Ohio USA
#
# See the file "license.terms" for information on usage and
# redistribution of this file, and for a DISCLAIMER OF ALL WARRANTIES.

# This an auxilary function to be used to dump out the
# contents of a structure in both hexidecimal and ASCII.

proc struct_hexdump {obj} {
    puts "Dumping Structure $obj"
    set addr [uplevel struct_info object $obj address]
    #puts "Address = $addr"
    set len [uplevel struct_info object $obj size]
    #puts "Length = $len"
    set offset 0

    while {$len > 0} {
	set nbytes $len
	if {$nbytes > 16} {set nbytes 16}
	puts -nonewline [format " %.4x " $offset]
	struct_new ptr ubyte*16 ubyte*16@[expr $addr + $offset]
	for {set i 0} {$i < 16} {incr i 1} {
	    if {$i >= $nbytes} {puts -nonewline "   "} \
	    else {puts -nonewline " $ptr($i._hex_)"}
	}
	puts -nonewline " "
	for {set i 0} {$i < 16} {incr i 1} {
	    if {$i >= $nbytes} {puts -nonewline " "} \
	    elseif {$ptr($i) < 32 || $ptr($i) > 127} {puts -nonewline "."} \
	    else {puts -nonewline "$ptr($i._char_)"}
	}
	puts {}
	unset ptr
	incr offset 16
	incr len -16
    }
}

