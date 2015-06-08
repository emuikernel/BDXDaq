/******************************************************************************
*              (c), Copyright, MOTOROLA  INC  - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA  OR ANY THIRD PARTY. MOTOROLA  RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

*******************************************************************************
* vxMpscIntCntl.c - MPSC Interrupt controller library
*
* DESCRIPTION:
*       This driver provides various interface routines to manipulate and
*       connect the hardware interrupts generated by the MPSC 0 and MPSC 1. 
*
*   Main features:
*       - The controller provides an easy way to hook a C Interrupt Service
*         Routine (ISR) to a specific interrupt caused by the MPSC channels.   
*       - This controller automatically acknowledges interrupts.
*       - The controller interrupt mechanism provides a way for the programmer
*         to set the priority of an interrupt.
*       - Full interrupt control over the MV MPSC channels.
*
*       Overall Interrupt Architecture description:
*       The MV handles interrupts in two stages:
*       STAGE 1: Main Cause register that summarize the interrupts generated by
*		         each Internal MV unit.
*       STAGE 2: Unit specified cause registers, that distinguish between each
*                specific interrupt event.
*       This System Interrupt Controller Driver handles the various interrupts 
*       generated by the MPSC interrupt cause register.
*
*       The driver's execution flow has three phases:
*       1) Driver initialization.
*          This initiation includes hooking driver's ISR to 
*          the MV Interrupt controller. Composed of vxMpscIntCntlInit() routine.
*       2) User ISR connecting. Here information about system ISR and interrupt 
*          priority is gathered. Composed of vxMpscIntConnect() routine.
*       3) Interrupt handler. Here an interrupt is being handle by the Interrupt 
*          Handlers (driver's ISR). Composed of mpsc0Handler() and 
*          mpsc0Handler(). Interrupt acknowledge is also done in this stage.
*
*   Full API:
*      vxMpscIntCntlInit() - Initiate MPSC Interrupt Controller driver.
*      vxMpscIntConnect()  - Connect the system ISR to a MPSCx interrupt event.
*      mpsc0Handler() 	   - Handle MPSC 0 interrupts.
*      mpsc1Handler() 	   - Handle MPSC 1 interrupts.
*      vxMpscIntDisable()  - Disable an MPSC interrupt cause.
*      vxMpscIntEnable()   - Enable an MPSC interrupt cause.
*
*
*   The controller concept is very simple:
*   Each of the Interrupt handlers has a table which holds information on 
*   the connected ISR. For example, mpsc0Handler() has mpsc0Array[] which 
*   holds the ISR concerning all MPSC 0 interrupts events. An Interrupt 
*   generated by one of those pins will result a search through this table in
*   order to locate the generating interrupt cause. After the initiating
*   interrupt cause is identify, the ISR reside in the same table entry is
*   executed. Before the ISR executes the interrupt is acknowledged.
*
*   The controller interface also includes interrupt control routines which can 
*   enable/disable specific interrupts: 
*   vxMpscIntDisable() and vxMpscIntEnable().
*
* DEPENDENCIES:
*
******************************************************************************/

/* includes */
#include <vxWorks.h>
#include "mpsc.h" 
#include "gtIntControl.h" 
#include "vxMpscIntCtrl.h" 
#include "intLib.h"  
#include "logLib.h" 

#ifdef PMC280_DEBUG_UART_VX
#include "dbgprints.h"
#endif /* PMC280_DEBUG_UART_VX */

#ifdef PMC280_DUAL_CPU
#include "gtSmp.h"
#endif /* PMC280_DUAL_CPU */

#include "config.h"

#ifdef INCLUDE_WINDVIEW
#include "private/funcBindP.h"
#include "private/eventP.h"
#include "wvLib.h"
#endif



/* defines  */

#define CAUSE_BIT(cause)		(1 << cause)

#define MPSC0_CAUSE_REG		(REG_CONTENT(MPSC_CAUSE_REG(MPSC_0)))
#define MPSC0_MASK_REG  	(REG_CONTENT(MPSC_MASK_REG(MPSC_0)))

#define MPSC1_CAUSE_REG		(REG_CONTENT(MPSC_CAUSE_REG(MPSC_1)))         
#define MPSC1_MASK_REG  	(REG_CONTENT(MPSC_MASK_REG(MPSC_1)))          

#define INVALID_CAUSE(cause)    \
                ((cause) <= MPSC_CAUSE_START || MPSC_CAUSE_END <= (cause))

