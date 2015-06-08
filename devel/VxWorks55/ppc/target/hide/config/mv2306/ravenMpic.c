/* ravenMpic.c - Raven Interrupt Controller driver */

/* Copyright 1984-1998 Wind River Systems, Inc. */
/* Copyright 1996,1997,1998 Motorola, Inc., All Rights Reserved */
#include "copyright_wrs.h"

/*
modification history
--------------------
01t,18jul02,dtr  SPR 30704.
01s,26mar02,dtr  Removing compiler warnings.
01r,24aug98,cjtc windview 2.0 event logging is now single step and handled in
		 the interrupt controller driver. Fixes a problem with out of
		 sequence timestamps in the event log for this architechture.
		 Method of initiating event logging is now via a macro
		 (SPR 21868)
01q,21sep98,ms_  SPR 21961: fix sysIbcPhantomInt to correctly handle
		 phantom interrupts in cascaded interrupt controllers.
                 Remove extra semicolons in sysIbcIntHandler()
01p,17apr98,pr   added windview 20 support
01o,24aug98,mdp  Fixed Phantom IRQ support
01n,14apr98,ms_  merged Motorola mv2700 support
01m,11apr98,scb  HW rework makes ISA IRQ's 14 & 15 edge (not level) sensitive.
01l,22apr98,scb  added phantom IBC int handling for IRQ7 and IRQ15
01k,13jan98,srr  updated copyright dates to 1998.
01j,11dec97,srr  Updated comment in sysIbcInit to show support for VIA chip.
01i,20jul97,srr  send end of interrupt acknowledge to master IBC first.
01h,23oct97,dat, removed IACK loop from sysMpicIntHandler() (SPR 9389).
            mas
01g,20jul97,dat  implemented dave's second revision + some code cleanup
01f,11jun97,dmw  MPIC handler cleanup. 
01e,06may97,dat  added IBC updates from sl82565 controller.
01d,30apr97,dat  added WindView instrumentation SPR 8434
01c,29apr97,dat  Fixed Motorola MR #62, moved MPIC_EOI
01b,23apr97,mas  merged with sl82565IntrCtl.c (SPR 8170).
01a,11nov96,rhk  created by Motorola.
*/

/*
DESCRIPTION
This module implements the Raven Mpic and the Winbond W83C553 PCI-ISA Bridge
(PIB) drivers.  (The W83C553 PIB was formerly known as the Motorola sl82565 ISA
Bridge Controller (IBC).

These merged drivers provide support for initializing their respective devices
and handling interrupts from them.
*/


/* includes */

#include "ravenMpic.h"
#include "private/eventP.h"


/* defines */

#define	MPIC_EOI	sysPciWrite32(MPIC_ADDR(MPIC_CPU0_EOI_REG),0)
#define IBC_INT_PENDING	0x80

/* globals */

IMPORT STATUS	      (*_func_intConnectRtn) (VOIDFUNCPTR *, VOIDFUNCPTR, int);
IMPORT int	      (*_func_intEnableRtn) (int);
IMPORT int	      (*_func_intDisableRtn) (int);
IMPORT void		sysOutByte (ULONG, UCHAR);
IMPORT UCHAR		sysInByte (ULONG);
IMPORT void		sysPciRead32 (UINT32, UINT32 *);
IMPORT void		sysPciWrite32 (UINT32, UINT32);
IMPORT STATUS		excIntConnect (VOIDFUNCPTR *, VOIDFUNCPTR);
void			sysIbcIntEnable (int);
void			sysIbcIntDisable (int);
void			sysIbcIntHandler (void);
void			sysMpicIntHandler (void);

IMPORT UINT		sysVectorIRQ0; 	/* vector for IRQ0 */
INT_HANDLER_DESC * sysIntTbl [256];	/* system interrupt table */
int tpr;

/* forward declarations */

LOCAL int       getMpicVecOffset (int);
LOCAL STATUS	sysMpicIntConnect (VOIDFUNCPTR * vector, VOIDFUNCPTR routine,
			       int parameter);
LOCAL int	sysMpicIntEnable (int);
LOCAL int	sysMpicIntDisable (int);

