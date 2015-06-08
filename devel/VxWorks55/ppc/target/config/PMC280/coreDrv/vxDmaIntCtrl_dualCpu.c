/******************************************************************************
*              (c), Copyright, FORCE COMPUTERS INDIA Limited                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF FORCE COMPUTERS, INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF FORCE OR ANY THIRD PARTY. FORCE RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO FORCE COMPUTERS.       *
* THIS CODE IS PROVIDED "AS IS". FORCE COMPUTERS MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

*******************************************************************************
* vxDmaIntCtrl.c - GT DMA interrurpt controller library
*
* DESCRIPTION:
*       This driver provides various interface routines to manipulate and
*       connect the hardware interrupts concerning the GT DMA facility.
*
*       Main features:
*        - The controller provides an easy way to hook a C Interrupt Service
*          Routine (ISR) to a specific interrupt caused by the GT DMA engines.
*        - The controller interrupt mechanism provides a way for the programmer
*          to set the priority of an interrupt.
*        - Full interrupt control over the GT DMA facility.
*
*       Overall Interrupt Architecture description:
*        The GT handles interrupts in two stages:
*        STAGE 1: Main Cause register that summarize the interrupts generated 
*                 by each Internal GT unit.
*        STAGE 2: Unit specified cause registers, that distinguish between each
*                 specific interrupt event.
*        This DMA Interrupt Controller Driver handles the various interrupts 
*        generated by the second stage.
*
*       All DMA various interrupt causes are numbered using the DMA_CAUSE
*       enumerator. Thus there are 64 interrupts causes (most of them are
*       Reserved) divided into two groups:
*       The DMA channels 0-3 interrupt causes (DMA_CAUSE 0 - 31)
*       and DMA channels 4-7 interrupt causes (DMA_CAUSE 32 - 63).
*
*       The driver's execution flow has three phases:
*        1) Driver initialization. This initiation includes hooking driver's 
*           ISR to the GT Interrupt controller. Composed of frcDmaIntCtrlInit() 
*           routine.
*        2) User ISR connecting. Here information about user ISR and interrupt 
*           priority is gathered. Composed of frcDmaIntConnect() routine.
*        3) Interrupt handler. Here an interrupt is being handle by the 
*           Interrupt Handlers (driver's ISR). Composed of vxDmaInt(), 
*
*       Full API:    
*         vxDmaInit()        - Initiate the DMA driver.
*         frcDmaIntConnect()  - Connect a user ISR to a DMA interrupt event.
*         frcDmaIntEnable()   - Enable a given DMA interrupt cause.
*         frcDmaIntDisable()  - Disable a given DMA interrupt cause.
*         frcDmaCompIntHandler()- Handle DMA 0-3 completion interrupts.
*         frcDmaErrIntHandler()- Handle DMA 0-3 Error interrupts.
*
*       The controller concept is very simple:
*        Each of the Interrupt handlers has a table which holds information on 
*        the connected ISR. For example, dma2_3CompInt() has dma2_3Array[] 
*        which holds the ISR concerning DMA engines 2 and 3.
*        An Interrupt generated by one of those DMA engines will result a 
*        search through this table in order to allocate the generating 
*        interrupt cause.
*        After the initiating interrupt cause is identify, the ISR reside in 
*        the same table entry is executed.
*       
*       The controller interface also includes interrupt control routines which
*       can enable/disable specific interrupts: frcDmaIntDisable() and
*       frcDmaIntEnable().
*   
* DEPENDENCIES:
*		GT Interrupt Control Driver.
*		VxWorks types and interrupt lib.
*
******************************************************************************/

/* includes */
#include "vxDmaIntCtrl.h" 

#ifdef PMC280_DUAL_CPU
#include "gtSmp.h"
#endif /* PMC280_DUAL_CPU */

/* defines  */
#define DMA_0_3_CAUSE_REG   REG_CONTENT(DMA_INTERRUPT_CAUSE_REG)
#define DMA_0_3_MASK_REG    REG_CONTENT(DMA_INTERRUPT_CAUSE_MASK)

