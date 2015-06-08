/* ataDrv.h - ATA/IDE (LOCAL and PCMCIA) disk controller header */

/* Copyright 1984-2002 Wind River Systems, Inc. */
/*
modification history
--------------------
01h,29apr02,pmr  SPR 76487: changed ataDrv() arg configType from BOOL to int.
01g,09nov01,jac  SPR#67795, added definitions for ATAPI CDROM extensions.
01f,30oct97,db   added macro ATA_SWAP for big endian targets. changed some 
                 elements in ATA_CTRL from short to UINT32.
01e,10dec97,ms   added usrAtaConfig prototype and ataResources import
01e,01nov96,hdn  added support for PCMCIA
01d,25sep96,hdn  added support for ATA-2
01c,18mar96,hdn  added ataShowInit().
01b,01mar96,hdn  cleaned up.
01a,02mar95,hdn  written based on ideDrv.h.
*/

#ifndef __INCpcataDrvh
#define __INCpcataDrvh

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE


#include "dosFsLib.h"
#include "blkIo.h"
#include "wdLib.h"
#include "private/semLibP.h"
#include "drv/pcmcia/pccardLib.h"

IMPORT ATA_RESOURCE ataResources[];
/* define swap macro for little/big endian machines */

#if _BYTE_ORDER == _BIG_ENDIAN
#   define ATA_SWAP(x) LONGSWAP(x)
#else
#   define ATA_SWAP(x) (x)
#endif

#define IDE_LOCAL		0 	/* ctrl type: LOCAL(IDE) */
#define ATA_PCMCIA		1	/* ctrl type: PCMCIA */

#define ATA_MAX_CTRLS		2	/* max number of ATA controller */
#define ATA_MAX_DRIVES		2	/* max number of ATA drives */

typedef struct ataParams 
    {
    short config;		/* general configuration */
    short cylinders;		/* number of cylinders */
    short removcyl;		/* number of removable cylinders */
    short heads;		/* number of heads */
    short bytesTrack;		/* number of unformatted bytes/track */
    short bytesSec;		/* number of unformatted bytes/sector */
    short sectors;		/* number of sectors/track */
    short bytesGap;		/* minimum bytes in intersector gap */
    short bytesSync;		/* minimum bytes in sync field */
    short vendstat;		/* number of words of vendor status */
    char  serial[20];		/* controller serial number */
    short type;			/* controller type */
    short size;			/* sector buffer size, in sectors */
    short bytesEcc;		/* ecc bytes appended */
    char  rev[8];		/* firmware revision */
    char  model[40];		/* model name */
    short multiSecs;		/* RW multiple support. bits 7-0 ia max secs */
    short reserved48;		/* reserved */
    short capabilities;		/* capabilities */
    short reserved50;		/* reserved */
    short pioMode;		/* PIO data transfer cycle timing mode */
    short dmaMode;		/* single word DMA data transfer cycle timing */
    short valid;		/* field validity */
    short currentCylinders;	/* number of current logical cylinders */
    short currentHeads;		/* number of current logical heads */
    short currentSectors;	/* number of current logical sectors / track */
    short capacity0;		/* current capacity in sectors */
    short capacity1;		/* current capacity in sectors */
    short multiSet;		/* multiple sector setting */
    short sectors0;		/* total number of user addressable sectors */
    short sectors1;		/* total number of user addressable sectors */
    short singleDma;		/* single word DMA transfer */
    short multiDma;		/* multi word DMA transfer */
    short advancedPio;		/* flow control PIO transfer modes supported */
    short cycletimeDma;		/* minimum multiword DMA transfer cycle time */
    short cycletimeMulti;	/* recommended multiword DMA cycle time */
    short cycletimePioNoIordy;	/* min PIO transfer cycle time wo flow ctl */
    short cycletimePioIordy;	/* min PIO transfer cycle time w IORDY */
    short reserved69;		/* reserved */
    short reserved70;		/* reserved */

    /* ATAPI */

    short pktCmdRelTime;	/* [71]Typical Time for Release after Packet */
    short servCmdRelTime;	/* [72]Typical Time for Release after SERVICE */
    short majorRevNum;		/* [73] Major Revision Number (0|FFFF if no) */
    short minorVersNum;		/* [74] Minor  Version Number (0|FFFF if no) */
    short reserved75[53];	/* reserved */
    short vendor[32];		/* vendor specific */
    short reserved160[96];	/* reserved */
    } ATA_PARAM;

