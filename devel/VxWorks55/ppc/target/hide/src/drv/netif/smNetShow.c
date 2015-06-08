/* smNetShow.c - shared memory network driver show routines */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01e,24oct01,mas  doc update (SPR 71149)
01d,02oct01,mas  fixed warnings, added new SM_INT types (SPR 4547 guise)
01c,16sep92,jmm  cleaned up some warnings
01b,21nov92,jdi  documentation cleanup.
01a,25jul92,elh  Created from smNetLib.c
*/

/*
DESCRIPTION
This library provides show routines for the shared memory network 
interface driver. 

The smNetShow() routine is provided as a diagnostic aid to show current
shared memory network status.

INCLUDE FILES: smNetLib.h, smPktLib.h

SEE ALSO: if_sm, smNetLib, smPktLib,
\tb VxWorks Network Programmer's Guide
*/

#include "vxWorks.h"
#include "smPktLib.h"
#include "vxLib.h"
#include "drv/netif/smNetLib.h"


/******************************************************************************
*
* smNetShowInit - dummy routine to drag in smNetShow
*
* This routine serves only as a reference point for the linker when building
* a system.
*
* RETURNS: N/A
*
* NOMANUAL
*/

void smNetShowInit (void)
    {
    }


/******************************************************************************
*
* smNetShow - show information about a shared memory network
*
* This routine displays information about the different CPUs configured
* in a shared memory network specified by <ifName>.  It prints error 
* statistics and zeros these fields if <zero> is set to TRUE.
*
* EXAMPLE
* \cs
*  -> smNetShow
*  Anchor at 0x800000
*  heartbeat = 705, header at 0x800010, free pkts = 237.
*
*  cpu int type    arg1       arg2       arg3    queued pkts
*  --- -------- ---------- ---------- ---------- -----------
*   0  poll            0x0        0x0        0x0       0
*   1  poll            0x0        0x0        0x0       0
*   2  bus-int         0x3       0xc9        0x0       0
*   3  mbox-2         0x2d     0x8000        0x0       0
*
*  input packets = 192     output packets = 164
*  output errors = 0       collisions = 0
*  value = 1 = 0x1
* \ce
*
* RETURNS: OK, or ERROR if there is a hardware setup problem or the routine
* cannot be initialized.
*
* SEE ALSO: smNetLib
*/