#define INVALID_CAUSE(cause)    \
                ((cause) <= DMA_CAUSE_START || DMA_CAUSE_END <= (cause))

/* typedefs */


/* locals   */

/* GT_ISR_ENTRY array accumulates user ISR information for each dma group. */
LOCAL GT_ISR_ENTRY dma0_3Array[4];         /* DMA 0-3 completion interrupt */
LOCAL GT_ISR_ENTRY dmaError0_3Array[32];   /* DMA 0-3 Error condition  */

/* Each counter accumulate the number of connection done to a cause array.*/
/* For example dma0_3Count counts the number of connection done to        */
/* dma0_3Array.                                                           */
LOCAL int dma0_3Count = 0; 
LOCAL int dmaError0_3Count = 0; 

LOCAL bool vxDmaIntCtrlInitialized = false;

/* Interrupt Controller's ISR */
LOCAL void  frcDmaCompIntHandler(void);     /* DMA completion event handler */
LOCAL void  frcDmaErrIntHandler(void);    /* general DMA event handler      */

                                                        
/*******************************************************************************
* frcDmaIntCtrlInit - Initiating the DMA Interrupt Controller driver.
*
* DESCRIPTION:
*       This routines connects the drivers interrupt handlers, each to its 
*       corresponding bit in the GT main Interrupt Controller using the
*       gtIntConnect() routine.
*
* INPUT:
*       None.
*
* OUTPUT:
*       The DMA Engine casue & mask register are initiated (set to zero).
*       Driver's ISR are connected to the main cause register. 
*       interrupts are unmasked.
*
* RETURN:
*       None.
*
*******************************************************************************/
void frcDmaIntCtrlInit(void)
{
#ifdef PMC280_DUAL_CPU
    /*
     * IDMA Channels 0 and 1 are assigned to CPU0 while IDMA Channels 2 and 3
     * are assigned to CPU1.
     */
    if(frcWhoAmI())
    {
        if(vxDmaIntCtrlInitialized)
            return;

        gtIntConnect(IDMA2, frcDmaCompIntHandler, 0, 9);
        gtIntConnect(IDMA3, frcDmaCompIntHandler, 0, 9);
        gtIntConnect(DMA_ERR, frcDmaErrIntHandler, 0, 9);

        gtIntEnable(IDMA2);
        gtIntEnable(IDMA3);
        gtIntEnable(DMA_ERR);
    }
    else
    {
        if(vxDmaIntCtrlInitialized)
            return;

        DMA_0_3_CAUSE_REG = 0;  /* Clean Cause regiser (case of warm reboot) */
        DMA_0_3_MASK_REG  = 0;  /* Clean Mask regiser (case of warm reboot) */

        /* Connect the user ISR's to the DMA bits in the main cause register */
        gtIntConnect(IDMA0, frcDmaCompIntHandler, 0, 9);
        gtIntConnect(IDMA1, frcDmaCompIntHandler, 0, 9);
        gtIntConnect(DMA_ERR, frcDmaErrIntHandler, 0, 9);

        /* Unmask those bits */
        gtIntEnable(IDMA0);
        gtIntEnable(IDMA1);
        gtIntEnable(DMA_ERR);
    }
#else
    if(vxDmaIntCtrlInitialized)
        return;

    DMA_0_3_CAUSE_REG = 0;  /* Clean Cause regiser (case of warm reboot) */
    DMA_0_3_MASK_REG  = 0;  /* Clean Mask regiser (case of warm reboot)	*/

    /* Connect the user ISR's to the DMA bits in the main cause register */
    gtIntConnect(IDMA0, frcDmaCompIntHandler, 0, 9);
    gtIntConnect(IDMA1, frcDmaCompIntHandler, 0, 9);
    gtIntConnect(IDMA2, frcDmaCompIntHandler, 0, 9);
    gtIntConnect(IDMA3, frcDmaCompIntHandler, 0, 9);
    gtIntConnect(DMA_ERR,frcDmaErrIntHandler, 0, 9);

    /* Unmask those bits */
    gtIntEnable(IDMA0);
    gtIntEnable(IDMA1);
    gtIntEnable(IDMA2);
    gtIntEnable(IDMA3);
    gtIntEnable(DMA_ERR);
#endif /* PMC280_DUAL_CPU */

    vxDmaIntCtrlInitialized = true;
}


