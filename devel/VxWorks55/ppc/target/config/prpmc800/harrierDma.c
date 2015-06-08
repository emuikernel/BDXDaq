/* harrierDma.c - Support for the Harrier's DMA Controller */

/* Copyright 2001 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01a,14jun01,srr  Initial Creation.
*/


/*
DESCRIPTION
The Harrier DMA driver will support multiple Harriers and consist of the
following seven routines:
	
harrierDmaInit   - Initialize DMA and attach DMA Interrupt Handler.
harrierDmaIntr   - Clear the interrupt and capture completion status.
harrierDmaStart  - Configure and start the DMA controller.
harrierDmaStatus - Read and return the DMA status register.
harrierDmaAbort  - Initiate an abort of the current DMA operation.
harrierDmaPause  - Initiate a pause of the current DMA operation.
harrierDmaResume - Resume a previously paused DMA operation.
	
Harrier has a single channel DMA Controller that facilitates the transfer of
large blocks of data without processor intervention.  This driver supports
both the Direct and Linked-List Modes of operation.  It also covers the six
directions for data movement in a DMA transfer:
   PowerPC to PCI, PCI to PowerPC, PowerPC to PowerPC, PCI to PCI,
   Data Pattern to PowerPC, Data Pattern to PCI.

Harrier does not snoop the PowerPC bus for data held within the DMA FIFO,
therefore caution should be exercised when using the DMA controller to move
data within coherent memory space.

The Harrier DMA driver must be initialized for each Harrier it will be executed
on.  This will usually be configured in sysHwInit2 with separate commands to
harrierDmaInit for each Harrier instance.  The customer will configure DMA
Descriptor Lists according to the description below and pass the address of
the first as a parameter to the harrierDmaStart routine.

The user has three options for being notified of the completion of a DMA
transaction depending on the contents of the userHandler parameter of
harrierDmaStart.  The first is to have the DMA start routine wait for the
done interrupt which will return the contents of the DMA status register
(userHandler == WAIT_FOREVER).  The second is to provide a routine for the
DMA interrupt handler to call upon completion which must follow the rules
for executing inside an ISR by not calling waits, prints, etc
(userHander == user routine).  The third is to have DMA start return
immediately so the user task can proceed with its execution and then call
harrierDmaStatus later to get the results (userHandler == 0).

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


/* includes */

#include "vxWorks.h"
#include "config.h"
#include "harrierDma.h"

/* defines */

/* typedefs */

typedef struct harrierDmaDrvCtrl
    {
    UINT32 * pHarrierBaseAdrs;		/* Base address of Harrier */
    UINT32   (*userHandler)(int);	/* Address of user Interrupt Handler */
    SEM_ID   semDrvId;			/* Driver access semaphore */
    SEM_ID   semIntId;			/* Interrupt interface semaphore */
    } HARRIER_DMA_DRV_CTRL;

/* globals */

/* locals */

/* THIS ARRAY INITIALIZATION MAY NEED ADJUSTMENT FOR MULTIPLE HARRIERS???? */

LOCAL HARRIER_DMA_DRV_CTRL harrierDmaCtrl [HARRIER_MAX_DEV] = {{0, 0, 0, 0}};

/* forward declarations */

LOCAL void harrierDmaIntr (UINT32);
int harrierDmaStart (UINT32, UINT32 (*)(), HARRIER_DMA_DESCRIPTOR *);
STATUS harrierDmaStatus (UINT32, UINT32 *);
STATUS harrierDmaAbort (UINT32);
STATUS harrierDmaPause (UINT32);
STATUS harrierDmaResume (UINT32);

/* external references */

/******************************************************************************
*
* harrierDmaInit - Initialize DMA and attach DMA Interrupt Handler.
*
* This routine configures the DMA driver for the specific Harrier instance.
* This routine must be called once for the Harrier DMA capability to be
* properly initialized before it can be used. 
*
* RETURNS: OK if initialization succeeded or ERROR if an error occured.
*/

