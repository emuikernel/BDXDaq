/* seqIo.h - Sequential I/O header file */

/* Copyright 1984-1995 Wind River Systems, Inc. */

/*
modification history
--------------------
01a,17jul95,jds  written
*/

#ifndef __INCseqIoh
#define __INCseqIoh

#ifdef __cplusplus
extern "C" {
#endif

#include "vxWorks.h"

typedef struct		/* SEQ_DEV */
    {
    FUNCPTR		sd_seqRd;		/* function to read blocks   */
    FUNCPTR		sd_seqWrt;		/* function to write blocks  */
    FUNCPTR		sd_ioctl;		/* function to ioctl device  */
    FUNCPTR		sd_seqWrtFileMarks;	/* function to write f-marks */
    FUNCPTR		sd_rewind;		/* function to rewind device */
    FUNCPTR		sd_reserve;		/* function to reserve dev   */
    FUNCPTR		sd_release;		/* function to release dev   */
    FUNCPTR		sd_readBlkLim;		/* function to read blk lims */
    FUNCPTR		sd_load;		/* function to load device   */
    FUNCPTR		sd_space;		/* function to space device  */
    FUNCPTR		sd_erase;		/* function to erase device  */
    FUNCPTR		sd_reset;		/* function to reset device  */
    FUNCPTR		sd_statusChk;		/* function to check status  */
    int 		sd_blkSize;		/* block size for device     */
    int			sd_mode;		/* O_RDONLY or O_WRONLY      */
    BOOL		sd_readyChanged;	/* indicates medium change   */
    int                 sd_maxVarBlockLimit;    /* maximum variable blk size */
    int			sd_density;		/* density of seq device     */
    } SEQ_DEV;


/* tapeFsLib ioctls */

#define FIOBLKSIZESET           1000		/* ioctl to set block size */
#define FIOBLKSIZEGET           1001		/* ioctl to get block size */
#define FIODENSITYSET           1002		/* ioctl to set tape density */
#define FIODENSITYGET           1003		/* ioctl to get tape density */
#define FIOERASE                1004		/* ioctl to erase tape */
#define MTIOCTOP                1005     	/* MTIO ioctl */


#ifdef __cplusplus
}
#endif

#endif /* __INCseqIoh */
