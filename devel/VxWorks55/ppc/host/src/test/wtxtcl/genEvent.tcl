# genEvent.tcl - this file is called by wtxRegisterForEventTest.tcl
#
#  Copyright 1995 Wind River Systems, Inc. 
#
# modification history
# --------------------
# 01b,14jun95,f_v added toolName to wtxToolAttach
# 01a,15feb95,f_v written


proc genEvent {tgt} {
    wtxToolAttach $tgt wtxtest
    wtxToolDetach 
 }

genEvent\
