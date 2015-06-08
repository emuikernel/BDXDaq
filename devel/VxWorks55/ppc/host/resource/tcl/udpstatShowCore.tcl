# udpstatShowCore.tcl - Implementation of udpstatShow core routines
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,15jun98,f_l  written.
#
# DESCRIPTION
# This module is the Tcl code for the udpstatShow core routines. 
#
# RESOURCE FILES
# 


##############################################################################
#
# udpstatInfoGet - get the udp statistics
#
# The Gopher script gets fields of the udpstat structure defined in
# netinet/udp_var.h:
# .CS
#     typedef struct udpstat
#         {
#         u_long  udps_ipackets;
#         u_long  udps_hdrops;
#         u_long  udps_badsum;
#         u_long  udps_badlen;
#         u_long  udps_noport;
#         u_long  udps_noportbcast;
#         u_long  udps_fullsock;
#         u_long  udpps_pcbcachemiss;
#         u_long  udpps_pcbhashmiss;
#         u_long  udps_opackets;
#         };
# .CE
#
# SYNOPSIS: N/A
#
# PARAMETERS: N/A
#
# RETURNS:
#   a list of 11 items:
#       udps_ipackets,
#       udps_hdrops,
#       udps_badsum,
#       udps_badlen,
#       udps_noport,
#       udps_noportbcast,
#       udps_fullsock,
#       udpps_pcbcachemiss,
#       udpps_pcbhashmiss,
#       udps_opackets,
#       udptotal
#
# ERRORS:
#   UDP statistics not found
#

proc udpstatInfoGet {} {

    # get the address of the UDP statistics from the udpstat symbol 
    # if it exists

    set udpstatAddr [symAddrGet udpstat "UDP statistics not found"]

    # get udps_ipackets udps_hdrops udps_badsum udps_badlen udps_noport
    # udps_noportbcast udps_fullsock udpps_pcbcachemiss udpps_pcbhashmiss
    # udps_opackets 

    set result [wtxGopherEval "$udpstatAddr @ @ @ @ @ @ @ @ @ @"]

    # udptotal = udps_ipackets + udps_opackets

    set udptotal [expr [lindex $result 0] + [lindex $result 9]]
    lappend result $udptotal

    return $result
}
