# 01tcpstatShow.tcl - Implementation of shell tcpstatShow command in Tcl
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,15jun98,f_l  written.
#
# DESCRIPTION
# This file holds the Tcl implementation of the windsh tcpstatShow command
#
# RESOURCE FILES
# wpwr/host/resource/tcl/tcpstatShowCore.tcl
#


# source the data extraction and formatting routines

source [wtxPath host resource tcl]tcpstatShowCore.tcl

##############################################################################
#
# tcpstatShow - display all statistics for the TCP protocol
#
# This routine displays detailed statistics for the
# TCP protocol.
#
# SYNOPSIS: N/A
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# ERRORS: N/A
#

shellproc tcpstatShow {args} {
    
    set tcpstatInfo [tcpstatList]

    bindNamesToList {tcps_connattempt     \
		     tcps_accepts         \
		     tcps_connects        \
		     tcps_drops           \
		     tcps_conndrops       \
		     tcps_closed          \
		     tcps_segstimed       \
		     tcps_rttupdated      \
		     tcps_delack          \
		     tcps_timeoutdrop     \
		     tcps_rexmttimeo      \
		     tcps_persisttimeo    \
		     tcps_keeptimeo       \
		     tcps_keepprobe       \
		     tcps_keepdrops       \
		     tcps_sndtotal        \
		     tcps_sndpack         \
		     tcps_sndbyte         \
		     tcps_sndrexmitpack   \
		     tcps_sndrexmitbyte   \
		     tcps_sndacks         \
		     tcps_sndprobe        \
		     tcps_sndurg          \
		     tcps_sndwinup        \
		     tcps_sndctrl         \
		     tcps_rcvtotal        \
		     tcps_rcvpack         \
		     tcps_rcvbyte         \
		     tcps_rcvbadsum       \
		     tcps_rcvbadoff       \
		     tcps_rcvshort        \
		     tcps_rcvduppack      \
		     tcps_rcvdupbyte      \
		     tcps_rcvpartduppack  \
		     tcps_rcvpartdupbyte  \
		     tcps_rcvoopack       \
		     tcps_rcvoobyte       \
		     tcps_rcvpackafterwin \
		     tcps_rcvbyteafterwin \
		     tcps_rcvafterclose   \
		     tcps_rcvwinprobe     \
		     tcps_rcvdupack       \
		     tcps_rcvacktoomuch   \
		     tcps_rcvackpack      \
		     tcps_rcvackbyte      \
		     tcps_rcvwinupd} $tcpstatInfo	
     
    puts stdout "TCP:"
    puts stdout [format "\t%d packet%s sent" $tcps_sndtotal \
			[plural $tcps_sndtotal]]
    puts stdout [format "\t\t%d data packet%s (%d byte%s)" $tcps_sndpack\
		[plural $tcps_sndpack] $tcps_sndbyte [plural $tcps_sndbyte]]
    puts stdout [format "\t\t%d data packet%s (%d byte%s) retransmitted"\
			$tcps_sndrexmitpack [plural $tcps_sndrexmitpack]\
			$tcps_sndrexmitbyte [plural $tcps_sndrexmitbyte]]
    puts stdout [format "\t\t%d ack-only packet%s (%d delayed)" $tcps_sndacks\
			[plural $tcps_sndacks] $tcps_delack]
    puts stdout [format "\t\t%d URG only packet%s" $tcps_sndurg \
			[plural $tcps_sndurg]]
    puts stdout [format "\t\t%d window probe packet%s" $tcps_sndprobe\
			[plural $tcps_sndprobe]]
    puts stdout [format "\t\t%d window update packet%s" $tcps_sndwinup \
			[plural $tcps_sndwinup]]
    puts stdout [format "\t\t%d control packet%s" $tcps_sndctrl \
			[plural $tcps_sndctrl]]
    puts stdout [format "\t%d packet%s received" $tcps_rcvtotal \
			[plural $tcps_rcvtotal]]
    puts stdout [format "\t\t%d ack%s (for %d byte%s)" $tcps_rcvackpack \
			[plural $tcps_rcvackpack] $tcps_rcvackbyte\
			[plural $tcps_rcvackbyte]]
    puts stdout [format "\t\t%d duplicate ack%s" $tcps_rcvdupack\
			[plural $tcps_rcvdupack]]
    puts stdout [format "\t\t%d ack%s for unsent data" $tcps_rcvacktoomuch \
			[plural $tcps_rcvacktoomuch]]
    puts stdout [format "\t\t%d packet%s (%d byte%s) received in-sequence"\
			$tcps_rcvpack [plural $tcps_rcvpack] $tcps_rcvbyte\
			[plural $tcps_rcvbyte]]
    puts stdout [format "\t\t%d completely duplicate packet%s (%d byte%s)" \
			$tcps_rcvduppack [plural $tcps_rcvduppack]\
			$tcps_rcvdupbyte [plural $tcps_rcvdupbyte]]
    puts stdout [format "\t\t%d packet%s with some dup. data \
				(%d byte%s duped)" $tcps_rcvpartduppack \
			[plural $tcps_rcvpartduppack]\
			$tcps_rcvpartdupbyte [plural $tcps_rcvpartdupbyte]]
    puts stdout [format "\t\t%d out-of-order packet%s (%d byte%s)"\
			$tcps_rcvoopack [plural $tcps_rcvoopack]\
			$tcps_rcvoobyte [plural $tcps_rcvoobyte]]
    puts stdout [format "\t\t%d packet%s (%d byte%s) of data after window"\
			$tcps_rcvpackafterwin [plural $tcps_rcvpackafterwin] \
			$tcps_rcvbyteafterwin [plural $tcps_rcvbyteafterwin]]
    puts stdout [format "\t\t%d window probe%s" $tcps_rcvwinprobe \
			[plural $tcps_rcvwinprobe]]
    puts stdout [format "\t\t%d window update packet%s" $tcps_rcvwinupd\
			[plural $tcps_rcvwinupd]]
    puts stdout [format "\t\t%d packet%s received after close" \
			$tcps_rcvafterclose [plural $tcps_rcvafterclose]]
    puts stdout [format "\t\t%d discarded for bad checksum%s" \
			$tcps_rcvbadsum [plural $tcps_rcvbadsum]]
    puts stdout [format "\t\t%d discarded for bad header offset field%s"\
			$tcps_rcvbadoff [plural $tcps_rcvbadoff]]
    puts stdout [format "\t\t%d discarded because packet too short"\
			$tcps_rcvshort]
    puts stdout [format "\t%d connection request%s" $tcps_connattempt \
			[plural $tcps_connattempt]]
    puts stdout [format "\t%d connection accept%s" $tcps_accepts \
			[plural $tcps_accepts]]
    puts stdout [format "\t%d connection%s established (including accepts)"\
			$tcps_connects [plural $tcps_connects]]
    puts stdout [format "\t%d connection%s closed (including %d drop%s)" \
			$tcps_closed [plural $tcps_closed] $tcps_drops \
			[plural $tcps_drops]]
    puts stdout [format "\t%d embryonic connection%s dropped" $tcps_conndrops\
			[plural $tcps_conndrops]]
    puts stdout [format "\t%d segment%s updated rtt (of %d attempt%s)" \
			$tcps_rttupdated [plural $tcps_rttupdated] \
			$tcps_segstimed [plural $tcps_segstimed]]
    puts stdout [format "\t%d retransmit timeout%s" $tcps_rexmttimeo \
			[plural $tcps_rexmttimeo]]
    puts stdout [format "\t\t%d connection%s dropped by rexmit timeout"\
			$tcps_timeoutdrop [plural $tcps_timeoutdrop]]
    puts stdout [format "\t%d persist timeout%s" $tcps_persisttimeo \
			[plural $tcps_persisttimeo]]
    puts stdout [format "\t%d keepalive timeout%s" $tcps_keeptimeo \
			[plural $tcps_keeptimeo]]
    puts stdout [format "\t\t%d keepalive probe%s sent" $tcps_keepprobe\
			[plural $tcps_keepprobe]]
    puts stdout [format "\t\t%d connection%s dropped by keepalive" \
			$tcps_keepdrops [plural $tcps_keepdrops]]
}