STATUS harrierDmaInit
    (
    UINT32   devInstance,		/* Instance of Harrier */
    UINT32 * pHarrierBaseAdrs		/* Base address of Harrier */
    )
    {
    HARRIER_DMA_DRV_CTRL * pHarrierDmaCtrl;


    /* Validate Device index */

    if (devInstance >= HARRIER_MAX_DEV)
	return(ERROR);

    /* Setup pointer to DMA Driver control structure */

    pHarrierDmaCtrl = &harrierDmaCtrl[devInstance];

    /* Verify the driver has not been previously initialized. */

    if (pHarrierDmaCtrl->pHarrierBaseAdrs != 0)
	return(ERROR);

    /* Store Harrier Base Adddress for instance of driver */

    pHarrierDmaCtrl->pHarrierBaseAdrs = pHarrierBaseAdrs;

    /* Disable DMA interrupt. */

/*
 * THE HARRIER MACROS ARE ONLY SET FOR A ONE HARRIER SYSTEM.
 * THIS MUST BE CHANGED TO HANDLE MULTIPLE HARRIERS!!!!
 */

    *(UINT16 *)HARRIER_EXCEPTION_ENABLE_REG &= ~HARRIER_FEEN_DMA;

    /* Clear any pending DMA interrupt by writing to FECL. */

    *(UINT16 *)HARRIER_EXCEPTION_CLEAR_REG = HARRIER_FECL_DMA;

    /* Setup DMA Control Register default values. */

    *(UINT32 *)HARRIER_DMA_CONTROL_REG = (HARRIER_DCTL_XTH_256BYTES |
                                          HARRIER_DCTL_PBT_0_CLKS |
                                          HARRIER_DCTL_CSE |
                                          HARRIER_DCTL_CRI |
                                          HARRIER_DCTL_GBL);

    /* Create a semaphore for access to the device instance. */

    pHarrierDmaCtrl->semDrvId = semBCreate(SEM_Q_PRIORITY, SEM_FULL);

    /* Create binary semaphore for synchronization with DMA start. */

    pHarrierDmaCtrl->semIntId = semBCreate(SEM_Q_FIFO, SEM_EMPTY);

    /* Connect the interrupt handler to HARR_FUNC_INT_VEC. */

    if (intConnect (INUM_TO_IVEC(HARR_FUNC_INT_VEC), harrierDmaIntr,
                    (int)devInstance) == ERROR)
	return (ERROR);

    /* Enable the interrupt to the processor in the MPIC. */

    intEnable (HARR_FUNC_INT_LVL);

    /* Clear the DMA interrupt mask by writing to FEMA. */

    *(UINT16 *)HARRIER_EXCEPTION_MASK_REG = HARRIER_FEST_DMA;

    /* Enable the DMA interrupt by writing to FEEN. */

    *(UINT16 *)HARRIER_EXCEPTION_ENABLE_REG |= HARRIER_FEEN_DMA;

    return (OK);
    }


/******************************************************************************
*
* harrierDmaIntr - Clear the interrupt and capture completion status.
*
* This routine cleans up after the DMA controller interrupt.
* It gives a semaphore to the DMA start routine if wait is set to -1.
* It executes a user routine if wait is set to something other than 0 or -1.
*
* RETURNS: N/A
*/

LOCAL void harrierDmaIntr 
    (
    UINT32   devInstance		/* Instance of Harrier */
    )
    {
    HARRIER_DMA_DRV_CTRL * pHarrierDmaCtrl;
             
    /* Setup pointer to DMA Driver control structure */

    pHarrierDmaCtrl = &harrierDmaCtrl[devInstance];

    /* Verify that the interrupt has occurred by reading FEST. */

    if (!(*(UINT16 *)HARRIER_EXCEPTION_STATUS_REG & HARRIER_FEST_DMA))
	return;

    /*
     * If the device instance wait is set to WAIT_FOREVER,
     *   then give semaphore to waiting DMA start.
     * Else if the device instance wait is non-0,
     *   then it contains a user interrupt routine,
     *   so call it and pass the device instance.
     * Else the handler is done.
     */

    if ((int)(pHarrierDmaCtrl->userHandler) == WAIT_FOREVER)
	semGive (pHarrierDmaCtrl->semIntId);

    else if ((int)(pHarrierDmaCtrl->userHandler) != 0)
	(pHarrierDmaCtrl->userHandler) (devInstance);

    /* Clear the interrupt by writing to FECL. */

    *(UINT16 *)HARRIER_EXCEPTION_CLEAR_REG = HARRIER_FECL_DMA;

    return;
    }