typedef enum /* ATAPI */
    {
    NON_DATA, /* non data command */
    OUT_DATA,
    IN_DATA
    } t_data_dir;

/* Device types */

#define ATA_TYPE_NONE		0x00	/* device is faulty or not present */

#define ATA_TYPE_ATA		0x01	/* ATA device */

#define ATA_TYPE_ATAPI		0x02	/* ATAPI device */

#define ATA_TYPE_INIT		255	/* device must be identified */

/* Device  states */

#define ATA_DEV_OK	0	/* device is OK */

#define ATA_DEV_NONE	1	/* device absent or does not respond */

#define ATA_DEV_DIAG_F	2	/* device diagnostic failed */

#define ATA_DEV_PREAD_F	3	/* read device parameters failed */

#define ATA_DEV_MED_CH	4	/* medium have been changed */

#define ATA_DEV_NO_BLKDEV	5	/* No block device available */

#define ATA_DEV_INIT	255	/* uninitialized device */

typedef struct ataDev
    {
    BLK_DEV     blkDev;		/* must be here */
    int         ctrl;		/* ctrl no.  0 - 1 */
    int         drive;		/* drive no. 0 - 1 */
    int         blkOffset;	/* sector offset */
    int		nBlocks;	/* number of sectors */

    char *	pBuf;		/* Current position in an user buffer */
    char *	pBufEnd;	/* End of user buffer */
    t_data_dir	direction;	/* Transfer direction */
    int		transCount;	/* Number of transfer cycles */
    int		errNum;		/* Error description message number */

    /* ATAPI Registers contents */

    uint8_t	intReason;	/* Interrupt Reason Register */
    uint8_t	status;		/* Status Register */
    uint16_t	transSize;	/* Byte Count Register */
    } ATA_DEV;

typedef struct ataDrive
    {
    ATA_PARAM	param;		/* geometry parameter */
    short	okMulti;	/* MULTI: TRUE if supported */
    short	okIordy;	/* IORDY: TRUE if supported */
    short	okDma;		/* DMA:   TRUE if supported */
    short	okLba;		/* LBA:   TRUE if supported */
    short	multiSecs;	/* supported max sectors for multiple RW */
    short	pioMode;	/* supported max PIO mode */
    short	singleDmaMode;	/* supported max single word DMA mode */
    short	multiDmaMode;	/* supported max multi word DMA mode */
    short	rwMode;		/* RW mode: PIO[0,1,2,3,4] or DMA[0,1,2] */
    short	rwBits;		/* RW bits: 16 or 32 */
    short	rwPio;		/* RW PIO unit: single or multi sector */
    short	rwDma;		/* RW DMA unit: single or multi word */

    uint8_t	state;		/* device state */
    uint8_t	diagCode;	/* diagnostic code */
    uint8_t	type;		/* device type */
    STATUS      (*Reset)	/* pointer to reset function */
        (
        int ctrl,
        int dev
        );

    ATA_DEV     *pAtaDev;	/* pointer to ATA block device structure */
    } ATA_DRIVE;

typedef struct ataCtrl
    {
    ATA_DRIVE	drive[ATA_MAX_DRIVES];	/* drives per controller */
    SEMAPHORE	syncSem;	/* binary sem for syncronization */
    SEMAPHORE	muteSem;	/* mutex  sem for mutual-exclusion */
    WDOG_ID	wdgId;		/* watch dog */
    BOOL	wdgOkay;	/* watch dog status */
    int		semTimeout;	/* timeout seconds for sync semaphore */
    int		wdgTimeout;	/* timeout seconds for watch dog */
    int		ctrlType;	/* type of controller */
    BOOL	installed;	/* TRUE if a driver is installed */
    BOOL	changed;	/* TRUE if a card is installed */
    int		intLevel;	/* interrupt level */
    int		intCount;	/* interrupt count */
    int		intStatus;	/* interrupt status */
    int		drives;		/* number of drives in the controller */
    UINT32	data;		/* (RW) data register (16 bits)	*/
    UINT32	error;		/* (R)  error register		*/
    UINT32	feature;	/* (W)  feature or write-precompensation */
    UINT32	seccnt;		/* (RW) sector count		*/
    UINT32	sector;		/* (RW) first sector number	*/
    UINT32	cylLo;		/* (RW) cylinder low byte	*/
    UINT32	cylHi;		/* (RW) cylinder high byte	*/
    UINT32	sdh;		/* (RW) sector size/drive/head	*/
    UINT32	command;	/* (W)  command register	*/
    UINT32	status;		/* (R)  immediate status	*/
    UINT32	aStatus;	/* (R)  alternate status	*/
    UINT32	dControl;	/* (W)  disk controller control	*/
    UINT32	dAddress;	/* (R)  disk controller address */

    int		configType;	/* or'd configuration flags     */
    } ATA_CTRL;

