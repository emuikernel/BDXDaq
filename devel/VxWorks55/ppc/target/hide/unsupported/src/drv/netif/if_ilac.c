/* if_ilac.c - AMD 79C900 ILACC Ethernet network interface driver */

/* Copyright 1989-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01e,15jul97,spm  added ARP request to SIOCSIFADDR ioctl handler
01d,07apr97,spm  code cleanup and upgraded to BSD 4.4
01c,25nov96,dat  coding standards, documentation
01b,09jan95,caf  added intConnect() call to ilacattach().
01a,02jan95,kat  written.
*/

/*
DESCRIPTION:
*/

/* includes */

#include "vxWorks.h"
#include "net/mbuf.h"
#include "net/protosw.h"
#include "sys/ioctl.h"
#include "sys/socket.h"
#include "errnoLib.h"
#include "net/if.h"
#include "net/route.h"
#include "netinet/in.h"
#include "netinet/in_systm.h"
#include "netinet/ip.h"
#include "netinet/ip_var.h"
#include "netinet/in_var.h"
#include "netinet/if_ether.h"
#include "etherLib.h"
#include "vme.h"
#include "iv.h"
#include "iosLib.h"
#include "ioLib.h"
#include "memLib.h"
#include "net/systm.h"
#include "net/if_subr.h"
#include "sysLib.h"
#include "vxLib.h"
#include "stdio.h"
#include "intLib.h"
#include "logLib.h"
#include "string.h"
#include "netLib.h"
#include "stdlib.h"

#include "drv/netif/if_ilac.h"

#define MINPKTSIZE   60

typedef struct /* SHIFT_BUFFER */
    { 
    char     dummy_space[16]; 
    TM_DESC  shift_tring[XMIT_DES_CNT]; 
    char     shift_xmit_buf[XMIT_DES_CNT * ROUND_BUF_SIZE]; 
    RM_DESC  shift_rring[RECV_DES_CNT]; 
    char     shift_recv_buf[RECV_DES_CNT * ROUND_BUF_SIZE]; 
    INIT_BLK shift_init_blk;
    } SHIFT_BUFFER;

LOCAL SHIFT_BUFFER shift_buffer;

typedef struct mbuf MBUF;
typedef struct arpcom IDR;	/* Interface Data Record wrapper */
typedef struct ifnet IFNET;	/* real Interface Data Record */
typedef struct sockaddr SOCK;
typedef struct ether_header ETH_HDR;

IMPORT unsigned char ilacEnetAddr[6];

typedef struct /* DRV_CTRL */
    {
    IDR           idr;          /* interface data record */

    int           unit;         /* which unit are we */
    ILAC_DEVICE  *addr;        	/* address of physical device */
    int           recvIndex;    /* next receive buffer */
    int           xmitIndex;    /* next transmit buffer */

    TM_DESC      *tranRing;	/* transmit message descriptors */
    RM_DESC      *recvRing;	/* receive message descriptors */
    INIT_BLK     *initBlk;	/* initialization block */
    char         *xmitBuf;	/* transmit buffers */
    char         *recvBuf;	/* receive buffers */

    volatile BOOL rcvHandling;  /* flag, indicates netTask active */
    BOOL          attached;     /* TRUE if attach() has succeeded */
    } DRV_CTRL;

LOCAL DRV_CTRL drvCtrl[1];

LOCAL STATUS ilacInit(int unit, char *eaddr);

LOCAL void   ilacInt(int unit);

/* ether_attach functions */

LOCAL int    ilacIoctl(IDR *pIDR, int cmd, caddr_t data);
#ifdef BSD43_DRIVER
LOCAL STATUS ilacOutput(IDR *pIDR, MBUF *pMbuf, SOCK *pDest);
#endif
LOCAL STATUS ilacReset(int unit);

LOCAL void   ilacHandleRecvInt (DRV_CTRL *pDrv);
#ifdef BSD43_DRIVER
LOCAL void   ilacTxStartup(int unit);
#else
LOCAL void   ilacTxStartup (DRV_CTRL *pDrv);
#endif

LOCAL STATUS ilacReceive(DRV_CTRL *pDrv, unsigned char *buf, int *len);
LOCAL STATUS ilacTransmit(DRV_CTRL *pDrv, unsigned char *buf, int len);

/*******************************************************************************
*
* ilacattach - attach ilac driver network system
*
* RETURNS:
*/

