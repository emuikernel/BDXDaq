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
* sysIntCtrl.c - System Interrupt controller library
*
* DESCRIPTION:
*       The PPC core accepts only one external interrupt exception (vector 0x500).
*       Control of asynchronous interrupts inputs are performed by this driver. 
*       This interrupt controller provides the support for the following routines:
*       1) intConnect();
*       2) intenable();
*       3) intDisable();
*       It uses the GPP Interrupt Controller which implements those
*       fanctionalities.
*       See Tornado BSP Developer's Kit for VxWorks User's Guide.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

/* includes */
#include <vxWorks.h>
#include "vxGppIntCtrl.h" 
#include "iv.h"  
#include "stdio.h"

#include <arch/ppc/esfPpc.h>
#include "arch/ppc/excPpcLib.h"
#include "pciConfigLib.h"
#include "cacheLib.h"

/* defines  */
#define INT_DEFAULT_PRIO 5


/* typedefs */


/* Locals   */
LOCAL STATUS    sysIntConnect (VOIDFUNCPTR* cause, VOIDFUNCPTR routine, 
                               int parameter);

/*Sergey: comment out LOCAL to make it global*/
/*LOCAL*/ STATUS    sysIntEnable  (int cause);

LOCAL STATUS    sysIntDisable (int cause);



LOCAL BOOL pciErrReport(int pciSeg);
LOCAL void   sdramEccErrorReport(void);
LOCAL void   sramErrorReport(void);
LOCAL STATUS sysVxMemProbeHook (
    char *  adrs,    /* address to be probed          */
    int     mode,    /* VX_READ or VX_WRITE           */
    int     length,  /* 1, 2, or 4                    */
    char *  pVal     /* where to return value,        */
		     /* or ptr to value to be written */
    );

LOCAL char	*pciCmds[] = { "IACK", "Spec", "IoRd", "IoWr", 
			       "res", "res", "MemRd", "MemWr",
			       "res", "res", "CfgRd", "CfgWr",
			       "MemRdMul", "DAC", "MemRdLn", "MemWrI" };


/*   Bit events enabled  0000000 1011 0110 0110  */
#define PCI_ERR_MASK	(0x0B66) 
	
void sysMachChkExcpInit(void);
void sysMachChkExcpHand(ESFPPC *);
/*================================== IMPORTS =================================*/
IMPORT void printExc (char* fmt, int arg1, int arg2, int arg3,int arg4, int arg5);
IMPORT void	excExcHandle (ESFPPC *);
IMPORT BOOL 	cacheDataEnabled;
IMPORT FUNCPTR	_func_vxMemProbeHook;
IMPORT void vxHid1Set(UINT32);
IMPORT UINT32 vxHid1Get();
IMPORT STATUS vxMemProbe
(
char * adrs, /* address to be probed */
int mode, /* VX_READ or VX_WRITE */
int length, /* 1, 2, 4, or 8 */
char * pVal /* where to return value, or ptr to value to be written */
);





/*******************************************************************************
* sysIntCtrlInit - Initiating the GPP Interrupt Controller driver.
*
* DESCRIPTION:
*       This driver initialize the GPP Interrupt Controller and assigns the
*       VxWorks interrupt controling routines pointers to the system interurpt
*       controller.
*
* INPUT:
*       None.
*
* OUTPUT:
*       Driver's routines are connected to the Vxworks Interrupt conroling 
*       pointers.
*
* RETURN:
*       None.
*
*******************************************************************************/
void sysIntCtrlInit(void)
{
    /* Init the GPP facility interrupt controller */
    frcGppIntCtrlInit();

    /* set the BSP driver specific Interrupt Handler and intConnect routines */
    _func_intConnectRtn = sysIntConnect;
    _func_intEnableRtn  = sysIntEnable;
    _func_intDisableRtn = sysIntDisable;
}

