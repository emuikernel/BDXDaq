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
* vxGppIntCtrl.h - Header File for : GPP Interrupt Controller driver
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

#ifndef __INCvxGppIntCtrlh
#define __INCvxGppIntCtrlh

/* includes */
#include "gtIntControl.h" 


/* defines  */
typedef enum gpp_cause{
    /* GPP facility Interrupt cause register bit description */
    GPP_CAUSE_START = -1   	,  	/* Cause low boundry 	   	*/
	GPP_PIN0_NOT_INT  ,  	/* this pin is not configured as an interrupt */
	GPP_PIN1_NOT_INT  ,  	/* this pin is not configured as an interrupt */
	GPP_PIN2_NOT_INT  ,  	/* this pin is not configured as an interrupt */
	GPP_PIN3_NOT_INT  ,  	/* this pin is not configured as an interrupt */
	GPP_PIN4_NOT_INT  ,  	/* this pin is not configured as an interrupt */ 
	GPP_PIN5_NOT_INT  ,  	/* this pin is not configured as an interrupt */
	GPP_PIN6_TEST1  ,  	/* this pin is not configured as an interrupt */
	GPP_PIN7_TEST2  ,  	/* this pin is not configured as an interrupt */
	GPP_PIN8_NOT_INT  ,  	/* this pin is not configured as an interrupt */
	GPP_PIN9_NOT_INT  ,  	/* this pin is not configured as an interrupt */
	GPP_PIN10_NOT_INT ,  	/* this pin is not configured as an interrupt */
	GPP_PIN11_NOT_INT ,  	/* this pin is not configured as an interrupt */
	GPP_PIN12_NOT_INT ,  	/* this pin is not configured as an interrupt */
	GPP_PIN13_NOT_INT ,  	/* this pin is not configured as an interrupt */
	GPP_PIN14_NOT_INT ,  	/* this pin is not configured as an interrupt */
	GPP_PIN15_NOT_INT ,  	/* this pin is not configured as an interrupt */
	GPP_PIN16_NOT_INT ,  	/* this pin is not configured as an interrupt */
	GPP_PIN17_NOT_INT ,  	/* this pin is not configured as an interrupt */
	GPP_PIN18_WD_NMI ,  	/* this pin is not configured as an interrupt */
	GPP_PIN19_NOT_INT ,  	/* this pin is not configured as an interrupt */
	GPP_PIN20_NOT_INT ,  	/* this pin is not configured as an interrupt */
	GPP_PIN21_RS232_A ,  	/* pin connected to interrupt of RS232 channel A*/
	GPP_PIN22_RS232_B ,  	/* pin connected to interrupt of RS232 channel B*/
	GPP_PIN23_NOT_INT ,  	/* this pin is not configured as an interrupt */
	GPP_PIN24_WD_NMI  ,  	/* pin connected to interrupt of watchdog NMI */
	GPP_PIN25_NOT_INT ,  	/* this pin is not configured as an interrupt */
	GPP_PIN26_RTC  	  ,  	/* pin connected to interrupt of Real Time Clock */
	GPP_PIN27_PCI_0	  ,  	/* pin connected to interrupt of PCI 0 */
	GPP_PIN28_NOT_INT ,  	/* this pin is not configured as an interrupt */
	GPP_PIN29_PCI_1	  ,  	/* pin connected to interrupt of PCI 1 */
	GPP_PIN30_NOT_INT ,  	/* this pin is not configured as an interrupt */
	GPP_PIN31_NOT_INT ,  	/* this pin is not configured as an interrupt */
    GPP_CAUSE_END		 	/* Cause High boundry 	   	*/
    }GPP_CAUSE;

/* The following definition describes a specific cause bit in Hex format. */

#define GPP_PIN21_RS232_A_BIT   GT_WORD_SWAP(1<<GPP_PIN21_RS232_A)        
#define GPP_PIN22_RS232_B_BIT   GT_WORD_SWAP(1<<GPP_PIN22_RS232_B)       
#define GPP_PIN24_WD_NMI_BIT    GT_WORD_SWAP(1<<GPP_PIN24_WD_NMI)
#define GPP_PIN26_RTC_BIT	GT_WORD_SWAP(1<<GPP_PIN26_RTC)       
#define GPP_PIN27_PCI_0_BIT    	GT_WORD_SWAP(1<<GPP_PIN27_PCI_0)       
#define GPP_PIN29_PCI_1_BIT  	GT_WORD_SWAP(1<<GPP_PIN29_PCI_1)        
#define GPP_PIN6_WD_NMI_BIT     GT_WORD_SWAP(1<<GPP_PIN6_WD_NMI)


#define GPP_INT10		GT_WORD_SWAP(1 << 10)
#define GPP_INT11		GT_WORD_SWAP(1 << 11)
#define GPP_INT12		GT_WORD_SWAP(1 << 12)
#define GPP_INT13		GT_WORD_SWAP(1 << 13)
#define GPP_INT24		GT_WORD_SWAP(1 << 24)


/* typedefs */

#define INT_RESERVED_BIT    0
#define INT_DEFAULT_PRIO    5


#define GPP_CAUSE_REG     REG_CONTENT(GPP_INTERRUPT_CAUSE)  
#define GPP_MASK_REG(cpu) REG_CONTENT(GPP_INTERRUPT_MASK(cpu))  
#define GPP_VALUE_REG     REG_CONTENT(GPP_VALUE)  


/* typedefs */


/* vxGppIntCtrl.h API list */		
void   frcGppIntCtrlInit (void); 
STATUS frcGppIntConnect  (GPP_CAUSE cause, VOIDFUNCPTR routine,
                         int parameter, int prio);
STATUS frcGppIntEnable   (GPP_CAUSE cause);
STATUS frcGppIntDisable  (GPP_CAUSE cause);
STATUS frcGppCPU1IntEnable(GPP_CAUSE cause);
STATUS frcGppCPU1IntDisable(GPP_CAUSE cause);

#endif /* __INCvxGppIntCtrlh */


