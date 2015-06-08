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
send "\et_start -n 1000 -s 100000 -f /tmp/et_sys_claswolf \r"
interact
