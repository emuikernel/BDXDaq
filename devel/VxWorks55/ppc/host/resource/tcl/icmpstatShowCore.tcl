# icmpstatShowCore.tcl - Implementation of icmpstatShow core routines in Tcl
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01a,15jun98,f_l  written.
#
# DESCRIPTION
# This module is the Tcl code for the icmpstatShow core routines.
#
# RESOURCE FILES
# 


##############################################################################
#
# icmpstatInfoGet - get the icmp statistics
#
# The Gopher script gets fields of the icmpstat structure defined in
# /netinet/icmp_var.h:
# .CS
#     typedef struct icmpstat 
#         {
#         u_long  icps_error;
#         u_long  icps_oldshort;
#         u_long  icps_oldicmp;
#         u_long  icps_outhist[ICMP_MAXTYPE + 1];
#         u_long  icps_badcode;
#         u_long  icps_tooshort;
#         u_long  icps_checksum;
#         u_long  icps_badlen;
#         u_long  icps_reflect;
#         u_long  icps_inhist[ICMP_MAXTYPE + 1];
#         };
# .CE
#
# SYNOPSIS: N/A
#
# PARAMETERS: N/A
#
# RETURNS:
#   a list of items:
#       icps_error, 
#       icps_oldshort, 
#       icps_oldicmp,
#       icps_outhist (sub-list of 19 elements), 
#       icps_badcode, 
#       icps_tooshort, 
#       icps_checksum, 
#       icps_badlen, 
#       icps_reflect,
#       icps_inhist (sub-list of 19 elements)
#
# ERRORS:
#    icmp statistics list not found
#

proc icmpstatInfoGet {} {

    # get the address of the icmp statistics from the icmpstat
    # symbol if it exists

    set icmpstatAddr [symAddrGet icmpstat "icmp statistics not found"]
    set result [wtxGopherEval "$icmpstatAddr @ @ @  (19<@> +4)\ 
				+76 @ @ @ @ @  (19<@> +4)"]
    return "[lrange $result 0 2] [list [lrange $result 3 21]]\
	 [lrange  $result 22 26] [list [lrange $result 27 end]] "
}
