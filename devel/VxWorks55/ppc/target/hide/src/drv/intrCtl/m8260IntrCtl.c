/* m8260Int.c - Motorola Voyager MPC 8260 interrupt controller driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01p,28mar02,dtr  Fix typo.
01o,28mar02,jrs  Correct loop bounds check for attaching ISR's to int no's.
01n,12jan02,g_h  Remove calling to intLock() in m8260IntrDeMux()
01m,07dec01,rcs  masked vxImmrGet() value with IMMR_ISB_MASK SPR# 72027 
01l,22oct01,g_h  Change the m8260IntrDeMux function to check for interrupt 0 
		 as indecation for no interrupt. As long as reading the SIVEC 
		 register defrent from 0 we have panding interrupt and we need
		 to server it, this fix SPR 71119.
01k,22oct01,g_h  Clean compilation warning from m8260IntDisable function, and 
		 change the declaration of intVec in m8260IntrDeMux to VINT32
		 from UINT32 to prevent optimization problem.
01j,16sep99,ms_  m8260IntrCtl.h comes from h/drv/intrCtl
01i,16sep99,ms_  rename m8260Int.h to m8260IntrCtl.h
01h,15jul99,ms_  make compliant with our coding standards
01g,13may99,ms_  minor fixes
01f,17apr99,ms_  final cleanup for EAR
01e,15apr99,ms_  disable only lower priority interrupts while processing
                 interrupt
01d,13apr99,ms_  redefine IVEC_TO_INUM and INUM_TO_IVEC to use the numbers as
                 shown in the MPC8260 user's manaul
01c,12apr99,ms_  remove any sio specific items; that is handled in the sio
                 driver
01b,29mar99,ms_  add ability to mask out lower priority interrupts when
                 processing an interrupt
01a,22feb99,ms_  adapted from ppc8260Intr.c version 01f
*/

/*
DESCRIPTION
The PowerPC 8260 CPU is divided in three units: PowerPC core, System
Interface Unit (SIU) and Communication Processor Unit (CPM). The PowerPC
core accepts only one external interrupt exception (vector 0x500). The SIU
provides an interrupt controller which provides 55 interrupt sources
The Interrupt controller is connected to the PowerPC core external 
interrupt. This library provides the routines to manage this interrupt 
controllers.

m8260IntrInit() connects the default demultiplexer, m8260IntrDeMux(), to the 
external interrupt vector and initializes a table containing a function pointer
and an associated parameter for each interrupt source. m8260IntrInit() is called
by sysHwInit() from sysLib.c.

The default demultimplexer, m8260IntrDeMux() detects which peripheral or 
controller has raised an interrupt and calls the associated routine with its 
parameter. 

INCLUDE FILES: m8260IntrCtl.h
*/

/* includes */

#include "intLib.h"
#include "iv.h"
#include "drv/intrCtl/m8260IntrCtl.h"

#ifdef  INCLUDE_WINDVIEW
#include "private/funcBindP.h"
#include "private/eventP.h"
#endif /* INCLUDE_WINDVIEW */

/* local */

LOCAL INTR_HANDLER	intrVecTable[IVEC_MAX+1];	/*  Intr vector table */

/* SIU Interrupt Mask structure; a value for the high and low parts */
typedef struct siu_int_mask
    {
    UINT32	simr_h;
    UINT32	simr_l;
    } SIU_INT_MASK;

/* 
 * for a give inum, return a pair of masks, one for SIMR_H and one for SIMR_L,
 * which will enable only higher priority interrupts.
 */

