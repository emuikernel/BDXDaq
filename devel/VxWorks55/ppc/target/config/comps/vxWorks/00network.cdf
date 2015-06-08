/*
Copyright 1984 - 2003 Wind River Systems, Inc.

modification history
--------------------
02q,23jan03,vvv  added configuration parameter NFS_MAXFILENAME (SPR #85166)
02p,13jan03,vvv  merged from branch wrn_velocecp, ver02w (SPR #82524, 75839,
		 83863, 82107, 80426, 81422)
02o,18jun02,vvv  added COUNT to SELECT_PROXY (SPR #78098)
02n,23may02,elr  ftpLibDebugLevelSet (FTP_DEBUG_LEVEL) is now 
                 ftpLibDebugOptionsSet (FTP_DEBUG_OPTIONS)
02m,20may02,vvv  fixed full simulator SELECT_NET_INIT problem (SPR #77374)
02l,16may02,vvv  restored SLIP support
02k,09may02,vvv  fixed problem with DEFER_NET_INIT (SPR #76147)
02j,28mar02,vvv  adding configuration parameters for NET_SHOW (SPR #69286/73759)
02i,22mar02,niq  added new authentication type parameter (SPR 73681) 
                 changed synopsis
02h,14mar02,elr  added ftp client component
02g,14mar02,mas  updated to make SM_NET_SHOW dependent upon SM_NET (SPR 73371)
02f,13mar02,vvv  removed SLIP, OSPF, HTTP; restored SNMPD (SPR #72992)
                 removed references to BOOTP (SPR #74183)
02e,07mar02,vvv  added defaults for DNS parameters (SPR #73222)
02d,23jan02,vvv  added configuration parameter ARP_MAX_ENTRIES
02c,07jan02,vvv  removed () from usrNetworkAddrCheck in usrNetworkInit
02b,20dec01,vvv  added INCLUDE_PROXY_DEFAULT_ADDR; added configlette field for 
		 NFS_SERVER
02a,12nov01,mas  made separately configurable components for SM_COMMON, SM_OBJ,
		 SM_NET
01z,06nov01,vvv  made NFS max. path length configurable (SPR #63551)
01y,09oct01,rae  merge from truestack ver 02b, base 01v (SPR #29741, M2, bootp)
01x,07feb01,spm  added merge record for 30jan01 update from version 01s of 
                 tor2_0_x branch (base 01q) and fixed modification history;
                 added general overhaul of telnet server (SPR #28675)
01w,30jan01,ijm  merged SPR# 28602 fixes: proxy ARP services are obsolete
01v,06dec00,spm  renamed usrConfigIf.c for project tool build fix
01u,16nov00,spm  fixed location of init routines for network device setup
01t,16nov00,spm  enabled new DHCP lease for runtime device setup (SPR #20438)
01s,17oct00,niq  Roll back Puli's changes. Changes will be installed in the
                 01network.cdf version
01r,30jul99,pul  adding NPT configurable parameters to T2 project facility
01q,21apr99,spm  added missing header file for ping application (SPR #26887)
01p,31mar99,ann  made INCLUDE_END_BOOT a default component
01o,24mar99,ann  changed the default values of NUM_SYS_CL_BLKS and NUM_SYS_MBLKS
                 to the sum of their individual components 
01n,09mar99,ann  added INCLUDE_DEFER_NET_INIT to defer net initialization
                 (SPR # 25354)
01m,09mar99,pr   moved ULIP components to arch/simnt/sim.cdf file (SPR #25140)
01l,03mar99,ead  Created the INCLUDE_SM_NET_SHOW component (SPR #25326)
01k,01mar99,ead  Deleted the MACRO_NEST rule from the INCLUDE_STREAMS_ALL
                 component (SPR #24583)
01j,01mar99,ann  Made END dependant on MUX (SPR# 25118)
01i,01mar99,ead  Created the INCLUDE_NET_SHOW component (SPR #23964)
01h,26feb99,nps  fix spr 25333, simple typo in NET_SETUP definition.
01g,25feb99,nps  fix spr 25215 by calling new NFS configlette.
01f,14sep98,ann  Rearranged smnet components and rearranged
		 the entire folder setup
01e,24aug98,ann  created configlette for NFS and cleaned up
                 it's module here
01d,18aug98,ms   removed some \n's that were causing problems
01c,18aug98,ann  merged ppp and snmp, removed certain dependencies
		 smnet still requires work
01b,12aug98,spm  reconfigured init groups 
01a,07may98,gnn	 written to start a BSD driver

DESCRIPTION

This file contains descriptions for all of the network components
that were not addressed in vxWorks.cdf.
*/

Folder	FOLDER_NETWORK {
	NAME		network components
	CHILDREN        FOLDER_CORE_COMPONENTS 	\
			FOLDER_NET_DEV		\
			FOLDER_NET_PROTOCOLS
        DEFAULTS        FOLDER_CORE_COMPONENTS	\
			FOLDER_NET_DEV          \
			FOLDER_NET_PROTOCOLS
}

Folder FOLDER_CORE_COMPONENTS {
	NAME		basic network initialization components
	CHILDREN	FOLDER_BOOTLINE_SETUP	\
			INCLUDE_MUX		\
			INCLUDE_NET_SETUP	\
			INCLUDE_NETWORK
	DEFAULTS	FOLDER_BOOTLINE_SETUP	\
			INCLUDE_NET_SETUP	\
			INCLUDE_NETWORK
}

Folder FOLDER_BOOTLINE_SETUP {
        NAME            bootline processing components
        CHILDREN        INCLUDE_NETMASK_GET     \
                        INCLUDE_NETDEV_NAMEGET  \
                        SELECT_NET_INIT         \
                        SELECT_NETADDR_INIT
        DEFAULTS        INCLUDE_NETMASK_GET     \
                        INCLUDE_NETDEV_NAMEGET  \
                        SELECT_NET_INIT         \
                        SELECT_NETADDR_INIT
}

Folder FOLDER_NET_PROTOCOLS {
	NAME		networking protocols
	CHILDREN	FOLDER_NET_FS		\
			FOLDER_NET_APP		\
			FOLDER_NET_API		\
			FOLDER_NET_ROUTE	\
			FOLDER_NET_DEBUG	\
			FOLDER_TCP_IP
	DEFAULTS	FOLDER_NET_API		\
			FOLDER_NET_FS		\
			FOLDER_TCP_IP
}

Folder FOLDER_NET_API {
	NAME		networking APIs
	CHILDREN	INCLUDE_RPC		\
			INCLUDE_ZBUF_SOCK
}

Folder FOLDER_NET_DEV {
	NAME		network devices
	SYNOPSIS	Installs support for network device types
	CHILDREN 	FOLDER_SMNET_COMPONENTS	\
			INCLUDE_BSD 		\
			INCLUDE_BSD_BOOT	\
			INCLUDE_END 		\
			INCLUDE_END_BOOT	\
			INCLUDE_PPP		\
			INCLUDE_PPP_BOOT	\
			INCLUDE_PPP_CRYPT 	\
			INCLUDE_SLIP 		\
			INCLUDE_SLIP_BOOT	\
			INCLUDE_NETDEV_CONFIG 	\
			FOLDER_BSD_NET_DRV
	DEFAULTS	FOLDER_BSD_NET_DRV	\
			INCLUDE_BSD
}

Folder FOLDER_NET_ROUTE {
	NAME		network routing protocols
	CHILDREN	INCLUDE_OSPF 		\
			INCLUDE_RIP 		\
			INCLUDE_ROUTE_SOCK
}

Folder FOLDER_NET_APP {
	NAME		network applications
	SYNOPSIS	high level services
	CHILDREN	INCLUDE_DNS_RESOLVER 	\
			INCLUDE_DHCPC 		\
			INCLUDE_DHCPR 		\
			INCLUDE_DHCPS 		\
			INCLUDE_SNTPC 		\
			INCLUDE_SNTPS 		\
			INCLUDE_PING		\
			INCLUDE_TELNET		\
			INCLUDE_RLOGIN 		\
			INCLUDE_SECURITY	\
			FOLDER_MIB2		\
                        SELECT_PROXY
}

Folder 	FOLDER_NET_FS {
	NAME		network filesystems
	CHILDREN	INCLUDE_FTP_SERVER	\
			INCLUDE_FTPD_SECURITY 	\
                        INCLUDE_FTP             \
			INCLUDE_NFS		\
			INCLUDE_NFS_MOUNT_ALL	\
			INCLUDE_NFS_SERVER	\
			FOLDER_NET_REM_IO	\
			INCLUDE_TFTP_CLIENT	\
			INCLUDE_TFTP_SERVER
        DEFAULTS        FOLDER_NET_REM_IO       \
                        INCLUDE_TFTP_CLIENT
}

Selection SELECT_PROXY {
	NAME 		proxy ARP services
	SYNOPSIS 	Protocols which merge physical networks
	COUNT           1-
	CHILDREN	INCLUDE_PROXY_CLIENT	\
			INCLUDE_PROXY_SERVER
}
 
Folder FOLDER_TCP_IP {
	NAME		core TCP/IP components
	SYNOPSIS	Select protocols to be included
	CHILDREN	FOLDER_TCPIP_CONFIG	\
			INCLUDE_IP 		\
			INCLUDE_TCP		\
			INCLUDE_TCP_DEBUG	\
			INCLUDE_UDP		\
			INCLUDE_IP_FILTER 	\
			INCLUDE_ICMP		\
			INCLUDE_IGMP		\
			INCLUDE_NET_LIB 	\
			INCLUDE_BSD_SOCKET	\
			BSD43_COMPATIBLE 	\
			INCLUDE_MCAST_ROUTING
        DEFAULTS        INCLUDE_IP              \
                        INCLUDE_TCP             \
                        INCLUDE_UDP             \
                        INCLUDE_NET_LIB         \
                        INCLUDE_BSD_SOCKET      \
                        INCLUDE_ICMP            \
                        INCLUDE_IGMP
}