/******************************************************************************
*
* harrierDmaStart - Configure and start the DMA controller.
*
* This routine sets up the DMA controller for a specific DMA activity.
* It expects the user to configure the DMA Descriptor List according to the
* specifications and does not do any validation of the list.  It uses the
* link field of the list to identify if the transaction will be done in
* Direct or Linked-List mode.
*
* The user has three options for being notified of the completion of a DMA
* transaction depending on the contents of the userHandler parameter.
* The first is to have the DMA start routine wait for the done interrupt
* which will return the contents of the DMA status register
* (userHandler == WAIT_FOREVER).  The second is to provide a routine for the
* DMA interrupt handler to call upon completion which must follow the rules
* for executing inside an ISR by not calling waits, prints, etc
* (userHander == user routine).  The third is to have DMA start return
* immediately so the user task can proceed with its execution and then call
* harrierDmaStatus later to get the results (userHandler == 0).
*
* RETURNS:          0: if succeeded without wait;
*          DMA Status: if succeeded with wait;
*                  -1: if error.
*/

int harrierDmaStart
    (
    UINT32   devInstance,		/* Instance of Harrier */
    UINT32   (*userHandler)(),		/* Address of user Interrupt Handler */
    HARRIER_DMA_DESCRIPTOR * pDescList	/* Address of DMA Descriptor List */
    )
    {
    HARRIER_DMA_DRV_CTRL * pHarrierDmaCtrl;
    int status = 0;


    /* Validate Device index */

    if (devInstance >= HARRIER_MAX_DEV)
	return(-1);

    /* Setup pointer to DMA Driver control structure */

    pHarrierDmaCtrl = &harrierDmaCtrl[devInstance];

    /* Verify the driver has been initialized. */

    if (pHarrierDmaCtrl->pHarrierBaseAdrs == 0)
	return(-1);

    /* Obtain semaphore for access to the device instance. */

    semTake (pHarrierDmaCtrl->semDrvId, WAIT_FOREVER);

    /* Verify the DMA controller is not busy or paused. */

    if (*(UINT16 *)HARRIER_DMA_STATUS_REG &
        (HARRIER_DSTA_BSY | HARRIER_DSTA_PAU))
	{
	semGive (pHarrierDmaCtrl->semDrvId);
	return(-1);
	}

    /* Store wait information in control structure for interrupt handler. */

    pHarrierDmaCtrl->userHandler = userHandler;

    /*
     * Initialize the DMA controller with the input descriptor.
     * If the Descriptor List Next Link Address (pDnla) entry is 0,
     *   then the transfer will be in Direct Mode and the
     *   descriptor list is copied into the controller registers.
     * Else the transfer will be in Linked-List Mode and only the
     *   DMA Control and Link Address Registers will be loaded.
     */

    if (pDescList->pDnla == 0)
	{
	*(UINT32 *)HARRIER_DMA_CONTROL_REG        |= HARRIER_DCTL_MOD;
	*(UINT32 *)HARRIER_DMA_SOURCEADDR_REG      = pDescList->dsad;
	*(UINT32 *)HARRIER_DMA_SOURCEATTR_REG      = pDescList->dsat;
	*(UINT32 *)HARRIER_DMA_DESTINATIONADDR_REG = pDescList->ddad;
	*(UINT32 *)HARRIER_DMA_DESTINATIONATTR_REG = pDescList->ddat;
	*(UINT32 *)HARRIER_DMA_NEXTLINKADDR_REG    = (UINT32) pDescList->pDnla;
	*(UINT32 *)HARRIER_DMA_COUNT_REG           = pDescList->dcnt;
	}
    else
	{
	*(UINT32 *)HARRIER_DMA_CONTROL_REG        &= ~HARRIER_DCTL_MOD;
	*(HARRIER_DMA_DESCRIPTOR **)HARRIER_DMA_NEXTLINKADDR_REG = pDescList;
	}

    /* Start the DMA operation by setting DMA Go. */

    *(UINT32 *)HARRIER_DMA_CONTROL_REG |= HARRIER_DCTL_DGO;

    /*
     * If the device instance wait is set to -1,
     *   then wait on semaphore from harrierDMAIntr
     *   and return results from the DMA Status Register.
     */

    if ((int)userHandler == WAIT_FOREVER)
	{
	semTake (pHarrierDmaCtrl->semIntId, WAIT_FOREVER);
	status = *(UINT32 *)HARRIER_DMA_STATUS_REG;
	}

    /* Release the device instance semaphore. */

    semGive (pHarrierDmaCtrl->semDrvId);

    return (status);
    }
            

/******************************************************************************
*
* harrierDmaStatus - Read and return the DMA status registers.
*
* Read the contents of the:
*               DMA Status Register;
*               DMA Current Source Address Register;
*               DMA Current Destination Address Register;
*               DMA Current Link Address Register.
*
* The contents of these registers will be placed, in order, at where the
* status parameter points to.
*
* RETURNS: OK or ERROR if the device instance is invalid.
*/