LOCAL SIU_INT_MASK enableHighPriInts[] =
    {
    {0x00000000, 0x00000000},	/* interrupt 0 error - enable nothing */
    {0x003F7C06, 0xECF02658},		/* I2C */
    {0x0FFF7F06, 0xECF0A7DE},		/* SPI */
    {0x003F7C06, 0xECF00658},		/* RISC Timer Table */
    {0x3FFF7F06, 0xECF0E7DE},		/* SMC1 */
    {0x3FFF7F06, 0xECF0F7DE},		/* SMC2 */
    {0x00077C06, 0xECF00050},		/* IDMA1 */
    {0x001F7C06, 0xECF00450},		/* IDMA2 */
    {0x00FF7E06, 0xECF0A658},		/* IDMA3 */
    {0x0FFF7F06, 0xECF0A75E},		/* IDMA4 */
    {0x00077C06, 0xECF00010},		/* SDMA Bus Error */
    {0xFFFFFFFF, 0xFFFFFFFF},	/* interrupt 11 reserved - enable everything */
    {0x00017C06, 0xECF00000},		/* Timer 1 */
    {0x001F7C06, 0xECF00650},		/* Timer 2 */
    {0x00FF7F06, 0xECF0A758},		/* Timer 3 */
    {0x07FF7F06, 0xECF0A75C},		/* Timer 4 */
    {0x00000000, 0x00000000},		/* TMCNT */
    {0x00000004, 0x00000000},		/* PIT */
    {0xFFFFFFFF, 0xFFFFFFFF},	/* interrupt 18 reserved - enable everything */
    {0x00000006, 0x00000000},		/* IRQ1 XXXX */
    {0x00004006, 0xEC000000},		/* IRQ2 */
    {0x00006006, 0xEC000000},		/* IRQ3 */
    {0x00007006, 0xEC000000},		/* IRQ4 */
    {0x00007806, 0xEC000000},		/* IRQ5 */
    {0x00FF7C06, 0xECF0A658},		/* IRQ6 */
    {0x00FF7E06, 0xECF0A758},		/* IRQ7 */
    {0xFFFFFFFF, 0xFFFFFFFF},	/* interrupt 26 reserved - enable everything */
    {0x00004006, 0x00000000},		/* FCC1 */
    {0x00004006, 0x80000000},		/* FCC2 */
    {0x00004006, 0xC0000000},		/* FCC3 */
    {0xFFFFFFFF, 0xFFFFFFFF},	/* interrupt 30 reserved - enable everything */
    {0x00004006, 0xE0000000},		/* MCC1 */
    {0x00004006, 0xE8000000},		/* MCC2 */
    {0xFFFFFFFF, 0xFFFFFFFF},	/* interrupt 33 reserved - enable everything */
    {0xFFFFFFFF, 0xFFFFFFFF},	/* interrupt 34 reserved - enable everything */
    {0x00007C06, 0xEC000000},		/* SCC1 */
    {0x00007C06, 0xEC800000},		/* SCC2 */
    {0x00007C06, 0xECC00000},		/* SCC3 */
    {0x00007C06, 0xECE00000},		/* SCC4 */
    {0xFFFFFFFF, 0xFFFFFFFF},	/* interrupt 39 reserved - enable everything */
    {0x00007C06, 0xECF00000},		/* PC15 */
    {0x00017C06, 0xECF00010},		/* PC14 */
    {0x00037C06, 0xECF00010},		/* PC13 */
    {0x00077C06, 0xECF00450},		/* PC12 */
    {0x000F7C06, 0xECF00450},		/* PC11 */
    {0x001F7C06, 0xECF00658},		/* PC10 */
    {0x003F7C06, 0xECF0A658},		/* PC9 */
    {0x007F7C06, 0xECF0A658},		/* PC8 */
    {0x00FF7F06, 0xECF0A75C},		/* PC7 */
    {0x01FF7F06, 0xECF0A75C},		/* PC6 */
    {0x03FF7F06, 0xECF0A75C},		/* PC5 */
    {0x07FF7F06, 0xECF0A75E},		/* PC4 */
    {0x0FFF7F06, 0xECF0E7DE},		/* PC3 */
    {0x1FFF7F06, 0xECF0E7DE},		/* PC2 */
    {0x3FFF7F06, 0xECF0FFDE},		/* PC1 */
    {0x7FFF7F06, 0xECF0FFDE},		/* PC0 */
    {0xFFFFFFFF, 0xFFFFFFFF},	/* interrupt 56 reserved - enable everything */
    {0xFFFFFFFF, 0xFFFFFFFF},	/* interrupt 57 reserved - enable everything */
    {0xFFFFFFFF, 0xFFFFFFFF},	/* interrupt 58 reserved - enable everything */
    {0xFFFFFFFF, 0xFFFFFFFF},	/* interrupt 59 reserved - enable everything */
    {0xFFFFFFFF, 0xFFFFFFFF},	/* interrupt 60 reserved - enable everything */
    {0xFFFFFFFF, 0xFFFFFFFF},	/* interrupt 61 reserved - enable everything */
    {0xFFFFFFFF, 0xFFFFFFFF},	/* interrupt 62 reserved - enable everything */
    {0xFFFFFFFF, 0xFFFFFFFF}	/* interrupt 63 reserved - enable everything */
    };