/*******************************************************************************
* frcDmaIntConnect - connect a C routine to a specific DMA interrupt.
*
* DESCRIPTION:
*       This routine connects a specified user ISR to a specified GT DMA 
*       interrupt cause (0x8c0 or 0x9c0).
*       Each ISR handler has its own user ISR array. 
*       The connection is done by setting the desired routine and parameter in 
*       the corresponding cause array (i.e. dma0_1Array[]):
*         1) Locating the correct Array to make the connction.
*            This is made according to the cause. 
*            DMA channel completion events has unique ISRs.
*         2) Check for existing connection for the cause bit in the table.
*         3) Connecting the user ISR by inserting the given parameters into 
*           an entry according to the user ISR given priority.
*
* INPUT:
*       cause     - DMA interrupt cause. See DMA_INT_CAUSE. 
*       routine   - user ISR.
*       parameter - user ISR parameter.
*       prio      - Interrupt handling priority where 0 is highest.
*
* OUTPUT:
*       An appropriate table entry is filled.
*
* RETURN:
*       OK    - if the table entry of the cause bit, was filled. 
*       ERROR - if cause argument is invalid or connected cause is already 
*                found in table.
*
*******************************************************************************/
STATUS frcDmaIntConnect(DMA_CAUSE cause, VOIDFUNCPTR routine, int parameter,
                       int prio)
{
    GT_ISR_ENTRY *intCauseArray; /* Pointer to the selected cause table    */
    int *intConnectCount;   	/* Pointer to the table connection counter*/
    int i, sysIntOldConfig;

    /* Make sure that this is an atomic operation */
    sysIntOldConfig = intLock();

    /* Check the validity of the parameters */
    if(routine == NULL)
    {
        logMsg("\nfrcDmaIntConnect: NULL pointer routine\n",0,0,0,0,0,0);
        intUnlock(sysIntOldConfig);
        return ERROR;
    }
    else if(prio < 0)
    {
        logMsg("\nfrcDmaIntConnect: Invalid interrupt priority\n",0,0,0,0,0,0);
        intUnlock(sysIntOldConfig);
        return ERROR;
    }
    else if(cause == DMA_CHAN0_COMP || cause == DMA_CHAN1_COMP ||
            cause == DMA_CHAN2_COMP || cause == DMA_CHAN3_COMP)
    {
        intCauseArray   = dma0_3Array;
        intConnectCount = &dma0_3Count;
    }
    else if(!INVALID_CAUSE(cause))/* cause bit is from DMA 0-3 cause register */
    {
        intCauseArray   = dmaError0_3Array;
        intConnectCount = &dmaError0_3Count;
    }
    else
    {
        logMsg("\nfrcDmaIntConnect: Invalid cause %d\n",cause,0,0,0,0,0);
        intUnlock(sysIntOldConfig);
        return ERROR;
    }

    /* Scan the table to check for double connection to the same cause bit */
    for(i = *intConnectCount; i >= 0; i--)
        if(intCauseArray[i].causeBit == SWAP_CAUSE_BIT(cause))
        {
            logMsg("\nfrcDmaIntConnect(cause %d): ERROR\n",cause,0,0,0,0,0);
            logMsg("   An attempt to reconnect ISR to an existing cause bit.\n",
                   0,0,0,0,0,0); 
            intUnlock(sysIntOldConfig);
            return ERROR;
        }

    /* Connection phase */
    /* The search starts from the bottom of the table, because:     */
    /*  1) if the priority is the highest, we can put it straight   */
    /*     in the end of the table                                  */
    /*  2) we can push the highests priority down the table,right   */
    /*     away, with out the need to copy the struct.              */

    for(i = *intConnectCount; i >= 0; i--)
    {
        if(i == 0 || intCauseArray[i-1].prio < prio) /* Make connection */
        {
            intCauseArray[i].causeBit = SWAP_CAUSE_BIT(cause);
            intCauseArray[i].userISR  = routine;
            intCauseArray[i].arg      = parameter;
            intCauseArray[i].prio     = prio;
            (*intConnectCount)++;
            break;
        }
        else
        {
            /* Push the low priority connection down the table */
            intCauseArray[i].causeBit = intCauseArray[i-1].causeBit;
            intCauseArray[i].userISR  = intCauseArray[i-1].userISR;
            intCauseArray[i].arg      = intCauseArray[i-1].arg;
            intCauseArray[i].prio     = intCauseArray[i-1].prio;    
        }
    }

    intUnlock(sysIntOldConfig);
    return OK;
}

