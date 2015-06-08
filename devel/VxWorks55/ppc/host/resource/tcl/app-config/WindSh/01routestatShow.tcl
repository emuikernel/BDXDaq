# 01routestatShow.tcl - Implementation of shell routestatShow command in Tcl
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,15jun98,f_l  written.

# DESCRIPTION
# This file holds the Tcl implementation of the windsh routestatShow command
#
# RESOURCE FILES
# wpwr/host/resource/tcl/routestatShowCore.tcl
#


# source the data extraction and formatting routines

source [wtxPath host resource tcl]routestatShowCore.tcl

##############################################################################
#
# routestatShow - display routing statistics
#
# This routine displays routing statistics.
#
# RETURNS: N/A
#

shellproc routestatShow {args} {

    set rtstatInfo [rtstatInfoGet]
    bindNamesToList {rts_badredirect rts_dynamic rts_newgateway rts_unreach\
		rts_wildcard} $rtstatInfo
    puts stdout "routing:"
    puts stdout [format "\t%u bad routing redirect%s" $rts_badredirect\
			[plural $rts_badredirect]]
    puts stdout [format "\t%u dynamically created route%s" $rts_dynamic\
			[plural $rts_dynamic]]
    puts stdout [format "\t%u new gateway%s due to redirects" $rts_newgateway\
			[plural $rts_newgateway]]
    puts stdout [format "\t%u destination%s found unreachable" $rts_unreach\
			[plural $rts_unreach]]
    puts stdout [format "\t%u use%s of a wild card route" $rts_wildcard\
			[plural $rts_wildcard]]
}

