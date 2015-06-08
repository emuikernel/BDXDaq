/* endLib.c - support library for END-based drivers */

/* Copyright 1984 - 2002 Wind River Systems, Inc.  */

/*
modification history
--------------------
01z,21jun02,wap  avoid bcopy() in endEtherAddressForm()
01y,21may02,vvv  updated mib2ErrorAdd doc (SPR #75064)
01x,17may02,rcs  added endMibIfInit() for initializing the MIB-II callback
                 pointers. SPR# 77478	
01w,26oct01,brk  added cast to stop compile warnings (SPR #65332)
01v,27jun01,rcs  Merg tor2.0.2 to Tornado-Comp-Drv
01u,06feb01,spm  fixed detection of 802.3 Ethernet packets (SPR #27844)
01t,16oct00,spm  merged version 01u from tor3_0_x branch (base version 01r):
		 adds support for link-level broadcasts and multiple snarf
		 protocols, and code cleanup; removes etherLib.h dependency
01s,29apr99,pul  Upgraded NPT phase3 code to tor2.0.0
01r,16mar99,spm  recovered orphaned code from tor2_0_x branch (SPR #25770)
01q,09dec98,n_s  fixed endEtherPacketDataGet to handle M_BLK chains. spr 23895
01p,18nov98,n_s  fixed end8023AddressForm. spr 22976.
01o,10dec97,gnn  made minor man page fixes
01n,08dec97,gnn  END code review fixes.
01m,17oct97,vin  changed prototypes, fixed endEtherPacketDataGet.
01k,25sep97,gnn  SENS beta feedback fixes
01j,27aug97,gnn  doc fixes and clean up.
01i,25aug97,vin  added netMblkDup in endEtherPacketDataGet().
01h,22aug97,gnn  cleaned up and improved endEtherAddrGet.
01g,19aug97,gnn  changes due to new buffering scheme.
		 added routine to get addresses from a packet.
01f,12aug97,gnn  changes necessitated by MUX/END update.
01e,jul3197,kbw  fixed man page problems found in beta review
01d,26feb97,gnn  Set the type of a returned M_BLK_ID correctly.
01c,05feb97,gnn  Added a check for NULL on allocation.
01b,03feb97,gnn  Added default memory management routines.
01a,26dec96,gnn  written.

*/
  
/*
DESCRIPTION
This library contains support routines for Enhanced Network Drivers.
These routines are common to ALL ENDs.  Specialized routines should only
appear in the drivers themselves.

INCLUDE FILES:
*/

/* includes */

#include "vxWorks.h"
#include "memLib.h"
#include "netBufLib.h"
#include "endLib.h"
#include "end.h"
#include "etherMultiLib.h"
#include "bufLib.h"
#include "netinet/if_ether.h"
#include "memLib.h"
#include "stdlib.h"

/* defints */

/* typedefs */

/* globals */

MIB_ROUTINES * 	pMibRtn;    /* structure to hold MIB-II callback pointers */ 

/* locals */

/* forward declarations */


void endRcvRtnCall
    (
    END_OBJ* pEnd,
    M_BLK_ID pData
    )
    {
    if (pEnd->receiveRtn) 
	{ 
	pEnd->receiveRtn ((pEnd), pData);
	}
    }

void tkRcvRtnCall
    (
    END_OBJ* pEnd,
    M_BLK_ID pData,
    long netSvcOffset,
    long netSvcType,
    BOOL wrongDstInt,
    void* pAsyncData
    )
    {
    if (pEnd->receiveRtn)
	{
	pEnd->receiveRtn ((pEnd), pData, netSvcOffset , netSvcType, 
			  wrongDstInt, pAsyncData);
	}
    }


void endTxSemTake
    (
    END_OBJ* pEnd,
    int tmout
    ) 
    {
    semTake(pEnd->txSem,tmout);
    }

void endTxSemGive
    (
    END_OBJ* pEnd
    )
    {
    semGive (pEnd->txSem);
    }

void endFlagsClr
    (
    END_OBJ* pEnd,
    int clrBits
    ) 
    {
    pEnd->flags &= ~clrBits;
    }

void endFlagsSet
    (
    END_OBJ* pEnd,
    int setBits
    )
    {
    pEnd->flags |= (setBits);
    }

