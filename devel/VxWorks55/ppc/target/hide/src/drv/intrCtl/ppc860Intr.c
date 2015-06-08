/* ppc860Intr.c - PowerPC 800 series interrupt driver */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1997,1998 Motorola, Inc., All Rights Reserved */
#include "copyright_wrs.h"

/*
modification history
--------------------
01i,28jan02,dtr  Removing diab warnings.
01h,15sep01,dat  Removed special def of CACHE_PIPE_FLUSH
01g,28apr99,dat  Merge from 2_0_0
01o,30jan99,cn   fixed SPR #22321.
01n,24aug98,cjtc windview 2.0 event logging is now single step. Timestamp no
                 longer stored by intEnt. Avoids out-of-sequence timestamps in
                 event log (SPR 21868)
0lm,12aug98,gls  corrected assignment of evtIntNum
01l,04aug98,gls  fixed SPR #21239, kernel locks on boot when instrumented
01k,16apr98,pr   added WindView 20 instrumentation.
01j,30mar98,map  initialize _func_int{Enable,Disable}Rtn [SPR# 20447]
01i,26mar98,map  added ppc860IntEnable, ppc860IntDisable, removed duplicate
                 #ifdef'd code, reworked ppc860IntrInit(). [SPR# 20447]
01h,11nov97,map  redo clearing CISR bits [SPR# 8781], and default SCC
                 interrupt priority
01g,23jun97,map  added default SCC interrupt priority [SPR# 8692]
01f,19jun97,map  added IV_SCC[34] definitions [SPR# 8450]
01e,17jun97,map  fixed clearing CISR bits [SPR# 8781]
01d,06mar97,tpr  fixed properly SPR #8042.
01c,21feb97,tpr  fixed interrupt bit clearing in CISR (SPR #8042).
01b,18sep96,pr   added WindView instrumentation.
01a,17feb96,tpr  written.
*/

/*
DESCRIPTION
The PowerPC 800 CPU series is divided in three units: PowerPC core, System
Interface Unit (SIU) and Communication Processor Unit (CPM). The PowerPC
core accepts only one external interrupt exception (vector 0x500). The SIU
provides an interrupt controller which provides 32 levels but only 16 are
used. The Interrupt controller is connected to the PowerPC core external 
interrupt. The CPM unit also provides an interrupt controller which can
be connected to one of the SIU interrupt controler entries. This library
provides the routines to manage these interrupt controllers


ppc860IntrInit() connects the default demultiplexers, ppc860IntrDeMux() and
ppc860CpmIntrDeMux(), to the external interrupt vector and initializes a table
containing a function pointer and an associated parameter for each interrupt
source. ppc860IntrInit() is called by sysHwInit() from sysLib.c.

The default demultimplexer, ppc860IntrDeMux() detects which peripheral or 
controller has raised an interrupt and calls the associated routine with its 
parameter. If the interrupt comes from the CPM interrupt contoller, the
ppc860CpmIntDeMux() function is called. As ppc860IntrDeMux(), this function
detects which peripheral or controller has raised an interrupt and calls the
associated routine with its parameter.

INCLUDE FILES: drv/intrCtl/ppc860Intr.h, drv/multi/ppc860Siu.h,
drv/multi/ppc860Cpm.h
*/

/* includes */

#include "intLib.h"
#include "iv.h"
#include "drv/intrCtl/ppc860Intr.h"
#include "drv/multi/ppc860Siu.h"
#include "drv/multi/ppc860Cpm.h"

/* default SCC relative interrupt priority */

#define	CICR_SCCP_DEF	(CICR_SCCDP_SCC4 | CICR_SCCCP_SCC3 | \
                         CICR_SCCBP_SCC2 | CICR_SCCAP_SCC1 )

#ifdef  INCLUDE_WINDVIEW

#include "private/funcBindP.h"
#include "private/eventP.h"

LOCAL  int		cpmIntNum;
LOCAL  int		cpmVecNum;

#endif /* INCLUDE_WINDVIEW */

/* local */

LOCAL INTR_HANDLER	intrVecTable[NUM_VEC_MAX];	/*  Intr vector table */

/* forward declarations */

LOCAL void	ppc860IntrDeMux (void);
LOCAL void	ppc860CpmIntrDeMux (int);
LOCAL STATUS	ppc860IntConnect (VOIDFUNCPTR *, VOIDFUNCPTR, int);
LOCAL int	ppc860IntEnable (int);
LOCAL int	ppc860IntDisable (int);

/*******************************************************************************
*
* ppc860IntrInit - initialize the interrupt manager for the PowerPC 800 series
*
* This routine connects the default demultiplexers, ppc860IntrDeMux() and 
* ppc860CpmIntrDeMux, to the external interrupt vector and associates all 
* interrupt sources with the default interrupt handler.  This routine is
* called by sysHwInit() in sysLib.c.
*
* NOTE: All interrupt from the SIU and CPM unit are enabled, CICR is setup so
* that SCC1 has the highest relative interrupt priority, through SCC4 with the
* lowest.
*
* RETURN : OK or ERROR if the SUI interrupt level to connect the CPM 
* interrupt contoller is wrong.
*/

