#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
spawn /bin/tcsh
send \r\r
send "\ssh \$CLON_EB\r"
expect clon
send "\cd coda_l3\r"
send "\coda_l3 -a clasprod -s clasprod\r"
interact
