/* rip2.h - Defines specifically for RIP version 2 */

/* Copyright 1984 - 2001 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,28apr02,rae  added extern C jazz (SPR #76303)
01e,12oct01,rae  merge from truestack (authentication)
01d,15may97,gnn added another new define.
01c,08may97,gnn added a new define.
01b,24feb97,gnn made rip_2_pkt look more logical.
01a,14nov96,gnn	written.

*/

#ifndef __INCrip2h
#define __INCrip2h

#ifdef __cplusplus
extern "C" {
#endif
 
/*
DESCRIPTION

INCLUDE FILES:
*/

/* includes */

/* defints */
#define RIP2_AUTH_LEN 16
#define RIP2_AUTH 0xffff
#define RIP2_DEST_OFFSET 2
#define RIP2_GATE_OFFSET 10
#define RIP2_MASK_OFFSET 6

/* typedefs */

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

typedef struct rip_2_pkt
    {
    UINT16 family;
    UINT16 tag;
    UINT32 dest;
    UINT32 subnet;
    UINT32 gateway;
    UINT32 metric;
    } RIP2PKT;

typedef struct rip2AuthPktHdr
    {
    UINT16 authIdent;        /* authentication identifier (always 0xff) */
    UINT16 authType;         /* the authentication type */
    UINT16 packetLen;        /* the offset (in bytes) to the digest */
    UINT8  keyId;            /* key identifier used to compute digest */
    UINT8  authDataLen;      /* length of the digest */
    UINT32 sequence;         /* sequence number of the packet */
    UINT32 zero1;            /* zeroed out */
    UINT32 zero2;            /* zeroed out */
    } RIP2_AUTH_PKT_HDR;

typedef struct rip2AuthPktTrl
    {
    UINT16 authIdent;        /* authentication identifier (always 0xff) */
    UINT16 authTag;          /* the authentication type/tag (always 0x01) */
    UINT8  authDigest [AUTH_KEY_LEN];  /* the computed packet digest */
    } RIP2_AUTH_PKT_TRL;

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif  /* CPU_FAMILY==I960 */

/* globals */

/* locals */

/* forward declarations */

#ifdef __cplusplus
}
#endif
#endif /* __INCrip2h */
