/* ty534Drv.c - tty driver for the Intel 534 serial controller board */

/* Copyright 1984,1985,1986,1987,1988,1989 Wind River Systems, Inc. */
extern char copyright_wind_river[]; static char *copyright=copyright_wind_river;

/*
modification history
--------------------
03o,30may88,dnw  changed to v4 names.
03n,19nov87,ecs  documentation.
03m,13nov87,gae  changed memAllocate() to malloc().
03l,06nov87,ecs  documentation.
03k,25mar87,jlf  documentation
03j,21dec86,dnw  changed to not get include files from default directories.
03i,03mar86,jlf  changed tyIoctrl to tyIoctl.
03h,21mar86,dnw  added interrupt lock out around baud rate setting to
		   avoid race with char. output.
*/

/*
DESCRIPTION
This is the driver for the Intel 534 serial controller board.

USER CALLABLE ROUTINES
Most of the routines in this driver are accessible only through the I/O
system.  Two routines, however, must be called directly: ty534Drv () to
initialize the driver, and ty534DevCreate to create devices.

TY534DRV
Before using the driver, it must be initialized by calling the routine:
.CS
    ty534Drv ()
.CE
This routine should be called exactly once, before any reads, writes, or
ty534DevCreate's.  Normally, it is called from usrRoot (2) in usrConfig.c.

CREATING TERMINAL DEVICES
Before a terminal can be used, it must be created.
This is done with the ty534DevCreate call.
Each port to be used should have exactly one device associated with it,
by calling this routine.

.CS
 STATUS ty534DevCreate (name, channel, rdBufSize, wBufSize, baudRate)
     char *name;     /* Name to use for this device *
     int channel;    /* Physical channel for this device (0 - 3) *
     int rdBufSize;  /* Read buffer size, in bytes *
     int wBufSize;   /* Write buffer size, in bytes *
     int baudRate;   /* Baud rate to create device with *
.CE

For instance, to create the device "/ty534/0", with buffer sizes of 512 bytes,
at 9600 baud, the proper call would be:
.CS
   ty534DevCreate ("/ty534/0", 0, 512, 512, 9600);
.CE
IOCTL
This driver responds to all the same ioctl codes as a normal ty driver.
All baud rates between 110 and 19200 are available.
*/

#include "vxWorks.h"
#include "tyLib.h"
#include "ioLib.h"
#include "iosLib.h"
#include "memLib.h"

#include "i534.h"


#define IV_534		((FUNCPTR *) 0x0114)	/* Intel 534 bd. 8259 lvl 5 */


typedef struct			/* TY_534_DEV */
    {
    TY_DEV tyDev;
    char *uart_ptr;
    char *pit_ptr;
    } TY_534_DEV;

LOCAL char *uart_ptrs [] =	/* usart character output i/o addresses */
    {
    I534_U0_D,
    I534_U1_D,
    I534_U2_D,
    I534_U3_D,
    };

LOCAL char *pit_ptrs [] =	/* pit counter output i/o addresses */
    {
    I534_PIT0_0,
    I534_PIT0_1,
    I534_PIT0_2,
    I534_PIT1_0,
    };

LOCAL TY_534_DEV *ty534Dv [N_534_CHANNELS];	/* device descriptors */
LOCAL int ty534DrvNum;		/* driver number assigned to this driver */


/* forward declarations */

LOCAL int ty534Open ();
LOCAL int ty534Read ();
LOCAL int ty534Write ();
LOCAL int ty534Ioctl ();
LOCAL int ty534Startup ();
LOCAL VOID ty534Int ();

/*******************************************************************************
*
* ty534Drv - install Intel 534 driver
*
* RETURNS:
*    OK or
*    ERROR if board not present or unable to install driver 
*/

STATUS ty534Drv ()

    {
    static BOOL done;	/* FALSE = not done; TRUE = done */

    if (!done)
	{
	/* probe for 534 hardware */

	if (sysMemProbe (I534_U0_S) < OK)
	    {
	    ty534DrvNum = ERROR;
	    return (ERROR);
	    }

	/* connect to interrupt level and initialize hardware */

	intConnect (IV_534, ty534Int, 0);
	ty534HrdInit ();

	/* install driver */

	ty534DrvNum = iosDrvInstall (ty534Open, (FUNCPTR) NULL, 
				     ty534Open, (FUNCPTR) NULL, 
				     ty534Read, ty534Write, ty534Ioctl);
	}

    return (ty534DrvNum == ERROR ? ERROR : OK);
    }
