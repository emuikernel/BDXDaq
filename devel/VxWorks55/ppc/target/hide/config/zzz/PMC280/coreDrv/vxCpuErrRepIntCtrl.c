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
* vxCpuErrRepIntCtrl.c - CPU Error Report interrurpt controller library
*
* DESCRIPTION:
*       This driver provides various interface routines to manipulate and 
*       connect the hardware interrupts concerning the GT CPU facility.
*
*       Main features:
*       - The controller provides an easy way to hook a C Interrupt Service
*         Routine (ISR) to a specific interrupt caused by the GT CPU Error 
*		  Report mechanism.
*       - The controller interrupt mechanism provides a way for the programmer 
*         to set the priority of an interrupt.
*       - Full interrupt control over the GT CPU Error Report facility.
*
*       Overall Interrupt Architecture description:
*       The GT handles interrupts in two stages:
*       STAGE 1: Main Cause register that summarize the interrupts generated by 
*   	         each Internal GT unit.
*       STAGE 2: Unit specified cause registers, that distinguish between each 
*		         specific interrupt event.
*       This CPU Error Report Interrupt Controller Driver handles the various 
*		interrupts generated by the second stage.
*
*       All CPU Error Report various interrupt causes are numbered using the
*       CPU_ERR_REP_CAUSE enumerator. Thus there are 32 interrupts causes 
*       (most of them are Reserved). 
*
*       The driver's execution flow has three phases:
*         1) Driver initialization. This initiation includes hooking driver's
*            ISR to the GT Interrupt controller. Composed of
*            vxCpuErrRepIntCtrlInit() routine.
*         2) User ISR connecting. Here information about user ISR and interrupt 
*            priority is gathered. Composed of vxCpuErrRepIntConnect() routine.
*         3) Interrupt handler. Here an interrupt is being handle by the  
*            Interrupt Handlers (driver's ISR). Composed of vxCpuErrRepInt(), 
*
*       Full API:    
*        vxCpuErrRepIntCtrlInit() - Initiate the CPU Error Report driver.
*        vxCpuErrRepIntConnect()  - Connect a user ISR to a CPU Error Report 
*                                   interrupt event.
*        vxCpuErrRepIntEnable()   - Enable a given CPU Error Report interrupt.
*        vxCpuErrRepIntDisable()  - Disable a given CPU Error Report interrupt.
*        vxCpuErrRepInt()         - CPU Error Report event handler
*    
*       The controller concept is very simple:
*       The Interrupt handlers has a table which holds information on the
*       connected ISR. cpuErrRepArray[] is the table which holds the ISR  
*       concerning the CPU Error Report. An Interrupt generated by the CPU 
* 		Error Report will result a search through this table in order to 
*		allocate the generating interrupt cause. After the initiating interrupt 
*		cause is identify, the ISR reside in the same table entry is executed.
*
*       The controller interface also includes interrupt control routines which 
*		can enable/disable specific interrupts: vxCpuErrRepIntDisable() and 
*       vxCpuErrRepIntEnable().
*
*
* DEPENDENCIES:
*		GT Interrupt Control Driver.
*		VxWorks types and interrupt lib.
*
******************************************************************************/
 
/* includes */
#include "vxCpuErrRepIntCtrl.h" 
#include "config.h"

#ifdef INCLUDE_WINDVIEW
#include "private/funcBindP.h"
#include "private/eventP.h"
#include "wvLib.h"
#endif  /* INCLUDE_WINDVIEW */
 
/* defines  */
#define CPU_ERR_REP_CAUSE_REG     REG_CONTENT(CPU_ERROR_CAUSE)  
#define CPU_ERR_REP_MASK_REG      REG_CONTENT(CPU_ERROR_MASK)  

#define INVALID_CAUSE(cause)    \
                ((cause) <= CPU_ERR_CAUSE_START || CPU_ERR_CAUSE_END <= (cause))


/* typedefs */
 
 
/* locals   */

/* cpuErrRepArray[] accumulates user ISR information for CPU Error 			*/
/* Report, This table holds the ISR functions for the interrupt bits, 		*/
/* in the order of their priority. Not all the bits have an ISR.      		*/
LOCAL GT_ISR_ENTRY cpuErrRepArray[32];

/*The counter accumulates the number of connection done to the cause array. */
LOCAL int cpuErrRepCount = 0; 

LOCAL bool vxCpuErrRepIntCtrlInitialized = false;

/* Interrupt Controller's ISR 		*/
LOCAL void  vxCpuErrRepInt (void); 	/* CPU Error Report event handler */
  
              
/*******************************************************************************
* vxCpuErrRepIntCtrlInit - Initiating the CPU Error report Interrupt Controller.
*
* DESCRIPTION:
*       This routines connects the driver interrupt handler, to its 
*       corresponding bit in the GT main Interrupt Controller using 
*       the gtIntConnect() routine.
* 
* INPUT:
*       None.
*
* OUTPUT:
*       The CPU Error Report casue & mask register are initiated (set to zero).
*       Driver's ISR is connected to the main cause register. 
*       interrupt is unmasked.
*
* RETURN:
*       None.
*
*******************************************************************************/
void vxCpuErrRepIntCtrlInit(void)
{
#ifdef PMC280_DUAL_CPU
    if(!frcWhoAmI())
    {
        if(vxCpuErrRepIntCtrlInitialized)
            return;
        
        CPU_ERR_REP_CAUSE_REG = 0;  /* Clean Cause regiser (case of warm reboot)*/
        CPU_ERR_REP_MASK_REG = 0;   /* Clean Mask regiser (case of warm reboot) */
    
        /* Connect a user ISR to the CPU Error Report bit in the main cause */
        /* register                                                         */
        gtIntConnect(CPU_ERR, vxCpuErrRepInt, 0,9);
    
        /* Unmask that bit */
        gtIntEnable(CPU_ERR);

        vxCpuErrRepIntCtrlInitialized = true;
    }
#else
    if(vxCpuErrRepIntCtrlInitialized)
        return;
        
    CPU_ERR_REP_CAUSE_REG = 0;  /* Clean Cause regiser (case of warm reboot)*/
    CPU_ERR_REP_MASK_REG = 0;   /* Clean Mask regiser (case of warm reboot) */
    
    /* Connect a user ISR to the CPU Error Report bit in the main cause */
    /* register                                                         */
    gtIntConnect(CPU_ERR, vxCpuErrRepInt, 0,9);
    
    /* Unmask that bit */
    gtIntEnable(CPU_ERR);

    vxCpuErrRepIntCtrlInitialized = true;
#endif /* PMC280_DUAL_CPU */

}

/*******************************************************************************
* vxCpuErrRepIntConnect - Connect a C routine to a specific CPU interrupt.
*                        
* DESCRIPTION:
*       This routine connects a specified user ISR to a specified CPU interrupt
*       cause bit (Reg 0x140).
*       The ISR handler has its own user ISR array.
*       The connection is done by setting the desired routine and parameter in 
*       the cause array (CpuErrRepArray[]):
*         1) Check for existing connection of the cause bit in the table.
*         2) Connecting the user ISR by inserting the given parameters into an 
*            entry according to the user ISR given priority.
*  
* INPUT:
*       cause     - CPU Error Report interrupt cause bit. See CPU_ERR_CAUSE. 
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
STATUS vxCpuErrRepIntConnect(CPU_ERR_CAUSE cause, VOIDFUNCPTR routine, 
                             int parameter, int prio)
{
    int i, sysIntOldConfig;

    /* Make sure that this is an atomic operation */
    sysIntOldConfig = intLock();  

	/* Check the validity of the parameters */
	if (routine == NULL) 
    {
        logMsg("\nvxCpuErrRepIntConnect: NULL pointer routine\n",0,0,0,0,0,0);
        intUnlock(sysIntOldConfig);
		return ERROR;
    }
	else if (prio < 0) 
    {
        logMsg("\nvxCpuErrRepIntConnect: Invalid interrupt priority\n",
																 0,0,0,0,0,0);
        intUnlock(sysIntOldConfig);
		return ERROR;
    }
	else if (INVALID_CAUSE(cause)) 
    {
        logMsg("\nvxCpuErrRepIntConnect: Invalid cause %d\n",cause,0,0,0,0,0);
        intUnlock(sysIntOldConfig);
		return ERROR;
    }
        
    /* Scan the table to check for double connection to the same cause bit */
    for(i = cpuErrRepCount; i >= 0; i--)
        if (cpuErrRepArray[i].causeBit == SWAP_CAUSE_BIT(cause)) 
        {
            logMsg("\nvxCpuErrRepIntConnect(cause %d): ERROR\n",cause,0,0,0,0,0);
            logMsg("   An attempt to reconnect ISR to an existing cause bit.\n",
                   0,0,0,0,0,0); 
            intUnlock(sysIntOldConfig);
            return ERROR;
        }
    

    /* Connection phase 											*/
    /* The search starts from the bottom of the table, because: 	*/
    /*  1) if the priority is the highest, we can put it straight   */
    /*     in the end of the table                                  */
    /*  2) we can push the highests priority down the table, right  */
    /*     away, without the need to copy the struct.               */
    for(i = cpuErrRepCount; i >= 0; i--)                                 
    {                                                                
        if(i == 0 || cpuErrRepArray[i-1].prio < prio) /* Make connection */
        {
            cpuErrRepArray[i].causeBit = SWAP_CAUSE_BIT(cause);
            cpuErrRepArray[i].userISR  = routine;
            cpuErrRepArray[i].arg      = parameter;
            cpuErrRepArray[i].prio     = prio;
            cpuErrRepCount++;
            break;
        }
        else
        {
            /* Push the low priority connection down the table */
            cpuErrRepArray[i].causeBit = cpuErrRepArray[i-1].causeBit;
            cpuErrRepArray[i].userISR  = cpuErrRepArray[i-1].userISR;
            cpuErrRepArray[i].arg      = cpuErrRepArray[i-1].arg;
            cpuErrRepArray[i].prio     = cpuErrRepArray[i-1].prio;    
        }
    }

    intUnlock(sysIntOldConfig);
    return OK;
}

