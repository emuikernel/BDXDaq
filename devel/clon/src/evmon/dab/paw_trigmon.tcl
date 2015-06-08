#!/opt/sfw/bin/expect --
set filename [lindex $argv 0]
set screen_or_print [lindex $argv 1]
spawn /bin/tcsh -f
expect ">"
send "set prompt=\"clon> \"\r"
send "paw\r"
expect "Workstation type"
send \r
expect "PAW >"
if {[string compare $filename "TRIG"] == 0} {
    send "global_sect TRIG\r"
} else {
    send "h/file 1 /hist/trigmon/$filename\r"
}
expect "PAW >"
if {$screen_or_print == "print"} {
    send "exe level2mon print $filename \r"
    expect "PAW >"
    send "quit\r"
    expect ">"
    send "lp l2_summary.ps\r"
    send "exit\r"
} else {
    send "exe level2mon screen \r"
    interact
}
