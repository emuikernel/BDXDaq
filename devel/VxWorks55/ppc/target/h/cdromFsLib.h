/* cdromFsLib.h - ISO 9660 File System definitions */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
02i,05dec02,tkj  cdromFs joliet code review changes.
02h,22oct02,tkj  Change ioctl() codes to standard values.
02g,10oct02,tkj  Code cleanup: Split into cdromFsLib.h and cdromFsLibP.h.
02f,01oct02,tkj  Fix SPR#78416 Handles .. wrong.  Accepts path xx/../yy even
                 if xx does not exist.
02e,16sep02,tkj  Fix compiler errors/warnings.
02d,20aug02,tkj  More work on SPR#78208 - Productice Joliet extensions;
                 Changes from Europe.
02c,27jun02,tkj  More work on SPR#78208 - Productice Joliet extensions.
02b,26jun02,tkj  Enchancememts: SPR#78208 - Productice Joliet extensions.
		 Bug fixes: SPR#32715/SPR#34826, SPR#78415, SPR#78452,
		 SPR#78454, SPR#78455, SPR#78456, SPR#79162.
		 Bugs not fixed yet: SPR#34659, SPR#75766, SPR#78416,
		 SPR#79162.
02a,26jun02,tkj  Checkin cdromFsLib Joliet extensions as received from Wind
                 River Services in the Netherlands.  It needs work to be
                 productized.
