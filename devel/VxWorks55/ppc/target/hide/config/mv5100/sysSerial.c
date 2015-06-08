/* sysSerial.c - BSP serial device initialization */

/* Copyright 1984-2001 Wind River Systems, Inc. */
/* Copyright 1999-2001 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01d,08mar01,rhk  Additional code cleanup.
01c,16aug00,dmw  Cleanup following Wind River coding standards review.
01b,30mar00,dmw  Added UART register offsets for SIO and Zilog parts.
01a,13mar00,dmw  Written (from version 01b of mcpn765/sysSerial.c).
*/

/*
The sysSerial.c file is normally included as part of the sysLib.c file.
This code segment configures the serial ports for the BSP.

This BSP can support the two 16550 UARTS connected via the Hawk's X bus, a 
single I8250 DUSART (on the SuperI/O chip) and the onboard SCC channels
of the Z85230 ESCC.  All, X-Bus UARTS or none may be configured into the
system.  The file used the data structure I8250_CHAN for the X-bus UARTS.
*/

#include "vxWorks.h"
#include "iv.h"
#include "intLib.h"
#include "config.h"
#include "sysLib.h"
#include "drv/sio/i8250Sio.h"
#ifdef INCLUDE_I8250_SIO
    #include "drv/sio/z8530Sio.h"
#endif /* INCLUDE_I8250_SIO */

/* externals */

IMPORT UCHAR	sysInByte(ULONG);
IMPORT void	sysOutByte(ULONG, UCHAR);
IMPORT int	intEnable (int intLevel);

/* device initialization structures */

typedef struct
    {
        USHORT vector;      /* Interrupt vector */
        ULONG  baseAdrs;    /* Register base address */
        USHORT regSpace;    /* Address Interval */
        USHORT intLevel;    /* Interrupt level */
    } 
    UART_CHAN_PARAS;

/* Local data structures */

    static I8250_CHAN  i8250Chan[N_UART_CHANNELS];

    static UART_CHAN_PARAS devParas[] = 
        {
           {COM1_INT_VEC, COM1_BASE_ADR, UART_REG_ADDR_INTERVAL1, COM1_INT_LVL},
           {COM2_INT_VEC, COM2_BASE_ADR, UART_REG_ADDR_INTERVAL1, COM2_INT_LVL},
#ifdef INCLUDE_I8250_SIO
           {COM3_INT_VEC, COM3_BASE_ADR, UART_REG_ADDR_INTERVAL2, COM3_INT_LVL},
           {COM4_INT_VEC, COM4_BASE_ADR, UART_REG_ADDR_INTERVAL2, COM4_INT_LVL}
#endif /* INCLUDE_I8250_SIO */
        };

#define UART_REG(reg,chan) \
		(devParas[chan].baseAdrs + reg * devParas[chan].regSpace)

#ifdef INCLUDE_Z85230_SIO
    static Z8530_DUSART z85230Escc;
#endif /*INCLUDE_Z85230_SIO*/

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
    (SIO_CHAN *)&i8250Chan[0].pDrvFuncs,	/* /tyCo/0 */
    (SIO_CHAN *)&i8250Chan[1].pDrvFuncs,        /* /tyCo/1 */
#ifdef INCLUDE_I8250_SIO
    (SIO_CHAN *)&i8250Chan[2].pDrvFuncs,        /* /tyCo/2 */
    (SIO_CHAN *)&i8250Chan[3].pDrvFuncs,        /* /tyCo/3 */
#endif /*INCLUDE_I8250_SIO*/

#ifdef INCLUDE_Z85230_SIO
    (SIO_CHAN *)&z85230Escc.portA.pDrvFuncs,	/* /tyCo/4 */
    (SIO_CHAN *)&z85230Escc.portB.pDrvFuncs,	/* /tyCo/5 */
#endif /*INCLUDE_Z85230_SIO*/
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

#ifdef INCLUDE_Z85230_SIO
    /* intialize the chips device descriptors */

    z85230Escc.portA.cr       = (char *) z85230_PORTA_CTRL;
    z85230Escc.portA.dr       = (char *) z85230_PORTA_DATA;
    z85230Escc.portA.baudFreq = BAUD_CLK_FREQ;
    z85230Escc.portA.writeReg11 = (SCC_WR11_RX_BR_GEN | SCC_WR11_TX_BR_GEN |
                                   SCC_WR11_OUT_BR_GEN);
    z85230Escc.portA.writeReg14 = (SCC_WR14_BR_EN | SCC_WR14_BR_SRC |
                                   SCC_WR14_SRC_BR);
    z85230Escc.portA.intType  = SCC_WR9_VIS;
    z85230Escc.portA.intVec   = ESCC_INTERRUPT_BASE;

    z85230Escc.portB.cr       = (char *) z85230_PORTB_CTRL;
    z85230Escc.portB.dr       = (char *) z85230_PORTB_DATA;
    z85230Escc.portB.baudFreq = BAUD_CLK_FREQ;
    z85230Escc.portB.writeReg11 = (SCC_WR11_RX_BR_GEN | SCC_WR11_TX_BR_GEN |
                                   SCC_WR11_OUT_BR_GEN);
    z85230Escc.portB.writeReg14 = (SCC_WR14_BR_EN | SCC_WR14_BR_SRC |
                                   SCC_WR14_SRC_BR);
    z85230Escc.portB.intType  = SCC_WR9_VIS;
    z85230Escc.portB.intVec   = ESCC_INTERRUPT_BASE;

    /* reset the chip */

    z8530DevInit (&z85230Escc);
#endif /*INCLUDE_Z85230_SIO*/
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
         if (i8250Chan[i].int_vec)
             {
             (void) intConnect (INUM_TO_IVEC ((int)i8250Chan[i].int_vec),
                                i8250Int, (int)&i8250Chan[i] );

             intEnable (devParas[i].intLevel); 
             }

#ifdef INCLUDE_Z85230_SIO
    (void)intConnect ((VOIDFUNCPTR *)(Z85230_INT_VEC), z8530Int,
		      (int)&z85230Escc);
    intEnable (Z85230_INT_LVL);
#endif /*INCLUDE_Z85230_SIO*/

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
    int i;

    for (i = 0; i < N_UART_CHANNELS; i++)
        i8250HrdInit (&i8250Chan[i]);

#ifdef INCLUDE_Z85230_SIO
    z8530DevInit (&z85230Escc);
#endif /*INCLUDE_Z85230_SIO*/
    }

