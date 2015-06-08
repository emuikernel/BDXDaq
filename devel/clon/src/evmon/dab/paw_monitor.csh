#! /bin/tcsh -f
set filename=$1
set screen_or_print=$2
set mon_or_recon=$3
set standard_or_monPlot=$4
/bin/rm -f ~/higz_windows.dat
if ($mon_or_recon == "mon") then
    if ($standard_or_monPlot == "std_electron") then
	cd $MONITOR/kumac/standard
	$DAB/paw_standard.tcl $filename $screen_or_print monstd_electron
    else if ($standard_or_monPlot == "std_photon") then
	cd $MONITOR/kumac/standard
	$DAB/paw_standard.tcl $filename $screen_or_print monstd_photon
    else if ($standard_or_monPlot == "daily") then
	cd $MONITOR/kumac/standard
	$DAB/paw_standard.tcl $filename $screen_or_print monstd_daily
    else
	cd $MONITOR
	$DAB/paw_monPlot.tcl $filename $screen_or_print
    endif
else
    if ($mon_or_recon == "trigmon") then
	cd $TRIGMON
	$DAB/paw_trigmon.tcl $filename $screen_or_print
    else
	cd $RECON
	$DAB/paw_recon.tcl $filename $screen_or_print
    endif
endif