/*******************************************************************************
* frcDmaIntEnable - Enable a DMA interrupt
*
* DESCRIPTION:
*       This routine unmasks a specified DMA interrupt cause on the appropriate  
*       mask register.
*       The routine will preform argument validity check. 
*
* INPUT:
*       cause - DMA interrupt cause as defined in DMA_CAUSE.  
*
* OUTPUT:
*       The appropriate bit in the appropriate mask register is set.
*
* RETURN:
*       OK    - If the bit was unmasked
*       ERROR - if the bit was invalid
*
*******************************************************************************/
STATUS frcDmaIntEnable(DMA_CAUSE cause)
{
    volatile UINT32 *maskReg;         /* The issued mask register */           

#ifdef PMC280_DUAL_CPU
    /* Lock */
    frcMv64360semTake(DMA_SEM, MV64360_SMP_WAIT_FOREVER);

    if (INVALID_CAUSE(cause))
    {
        logMsg("\nfrcDmaIntEnable: Invalid cause %d\n", cause,0,0,0,0,0);
        return ERROR;
    }
    else if(cause == DMA_CHAN0_COMP || cause == DMA_CHAN1_COMP ||
            cause == DMA_CHAN2_COMP || cause == DMA_CHAN3_COMP)
    {
        GT_INT_CAUSE gtCause;
#ifdef PMC280_DUAL_CPU
        switch(cause)
        {
            case DMA_CHAN0_COMP: gtIntEnable(IDMA0); break;
            case DMA_CHAN1_COMP: gtIntEnable(IDMA1); break;
            case DMA_CHAN2_COMP: gtIntEnable(IDMA2); break;
            case DMA_CHAN3_COMP: gtIntEnable(IDMA3); break;
        }
#else
        gtCause = (cause << 3 ) | 0x4; /* to enable IDMA0-3 in main cause */
        gtIntEnable(gtCause);
#endif /* PMC280_DUAL_CPU */
    }
    maskReg = &DMA_0_3_MASK_REG;   
    *maskReg |= SWAP_CAUSE_BIT(cause);

    /* Unlock */
    frcMv64360semGive(DMA_SEM);
#else
    if (INVALID_CAUSE(cause))
    {
        logMsg("\nfrcDmaIntEnable: Invalid cause %d\n", cause,0,0,0,0,0);
        return ERROR;
    }
    else if(cause == DMA_CHAN0_COMP || cause == DMA_CHAN1_COMP ||
            cause == DMA_CHAN2_COMP || cause == DMA_CHAN3_COMP)
    {
        GT_INT_CAUSE gtCause;
        gtCause = (cause << 3 ) | 0x4; /* to enable IDMA0-3 in main cause */
        gtIntEnable(gtCause);
    }
    maskReg = &DMA_0_3_MASK_REG;   
        
    *maskReg |= SWAP_CAUSE_BIT(cause);
#endif /* PMC280_DUAL_CPU */
    return OK;
}

