# sun4-solaris2.tcl - sun4-solaris2 commands overwritting host.tcl defines
#
# Copyright 1995-1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01j,29jan99,cth  adding map of quit to CNTL-\ for VxSim Sol 2.6 (SPR 23967)
# 01i,30nov98,jmp  removed installCDCmd var, it is correctly set in
#                  hostUtils.tcl.
# 01h,18mar98,dbt  replaced ps with /usr/bin/ps. Added Copyright.
# 01g,07oct97,cth  changed processListCmd format, related to (SPR 8839)
# 01f,23jan97,pad  overwrote cdrom path defined in host.tcl.
# 01e,05nov96,jco  centralizing installCDCmd into host.tcl.
# 01d,26feb96,jco  added processListCmd and terminal, layout clean up.
# 01c,02oct95,jco  added remoteShellCmd.
# 01b,28aug95,jcf  added installCDCmd
# 01a,20jun95,jco  written.

# WRS Tornado global definitions
  
set     remoteShellCmd  remsh
set	processListCmd	"/usr/bin/ps -ef"

# user-customizable global definitions
#
# The '^\' xterm option below is a single character equivelent to CNTL-\.
#

set 	printCmd	lp
set     terminal        "xterm -sb -sl 1024 -tm \"quit \""
