#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
#
spawn /bin/tcsh
send "cd /usr/local/clas \r"
send \r\r
expect clon
send "coda_er -s clastest -o \"ER5 ER\" -i \r"
interact
