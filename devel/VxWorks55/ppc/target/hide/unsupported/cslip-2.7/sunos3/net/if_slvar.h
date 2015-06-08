/*
 * Definitions for SLIP interface data structures
 *
 * (this exists so programs like slstats can get at the definition
 *  of sl_softc.)
 *
 * @(#) $Header: if_slvar.h,v 1.9 93/09/04 21:14:05 leres Exp $ (LBL)
 *
 * Copyright (c) 1989, 1990, 1992, 1993 Regents of the University of
 * California. All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	Van Jacobson (van@ee.lbl.gov), Dec 31, 1989:
 *	- Initial distribution.
 */
struct sl_softc {
	struct ifnet sc_if;	/* network-visible interface */
	struct ifqueue sc_fastq; /* interactive output queue */
	struct tty *sc_ttyp;	/* pointer to tty structure */
	struct mbuf *sc_mbuf;	/* pointer to output mbuf chain */
	u_long sc_moff;		/* saved m_off from first mbuf */
	u_char *sc_mp;		/* pointer to next available buffer char */
	u_char *sc_ep;		/* pointer to last available buffer char */
	u_int sc_escape;	/* =1 if last char input was FRAME_ESCAPE */
	u_int sc_bytessent;
	u_int sc_bytesrcvd;
	struct slcompress sc_comp; /* tcp compression data */
	caddr_t sc_bpf;		/* bpf magic cookie */
};

/*
 * There are three per-line options kept in the device specific part of
 * the interface flags word:  IFF_LINK0 enables compression; IFF_LINK1
 * enables compression if a compressed packet is received from the
 * other side; IFF_LINK2 will drop (not send) ICMP packets.
 */
#ifndef IFF_LINK0
/*
 * This system doesn't have defines for device specific interface flags,
 * Define them.
 */
#define IFF_LINK0	0x8000
#define IFF_LINK1	0x4000
#define IFF_LINK2	0x2000
#endif
