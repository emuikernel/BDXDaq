# ifShowCore.tcl - Implementation of the ifShow core routines in Tcl
#
# Copyright 1998 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01b,06aug99,pfl  fixed spr 28174
# 01a,15jun98,f_l  written.
#
# DESCRIPTION
# This module is the Tcl code for the ifShow core routines.
#
# RESOURCE FILES
# 


##############################################################################
#
# netIfList - get the id of all attached network interfaces
#
# (From 'Tornado API guide, section 5 Extending Tornado Tools').
# This routine gets the id of all attached network interfaces.
# The Gopher script gets the 'if_next' field of the ifnet structure defined 
# in /net/if.h:
#. CS
#     typedef struct ifnet 
#         {
#         char    *if_name;
#         struct  ifnet *if_next;
#         struct  ifaddr *if_addrlist;
#         int     if_pcount;
#         caddr_t if_bpf;
#         u_short if_index;
#         short   if_unit;
#         short   if_timer;
#         short   if_flags;
#         struct  if_data if_data; 
#         struct  mBlk *  pInmMblk;
#         int     (*if_init) (int unit);
#         int     (*if_output) (struct ifnet *, 
#				struct mbuf *, 
#				struct sockaddr *,
#                 		struct rtentry *);
#         int     (*if_start) (struct ifnet *);
#         int     (*if_ioctl) (struct ifnet *, int cmd, caddr_t data);
#         int     (*if_reset) (int unit);
#         int     (*if_resolve)();
#         void    (*if_watchdog) (int unit);
#         struct  ifqueue if_snd;
#         };
# .CE
#
# SYNOPSIS: N/A
#
# PARAMETERS: N/A
#
# RETURNS: 
#   a list of ids
#
# ERRORS:
#    ifnet not found
#

proc netIfList {} {

    # get the value of ifnet symbol if it exists

    set ifnet [symValueGet ifnet "ifnet not found"]
    return [wtxGopherEval "$ifnet {! +4 *}"]
}

##############################################################################
#
# netIfInfo - get  information about an attached network interface
#
# (From 'Tornado API guide, section 5 Extending Tornado Tools').
# This routine gets get  information about an attached network interface.
# The Gopher script gets fields of the ifnet structure defined in
# /net/if.h:
#. CS
#     typedef struct ifnet 
#         {
#         char    *if_name;
#         struct  ifnet *if_next;
#         struct  ifaddr *if_addrlist;
#         int     if_pcount;
#         caddr_t if_bpf;
#         u_short if_index;
#         short   if_unit;
#         short   if_timer;
#         short   if_flags;
#         struct  if_data if_data; 
#         struct  mBlk *  pInmMblk;
#         int     (*if_init) (int unit);
#         int     (*if_output) (struct ifnet *, 
#				struct mbuf *, 
#				struct sockaddr *,
#                 		struct rtentry *);
#         int     (*if_start) (struct ifnet *);
#         int     (*if_ioctl) (struct ifnet *, int cmd, caddr_t data);
#         int     (*if_reset) (int unit);
#         int     (*if_resolve)();
#         void    (*if_watchdog) (int unit);
#         struct  ifqueue if_snd;
#         };
#
#     typedef struct if_data 
#         {
#         u_char  ifi_type;
#         u_char  ifi_addrlen;
#         u_char  ifi_hdrlen;
#         u_long  ifi_mtu;
#         u_long  ifi_metric;
#         u_long  ifi_baudrate;
#         u_long  ifi_ipackets;
#         u_long  ifi_ierrors;
#         u_long  ifi_opackets;
#         u_long  ifi_oerrors;
#         u_long  ifi_collisions;
#         u_long  ifi_ibytes;
#         u_long  ifi_obytes;
#         u_long  ifi_imcasts;
#         u_long  ifi_omcasts;
#         u_long  ifi_iqdrops;
#         u_long  ifi_noproto;
#         u_long  ifi_lastchange;
#         };
# .CE
#
# SYNOPSIS:
#   netIfInfo netIf
#
# PARAMETERS:
#   netIf : id of an attached network interface.
#
# RETURNS:
#   a list of items:
#       if_name, 
#       if_unit, 
#       if_flags, 
#       ifi_type, 
#       ifi_mtu, 
#       ifi_metric, 
#       ifi_ipackets,
#       ifi_ierrors, 
#       ifi_opackets, 
#       ifi_oerrors, 
#       ifi_collisions, 
#       ifi_imcasts,
#       ifi_omcasts, 
#       ifi_iqdrops
#
# ERRORS: N/A
#

proc netIfInfo {netIf} {

    # Get the fields describe above from the ifnet structure

    return [wtxGopherEval "$netIf <* $> +22 @w +2 @w  @b +3 @ @\
		 +4 @ @ @ @ @ +8 @ @ @"]
}