Folder FOLDER_TCPIP_CONFIG {
	NAME		TCP/IP configuration components
	CHILDREN	INCLUDE_ARP_API		\
			INCLUDE_HOST_TBL
	DEFAULTS	INCLUDE_HOST_TBL
}

Folder FOLDER_SMNET_COMPONENTS	{
	NAME		shared memory network components
	SYNOPSIS	components to be chosen for shared memory network
	CHILDREN	INCLUDE_SM_NET_ADDRGET		\
			INCLUDE_SECOND_SMNET		\
			INCLUDE_SM_NET			\
			INCLUDE_SM_SEQ_ADDR             \
			INCLUDE_PROXY_DEFAULT_ADDR
	DEFAULTS	INCLUDE_SM_NET
			}

Component INCLUDE_NETWORK
        {
        NAME            basic network support
	SYNOPSIS	This should always be included for backward compatibility
	REQUIRES	INCLUDE_TCP INCLUDE_UDP INCLUDE_IP SELECT_NET_INIT
	INCLUDE_WHEN	INCLUDE_BSD_SOCKET
        }

Component INCLUDE_PPP
        {
        NAME            PPP
        SYNOPSIS        Point to Point Protocol
        MODULES         pppLib.o
	CONFIGLETTES	net/usrPPPCfg.c
        CFG_PARAMS      PPP_OPTIONS_FILE
	HDR_FILES	stdio.h
        }

Parameter PPP_OPTIONS_FILE
        {
        NAME            PPP options file
        TYPE            string
        DEFAULT         NULL
        }

Component INCLUDE_PPP_CRYPT
        {
        NAME            PPP CRYPT
        SYNOPSIS        Unbundled PPP cryptography support.
        INIT_RTN        cryptRtnInit (PPP_CRYPT_RTN);
        CFG_PARAMS      PPP_CRYPT_RTN
        MACRO_NEST      INCLUDE_PPP
        REQUIRES        INCLUDE_PPP
        }

Parameter PPP_CRYPT_RTN
        {
        NAME            PPP crypt routine
        TYPE            string
        DEFAULT         NULL
        }

Folder FOLDER_MIB2 {
	NAME		MIB2 components
	CHILDREN	INCLUDE_MIB2_ALL	\
			INCLUDE_MIB2_AT		\
			INCLUDE_MIB2_ICMP	\
			INCLUDE_MIB2_IF		\
			INCLUDE_MIB2_IP		\
			INCLUDE_MIB2_SYSTEM	\
			INCLUDE_MIB2_TCP	\
			INCLUDE_MIB2_UDP        \
			INCLUDE_SNMPD
}

Folder FOLDER_NET_DEBUG
	{
	NAME		network debugging
	SYNOPSIS	Network show routines
	CHILDREN	INCLUDE_DHCPC_SHOW \
			INCLUDE_UDP_SHOW \
			INCLUDE_TCP_SHOW \
			INCLUDE_ICMP_SHOW \
			INCLUDE_IGMP_SHOW \
			INCLUDE_NET_SHOW \
			INCLUDE_SM_NET_SHOW
	}

Component INCLUDE_MUX
	{
	NAME		network mux initialization
	SYNOPSIS	network driver to protocol multiplexer
	MODULES		muxLib.o
	INIT_RTN	muxLibInit ();
        REQUIRES 	INCLUDE_NET_SETUP
	}

Component INCLUDE_SM_NET_SHOW
	{
	NAME		shared memory network driver show routines
	SYNOPSIS	Initializes the shared memory network driver show facility
	INIT_RTN	smNetShowInit();
	MODULES		smNetShow.o
	REQUIRES	INCLUDE_SM_NET
	}

Component INCLUDE_DHCPC_SHOW
	{
	NAME		DHCP client show routines
	SYNOPSIS	Initializes the DHCP show facility
	INIT_RTN	dhcpcShowInit();
	MODULES		dhcpcShow.o
	}

Component INCLUDE_ICMP_SHOW
	{
	NAME		ICMP show routines
	SYNOPSIS	Initialize the ICMP show facility
	INIT_RTN	icmpShowInit();
	MODULES		icmpShow.o
	}

Component INCLUDE_IGMP_SHOW
	{
	NAME		IGMP show routines
	SYNOPSIS	Initialize the IGMP show facility
	INIT_RTN	igmpShowInit();
	MODULES		igmpShow.o
	}

Component INCLUDE_UDP_SHOW 
	{
	NAME		UDP show routines
	SYNOPSIS	Initialize the UDP show facility
	INIT_RTN	udpShowInit();
	MODULES		udpShow.o
	}

Component INCLUDE_TCP_SHOW
	{
	NAME		TCP show routines
	SYNOPSIS	Initialize the TCP show facility
	INIT_RTN	tcpShowInit();
	MODULES		tcpShow.o
	}

Component INCLUDE_NET_SHOW
	{
	NAME		Network show routines
	SYNOPSIS	Initialize the Network show facility
	CONFIGLETTES	net/usrNetShow.c
	INIT_RTN	netShowInit();
	CFG_PARAMS	RT_BUFFERED_DISPLAY RT_DISPLAY_MEMORY
	MODULES		netShow.o
	}

Parameter RT_BUFFERED_DISPLAY
	{
	NAME            Buffer routes before displaying
	SYNOPSIS        Indicates whether routes are buffered before they are \
			displayed. This should be set to TRUE when using \
			arpShow, routeShow or mRouteShow through a remote \
			session like telnet or through the host shell using \
			WDB_COMM_NETWORK.
	TYPE            BOOL
	DEFAULT         FALSE
	}

Parameter RT_DISPLAY_MEMORY
	{
	NAME            Route display memory
	SYNOPSIS        Memory allocated (bytes) for buffering routes before \
			display. This parameter is relevant when  \
			RT_BUFFERED_DISPLAY is set to TRUE.
	TYPE            uint
	DEFAULT         10000
	}

Component INCLUDE_DHCPC 
	{
        NAME 		DHCPv4 runtime client
        SYNOPSIS 	Dynamic host configuration protocol client
	MODULES		dhcpcLib.o 
	CONFIGLETTES	net/usrNetDhcpcCfg.c net/usrNetBoot.c
	INIT_RTN	usrDhcpcStart ();
	CFG_PARAMS	DHCPC_SPORT DHCPC_CPORT \
                        DHCPC_MAX_LEASES DHCPC_OFFER_TIMEOUT \
                        DHCPC_DEFAULT_LEASE DHCPC_MIN_LEASE
        REQUIRES 	INCLUDE_NET_SETUP INCLUDE_ROUTE_SOCK
	HDR_FILES	dhcp/dhcpcInit.h sysLib.h
	}

Parameter DHCPC_SPORT
	{
	NAME		DHCP Client Target Port
	SYNOPSIS	Port monitored by DHCP servers
        TYPE 		uint
        DEFAULT 	67
	}

Parameter DHCPC_CPORT
	{
	NAME		DHCP Client Host Port
	SYNOPSIS	Port monitored by DHCP clients
        TYPE 		uint
        DEFAULT 	68
	}

Parameter DHCPC_MAX_LEASES
	{
	NAME		DHCP Client Maximum Leases
	SYNOPSIS	Number of simultaneous leases supported
        TYPE 		uint
        DEFAULT 	4
	}

Parameter DHCPC_OFFER_TIMEOUT
	{
	NAME		DHCP Client Timeout Value
	SYNOPSIS	Interval for collecting server lease offers, in seconds
        TYPE 		uint
        DEFAULT 	5
	}

Parameter DHCPC_DEFAULT_LEASE
	{
	NAME		DHCP Client Default Lease
	SYNOPSIS	Desired lease length in seconds
        TYPE 		uint
        DEFAULT 	3600
	}

Parameter DHCPC_MIN_LEASE
	{
	NAME		DHCP Client Minimum Lease
	SYNOPSIS	Acceptable lease duration in seconds
        TYPE 		uint
        DEFAULT 	30
	}

Component INCLUDE_DHCPC_LEASE_TEST
	{
        NAME 		DHCPv4 Client Lease Verification/Assignment
        SYNOPSIS 	Renew IP address in boot parameters or retrieve one
	CONFIGLETTES	net/usrNetDhcpcBootSetup.c net/usrNetBoot.c
	INIT_RTN	usrDhcpcLeaseTest ();
        INCLUDE_WHEN 	INCLUDE_DHCPC
	REQUIRES	INCLUDE_DHCPC INCLUDE_NET_INIT INCLUDE_NETMASK_GET
	HDR_FILES	dhcp/dhcpcCommonLib.h
	}

Component INCLUDE_DHCPC_LEASE_SAVE
	{
	NAME 		DHCPv4 client timestamp storage
	SYNOPSIS 	Saves lease timestamp information in address field
	CONFIGLETTES	net/usrNetDhcpcBootSetup.c
	INIT_RTN	usrDhcpcLeaseSave ();
        INCLUDE_WHEN 	INCLUDE_DHCPC
	REQUIRES	INCLUDE_DHCPC INCLUDE_NET_INIT
	}

Component INCLUDE_DHCPR	
	{
	NAME		DHCP relay agent
	SYNOPSIS	Dynamic host configuration protocol relay agent
	MODULES		dhcprLib.o
	CONFIGLETTES	net/usrNetDhcprCfg.c
	INIT_RTN	usrDhcprStart ();
	CFG_PARAMS	DHCPS_SPORT DHCPS_CPORT \
                        DHCP_MAX_HOPS DHCPS_MAX_MSGSIZE
        REQUIRES 	INCLUDE_NET_SETUP
	HDR_FILES	ioLib.h
	}

