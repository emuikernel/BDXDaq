/* tffsDrv.h - TFFS FLite driver header */

/* Copyright 1984-1997 Wind River Systems, Inc. */

/*
modification history
--------------------
01i,25jan99,yp   added new function tffsDevOptionsSet()'s prototype.
01h,19jan98,hdn  added new function tffsShowAll()'s prototype.
01g,06jan98,hdn  added usrTffsConfig()'s prototype.
01f,06jan98,hdn  changed tffsShow()'s prototype.
01e,15dec97,hdn  added _ASMLANGUAGE macro.
01d,11dec97,hdn  added tffsShow()'s prototype.
01c,05dec97,hdn  added tffsRawio()'s function codes and prototype.
01b,07nov97,hdn  cleaned up.
01a,25aug97,and  written by Andray in M-Systems
*/

#ifndef __INCtffsDrvh
#define __INCtffsDrvh

#ifdef __cplusplus
extern "C" {
#endif


#ifndef _ASMLANGUAGE

#include "blkIo.h"

#define TFFS_MAX_DRIVES		5	/* same to DRIVES in flcustom.h */

/* function number for tffsRawio() */

#define	TFFS_GET_PHYSICAL_INFO	0	/* get physical info */
#define TFFS_PHYSICAL_READ	1	/* physical read */
#define TFFS_PHYSICAL_WRITE	2	/* physical write */
#define TFFS_PHYSICAL_ERASE	3	/* physical erase */
#define TFFS_ABS_READ		4	/* absolute read */
#define TFFS_ABS_WRITE		5	/* absolute write */
#define TFFS_ABS_DELETE		6	/* absolute erase */
#define TFFS_DEFRAGMENT_VOLUME	7	/* defragment volume */


/*--------------------------------------------------------------------------*/
/*      TFFS block devices                                                  */
/*--------------------------------------------------------------------------*/

typedef struct
    {
    BLK_DEV  tffsBlkdev;         /* VxWorks block device descriptor        */
    int      tffsDriveNo;        /* 0..DRIVES-1, internal FLite numbering  */
    } TFFS_DEV;

/*---------------------------------------------------------------------------*/
/*                Formatting parameter structure definition                  */
/*---------------------------------------------------------------------------*/

typedef struct 
    {
    /* FTL formatting section */
    long int	bootImageLen;
		/* Space to reserve for a boot-image at the start of the
		   medium. The FLite volume will begin at the next higher
		   erase unit boundary */

    unsigned	percentUse;
		/* FTL performance depends on how full the Flash media is,
		   getting slower when the media is close to 100%. It is
		   possible to avoid the worst consequences of this effect by
		   formatting the media to less than 100% capacity, so
		   guaranteeing some free space at all times. This of course
		   sacrifices some capcity.
		   The standard value to use is 99 */

    unsigned	noOfSpareUnits;
		/* FTL needs at least one spare erase unit to function as a
		   read/write media (it is possible to specify 0 to achieve
		   WORM functionality). It is possible to specify more than
		   one spare unit, which costs more media space.
		   The advantage to doing this is in case one of the Flash
		   erase units becomes bad and unerasable in the future, one
		   of the spare units will need to replace it. A second spare
		   unit will then enable to continue read/write functionality
		   (in its absence, the media will go into read-only mode).
		   The standard value to use is 1 */

    unsigned long vmAddressingLimit;
		/* A part of the FTL Virtual Map always resides in RAM. The
		   RAM part is the one that is used to address the part of
		   the media below the VM addressing limit. Reading and
		   writing to this part is usually faster to some degree.
		   The downside is that the bigger the limit, the more RAM
		   size is required.
		   The minimum VM limit is 0.
		   The standard value to use is 0x10000 (first 64 KBytes) */

    int		(*progressCallback)(int totalUnitsToFormat,
				    int totalUnitsFormattedSoFar);
		/* Progress callback routine, will be called if not NULL.
		   The callback routine is called after erasing each unit,
		   and its parameters are the total number of erase units
		   to format and the number erased so far.
		   The callback routine returns a Status value. A value of
		   OK (0) allows formatting to continue. Any other value
		   will abort the formatting with the returned status code. */

    /* DOS formatting section */
    char		volumeId[4];
		/* Volume identification number */

    char *	volumeLabel;
		/* Volume label string. If NULL, no label */

    unsigned	noOfFATcopies;
		/* It is customary to format DOS media with 2 FAT copies.
		   The first copy is always used, but more copies make it
		   possible to recover if the FAT becomes corrupted (a
		   rare occurrence). On the other hand, this slows down
		   performance and uses media space.
		   The standard value to use is 2 */

    unsigned	embeddedCISlength;
		/* Length in bytes of CIS to embed after the unit header */

    char *	embeddedCIS;
		/* The unit header is structured as a beginning of a PCMCIA
		   'tuple' chain (a CIS). The unit header contains a
		   data-organization tuple, which points past the end of the
		   unit header to a location which usually just contains
		   hex FF's which mark an 'end-of-tuple-chain'. Optionally,
		   it is possible to embed an entire CIS chain at this
		   location. If so, 'embeddedCISlength' marks the length in
		   bytes */

    } tffsFormatParams;

/*---------------------------------------------------------------------------*/
/*            IOCTL FIODISKFORMAT structure definition                       */
/*---------------------------------------------------------------------------*/

typedef struct 
    {
    tffsFormatParams  formatParams;
    unsigned          formatFlags;
    } tffsDevFormatParams;

/* values for tffsDevFormatParams.formatFlags */

#define FTL_FORMAT	     1  /* FAT & FTL formatting                     */
#define FTL_FORMAT_IF_NEEDED 2	/* FAT formatting, FTL formatting if needed */
#define NO_FTL_FORMAT	     0	/* FAT formatting only                      */

/* Standard initializer for IOCTLParams structure */

#define TFFS_STD_FORMAT_PARAMS	{{0, 99, 1, 0x10000l, NULL, {0,0,0,0}, NULL, \
				 2, 0, NULL}, FTL_FORMAT_IF_NEEDED}


/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS	 tffsDrv (void);
extern BLK_DEV * tffsDevCreate (int tffsDriveNo, int removableMediaFlag);
extern STATUS	 tffsDevFormat (int tffsDriveNo, int arg);
extern void      flAnnonceDiskChange (unsigned volNo);
extern void      tffsShowAll (void);
extern void      tffsShow (int driveNo);
extern STATUS	 tffsRawio (int tffsDriveNo, int functionNo, int arg0,
			    int arg1, int arg2);
extern STATUS	 tffsDevOptionsSet (TFFS_DEV * pTffsDev);
extern STATUS	 usrTffsConfig (int drive, int removable, char *  fileName);



#else	/* __STDC__ */

extern STATUS	 tffsDrv ();
extern BLK_DEV * tffsDevCreate ();
extern STATUS	 tffsDevFormat ();
extern void      flAnnonceDiskChange ();
extern void      tffsShowAll ();
extern void      tffsShow ();
extern STATUS	 tffsRawio ();
extern STATUS    tffsDevOptionsSet ();
extern STATUS	 usrTffsConfig ();

#endif	/* __STDC__ */


#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCtffsDrvh */
