#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#

    set xpos 0
    set xinc 640
    set ypos 0
    set yinc 160
    set lines 8
set nlist 7
set roc_list "primexroc4pmc1 primexroc5pmc1 primexroc6pmc1 tagepmc1 tage2pmc1 tage3pmc1 primextdc1pmc1"
   for {set i 0} {$i < $nlist} {incr i 1} {
	set roc [lindex $roc_list $i]
	puts $roc
	set bg_color "lightblue"
	set fg_color "black"
	exec xterm -title "$roc" -sb -sl 1000 \
		-geometry 67x$lines+$xpos+$ypos -bg $bg_color -fg $fg_color \
		-e prxt.tcl $roc &

	set ypos [expr $ypos + $yinc]
	if {$i == 4} {
	    set xpos [expr $xpos + $xinc]
	    set ypos 0
	}	    

    }







