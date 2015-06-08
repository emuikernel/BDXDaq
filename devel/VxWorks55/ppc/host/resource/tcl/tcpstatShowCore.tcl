# tcpstatShowCore.tcl - Implementation of tcpstatShow core routines
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,15jun98,f_l  written.
#
# DESCRIPTION
# This module is the Tcl code for the tcpstatShow core routines. 
#
# RESOURCE FILES
# 


##############################################################################
#
# tcpstatList - get all statistics for the TCP protocol
# 
# The Gopher script gets fields of the tcpstat structure defined in
# netinet/tcp_var.h:
# .CS
#     struct  tcpstat 
#         {
#         u_long  tcps_connattempt;
#         u_long  tcps_accepts;
#         u_long  tcps_connects;
#         u_long  tcps_drops;
#         u_long  tcps_conndrops;
#         u_long  tcps_closed;
#         u_long  tcps_segstimed;
#         u_long  tcps_rttupdated;
#         u_long  tcps_delack;
#         u_long  tcps_timeoutdrop;
#         u_long  tcps_rexmttimeo;
#         u_long  tcps_persisttimeo;
#         u_long  tcps_keeptimeo;
#         u_long  tcps_keepprobe;
#         u_long  tcps_keepdrops;
#         u_long  tcps_sndtotal;
#         u_long  tcps_sndpack;
#         u_long  tcps_sndbyte;
#         u_long  tcps_sndrexmitpack;
#         u_long  tcps_sndrexmitbyte;
#         u_long  tcps_sndacks;
#         u_long  tcps_sndprobe;
#         u_long  tcps_sndurg;
#         u_long  tcps_sndwinup;
#         u_long  tcps_sndctrl;
#         u_long  tcps_rcvtotal;
#         u_long  tcps_rcvpack;
#         u_long  tcps_rcvbyte;
#         u_long  tcps_rcvbadsum;
#         u_long  tcps_rcvbadoff;
#         u_long  tcps_rcvshort;
#         u_long  tcps_rcvduppack;
#         u_long  tcps_rcvdupbyte;
#         u_long  tcps_rcvpartduppack;
#         u_long  tcps_rcvpartdupbyte;
#         u_long  tcps_rcvoopack;
#         u_long  tcps_rcvoobyte;
#         u_long  tcps_rcvpackafterwin;
#         u_long  tcps_rcvbyteafterwin;
#         u_long  tcps_rcvafterclose;
#         u_long  tcps_rcvwinprobe;
#         u_long  tcps_rcvdupack;
#         u_long  tcps_rcvacktoomuch;
#         u_long  tcps_rcvackpack;
#         u_long  tcps_rcvackbyte;
#         u_long  tcps_rcvwinupd;
#         u_long  tcps_pawsdrop;
#         u_long  tcps_predack;
#         u_long  tcps_preddat;
#         u_long  tcps_pcbcachemiss;
#         u_long  tcps_persistdrop;
#         u_long  tcps_badsyn;
#         };
# .CE
#
# SYNOPSIS: N/A
#
# PARAMETERS: N/A
#
# RETURN: 
#   a list of items:
#       tcps_connattempt,
#       tcps_accepts,
#       tcps_connects,
#       tcps_drops,
#       tcps_conndrops,
#       tcps_closed,
#       tcps_segstimed, 
#       tcps_rttupdated, 
#       tcps_delack,
#       tcps_timeoutdrop, 
#       tcps_rexmttimeo, 
#       tcps_persisttimeo, 
#       tcps_keeptimeo,
#       tcps_keepprobe, 
#       tcps_keepdrops, 
#       tcps_sndtotal, 
#       tcps_sndpack, 
#       tcps_sndbyte, 
#       tcps_sndrexmitpack, 
#       tcps_sndrexmitbyte, 
#       tcps_sndacks, 
#       tcps_sndprobe, 
#       tcps_sndurg, 
#       tcps_sndwinup, 
#       tcps_sndctrl, 
#       tcps_rcvtotal,
#       tcps_rcvpack, 
#       tcps_rcvbyte, 
#       tcps_rcvbadsum, 
#       tcps_rcvbadoff, 
#       tcps_rcvshort,
#       tcps_rcvduppack, 
#       tcps_rcvdupbyte, 
#       tcps_rcvpartduppack, 
#       tcps_rcvpartdupbyte, 
#       tcps_rcvoopack,
#       tcps_rcvoobyte, 
#       tcps_rcvpackafterwin,
#       tcps_rcvbyteafterwin, 
#       tcps_rcvafterclose, 
#       tcps_rcvwinprobe, 
#       tcps_rcvdupack, 
#       tcps_rcvacktoomuch,
#       tcps_rcvackpack, 
#       tcps_rcvackbyte, 
#       tcps_rcvwinupd,
#
# ERRORS:
#   TCP statistics not found
#

proc tcpstatList {} {

    # get the address of the tcp statistics from the tcpstat symbol 
    # if it exists

    set tcpstatAddr [symAddrGet tcpstat "TCP statistics not found"]
    return [wtxGopherEval "$tcpstatAddr @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\
				@@@@@@@@@@@@@"]
}
