/* sysMv64360Init.c - Statically initialize the Mv64360 registers */

/* Copyright 2003 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01a,02apr03,simon  Ported. from ver 01a, mcpm905/sysMv64360Init.c.
*/

/*
DESCRIPTION
This module statically initializes the Mv64360 registers.
*/

/* includes */

#include "mv64360.h"

/* defines */

/* For internal register initialization from table */

#define INTERNAL_REG_OFFSET(x)	*(x+1)
#define INTERNAL_REG_VALUE(x)	*(x+2)

/* For PCI config register updating from table */

#define CFG_BUS(x)	   0		/* Always access from bus 0 */
#define CFG_DEV(x)	   0		/* Device number is always 0 */
#define CFG_FCN(x)    *(x+1)		/* Function number */
#define CFG_OFFSET(x) *(x+2)		/* Device number */
#define CFG_SIZE(x)        4		/* Size in bytes is always 4 */
#define CFG_VALUE(x)  *(x+3)		/* Value to program */

/* globals */

/* locals */

/*
 * This table contains many of the the static initialization values for the 
 * MV64360 system controller chip.  The registers initialized here 
 * do not directly belong to a piece of functionality that has 
 * a supporting ".c" file - e.g. interrupt controller initialization
 * has another supporting ".c" file so the related register initialization
 * does NOT appear in this table.
 * 
 */

