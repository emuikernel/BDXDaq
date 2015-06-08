#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
set roc $argv 
spawn /bin/tcsh
send \r\r
expect clon00
puts $roc
send "cu -l /dev/cua/$roc \r"
send "\r"
interact
