/* vic068.c - VTC VMEbus Interface Controller (VIC068) */

/* Copyright 1984-1996 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01h,20dec96,dat  fixed SPR 3430, bug in sysIntDisable.
01g,11jul96,wlf  doc: more cleanup.
01f,11jul96,wlf  doc: cleanup.
01e,13jun96,wlf  doc: cleanup and spelling chgs to a printf and logMsg.
01d,09Aug95,kkk  fix warnings for i960 (spr# 4177)
01c,25apr94,wmd  Changed line to correctly test range of interrupt level.
01b,02mar93,ccc  removed reference to HKV960, fixed warnings.
01a,02oct92,ccc	 written.
*/

/*
DESCRIPTION
This library provides VMEbus Interface Controller (VIC) routines.
*/

/* LINTLIBRARY */

#include "drv/vme/vic068.h"

/* locals */

struct mailbox
    {
    FUNCPTR  sysMailboxRoutine;         /* routine called upon interrupt */
    int	     sysMailboxArg;             /* argument passed to routine */
    BOOL     sysMailboxConnected;       /* mail connected state */
    } mailTab[] =
	{
	{((FUNCPTR) logMsg), ((int) "mailbox0 interrupt\n"), FALSE},
	{((FUNCPTR) logMsg), ((int) "mailbox1 interrupt\n"), FALSE},
	{((FUNCPTR) logMsg), ((int) "mailbox2 interrupt\n"), FALSE},
	{((FUNCPTR) logMsg), ((int) "mailbox3 interrupt\n"), FALSE},
	{((FUNCPTR) logMsg), ((int) "mailbox4 interrupt\n"), FALSE},
	{((FUNCPTR) logMsg), ((int) "mailbox5 interrupt\n"), FALSE},
	{((FUNCPTR) logMsg), ((int) "mailbox6 interrupt\n"), FALSE},
	{((FUNCPTR) logMsg), ((int) "mailbox7 interrupt\n"), FALSE}
	};

/* temp routines for sysMailboxConnect */

LOCAL FUNCPTR	tmpMailboxRoutine = (FUNCPTR) logMsg;
LOCAL int	tmpMailboxArg =  (int) "mailbox interrupt\n";
LOCAL BOOL	tmpMailboxConnected = FALSE;

LOCAL char *    sysMailboxBase      = 0;

/* forward declarations */

UINT8	sysVMEIntAck ();

/******************************************************************************
*
* sysBusIntGen - generate a bus interrupt
*
* This routine generates a bus interrupt of a specified level with a
* specified vector.
*
* RETURNS: OK, or ERROR if <sysIntLvl> is out of range or the board cannot
* generate a bus interrupt.
*
* SEE ALSO: sysBusIntAck()
*/

STATUS sysBusIntGen
    (
    INT32 	sysIntLvl,      /* bus interrupt level to generate (1-7) */
    INT32 	vector          /* interrupt vector to generate (0-255)  */
    )
    {
    int sysIplNo = DEFAULT_IPL;

    /* check to see if sysIntLvl and IPL are in range */

    if ((sysIntLvl < 1) || (sysIntLvl > 7))
         return (ERROR);

    /* check for unacknowledged interrupt */

    if (*VIC_VIRSR & (1 << sysIntLvl))
	{
	logMsg ("Unacknowledged interrupt present on VMEbus.\n");
	return (ERROR);
	}

    /* write vector to appropriate location */

    *(VIC_VIVR1 + (sysIntLvl - 1) * 4) = (UINT8) vector;

    *VIC_VIRSR = (1 | (1 << sysIntLvl));    /* generate the VME interrupt */

    *(VIC_VIICR) = 0x80 | sysIplNo;

    return (OK);
    }

/*******************************************************************************
*
* sysMailboxInt - handle a mailbox interrupt
*
* This routine handles mailbox interrupts. The icg and icm switches will 
* generate an interrupt on a  clear to set transition. The mailboxes are 
* numbered here from 0-7 (icgs 1-4: mailbox 0-3, icms 1-4: mailbox 4-7.
*
* The icgs mailboxes are cleared by writing to the clear address of the 
* associated mailbox. This isn't necessary with the icms mailboxes but it
* would function the same if the icms clear was written to.
*/