/* typedefs */


/* locals   */
static GT_ISR_ENTRY mpsc0Array[32]; /* MPSC0 Int connection data storage    */
static GT_ISR_ENTRY mpsc1Array[32]; /* MPSC1 Int connection data storage    */                                                    

static int mpsc0Count = 0;  /* Accumulates MPSC 0 number of connection */ 
static int mpsc1Count = 0;  /* Accumulates MPSC 1 number of connection */

static int mpscIntCtrlInitialized = false;   

static void mpsc0Handler(void); /* MPSC 0 handler */
static void mpsc1Handler(void); /* MPSC 1 handler */

/*******************************************************************************
* vxMpscIntCntlInit - Initiating the MPSC Interrupt Controller driver.
*
* DESCRIPTION:
*       This routines connects the drivers interrupt handlers, each to its 
*       corresponding bit in the MV main Interrupt Controller using the 
*       gtIntConnect() routine. 
*
* INPUT:
*       None.
*
* OUTPUT:
*       Driver's ISR are connected to the CPU's Interrupt vector and 
*       interrupts are unmasked.
*
* RETURN:
*       None.
*
*******************************************************************************/
void vxMpscIntCntlInit()
{

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Entering vxMpscIntCntlInit\n");
    }
    else
    {
        dbg_puts0("CPU0: Entering vxMpscIntCntlInit\n");
    }
#else
    dbg_puts0("Entering vxMpscIntCntlInit\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

    if(mpscIntCtrlInitialized)
        return;

#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
	MPSC1_CAUSE_REG	= 0;  /* Clean Mask register (case of warm reboot)	*/
	MPSC1_MASK_REG 	= 0;  /* Clean Mask register (case of warm reboot)	*/
    }
    else
    {
	MPSC0_CAUSE_REG	= 0;  /* Clean Cause register (case of warm reboot)	*/
	MPSC0_MASK_REG 	= 0;  /* Clean Cause register (case of warm reboot)	*/
    }
#else

	MPSC0_CAUSE_REG	= 0;  /* Clean Cause register (case of warm reboot)	*/
	MPSC0_MASK_REG 	= 0;  /* Clean Cause register (case of warm reboot)	*/
	
	MPSC1_CAUSE_REG	= 0;  /* Clean Mask register (case of warm reboot)	*/
	MPSC1_MASK_REG 	= 0;  /* Clean Mask register (case of warm reboot)	*/
#endif /* PMC280_DUAL_CPU */

#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        gtIntConnect(MPSC1, mpsc1Handler, 0, MPSC1_ISR_PRIO);      
        gtIntEnable(MPSC1);
    }
    else
    {
        gtIntConnect(MPSC0, mpsc0Handler, 0, MPSC0_ISR_PRIO);      
        gtIntEnable(MPSC0);
    }
#else		
    /* Connect the user ISR's to the MPSCs bits in the main cause register */
    gtIntConnect(MPSC0, mpsc0Handler, 0, MPSC0_ISR_PRIO);      
    gtIntConnect(MPSC1, mpsc1Handler, 0, MPSC1_ISR_PRIO);      

    /* Unmask those bits */
    gtIntEnable(MPSC0);
    gtIntEnable(MPSC1);
#endif /* PMC280_DUAL_CPU */

    mpscIntCtrlInitialized = true;

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Exiting vxMpscIntCntlInit\n");
    }
    else
    {
        dbg_puts0("CPU0: Exiting vxMpscIntCntlInit\n");
    }
#else
    dbg_puts0("Exiting vxMpscIntCntlInit\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */
}

