#ifndef _ulip_io_h
#define _ulip_io_h

/* Copyright 1993-96 Wind River Systems, Inc. */
/*
modification history
--------------------
01a,07jun95,ism  Imported.
*/

/* Description: Interface for direct access to ulip
 *              device driver.  The ulip driver is
 *              a DLPI device simliar to Ethernet's
 *              le0.  To talk directly to the
 *              device we need to wrap messages
 *              in DLPI protocol.
 */

#define MAX_BUF_LEN 1500
int ReadUlip  (int fd, char * buf, int len);
int WriteUlip (int fd, char * buf, int len);

#endif