STATUS ilacattach
    (
    int unit,
    char *addrs,
    int ivec
    )
    {
    DRV_CTRL *pDrv = &drvCtrl[unit];
    
    if (pDrv->attached)
	{
        return OK;
	}

#ifdef BSD43_DRIVER
    ether_attach(&pDrv->idr.ac_if, unit, "ilac", (FUNCPTR) NULL,
		 (FUNCPTR) ilacIoctl, (FUNCPTR) ilacOutput,
		 (FUNCPTR) ilacReset);
#else
    ether_attach (
                 &pDrv->idr.ac_if, 
                 unit, 
                 "ilac", 
                 (FUNCPTR) NULL,
		 (FUNCPTR) ilacIoctl, 
                 (FUNCPTR) ether_output,
                 (FUNCPTR) ilacReset
                 );
    pDrv->idr.ac_if.if_start = (FUNCPTR)ilacTxStartup;
#endif

    pDrv->addr = (ILAC_DEVICE *)addrs;

    /* connect the interrupt handler */

    (void) intConnect ((VOIDFUNCPTR *)INUM_TO_IVEC (ivec),
                       (VOIDFUNCPTR)ilacInt, NULL);

    ilacInit(unit, (char *)pDrv->idr.ac_enaddr);
    
    pDrv->idr.ac_if.if_flags |= IFF_UP | IFF_RUNNING | IFF_NOTRAILERS;
    pDrv->rcvHandling = FALSE;

    /* Set our success indicator */
    pDrv->attached = TRUE;

    return OK;
    }

/*******************************************************************************
*
* ilacInit - initialize an ILAC device 
*
* RETURNS:
*/

LOCAL STATUS ilacInit
    (
    int unit,
    char *ethernet_num
    )
    { 
    int          i;
    unsigned int csr0; 
    unsigned int count; 
    DRV_CTRL    *pDrv = &drvCtrl[unit];
    ILAC_DEVICE *ilac = pDrv->addr;
    
    /* Set init bit in CSR0 register */
    ilac->rap = 0;
    ilac->rdp = CSR0_STOP;
    
    pDrv->tranRing =
	(TM_DESC *) (((unsigned int)&shift_buffer.shift_tring[0]) & 0xFFFFFFF0);
    pDrv->recvRing =
	(RM_DESC *) (((unsigned int)&shift_buffer.shift_rring[0]) & 0xFFFFFFF0);
    pDrv->xmitBuf =
	(char *) (((unsigned int)&shift_buffer.shift_xmit_buf[0]) & 0xFFFFFFF0);
    pDrv->recvBuf =
	(char *) (((unsigned int)&shift_buffer.shift_recv_buf[0]) & 0xFFFFFFF0);
    pDrv->initBlk =
	(INIT_BLK *)(((unsigned int)&shift_buffer.shift_init_blk) & 0xFFFFFFF0);
    
    /* Setup the transmit descriptors */ 
    for(i = 0; i < XMIT_DES_CNT; i++)
	{
	pDrv->tranRing[i].addr =
			(unsigned long)&pDrv->xmitBuf[i * ROUND_BUF_SIZE];
	pDrv->tranRing[i].error = 0;
	pDrv->tranRing[i].status = (TST_STP | TST_ENP);
	}

    /* Setup the receive descriptors */ 
    for(i = 0; i < RECV_DES_CNT; i++)
	{
	pDrv->recvRing[i].addr = 
			(unsigned long)&pDrv->recvBuf[i * ROUND_BUF_SIZE];
	pDrv->recvRing[i].bcnt = -(ROUND_BUF_SIZE);
	pDrv->recvRing[i].mcnt = 0;
	pDrv->recvRing[i].flags = RFLG_OWN; /* give buffer to ilac */
	}

    /* set up ilac init block */

    pDrv->initBlk->mode = 0; /* IBM_PROM; 0 */
    pDrv->initBlk->rlen = L2RECV_DES_CNT;
    
    /* Set the receive descriptor ring address */
    pDrv->initBlk->rdra = (unsigned long)pDrv->recvRing;
    
    /* Set the transmit descriptor ring length */
    pDrv->initBlk->tlen = L2XMIT_DES_CNT;

    /* Set the tranmit descriptor ring address */
    pDrv->initBlk->tdra = (unsigned long)pDrv->tranRing;
    
    /* grab the hardware enet address */
    for (i = 0; i < 6; i++)
	{
	ethernet_num[i] = ilacEnetAddr[i];
	} 

    pDrv->initBlk->padr[3] = ethernet_num[0];
    pDrv->initBlk->padr[2] = ethernet_num[1];
    pDrv->initBlk->padr[1] = ethernet_num[2];
    pDrv->initBlk->padr[0] = ethernet_num[3];
    pDrv->initBlk->padr[7] = ethernet_num[4];
    pDrv->initBlk->padr[6] = ethernet_num[5];

    /* CSR4 controls selected diagnostic functions, microprocessor bus  */
    /* acquisition and DMA signaling */
    ilac->rap = 4;
    ilac->rdp = CSR4_BACON_68K | CSR4_TXSTRTM | CSR4_LBDM;
    ilac->rap = 3;
    ilac->rdp = CSR3_ACON | CSR3_IDONM | CSR3_TINTM;
 
    /* CSR2 must contain the high order 16 bits of the first word in */
    /* the initialization block */
    ilac->rap = 2;
    ilac->rdp = (((int)pDrv->initBlk & 0xff0000) >> 16);
    
    /* CSR1 must contain the low order 16 bits of the first word in */
    /* the initialization block */
    ilac->rap = 1;
    ilac->rdp = ((int)pDrv->initBlk & 0xffff);
    
    ilac->rap = 0;
    ilac->rdp = CSR0_INIT | CSR0_INEA;
    
    count = 0;  
    while (!((csr0 = ilac->rdp) & CSR0_IDON) && count < INIT_TMO)
	{
	count++;
	}

    if (count >= INIT_TMO)
	{
	printf ("ilac: INIT timeout: CSR0 = 0x%x\n", (unsigned short)csr0);
	return ERROR;
	}
    else
	{
	count = 0;
	ilac->rdp = CSR0_STRT | CSR0_INEA;
	while (!((csr0 = ilac->rdp) & (CSR0_RXON | CSR0_TXON)) &&
		count < STRT_TMO)
	    {
	    count++;
	    }
	if (count >= STRT_TMO)
	    {
	    printf ("ilac: STRT timeout: CSR0 = 0x%x\n", (unsigned short)csr0);
	    return ERROR;
	    }
	}
    
    pDrv->recvIndex = 0; 
    pDrv->xmitIndex = 0;
    
    return OK;
    }

