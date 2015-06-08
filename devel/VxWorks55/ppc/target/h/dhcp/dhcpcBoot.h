/* dhcpcBoot.h - DHCP client include file for boot-time internal usage */

/* Copyright 1984 - 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01g,05nov01,vvv  fixed compilation warning
01f,09oct01,rae  merge from truestack
01e,05apr00,spm  upgraded to RFC 2131 and removed direct link-level access;
                 removed entries shared with run-time client
01d,04dec97,spm  added code review modifications
01c,06oct97,spm  added support for dynamic driver type detection
01b,02sep97,spm  removed name conflicts with runtime DHCP client (SPR #9241)
01a,26aug97,spm  created to preserve single-lease interface at boot time
                 from existing include files - 01f of dhcpClientCommon.h, 
                 01d of dhcp.h, 01c of dhcpCommonSubr.h, and 01e of dhcpc.h
*/

#ifndef __INCdhcpcBooth
#define __INCdhcpcBooth

#ifdef __cplusplus
extern "C" {
#endif

#include "net/if.h"
#include "netinet/in.h"
#include "netinet/if_ether.h"

#include "dhcp/dhcpc.h"

/* Define sources for events. */

#define DHCP_AUTO_EVENT 0

/*  Define types of events. */

#define DHCP_MSG_ARRIVED 0
#define DHCP_TIMEOUT 1

/* Status of DHCP client. */

#define DHCP_MANUAL 0      /* IP address assigned manually. */
#define DHCP_BOOTP 1       /* Lease established using BOOTP reply. */
#define DHCP_NATIVE 2      /* Lease established using DHCP reply. */

/* DHCP boot-time client state definitions */

#define  INIT         0
#define  WAIT_OFFER   1
#define  SELECTING    2
#define  REQUESTING   3
#define  BOUND        4
#define  INFORMING    5

/* DHCP client message processing definitions */

#define CHKOFF(LEN)  (offopt + 2 + (LEN) < DFLTOPTLEN) /* check option offset */

#define WORD 		4

#define EVENT_RING_SIZE (10 * sizeof (EVENT_DATA))

IMPORT int 	dhcpcBindType; 	/* Type of DHCP lease, if any. */

IMPORT struct dhcp_param * 	dhcpcBootParam; /* Configuration parameters */

/* Mirror variables for values assigned externally when building image. */

IMPORT int dhcpcMinLease; 		/* Minimum lease length accepted. */
IMPORT int _dhcpcReadTaskPriority; 	/* Priority level of data retriever */
IMPORT int _dhcpcReadTaskOptions; 	/* Option settings of data retriever */
IMPORT int _dhcpcReadTaskStackSize; 	/* Stack size of data retriever */
IMPORT int dhcpcReadTaskId; 	/* Identifier for data retrieval task */

/*
 * Access point to start message exchanges for lease negotation, or for
 * additional configuration parameters if an address is already known.
 */

IMPORT int dhcp_boot_client (struct if_info *, int, int, BOOL);

#ifdef __cplusplus
}
#endif

#endif