LOCAL void sysMailboxInt (void)

    {
    UINT8 iack = 0;   /* interrupt vector */
    int    box;       /* mailbox pointer into the mailTab array */
    UINT8  ipl;       /* ipl number of interrupting mailbox */
    UINT8  switchReg; /* interrupting mailbox */
    UINT32 ix;

    /*
     * Acknowledge the interrupt based on the ipl number. The ipl number is 
     * determined by which mailbox set (icgs or icms) caused the interrupt and 
     * the appropriate interrupt control register is read 
     */

    switchReg = *VIC_ICSR;

    /* retrieve the ipl number from the requested mailbox's Int control reg */

    ipl = (switchReg & 0xf) ? (*VIC_ICMICR & ICMICR_IRQ_LVL_MASK) :
			      (*VIC_ICGICR & ICGICR_IRQ_LVL_MASK);

    switch (ipl)
	{
	case  TARGET_IPL0:
	    iack = *VIC_IACK_IPL0;
	    break;
	case  TARGET_IPL1:
	    iack = *VIC_IACK_IPL1;
	    break;
	case  TARGET_IPL2:
	    iack = *VIC_IACK_IPL2;
	    break;
	default:
	    logMsg ("VIC mailbox interrupt error. Wrong ipl number.\n");
	}

    /* determine the mailbox number based on the status register */

    switch (switchReg)
	{
	case 0x1 :
	case 0x2 :
	case 0x4 :
	case 0x8 :
	    box = (iack & 0x3) + 4; /* the mailbox is passed in with the */
	                            /* vector number (lower 2 bits) */
	    break;
	case 0x10:
	    box = 3;
	    break;
	case 0x20:
	    box = 2;
	    break;
	case 0x40:
	    box = 1;
	    break;
	case 0x80:
	    box = 0;
	    break;
	default:
	    logMsg ("VIC mailbox interrupt error. Wrong switch number.\n");
	    box = 7;
	}

    /* clear the the interrupt by writing to the clear address associated with
     * the mailbox that got hit
     */

    if (box < 4)   /* clear the icgs box */
	*VIC_ICSR &= 0x0f & ~(switchReg);

    else	   /* clear the icms side of interrupt status register */
	*VIC_ICSR &= 0xf0 & (~switchReg);

    /* call mailbox service routine */

    if (mailTab[box].sysMailboxRoutine != NULL)
	(*mailTab[box].sysMailboxRoutine) (mailTab[box].sysMailboxArg);

#ifdef DEBUG
    if (*VIC_ICSR)
	logMsg ("missed interrupt\n");   /**/
#endif /* DEBUG */

    vxIPNDClear (ipl);              /* clear pending bit in IPND reg */
    for (ix = 0; ix < 16; ix++)
	;    			    /* required to provide settling time */
    }

/******************************************************************************
*
* sysMailboxEnable - enable the mailbox interrupt
*
* This routine enables the mailbox interrupt.  It should be called
* after sysMailboxConnect(). 
*
* The mailbox table is filled in based on what was done in
* sysMailboxConnect().  There are eight mailboxes to choose from.  The
* mailbox used is determined from the address of the mailbox to be enabled.
* Zero is reserved for the backplane driver.
*
* RETURNS: OK, always.
*
* SEE ALSO: sysMailboxConnect()
*/

