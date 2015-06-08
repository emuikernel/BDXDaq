/* sysSerial.c - BSP serial device initialization */

/* Copyright 1984-1997,1999 Wind River Systems, Inc. */
/* Copyright 1999-2003 Motorola, Inc. All Rights Reserved */

#include "copyright_wrs.h"

/*
modification history
--------------------
01a,02apr03,simon  Ported. from ver 01a, mcpm905/sysSerial.c.
*/

/*
The sysSerial.c file is normally included as part of the sysLib.c file.
This code segment configures the serial ports for the BSP.
*/

#include "vxWorks.h"
#include "iv.h"
#include "intLib.h"
#include "config.h"
#include "sysLib.h"
#include "mv64360UartDrv.h"

/* externals */

IMPORT int	intEnable (int intLevel);

/* device initialization structures */

/* Local data structures */

static mv64360Serial_CHAN	mvSioChan [NUM_TTY];

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
    (SIO_CHAN *)&mvSioChan[0].pDrvFuncs,	/* /tyCo/0 */
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
    int i;

    for (i = 0; i < N_UART_CHANNELS; i++)
        {
        #if 1
		mvSioChan [i].baudRate = CONSOLE_BAUD_RATE;
		
		/* Use BRG0 for channel 0 and BRG1 for channel 1 */
		mvSioChan [i].BRGNum        = 0;
		mvSioChan [i].MPSCNum        = 0;
		mvSioChan [i].sysCLK        = BUS_RATE_133;
		
		/* Initialize the MV serial device */
		mv64360UartDrvInit(&(mvSioChan [i]));
		#endif
        }
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
    int i;

    /* connect serial interrupts */

     for (i = 0; i < N_UART_CHANNELS; i++)
     	{
             (void) intConnect ((INUM_TO_IVEC(MPSC0_INT_VEC + i)),
				mv64360UartDrvInt, (int)&mvSioChan[i] );

             intEnable (MPSC0_INT_VEC + i); 
     	}
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
    if (channel < 0 || (UINT)channel >= NELEMENTS(sysSioChans))
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
}
