/* templateSio.h - header file for template serial driver */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01f,26apr02,dat  Adding cplusplus protection, SPR 75017
01e,29sep01,dat  Rework for adaptor type struct, new API is just
		 create and destroy.
01d,01jul97,db	added registers and defines for modem control. 
01c,24feb97,dat  used SIO_CHAN as first structure element,
		instead of SIO_DRV_FUNCS *.
01b,17dec96,dat added 'options' to chan structure.
01a,02aug95,ms  written.
*/

#ifndef __INCtemplateSioh
#define __INCtemplateSioh

#include "sioLib.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This driver for a generic SIO device only uses 8 bit reads/writes.
 * These are the register offsets for our fictitious device.
 *
 * Control Register	 	0x0	(write)
 * Status Register	 	0x0	(read)
 * Data register		0x1	(read/write)
 * Modem Status Register	0x2	(read)
 * Modem Control Register	0x2	(write)
 * Baud Rate Control Reg.       0x3	(read/write)
 *
 * The template driver uses 3 interrupt vectors.
 * Receive Interrupt		0x0
 * Transmit Interrupt		0x1
 * Error/Modem Interrupt	0x2
 *
 * Other adaptor functions used include:
 * intConnect, intDisconnect.
 */

#define TEMPLATE_CSR_ID		0x0	/* register offsets */
#define TEMPLATE_DATA_ID	0x1
#define TEMPLATE_MSR_ID		0x2
#define TEMPLATE_BAUD_ID	0x3

#define TEMPLATE_RXINT_ID	0x0	/* vector offsets */
#define TEMPLATE_TXINT_ID	0x1
#define TEMPLATE_ERRINT_ID	0x2

/*
 * Here is the real API for creating and destroying an instance of a template
 * SIO device.  Use the standard sioLib functions
 * for manipulating the device.  The only really device specific
 * functions are for creation and destruction.
 */

IMPORT SIO_CHAN * templateSioCreate /* create an instance */
	(
	UINT32 ioBase,		/* device adaptor specific data */
	UINT32 vecBase,		/* vector base number */
	UINT32 intLevel,	/* interrupt level for vectors */
	UINT32 clkFreq		/* device clock frequency */
	);

IMPORT STATUS templateSioDestroy /* destroy an instance */
	(SIO_CHAN *);		/* must be a template SIO type object */

#ifdef __cplusplus
}
#endif

#endif  /* __INCtemplateSioh */
