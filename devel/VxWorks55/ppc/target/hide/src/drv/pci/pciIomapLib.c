/* pciIomapLib.c - Support for PCI drivers */

/* Copyright 1984-2001 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
01i,27oct01,dat  Adding warnings about obsolete drivers
01h,15oct98,dat  SPR 9098, added iv.h and private/semLibP.h
01g,12jan97,hdn  changed variable name "vender" to "vendor".
01f,12jan97,hdn  changed variable name "class" to "classCodeReg".
01e,03dec96,hdn  added single/multi function check.
01d,16sep96,dat  made pciConfigMech global (for pciIomapShow).
01c,06aug96,hdn  added pciInt(), pciIntConnect() and pciIntDisconnect().
01b,14mar96,hdn  re-written.  changed parameters of the functions.
		 removed BIOS dependent codes.
01a,25feb95,bcs  written
*/


/*
DESCRIPTION

This library is PCI Revision 2.1 compliant.
This module contains routines to support PCI bus mapped on IO address space
for x86 and PowerPC architecture.  Functions in this library should not be
called from the interrupt level, except pciInt(), since it uses a mutual
exclusion semaphore for consecutive register access.  
The functions addressed here include:

  - Initialize the library.
  - Locate the device by deviceID and vendorID.
  - Locate the device by classCode.
  - Generate the special cycle.
  - Access its configuration registers.
  - Connect a shared interrupt handler.
  - Disconnect a shared interrupt handler.
  - Master shared interrupt handler.

There are functions to access the IO address space.  In x86 architecture,
they are sysInXXX() and sysOutXXX().  Macro PCI_IN_XXX() and PCI_OUT_XXX()
are provided to use other IO address space functions.

Shared PCI interrupt are supported by three functions: pciInt(),
pciIntConnect(), pciIntDisconnect().  pciIntConnect() adds the specified
interrupt handler to the link list and pciIntDisconnect() removes it from
the link list.  Master interrupt handler pciInt() executes these interrupt
handlers in the link list for a PCI interrupt.  Each interrupt handler must
check the device dependent interrupt status bit to determine the source of
the interrupt, since it simply execute all interrupt handlers in the link
list.  pciInt() should be attached by intConnect() function in the BSP 
initialization with its parameter. The parameter is an IRQ associated 
to the PCI interrupt.
*/

#include "vxWorks.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "intLib.h"
#include "sysLib.h"
#include "drv/pci/pciIomapLib.h"
#include "iv.h"
#include "private/semLibP.h"

/* pciIomapLib is being obsoleted. Please use pciConfigLib instead. */

#warning "src/drv/pci/pciIomapLib.c is outdated. Please use pciConfigLib"

/* defines */

#ifndef	PCI_IN_BYTE
#define PCI_IN_BYTE(x)		sysInByte (x)
#endif
#ifndef	PCI_IN_WORD
#define PCI_IN_WORD(x)		sysInWord (x)
#endif
#ifndef	PCI_IN_LONG
#define PCI_IN_LONG(x)		sysInLong (x)
#endif
#ifndef	PCI_OUT_BYTE
#define PCI_OUT_BYTE(x,y)	sysOutByte (x,y)
#endif
#ifndef	PCI_OUT_WORD
#define PCI_OUT_WORD(x,y)	sysOutWord (x,y)
#endif
#ifndef	PCI_OUT_LONG
#define PCI_OUT_LONG(x,y)	sysOutLong (x,y)
#endif


/* globals */

STATUS	pciLibInitStatus = NONE;	/* initialization done */
DL_LIST	pciIntList[PCI_IRQ_LINES];	/* link list of int handlers */
int	pciConfigMech = NONE;		/* 1=mechanism-1, 2=mechanism-2 */

/* locals */

LOCAL int pciConfigAddr0;		/* config-addr-reg, CSE-reg*/
LOCAL int pciConfigAddr1;		/* config-data-reg, forward-reg */
LOCAL int pciConfigAddr2;		/* not-used, base-addr */
LOCAL SEMAPHORE pciMuteSem;		/* mutual exclusion semaphore */


