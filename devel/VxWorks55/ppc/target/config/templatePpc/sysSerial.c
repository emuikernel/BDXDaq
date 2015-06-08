/* sysSerial.c - template BSP serial device initialization */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
TODO -  Remove the template modification history and begin a new history
        starting with version 01a and growing the history upward with
        each revision.

modification history
--------------------
01f,08dec01,dat  fix warning about unused variables
01e,15oct01,dat  new templateSio driver
01d,21sep98,fle  added library description
01c,10mar97,dat  comments from reviewers.
01b,17jan97,dat  added sysSiosChans[], chgs sysSerialChanGet
01a,22oct96,ms   derived from the mv147 BSP
*/

/*
DESCRIPTION
This file contains the board-specific routines for serial channel
initialization.

During development, it is easiest to #include the
generic driver (in this case "src/drv/sio/templateSio.c").
Once the BSP initialization code is working with the generic
driver, it should no longer be #included, but rather should be
linked from the library archives.
*/

#include "vxWorks.h"
#include "config.h"

#include "intLib.h"
#include "iv.h"
#include "sysLib.h"
#include "template.h"

#include "drv/sio/templateSio.h"

/*
 * Sio drivers are delivered in both source and object formats.  You
 * can include it in source form, or link against it in the library.
 */
#include "sio/templateSio.c"

/* static variables */

/*
 * Array of pointers to all serial channels configured in system.
 * See sioChanGet(). It is this array that maps channel pointers
 * to standard device names.  The first entry will become "/tyCo/0",
 * the second "/tyCo/1", and so forth.
 */

#define MAX_SIOS	4
SIO_CHAN * sysSioChans [MAX_SIOS];

/* definitions */

/* Defines for template SIO #0 */

#define TEMPL_SIO_BASE0	0x00100000	/* base addr for device 0 */
#define TEMPL_SIO_VEC0  0x05		/* base vector for device 0 */
#define TEMPL_SIO_LVL0	TEMPL_SIO_VEC0	/* level 5 */
#define TEMPL_SIO_FREQ0 3000000		/* clk freq is 3 MHz */

/******************************************************************************
*
* sysSerialHwInit - initialize the BSP serial devices to a quiescent state
*
* This routine initializes the BSP serial device descriptors and puts the
* devices in a quiescent state.  It is called from sysHwInit() with
* interrupts locked.
*
* RETURNS: N/A
*
* SEE ALSO: sysHwInit()
*/ 

void sysSerialHwInit (void)
    {
    /*
     * TODO - Do any special board-specific init of hardware to shut down
     * any hardware that may be active (dma, interrupting, etc).
     *
     * Any code called here cannot use malloc/free, or do any intConnect
     * type functions.
     */

    }

/******************************************************************************
*
* sysSerialHwInit2 - connect BSP serial device interrupts
*
* This routine connects the BSP serial device interrupts.  It is called from
* sysHwInit2().  
*
* Serial device interrupts cannot be connected in sysSerialHwInit() because
* the  kernel memory allocator is not initialized at that point and
* intConnect() calls malloc().
*
* This is where most device driver modules get called and devices are created.
*
* RETURNS: N/A
*
* SEE ALSO: sysHwInit2()
*/ 

void sysSerialHwInit2 (void)
    {
    SIO_CHAN * pChan;

    /* TODO - create/connect all serial device interrupts */

    /* Create the first instance of a template SIO device */

    pChan = templateSioCreate (TEMPL_SIO_BASE0, TEMPL_SIO_VEC0,
				TEMPL_SIO_LVL0, TEMPL_SIO_FREQ0);

    /* install this device as SIO channel #0 */

    sysSioChans[0] = pChan;

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
    int channel         /* serial channel */
    )
    {
    if (channel < 0
     || channel >= NELEMENTS(sysSioChans) )
	return (SIO_CHAN *) ERROR;

    return sysSioChans[channel];
    }
