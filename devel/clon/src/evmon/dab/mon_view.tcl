#!/usr/sfw/bin/wish8.3
####!/usr/bin/wish

source clear_confirm.tcl

frame .gs_or_disk
label .gs_or_disk.label -text "Data Source"
radiobutton .gs_or_disk.gs -text "Global Memory Section (clonmon1 only)" -variable gs_or_disk -value gs -anchor w
radiobutton .gs_or_disk.disk -text "Disk File" -variable gs_or_disk -value disk -anchor w

frame .screen_or_print
label .screen_or_print.label -text "Viewing Method"
radiobutton .screen_or_print.screen -text "View On Screen" -variable screen_or_print -value screen -anchor w
radiobutton .screen_or_print.print -text "Print on LPDEST" -variable screen_or_print -value print -anchor w

frame .std_or_mPlot
label .std_or_mPlot.label -text "Histogram Choice (for Monitor only)"
radiobutton .std_or_mPlot.std_electron -text "Electron Histos" -variable std_or_mPlot -value std_electron -anchor w
radiobutton .std_or_mPlot.std_photon -text "Photon Histos" -variable std_or_mPlot -value std_photon -anchor w
radiobutton .std_or_mPlot.daily -text "Daily Histos" -variable std_or_mPlot -value daily -anchor w
radiobutton .std_or_mPlot.monPlot -text "monPlot Histo Menu" -variable std_or_mPlot -value monPlot -anchor w

frame .mon_or_recon
label .mon_or_recon.label -text "Monitor or RECSIS or TRIGMON"
radiobutton .mon_or_recon.mon -text "Online Monitor" -variable mon_or_recon -value mon -anchor w
radiobutton .mon_or_recon.recon -text "Online RECSIS" -variable mon_or_recon -value recon -anchor w
radiobutton .mon_or_recon.trigmon -text "Online TRIGMON" -variable mon_or_recon -value trigmon -anchor w

frame .action
button .action.go -text "Go" -command {exec mon_paw.tcl $gs_or_disk $screen_or_print $mon_or_recon $std_or_mPlot &}
button .action.clear -text "Clear Global Sect" -command clear_confirm
button .action.dismiss -text Dismiss -command exit

canvas .hrule1 -height .1c -width 12c -background green
canvas .hrule2 -height .1c -width 12c -background green
canvas .hrule3 -height .1c -width 12c -background green
canvas .hrule4 -height .1c -width 12c -background green

pack .gs_or_disk.label .gs_or_disk .hrule1 .screen_or_print.label .screen_or_print .hrule2 .mon_or_recon.label .mon_or_recon .hrule3 .std_or_mPlot.label .std_or_mPlot .hrule4 .action -side top
pack .gs_or_disk.disk .gs_or_disk.gs -side left
pack .screen_or_print.print .screen_or_print.screen -side left
pack .std_or_mPlot.std_electron .std_or_mPlot.std_photon .std_or_mPlot.daily .std_or_mPlot.monPlot -side left
pack .mon_or_recon.mon .mon_or_recon.recon .mon_or_recon.trigmon -side left
pack .action.go .action.clear .action.dismiss -side left

.gs_or_disk.disk invoke
.screen_or_print.print invoke
.std_or_mPlot.std_photon invoke
.mon_or_recon.mon invoke
