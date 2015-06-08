/* sysSerial.c - BSP serial device initialization */

/* Copyright 1984-1997,1999-2001 Wind River Systems, Inc. */
/* Copyright 1999-2001 Motorola, Inc. All Rights Reserved */

#include "copyright_wrs.h"

/*
modification history
--------------------
01b,12sep00,dmw  Added PrPMC800XT support.
01a,31aug00,dmw  Written (from version 01a of prpmc750/sysSerial.c).
*/

/*
The sysSerial.c file is normally included as part of the sysLib.c file.
This code segment configures the serial ports for the BSP.

This BSP can support a single I8250 DUSART and the onboard SCC channels
of the Z85230 ESCC.  Either, both, or none may be configured into the
system.
*/

#include "vxWorks.h"
#include "iv.h"
#include "intLib.h"
#include "config.h"
#include "sysLib.h"
#include "drv/sio/i8250Sio.h"

/* externals */

IMPORT UCHAR	sysInByte(ULONG);
IMPORT void	sysOutByte(ULONG, UCHAR);
IMPORT int	intEnable (int intLevel);

/* device initialization structures */

#ifdef INCLUDE_I8250_SIO
typedef struct
    {
    USHORT vector;			/* Interrupt vector */
    ULONG  baseAdrs;			/* Register base address */
    USHORT regSpace;			/* Address Interval */
    USHORT intLevel;			/* Interrupt level */
    } I8250_CHAN_PARAS;
#endif /*INCLUDE_I8250_SIO*/

/* Local data structures */

#ifdef INCLUDE_I8250_SIO
static I8250_CHAN  i8250Chan[N_UART_CHANNELS];

static I8250_CHAN_PARAS devParas[] = 
    {
       {COM1_INT_VEC, COM1_BASE_ADR, UART_REG_ADDR_INTERVAL, COM1_INT_LVL},
#if (N_UART_CHANNELS > 1)
       {COM2_INT_VEC, COM2_BASE_ADR, UART_REG_ADDR_INTERVAL, COM2_INT_LVL}
#endif
    };
#endif /*INCLUDE_I8250_SIO*/

#define UART_REG(reg,chan) \
		(devParas[chan].baseAdrs + reg * devParas[chan].regSpace)

/* globals */

/*
 * sysSioChans - Array of pointers to all serial channels configured in system.
 *
 * Channel pointer position in this table determines the device name
 * under vxWorks.  The first pointer points to the device for /tyCo/0,
 * the second to /tyCo/1, etc.  See sysSerialChanGet().
 */

SIO_CHAN * sysSioChans [N_SIO_CHANNELS] =
    {
#ifdef INCLUDE_I8250_SIO
    (SIO_CHAN *)&i8250Chan[0].pDrvFuncs,	/* /tyCo/0 */
#   ifdef INCLUDE_PRPMC800XT                /* Extended PrPMC800 */
    (SIO_CHAN *)&i8250Chan[1].pDrvFuncs     /* /tyCo/1 */
#   endif /* INCLUDE_PRPMC800XT */
#endif /*INCLUDE_I8250_SIO*/
    };


/******************************************************************************
*
* sysSerialHwInit - initialize the BSP serial devices to a quiescent state
*
* This routine initializes the BSP serial device descriptors and puts the
* devices in a quiescent state.  It is called from sysHwInit() with
* interrupts locked.  Polled mode serial operations are possible, but not
* interrupt mode operations which are enabled by sysSerialHwInit2().
*
* RETURNS: N/A
*
* SEE ALSO: sysHwInit(), sysSerialHwInit2()
*/

void sysSerialHwInit (void)
    {
#ifdef INCLUDE_I8250_SIO
    int i;

    for (i = 0; i < N_UART_CHANNELS; i++)
        {
	i8250Chan[i].int_vec	 = devParas[i].vector;
	i8250Chan[i].channelMode = 0;
	i8250Chan[i].lcr	 = UART_REG(UART_LCR, i);
	i8250Chan[i].data	 = UART_REG(UART_RDR, i);
	i8250Chan[i].brdl	 = UART_REG(UART_BRDL, i);
	i8250Chan[i].brdh	 = UART_REG(UART_BRDH, i);
	i8250Chan[i].ier	 = UART_REG(UART_IER, i);
	i8250Chan[i].iid	 = UART_REG(UART_IID, i);
	i8250Chan[i].mdc	 = UART_REG(UART_MDC, i);
	i8250Chan[i].lst	 = UART_REG(UART_LST, i);
	i8250Chan[i].msr	 = UART_REG(UART_MSR, i);
	i8250Chan[i].outByte	 = (void (*) (int, char))  sysOutByte;
	i8250Chan[i].inByte	 = (UINT8 (*) ()) sysInByte;

	i8250HrdInit (&i8250Chan[i]);
        }
#endif /*INCLUDE_I8250_SIO*/
    }

/******************************************************************************
*
* sysSerialHwInit2 - connect BSP serial device interrupts
*
* This routine connects the BSP serial device interrupts.  It is called from
* sysHwInit2().  
* 
* Serial device interrupts cannot be connected in sysSerialHwInit() because
* the kernel memory allocator is not initialized at that point, and
* intConnect() calls malloc().
*
* RETURNS: N/A
*
* SEE ALSO: sysHwInit2()
*/

void sysSerialHwInit2 (void)
    {
#ifdef INCLUDE_I8250_SIO
    int i;

    /* connect serial interrupts */

     for (i = 0; i < N_UART_CHANNELS; i++)
         if (i8250Chan[i].int_vec)
	     {
             (void) intConnect (INUM_TO_IVEC ((int)i8250Chan[i].int_vec),
				i8250Int, (int)&i8250Chan[i] );

             intEnable (devParas[i].intLevel); 
             }
#endif /*INCLUDE_I8250_SIO*/
    }


/******************************************************************************
*
* sysSerialChanGet - get the SIO_CHAN device associated with a serial channel
*
* This routine returns a pointer to the SIO_CHAN device associated
* with a specified serial channel.  It is called by usrRoot() to obtain 
* pointers when creating the system serial devices, `/tyCo/x'.  It
* is also used by the WDB agent to locate its serial channel.
*
* RETURNS: A pointer to the SIO_CHAN structure for the channel, or ERROR
* if the channel is invalid.
*/

SIO_CHAN * sysSerialChanGet
    (
    int channel		/* serial channel */
    )
    {
    if (channel < 0 || channel >= NELEMENTS(sysSioChans))
        return (SIO_CHAN *)ERROR;

    return sysSioChans[channel];
    }

/******************************************************************************
*
* sysSerialReset - reset all serial devices to a quiescent state
*
* This routine resets all serial devices to a quiescent state.  It is called 
* by sysToMonitor().
*
* RETURNS: N/A
*
* SEE ALSO: sysToMonitor()
*/

void sysSerialReset (void)
    {
#ifdef INCLUDE_I8250_SIO
    int i;

    for (i = 0; i < N_UART_CHANNELS; i++)
        i8250HrdInit (&i8250Chan[i]);
#endif /*INCLUDE_I8250_SIO*/
    }

