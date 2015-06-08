/* i8259Intr.c - Intel 8259A PIC (Programmable Interrupt Controller) driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01p,16apr02,hdn  added support for the Special Fully Nested Mode, IRQ0 Early
		 EOI Mode, IRQ0 Special Mask Mode, AEOI Mode (spr 76411)
01o,29mar02,hdn  added forgotten back slash to the optimized code
01n,21mar02,hdn  fixed doc build by removing comments in the optimized code
01m,25feb02,hdn  updated i8259Init() for HTT (spr 73738)
		 added optimized version of EOI routines
01l,11sep01,hdn  updated comment regarding the spr-28547 fix.
01k,27aug99,hdn  fixed a bug for stray interrupt in i8259IntBoi(SPR 28547).
01j,11oct98,ms   fixed compiler warning via typecast
01i,04jun98,hdn  changed a method to exit i8259IntBoi().
01h,25may98,hdn  changed function name starting from "i8259".
		 added i8259IntEoiMaster(), i8259IntEoiSlave(), i8259IntBoi().
		 removed sysIntEOI().
01g,17mar97,hdn  added sysIntLock(), sysIntUnlock() for the system mode.
01f,25jun96,hdn  added sysIntLevel() for windview.
01e,23may96,wlf  doc: cleanup.
01d,14jun95,hdn  renamed sysEndOfInt to sysIntEOI.
		 moved global function prototypes to sysLib.h.
01c,08aug94,hdn  stopped toggling IRQ9 in enabling and disabling.
01b,22apr94,hdn  made IRQ9 off in the initialization.
		 moved sysVectorIRQ0 to sysLib.c.
01a,05sep93,hdn  written.
*/

/*
DESCRIPTION
This module is a driver for the Intel 8259A PIC (Programmable Interrupt
Controller).  The Intel 8259A handles up to 8 vectored priority interrupts
for the CPU.  It is cascadable for up to 64 vectored priority interrupts,
though this driver assumes two cascaded 8259A.  It is initialized for
"Fully Nested Mode", "Non-Specific EOI" mode.

Fully Nested Mode.
In this mode, interrupt requests are ordered in priority
from 0 through 7 (0 is the highest priority).  When an interrupt is
acknowledged, the highest priority request is determined and its vector is
placed on the bus.  Additionally, a bit of the Interrupt Service (IS)
register is set.  This bit remains set until the microprocessor issues an
EOI command immediately before returning from the service routine.  While
the IS bit is set, all further interrupts of the same or lower priority
are inhibited, while higher level interrupts are allowed.  The
i8259IntEoiSlaveNfnm() routine is used to issue the EOI command.  The PICs
in a PC typically operate in this mode (normal nested mode).  In this mode,
while the slave PIC is being serviced by the master PIC, the slave PIC blocks
all higher priority interrupt requests.  Alternatively, to allow interrupts of
a higher priority, enable the Special Fully Nested Mode.

Special Fully Nested Mode: I8259_SPECIAL_FULLY_NESTED_MODE.
This mode is similar to the Fully Nested Mode with the following exceptions:
1) When an interrupt request from a slave PIC is in service, the slave is
not locked out from the master's priority logic and further interrupt
requests from higher priority IRs within the slave will be recognized by
the master and will initiate interrupts to the processor.  2) When exiting
the interrupt service routine, the software must check whether or not the
interrupt serviced was the only interrupt request from the slave.  If it
was the only interrupt request, a non-specific EOI is sent to the master.
If not, no EOI is sent.  This is implemented by the i8259EoiSlaveSfnm()
routine.

Non-Specific EOI: When the 8259A is operated in the Fully Nested Mode, it
can determine which IS bit to reset on EOI.  When a non-specific EOI
command is issued, the 8259A will automatically reset the highest IS bit of
those that are set, since in the fully nested mode the highest IS level is
the last level acknowledged and serviced.

Spurious/Phantom Interrupt: The IR inputs must remain high until after the
falling edge of the first INTA.  If the IR input goes low before this time,
a DEFAULT(Spurious/Phantom) IR7 will occur when the CPU acknowledges the
interrupt.  The interrupt handler should simply return without sending
an EOI command.

The PIC(8259A) IRQ0 is hard wired to the PIT(8253) channel 0 in a PC
motherboard.  IRQ0 is the highest priority in the 8259A interrupt
controller.  Thus, the system clock interrupt handler blocks all lower
level interrupts.  This may cause a delay of the lower level interrupts in
some situations even though the system clock interrupt handler finishes its
job without any delay.  This is quite natural from the hardware point of
view, but may not be ideal from the application software standpoint.  The
following modes are supplied to mitigate this situation by providing
corresponding configuration macros in the BSP.  The three modes are mutually
exclusive.

Early EOI Issue in IRQ0 ISR.
In this mode, the EOI command is issued before the IRQ0 system clock interrupt
service routine starts the kernel work.  This lowers the IRQ0 ISR blocking
level to the next lower level.  If no IRQs are in service, the next lower
level is the lowest level.  If IRQn is in service, the next lower level
corresponds to the next lower priority.  As a result, the kernel work in the
system clock interrupt service routine can be interrupted by an interrupt
with a higher priority than the blocking level.  The i8259IntBoiEem() routine 
is called before the interrupt service routine, and no EOI is sent after 
the interrupt service routine.

Special Mask Mode in IRQ0 ISR.
In this mode, the Special Mask Mode is used in the IRQ0 system clock
service routine.  This lowers the blocking level to the specified level
(currently hard coded to the lowest level).  The i8259IntBoiSmm() routine
is called before the interrupt service routine, and the i8259IntEoiSmm()
routine is called after the interrupt service routine.

Automatic EOI Mode: I8259_AUTO_EOI.
This mode provides no nested multi-level interrupt structure in PIC1.  The
EOI command is automatically sent to the master PIC at the end of the
interrupt acknowledge cycle.  Thus, no software intervention is needed.  The
i8259IntBoi() routine is called before the IRQ7 and IRQ15 interrupt service
routines.  Either the i8259IntEoiSlaveNfnm() routine or the
i8259IntEoiSlaveSfnm() routine is called after the slave PIC's interrupt
service routine.

SEE ALSO: pc386/target.nr
*/

