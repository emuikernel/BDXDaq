#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
#
spawn /bin/tcsh
send "cd /usr/local/clas \r"
send \r\r
expect clon
send "coda_erc -s JLabFadc -o \"ER4b ER\" -i \r"
interact
