/******************************************************************************
*              (c), Copyright, MOTOROLA  INC  - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA  OR ANY THIRD PARTY. MOTOROLA  RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

*******************************************************************************
* vxMpscUart.h - Header File for : MV UART channel
*
* DESCRIPTION:
*		This file is the header file for MV device serial channel operating in
*		UART protocol. It includes definitions, typedefs and function 
*		declarations for the MV serial UART channel module.
*
* DEPENDENCIES:
*		None.
*
******************************************************************************/

#ifndef __INCvxMpscUarth
#define __INCvxMpscUarth

/* includes */
#include "vxWorks.h"
#include "serialChan.h"

#ifdef __cplusplus
extern "C" {
#endif

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */


/* defines  */
#define BAUD_RATE_9600		0x00010050
#define BAUD_RATE_19200		0x00010027
#define BAUD_RATE_38400		0x00010013
#define BAUD_RATE_57600		0x0001000c
#define BAUD_RATE_115200	0x00010006
/* Default channel parameters */
#define DEFAULT_TX_DESC_NUM		0x01
#define DEFAULT_RX_DESC_NUM		0x10
#define DEFAULT_TX_BUF_SIZE		0x01
#define DEFAULT_RX_BUF_SIZE		0x01


/* typedefs */
typedef struct mv_sio_chan
{
    /* always goes first */
    SIO_DRV_FUNCS	*pDrvFuncs;			/* driver functions 			*/
    
    /* callbacks */

    STATUS	(*getTxChar)();				/* pointer to a xmitr function 	*/
    STATUS	(*putRcvChar)();			/* pointer to a recvr function 	*/
    void *  getTxArg;
    void *  putRcvArg;

    char 		   *txBufBase;     /* Channel's Tx buffer base  */
    volatile INT16 channelMode;	   /* SIO_MODE                 	*/
    int            baudRate;   	   /* Serial channel baud rate 	*/
    SERIAL_CHANNEL serialChan;     /* UART MPSC channel         */
} MV_SIO_CHAN;

/* vxMpscUart.h API list */
void vxMpscUartDevReset(MV_SIO_CHAN *);
void vxMpscUartDevInit (MV_SIO_CHAN *);
void vxMpscUartRxInt   (MV_SIO_CHAN *);
void vxMpscUartTxInt   (MV_SIO_CHAN *);

#ifdef __cplusplus
}
#endif

#endif /* __INCvxMpscUarth */

