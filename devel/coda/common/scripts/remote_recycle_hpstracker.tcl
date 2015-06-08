#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#

set roc $argv

set roc_name "hpstracker"

set recycle_line "recycle2"

#bank (always 1 unless several devices are desy-chained
set target1 "1"

#port 1-8
set target2 "1"

set i [lsearch -exact $roc_name $roc]
set rline [lindex $recycle_line $i]
set hop1 [lindex $target1 $i]
set hop2 [lindex $target2 $i]

spawn tsconnect $rline
expect "help."
##expect ">"

send "of $hop1 $hop2\r"
expect ">"

sleep 5

send "on $hop1 $hop2\r"
expect ">"

send "~x"

exit