/*******************************************************************************
* sysIntConnect - connect a C routine to a specific System interrupt.
*
* DESCRIPTION:
*       This routine connects a specified ISR to a specified GT GPP interrupt 
*       cause. This routine connects the user ISR to the GPP Interrupt
*       Controller accordein to the given cause.
*
* INPUT:
*       *cause   : GPP interrupt cause. See GPP_CAUSE
*       routine  : user ISR.
*       parameter: user ISR parameter.
*
* OUTPUT:
*       The user ISR is connected to the GPP Interrupt Controller.
*
* RETURN:
*       OK.
*
*******************************************************************************/
STATUS sysIntConnect(VOIDFUNCPTR *cause, VOIDFUNCPTR routine, int parameter)
{
    if ( frcGppIntConnect((int)cause, routine, parameter, INT_DEFAULT_PRIO) == ERROR)
    {
	    logMsg("\n \n \n sysIntConnect : Error Occured While Connect \n",1,2,3,4,5,6);
	    return ERROR;
    }
    else
    {
    	return OK;
    }
}

/*******************************************************************************
* sysIntEnable - Enable a System interrupt
*
* DESCRIPTION:
*       This routine makes a specified System interrupt available to the CPU.
*
* INPUT:
*       int cause: GPP cause.  See GPP_CAUSE
*
* OUTPUT:
*       The GPP Interrupt Controller is invoke to disable the apropreate 
*       interrupt.
*
* RETURN:
*       OK.
*
* SEE ALSO: 
*       sysIntDisable()
*
*******************************************************************************/
STATUS sysIntEnable(int cause)
{
#ifdef PMC280_DEBUG_UART
    printf("Entering sysIntEnable: %d\n", cause);
#endif

   if( frcGppIntEnable (cause) == ERROR)
    {
    	logMsg("\n \n \n sysIntEnable : Invalid Cause Bit \n",1,2,3,4,5,6);
	return ERROR;
    } 

#ifdef PMC280_DEBUG_UART
    printf("Exiting sysIntEnable: %d\n", cause);
#endif
    return OK;
}

/*******************************************************************************
* sysIntDisable - Disable a System interrupt
*
* DESCRIPTION:
*       This routine makes a specified System interrupt unavailable to the CPU.
*
* INPUT:
*       int cause: GPP cause.  See GPP_CAUSE
*
* OUTPUT:
*       The GPP Interrupt Controller is invoke to disable the apropreate 
*       interrupt.
*
* RETURN:
*       OK.
*
* SEE ALSO: 
*       sysIntEnable()
*
*******************************************************************************/
STATUS sysIntDisable(int cause)
{
    if(frcGppIntDisable (cause) == ERROR)
    {
    	logMsg("\n \n \n sysIntDisable : Invalid Cause Bit\n",1,2,3,4,5,6);
	return ERROR;
    }
    else
    {	    
	return OK; 
    } 
}