Component INCLUDE_DHCPS	
	{
	NAME		DHCP server
	SYNOPSIS	Dynamic host configuration protocol server
	MODULES		dhcpsLib.o
	INIT_RTN	usrDhcpsStart ();
	CONFIGLETTES	net/usrNetDhcpsCfg.c
	CFG_PARAMS	DHCPS_SPORT DHCPS_CPORT \
                        DHCP_MAX_HOPS DHCPS_MAX_MSGSIZE
	EXCLUDES	INCLUDE_DHCPR
        REQUIRES 	INCLUDE_NET_SETUP
	HDR_FILES	ioLib.h
	}

Parameter DHCPS_LEASE_HOOK
	{
	NAME		DHCP Server Lease Storage Routine
	SYNOPSIS	Function pointer for recording active leases
	}

Parameter DHCPS_ADDRESS_HOOK
	{
	NAME		DHCP Server Address Storage Routine
	SYNOPSIS	Function pointer for preserving runtime pool entries
        DEFAULT 	NULL
	}

Parameter DHCPS_DEFAULT_LEASE
	{
	NAME		DHCP Server Standard Lease Length
	SYNOPSIS	Default lease duration in seconds
        TYPE 		uint
        DEFAULT 	3600
	}

Parameter DHCP_MAX_HOPS
	{
	NAME		DHCP Server/Relay Agent Network Radius
	SYNOPSIS	Maximum hop count before discarding message
        TYPE 		uint
        DEFAULT 	4
	}

Parameter DHCPS_SPORT
	{
	NAME		DHCP Server/Relay Agent Host Port
	SYNOPSIS	Port monitored by DHCP servers and relay agents
        TYPE 		uint
        DEFAULT 	67
	}

Parameter DHCPS_CPORT
	{
	NAME		DHCP Server/Relay Agent Target Port
	SYNOPSIS	Port monitored by DHCP clients
        TYPE 		uint
        DEFAULT 	68
	}

Parameter DHCPS_MAX_MSGSIZE 
	{
	NAME            DHCP Server/Relay Agent Maximum Message Size
	SYNOPSIS        Default allows minimum DHCP message in Ethernet frame
	DEFAULT         590
	TYPE            uint
	}

Component INCLUDE_DNS_RESOLVER  
	{
	NAME		DNS resolver
	SYNOPSIS	Domain name service client
	MODULES		res_comp.o res_query.o res_debug.o res_send.o \
			gethostnamadr.o res_mkquery.o resolvLib.o
	INIT_RTN	usrDnsInit (DNS_DEBUG);
	CFG_PARAMS	RESOLVER_DOMAIN_SERVER RESOLVER_DOMAIN DNS_DEBUG
	HDR_FILES	resolvLib.h
	CONFIGLETTES 	net/usrDnsCfg.c
        REQUIRES 	INCLUDE_NET_SETUP
	}

Parameter RESOLVER_DOMAIN_SERVER
	{
	NAME		Resolver Domain Server
	SYNOPSIS	The Domain server we contact with queries
	TYPE            string
	DEFAULT         "90.0.0.3"
	}

Parameter RESOLVER_DOMAIN
	{
	NAME		Resolver Domain
	SYNOPSIS	The Domain that we are in
	TYPE            string
	DEFAULT         "wrs.com"
	}

Parameter DNS_DEBUG
	{
	NAME		DNS Debug Messages Flag
	SYNOPSIS	Enables display of debug messages
        TYPE 		bool
        DEFAULT 	FALSE
        }
 
Component INCLUDE_IP_FILTER  
	{
	NAME		IPFilter
	SYNOPSIS	IP Filtering Library
	MODULES		ipFilterLib.o
	INIT_RTN	ipFilterLibInit ();
        REQUIRES 	INCLUDE_NET_SETUP
	}

Component INCLUDE_RIP
	{
	NAME		Routing Information Protocol
	SYNOPSIS	Automatic configuration of IP forwarding table
	MODULES		ripLib.o md5.o
	CONFIGLETTES    net/usrNetRipLib.c
	HDR_FILES       rip/ripLib.h
	INIT_RTN	usrRipLibInit ();
	CFG_PARAMS	RIP_SUPPLIER RIP_GATEWAY RIP_MULTICAST \
			RIP_VERSION RIP_TIMER_RATE RIP_SUPPLY_INTERVAL \
			RIP_EXPIRE_TIME RIP_GARBAGE_TIME RIP_AUTH_TYPE \
			RIP_IF_INIT RIP_ROUTING_SOCKET_BUF_SIZE
	REQUIRES	INCLUDE_ROUTE_SOCK INCLUDE_NET_SETUP
	EXCLUDES        BSD43_COMPATIBLE
	}

Parameter RIP_SUPPLIER
	{
	NAME		RIP Supplier Flag
	SYNOPSIS	Supply routes regardless of the number of interfaces?
        TYPE 		bool
	DEFAULT		FALSE
	}

Parameter RIP_GATEWAY
	{
	NAME		RIP Gateway Flag
	SYNOPSIS	Are we a gateway to the whole Internet?
        TYPE 		bool
	DEFAULT		FALSE
	}

Parameter RIP_VERSION
	{
	NAME		RIP Version Number
	SYNOPSIS	The version of RIP we are running
        TYPE 		uint
	DEFAULT		1
	}

Parameter RIP_MULTICAST
	{
	NAME		RIP Multicast Flag
	SYNOPSIS	Is multicast enabled for RIP communications
	DEFAULT		0
	}

Parameter RIP_TIMER_RATE
	{
	NAME		RIP Timer Rate
	SYNOPSIS	The rate at which our state is checked in seconds
	DEFAULT		1
	}

Parameter RIP_SUPPLY_INTERVAL
	{
	NAME		RIP Supply Interval
	SYNOPSIS	The time in seconds between sending updates
	DEFAULT		30
	}

Parameter RIP_EXPIRE_TIME
	{
	NAME		RIP Expire Time
	SYNOPSIS	The time in seconds until a route is expired
	DEFAULT		180
	}

Parameter RIP_GARBAGE_TIME
	{
	NAME		RIP Garbage Time
	SYNOPSIS	The time in seconds until a route is deleted
	DEFAULT		300
	}

Parameter RIP_AUTH_TYPE {
	NAME		RIP Authentication Type
	SYNOPSIS	Verification type from rip/m2RipLib.h (default is none)
	DEFAULT		1
	TYPE		uint
}

Parameter RIP_IF_INIT 
	{
	NAME		RIP Interface Flag
	SYNOPSIS	When set to TRUE, RIP will search the interfaces \
			when it initializes and when some interface-related \
			event occurs (interface status change, address \
			addition)
	DEFAULT		TRUE
	TYPE		BOOL
        }

Parameter RIP_ROUTING_SOCKET_BUF_SIZE 
	{
	NAME		RIP Routing Socket Buffer Size
	SYNOPSIS	This is the size of the routing socket receive buffer \
			in bytes
	DEFAULT		8192
	TYPE		uint
        }

Component INCLUDE_RLOGIN
	{
	NAME		RLOGIN server
	SYNOPSIS	Allows remote access to target
	MODULES		rlogLib.o
	INIT_RTN	rlogInit();
        REQUIRES 	INCLUDE_NET_SETUP
	}

Component INCLUDE_ROUTE_SOCK
	{
	NAME		BSD Routing Sockets
	SYNOPSIS	Allows manipulation of routing table with socket API
	REQUIRES	INCLUDE_BSD_SOCKET INCLUDE_NET_SETUP
	INIT_RTN	routeSockInit();
	CONFIGLETTES	net/routeSockCfg.c
	MODULES		routeSockLib.o
	HDR_FILES	sys/socket.h bsdSockLib.h
	}

Component INCLUDE_SNTPC 
	{
	NAME		SNTP client
	SYNOPSIS	Simple network time protocol client
	MODULES		sntpcLib.o
	INIT_RTN	sntpcInit (SNTP_PORT);
	CFG_PARAMS	SNTP_PORT
	HDR_FILES	sntpcLib.h
        REQUIRES 	INCLUDE_NET_SETUP
	}

Parameter SNTP_PORT
	{
	NAME		SNTP Client/Server Port
	SYNOPSIS	Port used for SNTP communication
        TYPE 		uint
	DEFAULT		123
	}

Component INCLUDE_SNTPS
	{
	NAME		SNTP server
	SYNOPSIS	Simple network time protocol server
	MODULES		sntpsLib.o
	INIT_RTN	usrSntpsInit ();
	CFG_PARAMS	SNTPS_MODE SNTPS_DSTADDR SNTPS_INTERVAL \
			SNTP_PORT SNTPS_TIME_HOOK
	HDR_FILES	sntpsLib.h
	CONFIGLETTES    net/usrSntpsCfg.c
        REQUIRES 	INCLUDE_NET_SETUP
	}

Parameter SNTPS_MODE
	{
	NAME		SNTP Server Mode Selection
	SYNOPSIS	Determines if server sends unsolicited update messages
	DEFAULT		SNTP_ACTIVE
	}

Parameter SNTPS_DSTADDR
	{
	NAME		SNTP Server Destination address
	SYNOPSIS	Recipient for active mode updates (NULL for broadcasts)
	DEFAULT		NULL
	}

Parameter SNTPS_INTERVAL
	{
	NAME		SNTP Server Update Interval
	SYNOPSIS	For active mode, update interval in seconds
	DEFAULT		64
	}

Parameter SNTPS_TIME_HOOK
	{
	NAME		SNTPS Time Hook
	SYNOPSIS	Name of required clock access routine
	DEFAULT         (FUNCPTR)NULL
	}