int endFlagsGet
    (
    END_OBJ* pEnd
    ) 
    {
    return(pEnd->flags);
    }

int endMultiLstCnt
    (
    END_OBJ* pEnd
    )
    {
    return (lstCount(&pEnd->multiList));
    }

ETHER_MULTI* endMultiLstFirst
    (
    END_OBJ *pEnd
    ) 
    {
    return ((ETHER_MULTI *)(lstFirst(&pEnd->multiList)));
    }

ETHER_MULTI* endMultiLstNext
    (
    ETHER_MULTI* pCurrent
    ) 
    {
    return ((ETHER_MULTI *)(lstNext(&pCurrent->node)));
    }

char* endDevName
    (
    END_OBJ* pEnd
    ) 
    {
    return(pEnd->devObject.name);
    }

void endObjectUnload
    (
    END_OBJ* pEnd
    )
    { 
    lstFree (&pEnd->multiList); 
    lstFree (&pEnd->protocols); 
    }

/******************************************************************************
*
* endMibIfInit - Initialize the MIB-II callback pointers
*
* This routine initializes the MIB-II pointers used by the callback macros.
*
* RETURNS: OK or ERROR.
*
* NOMANUAL
*/

STATUS endMibIfInit
    (
    FUNCPTR pMibAllocRtn,
    FUNCPTR pMibFreeRtn,
    FUNCPTR pMibCtrUpdate,
    FUNCPTR pMibVarUpdate
    )
    {
    if ((pMibAllocRtn != NULL) && (pMibFreeRtn != NULL) && 
        (pMibCtrUpdate != NULL) && (pMibVarUpdate != NULL))
        {
        if (pMibRtn == NULL)
            {
            if ((pMibRtn = (MIB_ROUTINES *)malloc (sizeof (MIB_ROUTINES))) == 
                 NULL)
                {
                return(ERROR);
                }
            }
        pMibRtn->mibAlloc = (FUNCPTR)pMibAllocRtn;
        pMibRtn->mibFree = (FUNCPTR)pMibFreeRtn;
        pMibRtn->mibCntUpdate = (FUNCPTR)pMibCtrUpdate;
        pMibRtn->mibVarUpdate = (FUNCPTR)pMibVarUpdate;

        return(OK);
        }
    else
        {
        return(ERROR);
        }
    }

/******************************************************************************
*
* mib2Init - initialize a MIB-II structure
*
* Initialize a MIB-II structure.  Set all error counts to zero.  Assume
* a 10Mbps Ethernet device.
*
* RETURNS: OK or ERROR.
*/

STATUS mib2Init
    (
    M2_INTERFACETBL *pMib,      /* struct to be initialized */
    long ifType,                /* ifType from m2Lib.h */
    UCHAR * phyAddr,            /* MAC/PHY address */
    int addrLength,             /* MAC/PHY address length */
    int mtuSize,                /* MTU size */
    int speed                   /* interface speed */
    )
    {
    /* Clear out our statistics. */

    bzero ((char *)pMib, sizeof (*pMib));

    pMib->ifPhysAddress.addrLength = addrLength;
    
    /* Obtain our Ethernet address and save it */
    bcopy ((char *) phyAddr, (char *)pMib->ifPhysAddress.phyAddress,
	    pMib->ifPhysAddress.addrLength);

    /* Set static statistics. assume ethernet */

    pMib->ifType = ifType;
    pMib->ifMtu =  mtuSize;
    pMib->ifSpeed = speed;

    return OK;
    }

/******************************************************************************
*
* mib2ErrorAdd - change a MIB-II error count
*
* This function adds a specified value to one of the MIB-II error counters in a 
* MIB-II interface table.  The counter to be altered is specified by the 
* errCode argument. errCode can be MIB2_IN_ERRS, MIB2_IN_UCAST, MIB2_OUT_ERRS 
* or MIB2_OUT_UCAST. Specifying a negative value reduces the error count, a 
* positive value increases the error count.
*
* RETURNS: OK or ERROR.
*/