/*******************************************************************************
* sysMachChkExcpInit - Initialize machine exception handling.
*
* This routine initializes the handling of PCI and other parity errors 
* installing a new Machine Exception Handler
* exception handler.
*
* RETURNS
* n/a
*
* NOMANUAL
*/
void sysMachChkExcpInit(void)
    {
    UINT32	tmp;
    UINT32	regVal;

    /* disable MCP exception */
    tmp = vxHid1Get();
    vxHid1Set (tmp & ~0x80000000);
    
#ifdef SRAM_ERROR_PROPAGATE
    /* Clear any pending SRAM error */
    GT_REG_WRITE(SRAM_ERROR_CAUSE, 0x0); 
    GT_REG_WRITE(SRAM_ERROR_ADDR, 0x0);
    GT_REG_WRITE(SRAM_ERROR_ADDR_HIGH, 0x0);
    GT_REG_WRITE(SRAM_ERROR_DATA_LOW, 0x0);
    GT_REG_WRITE(SRAM_ERROR_DATA_HIGH, 0x0);
    GT_REG_WRITE(SRAM_ERROR_DATA_PARITY, 0x0);
#endif /* SRAM_ERROR_PROPAGATE */   

#ifdef SDRAM_ECC_ERROR_PROPAGATE
    /* Clear any Pending SDRAM Error Address Registers */
     GT_REG_WRITE(SDRAM_ERROR_ADDR, 0x0);
     GT_REG_WRITE(SDRAM_ERROR_DATA_LOW, 0x0);
     GT_REG_WRITE(SDRAM_ERROR_DATA_HIGH,0x0);
     GT_REG_WRITE(SDRAM_RECEIVED_ECC, 0x0);
     GT_REG_WRITE(SDRAM_CALCULATED_ECC,0x0);
#endif /* SDRAM_ECC_ERROR_PROPAGATE  */   
     
#ifdef PCI_ERROR_REPORT     
    /* clear pending PCI errors */
    GT_REG_WRITE (PCI_0_ERROR_CAUSE, 0);
    GT_REG_WRITE (PCI_1_ERROR_CAUSE, 0);

    /* 
     * Clear all the Error bits forcefully 
     * in PCI Command and Status Register 
     */
    
	pciConfigInLong(0,0,0,PCI_CFG_COMMAND,&regVal);
	regVal = (regVal & 0x06FFFFFF);  /* Extract only RW Bits */

	 /* 
	  * Force the RWC bits to Zeros by writing 1's 
	  * Basically clear all the errors  
	  */
	regVal |= (0xF9000000);
	pciConfigOutLong(0,0,0,PCI_CFG_COMMAND,regVal);

#endif /* PCI_ERROR_REPORT */	
	
    /* connect routine for Machine Check Execption */
    excConnect ((VOIDFUNCPTR *) _EXC_OFF_MACH,
                   (VOIDFUNCPTR) sysMachChkExcpHand);
    
     /* Enable Parity Error Propagation Error for SRAM */  
#ifdef SRAM_ERROR_PROPAGATE
    GT_REG_READ (SRAM_CONFIG, &regVal);
    regVal |= 0x00000020;               /* Enable Parity Error Propagation Error */ 
    GT_REG_WRITE(SRAM_CONFIG, regVal); 
#endif /* SRAM_ERROR_PROPAGATE */

     /* Enable Propagation of errors to ECC bank  */
#ifdef SDRAM_ECC_ERROR_PROPAGATE
    GT_REG_READ (SDRAM_ECC_CONTROL, &regVal);
    regVal |= 0x00000200;             /* Enable Propagation of errors to ECC */
    GT_REG_WRITE(SDRAM_ECC_CONTROL, regVal); 
#endif /*  SDRAM_ECC_ERROR_PROPAGATE */

	/* 
	 * Enable the SERREN and PERREN Bits 
	 */
#ifdef PCI_ERROR_REPORT
	pciConfigInLong(0,0,0,PCI_CFG_COMMAND,&regVal);
	regVal = (regVal & 0x06FFFFFF);  /* Extract only RW Bits */
	 /*
	  *  Force the RWC bits to Zeros by writing 1's 
	  *  Basically clear all the errors and also 
	  *  Enable SERREn bit (0x100 )
	  *  Enable PERREn bit (0x040 )
	  * 
	  */
	regVal |= (0xF9000000 | 0x00000100 | 0x00000040);
	pciConfigOutLong(0,0,0,PCI_CFG_COMMAND,regVal);
	
      /* 
       * Enable the SERR Mask Register Bits  
       * for PCI Errors to be reported 
       */ 
    GT_REG_WRITE (PCI_0_SERR_MASK, PCI_ERR_MASK);
    GT_REG_WRITE (PCI_1_SERR_MASK, PCI_ERR_MASK);
#endif 
  
   /* 
    * Enable MCP interrupts in processor 
    * Enable 60x bus address parity checking
    * Enable 60x bus data parity checking
    */
    tmp = vxHid1Get();
        vxHid1Set (tmp | 0x80000000 | 0x20000000 | 0x10000000); 

	 _func_vxMemProbeHook = (FUNCPTR)sysVxMemProbeHook;
    }


/*******************************************************************************
* sysMcIntHandler - Handler for machine check errors
*
* RETURNS
* n/a
*/
void sysMachChkExcpHand
    (
    ESFPPC *    pEsf
    )
    {
	
	UINT32	regVal;
	BOOL	pciErr = FALSE;

	BOOL	cacheWasEnabled;

        cacheWasEnabled = cacheDataEnabled;
	
	/* 
	   Extract only bits concerned with Machine Check Exception 
	   i.e bit[11:15] 
	 */
	/* Disable data cache to fix catastrophic error test problem */
	cacheDisable (DATA_CACHE);
#if 0	
    regVal = vxSrr1Get();
	printExc("\nSRR1 = 0x%x\n",regVal,0,0,0,0);	
	regVal = ((regVal & 0x001F0000) >> 16);
	printExc("regVal = 0x%x\n",regVal,0,0,0,0);
	
	if(regVal & 0x10)
	{
		printExc("\n L2 data cache double bit error is detected\n",0,0,0,0,0);
	}	
	if(regVal & 0x8)
	{
		printExc("\n Assertion of MCP pin \n",0,0,0,0,0);
	}
	if(regVal & 0x4)
	{
		printExc("\n TEA assertion on 60x bus\n",0,0,0,0,0);
	}
	if(regVal & 0x2)
	{
		printExc("\n Data Bus Parity Error on 60x bus\n",0,0,0,0,0);
	}
	if(regVal & 0x1)
	{
		printExc("\n Address Bus Parity Error on 60x bus\n",0,0,0,0,0);
	}
#endif	
	
       printExc ("\nCALLING customized :\n", 0, 0, 0, 0, 0);
       printExc ("machine check \n", 0, 0, 0, 0, 0);
	
       GT_REG_READ (MAIN_INTERRUPT_CAUSE_LOW, &regVal);
#if 0
	printExc("Main Interrupt Cause Low  : 0x%08x\n",regVal,0,0,0,0); 
#endif

	/* Handling SRAM Error */
	if((regVal & (1 << 13)) == 0x2000 )
	{
		sramErrorReport();
		
	}  /* Handling SRAM Error */


	/* Handling SDRAM ECC Error */

	if(regVal & (1 << 17))
	{
		sdramEccErrorReport();
		
	} /* Handling SDRAM Error */


	 /* Handling PCI Errors */
	{
		
	      pciErr = pciErrReport(0);    /* PCI Bus 0 */
	      pciErr = pciErrReport(1);	 /* PCI Bus 1 */
 		
		/* clear pending PCI error interrupts */
	      GT_REG_WRITE (PCI_0_ERROR_CAUSE, 0);
   	      GT_REG_WRITE (PCI_1_ERROR_CAUSE, 0);

  	      /* 
	       * Clear PCI Bus 0 and 1  events 
	       */
          GT_REG_WRITE(MAIN_INTERRUPT_CAUSE_LOW, ~(1<<12));
          GT_REG_READ (MAIN_INTERRUPT_CAUSE_LOW, &regVal);
			
          GT_REG_WRITE(MAIN_INTERRUPT_CAUSE_LOW, ~(1<<16));
          GT_REG_READ (MAIN_INTERRUPT_CAUSE_LOW, &regVal);

	     /* 
	      * Clear all the Error bits forcefully 
	      * in PCI Command and Status Register 
	      */
              pciConfigInLong(0,0,0,PCI_CFG_COMMAND,&regVal);
              regVal = (regVal & 0x06FFFFFF);  /* Extract only RW Bits */
              regVal |= 0xF9000000;	 /* Force the RWC bits to Zeros by writing 1's */
              pciConfigOutLong(0,0,0,PCI_CFG_COMMAND,regVal);

	 }
	
	/* Re-enable data cache */
	if(cacheWasEnabled == TRUE)
	{
		cacheEnable (DATA_CACHE);
	}
	
	
	printExc ("\nCALLING generic    :", 0, 0, 0, 0, 0);
        /* Call generic handler */
    excExcHandle (pEsf);

}  /* End of sysMachChkExcpHand */