typedef struct ataType
    {
    int cylinders;		/* number of cylinders */
    int heads;			/* number of heads */
    int sectors;		/* number of sectors per track */
    int bytes;			/* number of bytes per sector */
    int precomp;		/* precompensation cylinder */
    } ATA_TYPE;

typedef struct ataRaw
    {				/* this is for ATARAWACCESS ioctl */
    UINT cylinder;		/* cylinder (0 -> (cylindres-1)) */
    UINT head;			/* head (0 -> (heads-1)) */
    UINT sector;		/* sector (1 -> sectorsTrack) */
    char *pBuf;			/* pointer to buffer (bytesSector * nSecs) */
    UINT nSecs;			/* number of sectors (1 -> sectorsTrack) */
    UINT direction;		/* read=0, write=1 */
    } ATA_RAW;


#define ATA_SEM_TIMEOUT_DEF	5       /* default timeout for ATA sync sem */
#define ATA_WDG_TIMEOUT_DEF	5       /* default timeout for ATA watch dog */


/* ATA registers */

#define	ATA_DATA(base0)		(base0 + 0) /* (RW) data register (16 bits) */
#define ATA_ERROR(base0)	(base0 + 1) /* (R)  error register	    */
#define	ATA_FEATURE(base0)	(base0 + 1) /* (W)  feature/precompensation */
#define	ATA_SECCNT(base0)	(base0 + 2) /* (RW) sector count	    */
#define	ATA_SECTOR(base0)	(base0 + 3) /* (RW) first sector number	    */
#define	ATA_CYL_LO(base0)	(base0 + 4) /* (RW) cylinder low byte	    */
#define	ATA_CYL_HI(base0)	(base0 + 5) /* (RW) cylinder high byte	    */
#define	ATA_SDH(base0)		(base0 + 6) /* (RW) sector size/drive/head  */
#define	ATA_COMMAND(base0)	(base0 + 7) /* (W)  command register	    */
#define	ATA_STATUS(base0) 	(base0 + 7) /* (R)  immediate status	    */
#define	ATA_A_STATUS(base1)	(base1 + 0) /* (R)  alternate status	    */
#define	ATA_D_CONTROL(base1)	(base1 + 0) /* (W)  disk controller control */
#define	ATA_D_ADDRESS(base1)	(base1 + 1) /* (R)  disk controller address */


/* diagnostic code */

#define ATA_DIAG_OK		0x01

/* control register */

#define ATA_CTL_4BIT	 	0x8	/* use 4 head bits (wd1003) */
#define ATA_CTL_RST 		0x4	/* reset controller */
#define ATA_CTL_IDS 		0x2	/* disable interrupts */

/* status register */

#define ATA_STAT_ACCESS		(ATA_STAT_BUSY | ATA_STAT_DRQ)
					/* device accessible */

#define	ATA_STAT_BUSY		0x80	/* controller busy */
#define	ATA_STAT_READY		0x40	/* selected drive ready */
#define	ATA_STAT_WRTFLT		0x20	/* write fault */
#define	ATA_STAT_SEEKCMPLT	0x10	/* seek complete */
#define	ATA_STAT_DRQ		0x08	/* data request */
#define	ATA_STAT_ECCCOR		0x04	/* ECC correction made in data */
#define	ATA_STAT_INDEX		0x02	/* index pulse from selected drive */
#define	ATA_STAT_ERR		0x01	/* error detect */

/* size/drive/head register: addressing mode CHS or LBA */

#define ATA_SDH_IBM		0xa0	/* chs, 512 bytes sector, ecc */
#define ATA_SDH_LBA		0xe0	/* lba, 512 bytes sector, ecc */

/* commands */

#define	ATA_CMD_RECALIB		0x10	/* recalibrate */
#define	ATA_CMD_SEEK		0x70	/* seek */
#define	ATA_CMD_READ		0x20	/* read sectors with retries */
#define	ATA_CMD_WRITE		0x30	/* write sectors with retries */
#define	ATA_CMD_FORMAT		0x50	/* format track */
#define	ATA_CMD_DIAGNOSE	0x90	/* execute controller diagnostic */
#define	ATA_CMD_INITP		0x91	/* initialize drive parameters */
#define	ATA_CMD_READP		0xEC	/* identify */
#define	ATA_CMD_SET_FEATURE 	0xEF	/* set features */
#define	ATA_CMD_SET_MULTI	0xC6	/* set multi */
#define	ATA_CMD_READ_MULTI	0xC4	/* read multi */
#define	ATA_CMD_WRITE_MULTI	0xC5	/* write multi */

