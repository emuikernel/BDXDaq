/* if_fn.c - Fujitsu MB86960 NICE Ethernet network interface driver */

/* Copyright 1992-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01v,15jul97,spm  added ARP request to SIOCSIFADDR ioctl handler
01u,07apr97,spm  code cleanup, corrected statistics, and upgraded to BSD 4.4
01t,02mar95,jdi  fixed line break in fnattach() title.
01s,05jul94,vin  removed spl calls, fixed collision control
01r,10may94,vin  fixed input parameter passed to etherInputHook
01q,04mar94,vin  added input & output hooks, removed explicit copying from
		 mbufs in the fnOutput(). Alternate scheme to detect 
		 transmit done implemented which negates the requirement for
		 processing the transmit done interrupt. 
01p,07jul93,rfs  Fixed SPR #2374.
01o,11aug93,jmm  Changed ioctl.h and socket.h to sys/ioctl.h and sys/socket.h
01n,19feb93,jdi  documentation cleanup.
01m,22oct92,rfs  Fixed SPR #1710.
01l,13oct92,rfs  Added documentation.
01k,02oct92,rfs  Made multiple attach calls return OK, not ERROR.
01j,09sep92,gae  documentation tweaks.
01i,31aug92,rfs  Free MBUFs on output if flags down.
01h,16jul92,rfs  Consolidated attach() and init().
01g,11jul92,jwt  incorporated JDI documentation cleanup; SPARC #ifdefs.
01f,16jun92,jwt  passed through the ansification filter
                 -changed includes to have absolute path from h/
                 -fixed #else and #endif
                 -changed copyright notice
                 -fixed path names for #include files.
01e,14may92,rfs  Added semaphore for transmitter access. Added one-time-only
                 calling of the attach() and init() routines.  Reduced size
                 of SRAM buffer.
01d,14apr92,rfs  Changed Rx to only look at "good packet" status bit.
                 Changed all splnet() to splimp().
01c,25mar92,jwt  enhanced interrupt and cache support; cleanup.
01b,24mar92,rfs  Moved sysEnet* () support routines to the sysLib.
                 Changed the attach and init routines to support new
                 method of obtaining board specific parameters.
                 Changed name of the NICE device header file.
                 Changed the name of the support routines called to access
                 ASI space.
01a,10mar92,rfs  Created from mule driver source.
*/

/*
This module implements the Fujitsu MB86960 NICE Ethernet network interface
driver.

This driver is non-generic and has only been run on the Fujitsu SPARClite
Evaluation Board.  It currently supports only unit number zero.
The driver must be given several target-specific parameters, and some external
support routines must be provided.  These parameters, and the mechanisms used
to communicate them to the driver, are detailed below.

BOARD LAYOUT
This device is on-board.  No jumpering diagram is necessary.

EXTERNAL INTERFACE
This driver provides the standard external interface with the following
exceptions.  All initialization is performed within the attach routine;
there is no separate initialization routine.  Therefore, in the global interface
structure, the function pointer to the initialization routine is NULL.

The only user-callable routine is fnattach(), which publishes the `fn'
interface and initializes the driver and device.

TARGET-SPECIFIC PARAMETERS
External support routines provide all parameters:
.iP "device I/O address"
This parameter specifies the base address of the device's I/O register
set.  This address is assumed to live in SPARClite alternate address
space.
.iP "interrupt vector"
This parameter specifies the interrupt vector to be used by the driver
to service an interrupt from the NICE device.  The driver will connect
the interrupt handler to this vector by calling intConnect().
.iP "Ethernet address"
This parameter specifies the unique, six-byte address assigned to the
VxWorks target on the Ethernet.
.LP

EXTERNAL SUPPORT REQUIREMENTS
This driver requires five external support functions:
.iP "char *sysEnetIOAddrGet (int unit)" "" 9 -1
This routine returns the base address of the NICE
control registers.  The driver calls this routine 
once, using fnattach().
.iP "int sysEnetVectGet (int unit)"
This routine returns the interrupt vector number
to be used to connect the driver's interrupt handler.
The driver calls this routine once, using fnattach().
.iP "STATUS sysEnetAddrGet (int unit, char *pCopy)"
This routine provides the six-byte Ethernet address
used by <unit>.  It must copy the six-byte
address to the space provided by <pCopy>.  It 
returns OK, or ERROR if it fails.
The driver calls this routine once, using fnattach().
.iP "void sysEnetIntEnable (int unit), void sysEnetIntDisable (int unit)"
These routines enable or disable the interrupt from the NICE for the
specified <unit>.  Typically, this involves interrupt controller hardware,
either internal or external to the CPU.  The driver calls these 
routines only during initialization, using fnattach().
.LP

SYSTEM RESOURCE USAGE
When implemented, this driver requires the following system resources:

    - one mutual exclusion semaphore
    - one interrupt vector
    - 3944 bytes in text section (text)
    - 0 bytes in the initialized data section (data)
    - 3152 bytes in the uninitialized data section (BSS)

The above data and BSS requirements are for the SPARClite architecture and may
vary for other architectures.  Code size (text) varies greatly between
architectures and is therefore not quoted here.

The NICE device maintains a private buffer for all packets transmitted and
received.  Therefore, the driver does not require any system memory to share
with the device.  This also eliminates all data cache coherency issues.

SEE ALSO: ifLib
*/