/*******************************************************************************
*
* ty534DevCreate - create a device for a channel
*/

STATUS ty534DevCreate (name, channel, rdBufSize, wrtBufSize, baudRate)
    char *name;		/* Name to use for this device */
    int channel;	/* Physical channel for this device (0 - 3) */
    int rdBufSize;	/* Read buffer size, in bytes */
    int wrtBufSize;	/* Write buffer size, in bytes */
    int baudRate;	/* Baud rate to create device with */

    {
    STATUS status;
    FAST TY_534_DEV *pTy534Dv;

    if (ty534DrvNum < 1)
	{
	errnoSet (S_ioLib_NO_DRIVER);
	return (ERROR);
	}

    /* allocate device descriptor */

    pTy534Dv = (TY_534_DEV *) malloc (sizeof (TY_534_DEV));

    if (pTy534Dv == NULL)
	return (ERROR);

    /* initialize ty device descriptors and baud rate */

    status = tyDevInit ((TY_DEV_ID) pTy534Dv,
			rdBufSize, wrtBufSize, ty534Startup);

    if (status != OK)
	return (ERROR);

    pTy534Dv->uart_ptr = uart_ptrs [channel];
    pTy534Dv->pit_ptr  = pit_ptrs [channel];

    ty534Ioctl (pTy534Dv, FIOBAUDRATE, baudRate);

    ty534Dv [channel] = pTy534Dv;

    return (iosDevAdd ((DEV_HDR *) pTy534Dv, name, ty534DrvNum));
    }
/*******************************************************************************
*
* ty534HrdInit - initialize the 534 board
*
* This routine initializes the 534 board.
* The initialization is performed as suggested by the Intel 534
* manual, though the terminal characteristics are set to be the
* same as for the Microbar UNIX system.
*/

LOCAL VOID ty534HrdInit ()

    {
    /* Reset the 534 board */

    *I534_RESET = 1;

    /* Select control block, and initialize the PITs.
       PIT counters 0-3 are used as the baud rate counters for
       USART 0-3 respectively. All 4 of those counters are used
       in mode 3.  The other 2 counters are neither initialized nor used. */

    *I534_SEL_C = 1;

    *I534_PIT0_C = PIT_SEL_C_0 + PIT_BOTH_BYTES + PIT_MODE_3;
    *I534_PIT0_C = PIT_SEL_C_1 + PIT_BOTH_BYTES + PIT_MODE_3;
    *I534_PIT0_C = PIT_SEL_C_2 + PIT_BOTH_BYTES + PIT_MODE_3;

    *I534_PIT1_C = PIT_SEL_C_0 + PIT_BOTH_BYTES + PIT_MODE_3;

    /* Set to write to the data block. */

    *I534_SEL_D = 1;

    /* Initialize the 8259 interrupt controller. PIC 0 is set
       for vector interval of 4, but the vector is set to 0,
       since we don't use it anyway. We don't initialize PIC 1,
       since we don't use it. */

    *I534_PIC0_B0 = PIC_ICW1_ID + PIC_INTERVAL_4 + PIC_SINGLE;
    *I534_PIC0_B1 = 0;

    /* Set up the USARTs.  All are set up for x16 clock, no parity,
       8 data bits, 1 stop bit. */

    *I534_U0_S = U_1_STOP + U_8_DBITS + U_X16;	       /* Mode word */
    *I534_U0_S = U_RTS + U_RX_ENABLE + U_TX_ENABLE + U_DTR;  /* Cmnd word */

    *I534_U1_S = U_1_STOP + U_8_DBITS + U_X16;	       /* Mode word */
    *I534_U1_S = U_RTS + U_RX_ENABLE + U_TX_ENABLE + U_DTR;  /* Cmnd word */

    *I534_U2_S = U_1_STOP + U_8_DBITS + U_X16;	       /* Mode word */
    *I534_U2_S = U_RTS + U_RX_ENABLE + U_TX_ENABLE + U_DTR;  /* Cmnd word */

    *I534_U3_S = U_1_STOP + U_8_DBITS + U_X16;	       /* Mode word */
    *I534_U3_S = U_RTS + U_RX_ENABLE + U_TX_ENABLE + U_DTR;  /* Cmnd word */
    } 

