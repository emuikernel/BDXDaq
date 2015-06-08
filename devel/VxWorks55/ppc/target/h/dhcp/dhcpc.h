/* dhcpc.h - DHCP runtime client include file for shared definitions */

/* Copyright 1984 - 2001 Wind River Systems, Inc. */

/*
modification history
---------------------
01k,12dec01,wap  Interface name field of if_info structure is too small for
                 some interface names (SPR #29087)
01j,31oct01,vvv  fixed warnings when BOOTP and DHCP are included in image
01i,09oct01,rae  merge from truestack ver 01b/2 base 01h (SPRs 69850, 68014)
01h,05apr00,spm  removed unused offset (BPF provides required alignment)
01g,24nov99,spm  upgraded to RFC 2131 and removed direct link-level access
01f,06oct97,spm  split interface name into device name and unit number
01e,06aug97,spm  removed parameters linked list to reduce memory required;
                 added definitions for C++ compilation; renamed class field
                 of dhcp_reqspec structure to prevent C++ errors (SPR #9079)
01d,02jun97,spm  changed DHCP option tags to prevent name conflicts (SPR #8667)
01c,07apr97,spm  removed #define's now in configAll.h, changed router_solicit
                 field in parameter descriptor from pointer to structure
01b,03jan97,spm  added little-endian support and moved macros to dhcp.h
01a,03oct96,spm  created by modifying WIDE project DHCP implementation
*/

#ifndef __INCdhcpch
#define __INCdhcpch