/*******************************************************************************
* vxMpscIntConnect - connect a C routine to a specific MPSC interrupt.
*
* DESCRIPTION:
*       This routine connects a specified ISR to a specified MPSC channel 
*       interrupt cause. Each ISR handler has its own user ISR array. 
*       The connection is done by setting the desired routine and parameter 
*       in the correspond cause array (i.e. mpsc0Array[]):
*       1) Check for existing connection for the same cause bit in the table.
*       2) Connecting the user ISR by inserting the given parameters into an
*          entry according to the user ISR given priority.
*
* INPUT:
*	MPSC_NUM    sdmaNum - MPSC channel number. See MPSC_NUM.
*      MPSC_CAUSE    cause - MPSC interrupt cause as defined in MPSC_CAUSE. 		
*       VOIDFUNCPTR routine - user ISR.
*       int       parameter - user ISR parameter.
*       int            prio - Interrupt handling priority where 0 is highest.    
*
* OUTPUT:
*       An appropriate table entry is filled.
*
* RETURN:
*       OK    if succeeded 
*       ERROR if cause argument is wrong or connected cause is already 
*             found in table.
*
*******************************************************************************/
bool vxMpscIntConnect(MPSC_NUM 	mpscNum,   
		  MPSC_CAUSE 	cause,               
		  VOIDFUNCPTR routine,           
		  int 		parameter,                 
		  int 		prio)                      
{
    GT_ISR_ENTRY *intCauseArray; /* pointer to the selected cause table */
    int *intConnectCount;   /* pointer to the table connection counter*/
    int i, sysIntOldConfig;

    sysIntOldConfig = intLock();
    /* check for cause parameter validity */
    if(routine == NULL)
    {
        logMsg("vxMpscIntConnect: NULL pointer routine\n",0,0,0,0,0,0);
        intUnlock(sysIntOldConfig);
        return false;
    }
    else if(prio < 0)
    {
        logMsg("vxMpscIntConnect: Invalid interrupt priority\n",0,0,0,0,0,0);
        intUnlock(sysIntOldConfig);
        return false;
    }
    
	switch(mpscNum)
	{
		case(MPSC_0):
				intCauseArray = mpsc0Array;
				intConnectCount = &mpsc0Count;
			break;
		
		case(MPSC_1):
				intCauseArray = mpsc1Array;
				intConnectCount = &mpsc1Count;
			break;
		
		default:
			logMsg("\vxMpscIntConnect: Invalid cause %d\n",cause,0,0,0,0,0);
			intUnlock(sysIntOldConfig);
			return false;
    }

    /* Scan the table to check for double connection to the same cause bit */
    for(i=*intConnectCount; i >= 0; i--)
        if(intCauseArray[i].causeBit == SWAP_CAUSE_BIT(cause))
        {
            logMsg("vxMpscIntConnect(cause %d): ERROR\nAn attempt to reconnect \
                   ISR to an existing cause bit.\n", cause,0,0,0,0,0); 
            intUnlock(sysIntOldConfig);
            return false;
        }

        /* Connection phase */
    for(i=*intConnectCount; i>=0; i--)
    {
        if(i==0 || intCauseArray[i-1].prio < prio) /* Make connection */
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
    return true;
}

/*******************************************************************************
* vxMpscIntEnable - Enable an MPSC interrupt
*
* DESCRIPTION:
*       This routine unmasks a specified MPSC interrupt in the mask register.
*       The routine will preform argument validity check. 
*
* INPUT:
*		MPSC_NUM sdmaNum - MPSC channel number. See MPSC_NUM.
*       MPSC_CAUSE cause - MPSC interrupt cause as defined in MPSC_CAUSE. 		
*
* OUTPUT:
*       The appropriate bit in the MPSC mask register is set.
*
* RETURN:
*       true    - If the bit was set
*       false - if the bit was invalid
*
*******************************************************************************/
bool vxMpscIntEnable(MPSC_NUM mpscNum, MPSC_CAUSE cause)
{
    unsigned int maskReg;
    
    if (INVALID_CAUSE(cause))
    {
        logMsg("\nvxSdmaIntEnable: Invalid cause %d\n", cause,0,0,0,0,0);
        return false;
    }
    
    GT_REG_READ(MPSC_MASK_REG(mpscNum), &maskReg); 
    maskReg |= CAUSE_BIT(cause);
    GT_REG_WRITE(MPSC_MASK_REG(mpscNum), maskReg); 
    
    return true;
}

/*******************************************************************************
* vxMpscIntDisable - Disable an MPSC interrupt
*
* DESCRIPTION:
*       This routine masks a specified MPSC interrupt in the mask register.
*       The routine will preform argument validity check. 
*
* INPUT:
*		MPSC_NUM sdmaNum - MPSC channel number. See MPSC_NUM.
*       MPSC_CAUSE cause - MPSC interrupt cause as defined in MPSC_CAUSE. 		
*
* OUTPUT:
*       The appropriate bit in the MPSC mask register is reset.
*
* RETURN:
*       true    - If the bit was reset
*       false - if the bit was invalid
*
*******************************************************************************/
bool vxMpscIntDisable(MPSC_NUM mpscNum, MPSC_CAUSE cause)
{
    unsigned int maskReg;
    
    if (INVALID_CAUSE(cause))
    {
        logMsg("\nvxSdmaIntEnable: Invalid cause %d\n", cause,0,0,0,0,0);
        return false;
    }
    
	GT_REG_READ(MPSC_MASK_REG(mpscNum), &maskReg); 
	maskReg &= ~CAUSE_BIT(cause);
	GT_REG_WRITE(MPSC_MASK_REG(mpscNum), maskReg); 
    
	return true;
}

/*******************************************************************************
* vxMpscIntClear - Clear an MPSC interrupt
*
* DESCRIPTION:
*       This routine clears a specified MPSC interrupt in the cause register.
*       The routine will preform argument validity check. 
*
* INPUT:
*		MPSC_NUM mpscNum - MPSC channel number. See MPSC_NUM.
*       MPSC_CAUSE cause - MPSC interrupt cause as defined in MPSC_CAUSE. 		
*
* OUTPUT:
*       The appropriate bit in the MPSC cause register is reset.
*
* RETURN:
*       true    - If the bit was reset
*       false - if the bit was invalid
*
*******************************************************************************/
bool vxMpscIntClear(MPSC_NUM mpscNum, MPSC_CAUSE cause)
{
    if (INVALID_CAUSE(cause))
    {
        logMsg("\nvxSdmaIntEnable: Invalid cause %d\n", cause,0,0,0,0,0);
        return false;
    }

	GT_REG_WRITE(MPSC_CAUSE_REG(mpscNum), ~CAUSE_BIT(cause));     
	return true;
}

/*******************************************************************************
* mpsc0Handler - MPSC channel 0 Interrupt controller handler.
*
* DESCRIPTION:
*       This routine handles the MPSC channel 0 interrupt events.
*       As soon as the interrupt signal is active the CPU analyzes the MPSC 0 
*       Interrupt Cause register in order to locate the originating 
*       interrupt event. 
*       Then the routine calls the user specified service routine for that 
*       interrupt cause.
*       The function scans the mpsc0Array[] (mpsc0Count valid entries)
*       trying to find a hit in the mpsc0Array cause table.
*       When found, the ISR in the same entry is executed.
*		Note: The handler automatically acknowledges the generating interrupts.
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
void mpsc0Handler(void)
{
    int     i;
    unsigned int activeInt;
#ifdef INCLUDE_WINDVIEW
	WV_EVT_INT_ENT(MPSC0);  /* support for windview */
#endif    


    activeInt = MPSC0_CAUSE_REG & MPSC0_MASK_REG;

    for(i = 0; i < mpsc0Count; i++)
    {                                                                                
	/* Check ISR array hit  */                                                    
        if(activeInt & mpsc0Array[i].causeBit)
        { 
            /* Reset the interrupt cause bit in cause register */
            MPSC0_CAUSE_REG = ~(mpsc0Array[i].causeBit);
            
            /* Perform the ISR */
            (*mpsc0Array[i].userISR) (mpsc0Array[i].arg);
        } 
    }
}