STATUS sysMailboxEnable
    (
    char 	*mailboxAdrs 		/* mailbox address */
    )
    {
    UINT8 	mask;
    UINT8 	boxNum;             /* mailbox number to enable */
    UINT8	boxSet;             /* interprocessor or global mailbox */

    /* determine the mailbox number based on the A1-A3 address lines */

    boxNum = (((UINT8) (int) mailboxAdrs & 0x0e) >> 1);

    /* determine which mailbox set based on A4/A5 address lines */

    boxSet = (UINT8) (int) mailboxAdrs & (VME_ICGS | VME_ICMS);
    boxNum += (boxSet == VME_ICMS) ? 4 : 0;

    if (mailTab[boxNum].sysMailboxConnected)
	return (ERROR);                          /* already used try another */

    mailTab[boxNum].sysMailboxRoutine   = tmpMailboxRoutine;
    mailTab[boxNum].sysMailboxArg       = tmpMailboxArg;
    mailTab[boxNum].sysMailboxConnected = tmpMailboxConnected;

    /* reinitialize the temporary mailbox variables */

    tmpMailboxRoutine   = (FUNCPTR) logMsg;
    tmpMailboxArg       = (int) "mailbox interrupt\n";
    tmpMailboxConnected = FALSE;

    if (intConnect ((VOIDFUNCPTR *) INT_VEC_MBOX,
		    (VOIDFUNCPTR) sysMailboxInt, 0) == ERROR)
	{
	printf ("had an error connecting to interrupt\n");
	return (ERROR);
	}

    /* set mailbox base address and save for future reference */

    sysMailboxBase = (char *) ((UINT32)mailboxAdrs & 0xffffff00);

    if (boxSet == (UINT8) VME_ICMS)    /* read current mailbox configuration */
        mask = *VIC_ICMICR;
    else
        mask = *VIC_ICGICR;

#ifdef	DEBUG
    if (bpDebug & 0x4)
	printf ("BoxEnable -- mailboxAdrs: %#x mbox >> 8: %#x  mask: %#x boxSet: %#x\n",
	    mailboxAdrs, (UINT32) mailboxAdrs >> 8, mask, boxSet);
#endif	/* DEBUG */

    /* set the box enable bit */

    mask &= (boxSet == VME_ICMS) ? ~(1 << boxNum) : ~(1 << (boxNum + 4));

#ifdef DEBUG
    if (bpDebug & 0x4)
	printf ("BoxEnable-- ~(1 << equ) : %#x\n",
		(boxSet == VME_ICMS) ? ~(1 << boxNum) : ~(1 << (boxNum + 4)));
#endif	/* DEBUG */

    mask = (mask & 0xF8) + VIC_MAILBOX_IPL0;   /* set the IPL number */

#ifdef DEBUG
    if (bpDebug & 0x4)
	printf ("sysMailboxBase: %#x   boxNum:  %#x    mask: %#x\n",
		sysMailboxBase, boxNum, mask);
#endif	/* DEBUG */

    *VIC_ICSR  = 0x0;               /* clear mailbox flags */
    if (boxSet == VME_ICMS)	        /* enable the selected mailbox */
        *VIC_ICMICR = mask;
    else
        *VIC_ICGICR = mask;

    *VIC_ICGIVBR = INT_VEC_MBOX;
    *VIC_ICMIVBR = INT_VEC_MBOX;

    /* enable the 960 interrupt mask register based on the ipl number in the
     * interrupt control register.
     */

    vxIMRSet (mask & 0x3);
    return (OK);
    }

/******************************************************************************
*
* sysMailboxConnect - connect a routine to the mailbox interrupt
*
* This routine specifies the interrupt service routine to be called at each
* mailbox interrupt.  It should be called before
* sysMailboxEnable().  
*
* This routine sets up temporary variables until the box number is
* determined.  The mailbox number is determined from the address passed into
* sysMailboxEnable().
*
* RETURNS: OK, or ERROR if the routine cannot be connected to the interrupt.
*
* SEE ALSO: intConnect(), sysMailboxEnable()
*/

STATUS sysMailboxConnect
    (
    FUNCPTR routine,    /* routine called at each mailbox interrupt */
    int     arg         /* argument with which to call routine */
    )
    {
#ifdef DEBUG
    if (bpDebug & 0x4)
        printf ("sysMailBoxConnect-- routine: %#x, arg: %#x\n", routine, arg);
#endif	/* DEBUG */

    if (tmpMailboxConnected)
	return (ERROR); /* tried to connect another routine before enabling */
			/* last routine connected. See sysMailboxEnable */

/* put the passed parameters in temporary storage until mailbox is enabled */

    tmpMailboxRoutine   = routine;
    tmpMailboxArg       = arg;
    tmpMailboxConnected = TRUE;

    return (OK);
    }