LOCAL void sramErrorReport(void)
{

		UINT32	regVal2, regVal3;

		printExc("\n\nSRAM ERROR :\n",0,0,0,0,0);
 	  		
		/*
		SRAM Error Address 
		Offset: 0x390
                If multiple errors occur, only the first error is latched. New error 
		report latching is only enabled after the SRAM_ADDR Error Address 
		register is read. 
		*/

		/* Latched Address upon SRAM parity error detection. */
		
		GT_REG_READ(SRAM_ERROR_ADDR, &regVal2);
		GT_REG_READ(SRAM_ERROR_ADDR_HIGH, &regVal3);
		regVal3 = regVal3 & 0xF; 

		printExc("SRAM Error Address    : 0x%08x.%08x\n",  regVal3,regVal2, 0, 0, 0);

		/*
		NOTE: SRAM  is 144 bit wide { 128 bit	data + 16 bit parity 
		(1 parity bit per byte)}.i.e SRAM is 16 byte Wide. 
		*/			     
		/*
		SRAM Error Data (Low)
		Offset: 0x398
		*/
		GT_REG_READ(SRAM_ERROR_DATA_LOW, &regVal2);
		printExc("SRAM Error Data Low   : 0x%08x\n",regVal2,0,0,0,0);

		/*SRAM Error Data (High)
		Offset: 0x3a0
		*/
		GT_REG_READ(SRAM_ERROR_DATA_HIGH, &regVal2);
		printExc("SRAM Error Data High  : 0x%08x\n",regVal2,0,0,0,0);

		/*
		SRAM Error Parity
		Offset: 0x3a8
		*/
		GT_REG_READ(SRAM_ERROR_DATA_PARITY, &regVal2);
		printExc("SRAM Error Parity     : 0x%08x\n",regVal2,0,0,0,0);


        /*
                SRAM Error Cause Register offset 0x388
		Bit[00] indicates parity error on data bits   [007:000].  byte 1
		Bit[01] indicates parity error on data bits   [015:008],  byte 2 
		Bit[02] indicates parity error on data bits   [023:016],  byte 3
		Bit[03] indicates parity error on data bits   [031:024],  byte 4
		Bit[04] indicates parity error on data bits   [039:032],  byte 5
		Bit[05] indicates parity error on data bits   [047:040],  byte 6
		Bit[06] indicates parity error on data bits   [055:048],  byte 7 
		Bit[07] indicates parity error on data bits   [063:056],  byte 8
		Bit[08] indicates parity error on data bits   [071:064],  byte 9
		Bit[09] indicates parity error on data bits   [079:072],  byte 10
		Bit[10] indicates parity error on data bits   [087:080],  byte 11
		Bit[11] indicates parity error on data bits   [095:088],  byte 12
		Bit[12] indicates parity error on data bits   [103:096],  byte 13
		Bit[13] indicates parity error on data bits   [111:102],  byte 14
		Bit[14] indicates parity error on data bits   [120:110],  byte 15
		Bit[15] indicates parity error on data bits   [127:119],  byte 16
		*/
		
		GT_REG_READ(SRAM_ERROR_CAUSE, &regVal2);

		printExc("SRAM Error Cause      : 0x%08x\n",regVal2,0,0,0,0);
		
	
		/* Clear any SRAM interrupt */
#if 0		
		printExc("Clearing SRAM Error Cause Register Bit\n",0,0,0,0,0);
#endif 
		GT_REG_WRITE(SRAM_ERROR_CAUSE, 0x0); /* Clear SRAM error cause register */

		GT_REG_WRITE(SRAM_ERROR_ADDR, 0x0);
		GT_REG_WRITE(SRAM_ERROR_ADDR_HIGH, 0x0);
		GT_REG_WRITE(SRAM_ERROR_DATA_LOW, 0x0);
		GT_REG_WRITE(SRAM_ERROR_DATA_HIGH, 0x0);
		GT_REG_WRITE(SRAM_ERROR_DATA_PARITY, 0x0);
		
		GT_REG_WRITE (MAIN_INTERRUPT_CAUSE_LOW, ~(1<<13));
                GT_REG_READ (MAIN_INTERRUPT_CAUSE_LOW, &regVal2);
#if 0		
		printExc("Main Interrupt Cause Low after clearing : 0x%08x\n",regVal2,0,0,0,0); 
#endif
			

}  /* sramErrorReport */

	

