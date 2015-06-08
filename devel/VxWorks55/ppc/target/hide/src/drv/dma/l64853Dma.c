/* l64853Dma.c - LSI Logic L64853 S-Bus DMA Controller library */

/* Copyright 1989-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01f,29nov96,wlf  doc: cleanup.
01h,01jul96,map  added documentation.
01g,25may96,dat  added #include "stdio.h", to eliminate warnings
01f,25may96,dds  (from jaideep) modified l64853{HwInit,Reset} to do a
                 complete job ...
01f,23may96,wlf  doc: cleanup.
01e,03dec93,ccc  fixed warning by adding include of stdlib.h.
01d,02sep92,ccc  renamed l64853Lib.c to l64853Dma.c.
01c,26may92,rrr  the tree shuffle
		  -changed includes to have absolute path from h/
01b,04oct91,rrr  passed through the ansification filter
                  -changed functions to ansi style
		  -changed includes to have absolute path from h/
		  -changed VOID to void
		  -changed copyright notice
01a,28feb90,jcc  written
*/

/*
DESCRIPTION

This is the driver for the LSI Logic L64853 S-Bus DMA Controller. The device
supports two DMA channels; an 8-bit D channel and a 16-bit E channel.  The two
channels can be used for DMA or programmed IO and using the controller as an
SBus master or a slave. As an SBus master, the L64853 DMA controller, is
capable of operating as a DVMA master, generating virtual memory addresses on
the SBUS which are translated to physical addresses by the MMU on the SBus
controller.

Only D channel DMA operations is supported by this driver.  These DMA
operation are controlled through L64853 internal programmable registers, while
the E channel DMA operations are programmed through external registers located
on an E channel device.

The l64853 DMA controller uses the pack/unpack registers to buffer data.

USAGE

The routine l64853CtrlCreate() is called to create a DMA_CTRL structure which
is used to describe the L64853 controller. The routine l64853CtrlInit() is
used to initialize the chip before any operations are attempted on the L64853
controller by calling l64853HwInit().

The steps to setup a D channel DMA operation are,
.iP
Set the virtual memory address by calling l64853AdrCountSet().
.iP
Set the number of bytes to transfer by calling l64853ByteCountSet().
.iP
Set the direction transfer by calling l64853Read(), to perform DMA from
memory, or l64853Write() to perform DMA into memory.
.iP
Program the D channel device to request a DMA transfer causing an assertion
of the D_REQ line.
.iP
Enable the DMA by calling l64853DmaEnable(). DMA will now begin, and continue
until the byte count expires or if there is an error. If interrupts were
enabled by calling l64853IntEnable(), an interrupt is generated.
.iP
Service the interrupt, and if the completed DMA operation was a write to
memory, bytes pending in the pack register should be drained into memory by
calling l64853Drain(), otherwise the bytes should be flushed by calling
l64853Flush().
.LP

INCLUDE FILES
l64853.h

SEE ALSO:
.pG "I/O System"
*/

#include "vxWorks.h"
#include "drv/dma/l64853.h"
#include "memLib.h"
#include "stdlib.h"
#include "stdio.h"

/* defines */

#define WD_33C93_MAX_BYTES_PER_XFER  ((UINT) 0xffffff)

/* macros */

/*******************************************************************************
*
* L64853_CSR_BIT_SET - set the specified bit(s) in the L64853 CSR register
*
* NOMANUAL
*/

#define L64853_CSR_BIT_SET(pL64853, bit)		\
    do							\
	{						\
    	*pL64853->pCsrReg |= bit;			\
	} while (FALSE)

/*******************************************************************************
*
* L64853_CSR_BIT_RESET - reset the specified bit(s) in the L64853 CSR register
*
* NOMANUAL
*/

#define L64853_CSR_BIT_RESET(pL64853, bit)		\
    do							\
	{						\
        *pL64853->pCsrReg &= ~bit;			\
	} while (FALSE)

