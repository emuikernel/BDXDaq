/* hawkMpic.c - Hawk Interrupt Controller driver */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1996-2001 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01f,15sep01,kab  fix for SPR 69439: MPIC Spurious Int
01e,07mar01,rhk  Wind River coding standards changes.
01d,16aug00,dmw  Wind River coding standards review.
01c,07jun00,dmw  Update to PrPMC name convention.
01b,03may00,dmw  Changed 8259 level/edge settings to match IPMC761.
01a,13mar00,dmw  Written (from version 01c of mcpn765/hawkMpic.c).
*/

/*
DESCRIPTION
This module implements the Hawk MPIC and the Intel 8259 compatable (IBC) ISA 
interrupt controllers.

This driver provides support for initializing the Hawk MPIC and handling
its interrupts.

The MPIC (Multi-Processor Iterrupt Controller) is an Open Peripheral Interrupt
Controller (OPIC) which is contained within the Hawk PCI Host Bridge ASIC. The
MPIC has the following general features.

.CS
  - Support for up to two processors.
  - Supports 16 external interrupt sources.
  - Supports 15 programmable Interrupt and Procesor Task priority levels.
  - Supports the connection of an external 8259 for ISA/AT compatibility.
  - Supports distributed delivery for external I/O interrupts.
  - Direct/Multicast interrupt delivery for Interprocessor and timer interrupts.
  - Four Inter-processor Interrupt sources.
  - Four Timers (See hawkAuxClk.c for additional information).
.CE

NOTE: The Raven MPIC was clocked using the 60x bus clock. The Hawk MPIC is
clocked using the PCI bus clock. This change can exacerbate spurious interrupts
caused by late-clearing device interrupts. It is recommended that all device
interrupt service routines use a PCI read operation as the final PCI transaction
in the interrupt service routine. The final read operation will flush any PCI
write posting FIFOs between the processor and the interrupting device. The flush
operation will guarantee that all interrupt clearing writes have been forwarded
to the interrupting device before the interrupt service code returns. Without
this flushing operation, the interrupt routine can terminate and return before
the interrupting device has withdrawn its interrupt line from the MPIC. If the
MPIC is re-enabled at this point, a spurious interrupt can be generated due to
the late clearing of the PCI interrupt.

*/


/* includes */

#include "hawkMpic.h"
#include "private/eventP.h"

/* defines */
#define	MPIC_PCI_FLUSH(x)  sysPciRead32(MPIC_ADDR(MPIC_VENDOR_ID_REG),x)
#define	MPIC_EOI        sysPciWrite32(MPIC_ADDR(MPIC_CPU0_EOI_REG),0)

#define IBC_INT_PENDING 0x80
#define IBC_INT_LVL2    2
#define IBC_INT_LVL7    7
#define IBC_INT_LVL15   15

/* external declarations */

IMPORT STATUS (*_func_intConnectRtn) (VOIDFUNCPTR *, VOIDFUNCPTR, int);
IMPORT int    (*_func_intEnableRtn) (int);
IMPORT int    (*_func_intDisableRtn) (int);
IMPORT void   sysOutByte (ULONG, UCHAR);
IMPORT UCHAR  sysInByte (ULONG);
IMPORT void   sysPciRead32 (UINT32, UINT32 *);
IMPORT void   sysPciWrite32 (UINT32, UINT32);
IMPORT STATUS excIntConnect (VOIDFUNCPTR *, VOIDFUNCPTR);
IMPORT UINT   sysVectorIRQ0; 	/* vector for IRQ0 */

/* global declarations */

INT_HANDLER_DESC * sysIntTbl [256];	/* system interrupt table */
int tpr;

/* forward declarations */

void          sysIbcIntEnable (int);
void          sysIbcIntDisable (int);
void          sysIbcIntHandler (void);
void          sysMpicIntHandler (void);

LOCAL  int    getMpicVecOffset (int);
LOCAL  STATUS sysMpicIntConnect (VOIDFUNCPTR * vector, 
                                 VOIDFUNCPTR routine,
                                 int parameter);
LOCAL int     sysMpicIntEnable (int);
LOCAL int     sysMpicIntDisable (int);

LOCAL void    sysIbcMpicConnect (void);
LOCAL UCHAR   sysIbcPhantomInt (UCHAR *intNum, int lvl7Int, int lvl15Int);
LOCAL void    sysIbcEndOfInt (int);
LOCAL void    sysIbcIntLevelSet (int);

/* Mask values are the currently disabled sources */

LOCAL UINT8   sysPicMask1 = 0xfb;	/* all levels disabled */
LOCAL UINT8   sysPicMask2 = 0xff;

/* Level values are the interrupt level masks */

LOCAL UINT8   sysPicLevel1;
LOCAL UINT8   sysPicLevel2;
LOCAL UINT8   sysPicLevelCur;		/* current priority level, 0 to 16 */

/* level values by real priority */

LOCAL UCHAR   sysPicPriMask1[17] = 
                  {0xFB,0xFA,0xF8,0xF8,0xF0,0xE0,0xC0,0x80,
			       0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0xF8,0x0};
LOCAL UCHAR   sysPicPriMask2[17] = 
                  {0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,
			       0xFF,0xFE,0xFC,0xF8,0xF0,0xE0,0xC0,0x80,0x0};


/* Hardware access methods */

#ifndef IBC_BYTE_OUT
#   define IBC_BYTE_OUT(reg,data) \
	(sysOutByte (reg,data))
#endif /* IBC_BYTE_OUT */

#ifndef IBC_BYTE_IN
#   define IBC_BYTE_IN(reg,pData) \
	(*pData = sysInByte(reg))
#endif /* IBC_BYTE_IN */

#ifndef CPU_INT_LOCK
#   define CPU_INT_LOCK(x) \
	(*x = intLock ())
#endif /* CPU_INT_LOCK */

#ifndef CPU_INT_UNLOCK
#   define CPU_INT_UNLOCK(data) \
	(intUnlock (data))
#endif /* CPU_INT_UNLOCK */


/*******************************************************************************
*
* sysMpicInit - initialize the MPIC in the Hawk
*
* This function initializes the Multi-Processor Interrupt Controller (MPIC)
* contained in the Hawk chip.
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

    /* 
     * Initialize the MPIC
     *
     * generate a PCI IACK to clear any pending interrupts
     */

    sysPciRead32( MPIC_ADDR(MPIC_CPU0_IACK_REG), &init.temp );

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
 

/******************************************************************************
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


/******************************************************************************
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
    UINT32		dummy;
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

        /* Call each respective chained interrupt handler */

        while (currHandler != NULL)
            {
            currHandler->vec (currHandler->arg);
            currHandler = currHandler->next;
            }
        }

    CPU_INT_LOCK (&dontCare);

    /* issue an end-of-interrupt to the MPIC */
    MPIC_PCI_FLUSH(&dummy);
    MPIC_EOI;
    MPIC_PCI_FLUSH(&dummy);

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
    int		intLevel /* specified MPIC interrupt level */
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

    /* check for the Hawk internal error interrupt */

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
*
* RETURNS: OK always
*/