STATUS ppc860IntrInit 
    (
    VOIDFUNCPTR *	cpmIntrVec	/* Intr level of the CPM Intr ctrl */
    )
    {
    VOIDFUNCPTR defaultVec;     	/* INTR3 default vector */
    UINT32	regBase;		/* device register base address */
    UINT32	cicrIntLevel;		/* CICR interupt level */
    UINT32	simaskIntLevel;
    int		vector;

    /* Get the default vector connected to the External Interrupt (0x500) */

    defaultVec = (VOIDFUNCPTR) excVecGet ((FUNCPTR *) _EXC_OFF_INTR);

    /* Connect the interrupt demultiplexer to External Interrupt (0x500) */

    excIntConnect ((VOIDFUNCPTR *) _EXC_OFF_INTR, ppc860IntrDeMux);

    /* Install `system' intConnect routine */

    if (_func_intConnectRtn == NULL)
        _func_intConnectRtn = ppc860IntConnect;

    if (_func_intEnableRtn == NULL)
        _func_intEnableRtn = ppc860IntEnable;

    if (_func_intDisableRtn == NULL)
        _func_intDisableRtn = ppc860IntDisable;

    /* Set all vectors to default handler */

    for (vector = 0; vector < NUM_VEC_MAX; vector++)
	intConnect (INUM_TO_IVEC(vector), defaultVec, 0);

    /* Get the CPM interrupt level */

    switch (IVEC_TO_INUM(cpmIntrVec))
	{
	case IVEC_TO_INUM(IV_LEVEL0): 
	    cicrIntLevel = CICR_IRL_LVL0;
	    simaskIntLevel = SIMASK_LVM0;
	    break; 

	case IVEC_TO_INUM(IV_LEVEL1):
	    cicrIntLevel = CICR_IRL_LVL1;
	    simaskIntLevel = SIMASK_LVM1;
	    break; 

	case IVEC_TO_INUM(IV_LEVEL2):
	    cicrIntLevel = CICR_IRL_LVL2;
	    simaskIntLevel = SIMASK_LVM2;
	    break; 

	case IVEC_TO_INUM(IV_LEVEL3):
	    cicrIntLevel = CICR_IRL_LVL3;
	    simaskIntLevel = SIMASK_LVM3;
	    break; 

	case IVEC_TO_INUM(IV_LEVEL4):
	    cicrIntLevel = CICR_IRL_LVL4;
	    simaskIntLevel = SIMASK_LVM4;
	    break; 

	case IVEC_TO_INUM(IV_LEVEL5):
	    cicrIntLevel = CICR_IRL_LVL5;
	    simaskIntLevel = SIMASK_LVM5;
	    break; 

	case IVEC_TO_INUM(IV_LEVEL6):
	    cicrIntLevel = CICR_IRL_LVL6;
	    simaskIntLevel = SIMASK_LVM6;
	    break; 

	case IVEC_TO_INUM(IV_LEVEL7):
	    cicrIntLevel = CICR_IRL_LVL7;
	    simaskIntLevel = SIMASK_LVM7;
	    break; 

	default:
	    return (ERROR);
	}

    /*
     * Connect the CPM interrupt demultiplexer at the level specified by
     * `cpmIntLevel'
     */

#ifdef  INCLUDE_WINDVIEW
    cpmVecNum = IVEC_TO_INUM(cpmIntrVec);
#endif /* INCLUDE_WINDVIEW */

    intConnect (cpmIntrVec, ppc860CpmIntrDeMux, simaskIntLevel);

    /* Get the register base address */

    regBase = vxImmrGet();

    /* 
     * Set the request interrupt level from the CPM and enable CPM interrupt
     * generation.
     */

    * CICR(regBase) = (CICR_SCCP_DEF | cicrIntLevel | CICR_IEN | CICR_HP_MSK);
     
    /* Enable required CPM interrupt */

    * SIMASK(regBase) = simaskIntLevel;

    return (OK);
 
    }

/*******************************************************************************
*
* ppc860IntConnect - connect a routine to an interrupt 
*
* This routine connects any C or assembly routine to one of the multiple 
* sources of interrupts.
*
* The connected routine can be any normal C code, except that it must not 
* invoke certain operating system functions that may block or perform I/O
* operations.
*
* <vector> types are defined in h/drv/intrClt/ppc860Intr.h.
*
* RETURNS: OK, or ERROR if <vector> is unknown.
*
* SEE ALSO: ppc860Intr.h
*/

LOCAL STATUS ppc860IntConnect
    (
    VOIDFUNCPTR *	vector,		/* interrupt vector to attach to */
    VOIDFUNCPTR		routine,	/* routine to be called */
    int 		parameter	/* parameter to be passed to routine */
    )
    {

    /* test the vector */

    if (IVEC_TO_INUM(vector) >= NUM_VEC_MAX)
	return (ERROR);

    intrVecTable[IVEC_TO_INUM(vector)].vec = routine;
    intrVecTable[IVEC_TO_INUM(vector)].arg = parameter;

    return (OK);
    }

