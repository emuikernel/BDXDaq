/* usrScsi.c - SCSI initialization */

/* Copyright 1992-1997 Wind River Systems, Inc. */

/*
modification history
--------------------
*/

/*
DESCRIPTION
This file is used to configure and initialize the VxWorks SCSI support.
This file is included by usrConfig.c.

MAKE NO CHANGES TO THIS FILE.

All BSP specific code should be added to
the sysLib.c file or a sysScsi.c file in the BSP directory.  Define the
macro SYS_SCSI_CONFIG to generate the call to sysScsiConfig().
See usrScsiConfig for more info on the SYS_SCSI_CONFIG macro.

SEE ALSO: usrExtra.c

NOMANUAL
*/

/*******************************************************************************
*
* usrScsiConfig - configure SCSI peripherals
*
* This code configures the SCSI disks and other peripherals on a SCSI
* controller chain.
*
* The macro SCSI_AUTO_CONFIG will include code to scan all possible device/lun
* id's and to configure a scsiPhysDev structure for each device found.  Of
* course this doesn't include final configuration for disk partitions,
* floppy configuration parameters, or tape system setup.  All of these actions
* must be performed by user code, either through sysScsiConfig(),
* the startup script, or by the application program.
*
* The user may customize this code on a per BSP basis using the SYS_SCSI_CONFIG
* macro.  If defined, then this routine will call the routine sysScsiConfig().
* That routine is to be provided by the BSP, either in sysLib.c or sysScsi.c.
* If SYS_SCSI_CONFIG is not defined, then sysScsiConfig() will not be called
* as part of this routine.
*
* An example sysScsiConfig() routine can be found in target/src/config/usrScsi.c.
* The example code contains sample configurations for a hard disk, a floppy
* disk and a tape unit.
*
* RETURNS: OK or ERROR.
*
* SEE ALSO:
* .pG "I/O System, Local File Systems"
*/

STATUS usrScsiConfig (void)

    {
    /*
     * initialize either the SCSI1 or SCSI2 interface; initialize SCSI2 when
     * the SCSI2 interface is available.
     */

#ifndef INCLUDE_SCSI2
    scsi1IfInit ();
#else
    scsi2IfInit ();
#endif

    if (sysScsiInit () != OK)
	{
	printf ("sysScsiInit() Failed, SCSI system not initialized\n");
	return (ERROR);
	}

#ifdef SCSI_AUTO_CONFIG

    /* Optional auto-config of physical devices on SCSI bus */

    taskDelay (sysClkRateGet () * 1);	/* allow devices to reset */

    printf ("Auto-configuring SCSI bus...\n\n");

    scsiAutoConfig (pSysScsiCtrl);

    scsiShow (pSysScsiCtrl);

    printf ("\n");
#endif	/* !SCSI_AUTO_CONFIG */

#ifdef SYS_SCSI_CONFIG

    /* Execute BSP configuration code, if any */

    sysScsiConfig ();

#endif

    return (OK);
    }