/*******************************************************************************
* frcDmaIntDisable - Disable a DMA interrupt
*
* DESCRIPTION:
*       This routine masks a specified DMA interrupt cause on the appropriate 
*       mask register.
*       The routine will preform argument validity check. 
*
* INPUT:
*       cause - DMA interrupt cause as defined in DMA_CAUSE. 
*
* OUTPUT:
*       The appropriate bit in the appropriate mask register is reset.
*
* RETURN:
*       OK    - If the bit was masked
*       ERROR - if the bit was invalid
*
*******************************************************************************/
STATUS frcDmaIntDisable(DMA_CAUSE cause)
{
    volatile UINT32 *maskReg;       /* The issued mask register */

#ifdef PMC280_DUAL_CPU
    /* Lock */
    frcMv64360semTake(DMA_SEM, MV64360_SMP_WAIT_FOREVER);

    if (INVALID_CAUSE(cause))
    {
        logMsg("\nfrcDmaIntDisable: Invalid cause %d\n", cause,0,0,0,0,0);
        return ERROR;
    }
    else if(cause == DMA_CHAN0_COMP || cause == DMA_CHAN1_COMP ||
            cause == DMA_CHAN2_COMP || cause == DMA_CHAN3_COMP)
    {
        GT_INT_CAUSE gtCause;
#ifdef PMC280_DUAL_CPU
        switch(cause)
        {
            case DMA_CHAN0_COMP: gtIntDisable(IDMA0); break;
            case DMA_CHAN1_COMP: gtIntDisable(IDMA1); break;
            case DMA_CHAN2_COMP: gtIntDisable(IDMA2); break;
            case DMA_CHAN3_COMP: gtIntDisable(IDMA3); break;
        }
#else
        gtCause = (cause << 3 ) | 0x4; /* to disable IDMA0-3 in main cause */
        gtIntDisable(gtCause);
#endif /* PMC280_DUAL_CPU */
    }
    maskReg = &DMA_0_3_MASK_REG;   
    *maskReg &= ~(SWAP_CAUSE_BIT(cause));

    /* Unlock */
    frcMv64360semGive(DMA_SEM);
#else
    if (INVALID_CAUSE(cause))
    {
        logMsg("\nfrcDmaIntDisable: Invalid cause %d\n", cause,0,0,0,0,0);
        return ERROR;
    }
    else if(cause == DMA_CHAN0_COMP || cause == DMA_CHAN1_COMP ||
            cause == DMA_CHAN2_COMP || cause == DMA_CHAN3_COMP)
    {
        GT_INT_CAUSE gtCause;
        gtCause = (cause << 3 ) | 0x4; /* to disable IDMA0-3 in main cause */
        gtIntDisable(gtCause);
    }
    maskReg = &DMA_0_3_MASK_REG;   
        
    *maskReg &= ~(SWAP_CAUSE_BIT(cause));
#endif
    return OK;
}

