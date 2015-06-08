#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
spawn /bin/tcsh
send \r\r
send "\ssh \$CLON_ER\r"
expect clon
send "\cd coda_er\r"
send "\coda_er -s clasprod -o \"ER3 ER\" -i\r"
interact