/* includes */

#include "vxWorks.h"            /* direct references */
#include "sys/types.h"          /* direct references */
#include "sys/ioctl.h"          /* direct references */
#include "sys/socket.h"		/* direct references */
#include "net/mbuf.h"           /* direct references */
#include "net/if.h"             /* direct and indirect references */
#include "net/if_subr.h"
#include "netinet/in.h"         /* indirect references only */
#include "netinet/in_systm.h"   /* indirect references only */
#include "netinet/in_var.h"     /* direct references */
#include "netinet/if_ether.h"   /* direct references */
#include "iv.h"                 /* direct references */
#include "errno.h"              /* direct references */
#include "semLib.h"             /* direct references */
#include "intLib.h"
#include "logLib.h"
#include "netLib.h"
#include "etherLib.h"
#include "stdio.h"
#include "drv/netif/if_fn.h"    /* device description */

/***** COMPILATION SWITCHES *****/

#undef FN_DEBUG


/* defines */

#define MAX_UNITS		1	/* maximum units supported */ 
#define DRV_CTRL_SIZ		sizeof (DRV_CTRL)
#define ENET_HDR_REAL_SIZ	14

/* Pre-defined interrupt masks */

#define NORM_INTRMASK   	(DLCR3_RX_PKT | DLCR3_OVR_FLO) 
#define NO_RX_INTRMASK  	(DLCR3_OVR_FLO)

/* Start transmission */

#define KICK_TRANSMIT		(BMR10_TMST | BMR11_MASK16 | BMR11_RST_TX16 \
				 | BMR11_OPT_16_COLL | 0x0100) 

/* typedefs */

typedef struct arpcom IDR;

typedef struct nice_ctrl	/* Driver control structure */
    {
    IDR idr; 			/* interface data record */
    BOOL attached;		/* indicates unit is attached */
    char *pDev;			/* ptr to the device registers */
    int vector;			/* vector number to use */
#ifdef BSD43_DRIVER
    SEM_ID TxSem;		/* transmitter semaphore */
#endif
    } DRV_CTRL;

/* shorthand for struct members */

#define fn_if 		idr.ac_if
#define fn_enaddr 	idr.ac_enaddr

/* Debug support */

#ifdef FN_DEBUG
#define DEBUG_SPEC  (fnDebug & 0x01)        /* debug special routines */
#define DEBUG_TX    (fnDebug & 0x02)        /* debug transmit routines */
#define DEBUG_RX    (fnDebug & 0x04)        /* debug receive routines */
#define DEBUG_INTR  (fnDebug & 0x08)        /* debug interrupt routines */
#endif /* FN_DEBUG */

/* globals */

#ifdef FN_DEBUG
int fnDebug;                    /* set this from shell to select debugging */
#endif /* FN_DEBUG */


/* External functions not defined in any header files.
 * Some of these are the functions required of the BSP modules.
 */

extern char *   sysEnetIOAddrGet ();
extern int      sysEnetVectGet ();
extern STATUS   sysEnetAddrGet ();
extern STATUS   sysEnetIntEnable ();
extern STATUS   sysEnetIntDisable ();
extern STATUS   sysEnetIntAck ();
extern BOOL	arpresolve ();

/* External functions that access the alternate address space that the
 * device lives in.
 */

#if (CPU_FAMILY == SPARC)
extern u_short      sysAsiGeth();
extern void         sysAsiSeth();
#endif

/* locals */

/* driver control structures, one per unit */ 

LOCAL DRV_CTRL	drvCtrl [MAX_UNITS];  

LOCAL int	overFlowCnt;	/* overFlow Count */
LOCAL int	rcvPktError;	/* receive packet error */

/* forward declarations */

LOCAL int	fnIoctl (struct ifnet *	pIfnet, int cmd, char *	pArgs);
LOCAL int	fnReset (int unit);
#ifdef BSD43_DRIVER
LOCAL int	fnOutput (struct arpcom * pIDR, struct mbuf * pMbuf,
			  struct sockaddr * pDest);
#else
LOCAL int       fnTxStartup (DRV_CTRL *pDrvCtrl);
#endif
LOCAL void	fnRxEvent (DRV_CTRL *pDrvCtrl);
LOCAL BOOL	fnRxMore (DRV_CTRL *pDrvCtrl);
LOCAL STATUS	fnRxProcessPkt (DRV_CTRL *pDrvCtrl);
LOCAL void	fnIntr (int unit);
#ifdef BSD43_DRIVER
LOCAL BOOL	convertDestAddr (IDR *pIdr, struct sockaddr * pDestSktAddr, 
				 char *	pDestEnetAddr, u_short * pPacketType,
				 struct mbuf * pMbuf);