LOCAL void	sysIbcMpicConnect (void);
LOCAL BOOL 	sysIbcPhantomInt (UCHAR *intNum, int lvl7Int, int lvl15Int);
LOCAL void	sysIbcEndOfInt (int);
LOCAL void	sysIbcIntLevelSet (int);

/* Mask values are the currently disabled sources */

LOCAL UINT8	sysPicMask1 = 0xfb;	/* all levels disabled */
LOCAL UINT8	sysPicMask2 = 0xff;

/* Level values are the interrupt level masks */

LOCAL UINT8	sysPicLevel1;
LOCAL UINT8	sysPicLevel2;
LOCAL UINT8	sysPicLevelCur;		/* current priority level, 0 to 16 */

/* level values by real priority */

LOCAL UCHAR sysPicPriMask1[17] = {0xFB,0xFA,0xF8,0xF8,0xF0,0xE0,0xC0,0x80,
			     0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0x0};
LOCAL UCHAR sysPicPriMask2[17] = {0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,
			     0xFF,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80,0x0};


/* Hardware access methods */

#ifndef IBC_BYTE_OUT
#   define IBC_BYTE_OUT(reg,data) \
	(sysOutByte (reg,data))
#endif

#ifndef IBC_BYTE_IN
#   define IBC_BYTE_IN(reg,pData) \
	(*pData = sysInByte(reg))
#endif

#ifndef CPU_INT_LOCK
#   define CPU_INT_LOCK(x) \
	(*x = intLock ())
#endif

#ifndef CPU_INT_UNLOCK
#   define CPU_INT_UNLOCK(data) \
	(intUnlock (data))
#endif

