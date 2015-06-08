/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

*******************************************************************************
* sysMpscSerial.c - BSP MPSC as serial device initialization
*
* DESCRIPTION:
*       The sysMpscSerial.c file is normally included as part of the sysLib.c
*       This code segment configures the serial ports for the BSP.
*       This BSP can support up to two MPSC channels in UART mode only.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

/* includes */
#include "vxWorks.h"
#include "config.h"
#include "vxMpscUart.h"
#include "ioLib.h"

#ifdef PMC280_DEBUG_UART_VX
#include "dbgprints.h"
#endif /* PMC280_DEBUG_UART_VX */


/* defines */


/* typedefs */


/* locals   */
static UINT8	chanNum = NUM_TTY;	/* number of channels actually used */
static MV_SIO_CHAN	mvSioChan [NUM_TTY];

/*******************************************************************************
* sysSerialHwInit - Initialize the BSP serial devices to a quiesent state
*
* DESCRIPTION:
*       This routine initializes the BSP serial device and puts the devices in 
*       a quiesent state.  It is called from sysHwInit() with interrupts locked.
*       The user can define his own channel memory parameters. However, 
*       defining the Tx descriptor number as zero will cause the driver to 
*       allocate the driver memory according to default parameters.
*       Both MPSC channels are initialized if the variable chanNum equals two. 
*       In this case a serial channel 1 is consist of MPSC1 SDMA1 BRG1 and 
*       serial channel 2 is consist of MPSC2 SDMA2 and BRG2. 
*
* INPUT:
*       None.
*
* OUTPUT:
*       UART device driver SW struct initialized. Serial channel is halted and 
*       SDMA interrupts are cleared and disabled.
*
* RETURN:
*       None.
*
*******************************************************************************/
void sysSerialHwInit (void)
    {
    int		ix=0;		/* an index */

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Entering sysSerialHwInit\n");
    }
    else
    {
        dbg_puts0("CPU0: Entering sysSerialHwInit\n");
    }
#else
     dbg_puts0("Entering sysSerialHwInit\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

#ifdef PMC280_DUAL_CPU 

    /*
     * The BSP will use MPSC 0 as CPU0 console and MPSC 1 as CPU1 console
     */
    if(frcWhoAmI())
        ix = 1;
    else  
        ix = 0;

    mvSioChan [ix].baudRate = CONSOLE_BAUD_RATE;
	
    /* Use BRG0 for channel 0 and BRG1 for channel 1 */
    mvSioChan [ix].serialChan.brg.brgNum        = ix;
	
    /* BRGCLK freq (Hz) */
    mvSioChan [ix].serialChan.brg.brgSrcClkRate = BCLKIN_RATE;
		
    /* BRG Clock source */
    mvSioChan [ix].serialChan.brg.brgSrcClk 	= BCLKIN;
			
    /* MPSC channel applied for UART protocol */
    mvSioChan [ix].serialChan.mpscChan.mpscNum  = ix;
	
    /* SDMA applied for MPSC channel */
    mvSioChan [ix].serialChan.sdmaChan.sdmaNum  = ix;
	
    /* Let the driver allocate the necessary memory itself */
    mvSioChan [ix].serialChan.sdmaChan.txDescNum = 0;
		
    /* Initialize the MV serial device */
    vxMpscUartDevInit(&(mvSioChan [ix]));
 #else
    /* Intialize the chips device descriptors */
    for (ix = 0; ix < chanNum; ix++)
    {
		
	mvSioChan [ix].baudRate = CONSOLE_BAUD_RATE;
	
	/* Use BRG0 for channel 0 and BRG1 for channel 1 */
	mvSioChan [ix].serialChan.brg.brgNum        = ix;
	
	/* BRGCLK freq (Hz) */
	mvSioChan [ix].serialChan.brg.brgSrcClkRate = BCLKIN_RATE;
		
	/* BRG Clock source */
	mvSioChan [ix].serialChan.brg.brgSrcClk 	= BCLKIN;  
			
	/* MPSC channel applied for UART protocol */
	mvSioChan [ix].serialChan.mpscChan.mpscNum  = ix;
	
	/* SDMA applied for MPSC channel */
	mvSioChan [ix].serialChan.sdmaChan.sdmaNum  = ix;
	
	/* Let the driver allocate the necessary memory itself */
	mvSioChan [ix].serialChan.sdmaChan.txDescNum = 0;
	



	/* Initialize the MV serial device */
	vxMpscUartDevInit(&(mvSioChan [ix]));
	
	
    }
#endif /* PMC280_DUAL_CPU */

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Exiting sysSerialHwInit\n");
    }
    else
    {
        dbg_puts0("CPU0: Exiting sysSerialHwInit\n");
    }