/*******************************************************************************
* vxCpuErrRepIntEnable - Enable a CPU Error Report interrupt
*
* DESCRIPTION:
*       This routine unmasks a specified CPU interrupt bit in the mask 
*       register (Reg 0x148).
*       The routine will preform argument validity check. 
*
* INPUT:
*       cause - CPU interrupt cause as defined in CPU_ERR_CAUSE.
*
* OUTPUT:
*      The appropriate bit in the mask register 0x148 is set.
*
* RETURN:
*       OK    - If the bit was unmasked
*       ERROR - if the bit was invalid
*
*******************************************************************************/
STATUS vxCpuErrRepIntEnable(CPU_ERR_CAUSE cause)
{      

    if (INVALID_CAUSE(cause)) 
    {
        logMsg("\nvxCpuErrRepIntEnable: Invalid cause %d\n", cause,0,0,0,0,0);
        return ERROR;
    }

    CPU_ERR_REP_MASK_REG |= SWAP_CAUSE_BIT(cause);
    return OK;
}

/*******************************************************************************
* vxCpuErrRepIntDisable - Disable a CPU Error Report interrupt
*
* DESCRIPTION:
*       This routine masks a specified CPU interrupt bit in the mask
*       register (Reg 0x148). 
*       The routine will preform argument validity check. 
*
* INPUT:
*       cause - CPU interrupt cause as defined in CPU_ERR_CAUSE.  
*
* OUTPUT:
*       The appropriate bit in the mask register 0x148 is reset.
*
* RETURN:
*       OK    - If the bit was masked
*       ERROR - if the bit was invalid
*
*******************************************************************************/
STATUS vxCpuErrRepIntDisable(CPU_ERR_CAUSE cause)
{    
    if (INVALID_CAUSE(cause))
    {
       logMsg("\nvxCpuErrRepIntDisable: Invalid cause %d\n", cause,0,0,0,0,0);
       return ERROR;
    }

	CPU_ERR_REP_MASK_REG &= ~(SWAP_CAUSE_BIT(cause));
    return OK;
}