LOCAL void sdramEccErrorReport(void)
{


		UINT32	regVal, regVal2;
		
      	 /*
		In case of ECC error detection, the MV64360/1/2 asserts an 
		interrupt (if not masked), and latches the:
		- Address in the ECC Error Address register.
		- 64-bit read data in the ECC Error Data register.
		- Read ECC byte in the SDRAM ECC register.
		- Calculated ECC byte in the Calculated ECC register.

		*/
		printExc("\n\nSDRAM ECC ERROR :\n",0,0,0,0,0);

		/*
		ECC checking and generation requires a 72-bit wide 
		DRAM to store the ECC information, 64-bits for data 
		and eight bits for ECC.
		*/
	
		GT_REG_READ(SDRAM_ERROR_ADDR, &regVal);

		printExc("%s bit error on SDRAM bank %d, address 0x%08x\n",
	       (int)(((regVal & 0x1) == 0) ? "single":"multiple"),
	       (regVal >> 1) & 3,
	       (regVal & 0xfffffff8),0,0);
 
              GT_REG_READ(SDRAM_ECC_ERROR_COUNTER, &regVal);
              printExc("Number of single bit errors : %d\n",regVal,0,0,0,0);
 
            GT_REG_READ(SDRAM_RECEIVED_ECC, &regVal);
	     printExc("ECC code read from SDRAM    : 0x%08x\n",regVal,0,0,0,0);
 
	     GT_REG_READ(SDRAM_CALCULATED_ECC, &regVal);
	     printExc("ECC code calculated         : 0x%08x\n",regVal,0,0,0,0);
 
	     GT_REG_READ(SDRAM_ERROR_DATA_LOW, &regVal);
	     GT_REG_READ(SDRAM_ERROR_DATA_HIGH, &regVal2);
	     printExc("Error data                  : 0x%08x.%08x\n",regVal2,regVal,0,0,0);


 	     /* Clear SDRAM Error Address Registers */
            GT_REG_WRITE(SDRAM_ERROR_ADDR, 0x0);

	    /* 
	     * Clear ECC interrupt.
	     */
           GT_REG_WRITE(MAIN_INTERRUPT_CAUSE_LOW, ~(1<<17));
           GT_REG_READ (MAIN_INTERRUPT_CAUSE_LOW, &regVal);

} /* sdramEccErrorReport */



