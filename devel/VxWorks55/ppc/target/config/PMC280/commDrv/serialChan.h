/******************************************************************************
*              (c), Copyright, FORCE COMPUTERS INDIA Limited                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF FORCE COMPUTERS, INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF FORCE OR ANY THIRD PARTY. FORCE RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO FORCE COMPUTERS.       *
* THIS CODE IS PROVIDED "AS IS". FORCE COMPUTERS MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

*******************************************************************************
* serialChan.h - Header File for : MV device serial channel
*
* DESCRIPTION:
*		This file is the header file for MV device serial channel. It includes 
*		definitions, typedefs and function declarations for the serial
*		channel module.
*
* DEPENDENCIES:
*		None.
*
******************************************************************************/

#ifndef __INCserialChanh
#define __INCserialChanh

/* includes */
#include "gtCore.h"
#include "mpsc.h"
#include "sdma.h"
#include "brg.h"
#include "vxMpscIntCtrl.h"
#include "vxSdmaIntCtrl.h"
#include "vxCunitIntCtrl.h"


#ifdef __cplusplus
extern "C" {
#endif

#if ((CPU_FAMILY==I960) && (defined __GNUC__))
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif	/* CPU_FAMILY==I960 */


/* typedefs */

typedef struct _serialChannel
{
	int				baudRate;
	MPSC_CHANNEL  	mpscChan;
	SDMA_CHANNEL	sdmaChan;
	BRG_ENGINE		brg;
} SERIAL_CHANNEL;


/* serialChan.h API list */
bool serialChanInit 		(SERIAL_CHANNEL	*serialChan);
bool serialChanStart		(SERIAL_CHANNEL	*serialChan);
void serialChanStop			(SERIAL_CHANNEL	*serialChan);
bool serialChanSetBaudRate	(SERIAL_CHANNEL	*serialChan, unsigned int baudRate);



#ifdef __cplusplus
}
#endif

#endif /* __INCserialChan.h */
