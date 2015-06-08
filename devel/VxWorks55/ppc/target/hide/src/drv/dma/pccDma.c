/* pccDma.c - Peripheral Channel Controller (PCC) DMA library */

/* Copyright 1984-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
02g,24apr02,pmr  SPR 74390: added include of header fcntlcom.h for O_RDONLY.
02f,18nov96,dds  SPR 7398: removed the calls to cacheInvalidate & cacheFlush
                 for SCSI-2. This is handled in scsi2Lib.c
02e,18nov96,dds  removed the calls to cacheInvalidate & cacheFlush
                 for SCSI-2. This is handled in scsi2Lib.c
02d,11oct96,dds  merged from vxw5_3_x.scsi2 branch.
02c,19aug96,dds  removed compiler warnings.
02b,22may96,dds  added code to transfer the contents of the DMA holding
                 register to memory during a sysScsiDmaAbort(). During
		 a tape read 1,2 or 3 bytes get left in the DMA holding
		 register. 
02a,10aug95,jds	 integrated changes for dma routines for SCSI-2. Should be
		 compatible for both SCSI-1 and SCSI-2.
01g,06jun96,wlf  doc: cleanup; (per jds) sysScsiDmaAbort() made NOMANUAL.
01f,12nov94,dzb  fixed c&p error.
01e,12oct93,jds  merged SCSI1 pccDma.c 
01d,09aug93,ccc  added SCSI disconnect support.
01c,27jul92,ccc  added cache support for SCSI DMA routines.
01b,11jul92,ccc  fixed function headers, and changed READ and WRITE
		 to O_RDONLY and O_WRONLY.
01a,11jun92,ccc  created by moving routines from mv147 sysLib.c, ansified.
*/

/*
DESCRIPTION
These routines provide DMA transfer for the PCC chip on the mv147.
*/
#ifndef INCLUDE_SCSI2
#ifdef INCLUDE_SCSI_DMA

#include "cacheLib.h"
#include "sys/fcntlcom.h"

#define	DMA_FUNCTION_CODE	(0x5 << 24)

/*******************************************************************************
*
* sysScsiDmaTransfer - start the DMA data transfer
*
* This routines initializes the DMA, and starts the transfer
*
* NOMANUAL
*/

void sysScsiDmaTransfer
    (
    UINT8 *pBuffer,		/* ptr to the data buffer  */
    int    bufLength,		/* number of bytes to xfer */
    int    direction		/* direction of transfer   */
    )
    {
    *SCSI_PCC_DMA_ADRS = (long) pBuffer;
    *SCSI_DMA_BYTE_COUNT = (long) (bufLength | DMA_FUNCTION_CODE);

    if (direction == O_RDONLY)
	{
	cacheInvalidate (DATA_CACHE, pBuffer, bufLength);
	*SCSI_DMA_CSR = DMA_CSR_ENABLE | DMA_CSR_READ;
	}
    else
	{
	cacheFlush (DATA_CACHE, pBuffer, bufLength);
	*SCSI_DMA_CSR = DMA_CSR_ENABLE | DMA_CSR_WRITE;
	}
    }

/********************************************************************************
*
* sysWdDmaBytesIn - input SCSI data bytes using on-board DMA
*
* This routine gets passed to the wd33c93CtrlCreate() call when DMA should
* be used for SCSI DATA IN transfers.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: wd33c93Lib
*
* NOMANUAL
*/

STATUS sysWdDmaBytesIn
    (
    SCSI_PHYS_DEV *pScsiPhysDev,       /* ptr to phys dev info    */
    UINT8 *pBuffer,                    /* ptr to the data buffer  */
    int bufLength                      /* number of bytes to xfer */
    )
    {
    FAST WD_33C93_SCSI_CTRL *pSbic;     /* ptr to SBIC info */

    pSbic = (WD_33C93_SCSI_CTRL *) pScsiPhysDev->pScsiCtrl;

    *pSbic->pAdrsReg = SBIC_REG_CONTROL;
    *pSbic->pRegFile = SBIC_CTRL_DMA_BYTE;

    if (bufLength != 1)
        {
        if (sbicXferCountSet (pSbic, (UINT) bufLength) == ERROR)
            return (ERROR);
 
        sbicCommand (pSbic, SBIC_CMD_XFER_INFO);
        }
    else
        sbicCommand (pSbic, SBIC_CMD_XFER_INFO | SBIC_CMD_SING_BYTE_XFER);

    sysScsiDmaTransfer (pBuffer, bufLength, O_RDONLY); 
    return (OK);
    }
 
/********************************************************************************
*
* sysWdDmaBytesOut - output SCSI data bytes using on-board DMA
*
* This routine passes control to wd33c93CtrlCreate() when DMA should
* be used for SCSI DATA OUT transfers.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO: wd33c93Lib
*
* NOMANUAL
*/
 
