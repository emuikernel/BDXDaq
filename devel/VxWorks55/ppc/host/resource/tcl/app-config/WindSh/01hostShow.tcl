# 01hostShow.tcl - Implementation of shell hostShow command in Tcl
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01b,19feb99,jmp  fixed hostShow() aliases formating (SPR# 25201).
# 01a,15jun98,f_l  written.
#
# DESCRIPTION
# This file holds the Tcl implementation of the windsh hostShow command
#
# RESOURCE FILES
# wpwr/host/resource/tcl/hostShowCore.tcl
#


# source the data extraction and formatting routines

source [wtxPath host resource tcl]hostShowCore.tcl

##############################################################################
#
# hostShow - display the host table
#
# This command prints a list of remote hosts, along with their Internet
# addresses and aliases.
#
# The vxWorks version of this routine does a semTake so as to be sure the
# hosts list is not changed while being displayed. This windsh version 
# doesn't use a semaphore. So it might not work in any cases.
#
# SYNOPSIS: N/A
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

shellproc hostShow {args} {

    puts stdout "hostname         inet address       aliases"
    puts stdout "--------         ------------       -------"

    foreach hostEntry [hostEntryList] {
	set hostInfo [hostNameAddr $hostEntry]

	puts stdout [format "%-16s %-18s %s" [lindex $hostInfo 4] \
	    [ifAddrFormat [lrange $hostInfo 0 3]] [hostAliasesList $hostEntry]]
    }
}