/* externals */

IMPORT DMA_CTRL *pSysDmaCtrl;


/* forward static functions */

static void l64853HwInit (L64853 *pL64853);


/*******************************************************************************
*
* l64853CtrlCreate - create an L64853 structure and initialize parts
*
* This routine creates an L64853 data structure.  It must be called before
* using an L64853 chip and should be called only once for a specified
* structure.  Since this routine allocates memory for a structure used by all
* routines in l64853Lib, it must be called before any other routines in the
* library.
*
* After calling this routine, at least one call to l64853CtrlInit() should
* be made before initiating any DMA transaction using the L64853.
*
* The input parameters are as follows:
* .iP <baseAdrs> 10
* the address where the CPU accesses the lowest (CSR) register of
* the L64853.
* .iP <regOffset> 10
* the address offset (in bytes) to access consecutive registers.
* .iP <pIdReg> 10
* the address where the CPU accesses the internal ID register.
* .LP
*
* RETURNS: A pointer to the L64853 control structure,
* or NULL if memory is unavailable or the parameters are invalid.
*
* SEE ALSO: l64853CtrlInit()
*/

DMA_CTRL *l64853CtrlCreate
    (
    UINT32 *    baseAdrs,       /* base address of the L64853 */
    int         regOffset,      /* address offset between consecutive regs. */
    UINT32 *    pIdReg          /* address of internal ID register */
    )
    {
    FAST L64853 *pL64853;		/* ptr to L64853 info */

    /* verify input parameters */

    if (regOffset == 0)
        return ((DMA_CTRL *) NULL);

    /* calloc the controller info structure; return NULL if unable */

    if ((pL64853 = (L64853 *) calloc (1, sizeof (L64853))) == NULL)
        return ((DMA_CTRL *) NULL);

    /* fill in L64853 specific data for this controller */

    pL64853->pCsrReg	= baseAdrs;
    pL64853->pAdrsReg	= (UINT32*) ((int) baseAdrs + (int) regOffset);
    pL64853->pBcntReg	= (UINT32*) ((int) baseAdrs + (0x2 * (int) regOffset));

    return ((DMA_CTRL *) pL64853);
    }

/*******************************************************************************
*
* l64853CtrlInit - initialize the L64853 hardware
*
* This routine initializes the L64853 hardware.  It should be called after
* l64853CtrlCreate(), but before the chip is actually used.
*
* The <pL64853> parameter is a pointer to the L64853_DMA_CTRL structure 
* created with l64853CtrlCreate().
*
* RETURNS: OK, always.
* 
* SEE ALSO: l64853CtrlCreate(), l64853HwInit()
*/

STATUS l64853CtrlInit
    (
    FAST L64853 *pL64853                /* ptr to L64853 info */
    )
    {
    l64853HwInit (pL64853); 	/* initialize the L64853 hardware */

    return (OK);
    }

/*******************************************************************************
*
* l64853IntEnable - enable L64853 DMA controller interrupts
*
* This routine allows the L64853 DMA controller to generate interrupts
* by setting the interrupt enable bit in the L64853 CSR register.
* The chip generates interrupts when an error is pending (CSR
* register bit L64853_CSR_ERR_PEND) or when the byte counter has expired (CSR
* register bit L64853_CSR_TC).  It also passes through interrupts from the D
* channel device.
*
* RETURNS: N/A
*
* SEE ALSO: l64853IntDisable()
*/

void l64853IntEnable
    (
    L64853 *pL64853          /* ptr to an L64853 structure */
    )
    {
    L64853_CSR_BIT_SET (pL64853, L64853_CSR_INT_EN);
    }

/*******************************************************************************
*
* l64853IntDisable - disable L64853 DMA controller interrupts
*
* This routine prevents the L64853 DMA controller from generating or 
* passing through interrupts by resetting the interrupt enable bit in 
* the L64853 CSR register.
*
* RETURNS: N/A
*
* SEE ALSO: l64853IntEnable()
*/