01b,07dec98,lrn  add C++ support (SPR#23776)
01a,10apr97,dds  SPR#7538: add CDROM file system support to vxWorks.
*/

/*
 * currently only ISO 9660 is supported. All data dependent on this 
 * standard has prefix "ISO", or "iso" or "Iso"
 */

#ifndef cdromFsLib_h
#define cdromFsLib_h

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include <vxWorks.h>
#include <blkIo.h>	/* for BLK_DEV */
#include <cbioLib.h>	/* for ioctl() command codes */

/* defines */

/* Sizes */

#define KB			*1024

#define CDROM_STD_LS_SIZE	(2 KB) /* standard cdrom logical sector size */

#define CDROM_MAX_PATH_LEN	1024
#define CDROM_MAX_FILE_NAME_LEN	128

/*
 * ioctl() function codes
 */

#define	CDROMFS_IOCTL(X)	((M_cdromFsLib >> 8) | (X))

/* Joliet support */

#define CDROMFS_DIR_MODE_SET		CDROMFS_IOCTL(0)
#define CDROMFS_DIR_MODE_GET		CDROMFS_IOCTL(1)

/* Miscellaneous */

#define CDROMFS_STRIP_SEMICOLON		CDROMFS_IOCTL(2)
#define	CDROMFS_GET_VOL_DESC		CDROMFS_IOCTL(3)

/* Multi-session support */

#define CDROMFS_SESSION_NUMBER_SET	CDROMFS_IOCTL(4)
#define CDROMFS_SESSION_NUMBER_GET	CDROMFS_IOCTL(5)
#define CDROMFS_MAX_SESSION_NUMBER_GET	CDROMFS_IOCTL(6)

#define DEFAULT_SESSION 100

/* Obsolete - For backwards compatibility only */

#define	IOCTL_DIR_MODE_SET		CDROMFS_DIR_MODE_SET
#define	IOCTL_DIR_MODE_GET		CDROMFS_DIR_MODE_GET
#define IOCTL_STRIP_SEMICOLON		CDROMFS_STRIP_SEMICOLON
#define	IOCTL_GET_VOL_DESC		CDROMFS_GET_VOL_DESC
#define IOCTL_SESSION_NUMBER_SET	CDROMFS_SESSION_NUMBER_SET
#define IOCTL_SESSION_NUMBER_GET	CDROMFS_SESSION_NUMBER_GET
#define IOCTL_MAX_SESSION_NUMBER_GET	CDROMFS_MAX_SESSION_NUMBER_GET

/* Mode values for IOCT_DIR_MODE_SET/GET */

#define MODE_ISO9660	0
#define MODE_JOLIET	1
#define MODE_AUTO	2
#define MODE_MAX	MODE_AUTO
#define MODE_DEFAULT    MODE_ISO9660

/*
 * ISO Volume Descriptors
 */

/* constants */

#define ISO_PVD_BASE_LS	16
#define ISO_STD_ID	"CD001"	/* must be in each VD */
#define ISO_STD_ID_SIZE	5	/* must be in each VD */
#define ISO_VD_SIZE	2048

#define ISO_VD_VERSION	((u_char)1)

/* Volume descriptor (VD) types: */

#define ISO_VD_BOOT	((u_char)0)
#define ISO_VD_PRIMARY	((u_char)1)
#define ISO_VD_SUPPLEM	((u_char)2)
#define ISO_VD_PARTN	((u_char)3)
#define ISO_VD_SETTERM	((u_char)255)

/* typedefs */

/*
 * Primary/Secondary volume descriptor returned by ioctl(IOCTL_GET_VOL_DESC)
 */

/* VOlume descriptor date/time field */

#define ISO_V_DATE_TIME_YEAR_SIZE		4
#define ISO_V_DATE_TIME_FIELD_STD_SIZE		2
#define ISO_V_DATE_TIME_FROM_GREENW_NIMUTE	15

typedef struct isoVDDateTime
    {
    u_char	year[ ISO_V_DATE_TIME_YEAR_SIZE ];		/* 1-9999 */
    u_char	month[ ISO_V_DATE_TIME_FIELD_STD_SIZE ];	/* 1-12 */
    u_char	day[ ISO_V_DATE_TIME_FIELD_STD_SIZE ];		/* 1-31 */
    u_char	hour[ ISO_V_DATE_TIME_FIELD_STD_SIZE ];		/* 0-23 */
    u_char	minute[ ISO_V_DATE_TIME_FIELD_STD_SIZE ];	/* 0-59 */
    u_char	sec[ ISO_V_DATE_TIME_FIELD_STD_SIZE ];		/* 0-59 */
    u_char	sec100[ ISO_V_DATE_TIME_FIELD_STD_SIZE ];	/* 0-99 */
    signed char	greenwOffBy15Minute;
    } T_ISO_VD_DATE_TIME;

typedef T_ISO_VD_DATE_TIME *	T_ISO_VD_DATE_TIME_ID;

/* Primary/Secondary Volume Descriptor fields sizes */

#define	ISO_VD_ID_STD_SIZE	128
#define	ISO_VD_F_ID_STD_SIZE	37
#define	ISO_VD_SYSTEM_ID_SIZE	(40-8)
#define	ISO_VD_VOLUME_ID_SIZE	(72-40)
#define	ISO_VD_VOL_SPACE_SIZE	(80+ENDIAN_OFF_LONG)
#define	ISO_VD_ESCAPE_SEC_SIZE	(120-88)

#define	CDROM_MAX_DIR_LEV	120	/* max dir nesting level */
#define	CDROM_MAX_DIR_SIZE	128	/* max directory/file Byte sice */
				/* For backwards compatibility */

#define	CDROM_MAX_DIR_SICE	CDROM_MAX_DIR_SIZE

/* Primary/Supplementary Volume Descriptor */

typedef struct isoPrimarySuplementaryVolDescriptor
    {
    u_long	volSize;	/* logical sector per volume */
    u_long	PTSize;		/* bytes per Path Table */
    u_long	PTSizeOnCD;	/* bytes per Path Table on physical CD*/
    u_long	PTOccur;	/* Path table occurence */
    u_long	PTOptOccur;	/* optional occurence */
    u_long	rootDirSize;	/* bytes per Root Directory */
    u_long	rootDirStartLB;	/* where root dir starts */

    u_long	volDescSector;	/* Sector containing the volume descriptor */
    u_long	volDescOffInSect; /* Volume descriptor offset in sector */
    u_short	volDescNum;	/* Volume descriptor number in session */

    u_short	volSetSize;	/* number of disks in set */
    u_short	volSeqNum;	/* disk number in set */
    u_short	SesiOnCd;	/* number of all sessions on device */
    u_short	ReadSession;	/* Seesion that contains the volume descriptor */
    u_short	LBSize;		/* bytes per Logical Block */
		
    u_char	type;		/* volume descriptor type, have to be one */
				/* of ISO_VD_PRIMARY or ISO_VD_SUPPLEM */
    u_char	uniCodeLev;	/* Unicode level */
				/* 0=ISO9660 */
				/* 1=UC-level1 2=UC-level2 3=UC-level3 */

    u_char	fileStructVersion;		/* currently 1 only (ISO) */
    u_char	stdID[ ISO_STD_ID_SIZE + 1 ];	/* currently have to be */
    						/* ISO_STD_ID */
    u_char	systemId[ ISO_VD_SYSTEM_ID_SIZE +1 ];	/* a-u_chars */
    u_char	volumeId[ ISO_VD_VOLUME_ID_SIZE +1 ];	/* d-u_chars */
    u_char	volSetId[ ISO_VD_ID_STD_SIZE +1 ];	/* d-u_chars */
    u_char	publisherId[ ISO_VD_ID_STD_SIZE +1 ];	/* a-u_chars */
    u_char	preparerId[ ISO_VD_ID_STD_SIZE +1 ]; 	/* a-u_chars */
    u_char	applicatId[ ISO_VD_ID_STD_SIZE +1 ];	/* a-u_chars */
    u_char	cprightFId[ ISO_VD_F_ID_STD_SIZE +1 ]; 	/* file ID */
    u_char	abstractFId[ ISO_VD_F_ID_STD_SIZE +1 ];	/* file ID */
    u_char	bibliogrFId[ ISO_VD_F_ID_STD_SIZE +1 ];	/* file ID */
    T_ISO_VD_DATE_TIME	creationDate;
    T_ISO_VD_DATE_TIME	modificationDate;
    T_ISO_VD_DATE_TIME	expirationDate;
    T_ISO_VD_DATE_TIME  effectiveDate;
    } T_ISO_PVD_SVD;

typedef T_ISO_PVD_SVD *	T_ISO_PVD_SVD_ID;

/*
 * CD-ROM volume descriptor (internal)
 *
 * THis is here only for the function declarations.
 */

typedef struct cdromVolDescr *	CDROM_VOL_DESC_ID;

/* ========= errno codes =========== */

#define S_cdromFsLib_ALREADY_INIT			(M_cdromFsLib | 1)
#define S_cdromFsLib_DEVICE_REMOVED			(M_cdromFsLib | 3)
#define S_cdromFsLib_SUCH_PATH_TABLE_SIZE_NOT_SUPPORTED (M_cdromFsLib | 4)
#define S_cdromFsLib_ONE_OF_VALUES_NOT_POWER_OF_2	(M_cdromFsLib | 5)
#define S_cdromFsLib_UNKNOWN_FILE_SYSTEM		(M_cdromFsLib | 6)

				/* For backwards compatibility */

#define S_cdromFsLib_UNNOWN_FILE_SYSTEM	S_cdromFsLib_UNKNOWN_FILE_SYSTEM

#define S_cdromFsLib_INVAL_VOL_DESCR			(M_cdromFsLib | 7)
#define S_cdromFsLib_INVALID_PATH_STRING		(M_cdromFsLib | 8)
#define S_cdromFsLib_MAX_DIR_HIERARCHY_LEVEL_OVERFLOW	(M_cdromFsLib | 9)
#define S_cdromFsLib_NO_SUCH_FILE_OR_DIRECTORY		(M_cdromFsLib | 10)
#define S_cdromFsLib_INVALID_DIRECTORY_STRUCTURE	(M_cdromFsLib | 11)
#define S_cdromFsLib_INVALID_FILE_DESCRIPTOR		(M_cdromFsLib | 12)
#define S_cdromFsLib_READ_ONLY_DEVICE			(M_cdromFsLib | 13)
#define S_cdromFsLib_END_OF_FILE			(M_cdromFsLib | 14)
#define S_cdromFsLib_INV_ARG_VALUE			(M_cdromFsLib | 15)
#define S_cdromFsLib_SEMTAKE_ERROR			(M_cdromFsLib | 16)
#define S_cdromFsLib_SEMGIVE_ERROR			(M_cdromFsLib | 17)
#define S_cdromFsLib_VOL_UNMOUNTED			(M_cdromFsLib | 18)
#define S_cdromFsLib_INVAL_DIR_OPER			(M_cdromFsLib | 19)
#define S_cdromFsLib_READING_FAILURE			(M_cdromFsLib | 20)
#define S_cdromFsLib_INVALID_DIR_REC_STRUCT     (M_cdromFsLib | 21)
#define S_cdromFsLib_SESSION_NR_NOT_SUPPORTED   (M_cdromFsLib | 23)

/*
 * NOTE: The following #include is for backwards compatibility.  It will
 * be removed the next release.  These definitions should have been
 * private all along, but they were public.  If you need any of these
 * #include private/cdromFsLibP.h yourself.  However, be aware these can
 * change at any time.
 */

#include <private/cdromFsLibP.h> /* Solely for backwards compatibility */

/* globals */

#ifndef _ASMLANGUAGE

/* function declarations */

/* interface functions */

extern STATUS cdromFsInit (void);

/* user-callable functions prototypes */

extern CDROM_VOL_DESC_ID cdromFsDevReset( void * pVolDesc, STATUS retStat );
extern CDROM_VOL_DESC_ID cdromFsDevCreate( char * devName, BLK_DEV * pBlkDev );
extern void cdromFsVolConfigShow (void * arg);
extern uint32_t cdromFsVersionNumGet (void);
extern void cdromFsVersionDisplay (int level);

#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* cdromFsLib_h */
