/* if_ring.h - network interface ring header file */

/* Copyright 1984-1992 Wind River Systems, Inc. */
/*
modification history
--------------------
02g,22sep92,rrr  added support for c++
02f,26may92,rrr  the tree shuffle
02e,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed copyright notice
02d,10jun91.del  added pragma for gnu960 alignment.
02c,05oct90,shl  added copyright notice.
                 made #endif ANSI style.
02b,20sep88,gae  added inclusion ifndef.
02a,03apr87,ecs  added header and copyright.
*/

#ifndef __INCif_ringh
#define __INCif_ringh

#ifdef __cplusplus
extern "C" {
#endif

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */

/*
 * The "ring" data structures.
 *  Contains some number of slots, and read & write indexes thereinto.
 *  Also a size field, specifying the number of slots allocated.
 */
struct ring {
	short		r_rdidx;	/* read index */
	short		r_wrtidx;	/* write index */
	short		r_size;		/* number of slots in ring */
	char		r_tas;		/* TAS multi-processor interlock flag */
	char		r_pad;		/* alignment pad */
	int		r_slot[1];	/* slots -- actually more than 1 */
};

struct ring32 {
	short		r_rdidx;	/* read index */
	short		r_wrtidx;	/* write index */
	short		r_size;		/* number of slots in ring */
	char		r_tas;		/* TAS multi-processor interlock flag */
	char		r_pad;		/* alignment pad */
	int		r_slot[32];	/* slots */
};

struct ring256 {
	short		r_rdidx;	/* read index */
	short		r_wrtidx;	/* write index */
	short		r_size;		/* number of slots in ring */
	char		r_tas;		/* TAS multi-processor interlock flag */
	char		r_pad;		/* alignment pad */
	int		r_slot[256];	/* slots */
};

struct bcb {
	struct bcb	*b_link;
	short		b_stat;
	short		b_len;
	char		*b_addr;
	short		b_msglen;
	short		b_reserved;
};

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 0                 /* turn off alignment requirement */
#endif	/* CPU_FAMILY==I960 */

#ifdef __cplusplus
}
#endif

#endif /* __INCif_ringh */
