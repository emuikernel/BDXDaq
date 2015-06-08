#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
spawn /bin/tcsh
send \r\r
send "\ssh \$CLON_EB\r"
expect clon
send "\cd et2er\r"
send "\et_2_et clondaq1:/tmp/et_sys_primex clondaq2-daq2:/tmp/et_sys_primex ET2ER\r"
interact
