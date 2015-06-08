# parisc-hpux10.tcl - parisc-hpux10 commands overwritting host.tcl defines
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,18mar98,dbt  written.

# WRS Tornado global definitions

set     remoteShellCmd	remsh
set	processListCmd	"/usr/bin/ps -ef"

# user-customizable global definitions

set	printCmd	lp
set	terminal	"/usr/bin/X11/hpterm -sb -sl 1024"
