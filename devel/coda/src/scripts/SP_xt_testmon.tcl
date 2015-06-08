#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
#
set roc $argv 
spawn /bin/tcsh
send \r\r
expect clon
send "source .login \r"
expect Welcome
expect clon
puts $roc
send "setup_epics_R3.14.12 \r"
expect clon

if       { $roc == "alh"  } {
  set epics   "alh hv_cr.alhConfig"
} elseif { $roc == "medm" } {
  set epics   "medm -x cr_mf.adl test_caen.adl"
} elseif { $roc == "telnet" } {
  set epics   "telnet hvtest 1527"
} else {
  set epics   ""
}

send "$epics \r"
send "\r"
interact
