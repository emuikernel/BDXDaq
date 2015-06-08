#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
spawn /bin/tcsh
send \r\r
send "\ssh \$CLON_ER\r"
expect clon
if       { $argv == "BOS"  } {
    send "\coda_er -s primex -o \"ER4 ER\" -i\r"
} elseif { $argv == "CODA" } {
    send "\coda_erc -s primex -o \"ER5 ER\" -i\r"
} else { }
interact