/*******************************************************************************
*
* ilacInt - ILAC interrupt service routine
*
* RETURNS:
*/

LOCAL void ilacInt
    (
    int unit
    )
    {
    int       status;
    DRV_CTRL *pDrv = (DRV_CTRL *)&drvCtrl[0];

    /* get csr0 */
    pDrv->addr->rap = 0;
    status = pDrv->addr->rdp;
    
    /* ack interrupt and reenable */
    pDrv->addr->rap = 0;
    pDrv->addr->rdp = CSR0_ACKINT | CSR0_INEA;
    
    if (status & CSR0_RINT)	/* handle receive interrput */
	{
	if (!(pDrv->rcvHandling))
	    {
            pDrv->rcvHandling = TRUE;
            netJobAdd ( (FUNCPTR)ilacHandleRecvInt, (int)pDrv, 0, 0, 0, 0);
	    }
	}
    else
	{
	logMsg("ilac0: got unexpected interrupt: 0x%4x\n", status,
		0, 0, 0, 0, 0);
	}
    }

/*******************************************************************************
*
* ilacIoctl - ILAC control functions
*
* RETURNS:
*/

LOCAL int ilacIoctl
    (
    IDR *pIDR,
    int cmd,
    caddr_t data
    )
    {
    STATUS error = OK;
    
    switch (cmd)
	{
        case SIOCSIFADDR:
           ((struct arpcom *)pIDR)->ac_ipaddr = IA_SIN (data)->sin_addr;
            arpwhohas (pIDR, &IA_SIN (data)->sin_addr);
            break;
	    
        case SIOCSIFFLAGS:
            break;

        default:
            error = EINVAL;
	}
    
    return error;
    }

#ifdef BSD43_DRIVER
/*******************************************************************************
*
* ilacOutput - packet transmit routine
*
* RETURNS:
*/

LOCAL STATUS ilacOutput
    (
    IDR *pIDR,
    MBUF *pMbuf,
    SOCK *pDest
    )
    {
    return ether_output ((IFNET *)pIDR, pMbuf, pDest, (FUNCPTR) ilacTxStartup,
			pIDR);
    }
#endif

/*******************************************************************************
*
* ilacReset - ILAC reset device routine
*
*
* RETURNS:
*/

LOCAL STATUS ilacReset
    (
    int unit
    )
    {
    logMsg ("ilac%d: reset called", unit, 0, 0, 0, 0, 0);
    
    return OK;
    }

/*******************************************************************************
*
* ilacHandleRecvInt - task level portion of packet receive
*
* RETURNS:
*/

LOCAL void ilacHandleRecvInt
    (
    DRV_CTRL *pDrv
    )
    {
    MBUF * 		pMbuf;
    int 		length;
    unsigned char 	packet[ROUND_BUF_SIZE];
    ETH_HDR * 		pEh;
    unsigned char * 	pData;
#ifdef BSD43_DRIVER
    unsigned short 	type;
#endif

    pDrv->rcvHandling = TRUE;

    while (ilacReceive (pDrv, packet, &length) != ERROR)
	{
	++pDrv->idr.ac_if.if_ipackets;

	pEh = (ETH_HDR *)packet;
	
	if ( (etherInputHookRtn != NULL) &&
	     (*etherInputHookRtn) (&pDrv->idr, (char *)pEh, length))
            continue;
	
        length -= sizeof (ETH_HDR);
	pData = (unsigned char *) (packet + sizeof (ETH_HDR));
#ifdef BSD43_DRIVER
        type = ntohs (pEh->ether_type);
#endif
        pMbuf = copy_to_mbufs (pData, length, 0, &pDrv->idr);

        if (pMbuf != NULL)
#ifdef BSD43_DRIVER
            do_protocol_with_type(type, pMbuf, &pDrv->idr, length);
#else
            do_protocol (pEh, pMbuf, &pDrv->idr, length);
#endif
        }
    pDrv->rcvHandling = FALSE; 
    }