/*******************************************************************************
*
* pciIomapLibInit - initialize the configuration access-method and addresses
*
* This routine initializes the configuration access-method and addresses.
*
* Configuration mechanism one utilizes two 32-bit IO ports located at addresses
* 0x0cf8 and 0x0cfc. These two ports are:
*   - 32-bit configuration address port, at 0x0cf8
*   - 32-bit configuration data port, at 0x0cfc
* Accessing a PCI function's configuration port is two step process.
*   - Write the bus number, physical device number, function number and 
*     register number to the configuration address port.
*   - Perform an IO read from or an write to the configuration data port.
*
* Configuration mechanism two uses following two single-byte IO ports.
*   - Configuration space enable, or CSE, register, at 0x0cf8
*   - Forward register, at 0x0cfa
* To generate a PCI configuration transaction, the following actions are
* performed.
*   - Write the target bus number into the forward register.
*   - Write a one byte value to the CSE register at 0x0cf8.  The bit
*     pattern written to this register has three effects: disables the
*     generation of special cycles; enables the generation of configuration
*     transactions; specifies the target PCI functional device.
*   - Perform a one, two or four byte IO read or write transaction within
*     the IO range 0xc000 through 0xcfff.
*
* RETURNS:
* OK, or ERROR if a mechanism is not 1 or 2.
*
*/

STATUS pciIomapLibInit
    (
    int mechanism,	/* configuration mechanism: 1 or 2 */
    int addr0,		/* config-addr-reg / CSE-reg */
    int addr1,		/* config-data-reg / Forward-reg */
    int addr2		/* none            / Base-address */
    )
    {
    int ix;

    if (pciLibInitStatus != NONE)
	return (pciLibInitStatus);

    switch (mechanism)
	{
	case PCI_MECHANISM_1:
	case PCI_MECHANISM_2:
	    pciConfigMech	= mechanism;
	    pciConfigAddr0	= addr0;
	    pciConfigAddr1	= addr1;
	    pciConfigAddr2	= addr2;
	    pciLibInitStatus	= OK;
	    break;

	default:
    	    pciLibInitStatus	= ERROR;
	    break;
	}

    semMInit (&pciMuteSem, SEM_Q_PRIORITY | SEM_DELETE_SAFE | 
	      SEM_INVERSION_SAFE);

    for (ix = 0; ix < PCI_IRQ_LINES; ix++)
        dllInit (&pciIntList[ix]);

    return (pciLibInitStatus);
    }

/*******************************************************************************
*
* pciFindDevice - find the nth device with the given device & vendor ID
*
* This routine finds the nth device with the given device & vendor ID.
*
* RETURNS:
* OK, or ERROR if the deviceId and vendorId didn't match.
*
*/

STATUS pciFindDevice
    (
    int vendorId,	/* vendor ID */
    int deviceId,	/* device ID */
    int	index,		/* desired instance of device */
    int * pBusNo,	/* bus number */
    int * pDeviceNo,	/* device number */
    int * pFuncNo	/* function number */
    )
    {
    STATUS status = ERROR;
    int busNo;
    int deviceNo;
    int funcNo;
    short device;
    short vendor;
    char header;

    if (pciLibInitStatus != OK)			/* sanity check */
        return (ERROR);

    for (busNo=0; busNo < 0xff; busNo++)
        for (deviceNo=0; deviceNo < 0x1f; deviceNo++)
            for (funcNo=0; funcNo < 0x07; funcNo++)
		{
		/* avoid a special bus cycle */

		if ((deviceNo == 0x1f) && (funcNo == 0x07))
		    continue;

		pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_VENDOR_ID,
				 &vendor);
		pciConfigInWord (busNo, deviceNo, funcNo, PCI_CFG_DEVICE_ID,
				 &device);
		if ((vendor == (short)vendorId) && 
		    (device == (short)deviceId) &&
		    (index-- == 0))
		    {
		    *pBusNo	= busNo;
		    *pDeviceNo	= deviceNo;
		    *pFuncNo	= funcNo;
		    status	= OK;
		    break;		/* terminate loop */
		    }

		/* goto next device if it is a single function device */

		pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_HEADER_TYPE, 
				 &header);
		if ((header & PCI_HEADER_MULTI_FUNC) != PCI_HEADER_MULTI_FUNC)
		    break;
		}

    return (status);
    }

/*******************************************************************************
*
* pciFindClass - find the nth device with the given class(3 bytes).
*
* This routine finds the nth device with the given class(class subclass prog).
*
* RETURNS:
* OK, or ERROR if the class didn't match.
*
*/