Component INCLUDE_PROXY_SERVER
	{
	NAME		Proxy ARP server
	SYNOPSIS	Merges separate physical networks
	MODULES		proxyArpLib.o
	INIT_RTN	usrProxyServerInit ();
        CFG_PARAMS      PROXYD_MAIN_ADDRESS PROXYD_PROXY_ADDRESS
	CONFIGLETTES    net/usrProxyServerCfg.c
        REQUIRES 	INCLUDE_NET_SETUP
	}

Parameter PROXYD_MAIN_ADDRESS
	{
	NAME 		Proxy ARP Server IP Address (Main Network)
	SYNOPSIS 	IP address of existing interface on main network
        TYPE 		string
	DEFAULT 	sysBootParams.ead
	}

Parameter PROXYD_PROXY_ADDRESS
	{
	NAME 		Proxy ARP Server IP Address (Proxy Network)
	SYNOPSIS 	IP address of existing interface on proxy network
        TYPE 		string
	DEFAULT 	sysBootParams.bad
	}

Component INCLUDE_SM_SEQ_ADDR
	{
	NAME		auto address setup
	SYNOPSIS	SM network auto address setup
	MACRO_NEST	INCLUDE_SM_NET
	REQUIRES	INCLUDE_SM_NET
	}

Component INCLUDE_PROXY_DEFAULT_ADDR
	{
	NAME            default address for backplane
	SYNOPSIS        Use ethernet address to generate default bp address
	REQUIRES        INCLUDE_SM_NET
	}

Component INCLUDE_PROXY_CLIENT
	{
	NAME		proxy ARP client
	SYNOPSIS	Routes traffic through existing proxy server
	MODULES		proxyLib.o
	INIT_RTN	usrProxyClientInit ();
	CFG_PARAMS 	PROXYC_DEVICE PROXYC_ADDRESS
	CONFIGLETTES    net/usrProxyClientCfg.c
        REQUIRES 	INCLUDE_NET_SETUP
	}

Parameter PROXYC_DEVICE
	{
	NAME 		Proxy ARP Client Proxy Device
	SYNOPSIS 	Name and unit number of active device on proxy network
        TYPE 		string
	DEFAULT 	NULL
	}

Parameter PROXYC_ADDRESS
	{
	NAME 		Proxy ARP Client IP Address
	SYNOPSIS 	IP address assigned to existing device on proxy network
	}


Component INCLUDE_SLIP
	{
	NAME		SLIP
	SYNOPSIS	Support for network devices using SLIP
	CONFIGLETTES	net/usrNetSlipCfg.c
	MODULES		if_sl.o
	CFG_PARAMS	SLIP_TTY SLIP_BAUDRATE CSLIP_ENABLE CSLIP_ALLOW \
			SLIP_MTU
	REQUIRES 	INCLUDE_NET_SETUP
	}

Parameter SLIP_TTY
	{
	NAME		SLIP Channel Identifier
	SYNOPSIS	Default TTY device for SLIP interface
	TYPE 		uint
	DEFAULT 	1
	}

Parameter SLIP_BAUDRATE
	{
	NAME		SLIP Channel Speed
	SYNOPSIS	Channel Rate, or 0 for previously selected value
	TYPE 		int
	DEFAULT 	0
	}

Parameter CSLIP_ENABLE
	{
	NAME		Transmit Header Compression Flag
	SYNOPSIS	Forces CSLIP header compression on transmit
	TYPE 		bool
	DEFAULT		TRUE	
	}

Parameter CSLIP_ALLOW
	{
	NAME 		Receive Header Compression Flag
	SYNOPSIS	Permits CSLIP header compression on receiver
	TYPE 		bool
	DEFAULT 	TRUE
	}

Parameter SLIP_MTU
	{
	NAME		SLIP Channel Capacity
        SYNOPSIS	Maximum message size for SLIP link
	TYPE            int
	DEFAULT 	576
	}

/* 
 * The configuration file for this component must precede the file for
 * the INCLUDE_BSD_BOOT component. Currently, that requirement is
 * met as long as the file names are in alphabetical order.
 */

Component INCLUDE_BSD
	{
	NAME		BSD interface support
	SYNOPSIS	Support for network devices using BSD interface.
	CONFIGLETTES	net/usrNetBsd.c
        REQUIRES 	INCLUDE_NET_SETUP INCLUDE_NETWORK
	HDR_FILES	sysLib.h
	}

Component INCLUDE_END
	{
	NAME		END interface support
	SYNOPSIS	Support for network devices using MUX/END interface
	CONFIGLETTES	net/usrEndLib.c
	HDR_FILES	configNet.h end.h muxLib.h netinet/if_ether.h \
			m2IfLib.h
	INIT_RTN	usrEndLibInit();
	CFG_PARAMS	END_POLL_STATS_ROUTINE_HOOK
        REQUIRES 	INCLUDE_NET_SETUP INCLUDE_NETWORK INCLUDE_MUX
	}

Parameter END_POLL_STATS_ROUTINE_HOOK {
	NAME		Polled stats update routine
	SYNOPSIS	Function pointer for reading END driver stats
	TYPE		FUNCPTR
	DEFAULT		m2PollStatsIfPoll
	}

Selection SELECT_NET_INIT
        {
        NAME            network initialization
        SYNOPSIS        decides when to include networking
        COUNT           1-1
        CHILDREN        INCLUDE_DEFER_NET_INIT         \
                        INCLUDE_NET_INIT
	DEFAULTS        INCLUDE_NET_INIT
        }

/*
 * The INCLUDE_DEFER_NET_INIT component is provided to allow
 * the user to attach interfaces at a later time. If this 
 * component is selected, exclude INCLUDE_BSD_BOOT
 * and INCLUDE_END_BOOT. To attach interfaces at a later time,
 * do the following.
 * Using END: Call usrNetEndDevStart (...) See usrNetEndBoot.c
 * Using BSD: Call usrNetBsdDevStart (...) see usrNetBsdBoot.c
 */

Component INCLUDE_DEFER_NET_INIT
        {
        NAME            defer network initialization
        SYNOPSIS        does not attach interfaces at boot time
        CONFIGLETTES    net/usrNetBsdBoot.c net/usrNetConfigIf.c \
                        net/usrNetBoot.c net/usrEndLib.c \
                        net/usrNetEndBoot.c net/usrNetBsd.c
        REQUIRES        INCLUDE_BOOT_LINE_INIT INCLUDE_NET_SETUP
        EXCLUDES        INCLUDE_NET_INIT
        }

/*
 * The configuration file for this component must precede the files for
 * all other components which depend on it. Currently, that requirement is
 * met as long as the file names are in alphabetical order.
 */

Component INCLUDE_NET_INIT
        {
        NAME            initialize network at boot time
        SYNOPSIS        Reads the enet address from the bootline parameters
        CONFIGLETTES    net/usrNetBoot.c
        INIT_RTN        usrNetBoot ();
        REQUIRES        INCLUDE_BOOT_LINE_INIT INCLUDE_NET_SETUP
        EXCLUDES        INCLUDE_DEFER_NET_INIT
        }

Component INCLUDE_NETDEV_CONFIG
	{
	NAME 		configure the network boot device
	SYNOPSIS 	Assigns an IP address and netmask
	CONFIGLETTES 	net/usrNetConfigIf.c
	INIT_RTN 	usrNetConfig (pDevName, uNum, pTgtName, pAddrString);
	INCLUDE_WHEN 	INCLUDE_NET_INIT
	}

Selection SELECT_NETADDR_INIT
        {
        NAME 		dhcp device address initialization
        SYNOPSIS 	Handles any lease timestamp information for address
        COUNT 		1-
        CHILDREN 	INCLUDE_DHCPC_LEASE_GET 	\
			INCLUDE_DHCPC_LEASE_CLEAN 	\
			INCLUDE_DHCPC_LEASE_TEST 	\
			INCLUDE_DHCPC_LEASE_SAVE
        }

Component INCLUDE_DHCPC_LEASE_GET
	{
	NAME		DHCP client timestamp setup
	SYNOPSIS	Extracts DHCP time values from address field
	CONFIGLETTES	net/usrNetDhcpcBootSetup.c net/usrNetBoot.c
	INIT_RTN	usrDhcpcLeaseGet ();
        INCLUDE_WHEN 	INCLUDE_DHCPC
	REQUIRES	INCLUDE_NET_INIT
        EXCLUDES 	INCLUDE_DHCPC_LEASE_CLEAN
	HDR_FILES	sysLib.h
	}

Component INCLUDE_DHCPC_LEASE_CLEAN
	{
	NAME		DHCP client timestamp removal
	SYNOPSIS	Deletes DHCP time values from address field
	CONFIGLETTES	net/usrNetBootUtil.c net/usrNetBoot.c
	INIT_RTN	usrDhcpcLeaseClean ();
        INCLUDE_WHEN 	INCLUDE_NET_INIT
	REQUIRES	INCLUDE_NET_INIT
	HDR_FILES	sysLib.h
	}

Component INCLUDE_PPP_BOOT
	{
	NAME 		PPP boot
	SYNOPSIS 	Creates a point to point attachment using boot parameters
	CONFIGLETTES 	net/usrNetBoot.c net/usrNetPPPBoot.c
	INIT_RTN	usrNetPPPStart ();
	REQUIRES 	INCLUDE_PPP INCLUDE_NET_INIT
	INCLUDE_WHEN 	INCLUDE_PPP INCLUDE_NET_INIT
	}