/*******************************************************************************
*
* sysMpicInit - initialize the MPIC in the Raven
*
* This function initializes the Multi-Processor Interrupt Controller (MPIC)
* contained in the Raven chip.
*
* It first initializes the system vector table, connects the MPIC interrupt
* handler to the PPC external interrupt and attaches the local MPIC routines
* for interrupt connecting, enabling and disabling to the corresponding system
* routine pointers.
*
* It then initializes the MPIC registers, clears any pending MPIC interrupts,
* enables interrupt handling by the MPIC and enables external ISA interrupts
* (from the W83C553).
*
* RETURNS: OK always
*/
STATUS sysMpicInit (void)
    {
    int			i;
    UINT32		timerReg;
    UINT32		ipiReg;
    UINT32		destReg;
    LOCAL_INT_DATA	init;


    /* Initialize the interrupt table */

    for (i = 0; i < 256; i++)
        sysIntTbl[i] = NULL;
 
    /* Connect the interrupt demultiplexer to the PowerPC external interrupt */

    excIntConnect ((VOIDFUNCPTR *) _EXC_OFF_INTR, sysMpicIntHandler);

    /*
     *  Set up the BSP specific routines
     *  Attach the local routines to the vxWorks system calls
     */

    _func_intConnectRtn = sysMpicIntConnect;
    _func_intEnableRtn = sysMpicIntEnable;
    _func_intDisableRtn = sysMpicIntDisable;

    /* Initialize the MPIC */

    /* generate a PCI IACK to clear any pending interrupts */

    sysPciRead32( MPIC_ADDR(MPIC_CPU0_IACK_REG), (UINT32*)&init.temp );

    /* inhibit the timer and IPI regs. and clear the counter regs. */

    timerReg = MPIC_TIMER0_BASE_CT_REG;
    ipiReg = MPIC_IPI0_VEC_PRI_REG;
    for (i=0; i<4; i++)
	{
	sysPciWrite32( MPIC_ADDR(ipiReg), IPI_INHIBIT );
	ipiReg += REG_OFFSET;
	sysPciWrite32( MPIC_ADDR(timerReg), TIMER_INHIBIT );
	timerReg += REG_OFFSET;
	sysPciWrite32( MPIC_ADDR(timerReg), TIMER_INHIBIT );
	timerReg += (REG_OFFSET * 3);
	}

    /* setup the external source vector/priority registers */

    sysPciWrite32( MPIC_ADDR(MPIC_EXT_SRC0_VEC_PRI_REG), INIT_EXT_SRC0 );
    sysPciWrite32( MPIC_ADDR(MPIC_EXT_SRC1_VEC_PRI_REG), INIT_EXT_SRC1 );
    sysPciWrite32( MPIC_ADDR(MPIC_EXT_SRC2_VEC_PRI_REG), INIT_EXT_SRC2 );
    sysPciWrite32( MPIC_ADDR(MPIC_EXT_SRC3_VEC_PRI_REG), INIT_EXT_SRC3 );
    sysPciWrite32( MPIC_ADDR(MPIC_EXT_SRC4_VEC_PRI_REG), INIT_EXT_SRC4 );
    sysPciWrite32( MPIC_ADDR(MPIC_EXT_SRC5_VEC_PRI_REG), INIT_EXT_SRC5 );
    sysPciWrite32( MPIC_ADDR(MPIC_EXT_SRC6_VEC_PRI_REG), INIT_EXT_SRC6 );
    sysPciWrite32( MPIC_ADDR(MPIC_EXT_SRC7_VEC_PRI_REG), INIT_EXT_SRC7 );
    sysPciWrite32( MPIC_ADDR(MPIC_EXT_SRC8_VEC_PRI_REG), INIT_EXT_SRC8 );
    sysPciWrite32( MPIC_ADDR(MPIC_EXT_SRC9_VEC_PRI_REG), INIT_EXT_SRC9 );
    sysPciWrite32( MPIC_ADDR(MPIC_EXT_SRC10_VEC_PRI_REG), INIT_EXT_SRC10 );
    sysPciWrite32( MPIC_ADDR(MPIC_EXT_SRC11_VEC_PRI_REG), INIT_EXT_SRC11 );
    sysPciWrite32( MPIC_ADDR(MPIC_EXT_SRC12_VEC_PRI_REG), INIT_EXT_SRC12 );
    sysPciWrite32( MPIC_ADDR(MPIC_EXT_SRC13_VEC_PRI_REG), INIT_EXT_SRC13 );
    sysPciWrite32( MPIC_ADDR(MPIC_EXT_SRC14_VEC_PRI_REG), INIT_EXT_SRC14 );
    sysPciWrite32( MPIC_ADDR(MPIC_EXT_SRC15_VEC_PRI_REG), INIT_EXT_SRC15 );

    destReg = MPIC_EXT_SRC0_DEST_REG;
    for (i=0; i<=15; i++)
	{
	sysPciWrite32( MPIC_ADDR(destReg), DESTINATION_CPU0 );
	destReg += (REG_OFFSET*2);
	}

    /* setup the Ext source 0 reg (8259 input) for normal operation */

    sysPciRead32( MPIC_ADDR(MPIC_EXT_SRC0_VEC_PRI_REG), &init.regVal );
    init.regVal |= PIB_INT_VEC;
    init.regVal &= (~INT_MASK_BIT);
    sysPciWrite32( MPIC_ADDR(MPIC_EXT_SRC0_VEC_PRI_REG), init.regVal );

    /* enable interrupts for this processor */

    sysPciWrite32( MPIC_ADDR(MPIC_CPU0_CUR_TASK_PRI_REG), 1 );

    /* setup the MPIC to process the 8259 interrupts ( mixed mode ) */

    sysPciWrite32( MPIC_ADDR(MPIC_GLOBAL_CONFIG_REG), SET_MIXED_MODE );

    return (OK);
    }

/******************************************************************************
*
* sysMpicIntConnect - connect an interrupt handler to the system vector table
*
* This function connects an interrupt handler to the system vector table.
*
* RETURNS: OK/ERROR.
*/
 