##############################################################################
#
# netIfAddrList - get  the list of IP adresses for one device
#
# (From 'Tornado API guide, section 5 Extending Tornado Tools').
# This routine gets the list of IP adresses for one device.
# The Gopher script gets fields of the ifnet structure defined in
# /net/if.h, (selected fields : ifa_addr.sa_family , ifa_addr.sa_data ,
# ifu_dsaddr.sa_data and ifa_next):
#. CS
#     typedef struct ifnet 
#         {
#         char    *if_name;
#         struct  ifnet *if_next;
#         struct  ifaddr *if_addrlist;
#         int     if_pcount;
#         caddr_t if_bpf;
#         u_short if_index;
#         short   if_unit;
#         short   if_timer;
#         short   if_flags;
#         struct  if_data if_data; 
#         struct  mBlk *  pInmMblk;
#         int     (*if_init) (int unit);
#         int     (*if_output) (struct ifnet *, 
#				struct mbuf *, 
#				struct sockaddr *,
#                 		struct rtentry *);
#         int     (*if_start) (struct ifnet *);
#         int     (*if_ioctl) (struct ifnet *, int cmd, caddr_t data);
#         int     (*if_reset) (int unit);
#         int     (*if_resolve)();
#         void    (*if_watchdog) (int unit);
#         struct  ifqueue if_snd;
#         };
#
#     typedef struct ifaddr
#         {
#         struct  sockaddr *ifa_addr;
#         struct  sockaddr *ifa_dstaddr;
#         struct  sockaddr *ifa_netmask;
#         struct  ifnet *ifa_ifp;
#         struct  ifaddr *ifa_next;
#         void    (*ifa_rtrequest)();
#         u_short ifa_flags;
#         short   ifa_refcnt;
#         int     ifa_metric;
#         };
#
#     typedef struct sockaddr 
#         {
#         u_char  sa_len;
#         u_char  sa_family;
#         char    sa_data[14];
#         };
# .CE
#
# SYNOPSIS:
#   netIfAddrList netIf
#
# PARAMETERS:
#   netIf : id of an attached network interface.
#
# RETURNS:
#   a list of items for each node of the if_addrlist list:
#       ifa_addr.sa_family , 
#       ifa_addr.sa_data , 
#       ifa_dsaddr.sa_data or (no destination address)
#       ifa_addr.sa_family , 
#       ifa_addr.sa_data 
#
# ERRORS: N/A
#

proc netIfAddrList {netIf} {

    # gopher request with the sentinel 0xEEEE so as to mark the beginning
    # of each node

    set result [wtxGopherEval "$netIf +8 <* { <0xEEEE !>\
		<*  +1 @b  +2 @b@b@b@b> +4 <*{ +2  +2 @b@b@b@b 0}>  +12* } >"]
    
    # cut the result list into sub lists. One list for each node

    return [sentiListCut $result 0xeeee]
}


##############################################################################
#
# ifFlagsFormat - convert flags into a meaningful string
#
# (From 'Tornado API guide, section 5 Extending Tornado Tools').
# Convert flags into a meaningful string.
#
# SYNOPSIS:
#   ifFlagsFormat flags
#
# PARAMETERS:
#   flags 
#
# RETURNS:
#   a messgage corresponding to the flags
#
# ERRORS: N/A
#

proc ifFlagsFormat {flags} {
    set result ""

    # IFF_UP = 0x1

    if {$flags & 0x1} {
	append result "UP "
    } else {
	append result "DOWN "
    }

    # IFF_BROADCAST = 0x2

    if {$flags & 0x2}		{append result "BROADCAST "}

    # IFF_DEBUG = 0x4

    if {$flags & 0x4}		{append result "DEBUG "}

    # IFF_LOOPBACK = 0x8

    if {$flags & 0x8}		{append result "LOOPBACK "}

    # IFF_POINTOPOINT = 0x10

    if {$flags & 0x10}		{append result "POINT-TO-POINT "}

    # IFF_NOTRAILERS = 0x20

    if {!($flags & 0x20)}	{append result "TRAILERS "}

    # IFF_RUNNING = 0x40

    if {$flags & 0x40}		{append result "RUNNING "}

    # IFF_NOARP = 0x80

    if {!($flags & 0x80)}	{append result "ARP "}

    # IFF_PROMISC = 0x100

    if {$flags & 0x100}		{append result "PROMISCUOUS "}

    # IFF_ALLMULTI = 0x200 not tested 
    # IFF_OACTIVE = 0x400 not tested
    # IFF_SIMPLEX = 0x800 not tested
    # IFF_LINK0 = 0x1000 not tested
    # IFF_LINK1 = 0x2000 not tested
    # IFF_LINK2 = 0x4000 not tested

    # IFF_MULTICAST = 0x8000

    if {$flags & 0x8000} 	{append result "MULTICAST "}
    return $result
}