Component INCLUDE_SLIP_BOOT
	{
	NAME 		SLIP boot
	SYNOPSIS 	Creates a serial line IP attachment using boot parameters
	CONFIGLETTES 	net/usrNetBoot.c net/usrNetSlipBoot.c
	INIT_RTN	usrNetSlipStart ();
	REQUIRES 	INCLUDE_SLIP INCLUDE_NET_INIT
	INCLUDE_WHEN 	INCLUDE_SLIP INCLUDE_NET_INIT
	}

Component INCLUDE_NETMASK_GET
	{
	NAME 		network device netmask setup
	SYNOPSIS 	Extracts netmask value from address field
        CONFIGLETTES 	net/usrNetBootUtil.c net/usrNetBoot.c
	INIT_RTN 	usrNetmaskGet ();
	INCLUDE_WHEN 	INCLUDE_NET_INIT
	HDR_FILES	sysLib.h
	}

/*
 *      Even if "sm" is not used as the boot device, a backplane
 *      device is still created using a variety of scenarios, depending on 
 *      whether proxy arp and/or sequential addressing are selected.
 *
 *      If "sm" is used as the boot device and DHCP is included with
 *      that flag set, the network initialization must return ERROR unless
 *      the proxy flag is also set.
 *
 *      Whether or not the device was booted over the backplane, the network
 *      initialization must create a proxy server when that component is
 *      included and INCLUDE_SM_NET is defined. It must also initialize
 *      the shared memory network show routines if included.
 *
 */

Component INCLUDE_SM_NET
	{
	NAME 		shared memory network initialization
	SYNOPSIS 	Creates a device accessing backplane like a network
	CONFIGLETTES 	net/usrNetSmBoot.c net/usrNetBoot.c
	INIT_RTN	usrNetSmnetStart ();
	MODULES		if_sm.o
	REQUIRES	INCLUDE_BSD_BOOT \
			INCLUDE_SM_COMMON
	CFG_PARAMS	SM_PKTS_SIZE \
			SM_MAX_PKTS
	HDR_FILES	sysLib.h
	} 

Component INCLUDE_SM_NET_ADDRGET
	{
	NAME		shared memory bootline parsing
	SYNOPSIS	Selects "inet on backplane" field if needed
	CONFIGLETTES	net/usrNetBoot.c
	INIT_RTN	usrSmNetAddrGet ();
        INCLUDE_WHEN 	INCLUDE_SM_NET
	REQUIRES	INCLUDE_SM_NET
	MACRO_NEST	INCLUDE_SM_NET
	}

Component INCLUDE_SECOND_SMNET
	{
	NAME		shared memory as second interface
	SYNOPSIS	Attaches the shared memory as a second interface
	CONFIGLETTES	net/usrNetSmSecBoot.c net/usrNetBoot.c
	INIT_RTN	usrBpSecondAttach ();
	INCLUDE_WHEN	INCLUDE_SM_NET
	MACRO_NEST	INCLUDE_SM_NET
	REQUIRES	INCLUDE_SM_NET
	HDR_FILES	sysLib.h
	}

Parameter SM_MAX_PKTS {
	NAME		max # of shared memory packets to support
	TYPE		uint
	DEFAULT		200
}

Parameter SM_PKTS_SIZE {
	NAME		shared memory packet size, 0 = DEFAULT_PKTS_SIZE
	TYPE		uint
	DEFAULT		0
}


Component INCLUDE_NETDEV_NAMEGET
	{
	NAME		network device name selection
	SYNOPSIS	Gets name from "other" field if booting from disk
	CONFIGLETTES	net/usrNetBoot.c
	INIT_RTN	usrNetDevNameGet ();
	INCLUDE_WHEN	INCLUDE_NET_INIT
	}

Component INCLUDE_END_BOOT
        {
        NAME            END attach interface
        SYNOPSIS        Uses boot parameters to start an END driver
        CONFIGLETTES    net/usrNetBoot.c net/usrNetEndBoot.c
        HDR_FILES       ipProto.h muxLib.h sysLib.h
        INIT_RTN        usrNetEndDevStart (pDevName, uNum);
        INCLUDE_WHEN    INCLUDE_END INCLUDE_NET_INIT
        REQUIRES        INCLUDE_END INCLUDE_NET_INIT
        }

Component INCLUDE_BSD_BOOT
        {
        NAME            BSD attach interface
        SYNOPSIS        Uses boot parameters to start a BSD driver
        CONFIGLETTES    net/usrNetBoot.c net/usrNetBsdBoot.c
        INIT_RTN        usrNetBsdDevStart (pDevName, uNum, pAddrString, \
					   netDevBootFlag);
        INCLUDE_WHEN    INCLUDE_BSD INCLUDE_NET_INIT
        REQUIRES        INCLUDE_BSD INCLUDE_NET_INIT
        HDR_FILES       sysLib.h iosLib.h
        }

Component INCLUDE_ARP_API
	{
	NAME		API to ARP tables
	SYNOPSIS	Includes the ARP cache APIs
	REQUIRES	INCLUDE_IP
	MODULES		arpLib.o
	LINK_SYMS	arpAdd
	}

Component INCLUDE_IP
	{
	NAME		IPv4
	SYNOPSIS	BSD 4.4 IPv4
	INIT_RTN	usrIpLibInit();
	CONFIGLETTES 	net/usrNetIpLib.c
	CFG_PARAMS	IP_FLAGS_DFLT IP_TTL_DFLT IP_QLEN_DFLT IP_FRAG_TTL_DFLT
	MODULES		ipLib.o
        REQUIRES 	INCLUDE_NET_SETUP INCLUDE_BSD_SOCKET
	HDR_FILES	netLib.h
	}

Parameter IP_FLAGS_DFLT
	{
	NAME		IP Configuration Flags
	SYNOPSIS	Selects optional features of IP layer
        DEFAULT 	(IP_DO_FORWARDING | IP_DO_REDIRECT | \
                         IP_DO_CHECKSUM_SND | IP_DO_CHECKSUM_RCV)
	}

Parameter IP_TTL_DFLT
	{
	NAME		IP Time-to-live Value
	SYNOPSIS	Default TTL value for IP packets
        TYPE 		uint
        DEFAULT 	64
	}

Parameter IP_QLEN_DFLT
	{
	NAME		IP Packet Queue Size
	SYNOPSIS	Number of packets stored by receiver
        TYPE 		uint
        DEFAULT 	50
	}

Parameter IP_FRAG_TTL_DFLT
	{
	NAME		IP Time-to-live Value for packet fragments
	SYNOPSIS	Number of slow timeouts (2 per second)
        TYPE 		uint
        DEFAULT 	60
	}

Component INCLUDE_ICMP
	{
	NAME		ICMPv4
	SYNOPSIS	BSD 4.4 ICMPv4
	CONFIGLETTES	net/usrNetIcmp.c
	INIT_RTN	icmpLibInit (&icmpCfgParams);
	CFG_PARAMS	ICMP_FLAGS_DFLT
	MODULES		icmpLib.o
        REQUIRES 	INCLUDE_NET_SETUP INCLUDE_BSD_SOCKET
	HDR_FILES	netLib.h
	}

Parameter ICMP_FLAGS_DFLT
	{
	NAME		ICMP Configuration Flags
	SYNOPSIS	Selects optional features of ICMP 
        DEFAULT 	(ICMP_NO_MASK_REPLY)
	}

Component INCLUDE_MCAST_ROUTING
	{
	NAME		MCAST ROUTING
	SYNOPSIS	Multicast Routing Support
	REQUIRES	INCLUDE_BSD_SOCKET
	INIT_RTN	mCastRouteLibInit();
	MODULES		mCastRouteLib.o
	}

Component INCLUDE_IGMP
	{
	NAME		IGMPv4
	SYNOPSIS	BSD 4.4 IGMPv4
	REQUIRES	INCLUDE_BSD_SOCKET
	INIT_RTN	igmpLibInit();
	MODULES		igmpLib.o
	}

Component INCLUDE_UDP
	{
	NAME		UDPv4
	SYNOPSIS	BSD 4.4. UDPv4
	REQUIRES	INCLUDE_BSD_SOCKET
	CONFIGLETTES	net/usrUdp.c
	INIT_RTN	udpLibInit (&udpCfgParams);
	CFG_PARAMS	UDP_FLAGS_DFLT UDP_SND_SIZE_DFLT UDP_RCV_SIZE_DFLT
	MODULES		udpLib.o
	HDR_FILES	netLib.h
	}

Parameter UDP_FLAGS_DFLT
	{
	NAME 		UDP Configuration Flags
	SYNOPSIS	Optional UDP features: default enables checksums
        DEFAULT 	(UDP_DO_CKSUM_SND | UDP_DO_CKSUM_RCV)
	}

Parameter UDP_SND_SIZE_DFLT
	{
	NAME		UDP Send Buffer Size
	SYNOPSIS	Number of bytes for outgoing UDP data (9216 by default)
        TYPE 		uint
        DEFAULT 	9216
	}

Parameter UDP_RCV_SIZE_DFLT
	{
	NAME		UDP Receive Buffer Size
	SYNOPSIS	Number of bytes for incoming UDP data (default 41600)
        TYPE 		uint
        DEFAULT 	41600
	}

Component INCLUDE_TCP
	{
	NAME		TCPv4
	SYNOPSIS	BSD 4.4 TCPv4
	REQUIRES	INCLUDE_BSD_SOCKET
	CONFIGLETTES	net/usrTcp.c
	CFG_PARAMS	TCP_FLAGS_DFLT TCP_SND_SIZE_DFLT TCP_RCV_SIZE_DFLT \
		 	TCP_CON_TIMEO_DFLT TCP_REXMT_THLD_DFLT \
			TCP_MSS_DFLT TCP_RND_TRIP_DFLT TCP_IDLE_TIMEO_DFLT \
			TCP_MAX_PROBE_DFLT TCP_RAND_FUNC TCP_MSL_CFG
	INIT_RTN	tcpLibInit (&tcpCfgParams);
	MODULES		tcpLib.o
	HDR_FILES	netLib.h netinet/ppp/random.h
	}

