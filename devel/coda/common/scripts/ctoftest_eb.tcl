#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
spawn /bin/tcsh
send \r\r
send "\setenv MSQL_TCP_HOST clasonl1\r"
expect clon
send "coda_eb -s ctoftest -o \"EB6 CDEB\" -i \r"
interact
