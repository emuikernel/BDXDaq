#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
set xpos 938
set ypos 0
set yinc 145
set lines 7

set fg_color "black"

# Starting runcontrol

set bg_color "lightgreen"
exec xterm -title "RC_TERM_PRIMEX" -sb -sl 1000 -geometry 70x$lines+$xpos+$ypos -bg $bg_color -fg $fg_color -e rc_primex_start.tcl &

# Starting ET systems on CLON_EB and CLON_ER nodes

set ypos [expr $ypos + $yinc]
set bg_color "lightblue"
exec xterm -title "ET1_TERM_PRIMEX" -sb -sl 1000 -geometry 70x$lines+$xpos+$ypos -bg $bg_color -fg $fg_color -e et1_primex_start_.tcl &

set ypos [expr $ypos + $yinc]
set bg_color "lightblue"
exec xterm -title "ET2_TERM_PRIMEX" -sb -sl 1000 -geometry 70x$lines+$xpos+$ypos -bg $bg_color -fg $fg_color -e et2_primex_start_.tcl &

exec sleep 10


# Starting other processes on CLON_EB and CLON_ER node

set ypos [expr $ypos + $yinc]
set bg_color "yellow"
exec xterm -title "EB_TERM_PRIMEX" -sb -sl 1000 -geometry 70x$lines+$xpos+$ypos -bg $bg_color -fg $fg_color -e eb_primex_start_.tcl $argv &

set ypos [expr $ypos + $yinc]
set bg_color "yellow"
exec xterm -title "ER_TERM_PRIMEX" -sb -sl 1000 -geometry 70x$lines+$xpos+$ypos -bg $bg_color -fg $fg_color -e er_primex_start_.tcl $argv &

set ypos [expr $ypos + $yinc]
set bg_color "yellow"
exec xterm -title "ET2ER_TERM_PRIMEX" -sb -sl 1000 -geometry 70x$lines+$xpos+$ypos -bg $bg_color -fg $fg_color -e et2er_primex_start_.tcl &

exit