void l64853IntDisable
    (
    L64853 *pL64853          /* ptr to an L64853 structure */
    )
    {
    L64853_CSR_BIT_RESET (pL64853, L64853_CSR_INT_EN);
    }

/*******************************************************************************
*
* l64853Flush - flush the L64853 D channel pack register
*
* This routine sets the flush buffer bit in the CSR register of the L64853 DMA
* controller, clearing the pack count (CSR register bits L64853_CSR_PACK_CNT), 
* any pending errors (CSR register bit L64853_CSR_ERR_PEND), and the terminal
* count bit (CSR register bit L64853_CSR_TC).
*
* RETURNS: N/A
*/

void l64853Flush
    (
    L64853 *pL64853          /* ptr to an L64853 structure */
    )
    {
    L64853_CSR_BIT_SET (pL64853, L64853_CSR_FLUSH);
    }

/*******************************************************************************
*
* l64853Drain - drain the L64853 D channel pack register
*
* This routine sets the drain buffer bit in the CSR register of the L64853 DMA
* controller, causing any bytes in the D channel pack register to be written
* out to SBus memory; then it clears the pack count (CSR register bits
* L64853_CSR_PACK_CNT).
*
* RETURNS: N/A
*/

void l64853Drain
    (
    L64853 *pL64853          /* ptr to an L64853 structure */
    )
    {
    L64853_CSR_BIT_SET (pL64853, L64853_CSR_DRAIN);
    }

/*******************************************************************************
*
* l64853Reset - reset the L64853 DMA controller
*
* This routine toggles the reset bit in the CSR register of the L64853 DMA 
* controller, putting the DMA controller into the default initial state.
*
* RETURNS: N/A
*/

void l64853Reset
    (
    L64853 *pL64853          /* ptr to an L64853 structure */
    )
    {
    /* toggle reset bit to hardware reset the L64853 */
    
    L64853_CSR_BIT_SET   (pL64853, L64853_CSR_RESET);
    L64853_CSR_BIT_RESET (pL64853, L64853_CSR_RESET);
    }

/*******************************************************************************
*
* l64853Write - enable DMA transfers from a D channel device to SBus memory
*
* This routine permits the L64853 DMA controller to write data from a D
* channel device to SBus memory, by setting the write bit (L64853_CSR_WRITE)
* of the CSR register.
*
* RETURNS: N/A
* 
* SEE ALSO: l64853Read()
*/

void l64853Write
    (
    L64853 *pL64853          /* ptr to an L64853 structure */
    )
    {
    L64853_CSR_BIT_SET (pL64853, L64853_CSR_WRITE);
    }

/*******************************************************************************
*
* l64853Read - enable DMA transfers to a D channel device from SBus memory
* 
* This routine permits the L64853 DMA controller to read data from SBus memory
* into a D channel device, by clearing the write bit (L64853_CSR_WRITE) of the
* CSR register.
*
* RETURNS: N/A
*
* SEE ALSO: l64853Write()
*/

void l64853Read
    (
    L64853 *pL64853          /* ptr to an L64853 structure */
    )
    {
    L64853_CSR_BIT_RESET (pL64853, L64853_CSR_WRITE);
    }

/*******************************************************************************
*
* l64853DmaEnable - enable the L64853 DMA controller to accept DMA requests
* 
* This routine enables the L64853 DMA controller to accept DMA requests from a
* D channel device, by setting the enable DMA bit (L64853_CSR_EN_DMA) in the
* CSR register.
*
* RETURNS: N/A
*
* SEE ALSO: l64853DmaDisable()
*/

void l64853DmaEnable
    (
    L64853 *pL64853          /* ptr to an L64853 structure */
    )
    {
    L64853_CSR_BIT_SET (pL64853, L64853_CSR_EN_DMA);
    }

/*******************************************************************************
*
* l64853DmaDisable - prevent the L64853 DMA chip from accepting DMA requests
* 
* This routine disables acceptance of DMA requests from a D channel device by
* the L64853 DMA controller, by resetting the enable DMA bit
* (L64853_CSR_EN_DMA) in the CSR register.
*
* RETURNS: N/A
*
* SEE ALSO: l64853DmaEnable()
*/

void l64853DmaDisable
    (
    L64853 *pL64853          /* ptr to an L64853 structure */
    )
    {
    L64853_CSR_BIT_RESET (pL64853, L64853_CSR_EN_DMA);
    }

/*******************************************************************************
*
* l64853CountEnable - enable the byte counter of the L64853 DMA controller
* 
* This routine enables the byte counter of the L64853 DMA controller, by
* setting the enable counter bit (L64853_CSR_EN_CNT) in the CSR register.  The
* byte counter holds the number of bytes remaining in the current D channel
* DMA transfer.
*
* RETURNS: N/A
*
* SEE ALSO: l64853CountDisable(), l64853TermCountTest()
*/

void l64853CountEnable
    (
    L64853 *pL64853          /* ptr to an L64853 structure */
    )
    {
    L64853_CSR_BIT_SET (pL64853, L64853_CSR_EN_CNT);
    }

/*******************************************************************************
*
* l64853CountDisable - disable the byte counter of the L64853 DMA controller
* 
* This routine disables the byte counter of the L64853 DMA controller, by
* resetting the enable counter bit (L64853_CSR_EN_CNT) in the CSR register.
*
* RETURNS: N/A
*
* SEE ALSO: l64853CountEnable()
*/

void l64853CountDisable
    (
    L64853 *pL64853          /* ptr to an L64853 structure */
    )
    {
    L64853_CSR_BIT_RESET (pL64853, L64853_CSR_EN_CNT);
    }

/*******************************************************************************
*
* l64853ErrPendTest - test the L64853 DMA controller for an error
*
* This routine determines whether the error pending bit (L64853_CSR_ERR_PEND)
* of the L64853 CSR register is set.
*
* RETURNS : TRUE if there is a DMA error, otherwise FALSE.
*/

BOOL l64853ErrPendTest
    (
    L64853 *pL64853          /* ptr to an L64853 structure */
    )
    {
    return ((*pL64853->pCsrReg & L64853_CSR_ERR_PEND) ? TRUE : FALSE);
    }

/*******************************************************************************
*
* l64853PackCountGet - get the L64853 DMA controller pack count
*
* This routine returns the number of bytes in the D channel pack register of
* the L64853 DMA controller.
*
* RETURNS : An integer value of the pack count bits (L64853_CSR_PACK_CNT) of
* the CSR register.
*/

int l64853PackCountGet
    (
    L64853 *pL64853          /* ptr to an L64853 structure */
    )
    {
    return ((*pL64853->pCsrReg & L64853_CSR_PACK_CNT) >> 2);
    }

/*******************************************************************************
*
* l64853ReqPendTest - test the L64853 DMA controller for a DMA transfer
*
* This routine determines whether the request pending bit
* (L64853_CSR_REQ_PEND) of the L64853 CSR register is set.  This bit is set
* when a D channel DMA transfer is in progress.
*
* RETURNS : TRUE if there is an active DMA transfer, otherwise FALSE.
*/

BOOL l64853ReqPendTest
    (
    L64853 *pL64853          /* ptr to an L64853 structure */
    )
    {
    return ((*pL64853->pCsrReg & L64853_CSR_REQ_PEND) ? TRUE : FALSE);
    }

/*******************************************************************************
*
* l64853ByteAdrsGet - get the byte address bits of the L64853 CSR register
*
* This routine returns the byte address bits (L64853_CSR_BYTE_ADDR) of the
* L64853 CSR register.  These are the two least significant bits of the
* address of the next byte to be accessed by the D channel controller.
*
* RETURNS : The integer value of the byte address bits.
*/

