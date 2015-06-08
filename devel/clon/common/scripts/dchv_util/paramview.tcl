#!/bin/sh
#\
exec $CODA_BIN/bltwish1.8 -f "$0" ${1+"$@"}
#

proc positionWindow w {
    wm geometry $w +190+735
}
proc paramview {} {
global stat_par
global w
global env clonwsrc

set w .text
catch {destroy $w}
toplevel $w
wm title $w "Parameters Packet Structure"
wm iconname $w "text"
positionWindow $w

frame $w.buttons
pack  $w.buttons -side bottom -expand y -fill x -pady 2m
button $w.buttons.dismiss -text Dismiss -command "destroy $w"

pack $w.buttons.dismiss -side left -expand 1

text $w.text -yscrollcommand "$w.scroll set" -setgrid true \
	-width 125 -height 15 -wrap word

scrollbar $w.scroll -command "$w.text yview"
pack $w.scroll -side right -fill y
pack $w.text -expand yes -fill both
$w.text delete 1.0 end
if {$stat_par == 1} {
$w.text insert 0.0 {NAME                VMON         IMON
}
} elseif {$stat_par == 2} {
$w.text insert 0.0 {NAME                                                     V0          V1    I0    I1   Vmax    RUP  RDWN  TRIP 
}
} else { puts o
}
set f [open ${clonwsrc}/vme/mondat/dc_mon.dat]
while { ![eof $f]} {
$w.text insert end [read $f 1000]
}
close $f