##############################################################################
#
# linkLevelAddressGet - get the link level address of an interface
#
# This routine gets the ethernet address of the interface.
# This routine is available only if the interface is not LOOPBACK or SLIP.
# Data is grabbed from the arpcom structure defined in /netinet/if_ether.h:
# .CS
#     typedef struct arpcom 
#         {
#         struct  ifnet ac_if;
#         u_char  ac_enaddr[6];
#         struct  in_addr ac_ipaddr;
#         struct  ether_multi *ac_multiaddrs;
#         int     ac_multicnt;
#         };
# .CE
#
# SYNOPSIS:
#   linkLevelAddressGet netIf
#
# PARAMETERS:
#   netIf : id of an attached network interface.
#
# RETURNS:
#   the ethernet address 
#
# ERRORS: N/A
#

proc linkLevelAddressGet {netIf} {

    # Get the ac_enaddr field from the arpcom structure

    return [wtxGopherEval "$netIf +148 @b@b@b@b@b@b"]
}

##############################################################################
#
# ifTypeLink - get the interface types from mib-2
#
# Get the interface types from mib-2
#
# SYNOPSIS:
#   ifTypeLink type
#
# PARAMETERS:
#   type: type of link
#
# RETURNS:
#   a messgage corresponding to the type link
#
# ERRORS: N/A
#

proc ifTypeLink {type} {
    
    set type [format "%d" $type]
    return [lindex {UNDEFINED                  \
		    OTHER REGULAR_1822         \
		    HDH1822 DDN_X25            \
		    RFC877_X25                 \
		    ETHERNET_CSMACD            \
		    ISO88023_CSMACD            \
		    ISO88024_TOKENBUS          \
		    ISO88025_TOKENRING         \
		    ISO88026_MAN               \
		    STARLAN                    \
		    PROTEON_10MBIT             \
		    PROTEON_80MBIT             \
		    HYPERCHANNEL               \
		    FDDI                       \
		    LAPB                       \
		    SDLC                       \
		    DS1                        \
		    E1                         \
		    BASIC_ISDN                 \
		    PRIMARY_ISDN               \
		    PROP_POINT_TO_POINT_SERIAL \
		    PPP                        \
		    SOFTWARE_LOOPBACK          \
		    EON                        \
		    ETHERNET_3MBIT             \
		    NSIP                       \
		    SLIP                       \
		    ULTRA                      \
		    DS3                        \
		    SIP                        \
		    FRAME_RELAY} $type]
}

###############################################################################
#
# interfacesMasksGet - get the list of netmasks and subnetmasks
#
# Initialize a list containing the netmask and the subnetmask for each
# active network interface. 
# Information is grabbed from fields ia_netmask, ia_subnetmask, ia_ifa.ifa_ifp
# of the in_ifaddr structure defined in /netinet/in_var.h:
# .CS
#     typedef struct in_ifaddr 
#         {
#         struct  ifaddr ia_ifa;
#         u_long  ia_net;
#         u_long  ia_netmask;
#         u_long  ia_subnet;
#         u_long  ia_subnetmask;
#         struct  in_addr ia_netbroadcast;
#         struct  in_ifaddr *ia_next;
#         struct  sockaddr_in ia_addr;
#         struct  sockaddr_in ia_dstaddr;
#         struct  sockaddr_in ia_sockmask;
#         };
# .CE
#
# SYNOPSIS: N/A
#
# PARAMETERS: N/A
#
# RETURNS:
#   a list of items:
#       interfaceId0 {ia_netmask ia_subnetmask}
#       interfaceId1 {ia_netmask ia_subnetmask} 
#       ...
#
# ERRORS:
#   Interfaces addresses list not found
#

proc  interfacesMasksGet {} {

    set addrList ""
    set in_ifaddr [symValueGet in_ifaddr "Interfaces addresses list not found"]

    # Get the three field for each items from the in_ifaddr structure

    set result [wtxGopherEval "$in_ifaddr  { +36 @ +4 @ -48  +12 <*!>  +40* }"]

    # Compose the list to be returned

    while {[llength $result] >=3} {

	# Write each field in the list

	set netMask [lindex $result 0]
	set subnetMask [lindex $result 1]
	set ifPointer [lindex $result 2]

	# Restart with a sub-list

	set result  [lrange $result 3 end]

	# Append each node in the returned list

	lappend addrList $ifPointer [list $netMask $subnetMask]
    }
    return $addrList
}
