#!/usr/sfw/bin/tclsh8.3
####!/usr/bin/tclsh

set gs_or_disk [lindex $argv 0]
set screen_or_print [lindex $argv 1]
set mon_or_recon [lindex $argv 2]
set standard_or_monPlot [lindex $argv 3]
if {$gs_or_disk == "disk"} {
    exec mon_disk.tcl $screen_or_print $mon_or_recon $standard_or_monPlot
} else {
    if {[string compare $mon_or_recon "mon"] == 0} {
	set global_sect MON
    } else {
	if {[string compare $mon_or_recon "trigmon"] == 0} {
	    set global_sect TRIG
	} else {
	    set global_sect RCON
	}
    }
    exec xterm -sb -sl 1000 -e paw_monitor.csh $global_sect $screen_or_print $mon_or_recon $standard_or_monPlot &
}
exit