int l64853ByteAdrsGet
    (
    L64853 *pL64853          /* ptr to an L64853 structure */
    )
    {
    return ((*pL64853->pCsrReg & L64853_CSR_BYTE_ADDR) >> 11);
    }

/*******************************************************************************
*
* l64853TermCountTest - test the L64853 DMA chip for an expired byte counter
*
* This routine determines whether the byte counter has expired by testing the
* terminal count bit (L64853_CSR_TC) of the L64853 CSR register.
*
* RETURNS : TRUE if the terminal count bit is set, otherwise FALSE.
*/

BOOL l64853TermCountTest
    (
    L64853 *pL64853          /* ptr to an L64853 structure */
    )
    {
    return ((*pL64853->pCsrReg & L64853_CSR_TC) ? TRUE : FALSE);
    }

/*******************************************************************************
*
* l64853DevIdGet - get the device ID bits of the L64853 CSR register
*
* This routine returns the device ID bits (L64853_CSR_DEV_ID) of the L64853
* CSR register.
*
* RETURNS : The integer value of the device ID bits.
*/


int l64853DevIdGet
    (
    L64853 *pL64853          /* ptr to an L64853 structure */
    )
    {
    return ((*pL64853->pCsrReg & L64853_CSR_DEV_ID) >> 28);
    }

/*******************************************************************************
*
* l64853AdrsCountGet - get the SBus address in the L64853 DMA address counter
*
* This routine returns the contents of the L64853 DMA address counter. This
* register contains the virtual address of the SBus memory for a D channel DMA
* transfer (read or write).
*
* RETURNS : The current virtual address in the address counter.
*
* SEE ALSO: l64853AdrsCountSet()
*/

char * l64853AdrsCountGet
    (
    L64853 *pL64853          /* ptr to an L64853 structure */
    )
    {
    return ((char *) *pL64853->pAdrsReg);
    }

/*******************************************************************************
*
* l64853AdrsCountSet - set the starting address for a D channel DMA transfer
*
* This routine writes a specified SBus address to the L64853 DMA address
* counter.  This address is used as the starting virtual address for a D
* channel DMA transfer (read or write).
*
* RETURNS: N/A
*
* SEE ALSO: l64853AdrsCountGet()
*/

void l64853AdrsCountSet
    (
    L64853 *pL64853,         /* ptr to an L64853 structure */
    char *pAdrsCount            /* ptr */
    )
    {
    *pL64853->pAdrsReg = (UINT32) pAdrsCount;
    }

/*******************************************************************************
*
* l64853ByteCountGet - get the contents of the L64853 DMA byte counter
*
* This routine returns the number of bytes remaining in the current D channel
* DMA transfer.
*
* RETURNS : The contents of the DMA byte counter.
*
* SEE ALSO: l64853ByteCountSet()
*/

int l64853ByteCountGet
    (
    L64853 *pL64853          /* ptr to an L64853 structure */
    )
    {
    return ((int) *pL64853->pBcntReg);
    }

/*******************************************************************************
*
* l64853ByteCountSet - set the size of a D channel DMA transfer
*
* This routine sets the size the D channel DMA transfer by loading the value,
* <byteCount>, into the L64853 DMA byte counter.
*
* RETURNS: N/A
* 
* SEE ALSO: l64853ByteCountGet()
*/

void l64853ByteCountSet
    (
    L64853 *pL64853,         /* ptr to an L64853 structure */
    int byteCount               /* ptr */
    )
    {
    *pL64853->pBcntReg = (UINT32) byteCount;
    }

/*******************************************************************************
*
* l64853HwInit - initialize the L64853 DMA controller to a quiescent state
*
* This routine resets the L64853 DMA chip, and clears the interrupt enable,
* DMA enable, and counter enable bits in the CSR register, putting the DMA
* controller in a quiescent state.
*
* RETURNS: N/A
*
* SEE ALSO: l64853Reset()
*/

