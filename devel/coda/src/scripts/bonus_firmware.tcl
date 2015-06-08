#!/bin/sh
#\
exec /usr/bin/expect -- "$0" ${1+"$@"}
#
# ssh to bonus crate '10.ip2' as root and load firmware
# we have:
#   upper right   1.3.242
#   bottom right  1.3.241
#   upper left    0.2.254
#   bottom left   0.2.253
#
set ip2 $argv 
set ip1 "10."
spawn /bin/tcsh
##send "ssh root@10.1.3.241\r"
send "ssh root@$ip1$ip2\r"
expect "password:"
send "dcs\r"
expect "$ "
send "cd /eg6\r"
expect "$ "
##################send "./program rcu_181009.bit\r"
send "./program rcu_26A009.bit\r"
expect "$ "
send "exit\r\r "
#send \r
#interact