#include "drv/intrCtl/i8259.h"


/* externs */

IMPORT void	intBoiExit ();
IMPORT UINT32	sysStrayIntCount;
IMPORT BOOL	sysBp;		/* TRUE(default) for BP, FALSE for AP */


/* defines */

#define	I8259_EOI_OPTIMIZED	/* use the optimized version */


/* globals */


/* local */

#ifndef	SYMMETRIC_IO_MODE
LOCAL INT8	i8259IntMask1;		/* interrupt mask for PIC1 */
LOCAL INT8	i8259IntMask2;		/* interrupt mask for PIC2 */
LOCAL INT8	i8259Mask = 0;		/* interrupt mask for PIC1 */
#endif	/* SYMMETRIC_IO_MODE */


/* forward static functions */


/*******************************************************************************
*
* i8259Init - initialize the PIC
*
* This routine initializes the PIC.
*
*/

VOID i8259Init (void)

    {
    UINT8 icw4 = 0x01;

    /* return if it is not the Boot Processor (BP) */

    if (sysBp == FALSE)
	return;

    /* initialize the PIC (Programmable Interrupt Controller) */

    sysOutByte (PIC_port1 (PIC1_BASE_ADR),0x11);        /* ICW1 */
    sysOutByte (PIC_port2 (PIC1_BASE_ADR),INT_NUM_IRQ0); /* ICW2 */
    sysOutByte (PIC_port2 (PIC1_BASE_ADR),0x04);        /* ICW3 */

#ifdef	I8259_SPECIAL_FULLY_NESTED_MODE
    icw4 |= 0x10;					/* SFNM */
#endif	/* I8259_SPECIAL_FULLY_NESTED_MODE */

#ifdef	I8259_AUTO_EOI
    icw4 |= 0x02;					/* AEOI */
#endif	/* I8259_AUTO_EOI */

    sysOutByte (PIC_port2 (PIC1_BASE_ADR),icw4);        /* ICW4 */

    sysOutByte (PIC_port1 (PIC2_BASE_ADR),0x11);        /* ICW1 */
    sysOutByte (PIC_port2 (PIC2_BASE_ADR),INT_NUM_IRQ0+8); /* ICW2 */
    sysOutByte (PIC_port2 (PIC2_BASE_ADR),0x02);        /* ICW3 */
    sysOutByte (PIC_port2 (PIC2_BASE_ADR),0x01);        /* ICW4 */

    /* disable interrupts */

    sysOutByte (PIC_IMASK (PIC1_BASE_ADR),0xfb);
    sysOutByte (PIC_IMASK (PIC2_BASE_ADR),0xff);
    }

