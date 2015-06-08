#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
#
spawn /bin/tcsh
send \r\r
expect clon
send "et_start -n 1000 -s 100000 -f /tmp/et_sys_fadc1720 \r"
interact