LOCAL UINT32		iNumToMaskPattern[] = 
    {
    0x00000000,			/* interrupt number 0: Error */
    0x00008000,			/* interrupt number 1: I2C */
    0x00004000,			/* interrupt number 2: SPI */
    0x00002000,			/* interrupt number 3: Risc Timer Table */
    0x00001000,			/* interrupt number 4: SMC 1 */
    0x00000800,			/* interrupt number 5: SMC 2 */
    0x00000400,			/* interrupt number 6: IDMA1 */
    0x00000200,			/* interrupt number 7: IDMA2 */
    0x00000100,			/* interrupt number 8: IDMA3 */
    0x00000080,			/* interrupt number 9: IDMA4 */
    0x00000040,			/* interrupt number 10: SDMA */
    0x00000000,			/* interrupt number 11: reserved */
    0x00000010,			/* interrupt number 12: Timer 1 */
    0x00000008,			/* interrupt number 13: Timer 2 */
    0x00000004,			/* interrupt number 14: Timer 3 */
    0x00000002,			/* interrupt number 15: Timer 4 */
    0x00000004,			/* interrupt number 16: TMCNT */
    0x00000002,			/* interrupt number 17: PIT */
    0x00000000,			/* interrupt number 18: reserved */
    0x00004000,			/* interrupt number 19: IRQ 1 interrupt */
    0x00002000,			/* interrupt number 20: IRQ 2 interrupt */
    0x00001000,			/* interrupt number 21: IRQ 3 interrupt */
    0x00000800,			/* interrupt number 22: IRQ 4 interrupt */
    0x00000400,			/* interrupt number 23: IRQ 5 interrupt */
    0x00000200,			/* interrupt number 24: IRQ 6 interrupt */
    0x00000100,			/* interrupt number 25: IRQ 7 interrupt */
    0x00000000,			/* interrupt number 26: reserved */
    0x80000000,			/* interrupt number 27: FCC1 */
    0x40000000,			/* interrupt number 28: FCC2 */
    0x20000000,			/* interrupt number 29: FCC3 */
    0x00000000,			/* interrupt number 30: reserved */
    0x08000000,			/* interrupt number 31: MCC1 */
    0x04000000,			/* interrupt number 32: MCC2 */
    0x00000000,			/* interrupt number 33: reserved */
    0x00000000,			/* interrupt number 34: reserved */
    0x00800000,			/* interrupt number 35: SCC1 */
    0x00400000,			/* interrupt number 36: SCC2 */
    0x00200000,			/* interrupt number 37: SCC3 */
    0x00100000,			/* interrupt number 38: SCC4 */
    0x00000000,			/* interrupt number 39: reserved */
    0x00010000,			/* interrupt number 40: Parallel I/O [15] */
    0x00020000,			/* interrupt number 41: Parallel I/O [14] */
    0x00040000,			/* interrupt number 42: Parallel I/O [13] */
    0x00080000,			/* interrupt number 43: Parallel I/O [12] */
    0x00100000,			/* interrupt number 44: Parallel I/O [11] */
    0x00200000,			/* interrupt number 45: Parallel I/O [10] */
    0x00400000,			/* interrupt number 46: Parallel I/O [9] */
    0x00800000,			/* interrupt number 47: Parallel I/O [8] */
    0x01000000,			/* interrupt number 48: Parallel I/O [7] */
    0x02000000,			/* interrupt number 49: Parallel I/O [6] */
    0x04000000,			/* interrupt number 50: Parallel I/O [5] */
    0x08000000,			/* interrupt number 51: Parallel I/O [4] */
    0x10000000,			/* interrupt number 52: Parallel I/O [3] */
    0x20000000,			/* interrupt number 53: Parallel I/O [2] */
    0x40000000,			/* interrupt number 54: Parallel I/O [1] */
    0x80000000,			/* interrupt number 55: Parallel I/O [0] */
    0x00000000,			/* interrupt number 56: reserved */
    0x00000000,			/* interrupt number 57: reserved */
    0x00000000,			/* interrupt number 58: reserved */
    0x00000000,			/* interrupt number 59: reserved */
    0x00000000,			/* interrupt number 60: reserved */
    0x00000000,			/* interrupt number 61: reserved */
    0x00000000,			/* interrupt number 62: reserved */
    0x00000000			/* interrupt number 63: reserved */
    };

/* forward declarations */

LOCAL void	m8260IntrDeMux (void);
LOCAL STATUS	m8260IntConnect (VOIDFUNCPTR *, VOIDFUNCPTR, int);