STATUS mib2ErrorAdd
    (
    M2_INTERFACETBL * pMib,
    int errCode,
    int value
    )
    {
    switch (errCode)
	{
	default:
	case MIB2_IN_ERRS:
	    pMib->ifInErrors += value;
	    break;

	case MIB2_IN_UCAST:
	    pMib->ifInUcastPkts += value;
	    break;

	case MIB2_OUT_ERRS:
	    pMib->ifOutErrors += value;
	    break;

	case MIB2_OUT_UCAST:
	    pMib->ifOutUcastPkts += value;
	    break;
	}
    
    return OK;
    }

/*******************************************************************************
*
* endObjInit - initialize an END_OBJ structure
*
* This routine initializes an END_OBJ structure and fills it with data from 
* the argument list.  It also creates and initializes semaphores and 
* protocol list.
*
* RETURNS: OK or ERROR.
*/

STATUS endObjInit
    (
    END_OBJ *   pEndObj,        /* object to be initialized */
    DEV_OBJ*    pDevice,        /* ptr to device struct */
    char *      pBaseName,      /* device base name, for example, "ln" */
    int         unit,           /* unit number */
    NET_FUNCS * pFuncTable,     /* END device functions */
    char*       pDescription
    )
    {
    pEndObj->devObject.pDevice = pDevice;

    /* Create the transmit semaphore. */

    pEndObj->txSem = semMCreate ( SEM_Q_PRIORITY |
				  SEM_DELETE_SAFE |
				  SEM_INVERSION_SAFE);

    if (pEndObj->txSem == NULL)
	{
	return (ERROR);
	}

    /* Install data and functions into the network node. */

    pEndObj->flags = 0;
    lstInit (&pEndObj->protocols);      /* Head of protocol list, */
					/* none yet. */

    /* Check and control the length of the name string. */
    if (strlen(pBaseName) > sizeof(pEndObj->devObject.name))
	pBaseName[sizeof(pEndObj->devObject.name - 1)] = EOS;

    strcpy (pEndObj->devObject.name, pBaseName);

    /* Check and control the length of the description string. */
    if (strlen(pDescription) > sizeof(pEndObj->devObject.description))
	pDescription[sizeof(pEndObj->devObject.description - 1)] = EOS;
    strcpy (pEndObj->devObject.description, pDescription);

    pEndObj->devObject.unit = unit;
    
    pEndObj->pFuncTable = pFuncTable;

    /* Clear multicast info. */

    lstInit (&pEndObj->multiList);
    pEndObj->nMulti = 0;

    pEndObj->snarfCount = 0;
    
    return OK;
    }

/*******************************************************************************
*
* endObjFlagSet - set the `flags' member of an END_OBJ structure
*
* As input, this routine expects a pointer to an END_OBJ structure 
* (the <pEnd> parameter) and a flags value (the <flags> parameter).
* This routine sets the 'flags' member of the END_OBJ structure
* to the value of the <flags> parameter. 
*
* Because this routine assumes that the driver interface is now up,  
* this routine also sets the 'attached' member of the referenced END_OBJ
* structure to TRUE. 
*
* RETURNS: OK
*/

STATUS endObjFlagSet
    (
    END_OBJ * pEnd,
    UINT        flags
    )
    {
    pEnd->attached = TRUE;
    pEnd->flags = flags;

    return OK;
    }

/******************************************************************************
*
* end8023AddressForm - form an 802.3 address into a packet
*
* This routine accepts the source and destination addressing information
* through <pSrcAddr> and <pDstAddr> mBlks and returns an M_BLK_ID to the
* assembled link level header.  If the <bcastFlag> argument is TRUE, it
* sets the destination address to the link-level broadcast address and
* ignores the <pDstAddr> contents. This routine prepends the link level header
* into <pMblk> if there is enough space available or it allocates a new
* mBlk/clBlk/cluster and prepends the new mBlk to the mBlk chain passed in
* <pMblk>.  This routine returns a pointer to an mBlk which contains the
* link level header information.
*
* RETURNS: M_BLK_ID or NULL.
*
* NOMANUAL
*/

