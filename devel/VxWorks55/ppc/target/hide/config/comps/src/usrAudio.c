/* usrAudio.c - Audio initialization */

/* Copyright 2000 Wind River Systems, Inc. */

/*
modification history
--------------------
*/

/*
DESCRIPTION
This file is used to configure and initialize the VxWorks audio support.
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
* usrAudioInit - initialize audio device driver
*
*
* RETURNS: OK or ERROR.
*
* SEE ALSO:
* .pG "I/O System, Local File Systems"
*/

STATUS usrAudioInit (void)

    {

    return (OK);
    }

