#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
spawn /bin/tcsh
send \r\r
###send "\ssh clasonl1\r"
###send "\source /home/boiarino/.cshrc\r"
send "\setenv MSQL_TCP_HOST clasonl1\r"
expect clon
send "coda_eb -s claspcal -o \"EB8 CDEB\" -i \r"
interact
