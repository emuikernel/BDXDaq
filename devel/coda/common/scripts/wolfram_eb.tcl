#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
spawn /bin/tcsh
send \r\r
##send "\ssh clon00\r"
##send "\setenv EXPID clasdev\r"
##send "\setenv SESSION claswolf\r"
##expect clon
expect wolfram
send "coda_eb -s claswolf -o \"EB2 CDEB\" -i \r"
interact
