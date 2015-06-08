/* usrPcmcia.c - PCMCIA initialization */

/* Copyright 1992-1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,28feb95,hdn  cleaning up.
01a,29jun95,hdn  written.
*/

/*
DESCRIPTION
This file is used to configure and initialize the VxWorks PCMCIA support.
This file is included by bootConfig.c and usrConfig.c.

SEE ALSO: usrExtra.c

NOMANUAL
*/

#ifndef  __INCusrPcmcia
#define  __INCusrPcmcia

/* imports */

IMPORT PCMCIA_CTRL pcmciaCtrl;


/*******************************************************************************
*
* usrPcmciaConfig - mount a DOS file system from a PCMCIA block device
*
* This routine mounts a DOS file system from a PCMCIA block device.
*
* The <sock> parameter is the socket number of the PCMCIA.
*
* The <fileName> parameter is the mount point, e.g., `/pc0/'.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO:
* .pG "I/O System, Local File Systems, Intel i386/i486 Appendix"
*/

STATUS usrPcmciaConfig
    (
    int     sock,	/* 0 - n: socket number */
    char *  fileName	/* mount point */
    )

    {
    PCMCIA_CTRL *pCtrl = &pcmciaCtrl;
    PCMCIA_CARD *pCard = &pCtrl->card[sock];
    PCMCIA_CHIP *pChip = &pCtrl->chip;
    char bootDir [BOOT_FILE_LEN];


    if ((UINT)sock >= pChip->socks)
	{
	printErr ("sock is out of range (0-%d).\n", pChip->socks - 1);
	return (ERROR);
	}

    if (pCard->pBlkDev == NULL)
	return (ERROR);

    /* split off boot device from boot file */

    devSplit (fileName, bootDir);

    /* Make DOS file system */

    if (dosFsDevInit(bootDir, pCard->pBlkDev, NULL) == (DOS_VOL_DESC *)NULL)
        {
	printErr ("Error during dosFsDevInit: %x\n", errno);
        return (ERROR);
        }

    return (OK);
    }

/*******************************************************************************
*
* pcmciaattach - attach a PCMCIA network device
*
* This routine attach a PCMCIA network device.
*
* RETURNS: OK or ERROR.
*
*/

STATUS pcmciaattach
    (
    int	unit,		/* unit number */
    int	arg1,		/* socket number */
    int	arg2,		/* reserved */
    int	arg3,		/* reserved */
    int	arg4,		/* reserved */
    int	arg5,		/* reserved */
    int	arg6,		/* reserved */
    int	arg7,		/* reserved */
    int	arg8		/* reserved */
    )

    {
    PCMCIA_CTRL *pCtrl	= &pcmciaCtrl;
    PCMCIA_CARD *pCard	= &pCtrl->card[arg1];
    NETIF	*pNetIf	= pCard->pNetIf;

    if (pNetIf == NULL)
	return (ERROR);
    
    return (pNetIf->attachRtn (unit, pNetIf->arg1, pNetIf->arg2,
		pNetIf->arg3, pNetIf->arg4, pNetIf->arg5,
		pNetIf->arg6, pNetIf->arg7, pNetIf->arg8));
    }

#endif /* __INCusrPcmcia */
