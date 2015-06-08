/* common_subr.h - DHCP server include file for function prototypes */

/* Copyright 1984 - 2000 Wind River Systems, Inc. */

/*
modification history
____________________
01c,14jul00,spm  added support for non-Ethernet devices
01b,06aug97,spm  added alignment pragma and definitions for C++ compilation
01a,07apr97,spm  created by modifying WIDE project DHCP implementation
*/

/*
 * WIDE Project DHCP Implementation
 * Copyright (c) 1995 Akihiro Tominaga
 * Copyright (c) 1995 WIDE Project
 * All rights reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided only with the following
 * conditions are satisfied:
 *
 * 1. Both the copyright notice and this permission notice appear in
 *    all copies of the software, derivative works or modified versions,
 *    and any portions thereof, and that both notices appear in
 *    supporting documentation.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by WIDE Project and
 *      its contributors.
 * 3. Neither the name of WIDE Project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPER ``AS IS'' AND WIDE
 * PROJECT DISCLAIMS ANY LIABILITY OF ANY KIND FOR ANY DAMAGES
 * WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE. ALSO, THERE
 * IS NO WARRANTY IMPLIED OR OTHERWISE, NOR IS SUPPORT PROVIDED.
 *
 * Feedback of the results generated from any improvements or
 * extensions made to this software would be much appreciated.
 * Any such feedback should be sent to:
 * 
 *  Akihiro Tominaga
 *  WIDE Project
 *  Keio University, Endo 5322, Kanagawa, Japan
 *  (E-mail: dhcp-dist@wide.ad.jp)
 *
 * WIDE project has the rights to redistribute these changes.
 */

#ifndef __INCcommon_subrh
#define __INCcommon_subrh

#ifdef __cplusplus
extern "C" {
#endif

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

typedef struct dhcpEvent       /* External event descriptor. */
    {
    unsigned char source;       /* Offset of receiving interface in list. */
    int length;                 /* Length of incoming message. */
    }
EVENT_DATA;

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

#define EVENT_RING_SIZE (10 * sizeof (EVENT_DATA))

/* Tests for flags field of DHCP messages. */

#define ISBRDCST(X)   ((X & htons(0x8000)) != 0)
#define SETBRDCST(X)  ((X) |= htons(0x8000))

#ifndef sun
void     align_msg();
#endif
void     set_srvport();
int      check_ipsum();
int      check_udpsum();
u_short  udp_cksum();
u_short  get_ipsum();
u_short  get_udpsum();
u_short  cksum();
struct if_info *read_interfaces (struct if_info *, int *, int);

#ifdef __cplusplus
}
#endif

#endif /* __INCcommon_subrh */