/*******************************************************************************
*
* m8260IntrInit - initialize the interrupt manager for the PowerPC 8260
*
* This routine connects the default demultiplexer, m8260IntrDeMux(), to the 
* external interrupt vector and associates all 
* interrupt sources with the default interrupt handler.  This routine is
* called by sysHwInit() in sysLib.c.
*
* NOTE: All interrupt from the SIU unit are enabled, CICR is setup so
* that SCC1 has the highest relative interrupt priority, through SCC4 with the
* lowest.
*
* RETURN : OK always
*/

STATUS m8260IntrInit 
    (
    )
    {
    VOIDFUNCPTR defaultVec;     	/* INTR3 default vector */
    int		vector;

    UINT32 immrVal = (vxImmrGet()& IMMR_ISB_MASK);

    /* Get the default vector connected to the External Interrupt (0x500) */

    defaultVec = (VOIDFUNCPTR) excVecGet ((FUNCPTR *) _EXC_OFF_INTR);

    /* Connect the interrupt demultiplexer to External Interrupt (0x500) */

    excIntConnect ((VOIDFUNCPTR *) _EXC_OFF_INTR, m8260IntrDeMux);

    /* Install `system' intConnect routine */

    if (_func_intConnectRtn == NULL)
        _func_intConnectRtn = m8260IntConnect;

    if (_func_intEnableRtn == NULL)
        _func_intEnableRtn = m8260IntEnable;

    if (_func_intDisableRtn == NULL)
        _func_intDisableRtn = m8260IntDisable;

    /* Set all vectors to default handler */

    for (vector = 0; vector <= IVEC_MAX; vector++)
	intConnect (INUM_TO_IVEC(vector), (VOIDFUNCPTR) defaultVec, 0); 

    /* disable all interrupts */

    *M8260_SIMR_L(immrVal) = 0x00000000;
    *M8260_SIMR_H(immrVal) = 0x00000000;

    return (OK);
 
    }

/*******************************************************************************
*
* m8260IntConnect - connect a routine to an interrupt 
*
* This routine connects any C or assembly routine to one of the multiple 
* sources of interrupts.
*
* The connected routine can be any normal C code, except that it must not 
* invoke certain operating system functions that may block or perform I/O
* operations.
*
* <vector> types are defined in h/drv/intrClt/m8260Intr.h.
*
* RETURNS: OK, or ERROR if <vector> is unknown.
*
* SEE ALSO: m8260Intr.h
*/

LOCAL STATUS m8260IntConnect
    (
    VOIDFUNCPTR *	vector,		/* interrupt vector to attach to */
    VOIDFUNCPTR		routine,	/* routine to be called */
    int 		parameter	/* parameter to be passed to routine */
    )
    {

    /* test the vector */

    if ((int)(vector) > IVEC_MAX)
	return (ERROR);

    intrVecTable[(int)(vector)].vec = routine;
    intrVecTable[(int)(vector)].arg = parameter;

    return (OK);
    }

/*******************************************************************************
*
* m8260IntrDeMux - SIU interrupt demultiplexer 
*
* This routine must be bound to external interrupt exception (vector 0x500). 
* It is used to call the appropriate handler with its argument when an
* interrupt occurs. 
*
* The interrupts are prioritized in the following order:
*
* NOTE: when this function is called the interrupts are still locked. It's
* this function responsability to unlock the interrupt.
*
* RETURNS: N/A
*/

void m8260IntrDeMux (void)
    {
    VINT32 	intVec;			/* interrupt vector */
    UINT32	oldIntMask_L;		/* current interrupt mask */
    UINT32	oldIntMask_H;		/* current interrupt mask */

    UINT32 immrVal = (vxImmrGet() & IMMR_ISB_MASK);

    while(WAIT_FOREVER)
        {
   
    	/* read the interrupt vector register */

    	intVec = (0x0000003f & ((* M8260_SIVEC(immrVal)) >> 26));
    	
    	if(!intVec)
    	    {
            break;
    	    }
#ifdef  INCLUDE_WINDVIEW
    	WV_EVT_INT_ENT(intVec)
#endif /* INCLUDE_WINDVIEW */

    	/* save the current interrupt mask */ 

    	oldIntMask_L = * M8260_SIMR_L(immrVal);
    	oldIntMask_H = * M8260_SIMR_H(immrVal);

    	/* enable only the higher priority interrupts */

    	* M8260_SIMR_L(immrVal)&= 
		enableHighPriInts[IVEC_TO_INUM((VOIDFUNCPTR *)intVec)].simr_l;
    	* M8260_SIMR_H(immrVal)&= 
		enableHighPriInts[IVEC_TO_INUM((VOIDFUNCPTR *)intVec)].simr_h;

    	/* unlock the interrupt */

    	intUnlock (_PPC_MSR_EE);

    	/* call the Interrupt Handler */

    	/* table is indexed by vector */

    	intrVecTable[intVec].vec (intrVecTable[intVec].arg); 

    	/* restore the interrupt mask */

    	* M8260_SIMR_L(immrVal) = oldIntMask_L;
    	* M8260_SIMR_H(immrVal) = oldIntMask_H;
        }

    return;
    }