STATUS sysWdDmaBytesOut
    (
    SCSI_PHYS_DEV *pScsiPhysDev,       /* ptr to phys dev info    */
    UINT8 *pBuffer,                    /* ptr to the data buffer  */
    int bufLength                      /* number of bytes to xfer */
    )
    {
    FAST WD_33C93_SCSI_CTRL *pSbic;     /* ptr to SBIC info */
 
    pSbic = (WD_33C93_SCSI_CTRL *) pScsiPhysDev->pScsiCtrl;
 
    *pSbic->pAdrsReg = SBIC_REG_CONTROL;
    *pSbic->pRegFile = SBIC_CTRL_DMA_BYTE;
 
    if (bufLength != 1)
        {
        if (sbicXferCountSet (pSbic, (UINT) bufLength) == ERROR)
            return (ERROR);
 
        sbicCommand (pSbic, SBIC_CMD_XFER_INFO);
        }
    else
        sbicCommand (pSbic, SBIC_CMD_XFER_INFO | SBIC_CMD_SING_BYTE_XFER);

    sysScsiDmaTransfer (pBuffer, bufLength, O_WRONLY);
    return (OK);
    }
 
/********************************************************************************
*
* sysScsiDmaIntr - MVME147 SCSI DMA interrupt routine
*
* This routine connects to the interrupt vector when handling SCSI DMA
* operations.
*
* RETURNS: N/A.
*
* NOMANUAL
*/
 
void sysScsiDmaIntr
    (
    WD_33C93_SCSI_CTRL *pSbic     /* ptr to SBIC info */
    )
    {
    FAST char dmaIntCtrl;
    FAST char dmaCsr;
 
    dmaIntCtrl = *SCSI_DMA_INT_CTL;
    dmaCsr     = *SCSI_DMA_CSR;
 
    if (scsiIntsDebug)
        {
        logMsg ("sysScsiDmaIntr called.\n", 0, 0, 0, 0, 0, 0);
 
        logMsg ("SCSI_DMA_INT_CTL (0x20) = 0x%02x\n", dmaIntCtrl,
		0, 0, 0, 0, 0);
 
        logMsg ("SCSI_DMA_CSR     (0x21) = 0x%02x\n", *SCSI_DMA_CSR,
		0, 0, 0, 0, 0);
        }
 
    if (dmaCsr & DMA_INT_CTL_PENDING)
        {
        *SCSI_DMA_INT_CTL = dmaIntCtrl |
                            DMA_INT_CTL_CLEAR;  /* reset interrupt */
 
        *SCSI_DMA_CSR     = DMA_CSR_DISABLE;    /* disable DMA */
        }
    else
        logMsg ("DMA not DONE.\n", 0, 0, 0, 0, 0, 0);
    }
       
#endif /* INCLUDE_SCSI_DMA */
#endif  /* #ifndef INCLUDE_SCSI2 */

#ifdef INCLUDE_SCSI2

#define SCSI_DMA_HOLDING_REG      ((long *) (PCC_BASE_ADRS +  0x0c))

#ifdef INCLUDE_SCSI
LOCAL BOOL sysScsiBusWasReset   = FALSE;
LOCAL UINT bufferLength;	/* original length of current dma operation */
#endif /* INCLUDE_SCSI */

#ifdef INCLUDE_SCSI_DMA

#include "cacheLib.h"
#include "sys/fcntlcom.h"

#define	DMA_FUNCTION_CODE	(0x5 << 24)

LOCAL BOOL sysScsiDmaInProgress = FALSE;

/*******************************************************************************
*
* sysScsiDmaStart - start DMA transfer to/from on-board SCSI controller
*
* This routine gets passed to the wd33c93CtrlCreate() call when DMA should
* be used for SCSI DATA IN and DATA out transfers.
*
* Starts the DMA transfer and returns immediately (i.e., without waiting for
* the transfer to complete).  May be called only at task level.
*
* Note that the SCSI library is responsible for properly serialising all SCSI
* DMA operations (in particular, starts and aborts) so it is an error for this
* routine to be called if a transfer is already in progress.
*
* RETURNS: OK | ERROR.
*
* NOMANUAL
*/