STATUS pciFindClass
    (
    int	classCode,	/* class code */
    int	index,		/* desired instance of device */
    int * pBusNo,	/* bus number */
    int * pDeviceNo,	/* device number */
    int * pFuncNo	/* function number */
    )
    {
    STATUS status = ERROR;
    int busNo;
    int deviceNo;
    int funcNo;
    int classCodeReg;
    char header;

    if (pciLibInitStatus != OK)			/* sanity check */
        return (ERROR);

    for (busNo=0; busNo < 0xff; busNo++)
        for (deviceNo=0; deviceNo < 0x1f; deviceNo++)
            for (funcNo=0; funcNo < 0x07; funcNo++)
		{
		/* avoid a special bus cycle */

		if ((deviceNo == 0x1f) && (funcNo == 0x07))
		    continue;

		pciConfigInLong (busNo, deviceNo, funcNo, PCI_CFG_REVISION,
				 &classCodeReg);
		if ((((classCodeReg >> 8) & 0x00ffffff) == classCode) &&
		    (index-- == 0))
		    {
		    *pBusNo	= busNo;
		    *pDeviceNo	= deviceNo;
		    *pFuncNo	= funcNo;
		    status	= OK;
		    break;		/* terminate loop */
		    }

		/* goto next device if it is a single function device */

		pciConfigInByte (busNo, deviceNo, funcNo, PCI_CFG_HEADER_TYPE, 
				 &header);
		if ((header & PCI_HEADER_MULTI_FUNC) != PCI_HEADER_MULTI_FUNC)
		    break;
		}

    return (status);
    }

/*******************************************************************************
*
* pciPack - pack parameters for the Configuration Address Register
*
* This routine packs three parameters into one integer for accessing the
* Configuration Address Register
*
* RETURNS:
* packed integer.
*
*/

LOCAL int pciPack
    (
    int	busNo,		/* bus number */
    int	deviceNo,	/* device number */
    int	funcNo		/* function number */
    )
    {
    return (((busNo    << 16) & 0x00ff0000) |
	    ((deviceNo << 11) & 0x0000f800) |
	    ((funcNo   << 8)  & 0x00000700));
    }

/*******************************************************************************
*
* pciConfigInByte - read one byte from the PCI configuration space
*
* This routine reads one byte from the PCI configuration space
*
* RETURNS:
* OK, or ERROR if this library is not initialized.
*
*/

STATUS pciConfigInByte
    (
    int	busNo,		/* bus number */
    int	deviceNo,	/* device number */
    int	funcNo,		/* function number */
    int	address,	/* address of the configuration space */
    char * pData	/* data read from the address */
    )
    {
    int retval = 0;

    if (pciLibInitStatus != OK)			/* sanity check */
        return (ERROR);

    semTake (&pciMuteSem, WAIT_FOREVER);	/* mutual execlusion start */

    switch (pciConfigMech)
	{
	case PCI_MECHANISM_1:
	    PCI_OUT_LONG (pciConfigAddr0, pciPack (busNo, deviceNo, funcNo) |
		          (address & 0xfc) | 0x80000000);
	    retval = PCI_IN_BYTE (pciConfigAddr1 + (address & 0x3));
	    break;

	case PCI_MECHANISM_2:
	    PCI_OUT_BYTE (pciConfigAddr0, 0xf0 | (funcNo << 1));
	    PCI_OUT_BYTE (pciConfigAddr1, busNo);
	    retval = PCI_IN_LONG (pciConfigAddr2 | ((deviceNo & 0x000f) << 8) |
				  (address & 0xfc));
	    PCI_OUT_BYTE (pciConfigAddr0, 0);
	    retval >>= (address & 0x03) * 8;
	    break;

	default:
	    break;
	}

    semGive (&pciMuteSem);			/* mutual execlusion stop */

    *pData = retval;

    return (OK);
    }

/*******************************************************************************
*
* pciConfigInWord - read one word from the PCI configuration space
*
* This routine reads one word from the PCI configuration space
*
* RETURNS:
* OK, or ERROR if this library is not initialized.
*
*/

STATUS pciConfigInWord
    (
    int	busNo,		/* bus number */
    int	deviceNo,	/* device number */
    int	funcNo,		/* function number */
    int	address,	/* address of the configuration space */
    short * pData	/* data read from the address */
    )
    {
    int retval = 0;

    if (pciLibInitStatus != OK)			/* sanity check */
        return (ERROR);

    semTake (&pciMuteSem, WAIT_FOREVER);	/* mutual execlusion start */

    switch (pciConfigMech)
	{
	case PCI_MECHANISM_1:
	    PCI_OUT_LONG (pciConfigAddr0, pciPack (busNo, deviceNo, funcNo) |
		          (address & 0xfc) | 0x80000000);
	    retval = PCI_IN_WORD (pciConfigAddr1 + (address & 0x2));
	    break;

	case PCI_MECHANISM_2:
	    PCI_OUT_BYTE (pciConfigAddr0, 0xf0 | (funcNo << 1));
	    PCI_OUT_BYTE (pciConfigAddr1, busNo);
	    retval = PCI_IN_LONG (pciConfigAddr2 | ((deviceNo & 0x000f) << 8) |
				  (address & 0xfc));
	    PCI_OUT_BYTE (pciConfigAddr0, 0);
	    retval >>= (address & 0x02) * 8;
	    break;

	default:
	    break;
	}

    semGive (&pciMuteSem);			/* mutual execlusion stop */

    *pData = retval;

    return (OK);
    }

