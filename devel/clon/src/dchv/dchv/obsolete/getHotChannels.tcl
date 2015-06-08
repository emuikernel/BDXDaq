#!/bin/sh
#\
exec $CODA_BIN/dpsh -f "$0" ${1+"$@"}
#
global clonparms
set voltage_thr 500
set bc_mon 0.0
set clonparms $env(CLON_PARMS)

set f [open ${clonparms}/dchv/ibeam.txt] 
gets $f line
set bc [lindex $line 0]
close $f

puts $bc

if { $bc > $bc_mon } {
set run_number [ exec run_number]
puts $run_number

# create noew empty file for hot channels
set f [open ${clonparms}/dchv/hot_channels.iconf w]
close $f

set f [open ${clonparms}/dchv/archive/Run_0${run_number}.dat] 
while { [gets $f line] >=0 } {
    set chcode [lindex $line 0]
    set monv   [ lindex $line 3]
puts  "$chcode $monv"

    if { $monv < $voltage_thr } {
	catch {	exec grep $chcode /usr/local/clas/parms/dchv/vme/type0_all.iconf >> /usr/local/clas/parms/dchv/hot_channels.iconf } ercatch
    }
}
close $f
}

exit 0