/*******************************************************************************
* mpsc1Handler - MPSC channel 1 Interrupt controller handler.
*
* DESCRIPTION:
*       This routine handles the MPSC channel 1 interrupt events.
*       As soon as the interrupt signal is active the CPU analyzes the MPSC 1 
*       Interrupt Cause register in order to locate the originating 
*       interrupt event. 
*       Then the routine calls the user specified service routine for that 
*       interrupt cause.
*       The function scans the mpsc1Array[] (mpsc1Count valid entries)
*       trying to find a hit in the mpsc1Array cause table.
*       When found, the ISR in the same entry is executed.
*		Note: The handler automatically acknowledges the generating interrupts.
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
void mpsc1Handler(void)
{
    int     i;
    unsigned int activeInt;
#ifdef INCLUDE_WINDVIEW
	WV_EVT_INT_ENT(MPSC1);  /* support for windview */
#endif    

    activeInt = MPSC1_CAUSE_REG & MPSC1_MASK_REG;
				    
    for(i = 0; i < mpsc1Count; i++)
    {                                                                                
	/* Check ISR array hit  */                                                    
        if(activeInt & mpsc1Array[i].causeBit)
        { 
            /* Reset the interrupt cause bit in cause register */
            MPSC1_CAUSE_REG = ~(mpsc1Array[i].causeBit);
            
            /* Perform the ISR */
            (*mpsc1Array[i].userISR) (mpsc1Array[i].arg);
        } 
    }
}
