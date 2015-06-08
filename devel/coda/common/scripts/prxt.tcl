#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
#
set roc $argv 
spawn /bin/tcsh
send \r\r
expect clon
puts $roc
send "tsconnect $roc\r"
send "\r"
interact