#ifndef	SYMMETRIC_IO_MODE

/*******************************************************************************
*
* i8259IntBoiEem - issue EOI before the IRQ0 interrupt handler
*
* This routine is called before the IRQ0 interrupt handler that is PIT(8253)
* channel 0 system clock interrupt handler in the Early EOI Mode.
*
*/

VOID i8259IntBoiEem
    (
    int irqNo		/* IRQ number of the interrupt */
    )
    {
    INT32 oldLevel = intLock ();			/* LOCK INTERRUPT */

    sysOutByte (PIC_IACK (PIC1_BASE_ADR), 0x20);	/* NS EOI to PIC1 */

    intUnlock (oldLevel);				/* UNLOCK INTERRUPT */
    }

/*******************************************************************************
*
* i8259IntBoiSmm - enable Special Mask Mode before the IRQ0 interrupt handler
*
* This routine is called before the IRQ0 interrupt handler that is PIT(8253)
* channel 0 system clock interrupt handler, in the Special Mask Mode.
*
*/

VOID i8259IntBoiSmm
    (
    int irqNo		/* IRQ number of the interrupt */
    )
    {
    INT32 oldLevel = intLock ();			/* LOCK INTERRUPT */

    sysOutByte (PIC_port1 (PIC1_BASE_ADR), 0x68);	/* enable SMM PIC1 */
    i8259Mask = sysInByte (PIC_IMASK (PIC1_BASE_ADR));	/* save int mask */
    sysOutByte (PIC_IMASK (PIC1_BASE_ADR), 0x01);	/* unlock except IRQ0 */

    intUnlock (oldLevel);				/* UNLOCK INTERRUPT */
    }

/*******************************************************************************
*
* i8259IntBoi - detect whether it is spurious interrupt or not
*
* This routine is called before the user's interrupt handler to detect the
* spurious interrupt.
*
*/

