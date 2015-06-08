/* wdbTyco.c - WDB serial communication initialization library */

/* Copyright 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,22may98,dbt  reworked
01a,11mar98,ms   written
*/

/*
DESCRIPTION
For obsolete tyCoDrv style serial drivers.

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
* This routine initializes the serial connection for tyCoDrv style serial
* drivers used by the WDB agent.
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
    static WDB_TYCO_SIO_CHAN	tyCoSioChan;	/* serial I/O device */
    static WDB_SLIP_PKT_DEV	wdbSlipPktDev;	/* SLIP packet device */
    static uint_t		wdbInBuf [WDB_MTU/4];
    static uint_t		wdbOutBuf [WDB_MTU/4];

    /* update input & output buffer pointers */

    *ppWdbInBuf = (char *) wdbInBuf;
    *ppWdbOutBuf = (char *) wdbOutBuf;

    /* update communication interface mtu */

    wdbCommMtu = WDB_MTU;

    /* an old VxWorks serial driver - supports a task agent */

    wdbTyCoDevInit (&tyCoSioChan, WDB_TTY_DEV_NAME, WDB_TTY_BAUD);

#ifdef  INCLUDE_WDB_TTY_TEST
    wdbSioTest ((SIO_CHAN *)&tyCoSioChan, SIO_MODE_INT, 0);
#endif  /* INCLUDE_WDB_TTY_TEST */

    wdbSlipPktDevInit (&wdbSlipPktDev, (SIO_CHAN *)&tyCoSioChan, udpRcv);

    if (udpCommIfInit (pCommIf, &wdbSlipPktDev.wdbDrvIf) == ERROR)
	return (ERROR);

    return (OK);
    }