Parameter TCP_FLAGS_DFLT
	{
	NAME		TCP Default Flags
	SYNOPSIS	Default value of the TCP flags
	DEFAULT		(TCP_DO_RFC1323)
	}

Parameter TCP_SND_SIZE_DFLT
	{
	NAME		TCP Send Buffer Size
	SYNOPSIS	Number of bytes for outgoing TCP data (8192 by default)
        TYPE 		uint
        DEFAULT 	8192
	}

Parameter TCP_RCV_SIZE_DFLT
	{
	NAME		TCP Receive Buffer Size
	SYNOPSIS	Number of bytes for incoming TCP data (8192 by default)
        TYPE 		uint
        DEFAULT 	8192
	}

Parameter TCP_CON_TIMEO_DFLT
	{
	NAME		TCP Connection Timeout
	SYNOPSIS	Timeout intervals to connect (default 150 = 75 secs)
        TYPE 		uint
        DEFAULT 	150
	}

Parameter TCP_REXMT_THLD_DFLT
	{
	NAME		TCP Retransmission Threshold
	SYNOPSIS	Number of retransmit attempts before error (default 3)
        TYPE 		uint
        DEFAULT 	3
	}

Parameter TCP_MSS_DFLT
	{
	NAME		Default TCP Maximum Segment Size
	SYNOPSIS	Initial number of bytes for a segment (default 512)
        TYPE 		uint
        DEFAULT 	512
	}

Parameter TCP_RND_TRIP_DFLT
	{
	NAME		Default Round Trip Interval
	SYNOPSIS	Initial value for round-trip-time, in seconds
        TYPE 		uint
        DEFAULT 	3
	}

Parameter TCP_IDLE_TIMEO_DFLT
	{
	NAME		TCP Idle Timeout Value
	SYNOPSIS	Seconds without data before dropping connection
        TYPE 		uint
        DEFAULT 	14400
	}

Parameter TCP_MAX_PROBE_DFLT
	{
	NAME		TCP Probe Limit
	SYNOPSIS	Number of probes before dropping connection (default 8)
        TYPE 		uint
        DEFAULT 	8
	}

Parameter TCP_RAND_FUNC
	{
	NAME		TCP Random Function
	SYNOPSIS	A random fucntion to use in tcp_init
        TYPE 		FUNCPTR
        DEFAULT 	(FUNCPTR)random
	}

Parameter TCP_MSL_CFG
        {
        NAME            TCP Maximum Segment Lifetime
        SYNOPSIS        TCP Maximum Segment Lifetime in seconds
        TYPE            uint
        DEFAULT         30
        }

Component INCLUDE_TCP_DEBUG
	{
	NAME		TCPv4 DEBUG
	SYNOPSIS	BSD 4.4 TCPv4 Debugging
	INIT_RTN	tcpTraceInit ();
	MODULES		tcp_debug.o
	}

Component INCLUDE_BSD_SOCKET
	{
	NAME		BSD SOCKET
	SYNOPSIS	BSD Socket Support
	CFG_PARAMS	NUM_FILES
	CONFIGLETTES	net/usrBsdSocket.c
	INIT_RTN	usrBsdSockLibInit();
	MODULES		bsdSockLib.o sockLib.o
        REQUIRES 	INCLUDE_NET_SETUP
	HDR_FILES	sys/socket.h bsdSockLib.h
	}

Component BSD43_COMPATIBLE
	{
	NAME		BSD 4.3 Compatible Sockets
	SYNOPSIS	BSD 4.3 Compatible Socket Library
	INIT_RTN	bsdSock43ApiFlag = TRUE;
	HDR_FILES	bsdSockLib.h
	}	

Component INCLUDE_NET_LIB
	{
	NAME		network library support
	SYNOPSIS	creates the network task that runs low-level \
			network interface routines in a task context
	INIT_RTN	netLibInit();
	MODULES		netLib.o
	HDR_FILES	netLib.h
        REQUIRES 	INCLUDE_NET_SETUP
	}

Component INCLUDE_HOST_TBL
	{
	NAME		HOST TBL
	SYNOPSIS	Host Table Support
	INIT_RTN	hostTblInit();
	MODULES		hostLib.o
	HDR_FILES	hostLib.h
        REQUIRES 	INCLUDE_NET_SETUP
	}

Folder FOLDER_NET_REM_IO
	{
	NAME		Network Remote I/O Support
	CHILDREN        INCLUDE_NET_HOST_SETUP \
			INCLUDE_NET_REM_IO
	DEFAULTS        INCLUDE_NET_HOST_SETUP
        }

/* 
 * This component currently uniquely uses the sethostname() routine in
 * hostLib.o, but should not depend on that library, so MODULES is
 * empty. It only references INCLUDE_NET_INIT for backward compatibility.
 * There are no dependencies in the processing which require execution
 * during the boot parameter processing.
 */

Component INCLUDE_NET_HOST_SETUP
        {
	NAME		network host setup
	SYNOPSIS	Route creation and hostname setup
	CONFIGLETTES	net/usrNetRemoteCfg.c
	INIT_RTN	usrNetHostSetup ();
        INCLUDE_WHEN 	INCLUDE_NET_INIT
        REQUIRES 	INCLUDE_BOOT_LINE_INIT INCLUDE_NET_SETUP
	HDR_FILES	ioLib.h private/funcBindP.h netDrv.h \
			private/ftpLibP.h remLib.h
	}

/* 
 * This component currently uniquely uses the sethostname() routine in
 * hostLib.o, but should not depend on that library, so MODULES is
 * empty. It only requires INCLUDE_NET_INIT for backward compatibility.
 * There are no dependencies in the processing which require execution
 * during the boot parameter processing.
 */

Component INCLUDE_NET_REM_IO
        {
	NAME		network remote I/O access
	SYNOPSIS	Allows access to file system on boot host
	CONFIGLETTES	net/usrNetRemoteCfg.c
	INIT_RTN	usrNetRemoteCreate ();
        MODULES 	netDrv.o
        INCLUDE_WHEN 	INCLUDE_NET_INIT
        REQUIRES 	INCLUDE_BOOT_LINE_INIT INCLUDE_NET_HOST_SETUP
	HDR_FILES	ioLib.h private/funcBindP.h netDrv.h \
			private/ftpLibP.h remLib.h
	}

Component INCLUDE_FTPD_SECURITY
	{
	NAME		FTP server security
	SYNOPSIS	File transfer protocol server library security
	INIT_RTN	{extern FUNCPTR loginVerifyRtn; loginVerifyRtn = loginUserVerify;}
	REQUIRES	INCLUDE_SECURITY INCLUDE_FTP_SERVER
	HDR_FILES	loginLib.h
	}
		
Component INCLUDE_FTP_SERVER
	{
	NAME		FTP server
	SYNOPSIS	File Transfer Protocol Server Library and Task
	MODULES		ftpdLib.o
	REQUIRES	INCLUDE_BSD_SOCKET
	INIT_RTN	ftpdInit ((FUNCPTR)NULL, 0);
	}

Component INCLUDE_FTP 
        {
        NAME            FTP client
        SYNOPSIS        File Transfer Protocol (FTP) library
        HDR_FILES       ftpLib.h private/ftpLibP.h
        CONFIGLETTES    net/usrFtp.c
        MODULES         ftpLib.o
        LINK_SYMS       ftpCommand
        INIT_RTN        usrFtpInit();
        CFG_PARAMS      FTP_TRANSIENT_MAX_RETRY_COUNT \
                        FTP_TRANSIENT_RETRY_INTERVAL  \
                        FTP_TRANSIENT_FATAL           \
                        FTP_DEBUG_OPTIONS
        }

Parameter FTP_DEBUG_OPTIONS
        {
        NAME        Debug logging facilities in ftpLib
        SYNOPSIS    Enable various debugging facilities within ftpLib
        TYPE        int
        DEFAULT     0
        }

Parameter FTP_TRANSIENT_MAX_RETRY_COUNT
        {
        NAME        FTP Transient response maximum retry limit
        SYNOPSIS    Maximum number of retries when FTP_TRANSIENT response encountered
        TYPE        int
        DEFAULT     100
        }

Parameter FTP_TRANSIENT_RETRY_INTERVAL 
        {
        NAME        Time delay between retries after FTP_TRANSIENT encountered
        SYNOPSIS    The time interval (in clock ticks) between reissuing a command
        TYPE        int 
        DEFAULT     0
        }

Parameter FTP_TRANSIENT_FATAL 
        {
        NAME        FTP transient fatal function
        SYNOPSIS    Should a transient response be retried or aborted
        DEFAULT     ftpTransientFatal
        }

Component INCLUDE_FTP_SERVER
	{
	NAME		FTP server
	SYNOPSIS	File Transfer Protocol Server Library and Task
	MODULES		ftpdLib.o
	REQUIRES	INCLUDE_BSD_SOCKET
	INIT_RTN	ftpdInit ((FUNCPTR)NULL, 0);
	}

Component INCLUDE_MIB2_ALL
        {
        NAME            MIB2 ALL
        SYNOPSIS        All groups supported
	HDR_FILES	m2Lib.h
	MODULES         m2Lib.o
        }

Component INCLUDE_MIB2_SYSTEM
	{
	NAME		MIB2 SYSTEM
	SYNOPSIS	MIB2 variables for the system group
	HDR_FILES	m2Lib.h
	INIT_RTN        m2SysInit (MIB2SYS_DESCR,\
		                   MIB2SYS_CONTACT,\
				   MIB2SYS_LOCATION,\
				   (M2_OBJECTID *)MIB2SYS_OBJECT_ID);
	CFG_PARAMS      MIB2SYS_DESCR \
		        MIB2SYS_CONTACT \
			MIB2SYS_LOCATION \
			MIB2SYS_OBJECT_ID
	MODULES         m2SysLib.o
	}
	