#endif

/*******************************************************************************
*
* fnattach - publish the `fn' network interface and initialize the driver and device
*
* The routine publishes the `fn' interface by filling in a network interface
* record and adding this record to the system list.  This routine also
* initializes the driver and the device to the operational state.
*
* RETURNS: OK or ERROR.
*/

STATUS fnattach
    (
    int		unit	/* unit number */
    )
    {
    char *	pDev;		/* ptr to the device */
    DRV_CTRL * 	pDrvCtrl;

    /* Sanity check the unit number */

    if (unit < 0 || unit >= MAX_UNITS)
        return (ERROR);

    /* Ensure single invocation per system life */

    pDrvCtrl = & drvCtrl [unit];
    if (pDrvCtrl->attached)
        return (OK);

    /* Publish our existence as an available interface */

#ifdef BSD43_DRIVER
    ether_attach (&pDrvCtrl->fn_if, unit, "fn", NULL, fnIoctl, 
                  fnOutput, fnReset);
#else
    ether_attach (
                 &pDrvCtrl->fn_if, 
                 unit,
                 "fn",
                 (FUNCPTR) NULL,
                 (FUNCPTR) fnIoctl,
                 (FUNCPTR) ether_output,
                 (FUNCPTR) fnReset
                 );
    pDrvCtrl->fn_if.if_start = (FUNCPTR)fnTxStartup;
#endif

	{ /*   Driver initialization    */

	/* First obtain the board and unit specific info from the BSP. */

	pDev = sysEnetIOAddrGet (unit);
	if (pDev == NULL)
	    {
	    printf ("fn: error obtaining IO addr\n");
	    return (ERROR);
	    }
	pDrvCtrl->pDev = pDev;                  /* save ptr in ctrl struct */

	pDrvCtrl->vector = sysEnetVectGet (unit);
	if (pDrvCtrl->vector == NULL)
	    {
	    printf ("fn: error obtaining interrupt vector\n");
	    return (ERROR);
	    }

	if (sysEnetAddrGet (unit, pDrvCtrl->fn_enaddr) == ERROR)
	    {
	    printf ("fn: error obtaining Ethernet address\n");
	    return (ERROR);
	    }

#ifdef BSD43_DRIVER
	pDrvCtrl->TxSem = semMCreate    (
					 SEM_Q_PRIORITY |
					 SEM_DELETE_SAFE |
					 SEM_INVERSION_SAFE
					);

	if (pDrvCtrl->TxSem == NULL)
	    {
	    printf ("fn: error creating transmitter semaphore\n");
	    return (ERROR);
	    }
#endif

	/* Connect the interrupt routine. */

	intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC (pDrvCtrl->vector), fnIntr, 
		    unit);
	
	} /* End block */

	{ /*    device initialization    */

	/* Block local variables */

	u_short temp;                               /* scratch pad */

	/* initial setup of config register 1 */

#if (CPU_FAMILY == SPARC)
	sysAsiSeth  (pDev + NICE_CONFIG,
		     DLCR6_DISABLE_DLC   |           /* resets things */
		     DLCR6_BIT_6         |           /* just do it */
		     DLCR6_SYS_BUS_16    |           /* use 16 bits */
		     DLCR6_BUF_BUS_16    |           /* use 16 bits */
		     DLCR6_TBS_4KB       |           /* Tx buf is 4kb */
		     DLCR6_BS_16KB       |           /* total buf is 16k */
		     DLCR7_CNF_NICE      |           /* normal NICE mode */
		     DLCR7_PWRDN_OFF     |           /* normal power mode */
		     DLCR7_BIT_4         |           /* just do it */
		     DLCR7_REG_BNK_DLC   |           /* map node ID regs */
		     DLCR7_ENDIAN_LITTLE             /* device is little */
		     );
#endif

	/* Mask off all interrupts and clear all event flags. */
	
#if (CPU_FAMILY == SPARC)
	sysAsiSeth (pDev + NICE_INTRMASK, 0);
	sysAsiSeth (pDev + NICE_STATUS, 0xffff);
#endif

	/* Disable device interrupt at system level and clear any pended. */
	
	sysEnetIntDisable (unit);
	
	/* The secondary DLC register group is mapped in.  Set the node ID. */

#if (CPU_FAMILY == SPARC)
	sysAsiSeth (pDev + NICE_ADDR1, ((u_short *) pDrvCtrl->fn_enaddr) [0]);
	sysAsiSeth (pDev + NICE_ADDR2, ((u_short *) pDrvCtrl->fn_enaddr) [1]);
	sysAsiSeth (pDev + NICE_ADDR3, ((u_short *) pDrvCtrl->fn_enaddr) [2]);
#endif

	/* Change register map. Select buffer memory register group. */

#if (CPU_FAMILY == SPARC)
	temp = sysAsiGeth (pDev + NICE_CONFIG);
#endif

	temp &= ~DLCR7_REG_BNK;		/* clear bank select bits */
	temp |= DLCR7_REG_BNK_BMR;	/* add desired bits */

#if (CPU_FAMILY == SPARC)
	sysAsiSeth (pDev + NICE_CONFIG, temp);
#endif

	/* Setup the receiver and transmitter modes. */

#if (CPU_FAMILY == SPARC)
	sysAsiSeth (pDev + NICE_MODE, DLCR4_LBC | DLCR5_BIT_2 | DLCR5_AF0);
#endif

	/* Do the board specific interrupt enable for the device. */

	sysEnetIntEnable (unit);

	/* Enable specific interrupts. */

#if (CPU_FAMILY == SPARC)
	sysAsiSeth (pDev + NICE_INTRMASK, NORM_INTRMASK);
#endif

	/* Enable the DLC. */

#if (CPU_FAMILY == SPARC)
	temp = sysAsiGeth (pDev + NICE_CONFIG);     /* get config reg */
#endif

	temp &= ~DLCR6_DISABLE_DLC;                 /* clear the disable bit */

#if (CPU_FAMILY == SPARC)
	sysAsiSeth (pDev + NICE_CONFIG, temp);      /* update config reg */

	/* set collision control drop packet after 16 collisions */

	sysAsiSeth (pDev + NICE_TRANSMIT, (BMR11_MASK16 | BMR11_RST_TX16
					   | BMR11_OPT_16_COLL));
#endif
	} /* End block */

    /* Set our interface flags. */

    pDrvCtrl->idr.ac_if.if_flags |= (IFF_UP | IFF_RUNNING | IFF_NOTRAILERS);

    /* That's all */

    pDrvCtrl->attached = TRUE;
    return (OK);

    } /* End of fnattach() */

