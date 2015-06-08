/* sysDma.h - Device independent DMA Header File */

/* Copyright 2002 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01b,05nov03,cak  BSP update.
01a,30oct02,cak  Ported from ver 01a, hxeb100/sysDma.h.
*/

/*
DESCRIPTION
This file contains the device independent DMA definitions and prototypes.
*/

#ifndef	INCsysDmah
#define	INCsysDmah

#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */

/*
 * The following structure defines the basic DMA transfer parameters.
 */

typedef struct sysDmaDesc
    {
    UINT32 byteCount;      /* Number of bytes to transfer */
    UINT32 sourceAddr;     /* Starting address of source data */
    UINT32 destAddr;       /* Starting address of destination */
    UINT32 nextDescPtr;    /* Next descriptor pointer */
    } DMA_DESCRIPTOR;

/*
 * The following structure defines user controlled attributes for a
 * given DMA transfer. The userHandler routine provides an optional
 * hook for notification upon completion interrupt occurrence. The
 * value of this field provides three notification options based on
 * its value: Wait in the DMA start routine until the completion 
 * interrupt occurs (userHandler == WAIT_FOREVER); return immediately
 * from the start routine, but call a user routine from the interrupt 
 * handler upon DMA completion (userHandler == user routine); return 
 * immediately without a user interrupt handler call (userHandler == 0). 
 */

typedef void (*HANDLER)(UINT32 chan);
typedef struct sysDmaAttrib
    {
    BOOL    holdSourceAddr;   /* Do not increment the source address */
    BOOL    holdDestAddr;     /* Do not increment the destination address */
    HANDLER userHandler;      /* User defined interrupt completion routine */
    } DMA_ATTRIBUTES;


STATUS sysDmaInit (void);

STATUS sysDmaStart (UINT32 chan, DMA_DESCRIPTOR *dmaDesc, 
                    DMA_ATTRIBUTES *dmaAttrib, void *dmaStatus);

STATUS sysDmaStatus (UINT32 chan, void *dmaStatus);

STATUS sysDmaAbort (UINT32 chan);

STATUS sysDmaPause (UINT32 chan);

STATUS sysDmaResume (UINT32 chan);


#ifdef __cplusplus
    }
#endif /* __cplusplus */
#endif /* INCsysDmah */