Parameter MIB2SYS_DESCR {
	NAME            Pointer to MIB-2 sysDescr
	SYNOPSIS        Pointer to MIB-2 sysDescr
	TYPE            string
	DEFAULT         NULL
	}

Parameter MIB2SYS_CONTACT {
	NAME            Pointer to MIB-2 sysContact
        SYNOPSIS        Pointer to MIB-2 sysContact
	TYPE            string
	DEFAULT         NULL
	}

Parameter MIB2SYS_LOCATION {
	NAME            Pointer to MIB-2 sysLocation
	SYNOPSIS        Pointer to MIB-2 sysLocation
        TYPE            string
	DEFAULT         NULL
	}

Parameter MIB2SYS_OBJECT_ID {
	NAME            Pointer to MIB-2 ObjectId
	SYNOPSIS        Pointer to MIB-2 ObjectId
	TYPE            int
	DEFAULT         NULL
	}

Component INCLUDE_MIB2_IF
	{
	NAME		MIB2 IF
	SYNOPSIS	MIB2 Variables for Interface Layer
	HDR_FILES	m2Lib.h
        INIT_RTN        m2IfInit (MIB2IF_TRAP_RTN, (void *)MIB2IF_TRAP_ARG);
	MODULES         m2IfLib.o
	CFG_PARAMS      MIB2IF_TRAP_RTN \
			MIB2IF_TRAP_ARG
	}

Parameter MIB2IF_TRAP_RTN {
	NAME            Pointer to user trap generator
	SYNOPSIS        Pointer to a user-supplied SNMP trap generator
	TYPE            FUNCPTR
	DEFAULT         NULL
	}

Parameter MIB2IF_TRAP_ARG {
	NAME            Pointer to user trap generator argument
	SYNOPSIS        Pointer to a user-supplied SNMP trap generators argument
	TYPE            int
	DEFAULT         NULL
	}

Component INCLUDE_MIB2_IP
	{
	NAME		MIB2 IP
	SYNOPSIS	MIB2 Variables for IP
	HDR_FILES	m2Lib.h
	INIT_RTN        m2IpInit (MIB2IP_MAX_ROUTE_TBL_SIZE);
	CFG_PARAMS      MIB2IP_MAX_ROUTE_TBL_SIZE
	MODULES         m2IpLib.o
	}

Parameter MIB2IP_MAX_ROUTE_TBL_SIZE {
	NAME            Max size of routing table
	SYNOPSIS        Default size of the MIB-II route table cache
	TYPE            int
	DEFAULT         0
	}

Component INCLUDE_MIB2_AT
	{
	NAME		MIB2 AT
	SYNOPSIS	MIB2 Variables for Address Translation (Obsolete)
	HDR_FILES	m2Lib.h
	}

Component INCLUDE_MIB2_ICMP
	{
	NAME		MIB2 ICMP
	SYNOPSIS	MIB2 Variables for ICMP
	HDR_FILES	m2Lib.h
        INIT_RTN        m2IcmpInit ();
	MODULES         m2IcmpLib.o
	}

Component INCLUDE_MIB2_TCP
	{
	NAME		MIB2 TCP
	SYNOPSIS	MIB2 Variables for TCP
	HDR_FILES	m2Lib.h
        INIT_RTN        m2TcpInit ();
	MODULES         m2TcpLib.o
	}

Component INCLUDE_MIB2_UDP
	{
	NAME		MIB2 UDP
	SYNOPSIS	MIB2 Variables for UDP
	HDR_FILES	m2Lib.h
        INIT_RTN        m2UdpInit ();
	MODULES         m2UdpLib.o
	} 

Component INCLUDE_NFS
	{
	NAME		NFS client
	SYNOPSIS	Network file system (version 2) client
	CONFIGLETTES    net/usrNfs.c
	REQUIRES	INCLUDE_RPC INCLUDE_BSD_SOCKET
	INIT_RTN	usrNfsInit();
	CFG_PARAMS	NFS_USER_ID NFS_GROUP_ID NFS_MAXPATH
	MODULES		nfsDrv.o
	}

Parameter NFS_USER_ID
	{
	NAME		user identifier for NFS access
        TYPE 		int
        DEFAULT 	2001
	}

Parameter NFS_GROUP_ID
	{
	NAME		group identifier for NFS access
        TYPE 		int
        DEFAULT 	100
	}
 
Component INCLUDE_NFS_MOUNT_ALL
	{
	NAME		NFS mount all
	SYNOPSIS	Mount all remote file systems at boot time
	CONFIGLETTES	net/usrNetNfsMount.c
	INIT_RTN	usrNetNfsMountAll();
	REQUIRES	INCLUDE_NFS
	}

Component INCLUDE_NFS_SERVER
	{
	NAME		NFS server
	SYNOPSIS	Network file system (version 2) server
	CONFIGLETTES    net/usrNfsd.c
	INIT_RTN	usrNfsdInit ();
	REQUIRES	INCLUDE_RPC INCLUDE_BSD_SOCKET
	CFG_PARAMS	NFS_USER_ID NFS_GROUP_ID NFS_MAXPATH NFS_MAXFILENAME
	HDR_FILES	nfsdLib.h
	MODULES		nfsdLib.o
	}

Parameter NFS_MAXPATH
        {
        NAME            Maximum file path length 
        SYNOPSIS        Indicates maximum length of a file path 
        DEFAULT         255
        }

Parameter NFS_MAXFILENAME
        {
        NAME            Maximum file name length 
        SYNOPSIS        Indicates maximum length of a filename. If a filename \
			is longer than this value then it is truncated before \
			returning it to the client. This parameter should be \
			set carefully since it affects the amount of memory \
			allocated by the server. Valid values range from 1 to \
			99.
        DEFAULT         40
        }

Component INCLUDE_PING
	{
	NAME		PING client
	SYNOPSIS	ICMP (ping) client
	HDR_FILES	pingLib.h
	MODULES		pingLib.o
	REQUIRES	INCLUDE_BSD_SOCKET
	INIT_RTN	pingLibInit();
	}
	
Component INCLUDE_RPC
	{
	NAME		RPC
	SYNOPSIS	Remote Procedure Call system
	REQUIRES	INCLUDE_BSD_SOCKET
	INIT_RTN	rpcInit ();
	MODULES		rpcLib.o
	}

Component INCLUDE_SNMPD
	{
	NAME		SNMP daemon
	SYNOPSIS	Simple network management protocol daemon
	MODULES		snmpdLib.o
	REQUIRES	INCLUDE_BSD_SOCKET
	INIT_RTN	usrSnmpdInit ();
	CFG_PARAMS	INCLUDE_SNMPD_DEBUG INCLUDE_TRACE_LEVEL
	HDR_FILES	snmp/snmpdInit.h
	CONFIGLETTES	net/usrSnmpCfg.c
	}

Parameter INCLUDE_SNMPD_DEBUG
        {
        NAME            SNMPD debug
        SYNOPSIS        Agent level debugging
        DEFAULT         0
        }

Parameter INCLUDE_TRACE_LEVEL
        {
        NAME            SNMPD trace level
        SYNOPSIS        Should be >= 0 or <= 3 with greater values \
                        showing more information and 0 showing none 
        DEFAULT         0
        }

Component INCLUDE_TELNET
	{
	NAME		Telnet server
	SYNOPSIS	Supports remote login using telnet protocol
	MODULES		telnetdLib.o
	CONFIGLETTES	net/usrNetTelnetdCfg.c
	INIT_RTN	usrTelnetdStart ();
	CFG_PARAMS	TELNETD_MAX_CLIENTS TELNETD_TASKFLAG TELNETD_PORT \
			TELNETD_PARSER_HOOK
	REQUIRES	INCLUDE_BSD_SOCKET
	}

Parameter TELNETD_MAX_CLIENTS
	{
	NAME		Telnet server client limit
	SYNOPSIS	Maximum number of simultaneous client sessions
	DEFAULT		1
	}

Parameter TELNETD_TASKFLAG
	{
	NAME		Telnet server resource allocation method
	SYNOPSIS	Allows server to create tasks before any connections
        TYPE 		bool
	DEFAULT		FALSE
	}

Parameter TELNETD_PORT
	{
	NAME		Telnet Server Port
	SYNOPSIS	Port monitored by telnet server
	TYPE 		uint
	DEFAULT		23
	}

Parameter TELNETD_PARSER_HOOK
	{
	NAME		Telnet server access to command interpreter
	SYNOPSIS	Connects clients to parser (default is target shell)
	DEFAULT		shellParserControl
	}

Component INCLUDE_SECURITY {
        NAME            RLOGIN/TELNET password protection
        SYNOPSIS        rlogin and telnet password protection
        MODULES         loginLib.o
        CONFIGLETTES    usrSecurity.c
        INIT_RTN        usrSecurity ();
        CFG_PARAMS      LOGIN_USER_NAME LOGIN_PASSWORD
        HDR_FILES       shellLib.h loginLib.h sysLib.h
}

Component INCLUDE_TFTP_CLIENT
	{
	NAME		TFTP client
	SYNOPSIS	Trivial file transfer protocol client
	REQUIRES	INCLUDE_BSD_SOCKET
	MODULES		tftpLib.o
	LINK_SYMS	tftpCopy
	}

Component INCLUDE_TFTP_SERVER
	{
	NAME		TFTP server
	SYNOPSIS	Trivial file transfer protocol server
	MODULES		tftpdLib.o
	REQUIRES	INCLUDE_BSD_SOCKET
	INIT_RTN	tftpdInit (0, 0, 0, FALSE, 0);
	}