/*******************************************************************************
*
* fnIoctl - the driver I/O control routine
*
* This routine will process an ioctl request 
*
* RETURNS: 0 or errno
*/

LOCAL int fnIoctl 
    (
    struct ifnet *	pIfnet,		/* ptr to our interface struct */
    int			cmd,		/* the command code */
    char *		pArgs		/* ptr to the command arguments */
    )
    {
    int		errno;		/* hold error value or zero */
    int  	unit = pIfnet->if_unit;
    DRV_CTRL * 	pDrvCtrl = &drvCtrl[unit];

#ifdef FN_DEBUG
    if (DEBUG_SPEC)
        printf  (
                "fn: ioctl: pIfnet=%x  cmd=%x  pArgs=%x\n",
                pIfnet, cmd, pArgs
                );
#endif /* FN_DEBUG */

    errno = 0;                              /* assume no error */

    switch (cmd)
        {
        case SIOCSIFADDR:                   /* set i/f address */

#ifdef FN_DEBUG
            if (DEBUG_SPEC)
                printf ("fn: ioctl: set addr\n");
#endif /* FN_DEBUG */

            /* copy the INET address into our arpcom structure */

            pDrvCtrl->idr.ac_ipaddr = IA_SIN (pArgs)->sin_addr;
            arpwhohas (pIfnet, &IA_SIN (pArgs)->sin_addr);

            break;

        case SIOCGIFADDR:                   /* get i/f address */

#ifdef FN_DEBUG
            if (DEBUG_SPEC)
                printf ("fn: ioctl: get addr\n");
#endif /* FN_DEBUG */

            bcopy   (
                    (caddr_t) pDrvCtrl->fn_enaddr,
                    (caddr_t) ((struct ifreq *)pArgs)->ifr_addr.sa_data,
                    6
                    );
            break;

        case SIOCSIFFLAGS:                  /* set i/f flags */

#ifdef FN_DEBUG
            if (DEBUG_SPEC)
                printf ("fn: ioctl: set flags\n");
#endif /* FN_DEBUG */

            break;

        case SIOCGIFFLAGS:                  /* get i/f flags */

#ifdef FN_DEBUG
            if (DEBUG_SPEC)
                printf ("fn: ioctl: get flags\n");
#endif /* FN_DEBUG */

            *(short *)pArgs = pIfnet->if_flags;
            break;

        default :
            errno = EINVAL;
        }

    return (errno);

    } /* End of fnIoctl() */

/*******************************************************************************
*
* fnReset - the driver reset routine
*
* This routine resets the device
*
* RETURNS: OK.
*/

