#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
spawn /bin/tcsh
send \r\r
expect wolfram
send "runcontrol -s claswolf \r"
interact
