#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
spawn /bin/tcsh
send \r\r
expect clon
send "setenv SESSION primex\r"
send "runcontrol -s primex -cedit -dbedit \r"
interact