LOCAL int fnReset 
    (
    int		unit
    )
    {

#ifdef FN_DEBUG
    if (DEBUG_SPEC)
        printf ("fnReset\n");
#endif /* FN_DEBUG */

    return (OK);

    } /* End of fnReset() */

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* fnOutput - The driver's output routine
*
* This routine is responsible for tranmitting packets over physical media.
* The destination address is passed to this function as a pointer to a
* generic socket address structure.  This address information is first
* converted to the six byte destination address and type field that we
* need to build a proper Ethernet frame.
*
* The data to be sent out is held in a chain of mbufs.  This function is
* passed a pointer to the head mbuf.  The data in each mbuf fragment must
* be copied to the device's packet buffer.
*
* RETURNS: OK, or ERROR
*/

LOCAL int fnOutput 
    (
    struct arpcom *	pIDR,		/* ptr to our interface struct */
    struct mbuf *	pMbuf,		/* ptr to the mbufs to send */
    struct sockaddr *	pDest		/* ptr to dest socket addr */
    )
    {
    char 	destEnetAddr [6];	/* space for enet addr */
    u_short	packetType;		/* type field for the packet */
    int		pktLen;			/* total length of packet */
    int		dataLen;		/* real length of data portion */
    int		loopy;			/* loop counter */
    char	*nicePort;		/* ptr to the nice buffer port */
    u_short	*pSource;		/* ptr to source of data byte */
    DRV_CTRL * 	pDrvCtrl; 		/* ptr to driver control structure */
    static char	privateBuf [ETHERMTU + ENET_HDR_REAL_SIZ];

#ifdef FN_DEBUG
    if (DEBUG_TX)
        printf  (
                "fn: Tx: pIfnet=%x  pMbuf=%x  pDest=%x\n",
                pIfnet, pMbuf, pDest
                );
#endif /* FN_DEBUG */

    /* Check ifnet flags. Return error if incorrect. */

    if  (
        (pIfnet->if_flags & (IFF_UP | IFF_RUNNING)) !=
        (IFF_UP | IFF_RUNNING)
        )
        {
        m_freem (pMbuf);
        return (ENETDOWN);
        }

    /* Attempt to convert socket addr into enet addr and packet type.
     * Note that if ARP resolution of the address is required, the ARP
     * module will call our routine again to transmit the ARP request
     * packet.  This means we may actually call ourselves recursively!
     */

    if (convertDestAddr (pIDR, pDest, destEnetAddr, &packetType, pMbuf) 
           == FALSE)
        return (OK);    /* I KNOW returning OK is stupid, but it is correct */

    /* Get driver control pointer */

    pDrvCtrl = &drvCtrl [pIDR->ac_if.if_unit];

    /* get ptr to the buffer port */

    nicePort = pDrvCtrl->pDev + NICE_PORT;

    semTake (pDrvCtrl->TxSem, WAIT_FOREVER);	/* take the xmit semaphore */

    bcopy (destEnetAddr, privateBuf, 6);
    bcopy ( (char *) pDrvCtrl->fn_enaddr, (privateBuf + 6), 6);
    ((struct ether_header *)privateBuf)->ether_type = packetType;
    dataLen = 0;
    copy_from_mbufs ((privateBuf + ENET_HDR_REAL_SIZ),
			  pMbuf, 
			  dataLen); 			/* stuffed by macro */

#ifdef FN_DEBUG
    if (DEBUG_TX)
        printf ("fn: Tx: dataLen=%d\n", dataLen);
#endif /* FN_DEBUG */
    
    /* Ensure we send a legal size frame. */

    dataLen += ENET_HDR_REAL_SIZ;
    pktLen = max (ETHERSMALL, dataLen);

    /* taking care of short packets, pad with 0s */

    if (dataLen < ETHERSMALL)
	for (loopy = dataLen; loopy < pktLen; loopy++)
            privateBuf [loopy] = 0;

    /*
     * Support for network interface hooks. Call output hook
     * if any specified. If the routine returns non-zero, 
     * assume the hook has taken care of the frame (don't send
     * on interface). See etherLib for more information
     */

    if ((etherOutputHookRtn != NULL) && 
	(* etherOutputHookRtn) (&pDrvCtrl->fn_if, privateBuf, pktLen))
	goto outputDone;	/* goto the end */

    /* Tell the device the length; in little endian order */

#if (CPU_FAMILY == SPARC)
    sysAsiSeth  (nicePort,
                ((pktLen & 0xff00) >> 8) |
                ((pktLen & 0x00ff) << 8)
                );
#endif
    pSource = (u_short *) privateBuf;
    for (loopy = pktLen; loopy > 0 ; loopy -= sizeof (u_short))
	{
#if (CPU_FAMILY == SPARC)
    	sysAsiSeth (nicePort, *pSource);
#endif
	pSource++;
	}

    if (pktLen % sizeof (u_short))	/* packet size is odd */
	{
	unsigned short lastByte;
	lastByte = 0;
	((char *) (&lastByte)) [0] = ((char *) (pSource)) [0];
#if (CPU_FAMILY == SPARC)
	 sysAsiSeth (nicePort, lastByte);
#endif
	}

    /* See if transmitter is free and start it orelse  wait.  */

#if (CPU_FAMILY == SPARC)
    while ((sysAsiGeth (pDrvCtrl->pDev + NICE_TRANSMIT)) & (0x7f << 8))
    	;	/* loop until transmit count is 0 */

    sysAsiSeth (pDrvCtrl->pDev + NICE_TRANSMIT, KICK_TRANSMIT);
#endif

    /* Bump the statistic counter. */

    pDrvCtrl->fn_if.if_opackets++;

    /* That's all. */
    outputDone:

    semGive (pDrvCtrl->TxSem);
    return (0);
    } /* End of fnOutput() */