LOCAL STATUS sysMpicIntConnect
    (
     VOIDFUNCPTR *      vector,         /* interrupt vector to attach */
     VOIDFUNCPTR        routine,        /* routine to be called */
     int                parameter       /* parameter to be passed to routine */
    )
    {
    INT_HANDLER_DESC * newHandler;
    INT_HANDLER_DESC * currHandler;
    LOCAL_INT_DATA     connect;
    static BOOL	       firstTime = TRUE;

    if (((int)vector < 0) || ((int)vector > 0xff))      /* Out of Range? */
        return (ERROR);
 
    if (firstTime)
	{

	/* connect the PIB to MPIC, before any other connections */

	firstTime = FALSE;
	sysIbcMpicConnect (); /* calls this rtn, recursively */
	}

    /* create a new interrupt handler */

    if ((newHandler = (INT_HANDLER_DESC *)calloc (1, sizeof (INT_HANDLER_DESC)))
	 == NULL)
        return (ERROR);
 
    /* initialize the new handler */

    newHandler->vec  = routine;
    newHandler->arg  = parameter;
    newHandler->next = NULL;

    /* install the handler in the system interrupt table */

    if (sysIntTbl[(int) vector] == NULL)
        sysIntTbl [(int ) vector] = newHandler; /* single int. handler case */
    else
        {
        currHandler = sysIntTbl[(int) vector]; /* multiple int. handler case */
        while (currHandler->next != NULL)
            {
            currHandler = currHandler->next;
            }
        currHandler->next = newHandler;
        }

    /* 
     * if the connect is for an MPIC interrupt, 
     * then store the vector into the appropriate MPIC vector register
     */

    connect.regAddr = getMpicVecOffset( (int)vector );
    if ( connect.regAddr > 0 )
        {
        /* read the vector register */

        sysPciRead32( MPIC_ADDR(connect.regAddr), &connect.regVal );

        /* store the interrupt vector number */

        connect.regVal |= (int)vector;

        /* write the contents of the vector register back */

        sysPciWrite32( MPIC_ADDR(connect.regAddr), connect.regVal );
        }

    return (OK);
    }
 

/*******************************************************************************
*
* sysMpicIntEnable - enable an Mpic interrupt level
*
* This routine enables a specified Mpic interrupt level.
*
* RETURNS: OK or ERROR if interrupt level not supported
*/
 
LOCAL int sysMpicIntEnable
    (
    int intLevel        /* interrupt level to enable */
    )
    {
    LOCAL_INT_DATA	enable;

    /*
     * if the int. level is not for ISA or MPIC, then it is not supported.
     * If not supported, just return.
     */

    if ((intLevel < 0) || (intLevel > ERR_INTERRUPT_BASE))
        return (ERROR);

    /* If ISA interrupt level,call the IBC driver enable routine, */

    if ( intLevel < EXT_INTERRUPT_BASE )
        sysIbcIntEnable( intLevel );

    enable.regAddr = getMpicVecOffset( intLevel );

    if ( enable.regAddr > 0 )
        {
        /* read the vector register */

        sysPciRead32( MPIC_ADDR(enable.regAddr), &enable.regVal );

        /* enable the interrupt */

        enable.regVal &= (~INT_MASK_BIT);

        /* write the contents of the vector register back */

        sysPciWrite32( MPIC_ADDR(enable.regAddr), enable.regVal );
        }

    return (OK);
    }


/*******************************************************************************
*
* sysMpicIntDisable - disable an Mpic interrupt level
*
* This routine disables a specified Mpic interrupt level.
*
* RETURNS: OK or ERROR if interrupt level not supported
*/
 
LOCAL int sysMpicIntDisable
    (
    int intLevel        /* interrupt level to disable */
    )
    {
    LOCAL_INT_DATA      disable;
 
    /*
     * if the int. level is not for ISA or MPIC, then it is not supported.
     * If not supported, just return.
     */

    if ((intLevel < 0) || (intLevel > ERR_INTERRUPT_BASE))
        return (ERROR);

    /* If ISA interrupt level, call the IBC driver disable routine, */

    if ( intLevel < EXT_INTERRUPT_BASE )
        sysIbcIntDisable( intLevel );
 
    /* else, it is an MPIC interrupt level */
    else
        {
        /* get the vector reg. offset value */
        disable.regAddr = getMpicVecOffset( intLevel );
 
        if ( disable.regAddr > 0 )
            {
            /* read the vector register */

            sysPciRead32( MPIC_ADDR(disable.regAddr), &disable.regVal );

            /* disable the interrupt */

            disable.regVal |= INT_MASK_BIT;

            /* write the contents of the vector register back */

            sysPciWrite32( MPIC_ADDR(disable.regAddr), disable.regVal );
            }
        }

    return (OK);
    }


/******************************************************************************
*
* sysMpicIntHandler - handle an interrupt received at the Mpic
* 
* This routine will process interrupts received from PCI or ISA devices as
* these interrupts arrive via the MPIC.  This routine supports MPIC interrupt
* nesting.
*
* RETURNS: N/A
*/