VOID i8259IntBoi
    (
    INT32 irqNo		/* IRQ number of the interrupt */
    )
    {
    INT32 oldLevel;
    INT8 inserviceReg;

    /* we are interested in IRQ7 and IRQ15 */

    if ((irqNo != 7) && (irqNo != 15))
	return;

    /* if ISR bit is not set, we change the return address */

    oldLevel = intLock ();		/* LOCK INTERRUPT */

    if (irqNo == 7)
	{
        sysOutByte (PIC_port1 (PIC1_BASE_ADR), 0x0b);
        inserviceReg = sysInByte (PIC_port1 (PIC1_BASE_ADR));
	}
    else
	{
        sysOutByte (PIC_port1 (PIC2_BASE_ADR), 0x0b);
        inserviceReg = sysInByte (PIC_port1 (PIC2_BASE_ADR));
	}
    
    intUnlock (oldLevel);		/* UNLOCK INTERRUPT */

    /*
     * another implementation idea...
     *
     * *((UINT32 *)&irqNo - 1) = (UINT32)intBoiExit;
     * This changes the return addr on the stack.  This is architecture
     * specific and tricky.  Thus making following change may be good idea.
     * 
     * - let this routine return OK or ERROR
     * - let interrupt stub code check the ret value and jump to intExit.
     *
     * New code would be like this:
     * :
     * if ((irqNo != 7) && (irqNo != 15))
     *	return (OK);
     * :
     * if ((inserviceReg & 0x80) == 0)	* check bit7 for IRQ7 and IRQ15 *
     *	{
     *	sysStrayIntCount++;		* increment the counter *
     *  return (ERROR);
     *	}
     * }	
     *
     * 00  e8 kk kk kk kk           call    _intEnt         * tell kernel
     * 05  50                       pushl   %eax            * save regs
     * 06  52                       pushl   %edx
     * 07  51                       pushl   %ecx
     * 08  68 pp pp pp pp           pushl   $_parameterBoi  * push BOI param
     * 13  e8 rr rr rr rr           call    _routineBoi     * call BOI routine
     *                              addl    $4, %esp
     *                              cmpl    $0, %eax
     *                              jne     intConnectCode0
     * 18  68 pp pp pp pp           pushl   $_parameter     * push param
     * 23  e8 rr rr rr rr           call    _routine        * call C routine
     *                              addl    $4, %esp
     * 28  68 pp pp pp pp           pushl   $_parameterEoi  * push EOI param
     * 33  e8 rr rr rr rr           call    _routineEoi     * call EOI routine
     * 38  83 c4 04                 addl    $4, %esp        * pop param
     * intConnectCode0:
     * 41  59                       popl    %ecx            * restore regs
     * 42  5a                       popl    %edx
     * 43  58                       popl    %eax
     * 44  e9 kk kk kk kk           jmp     _intExit        *  exit via kernel
     */
     
    if ((inserviceReg & 0x80) == 0)	/* check bit7 for IRQ7 and IRQ15 */
	{
	sysStrayIntCount++;		/* increment the counter */
        *((UINT32 *)&irqNo - 1) = (UINT32)intBoiExit;	/* change return addr */
	}
    }

/*******************************************************************************
*
* i8259IntEoiSmm - disable Special Mask Mode with EOI signal to the master PIC
*
* This routine is called at the end of the IRQ0 interrupt handler that is 
* PIT(8253) channel 0 system clock interrupt handler in the Special Mask Mode.
*
*/

VOID i8259IntEoiSmm 
    (
    int irqNo		/* IRQ number to send EOI */
    )
    {
    INT32 oldLevel = intLock ();			/* LOCK INTERRUPT */

    sysOutByte (PIC_IMASK (PIC1_BASE_ADR), i8259Mask);	/* restore old mask */
    sysOutByte (PIC_port1 (PIC1_BASE_ADR), 0x08);	/* disable SMM PIC1 */
    sysOutByte (PIC_IACK (PIC1_BASE_ADR), 0x20);	/* NS EOI to PIC1 */

    intUnlock (oldLevel);				/* UNLOCK INTERRUPT */
    }

/*******************************************************************************
*
* i8259IntEoiMaster - send EOI(end of interrupt) signal to the master PIC.
*
* This routine is called at the end of the interrupt handler.
*
*/

VOID i8259IntEoiMaster 
    (
    INT32 irqNo		/* IRQ number to send EOI */
    )
    {
#ifdef	I8259_EOI_OPTIMIZED	/* optimized version */

    WRS_ASM ("			\
	pushfl;			\
	cli;			\
	movl	$0x20, %edx;	\
	movl	$0x20, %eax;	\
	outb	%al, %dx;	\
	popfl;			\
	");

#else				/* portable version */

    INT32 oldLevel = intLock ();	/* LOCK INTERRUPTS */

    sysOutByte (PIC_IACK (PIC1_BASE_ADR), I8259_EOI); /* non-specific EOI */

    intUnlock (oldLevel);		/* UNLOCK INTERRUPTS */

#endif	/* I8259_EOI_OPTIMIZED */
    }

/*******************************************************************************
*
* i8259IntEoiSlave - send EOI(end of interrupt) signal to the slave PIC.
*
* This routine is called at the end of the interrupt handler in the Normal 
* Fully Nested Mode.  This is kept for the backward compatibility.
*
*/

