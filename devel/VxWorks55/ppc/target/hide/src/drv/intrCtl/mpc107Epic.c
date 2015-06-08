/* mpc107Epic.c - Driver for Embedded Programmable Interrupt Controller */

/* Copyright 1984-2000 Wind River Systems, Inc. */

#include "copyright_wrs.h"

/*
modification history
--------------------
01b,11sep00,rcs fix include paths mpc107.h & mpc107Epic.h
01a,18jun00,bri written
*/

/*
DESCRIPTION
This module implements the Embedded Programmable Interrupt Controller (EPIC)
driver for the MPC107 chip .

The EPIC is an integrated interrupt controller in the MPC107 which provides
the following major capabilities:

Support for five external interrupt sources or one serial-style interrupt
(16 interrupt sources).

Four global high-resolution timers that can be interrupt sources .

Interrupt control for the MPC107 I2C unit, DMA unit (2 channels)
and message unit (MU) .

Processor initialization control : The processor can reset itself by setting
the processor initialization register, causing the assertion of the SRESET
signal .

Programmable resetting of the EPIC unit through the global configuration
register

16 programmable interrupt priority levels

Fully nested interrupt delivery

Spurious vector generation

32 bit configuration registers that are aligned on 128 bit boundaries

EPIC features are customized by writing into general control registers or into
interrupt level specific registers (IVPRs).

This driver allows a basic interface to the EPIC such as intializing it,setting
interrupt vectors, priorities, level/edge sense and interrupt polarities, as
well as enabling and disabling specific interrupts.

This driver implements a complete interrupt architecture system, complete with
vector table.

This driver provides the vector table for the system.  It can support a total of
256 vectors.  In this driver the EPIC controller device can only generate 5
different vectors, one for each external interrupt.

If there are other devices in the system capable of generating their own
vectors then we presume that an appropriate interrupt handler is created
and attached to the vector associated with the correct IRQ number.  That
interrupt handler would get a new vector directly from the device and then
call all of the handlers attached to that new vector.  Vector information is
stored in a linked list of INT_HANDLER_DESC structures. The sysIntTbl array
contains a pointer to the first entry for each vector.

This driver supports only "Direct IRQ" or "Serial IRQ" .It does not support
pass through mode .

.SH INITIALIZATION
This driver is initialized from the BSP, usually as part of sysHwInit().
The first routine to be called is mpc107EpicInit(). The routine resets the
global configuration register and resets the EPIC registers to default
values.The user has to specify whether the interface is to be used for
"Direct " or "Serial" IRQs .If "Serial" IRQ has to be used then the user
has to specify the  <clkRatio> .

The second routine to be called is mpc107EpicIntrInit().  This routine takes
no arguments. This routine allocates the vector table and initializes the
chips to a default state.  All individual interrupt sources are disabled.
Each has to be individually enabled by intEnable() before it will be
unmasked and allowed to generate an interrupt.

.SH CUSTOMIZING THIS DRIVER

The macros CPU_INT_LOCK() and CPU_INT_UNLOCK provide the access
to the CPU level interrupt lock/unlock routines.  We presume that there
is a single interrupt line to the CPU.  By default these macros call
intLock() and intUnlock() respectively.
*/

/* includes */

#include "vxWorks.h"
#include "drv/multi/mpc107.h"
#include "sysLib.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "drv/intrCtl/mpc107Epic.h"
#include "excLib.h"
#include "memLib.h"
#include "intLib.h"
#include "logLib.h"


#ifdef INCLUDE_WINDVIEW
#include "private/eventP.h"
#endif

/* defines */

#ifndef CPU_INT_LOCK
#   define CPU_INT_LOCK(pData) \
	(*pData = intLock ())
#endif

#ifndef CPU_INT_UNLOCK
#   define CPU_INT_UNLOCK(data) \
	(intUnlock (data))
#endif

/* externs */

IMPORT STATUS 	excIntConnect (VOIDFUNCPTR *, VOIDFUNCPTR);