/* ATAPI commands */

#define	ATA_PI_CMD_SRST		0x08	/* ATAPI Soft Reset */
#define	ATA_PI_CMD_PKTCMD	0xA0	/* ATAPI Pakcet Command */
#define	ATA_PI_CMD_IDENTD	0xA1	/* ATAPI Identify Device */
#define	ATA_PI_CMD_SERVICE	0xA2	/* Service */

/* CDROM commands */

#define CDROM_CMD_TEST_UNIT_READY  0x00     /* CDROM Test Unit Ready      */
#define CDROM_CMD_REQUEST_SENSE    0x03     /* CDROM Request Sense        */
#define CDROM_CMD_INQUIRY          0x12     /* CDROM Inquiry              */
#define CDROM_CMD_READ_CDROM_CAP   0x25     /* CDROM Read CD-ROM Capacity */
#define CDROM_CMD_READ_12          0xA8     /* CDROM Read (12)            */

/* sub command of ATA_CMD_SET_FEATURE */

#define ATA_SUB_ENABLE_8BIT	0x01	/* enable 8bit data transfer */
#define ATA_SUB_ENABLE_WCACHE	0x02	/* enable write cache */
#define ATA_SUB_SET_RWMODE	0x03	/* set transfer mode */
#define ATA_SUB_DISABLE_RETRY	0x33	/* disable retry */
#define ATA_SUB_SET_LENGTH	0x44	/* length of vendor specific bytes */
#define ATA_SUB_SET_CACHE	0x54	/* set cache segments */
#define ATA_SUB_DISABLE_LOOK	0x55	/* disable read look-ahead feature */
#define ATA_SUB_DISABLE_REVE	0x66	/* disable reverting to power on def */
#define ATA_SUB_DISABLE_ECC	0x77	/* disable ECC */
#define ATA_SUB_DISABLE_8BIT	0x81	/* disable 8bit data transfer */
#define ATA_SUB_DISABLE_WCACHE	0x82	/* disable write cache */
#define ATA_SUB_ENABLE_ECC	0x88	/* enable ECC */
#define ATA_SUB_ENABLE_RETRY	0x99	/* enable retries */
#define ATA_SUB_ENABLE_LOOK	0xaa	/* enable read look-ahead feature */
#define ATA_SUB_SET_PREFETCH	0xab	/* set maximum prefetch */
#define ATA_SUB_SET_4BYTES	0xbb	/* 4 bytes of vendor specific bytes */
#define ATA_SUB_ENABLE_REVE	0xcc	/* enable reverting to power on def */

/* transfer modes of ATA_SUB_SET_RWMODE */

#define ATA_PIO_DEF_W		0x00	/* PIO default trans. mode */
#define ATA_PIO_DEF_WO		0x01	/* PIO default trans. mode, no IORDY */
#define ATA_PIO_W_0		0x08	/* PIO flow control trans. mode 0 */
#define ATA_PIO_W_1		0x09	/* PIO flow control trans. mode 1 */
#define ATA_PIO_W_2		0x0a	/* PIO flow control trans. mode 2 */
#define ATA_PIO_W_3		0x0b	/* PIO flow control trans. mode 3 */
#define ATA_PIO_W_4		0x0c	/* PIO flow control trans. mode 4 */
#define ATA_DMA_SINGLE_0	0x10	/* singleword DMA mode 0 */
#define ATA_DMA_SINGLE_1	0x11	/* singleword DMA mode 1 */
#define ATA_DMA_SINGLE_2	0x12	/* singleword DMA mode 2 */
#define ATA_DMA_MULTI_0		0x20	/* multiword DMA mode 0 */
#define ATA_DMA_MULTI_1		0x21	/* multiword DMA mode 1 */
#define ATA_DMA_MULTI_2		0x22	/* multiword DMA mode 2 */
#define ATA_MAX_RW_SECTORS	0x100	/* max sectors per transfer */

/* configuration flags: transfer mode, bits, unit, geometry */