/*******************************************************************************
*
* ty534Open - open device
*
* ARGSUSED
*/

LOCAL int ty534Open (pTy534Dv, name, mode)
    TY_534_DEV *pTy534Dv;
    char *name;
    int mode;

    {
    return ((int) pTy534Dv);
    }
/*******************************************************************************
*
* ty534Read - task level read routine for Intel 534 board
*
* This routine fields all read calls to the Intel 534.
* It calls tyRead with a pointer to the appropriate element of ty534Dv.
*/

LOCAL int ty534Read (pTy534Dv, buffer, maxbytes)
    TY_534_DEV *pTy534Dv;
    char *buffer;
    int maxbytes;

    {
    return (tyRead ((TY_DEV_ID) pTy534Dv, buffer, maxbytes));
    }
/*******************************************************************************
*
* ty534Write - task level write routine for Intel 534 board
*
* This routine fields all write calls to the Intel 534.
* It calls tyWrite with a pointer to the appropriate element of ty534Dv.
*/

LOCAL int ty534Write (pTy534Dv, buffer, nbytes)
    TY_534_DEV *pTy534Dv;
    char *buffer;
    int nbytes;

    {
    return (tyWrite ((TY_DEV_ID) pTy534Dv, buffer, nbytes));
    }
/*******************************************************************************
*
* ty534Ioctl - special device control
*/

LOCAL int ty534Ioctl (pTy534Dv, request, arg)
    TY_534_DEV *pTy534Dv;
    int request;	/* request code */
    int arg;		/* some argument */

    {
    int count;
    int oldLevel;

    switch (request)
	{
	case FIOBAUDRATE:

	    count = PIT_CLOCK_RATE / (16 * arg);

	    oldLevel = intLock ();	/* lock out interrupts */

	    *I534_SEL_C = 1;		/* select control block */

	    *pTy534Dv->pit_ptr = LSB (count);	/* set pit counter */
	    *pTy534Dv->pit_ptr = MSB (count);

	    *I534_SEL_D = 1;		/* leave data block selected */

	    intUnlock (oldLevel);	/* re-enable interrupts */

	    return (OK);

	default:
	    return (tyIoctl ((TY_DEV_ID) pTy534Dv, request, arg));
	}
    }

/*******************************************************************************
*
* ty534Int - interrupt level processing
*
* This routine handles an interrupt from the 534 board.  The interrupt
* is decoded and the appropriate routine invoked.  The interrupt is
* terminated with a 'specific EOI'.
*/

LOCAL VOID ty534Int ()

    {
    FAST TY_534_DEV *pTy534Dv;
    FAST char int_level;
    char outchar;

    /* read interrupt type from interrupt controller on 534 board */

    *I534_PIC0_B0 = PIC_OCW3_ID | PIC_POLLING;
    int_level = *I534_PIC0_B0;

    /* if channel requesting, process request */

    if (int_level & PIC_IL_REQUEST)
	{
	int_level &= PIC_IL_MASK;

	pTy534Dv = ty534Dv [int_level >> 1];

	if (pTy534Dv != NULL)
	    {
	    if (int_level & PIC_IL_TX)
		{
		/* transmitter buffer empty - 
		 *   get character to output, if any, from i/o system. */

		if (tyITx ((TY_DEV_ID) pTy534Dv, &outchar) == OK)
		    *pTy534Dv->uart_ptr = outchar;	/* output character */
		}
	    else
		{
		/* received character - get char and give it to i/o system */

		tyIRd ((TY_DEV_ID) pTy534Dv, *pTy534Dv->uart_ptr);
		}
	    }


	/* send specific EOI to interrupting channel */

	*I534_PIC0_B0 = PIC_SEOI | int_level;
	}
    }
/*******************************************************************************
*
* ty534Startup - transmitter startup routine
*
* Call interrupt level character output routine for Intel 534.
*/

LOCAL VOID ty534Startup (pTy534Dv)
    TY_534_DEV *pTy534Dv;		/* ty device to start up */

    {
    char outchar;

    /* any character to send ? */

    if (tyITx ((TY_DEV_ID) pTy534Dv, &outchar) == OK)
	*pTy534Dv->uart_ptr = outchar;	/* output the character */
    }
