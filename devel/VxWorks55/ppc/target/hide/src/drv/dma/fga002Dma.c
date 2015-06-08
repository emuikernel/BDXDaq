/* fga002Dma.c - FGA-002 DMA library */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01c,21jun96,wlf  doc: cleanup.
01b,28may96,dat	 Updated with TechSupport patch for SPR #2211
01a,03aug92,ccc  written for the frc30.
*/

/*
DESCRIPTION
These routines provide DMA transfer for the mb87031 SCSI chip using the
FGA-002 DMA function.
*/

#ifdef INCLUDE_SCSI
#ifdef INCLUDE_SCSI_DMA

#include "cacheLib.h"
#include "logLib.h"

#define	DMA_FUNCTION_CODE	(0x05)

/*******************************************************************************
*
* fgaDmaTransfer - start the DMA data transfer
*
* This routines initializes the DMA, and starts the transfer
*
* NOMANUAL
*/

void fgaDmaTransfer
    (
    UINT8 *pBuffer,		/* ptr to the data buffer  */
    int    bufLength,		/* number of bytes to xfer */
    int    direction		/* direction of transfer   */
    )
    {
    *FGA_DMATRFCNT = (long) bufLength;

    if (direction == O_RDONLY)
	{
	/* FLUSH any shared CACHE lines */

	cacheFlush (DATA_CACHE, pBuffer, 1);
	cacheFlush (DATA_CACHE, pBuffer + bufLength, 1);

	*FGA_DMADSTADR = (long) pBuffer;

	*FGA_DMASRCATT = FGA_DMA_AUX_BUS;

	*FGA_DMADSTATT = FGA_DMA_MAIN_MEMORY |
			 DMA_FUNCTION_CODE;

	*FGA_DMAGENERAL = FGA_DMA_SRC_NO_COUNT |
			  FGA_DMA_DST_COUNTS   |
			  FGA_DMA_ENABLE;
	}
    else
	{
	/* flush entire CACHE if more than 1/2 */
	if (bufLength < 0x800)
	    cacheFlush (DATA_CACHE, pBuffer, bufLength);
	else
	    cacheFlush (DATA_CACHE, 0, ENTIRE_CACHE);

	*FGA_DMASRCADR = (long) pBuffer;

	*FGA_DMASRCATT = FGA_DMA_MAIN_MEMORY |
			 DMA_FUNCTION_CODE;

	*FGA_DMADSTATT = FGA_DMA_AUX_BUS;

	*FGA_DMAGENERAL = FGA_DMA_SRC_COUNTS |
			  FGA_DMA_DST_NO_COUNT |
			  FGA_DMA_ENABLE;
	}

    *FGA_DMARUNCTL = 0x01;	/* start the DMA transfer */

    /*XXX*/
    while ((*FGA_DMARUNCTL & 0x80) == 0x80)
	;

    if (direction == O_RDONLY)
	cacheInvalidate (DATA_CACHE, pBuffer, bufLength);
    }

/********************************************************************************
* fgaDmaBytesIn - input SCSI data bytes using on-board DMA
*
* This routine gets passed to the mb87030CtrlCreate() call when DMA should
* be used for SCSI DATA IN transfers.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: mb87030Lib
*
* NOMANUAL
*/

STATUS fgaDmaBytesIn
    (
    SCSI_PHYS_DEV *pScsiPhysDev,       /* ptr to phys dev info    */
    UINT8 *pBuffer,                    /* ptr to the data buffer  */
    int bufLength                      /* number of bytes to xfer */
    )
    {
    fgaDmaTransfer (pBuffer, bufLength, O_RDONLY);
    return (OK);
    }
 
/********************************************************************************
* fgaDmaBytesOut - output SCSI data bytes using on-board DMA
*
* This routine passes control to mb87030CtrlCreate() when DMA should
* be used for SCSI DATA OUT transfers.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: mb87030Lib
*
* NOMANUAL
*/
 
STATUS fgaDmaBytesOut
    (
    SCSI_PHYS_DEV *pScsiPhysDev,       /* ptr to phys dev info    */
    UINT8 *pBuffer,                    /* ptr to the data buffer  */
    int bufLength                      /* number of bytes to xfer */
    )
    {
    fgaDmaTransfer (pBuffer, bufLength, O_WRONLY);
    return (OK);
    }
 
/********************************************************************************
* fgaDmaIntr - Force FGA-002 SCSI DMA interrupt routine
*
* This routine connects to the interrupt vector when handling SCSI DMA
* operations.
*
* RETURNS: N/A.
*
* NOMANUAL
*/
 
void fgaDmaIntr
    (
    MB_87030_SCSI_CTRL *pSbic     /* ptr to SBIC info */
    )
    {
    if (scsiIntsDebug)
        logMsg ("fgaDmaIntr called.\n", 0, 0, 0, 0, 0, 0);

    *FGA_ISDMANORM = 0x00;	/* write anything to clear interrupt */

    *pSbic->pSctlReg |= SPC_SCTL_INT_ENBL;  /* spcIntsEnable (pSpc); */
    }
       
#endif  /* INCLUDE_SCSI_DMA */
#endif	/* INCLUDE_SCSI */