/******************************************************************************
*
* sysBusTas - test and set a location across the bus
*
* This routine performs a 80960 test-and-set instruction across the backplane.
*
* RETURNS: TRUE if the value had not been set but is now, or FALSE if the value
* was set already.
*
* SEE ALSO: vxTas()
*/

BOOL sysBusTas
    (
    char *addr         /* address to be tested and set */
    )
    {
    volatile UINT  dummy;
    UINT oldRelCntrl;
    BOOL retVal;

    /* get current state of release control register */

    oldRelCntrl = *VIC_RCR;

    /* tell the vic to release the bus only when done */

    *VIC_RCR = RCR_RWD;

    dummy = *addr; /* attempt to grab the bus for good measure */
    retVal = vxTas (addr);

    /* put release control register back to previous state */

    *VIC_RCR = oldRelCntrl;
    return(retVal);
    }

/******************************************************************************
*
* sysIntEnable - enable a bus interrupt level
*
* This routine unmasks the VMEbus interrupt specified by <sysVMEIntNo>.  The
* interrupt is presented to the processor as a value between INT0 and INT2 as 
* specified by the local variable <sysIPLNo>, which is computed internally by
* this routine.
*
* RETURNS: OK, or ERROR if <sysVMEIntNo> is greater than SYS_MAX_VME_INTS
* or <sysIPLNo> is greater than 0x02.
*
* SEE ALSO: sysIntDisable()
*/

STATUS sysIntEnable
    (
    int 	sysVMEIntNo		/* VME interrupt request number */
    )
    {
    UINT32 	sysIplNo;

    /* need to test for system defined constants in config.h  */

    switch (sysVMEIntNo)
	{
	case INT_LVL_MBOX:
	    sysIplNo = INT_LVL_MBOX - 1;
	    break;
	case INT_LVL_ENP:
	    sysIplNo = INT_LVL_ENP - 1;
	    break;
	case INT_LVL_EX:
	    sysIplNo = INT_LVL_EX - 1;
	    break;
        default:
	    sysIplNo = DEFAULT_IPL;
	}

    if (sysVMEIntNo > SYS_MAX_VME_INTS || sysIplNo > 0x02)
	return (ERROR);

    sysIplNo = 1 << sysIplNo;

    *(VIC_VICR1 + (sysVMEIntNo - 1) * 4) = sysIplNo;

    *VIC_VIICR &= 0x7f;		/* enable global interrupt */

    vxIMRSet (sysIplNo);

    return (OK);
    }

/******************************************************************************
*
* sysIntDisable - disable a bus interrupt level
*
* This routine disables the VMEbus interrupt specified by <sysVMEIntNo>.  The
* interrupt is presented to the processor as a value between 
* INT0 and INT2.  If the local variable <sysIPLNo>--computed internally in this
* routine--is 0x00, sysIntDisable() cannot disable VMEbus 
* interrupts.
*
* RETURNS: OK, or ERROR if <sysVMEIntNo> is greater than SYS_MAX_VME_INTS
* or <sysIPLNo> is greater than 0x02.
*
* SEE ALSO: sysIntEnable()
*/

STATUS sysIntDisable
    (
    int 	sysVMEIntNo		/* VME interrupt request number */
    )
    {
    UINT32 	sysIplNo;

    /* need to test for system defined constants in config.h  */

    switch (sysVMEIntNo)
	{
	case INT_LVL_MBOX:
	    sysIplNo = INT_LVL_MBOX - 1;
	    break;
	case INT_LVL_ENP:
	    sysIplNo = INT_LVL_ENP - 1;
	    break;
	case INT_LVL_EX:
	    sysIplNo = INT_LVL_EX - 1;
	    break;
        default:
	    sysIplNo = DEFAULT_IPL;
	}

    sysIplNo = 1 << sysIplNo;

    if (sysVMEIntNo > SYS_MAX_VME_INTS || sysIplNo > 0x04 ||
	(*(VIC_VICR1 + (sysVMEIntNo - 1) * 4) & 0x07) != sysIplNo)
	{
	return (ERROR);
	}

    if (sysVMEIntNo != 0x00)
        *(VIC_VICR1 + (sysVMEIntNo - 1) * 4) = 0x80 | sysIplNo;

    return (OK);
    }

