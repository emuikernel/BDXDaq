#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
#
spawn /bin/tcsh
send \r\r
expect clon
send "coda_eb -s fadc1720 -o \"EB4 CDEB\" -i \r"
interact