/* ******************************************************************************
* pciErrReport - Report PCI errors
*
* RETURNS
* TRUE if an PCI Error occured, FALSE if not 
* 
*/
LOCAL BOOL pciErrReport
    (
    int		pciSeg	/* PCI segment (0 or 1) */
    )
    {
    UINT32	tmp, tmp2;
    int		regOff = 0;
    
    if (pciSeg == 1)
	regOff = 0x80;

	
    GT_REG_READ (PCI_0_ERROR_CAUSE + regOff, &tmp);

#if 0	
    printExc ("\n PCI_%d Error: Status=0x%08x\n", pciSeg, tmp, 0, 0, 0); /* Delete it later on */
#endif

    if ((tmp & PCI_ERR_MASK) == 0)
	return FALSE;
    printExc ("\nPCI_%d DEVICE ERROR :\n", pciSeg, 2, 3, 4, 5);
    printExc ("PCI_%d Error: Status=0x%08x\n", pciSeg, tmp, 0, 0, 0);

    if (tmp & (1<<1))
	printExc ("Slave: Bad write data parity\n", tmp, 0, 0, 0, 0);
    if (tmp & (1<<2))
	printExc ("Slave: Bad read data parity\n", tmp, 0, 0, 0, 0);
    if (tmp & (1<<5))
	printExc ("Master: Bad write data parity\n", tmp, 0, 0, 0, 0);
    if (tmp & (1<<6))
	printExc ("Master: Bad read data parity\n", tmp, 0, 0, 0, 0);
    if (tmp & (1<<8))
	printExc ("Master abort\n", tmp, 0, 0, 0, 0);
    if (tmp & (1<<9))
	printExc ("Target abort\n", tmp, 0, 0, 0, 0);
    if (tmp & (1<<11))
	printExc ("Retry counter limit reached\n", tmp, 0, 0, 0, 0);

    GT_REG_READ (PCI_0_ERROR_COMMAND + regOff, &tmp);
	printExc ("PCI Cmd=%x (%s)  ByteEnable=%x  Par=%d\n", 
		tmp & 0xf, 
		(int)pciCmds[tmp & 0xf],
		(tmp & 0xff00) >> 8,
		(tmp >> 16) & 1,
		0);

    GT_REG_READ (PCI_0_ERROR_ADDR_HIGH + regOff, &tmp2);
    GT_REG_READ (PCI_0_ERROR_ADDR_LOW + regOff, &tmp);
    printExc ("Error Address High: 0x%08x\n", tmp2, 0, 0, 0, 0);
    printExc ("Error Address Low : 0x%08x\n", tmp, 0, 0, 0, 0);

          
    return TRUE;
    }


/* ******************************************************************************
* sysVxMemProbeHook - Hook for vxmemProbe
*
* We can't use original vxMemProbe since we need to clear error cause registers.
* Otherwise subsequent errors won't cause an exception ...
*
* This routine catches calls to vxMemProbe and clears the errors if necessary.
*
* RETURNS
* n/a
*
* NOMANUAL
* */
LOCAL STATUS sysVxMemProbeHook 
    (
    char *  adrs,    /* address to be probed          */
    int     mode,    /* VX_READ or VX_WRITE           */
    int     length,  /* 1, 2, or 4                    */
    char *  pVal     /* where to return value,        */
		     /* or ptr to value to be written */
    )
    {
    UINT32	cause;
    STATUS	rc = OK;
    int		l;
    
    l = intLock();
    
    /* call original routine to check for access error */
    _func_vxMemProbeHook = NULL;
    rc = vxMemProbe (adrs, mode, length, pVal);
    _func_vxMemProbeHook = (FUNCPTR)sysVxMemProbeHook;

    /* reset logged PCI errors */

    /*
     * A bug in vxMemProbe he does not return error if there is a write to a 
     * mapped but no hardware access, hence donot look at rc status
     * but forcfully clear if PCI_ERR is set. 
     */
    if (rc == ERROR)
    {
    
	     GT_REG_READ (PCI_0_ERROR_CAUSE, &cause);

	     if (cause & PCI_ERR_MASK)
	     {
			GT_REG_WRITE (PCI_0_ERROR_CAUSE, 0);
			rc=ERROR;
	     }

	     GT_REG_READ (PCI_1_ERROR_CAUSE, &cause);

	     if (cause & PCI_ERR_MASK)
	     {
	   		GT_REG_WRITE (PCI_1_ERROR_CAUSE, 0);
			rc=ERROR;
	     }

	/* We should clear the errors logged in all PCI devices, but
	 * this would make the implementation quite ineffective ... 
	 */
		
    }
    
    intUnlock (l);
    
    return rc;

    
    }  /* end of sysVxMemProbeHook */
