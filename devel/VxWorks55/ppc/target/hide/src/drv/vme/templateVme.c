/* templateVme.c - template VMEbus library */

/* Copyright 1984-1997 Wind River Systems, Inc. */
#include "copyright_wrs.h"

/*
modification history
--------------------
TODO -	Remove the template modification history and begin a new history
	starting with version 01a and growing the history upward with
	each revision.

01b,02apr97,dat  chg'd sysBusToLocalAdrs, and sysLocalToBusAdrs
01a,10mar97,dat  written, from nullVme.c, ver 01g
*/

/*
TODO - This is an example driver.  The documentation and code must be
replaced with appropriate information.

DESCRIPTION
Describe the complete device, including features not controlled by this
driver.

Describe the device's VME capabilities, including those not supported or
used by this driver.  Describe the capabilities this driver does support
and any restrictions or limitations imposed.

The functions usually addressed here include:

    Mapping of master and slave windows on the bus.

    Test and set functions on the bus.

    Bus interrupt functions:
	- enable/disable VMEbus interrupt levels
	- generate bus interrupts

    Mailbox/locations monitor functions:
	- enable mailbox/location monitor interrupts

In this example, the following macros control the mapping of VMEbus addresses
to local addresses.  Master accesses are accesses where the local processor
is the bus master.  The one-time setup of the control registers that implements
master windows is normally done in sysHwInit(), in file sysLib.c.

    VME_A16_MSTR_BUS	Master window base (bus addr)
    VME_A16_MSTR_SIZE	Master window size
    VME_A16_MSTR_LOCAL	Local address of window

    VME_A24_MSTR_BUS	Master window base (bus addr)
    VME_A24_MSTR_SIZE	Master window size
    VME_A24_MSTR_LOCAL	Local address of window

    VME_A32_MSTR_BUS	Master window base (bus addr)
    VME_A32_MSTR_SIZE	Master window size
    VME_A32_MSTR_LOCAL	Local address of window

Slave accesses are initiated by some other CPU and the board acts as
the VMEbus slave device.  When a CPU exports it's memory to the bus, it
is creating a slave window on the bus.  In this example driver, the
following macros describe the slave windows on the VMEbus.  The setup
of the slave window control registers is usually a one-time operation
done as part of sysProcNumSet, and only for processor number 0.

    VME_A16_SLV_LOCAL	Local address mapped to window on bus.
    VME_A16_SLV_WINDOW	Bus address of window
    VME_A16_SLV_SIZE	Slave window size

    VME_A24_SLV_LOCAL	Local address mapped to window on bus.
    VME_A24_SLV_WINDOW	Bus address of window
    VME_A24_SLV_SIZE	Slave window size

    VME_A32_SLV_LOCAL	Local address mapped to window on bus.
    VME_A32_SLV_WINDOW	Bus address of window
    VME_A32_SLV_SIZE	Slave window size

This sample driver uses the macros mentioned above to implement the functions
sysBusToLocalAdrs() and sysLocalToBusAdrs().  These are commonly declared
directly in the sysLib.c file rather than a driver file.  Declaring them 
in the driver is preferred.

The macros described above are usually defined in either config.h or
<target>.h.  Those macros that are user changeable should be in config.h, while
those that are fixed and unchangeable should be in <target>.h.  The user
should be able to set any xxxx_SIZE macro to 0 and cause the associated window,
master or slave, to be disabled.

INCLUDES:
vme.h, vxLib.h, sysLib.h
*/

/* This driver contributes nothing, if INCLUDE_VME is not defined. */

#ifdef INCLUDE_VME

/* includes */

#include "vme.h"
#include "vxLib.h"
#include "sysLib.h"

/* locals */

LOCAL FUNCPTR sysMailboxRoutine  = NULL;
LOCAL int sysMailboxArg          = NULL;

/*******************************************************************************
*
* sysIntDisable - disable a bus interrupt level
*
* This routine disables a specified VMEbus interrupt level.  SysHwInit()
* should have disabled all interrupts by default.  It should not be
* necessary to disable individual interrupts following startup.
*
* RETURNS: OK, or ERROR if <intLevel> is not in the range 1 - 7.
*
* SEE ALSO: sysIntEnable()
*/

STATUS sysIntDisable
    (
    int intLevel        /* interrupt level to disable (1-7) */
    )
    {
    if (intLevel < 1 || intLevel > 7)
        return (ERROR);

    /* TODO - Disable the requested interrupt level */

    return (OK);
    }

/*******************************************************************************
*
* sysIntEnable - enable a bus interrupt level
*
* This routine enables a specified VMEbus interrupt level.  At startup,
* sysHwInit() should have disabled all interrupts.  Specific interrupts
* must be enabled if interrupts are expected.
*
* RETURNS: OK, or ERROR if <intLevel> is not in the range 1 - 7.
*
* SEE ALSO: sysIntDisable()
*/

