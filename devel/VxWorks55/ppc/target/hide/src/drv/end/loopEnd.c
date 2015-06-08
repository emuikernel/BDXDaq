/* loopEnd.c - software loopback END */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*	$NetBSD: loopEnd.c,v 1.13 1994/10/30 21:48:50 cgd Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1993
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
 *	@(#)loopEnd.c	8.1 (Berkeley) 6/10/93
 */

/*
modification history
--------------------
01k,14jan99,sut  prepended protocol information
01j,14jan99,sut  moved to tor2_0_0, renamed to loopEnd.c
01i,14jan99,sut  added routines loAddressForm, loPacketDataGet, loPacketAddrGet
01h,13jan99,sut  revoved loPollStart and loPollEnd
01g,13jan99,sut  put back again multicast support
01f,13jan99,sut  added more documentation
01e,12jan99,sut  removed ether address storage
01d,12jan99,sut  added debug routines
01c,11jan99,sut  removed multicast support
01b,11jan99,sut  converted into END driver
01a,07jan99,sut  started with netif/if_loop.c
*/

/*
DESCRIPTION
This module implements the software loopback Enhanced Network  Driver (END).
The only external routine is loEndLoad(), which can only be called from mux
interface.

The driver does not support polled mode operations. Even though the polled
mode interfaces are exported to mux layer, just returns ERROR for polled mode
operations.


BOARD LAYOUT
This device is "software only."  A jumpering diagram is not applicable.


EXTERNAL INTERFACE
The only external interface is the loEndLoad() routine, which expects
the <initString> parameter as input.  At present this rouine expects only
<unit> from this string. This parameter passes in a colon-delimited string of
the format:

<unit>

The loEndLoad() function uses strtok() to parse the string

TARGET-SPECIFIC PARAMETERS

.IP <unit>
A convenient holdover from the former model.  This parameter is used only
in the string name for the driver.

SEE ALSO: ifLib
*/
#include "vxWorks.h"
#include "stdlib.h"
#include "end.h"
#include "endLib.h"
#include "lstLib.h"
#include "wdLib.h"
#include "iv.h"
#include "semLib.h"
#include "etherLib.h"
#include "logLib.h"
#include "netLib.h"
#include "stdio.h"
#include "sysLib.h"
#include "errno.h"
#include "errnoLib.h"
#include "memLib.h"
#include "iosLib.h"
#undef	ETHER_MAP_IP_MULTICAST
#include "etherMultiLib.h"		/* multicast stuff. */

#include "net/mbuf.h"
#include "net/unixLib.h"
#include "net/protosw.h"
#include "net/systm.h"
#include "net/if_subr.h"
#include "net/route.h"

#include "sys/socket.h"
#include "sys/ioctl.h"
#include "sys/times.h"



/* maximum packet size */
#define LO_SPEED	10000000

#define LO_MIN_FBUF	0	/* Minimum size of the first buffer in a */
                        	/* chain. */

#define LO_ADDR_LEN	0	/* address length */
#define LO_HDR_LEN 	0	/* header length */


/* Naming items */
#define LO_DEV_NAME	"lo"
#define LO_DEV_NAME_LEN	3


/* device states */

#define LO_ST_STOPPED	0x80

#define LO_IS_DEV_STOPPED(pDrvCtrl)\
			   ((pDrvCtrl->state & LO_ST_STOPPED) == LO_ST_STOPPED)

#define LO_DEV_STOP(pDrvCtrl) (pDrvCtrl->state |= LO_ST_STOPPED)
#define LO_DEV_START(pDrvCtrl) (pDrvCtrl->state &= (!LO_ST_STOPPED))

#define LO_PROTOCOL_LEN sizeof(USHORT)

#undef DRV_DEBUG

#ifdef	DRV_DEBUG
#define DRV_DEBUG_OFF		0x0000
#define	DRV_DEBUG_POLL		0x0001
#define	DRV_DEBUG_LOAD		0x0002
#define	DRV_DEBUG_IOCTL		0x0004
#define	DRV_DEBUG_LOG_NVRAM	0x0008

#ifdef LOCAL
#undef LOCAL
#endif /* LOCAL */

#define LOCAL ;

int	loDebug = DRV_DEBUG_OFF; /* Turn it off initially. */
#include "nvLogLib.h"


#define DRV_LOG(FLG, X0, X1, X2, X3, X4, X5, X6)                    \
	if ((loDebug & FLG)&&(FLG & DRV_DEBUG_POLL))                \
            nvLogMsg(X0, X1, X2, X3, X4, X5, X6);                   \
        else if (loDebug & FLG)                                     \
            logMsg(X0, X1, X2, X3, X4, X5, X6);

#define DRV_PRINT(FLG,X)                                            \
	if (loDebug & FLG) printf X;

#else /*DRV_DEBUG*/

#define DRV_LOG(DBG_SW, X0, X1, X2, X3, X4, X5, X6)
#define DRV_PRINT(DBG_SW,X)

#endif /*DRV_DEBUG*/


/* The definition of the driver control structure */
#define U16 unsigned short int

typedef struct lo_device
    {
    END_OBJ     end;			/* The class we inherit from. */
    int		unit;			/* unit number */
    U16		state;
    u_short     errorStat;              /* error status */
    } LOEND_DEVICE;

/* forward declarations */

/* This is the only externally visible interface. */

END_OBJ* 	loEndLoad (char* initString);

/* END interfaces */

LOCAL STATUS	loUnload (LOEND_DEVICE* pDrvCtrl);
LOCAL STATUS	loIoctl (LOEND_DEVICE* pDrvCtrl, int cmd, caddr_t data);
LOCAL STATUS	loSend (LOEND_DEVICE* pDrvCtrl, M_BLK_ID pBuf);
LOCAL STATUS	loStart (LOEND_DEVICE* pDrvCtrl);
LOCAL STATUS	loStop (LOEND_DEVICE* pDrvCtrl);
LOCAL STATUS	loMCastAddrAdd (LOEND_DEVICE* pDrvCtrl, char* pAddress);
LOCAL STATUS	loMCastAddrDel (LOEND_DEVICE* pDrvCtrl, char* pAddress);
LOCAL STATUS	loMCastAddrGet (LOEND_DEVICE* pDrvCtrl,
				    MULTI_TABLE* pTable);
LOCAL STATUS	loPollSend (LOEND_DEVICE* pDrvCtrl, M_BLK_ID pBuf);
LOCAL STATUS	loPollReceive (LOEND_DEVICE* pDrvCtrl, M_BLK_ID pBuf);

M_BLK_ID loAddressForm (M_BLK_ID pMblk, M_BLK_ID pSrcAddr, M_BLK_ID pDstAddr);

STATUS loPacketDataGet (M_BLK_ID pMblk, LL_HDR_INFO* pLinkHdrInfo);
STATUS loPacketAddrGet (M_BLK_ID pMblk, M_BLK_ID pSrc, M_BLK_ID pDst,
                        M_BLK_ID pESrc, M_BLK_ID pEDst);

LOCAL STATUS	loInitStringParse ();


/*
 * Declare our function table.  This is static across all driver
 * instances.
 */

LOCAL NET_FUNCS loFuncTable =
    {
    (FUNCPTR)loStart,		/* Function to start the device. */
    (FUNCPTR)loStop,		/* Function to stop the device. */
    (FUNCPTR)loUnload,		/* Unloading function for the driver. */
    (FUNCPTR)loIoctl,		/* Ioctl function for the driver. */
    (FUNCPTR)loSend,		/* Send function for the driver. */
    (FUNCPTR)loMCastAddrAdd,	/* Multicast address add */
    (FUNCPTR)loMCastAddrDel, 	/* Multicast address delete */
    (FUNCPTR)loMCastAddrGet, 	/* Multicast table retrieve */
    (FUNCPTR)loPollSend,	/* Polling send function for the driver. */
    (FUNCPTR)loPollReceive,	/* Polling receive function for the driver. */
    loAddressForm,		/* Put address info into a packet.  */
    loPacketDataGet,		/* Get a pointer to packet data.   */
    loPacketAddrGet		/* Get packet addresses.  */
    };


/******************************************************************************
*
* loEndLoad - initialize the driver and device
*
* This routine initializes the driver and the device to the operational state.
* All of the device-specific parameters are passed in <initString>, which
* expects a string of the following format:
*
* <unit>
*
* This routine can be called in two modes. If it is called with an empty but
* allocated string, it places the name of this device (that is, "lo") into 
* the <initString> and returns 0.
*
* If the string is allocated and not empty, the routine attempts to load
* the driver using the values specified in the string.
*
* RETURNS: An END object pointer, or NULL on error, or 0 and the name of the
* device if the <initString> was NULL.
*/
END_OBJ* loEndLoad
    (
    char* initString		/* string to be parse by the driver */
    )
    {
    LOEND_DEVICE 	*pDrvCtrl;

    DRV_LOG (DRV_DEBUG_LOAD, "Loading lo...\n", 1, 2, 3, 4, 5, 6);

    if (initString == NULL)
        return (NULL);
    
    if (initString[0] == NULL)
        {
        bcopy((char *)LO_DEV_NAME, initString, LO_DEV_NAME_LEN);
        return (0);
        }
    
    /* allocate the device structure */

    pDrvCtrl = (LOEND_DEVICE *)calloc (sizeof (LOEND_DEVICE), 1);
    
    if (pDrvCtrl == NULL)
	goto errorExit;

    /* parse the init string, filling in the device structure */

    if (loInitStringParse (pDrvCtrl, initString) == ERROR)
	goto errorExit;


    /* initialize the END and MIB2 parts of the structure */

    if (END_OBJ_INIT (&pDrvCtrl->end, (DEV_OBJ *)pDrvCtrl, LO_DEV_NAME,
		    pDrvCtrl->unit, &loFuncTable,
                      "Loopback Enhanced Network Driver") == ERROR
     || END_MIB_INIT (&pDrvCtrl->end, M2_ifType_softwareLoopback, NULL,
                      LO_ADDR_LEN, 0, LO_SPEED)
                    == ERROR)
	goto errorExit;


    /* set the flags to indicate readiness */

    END_OBJ_READY (&pDrvCtrl->end,
		    IFF_UP | IFF_RUNNING | IFF_LOOPBACK | IFF_MULTICAST);
    
    DRV_LOG (DRV_DEBUG_LOAD, "Done loading lo...\n", 1, 2, 3, 4, 5, 6);

    return (&pDrvCtrl->end);

errorExit:
    if (pDrvCtrl != NULL)
	free ((char *)pDrvCtrl);

    return NULL;
    }


/******************************************************************************
*
* loUnload - unload a driver from the system
*
* This function first brings down the device, and then frees any
* stuff that was allocated by the driver in the load function.
*/

LOCAL STATUS loUnload
    (
    LOEND_DEVICE* pDrvCtrl
    )
    {
    END_OBJECT_UNLOAD (&pDrvCtrl->end);

    return (OK);
    }


/*******************************************************************************
*
* loStart - start the device
*
* This function calls BSP functions to connect interrupts and start the
* device running in interrupt mode.
*
* RETURNS: OK or ERROR
*
*/

LOCAL STATUS loStart
    (
    LOEND_DEVICE *pDrvCtrl
    )
    {
    LO_DEV_START(pDrvCtrl);
    return OK;
    }


/*******************************************************************************
*
* loStop - stop the device
*
* This function calls BSP functions to disconnect interrupts and stop
* the device from operating in interrupt mode.
*
* RETURNS: OK or ERROR
*/

LOCAL STATUS loStop
    (
    LOEND_DEVICE *pDrvCtrl
    )
    {
    LO_DEV_STOP(pDrvCtrl);    
    return OK;    
    }

/*******************************************************************************
*
* loIoctl - the driver I/O control routine
*
* Process an ioctl request.
*/

LOCAL STATUS loIoctl
    (
    LOEND_DEVICE *pDrvCtrl,
    int cmd,
    caddr_t data
    )
    {
    STATUS error = OK;
    long value;

    if (LO_IS_DEV_STOPPED(pDrvCtrl))
        return ERROR;

    switch (cmd)
        {
        case EIOCSADDR:
		return (ERROR);
            break;

        case EIOCGADDR:
		return (ERROR);
            break;

        case EIOCSFLAGS:
	    value = (long)data;
	    if (value < 0)
		{
		value = -value;
		value--;		/* HELP: WHY ??? */
		END_FLAGS_CLR (&pDrvCtrl->end, value);
		}
	    else
		{
		END_FLAGS_SET (&pDrvCtrl->end, value);
		}
            break;
            
        case EIOCGFLAGS:
	    *(int *)data = END_FLAGS_GET(&pDrvCtrl->end);
            break;

	case EIOCPOLLSTART: /* fall through */ 
	case EIOCPOLLSTOP:
	    error = ERROR;            
	    break;

        case EIOCGMIB2:
            if (data == NULL)
                return (EINVAL);
            bcopy((char *)&pDrvCtrl->end.mib2Tbl, (char *)data,
                  sizeof(pDrvCtrl->end.mib2Tbl));
            break;
            
        case EIOCGFBUF:
            if (data == NULL)
                return (EINVAL);
            *(int *)data = LO_MIN_FBUF;
            break;
            
        case EIOCGMWIDTH:
            if (data == NULL)
                return (EINVAL);
            *(int *)data = 0;
            break;

            
        case EIOCGHDRLEN:
            if (data == NULL)
                return (EINVAL);
            *(int *)data = LO_HDR_LEN;
            break;

        case EIOCMULTIADD:
        case EIOCMULTIDEL:
        case EIOCMULTIGET:
            error = EINVAL;
            break;
            
        default:
            error = EINVAL;
        }
    return (error);
    }


/*******************************************************************************
*
* loSend - the driver send routine
*
* This routine takes a M_BLK_ID sends off the data in the M_BLK_ID.
* The buffer must already have the addressing information properly installed
* in it.  This is done by a higher layer.  The last arguments are a free
* routine to be called when the device is done with the buffer and a pointer
* to the argument to pass to the free routine.  
*
* RETURNS: OK or ERROR.
*/

LOCAL STATUS loSend
    (
    LOEND_DEVICE *pDrvCtrl,	/* device ptr */
    M_BLK_ID pMblk		/* data to send */
    )
    {
    
    STATUS retValue = OK;
    
    if (LO_IS_DEV_STOPPED(pDrvCtrl))
        retValue = ERROR;
    else
        {
        /* Bump the statistic counter. */

        END_ERR_ADD (&pDrvCtrl->end, MIB2_OUT_UCAST, +1);
        END_ERR_ADD (&pDrvCtrl->end, MIB2_IN_UCAST, +1);

        /* Call the upper layer's receive routine. */

        END_RCV_RTN_CALL(&pDrvCtrl->end, pMblk);
        }
        return (retValue);    
    }


/*******************************************************************************
*
* loInitStringParse - parse the initialization string
*
* Parse the input string.  Fill in values in the driver control structure.
* The initialization string format is:
* <unit>
*
* .IP <unit>
* Device unit number, a small integer.
*
* RETURNS: OK, or ERROR if any arguments are invalid.
*/
STATUS loInitStringParse
    (
    LOEND_DEVICE* pDrvCtrl,
    char* initString
    )
    {
    char *	tok;
    char *	pHolder = NULL;

    
    /* Parse the initString */

    /* Unit number. */

    tok = strtok_r (initString, ":", &pHolder);
    if (tok == NULL)
	return ERROR;
    
    pDrvCtrl->unit = atoi (tok);

    return OK;
    }


/*****************************************************************************
*
* loMCastAddrAdd - add a multicast address for the device
*
* This routine adds a multicast address to whatever the driver
* is already listening for.  It then resets the address filter.
*
* RETURNS: OK, or ERROR if fails to add
*/

LOCAL STATUS loMCastAddrAdd
    (
    LOEND_DEVICE *pDrvCtrl,
    char* pAddress
    )
    {
    int error;
    
    error = etherMultiAdd (&pDrvCtrl->end.multiList, pAddress);
    
    return (error);
    }

/*****************************************************************************
*
* loMCastAddrDel - delete a multicast address for the device
*
* This routine removes a multicast address from whatever the driver
* is listening for.  It then resets the address filter.
*
* RETURNS: OK, or ERROR if fails to delete
*/

LOCAL STATUS loMCastAddrDel
    (
    LOEND_DEVICE* pDrvCtrl,
    char* pAddress
    )
    {
    int error;
    
    error = etherMultiDel (&pDrvCtrl->end.multiList, pAddress);
    
    return (error);
    }

/*****************************************************************************
*
* loMCastAddrGet - get the multicast address list for the device
*
* This routine gets the multicast list of whatever the driver
* is already listening for.
*
* RETURNS: OK, or ERROR if fails to get
*
*/

LOCAL STATUS loMCastAddrGet
    (
    LOEND_DEVICE* pDrvCtrl,
    MULTI_TABLE* pTable
    )
    {
    int error;
    
    error = etherMultiGet (&pDrvCtrl->end.multiList, pTable);
    
    return (error);
    }


/*******************************************************************************
*
* loPollSend - routine to send a packet in polled mode.
*
* This routine is called by a user to try and send a packet on the
* device.
*
* RETURNS: ERROR always
*/

LOCAL STATUS loPollSend
    (
    LOEND_DEVICE* pDrvCtrl,
    M_BLK_ID pMblk
    )
    {
    return ERROR;    
    }


/*******************************************************************************
*
* loPollReceive - routine to receive a packet in polled mode.
*
* This routine is called by a user to try and get a packet from the
* device.
*
* RETURNS: ERROR always
*/

LOCAL STATUS loPollReceive
    (
    LOEND_DEVICE *pDrvCtrl,
    M_BLK_ID pMblk
    )
    {
    return ERROR;
    }


/******************************************************************************
*
* loAddressForm - form an Ethernet address into a packet
*
* This routine accepts the source and destination addressing information 
* through <pSrcAddr> and <pDstAddr> and returns an 'M_BLK_ID' that points 
* to the assembled link-level header.  To do this, this routine prepends 
* the link-level header into the cluster associated with <pMblk> if there 
* is enough space available in the cluster.  It then returns a pointer to 
* the pointer referenced in <pMblk>.  However, if there is not enough space 
* in the cluster associated with <pMblk>, this routine reserves a 
* new 'mBlk'-'clBlk'-cluster construct for the header information. 
* It then prepends the new 'mBlk' to the 'mBlk' passed in <pMblk>.  As the 
* function value, this routine then returns a pointer to the new 'mBlk', 
* which the head of a chain of 'mBlk' structures.  The second element of this 
* chain is the 'mBlk' referenced in <pMblk>. 
*
* RETURNS: M_BLK_ID or NULL.
*/

M_BLK_ID loAddressForm
    (
    M_BLK_ID pMblk,     /* pointer to packet mBlk */
    M_BLK_ID pSrcAddr,  /* pointer to source address */
    M_BLK_ID pDstAddr   /* pointer to destination address */
    )
    {
    
    M_PREPEND(pMblk, LO_PROTOCOL_LEN, M_DONTWAIT);

    if (pMblk != NULL)
        bcopy ((char *)&pDstAddr->mBlkHdr.reserved,
               pMblk->m_data, LO_PROTOCOL_LEN);

    return (pMblk); 
    }

/******************************************************************************
*
* loPacketDataGet - return the beginning of the packet data
*
* This routine fills the given <pLinkHdrInfo> with the appropriate offsets.
*
* RETURNS: OK or ERROR.
*/

STATUS loPacketDataGet
    (
    M_BLK_ID 		pMblk,
    LL_HDR_INFO * 	pLinkHdrInfo
    )
    {
    pLinkHdrInfo->destAddrOffset	= 0;
    pLinkHdrInfo->destSize 		= 0;
    pLinkHdrInfo->srcAddrOffset		= 0;
    pLinkHdrInfo->srcSize		= 0;
    pLinkHdrInfo->dataOffset		= LO_PROTOCOL_LEN;
    bcopy (pMblk->m_data, (char *)&pLinkHdrInfo->pktType, LO_PROTOCOL_LEN);

    return (OK);
    }

/******************************************************************************
*
* loPacketAddrGet - locate the addresses in a packet
*
* This routine takes a 'M_BLK_ID', locates the address information, and 
* adjusts the M_BLK_ID structures referenced in <pSrc>, <pDst>, <pESrc>, 
* and <pEDst> so that their pData members point to the addressing 
* information in the packet.  The addressing information is not copied. 
* All 'mBlk' structures share the same cluster.
*
* RETURNS: OK or ERROR.
*/

STATUS loPacketAddrGet
    (
    M_BLK_ID pMblk, /* pointer to packet */
    M_BLK_ID pSrc,  /* pointer to local source address */
    M_BLK_ID pDst,  /* pointer to local destination address */
    M_BLK_ID pESrc, /* pointer to remote source address (if any) */
    M_BLK_ID pEDst  /* pointer to remote destination address (if any) */
    )
    {

    if (pSrc != NULL)
        {
        pSrc->mBlkHdr.mData = NULL;
        pSrc->mBlkHdr.mLen = 0;
        }
    if (pDst != NULL)
        {
        pDst->mBlkHdr.mLen = 0;
        }
    if (pESrc != NULL)
        {
        pESrc->mBlkHdr.mData = NULL;
        pESrc->mBlkHdr.mLen = 0;
        }
    if (pEDst != NULL)
        {
        pEDst->mBlkHdr.mLen = 0;
        }
    
    return (OK);
    }