#else
     dbg_puts0("Exiting sysSerialHwInit\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */
}

/*******************************************************************************
* sysSerialHwInit2 - Connect serial device interrupts and initiate it.
*
* DESCRIPTION:
*       This routine connects the BSP serial device interrupts and initialize 
*       the device. Serial device interrupts could not be connected
*       in sysSerialHwInit() because the kernel memory allocator was not 
*       initialized at that point, and intConnect() calls malloc(). Device 
*       memory area for descriptors and buffers can not be allocated in 
*       sysSerialHwInit() as well for the same reasons. This is why serial 
*       device routine is called from this routine
*
* INPUT:
*       None.
*
* OUTPUT:
*       Serial channels are initiated and ready to operate.
*
* RETURN:
*       None.
*
*******************************************************************************/
void sysSerialHwInit2 (void)
{
    int		ix;	/* an index */
#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Entering sysSerialHwInit2\n");
    }
    else
    {
        dbg_puts0("CPU0: Entering sysSerialHwInit2\n");
    }
#else
     dbg_puts0("Entering sysSerialHwInit2...\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */

#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
        ix = 1;
    else
        ix = 0;

   /* connect serial interrupts */
   vxSdmaIntConnect(ix, SDMA_RX_BUF, (VOIDFUNCPTR) vxMpscUartRxInt, 
                   (int) &mvSioChan [ix], 0);
        
   vxSdmaIntConnect(ix, SDMA_TX_BUF, (VOIDFUNCPTR) vxMpscUartTxInt, 
                   (int) &mvSioChan [ix], 0);
#else
    for (ix = 0; ix < chanNum; ix++)
    {        
        /* connect serial interrupts */
	vxSdmaIntConnect(ix, SDMA_RX_BUF, (VOIDFUNCPTR) vxMpscUartRxInt, 
			 (int) &mvSioChan [ix], 0);
        
	vxSdmaIntConnect(ix, SDMA_TX_BUF, (VOIDFUNCPTR) vxMpscUartTxInt, 
			 (int) &mvSioChan [ix], 0);
    }
#endif /* PMC280_DUAL_CPU */

#ifdef PMC280_DEBUG_UART_VX
#ifdef PMC280_DUAL_CPU
    if(frcWhoAmI())
    {
        dbg_puts1("CPU1: Exiting sysSerialHwInit2\n");
    }
    else
    {
        dbg_puts0("CPU0: Exiting sysSerialHwInit2\n");
    }
#else
     dbg_puts0("Exiting sysSerialHwInit2...\n");
#endif /* PMC280_DUAL_CPU */
#endif /* PMC280_DEBUG_UART_VX */
}

/*******************************************************************************
* sysSerialChanGet - Get the SIO_CHAN device associated with a serial channel
*
* DESCRIPTION:
*       This routine gets the SIO_CHAN device associated with a specified 
*       serial channel.
*
* INPUT:
*       int channel Serial channel number
*
* OUTPUT:
*       None.
*
* RETURN:
*       SIO channel structure defining the channel number or ERROR if the 
*       channel number is invalid.
*
*******************************************************************************/
SIO_CHAN * sysSerialChanGet(int channel)
{
    if (channel >= chanNum)
    	return ((SIO_CHAN *) ERROR);

    return ((SIO_CHAN *) &mvSioChan[channel]);
}

