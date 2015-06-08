#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
spawn /bin/tcsh
send \r\r
expect clon
send "\r"
expect clon
send "\et_start -n 4000 -s 200000 -f /tmp/et_sys_clasprod -m daq -v\r"
interact

