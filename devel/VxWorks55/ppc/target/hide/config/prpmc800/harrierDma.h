/* harrierDma.h - Harrier DMA Controller definitions file */

/* Copyright 2001 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01a,18jun01,srr  Initial Creation.
*/


/*
The DMA Descriptor List contains six fields which must be cache-line aligned
and is described here in detail.  If the Descriptor List Next Link Address
(DNLA) entry is 0, the Harrier DMA driver will configure the transfer for
Direct Mode.

.CS
                        ------------------------------------------
                        |            Bit Positions               |
             -----------------------------------------------------
             | Offset   |0               31|32                 63|
             -----------------------------------------------------
             |  0x00    |      DSAD        |         DSAT        |
             -----------------------------------------------------
             |  0x08    |      DDAD        |         DDAT        |
             -----------------------------------------------------
             |  0x10    |      DNLA        |         DCNT        |
             -----------------------------------------------------
.CE

DSAD bits 0:31
    This contains the 32-bit DMA transfer source address.
    This is either a PCI address, PowerPC address or a data pattern.
    If it is a byte data pattern, the byte is in bit positions 24 thru 31.


.CS
DSAT bits  2:3  Type
             7  No Increment (NIN)
             15 Pattern Size (PSZ)
          20:23 PCI Read Command (PRC)
             29 Cache-Line Read Invalidate (CRI)
             31 Global (GBL)
.CE

    This contains the source attributes for a DMA transfer.

    Type - Indicates type of source for a transfer:
.CS
                                 ---------------------------------
                                 |       Applicable Fields       |
             -----------------------------------------------------
             | Type | DMA Source |  NIN  |  PSZ  |  CRI  |  GBL  |
             -----------------------------------------------------
             |  00  | PowerPC bus|       |       |   X   |   X   |
             -----------------------------------------------------
             |  01  |   PCI bus  |   X   |       |       |       |
             -----------------------------------------------------
             |  1x  |Data Pattern|   X   |   X   |       |       |
             -----------------------------------------------------

    NIN - If set, source increment will be disabled.
    PSZ - If set, data pattern transfers will be bytes.
    PRC - If Type is set to PCI bus (01), the command used
          during PCI read cycles is:
             -----------------------------------------------------
             |  PRC  |             PCI Command                   |
             -----------------------------------------------------
             | 0010  |               IO Read                     |
             -----------------------------------------------------
             | 0110  |             Memory Read                   |
             -----------------------------------------------------
             | 1100  |         Memory Read Multiple              |
             -----------------------------------------------------
             | 1110  |           Memory Read Line                |
             -----------------------------------------------------

    CRI - If set, DMA will use "Read-with-intent-to-modify" during
          PowerPC read cycles.  Else "Read" transfer type will be used
          which will retain cached data in the E and S states.

    GBL - If set, DMA will assert GBL_ pin during PowerPC read cycles to
          allow the processor to snoop DMA.
.CE


DDAD bits 0:31
    This contains the 32-bit DMA transfer destination address.
    This is either a PCI or PowerPC address.


.CS
DDAT bits    3  Type
             7  No Increment (NIN)
          20:23 PCI Write Command (PWC)
             30 Cache-Line Write Flush (CWF)
             31 Global (GBL)
.CE

    This contains the destination attributes for a DMA transfer.

    Type - Indicates type of source for a transfer:
.CS
                                 ---------------------------------
                                 |       Applicable Fields       |
             -----------------------------------------------------
             | Type | DMA Source |  NIN  |       |  CFW  |  GBL  |
             -----------------------------------------------------
             |   0  | PowerPC bus|       |       |   X   |   X   |
             -----------------------------------------------------
             |   1  |   PCI bus  |   X   |       |       |       |
             -----------------------------------------------------

    NIN - If set, destination increment will be disabled.
    PWC - If Type is set to PCI bus (01), the command used
          during PCI write cycles is:
             -----------------------------------------------------
             |  PRC  |             PCI Command                   |
             -----------------------------------------------------
             | 0011  |               IO Write                    |
             -----------------------------------------------------
             | 0111  |             Memory Write                  |
             -----------------------------------------------------
             | 1111  |       Memory Write and Invalidate         |
             -----------------------------------------------------

    CWF - If set, DMA will use "Write-with-flush" during PowerPC burst
          write cycles to force copyback writes during snoop hits.
          Else "Write-with-kill" transfer type will be used to force
          cache entries to be invalidated during snoop hits.

    GBL - If set, DMA will assert GBL_ pin during PowerPC
          write cycles to allow the processor to snoop DMA.
.CE


.CS
DNLA bits  0:26 Next Link Address (NLA)
             31 Last Link Address (LLA)
.CE

    This contains the 27-bit Most significant bits of the next Linked-List
    Mode Descriptor.  If the Descriptor List Next Link Address (DNLA) entry
    is 0, the Harrier DMA driver will configure the transfer for Direct Mode.

    NLA - The next descriptor address in 32-byte cache-line resolution.

    LLA - If set, this is the last descriptor in the list.


DCNT bits 0:31
    This contains the byte count for the DMA transfer.


Here is the format of the DMA Status Register:

.CS
DSTA bits 0  Signalled Master Abort (SMA)
          1  Received Target Abort (RTA)
          2  Maximum Retry Count (MRC)
          3  PowerPC Bus Time-out (XBT)
          4  Abort (ABT)
          5  Pause (PAU)
          6  Done (DON)
          7  Busy (BSY)
.CE
*/

#ifndef	INCharrierDma
#define	INCharrierDma

#ifdef __cplusplus
   extern "C" {
#endif

typedef struct harrierDmaDescriptor
    {
    UINT32 dsad;			/* DMA Source Address Register */
    UINT32 dsat;			/* DMA Source Attribute Register */
    UINT32 ddad;			/* DMA Destination Address Register */
    UINT32 ddat;			/* DMA Destination Attribute Register */
    UINT32 * pDnla;			/* DMA Next Link Address Register */
    UINT32 dcnt;			/* DMA Count Register */
    } HARRIER_DMA_DESCRIPTOR;

#ifdef __cplusplus
    }
#endif

#endif /* INCharrierDma */
