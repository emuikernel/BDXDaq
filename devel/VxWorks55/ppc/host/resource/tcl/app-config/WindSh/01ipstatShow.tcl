# 01ipstatShow.tcl - Implementation of shell ipstatShow command in Tcl
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,15jun98,f_l  written.
#
# DESCRIPTION
# This file holds the Tcl implementation of the windsh ipstatShow command
#
# RESOURCE FILES
# wpwr/host/resource/tcl/ipstatShowCore.tcl
#


# source the data extraction and formatting routines

source [wtxPath host resource tcl]ipstatShowCore.tcl

##############################################################################
#
# ipstatShow - display detailed statistics for the IP protocol
#
# SYNOPSIS:
#   ipstatShow [zero]
#
# PARAMETERS:
#    zero (optional): 1  = reset statistics to 0
#
# RETURNS: N/A
#
# ERRORS: N/A
#

shellproc ipstatShow {args} {

    bindNamesToList {zero} $args
    set ipstatId [ipstatIdGet]
    set ipstatInfo [ipstatList $ipstatId]
    set itemList [list total badsum tooshort toosmall badhlen badlen \
		 infragments fragdropped fragtimeout forward cantforward \
		redirectsent unknownprotocol nobuffers reassembled \
		outfragments noroute]

    # list of item shouldn't contains the ipstatId

    bindNamesToList $itemList $ipstatInfo 
    foreach item $itemList {
	puts stdout [format "%20s %4d" $item  [expr $$item]]
    }

    # clear ip statistics . Size of the ipstat structure : 0x60 bytes
    if {$zero} {
	bzero $ipstatId 0x60
    }
}