#if FALSE /* defined(INCLUDE_WINDVIEW) || defined(INCLUDE_INSTRUMENTATION) */
IMPORT	int	evtTimeStamp;  /* Windview 1.0 only */
#endif

/* get the interrupt hook routines  prototypes*/

IMPORT STATUS	(*_func_intConnectRtn) (VOIDFUNCPTR *, VOIDFUNCPTR, int);
IMPORT int	(*_func_intEnableRtn) (int);
IMPORT int	(*_func_intDisableRtn)  (int);

/* globals */

INT_HANDLER_DESC * sysIntTbl [INTERRUPT_TABLESIZE]; /* system interrupt tbl */

/* locals */

/* forward declarations */

STATUS 		mpc107EpicIntConnect (VOIDFUNCPTR * vector, VOIDFUNCPTR routine,
                                      int parameter);
LOCAL STATUS	mpc107EpicIntEnable (int intNum);
LOCAL STATUS	mpc107EpicIntDisable (int intNum);
void 		mpc107EpicIntHandler (void);
LOCAL ULONG 	mpc107EpicCurTaskPrioSet (int prioNum);
LOCAL void 	mpc107EpicEOI(void);
STATUS    	mpc107EpicTimerInit (void);
VOID		mpc107EpicDelayTimer1 (UINT32 milliSec);
int             mpc107EpicIntAck (void);
STATUS          mpc107EpicIntSourceSet (ULONG srcAddr, int polarity, int sense,
                                        int priority, int vector);
int             mpc107EpicIntEnableVect (int srcAddr);
int             mpc107EpicIntDisableVect (int srcAddr);
STATUS          mpc107EpicsrcAddrCheck (ULONG srcAddr);


/***************************************************************************
*
* mpc107EpicInit - initialize the EPIC controller
*
* This routine resets the global Configuration Register, thus it:
* -  disables all interrupts
* -  sets EPIC registers to reset values
*
* It then sets the EPIC operation mode to Mixed Mode .
*
* <irqType> is either Direct IRQs (0) or Serial Interrupt IRQs (1).
* <clkRatio> is clock frequency driving the serial interrupt interface.
*
* If <irqType> is Direct IRQs:
* - <irqType> is written to the SIE bit of the EPIC Interrupt Configuration
* register.
* - <clkRatio> is ignored.
*
* If <irqType> is Serial IRQs:
* - both <irqType> and <clkRatio> will be written to the EPIC
* Interrupt Configuration register.
*
* RETURNS: N/A
*/

void mpc107EpicInit
    (
    ULONG 	irqType, 	/* irq type to initialize with */
                                /* Direct IRQ or Serial IRQ    */
    ULONG 	clkRatio	/* clock ratio  for Serial IRQ */
    )
    {
    ULONG 	gcrVal;
    ULONG 	icrVal;
    int 	irq;

    /* read Global Config Reg */

    gcrVal = MPC107EUMBBARREAD (MPC107_EPIC_GLOBAL_REG);

    gcrVal |= (MPC107_EPIC_GCR_RESET);

    /* Reset the EPIC */

    MPC107EUMBBARWRITE (MPC107_EPIC_GLOBAL_REG, gcrVal);

    /* wait for the reset sequence to be completed */

    while (MPC107EUMBBARREAD (MPC107_EPIC_GLOBAL_REG) & MPC107_EPIC_GCR_RESET)
        {
        ; /* do nothing */
        }

    /* configure for mixed mode */

    gcrVal = MPC107EUMBBARREAD (MPC107_EPIC_GLOBAL_REG);
    gcrVal |= (MPC107_EPIC_GCR_MODE_MIXED);
    MPC107EUMBBARWRITE (MPC107_EPIC_GLOBAL_REG, gcrVal);

    icrVal = MPC107EUMBBARREAD (MPC107_EPIC_INT_CONF_REG); /* read EICR */

    if (irqType == MPC107_EPIC_DIRECT_IRQ)
        {
        icrVal &= ~(MPC107_EPIC_ICR_SEI); /* disable serial mode interrupts */
        }

    else /* serial mode is configured */
        {
        if (clkRatio != 0)
            {
            icrVal |= MPC107_EPIC_ICR_CLK_RATIO (clkRatio);
            }
        }

    MPC107EUMBBARWRITE (MPC107_EPIC_INT_CONF_REG, icrVal);

    while (mpc107EpicIntAck() != 0xff)      /* Clear all pending interrupts */
        {
        /* do nothing */
	}

    /* init all IVPRs to sense = 1, polarity = defined, vec = 0, prio = 0 */

    for (irq = 0; irq < MPC107_EPIC_MAX_EXT_IRQS; irq++)
        {
        mpc107EpicIntDisable (MPC107_EPIC_VEC_REG(irq));

        mpc107EpicIntSourceSet (MPC107_EPIC_VEC_REG(irq),
                                MPC107_EPIC_INT_POLARITY,
                                MPC107_EPIC_SENSE_LVL, 0x0, 0x0);
        }


    /* set it to highest priority */

    mpc107EpicCurTaskPrioSet (MPC107_EPIC_PRIORITY_MAX);

   /* Timer interface initialization */

    mpc107EpicTimerInit ();

    }

