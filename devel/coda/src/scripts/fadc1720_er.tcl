#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
#
spawn /bin/tcsh
send "cd /usr/local/clas \r"
send \r\r
expect clon
send "coda_er -s fadc1720 -o \"ER4 ER\" -i \r"
interact