/*******************************************************************************
*
* ilacTxStartup - startup transmitter
*
*
* RETURNS:
*/

#ifdef BSD43_DRIVER
LOCAL void ilacTxStartup
    (
    int unit
    )
    {
    DRV_CTRL     *pDrv = &drvCtrl[unit];
#else
LOCAL void ilacTxStartup
    (
    DRV_CTRL     *pDrv
    )
    {
#endif
    MBUF         *pMbuf;
    int           length;
    unsigned char packet[ROUND_BUF_SIZE];
    STATUS result;

    /*
     * Loop until there are no more packets ready to send or we
     * have insufficient resources left to send another one.
     */
    
    while (pDrv->idr.ac_if.if_snd.ifq_head)
	{
        IF_DEQUEUE (&pDrv->idr.ac_if.if_snd, pMbuf);  /* dequeue a packet */

        copy_from_mbufs ( (ETH_HDR *)packet, pMbuf, length);

        if ( (etherOutputHookRtn != NULL) &&
             (*etherOutputHookRtn) (&pDrv->idr, (ETH_HDR *)packet, length))
            continue;
	
        result = ilacTransmit (pDrv, packet, length);
        if (result == ERROR)
            pDrv->idr.ac_if.if_oerrors++;
#ifndef BSD43_DRIVER    /* BSD 4.4 ether_output doesn't bump statistic. */
        else
            pDrv->idr.ac_if.if_opackets++; 
#endif
	}
    }

/*******************************************************************************
*
* ilacReceive - receive packet
*
*
* RETURNS:
*/

LOCAL STATUS ilacReceive
    (
    DRV_CTRL *pDrv,
    unsigned char *packet,
    int *len
    )
    {
    RM_DESC      *rmd = (RM_DESC *)&pDrv->recvRing[pDrv->recvIndex]; 

    
    if (rmd->flags & RFLG_OWN)
	{
	return ERROR;
	} 

    if (rmd->flags & RFLG_ERR)
	{ 
	pDrv->recvIndex++; 
	pDrv->recvIndex %= RECV_DES_CNT;
	
	rmd->flags |= RFLG_OWN;
	
	logMsg("ilac0: recv ERROR: 0x%x\n", rmd->flags, 0, 0, 0, 0, 0); 
	return ERROR;  
	}
    
    *len = rmd->mcnt; 
    
    bcopy((char *)rmd->addr, (char *)packet, *len);

    pDrv->recvIndex++; 
    pDrv->recvIndex %= RECV_DES_CNT;

    /* ilac can have this buffer back again */
    rmd->mcnt = 0; 
    rmd->flags |= RFLG_OWN;
    
    return OK;
    } 

/*******************************************************************************
*
* ilacTransmit - actual packet transmission
*
*
* RETURNS:
*/

LOCAL STATUS ilacTransmit
    (
    DRV_CTRL *pDrv,
    unsigned char *packet,
    int len
    )
    {
    unsigned int   count;
    unsigned char  status;
    
    TM_DESC *tmd;

    tmd = (TM_DESC *)&pDrv->tranRing[pDrv->xmitIndex]; 	
    
    bcopy ((const char *)packet, (char *)tmd->addr, len);
    
    len = (len < MINPKTSIZE) ? MINPKTSIZE : len;

    tmd->bcnt = TBCNT_ONES | -(len);
    tmd->error = 0;
    tmd->status = TST_XMIT;
    
    count = 0;
    while ( (status = tmd->status) & TST_OWN)
	if (count++ > XMIT_TMO)
	    return ERROR;

    if (status & TST_ERR)
	{
	pDrv->xmitIndex++; 
	pDrv->xmitIndex %= XMIT_DES_CNT;
	
	if ( (tmd->error << 10) & TERR_LCAR)
	    logMsg ("ilac%d: lost carrier\n", pDrv->idr.ac_if.if_unit,
		    0, 0, 0, 0, 0);
	else
	    logMsg ("ilac%d: ERROR = 0x%x\n", pDrv->idr.ac_if.if_unit,
		    tmd->error, 0, 0, 0, 0); 
	return ERROR;
	}

    pDrv->xmitIndex++; 
    pDrv->xmitIndex %= XMIT_DES_CNT;

    return OK;
    }
