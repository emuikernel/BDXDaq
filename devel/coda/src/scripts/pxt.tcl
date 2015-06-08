#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
    set xpos 0
#    set xinc 425
    set xinc 530
    set ypos 0
#    set yinc 83
    set yinc 100
    set lines 5
set nlist 19
set roc_list "dc1pmc1 dc2pmc1 dc3pmc1 dc4pmc1 dc5pmc1 dc6pmc1 dc7pmc1 dc8pmc1 dc9pmc1 dc10pmc1 dc11pmc1
 cc1pmc1 ec1pmc1 ec2pmc1 sc1pmc1 sc2pmc1 ec3pmc1 ec4pmc1 polarpmc1" 
   for {set i 0} {$i < $nlist} {incr i 1} {
	set roc [lindex $roc_list $i]
	puts $roc
	set bg_color "lightblue"
	set fg_color "black"
	exec xterm -title "$roc" -sb -sl 1000 \
		-geometry 83x$lines+$xpos+$ypos -bg $bg_color -fg $fg_color \
		-e prxt.tcl $roc &

	set ypos [expr $ypos + $yinc]
	if {($i == 9) || ($i == 18)} {
	    set xpos [expr $xpos + $xinc]
	    set ypos 0
	}	    
    }