/*******************************************************************************
* frcDmaCompIntHandler - DMA 0-3 completion interrupt controller routine.
*
* DESCRIPTION:
*       This routine handles the DMA 0-3 completion interrupts. 
*       As soon as the interrupt signal is active the CPU analyzes the GT DMA 
*       0-3 Interrupt Cause register in order to locate the originating 
*       interrupt event. 
*       Then the routine calls the user specified service routine for that 
*       interrupt cause.
*       The function scans the dma0_3Array[] (dma0_3Count valid entries)
*       trying to find a hit in the dma0_3Array cause table.
*       When found, the ISR in the same entry is executed.
*
* INPUT:
*       None.
*
* OUTPUT:
*       If a cause bit is active and it's connected to an ISR function,
*       the function will be called.
*
* RETURN:
*       None.
*
*******************************************************************************/
void frcDmaCompIntHandler (void)
{
    int     i; 
    unsigned int causeAndMask;

#ifdef PMC280_DUAL_CPU
    /* Lock */
    frcMv64360semTake(DMA_SEM, MV64360_SMP_WAIT_FOREVER);

    /* Cause bit is active if an interrupt occured and the bit is unmasked. */
    causeAndMask = DMA_0_3_CAUSE_REG & DMA_0_3_MASK_REG ;

    for(i = 0; i < dma0_3Count; i++)

        if(causeAndMask & dma0_3Array[i].causeBit)
        {
            /* Perform the ISR */
            (*dma0_3Array[i].userISR) (dma0_3Array[i].arg);

            /* Reset the interrutp cause bit in the DMA cause register */
            DMA_0_3_CAUSE_REG = ~(dma0_3Array[i].causeBit);
        }                                                            

    /* Unlock */
    frcMv64360semGive(DMA_SEM);
#else
    /* Cause bit is active if an interrupt occured and the bit is unmasked. */
    causeAndMask = DMA_0_3_CAUSE_REG & DMA_0_3_MASK_REG ;

    for(i = 0; i < dma0_3Count; i++)

        if(causeAndMask & dma0_3Array[i].causeBit)
        {
            /* Perform the ISR */
            (*dma0_3Array[i].userISR) (dma0_3Array[i].arg);

            /* Reset the interrutp cause bit in the DMA cause register */
            DMA_0_3_CAUSE_REG = ~(dma0_3Array[i].causeBit);
        }                                                            
#endif /* PMC280_DUAL_CPU */
}


/*******************************************************************************
* frcDmaErrIntHandler - DMA 0-3 Error interrupt controller routine
*
* DESCRIPTION:
*       This routine handles the DMA 0-3 Error interrupts. 
*       As soon as the interrupt signal is active the CPU analyzes the GT DMA 
*       0-3 Interrupt Cause register in order to locate the originating 
*       interrupt event. 
*       Then the routine calls the user specified service routine for that 
*       interrupt cause.
*       The function scans the dmaError0_3Array[] (dmaError0_3count valid 
*       entries) trying to find a hit. 
*       When found, the ISR in the same entry is executed.
*
* INPUT:
*       None.
*
* OUTPUT:
*       If a cause bit is active and it's connected to an ISR function,
*       the function will be called.
*
* RETURN:
*       None.
*
*******************************************************************************/
void frcDmaErrIntHandler(void)
{
    int     i;         
    unsigned int causeAndMask;

#ifdef PMC280_DUAL_CPU
    /* Lock */
    frcMv64360semTake(DMA_SEM, MV64360_SMP_WAIT_FOREVER);

    /* Cause bit is active if an interrupt occured and the bit is unmasked. */
    causeAndMask = DMA_0_3_CAUSE_REG & DMA_0_3_MASK_REG ;

    for(i = 0; i < dmaError0_3Count; i++)
        if(causeAndMask & dmaError0_3Array[i].causeBit)
        {
            /* Perform the ISR */
            (*dmaError0_3Array[i].userISR) (dmaError0_3Array[i].arg);

            /* Reset the interrutp cause bit in the DMA cause register */
            DMA_0_3_CAUSE_REG = ~(dmaError0_3Array[i].causeBit);
        }

    /* Unlock */
    frcMv64360semGive(DMA_SEM);
#else
    /* Cause bit is active if an interrupt occured and the bit is unmasked. */
    causeAndMask = DMA_0_3_CAUSE_REG & DMA_0_3_MASK_REG ;

    for(i = 0; i < dmaError0_3Count; i++)
        if(causeAndMask & dmaError0_3Array[i].causeBit)
        {
            /* Perform the ISR */
            (*dmaError0_3Array[i].userISR) (dmaError0_3Array[i].arg);

            /* Reset the interrutp cause bit in the DMA cause register */
            DMA_0_3_CAUSE_REG = ~(dmaError0_3Array[i].causeBit);
        }
#endif /* PMC280_DUAL_CPU */
}
