/* if_types.h - interface types header file */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
 * Copyright (c) 1989, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)if_types.h	8.2 (Berkeley) 4/20/94
 */

/*
modification history
--------------------
01d,16may02,vvv  updated interface types (SPR #76443)
01c,15aug01,vvv  added entry for IFT_PMP; updated IFT_MAX_TYPE (SPR #69546)
01b,09sep98,ham  added IFT_MAX_TYPE
01a,03mar96,vin  created from BSD4.4lite2.
*/

#ifndef __INCif_typesh
#define __INCif_typesh

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Interface types for benefit of parsing media address headers.
 * This list is derived from the SNMP list of ifTypes, currently
 * documented in RFC1573.
 */

#define	IFT_OTHER	0x1		/* none of the following */
#define	IFT_1822	0x2		/* old-style arpanet imp */
#define	IFT_HDH1822	0x3		/* HDH arpanet imp */
#define	IFT_X25DDN	0x4		/* x25 to imp */
#define	IFT_X25		0x5		/* PDN X25 interface (RFC877) */
#define	IFT_ETHER	0x6		/* Ethernet CSMACD */
#define	IFT_ISO88023	0x7		/* CMSA CD */
#define	IFT_ISO88024	0x8		/* Token Bus */
#define	IFT_ISO88025	0x9		/* Token Ring */
#define	IFT_ISO88026	0xa		/* MAN */
#define	IFT_STARLAN	0xb
#define	IFT_P10		0xc		/* Proteon 10MBit ring */
#define	IFT_P80		0xd		/* Proteon 10MBit ring */
#define	IFT_HY		0xe		/* Hyperchannel */
#define	IFT_FDDI	0xf
#define	IFT_LAPB	0x10
#define	IFT_SDLC	0x11
#define	IFT_T1		0x12
#define	IFT_CEPT	0x13		/* E1 - european T1 */
#define	IFT_ISDNBASIC	0x14
#define	IFT_ISDNPRIMARY	0x15
#define	IFT_PTPSERIAL	0x16		/* Proprietary PTP serial */
#define	IFT_PPP		0x17		/* RFC 1331 */
#define	IFT_LOOP	0x18		/* loopback */
#define	IFT_EON		0x19		/* ISO over IP */
#define	IFT_XETHER	0x1a		/* obsolete 3MB experimental ethernet */
#define	IFT_NSIP	0x1b		/* XNS over IP */
#define	IFT_SLIP	0x1c		/* IP over generic TTY */
#define	IFT_ULTRA	0x1d		/* Ultra Technologies */
#define	IFT_DS3		0x1e		/* Generic T3 */
#define	IFT_SIP		0x1f		/* SMDS */
#define	IFT_FRELAY	0x20		/* Frame Relay DTE only */
#define	IFT_RS232	0x21
#define	IFT_PARA	0x22		/* parallel-port */
#define	IFT_ARCNET	0x23
#define	IFT_ARCNETPLUS	0x24
#define	IFT_ATM		0x25		/* ATM cells */
#define	IFT_MIOX25	0x26
#define	IFT_SONET	0x27		/* SONET or SDH */
#define	IFT_X25PLE	0x28
#define	IFT_ISO88022LLC	0x29
#define	IFT_LOCALTALK	0x2a
#define	IFT_SMDSDXI	0x2b
#define	IFT_FRELAYDCE	0x2c		/* Frame Relay DCE */
#define	IFT_V35		0x2d
#define	IFT_HSSI	0x2e
#define	IFT_HIPPI	0x2f
#define	IFT_MODEM	0x30		/* Generic Modem */
#define	IFT_AAL5	0x31		/* AAL5 over ATM */
#define	IFT_SONETPATH	0x32
#define	IFT_SONETVT	0x33
#define	IFT_SMDSICIP	0x34		/* SMDS InterCarrier Interface */
#define	IFT_PROPVIRTUAL	0x35		/* Proprietary Virtual/internal */
#define	IFT_PROPMUX	0x36		/* Proprietary Multiplexing */
#define IFT_IEEE80212                0x37
#define IFT_FIBRE_CHANNEL            0x38
#define IFT_HIPPI_INTERFACE          0x39
#define IFT_FRAME_RELAY_INTERCONNECT 0x3a
#define IFT_AFLANE8023               0x3b
#define IFT_AFLANE8025               0x3c
#define IFT_CCT_EMUL                 0x3d
#define IFT_FAST_ETHER               0x3e
#define IFT_ISDN                     0x3f
#define IFT_V11                      0x40
#define IFT_V36                      0x41
#define IFT_G703AT64K                0x42
#define IFT_G703AT2MB                0x43
#define IFT_QLLC                     0x44
#define IFT_FAST_ETHER_FX            0x45
#define IFT_CHANNEL                  0x46
#define IFT_IEEE80211                0x47
#define IFT_IBM370PAR_CHAN           0x48
#define IFT_ESCON                    0x49
#define IFT_DLSW                     0x4a
#define IFT_ISDNS                    0x4b
#define IFT_ISDNU                    0x4c
#define IFT_LAPD                     0x4d
#define IFT_IP_SWITCH                0x4e
#define IFT_RSRB                     0x4f
#define IFT_ATM_LOGICAL              0x50
#define IFT_DS0                      0x51
#define IFT_DS0_BUNDLE               0x52
#define IFT_BSC                      0x53
#define IFT_ASYNC                    0x54
#define IFT_CNR                      0x55
#define IFT_ISO88025_DTR             0x56
#define IFT_EPLRS                    0x57
#define IFT_ARAP                     0x58
#define IFT_PROP_CNLS                0x59
#define IFT_HOST_PAD                 0x5a
#define IFT_TERM_PAD                 0x5b
#define IFT_FRAME_RELAY_MPI          0x5c
#define IFT_X213                     0x5d
#define IFT_ADSL                     0x5e
#define IFT_RADSL                    0x5f
#define IFT_SDSL                     0x60
#define IFT_VDSL                     0x61
#define IFT_ISO88025_CRFP_INT        0x62
#define IFT_MYRINET                  0x63
#define IFT_VOICE_EM                 0x64
#define IFT_VOICE_FXO                0x65
#define IFT_VOICE_FXS                0x66
#define IFT_VOICE_ENCAP              0x67
#define IFT_VOICE_OVER_IP            0x68
#define IFT_ATM_DXI                  0x69
#define IFT_ATM_FUNI                 0x6a
#define IFT_ATM_IMA                  0x6b
#define IFT_PPP_MULTILINK_BUNDLE     0x6c
#define IFT_IP_OVER_CDLC             0x6d
#define IFT_IP_OVER_CLAW             0x6e
#define IFT_STACK_TO_STACK           0x6f
#define IFT_VIRTUAL_IP_ADDRESS       0x70
#define IFT_MPC                      0x71
#define IFT_IP_OVER_ATM              0x72
#define IFT_ISO88025_FIBER           0x73
#define IFT_TDLC                     0x74
#define IFT_GIGABIT_ETHERNET         0x75
#define IFT_HDLC                     0x76
#define IFT_LAPF                     0x77
#define IFT_V37                      0x78
#define IFT_X25MLP                   0x79
#define IFT_X25HUNT_GROUP            0x7a
#define IFT_TRASNP_HDLC              0x7b
#define IFT_INTERLEAVE               0x7c
#define IFT_FAST                     0x7d
#define IFT_IP                       0x7e
#define IFT_DOCS_CABLE_MACLAYER      0x7f
#define IFT_DOCS_CABLE_DOWNSTREAM    0x80
#define IFT_DOCS_CABLE_UPSTREAM      0x81
#define IFT_A12_MPP_SWITCH           0x82
#define IFT_TUNNEL                   0x83
#define IFT_COFFEE                   0x84
#define IFT_CES                      0x85
#define IFT_ATM_SUB_INTERFACE        0x86
#define IFT_L2VLAN                   0x87
#define IFT_L3IPVLAN                 0x88
#define IFT_L3IPXVLAN                0x89
#define IFT_DIGITAL_POWERLINE        0x8a
#define IFT_MEDIA_MAIL_OVER_IP       0x8b
#define IFT_DTM                      0x8c
#define IFT_DCN                      0x8d
#define IFT_IP_FORWARD               0x8e
#define IFT_MSDSL                    0x8f
#define IFT_IEEE1394                 0x90
#define IFT_IF_GSN                   0x91
#define IFT_DVB_RCC_MAC_LAYER        0x92
#define IFT_DVB_RCC_DOWNSTREAM       0x93
#define IFT_DVB_RCC_UPSTREAM         0x94
#define IFT_ATM_VIRTUAL              0x95
#define IFT_MPLS_TUNNEL              0x96
#define IFT_SRP                      0x97
#define IFT_VOICE_OVER_ATM           0x98
#define IFT_VOICE_OVER_FRAME_RELAY   0x99
#define IFT_IDSL                     0x9a
#define IFT_COMPOSITE_LINK           0x9b
#define IFT_SS7_SIG_LINK             0x9c
#define IFT_PMP                      0x9d     /* Point to Multipoint */
#define IFT_MAX_TYPE                 IFT_PMP  /* the maximum ifType number */
#ifdef __cplusplus
}
#endif

#endif /* __INCif_typesh */