/*******************************************************************************
*
* pciConfigInLong - read one longword from the PCI configuration space
*
* This routine reads one longword from the PCI configuration space
*
* RETURNS:
* OK, or ERROR if this library is not initialized.
*
*/

STATUS pciConfigInLong
    (
    int	busNo,		/* bus number */
    int	deviceNo,	/* device number */
    int	funcNo,		/* function number */
    int	address,	/* address of the configuration space */
    int * pData		/* data read from the address */
    )
    {
    int retval = 0;

    if (pciLibInitStatus != OK)			/* sanity check */
        return (ERROR);

    semTake (&pciMuteSem, WAIT_FOREVER);	/* mutual execlusion start */

    switch (pciConfigMech)
	{
	case PCI_MECHANISM_1:
	    PCI_OUT_LONG (pciConfigAddr0, pciPack (busNo, deviceNo, funcNo) |
		          (address & 0xfc) | 0x80000000);
	    retval = PCI_IN_LONG (pciConfigAddr1);
	    break;

	case PCI_MECHANISM_2:
	    PCI_OUT_BYTE (pciConfigAddr0, 0xf0 | (funcNo << 1));
	    PCI_OUT_BYTE (pciConfigAddr1, busNo);
	    retval = PCI_IN_LONG (pciConfigAddr2 | ((deviceNo & 0x000f) << 8) |
				  (address & 0xfc));
	    PCI_OUT_BYTE (pciConfigAddr0, 0);
	    break;

	default:
	    break;
	}

    semGive (&pciMuteSem);			/* mutual execlusion stop */

    *pData = retval;

    return (OK);
    }

/*******************************************************************************
*
* pciConfigOutByte - write one byte to the PCI configuration space
*
* This routine writes one byte to the PCI configuration space.
*
* RETURNS:
* OK, or ERROR if this library is not initialized.
*
*/

STATUS pciConfigOutByte
    (
    int	busNo,		/* bus number */
    int	deviceNo,	/* device number */
    int	funcNo,		/* function number */
    int	address,	/* address of the configuration space */
    char data		/* data written to the address */
    )
    {
    int retval;
    int mask	= 0x000000ff;
  char txt[100];

  sprintf(txt,"pciConfigOutByte: %d %d %d 0x%08x 0x%08x\r\n",
    busNo,deviceNo,funcNo,address,data);
  sysDebugMsg(txt,CONTINUE_EXECUTION);

    if (pciLibInitStatus != OK)			/* sanity check */
        return (ERROR);

    semTake (&pciMuteSem, WAIT_FOREVER);	/* mutual execlusion start */

    switch (pciConfigMech)
	{
	case PCI_MECHANISM_1:
	    PCI_OUT_LONG (pciConfigAddr0, pciPack (busNo, deviceNo, funcNo) |
		          (address & 0xfc) | 0x80000000);
	    PCI_OUT_BYTE ((pciConfigAddr1 + (address & 0x3)), data);
	    break;

	case PCI_MECHANISM_2:
	    PCI_OUT_BYTE (pciConfigAddr0, 0xf0 | (funcNo << 1));
	    PCI_OUT_BYTE (pciConfigAddr1, busNo);
	    retval = PCI_IN_LONG (pciConfigAddr2 | ((deviceNo & 0x000f) << 8) |
				  (address & 0xfc));
	    data = (data & mask) << ((address & 0x03) * 8);
	    mask <<= (address & 0x03) * 8;
	    retval = (retval & ~mask) | data;
	    PCI_OUT_LONG ((pciConfigAddr2 | ((deviceNo & 0x000f) << 8) |
			  (address & 0xfc)), retval);
	    PCI_OUT_BYTE (pciConfigAddr0, 0);
	    break;

	default:
	    break;
	}

    semGive (&pciMuteSem);			/* mutual execlusion stop */

    return (OK);
    }