/*******************************************************************************
* vxCpuErrRepInt - CPU Error Report Interrupt controller routine.
*
* DESCRIPTION:
*       This routine handles the CPU Error Report interrupts described in 
*       cause register (0x140 ONLY).
*       As soon as the interrupt signal is active the CPU analyzes the cause 
*       register 0x140 in order to locate the originating interrupt event. 
*       Then, the routine calls the user specified service routine for that
*       cause. The function scans the cpuErrRepIntArray[] (cpuErrRepCount valid 
*       entries) trying to find a hit in the cpuErrRepIntArray cause table.
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
void vxCpuErrRepInt (void)
{
	int     i;       
    unsigned int causeAndMask;

    /* Cause bit is active if an interrupt occured and the bit is unmasked. */
    causeAndMask = CPU_ERR_REP_CAUSE_REG & CPU_ERR_REP_MASK_REG;
    
    for(i = 0; i < cpuErrRepCount; i++)
        
		if(causeAndMask & cpuErrRepArray[i].causeBit)  
        {   
		#ifdef INCLUDE_WINDVIEW
		WV_EVT_INT_ENT(CPU_ERR);  
		#endif 
            /* Perform the ISR */
            (*cpuErrRepArray[i].userISR) (cpuErrRepArray[i].arg);

            /* Reset the interrutp cause bit in the CPU cause register */
            CPU_ERR_REP_CAUSE_REG = ~(cpuErrRepArray[i].causeBit);
        }  
}