#define ATA_PIO_DEF_0		ATA_PIO_DEF_W	/* PIO default mode */
#define ATA_PIO_DEF_1		ATA_PIO_DEF_WO	/* PIO default mode, no IORDY */
#define ATA_PIO_0		ATA_PIO_W_0	/* PIO mode 0 */
#define ATA_PIO_1		ATA_PIO_W_1	/* PIO mode 1 */
#define ATA_PIO_2		ATA_PIO_W_2	/* PIO mode 2 */
#define ATA_PIO_3		ATA_PIO_W_3	/* PIO mode 3 */
#define ATA_PIO_4		ATA_PIO_W_4	/* PIO mode 4 */
#define ATA_PIO_AUTO		0x000d		/* PIO max supported mode */
#define ATA_DMA_0		0x0002		/* DMA mode 0 */
#define ATA_DMA_1		0x0003		/* DMA mode 1 */
#define ATA_DMA_2		0x0004		/* DMA mode 2 */
#define ATA_DMA_AUTO		0x0005		/* DMA max supported mode */
#define ATA_MODE_MASK		0x000F		/* transfer mode mask */

#define ATA_BITS_16		0x0040		/* RW bits size, 16 bits */
#define ATA_BITS_32		0x0080		/* RW bits size, 32 bits */
#define ATA_BITS_MASK		0x00c0		/* RW bits size mask */

#define ATA_PIO_SINGLE		0x0010		/* RW PIO single sector */
#define ATA_PIO_MULTI		0x0020		/* RW PIO multi sector */
#define ATA_PIO_MASK		0x0030		/* RW PIO mask */

#define ATA_DMA_SINGLE		0x0400		/* RW DMA single word */
#define ATA_DMA_MULTI		0x0800		/* RW DMA multi word */
#define ATA_DMA_MASK		0x0c00		/* RW DMA mask */

#define ATA_GEO_FORCE		0x0100		/* set geometry in the table */
#define ATA_GEO_PHYSICAL	0x0200		/* set physical geometry */
#define ATA_GEO_CURRENT		0x0300		/* set current geometry */
#define ATA_GEO_MASK		0x0300		/* geometry mask */


/****************************** ATAPI Devices ******************************/

/* config */

#define CONFIG_PROT_TYPE	0xc000 /* Protocol Type */
#define CONFIG_PROT_TYPE_ATAPI	0x8000 /* ATAPI */

#define CONFIG_DEV_TYPE		0x1f00 /* Device Type */
#define CONFIG_DEV_TYPE_CD_ROM	0x0500

#define CONFIG_REMOVABLE	0x0080 /* Removable */

#define CONFIG_PKT_TYPE		0x0060 /* CMD DRQ Type */
#define CONFIG_PKT_TYPE_MICRO	0x0000 /* Microprocessor DRQ */
#define CONFIG_PKT_TYPE_INTER	0x0020 /* Interrupt DRQ */
#define CONFIG_PKT_TYPE_ACCEL	0x0040 /* Accelerated DRQ */

#define CONFIG_PKT_SIZE		0x0003 /* Command Packet Size */
#define CONFIG_PKT_SIZE_12	0x0000 /* 12 bytes */

/* capabilities */

#define CAPABIL_DMA		0x0100	/* DMA Supported */
#define CAPABIL_LBA		0x0200	/* LBA Supported */
#define CAPABIL_IORDY_CTRL	0x0400	/* IORDY can be disabled */
#define CAPABIL_IORDY		0x0800	/* IORDY Supported */
#define CAPABIL_OVERLAP		0x2000	/* Overlap Operation Supported */

/* valid */

#define FIELDS_VALID		0x0002

/* singleDma */
 
#define SINGLEDMA_MODE		0xff00	/* 15-8: mode active */
#define SINGLEDMA_SUPPORT	0x00ff	/* 7-0: modes supported */

/* multiDma */
 
#define MULTIDMA_MODE		0xff00	/* 15-8: mode active */
#define MULTIDMA_SUPPORT	0x00ff	/* 7-0: modes supported */

/* advPio */

#define ADVPIO_MODE3		0x0001	/* The Device supports PIO Mode 3 */

/* ATAPI registers */

