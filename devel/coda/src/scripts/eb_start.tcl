#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
spawn /bin/tcsh
send \r\r
send "\ssh clondaq1\r"
expect clon
send "\cd coda_eb\r"
send "\coda_eb -s clasprod -o \"EB2 CDEB\" -i\r"
interact
