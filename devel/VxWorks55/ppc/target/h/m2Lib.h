/* m2Lib.h - VxWorks MIB-II interface to SNMP Agent */

/* Copyright 1984 - 2003 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01k,27jan03,vvv  added support for user-defined counters (SPR #83254)
01j,13jan03,rae  Merged from velocecp branch
01i,22apr02,rae  Fixed MAXOIDLENGH typo (SPR #73303)
01h,05dec01,vvv  changed M2_IFSTACKTBL index to unsigned (SPR #71316)
01g,10oct01,rae  merge from truestack ver 01n, base o1f
01f,25oct00,niq  Merge RFC2233 changes from open_stack-p1-r1 branch
01j,18may00,ann  merging from post R1 to include RFC2233 and the ipRouteMask
                 bug fix
01i,14apr00,ann  adding changes to make ipRouteMask writeable
01h,28mar00,rae  merging in IGMPv2 changes
01g,10mar00,ead  added function declaration for m2IfStackTblUpdate
01f,11feb00,ann  making changes for RFC 2233 implementation
01e,24mar99,ead  added m2SetIfLastChange() declaration (SPR #23290)
01d,10sep98,ann  added doc to M2_INTERFACETBL fields spr# 21078
01c,11apr97,rjc  fixed bad value of error define
01b,05feb97,rjc  added new error codes to support phase-2 ospf mib  
01a,08dec93,jag  written
*/

#ifndef __INCm2Libh
#define __INCm2Libh