#else
/*******************************************************************************
*
* fnTxStartup - kick start the transmitter after generic ether_output
*
* This routine is responsible for tranmitting packets over physical media.
* The data to be sent out is held in a chain of mbufs. The data in each mbuf 
* fragment must be copied to the device's packet buffer.
*
* RETURNS: OK, or ERROR
*/

LOCAL int fnTxStartup
    (
    DRV_CTRL * 	pDrvCtrl 	/* pointer to driver control structure */
    )
    {
    struct mbuf *	pMbuf;		/* ptr to the mbufs to send */
    int		pktLen;			/* total length of packet */
    int		dataLen;		/* real length of data portion */
    int		loopy;			/* loop counter */
    char	*nicePort;		/* ptr to the nice buffer port */
    u_short	*pSource;		/* ptr to source of data byte */
    static char	privateBuf [ETHERMTU];

    /* get ptr to the buffer port */

    nicePort = pDrvCtrl->pDev + NICE_PORT;

    while (pDrvCtrl->idr.ac_if.if_snd.ifq_head)
        {
        /* Dequeue a packet from the send queue. */

        IF_DEQUEUE (&pDrvCtrl->idr.ac_if.if_snd, pMbuf);

        /*
         * This driver maintains transmit resources internally. The 
         * CPU cannot retrieve or examine them.
         */

        copy_from_mbufs (privateBuf, pMbuf, dataLen);

#ifdef FN_DEBUG
        if (DEBUG_TX)
            printf ("fn: Tx: dataLen=%d\n", dataLen);
#endif /* FN_DEBUG */
    
        /* Ensure we send a legal size frame. */

        pktLen = max (ETHERSMALL, dataLen);

        /* taking care of short packets, pad with 0s */

        if (dataLen < ETHERSMALL)
	    for (loopy = dataLen; loopy < pktLen; loopy++)
               privateBuf [loopy] = 0;

        /*
         * Support for network interface hooks. Call output hook
         * if any specified. If the routine returns non-zero, 
         * assume the hook has taken care of the frame (don't send
         * on interface). See etherLib for more information
         */

        if ( (etherOutputHookRtn != NULL) && 
	     (* etherOutputHookRtn) (&pDrvCtrl->fn_if, privateBuf, pktLen))
            continue;

         /* Tell the device the length; in little endian order */

#if (CPU_FAMILY == SPARC)
        sysAsiSeth  (nicePort,
                     ( (pktLen & 0xff00) >> 8) | ( (pktLen & 0x00ff) << 8));
#endif
        pSource = (u_short *)privateBuf;
        for (loopy = pktLen; loopy > 0 ; loopy -= sizeof (u_short))
	    {
#if (CPU_FAMILY == SPARC)
    	    sysAsiSeth (nicePort, *pSource);
#endif
	    pSource++;
	    }

        if (pktLen % sizeof (u_short))	/* packet size is odd */
	    {
	    unsigned short lastByte;
	    lastByte = 0;
	    ( (char *) (&lastByte))[0] = ((char *) (pSource))[0];
#if (CPU_FAMILY == SPARC)
	    sysAsiSeth (nicePort, lastByte);
#endif
	    }

        /* See if transmitter is free and start it orelse  wait.  */

#if (CPU_FAMILY == SPARC)
        while ( (sysAsiGeth (pDrvCtrl->pDev + NICE_TRANSMIT)) & (0x7f << 8))
    	    ; 	/* loop until transmit count is 0 */

        sysAsiSeth (pDrvCtrl->pDev + NICE_TRANSMIT, KICK_TRANSMIT);
#endif

        /* Bump the statistic counter. */

        pDrvCtrl->fn_if.if_opackets++;
        }
    return (0);
    }
#endif

/*******************************************************************************
*
* fnRxEvent - process the receiver event from the device
*
* This routine is the top level receiver handler.  Runs as long as packets 
* left in the buffer.
*
* RETURNS: N/A
*/