void sysMpicIntHandler (void)
    {
    INT_HANDLER_DESC *  currHandler;
    UINT32		vecNum;
    int			dontCare;

    /* get the vector from the MPIC IACK reg. */

    sysPciRead32 (MPIC_ADDR (MPIC_CPU0_IACK_REG), &vecNum);
    vecNum &= VECTOR_MASK;

    /* Ignore spurious interrupts */

    if (vecNum == 0xFF)
        {
        logMsg ("MPIC Spurious Interrupt!\n", 0,0,0,0,0,0);
        return;
        }
 
    /*
     * Allow maskable interrupts to the CPU.  MPIC will hold off
     * lower and equal interrupts until MPIC_EOI is performed.
     */

    CPU_INT_UNLOCK(_PPC_MSR_EE);

#ifdef INCLUDE_WINDVIEW
    WV_EVT_INT_ENT(vecNum)
#endif

    /* call the necessary interrupt handlers */

    if ((currHandler = sysIntTbl [vecNum]) == NULL)
        {
        logMsg ("uninitialized MPIC interrupt %d\r\n", vecNum, 0,0,0,0,0);
        }
    else
        {
        /* Call EACH respective chained interrupt handler */

        while (currHandler != NULL)
            {
            currHandler->vec (currHandler->arg);
            currHandler = currHandler->next;
            }
        }

    /* issue an end-of-interrupt to the MPIC */

    CPU_INT_LOCK (&dontCare);

    MPIC_EOI;

    return;
    }


/*******************************************************************************
*
* getMpicVecOffset - get the vector offset value of an MPIC register 
*
* This routine calculates the appropriate MPIC register offset based on the
* specified MPIC interrupt level.
*
* RETURNS: MPIC register offset or zero if not a supported level.
*/
 
LOCAL int getMpicVecOffset
    (
    int		intLevel
    ) 
    {
    int 	offset = 0;

    /* check for external interrupt level */

    if ((intLevel >= EXT_INTERRUPT_BASE) && (intLevel < TIMER_INTERRUPT_BASE))
        {
        offset = intLevel - EXT_INTERRUPT_BASE;
        offset = MPIC_EXT_SRC0_VEC_PRI_REG + ( offset * REG_OFFSET * 2 );
        }
 
    /* check for a timer interrupt level */

    if ((intLevel >= TIMER_INTERRUPT_BASE) && (intLevel < IPI_INTERRUPT_BASE))
        {
        offset = intLevel - TIMER_INTERRUPT_BASE;
        offset =  MPIC_TIMER0_VEC_PRI_REG + ( offset * REG_OFFSET * 4 );
        }
 
    /* check for an IPI interrupt level */

    if ((intLevel >= IPI_INTERRUPT_BASE) && (intLevel < ERR_INTERRUPT_BASE))
        {
        offset = intLevel - IPI_INTERRUPT_BASE;
        offset =  MPIC_IPI0_VEC_PRI_REG + ( offset * REG_OFFSET );
        }
    /* check for the Raven internal error interrupt */

    if (intLevel == ERR_INTERRUPT_BASE )
        {
        offset = MPIC_ERR_VEC_PRI_REG;
        }

    return (offset);
    }


/*******************************************************************************
*
* sysIbcInit - Initialize the IBC
*
* This routine initializes the non-PCI Header configuration registers of the
* IBC within the W83C553 PIB.
* This is also compatible for the PBC within the VT82C586B PIPC.
*
* RETURNS: OK always
*/