VOID i8259IntEoiSlave
    (
    INT32 irqNo		/* IRQ number to send EOI */
    )
    {
#ifdef	I8259_EOI_OPTIMIZED	/* optimized version */

    WRS_ASM ("			\
	pushfl;			\
	cli;			\
	movl	$0xa0, %edx;	\
	movl	$0x20, %eax;	\
	outb	%al, %dx;	\
	movl	$0x20, %edx;	\
	outb	%al, %dx;	\
	popfl;			\
	");

#else				/* portable version */

    INT32 oldLevel = intLock ();		/* LOCK INTERRUPTS */

    sysOutByte (PIC_IACK (PIC2_BASE_ADR), I8259_EOI); /* non-specific EOI */
    sysOutByte (PIC_IACK (PIC1_BASE_ADR), I8259_EOI); /* non-specific EOI */

    intUnlock (oldLevel);			/* UNLOCK INTERRUPTS */

#endif	/* I8259_EOI_OPTIMIZED */
    }

/*******************************************************************************
*
* i8259IntEoiSlaveNfnm - send EOI(end of interrupt) signal to the slave PIC.
*
* This routine is called at the end of the interrupt handler in the Normal 
* Fully Nested Mode.
*
*/

VOID i8259IntEoiSlaveNfnm
    (
    INT32 irqNo		/* IRQ number to send EOI */
    )
    {
#ifdef	I8259_EOI_OPTIMIZED	/* optimized version */

    WRS_ASM ("			\
	pushfl;			\
	cli;			\
	movl	$0xa0, %edx;	\
	movl	$0x20, %eax;	\
	outb	%al, %dx;	\
	movl	$0x20, %edx;	\
	outb	%al, %dx;	\
	popfl;			\
	");

#else				/* portable version */

    INT32 oldLevel = intLock ();		/* LOCK INTERRUPTS */

    sysOutByte (PIC_IACK (PIC2_BASE_ADR), I8259_EOI); /* non-specific EOI */
    sysOutByte (PIC_IACK (PIC1_BASE_ADR), I8259_EOI); /* non-specific EOI */

    intUnlock (oldLevel);			/* UNLOCK INTERRUPTS */

#endif	/* I8259_EOI_OPTIMIZED */
    }

/*******************************************************************************
*
* i8259IntEoiSlaveSfnm - send EOI(end of interrupt) signal to the slave PIC.
*
* This routine is called at the end of the interrupt handler in the Special 
* Fully Nested Mode.
*
*/

VOID i8259IntEoiSlaveSfnm
    (
    int irqNo		/* IRQ number to send EOI */
    )
    {
    INT8  inserviceReg;
    INT32 oldLevel = intLock ();			/* LOCK INTERRUPT */

    sysOutByte (PIC_IACK (PIC2_BASE_ADR), 0x20);	/* NS EOI to PIC2 */

    sysOutByte (PIC_port1 (PIC2_BASE_ADR), 0x0b);	/* issue OCW3 */
    inserviceReg = sysInByte (PIC_port1 (PIC2_BASE_ADR)); /* read ISR */
    if (inserviceReg == 0) 
        sysOutByte (PIC_IACK (PIC1_BASE_ADR), 0x20);	/* NS EOI to PIC1 */

    intUnlock (oldLevel);				/* UNLOCK INTERRUPT */
    }

/*******************************************************************************
*
* i8259IntDisable - disable a specified PIC interrupt input line
*
* This routine disables a specified PIC interrupt input line.
*
* RETURNS: OK, always.
*
* SEE ALSO: i8259IntEnable()
*
* ARGSUSED0
*/

LOCAL STATUS i8259IntDisable
    (
    INT32 irqNo        /* IRQ number to disable */
    )
    {

    if (irqNo < 8)
	{
	sysOutByte (PIC_IMASK (PIC1_BASE_ADR),
	    sysInByte (PIC_IMASK (PIC1_BASE_ADR)) | (1 << irqNo));
	}
    else
	{
	sysOutByte (PIC_IMASK (PIC2_BASE_ADR),
	    sysInByte (PIC_IMASK (PIC2_BASE_ADR)) | (1 << (irqNo - 8)));
	}

    return (OK);
    }

