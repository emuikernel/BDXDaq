# genEvent2.tcl -this file is called by wtxEventAdd.tcl,wtxEventGetTest.tcl
#		  for created genEvent2 file 
#
#  Copyright 1995 Wind River Systems, Inc. 
#
# modification history
# --------------------
# 01c,28jan98,p_b  Adapted for WTX 2.0
# 01b,14jun95,f_v added toolName to wtxToolAttach
# 01a,16feb95,f_v written


proc genEvent2 {tgt} {
    wtxToolAttach $tgt wtxtest
    wtxEventAdd EVENTTEST
    wtxToolDetach 
 }

genEvent2\
