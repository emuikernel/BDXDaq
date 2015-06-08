#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#

#set xinc 425
#set yinc 83

set xpos  0
set xinc  530
set ypos  0
set yinc  100
set lines 5

set nlist 24
set roc_list "dc1 dc2 dc3 dc4 dc5 dc6 dc7 dc8 dc9 dc10 dc11 \
              ec1 ec2 sc1 cc1 scaler1 scaler2 scaler3 scaler4 \
              polar sc2 ec3 ec4 clastrig2"

set Lroc_list " tage tage2 croctest1 "

##set nlist 2
##set roc_list "camac1 camac2 " 

for {set i 0} {$i < $nlist} {incr i 1} {
  set roc [lindex $roc_list $i]
  puts $roc
#if [string match clastrig* $roc] {
#  set bg_color "lightblue"
#  set fg_color "black"
#} else {
  set bg_color "DarkGreen"
  set fg_color "white"
#}


  if {[string match "* $roc *" $Lroc_list]} {
    exec xterm -title "$roc" -sb -sl 1000 \
               -geometry 83x$lines+$xpos+$ypos -bg $bg_color -fg $fg_color \
               -e prxt_linux.tcl $roc &
  } else {

    if [string match clastrig* $roc] {
      exec xterm -title "$roc" -sb -sl 1000 \
                 -geometry 83x30+$xpos+$ypos -bg $bg_color -fg $fg_color \
                 -e prxt.tcl $roc &
    } else {
      exec xterm -title "$roc" -sb -sl 1000 \
                 -geometry 83x$lines+$xpos+$ypos -bg $bg_color -fg $fg_color \
                 -e prxt.tcl $roc &
    }
  }


  set ypos [expr $ypos + $yinc]
  if {($i == 9) || ($i == 19)} {
    set xpos [expr $xpos + $xinc]
    set ypos 0
  }
}

