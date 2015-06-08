/* wdbSerial.c - WDB serial communication initialization library */

/* Copyright 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,13sep01,jhw  Fixed sysSerialChanGet return value test (SPR 70225).
01b,22may98,dbt  reworked
01a,11mar98,ms   written
*/

/*
DESCRIPTION
Initializes the serial connection for the WDB agent.

NOMANUAL
*/

/* lower WDB_MTU to SLMTU bytes for serial connection */

#if     WDB_MTU > SLMTU
#undef  WDB_MTU
#define WDB_MTU SLMTU
#endif  /* WDB_MTU > SLMTU */

/******************************************************************************
*
* wdbCommDevInit - initialize the serial connection
*
* This routine initializes the serial connection used by the WDB agent.
*
* RETURNS :
* OK or ERROR if the serial connection can not be initialized.
*
* NOMANUAL
*/

STATUS wdbCommDevInit
    (
    WDB_COMM_IF *	pCommIf,
    char ** 		ppWdbInBuf,
    char **		ppWdbOutBuf
    )
    {
    SIO_CHAN *			pSioChan;	/* serial I/O channel */
    static WDB_SLIP_PKT_DEV	wdbSlipPktDev;	/* SLIP packet device */
    static uint_t		wdbInBuf [WDB_MTU/4];
    static uint_t		wdbOutBuf [WDB_MTU/4];

    /* update input & output buffer pointers */

    *ppWdbInBuf = (char *) wdbInBuf;
    *ppWdbOutBuf = (char *) wdbOutBuf;

    /* update communcation interface mtu */

    wdbCommMtu = WDB_MTU;

    /* a raw serial channel supports task or external agent */

    pSioChan = sysSerialChanGet (WDB_TTY_CHANNEL);

    if (pSioChan == (SIO_CHAN *) ERROR)
	{
	if (_func_printErr != NULL)
	    _func_printErr ("wdbSerialInit error: bad serial channel %d\n",
		WDB_TTY_CHANNEL);
	return (ERROR);
	}

    sioIoctl (pSioChan, SIO_BAUD_SET, (void *)WDB_TTY_BAUD);

#ifdef  INCLUDE_WDB_TTY_TEST
    {
    int	waitChar = 0;
    
    if (WDB_TTY_ECHO)
	waitChar = 0333;

#ifdef	INCLUDE_KERNEL
    /* test in polled mode if the kernel hasn't started */

    if (taskIdCurrent == 0)
	wdbSioTest (pSioChan, SIO_MODE_POLL, waitChar);
    else
	wdbSioTest (pSioChan, SIO_MODE_INT, waitChar);
#else	/* INCLUDE_KERNEL */
    wdbSioTest (pSioChan, SIO_MODE_POLL, waitChar);
#endif	/* INCLUDE_KERNEL */
    }
#endif  /* INCLUDE_WDB_TTY_TEST */

    wdbSlipPktDevInit   (&wdbSlipPktDev, pSioChan, udpRcv);

    if (udpCommIfInit (pCommIf, &wdbSlipPktDev.wdbDrvIf) == ERROR)
	return (ERROR);

    return (OK);
    }