STATUS sysIbcInit (void)
    {
    UCHAR	intVec;

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

    /* Insure that all controller interrupts are edge sensitive */

    IBC_BYTE_OUT (SL82565_INT1_ELC, 0x00);

    /* Initialize the Interrupt Controller #2 */

    IBC_BYTE_OUT (PIC_port1 (PIC2_BASE_ADR),0x11);	/* ICW1 */
    IBC_BYTE_OUT (PIC_port2 (PIC2_BASE_ADR),sysVectorIRQ0+8); /* ICW2 */
    IBC_BYTE_OUT (PIC_port2 (PIC2_BASE_ADR),0x02);	/* ICW3 */
    IBC_BYTE_OUT (PIC_port2 (PIC2_BASE_ADR),0x01);	/* ICW4 */

    /* Mask interrupts IRQ 8-15 by writing to OCW1 register */

    IBC_BYTE_OUT (PIC_IMASK (PIC2_BASE_ADR),0xff);

    /* Make IRQ 14, 10, and 9 level sensitive */

    IBC_BYTE_OUT (SL82565_INT2_ELC, 0x40 | 0x04 | 0x02);

    /* Permanently turn off ISA refresh by never completing init steps */

    IBC_BYTE_OUT (SL82565_TMR1_CMOD, 0x74);

    /*	Perform the PCI Interrupt Ack cycle */

    IBC_BYTE_IN (HAWK_PHB_BASE_ADRS + 0x30, &intVec);

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

    IBC_BYTE_IN (HAWK_PHB_BASE_ADRS + 0x30, &intNum);

    /* Special check for phantom interrupts */

    if ((intNum & IBC_INT_LVL7) == IBC_INT_LVL7)	
        {
        if (sysIbcPhantomInt (&intNum, lvl7Int, lvl15Int) == TRUE)
            return;		/* It's phantom so just return. */
        }

    /*
     * If cascade from IBC2 just EIO and return. It has been seen on 
     * certain PCI ISA bridge devices, that it is possible to see the
     * cascade IRQ2.  In particular, this has been observed on the 
     * Winbond W83C553F on certain revs of the chip prior to Rev G.
     */

    if (intNum == IBC_INT_LVL2) 
        {
        sysIbcEndOfInt (intNum);
        return;
        }

    /* Keep track of level 7 and 15 nesting for phantom interrupt handling */

    if (intNum == IBC_INT_LVL7)
        lvl7Int++;
    else if (intNum == IBC_INT_LVL15)
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

    if (intNum == IBC_INT_LVL7)
        lvl7Int--;
    else if (intNum == IBC_INT_LVL15)
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

LOCAL UCHAR sysIbcPhantomInt
    (
    UCHAR *intNum,      /* interrupt number received on acknowledge */
    int	  lvl7Int,	/* interrupt 7 nesting level */
    int	  lvl15Int 	/* interrupt 15 nesting level */
    )
    {
    UCHAR irqBit;
    UINT  irqNum;

    logMsg ("sysIbcPhantomInt: incoming IRQ %d\n", *intNum,0,0,0,0,0);

    /* Read the master in-service register (ISR) */

    IBC_BYTE_OUT (PIC_port1 (PIC1_BASE_ADR), PIC_OCW3_SEL + PIC_ISR_READ);

    IBC_BYTE_IN (PIC_port1 (PIC1_BASE_ADR), &irqBit);

    if (irqBit == 0)
        {
        logMsg ("sysIbcPhantomInt: no interrupt in master\n", 0,0,0,0,0,0);
        return (TRUE);	/* No in-service int so it MUST be phantom */
        }

    for (irqNum = 0; ((irqBit & 1) == 0) ; irqNum++, irqBit >>= 1)
        ;

    if (irqNum == IBC_INT_LVL7)
        if (lvl7Int > 1)
            {
            logMsg ("sysIbcPhantomInt: nested interrupt in master\n",
                    0,0,0,0,0,0);
            return (TRUE);  /* We're nested so it MUST be phantom */
            }

    /* if irqNum isn't 2 we have an interrupt on the master */

    if (irqNum != IBC_INT_LVL2)
        {
        *intNum = irqNum;
        logMsg ("sysIbcPhantomInt: IRQ adjusted to %d\n", *intNum,0,0,0,0,0);
        return (FALSE);
        }

    /* Read the slave in-service register (ISR) */

    IBC_BYTE_OUT (PIC_port1 (PIC2_BASE_ADR), PIC_OCW3_SEL + PIC_ISR_READ);

    IBC_BYTE_IN (PIC_port1 (PIC2_BASE_ADR), &irqBit);

    if (irqBit == 0)	/* should never happen, really */
        {
        logMsg ("sysIbcPhantomInt: no interrupt in slave\n", 0,0,0,0,0,0);
        return (TRUE);	/* No in-service int so it MUST be phantom */
        }

    for (irqNum = 8; ((irqBit & 1) == 0) ; irqNum++, irqBit >>= 1)
        ;

    if (irqNum == 15)
        if (lvl15Int > 1)
            {
            logMsg ("sysIbcPhantomInt: nested interrupt in slave\n",
                    0,0,0,0,0,0);
            return (TRUE);  /* We're nested so it MUST be phantom */
            }

    *intNum = irqNum;
    logMsg ("sysIbcPhantomInt: IRQ adjusted to %d\n", *intNum,0,0,0,0,0);
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
    int intNum /* interrupt number to retire */
    )
    {
    if (intNum > IBC_INT_LVL7)
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

void sysIbcIntLevelSet
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