STATUS sysIbcInit (void)
    {
    UCHAR	intVec;
    UCHAR	temp;


    /* Initialize the Interrupt Controller #1 */

    IBC_BYTE_OUT (PIC_port1 (PIC1_BASE_ADR),0x11);        /* ICW1 */
    IBC_BYTE_OUT (PIC_port2 (PIC1_BASE_ADR),sysVectorIRQ0); /* ICW2 */
    IBC_BYTE_OUT (PIC_port2 (PIC1_BASE_ADR),0x04);	/* ICW3 */
    IBC_BYTE_OUT (PIC_port2 (PIC1_BASE_ADR),0x01);	/* ICW4 */

    /*
     *	Mask interrupts IRQ 0, 1, and 3-7 by writing to OCW1 register
     *	IRQ 2 is the cascade input
     */

    IBC_BYTE_OUT (PIC_IMASK (PIC1_BASE_ADR),0xfb);

    /* Make IRQ 5 level sensitive */

    IBC_BYTE_OUT (SL82565_INT1_ELC, 0x20);

    /* Initialize the Interrupt Controller #2 */

    IBC_BYTE_OUT (PIC_port1 (PIC2_BASE_ADR),0x11);	/* ICW1 */
    IBC_BYTE_OUT (PIC_port2 (PIC2_BASE_ADR),sysVectorIRQ0+8); /* ICW2 */
    IBC_BYTE_OUT (PIC_port2 (PIC2_BASE_ADR),0x02);	/* ICW3 */
    IBC_BYTE_OUT (PIC_port2 (PIC2_BASE_ADR),0x01);	/* ICW4 */

    /* Mask interrupts IRQ 8-15 by writing to OCW1 register */

    IBC_BYTE_OUT (PIC_IMASK (PIC2_BASE_ADR),0xff);

    /* Make IRQ 15, 14, 11, 10, and 9 level sensitive */

    temp = ( 0x80 | 0x40 |  0x08 | 0x04 | 0x02 );

#ifdef MCP750
    /*
     * Note: Reworked MCP750s have ISA IRQ's 14 and 15 edge sensitive 
     * instead of level sensitive like the other PowerPlus boards.
     */
    if (EXTENDED_FEATURE_PRESENT(MCP750_ISA_INT_REWORK))
	{
        /* Make IRQ 11, 10, and 9 level sensitive */

        temp =  0x08 | 0x04 | 0x02;
	}
#endif

    IBC_BYTE_OUT (SL82565_INT2_ELC, temp);

    /* Permanently turn off ISA refresh by never completing init steps */

    IBC_BYTE_OUT (SL82565_TMR1_CMOD, 0x74);

    /*	Perform the PCI Interrupt Ack cycle */

    IBC_BYTE_IN (RAVEN_BASE_ADRS + 0x30, &intVec);

    /* Perform the end of interrupt procedure */

    sysIbcEndOfInt (15);

    sysIbcIntLevelSet (16);

    return (OK);
    }


/*******************************************************************************
*
* sysIbcIntEnable - enable a IBC interrupt level
*
* This routine enables a specified IBC interrupt level.
*
* RETURNS: N/A
*/

void sysIbcIntEnable
    (
    int intNum        /* interrupt level to enable */
    )
    {
    if (intNum < 8)
	{
	sysPicMask1 &= ~(1 << intNum);
	IBC_BYTE_OUT (PIC_IMASK (PIC1_BASE_ADR), sysPicMask1 | sysPicLevel1);
	}
    else
	{
	sysPicMask2 &= ~(1 << (intNum - 8));
	IBC_BYTE_OUT (PIC_IMASK (PIC2_BASE_ADR), sysPicMask2 | sysPicLevel2);
	}
    }


/*******************************************************************************
*
* sysIbcIntDisable - disable a IBC interrupt level
*
* This routine disables a specified IBC interrupt level.
*
* RETURNS: N/A
*/

void sysIbcIntDisable
    (
    int intNum        /* interrupt level to disable */
    )
    {
    if (intNum < 8)
	{
	sysPicMask1 |= (1 << intNum);
	IBC_BYTE_OUT (PIC_IMASK (PIC1_BASE_ADR), sysPicMask1 | sysPicLevel1 );
	}
    else
	{
	sysPicMask2 |= (1 << (intNum - 8));
	IBC_BYTE_OUT (PIC_IMASK (PIC2_BASE_ADR), sysPicMask2 | sysPicLevel2);
	}
    }

/******************************************************************************
*
* sysIbcIntHandler - handler of the sl82565 IBC interrupt.
*
* This routine handles interrupts originating from the W83C553 PIB ISA Bus
* Controller (IBC).  This device implements the functional equivalent of two
* cascaded 8259 PICs.
*
* This routine is entered with CPU external interrupts enabled.
*
* Because the ISA bus is only accessible via the PCI bus, this driver first
* initiates a PCI interrupt acknowledge cycle to read the interrupt number
* (vector) coming from the IBC.
*
* This routine then processes the interrupt by calling all interrupt service
* routines chained to the vector.
*
* Finally, this routine re-arms the interrupt at the IBC by performing a
* IBC EOI.
*
* RETURNS: N/A
*/