/******************************************************************************
*
* sysBusIntAck - acknowledge a bus interrupt
*
* This routine acknowledges a specified VMEbus interrupt level.
*
* When an interrupt is presented to the 80960CA on XINT0 - XINT2, the vector
* must be read to remove the interrupt request.  This routine returns the
* vector for the specific interrupt priority level (IPL) line, which is read
* from the interrupter interrupt control register on the VIC chip.
*
* RETURNS: The VMEbus interrupt vector level.
*
* SEE ALSO: sysBusIntGen()
*/

int sysBusIntAck
    (
    int	sysIntLvl		/* interrupt level to acknowledge */
    )
    {
    return ((int) sysVMEIntAck ((((UINT32) * (VIC_VICR1 + (sysIntLvl - 1) * 4)
				 & 0x07) >> 0x01)));
    }

/******************************************************************************
*
* sysVMEIntAck - acknowledge a VMEbus interrupt
*
* This routine acknowledges the VMEbus interrupt by interrupt level and IPL
* number.
*
* RETURNS: The VMEbus interrupt vector.
*
* NOMANUAL
*/

UINT8 sysVMEIntAck
    (
    UINT32 sysIPLNo	/* IPL number on VIC chip */
    )
    {
    UINT8 retVal;	/* status/id to be returned */
    INT32 delay;	/* tmp val for delay */

    switch(sysIPLNo)
	{
	case 0x00:
	    retVal = *VIC_IACK_IPL0;
	    break;
	case 0x01:
	    retVal = *VIC_IACK_IPL1;
	    break;
	case 0x02:
	    retVal = *VIC_IACK_IPL2;
	    break;
	default:
	    retVal = -1;
	}

    for (delay = 0; delay < 16; ++delay)
	;

    vxIPNDClear (1 << sysIPLNo);	/* clear pending bit in IPND reg */

    return ((UINT8)retVal);
    }

/******************************************************************************
*
* sysVicInit - initialize the registers in the vic chip
*
* This initializes the VIC chip. Some of the defines are located in vic068.h
* and some are in vic.h. This function is meant to be called from sysHwInit
* only.
*
* RETURNS: N/A
*/

LOCAL void sysVicInit (void)
    {
    *VIC_VIICR  = VIC_VIICR_INIT;     /* disable bus interrupt */
    *VIC_ICMICR = VIC_ICMS_INIT;      /* disable mailbox interrupts */
    *VIC_ICGICR = VIC_ICGS_INIT;      /* disable mailbox interrupts */
    *VIC_ARCR   = ARCR_VBRL_BR3;      /* bus request level 3 */
    
    /* set the VME and local time outs */
    
    *VIC_TTR    = TTR_VTO_64 | TTR_LTO_64;
    
    *VIC_RCR    = RCR_RWD;            /* release when done */
    *VIC_ICR6   = ICR6_HALT_IRESET;   /* VMEbus HALT or IRESET asserted 6 */
    *VIC_ICR7   = ICR7_SYSFAIL_INH;   /* turn off sysFail */
    
    /* local bus timeout: 32us and VME: 64us */
    
    *VIC_LBTR   = LBTR_PASL_10 | LBTR_PASH_3;
    
    *VIC_ICSR   = 0x0;                  /* clear the mailbox switches */
    *VIC_ICR    = VIC_ICONR_AS_STRETCH; /* stretch Address Strobe */

    *VIC_SS1CR0 = 0x10;         /* A32 D32 */
    *VIC_SS0CR0 = 0x14;		/* A24 D32 */
    *VIC_SS1CR1 = 0x00;
    *VIC_SS0CR1 = 0x00;

    *VIC_ICR6   = 0x40;
    *VIC_ICR7   = 0x80;
    }

