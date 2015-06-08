# ipstatShowCore.tcl - Implementation of ipstatShow core routines
#
# Copyright 1998 - 2003 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01b,13jan03,vvv  merged from branch wrn_velocecp, ver01b (SPR #79870)
# 01a,15jun98,f_l  written.

# DESCRIPTION
# This module is the Tcl code for the ipstatShow core routines.
#
# RESOURCE FILES
#


##############################################################################
#
# ipstatIdGet - get the ID of the ipstat symbole 
#
# This routine is a binding to the symAddrGet routine.
#
# SYNOPSIS: N/A
#
# PARAMETERS: N/A
#
# RETURNS: ID of the ipstat symbole
#
# ERRORS: 
#   IP statistics not found
#

proc ipstatIdGet {} {

    # get the address of th IP statistics from the symbol ipstat if it exists

    set result [symAddrGet ipstat "IP statistics not found"]
    return $result
}

##############################################################################
#
# ipstatList - get detailed statistics for the IP protocol
#
# The Gopher script gets the fields of the ipstat structure defined in
# /netinet/ip_var.h.
# .CS
# 
# struct	ipstat {
# 	u_long	ips_total;		/* total packets received */
# 	u_long	ips_badsum;		/* checksum bad */
# 	u_long	ips_tooshort;		/* packet too short */
# 	u_long	ips_toosmall;		/* not enough data */
# 	u_long	ips_badhlen;		/* ip header length < data size */
# 	u_long	ips_badlen;		/* ip length < ip header length */
# 	u_long	ips_fragments;		/* fragments received */
# 	u_long	ips_fragdropped;	/* frags dropped (dups, out of space) */
# 	u_long	ips_fragtimeout;	/* fragments timed out */
# 	u_long	ips_forward;		/* packets forwarded */
# 	u_long	ips_cantforward;	/* packets rcvd for unreachable dest */
# 	u_long	ips_redirectsent;	/* packets forwarded on same net */
# 	u_long	ips_noproto;		/* unknown or unsupported protocol */
# 	u_long	ips_delivered;		/* datagrams delivered to upper level*/
# 	u_long	ips_localout;		/* total ip packets generated here */
# 	u_long	ips_odropped;		/* lost packets due to nobufs, etc. */
# 	u_long	ips_reassembled;	/* total packets reassembled ok */
# 	u_long	ips_fragmented;		/* datagrams sucessfully fragmented */
# 	u_long	ips_ofragments;		/* output fragments created */
# 	u_long	ips_cantfrag;		/* don't fragment flag was set, etc. */
# 	u_long	ips_badoptions;		/* error in option processing */
# 	u_long	ips_noroute;		/* packets discarded due to no route */
# 	u_long	ips_badvers;		/* ip version != 4 */
# 	u_long	ips_rawout;		/* total raw ip packets generated */
# };
# .CE
#
# SYNOPSIS:
#   ipstatList ipstatId
#
# PARAMETERS:
#   ipstatId: ID of the ipstat symbole 
#
# RETURNS:
#   a list of 17 items:
#   ips_total ips_badsum ips_tooshort ips_toosmall ips_badhlen ips_badlen
#   ips_fragments ips_fragdropped ips_fragtimeout ips_forward ips_cantforward
#   ips_redirectsent, ips_noproto, ips_odropped, ips_reassembled,
#   ips_ofragments, ips_noroute
#
# ERRORS: N/A
#

proc ipstatList {ipstatId} {
    return [wtxGopherEval "$ipstatId @@@@@@@@@@@@@ +8 @@ +4 @ +8 @"]
}
