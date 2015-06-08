#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
#
spawn /bin/tcsh
send \r\r
expect clon
send "coda_ebc -s JLabFadc -o \"EB4b CDEB\" -i \r"
interact