#ifdef __cplusplus
extern "C" {
#endif

#include "wdLib.h"

#include "netinet/in.h"
#include "netinet/if_ether.h"

#include "dhcp/dhcp.h"
#include "dhcp/copyright_dhcp.h"

#define ETHERHL    sizeof(struct ether_header)

#ifndef IPHL
#define IPHL       sizeof(struct ip)
#endif

#ifndef UDPHL
#define UDPHL      sizeof(struct udphdr)
#endif

#define WORD 		4 	/* Multiplier for IP header length field */

/* Notification routine event types. */

#define DHCPC_LEASE_NEW 0
#define DHCPC_LEASE_INVALID 1

/* DHCP client operations to access permanent storage. */

#define DHCP_CACHE_READ 0
#define DHCP_CACHE_WRITE 1
#define DHCP_CACHE_ERASE 2

#if CPU_FAMILY==I960
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

struct client_id
    {
    /*
     * The DHCP client identifer consists of a string of characters.
     * Typically, the client identifier contains a type value (from the
     * hardware type values in the ARP section of the assigned numbers
     * RFC) followed by the hardware address from the data link layer.
     */

    unsigned char       len;
    char *              id;
    };

struct request_list
    {
    /*
     * This structure defines the contents of the option request list, which
     * consists of a sequence of DHCP option tags of the indicated length.
     * A DHCP server may provide any, all, or none of the specified options
     * to the requesting client.
     */

    u_char      len; 		/* Length of request list. */
    char        list [0xff];     /* Storage for option tags. */
    };

struct vendor_list
    {
    /*
     * This structure defines the components of vendor-specific information
     * which may be returned by a DHCP server. The contents of that option
     * are not defined by the DHCP specification.
     */

    u_char      len;            /* Amount of vendor-specific data. */
    char        list [255];     /* Contents of vendor-specific data. */
    };

/* 
 * Request specification for DHCP lease - partially filled by 
 * dhcpcOptionSet() and/or dhcpcOptionAdd() routines.
 */

struct dhcp_reqspec
    {
    u_short 		maxlen; 	/* Maximum supported DHCP message */
    u_short 		optlen; 	/* Length of message's option field. */
    int waitsecs;       /* Duration for collecting additional lease offers. */
    struct in_addr      ipaddr;         /* Requested IP address */
    struct in_addr      srvaddr;        /* IP address of DHCP server */
    struct request_list reqlist;        /* List of requested options */
    struct client_id *  clid;           /* Client identifier */
    char *              dhcp_errmsg;
    struct dhcpcOpts *  pOptList; 	/* Suggested values for options */
    char * 		pOptions; 	/* Derived options field contents */
    };

struct in_addrs
    {
    /*
     * This structure defines the components of a list of IP addresses.
     * Some of the options provided by the DHCP server use this format.
     */

    unsigned char 	num; 	/* Number of IP addresses returned. */
    struct in_addr * 	addr; 	/* IP address values. */
    };

struct u_shorts
    {
    /*
     * This structure defines a list of 16-bit numbers used for some of
     * the DHCP options.
     */

    unsigned char 	num; 		/* Number of 16-bit values returned. */
    unsigned short * 	shortnum; 	/* List of values returned. */
    };

#define MAXTAGNUM     (_DHCP_LAST_OPTION + 1)
#define OPTMASKSIZE     (MAXTAGNUM / 8 + 1 )

/* Options storage structure - used by the dhcpcOptionAdd () routine. */

struct dhcpcOpts
    {
    int 	optlen; 	/* Current total size of option data */
    char 	optmask [OPTMASKSIZE]; 	/* indicates available entries */
    ULONG 	tag1; 		/* subnet mask value */
    long 	tag2; 		/* time offset value */
    UCHAR * 	pTag3; 		/* router address values */
    UCHAR * 	pTag4; 		/* time server address values */
    UCHAR * 	pTag5; 		/* name server address values */
    UCHAR * 	pTag6; 		/* domain name server address values */
    UCHAR * 	pTag7; 		/* log server address values */
    UCHAR * 	pTag8; 		/* cookie server address values */
    UCHAR * 	pTag9; 		/* LPR server address values */
    UCHAR * 	pTag10; 	/* Impress server address values */
    UCHAR * 	pTag11; 	/* Resource location server address values */
    char * 	pTag12; 	/* Host name value */
    USHORT 	tag13; 		/* Boot file size value */
    char * 	pTag14; 	/* Merit dump file value */
    char * 	pTag15; 	/* Domain name value */
    ULONG 	tag16; 		/* Swap server value */
    char * 	pTag17; 	/* Root path value */
    char * 	pTag18; 	/* Extensions path value */
    UCHAR 	tag19; 		/* IP Forwarding flag value */
    UCHAR 	tag20; 		/* Non-local source routing flag value */
    UCHAR * 	pTag21; 	/* Policy filter address/mask values */
    USHORT 	tag22; 		/* Maximum datagram size value */
    UCHAR 	tag23; 		/* Default IP TTL value */
    ULONG 	tag24; 		/* Path MTU aging timeout value */
    UCHAR * 	pTag25; 	/* Path MTU plateau table values */
    USHORT 	tag26; 		/* MTU size value */
    UCHAR 	tag27; 		/* All subnets local flag value */
    ULONG 	tag28; 		/* Broadcast address value */
    UCHAR 	tag29; 		/* Mask discovery flag value */
    UCHAR	tag30; 		/* Mask supplier flag value */
    UCHAR 	tag31; 		/* Router discovery flag value */
    ULONG 	tag32; 		/* Router soliciation address value */
    UCHAR * 	pTag33; 	/* Static route address/mask values */
    UCHAR 	tag34; 		/* Trailer encapsulation flag value */
    ULONG 	tag35; 		/* ARP cache timeout value */
    UCHAR	tag36; 		/* Ethernet encapsulation flag value */
    UCHAR 	tag37; 		/* Default TCP TTL value */
    ULONG 	tag38; 		/* TCP keepalive interval value */
    UCHAR 	tag39; 		/* TCP keepalive garbage octet flag value */
    char * 	pTag40; 	/* NIS domain value */
    UCHAR * 	pTag41; 	/* NIS server address values */
    UCHAR * 	pTag42; 	/* NTP server address values */
    UCHAR * 	pTag43; 	/* Vendor specific information value */
    UCHAR * 	pTag44; 	/* NetBIOS name server address values */
    UCHAR * 	pTag45; 	/* NBDD server address values */
    UCHAR 	tag46; 		/* NetBIOS node type value */
    char * 	pTag47; 	/* NetBIOS scope value */
    UCHAR * 	pTag48; 	/* X Windows font server address values */
    UCHAR * 	pTag49; 	/* X Windows display manager address values */
     /* Tag 50 may not be set manually. */
    ULONG 	tag51; 		/* Lease time value */
     /* Tags 52, 53, 54 and 57 may not be set manually. */
    UCHAR * 	pTag55; 	/* Parameter request list value */
    char * 	pTag56; 	/* Error message value */
    ULONG 	tag58; 		/* Renewal time value */
    ULONG 	tag59; 		/* Rebinding time value */
    UCHAR * 	pTag60; 	/* Class identifier value */
    UCHAR * 	pTag61; 	/* Client identifier value */
     /* Tags 62 and 63 are currently undefined. */
    char * 	pTag64; 	/* NIS+ domain value */
    UCHAR * 	pTag65; 	/* NIS+ server address values */
    char * 	pTag66; 	/* TFTP server name value */
    char * 	pTag67; 	/* Boot file name value */
    UCHAR * 	pTag68; 	/* Mobile IP home agent address values */
    UCHAR * 	pTag69; 	/* SMTP server address values */
    UCHAR * 	pTag70; 	/* POP server address values */
    UCHAR * 	pTag71; 	/* NNTP server address values */
    UCHAR * 	pTag72; 	/* WWW server address values */
    UCHAR * 	pTag73; 	/* Finger server address values */
    UCHAR * 	pTag74; 	/* IRC server address values */
    UCHAR * 	pTag75; 	/* StreetTalk server address values */
    UCHAR * 	pTag76; 	/* STDA server address values */
    };

/* Parameter retrieval structure - used by the dhcpcParamsGet () routine. */

struct dhcp_param
    {
    char        got_option [OPTMASKSIZE];
    char        msgtype;                /* DHCP reply or BOOTP reply? */
    char *      sname;                  /* Host name of DHCP server */
    char *      temp_sname;    /* Server host name if 'sname' holds options. */
    char *      file;                   /* Boot file name */
    char *      temp_file;      /* File name if 'file' holds options. */
    char *      hostname;               /* Host name of bootstrap server */
    char *      merit_dump;
    char *      dns_domain;
    char *      root_path;
    char *      extensions_path;
    char *      nis_domain;
    char *      nb_scope;               /* NetBIOS scope */
    char *      errmsg;
    char *      nisp_domain;            /* NIS+ domain name */
    unsigned char       ip_forward;     /* Boolean flag */
    unsigned char       nonlocal_srcroute;      /* Boolean flag */
    unsigned char       all_subnet_local;       /* Boolean flag */
    unsigned char       mask_discover;          /* Boolean flag */
    unsigned char       mask_supplier;          /* Boolean flag */
    unsigned char       router_discover;        /* Boolean flag */
    unsigned char       trailer;                /* Boolean flag */
    unsigned char       ether_encap;            /* Boolean flag */
    unsigned char       keepalive_garba;        /* Boolean flag */
    unsigned char       nb_nodetype;            /* NetBIOS node type */

#define BNODE 0x1
#define PNODE 0x2
#define MNODE 0x4
#define HNODE 0x8
    unsigned char       default_ip_ttl;         /* time to live (1 - 255) */
    unsigned char       default_tcp_ttl;
    unsigned short      bootsize;               /* bootfile size in 16 bits */
    unsigned short      max_dgram_size;         /* max dgram reassembly */
    unsigned short      intf_mtu;               /* interface mtu */
    long        time_offset;            /* 32 bits integer (network byte order)
                                                offset from UTC */
    unsigned long       lease_origin;           /* DHCP lease start time */
    unsigned long       lease_duration;         /* DHCP lease duration */
    unsigned long       dhcp_t1;
    unsigned long       dhcp_t2;
    unsigned long       mtu_aging_timeout;      /* 32 bit unsigned integer */
    unsigned long       arp_cache_timeout;
    unsigned long       keepalive_inter;
    struct u_shorts *   mtu_plateau_table;      /* array of 16 bit integers */
    struct in_addr      server_id;              /* DHCP server's IP address */
    struct in_addr      ciaddr;                 /* Client's current address */
    struct in_addr      yiaddr;                 /* Client's assigned address */
    struct in_addr      siaddr;                 /* Address of (tftp) server */
    struct in_addr      giaddr;                 /* Address of relay agent */
    struct in_addr *    subnet_mask;
    struct in_addr *    swap_server;
    struct in_addr *    brdcast_addr;
    struct in_addr      router_solicit;
    struct in_addrs *   router;
    struct in_addrs *   time_server;
    struct in_addrs *   name_server;
    struct in_addrs *   dns_server;     /* DNS server */
    struct in_addrs *   log_server;
    struct in_addrs *   cookie_server;
    struct in_addrs *   lpr_server;
    struct in_addrs *   impress_server;
    struct in_addrs *   rls_server;     /* Resource Location Server */
    struct in_addrs *   policy_filter;
    struct in_addrs *   static_route;
    struct in_addrs *   nis_server;
    struct in_addrs *   ntp_server;
    struct in_addrs *   nbn_server;     /* NetBIOS name server */
    struct in_addrs *   nbdd_server;    /* NetBIOS dgram distrib server */
    struct in_addrs *   xfont_server;   /* X font erver */
    struct in_addrs *   xdisplay_manager;       /* X display manager */
    struct in_addrs *   nisp_server;    /* NIS+ server */
    struct in_addrs *   mobileip_ha;    /* Mobile IP Home Agent */
    struct in_addrs *   smtp_server;
    struct in_addrs *   pop3_server;
    struct in_addrs *   nntp_server;
    struct in_addrs *   dflt_www_server;
    struct in_addrs *   dflt_finger_server;
    struct in_addrs *   dflt_irc_server;
    struct in_addrs *   streettalk_server;
    struct in_addrs *   stda_server;    /* StreetTalk Directory Assistance */
    struct vendor_list * vendlist;      /* Vendor-specific information */
    };

struct chaddr 
    {
    unsigned char 	htype;
    unsigned char 	hlen;
    char 		haddr[MAX_HLEN];
    };

struct if_info 
    {
    char 		name [IFNAMSIZ];
    short 		unit;
    int 		bpfDev;     /* BPF file descriptor */
    int 		bufSize;    /* Size of BPF data buffer */
    struct chaddr 	haddr;
    struct ifnet * 	iface;
    };


struct msg
    {
    struct ip *ip;
    struct udphdr *udp;
    struct dhcp *dhcp;
    };

#if CPU_FAMILY==I960
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

#ifdef __cplusplus
}
#endif

#endif