STATUS harrierDmaStatus
    (
    UINT32   devInstance,		/* Instance of Harrier */
    UINT32 * pStatus			/* Address status block */
    )
    {
    /* Validate Device index */

    if (devInstance >= HARRIER_MAX_DEV)
	return(ERROR);

    /* Read DMA Status Register and store. */

    *pStatus++ = *(UINT32 *)HARRIER_DMA_STATUS_REG;

    /* Read DMA Current Source Address Register and store. */

    *pStatus++ = *(UINT32 *)HARRIER_DMA_CURRENTSOURCEADDRREG;

    /* Read DMA Current Destination Address Register and store. */

    *pStatus++ = *(UINT32 *)HARRIER_DMA_CURRENTDESTINATIONADDR_REG;

    /* Read DMA Current Link Address Register and store. */

    *pStatus   = *(UINT32 *)HARRIER_DMA_CURRENTLINKADDR_REG;

    return (OK);
    }


/******************************************************************************
*
* harrierDmaAbort - Initiate an abort of the current DMA operation.
*
* If the DMA Controller is not busy, then ERROR is returned.
* Otherwise, an abort command is issued and OK is returned.
*
* The user will need to check the DMA Status Register to verify
* the DMA Controller is aborted.
*
* RETURNS: OK if abort succeeded or ERROR if an error occured.
*/

STATUS harrierDmaAbort
    (
    UINT32   devInstance		/* Instance of Harrier */
    )
    {
    HARRIER_DMA_DRV_CTRL * pHarrierDmaCtrl;

    /* Validate Device index */

    if (devInstance >= HARRIER_MAX_DEV)
	return(ERROR);

    /* Setup pointer to DMA Driver control structure */

    pHarrierDmaCtrl = &harrierDmaCtrl[devInstance];

    /* Obtain semaphore for access to the device instance. */

    semTake (pHarrierDmaCtrl->semDrvId, WAIT_FOREVER);

    /* Verify the DMA controller is busy, else return ERROR. */

    if (!(*(UINT16 *)HARRIER_DMA_STATUS_REG & HARRIER_DSTA_BSY))
	{
	return(ERROR);
	}

    /* Set the Abort bit in the DMA control register. */

    *(UINT32 *)HARRIER_DMA_CONTROL_REG |= HARRIER_DCTL_ABT;

    /* Release the device instance semaphore. */

    semGive (pHarrierDmaCtrl->semDrvId);

    return(OK);
    }


/******************************************************************************
*
* harrierDmaPause - Initiate a pause of the current DMA operation.
*
* If the DMA Controller is not busy, then ERROR is returned.
* Otherwise, a pause command is issued and OK is returned.
*
* The user will need to check the DMA Status Register to verify
* the DMA Controller is paused.
*
* RETURNS: OK if pause succeeded or ERROR if an error occured.
*/

STATUS harrierDmaPause
    (
    UINT32   devInstance		/* Instance of Harrier */
    )
    {
    HARRIER_DMA_DRV_CTRL * pHarrierDmaCtrl;

    /* Validate Device index */

    if (devInstance >= HARRIER_MAX_DEV)
	return(ERROR);

    /* Setup pointer to DMA Driver control structure */

    pHarrierDmaCtrl = &harrierDmaCtrl[devInstance];

    /* Obtain semaphore for access to the device instance. */

    semTake (pHarrierDmaCtrl->semDrvId, WAIT_FOREVER);

    /* Verify the DMA controller is busy, else return ERROR. */

    if (!(*(UINT16 *)HARRIER_DMA_STATUS_REG & HARRIER_DSTA_BSY))
	{
	return(ERROR);
	}

    /* Set the Pause bit in the DMA control register. */

    *(UINT32 *)HARRIER_DMA_CONTROL_REG |= HARRIER_DCTL_PAU;

    /* Release the device instance semaphore. */

    semGive (pHarrierDmaCtrl->semDrvId);

    return(OK);
    }


/******************************************************************************
*
* harrierDmaResume - Resume a previously paused DMA operation.
*
* If the DMA Controller is not paused, then ERROR is returned.
* Otherwise, a Go command is issued and OK is returned.
*
* The user will need to check the DMA Status Register to verify
* the DMA Controller is no longer paused.
*
* RETURNS: OK if resume succeeded or ERROR if an error occured.
*/