/***************************************************************************
*
* mpc107EpicIntrInit - initialize the interrupt table
*
* This function initializes the interrupt mechanism of the board.
*
* RETURNS: N/A
*/

VOID  mpc107EpicIntrInit (void)
    {
    int vector;

    /* initialize the interrupt table */

    for (vector = 0; vector < INTERRUPT_TABLESIZE; vector++)
        {
	sysIntTbl [vector] = NULL;
        }

    /*
     * connect the interrupt demultiplexer to the PowerPC external
     * interrupt exception vector.
     * i. e.  put the address of this interrupt handler in
     * the PowerPC's only external interrupt exception vector
     * which is  _EXC_OFF_INTR = 0x500
     */

    excIntConnect ((VOIDFUNCPTR *) _EXC_OFF_INTR, mpc107EpicIntHandler);

    /*
     * set up the BSP specific interrupt routines
     * Attach the local routines to the VxWorks system calls
     *
     */

    _func_intConnectRtn  =  mpc107EpicIntConnect;
    _func_intEnableRtn   =  mpc107EpicIntEnable;
    _func_intDisableRtn  =  mpc107EpicIntDisable;

    /* set the task priority to the lowest priority */

    mpc107EpicCurTaskPrioSet (MPC107_EPIC_PRIORITY_MIN);

    }


/***************************************************************************
*
* mpc107EpicIntEnable - enable a EPIC interrupt
*
* This routine enables a specified EPIC interrupt.
*
* RETURNS: OK, or ERROR if the specified <intNum> is not correct .
*/

LOCAL int  mpc107EpicIntEnable
    (
    int intNum
    )
    {
    if ((intNum < MPC107_EPIC_MAX_EXT_IRQS))
        {

	/* enable interrupt on EPIC */

        mpc107EpicIntEnableVect (MPC107_EPIC_VEC_REG (intNum));
        return OK;
        }
    else
        return (ERROR);

    }

/***************************************************************************
*
* mpc107EpicIntDisable - disable a EPIC interrupt level
*
* This routine disables a specified EPIC interrupt level.
*
* RETURNS: OK, or ERROR if the specified <intNum> is not correct .
*/

LOCAL int  mpc107EpicIntDisable
    (
    int 	intNum
    )
    {
    if ((intNum < MPC107_EPIC_MAX_EXT_IRQS))
        {

        /* disable interrupt on EPIC */

        mpc107EpicIntDisableVect (MPC107_EPIC_VEC_REG (intNum));

        return OK;
        }
    else
        return (ERROR);

    }

/***************************************************************************
*
* mpc107EpicIntConnect - connect an interrupt handler to the system vector table
*
* This function connects an interrupt handler to the system vector table.
*
* RETURNS: OK, or ERROR if the <vector> is out of range or if memory
* allocation has failed .
*/

STATUS mpc107EpicIntConnect
    (
    VOIDFUNCPTR * 	vector,		/* interrupt vector to attach */
    VOIDFUNCPTR		routine,	/* routine to be called */
    int			parameter	/* parameter to be passed to routine */
    )
    {
    INT_HANDLER_DESC *	pNewHandler;
    INT_HANDLER_DESC *	pCurrHandler;
    int			intVal;
    BOOL		sharing = FALSE;


    if (((int)vector < 0)  || ((int) vector > INTERRUPT_TABLESIZE))
	{
        return (ERROR);   /*  out of range  */
	}

    /* create a new interrupt handler */

    pNewHandler = (INT_HANDLER_DESC *)malloc (sizeof (INT_HANDLER_DESC));

    /* check if the memory allocation succeed */

    if (pNewHandler == NULL)

	return (ERROR);

    /*  initialize the new handler  */

    pNewHandler->vec = routine;
    pNewHandler->arg = parameter;
    pNewHandler->next = NULL;

    /* install the handler in the system interrupt table  */

    intVal = intLock(); /* lock interrupts to prevent races */

    if (sysIntTbl [(int) vector] == NULL)
	{
        sysIntTbl [(int) vector] = pNewHandler;  /* single int. handler case */
	}
    else
	{
        pCurrHandler = sysIntTbl[(int) vector];/* multiple int. handler case */

        while (pCurrHandler->next != NULL)
            {
            pCurrHandler = pCurrHandler->next;
            }

        pCurrHandler->next = pNewHandler;

        sharing = TRUE;
	}


        /* EPIC IRQ set EPIC registers  */

        if (!sharing)
            mpc107EpicIntSourceSet (MPC107_EPIC_VEC_REG((int)vector),
                                    MPC107_EPIC_INT_POLARITY,
                                    MPC107_EPIC_SENSE_LVL,
                                    MPC107_EPIC_PRIORITY_GENERAL,
                                    (int) vector);

    intUnlock(intVal);

    return (OK);
    }


/***************************************************************************
*
* mpc107EpicIntDisconnect - disconnect an int. handler from the vector table
*
* This function diconnects an interrupt handler from the system vector table.
*
* RETURNS: OK, or ERROR if the <vector> is out of range or if the specified
* <routine> is not found in the vector table .
*/

STATUS mpc107EpicIntDisconnect
    (
    VOIDFUNCPTR * 	vector,		/* interrupt vector to attach */
    VOIDFUNCPTR		routine	/* routine to be called */
    )
    {
    INT_HANDLER_DESC *	pCurrHandler;
    INT_HANDLER_DESC *	pTempHandler;
    int			intVal;


    if (((int)vector < 0)  || ((int) vector > INTERRUPT_TABLESIZE))
	{
        return (ERROR);   /*  out of range  */
	}


    if (sysIntTbl [(int) vector] == NULL)

	return (ERROR);


    intLock (); /* lock interrupts to prevent races */

    pCurrHandler = sysIntTbl[(int) vector];/* multiple int. handler case */

    pTempHandler = pCurrHandler;

    while (( pCurrHandler ->vec != routine) || (pCurrHandler == NULL))
        {

          pTempHandler = pCurrHandler;

          pCurrHandler = pCurrHandler->next;

        }

     if (pCurrHandler != NULL)
        {

         if ((pCurrHandler ->vec == routine) && (pCurrHandler->next == NULL))
            {
            if (pTempHandler == pCurrHandler)
                {

                free (pCurrHandler);

                sysIntTbl[(int) vector] = NULL;

                }
            else
                {

                pTempHandler->next =  pCurrHandler->next;

                free (pCurrHandler);

                }

            }

        }

    intUnlock(intVal);

    return (OK);
    }


/***************************************************************************
*
* mpc107EpicIntHandlerExec  - execute the handlers for a given vector
*
* This routine executes all the handlers chained to a given vector.
* If a vector has no handlers attached to it, a logMsg is generated.
*
* NOMANUAL
*
* RETURNS: N/A
*
*/

void mpc107EpicIntHandlerExec
    (
    UCHAR intVec
    )
    {
    INT_HANDLER_DESC * pCurrHandler;

    /*  call each respective interrupt handler */

    if ((pCurrHandler = sysIntTbl [intVec]) == NULL)
        {
	logMsg ("uninitalized PIC interrupt vector %x\r\n",
                intVec, 0,0,0,0,0);
        }
    else
        {

	/* call Each respective chained interrupt handler  */

	while (pCurrHandler != NULL)
            {
  	    (*pCurrHandler->vec) (pCurrHandler->arg);
	    pCurrHandler = pCurrHandler->next;
            }
        }
    }

/***************************************************************************
*
* mpc107EpicIntHandler - handles the EPIC interrupts to the CPU
*
* This routine handles interrupts originating from the embedded interrupt
* controller.
* This handler is entered from the 0x500 exception.
*
* This routine is entered with CPU external interrupts enables.
*
* Since the EPIC is the primary interrupt controller this driver
* first initiates an Epic acknowledge call and reads the vector
* put out by the EPIC. Subsequent vectors have to be obtained if
* an external interrupt controller is connected to one of the
* EPIC handlers.
*
* This routine then processes the interrupt by calling all the interrupt
* service routines chained to the vector.
*
* Finally, this routine re-arms the interrupt at the PIC by performing an
* PIC EOI for the EPIC.
*
* RETURNS:  N/A
*
*/

void  mpc107EpicIntHandler (void)
    {
    int		dontCare;
    UCHAR 	epicIntVec;
    int 	oldLevel;

    /* clear int, return the vector for highest IRQ */

    epicIntVec = mpc107EpicIntAck ();

#ifdef INCLUDE_WINDVIEW
    WV_EVT_INT_ENT (epicIntVec);
#endif

    /* block all lower priority interrupts */

    oldLevel = mpc107EpicCurTaskPrioSet (epicIntVec);

    if  (oldLevel == MPC107_EPIC_INV_PRIO_ERROR)
        {
        return;
        }

    /* Allow external interrupts to the CPU. */

    CPU_INT_UNLOCK (_PPC_MSR_EE);

    mpc107EpicIntHandlerExec (epicIntVec);

    /*
     * Disable External Interrupts
     * External Interrupts will be re-enabled in the kernel's wrapper
     * of this Interrupt.
     */

    CPU_INT_LOCK (&dontCare);

    mpc107EpicEOI ();			 /* signal end of interrupt on EPIC */

    mpc107EpicCurTaskPrioSet (oldLevel); /* Allow the next interrupt to occur */

    }

/***************************************************************************
*
* mpc107EpicCurTaskPrioSet - set the priority of the current task.
*
* NOTES
*
* mpc107EpicCurTaskPrioSet sets the priority of the Processor Current Task
* Priority register to the value of the prioNum parameter.  This function
* should be called after mpc107EpicInit() to lower the priority of the processor
* current task. Note that valid priority values are 0 through 15 (15 being
* the highest priority)
*
* NOMANUAL
*
* RETURNS: previous priority of the task.
*/

ULONG mpc107EpicCurTaskPrioSet
    (
    int prioNum
    )
    {
    ULONG oldPrio;

    if ((prioNum < MPC107_EPIC_PRIORITY_MIN) ||
        (prioNum > MPC107_EPIC_PRIORITY_MAX))
        {

        /*
         * If the Priority number is less than "0"
         * and more than "15" return an error
         */

        return (MPC107_EPIC_INV_PRIO_ERROR);
	}

    /* Read the Processor Current Task Priority Register */

    oldPrio = MPC107EUMBBARREAD (MPC107_EPIC_PROC_CTASK_PRI_REG);

    /* Set the Processor Current Task Proirity Register with the new Priority */

    MPC107EUMBBARWRITE (MPC107_EPIC_PROC_CTASK_PRI_REG, prioNum);

    /* Return the previous Priority of the task */

    return (oldPrio);
    }

/***************************************************************************
*
* mpc107EpicIntEnableVect - enable an EPIC interrupt, given its IVPR
*
* This function clears the mask bit of an external, an internal or
* a Timer register to enable the interrupt.
*
* NOMANUAL
*
* RETURNS: OK ,or an error code if the IVPR passed is invalid.
*/

int mpc107EpicIntEnableVect
    (
    int 	srcAddr  /* address offset of the Vector Priority register */
    )
    {
    ULONG 	srcVal;
    int 	errCode;

    errCode = mpc107EpicsrcAddrCheck (srcAddr); /* Check if the IVPR is valid */

    if (errCode != MPC107_EPIC_INV_INTER_SOURCE) /* If the IVPR is valid */
        {

        /* Read the Interrupt/Vector Priority Register */

        srcVal = MPC107EUMBBARREAD (srcAddr);

        srcVal &= ~(MPC107_EPIC_IVPR_INTR_MSK);

        /*
         * clear the mask bit in the Interrupt/Vector Priority
         * Register to Enable the interrupt
         */

        MPC107EUMBBARWRITE(srcAddr, srcVal);
        return (OK);
        }
    else
        {

        /* Return an error if the IVPR passed is  invalid */

        return (errCode);
        }
    }

/***************************************************************************
*
* mpc107EpicIntDisableVect - disable an EPIC interrupt, given its IVPR
*
* This function sets the mask bit of an external, an internal or
* a Timer register to disable the interrupt.
*
* NOMANUAL
*
* RETURNS: OK, or an error code if the IVPR passed in was invalid.
*/

int mpc107EpicIntDisableVect
    (
    int 	srcAddr /* address offset of the Vector Priority register */
    )
    {
    ULONG 	srcVal;
    int 	errCode;

    errCode = mpc107EpicsrcAddrCheck (srcAddr); /* Check if the IVPR is valid */

    if (errCode != MPC107_EPIC_INV_INTER_SOURCE)   /* If the IVPR is valid */
        {

        /* Read the Interrupt/Vector Priority Register */

        srcVal = MPC107EUMBBARREAD (srcAddr);

        srcVal |= MPC107_EPIC_IVPR_INTR_MSK;

        /*
         * Set the mask bit in the Interrupt/Vector Priority
         * Register to disable the interrupt
         */

        MPC107EUMBBARWRITE (srcAddr, srcVal);

        return OK;
        }
    else
        {

        /* Return an error if the IVPR passed is  invalid */

        return (errCode);
        }
    }

/***************************************************************************
*
* mpc107EpicIntAck - read the IACK register and return vector
*
* NOTES
*
* mpc107EpicIntAck reads the Interrupt acknowldge register and return
* the vector number of the highest pending interrupt.
*
* NOMANUAL
*
* RETURNS: the vector number of the highest priority pending interrupt.
*/

int mpc107EpicIntAck(void)
    {
    return (MPC107EUMBBARREAD (MPC107_EPIC_PROC_INT_ACK_REG));
    }

/***************************************************************************
*
* mpc107EpicEOI - signal end of interrupt on the EPIC
*
* NOTES
*
* mpc107EpicEOI writes 0x0 to the EOI register to signal end of interrupt.
* This function is usually called after an interrupt routine is served.
*
* NOMANUAL
*
* RETURNS: N/A
*/

void mpc107EpicEOI(void)
    {
    MPC107EUMBBARWRITE (MPC107_EPIC_PROC_EOI_REG, 0x0);
    }

/***************************************************************************
*
* mpc107EpicsrcAddrCheck - check if the IVPR address passed in is
*                           internal or external
*
* NOTES
*
* Check if the address of a vector priority register is of an internal
* interrupt vector or an external interrupt vector.
*
* NOMANUAL
*
* RETURNS: EPIC_INTERNAL_INTERRUPT for internal interrupt sources,
*          or EPIC_EXTERNAL_INTERRUPT for external ones, or
*          EPIC_INV_INTER_SOURCE if an invalid IVPR was passed.
*/

STATUS mpc107EpicsrcAddrCheck
    (
    ULONG srcAddr /* address offset of the Vector Priority register */
    )
    {
    switch(srcAddr)
        {
        case MPC107_EPIC_TM0_VEC_REG:
        case MPC107_EPIC_TM1_VEC_REG:
        case MPC107_EPIC_TM2_VEC_REG:
        case MPC107_EPIC_TM3_VEC_REG:
        case MPC107_EPIC_I2C_INT_VEC_REG:
        case MPC107_EPIC_DMA0_INT_VEC_REG:
        case MPC107_EPIC_DMA1_INT_VEC_REG:
        case MPC107_EPIC_MSG_INT_VEC_REG:
            return (MPC107_EPIC_INTERNAL_INTERRUPT);

        case MPC107_EPIC_SR_INT0_VEC_REG:
        case MPC107_EPIC_SR_INT1_VEC_REG:
        case MPC107_EPIC_SR_INT2_VEC_REG:
        case MPC107_EPIC_SR_INT3_VEC_REG:
        case MPC107_EPIC_SR_INT4_VEC_REG:
        case MPC107_EPIC_SR_INT5_VEC_REG:
        case MPC107_EPIC_SR_INT6_VEC_REG:
        case MPC107_EPIC_SR_INT7_VEC_REG:
        case MPC107_EPIC_SR_INT8_VEC_REG:
        case MPC107_EPIC_SR_INT9_VEC_REG:
        case MPC107_EPIC_SR_INT10_VEC_REG:
        case MPC107_EPIC_SR_INT11_VEC_REG:
        case MPC107_EPIC_SR_INT12_VEC_REG:
        case MPC107_EPIC_SR_INT13_VEC_REG:
        case MPC107_EPIC_SR_INT14_VEC_REG:
        case MPC107_EPIC_SR_INT15_VEC_REG:
            return (MPC107_EPIC_EXTERNAL_INTERRUPT);

        default:
            return (MPC107_EPIC_INV_INTER_SOURCE);

        }
    }

/***************************************************************************
*
* mpc107EpicIntSourceSet - set interrupt parameters for an interrupt register
*
* This function sets the interrupt parameters for an External Interrupt
* Source Vector Priority register, an Internal Interrupt Source Vector
* Priority register, or a Global Timer register.  The interrupt parameters
* can only be set when the current source is not in-request or in-service,
* which is determined by the Activity bit.  This routine reads the Activity
* bit; if the value of this bit is 1 (in-request or in-service), it returns
* error; otherwise, it sets the value of the parameters to the register
* offset defined in srcAddr.
*
*
* NOMANUAL
*
* RETURNS: OK ,or ERROR if <srcAddr> is not valid or <priority> is not valid .
*/

STATUS mpc107EpicIntSourceSet
    (
    ULONG 	srcAddr,  /* address Offset of the source interrupt register */
    int 	polarity, /* polarity of the external interrupt */
    int 	sense,    /* Level Sensitive Interrupt (0),  */
                          /* Edge Sensitive Interrupt (1) */
    int 	priority, /* priority of the interrupt (0 to 15)*/
    int 	vector    /* vector number (0 to 128)*/
    )
    {
    ULONG 	srcVal;
    ULONG	errCode;

    errCode = mpc107EpicsrcAddrCheck(srcAddr);/* Check If IVPR is valid */

    if (errCode == MPC107_EPIC_INV_INTER_SOURCE)
        {

         /* Return an error if the IVPR passed is  invalid */

        return (errCode);
        }

    srcVal = MPC107EUMBBARREAD(srcAddr);

    if (srcVal & MPC107_EPIC_IVPR_INTR_ACTIVE)
        {
        return (MPC107_EPIC_INTER_IN_SERVICE);	/* interrupt in process */
        }

    /* polarity and sense doesn't apply to internal interrupts */

    if (errCode == MPC107_EPIC_INTERNAL_INTERRUPT)
        {
      	polarity = 0;
        sense    = 0;
        }

    /* erase previously set polority, sense, prority and vector values */

    srcVal &= ~(MPC107_EPIC_IVPR_INTR_POLARITY |
                MPC107_EPIC_IVPR_INTR_SENSE |
                MPC107_EPIC_IVPR_PRIORITY_MSK  |
                MPC107_EPIC_IVPR_VECTOR_MSK);

    srcVal |= (MPC107_EPIC_IVPR_POLARITY (polarity) |
               MPC107_EPIC_IVPR_SENS (sense) |
               MPC107_EPIC_IVPR_PRIORITY (priority) |
               MPC107_EPIC_IVPR_VECTOR (vector));

    MPC107EUMBBARWRITE(srcAddr, srcVal);

    return (OK);
    }


/***************************************************************************
*
* mpc107EpicIntSourceGet - retrieve information of an EPIC source vector reg.
*
* This function retrieves information of an EPIC source vector register.
* The information includes the Enable bit, the polarity, sense bits, the
* interrupt priority and interrupt vector number.
*
* NOMANUAL
*
* RETURNS: OK or ERROR
*/

STATUS mpc107EpicIntSourceGet
    (
    ULONG 	srcAddr,     /* address of the source vector register */
    int *	pEnableMask, /* whether the interrupt is enabled */
    int *	pPolarity,   /* interrupt polarity (high or low) */
    int *	pSense,      /* interrupt sense (level or edge) */
    int *	pPriority,   /* interrupt priority */
    int *	pVector      /* interrupt vector number */
    )
    {
    ULONG 	srcVal;
    int 	errCode;

    errCode = mpc107EpicsrcAddrCheck (srcAddr);

    if (errCode == MPC107_EPIC_INV_INTER_SOURCE)
        {

         /* Return an error if the IVPR passed is  invalid */

        return errCode;

        }

    srcVal = MPC107EUMBBARREAD(srcAddr);

    *pEnableMask  = (srcVal & 0x80000000) >> 31; /* retrieve enable mask-b31 */
    *pPolarity    = (srcVal & 0x00800000) >> 23;
    *pSense       = (srcVal & 0x00400000) >> 22;
    *pPriority    = (srcVal & 0x000F0000) >> 16;
    *pVector      = (srcVal & 0x000000FF);

    return OK;
    }



/***************************************************************************
*
* mpc107EpicTimerInit -  initialization of EPIC Timer's
*
* This routine should be called before using any timer related routines .
*
* NOMANUAL
*
* RETURNS: N/A
*/

STATUS mpc107EpicTimerInit (void)
    {

    /* setup timer frequency register */

    MPC107EUMBBARWRITE(MPC107_EPIC_TM_FREQ_REG, MPC107_DEFAULT_TIMER_CLOCK);
    return OK;
    }


/***************************************************************************
*
* mpc107EpicDelayTimer1 -  generate a delay using TIMER 1 of EPIC
*
* this routine is used for generating a delay in milliseconds using
* the timer 1 on the EPIC
*
* NOMANUAL
*
* RETURNS: N/A
*/

void mpc107EpicDelayTimer1
    (
    UINT milliSec  /* time in millisecs */
    )
    {

    UINT32 toggleInitStat = 0;

    /* Read the toggle bit in the global timer current count register */

    toggleInitStat= MPC107EUMBBARREAD(MPC107_EPIC_TM1_CUR_COUNT_REG) &
                    MPC107_EPIC_TM_CUR_COUNT_TB;


    /* enable counter and write value to count from */

    MPC107EUMBBARWRITE(MPC107_EPIC_TM1_BASE_COUNT_REG,
                      ((MPC107_DEFAULT_TIMER_CLOCK/milliSec) &
                      ~(MPC107_EPIC_TM_BASE_COUNT_CI )));



    while ((MPC107EUMBBARREAD(MPC107_EPIC_TM1_CUR_COUNT_REG) &
            MPC107_EPIC_TM_CUR_COUNT_TB) == toggleInitStat);


    }
