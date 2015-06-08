#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
spawn /bin/tcsh
send \r\r
send "\ssh clondaq2\r"
expect clon
send "\cd et2_start\r"
send "\r"
expect clon
send "\et_start -n 4000 -s 200000 -f /tmp/et_sys_clasprod -m er -v\r"
interact