STATUS sysIntEnable
    (
    int intLevel        /* interrupt level to enable (1-7) */
    )
    {
    if (intLevel < 1 || intLevel > 7)
        return (ERROR);

    /* TODO - Enable the requested interrupt level */

    return (OK);
    }

/******************************************************************************
*
* sysBusIntAck - acknowledge a bus interrupt
*
* This routine acknowledges a specified VMEbus interrupt level.
*
* RETURNS: NULL.
*/

int sysBusIntAck
    (
    int intLevel        /* interrupt level to acknowledge */
    )
    {
    /* TODO - perform any special acknowledge cycles */

    return (NULL);
    }

/******************************************************************************
*
* sysBusIntGen - generate a bus interrupt
*
* This routine generates a VMEbus interrupt for a specified level with a
* specified vector.
*
* WARNING:
* The local CPU is sending an interrupt to a remote CPU.  If the local
* CPU has enabled the same VMEbus interrupt level that it is sending, then
* the local CPU will interrupt itself and this sequence will probably fail.
*
* RETURNS: OK or ERROR if unable to generate requested interrupt.
*/

STATUS sysBusIntGen
    (
    int level,          /* bus interrupt level to generate          */
    int vector          /* interrupt vector to return (0-255)       */
    )
    {
    /*
     * TODO - cause interrupt to occur on the VMEbus, present vector
     * during IACK cycle.
     */

    return (ERROR);
    }

/*******************************************************************************
*
* sysMailboxInt - handle mailbox interrupt
*
* Mailbox interrupts usually must be acknowledged.
*
* RETURNS: N/A.
*/

LOCAL void sysMailboxInt (void)

    {
    /* TODO - Acknowledge and reset mailbox interrupt as needed */

    if (sysMailboxRoutine != NULL)
        sysMailboxRoutine (sysMailboxArg);
    }

/******************************************************************************
*
* sysMailboxConnect - connect a routine to the mailbox interrupt
*
* This routine specifies the interrupt service routine to be called at each
* mailbox interrupt.
*
* RETURNS: OK or ERROR, if mailboxes are not supported.
*
* SEE ALSO: sysMailboxEnable()
*/

STATUS sysMailboxConnect
    (
    FUNCPTR routine,    /* routine called at each mailbox interrupt */
    int     arg         /* argument with which to call routine      */
    )
    {
    sysMailboxRoutine   = NULL;
    sysMailboxArg       = arg;
    sysMailboxRoutine   = routine;

    return (OK);
    }

/******************************************************************************
*
* sysMailboxEnable - enable the mailbox interrupt
*
* This routine enables the mailbox interrupt.
*
* RETURNS: OK or ERROR if mailboxes are not supported.
*
* SEE ALSO: sysMailboxConnect()
*/

STATUS sysMailboxEnable
    (
    INT8 *mailboxAdrs           /* mailbox address */
    )
    {
    static BOOL connected = FALSE;

    if (!connected)
	{
	/* TODO - connect the mailbox interrupt */

	connected = TRUE;
	}

    /* TODO - enable mailbox interrupts */

    return (ERROR);
    }

/********************************************************************************
* sysBusTas - test and set a location across the bus
*
* This routine performs a test-and-set (TAS) instruction across the backplane.
*
* RETURNS: TRUE if the value had not been set but is now,
* or FALSE if the value was set already.
*
* SEE ALSO: vxTas()
*/
 
BOOL sysBusTas
    (
    INT8 *addr          /* address to be tested and set */
    )
    {
    /* TODO - Modify as necessary to guarantee atomic read/write operation */

    return (vxTas(addr));       /* RMW cycle */
    }

/********************************************************************************
* sysBusTasClear - clear a test and set location on the bus
*
* This routine clears a bus test and set location.  Usually this routine is
* not even required.  It is required if the sysBusTas() routine uses any
* special semaphore techniques.
*
* If used, the BSP activates this routine by placing its address into the
* global variable 'smUtilTasClearRtn'. e.g.
*
	smUtilTasClearRtn = sysBusTasClear;
*
* RETURNS: N/A.
*/
 
void sysBusTasClear
    (
    INT8 *addr          /* address to be cleared */
    )
    {
    /* TODO - Modify as necessary to guarantee atomic read/write operation */

    *addr = 0;
    }

/*******************************************************************************
*
* sysLocalToBusAdrs - convert a local address to a bus address
*
* This routine gets the VMEbus address that accesses a specified local
* memory address.
*
* RETURNS: OK, or ERROR if the address space is unknown or not mapped. 
*
* SEE ALSO: sysBusToLocalAdrs()
*/
 