#define	ATAPI_DATA	pCtrl->data	/* ATA_DATA (RW) data reg. (16 bits) */
#define ATAPI_ERROR	pCtrl->error	/* ATA_ERROR (R) error reg. */
#define	ATAPI_FEATURE	pCtrl->feature	/* ATA_FEATURE (W) feature reg. */
#define	ATAPI_INTREASON	pCtrl->seccnt	/* ATA_SECCNT (R) interrupt reason */
#define	ATAPI_BCOUNT_LO	pCtrl->cylLo	/* ATA_CYL_LO (RW) byte count (low) */
#define	ATAPI_BCOUNT_HI	pCtrl->cylHi	/* ATA_CYL_HI (RW) byte count (high) */
#define	ATAPI_D_SELECT	pCtrl->sdh	/* ATA_SDH (RW) drive select reg. */
#define	ATAPI_STATUS	pCtrl->status 	/* ATA_STATUS (R) status reg. */
#define	ATAPI_COMMAND	pCtrl->command	/* ATA_COMMAND (W) command reg. */
#define	ATAPI_D_CONTROL	pCtrl->dControl	/* ATA_D_CONTROL (W) device control */

/* Error Register */

#define	ERR_SENSE_KEY		0xf0	/* Sense Key mask */
#define	SENSE_NO_SENSE		0x00 /* no sense sense key */
#define	SENSE_RECOVERED_ERROR	0x10 /* recovered error sense key */
#define	SENSE_NOT_READY		0x20 /* not ready sense key */
#define	SENSE_MEDIUM_ERROR	0x30 /* medium error sense key */
#define	SENSE_HARDWARE_ERROR	0x40 /* hardware error sense key */
#define	SENSE_ILLEGAL_REQUEST	0x50 /* illegal request sense key */
#define	SENSE_UNIT_ATTENTION	0x60 /* unit attention sense key */
#define	SENSE_DATA_PROTECT	0x70 /* data protect sense key */
#define	SENSE_ABBORTED_COMMAND	0xb0 /* aborted command sense key */
#define	SENSE_MISCOMPARE	0xe0 /* miscompare sense key */
#define	ERR_MCR			0x08	/* Media Change Requested */
#define	ERR_ABRT		0x04	/* Aborted command */
#define	ERR_EOM 		0x02	/* End Of Media */
#define	ERR_ILI 		0x01	/* Illegal Length Indication */

/* Feature Register */

#define	FEAT_OVERLAP	0x02	/* command may be overlapped */
#define	FEAT_DMA	0x01	/* data will be transferred via DMA */

/* Interrupt Reason Register */

#define	INTR_RELEASE	0x04	/* Bus released before completing the command */
#define	INTR_IO		0x02	/* 1 - In to the Host; 0 - Out to the device */
#define	INTR_COD	0x01	/* 1 - Command; 0 - user Data */

/* Drive Select Register */

#define	DSEL_FILLER	0xa0	/* to fill static fields */
#define	DSEL_DRV	0x10	/* Device 0 (DRV=0) or 1 (DRV=1) */

/* Status Register */

#define	STAT_BUSY	0x80	/* controller busy */
#define	STAT_READY	0x40	/* selected drive ready */

#define STAT_DMA_READY	0x20	/* ready to a DMA data transfer */

#define	STAT_WRTFLT	0x20	/* write fault */

#define STAT_SERVICE	0x10	/* service or interrupt request */

#define	STAT_SEEKCMPLT	0x10	/* seek complete */
#define	STAT_DRQ	0x08	/* data request */
#define	STAT_ECCCOR	0x04	/* ECC correction made in data */
#define	STAT_ERR	0x01	/* error detect */

/* Device Control Register */

#define	CTL_FILLER	0x8		/* bit 3 must be always set */
#define	CTL_RST		0x4		/* reset controller */
#define	CTL_IDS		0x2		/* disable interrupts */

/**********************************************************************/

/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern STATUS	ataDrv		(int ctrl, int drives, int vector, int level,
				 int configType, int semTimeout,
				 int wdgTimeout);
extern BLK_DEV	*ataDevCreate	(int ctrl, int drive, int nBlks, int offset);
extern STATUS	ataRawio	(int ctrl, int drive, ATA_RAW *pAtaRaw);
extern void	ataShowInit	(void);
extern STATUS	ataShow		(int ctrl, int drive);
extern int	usrAtaPartition	(int ctrl, int drive, DOS_PART_TBL *pPart);
extern STATUS   usrAtaConfig	(int ctrl, int drive, char *fileName);
extern void     usrAtaInit	(void);

#else

extern STATUS	ataDrv		();
extern BLK_DEV	*ataDevCreate	();
extern STATUS	ataRawio	();
extern void	ataShowInit	();
extern STATUS	ataShow		();
extern int	usrAtaPartition	();
extern STATUS   usrAtaConfig	();
extern void     usrAtaInit	();

#endif  /* __STDC__ */


#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCpcataDrvh */
