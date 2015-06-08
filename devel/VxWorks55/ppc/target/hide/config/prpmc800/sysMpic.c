/* sysMpic.c - MPIC Interrupt Controller driver */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1996-2001 Motorola, Inc., All Rights Reserved */

#include "copyright_wrs.h"

/*
modification history
--------------------
01c,28oct00,krp  Modified offset for destination & ext interrupt registers.
01b,08oct00,dmw  Added OPIC enable to init routine, fixed getMpicVecOffset.
01a,05sep00,krp  derived from ppmc765/hawkMpic.c source file.
*/

/*
DESCRIPTION
This module implements the MPIC and the Intel 8259 compatable (IBC) ISA
interrupt controllers.

This driver provides support for initializing the MPIC and handling
its interrupts.

The MPIC (Multi-Processor Iterrupt Controller) is an Open Peripheral Interrupt
Controller (OPIC) which is contained within the Motorola PCI Host Bridge ASIC. The
MPIC has the following general features.

.CS
  - Support for up to two processors.
  - Supports 16 external interrupt sources.
  - Supports 15 programmable Interrupt and Procesor Task priority levels.
  - Supports the connection of an external 8259 for ISA/AT compatibility.
  - Supports distributed delivery for external I/O interrupts.
  - Direct/Multicast interrupt delivery for Interprocessor and timer interrupts.
  - Four Inter-processor Interrupt sources.
  - Four Timers (See board Aux Clock driver for additional information).
.CE

In the environment, there is no 8259 ISA/AT interrupt controller.
Therefore, the 8259-compatible operating mode of the MPIC is not applicable.

NOTE: The system MPIC was clocked using the 60x bus clock. The MPIC is
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

#include "sysMpic.h"

/* defines */

#define MPIC_EOI       MPIC_WRITE( MPIC_CPU0_EOI_REG, 0 )
#define IBC_INT_PENDING	0x80

/* globals */

IMPORT STATUS       (*_func_intConnectRtn) (VOIDFUNCPTR *, VOIDFUNCPTR, int);
IMPORT int          (*_func_intEnableRtn) (int);
IMPORT int          (*_func_intDisableRtn) (int);
IMPORT void         sysOutByte (ULONG, UCHAR);
IMPORT UCHAR        sysInByte (ULONG);
IMPORT STATUS       excIntConnect (VOIDFUNCPTR *, VOIDFUNCPTR);
void                sysMpicIntHandler (void);

IMPORT UINT         sysVectorIRQ0; 	   /* vector for IRQ0 */
INT_HANDLER_DESC    *sysIntTbl [256];   /* system interrupt table */
int tpr;

#ifdef INCLUDE_INSTRUMENTATION
IMPORT	int         evtTimeStamp;
#endif

/* forward declarations */

LOCAL int     getMpicVecOffset  (int);
LOCAL STATUS  sysMpicIntConnect (VOIDFUNCPTR *vector, 
                                 VOIDFUNCPTR routine, int parameter);
LOCAL int     sysMpicIntEnable  (int);
LOCAL int     sysMpicIntDisable (int);

/* Hardware access methods */

#ifndef CPU_INT_LOCK
#   define CPU_INT_LOCK(x) \
	(*x = intLock ())
#endif

#ifndef CPU_INT_UNLOCK
#   define CPU_INT_UNLOCK(data) \
	(intUnlock (data))
#endif


