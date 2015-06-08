/* cdromFsLibP.h - ISO 9660 File System private definitions */

/* Copyright 2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,05dec02,tkj  cdromFs joliet code review changes.
01b,23oct02,tkj  Move FIOREADTOCINFO to ioLib.h.
01a,10oct02,tkj  Code cleanup: Split into cdromFsLib.h and cdromFsLibP.h.
*/

/*
 * currently only ISO 9660 is supported. All data dependent on this 
 * standard has prefix "ISO", or "iso" or "Iso"
 */

#ifndef cdromFsLibP_h
#define cdromFsLibP_h

#ifdef __cplusplus
extern "C" {
#endif

/* includes */

#include <vxWorks.h>
#include <iosLib.h>
#include <lstLib.h>	/* LIST, NODE */
#include <semLib.h>	/* SEM_ID */
#include <blkIo.h>	/* BLK_DEV */
#include <time.h>

/* defines */

#define LEN32 4 /* for fields reading in CD data descriptors */
#define LEN16 2

/* Magic numbers to verify validity of structs */

#define VD_SET_MAG	(0x06200556) /* CDROM_VOL_DESC.magic */
#define VD_LIST_MAG	(0x03110364) /* T_CDROMFS_VD_LST.magic */
#define FD_MAG		(0x08211061) /* T_CDROM_FILE.magic */
#define SEC_BUF_MAG	(0x05071163) /* SEC_BUF.magic */

/*---------------------------------------------------------------------------*/

/*
 * Macros to include/exclude certain features
 *
 * Wind River does not expect customers to modify this file.  These
 * defines are here instead of inside cdromFsLib.c since they are used in
 * this file to delete optional structure members, thus saving memory.
 */

/*
 * CDROMFS_MODE_AUTO_COMBINE_VOLUME_DESCRIPTORS
 *
 * Define the below to in MODE_AUTO combine together in readdir() output
 * of one directory all the files of all volume descriptors from their
 * corresponding directory.  Without this readdir() will show results for
 * only one volume descriptor.
 *
 * Even without this any file of any volume descriptor can be opened in
 * MODE_AUTO.
 *
 * WARNING: This code only partially works.  It does not handle either
 * directories larger than one logical block, or the combined directory
 * entries larget than one logical block.
 *
 * This code should normally be ommitted since it only partially works.
 */

#undef	CDROMFS_MODE_AUTO_COMBINE_VOLUME_DESCRIPTORS /* Normally undef */

/*
 * CDROMFS_MULTI_SESSION_ATAPI_IOCTL
 *
 * Define the below to ask the ATAPI driver for the CD table of contents
 * before trying to determine it ourself.  This is not indluded since we
 * need one ioctl() command code that works for both ATAPI and SCSI.  As
 * currently defined, the ATAPI ioctl() command is different from the
 * corresponding Mt. Fuji SCSI ioctl().  They need to be unified with the
 * same ioctl() command code to the ATAPI definition, the SCSI definition,
 * or a new joint definition.  The existing different ATAPI and SCSI
 * ioctl() commands can then be either kept or deleted.
 *
 * The value of this macro does not matter if
 * CDROMFS_MULTI_SESSION_SUPPORT is omitted.
 *
 * This code should normally be ommitted since it considers only ATAPI
 * drivers, not SCSI drivers.
 */

#undef	CDROMFS_MULTI_SESSION_ATAPI_IOCTL /* Normally undef */

/*
 * CDROMFS_MULTI_SESSION_SUPPORT
 *
 * Define the below to include multi-session support.  If this is defined
 * but CDROMFS_MULTI_SESSION_ATAPI_IOCTL is not, cdromFs always reads the
 * CD itself to get the CD table of contents.
 *
 * This code should normally be ommitted since some CD drives hang if
 * cdromFs gets the table of contents itself and references a session that
 * does not exist.  This is common since cdromFs does not know the number
 * of sessions on the disc, so it does not know when to stop when scanning
 * the table of contents..
 *
 * IMPORTANT: Edit the description of cdromFsLib.c to match the value
 * chosen for this macro.  See the internal section for instructions.
 */

#undef	CDROMFS_MULTI_SESSION_SUPPORT /* Normally undef */

/*---------------------------------------------------------------------------*/

/* Disk offset adjustments for big/little endian */

#if (_BYTE_ORDER == _LITTLE_ENDIAN)
#define BYTE_ORDER_LITTLE_ENDIAN
#define ENDIAN_OFF_SHORT	0 /* ISO-9660: Section 7.2.3, Page 10 */
#define ENDIAN_OFF_LONG		0 /* ISO-9660: Section 7.3.3, Page 10 */
#elif (_BYTE_ORDER == _BIG_ENDIAN)
#define BYTE_ORDER_BIG_ENDIAN
#define ENDIAN_OFF_SHORT	2 /* ISO-9660: Section 7.2.3, Page 10 */
#define ENDIAN_OFF_LONG		4 /* ISO-9660: Section 7.3.3, Page 10 */
#else
#error "_BYTE_ORDER must be defined, check compilation flags"
#endif /* (_BYTE_ORDER == _LITTLE_ENDIAN) */

/*---------------------------------------------------------------------------*/

/*
 * Volume descrioptr date disk offsets/sizes
 * ISO-9660: Section 8.4.26.1, Table 5, Page 15
 * In memory: T_FILE_DATE_TIME
 */

#define ISO_V_DATE_TIME_YEAR	       		0
#define ISO_V_DATE_TIME_MONTH			4
#define ISO_V_DATE_TIME_DAY			6
#define ISO_V_DATE_TIME_HOUR			8
#define ISO_V_DATE_TIME_MINUTE			10
#define ISO_V_DATE_TIME_SEC			12
#define ISO_V_DATE_TIME_100_OF_SEC		14
#define ISO_V_DATE_TIME_FROM_GREENW_OFF		16
#define ISO_V_DATE_TIME_FROM_GREENW_NIMUTE	15

/*---------------------------------------------------------------------------*/

/*
 * Primary volume descriptor and seondary volume descriptor disk offsets/sizes
 * ISO-9660: Section 8.4, Tables 4, Page 13; section 8.5, Table 6, Page 16
 * In memory: T_CDROMFS_VD_LST
 */

#define	ISO_VD_SYSTEM_ID	8
#define	ISO_VD_VOLUME_ID	40
#define	ISO_VD_VOL_SPACE_SIZE	(80+ENDIAN_OFF_LONG)
#define	ISO_VD_ESCAPE_SEC	88
#define	ISO_VD_VOL_SET_SIZE	(120+ENDIAN_OFF_SHORT)
#define	ISO_VD_VOL_SEQUENCE_N	(124+ENDIAN_OFF_SHORT)
#define	ISO_VD_LB_SIZE		(128+ENDIAN_OFF_SHORT)
#define	ISO_VD_PT_SIZE		(132+ENDIAN_OFF_LONG)

/* 
 * ISO 9660 structures contan some data in both Big and Little endian order
 * Hopefully all mastering software implementations fill them all in 
 */

#ifdef BYTE_ORDER_BIG_ENDIAN
#define	ISO_VD_PT_OCCUR		148	/* u_long */
#define	ISO_VD_PT_OPT_OCCUR	152	/* u_long */

#else	/* BYTE_ORDER_LITTLE_ENDIAN */

#define	ISO_VD_PT_OCCUR		140	/* u_long */
#define	ISO_VD_PT_OPT_OCCUR	144	/* u_long */
#endif	/* BYTE_ORDER_BIG_ENDIAN */

#define	ISO_VD_ROOT_DIR_REC		156
#define	ISO_VD_ROOT_DIR_REC_SIZE	(190-156)

#define	ISO_VD_VOL_SET_ID	190
#define	ISO_VD_PUBLISH_ID	318
#define	ISO_VD_DATA_PREP_ID	446
#define	ISO_VD_APPLIC_ID	574

#define	ISO_VD_COPYR_F_ID	702
#define	ISO_VD_ABSTR_F_ID	739
#define	ISO_VD_BIBLIOGR_F_ID	776

#define	ISO_VD_VOL_DATE_TIME_STD_SIZE	17
#define	ISO_VD_VOL_CR_DATE_TIME		813
#define	ISO_VD_VOL_MODIF_DATE_TIME	830
#define	ISO_VD_VOL_EXPIR_DATE_TIME	847
#define	ISO_VD_VOL_EFFECT_DATE_TIME	864

#define	ISO_VD_FILE_STRUCT_VER		881

/* Values for ISO_VD_ESCAPE_SEC - From Joliet standard */

#define UCS_2_LEVEL1_ID		"%/@"	/* must be in each VD LEVEL 1 */
#define UCS_2_LEVEL2_ID		"%/C"	/* must be in each VD LEVEL 2 */
#define UCS_2_LEVEL3_ID		"%/E"	/* must be in each VD LEVEL 3 */

/*---------------------------------------------------------------------------*/

/*
 * ISO Path Table record disk offsets/sizes
 * ISO-9660: Section 9.4, Table 11, Page 22
 */

#define ISO_PT_REC_LEN_DI		0	
#define ISO_PT_REC_EAR_LEN		1
#define ISO_PT_REC_EXTENT_LOCATION	2
#define ISO_PT_REC_PARENT_DIR_N		6 /* short in format due to PT type */
#define ISO_PT_REC_DI				8
#define ISO_PT_REC_DI_LEN			8

/*---------------------------------------------------------------------------*/

/*
 * ISO Directory Record disk offsets/sizes
 * ISO-9660: Section 9.1, Table 8, Page 19
 */

#define ISO_DIR_REC_REC_LEN		0
#define ISO_DIR_REC_EAR_LEN		1
#define ISO_DIR_REC_EXTENT_LOCATION	(2+ENDIAN_OFF_LONG)
#define ISO_DIR_REC_DATA_LEN		(10+ENDIAN_OFF_LONG)
#define ISO_DIR_REC_DATA_TIME		18
#define ISO_DIR_REC_FLAGS		25
#define ISO_DIR_REC_FU_SIZE		26
#define ISO_DIR_REC_IGAP_SIZE		27
#define ISO_DIR_REC_VOL_SEQU_N		(28+ENDIAN_OFF_SHORT)
#define ISO_DIR_REC_LEN_FI		32	
#define ISO_DIR_REC_FI			33
#define ISO_DIR_REC_LENGH		33

/*
 * flags masks
 * ISO-9660: Section 9.1.6, Table 10, Page 20
 */

#define DRF_DIRECTORY		0x02
#define DRF_IS_REC_DESCRIPT	0x04	/* file records are described in EAR */
#define DRF_PROTECT		0x10    /* location of user's permissions */
#define DRF_LAST_REC		0x80	/* last file-record */


/*---------------------------------------------------------------------------*/

/*
 * ISO Extended Attribute Record fields offsets and sizes
 * ISO-9660: 9.5, Table 12, Page 23
 */

#define ISO_EAR_ONER		(0+ENDIAN_OFF_SHORT)
#define ISO_EAR_GROUPE		(4+ENDIAN_OFF_SHORT)
#define ISO_EAR_PERMIT		8

#define ISO_EAR_F_CR_DATE_TIME_SIZE	ISO_VD_VOL_DATE_TIME_STD_SIZE
#define ISO_EAR_F_CR_DATE_TIME		10
#define ISO_EAR_F_MODIF_DATE_TIME	27
#define ISO_EAR_F_EXPIR_DATE_TIME	44
#define ISO_EAR_F_EFFECT_DATE_TIME	81

#define ISO_EAR_REC_FORMAT	78
#define ISO_EAR_REC_ATTR	79
#define ISO_EAR_REC_LEN		(80+ENDIAN_OFF_SHORT)

#define ISO_EAR_SYS_ID		84
#define ISO_EAR_SYS_ID_SIZE	(116-84)
#define ISO_EAR_SYS_USE		116
#define ISO_EAR_SYS_USE_SIZE	(180-116)
#define ISO_EAR_VERSION		180	  /* currently have to be 1 (ISO) */

#define	ISO_EAR_LEN_ESC		181
#define	ISO_EAR_RESERVED	182
#define	ISO_EAR_RESERVED_SIZE	(246-182)
#define	ISO_EAR_LEN_AU		246
#define	ISO_EAR_APP_USE		250

/*
 * EAR-Record Format values
 * ISO-9660: Section 9.5.8, Page 24
 */

#define EAR_REC_FORM_IGNORE	((u_char)0)
#define EAR_REC_FORM_FIX_LEN	CDROM_MDU_TYPE_FIX
#define EAR_REC_FORM_VAR_LEN_LE	CDROM_MDU_TYPE_VAR_LE	/* RCW in 7.2.1 */
#define EAR_REC_FORM_FIX_LEN_BE	CDROM_MDU_TYPE_VAR_BE	/* RCW in 7.2.2 */

#define CDROM_MDU_TYPE_FIX	((u_char)1)
#define CDROM_MDU_TYPE_VAR_LE	((u_char)2)	/* RCW in 7.2.1 */
#define CDROM_MDU_TYPE_VAR_BE	((u_char)3)	/* RCW in 7.2.2 */

/*
 * EAR-Record Attribute values
 * ISO-9660: Section 9.5.9, Page 25
 */

#define EAR_REC_ATTR_LF_CR	((u_char)0)	/* cr-record-lf */
#define EAR_REC_ATTR_VERT_SP	((u_char)1)
#define EAR_REC_ATTR_CONTROL	((u_char)2)

/*---------------------------------------------------------------------------*/

/* Related to reading table of contents (Start/End of each session) */

#define CDROM_LEAD_IN		4500
#define CDROM_LEAD_OUT_FIRST	6750
#define CDROM_LEAD_OUT		2250
#define CDROM_SESSION_OFFSET	318

/*---------------------------------------------------------------------------*/
/*
 * XXX Move to ioLib.h later
 */

#ifdef CDROMFS_MULTI_SESSION_ATAPI_IOCTL /* Ask ATAPI driver for TOC */
#define FIOREADTOCINFO	0xf8		/* get CD-ROM TOC from ATAPI driver */
#endif /* CDROMFS_MULTI_SESSION_ATAPI_IOCTL */

/* Table of contents (TOC) of a CD: Maximum tracks on a CD */

#define CDROM_MAX_TRACKS	99

/*---------------------------------------------------------------------------*/

/* Values for T_CDROM_FILE.FCSInterleaved */

#define CDROM_NOT_INTERLEAVED	0
#define CDROM_INTERLEAVED	1

/*---------------------------------------------------------------------------*/

/* typedefs */

/*---------------------------------------------------------------------------*/

/*
 * XXX Move to ioLib.h later
 */

/*
 * FIOREADTOCINFO
 */

/* Table of contents (TOC) of a CD: Header */

typedef struct readTocHeader
    {
    UINT8 tocDataLength[2];
    UINT8 firstSessionNumber;
    UINT8 lastSessionNumber;
    } readTocHeaderType;

/* Table of contents (TOC) of a CD: Session descriptor */

typedef struct readTocSessionDescriptor
    {
    UINT8 reserved1;
    UINT8 control:4;
    UINT8 adr:4;
    UINT8 sessionNumber;
    UINT8 reserved2;
    UINT8 sessionStartAddress[4]; /* big-endian.  use ntohl() */
    } readTocSessionDescriptorType;

/*
 * FIOREADTOCINFO: Type of arg parameter is CDROM_TRACK_RECORD_ID
 *
 * The size of *statBuffer should be sizeof (readTocHeaderType) +
 * (CDROM_MAX_TRACKS * sizeof (readTocSessionDescriptorType))
 *
 * statBuffer actually points at one recTocHeaderType immediately followed
 * by up to CDROM_MAX_TRACKS readTocSessionDescriptorType records.
 */

typedef struct cdRomTrackStat	/* Table of contents for multi-session CD */
    {
    int		bufferLength;	/* length of the Buffer */
    u_char *	statBuffer;	/* File's track records buffer */
				/* (only for command dir) */
    } CDROM_TRACK_RECORD;

typedef CDROM_TRACK_RECORD *	CDROM_TRACK_RECORD_ID;

/*---------------------------------------------------------------------------*/

/*
 * Volume Descriptor header, may be used as mask, since byte order is stored
 */

typedef struct isoVdHead
    {
    u_char   type;		       /* any of VD types */
    u_char   stdID[ ISO_STD_ID_SIZE ]; /* have to be ISO_STD_ID */
    u_char   version;		       /* have to be ISO_VD_VERSION */
				       /* 0=ISI9660 1=Joliet(Unicode) 2=Auto */
    } T_ISO_VD_HEAD;

typedef T_ISO_VD_HEAD *	T_ISO_VD_HEAD_ID;

/*---------------------------------------------------------------------------*/

/*
 * Disk cache: structure to hold data while files are opened.
 */

typedef struct secBuf	/* buffer for reading sectors */
    {
    u_long	startSecNum;
    u_long	numSects;	  /* Number of sectors it contains */
    u_long	maxSects;	  /* Maximum number sectors it can contain */
    int		magic;		  /* SEC_BUF_MAG after sectData allocation */
    u_char *	sectData;
    } SEC_BUF;

typedef SEC_BUF *	SEC_BUF_ID;

/*---------------------------------------------------------------------------*/

/* 
 * CDROM_VOL_DESCR - Mounted volume
 *
 * Since in future it will cover, we'll hope, Rock Ridge extensions, a name
 * has no preffix 'ISO'.
 * Many fields, pertaining to T_ISO_PVD_SVD, are excluded from
 * CDROMFS_VOL_DESCR for memory saving and code simplicity,
 * since they aren't realy use for device control (such as publisherId ...).
 *
 * 1 per mounted volume
 */

typedef struct cdromVolDescr	/* CDROM_VOL_DESC */
    {
    DEV_HDR    devHdr;		/* for adding to device table */
    int	       magic;		/* VD_SET_MAG */
    BLK_DEV *  pBlkDev;	        /* Ptr to Block Device structure. */
    SEM_ID     mDevSem;	        /* device mutual-exclusion semaphore */
    SEC_BUF    sectBuf;	        /* Sector reading buffer. must be inited */
				/* over cdromFsDevCreate() */
    u_int      sectSize;	/* device sector size, copy of  */
				/* bd_bytesPerBlk in BLK_DEV */
    LIST       VDList;		/* VD list header. All device VD (at least */
				/* one primary and each supplementary, if */
				/* any exist) are connected to linked list */
    LIST       FDList;		/* FD list header. All file descriptors are */
				/* connected to linked list for to arrive */
				/* each of them in case vol unmount */
    u_char     LSToPhSSizeMult; /* LSSize/PhSSize ( may be need in case */
				/* any blkDevDrv understand cdrom LS size */
				/* less then 2Kb ) */
    u_char     unmounted;	/* != 0 => VDList must be bult newrly */
    u_char     DIRMode;		/* Mode of the directory */
#ifdef	CDROMFS_MULTI_SESSION_SUPPORT
    u_short    SesiOnCD;	/* Count of sessions on mounted device */
    u_short    SesiToRead;	/* Which session to read, or DEFAULT_SESSION */
    u_long     SesiPseudoLBNum;	/* Start of session in pseudo LB */
#endif	/* CDROMFS_MULTI_SESSION_SUPPORT */
    BOOL       StripSemicolon;  /* Remove semicolon when reading directories */
    } CDROM_VOL_DESC;

/* Note, the pointer type CDROM_VOL_DESC_ID is defined in cdromFsLib.h */

/*---------------------------------------------------------------------------*/

/*
 * T_CDROMFS_VD_LST; - Colume descriptor of a mouned volume
 * ISO-9660: Section 8.4, Tables 4, Page 13; section 8.5, Table 6, Page 16
 * Disk offset/size: ISO_VD_xxx
 *
 * device VDs-list element - On CDROM_VOL_DESC.VDList.
 *
 * 1 per primary or supplementary volume descriptor
 */

typedef struct cdromVDLst
    {
    NODE	list;		  /* Member of CDROM_VOL_DESC.VDList */
    int		magic;		  /* VD_LIST_MAG */
    CDROM_VOL_DESC_ID	pVolDesc; /* ptr to list header's CDROM_VOL_DESC, */
				  /* may be uzed as BLK_DEV ptr */
    u_long	VDPseudoLBNum;	  /* Pseudo LB, VD is in */
    u_long	volSize;	  /* LS per volume */
    u_long	PTSize;		  /* bytes per Path Table ( in Memory )*/
    u_long	PTSizeOnCD;	  /* bytes per Path Table ( on Disk/CD )*/
    u_long	PTStartLB;
    u_long	rootDirSize;	  /* bytes per Root Directory */
    u_long	rootDirStartLB;
    u_short	volSetSize;	  /* number of phis devices in set */
    u_short	volSeqNum;	  /* device number in set */
    u_short	LBSize;		  /* bytes per Logical Block */
		
    /* In accordance with ISO9660 all directory records in
     * PT are sorted by hierarchy levels and are numbered from 1
     * ( only root has number 1 and is always placed on level 1 ).
     * Number of levels is restricted by 8.
     * dirLevBordersOff[ n ] contains offset of first PT record on level
     * (n+2) (root is excluded and array is encountered from 0) from PT start.
     * dirLevLastRecNum[ n ] contains number of last PT record on level
     * (n+2).
     */

    u_short	dirLevBordersOff[ CDROM_MAX_DIR_LEV ];	/* first PT-record */
							/* offset in each */
							/* level */
    u_short	dirLevLastRecNum[ CDROM_MAX_DIR_LEV ];	/* last PT-record */
							/* number in each */
							/* level */
    u_short	numDirLevs;	/* number of PT dir hierarchy levels, */
				/* exclude root */
    u_short	numPTRecs;	/* last PT record number (number of records) */
    u_char	LBToLSShift;	/* to get LS number, containes given LS */
				/* number of last may be just shifted */
    u_short	type;		/* volume descriptor type, have to be one */
				/*  of ISO_VD_PRIMARY or ISO_VD_SUPPLEM */
    u_short	uniCodeLev;	/* Unicode level */
				/*  of ISO_VD_PRIMARY or ISO_VD_SUPPLEM */
				/* 0=ISO9660 */
				/* 1=UC-level1 2=UC-level2 3=UC-level3 */

    u_short	fileStructVersion; /* currently 1 only (ISO) */
    SEC_BUF	PTBuf;		   /* buffer for permanent storage PT during */
				   /* volume is mounted */
    } T_CDROMFS_VD_LST;				

typedef T_CDROMFS_VD_LST *	T_CDROMFS_VD_LST_ID;

/*---------------------------------------------------------------------------*/

/*
 * Directory record date time
 * ISO-9660: Section 9.1.5, Table 9, Page 20
 */

typedef struct cdromFileDateTime
    {					/* copied directly to this structure */
    u_char	year;	/* from 1900 */
    u_char	month;
    u_char	day;
    u_char	hour;
    u_char	minuts;
    u_char	seconds;
    u_char	fromGreenwOf;
    } T_FILE_DATE_TIME;

typedef T_FILE_DATE_TIME *	T_FILE_DATE_TIME_ID;

/*---------------------------------------------------------------------------*/

/*
 * Open file - Used for file descriptor
 */

typedef struct cdromFile
    {
    NODE	        list;	   /* Member of CDROM_VOL_DESC.FDList */
    int		        magic;	   /* FD_MAG */
    u_char	        inList;	   /* != 0 <=> FD added to volume FD list */
    T_CDROMFS_VD_LST_ID	pVDList;   /* not need to comment */

    /* File (F) static data */

    u_char	name[ CDROM_MAX_FILE_NAME_LEN + 1 ];
    u_short	parentDirNum;	   /* parent dir PT number */
    u_char *	FRecords;	   /* File's Directory Records buffer */

    u_long	FStartLB;          /* file first LB ( include EAR ) */
    u_long	FSize;		   /* total file data size (netto)	*/
    u_short	FType;		   /*  Proper constants are contained in */
    				   /*  ioLib.h                           */
#if 0			/* NOT USED until record format files supported */
    u_char	FMDUType;	   /* EAR_REC_FORM_IGNORE/		*/
				   /* CDROM_MDU_TYPE_FIX/		*/
				   /* CDROM_MDU_TYPE_VAR_LE/	*/
				   /* CDROM_MDU_TYPE_VAR_BE	*/
    u_short	FMDUSize;	   /* byte per MDU (=0, in case MDU varLen) */
#endif /* NOT USED until record format files supported */

    /* time-date */

    time_t      FDateTimeLong;     /* 32-bit seconds */

    SEC_BUF	sectBuf;	   /* Sector reading buffer. must be inited */
				   /* over openDir() (and may be, over open */
				   /* too )                                 */

    /*
     * File Current (FC) Dir Record and File Current Section descriptions
     * this fields are together used in case directory was opened (not file)
     */

    u_char *	FCDirRecPtr;	 /* ptr on current dir rec in dir rec buffer */
    u_int	FCDirRecAbsOff;	    /* current dir rec offset in directory */
    u_long	FCDirFirstRecLB; /* LB containing first directory record */

    u_long	FCSStartLB;	 /* File Section first LB ( after EAR ) */

    u_long	FCSSize;	 /* FS bytes (exclude EAR) */
    u_long	FCSAbsOff;	 /* total data size in all previous sections */
    u_short	FDType;		 /*  Proper constants are contained in */

    u_char	FCSInterleaved;	 /* CDROM_INTERLEAVED/CDROM_NOT_INTERLEAVED */
				 /* just may be checked as bolean */
    u_char	FCSFUSizeLB;	 /* LB per FileUnit */
    u_char	FCSGapSizeLB;	 /* LB per Gap */

    /* File Current Data (FCD) positions description */

    u_long	FCDAbsPos;	 /* absolute position in file (bytes) */
    int		FCEOF;		 /* !=0 <=> EOF encounted */

    /* some fields for using this structure for directories */

    u_short	DNumInPT;	 /* Directory record number in PT */

#if 0			/* NOT USED until record format files supported */
    u_long	DRecLBPT;	 /* absolute LB number, that contain  */
				 /* PT directory record                 */
    u_int	DRecOffLBPT;	 /* PT Directory record offset within LB */
#endif /* NOT USED until record format files supported */

#ifdef	CDROMFS_MODE_AUTO_COMBINE_VOLUME_DESCRIPTORS
    u_long *	pMultDir;	 /* pointer for multi VD'S struct */
    u_int	nrMultDir;	 /* number of multi VD's for search */
#endif	/* CDROMFS_MODE_AUTO_COMBINE_VOLUME_DESCRIPTORS */
    } T_CDROM_FILE;

typedef T_CDROM_FILE *	T_CDROM_FILE_ID; /* not to mix with ISO */
					 /* file ID, that equevalents */
					 /* to file name in fact */

#ifndef _ASMLANGUAGE

/* function declarations */

#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* cdromFsLibP_h */