LOCAL void fnRxEvent 
    (
    DRV_CTRL * 	pDrvCtrl 	/* pointer to driver control structure */
    )
    {
    char *	pDev;		/* ptr to the device regs */

    /* Check ifnet flags. Return if we're down, leaving interrupt disabled. */

    if  (
        (pDrvCtrl->fn_if.if_flags & (IFF_UP | IFF_RUNNING)) !=
        (IFF_UP | IFF_RUNNING)
        )
        return;

    pDev = pDrvCtrl->pDev;                      /* get ptr to device */

    /* While more packets, process the packet. */

    while ( fnRxMore (pDrvCtrl) )
	{
        if (fnRxProcessPkt (pDrvCtrl) == ERROR)
	    {
	    rcvPktError++; 	/* bump the recieve packet error counter */
            break;
	    }
	}

    /* Turn the receive interrupts on again. */

#if (CPU_FAMILY == SPARC)
    sysAsiSeth (pDev + NICE_INTRMASK, NORM_INTRMASK);
#endif

    } /* End of fnRxEvent() */

/*******************************************************************************
*
* fnRxMore - indicates more packets in the buffer
*
* This routine indicates that there are more packets left in the buffer
*
* RETURNS: TRUE or FALSE
*/

LOCAL BOOL fnRxMore 
    (
    DRV_CTRL * 	pDrvCtrl 	/* pointer to driver control structure */ 
    )
    {
    u_short 	mode;
    char *	pDev;		/* ptr to the device regs */

    pDev = pDrvCtrl->pDev;	/* get ptr to device */

#if (CPU_FAMILY == SPARC)
    mode = sysAsiGeth (pDev + NICE_MODE);
#endif

    if (mode & DLCR5_BUF_EMPTY)
        return (FALSE);
    else
        return (TRUE);

    } /* End of fnRxMore() */

/*******************************************************************************
*
* fnRxProcessPkt - process the next available packet
*
* RETURNS: OK, or ERROR.
*/

LOCAL STATUS fnRxProcessPkt 
    (
    DRV_CTRL * 	pDrvCtrl 	/* pointer to driver control structure */
    )
    {
    int			loopy;		/* loop counter */
    int 		dataLen;	/* length of packet data */
    char *		pDev;		/* ptr to the device regs */
    u_short *		pDest;		/* ptr to destination */
    struct mbuf *	pMbuf;		/* ptr to mbufs */
    RX_HDR 		rxHdr;		/* an Rx pkt header */

    /* allocate space for one received frame */

    static struct rx_frame
        {
        struct ether_header enetHdr;            /* the Ethernet header */
        char data [1500];                       /* the data field */
        } rxFrame;


    pDev = pDrvCtrl->pDev;                      /* get ptr to device */

    /* Fill our local copy of the Rx header from the device's buffer. */

#if (CPU_FAMILY == SPARC)
    rxHdr.status = sysAsiGeth (pDev + NICE_PORT);
    rxHdr.len    = sysAsiGeth (pDev + NICE_PORT);
#endif
    rxHdr.len    = ((rxHdr.len & 0xff00) >> 8) |            /* swap bytes */
                    ((rxHdr.len & 0x00ff) << 8);

    /* Clear the status bit for packet ready. */

#if (CPU_FAMILY == SPARC)
    sysAsiSeth (pDev + NICE_STATUS, DLCR1_RX_PKT);
#endif

#ifdef FN_DEBUG
    if (DEBUG_RX)
        printf  (
                "fn: Rx: status=%04x  len=%d\n",
                rxHdr.status & 0xff00,  rxHdr.len
                );
#endif /* FN_DEBUG */

    /* Sanity check the packet info. */

    if  (
        (!(rxHdr.status & RX_HDR_STAT_GOOD))        ||
        (rxHdr.len < 60)                            ||
        (rxHdr.len > 1514)
        )
        {
        printf  ("fn: Fatal error, invalid receiver header %04x %04x\n",
                rxHdr.status, rxHdr.len);
        pDrvCtrl->fn_if.if_ierrors++;                       /* bump errors */
        pDrvCtrl->fn_if.if_flags &= ~(IFF_UP | IFF_RUNNING);   /* flags down */
        return (ERROR);
        }

    /* Bump the statistic counter */

    pDrvCtrl->fn_if.if_ipackets++;

    /* Fill our frame buffer from the device's buffer. */

    pDest = (u_short *) &rxFrame;                   /* stuff ptr */

    for (loopy = rxHdr.len; loopy > 0; loopy -= sizeof (u_short))
#if (CPU_FAMILY == SPARC)
        *pDest++ = sysAsiGeth (pDev + NICE_PORT);
#else
        ;
#endif

    /*
     * Support for network interface hooks. Call input hook if any
     * specified. If the routine returns non-zero, assume the 
     * hook has taken care of the frame. See etherLib in the Vxworks
     * Reference Manual for more information.
     */

    if ((etherInputHookRtn != NULL) &&
	((*etherInputHookRtn) (&pDrvCtrl->fn_if, (char *) &rxFrame , rxHdr.len) 
	 != 0))
    	return (OK);

    /* Copy our frame buffer into mbufs. */

    dataLen = rxHdr.len - ENET_HDR_REAL_SIZ;        /* obtain data length */
    pMbuf = copy_to_mbufs (rxFrame.data, dataLen, 0, &pDrvCtrl->fn_if);

    /* Give the mbufs to the upper layers */

    if (pMbuf != NULL)
#ifdef BSD43_DRIVER
        do_protocol_with_type   (
                                rxFrame.enetHdr.type,   /* packet type */
                                pMbuf,                  /* the mbufs */
                                &pDrvCtrl->idr,  /* interface data record */
                                dataLen                 /* length of data */
                                );
#else
        do_protocol (&rxFrame.enetHdr, pMbuf, &pDrvCtrl->idr, dataLen);
#endif
    else
        pDrvCtrl->idr.ac_if.if_ierrors++;

#ifdef FN_DEBUG
    logMsg ("fn: Sent packet up.\n", 0, 0,  0, 0, 0, 0);
#endif
    return (OK);

    } /* End of fnRxProcessPkt() */

