#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#

set   nn   [llength $argv]
if { $nn < 1 } { puts " No input arguments"; exit }

spawn /bin/tcsh
send \r
expect clon

if {[string match "*coda_roc*" $argv]} {
  set roc [lindex [split [lindex [split [lindex [split $argv] 4] '"'] 1] '\}'] 0]
  if { [string length $roc] < 1 } { puts "\n Wrong  roc  name \n"; exit }
  send "ssh $roc \r"
  send \r
  expect "$roc"
  send \r
  expect "not known" { puts "\n Wrong roc name \n"; exit }
}

if {[string match "*tsconnect*" $argv]} {
  send "ssh clon10 \r"
  send \r
  expect "clon10"
  send \r
}

set str ""
foreach copm $argv { set str "$str $copm" }
send \r
send $str
send \r
interact