LOCAL UINT32 mv64360InitTbl [] =

    {

    /*
     * Timer/counter 0 is used as the MV64360 AuxClock timer.
     */

    YES_INIT_MV64360_REG (TMR_CNTR0, 0)

    YES_INIT_MV64360_REG (TMR_CNTR1, 0)

    YES_INIT_MV64360_REG (TMR_CNTR2, 0)

    YES_INIT_MV64360_REG (TMR_CNTR3, 0)

    /*
     * Timer/counter Control Register
     * Bit 00 = 0  Disable timer/counter 0.
     *     01 = 0  Counter mode for timer/counter 0.
     *     02 = 0  No external trigger for timer/counter 0.
     *     03 = 0  TCTcnt asserted for one Tclk cycle for timer/counter 0.
     *  07:04 = 0  Reserved and set to 0.
     *     08 = 0  Disable timer/counter 1.
     *     09 = 0  Counter mode for timer/counter 1.
     *     10 = 0  No external trigger for timer/counter 1.
     *     11 = 0  TCTcnt asserted for one Tclk cycle for timer/counter 1.
     *  15:12 = 0  Reserved and set to 0.
     *     16 = 0  Disable timer/counter 2.
     *     17 = 0  Counter mode for timer/counter 2.
     *     18 = 0  No external trigger for timer/counter 2.
     *     19 = 0  TCTcnt asserted for one Tclk cycle for timer/counter 2.
     *  23:20 = 0  Reserved and set to 0.
     *     24 = 0  Disable timer/counter 3.
     *     25 = 0  Counter mode for timer/counter 3.
     *     26 = 0  No external trigger for timer/counter 3.
     *     27 = 0  TCTcnt asserted for one Tclk cycle for timer/counter 3.
     *  31:28 = 0  Reserved and set to 0.
     */

    YES_INIT_MV64360_REG (TMR_CNTR_CTRL, 0)

    /*
     * Timer/counter Interrupt Cause Register
     * Bit 00 = 0  Timer/counter 0 terminal count cleared.
     *     01 = 0  Timer/counter 1 terminal count cleared.
     *     02 = 0  Timer/counter 2 terminal count cleared.
     *     03 = 0  Timer/counter 3 terminal count cleared.
     *  30:04 = 0  Reserved and set to 0.
     *     31 = 0  Summary of all cause bits, read only.
     */

    YES_INIT_MV64360_REG (TMR_CNTR_INT_CAUSE, 0)

    /*
     * Timer/counter Interrupt Mask Register
     * Bit 00 = 0  Timer/counter 0 interrupt disabled.
     *     01 = 0  Timer/counter 1 interrupt disabled.
     *     02 = 0  Timer/counter 2 interrupt disabled.
     *     03 = 0  Timer/counter 3 interrupt disabled.
     *  31:04 = 0  Read only.
     */

    YES_INIT_MV64360_REG (TMR_CNTR_INT_MASK, 0)

    /* MPP/GPP init */
    YES_INIT_MV64360_REG ( MPP_CTRL0, (MPP_PIN0_SEL  <<	0)  | \
				      (MPP_PIN1_SEL  <<	4)  | \
				      (MPP_PIN2_SEL  <<	8)  | \
				      (MPP_PIN3_SEL  <<	12) | \
				      (MPP_PIN4_SEL  <<	16) | \
				      (MPP_PIN5_SEL  <<	20) | \
				      (MPP_PIN6_SEL  <<	24) | \
				      (MPP_PIN7_SEL  <<	28) )

    YES_INIT_MV64360_REG ( MPP_CTRL1, (MPP_PIN8_SEL  <<	0)  | \
				      (MPP_PIN9_SEL  <<	4)  | \
				      (MPP_PIN10_SEL <<	8)  | \
				      (MPP_PIN11_SEL <<	12) | \
				      (MPP_PIN12_SEL <<	16) | \
				      (MPP_PIN13_SEL <<	20) | \
				      (MPP_PIN14_SEL <<	24) | \
				      (MPP_PIN15_SEL <<	28) )

    YES_INIT_MV64360_REG ( MPP_CTRL2, (MPP_PIN16_SEL <<	0)  | \
				      (MPP_PIN17_SEL <<	4)  | \
				      (MPP_PIN18_SEL <<	8)  | \
				      (MPP_PIN19_SEL <<	12) | \
				      (MPP_PIN20_SEL <<	16) | \
				      (MPP_PIN21_SEL <<	20) | \
				      (MPP_PIN22_SEL <<	24) | \
				      (MPP_PIN23_SEL <<	28) )

    YES_INIT_MV64360_REG ( MPP_CTRL3, (MPP_PIN24_SEL <<	0)  | \
				      (MPP_PIN25_SEL <<	4)  | \
				      (MPP_PIN26_SEL <<	8)  | \
				      (MPP_PIN27_SEL <<	12) | \
				      (MPP_PIN28_SEL <<	16) | \
				      (MPP_PIN29_SEL <<	20) | \
				      (MPP_PIN30_SEL <<	24) | \
				      (MPP_PIN31_SEL <<	28) )

    YES_INIT_MV64360_REG ( GPP_IO_CTRL, (GPP_IO0 << 0)   | (GPP_IO1  << 1)  | \
					(GPP_IO2 << 2)   | (GPP_IO3  << 3)  | \
					(GPP_IO4 << 4)   | (GPP_IO5  << 5)  | \
					(GPP_IO6  << 6)  | (GPP_IO7  << 7)  | \
					(GPP_IO8  << 8)  | (GPP_IO9  << 9)  | \
					(GPP_IO10 << 10) | (GPP_IO11 << 11) | \
					(GPP_IO12 << 12) | (GPP_IO13 << 13) | \
					(GPP_IO14 << 14) | (GPP_IO15 << 15) | \
					(GPP_IO16 << 16) | (GPP_IO17 << 17) | \
					(GPP_IO18 << 18) | (GPP_IO19 << 19) | \
					(GPP_IO20 << 20) | (GPP_IO21 << 21) | \
					(GPP_IO22 << 22) | (GPP_IO23 << 23) | \
					(GPP_IO24 << 24) | (GPP_IO25 << 25) | \
					(GPP_IO26 << 26) | (GPP_IO27 << 27) | \
					(GPP_IO28 << 28) | (GPP_IO29 << 29) | \
					(GPP_IO30 << 30) | (GPP_IO31 << 31) )
					
    YES_INIT_MV64360_REG ( GPP_LEVEL_CTRL, (GPP_LEVEL0 << 0) | \
					   (GPP_LEVEL1 << 1) | \
					   (GPP_LEVEL2 << 2) | \
					   (GPP_LEVEL3 << 3) | \
					   (GPP_LEVEL4 << 4) | \
					   (GPP_LEVEL5 << 5) | \
					   (GPP_LEVEL6 << 6) | \
					   (GPP_LEVEL7 << 7) | \
					   (GPP_LEVEL8 << 8) | \
					   (GPP_LEVEL9 << 9) | \
					   (GPP_LEVEL10 << 10) | \
					   (GPP_LEVEL11 << 11) | \
					   (GPP_LEVEL12 << 12) | \
					   (GPP_LEVEL13 << 13) | \
					   (GPP_LEVEL14 << 14) | \
					   (GPP_LEVEL15 << 15) | \
					   (GPP_LEVEL16 << 16) | \
					   (GPP_LEVEL17 << 17) | \
					   (GPP_LEVEL18 << 18) | \
					   (GPP_LEVEL19 << 19) | \
					   (GPP_LEVEL20 << 20) | \
					   (GPP_LEVEL21 << 21) | \
					   (GPP_LEVEL22 << 22) | \
					   (GPP_LEVEL23 << 23) | \
					   (GPP_LEVEL24 << 24) | \
					   (GPP_LEVEL25 << 25) | \
					   (GPP_LEVEL26 << 26) | \
					   (GPP_LEVEL27 << 27) | \
					   (GPP_LEVEL28 << 28) | \
					   (GPP_LEVEL29 << 29) | \
					   (GPP_LEVEL30 << 30) | \
					   (GPP_LEVEL31 << 31) )

    NOT_INIT_MV64360_REG ( GPP_VAL			  , readOnly )
    YES_INIT_MV64360_REG ( GPP_INTERRUPT_CAUSE		  , 0 )
    YES_INIT_MV64360_REG ( GPP_INTERRUPT_MASK0		  , MASK_ALL )
    YES_INIT_MV64360_REG ( GPP_INTERRUPT_MASK1		  , MASK_ALL )
    NOT_INIT_MV64360_REG ( GPP_VAL_SET			  , readOnly )
    NOT_INIT_MV64360_REG ( GPP_VAL_CLEAR		  , readOnly )
    
    END_INIT_MV64360_REG
    };