STATUS sysScsiDmaStart
    (
    int    arg,                         /* call-back argument (ignored) */
    UINT8 *pBuffer,                     /* ptr to the data buffer */
    UINT   bufLength,                   /* number of bytes to xfer */
    int    direction                    /* 0 => read, <>0 => write */
    )
    {
    SCSI_DEBUG_MSG ("sysScsiDmaStart: %d bytes %s\n", bufLength,
                    (direction == READ) ? (int)"in" : (int)"out", 0, 0, 0, 0);

    if ((bufLength == 0) || (bufLength > 0x00ffffff))
        {
        SCSI_DEBUG_MSG ("sysScsiDmaStart: invalid byte count (%u)\n",
                        bufLength, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    bufferLength = bufLength;

    /*
     *  No interrupt lock needed here as there is no way a DMA interrupt can
     *  occur unless a transfer is already in progress (in which case we
     *  don't touch anything).
     */

    if (sysScsiDmaInProgress)
        {
        SCSI_DEBUG_MSG ("sysScsiDmaStart: DMAC already in use\n",
			0, 0, 0, 0, 0, 0);
        return (ERROR);
        }

    sysScsiDmaInProgress = TRUE;

    *SCSI_PCC_DMA_ADRS   = (long) pBuffer;
    *SCSI_DMA_BYTE_COUNT = (long) (bufLength | DMA_FUNCTION_CODE);

    if (direction == O_RDONLY)
        {
        *SCSI_DMA_CSR = DMA_CSR_ENABLE | DMA_CSR_READ;
        }
    else
        {
        *SCSI_DMA_CSR = DMA_CSR_ENABLE | DMA_CSR_WRITE;
        }

    return (OK);
    }

/*******************************************************************************
*
* sysScsiDmaAbort - abort current DMA transfers to or from the SCSI controller
*
* This routine gets passed to wd33c93CtrlCreate() when DMA should
* be used for SCSI data-in and data-out transfers.
*
* If a DMA transfer is in progress when this routine is called, it is
* aborted with a write to the DMAC CSR.  This routine can be called at task or
* interrupt level.
*
* NOTE: The SCSI library never calls this routine without the successful return
* of a prior call to sysScsiDmaStart().
*
* RETURNS: OK or ERROR.
*
* NOMANUAL
*/


STATUS sysScsiDmaAbort
    (
    int arg                            /* call-back argument (ignored) */
    )
    {
    int lockKey;
    char *memLoc;
    UINT bytesToXfer;
    union
	{
	long holdRegLong;
	char holdRegBytes[4];
	} holdReg;

    int i=0;
 
    SCSI_INT_DEBUG_MSG ("sysScsiDmaAbort:\n", 0, 0, 0, 0, 0, 0);
 
    lockKey = intLock ();      /* avoid race with DMA complete ISR */
 
    /* Bytes to tranfer from the DMA holding register to memory */
    bytesToXfer =  (bufferLength - (*SCSI_DMA_BYTE_COUNT & 0x00ffffff)) % 4;

    holdReg.holdRegLong = *((long *) SCSI_DMA_HOLDING_REG);

    memLoc = (char *) (*SCSI_PCC_DMA_ADRS);

    while (bytesToXfer--)
	{
	*memLoc++ = holdReg.holdRegBytes[i++];
	}
 
    if (sysScsiDmaInProgress)
	{
	*SCSI_DMA_CSR = (char) DMA_CSR_DISABLE;

	sysScsiDmaInProgress = FALSE;
	}
 
    intUnlock (lockKey);
 
    return (OK);
    }

/********************************************************************************
*
* sysScsiDmaIntr - MVME147 SCSI DMA interrupt routine
*
* This routine connects to the interrupt vector when handling SCSI DMA
* operations.
*
* RETURNS: N/A.
*
* NOMANUAL
*/
 
void sysScsiDmaIntr
    (
    WD_33C93_SCSI_CTRL *pSbic     /* ptr to SBIC info */
    )
    {
    int lockKey;
    FAST char dmaIntCtrl;
    FAST char dmaCsr;
 
    dmaIntCtrl = *SCSI_DMA_INT_CTL;
    dmaCsr     = *SCSI_DMA_CSR;
 
    if (scsiIntsDebug)
        {
        logMsg ("sysScsiDmaIntr called.\n", 0, 0, 0, 0, 0, 0);
 
        logMsg ("SCSI_DMA_INT_CTL (0x20) = 0x%02x\n", dmaIntCtrl,
		0, 0, 0, 0, 0);
 
        logMsg ("SCSI_DMA_CSR     (0x21) = 0x%02x\n", *SCSI_DMA_CSR,
		0, 0, 0, 0, 0);
        }
 
    if (dmaCsr & DMA_INT_CTL_PENDING)
        {
        *SCSI_DMA_INT_CTL = dmaIntCtrl |
                            DMA_INT_CTL_CLEAR;  /* reset interrupt */
        }
    else
        logMsg ("DMA not DONE.\n", 0, 0, 0, 0, 0, 0);

    lockKey = intLock ();		/* avoid rate with DMA abort */

    if (sysScsiDmaInProgress)
	{
	*SCSI_DMA_CSR = DMA_CSR_DISABLE;	/* disable DMA */

	sysScsiDmaInProgress = FALSE;
	}

    intUnlock (lockKey);
    }

#endif  /* INCLUDE_SCSI_DMA */
#endif	/* INCLUDE_SCSI2 */
