# genListEvent.tcl - This file is called by wtxListEventGet.tcl
#		  for created genListEvent file 
#
#  Copyright 1998 Wind River Systems, Inc. 
#
# modification history
# --------------------
# 01a,28jan98,p_b  written


proc genListEvent {tgt} {
    wtxToolAttach $tgt wtxtest
    wtxEventAdd EVENTTEST01
    wtxEventAdd EVENTTEST02
    wtxEventAdd EVENTTEST03
    wtxToolDetach 
}

genListEvent\
