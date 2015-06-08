#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
spawn /bin/tcsh
send \r\r
send "\ssh \$CLON_EB\r"
expect clon
if       { $argv == "BOS"  } {
    send "\coda_eb -s primex -o \"EB4 CDEB\" -i\r"
} elseif { $argv == "CODA" } {
    send "\coda_ebc -s primex -o \"EB5 CDEB\" -i\r"
} else { }
interact
