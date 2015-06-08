/* frcEagle01.c - Force EAGLE-01C module library */

/* Copyright 1984-1992 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01e,24jun96,wlf  doc: cleanup.
01d,20oct92,caf  changed sysEagle01Init() to sysEagleInit(), made it NOMANUAL.
		 included fga002Dma.c.
01c,26sep92,ccc  moved SCSI interrupt enable after mb87030CtrlInit().
01b,22aug92,ccc  fixed sysScsiInit() for new Eagle-01.
01a,16jul92,caf  created.
*/

/*
DESCRIPTION
This library contains routines to manipulate the Force EAGLE-01C
module.  This library addresses initialization, plus the SCSI and
LANCE Ethernet functions of the EAGLE-01C.
*/

#include "dma/fga002Dma.c"

/*******************************************************************************
*
* sysEagleInit - initialize the Force EAGLE-01C module hardware
*
* RETURNS: N/A
*
* NOMANUAL
*/

void sysEagleInit (void)
    {
    *FGA_ICRLOCAL1 = INT_LVL_FDC | FGA_ICR_AUTOCLEAR;
    *FGA_ICRLOCAL6 = INT_LVL_LANCE;
    *FGA_ICRLOCAL7 = INT_LVL_SCSI | FGA_ICR_AUTOCLEAR | FGA_ICR_ACTIVITY;
    }

/*******************************************************************************
*
* sysLanIntEnable - enable a LAN interrupt level
*
* This routine enables interrupts for the on-board LAN chip at a specified
* level.  LAN interrupts are controlled by the FGA-002 chip.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysLanIntDisable()
*/

STATUS sysLanIntEnable
    (
    int intLevel        /* interrupt level to enable */
    )
    {
    /* unmask local LANCE interrupt */

    *FGA_ICRLOCAL6 |= FGA_ICR_ENABLE;

    return (OK);
    }

/*******************************************************************************
*
* sysLanIntDisable - disable a LAN interrupt level
*
* This routine disables a specified interrupt level for the on-board LAN chip.
* LAN interrupts are controlled by the FGA-002 chip.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysLanIntEnable()
*/

STATUS sysLanIntDisable (void)

    {
    /* mask LANCE interrupt */

    *FGA_ICRLOCAL6 &= ~FGA_ICR_ENABLE;

    return (OK);
    }

#ifdef	INCLUDE_SCSI
#ifdef	INCLUDE_SCSI_DMA
/******************************************************************************
*
* sysDmaBytesIn - initialize the DMA data direction
*
* This routine initializes the DMA data direction then calls fgaDmaBytesIn().
*
* RETURNS: The return value from fgaDmaBytesIn().
*
* NOMANUAL
*/

STATUS sysDmaBytesIn
    (
    SCSI_PHYS_DEV *pScsiPhysDev,	/* ptr to phys dev info    */
    UINT8 *pBuffer,			/* ptr to the data buffer  */
    int bufLength			/* number of bytes to xfer */
    )
    {
    *FRC_SCSI_DMA_CR = FRC_SCSI_DMA_READ;
    return (fgaDmaBytesIn (pScsiPhysDev, pBuffer, bufLength));
    }

/******************************************************************************
*
* sysDmaBytesOut - initialize the DMA data direction
*
* This routine initialized the DMA data direction then calls fgaDmaBytesOut().
*
* RETURNS: The return value from fgaDmaBytesOut().
*
* NOMANUAL
*/

STATUS sysDmaBytesOut
    (
    SCSI_PHYS_DEV *pScsiPhysDev,	/* ptr to phys dev info    */
    UINT8 *pBuffer,			/* ptr to the data buffer  */
    int bufLength			/* number of bytes to xfer */
    )
    {
    *FRC_SCSI_DMA_CR &= ~FRC_SCSI_DMA_READ;	/* set for write */
    return (fgaDmaBytesOut (pScsiPhysDev, pBuffer, bufLength));
    }
#endif	/* INCLUDE_SCSI_DMA */
/******************************************************************************
*
* sysScsiInit - initialize an on-board SCSI port 
*
* This routine creates and initializes a Fujitsu SPC structure, enabling use
* of the on-board SCSI port.  It connects the proper interrupt service
* routine to the desired vector, and enables the interrupt at the desired
* level.
*
* RETURNS: OK, or ERROR if the SPC structure cannot be connected, the
* controller cannot be initialized, or the interrupt service routine cannot be
* connected to the interrupt.
*/

STATUS sysScsiInit (void)

    {
    pSysScsiCtrl = (SCSI_CTRL *) mb87030CtrlCreate (FRC40_SPC_BASE_ADRS,
						    FRC40_SPC_REG_OFFSET,
						    FRC40_SPC_CLK_PERIOD,
						    FRC40_SPC_PARITY,
#ifdef	INCLUDE_SCSI_DMA
						    (FUNCPTR) sysDmaBytesIn,
						    (FUNCPTR) sysDmaBytesOut);
#else	/* INCLUDE_SCSI_DMA */
						    (FUNCPTR) NULL,
						    (FUNCPTR) NULL);
#endif	/* INCLUDE_SCSI_DMA */

    if (pSysScsiCtrl == NULL)
	return (ERROR);

    /* connect the SCSI controller's interrupt service routine */

    if (intConnect (INUM_TO_IVEC (INT_VEC_SCSI), spcIntr, (int) pSysScsiCtrl)
	== ERROR)
	return (ERROR);

    /* initialize SCSI controller with default parameters (user tunable) */

    if (mb87030CtrlInit ((MB_87030_SCSI_CTRL *)pSysScsiCtrl,
		         SCSI_DEF_CTRL_BUS_ID,
		         SCSI_DEF_SELECT_TIMEOUT,
	         	 NONE) == ERROR)
        {
	return (ERROR);
        }

    /* enable the SPC interrupt */
    *FGA_ICRLOCAL7 |= FGA_ICR_ENABLE;

#ifdef	INCLUDE_SCSI_DMA

    /* connect the DMA controller's interrupt service routine */

    if (intConnect (INUM_TO_IVEC (INT_VEC_SCSI_DMA), fgaDmaIntr,
		    (int) pSysScsiCtrl) == ERROR)
	{
	return (ERROR);
	}

    *FGA_AUXPINCTL = FGA_AUXPIN_NOAUTOREQ |
		     FGA_AUXPIN_RDY_HIGH  |
		     FGA_AUXPIN_ACK_LOW   |
		     FGA_AUXPIN_REQ_HIGH;

    *FGA_AUXDSTSTART = FGA_ASSACK_4CLK |  
		       FGA_RDY_1CLK;

    *FGA_AUXDSTTERM = FGA_RELACK_5CLK |
		      FGA_NEWCYC_6CLK;

    *FGA_AUXFIFWEX = FGA_AUXFIFO_TIMING15;

    *FGA_AUXSRCSTART = FGA_ASSACK_4CLK |
		       FGA_RDY_4CLK;

    *FGA_AUXSRCTERM = FGA_RELACK_1CLK |
		      FGA_NEWCYC_2CLK;
      
    *FGA_ICRDMANORM = FGA_ICR_ENABLE | INT_LVL_SCSI;

#endif  /* INCLUDE_SCSI_DMA */

    return (OK);
    }
#endif	/* INCLUDE_SCSI */
