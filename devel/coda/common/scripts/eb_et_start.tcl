#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
spawn /bin/tcsh
send \r\r
expect clon
send "\coda_eb -s clasprod -o \"EB1 CDEB\" -i\r"
interact
