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
send "\coda_er -s claswolf -o \"ER2 ER\" -i \r"
interact