/*******************************************************************************
*
* fnIntr - the driver interrupt handler routine
*
* This routine is called at interrupt level in response to an interrupt from
* the controller.
*
* RETURNS: N/A
*/

LOCAL void fnIntr 
    (
    int		unit		/* unit number to service */
    )
    {
    char	*pDev;	/* ptr to the device regs */
    u_short	event;
    u_short	reg;
    DRV_CTRL * 	pDrvCtrl; 	/* ptr to driver control structure */

    pDrvCtrl = &drvCtrl [unit]; 

    pDev    = pDrvCtrl->pDev;

#if (CPU_FAMILY == SPARC)
    event   = sysAsiGeth (pDev + NICE_STATUS);      /* get intr status */
    reg     = sysAsiGeth (pDev + NICE_INTRMASK);    /* get current mask */
#endif


#ifdef FN_DEBUG
    if (DEBUG_INTR)
        logMsg ("fn: Intr: event=%04x\n", event, 0, 0, 0, 0, 0);
#endif /* FN_DEBUG */

    /* Handle receiver overflow event. */

    if (event & DLCR1_OVR_FLO)
	{
	overFlowCnt++; 	/* bump the counter */
#if (CPU_FAMILY == SPARC)
 	sysAsiSeth (pDev + NICE_STATUS, DLCR1_OVR_FLO); 
#endif
 	return; 
	} 

    /* Handle the receiver event, only if it was enabled. */

    if ((reg & DLCR3_RX_PKT) && (event & DLCR1_RX_PKT))
        {
#if (CPU_FAMILY == SPARC)
        sysAsiSeth (pDev + NICE_INTRMASK, NO_RX_INTRMASK);  /* disable */
#endif
        netJobAdd ((FUNCPTR)fnRxEvent, (int)pDrvCtrl, 0,0,0,0);
        }

    /* ACK the interrupts */

#if (CPU_FAMILY == SPARC)
    sysAsiSeth (pDev + NICE_STATUS, event);
#endif

    } /* End of fnIntr() */

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* convertDestAddr - converts socket addr into enet addr and packet type
*
* RETURNS: TRUE or FALSE
*/

LOCAL BOOL convertDestAddr 
    (
    IDR * 		pIDR, 		/* ptr to interface data record */
    struct sockaddr *	pDestSktAddr,	/* ptr to a generic sock addr */
    char *		pDestEnetAddr,	/* where to write enet addr */
    u_short *		pPacketType,	/* where to write packet type */
    struct mbuf *	pMbuf		/* ptr to mbuf data */
    )
    {

    /***** Internet family *****/

    {
    struct in_addr	destIPAddr;	/* not used */
    int		trailers;	/* not supported */

    if (pDestSktAddr->sa_family == AF_INET)
        {
        *pPacketType = ETHERTYPE_IP;            /* stuff packet type */
        destIPAddr = ((struct sockaddr_in *) pDestSktAddr)->sin_addr;
 
        if (!arpresolve (pIDR, pMbuf, &destIPAddr, pDestEnetAddr, &trailers))
            return (FALSE);     /* if not yet resolved */

        return (TRUE);
        }
    }

    /***** Generic family *****/

    {
    struct ether_header *	pEnetHdr;

    if (pDestSktAddr->sa_family == AF_UNSPEC)
        {
        pEnetHdr = (struct ether_header *) pDestSktAddr->sa_data;
        bcopy ( (char *)pEnetHdr->ether_dhost, pDestEnetAddr, 6);
        *pPacketType = pEnetHdr->ether_type;
        return (TRUE);
        }
    }

    /* Unsupported family */
    
    return (FALSE);

    }
#endif    /* BSD43_DRIVER */

/* END OF FILE */
