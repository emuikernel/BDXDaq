#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
#
set roc $argv 
set dop "_testsetup"
spawn /bin/tcsh
send \r\r
send "ssh clon10\r"
send "perl -e 'print \"\\033]0;$roc$dop\\007\";' \r"
expect clon
puts $roc
send "tsconnect $roc\r"
send "\r"
interact
