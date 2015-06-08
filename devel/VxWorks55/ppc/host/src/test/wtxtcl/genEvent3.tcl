# genEvent3.tcl -this file is called by wtxEventAdd.tcl for created
#	          genEvent3 file 
#
#  Copyright 1995 Wind River Systems, Inc. 
#
# modification history
# --------------------
# 01c,28jan98,p_b  Adapted for WTX 2.0
# 01b,14jun95,f_v added toolName to wtxToolAttach
# 01a,16feb95,f_v written


proc genEvent3 {tgt} {
    wtxToolAttach $tgt wtxtest
    set event ""
    for { set i 0 } { $i < 255 } { incr i } {
     lappend event x 
     }
    set event [join $event {}]

    wtxEventAdd $event
    wtxToolDetach 
 }

genEvent3\
