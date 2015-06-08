#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
#
spawn /bin/tcsh
send \r\r
expect clon
send "killall -9 et_start \r "
send "sleep 3 \r"
send "et_start -v -n 1000 -s 100000 -f /tmp/et_sys_JLabFadc \r"
interact