#ifdef __cplusplus
extern "C" {
#endif

#include "net/if.h"
#include "avlLib.h"

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

/* m2Lib.c Error Codes */

#define S_m2Lib_INVALID_PARAMETER               (M_m2Lib | 1)
#define S_m2Lib_ENTRY_NOT_FOUND			(M_m2Lib | 2)
#define S_m2Lib_TCPCONN_FD_NOT_FOUND		(M_m2Lib | 3)
#define S_m2Lib_INVALID_VAR_TO_SET		(M_m2Lib | 4)
#define S_m2Lib_CANT_CREATE_SYS_SEM		(M_m2Lib | 5)
#define S_m2Lib_CANT_CREATE_IF_SEM		(M_m2Lib | 6)
#define S_m2Lib_CANT_CREATE_ROUTE_SEM		(M_m2Lib | 7)
#define S_m2Lib_ARP_PHYSADDR_NOT_SPECIFIED	(M_m2Lib | 8)
#define S_m2Lib_IF_TBL_IS_EMPTY			(M_m2Lib | 9)
#define S_m2Lib_IF_CNFG_CHANGED			(M_m2Lib | 10)
#define S_m2Lib_TOO_BIG                         (M_m2Lib | 11)
#define S_m2Lib_BAD_VALUE                       (M_m2Lib | 12)
#define S_m2Lib_READ_ONLY                       (M_m2Lib | 13)
#define S_m2Lib_GEN_ERR                         (M_m2Lib | 14)

#define   ETHERADDRLEN            6
#define   M2DISPLAYSTRSIZE	256
#define   MAXOIDLENGTH           40
#define   MAXIFPHYADDR		 16

/* defines for enumerated types as specified by RFC 1213 */

/* possible values for ifType (found in IANA-MIB) */

#define M2_ifType_other                  1   /* none of the following */
#define M2_ifType_regular1822            2
#define M2_ifType_hdh1822                3
#define M2_ifType_ddnX25                 4
#define M2_ifType_rfc877x25              5
#define M2_ifType_ethernetCsmacd         6
#define M2_ifType_ethernet_csmacd        M2_ifType_ethernetCsmacd
#define M2_ifType_iso88023Csmacd         7
#define M2_ifType_iso88023_csmacd        M2_ifType_iso88023Csmacd
#define M2_ifType_iso88024TokenBus       8
#define M2_ifType_iso88024_tokenBus      M2_ifType_iso88024TokenBus
#define M2_ifType_iso88025TokenRing      9
#define M2_ifType_iso88025_tokenRing     M2_ifType_iso88025TokenRing
#define M2_ifType_iso88026Man            10
#define M2_ifType_iso88026_man           M2_ifType_iso88026Man
#define M2_ifType_starLan                11
#define M2_ifType_proteon10Mbit          12
#define M2_ifType_proteon80Mbit          13
#define M2_ifType_hyperchannel           14
#define M2_ifType_fddi                   15
#define M2_ifType_lapb                   16
#define M2_ifType_sdlc                   17
#define M2_ifType_ds1                    18  /* DS1-MIB */
#define M2_ifType_e1                     19  /* Obsolete see DS1-MIB */
#define M2_ifType_basicISDN              20
#define M2_ifType_primaryISDN            21
#define M2_ifType_propPointToPointSerial 22  /* proprietary serial */
#define M2_ifType_ppp                    23
#define M2_ifType_softwareLoopback       24
#define M2_ifType_eon                    25  /* CLNP over IP */
#define M2_ifType_ethernet3Mbit          26
#define M2_ifType_nsip                   27  /* XNS over IP */
#define M2_ifType_slip                   28  /* generic SLIP */
#define M2_ifType_ultra                  29  /* ULTRA technologies */
#define M2_ifType_ds3                    30  /* DS3-MIB */
#define M2_ifType_sip                    31  /* SMDS, coffee */
#define M2_ifType_frameRelay             32  /* DTE only. */
#define M2_ifType_rs232                  33
#define M2_ifType_para                   34  /* parallel-port */
#define M2_ifType_arcnet                 35  /* arcnet */
#define M2_ifType_arcnetPlus             36  /* arcnet plus */
#define M2_ifType_atm                    37  /* ATM cells */
#define M2_ifType_miox25                 38
#define M2_ifType_sonet                  39  /* SONET or SDH */
#define M2_ifType_x25ple                 40
#define M2_ifType_iso88022llc            41
#define M2_ifType_localTalk              42
#define M2_ifType_smdsDxi                43
#define M2_ifType_frameRelayService      44  /* FRNETSERV-MIB */
#define M2_ifType_v35                    45
#define M2_ifType_hssi                   46
#define M2_ifType_hippi                  47
#define M2_ifType_modem                  48  /* Generic modem */
#define M2_ifType_aal5                   49  /* AAL5 over ATM */
#define M2_ifType_sonetPath              50
#define M2_ifType_sonetVT                51
#define M2_ifType_smdsIcip               52  /* SMDS InterCarrier Interface */
#define M2_ifType_propVirtual            53  /* proprietary virtual/internal */
#define M2_ifType_propMultiplexor        54  /* proprietary multiplexing */
#define M2_ifType_ieee80212              55  /* 100BaseVG */
#define M2_ifType_fibreChannel           56  /* Fibre Channel */
#define M2_ifType_hippiInterface         57  /* HIPPI interfaces */
#define M2_ifType_frameRelayInterconnect 58  /* Obsolete use either 32 or 44 */
#define M2_ifType_aflane8023             59  /* ATM Emulated LAN for 802.3 */
#define M2_ifType_aflane8025             60  /* ATM Emulated LAN for 802.5 */
#define M2_ifType_cctEmul                61  /* ATM Emulated circuit */
#define M2_ifType_fastEther              62  /* Fast Ethernet (100BaseT) */
#define M2_ifType_isdn                   63  /* ISDN and X.25 */
#define M2_ifType_v11                    64  /* CCITT V.11/X.21 */
#define M2_ifType_v36                    65  /* CCITT V.36 */
#define M2_ifType_g703at64k              66  /* CCITT G703 at 64Kbps */
#define M2_ifType_g703at2mb              67  /* Obsolete see DS1-MIB */
#define M2_ifType_qllc                   68  /* SNA QLLC */
#define M2_ifType_fastEtherFX            69  /* Fast Ethernet (100BaseFX) */
#define M2_ifType_channel                70  /* channel */
#define M2_ifType_ieee80211              71  /* radio spread spectrum */
#define M2_ifType_ibm370parChan          72  /* IBM 360/370 OEMI Channel */
#define M2_ifType_escon                  73  /* IBM Enterprise Systems Cnx */
#define M2_ifType_dlsw                   74  /* Data Link Switching */
#define M2_ifType_isdns                  75  /* ISDN S/T interface */
#define M2_ifType_isdnu                  76  /* ISDN U interface */
#define M2_ifType_lapd                   77  /* Link Access Protocol D */
#define M2_ifType_ipSwitch               78  /* IP Switching Objects */
#define M2_ifType_rsrb                   79  /* Remote Source Route Bridging */
#define M2_ifType_atmLogical             80  /* ATM Logical Port */
#define M2_ifType_ds0                    81  /* Digital Signal Level 0 */
#define M2_ifType_ds0Bundle              82  /* ds0s on the same ds1 */
#define M2_ifType_bsc                    83  /* Bisynchronous Protocol */
#define M2_ifType_async                  84  /* Asynchronous Protocol */
#define M2_ifType_cnr                    85  /* Combat Net Radio */
#define M2_ifType_iso88025Dtr            86  /* ISO 802.5r DTR */
#define M2_ifType_eplrs                  87  /* Ext Pos Loc Report Sys */
#define M2_ifType_arap                   88  /* Appletalk Remote Access Proto */
#define M2_ifType_propCnls               89  /* Proprietary Cnx-less Proto */
#define M2_ifType_hostPad                90  /* CCITT-ITU X.29 PAD Protocol */
#define M2_ifType_termPad                91  /* CCITT-ITU X.3 PAD Facility */
#define M2_ifType_frameRelayMPI          92  /* Multiproto Interconnect FR */
#define M2_ifType_x213                   93  /* CCITT-ITU X213 */
#define M2_ifType_adsl                   94  /* Asymmetric DSL */
#define M2_ifType_radsl                  95  /* Rate-Adapt. DSL */
#define M2_ifType_sdsl                   96  /* Symmetric DSL */
#define M2_ifType_vdsl                   97  /* Very H-Speed DSL */
#define M2_ifType_iso88025CRFPInt        98  /* ISO 802.5 CRFP */
#define M2_ifType_myrinet                99  /* Myricom Myrinet */
#define M2_ifType_voiceEM                100 /* voice recEive and transMit */
#define M2_ifType_voiceFXO               101 /* voice Foreign Exchg Office */
#define M2_ifType_voiceFXS               102 /* voice Foreign Exchg Station */
#define M2_ifType_voiceEncap             103 /* voice encapsulation */
#define M2_ifType_voiceOverIp            104 /* voice over IP encapsulation */
#define M2_ifType_atmDxi                 105 /* ATM DXI */
#define M2_ifType_atmFuni                106 /* ATM FUNI */
#define M2_ifType_atmIma                 107 /* ATM IMA */
#define M2_ifType_pppMultilinkBundle     108 /* PPP Multilink Bundle */
#define M2_ifType_ipOverCdlc             109 /* IBM ipOverCdlc */
#define M2_ifType_ipOverClaw             110 /* IBM Common Link Access */
#define M2_ifType_stackToStack           111 /* IBM stackToStack */
#define M2_ifType_virtualIpAddress       112 /* IBM VIPA */
#define M2_ifType_mpc                    113 /* IBM multi-protocol channel */
#define M2_ifType_ipOverAtm              114 /* IBM ipOverAtm */
#define M2_ifType_iso88025Fiber          115 /* ISO 802.5j Fiber Token Ring */
#define M2_ifType_tdlc                   116 /* IBM twinaxial data link */
#define M2_ifType_gigabitEthernet        117 /* Gigabit Ethernet */
#define M2_ifType_hdlc                   118 /* HDLC */
#define M2_ifType_lapf                   119 /* LAP F */
#define M2_ifType_v37                    120 /* V.37 */
#define M2_ifType_x25mlp                 121 /* Multi-Link Protocol */
#define M2_ifType_x25huntGroup           122 /* X25 Hunt Group */
#define M2_ifType_trasnpHdlc             123 /* Transp HDLC */
#define M2_ifType_interleave             124 /* Interleave channel */
#define M2_ifType_fast                   125 /* Fast channel */
#define M2_ifType_ip                     126 /* IP (APPN HPR in IP networks) */
#define M2_ifType_docsCableMaclayer      127 /* CATV Mac Layer */
#define M2_ifType_docsCableDownstream    128 /* CATV Downstream interface */
#define M2_ifType_docsCableUpstream      129 /* CATV Upstream interface */
#define M2_ifType_a12MppSwitch           130 /* Avalon Parallel Processor */
#define M2_ifType_tunnel                 131 /* Encapsulation interface */
#define M2_ifType_coffee                 132 /* coffee pot */
#define M2_ifType_ces                    133 /* Circuit Emulation Service */
#define M2_ifType_atmSubInterface        134 /* ATM Sub Interface */
#define M2_ifType_l2vlan                 135 /* Layer 2 V-LAN using 802.1Q */
#define M2_ifType_l3ipvlan               136 /* Layer 3 V-LAN using IP */
#define M2_ifType_l3ipxvlan              137 /* Layer 3 V-LAN using IPX */
#define M2_ifType_digitalPowerline       138 /* IP over Power Lines */
#define M2_ifType_mediaMailOverIp        139 /* Multimedia Mail over IP */
#define M2_ifType_dtm                    140 /* Dynamic sync Transfer Mode */
#define M2_ifType_dcn                    141 /* Data Communications Network */
#define M2_ifType_ipForward              142 /* IP Forwarding Interface */
#define M2_ifType_msdsl                  143 /* Multi-rate Symmetric DSL */
#define M2_ifType_ieee1394               144 /* IEEE1394 High Perf Serial Bus */
#define M2_ifType_if_gsn                 145 /* HIPPI-6400 */
#define M2_ifType_dvbRccMacLayer         146 /* DVB-RCC MAC Layer */
#define M2_ifType_dvbRccDownstream       147 /* DVB-RCC Downstream Channel */
#define M2_ifType_dvbRccUpstream         148 /* DVB-RCC Upstream Channel */
#define M2_ifType_atmVirtual             149 /* ATM Virtual Interface */
#define M2_ifType_mplsTunnel             150 /* MPLS Tunnel Virtual Interface */
#define M2_ifType_srp                    151 /* Spatial Reuse Protocol */
#define M2_ifType_voiceOverAtm           152 /* Voice Over ATM */
#define M2_ifType_voiceOverFrameRelay    153 /* Voice Over Frame Relay */
#define M2_ifType_idsl                   154 /* DSL over ISDN */
#define M2_ifType_compositeLink          155 /* Avici Composite Link */
#define M2_ifType_ss7SigLink             156 /* SS7 Signaling Link */
#define M2_ifType_pmp                    157 /* Point to Multipoint */

/* packet control used for stat counting routines (in or out) */

#define M2_PACKET_IN    1
#define M2_PACKET_OUT   2

/* interface attach/insert and detach/remove */

#define M2_IF_TABLE_INSERT 1
#define M2_IF_TABLE_REMOVE 2

/* stack layer insert/remove */
#define M2_STACK_TABLE_INSERT 1
#define M2_STACK_TABLE_REMOVE 2

/* interface mib styles used by the MUX/drivers */

#define MIB_STYLE_1213 1
#define MIB_STYLE_2233 2

/* interface counter IDs */

#define M2_ctrId_ifInOctets            1
#define M2_ctrId_ifInUcastPkts         2
#define M2_ctrId_ifInNUcastPkts        3
#define M2_ctrId_ifInDiscards          4
#define M2_ctrId_ifInErrors            5
#define M2_ctrId_ifInUnknownProtos     6
#define M2_ctrId_ifOutOctets           7
#define M2_ctrId_ifOutUcastPkts        8
#define M2_ctrId_ifOutNUcastPkts       9
#define M2_ctrId_ifOutDiscards         10
#define M2_ctrId_ifOutErrors           11
#define M2_ctrId_ifInMulticastPkts     12
#define M2_ctrId_ifInBroadcastPkts     13
#define M2_ctrId_ifOutMulticastPkts    14
#define M2_ctrId_ifOutBroadcastPkts    15
#define M2_ctrId_ifHCInOctets          16
#define M2_ctrId_ifHCInUcastPkts       17
#define M2_ctrId_ifHCInMulticastPkts   18
#define M2_ctrId_ifHCInBroadcastPkts   19
#define M2_ctrId_ifHCOutOctets         20
#define M2_ctrId_ifHCOutUcastPkts      21
#define M2_ctrId_ifHCOutMulticastPkts  22
#define M2_ctrId_ifHCOutBroadcastPkts  23

/* interface variable IDs */

#define M2_varId_ifAdminStatus               1
#define M2_varId_ifLinkUpDownTrapEnable      2
#define M2_varId_ifMtu                       3
#define M2_varId_ifPromiscuousMode           4
#define M2_varId_ifPhysAddress               5
#define M2_varId_ifDescr                     6
#define M2_varId_ifOperStatus                7
#define M2_varId_ifAlias                     8
#define M2_varId_ifOutQLen                   9
#define M2_varId_ifSpecific                  10
#define M2_varId_ifName                      11
#define M2_varId_ifType                      12
#define M2_varId_ifHighSpeed                 13
#define M2_varId_ifSpeed                     14
#define M2_varId_ifConnectorPresent          15
#define M2_varId_ifLastChange                16

/* possible values for ifAdminStatus */

#define M2_ifAdminStatus_up           			1
#define M2_ifAdminStatus_down				2
#define M2_ifAdminStatus_testing        		3

/* possible values for ifOperStatus */

#define M2_ifOperStatus_up              		1
#define M2_ifOperStatus_down            		2
#define M2_ifOperStatus_testing         		3

/* possible values for ifPromiscuousMode */

#define M2_ifPromiscuousMode_on                         1
#define M2_ifPromiscuousMode_off                        2

/* Row Status defines */

#define ROW_ACTIVE                                      1
#define ROW_NOTINSERVICE                                2
#define ROW_NOTREADY                                    3
#define ROW_CREATEANDGO                                 4
#define ROW_CREATEANDWAIT                               5
#define ROW_DESTROY                                     6

/* Storage type defines */

#define STORAGE_OTHER                                   1
#define STORAGE_VOLATILE                                2
#define STORAGE_NONVOLATILE                             3    

/* Boolean Values */

#define M2_TRUE                                         1
#define M2_FALSE                                        2

/* defines for all the objects in the interface table */

#define M2_IFLASTCHANGE                                 1
#define M2_IFINOCTETS                                   2
#define M2_IFINUCASTPKTS                                3
#define M2_IFINNUCASTPKTS                               4
#define M2_IFINDISCARDS                                 5
#define M2_IFINERRORS                                   6
#define M2_IFINUNKNOWNPROTOS                            7
#define M2_IFOUTOCTETS                                  8
#define M2_IFOUTUCASTPKTS                               9
#define M2_IFOUTNUCASTPKTS                              10
#define M2_IFOUTDISCARDS                                11
#define M2_IFOUTERRORS                                  12
#define M2_IFOUTQLEN                                    13
#define M2_IFINMULTICASTPKTS                            14
#define M2_IFINBROADCASTPKTS                            15
#define M2_IFOUTMULTICASTPKTS                           16
#define M2_IFOUTBROADCASTPKTS                           17
#define M2_IFHCINOCTETS                                 18
#define M2_IFHCINUCASTPKTS                              19
#define M2_IFHCINMULTICASTPKTS                          20
#define M2_IFHCINBROADCASTPKTS                          21
#define M2_IFHCOUTOCTETS                                22
#define M2_IFHCOUTUCASTPKTS                             23
#define M2_IFHCOUTMULTICASTPKTS                         24
#define M2_IFHCOUTBROADCASTPKTS                         25
#define M2_IFLINKUPDOWNTRAPENABLE                       26
#define M2_IFHIGHSPEED                                  27
#define M2_IFPROMISCUOUSMODE                            28
#define M2_IFCONNECTORPRESENT                           29

/* defines for rcvAddr table entries */
#define M2_IFRCVADDRSTATUS                              1
#define M2_IFRCVADDRTYPE                                2

/* possible values for  ipForwarding */

#define M2_ipForwarding_forwarding      		1
#define M2_ipForwarding_not_forwarding  		2

/* possible values for ipRouteType */

#define M2_ipRouteType_other            		1
#define M2_ipRouteType_invalid          		2
#define M2_ipRouteType_direct           		3
#define M2_ipRouteType_indirect         		4

/* possible values for ipRouteProto */

#define M2_ipRouteProto_other           		1
#define M2_ipRouteProto_local           		2
#define M2_ipRouteProto_netmgmt         		3
#define M2_ipRouteProto_icmp            		4
#define M2_ipRouteProto_egp             		5
#define M2_ipRouteProto_ggp             		6
#define M2_ipRouteProto_hello           		7
#define M2_ipRouteProto_rip             		8
#define M2_ipRouteProto_is_is           		9
#define M2_ipRouteProto_es_is				10
#define M2_ipRouteProto_ciscoIgrp			11
#define M2_ipRouteProto_bbnSpfIgp			12
#define M2_ipRouteProto_ospf				13
#define M2_ipRouteProto_bgp     			14

/* possible values for ipNetToMediaType */

#define M2_ipNetToMediaType_other			1
#define M2_ipNetToMediaType_invalid     		2
#define M2_ipNetToMediaType_dynamic     		3
#define M2_ipNetToMediaType_static      		4

/* possible values for tcpRtoAlgorithm */

#define M2_tcpRtoAlgorithm_other        		1
#define M2_tcpRtoAlgorithm_constant     		2
#define M2_tcpRtoAlgorithm_rsre         		3
#define M2_tcpRtoAlgorithm_vanj         		4

/* possible values for tcpConnState */

#define M2_tcpConnState_closed          		1
#define M2_tcpConnState_listen          		2
#define M2_tcpConnState_synSent         		3
#define M2_tcpConnState_synReceived     		4
#define M2_tcpConnState_established     		5
#define M2_tcpConnState_finWait1        		6
#define M2_tcpConnState_finWait2        		7
#define M2_tcpConnState_closeWait       		8
#define M2_tcpConnState_lastAck         		9
#define M2_tcpConnState_closing         		10
#define M2_tcpConnState_timeWait        		11
#define M2_tcpConnState_deleteTCB       		12


/*
 * When using vxWorks SNMP this constants must have the same value as NEXT and
 * EXACT
 */

#define M2_EXACT_VALUE      	0xA0
#define M2_NEXT_VALUE           0xA1 


                                             /* defines for IGMP varToSet */
#define M2_var_igmpInterfaceQueryInterval                1
#define M2_var_igmpInterfaceStatus                       2
#define M2_var_igmpInterfaceVersion                      4
#define M2_var_igmpInterfaceQueryMaxResponseTime         8
#define M2_var_igmpInterfaceRobustness                  16 
#define M2_var_igmpInterfaceLastMembQueryIntvl          32



/* IGMP defines from leaf.h generated file */

#define LEAF_igmpInterfaceIfIndex       1
#define MIN_igmpInterfaceIfIndex        1L
#define MAX_igmpInterfaceIfIndex        2147483647L
#define LEAF_igmpInterfaceQueryInterval 2
#define LEAF_igmpInterfaceStatus        3
#define VAL_igmpInterfaceStatus_active  1L
#define VAL_igmpInterfaceStatus_notInService    2L
#define VAL_igmpInterfaceStatus_notReady        3L
#define VAL_igmpInterfaceStatus_createAndGo     4L
#define VAL_igmpInterfaceStatus_createAndWait   5L
#define VAL_igmpInterfaceStatus_destroy 6L
#define LEAF_igmpInterfaceVersion       4
#define LEAF_igmpInterfaceQuerier       5
#define LEAF_igmpInterfaceQueryMaxResponseTime  6
#define MIN_igmpInterfaceQueryMaxResponseTime   0L
#define MAX_igmpInterfaceQueryMaxResponseTime   255L
#define LEAF_igmpInterfaceQuerierUpTime 7
#define LEAF_igmpInterfaceQuerierExpiryTime     8
#define LEAF_igmpInterfaceVersion1QuerierTimer  9
#define LEAF_igmpInterfaceWrongVersionQueries   10
#define LEAF_igmpInterfaceJoins 11
#define LEAF_igmpInterfaceProxyIfIndex  12
#define MIN_igmpInterfaceProxyIfIndex   0L
#define MAX_igmpInterfaceProxyIfIndex   2147483647L
#define LEAF_igmpInterfaceGroups        13
#define LEAF_igmpInterfaceRobustness    14
#define MIN_igmpInterfaceRobustness     1L
#define MAX_igmpInterfaceRobustness     255L
#define LEAF_igmpInterfaceLastMembQueryIntvl    15
#define MIN_igmpInterfaceLastMembQueryIntvl     0L
#define MAX_igmpInterfaceLastMembQueryIntvl     255L
#define LEAF_igmpCacheAddress   1
#define LEAF_igmpCacheIfIndex   2
#define MIN_igmpCacheIfIndex    1L
#define MAX_igmpCacheIfIndex    2147483647L
#define LEAF_igmpCacheSelf      3
#define VAL_igmpCacheSelf_true  1L
#define VAL_igmpCacheSelf_false 2L
#define LEAF_igmpCacheLastReporter      4
#define LEAF_igmpCacheUpTime    5
#define LEAF_igmpCacheExpiryTime        6
#define LEAF_igmpCacheStatus    7
#define VAL_igmpCacheStatus_active      1L
#define VAL_igmpCacheStatus_notInService        2L
#define VAL_igmpCacheStatus_notReady    3L
#define VAL_igmpCacheStatus_createAndGo 4L
#define VAL_igmpCacheStatus_createAndWait       5L
#define VAL_igmpCacheStatus_destroy     6L
#define LEAF_igmpCacheVersion1HostTimer 8


/* simple type used for imitating 64 bit integers */

typedef struct ui64
    {
    UINT high;
    UINT low;
    } UI64;

/*
 * UI64_ZERO - Zero a UI64 struct
 *
 * PARAMETERS:  UI64 * Pointer to the object to be zeroed.
 */
#define UI64_ZERO(N) (N)->high = (N)->low = 0L

/*
 * UI64_ADD32 - Add a UINT to a UI64
 * 
 * PARAMETERS:  UI64 * pointer to the object to be updated
 *              UINT   the quantity to add in.
 * 
 * RESTRICTIONS: This routine will wrap the object if necessary and not give
 *               any warning about having done so.
 */
#define UI64_ADD32(N, I) (N)->low += I; if ((N)->low < I) (N)->high++

/*
 * UI64_SUB64 - Subtract two UI64s
 *
 * PARAMETERS:  UI64 * the result goes here
 *              UI64 * Subtract from this object
 *              UI64 * the object to subtract
 *
 * RESTRICTIONS: This routine will wrap the object if necessary and not give
 *               any warning about having done so.
 */
#define UI64_SUB64(X, Y, Z)  (X)->low = (Y)->low - (Z)->low; \
                                   (X)->high = (Y)->high - (Z)->high; \
                                   if ((X)->low > (Y)->low) (X)->high-- 

/*
 * UI64_COPY - Copy one UI64 to another
 *
 * PARAMETERS:  UI64 * the result goes here
 *              UI64 * copy from this object
 */
#define UI64_COPY(X, Y)  (X)->low = (Y)->low; (X)->high = (Y)->high

/*
 * UI64_COMP - Compare two UI64s
 *
 * PARAMETERS:  UI64 * The two objects to compare
 *              UI64 * 
 *
 * RETURNS:  < 0 if the first is smaller than the second
 *           > 0 if the first is greater than the second
 *             0 if they are equal.
 */
#define UI64_COMP(X, Y) \
  (((X)->high == (Y)->high) ? \
   (((X)->low == (Y)->low)  ? (0) : (((X)->low > (Y)->low) ? (1) : (-1))) : \
   (((X)->high > (Y)->high) ? (1) : (-1)))


typedef struct
    {
    long   idLength;			/* Length of the object identifier */
    long   idArray [MAXOIDLENGTH];	/* Object Id numbers */
 
    } M2_OBJECTID;

typedef struct
    {
    long           addrLength;			/* Length of address */
    unsigned char  phyAddress [MAXIFPHYADDR];   /* physical address value */

    } M2_PHYADDR;


/*
 * The structures that follow are based on the MIB-II RFC-1213.  Each field in
 * each of the structures has the same name as the name specified in by the RFC.
 * Please refer to the RFC for a complete description of the variable and its
 * semantics.
 */


/* System Group bit fields that map to variables that can be set */

#define    M2SYSNAME    	0x01
#define    M2SYSCONTACT 	0x02
#define	   M2SYSLOCATION	0x04

typedef struct
    {
    unsigned char   sysDescr     [M2DISPLAYSTRSIZE];
    M2_OBJECTID     sysObjectID;
    unsigned long   sysUpTime;
    unsigned char   sysContact   [M2DISPLAYSTRSIZE];
    unsigned char   sysName      [M2DISPLAYSTRSIZE];
    unsigned char   sysLocation  [M2DISPLAYSTRSIZE];
    long            sysServices;

    }  M2_SYSTEM;


/* Interface group variables */

typedef struct
    {
    long  ifNumber;         /* Number of Interfaces in the System */
    ULONG ifTableLastChange; /* Time the ifTable was last changed */
    ULONG ifStackLastChange; /* Time the stackTable was last changed */
    }  M2_INTERFACE;


/* values as per RFC 1215 */

#define M2_LINK_DOWN_TRAP	2
#define M2_LINK_UP_TRAP		3

#define M2_LINK_UP_DOWN_TRAP_ENABLED   1
#define M2_LINK_UP_DOWN_TRAP_DISABLED  2

#define M2_PROMISCUOUS_MODE_ON   1
#define M2_PROMISCUOUS_MODE_OFF  2

#define M2_CONNECTOR_PRESENT      1
#define M2_CONNECTOR_NOT_PRESENT  2

typedef struct
    {
    int  	    ifIndex;			 /* Unique value for each interface */
    char 	    ifDescr  [M2DISPLAYSTRSIZE]; /* Information about the interface */
    long            ifType;            /* Type of interface */
    long            ifMtu;             /* Max size of datagram for the interface */
    unsigned long   ifSpeed;           /* Interface's current bandwidth in bits/sec */
    M2_PHYADDR      ifPhysAddress;     /* Mac layer address of the interface */
    long            ifAdminStatus;     /* Desired state for the interface */
    long            ifOperStatus;      /* Current operational status of the interface */
    unsigned long   ifLastChange;      /* Value of sysUpTime when interface entered 
                                          its current operational state */
    unsigned long   ifInOctets;        /* Number of octets received on the interface */
    unsigned long   ifInUcastPkts;     /* Number of subnetwork-unicast packets
                                          delivered to a higher-layer protocol */
    unsigned long   ifInNUcastPkts;    /* Number of non-unicast packets delivered
                                          to a higher layer protocol */
    unsigned long   ifInDiscards;      /* Number of inbound packets discarded */
    unsigned long   ifInErrors;        /* Inbound packets that contained errors */
    unsigned long   ifInUnknownProtos; /* Inbound packets discarded due to unknown 
                                          or unsupported protocol */
    unsigned long   ifOutOctets;       /* Number of octets transmitted on the interface */
    unsigned long   ifOutUcastPkts;    /* Number of packets transmitted to subnet 
                                          unicast address */
    unsigned long   ifOutNUcastPkts;   /* Number of packets sent to non-unicast
                                          address */
    unsigned long   ifOutDiscards;     /* Number of outbound packets discarded */
    unsigned long   ifOutErrors;       /* Number of outbound packets that could not
                                          be transmitted due to errors */
    unsigned long   ifOutQLen;         /* Length of output packet queue */
    M2_OBJECTID     ifSpecific;        /* Reference to MIB definitions specific to
                                          particular media being used */
    }  M2_INTERFACETBL;

/* The RFC 2233 additions to the ifTable */

typedef struct 
    {
    char         ifName [M2DISPLAYSTRSIZE];
    ULONG        ifInMulticastPkts;
    ULONG        ifInBroadcastPkts;
    ULONG        ifOutMulticastPkts;
    ULONG        ifOutBroadcastPkts;
    UI64         ifHCInOctets;
    UI64         ifHCInUcastPkts;
    UI64         ifHCInMulticastPkts;
    UI64         ifHCInBroadcastPkts;
    UI64         ifHCOutOctets;
    UI64         ifHCOutUcastPkts;
    UI64         ifHCOutMulticastPkts;
    UI64         ifHCOutBroadcastPkts;
    UINT         ifLinkUpDownTrapEnable;
    ULONG        ifHighSpeed;
    UINT         ifPromiscuousMode;
    UINT         ifConnectorPresent;
    char         ifAlias [M2DISPLAYSTRSIZE];
    ULONG        ifCounterDiscontinuityTime;
    } M2_2233TBL;

/* Update Routines */

typedef struct M2_ID_S M2_ID;
typedef struct M2_IFSTACKTBL_S M2_IFSTACKTBL;
typedef STATUS (* M2_PKT_COUNT_RTN) (M2_ID *, UINT, UCHAR *, ULONG);
typedef STATUS (* M2_CTR_UPDATE_RTN) (M2_ID *, UINT, ULONG);
typedef STATUS (* M2_VAR_UPDATE_RTN) (M2_ID *, UINT, caddr_t);

/* Combination of the above interface structs */

typedef struct
    {
    M2_INTERFACETBL   mibIfTbl;         /* the RFC 1213 objects */
    M2_2233TBL        mibXIfTbl;        /* the RFC 2233 extension */
    } M2_DATA;

/* The interface table struct for RFC 2233 compliance */

struct M2_ID_S
    {
    M2_DATA           m2Data;           /* the interface table */
    M2_PKT_COUNT_RTN  m2PktCountRtn;    /* packet counter function */
    M2_CTR_UPDATE_RTN m2CtrUpdateRtn;   /* counter increment function */
    M2_VAR_UPDATE_RTN m2VarUpdateRtn;   /* variable update routine */
    void *            m2UsrCounter;     /* pointer to additional counters */
    };

/* The structure for the ifStack table as described in RFC 2233 */

struct M2_IFSTACKTBL_S
    {
    UINT                     index;      /* The ifIndex */ 
    int                      status;     /* status of relationship */
    struct M2_IFSTACKTBL_S * pNextLower; /* Pointer to the next relation */
    };

/* The structure for the ifRcv address table as described in RFC 2233 */

typedef struct M2_IFRCVADDRTBL_S 
    {
    M2_PHYADDR                 ifRcvAddrAddr;/* The physical address for which
                                              * system will accept pkts/frames
                                              */
    int                        ifRcvAddrStatus; /* The rowStatus */
    int                        ifRcvAddrType;   /* The address type */
    struct M2_IFRCVADDRTBL_S * pNextEntry;      /* Next pointer */
    } M2_IFRCVADDRTBL;
    
/* The structure for the ifIndex value and corresponding ifnet pointer */
    
typedef struct M2_IFINDEX_S
    {
    AVL_NODE          ifNode;       /* Node representing an if in AVL tree */ 
    int               ifType;       /* Interface type */
    int               ifIndex;      /* Unique value for each interface */
    M2_OBJECTID       ifOid;        /* I/F Object Id */
    void *            pIfStats;     /* Pointer to the driver specific
                                     * structure
                                     */
    M2_IFSTACKTBL *   pNextLower;   /* List of lower sub-layers */
    M2_IFRCVADDRTBL * pRcvAddr;     /* rcvAddr list */

		      /* Ioctl to munge interface flags */
		      /* Called from m2IfTblEntrySet to change the values in */
		      /* the ifnet structure in a non-protocol-specific way. */
		      /* Callbacks are bad for tor3 (so I've heard). */
    int               (*ifIoctl) (struct socket*, u_long, caddr_t);
    STATUS            (*rcvAddrGet) (struct ifnet *, struct M2_IFINDEX_S *);
    BOOL              mibStyle;     /* RFC2233 supported if true */
    } M2_IFINDEX;

/* Structure to perform the set operation for if table */
    
typedef struct ifSetentry
    {
    unsigned int    varToSet;
    int             ifIndex;
    long            ifAdminStatus;
    UINT            ifPromiscuousMode;
    UINT            ifLinkUpDownTrapEnable;
    char            ifAlias[M2DISPLAYSTRSIZE];
    } IF_SETENTRY;

/* IP group bit fields that map to variables that can be set */

#define M2_IPFORWARDING		0x01
#define M2_IPDEFAULTTTL		0x02

typedef struct
    {
    long            ipForwarding;
    long            ipDefaultTTL;
    unsigned long   ipInReceives;
    unsigned long   ipInHdrErrors;
    unsigned long   ipInAddrErrors;
    unsigned long   ipForwDatagrams;
    unsigned long   ipInUnknownProtos;
    unsigned long   ipInDiscards;
    unsigned long   ipInDelivers;
    unsigned long   ipOutRequests;
    unsigned long   ipOutDiscards;
    unsigned long   ipOutNoRoutes;
    long            ipReasmTimeout;
    unsigned long   ipReasmReqds;
    unsigned long   ipReasmOKs;
    unsigned long   ipReasmFails;
    unsigned long   ipFragOKs;
    unsigned long   ipFragFails;
    unsigned long   ipFragCreates;
    unsigned long   ipRoutingDiscards;

    } M2_IP;


/* IP Address Table group */

typedef struct
    {
    unsigned long   ipAdEntAddr;
    long            ipAdEntIfIndex;
    unsigned long   ipAdEntNetMask;
    long            ipAdEntBcastAddr;
    long            ipAdEntReasmMaxSize;

    } M2_IPADDRTBL;

/* IP Routing Table group */

typedef struct
    {
    unsigned long   ipRouteDest;
    long            ipRouteIfIndex;
    long            ipRouteMetric1;
    long            ipRouteMetric2;
    long            ipRouteMetric3;
    long            ipRouteMetric4;
    unsigned long   ipRouteNextHop;
    long            ipRouteType;   
    long            ipRouteProto;
    long            ipRouteAge;
    unsigned long   ipRouteMask;
    long            ipRouteMetric5;
    M2_OBJECTID     ipRouteInfo;

    } M2_IPROUTETBL;

/* IP route table entry bit fields that map to variables that can be set */

#define M2_IP_ROUTE_DEST 		1
#define M2_IP_ROUTE_NEXT_HOP		2
#define M2_IP_ROUTE_TYPE		4
#define M2_IP_ROUTE_MASK                8

/* IP Address Translation Table group */

typedef struct
    {
    long            ipNetToMediaIfIndex;
    M2_PHYADDR 	    ipNetToMediaPhysAddress;
    unsigned long   ipNetToMediaNetAddress;
    long            ipNetToMediaType; 

    } M2_IPATRANSTBL;

/* ICMP group */

typedef struct
    {
    unsigned long   icmpInMsgs;
    unsigned long   icmpInErrors;
    unsigned long   icmpInDestUnreachs;
    unsigned long   icmpInTimeExcds;
    unsigned long   icmpInParmProbs;
    unsigned long   icmpInSrcQuenchs;
    unsigned long   icmpInRedirects;
    unsigned long   icmpInEchos;
    unsigned long   icmpInEchoReps;
    unsigned long   icmpInTimestamps;
    unsigned long   icmpInTimestampReps;
    unsigned long   icmpInAddrMasks;
    unsigned long   icmpInAddrMaskReps;
    unsigned long   icmpOutMsgs;
    unsigned long   icmpOutErrors;
    unsigned long   icmpOutDestUnreachs;
    unsigned long   icmpOutTimeExcds;
    unsigned long   icmpOutParmProbs;
    unsigned long   icmpOutSrcQuenchs;
    unsigned long   icmpOutRedirects;
    unsigned long   icmpOutEchos;
    unsigned long   icmpOutEchoReps;
    unsigned long   icmpOutTimestamps;
    unsigned long   icmpOutTimestampReps;
    unsigned long   icmpOutAddrMasks;
    unsigned long   icmpOutAddrMaskReps;

    } M2_ICMP;


/* TCP Group */

typedef struct 
    {
    long            tcpRtoAlgorithm;
    long            tcpRtoMin;
    long            tcpRtoMax;
    long            tcpMaxConn;
    unsigned long   tcpActiveOpens;
    unsigned long   tcpPassiveOpens;
    unsigned long   tcpAttemptFails;
    unsigned long   tcpEstabResets;
    unsigned long   tcpCurrEstab;
    unsigned long   tcpInSegs;
    unsigned long   tcpOutSegs;
    unsigned long   tcpRetransSegs;
    unsigned long   tcpInErrs;
    unsigned long   tcpOutRsts;

    } M2_TCPINFO;


/* TCP Connection Table Entry */

typedef struct
    {
    long            tcpConnState;
    unsigned long   tcpConnLocalAddress;
    long            tcpConnLocalPort;
    unsigned long   tcpConnRemAddress;
    long            tcpConnRemPort;

    } M2_TCPCONNTBL;


/* User Datagram Protocol Group */

typedef struct
    {
    unsigned long   udpInDatagrams;
    unsigned long   udpNoPorts;
    unsigned long   udpInErrors;
    unsigned long   udpOutDatagrams;

    } M2_UDP;

/* UDP Connection Table Entry */

typedef struct
    {
    unsigned long   udpLocalAddress;
    long            udpLocalPort;

    } M2_UDPTBL;
/* IGMP  */
typedef struct
    {
    ULONG       igmpInterfaceIfIndex; 
    ULONG       igmpInterfaceQueryInterval; 
    ULONG       igmpInterfaceStatus; 
    ULONG       igmpInterfaceVersion;
    ULONG       igmpInterfaceQuerier;
    ULONG       igmpInterfaceQueryMaxResponseTime;
    ULONG       igmpInterfaceQuerierPresentTimeout;
    ULONG       igmpInterfaceLeaveEnabled;
    ULONG       igmpInterfaceVersion1QuerierTimer;
    ULONG       igmpInterfaceWrongVersionQueries;
    ULONG       igmpInterfaceJoins;
    ULONG       igmpInterfaceLeaves;
    ULONG	igmpInterfaceGroups;
    ULONG       igmpInterfaceRobustness;
    ULONG       igmpInterfaceQuerierUpTime;
    ULONG       igmpInterfaceLastMembQueryIntvl;
    } M2_IGMP;
    
typedef struct 
    {
    ULONG       igmpCacheAddress;
    ULONG       igmpCacheIfIndex;
    ULONG       igmpCacheSelf;               /* was BOOLEAN */
    ULONG       igmpCacheLastReporter;
    ULONG       igmpCacheUpTime;
    ULONG       igmpCacheExpiryTime;
    ULONG       igmpCacheStatus;
    ULONG       igmpCacheVersion1HostTimer;
    } M2_IGMP_CACHE;
        

    
/* function declarations */
 
#if defined(__STDC__) || defined(__cplusplus)

extern STATUS m2SysInit (char *	mib2SysDescr, char * mib2SysContact,
		 char *	mib2SysLocation, M2_OBJECTID * pObjectId);

extern STATUS m2SysGroupInfoGet (M2_SYSTEM * pSysInfo);
extern STATUS m2SysGroupInfoSet (unsigned int varToSet, M2_SYSTEM * pSysInfo);
extern STATUS m2SysDelete (void);

extern M2_ID * m2IfAlloc (ULONG, UCHAR *, ULONG, ULONG, ULONG, char *, int);
extern STATUS  m2IfFree (M2_ID *);
extern STATUS  m2IfGenericPacketCount (M2_ID *, UINT, UCHAR *, ULONG);
extern STATUS  m2IfCounterUpdate (M2_ID *, UINT, ULONG);
extern STATUS  m2IfVariableUpdate (M2_ID *, UINT, caddr_t);
extern STATUS  m2IfPktCountRtnInstall (M2_ID *, M2_PKT_COUNT_RTN);
extern STATUS  m2IfCtrUpdateRtnInstall (M2_ID *, M2_CTR_UPDATE_RTN);
extern STATUS  m2IfVarUpdateRtnInstall (M2_ID *, M2_VAR_UPDATE_RTN);
extern STATUS  m2If8023PacketCount (M2_ID *, UINT, UCHAR *, ULONG);

extern STATUS m2IfInit (FUNCPTR pTrapRtn, void * pTrapArg);
extern STATUS m2IfTableUpdate (struct ifnet *, UINT,
		 int (*func) (struct socket*, u_long, caddr_t),
		 STATUS (func2) (struct ifnet*, M2_IFINDEX*));
extern STATUS m2IfGroupInfoGet (M2_INTERFACE * pIfInfo);
extern STATUS m2IfTblEntryGet (int search, void * pIfTblEntry);
extern STATUS m2IfTblEntrySet (void *);
extern STATUS m2IfStackTblUpdate (UINT lowerIndex, UINT higherIndex, int action);
extern STATUS m2IfStackEntryGet (int, int *,  M2_IFSTACKTBL *);
extern STATUS m2IfStackEntrySet (int, M2_IFSTACKTBL *);
extern STATUS m2IfRcvAddrEntryGet (int, int *, M2_IFRCVADDRTBL *);
extern STATUS m2IfRcvAddrEntrySet (int, int, M2_IFRCVADDRTBL *);
extern STATUS m2IfDelete (void);

extern STATUS m2IpInit (int maxRouteTableSize);
extern STATUS m2IpGroupInfoGet (M2_IP * pIpInfo);
extern STATUS m2IpGroupInfoSet (unsigned int varToSet, M2_IP * pIpInfo);

extern STATUS
	   m2IpAddrTblEntryGet (int search, M2_IPADDRTBL * pIpAddrTblEnry);

extern STATUS
m2IpRouteTblEntryGet (int   search, M2_IPROUTETBL * pIpRouteTblEntry);

extern STATUS
m2IpRouteTblEntrySet (int varToSet, M2_IPROUTETBL * pIpRouteTblEntry);

extern STATUS
    m2IpAtransTblEntryGet (int search, M2_IPATRANSTBL * pIpAtEntry);
extern STATUS
    m2IpAtransTblEntrySet (M2_IPATRANSTBL * pIpAtEntry);
extern STATUS m2IpDelete (void);

extern STATUS m2IcmpInit (void);
extern STATUS m2IcmpGroupInfoGet (M2_ICMP * pIcmpInfo);

extern STATUS m2TcpInit (void);
extern STATUS m2TcpGroupInfoGet (M2_TCPINFO * pTcpInfo);

extern STATUS m2TcpConnEntryGet (int search, M2_TCPCONNTBL * pTcpConnEntry);
extern STATUS m2TcpConnEntrySet (M2_TCPCONNTBL * pTcpConnEntry);

extern STATUS m2UdpInit (void);
extern STATUS m2UdpGroupInfoGet (M2_UDP * pUdpInfo);

extern STATUS m2UdpTblEntryGet (int search, M2_UDPTBL * pUdpEntry);
STATUS m2Init (char * mib2SysDescr, char * mib2SysContact, 
               char * mib2SysLocation, M2_OBJECTID * pMib2SysObjectId,
	       FUNCPTR pTrapRtn, void * pTrapArg, int maxRouteTableSize);
extern STATUS m2Delete (void);
extern STATUS m2TcpDelete (void);
extern STATUS m2UdpDelete (void);
extern STATUS m2IcmpDelete (void);
extern STATUS m2SetIfLastChange (int ifIndex);
extern STATUS m2IgmpInit ();
extern STATUS m2IgmpInterfaceEntrySet(M2_IGMP *data, int varsToSet);
extern STATUS m2IgmpInterfaceEntryGet(M2_IGMP *data);
extern STATUS m2IgmpInterfaceEntryNextGet(M2_IGMP *data);
extern STATUS m2IgmpCacheEntryGet(M2_IGMP_CACHE *data);
extern STATUS m2IgmpCacheEntryNextGet(M2_IGMP_CACHE *data);
extern M2_IGMP_CACHE * igmpCacheEntry_first(void);
extern M2_IGMP_CACHE * igmpCacheEntry_next_inst(M2_IGMP_CACHE * data);
extern unsigned igmpCacheEntry_build_instance(M2_IGMP_CACHE * data, int * inst);
extern STATUS m2IgmpTreeAdd ();

#else   /* __STDC__ */

extern STATUS m2SysInit ();
extern STATUS m2SysGroupInfoGet ();
extern STATUS m2SysGroupInfoSet ();
extern STATUS m2SysDelete ();
extern M2_ID * m2IfAlloc ();
extern STATUS m2IfFree ();
extern STATUS m2IfGenericPacketCount ();
extern STATUS m2IfCounterUpdate ();
extern STATUS m2IfVariableUpdate ();
extern STATUS m2IfPktCountRtnInstall ();
extern STATUS m2IfCtrUpdateRtnInstall ();
extern STATUS m2IfVarUpdateRtnInstall ();
extern STATUS m2If8023PacketCount ();
extern STATUS m2IfInit ();
extern STATUS m2IfTableUpdate ();
extern STATUS m2IfGroupInfoGet ();
extern STATUS m2IfTblEntryGet ();
extern STATUS m2IfTblEntrySet ();
extern STATUS m2IfStackTblUpdate ();
extern STATUS m2IfStackEntryGet ();
extern STATUS m2IfStackEntrySet ();
extern STATUS m2IfRcvAddrEntryGet ();
extern STATUS m2IfRcvAddrEntrySet ();
extern STATUS m2IfDelete ();
extern STATUS m2IpInit ();
extern STATUS m2IpGroupInfoGet ();
extern STATUS m2IpGroupInfoSet ();
extern STATUS m2IpAddrTblEntryGet ();
extern STATUS m2IpRouteTblEntryGet ();
extern STATUS m2IpRouteTblEntrySet ();
extern STATUS m2IpAtransTblEntryGet ();
extern STATUS m2IpAtransTblEntrySet ();
extern STATUS m2IpDelete ();
extern STATUS m2IcmpInit ();
extern STATUS m2IcmpGroupInfoGet ();
extern STATUS m2TcpInit ();
extern STATUS m2TcpGroupInfoGet ();
extern STATUS m2TcpConnEntryGet ();
extern STATUS m2TcpConnEntrySet ();
extern STATUS m2UdpInit ();
extern STATUS m2UdpGroupInfoGet ();
extern STATUS m2UdpTblEntryGet ();
extern STATUS m2Init ();
extern STATUS m2Delete ();
extern STATUS m2TcpDelete ();
extern STATUS m2UdpDelete ();
extern STATUS m2IcmpDelete ();
extern STATUS m2SetIfLastChange ();
extern STATUS m2IgmpInit ();
extern STATUS m2IgmpInterfaceEntrySet();
extern STATUS m2IgmpInterfaceEntryGet();
extern STATUS m2IgmpInterfaceEntryNextGet();
extern STATUS m2IgmpCacheEntryGet();
extern STATUS m2IgmpCacheEntryNextGet();
extern M2_IGMP_CACHE * igmpCacheEntry_first();
extern M2_IGMP_CACHE * igmpCacheEntry_next_inst();
extern unsigned igmpCacheEntry_build_instance();
extern STATUS m2IgmpTreeAdd ();
#endif  /* __STDC__ */

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

#ifdef __cplusplus
}
#endif

#endif /* __INCm2Libh */
