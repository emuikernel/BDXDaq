# 01ifShow.tcl - Implementation of shell ifShow commands in Tcl
#
# Copyright 1998-2002 Wind River Systems, Inc.
#
# modification history
# --------------------
# 01c,07nov02,bpn  SPR#25798: fix the display of the (sub)netmask.
# 01b,02dec98,dbt  no longer use argsConvert() routine (obsolete).
# 01a,15jun98,f_l  written.
#
# DESCRIPTION
# This file holds the Tcl implementation of the windsh ifShow command
#
# RESOURCE FILES
# wpwr/host/resource/tcl/ifShowCore.tcl
#


# source the data extraction and formatting routines

source [wtxPath host resource tcl]ifShowCore.tcl


##############################################################################
#
# ifShow - display all the attached network interfaces
#
# This routine displays the attached network interfaces for debugging and
# diagnostic purposes.  If <ifName> is given, only the interfaces belonging
# to that group are displayed.  If <ifName> is omitted, all attached
# interfaces are displayed.
#
# For each interface selected, the following are shown: Internet
# address, point-to-point peer address (if using SLIP), broadcast address,
# netmask, subnet mask, Ethernet address, route metric, maximum transfer
# unit, number of packets sent and received on this interface, number of
# input and output errors, and flags (e.g., loopback, point-to-point,
# broadcast, promiscuous, arp, running, debug).
#
# EXAMPLE:
# The following call displays all interfaces whose names begin with "ln",
# (e.g., "ln0", "ln1", "ln2", etc.).:
#
# .CS
#     -> ifShow "ln"
# .CE
#
# The following call displays just the interface "ln0":
#
# .CS
#     -> ifShow "ln0"
# .CE
#
# SYNOPSIS: N/A
#
# PARAMETERS: N/A
#
# RETURNS: N/A
#
# SEE ALSO: routeShow(), ifLib
#
# ERRORS: N/A
#

shellproc ifShow {args} {

    set interfName [stringGet [lindex $args 0]]

    if {$interfName == 0} {
	set displayMode 0
    } else {
	set displayMode 1
    }
    set found 0

    # initialize a table containing the netmask and the subnetmask for each
    # active interface. index is the interface ID (netIf)

    array set interfacesMasksTbl [interfacesMasksGet]

    foreach netIf [netIfList] {
	set ifInfo [netIfInfo $netIf]

	bindNamesToList {if_name        \
			 if_unit        \
			 if_flags       \
			 ifi_type       \
			 ifi_mtu        \
			 ifi_metric     \
			 ifi_ipackets   \
			 ifi_ierrors    \
			 ifi_opackets   \
			 ifi_oerrors    \
			 ifi_collisions \
			 ifi_imcasts    \
			 ifi_omcasts    \
			 ifi_iqdrops } $ifInfo

	if {$displayMode && ![regexp $interfName $if_name]} {continue}

	set found 1

	puts stdout [format "%s (unit number %d):" $if_name $if_unit] 
	puts stdout [format "    Flags: (%#x) %s" $if_flags \
				[ifFlagsFormat $if_flags]]
	puts stdout [format "    Type: %s" [ifTypeLink $ifi_type]]

	set ifAddrList [netIfAddrList $netIf]

	foreach ifAddrSubList $ifAddrList {
	    set sa_family [lindex $ifAddrSubList 0]

	    # compare sa_family to the AF_INET = 0x2 constante,
	    # defined in sys/socket.h
	
	    if {$sa_family == 0x2} {	

	        set iAddr [lrange $ifAddrSubList 1 4]
	        set dAddr [lrange $ifAddrSubList 5 8]
		puts stdout "    Internet address: [ifAddrFormat $iAddr] "

        	# IFF_POINTOPOINT = 0x10
		# IFF_BROADCAST = 0x2

        	if { $if_flags & 0x10} {
		    puts stdout "    Destination Internet address:  \
 				[ifAddrFormat $dAddr]"
		} elseif {$if_flags & 0x2} {
		    puts stdout "    Broadcast address:  \
			[ifAddrFormat $dAddr]"
		}
		
		bindNamesToList {netMask subnetMask} $interfacesMasksTbl($netIf)

		puts stdout [format "    Netmask 0x%x Subnetmask 0x%x" \
				$netMask $subnetMask]
	    }
	}

	# if the interface is not LOOPBACK or SLIP, print the link
	# level address.
	# IFF_POINTOPOINT = 0x10
	# IFF_LOOPBACK = 0x8 defined in /net/if.h

        if { !($if_flags & 0x10) && !($if_flags & 0x8) } {
            puts stdout [format "    Ethernet address is %s" \
                        [ethernetAddrFormat [linkLevelAddressGet $netIf]] ]
        }

	puts stdout [format "    Metric is %d" $ifi_metric]
	puts stdout [format "    Maximum Transfer Unit size is %d" $ifi_mtu]
	puts stdout [format "    %d packets received; %d packets sent" \
			$ifi_ipackets $ifi_opackets]	
	puts stdout [format "    %ld multicast packets received" $ifi_imcasts]
	puts stdout [format "    %ld multicast packets sent" $ifi_omcasts]
	puts stdout [format "    %d input errors; %d output errors" \
				$ifi_ierrors $ifi_oerrors]
	puts stdout [format "    %d collisions; %ld dropped" \
			$ifi_collisions $ifi_iqdrops]
    }

    if { !$found } {
	if {$displayMode} {
	    error "$interfName network interface not found"
	}
	error "No network interface active."
    }
}