STATUS smNetShow
    (
    char *	ifName,		/* backplane interface name (NULL == "sm0") */
    BOOL 	zero		/* TRUE = zap totals */
    )
    {
    SM_SOFTC *	     xs;		/* softc pointer */
    SM_ANCHOR *      pAnchor;		/* pointer to anchor */
    int 	     cpuNum;		/* cpu number */
    struct in_addr   masterAddr;	/* master address */
    SM_PKT_MEM_HDR * pSmPktHdr;		/* packet header */
    SM_PKT_INFO      smPktInfo;		/* info structure */
    int		     bogus;		/* bogus space */
    char 	     addrString [INET_ADDR_LEN];

    /* use "sm0" if no interface name specified */

    if (ifName == NULL)
        {
	ifName = "sm0";
        }

    if ((xs = (SM_SOFTC *) ifunit (ifName)) == NULL)
	{
	printf ("interface %s not attached.\n", ifName);
	return (ERROR);
	}

    /* check validity of anchor */

    pAnchor = xs->smPktDesc.smDesc.anchorLocalAdrs;
    printf ("Anchor Local Addr: 0x%x", (unsigned int) pAnchor);
    if (vxMemProbe ((char *) pAnchor, VX_READ, 4, (char *) &bogus) != OK)
	{
	printf (", UNREADABLE\n");
	return (ERROR);
	}

    if (ntohl (pAnchor->readyValue) != SM_READY) /* check ready value */
	{
	printf (", UNINITIALIZED\n");
	return (ERROR);
	}

    if (smIsAlive (pAnchor, &(pAnchor->smPktHeader), xs->smPktDesc.smDesc.base,
		   5, 0) == FALSE)
	{	
	printf (", NO HEARTBEAT\n");
	return (ERROR);
	}

    if (smPktInfoGet (&(xs->smPktDesc), &smPktInfo) == ERROR)
	{
	printf (" CPU NOT ATTACHED\n");
	return (ERROR);
	}

     if (smPktInfo.smInfo.tasType == SM_TAS_SOFT)
         {
	 printf (", SOFT TAS");
         }

     pSmPktHdr = SM_OFFSET_TO_LOCAL (ntohl (pAnchor->smPktHeader),
				     xs->smPktDesc.smDesc.base,
				     SM_PKT_MEM_HDR *);
    
    if (pSmPktHdr->reserved1 != 0)
	{
	masterAddr.s_addr = pSmPktHdr->reserved1;
        inet_ntoa_b (masterAddr, addrString);
	printf ("\nSequential addressing enabled.  Master address: %s\n",
		addrString);
	}

    printf ("\nheartbeat = %d, header at 0x%x, free pkts = %d.\n\n",
	    ntohl (pSmPktHdr->heartBeat), (unsigned int) pSmPktHdr,
	    smPktInfo.freePkts);


    /* print out info for each CPU */

    printf (
	"cpu int type    arg1       arg2       arg3    queued pkts\n");
    printf (
	"--- -------- ---------- ---------- ---------- -----------\n");

    for (cpuNum = 0; cpuNum < smPktInfo.smInfo.maxCpus; ++cpuNum)
	{
	SM_PKT_CPU_INFO		cpuInfo;	/* cpu info struct */

	if (smPktCpuInfoGet (&(xs->smPktDesc), cpuNum, &cpuInfo) == ERROR)
	    {
	    return (ERROR);
	    }

	if (cpuInfo.status == SM_CPU_ATTACHED)
	    {
	    printf ("%2d  ", cpuNum);
	    switch (cpuInfo.smCpuInfo.intType)
		{
		case SM_INT_NONE:	printf ("poll    "); break;
		case SM_INT_MAILBOX_1:	printf ("mbox-1  "); break;
		case SM_INT_MAILBOX_2:	printf ("mbox-2  "); break;
		case SM_INT_MAILBOX_4:	printf ("mbox-4  "); break;
		case SM_INT_BUS:	printf ("bus-int "); break;
		case SM_INT_MAILBOX_R1:	printf ("mbox-1r "); break;
		case SM_INT_MAILBOX_R2:	printf ("mbox-2r "); break;
		case SM_INT_MAILBOX_R4:	printf ("mbox-4r "); break;
		case SM_INT_USER_1:	printf ("user-1  "); break;
		case SM_INT_USER_2:	printf ("user-2  "); break;
		default:		printf ("%#8x",cpuInfo.smCpuInfo.intType); break;
		}

	    printf (" %#10x %#10x %#10x", cpuInfo.smCpuInfo.intArg1,
		    cpuInfo.smCpuInfo.intArg2, cpuInfo.smCpuInfo.intArg3);
	    printf ("     %3d\n", cpuInfo.inputPkts);
	    }
	}

    printf ("\ninput packets = %3lu  ", xs->xs_if.if_ipackets);
    printf ("output packets = %3lu\n",  xs->xs_if.if_opackets);
    printf ("input errors  = %3lu  ",   xs->xs_if.if_ierrors);
    printf ("output errors = %3lu  \n", xs->xs_if.if_oerrors);
    printf ("collisions    = %3lu\n",   xs->xs_if.if_collisions);

    if (zero)				/* zero interface statistics */
	{
	xs->xs_if.if_ipackets   = 0;
	xs->xs_if.if_opackets   = 0;
	xs->xs_if.if_ierrors    = 0;
	xs->xs_if.if_oerrors    = 0;
	xs->xs_if.if_collisions = 0;
	}

    return (OK);
    }

