#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#

set xpos  1920
set xinc  400
set ypos  0
set yinc  90
set lines 4

set nlist 26
set roc_list "dc1 dc2 dc3 dc4 dc5 dc6 dc7 dc8 dc9 dc10 dc11 \
              ec1 ec2 sc1 cc1 scaler1 scaler2 scaler3 scaler4 \
              polar sc2 ec3 ec4 tage tage2 clastrig2"

set Lroc_list " tage tage2 "

for {set i 0} {$i < $nlist} {incr i 1} {
  set roc [lindex $roc_list $i]
  puts $roc
  set bg_color "DarkGreen"
  set fg_color "white"


  if {[string match "* $roc *" $Lroc_list]} {
    exec xterm -title "$roc" -sb -sl 1000 \
               -geometry 41x$lines+$xpos+$ypos -bg $bg_color -fg $fg_color \
               -e prxt_linux.tcl $roc &
  } else {

    if [string match clastrig* $roc] {
      exec xterm -title "$roc" -sb -sl 1000 \
                 -geometry 41x28+$xpos+$ypos -bg $bg_color -fg $fg_color \
                 -e prxt.tcl $roc &
    } else {
      exec xterm -title "$roc" -sb -sl 1000 \
                 -geometry 41x$lines+$xpos+$ypos -bg $bg_color -fg $fg_color \
                 -e prxt.tcl $roc &
    }
  }

  set ypos [expr $ypos + $yinc]
  if {($i == 9) || ($i == 19)} {
    set xpos [expr $xpos + $xinc]
    set ypos 0
  }
}


set xpos [expr $xpos + $xinc]
set xinc  400
set ypos  0
set yinc  90
set lines 4

set mlist 19
set pmc_list "dc1pmc1 dc2pmc1 dc3pmc1 dc4pmc1 dc5pmc1 dc6pmc1 dc7pmc1 dc8pmc1 dc9pmc1 dc10pmc1 dc11pmc1 \
              ec1pmc1 ec2pmc1 sc1pmc1 cc1pmc1 sc2pmc1 ec3pmc1 ec4pmc1 polarpmc1"

for {set i 0} {$i < $mlist} {incr i 1} {
  set roc [lindex $pmc_list $i]
  puts $roc
  set bg_color "LightBlue"
  set fg_color "Black"

  exec xterm -title "$roc" -sb -sl 1000 \
             -geometry 41x$lines+$xpos+$ypos -bg $bg_color -fg $fg_color \
             -e prxt.tcl $roc &

  set ypos [expr $ypos + $yinc]
  if {($i == 9) || ($i == 19)} {
    set xpos [expr $xpos + $xinc]
    set ypos 0
  }
}
