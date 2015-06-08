#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
spawn /bin/tcsh
send \r\r
expect clon
send "\coda_er -s clasprod -o \"ER1 ER\" -i\r"
interact
