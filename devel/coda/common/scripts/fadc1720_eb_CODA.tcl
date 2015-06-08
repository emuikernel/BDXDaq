#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
#
spawn /bin/tcsh
send \r\r
expect clon
send "coda_ebc -s fadc1720 -o \"EB4a CDEB\" -i \r"
interact
