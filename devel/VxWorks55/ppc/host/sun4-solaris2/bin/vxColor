#!/bin/sh
# vxColor - graph coloring demo for Tornado
#
# Copyright 1984-1998 Wind River Systems, Inc.
# 
# modification history
# -------------------
# 01j,24mar99,dgp  docs: final formatting changes
# 01i,06oct98,dbt  doc: fixed documentation.
# 01h,20jul98,p_m  T2 doc update
# 01g,15nov96,jdi doc: cleanup, and note that demo is based on WTX and TK
#                  interpreter.
# 01f,30sep96,jco added TCL,tclX,TK,TKX_LIBRARY setting.
# 01e,09jul96,jco removed reference to environment variable WIND_HOST_TYPE
#		   in the documentation.
# 01d,13jun96,p_m get rid of objectModule parameter since we always use
#                 vxColor.o. Updated manual accordingly.
# 01c,08nov95,jco doc update, after removing of Launcher link.
# 01b,25sep95,jco doc update.
# 01a,03aug95,jco written.
#
# SYNOPSIS
# .tS
# vxColor <targetServer> [-V.erbose]
# .tE
# 
# DESCRIPTION
# This command launches the `vxColor' demo Graphical User Interface.  The
# `vxColor' demo is based on a self-stabilizing algorithm for coloring a
# planar graph.
# Given a planar graph (a 2D graph with no intersecting arc) and given 6
# colors, the aim of the algorithm is to assign each region of the graph
# a color so that no
# pair of connected regions (regions sharing a least one arc) can have the
# same color.  The main tasks in the demo are the "controller" task and
# some number of "region" tasks.
#
# The GUI can be invoked from the command line on the host shell.
# The following parameter is required and a verbose option is available:
# .IP "<targetServer>" 4
# A living target server connected to the target to be used.
# .IP `-V'
# Turn on verbose mode, which displays important steps of the
# interaction between host and target.
# .LP
# 
# NOTE: The `vxColor' demo is based on WTX and a TK-capable interpreter.
#
# LAUNCHING THE DEMO
# .IP (1) 4
# Switch on your target system (booting with VxWorks).
# .IP (2)
# Launch a target server for this target. 
# .IP (3)
# Type the command line.  For example, with a Motorola 
# MVME162 board, assuming you named your target server 
# `demo162', enter:
# .CS
#     vxColor demo162 
# .CE
# .LP
# After executing the above steps, your screen should display a window named 
# "Tornado Graph Coloring Demo".  The window displays three buttons:
# .iP (1) 
# a toggle that switches debug mode on or off 
# .iP (2) 
# a menu button that allows you to select a predefined graph to 
# run the demo with 
# .iP (3) 
# a quit button
# .LP
#
# Debug mode should be turned on to debug the target side
# of the VxColor demonstration with CrossWind.  Turning on debug mode 
# makes the stack size for the graph coloring tasks bigger in order to
# accomodate the extra space needed by local variable when the code
# is compiled with the debug option (-g) turned on.
# The header file of the demo ($WIND_BASE/target/src/demo/color/vxColor.h) 
# contains defines for different 
# target types of the stack sizes of the controller and region tasks,
# in debug or normal mode.  Normal mode demands the least memory and 
# should be used when running the demo on boards with little memory capacity. 
# You can adjust the stack size by editing the header file. 
# Whichever mode you select, remember that you can watch the stack usage 
# with the Tornado browser and that a stack overflow can lead 
# to a severe system crash.
#
# When a graph has been selected, the main window displays this graph
# as a set of colored regions separated with border lines.  The window
# displays a new set of buttons which are contextual controls.
# 
# The following gives a description of the contextual controls:
#
# .TS
# tab(|);
# l l.
# START           || Starts a coloring session
# COLOR EDIT      || Menu for changing region colors:
# | - RANDOM      | randomly initializes each region's color
# | - UNIFORM     | uniformly initializes all regions' colors
# | - LOCAL       | allows editing an individual region's color
# NEIGHBORHOOD    || Displays each region neighbors (following cursor)
# QUIT            || Exits the demo
# .TE
#
# During a coloring session, the contextual controls are:
#
# .TS
# tab(|);
# l s
# l l.
# STOP            | Breaks a coloring session
# PAUSE           | Temporarily halts a coloring session
# .TE
#
# During a PAUSE, the only contextual control is:
#
# .TS
# tab(|);
# l s
# l l.
# CONTINUE        | Resumes a coloring session after a pause
# .TE
#
# If you press the START button, you can see region colors changing 
# until a steady state is reached.
# What does all this activity mean and what does the steady state represent?
# 
# Actually, each region in the graph has a color attribute and is controlled 
# by a task running on the target.  The existing neighborhood relationships 
# within the graph are represented on the target as communication channels 
# between tasks.  Thus, the target is populated with a set of tasks that 
# have exchanges between each other.  The purpose of these exchanges
# is to honor a simple rule which says that no neighboring regions can have the 
# same color and that only six (6) different colors are available.
# This scheme has been proven to be always possible.  See 
# "A Self_stabilizing algorithm for Coloring a Planar Graph,"
# by S. Gosh and M.H. Karaata in
# .I "Distributed Computing"
# (1993) 7:55-59. 
# 
# When a solution is found, region colors freeze.  You can then break this 
# stabilized status by changing some colors using the COLOR EDIT menu.  If you
# select LOCAL mode, choose a new color for any region by clicking on the 
# desired region as many time as necessary (one out of the 6 possible colors 
# will be selected in a ring fashion).  When you are satisfied with your 
# changes, press the START button to compute a new coloring scheme.
#
# While coloring is in progress, you can use the STOP and PAUSE buttons. 
# The STOP button lets you break the target's coloring activity; the
# PAUSE button temporarily suspends this activity so that you can observe the
# tasks of the demo with various Tornado tools.  When PAUSE is on, the only 
# available action is CONTINUE, which resumes the coloring activity.
# 
# RECOMPILING THE TARGET MODULE
# The demo's target-side building technology uses a simple makefile that
# requires two parameters settings: one for CPU and the second for TOOL.
# The CPU and TOOL parameters must be set to a supported architecture 
# value and to a supported toolchain value (the default value of TOOL is
# "gnu" when omitted).
#
# .SS Examples
# To recompile the demo's target module for the listed architectures,
# enter the following, in the directory `target/src/demo/color': 
#
# Motorola 68040 board:
# .CS
#     make CPU=MC68040
# .CE
# Solaris simulator:
# .CS
#     make CPU=SIMSPARCSOLARIS
# .CE
# Intel i960CA board:
# .CS
#     make CPU=I960CA 
# .CE
#
# To produce debugging information:
# .IP (1) 4
# Recompile the demo's target module (as described here above).
# .IP (2)
# Edit the command line produced in (1) to change the optimization flag to `-g'.
# .IP (3)
# Execute this modified command line.
# .LP
#
# ENVIRONMENT VARIABLES
# .iP WIND_BASE
# root location of the Tornado tree.
# .iP WIND_REGISTRY 
# host on which the Tornado Registry daemon runs (see `wtxregd').
# .LP
#
# FILES
# The following files are required by the vxColor demo:
# .iP $WIND_BASE/host/<WIND_HOST_TYPE>/bin/wtxwish 10 3
# WTX protocol capable TK interpreter.
# .iP $WIND_BASE/host/<WIND_HOST_TYPE>/bin/vxColor 10 3
# shell script invoking `wtxwish' to interpret `demoHost.tk'.
# .iP $WIND_BASE/host/src/demo/color/demoHost.tk 10 3
# demo host GUI. 
# .iP $WIND_BASE/host/src/demo/color/United-States 10 3
# a planar graph representing the border states of the USA.
# .iP $WIND_BASE/host/src/demo/color/FranceRegions 10 3
# a planar graph representing the border regions of France.
# .iP $WIND_BASE/host/src/demo/color/Wheel 10 3
# a simple planar graph.
# .iP "$WIND_BASE/target/lib/obj<CPU><TOOL>test/vxColor.o " 10 3
# a  CPU specific demo object module.
# .LP
#
# BUGS
# If the NumLock key is on, the demo will hang.
# 
# SEE ALSO
# `tgtsvr',
# .tG "Whirlwind Tour"
# 
# NOROUTINES
#

# Environment Check

wb_message="this environment variable must be set to the root location \
of the Tornado tree."
wr_message="this environment variable must be set to the host type \
on which the Tornado Registry daemon is running."
 
: ${WIND_BASE?$wb_message} ${WIND_REGISTRY?$wr_message}

usage="usage: vxColor tgtsvr [-V.erbose]"

TCL_LIBRARY=${WIND_BASE}/host/tcl/tcl
export TCL_LIBRARY

TCLX_LIBRARY=${WIND_BASE}/host/tcl/tclX
export TCLX_LIBRARY

TK_LIBRARY=${WIND_BASE}/host/tcl/tk
export TK_LIBRARY

TKX_LIBRARY=${WIND_BASE}/host/tcl/tkX
export TKX_LIBRARY

# usefull proc
msg()           # -- print message on standard error output
{
    echo "$@" >&2
}

# check the command line
if ( (test $# -lt 1) || (test $# -gt 2) ) then
    msg ""
    msg "$usage"
    msg ""
    exit 0
fi

demoFile=$WIND_BASE/host/src/demo/color/demoHost.tk

if (test $# -eq 2) then
    wtxwish -f $demoFile $1 $2 
else
    wtxwish -f $demoFile $1 
fi