Component INCLUDE_ZBUF_SOCK
	{
	NAME		ZBUF socket
	SYNOPSIS	Zero copy buffer based socket interface
	MODULES		zbufLib.o zbufSockLib.o
	INIT_RTN	zbufSockLibInit ();
	}

Component INCLUDE_NET_SETUP
	{
	NAME		network buffer initialization
	SYNOPSIS	network buffer creation and device support
	CFG_PARAMS	NUM_NET_MBLKS NUM_CL_BLKS \
                        NUM_64 NUM_128 NUM_256 NUM_512 NUM_1024 NUM_2048 \
                        NUM_SYS_MBLKS NUM_SYS_CL_BLKS \
                        NUM_SYS_64 NUM_SYS_128 NUM_SYS_256 NUM_SYS_512 \
			IP_MAX_UNITS ARP_MAX_ENTRIES
	CONFIGLETTES	net/usrNetLib.c
	INIT_RTN        usrNetLibInit ();
	HDR_FILES	netBufLib.h ipProto.h
	}


Parameter NUM_NET_MBLKS
	{
	NAME		Network memory blocks for user data
        SYNOPSIS 	Private access to data in clusters
        TYPE 		uint
        DEFAULT 	400
	}

Parameter NUM_64
	{
	NAME		Number of 64 byte clusters for user data
        TYPE 		uint
        DEFAULT 	100
	}

Parameter NUM_128
	{
	NAME		Number of 128 byte clusters for user data
        TYPE 		uint
        DEFAULT 	100
	}

Parameter NUM_256
	{
	NAME		Number of 256 byte clusters for user data
        TYPE 		uint
        DEFAULT 	40
	}

Parameter NUM_512
	{
	NAME		Number of 512 byte clusters for user data
        TYPE 		uint
        DEFAULT 	40
	}

Parameter NUM_1024
	{
	NAME		Number of 1024 byte clusters for user data
        TYPE 		uint
        DEFAULT 	25
	}

Parameter NUM_2048
	{
	NAME		Number of 2048 byte clusters for user data
        TYPE 		uint
        DEFAULT 	25
	}

Parameter NUM_CL_BLKS
	{
	NAME		Size of network memory pool for user data
        SYNOPSIS 	Total of all cluster sizes for shared user data
        TYPE 		uint
        DEFAULT 	NUM_64 + NUM_128 + NUM_256 + \
                        NUM_512 + NUM_1024 + NUM_2048
	}

Parameter NUM_SYS_CL_BLKS
	{
	NAME		Size of network memory pool for system data
        SYNOPSIS 	Total of all cluster sizes for shared system data
        TYPE 		uint
        DEFAULT 	256
	}

Parameter NUM_SYS_MBLKS
	{
	NAME		Network memory blocks for system data
        SYNOPSIS 	Private access to system data in clusters
        TYPE 		uint
        DEFAULT 	2 * NUM_SYS_CL_BLKS
	}


Parameter NUM_SYS_64
	{
	NAME		Number of 64 byte clusters for system data
        TYPE 		uint
        DEFAULT 	64
	}

Parameter NUM_SYS_128
	{
	NAME		Number of 128 byte clusters for system data
        TYPE 		uint
        DEFAULT 	64
	}

Parameter NUM_SYS_256
	{
	NAME		Number of 256 byte clusters for system data
        TYPE 		uint
        DEFAULT 	64
	}

Parameter NUM_SYS_512
	{
	NAME		Number of 512 byte clusters for system data
        TYPE 		uint
        DEFAULT 	64
	} 


Parameter IP_MAX_UNITS
	{
	NAME		IP Driver Control Limit
	SYNOPSIS	Maximum number of interfaces attached to IP layer
        TYPE 		uint
        DEFAULT 	1
	}

Parameter ARP_MAX_ENTRIES
	{
	NAME            ARP Table Size Limit
	SYNOPSIS        Maximum number of entries in ARP table. A value of 0 \
			allows unlimited ARP entries. To maintain the limit, \
			new ARP entries are added by deleting older non- \
			permanent entries. If the table fills up with all \
			permanent entries then it will not be possible to \
			add any new entries since none of the old entries \
			can be deleted.
	TYPE            uint
	DEFAULT         0
	}

InitGroup usrNetworkInit
        {
        INIT_RTN        usrNetworkInit ();
        SYNOPSIS        Initialize the network subsystem
        INIT_ORDER      INCLUDE_NET_SETUP 	\
			usrNetProtoInit		\
			INCLUDE_MUX 		\
			INCLUDE_END 		\
                        INCLUDE_PPP 		\
			INCLUDE_PPP_CRYPT 	\
			INCLUDE_SLIP 		\
			INCLUDE_NETWORK		\
 			usrNetworkBoot 		\
			usrNetworkAddrCheck     \
			INCLUDE_SECOND_SMNET 	\
			INCLUDE_DHCPC_LEASE_SAVE \
			usrNetRemoteInit 	\
			usrNetAppInit
        }

/* 
 * The components initialized by this group are strongly dependent on
 * initialization order. The associated configuration files also reflect
 * this dependency since earlier components create variables referenced
 * by later ones. As a side effect, the names of the configuration files
 * must be in alphabetical order, since the current implementation of the
 * configuration tool sorts by file name when generating the configlette 
 * list.
 */

InitGroup usrNetworkBoot
        {
        INIT_RTN        usrNetworkBoot ();
        SYNOPSIS        Setup a network device using the boot parameters
        INIT_ORDER      INCLUDE_NET_INIT 	\
                        usrNetworkAddrInit 	\
			INCLUDE_PPP_BOOT 	\
			INCLUDE_SLIP_BOOT 	\
			INCLUDE_NETMASK_GET 	\
			INCLUDE_NETDEV_NAMEGET 	\
			INCLUDE_SM_NET_ADDRGET   \
                        usrNetworkDevStart
        }

InitGroup usrNetworkAddrInit
        {
        INIT_RTN 	usrNetworkAddrInit ();
        SYNOPSIS 	Initialize the network address for a device
        INIT_ORDER 	INCLUDE_DHCPC_LEASE_GET \
			INCLUDE_DHCPC_LEASE_CLEAN
        }

InitGroup usrNetworkDevStart
        {
        INIT_RTN 	usrNetworkDevStart ();
        SYNOPSIS 	Attach a network device and start the loopback driver
	INIT_ORDER 	INCLUDE_SM_NET 		\
			INCLUDE_END_BOOT   	\
			INCLUDE_BSD_BOOT	\
			INCLUDE_LOOPBACK
        }

InitGroup usrNetworkAddrCheck
        {
        INIT_RTN 	usrNetworkAddrCheck ();
        SYNOPSIS 	Get an IP address if needed and setup the boot device
        INIT_ORDER 	INCLUDE_DHCPC \
			INCLUDE_DHCPC_LEASE_TEST \
			INCLUDE_NETDEV_CONFIG
        }

InitGroup usrNetProtoInit {
	NAME		initialize network protocol stacks
	SYNOPSIS	Initialize the network protocol stacks
	INIT_RTN	usrNetProtoInit ();
	INIT_ORDER	INCLUDE_BSD_SOCKET \
			INCLUDE_ZBUF_SOCK \
			BSD43_COMPATIBLE \
			INCLUDE_ROUTE_SOCK \
			INCLUDE_HOST_TBL \
			INCLUDE_IP \
			INCLUDE_MIB2_SYSTEM \
			INCLUDE_MIB2_IF \
			INCLUDE_IP_FILTER \
			INCLUDE_UDP \
			INCLUDE_MIB2_UDP \
			INCLUDE_UDP_SHOW \
			INCLUDE_TCP \
			INCLUDE_MIB2_TCP \
			INCLUDE_TCP_SHOW \
			INCLUDE_ICMP \
			INCLUDE_MIB2_ICMP \
			INCLUDE_ICMP_SHOW \
			INCLUDE_IGMP \
			INCLUDE_IGMP_SHOW \
			INCLUDE_SM_NET_SHOW \
			INCLUDE_MCAST_ROUTING \
			INCLUDE_OSPF \
			INCLUDE_NET_LIB \
			INCLUDE_MIB2_IP \
			INCLUDE_TCP_DEBUG \
			INCLUDE_ARP_API \
			INCLUDE_NET_SHOW
}

InitGroup usrNetRemoteInit {
	NAME		initialize network remote I/O access
	INIT_RTN	usrNetRemoteInit ();
	INIT_ORDER	INCLUDE_NET_HOST_SETUP \
                        INCLUDE_NET_REM_IO \
                        INCLUDE_NFS \
                        INCLUDE_NFS_MOUNT_ALL
}

InitGroup usrNetAppInit {
	NAME		initialize network application protocols
	INIT_RTN	usrNetAppInit ();
	INIT_ORDER	INCLUDE_RPC \
			INCLUDE_RLOGIN \
			INCLUDE_TELNET \
			INCLUDE_SECURITY \
			INCLUDE_TFTP_SERVER \
			INCLUDE_FTP_SERVER \
			INCLUDE_FTPD_SECURITY \
			INCLUDE_FTP \
			INCLUDE_NFS_SERVER \
                        INCLUDE_DHCPC_SHOW \
                        INCLUDE_DHCPR \
			INCLUDE_DHCPS \
			INCLUDE_SNTPC \
			INCLUDE_SNTPS \
			INCLUDE_PING \
			INCLUDE_PROXY_CLIENT \
			INCLUDE_PROXY_SERVER \
			INCLUDE_RIP \
			INCLUDE_DNS_RESOLVER \
			INCLUDE_SNMPD
}		