/*******************************************************************************
*
* ppc860IntrDeMux - SIU interrupt demultiplexer 
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

LOCAL void ppc860IntrDeMux (void)
    {
#ifdef	INCLUDE_WINDVIEW
    UINT32 	evtIntNum;		/* windview interrupt vector */
#endif /* INCLUDE_WINDVIEW */
    UINT8 	intVec;			/* interrupt vector */
    UINT32	regBase;		/* register Base Address */
    UINT32	intMask;		/* current interrupt mask */


    regBase = vxImmrGet();

    /* read the interrupt vector register */

    intVec = (* SIVEC(regBase)) >> 26;

#ifdef  INCLUDE_WINDVIEW

    /* 
     * if it is a cpm interrupt we need to know the corresponding cpm
     * level, that we should get from the CPM demultiplexer. But we
     * need the value here and the value can be read only once. We 
     * save the value here so that once the CPM demultiplexer is called,
     * it will read the value from the global variable cpmIntNum.
     * 
     * Since we do not know the CPM level a priori, we have to leave
     * enough space for the cpm interrupts in the windview list.
     * Therefore, any interrupt number bigger than the CPM level will
     * be increased of 15 so to leave those spots to the corresponding
     * CPM interrupts.
     */

    if (intVec == cpmVecNum)
	{
	/* acknowledge the interrupt */
	
	*CIVR(regBase) = CIVR_IACK;    

	CACHE_PIPE_FLUSH ();
	
	cpmIntNum = * CIVR(regBase) >> 11; 
	evtIntNum = cpmIntNum + 32;
	}
    else
	evtIntNum = intVec;

    WV_EVT_INT_ENT(evtIntNum)

#endif /* INCLUDE_WINDVIEW */

    /* save the current interrupt mask */ 

    intMask = * SIMASK(regBase);

    /* lock all levels inferior to the interrupt detected */

    * SIMASK(regBase) &= (0xffffffff << (32 - intVec));

    /* unlock the interrupt */

    intUnlock (_PPC_MSR_EE);

    /* call the Interrupt Handler */

    intrVecTable[intVec].vec (intrVecTable[intVec].arg);

    /* restore the interrupt mask */

    * SIMASK(regBase) = intMask;

    return;
    }

/*******************************************************************************
*
* ppc860CpmIntrDeMux - CPM interrupt demultiplexer 
*
* This routine must be bound to external interrupt exception (vector 0x500). 
* It is used to call the appropriate handler with its argument when an
* interrupt occurs. 
*
* The interrupts are prioritized in the following order:
*
* RETURNS: N/A
*/

LOCAL void ppc860CpmIntrDeMux 
    (
    int simaskLevel
    )
    {
    int		intNum;			/* interrupt number */
    UINT32	regBase;		/* register Base Address */
    UINT32	intOldMask;		/* interrupt mask before the interrupt*/

    /* get the device register base address */

    regBase = vxImmrGet();

    /* get the interrupt vector */

#ifdef  INCLUDE_WINDVIEW
    intNum = cpmIntNum; 
#else
    /* acknowledge the interrupt */

    * CIVR(regBase) = CIVR_IACK;

    intNum = * CIVR(regBase) >> 11; 
#endif /* INCLUDE_WINDVIEW */

    /* save the interrupt mask before the interrupt */

    intOldMask = * CIMR(regBase);

    /* lock all levels inferior to the interrupt detected */

    * CIMR(regBase) &= (0xffffffff << (intNum + 1));

    /* enable the CPM interrupt in the SIU controller */

    * SIMASK(regBase) |= simaskLevel;

    /* call the Interrupt Handler */

    intrVecTable[intNum + 32].vec (intrVecTable[intNum + 32].arg);

    /* clear the bit in the CISR */

    * CISR(regBase) = (1 << intNum);

    /* restore the interrupt mask */
    
    * CIMR(regBase) = intOldMask;
    }

/*******************************************************************************
*
* ppc860IntEnable - enable one of the Level or IRQ interrupts into the SIU
*
* This routine will unmask the bit in the SIMASK register corresponding to
* the requested interrupt level.  The interrupt level must be in the range
* of 0 - 31.
* 
* RETURNS: 0, always.
*/

LOCAL int ppc860IntEnable
    (
    int intNum		/* interrupt level to enable (0 - 31) */
    )
    {
    UINT32 regBase;

    regBase = vxImmrGet();
    if ((intNum >= 0) && (intNum < 32))
	* SIMASK(regBase) |= (1 << (31 - intNum));
    return 0;
    }

/*******************************************************************************
*
* ppc860IntDisable - Disable one of the Level or IRQ interrupts into the SIU
*
* This routine will mask the bit in the SIMASK register corresponding to
* the requested interrupt level.  The interrupt level must be in the range
* of 0 - 31.
* 
* RETURNS: 0, always.
*/

LOCAL int ppc860IntDisable
    (
    int intNum          /* interrupt level to disable (0 - 31) */
    )
    {
    UINT32 regBase;
    
    regBase = vxImmrGet();
    if ((intNum >= 0) && (intNum < 32))
        * SIMASK(regBase) &= ~(1 << (31 - intNum));
    return 0;
    }

