#!/usr/bin/sh
#\
exec /opt/sfw/bin/expectk -f "$0" ${1+"$@"}

#####exec /usr/bin/expectk -f "$0" ${1+"$@"}




proc ScrolledListbox {parent args} {
    frame $parent
    eval {listbox $parent.list\
	    -yscrollcommand [list $parent.sy set] \
	    -xscrollcommand [list $parent.sx set]} $args
    scrollbar $parent.sx -orient horizontal \
	    -command [list $parent.list xview]
    scrollbar $parent.sy -orient vertical \
	    -command [list $parent.list yview]
    pack $parent.sx -side bottom -fill x
    pack $parent.sy -side right -fill y
    pack $parent.list -side left -fill both -expand true
    return $parent.list
}
#
#
#
set screen_or_print [lindex $argv 0]
set mon_or_recon [lindex $argv 1]
set standard_or_monPlot [lindex $argv 2]
#
###source ScrolledListbox.tcl
#
if {[string compare $mon_or_recon "mon"] == 0} {
    set hist_dir /hist/monitor
} else {
    if {[string compare $mon_or_recon "trigmon"] == 0} {
	set hist_dir /hist/trigmon
    } else {
	set hist_dir /hist/recon
    }
}
#
#
#
set ls [exec ls -r $hist_dir]
send_user $ls\r
ScrolledListbox .files -width 25 -height 20 -setgrid true -selectmode single
frame .buttons
button .buttons.cancel -text Cancel -command {
    exit
}
button .buttons.select -text "Select File" -command {
    set file_selected [selection get]
    exec xterm -sb -sl 1000 -e paw_monitor.csh $file_selected \
	    $screen_or_print $mon_or_recon $standard_or_monPlot &
###example	    screen mon std_electron &
    exit
}
pack .buttons.select .buttons.cancel -side left
pack .files.list -fill both -expand true
set nlist [llength $ls]
for {set i 0} {$i < $nlist} {incr i 1} {
    set filename [lindex $ls $i]
    .files.list insert end $filename
}
pack .files .buttons -side top
