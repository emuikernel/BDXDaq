#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
#
spawn /bin/tcsh
send \r\r
expect clon
send "runcontrol -s fadc1720 -cedit -dbedit \r"
interact
