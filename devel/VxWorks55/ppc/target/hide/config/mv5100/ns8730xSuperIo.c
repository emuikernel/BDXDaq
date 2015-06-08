/* ns8730xSuperIo.c - National Semiconductor 8730x Super IO initialization */

/* Copyright 1984-1998 Wind River Systems, Inc. */
/* Copyright 1996-2000 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01c,10apr00,dmw  updated copyright and added 97307.
01b,15apr99,srr  Changed to WRS address space naming convention.
01a,12dec97,rhk  created by Motorola.
*/

/*
DESCRIPTION
This module provides the necessary initialization of the National 
Semiconductor 87307, 87308 and 97307 Super I/O chips.  This chip provides 
support for a Floppy Disk Controller, 2 serial ports, a parallel port, 
a real-time clock, keyboard controller and Auxillary I/O.

.CS
    Upon completion, SIO resource registers are mapped as follows:
    Resource    Enabled     Address
    FDC          Yes     PRI     3F0-3F7
    UART1        Yes     COM1    3F8-3FF
    UART2        Yes     COM2    2F8-2FF
    PARALLEL     Yes     LPT1    3BC-3BE
    RTC          Yes             070, 071
    KBC          Yes             060, 064
.CE
*/

/* includes */

#include "ns8730xSuperIo.h"

/* defines */

/* typedefs */

/* globals */

/* forward declarations */

/* externals */

IMPORT UINT8 sysSioRead();
IMPORT void  sysSioWrite();

/*******************************************************************************
*
* ns8730xSuperIoInit - Super I/O initialization
*
* This function performs the necessary initialization for the National
* Semiconductor chips pc87307 and pc87308.
*
* RETURNS: OK or ERROR if chip base address not found.
*/

STATUS ns8730xSuperIoInit (void)
    {
    UINT32	sioBaseAdr;
    UINT8	sioType;
    UINT8	dataIn;

    sioBaseAdr = 0;

    /* 
     * determine the chip type and base address for the super IO.
     * Probe the two possible base addresses that the 307/308 chip 
     * can reside at.
     */

    sioType = sysSioRead( (ISA_MSTR_IO_LOCAL + SUPER_IO_BASE1), SIO_SIDPNP );
    sioType = (sioType & SIO_SIDMASK);
    switch (sioType)
        {
        case SIO_SID87307:
        case SIO_SID87308:
        case SIO_SID97307:
            sioBaseAdr = (ISA_MSTR_IO_LOCAL + SUPER_IO_BASE1);
            break;
        }

    if ( sioBaseAdr == 0 )
        {
        sioType = sysSioRead((ISA_MSTR_IO_LOCAL + SUPER_IO_BASE2), 
                             SIO_SIDPNP );
        sioType = (sioType & SIO_SIDMASK);
        switch (sioType)
            {
            case SIO_SID87307:
            case SIO_SID87308:
            case SIO_SID97307:
                sioBaseAdr = (ISA_MSTR_IO_LOCAL + SUPER_IO_BASE2);
                break;
            }
        }

    /* if no device is present, return an error */

    if ( sioBaseAdr == 0 )
	return (ERROR);

    /* 
     * Do the initialization for the 308 or 307 super IO chip.
     *
     * First, enable the PS/2 mode in configuration register #1
     */

    dataIn = sysSioRead( sioBaseAdr, SIO_CNFG1 );
    dataIn = (dataIn & SIO_CNFG1_MASK);
    sysSioWrite( sioBaseAdr, SIO_CNFG1, dataIn );

    /* do the Keyboard Controller, LUN 0 */

    sysSioWrite( sioBaseAdr, SIO_LUNINDEX, SIO_LUN_KBC );
    sysSioWrite( sioBaseAdr, SIO_ACTIVATE, SIO_LUNDISABLE ); /* disable KBC */
    sysSioWrite( sioBaseAdr, SIO_LUNCNFGR, 0 ); /* 8 MHz clock */
    sysSioWrite( sioBaseAdr, SIO_DBASEHI, 0 );
    sysSioWrite( sioBaseAdr, SIO_DBASELO, SUPER_IO_KBC_BASE_ADR );
    sysSioWrite( sioBaseAdr, SIO_CBASEHI, 0 );
    sysSioWrite( sioBaseAdr, SIO_CBASELO, SUPER_IO_KBC_CMND_ADR );
    sysSioWrite( sioBaseAdr, SIO_ACTIVATE, SIO_LUNENABLE ); /* enable the KBC */

    /* do the Floppy Disk Controller, LUN 3 */

    sysSioWrite( sioBaseAdr, SIO_LUNINDEX, SIO_LUN_FDC );
    sysSioWrite( sioBaseAdr, SIO_IOBASEHI, SUPER_IO_FDC_BASEHI );
    sysSioWrite( sioBaseAdr, SIO_IOBASELO, SUPER_IO_FDC_BASELO );
    sysSioWrite( sioBaseAdr, SIO_LUNCNFGR, SUPER_IO_FDC_PS2 );
    sysSioWrite( sioBaseAdr, SIO_ACTIVATE, SIO_LUNENABLE );

    /* do Serial Port 2 */

    sysSioWrite( sioBaseAdr, SIO_LUNINDEX, SIO_LUN_COM2 );
    sysSioWrite( sioBaseAdr, SIO_IOBASEHI, SUPER_IO_COM2_BASEHI );
    sysSioWrite( sioBaseAdr, SIO_IOBASELO, SUPER_IO_COM2_BASELO );
    sysSioWrite( sioBaseAdr, SIO_ACTIVATE, SIO_LUNENABLE );

    /* do Serial Port 1 */

    sysSioWrite( sioBaseAdr, SIO_LUNINDEX, SIO_LUN_COM1 );
    sysSioWrite( sioBaseAdr, SIO_IOBASEHI, SUPER_IO_COM1_BASEHI );
    sysSioWrite( sioBaseAdr, SIO_IOBASELO, SUPER_IO_COM1_BASELO );
    sysSioWrite( sioBaseAdr, SIO_ACTIVATE, SIO_LUNENABLE );

    /* do the Parallel Port */

    sysSioWrite( sioBaseAdr, SIO_LUNINDEX, SIO_LUN_PP );
    sysSioWrite( sioBaseAdr, SIO_IOBASEHI, SUPER_IO_PP_BASEHI );
    sysSioWrite( sioBaseAdr, SIO_IOBASELO, SUPER_IO_PP_BASELO );
    sysSioWrite( sioBaseAdr, SIO_LUNCNFGR, SUPER_IO_PP_CFG );
#ifdef MCP750
    sysSioWrite( sioBaseAdr, SIO_IRQREQ, SUPER_IO_ENBL_INTS );
#endif
    sysSioWrite( sioBaseAdr, SIO_ACTIVATE, SIO_LUNENABLE );

    return (OK);
    }

