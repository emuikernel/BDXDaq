global history
global DANGER
global cont

set histo_memory 100
set parms $env(CLON_PARMS)/dchv
 set clon $env(CLON)

set bin ${clon}/common/scripts
set frik 0

# ===== For debugging ====
set cont 0
set history 100
# ========================


while { $frik == 0 } {
    if { $cont == 0 } {
set DANGER 111
catch { set currun [ exec run_number ]  } result
    catch { exec dchv_mon } ercatch
#    exec $bin/dchv_mon 
set DANGER 100
#--sleeping inbackgraound: that is possibilities to work when he is waiting--
after 20
#-- if there is no beam copy monitored data in to nb_current.dat file --
set ff [open ${parms}/ibeam.txt r]
while {[gets $ff line] >=0} {
set current [lindex $line 0]
}
if { $current == 0.0} {
    catch { exec cp ${parms}/archive/Run_0${currun}.dat ${parms}/nb_currents.dat} ercatch
}
#-- generate file for timeline
#	catch { exec genfile4tg } ercatch
	 exec $bin/genfile4tg 
for { set hop 1 } { $hop <= 11111 } { incr hop } {
update
#puts " $hop $DANGER"
after 20
}
# histo_memory constant shows the depth of the histogram filled in the $CLON_SOURCE/stadis/gui/dctimeline
if { $history < $histo_memory } {
    catch { exec cat $parms/online_currents.txt >> $parms/onl_current_history.txt } ercatch
    catch { exec date >> $parms/time_of_measurements.txt } ercatch
set history [ expr $history + 1 ]
#puts " DEBUG1 $history $histo_memory "
} else {
set hopla [ exec date ]
set name [lindex $hopla 1]_[lindex $hopla 2]_[lindex $hopla 3]_history.txt
set tname [lindex $hopla 1]_[lindex $hopla 2]_[lindex $hopla 3]_time.txt
catch { exec cp $parms/onl_current_history.txt $parms/archive/$name } errcatch
catch { exec cp $parms/time_of_measurements.txt $parms/archive/$tname } errcatch
catch { exec cat $parms/online_currents.txt > $parms/onl_current_history.txt } ercatch
catch { exec date > $parms/time_of_measurements.txt } ercatch
# ===== Open file for debugging messages
#catch { set f [ open ${clonparms}/dchv/debug.txt w ] } ercatch
#set ercatch [ lindex $ercatch 0 ]
#if { $ercatch == "couldn't" } {
#puts " Error debug file"
#} else {
#puts $f " $history $histo_memory "
#close $f
#}
#puts " DEBUG2 $history $histo_memory "
set history 0
}
}
puts $history
}