void sysIbcIntHandler (void)
    {
    UCHAR		intNum;
    INT_HANDLER_DESC *	currHandler;
    static int		lvl7Int = 0;
    static int		lvl15Int = 0;

    IBC_BYTE_IN (RAVEN_BASE_ADRS + 0x30, &intNum);

    if ((intNum & 7) == 7)	/* Special check for phantom interrupts */
	{
        if (sysIbcPhantomInt (&intNum, lvl7Int, lvl15Int) == TRUE)
	    return;			/* It's phantom so just return. */
	}
    /* Check for cascade from IBC2.  If so get IBC2 interrupt.  */

    /*
     * If cascade from IBC2 just EIO and return. It has been seen on 
     * certain PCI ISA bridge devices, that it is possible to see the
     * cascade IRQ2.  In particular, this has been observed on the 
     * Winbond W83C553F on certain revs of the chip prior to Rev G.
     */

    if (intNum == 0x02) 
        {
        sysIbcEndOfInt (intNum);
        return;
        }

    /* Keep track of level 7 and 15 nesting for phantom interrupt handling */

    if (intNum == 7)
        lvl7Int++;
    else if (intNum == 15)
	lvl15Int++;

    if ((currHandler = sysIntTbl [intNum]) == NULL)
	{
	logMsg ("uninitialized IBC interrupt level %x\r\n", intNum, 0,0,0,0,0);
	}
    else
	{
	/* Call EACH respective chained interrupt handler */

	while (currHandler != NULL)
	    {
	    currHandler->vec (currHandler->arg);
	    currHandler = currHandler->next;
	    }
	}

    /* Keep track of level 7 and 15 nesting for phantom interrupt handling */

    if (intNum == 7)
        lvl7Int--;
    else if (intNum == 15)
	lvl15Int--;

    /* Re-arm (enable) the interrupt on the IBC */

    sysIbcEndOfInt (intNum);
    }

/*******************************************************************************
*
* sysIbcPhantomInt - Determine if IRQ interrupt number 7 or 15 is "phantom".
* 
* This routine determines if an IRQ number of 7 or 15 is a phantom
* interrupt.  According to Intel 82C59A-2 documentation, the IR (interrupt
* request) inputs must remain high until after the falling edge of the first
* INTA (interrupt acknowledge).  If the IR goes low before this, a DEFAULT
* (phantom) IRQ7 will occur when the CPU acknowledges the interrupt.  Note
* that if an IRQ7 is generated it may really be another interrupt, IRQ4 for
* example.  IRQ 7 is associated  with the master 8259, IRQ 15 is associated 
* with the slave 8259.  This function should only be called if the 
* acknowledged IRQ number is 7 or 15 but does behave sanely if called with 
* other IRQ numbers.
*
* As mentioned above, IRQ 7 is supposed to be associated with the master
* 8259. However, it has been observed that a phantom IRQ 7 was caused by
* an interrupt on the slave 8259. Thus, the algorithm now implemented is
* to scan both the master and the slave 8259 in the correct priority order
* (from highest to lowest: 0 1 8 9 10 11 12 13 14 15 3 4 5 6 7).
*
* RETURNS: TRUE if phantom IRQ, *intNum unaltered.
* FALSE if no phantom interrupt, *intNum is "real" IRQ number.
*/

