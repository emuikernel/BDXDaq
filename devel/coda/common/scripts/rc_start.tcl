#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
spawn /bin/tcsh
send \r\r
expect clon
send "setenv SESSION clasprod\r"
send "runcontrol -s clasprod -cedit -dbedit \r"
interact


