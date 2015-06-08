# 01udpstatShow.tcl - Implementation of shell udpstatShow command in Tcl
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,15jun98,f_l  written.
#
# DESCRIPTION
# This file holds the Tcl implementation of the windsh udpstatShow command
#
# RESOURCE FILES
# wpwr/host/resource/tcl/udpstatShowCore.tcl
#


# source the data extraction and formatting routines

source [wtxPath host resource tcl]udpstatShowCore.tcl

##############################################################################
#
# udpstatShow - display statistics for the UDP protocol
#
# This routine displays statistics for the UDP protocol.
#
# SYNOPSIS: N/A
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

shellproc udpstatShow {args} {

    set udpstatInfo [udpstatInfoGet]

    bindNamesToList {udps_ipackets      \
		     udps_hdrops        \
		     udps_badsum        \
		     udps_badlen        \
		     udps_noport        \
		     udps_noportbcast   \
		     udps_fullsock      \
		     udpps_pcbcachemiss \
		     udpps_pcbhashmiss  \
		     udps_opackets      \
		     udptotal } $udpstatInfo

    puts stdout [format "UDP:\n\t%u total packets" $udptotal]
    puts stdout [format "\t%u input packets" $udps_ipackets]
    puts stdout [format "\t%u output packets" $udps_opackets]
    puts stdout [format "\t%u incomplete header%s" $udps_hdrops\
			[plural $udps_hdrops]]
    puts stdout [format "\t%u bad data length field%s" $udps_badlen\
			[plural $udps_badlen]]
    puts stdout [format "\t%u bad checksum%s" $udps_badsum \
			[plural $udps_badsum]]
    puts stdout [format "\t%u broadcasts received with no ports" \
			$udps_noportbcast [plural $udps_noportbcast]]
    puts stdout [format "\t%u full socket%s" $udps_fullsock\
			[plural $udps_fullsock]]
    puts stdout [format "\t%u pcb cache lookup%s failed"\
			$udpps_pcbcachemiss [plural $udpps_pcbcachemiss]]
    puts stdout [format "\t%u pcb hash lookup%s failed"\
			$udpps_pcbhashmiss [plural $udpps_pcbhashmiss]]
}