STATUS harrierDmaResume
    (
    UINT32   devInstance		/* Instance of Harrier */
    )
    {
    /* Validate Device index */

    if (devInstance >= HARRIER_MAX_DEV)
	return(ERROR);

    /* Verify the DMA controller is paused, else return ERROR. */

    if (!(*(UINT16 *)HARRIER_DMA_STATUS_REG & HARRIER_DSTA_PAU))
	{
	return(ERROR);
	}

    /* Set the DMA Go bit in the DMA control register. */

    *(UINT32 *)HARRIER_DMA_CONTROL_REG |= HARRIER_DCTL_DGO;

    return(OK);
    }



/**************************/
/* Sergey: user functions */

/* sysPci2MemDmaCopy(0x10000000,0x02000000,20) */

int
usrPci2MemDmaStart(UINT32 chan, UINT32 *pciAddr, UINT32 *localAddr, UINT32 nbytes)
{
  HARRIER_DMA_DESCRIPTOR pDescList;
  int status;

  pDescList.dsad = pciAddr;    /* DMA Source Address Register */
  pDescList.dsat = 0x10000600; /* DMA Source Attribute Register (PCI mem) */
  pDescList.ddad = localAddr;  /* DMA Destination Address Register */
  pDescList.ddat = 0x00000701; /* DMA Destination Attribute Register */
  pDescList.pDnla = 0;         /* DMA Next Link Address Register */
  pDescList.dcnt = nbytes;     /* DMA Count Register */

#ifdef DEBUG
  printf("dma params: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
    pDescList.dsad,pDescList.dsat,pDescList.ddad,
    pDescList.ddat,pDescList.pDnla,pDescList.dcnt);
#endif
  /* second param: '-1' - wait, '0' - return immediately */
  status = harrierDmaStart(0, 0, &pDescList);
#ifdef DEBUG
  printf("status=%d\n",status);
#endif
  return(status);
}

int
usrMem2PciDmaStart(UINT32 chan, UINT32 *localAddr, UINT32 *pciAddr, UINT32 nbytes)
{
  HARRIER_DMA_DESCRIPTOR pDescList;
  int status;

  pDescList.dsad = localAddr;  /* DMA Source Address Register */
  pDescList.dsat = 0x00000601; /* DMA Source Attribute Register */
  pDescList.ddad = pciAddr;    /* DMA Destination Address Register */
  pDescList.ddat = 0x10000700; /* DMA Destination Attribute Register */
  pDescList.pDnla = 0;         /* DMA Next Link Address Register */
  pDescList.dcnt = nbytes;     /* DMA Count Register */

#ifdef DEBUG
  printf("dma params: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
    pDescList.dsad,pDescList.dsat,pDescList.ddad,
    pDescList.ddat,pDescList.pDnla,pDescList.dcnt);
#endif
  /* second param: '-1' - wait, '0' - return immediately */
  status = harrierDmaStart(0, 0, &pDescList);
#ifdef DEBUG
  printf("status=%d\n",status);
#endif
  return(status);
}


int
usrDmaDone(UINT32 chan)
{
  UINT32 status, pStatus[4];

  harrierDmaStatus(0, pStatus);
  if( pStatus[0]&0x02000000 ) status = 1;
  else                        status = 0;

  return(status);
}



/* temporary: simple syncronization mechanizm */

/* PMC bridge general purpose register access */
/* always swap !!! */

int
usrReadGPR()
{
  UINT32 data, offset, id;

  /*
  pciConfigInLong(0,0,0,0x0,&id);
  if(id == 0x646011ab)
  {
    offset = 0x24;
    pciConfigInLong(0,0,0,offset,&data);
  }
  else*/
  {
    offset = 0x84; /* prpmc800 */
    data = LONGSWAP(*((UINT32 *)0xfeff0384));
  }

  /*printf("usrReadGPR: data=0x%08x\n",data);*/

  return(data);
}

int
usrWriteGPR(UINT32 data)
{
  UINT32 offset, id;

  /*
  pciConfigInLong(0,0,0,0x0,&id);
  if(id == 0x646011ab)
  {
    offset = 0x24;
    pciConfigOutLong(0,0,0,offset,data);
  }
  else*/
  {
    offset = 0x84;
    *((UINT32 *)0xfeff0384) = LONGSWAP(data);
  }
  /*
  printf("usrWriteGPR: data=0x%08x\n",data);
  */
  return(0);
}