/*******************************************************************************
*
* m8260IntEnable - enable the indicated interrupt
*
* This routine will enable the indicated interrupt by setting the appropriate
* bit in the SIU Interrupt Mask Registers.
*
* The design of the 8260 presents the following design requirements:
*
* 1. the mapping from interrupt number to mask bit can not be represented by 
*    a function. An array, indexed by interrupt number (INUM), is used to map
*    the interrupt number to the appropriate mask. 
*
* 2. There are two 32 bit mask registers (SIMR_L and SIMR_H). The interrupt
*    number must be compared to 4 ranges to determine which register contains
*    its mask bit:
*	
* .CS
*	interrupt number
*	in m8260IntrCtl.h       register
*	----------------        -------
*	0-15                    SIMR_L
*	16-31                   SIMR_H
*	32-47                   SIMR_L
*	48-63                   SIMR_H
* .CE
*
* RETURNS: 0, always.
*/

int m8260IntEnable
    (
    int intNum		/* interrupt level to enable */
    )
    {
    UINT32 immrVal = (vxImmrGet() & IMMR_ISB_MASK);

    if ((intNum > 0) && (intNum <= IVEC_MAX))
	{
	if (((intNum >= 0) && (intNum <= 15)) || 
	    ((intNum >= 27) && (intNum <= 38)))
	    *M8260_SIMR_L(immrVal) |= iNumToMaskPattern[intNum];
	else
	    *M8260_SIMR_H(immrVal) |= iNumToMaskPattern[intNum];
	}
    return 0;
    }

/*******************************************************************************
*
* m8260IntDisable - Disable one of the Level or IRQ interrupts into the SIU
*
* This routine will mask the bit in the SIMASK register corresponding to
* the requested interrupt level.  
* 
* RETURNS: 0, always.
*/

int m8260IntDisable
    (
    int intNum          /* interrupt level to disable */
    )
    {
    UINT32 immrVal = (vxImmrGet() & IMMR_ISB_MASK);

    if ((intNum >= 0) && (intNum <= IVEC_MAX))
        { 
	if (((intNum >= 0) && (intNum <= 15)) || 
	    ((intNum >= 27) && (intNum <= 38)))
	    *M8260_SIMR_L(immrVal) &= ~iNumToMaskPattern[intNum];   
	else
	    *M8260_SIMR_H(immrVal) &= ~iNumToMaskPattern[intNum];
        }

    return 0;
    }

VOIDFUNCPTR * m8260InumToIvec 
    (
    int intNum
    )
    {

    if (intNum <0) 	/* error */
	return ((VOIDFUNCPTR *) 0);

    else if (intNum <= 25)
	return ((VOIDFUNCPTR *) intNum);

    else if (intNum == 26) 	/* error */
	return ((VOIDFUNCPTR *) 0);

    else if (intNum <= 38)
	return ((VOIDFUNCPTR *) (intNum + 5));

    else if (intNum == 39) 	/* error */
	return ((VOIDFUNCPTR *) 0);

    else if (intNum <= 55)
	return ((VOIDFUNCPTR *) (intNum + 8));

    else	/* error */
	return ((VOIDFUNCPTR *) 0);
    }


/*******************************************************************************
*
* m8260IvecToInum - get the relevant interrupt number
*
* This routine finds out the interrupt number associated with the vector
* in <vector>.
*
* <vector> types are defined in h/drv/intrClt/m8260Intr.h.
*
* RETURNS: the interrupt number for the vector
*
* SEE ALSO: m8260Intr.h
*/
 
int m8260IvecToInum
    (
    VOIDFUNCPTR *       vector         /* interrupt vector to attach to */
    )
    {
 
    /* test the vector */
 
    if ((int) (vector) <= 0)
        return ((int ) 0);
 
    else if ((int) vector < INUM_RESERVED26)
        return ((int) vector);
 
    else if ((int) vector < 32)
        return (INUM_RESERVED26);

    else if ((int) vector < 44)
        return ((int) vector - 5);

    else if ((int) vector < 48)
        return ((int) INUM_RESERVED39);

    else if ((int) vector <= IVEC_MAX)
        return ((int) vector - 8);
 
    else 
        return (0);
    }

