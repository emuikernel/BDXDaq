#!/opt/sfw/bin/expect --
#
#
#
set filename [lindex $argv 0]
set screen_or_print [lindex $argv 1]
set kumac_name [lindex $argv 2]
set filename_ps /tmp/paw_standard.ps
spawn /bin/tcsh -f
expect ">"
send "set prompt=\"clon> \"\r"
send "paw\r"
expect "Workstation type"
send \r
expect "PAW >"
if {$screen_or_print == "print"} {
    send "opt zfl\r"
    expect "PAW >"
}
set timeout 60
send "exe $kumac_name $filename $screen_or_print\r"
if {$screen_or_print == "print"} {
    expect "PAW >"
    send "exe \$MONITOR/kumac/postscript $filename_ps\r"
    send "shel lpr -P clonhp $filename_ps\r"
    expect "PAW >"
#
# something is wrong, set big timeout to make sure print is completed
#    set timeout 10
    set timeout 600
#
    send "quit\r"
    expect "clonmon1"
#    send "exit\r"
#    interact
} else {
    interact
}
#$Id: paw_standard.tcl,v 1.9 2001/01/20 23:46:53 marki Exp $
#
# end of expect file

