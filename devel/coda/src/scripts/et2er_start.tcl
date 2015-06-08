#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
spawn /bin/tcsh
send \r\r
send "\ssh clondaq1\r"
expect clon
send "\cd et2er\r"
send "\et_2_et clondaq1:/tmp/et_sys_clasprod clondaq2-daq2:/tmp/et_sys_clasprod ET2ER\r"
interact