LOCAL BOOL sysIbcPhantomInt
    (
    UCHAR *intNum,      /* interrupt number received on acknowledge */
    int	  lvl7Int,	/* interrupt 7 nesting level */
    int	  lvl15Int 	/* interrupt 15 nesting level */
    )
    {
    UCHAR irqBit;
    UINT  irqNum;

    /* Read the master in-service register (ISR) */

    IBC_BYTE_OUT (PIC_port1 (PIC1_BASE_ADR), PIC_OCW3_SEL + PIC_ISR_READ);

    IBC_BYTE_IN (PIC_port1 (PIC1_BASE_ADR), &irqBit);

    if (irqBit == 0)    /* no interrupt bit is set */
	return (TRUE);	/* No in-service int so it MUST be phantom */

    for (irqNum = 0; ((irqBit & 1) == 0) ; irqNum++, irqBit >>= 1)
	;

    if (irqNum == 7)
	if (lvl7Int > 1)
	    return (TRUE);  /* We're nested so it MUST be phantom */

    /* if irqNum is 2 the interrupt is on the slave */

    if (irqNum != 2)
	{
	*intNum = irqNum;   /* adjust the interrupt number to the true one */
	return (FALSE);
	}

    /* Read the slave in-service register (ISR) */

    IBC_BYTE_OUT (PIC_port1 (PIC2_BASE_ADR), PIC_OCW3_SEL + PIC_ISR_READ);

    IBC_BYTE_IN (PIC_port1 (PIC2_BASE_ADR), &irqBit);

    if (irqBit == 0)	/* should never happen, really */
	return (TRUE);	/* No in-service int so it MUST be phantom */

    for (irqNum = 8; ((irqBit & 1) == 0) ; irqNum++, irqBit >>= 1)
	;

    if (irqNum == 15)
	if (lvl15Int > 1)
	    return (TRUE);  /* We're nested so it MUST be phantom */

    *intNum = irqNum;   /* adjust the interrupt number to the true one */
    return (FALSE);

    }

/*******************************************************************************
*
* sysIbcEndOfInt - send EOI (end of interrupt) signal.
*
* This routine is called at the end of the interrupt handler to
* send a non-specific end of interrupt (EOI) signal.
*
* The second PIC is acked only if the interrupt came from that PIC.
* The first PIC is always acked.
*/

LOCAL void sysIbcEndOfInt
    (
    int intNum
    )
    {
    if (intNum > 7)
	{
	IBC_BYTE_OUT (PIC_IACK (PIC2_BASE_ADR), 0x20);
	}
    IBC_BYTE_OUT (PIC_IACK (PIC1_BASE_ADR), 0x20);
    }

/*******************************************************************************
*
* sysIbcIntLevelSet - set the interrupt priority level
*
* This routine masks interrupts with real priority equal to or lower than
* <intNum>.  The special
* value 16 indicates all interrupts are enabled. Individual interrupt
* numbers have to be specifically enabled by sysIbcIntEnable() before they
* are ever enabled by setting the interrupt level value.
*
* Note because of the IBM cascade scheme, the actual priority order for
* interrupt numbers is (high to low) 0, 1, 8, 9, 10, 11, 12, 13, 14, 15,
* 3, 4, 5, 6, 7, 16 (all enabled)
*
* INTERNAL: It is possible that we need to determine if we are raising
* or lowering our priority level.  It may be that the order of loading the
* two mask registers is dependent upon raising or lowering the priority.
*
* RETURNS: N/A
*/

LOCAL void sysIbcIntLevelSet
    (
    int intNum	/* interrupt level to implement */
    )
    {
    if (intNum > 16)
	intNum = 16;

    sysPicLevelCur = intNum;

    if (sysPicLevel2 != sysPicPriMask2[intNum])
	{
	sysPicLevel2 = sysPicPriMask2[intNum];
	IBC_BYTE_OUT (PIC_IMASK (PIC2_BASE_ADR), sysPicMask2 | sysPicLevel2);
	}

    if (sysPicLevel1 != sysPicPriMask1[intNum])
	{
	sysPicLevel1 = sysPicPriMask1[intNum];
	IBC_BYTE_OUT (PIC_IMASK (PIC1_BASE_ADR), sysPicMask1 | sysPicLevel1);
	}
    }

/*******************************************************************************
*
* sysIbcMpicConnect - routine to connect IBC interrupts to MPIC
*
* This function is called from sysHwInit2 and sets the IBC interrupt
* handler into the MPIC interrupt vector table.
*
* RETURNS: N/A
*/

LOCAL void sysIbcMpicConnect (void)
    {
    intConnect (INUM_TO_IVEC(PIB_INT_VEC), sysIbcIntHandler, 0); 
    intEnable (PIB_INT_LVL);
    }