M_BLK_ID end8023AddressForm
    (
    M_BLK_ID pMblk,
    M_BLK_ID pSrcAddr,
    M_BLK_ID pDstAddr,
    BOOL bcastFlag
    )
    {
    short dataLen;     /* length of data including LLC */
    short etherType;   /* Ethernet network type code */

    struct llc * pLlc; /* link layer control header */

    dataLen = pMblk->mBlkPktHdr.len + LLC_SNAP_FRAMELEN;
    dataLen = htons (dataLen);
 
    M_PREPEND(pMblk, SIZEOF_ETHERHEADER + LLC_SNAP_FRAMELEN, M_DONTWAIT);

    if (pMblk != NULL)
	{
	if (bcastFlag)
	    bcopy ((char *)etherbroadcastaddr, pMblk->m_data, 6);
	else
	    bcopy (pDstAddr->m_data, pMblk->m_data, pDstAddr->m_len);
	bcopy (pSrcAddr->m_data, pMblk->m_data + pDstAddr->m_len,
	       pSrcAddr->m_len);
	bcopy ((char *) &dataLen, pMblk->m_data + pDstAddr->m_len + 
	       pSrcAddr->m_len, sizeof (short));

	/* Fill in LLC using SNAP values */

	pLlc = (struct llc *) (pMblk->m_data + pDstAddr->m_len + 
			       pSrcAddr->m_len + sizeof (short));
	  
	pLlc->llc_dsap = LLC_SNAP_LSAP;
	pLlc->llc_ssap = LLC_SNAP_LSAP; 
	pLlc->llc_un.type_snap.control = LLC_UI;
	bzero ((char *)pLlc->llc_un.type_snap.org_code, 
	       sizeof (pLlc->llc_un.type_snap.org_code));

	/* Enter ethernet network type code into the LLC snap field */

	etherType = htons (pDstAddr->mBlkHdr.reserved);
	bcopy ((char *) &etherType, 
	       (char *) &(pLlc->llc_un.type_snap.ether_type), 
	       sizeof (short));
	}
    return (pMblk);
    }

/******************************************************************************
*
* endEtherAddressForm - form an Ethernet address into a packet
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

M_BLK_ID endEtherAddressForm
    (
    M_BLK_ID pMblk,     /* pointer to packet mBlk */
    M_BLK_ID pSrcAddr,  /* pointer to source address */
    M_BLK_ID pDstAddr,  /* pointer to destination address */
    BOOL bcastFlag      /* use link-level broadcast? */
    )
    {
    USHORT *pDst;
    USHORT *pSrc;
    M_PREPEND(pMblk, SIZEOF_ETHERHEADER, M_DONTWAIT);

    /*
     * This routine has been optimized somewhat in order to avoid
     * the use of bcopy(). On some architectures, a bcopy() could
     * result in a call into (allegedly) optimized architecture-
     * specific routines. This may be fine for copying large chunks
     * of data, but we're only copying 6 bytes. It's simpler just
     * to open code some 16-bit assignments. The compiler would be
     * hard-pressed to produce sub-optimal code for this, and it
     * avoids at least one function call (possibly several).
     */

    if (pMblk != NULL)
	{
        pDst = (USHORT *)pMblk->m_data;
	if (bcastFlag)
            {
            pDst[0] = 0xFFFF;
            pDst[1] = 0xFFFF;
            pDst[2] = 0xFFFF;
            }
	else
            {
            pSrc = (USHORT *)pDstAddr->m_data;
            pDst[0] = pSrc[0];
            pDst[1] = pSrc[1];
            pDst[2] = pSrc[2];
            }

	/* Advance to the source address field, fill it in. */
        pDst += 3;
        pSrc = (USHORT *)pSrcAddr->m_data;
        pDst[0] = pSrc[0];
        pDst[1] = pSrc[1];
        pDst[2] = pSrc[2];

	((struct ether_header *)pMblk->m_data)->ether_type =
	    pDstAddr->mBlkHdr.reserved;
	}
    return (pMblk);
    }

/******************************************************************************
*
* endEtherPacketDataGet - return the beginning of the packet data
*
* This routine fills the given <pLinkHdrInfo> with the appropriate offsets.
*
* RETURNS: OK or ERROR.
*/

