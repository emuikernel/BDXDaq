#!/bin/sh
#\
exec $CODA_BIN/bltwish1.8 -f "$0" ${1+"$@"}
#
wm title  . "Dchv"
wm iconname . "DCHV"
wm geometry . 1200x710
set c .c

global stopik

button .wait -bitmap @/apps/coda/97_06_16/source/tk4.0/bitmaps/hourglass -width 3c -height 3c -relief ridge -borderwidth 8
pack .wait -side top

proc blink { w option value1 value2 interval} {
global stopik

$w config $option $value1
if {$stopik == 0} {
after $interval [list blink $w $option $value2 $value1 $interval]
}
}

set stopik 0
blink .wait -bg yellow green 500
