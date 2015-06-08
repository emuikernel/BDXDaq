#!/opt/sfw/bin/expect --
set file_no_path [lindex $argv 0]
set screen_or_print [lindex $argv 1]
if {$file_no_path != "MON"} then {
    set filename /hist/monitor/$file_no_path
} else {
    set filename MON
}
spawn paw
expect "Workstation type"
send \r
expect CLASPAW>
if {$screen_or_print == "print"} {
puts "To print, use monPlot print option.\r"
}
send \r
expect CLASPAW>
#send_user $filename\r
send "clas $filename\r"
interact