/*******************************************************************************
*
* pciConfigOutWord - write one word to the PCI configuration space
*
* This routine writes one word to the PCI configuration space.
*
* RETURNS:
* OK, or ERROR if this library is not initialized.
*
*/

STATUS pciConfigOutWord
    (
    int	busNo,		/* bus number */
    int	deviceNo,	/* device number */
    int	funcNo,		/* function number */
    int	address,	/* address of the configuration space */
    short data		/* data written to the address */
    )
    {
    int retval;
    int mask	= 0x0000ffff;
  char txt[100];

  sprintf(txt,"pciConfigOutWord: %d %d %d 0x%08x 0x%08x\r\n",
    busNo,deviceNo,funcNo,address,data);
  sysDebugMsg(txt,CONTINUE_EXECUTION);

    if (pciLibInitStatus != OK)			/* sanity check */
        return (ERROR);

    semTake (&pciMuteSem, WAIT_FOREVER);	/* mutual execlusion start */

    switch (pciConfigMech)
	{
	case PCI_MECHANISM_1:
	    PCI_OUT_LONG (pciConfigAddr0, pciPack (busNo, deviceNo, funcNo) |
		          (address & 0xfc) | 0x80000000);
	    PCI_OUT_WORD ((pciConfigAddr1 + (address & 0x2)), data);
	    break;

	case PCI_MECHANISM_2:
	    PCI_OUT_BYTE (pciConfigAddr0, 0xf0 | (funcNo << 1));
	    PCI_OUT_BYTE (pciConfigAddr1, busNo);
	    retval = PCI_IN_LONG (pciConfigAddr2 | ((deviceNo & 0x000f) << 8) |
				  (address & 0xfc));
	    data = (data & mask) << ((address & 0x02) * 8);
	    mask <<= (address & 0x02) * 8;
	    retval = (retval & ~mask) | data;
	    PCI_OUT_LONG ((pciConfigAddr2 | ((deviceNo & 0x000f) << 8) |
			  (address & 0xfc)), retval);
	    PCI_OUT_BYTE (pciConfigAddr0, 0);
	    break;

	default:
	    break;
	}

    semGive (&pciMuteSem);			/* mutual execlusion stop */

    return (OK);
    }

/*******************************************************************************
*
* pciConfigOutLong - write one longword to the PCI configuration space
*
* This routine writes one longword to the PCI configuration space.
*
* RETURNS:
* OK, or ERROR if this library is not initialized.
*
*/

STATUS pciConfigOutLong
    (
    int	busNo,		/* bus number */
    int	deviceNo,	/* device number */
    int	funcNo,		/* function number */
    int	address,	/* address of the configuration space */
    int data		/* data written to the address */
    )
{
  char txt[100];

  sprintf(txt,"pciConfigOutLong: %d %d %d 0x%08x 0x%08x\r\n",
    busNo,deviceNo,funcNo,address,data);
  sysDebugMsg(txt,CONTINUE_EXECUTION);

    if (pciLibInitStatus != OK)			/* sanity check */
        return (ERROR);

    semTake (&pciMuteSem, WAIT_FOREVER);	/* mutual execlusion start */

    switch (pciConfigMech)
	{
	case PCI_MECHANISM_1:
	    PCI_OUT_LONG (pciConfigAddr0, pciPack (busNo, deviceNo, funcNo) |
		          (address & 0xfc) | 0x80000000);
	    PCI_OUT_LONG (pciConfigAddr1, data);
	    break;

	case PCI_MECHANISM_2:
	    PCI_OUT_BYTE (pciConfigAddr0, 0xf0 | (funcNo << 1));
	    PCI_OUT_BYTE (pciConfigAddr1, busNo);
	    PCI_OUT_LONG ((pciConfigAddr2 | ((deviceNo & 0x000f) << 8) |
			  (address & 0xfc)), data);
	    PCI_OUT_BYTE (pciConfigAddr0, 0);
	    break;

	default:
	    break;
	}

    semGive (&pciMuteSem);			/* mutual execlusion stop */

    return (OK);
    }

/*******************************************************************************
*
* pciSpecialCycle - generate a special cycle with a message
*
* This routine generates a special cycle with a message.
*
* RETURNS:
* OK, or ERROR if this library is not initialized.
*
*/