STATUS sysLocalToBusAdrs
    (
    int adrsSpace,        /* bus address space in which pBusAdrs resides */
    char * pLocalAdrs,    /* local address to convert                   */ 
    char ** ppBusAdrs     /* where to return bus address                */ 
    )
    {
    /* return error if local memory is not mapped to VMEbus */

    if (sysProcNumGet () != 0)
	return ERROR;

    switch (adrsSpace)
        {
        case VME_AM_EXT_SUP_PGM:
        case VME_AM_EXT_SUP_DATA:
        case VME_AM_EXT_USR_PGM:
        case VME_AM_EXT_USR_DATA:

	    if ((VME_A32_SLV_SIZE != 0) &&
		((ULONG)localAdrs >= VME_A32_SLV_LOCAL) &&
		((ULONG)localAdrs < (VME_A32_SLV_LOCAL + VME_A32_SLV_SIZE)))
		{
                *pBusAdrs = localAdrs + (VME_A32_SLV_BUS - VME_A32_SLV_LOCAL);
                return (OK);
                }
	    return (ERROR);

        case VME_AM_STD_SUP_PGM:
        case VME_AM_STD_SUP_DATA:
        case VME_AM_STD_USR_PGM:
        case VME_AM_STD_USR_DATA:

	    if ((VME_A24_SLV_SIZE != 0) &&
		((ULONG)localAdrs >= VME_A24_SLV_LOCAL) &&
		((ULONG)localAdrs < (VME_A24_SLV_LOCAL + VME_A24_SLV_SIZE)))
		{
                *pBusAdrs = localAdrs + (VME_A24_SLV_BUS - VME_A24_SLV_LOCAL);
                return (OK);
                }
	    return (ERROR);

        case VME_AM_SUP_SHORT_IO:
        case VME_AM_USR_SHORT_IO:

	    if ((VME_A16_SLV_SIZE != 0) &&
		((ULONG)localAdrs >= VME_A16_SLV_LOCAL) &&
		((ULONG)localAdrs < (VME_A16_SLV_LOCAL + VME_A16_SLV_SIZE)))
		{
                *pBusAdrs = localAdrs + (VME_A16_SLV_BUS - VME_A16_SLV_LOCAL);
                return (OK);
                }
	    return (ERROR);
 
        default:
            return (ERROR);
        }
    }

/*******************************************************************************
*
* sysBusToLocalAdrs - convert a bus address to a local address
*
* This routine gets the local address that accesses a specified VMEbus
* memory address.
*
* RETURNS: OK, or ERROR if the address space is unknown or the mapping is not
* possible.
*
* SEE ALSO: sysLocalToBusAdrs()
*/

STATUS sysBusToLocalAdrs
    (
    int adrsSpace,      /* bus address space in which pBusAdrs resides */
    char *pBusAdrs,      /* bus address to convert                     */
    char **ppLocalAdrs   /* where to return local address              */
    )
    {
    switch (adrsSpace)
	{
	case VME_AM_EXT_SUP_PGM:
	case VME_AM_EXT_USR_PGM:
	case VME_AM_EXT_SUP_DATA:
	case VME_AM_EXT_USR_DATA:

	    if ((VME_A32_MSTR_SIZE == 0) ||
		((ULONG) busAdrs < VME_A32_MSTR_BUS) ||
		((ULONG) busAdrs >= (VME_A32_MSTR_BUS + VME_A32_MSTR_SIZE)))
		{
	    	return (ERROR);
		}

	    *pLocalAdrs = (char *)busAdrs +
			    (VME_A32_MSTR_LOCAL - VME_A32_MSTR_BUS);
	    return (OK);

	case VME_AM_STD_SUP_PGM:
	case VME_AM_STD_USR_PGM:
	case VME_AM_STD_SUP_DATA:
	case VME_AM_STD_USR_DATA:

	    if ((VME_A24_MSTR_SIZE == 0) ||
		((ULONG) busAdrs < VME_A24_MSTR_BUS) ||
		((ULONG) busAdrs >= (VME_A24_MSTR_BUS + VME_A24_MSTR_SIZE)))
		{
		return (ERROR);
		}

	    *pLocalAdrs = (char *) busAdrs +
			    (VME_A24_MSTR_LOCAL - VME_A24_MSTR_BUS);
	    return (OK);

        case VME_AM_SUP_SHORT_IO:
        case VME_AM_USR_SHORT_IO:

	    if ((VME_A16_MSTR_SIZE == 0) ||
		((ULONG) busAdrs < VME_A16_MSTR_BUS) ||
		((ULONG) busAdrs >= (VME_A16_MSTR_BUS + VME_A16_MSTR_SIZE)))
		{
		return (ERROR);
		}

            *pLocalAdrs = (char *) busAdrs + 
			    (VME_A16_MSTR_LOCAL - VME_A16_MSTR_BUS);
            return (OK);

	default:
	    return (ERROR);
	}
    }

#endif /*INCLUDE_VME*/