LOCAL void l64853HwInit
    (
    L64853 *pL64853          /* ptr to an L64853 structure */
    )
    {
    l64853Reset (pL64853);

    /* clear various enable bits */

    L64853_CSR_BIT_RESET (pL64853, L64853_CSR_INT_EN);
    L64853_CSR_BIT_RESET (pL64853, L64853_CSR_EN_DMA);
    L64853_CSR_BIT_RESET (pL64853, L64853_CSR_EN_CNT);
    }

/*******************************************************************************
*
* l64853Show - display the contents of all readable L64853 DMA chip registers
*
* This routine displays the state of the L64853 DMA controller, by printing
* the contents of all readable registers.  It is used only during debugging.
*
* RETURNS: OK, or ERROR if both the parameter <pDmaCtrl> and the global
* variable <pSysDmaCtrl> are NULL.
*
* SEE ALSO: sysDmaShow()
*/

STATUS l64853Show
    (
    FAST DMA_CTRL *pDmaCtrl     /* ptr to DMA controller info */
    )
    {
    FAST UINT32 csrRegCopy;
    FAST L64853_DMA_CTRL *pl64853DmaCtrl;	/* ptr to L64853 info */

    if (pDmaCtrl == NULL)
        {
        if (pSysDmaCtrl != NULL)
            pDmaCtrl = pSysDmaCtrl;
        else
            {
            printErr ("No DMA controller specified.\n");
            return (ERROR);
            }
        }

    pl64853DmaCtrl = (L64853_DMA_CTRL *) pDmaCtrl;

    csrRegCopy = *pl64853DmaCtrl->pCsrReg;

    printf ("Interrupt Pending:   %s\n",
	    (csrRegCopy & L64853_CSR_INT_PEND) 	? " SET " : "reset");
    printf ("Error Pending:       %s\n",
	    (csrRegCopy & L64853_CSR_ERR_PEND) 	? " SET " : "reset");
    printf ("Interrupt Enable:    %s\n",
	    (csrRegCopy & L64853_CSR_INT_EN) 	? " SET " : "reset");
    printf ("Drain Buffer:        %s\n",
	    (csrRegCopy & L64853_CSR_DRAIN) 	? " SET " : "reset");
    printf ("Reset DMA:           %s\n",
	    (csrRegCopy & L64853_CSR_RESET) 	? " SET " : "reset");
    printf ("Write:               %s\n",
	    (csrRegCopy & L64853_CSR_WRITE) 	? " SET " : "reset");
    printf ("Enable DMA:          %s\n",
	    (csrRegCopy & L64853_CSR_EN_DMA) 	? " SET " : "reset");
    printf ("Request Pending:     %s\n",
	    (csrRegCopy & L64853_CSR_REQ_PEND) 	? " SET " : "reset");
    printf ("Enable Counter:      %s\n",
	    (csrRegCopy & L64853_CSR_EN_CNT) 	? " SET " : "reset");
    printf ("Terminal Count:      %s\n",
	    (csrRegCopy & L64853_CSR_TC) 	? " SET " : "reset");
    printf ("ILACC 79C900:        %s\n",
	    (csrRegCopy & L64853_CSR_ILACC) 	? " SET " : "reset");

    printf ("\n");

    printf ("Pack Count:        %d\n",
	    (csrRegCopy & L64853_CSR_PACK_CNT) >> 2);
    printf ("Byte Address:      %d\n",
	    (csrRegCopy & L64853_CSR_BYTE_ADDR) >> 11);
    printf ("Device ID:         %d\n",
	    (csrRegCopy & L64853_CSR_DEV_ID) >> 28);

    printf ("\n");

    printf ("DMA Address    Reg = 0x%08x\n", *pl64853DmaCtrl->pAdrsReg);
    printf ("DMA Byte Count Reg = 0x%08x\n", *pl64853DmaCtrl->pBcntReg);

    return (OK);
    }