STATUS pciSpecialCycle
    (
    int	busNo,		/* bus number */
    int message		/* data on AD[31:0] during the special cycle */
    )
    {
    int deviceNo	= 0x0000001f;
    int funcNo		= 0x00000007;

    if (pciLibInitStatus != OK)			/* sanity check */
        return (ERROR);

    semTake (&pciMuteSem, WAIT_FOREVER);	/* mutual execlusion start */

    switch (pciConfigMech)
	{
	case PCI_MECHANISM_1:
	    PCI_OUT_LONG (pciConfigAddr0, pciPack (busNo, deviceNo, funcNo) |
		          0x80000000);
	    PCI_OUT_LONG (pciConfigAddr1, message);
	    break;

	case PCI_MECHANISM_2:
	    PCI_OUT_BYTE (pciConfigAddr0, 0xff);
	    PCI_OUT_BYTE (pciConfigAddr1, 0x00);
	    PCI_OUT_LONG ((pciConfigAddr2 | ((deviceNo & 0x000f) << 8)),
			  message);
	    PCI_OUT_BYTE (pciConfigAddr0, 0);
	    break;

	default:
	    break;
	}

    semGive (&pciMuteSem);			/* mutual execlusion stop */

    return (OK);
    }

/*******************************************************************************
*
* pciInt - interrupt handler for shared PCI interrupt.
*
* This routine executes multiple interrupt handlers for a PCI interrupt.
* Each interrupt handler must check the device dependent interrupt status bit
* to determine the source of the interrupt, since it simply execute all
* interrupt handlers in the link list.
*
* RETURNS: N/A
*
*/

VOID pciInt
    (
    int irq		/* IRQ associated to the PCI interrupt */
    )
    {
    PCI_INT_RTN *pRtn;

    for (pRtn = (PCI_INT_RTN *)DLL_FIRST (&pciIntList[irq]); pRtn != NULL;
	 pRtn = (PCI_INT_RTN *)DLL_NEXT (&pRtn->node))
	(* pRtn->routine) (pRtn->parameter);
    }

/*******************************************************************************
*
* pciIntConnect - connect the interrupt handler to the PCI interrupt.
*
* This routine connects an interrupt handler to the PCI interrupt line(A - D).
* Link list is created if multiple handlers are assigned to the single PCI
* interrupt.
*
* RETURNS:
* OK, or ERROR if the interrupt handler cannot be built.
*
*/

STATUS pciIntConnect
    (
    VOIDFUNCPTR *vector,        /* interrupt vector to attach to     */
    VOIDFUNCPTR routine,        /* routine to be called              */
    int parameter               /* parameter to be passed to routine */
    )
    {
    int irq = IVEC_TO_INUM ((int)vector) - INT_NUM_IRQ0;
    PCI_INT_RTN *pRtn;
    int oldLevel;

    if (pciLibInitStatus != OK)
	return (ERROR);

    pRtn = (PCI_INT_RTN *)malloc (sizeof (PCI_INT_RTN));
    if (pRtn == NULL)
	return (ERROR);

    pRtn->routine   = routine;
    pRtn->parameter = parameter;

    oldLevel = intLock ();			/* LOCK INTERRUPT */
    dllAdd (&pciIntList[irq], &pRtn->node);
    intUnlock (oldLevel);			/* UNLOCK INTERRUPT */

    return (OK);
    }

/*******************************************************************************
*
* pciIntDisconnect - disconnect the interrupt handler from the PCI interrupt.
*
* This routine disconnects the interrupt handler from the PCI interrupt line.
*
* RETURNS:
* OK, or ERROR if the interrupt handler cannot be removed.
*
*/

STATUS pciIntDisconnect
    (
    VOIDFUNCPTR *vector,        /* interrupt vector to attach to     */
    VOIDFUNCPTR routine         /* routine to be called              */
    )
    {
    int irq = IVEC_TO_INUM ((int)vector) - INT_NUM_IRQ0;
    PCI_INT_RTN *pRtn;
    int oldLevel;

    if (pciLibInitStatus != OK)
	return (ERROR);

    for (pRtn = (PCI_INT_RTN *)DLL_FIRST (&pciIntList[irq]); pRtn != NULL;
	 pRtn = (PCI_INT_RTN *)DLL_NEXT (&pRtn->node))
	{
	if (pRtn->routine == routine)
	    {
	    oldLevel = intLock ();			/* LOCK INTERRUPT */
	    dllRemove (&pciIntList[irq], &pRtn->node);
	    intUnlock (oldLevel);			/* UNLOCK INTERRUPT */

	    free ((char *)pRtn);
	    return (OK);
	    }
	}

    return (ERROR);
    }

