/* tapeFsLib.h - header for tape sequential device file system library */

/* Copyright 1984-1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,17jul95,jds  written
*/

#ifndef __INCtapeFsLibh
#define __INCtapeFsLibh

#ifdef __cplusplus
extern "C" {
#endif

#include "seqIo.h"
#include "iosLib.h"
#include "semLib.h"
#include "vwModNum.h"

/* Tape configuration parameters structure */

typedef struct 		/* TAPE_CONFIG */
    {
    int  blkSize;	/* 0 => variable blocksize; > 0 => fixed blocksize */
    BOOL rewind;	/* TRUE => a rewind device; FALSE => no rewind     */
    int  numFileMarks;	/* How many filemarks to write upon closing a file */
    int  density;	/* Tape density, which is a SEQ_DEV parameter      */ 
    } TAPE_CONFIG;


/* Tape volume descriptor */

typedef struct		/* TAPE_VOL_DESC */
    {
    DEV_HDR	    tapevd_devHdr;	/* std. I/O system device header      */
    SEM_ID	    tapevd_semId;	/* volume descriptor semaphore id     */
    SEQ_DEV *       tapevd_pSeqDev;	/* ptr to sequential device info      */
    struct tapefd * tapevd_pTapeFd;	/* ptr to file descriptor 	      */
    int		    tapevd_status;	/* (OK | ERROR)                       */
    int		    tapevd_state;	/* state of volume (see below)        */
    int		    tapevd_retry;	/* current retry count for I/O errors */
    BOOL	    tapevd_rewind;	/* is this a rewind | norewind volume */
    int		    tapevd_blkSize;	/* 0 => variable blk; >0 => fixed blk */
    int		    tapevd_numFileMarks;/* num of f-marks to write upon close */
    int		    tapevd_density;	/* density of the tape device         */
    } TAPE_VOL_DESC;


/* Tape file descriptor */

typedef struct tapefd	/* TAPE_FILE_DESC */
    {
    TAPE_VOL_DESC * tapefd_pTapeVol;	/* pointer to tape volume descriptor */
    BOOL	    tapefd_inUse;	/* is the tape fd in use             */
    int	 	    tapefd_mode;	/* mode of tape O_RDONLY | O_WRONLY  */

			/* Buffering is used for fixed blocks only   */

    char *	    tapefd_buffer;	/* pointer to read/write buffer area */
    int 	    tapefd_bufSize;	/* buffer size 			     */
    int		    tapefd_bufIndex;	/* current byte index within the buf */
    } TAPE_FILE_DESC;


/*
 * Structure that defines the Unix MT ioctl parameter passed to the ioctl 
 * routine in tapeFsLib
 */

typedef struct mtop 
    {
    short 	mt_op;			/* operation */
    int		mt_count;		/* number of operations */
    } MTOP;


/* Volume states */

#define TAPE_VD_READY_CHANGED	0	/* vol not accessed since ready change*/
#define TAPE_VD_MOUNTED		2	/* volume mounted                     */
#define TAPE_VD_UNMOUNTED  	4	/* volume unmounted                   */

#define VARIABLE_BLOCK_SIZE     0 	/* blkSize of 0 means variable block  */
#define LOAD   			TRUE	/* load or mount a volume             */
#define UNLOAD 			FALSE 	/* unload or unmount a volume         */
#define LONG   			TRUE 	/* erase entire tape (long erase)     */
#define RETEN  			TRUE 	/* retension the tape during load     */
#define EOT  			TRUE 	/* unload tape at the end-of-tape     */
#define FIXED_BLK		TRUE	/* use fixed block size               */
#define VARIABLE_BLK		FALSE   /* use variable block size	      */

/* defined in scsiLib */

#define SPACE_CODE_DATABLK  	0x00   	/* space data blocks		      */
#define SPACE_CODE_FILEMARK 	0x01	/* space file-marks		      */

/* I/O controls */

/* MTIOCTOP operations */

#define MTWEOF          0       /* write an end-of-file record */
#define MTFSF           1       /* forward space over file mark */
#define MTBSF           2       /* backward space over file mark (1/2" only) */
#define MTFSR           3       /* forward space to inter-record gap */
#define MTBSR           4       /* backward space to inter-record gap */
#define MTREW           5       /* rewind */
#define MTOFFL          6       /* rewind and put the drive offline */
#define MTNOP           7       /* no operation, sets status only */
#define MTRETEN         8       /* retension the tape (cartridge tape only) */
#define MTERASE         9       /* erase the entire tape */
#define MTEOM           10      /* position to end of media */
#define MTNBSF          11      /* backward space file to BOF */

/* tapeFsLib errnos */

#define S_tapeFsLib_NO_SEQ_DEV 			(M_tapeFsLib | 1)
#define S_tapeFsLib_ILLEGAL_TAPE_CONFIG_PARM 	(M_tapeFsLib | 2)
#define S_tapeFsLib_SERVICE_NOT_AVAILABLE	(M_tapeFsLib | 3)
#define S_tapeFsLib_INVALID_BLOCK_SIZE		(M_tapeFsLib | 4)
#define S_tapeFsLib_ILLEGAL_FILE_SYSTEM_NAME	(M_tapeFsLib | 5)
#define S_tapeFsLib_ILLEGAL_FLAGS		(M_tapeFsLib | 6)
#define S_tapeFsLib_FILE_DESCRIPTOR_BUSY	(M_tapeFsLib | 7)
#define S_tapeFsLib_VOLUME_NOT_AVAILABLE	(M_tapeFsLib | 8)
#define S_tapeFsLib_BLOCK_SIZE_MISMATCH		(M_tapeFsLib | 9)
#define S_tapeFsLib_INVALID_NUMBER_OF_BYTES	(M_tapeFsLib | 10)

/* Function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern TAPE_VOL_DESC * tapeFsDevInit (char *volName, SEQ_DEV *pBlkDev,
						    TAPE_CONFIG *pTapeConfig);
extern STATUS 	       tapeFsInit ();
extern STATUS 	       tapeFsVolUnmount (TAPE_VOL_DESC *vdptr);
extern STATUS 	       tapeFsReadyChange (TAPE_VOL_DESC *vdptr);

#else	/* __STDC__ */

extern TAPE_VOL_DESC * tapeFsDevInit ();
extern STATUS 	       tapeFsInit ();
extern STATUS 	       tapeFsVolUnmount ();
extern STATUS 	       tapeFsReadyChange ();

#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* __INCtapeFsLibh */
