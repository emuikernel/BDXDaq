#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#

    set xpos 0
    set xinc 640
    set ypos 0
    set yinc 160
    set lines 8
set nlist 8
set roc_list "primexroc4 primexroc5 primexroc6 tage tage2 tage3 primextdc1 primexts2"
   for {set i 0} {$i < $nlist} {incr i 1} {
	set roc [lindex $roc_list $i]
	puts $roc
	if [string match primexts2 $roc] {
	    set bg_color "lightblue"
	    set fg_color "black"
	exec xterm -title "$roc" -sb -sl 1000 \
		-geometry 67x29+$xpos+$ypos -bg $bg_color -fg $fg_color \
		-e prxt.tcl $roc &
	} else {
	    set bg_color "DarkGreen"
	    set fg_color "white"
	exec xterm -title "$roc" -sb -sl 1000 \
		-geometry 67x$lines+$xpos+$ypos -bg $bg_color -fg $fg_color \
		-e prxt.tcl $roc &
	}

	set ypos [expr $ypos + $yinc]
	if {$i == 4} {
	    set xpos [expr $xpos + $xinc]
	    set ypos 0
	}	    

    }