/*******************************************************************************
*
* i8259IntEnable - enable a specified PIC interrupt input line
*
* This routine enables a specified PIC interrupt input line.
*
* RETURNS: OK, always.
*
* SEE ALSO: i8259IntDisable()
*
* ARGSUSED0
*/

LOCAL STATUS i8259IntEnable
    (
    INT32 irqNo        /* IRQ number to enable */
    )
    {

    if (irqNo < 8)
	{
	sysOutByte (PIC_IMASK (PIC1_BASE_ADR),
	    sysInByte (PIC_IMASK (PIC1_BASE_ADR)) & ~(1 << irqNo));
	}
    else
	{
	sysOutByte (PIC_IMASK (PIC2_BASE_ADR),
	    sysInByte (PIC_IMASK (PIC2_BASE_ADR)) & ~(1 << (irqNo - 8)));
	}

    return (OK);
    }

/*******************************************************************************
*
* i8259IntLock - lock out all PIC interrupts
*
* This routine saves the mask and locks out all PIC interrupts.
* It should be called in the interrupt disable state(IF bit is 0).
*
* SEE ALSO: i8259IntUnlock()
*
* ARGSUSED0
*/

LOCAL VOID i8259IntLock (void)

    {

    i8259IntMask1 = sysInByte (PIC_IMASK (PIC1_BASE_ADR));
    i8259IntMask2 = sysInByte (PIC_IMASK (PIC2_BASE_ADR));
    sysOutByte (PIC_IMASK (PIC1_BASE_ADR), 0xff);
    sysOutByte (PIC_IMASK (PIC2_BASE_ADR), 0xff);
    }

/*******************************************************************************
*
* i8259IntUnlock - unlock the PIC interrupts
*
* This routine restores the mask and unlocks the PIC interrupts
* It should be called in the interrupt disable state(IF bit is 0).
*
* SEE ALSO: i8259IntLock()
*
* ARGSUSED0
*/

LOCAL VOID i8259IntUnlock (void)

    {

    sysOutByte (PIC_IMASK (PIC1_BASE_ADR), i8259IntMask1);
    sysOutByte (PIC_IMASK (PIC2_BASE_ADR), i8259IntMask2);
    }

#if	FALSE
/*
 * There is a function sysIntLevel() which get the interrupt level from the 
 * interrupt stub, so we don't need it anymore.
 */
/*******************************************************************************
*
* i8259IntLevel - Get IRQ number by reading Interrupt Service Register.
*
* This routine is called to get an IRQ number in service.
*
* RETURNS: 0 - 15.
*
* ARGSUSED0
*/

LOCAL INT32 i8259IntLevel (void)

    {
    INT32 inserviceReg;
    INT32 irq;
    INT32 oldLevel;
    INT32 retry;

    oldLevel = intLock ();

    for (retry=0; retry < 1; retry ++)
	{
        sysOutByte (PIC_port1 (PIC1_BASE_ADR), 0x0b);
        inserviceReg = sysInByte (PIC_port1 (PIC1_BASE_ADR));
        for (irq=0; irq < 8; irq++)
	    if ((inserviceReg & 1) && (irq != 2))
	        goto i8259IntLevelExit;
	    else
	        inserviceReg >>= 1;

        sysOutByte (PIC_port1 (PIC2_BASE_ADR), 0x0b);
        inserviceReg = sysInByte (PIC_port1 (PIC2_BASE_ADR));
        for (irq=8; irq < 16; irq++)
	    if (inserviceReg & 1)
	        goto i8259IntLevelExit;
	    else
	        inserviceReg >>= 1;
	}

    i8259IntLevelExit:
    intUnlock (oldLevel);

    return (irq);
    }
#endif	/* FALSE */

#endif	/* SYMMETRIC_IO_MODE */