/******************************************************************************
*
* sysVicDump - dump the registers in the vic chip
*
* This is the big endian version of vicDump. This was necessary because
* vic068.h uses macro's instead of structures to define the vic interface.
* Sprintf these to RAM, unless pointer is null.
*
* RETURNS: Updated pointer if where is not NULL, otherwise NULL.
*
* NOMANUAL
*/

int sysVicDump
    (
    char * where
    )
    {
#define	VIC        ((struct vicChip *) VIC_BASE_ADRS)

    FAST vicChip * vic = VIC;

    if (where == NULL)
	{
        printf("\nvmeIntIntCntrl=%02x",vic->vmeIntIntCntrl.reg);
	printf("\nvmeIntCntrl[1]=%02x",vic->vmeIntCntrl[0].reg);
	printf("\tvmeIntCntrl[2]=%02x",vic->vmeIntCntrl[1].reg);
	printf("\tvmeIntCntrl[3]=%02x",vic->vmeIntCntrl[2].reg);
	printf("\nvmeIntCntrl[4]=%02x",vic->vmeIntCntrl[3].reg);
	printf("\tvmeIntCntrl[5]=%02x",vic->vmeIntCntrl[4].reg);
	printf("\tvmeIntCntrl[6]=%02x",vic->vmeIntCntrl[5].reg);
	printf("\nvmeIntCntrl[7]=%02x",vic->vmeIntCntrl[6].reg);
	printf("\tdmaIntCntrl=%02x",vic->dmaIntCntrl.reg);
	printf("\nlocIntCntrl[1]=%02x",vic->locIntCntrl[0].reg);
	printf("\tlocIntCntrl[2]=%02x",vic->locIntCntrl[1].reg);
	printf("\tlocIntCntrl[3]=%02x",vic->locIntCntrl[2].reg);
	printf("\nlocIntCntrl[4]=%02x",vic->locIntCntrl[3].reg);
	printf("\tlocIntCntrl[5]=%02x",vic->locIntCntrl[4].reg);
	printf("\tlocIntCntrl[6]=%02x",vic->locIntCntrl[5].reg);
	printf("\nlocIntCntrl[7]=%02x",vic->locIntCntrl[6].reg);
	printf("\nicgsIntCntrl=%02x",vic->icgsIntCntrl.reg);
	printf("\ticmsIntCntrl=%02x",vic->icmsIntCntrl.reg);
	printf("\terrIntCntrl=%02x",vic->errIntCntrl.reg);
	printf("\ticgsVecBase=%02x",vic->icgsVecBase.reg);
	printf("\nicmsVecBase=%02x",vic->icmsVecBase.reg);
	printf("\tlocVecBase=%02x",vic->locVecBase.reg);
	printf("\nerrVecBase=%02x",vic->errVecBase.reg);
	printf("\ticSwitch=%02x",vic->icSwitch.reg);
	printf("\nicr[0]=%02x",vic->icr[0].reg);
	printf("\ticr[1]=%02x",vic->icr[1].reg);
	printf("\ticr[2]=%02x",vic->icr[2].reg);
	printf("\ticr[3]=%02x",vic->icr[3].reg);
	printf("\nicr[4]=%02x",vic->icr[4].reg);
	printf("\ticr[5]=%02x",vic->icr[5].reg);
	printf("\ticr[6]=%02x",vic->icr[6].reg);
	printf("\ticr[7]=%02x",vic->icr[7].reg);
	printf("\nvmeIntReqStat=%02x",vic->vmeIntReqStat.reg);
	printf("\nvmeIntVec[1]=%02x",vic->vmeIntVec[0].reg);
	printf("\tvmeIntVec[2]=%02x",vic->vmeIntVec[1].reg);
	printf("\tvmeIntVec[3]=%02x",vic->vmeIntVec[2].reg);
	printf("\nvmeIntVec[4]=%02x",vic->vmeIntVec[3].reg);
	printf("\tvmeIntVec[5]=%02x",vic->vmeIntVec[4].reg);
	printf("\tvmeIntVec[6]=%02x",vic->vmeIntVec[5].reg);
	printf("\nvmeIntVec[7]=%02x",vic->vmeIntVec[6].reg);
	printf("\ttranTimeOut=%02x",vic->tranTimeOut.reg);
	printf("\tlocBusTiming=%02x",vic->locBusTiming.reg);
	printf("\nvmeConfig0=%02x",vic->vmeConfig0.reg);
	printf("\tvmeConfig1=%02x",vic->vmeConfig1.reg);
	printf("\tarbReqConfig=%02x",vic->arbReqConfig.reg);
	printf("\naddModSrc=%02x",vic->addModSrc.reg);
	printf("\tberrStat=%02x",vic->berrStat.reg);
	printf("\tdmaStat=%02x",vic->dmaStat.reg);
	printf("\nslvSel0Cntrl0=%02x",vic->slvSel0Cntrl0.reg);
	printf("\tslvSel0Cntrl1=%02x",vic->slvSel0Cntrl1.reg);
	printf("\tslvSel1Cntrl0=%02x",vic->slvSel1Cntrl0.reg);
	printf("\nslvSel1Cntrl1=%02x",vic->slvSel1Cntrl1.reg);
	printf("\trelCntrl=%02x",vic->relCntrl.reg);
	printf("\tblkTranCntrl=%02x",vic->blkTranCntrl.reg);
	printf("\nblkTranLen0=%02x",vic->blkTranLen0.reg);
	printf("\tblkTranLen1=%02x",vic->blkTranLen1.reg);
	printf("\tsysReset=%02x",vic->sysReset.reg);
		printf("\n");
	}
    else
	{
	where += sprintf(where," vmeIntIntCntrl=%02x",vic->vmeIntIntCntrl.reg);
	where += sprintf(where," vmeIntCntrl[1]=%02x",vic->vmeIntCntrl[0].reg);
	where += sprintf(where," vmeIntCntrl[2]=%02x",vic->vmeIntCntrl[1].reg);
	where += sprintf(where," vmeIntCntrl[3]=%02x",vic->vmeIntCntrl[2].reg);
	where += sprintf(where," vmeIntCntrl[4]=%02x",vic->vmeIntCntrl[3].reg);
	where += sprintf(where," vmeIntCntrl[5]=%02x",vic->vmeIntCntrl[4].reg);
	where += sprintf(where," vmeIntCntrl[6]=%02x",vic->vmeIntCntrl[5].reg);
	where += sprintf(where," vmeIntCntrl[7]=%02x",vic->vmeIntCntrl[6].reg);
	where += sprintf(where," dmaIntCntrl=%02x",vic->dmaIntCntrl.reg);
	where += sprintf(where," locIntCntrl[1]=%02x",vic->locIntCntrl[0].reg);
	where += sprintf(where," locIntCntrl[2]=%02x",vic->locIntCntrl[1].reg);
	where += sprintf(where," locIntCntrl[3]=%02x",vic->locIntCntrl[2].reg);
	where += sprintf(where," locIntCntrl[4]=%02x",vic->locIntCntrl[3].reg);
	where += sprintf(where," locIntCntrl[5]=%02x",vic->locIntCntrl[4].reg);
	where += sprintf(where," locIntCntrl[6]=%02x",vic->locIntCntrl[5].reg);
	where += sprintf(where," locIntCntrl[7]=%02x",vic->locIntCntrl[6].reg);
	where += sprintf(where," icgsIntCntrl=%02x",vic->icgsIntCntrl.reg);
	where += sprintf(where," icmsIntCntrl=%02x",vic->icmsIntCntrl.reg);
	where += sprintf(where," errIntCntrl=%02x",vic->errIntCntrl.reg);
	where += sprintf(where," icgsVecBase=%02x",vic->icgsVecBase.reg);
	where += sprintf(where," icmsVecBase=%02x",vic->icmsVecBase.reg);
	where += sprintf(where," locVecBase=%02x",vic->locVecBase.reg);
	where += sprintf(where," errVecBase=%02x",vic->errVecBase.reg);
	where += sprintf(where," icSwitch=%02x",vic->icSwitch.reg);
	where += sprintf(where," icr[0]=%02x",vic->icr[0].reg);
	where += sprintf(where," icr[1]=%02x",vic->icr[1].reg);
	where += sprintf(where," icr[2]=%02x",vic->icr[2].reg);
	where += sprintf(where," icr[3]=%02x",vic->icr[3].reg);
	where += sprintf(where," icr[4]=%02x",vic->icr[4].reg);
	where += sprintf(where," icr[5]=%02x",vic->icr[5].reg);
	where += sprintf(where," icr[6]=%02x",vic->icr[6].reg);
	where += sprintf(where," icr[7]=%02x",vic->icr[7].reg);
	where += sprintf(where," vmeIntReqStat=%02x",vic->vmeIntReqStat.reg);
	where += sprintf(where," vmeIntVec[1]=%02x",vic->vmeIntVec[0].reg);
	where += sprintf(where," vmeIntVec[2]=%02x",vic->vmeIntVec[1].reg);
	where += sprintf(where," vmeIntVec[3]=%02x",vic->vmeIntVec[2].reg);
	where += sprintf(where," vmeIntVec[4]=%02x",vic->vmeIntVec[3].reg);
	where += sprintf(where," vmeIntVec[5]=%02x",vic->vmeIntVec[4].reg);
	where += sprintf(where," vmeIntVec[6]=%02x",vic->vmeIntVec[5].reg);
	where += sprintf(where," vmeIntVec[7]=%02x",vic->vmeIntVec[6].reg);
	where += sprintf(where," tranTimeOut=%02x",vic->tranTimeOut.reg);
	where += sprintf(where," locBusTiming=%02x",vic->locBusTiming.reg);
	where += sprintf(where," vmeConfig0=%02x",vic->vmeConfig0.reg);
	where += sprintf(where," vmeConfig1=%02x",vic->vmeConfig1.reg);
	where += sprintf(where," arbReqConfig=%02x",vic->arbReqConfig.reg);
	where += sprintf(where," addModSrc=%02x",vic->addModSrc.reg);
	where += sprintf(where," berrStat=%02x",vic->berrStat.reg);
	where += sprintf(where," dmaStat=%02x",vic->dmaStat.reg);
	where += sprintf(where," slvSel0Cntrl0=%02x",vic->slvSel0Cntrl0.reg);
	where += sprintf(where," slvSel0Cntrl1=%02x",vic->slvSel0Cntrl1.reg);
	where += sprintf(where," slvSel1Cntrl0=%02x",vic->slvSel1Cntrl0.reg);
	where += sprintf(where," slvSel1Cntrl1=%02x",vic->slvSel1Cntrl1.reg);
	where += sprintf(where," relCntrl=%02x",vic->relCntrl.reg);
	where += sprintf(where," blkTranCntrl=%02x",vic->blkTranCntrl.reg);
	where += sprintf(where," blkTranLen0=%02x",vic->blkTranLen0.reg);
	where += sprintf(where," blkTranLen1=%02x",vic->blkTranLen1.reg);
	where += sprintf(where," sysReset=%02x",vic->sysReset.reg);
	}

    return ((int) where);
    }

/******************************************************************************
*
* sysMailboxTblDump - dump the mailbox table contents
*
* RETURNS: N/A
*
* NOMANUAL
*/

void sysMailboxTblDump (void)
    {
    int ix;

    for (ix = 0; ix < VIC_MAILBOX_TOTAL; ix++)
        printf ("routine: %#x  arg: %#x connected: %#x\n",
	    (int) mailTab[ix].sysMailboxRoutine,
	    mailTab[ix].sysMailboxArg,
	    mailTab[ix].sysMailboxConnected);
    }
