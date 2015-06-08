/* usrNetNfsMount.c - Does the NFS mount */

/* Copyright 1992 - 1998 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,05dec97,spm  added DHCP code review modifications
*/

/*
DESCRIPTION
This file gets included when INCLUDE_NFS_MOUNT_ALL is defined. It makes a call 
to nfsMountAll().

NOMANUAL
*/

void usrNetNfsMountAll ()
    {
    printf ("Mounting NFS file systems from host %s", sysBootParams.hostName);\
    if (sysBootParams.targetName[0] != EOS)\
       printf (" for target %s:\n", sysBootParams.targetName);\
    else \
       printf (":\n"); \
    nfsMountAll (sysBootParams.hostName, sysBootParams.targetName, FALSE); \
    printf ("...done\n"); \
    }
 