STATUS endEtherPacketDataGet
    (
    M_BLK_ID            pMblk,
    LL_HDR_INFO *       pLinkHdrInfo
    )
    {
    struct ether_header *  pEnetHdr;
    struct ether_header    enetHdr;

    struct llc *           pLLCHdr;
    struct llc             llcHdr;

    USHORT                 etherType;

    pLinkHdrInfo->destAddrOffset        = 0;
    pLinkHdrInfo->destSize              = 6;
    pLinkHdrInfo->srcAddrOffset         = 6;
    pLinkHdrInfo->srcSize               = 6;

    /* Try for RFC 894 first as it's the most common. */

    /* 
     * make sure entire ether_header is in first M_BLK 
     * if not then copy the data to a temporary buffer 
     */

    if (pMblk->mBlkHdr.mLen < SIZEOF_ETHERHEADER) 
	{
	pEnetHdr = &enetHdr;
	if (netMblkOffsetToBufCopy (pMblk, 0, (char *) pEnetHdr, 
				    SIZEOF_ETHERHEADER, (FUNCPTR) bcopy) 
	    < SIZEOF_ETHERHEADER)
	    {
	    return (ERROR);
	    }
	}
    else
	pEnetHdr = (struct ether_header *)pMblk->mBlkHdr.mData;

    etherType = ntohs(pEnetHdr->ether_type);

    /* Deal with 802.3 addressing. */

    /* Here is the algorithm. */
    /* If the etherType is less than the MTU then we know that */
    /* this is an 802.x address from RFC 1700. */
    if (etherType <= ETHERMTU)
	{

	/* 
	 * make sure entire ether_header + llc_hdr is in first M_BLK 
	 * if not then copy the data to a temporary buffer 
	 */
	
	if (pMblk->mBlkHdr.mLen < SIZEOF_ETHERHEADER + LLC_SNAP_FRAMELEN)
	    {
	    pLLCHdr = &llcHdr;
	    if (netMblkOffsetToBufCopy (pMblk, SIZEOF_ETHERHEADER, 
					(char *) pLLCHdr, LLC_SNAP_FRAMELEN,
					(FUNCPTR) bcopy)
		< LLC_SNAP_FRAMELEN)
		{
		return (ERROR);
		}
	    }
	else
	    pLLCHdr = (struct llc *)((char *)pEnetHdr + SIZEOF_ETHERHEADER);
	
	/* Now it may be IP over 802.x so we check to see if the */
	/* destination SAP is IP, if so we snag the ethertype from the */
	/* proper place. */
	
	/* Now if it's NOT IP over 802.x then we just used the DSAP as */
	/* the etherType.  */
	
	if (pLLCHdr->llc_dsap == LLC_SNAP_LSAP)
	    {
	    etherType = ntohs(pLLCHdr->llc_un.type_snap.ether_type);
	    pLinkHdrInfo->dataOffset = SIZEOF_ETHERHEADER + 8;
	    }
	else
	    { /* no SNAP header */
	    pLinkHdrInfo->dataOffset = SIZEOF_ETHERHEADER + 3;
	    etherType = pLLCHdr->llc_dsap;
	    }
	}
    else
	{
	pLinkHdrInfo->dataOffset        = SIZEOF_ETHERHEADER;
	}
    pLinkHdrInfo->pktType               = etherType;

    return (OK);
    }

/******************************************************************************
*
* endEtherPacketAddrGet - locate the addresses in a packet
*
* This routine takes a 'M_BLK_ID', locates the address information, and 
* adjusts the M_BLK_ID structures referenced in <pSrc>, <pDst>, <pESrc>, 
* and <pEDst> so that their pData members point to the addressing 
* information in the packet.  The addressing information is not copied. 
* All 'mBlk' structures share the same cluster.
*
* RETURNS: OK or ERROR.
*/

STATUS endEtherPacketAddrGet
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
	pSrc = netMblkDup (pMblk, pSrc);
	pSrc->mBlkHdr.mData += 6;
	pSrc->mBlkHdr.mLen = 6;
	}
    if (pDst != NULL)
	{
	pDst = netMblkDup (pMblk, pDst);
	pDst->mBlkHdr.mLen = 6;
	}
    if (pESrc != NULL)
	{
	pESrc = netMblkDup (pMblk, pESrc);
	pESrc->mBlkHdr.mData += 6;
	pESrc->mBlkHdr.mLen = 6;
	}
    if (pEDst != NULL)
	{
	pEDst = netMblkDup (pMblk, pEDst);
	pEDst->mBlkHdr.mLen = 6;
	}
    
    return (OK);
    }

