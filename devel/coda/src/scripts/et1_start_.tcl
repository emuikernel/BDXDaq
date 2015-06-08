#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
spawn /bin/tcsh
send \r\r
send "\ssh \$CLON_EB\r"
expect clon
send "\cd et1_start\r"
send "\r"
expect clon
send "\et_start -n 4000 -s 200000 -f /tmp/et_sys_clasprod -m eb -v\r"
interact