/* forward declarations */

void sysMv64360RegInit (UINT32 *);

LOCAL void  sysMv64360Init (void);

/******************************************************************************
*
* sysMv64360RegInit - Initialize MV64360 registers
*
* This function statically initializes specific MV64360 registers specified
* by a table pointed at by the input parameter.
*
* RETURNS: NA
*/


void sysMv64360RegInit 
(
  UINT32 *p		/* Pointer to initialization table */
)
{
  char txt[100];

/*(volatile UINT8 *)0xf1100005 = 3;*/
/*gorit*/

#ifdef MYDEBUG
  sprintf(txt,"PCI0_MSTR_ZERO_LOCAL=0x%08x, PCI0_MSTR_ZERO_SIZE=0x%08x\r\n ",
    (PCI0_MSTR_ZERO_LOCAL), (PCI0_MSTR_ZERO_SIZE));
  sysDebugMsg(txt,CONTINUE_EXECUTION);
#endif

  /*  
  sprintf(txt,"sysMv64360RegInit reached, p=0x%08x\r\n ",(int)p);
  sysDebugMsg(txt,CONTINUE_EXECUTION);
  */
  /*
  sprintf(txt,"p-> 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\r\n ",
    p[0],p[1],p[2],p[3],p[4],p[5]);
  sysDebugMsg(txt,CONTINUE_EXECUTION);
  */
  /*p=0x008812bc*/

  while (*p != END_INIT_MV64360_REG)
  {
	switch (*p)
	{
	  case MV64360_INTERNAL:
		/*
  sprintf(txt,"1\r\n ");
  sysDebugMsg(txt,CONTINUE_EXECUTION);
		*/

	    MV64360_WRITE32_PUSH(MV64360_REG_BASE, INTERNAL_REG_OFFSET(p), 
				      INTERNAL_REG_VALUE(p));

		p += MV64360_INTERNAL_INIT_SIZE;
		/*
  sprintf(txt,"2\r\n ");
  sysDebugMsg(txt,CONTINUE_EXECUTION);
		*/
		break;

	  case MV64360_PCICNFG:
		/*
  sprintf(txt,"3\r\n ");
  sysDebugMsg(txt,CONTINUE_EXECUTION);
		*/
		sysPciConfigWrite (CFG_BUS(p), CFG_DEV(p), CFG_FCN(p), 
				   CFG_OFFSET(p), CFG_SIZE(p), CFG_VALUE(p));

		p += MV64360_PCICNFG_INIT_SIZE;
		break;

      default:
		/*
  sprintf(txt,"9\r\n ");
  sysDebugMsg(txt,CONTINUE_EXECUTION);
		*/
		break;	/* Should never get here, if so we're pooched!  */
	}	
  }
/*ne gorit*/
  /*
  sprintf(txt,"sysMv64360RegInit done\r\n ");
  sysDebugMsg(txt,CONTINUE_EXECUTION);
  */
}

/******************************************************************************
*
* sysMv64360RegInit - Initialize MV64360 registers
*
* This function initializes the MV64360 registers which can be statically
* initialized.
*
* RETURNS: NA
*/

LOCAL void sysMv64360Init (void)
    {
    UINT32 	*p = mv64360InitTbl;
    sysMv64360RegInit (p);
    }
