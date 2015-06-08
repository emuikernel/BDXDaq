# 01inetstatShow.tcl - Implementation of shell inetstatShow commands in Tcl
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,15jun98,f_l  written.
#
# DESCRIPTION
# This file holds the Tcl implementation of the windsh inetstatShow command
#
# RESOURCE FILES
# wpwr/host/resource/tcl/inetstatShowCore.tcl
#

# source the data extraction and formatting routines

source [wtxPath host resource tcl]inetstatShowCore.tcl


#############################################################################
#
# inetstatShow - display all active connections for Internet protocol sockets
#
# This command displays a list of all active Internet protocol sockets in a
# format similar to the UNIX \f3netstat\fP command.
#
# SYNOPSIS: N/A
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

shellproc inetstatShow {args} {

    puts stdout "Active Internet connections (including servers)"
    puts stdout [format "%-8.8s %-5.5s %-6.6s %-6.6s  %-18.18s %-18.18s %s" \
		"PCB" "Proto" "Recv-Q" "Send-Q" "Local Address"\
		 "Foreign Address" "(state)"]
    puts stdout [format "%-8.8s %-5.5s %-6.6s %-6.6s  %-18.18s %-18.18s %s"\
		"--------" "-----" "------" "------" "------------------"\
           	"------------------" "-------"]

    inpcbPrint TCP tcpcb
    inpcbPrint UDP udb
}
