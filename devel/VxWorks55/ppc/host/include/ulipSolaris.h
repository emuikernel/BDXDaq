/* ulipSolaris.h - Ulip driver header	*/

/* Copyright 1996-1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01d,19aug97,cth  Changed MEDIUM_MTU from 1500 to 1024 (SPR 8640)
01c,21mar97,cth  Added FIOSETD,FIOSETUSED definitions, added isUsed field to 
		 ULIP structure (SPR 8221, 8240)
01b,29may96,ism  Added #include <sys/stream.h>
01a,26feb96, sgv Written
*/

#ifndef _INCulsolarish
#define _INCulsolarish

#include <sys/stream.h>

/* defines */

/* Ioctl command values for establishing loopback connection */

#define LOOP_SET (('l'<<8)|1)		/* Ioctl to set loopback connections */
#define FIOSETD  (('l'<<8)|2)		/* Ioctl to turn on/off debugging */
#define FIOSETUSED (('l'<<8)|4)		/* Ioctl to set the isUsed flag */

/* this flag is used by ulipInit during booting */

#define ISBOOTING	0x4000

#ifdef _KERNEL
#define STATIC static

#define MACSZ		6		/* size of ethernet address */
#define ETHERTYPE_IP    0x0800
#define MEDIUM_MTU	1024		/* max size of ulip packet */

/* the following defines are used for M_ERROR packet handling */

#define ERROR		-1	/* system call error return value */
#define OK		0	/* system call success return value */
#define ONE_BYTE	1	/* size of the M_ERROR message block */


#define ULIPIOC	('U'<<8)
#define SETADDR	(ULIPIOC|1)	/* ioctl cmd to set ulip interface addr */
#define GETADDR	(ULIPIOC|2)     /* ioctl cmd to get ulip interface addr */

#define DB_LIM(mp)      ((mp)->b_datap->db_lim)
#define DB_REF(mp)      ((mp)->b_datap->db_ref)
#define DB_TYPE(mp)     ((mp)->b_datap->db_type)
#define MBLKL(mp)       ((mp)->b_wptr - (mp)->b_rptr)
#define MBLKSIZE(mp)    ((mp)->b_datap->db_lim - (mp)->b_datap->db_base)

#define MEDIUMTYPE_MAX  (0xffff)        /* max valid medium type */

#define MEDIUM_ADDRL    MACSZ		/*  Medium address length */

#define NUM_LOOP	32		/* Number of loopback devices */

/* Full DLSAP address length */

#define DEVICE_ADDRL (sizeof (u_short) + MEDIUM_ADDRL)

/* per-stream flags */

#define SLRAW           0x02    /* M_DATA plain raw mode */

typedef struct 
    {
    int             	minorNum;	/* minor number */
    dev_info_t *	pDip;		/* pointer to dev_info structure */
    } ULIPDEV;

ULIPDEV ulipDev [NUM_LOOP];

typedef struct 
    {
    uchar_t dummy [MACSZ-2];
    u_short  addr;
    } ETHERADDR;

/* The ulip structure is used to maintain the queue pointers,states,flags
 * and packet statistics for each interface
 */

typedef struct 
    {
    queue_t *	pQptr;              /* Pointer to device own queue */
    queue_t *	pOqptr;             /* Pointer to loopback queue */
    int     	minorDev;           /* Minor number for the device */
    int		isUsed;		    /* Flag to check if dev is already used */
    u_long  	slState;            /* DLPI state maintained for device */
    u_long  	slSap;              /* Sap value bound for interface */
    u_long  	slFlags;            /* flags to store promiscous/multicast */
			            /* state */
    uchar_t 	ulipAddr [MACSZ];   /* MAC address for the interface */
    } ULIP ;

#endif /* _KERNEL */
#endif /* _INCulsolarish */