/*******************************************************************************
 * sysMpicInit - initialize the MPIC in the Harrier ASIC
 *
 * This function initializes the Multi-Processor Interrupt Controller (MPIC)
 * contained in the BSP chipset.
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
    UINT32	  	dontCare;
    UINT32              timerReg;
    UINT32              ipiReg;
    UINT32              destReg;
    LOCAL_INT_DATA      init;

    /* initialize the base address of the MPIC and enable */

    *(UINT32 *)(HARRIER_MPIC_BASEADDRESS_REG) = 
        ((MPIC_BASE_ADRS & HARRIER_MBAR_MBA_MASK) | HARRIER_MBAR_ENA);
    
    /* reset the MPIC and give it sometime to complete */

    MPIC_WRITE(MPIC_GLOBAL_CONFIG_REG, RESET_CNTRLR);

    MPIC_READ(MPIC_GLOBAL_CONFIG_REG, dontCare);

    sysUsDelay(50);
 
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
    _func_intEnableRtn  = sysMpicIntEnable;
    _func_intDisableRtn = sysMpicIntDisable;


    /* generate a PCI IACK to clear any pending interrupts */

    MPIC_READ(MPIC_CPU0_IACK_REG, dontCare);

    /*  
     * inhibit(mask) the IPI registers
     * clear and disable the Timer count regiser
     * inhibit(mask) the Timer regisers.
     */

    timerReg = MPIC_TIMER0_BASE_CT_REG;
    ipiReg   = MPIC_IPI0_VEC_PRI_REG;
    for (i=0; i<4; i++)
        {
        MPIC_WRITE(ipiReg, IPI_INHIBIT);
        ipiReg += REG_OFFSET;
        MPIC_WRITE(timerReg, TIMER_INHIBIT);
        timerReg += REG_OFFSET;
        MPIC_WRITE(timerReg, TIMER_INHIBIT);
        timerReg += (REG_OFFSET * 3);  
        }

    /* setup the external source vector/priority registers */

    MPIC_WRITE( MPIC_EXT_SRC0_VEC_PRI_REG, INIT_EXT_SRC0 );
    MPIC_WRITE( MPIC_EXT_SRC1_VEC_PRI_REG, INIT_EXT_SRC1 );
    MPIC_WRITE( MPIC_EXT_SRC2_VEC_PRI_REG, INIT_EXT_SRC2 );
    MPIC_WRITE( MPIC_EXT_SRC3_VEC_PRI_REG, INIT_EXT_SRC3 );
    MPIC_WRITE( MPIC_EXT_SRC4_VEC_PRI_REG, INIT_EXT_SRC4 );
    MPIC_WRITE( MPIC_EXT_SRC5_VEC_PRI_REG, INIT_EXT_SRC5 );
    MPIC_WRITE( MPIC_EXT_SRC6_VEC_PRI_REG, INIT_EXT_SRC6 );
    MPIC_WRITE( MPIC_EXT_SRC7_VEC_PRI_REG, INIT_EXT_SRC7 );
    MPIC_WRITE( MPIC_EXT_SRC8_VEC_PRI_REG, INIT_EXT_SRC8 );
    MPIC_WRITE( MPIC_EXT_SRC9_VEC_PRI_REG, INIT_EXT_SRC9 );
    MPIC_WRITE( MPIC_EXT_SRC10_VEC_PRI_REG, INIT_EXT_SRC10 );
    MPIC_WRITE( MPIC_EXT_SRC11_VEC_PRI_REG, INIT_EXT_SRC11 );
    MPIC_WRITE( MPIC_EXT_SRC12_VEC_PRI_REG, INIT_EXT_SRC12 );
    MPIC_WRITE( MPIC_EXT_SRC13_VEC_PRI_REG, INIT_EXT_SRC13 );
    MPIC_WRITE( MPIC_EXT_SRC14_VEC_PRI_REG, INIT_EXT_SRC14 );
    MPIC_WRITE( MPIC_EXT_SRC15_VEC_PRI_REG, INIT_EXT_SRC15 );

    /* 
     * setup the destination register to send all external
     * interrupts to CPU 0
     */

    destReg = MPIC_EXT_SRC0_DEST_REG;
    for (i=0; i<15; i++)
        {
        MPIC_WRITE(destReg, DESTINATION_CPU0);
        destReg += (REG_OFFSET*2);
        }

    /* 
     * setup the destination register to send all external
     * interrupts to CPU 0
     */

    destReg = MPIC_CPU0_IPI0_DISP_REG;
    for (i = 0; i < 4; i++)
        {
        MPIC_WRITE(destReg, DESTINATION_CPU0);
        destReg += REG_OFFSET;
        }

    /* set Timer interrupts to be destine for CPU0 */

    destReg = MPIC_TIMER0_DEST_REG;
    for (i = 0; i < 4; i++)
        {
        MPIC_WRITE(destReg, DESTINATION_CPU0);
        destReg += (REG_OFFSET*4); 
        }

    /* set Internal Functional/Error interrupts to be destine for CPU0 */

    MPIC_WRITE(MPIC_FUNC_DEST_REG, DESTINATION_CPU0);
    MPIC_WRITE(MPIC_ERR_DEST_REG, DESTINATION_CPU0);

    /* set the Internal Functional/Error interrupt vector/priority */

    MPIC_WRITE(MPIC_FUNC_VEC_PRI_REG, INIT_INT_SRC0);
    MPIC_WRITE(MPIC_ERR_VEC_PRI_REG, INIT_INT_SRC1);

    /* setup the Ext source 0 reg (8259 input) for normal operation */

    MPIC_READ(MPIC_EXT_SRC0_VEC_PRI_REG, init.regVal);
    init.regVal |= PIB_INT_VEC;
    init.regVal &= (~INT_MASK_BIT);
    MPIC_WRITE(MPIC_EXT_SRC0_VEC_PRI_REG, init.regVal);

    /* set Internal Functional/Error interrupts to be destine for CPU0 */

    MPIC_WRITE(MPIC_CPU0_CUR_TASK_PRI_REG, 1);

    /* setup the MPIC to process the 8259 interrupts ( mixed mode ) */

    MPIC_WRITE(MPIC_GLOBAL_CONFIG_REG, SET_MIXED_MODE);

    /* OPIC interrupt */

    *(UINT8 *)(HARRIER_MPIC_CONTROL_STATUS_REG) = HARRIER_MCSR_OPI;  

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
    VOIDFUNCPTR       *vector,         /* interrupt vector to attach */
    VOIDFUNCPTR       routine,         /* routine to be called */
    int               parameter        /* parameter to be passed to routine */
    )
    {
    INT_HANDLER_DESC  *newHandler;
    INT_HANDLER_DESC  *currHandler;
    LOCAL_INT_DATA    connect;
    static BOOL       firstTime = TRUE;

    if (((int)vector < 0) || ((int)vector > 0xff))	/* Check within Range */
        return (ERROR);
 
    if (firstTime)
        {
        /* connect the PIB to MPIC, before any other connections */

        firstTime = FALSE;
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

        MPIC_READ(connect.regAddr, connect.regVal);

        /* store the interrupt vector number */

        connect.regVal |= (int)vector;

        /* write the contents of the vector register back */

        MPIC_WRITE(connect.regAddr, connect.regVal);
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

    if ((intLevel < 0) || (intLevel > INTERNAL_INTERRUPT_BASE))
        return (ERROR);

    /* If ISA interrupt level,call the IBC driver enable routine, */

    if ( intLevel < EXT_INTERRUPT_BASE )
        return (ERROR);

    enable.regAddr = getMpicVecOffset( intLevel );

    if ( enable.regAddr > 0 )
        {
        /* read the vector register */

        MPIC_READ(enable.regAddr, enable.regVal);

        /* enable the interrupt */

        enable.regVal &= ~(INT_MASK_BIT);

        /* write the contents of the vector register back */

        MPIC_WRITE(enable.regAddr, enable.regVal);
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

    if ((intLevel < 0) || (intLevel > INTERNAL_INTERRUPT_BASE))
        return (ERROR);

    /* If ISA interrupt level, call the IBC driver disable routine, */

    if (intLevel < EXT_INTERRUPT_BASE)
        return (ERROR);
 
    /* else, it is an MPIC interrupt level */

    else
        {
        /* get the vector reg. offset value */

        disable.regAddr = getMpicVecOffset(intLevel);
 
        if (disable.regAddr > 0)
            {
            /* read the vector register */

            MPIC_READ(disable.regAddr, disable.regVal);

            /* disable the interrupt */

            disable.regVal |= INT_MASK_BIT;

            /* write the contents of the vector register back */

            MPIC_WRITE(disable.regAddr, disable.regVal);
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
    UINT32		vecNum = 0xFF;
    int			dontCare;

    /* get the vector from the MPIC IACK reg. */

    MPIC_READ (MPIC_CPU0_IACK_REG, vecNum);

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

    CPU_INT_LOCK (&dontCare);

    /* issue an end-of-interrupt to the MPIC */

    MPIC_EOI;

    return;
    }


/*******************************************************************************
*
* sysMpicBusIntGen - generate an out-bound PCI bus interrupt using the MPIC
*
* This routine generates an out-bound PCI interrupt using the CPU1 section of
* the MPIC Inter-Processor Interrupt (IPI) mechanism. The board hardware
* routes the CPU1 interrupt line to the PCU bus to permit this use.
*
* RETURNS: OK or ERROR if interrupt vector not supported
*/
 
int sysMpicBusIntGen
    (
    int level,          /* interrupt level to generate (not used) */
    int vector          /* interrupt vector for interrupt */
    )
    {
 
    /* Validate the vector number. */
 
    if (vector != SM_BUS_INT_VEC)
        return (ERROR);
 
    /* Set the trigger bit in the proper IPI dispatch register. */
 
    MPIC_WRITE( SM_BUS_INT_TRIGGER_OFFSET, (1 << SM_BUS_INT_BIT) );
    return (OK);
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

    for (;;)
        {
        if ((intLevel >= EXT_INTERRUPT_BASE) && 
            (intLevel < TIMER_INTERRUPT_BASE))
            {
            offset = intLevel - EXT_INTERRUPT_BASE;
            offset = MPIC_EXT_SRC0_VEC_PRI_REG + ( offset * REG_OFFSET * 2 );
            break;
            }
 
        /* check for a timer interrupt level */

        if ((intLevel >= TIMER_INTERRUPT_BASE) && 
            (intLevel < IPI_INTERRUPT_BASE))
            {
            offset = intLevel - TIMER_INTERRUPT_BASE;
            offset =  MPIC_TIMER0_VEC_PRI_REG + ( offset * REG_OFFSET * 4 );
            break;
            }
 
        /* check for an IPI interrupt level */

        if ((intLevel >= IPI_INTERRUPT_BASE) && 
            (intLevel < INTERNAL_INTERRUPT_BASE))
            {
            offset = intLevel - IPI_INTERRUPT_BASE;
            offset =  MPIC_IPI0_VEC_PRI_REG + ( offset * REG_OFFSET );
            break;
            }

        /* check for the MPIC internal error interrupt */

        if ((intLevel >= INTERNAL_INTERRUPT_BASE ) && 
            (intLevel < (INTERNAL_INTERRUPT_BASE + 2)))
            {
            offset = intLevel - INTERNAL_INTERRUPT_BASE;
            offset = MPIC_FUNC_VEC_PRI_REG + ( offset * REG_OFFSET );
            break;
            }

        break;
        }

    return (offset);
    }

