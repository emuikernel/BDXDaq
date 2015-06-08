#!/opt/sfw/bin/expect --
set filename [lindex $argv 0]
set screen_or_print [lindex $argv 1]
spawn /bin/tcsh -f
expect ">"
send "set prompt=\"clon> \"\r"
if {[string compare $filename "RCON"] == 0} {
    send "setenv RECON_HIST_DIR RCON\r"
} else {
    send "setenv RECON_HIST_DIR lun1\r"
}
send "paw\r"
expect "Workstation type"
send \r
expect "PAW >"
if {[string compare $filename "RCON"] == 0} {
    send "global_sect RCON\r"
} else {
    send "h/file 1 /hist/recon/$filename\r"
}
if {$screen_or_print == "print"} {
    expect "PAW >"
    if {[string compare $filename "RCON"] == 0} {
	send "exe recon_print\r"
    } else {
	send "exe recon_print_file\r"
    }
    expect "PAW >"
    send "quit\r"
    expect ">"
    send "exit\r"
} else {
    expect "PAW >"
    send "exec welcome\r"
    interact
}
