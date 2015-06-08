/* scsi2Lib.h - SCSI library header file */

/* Copyright 1984-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
04o,18oct01,pmr  setting SCSI_SYNC_XFER_MIN_PERIOD to 1 so that highest speed
                 is negotiated (SPR 71337).
04n,08oct01,rcs  added SCSI_ADD_SENSE_NO_MEDIUM SPR# 32203
04m,10jul97,dds  added new fields to SCSI_CTRL structure.
04l,28mar97,dds  SPR 8220: added new event type SCSI_EVENT_PARITY_ERR.
04k,06mar97,dds  SPR 8120: changed declaration for scsiRdTape.
04j,13sep96,dds  added declarations required by scsiMgrLib.c and scsiCtrlLib.c
04i,29jul96,jds  added hardware cache snooping support
04h,15apr96,jds  added WIDE data transfer support
04g,20jul95,jds  added scsiSeqStatusCheck as a global function
04f,24jul95,jds  added some additional sense key macros; added pointer to
		 SEQ_DEV in SCSI_PHYS_DEV
04e,28apr95,jds  integrated into WRS tree; added backward compatability 
04d,24mar95,ihw  added "identMsg" field in SCSI_EVENT structure
04c,07oct94,ihw  added errno's for reaching max threads, bad sense data
04b,27may94,ihw  documented prior to release
04a,30mar94,ihw  modified for enhanced SCSI library: tagged command queueing
                 removed redundant "pAddLengthByte" field from SCSI_PHYS_DEV
		 added definitions for proper handling of max length ext msgs
03a,12jan94,ihw  modified for enhanced SCSI library: multiple initiators,
    	    	    disconnect/reconnect and synchronous transfer supported
02j,11jan93,ccc  added #ifndef _ASMLANGUAGE.
02i,30sep92,ccc  added two new errno's.
02h,24sep92,ccc  removed timeout parameter to scsiAutoConfig().
02g,22sep92,rrr  added support for c++
02f,27aug92,ccc  added timeout to SCSI_TRANSACTION structure.
02e,26jul92,rrr  Removed decl of scsiSyncTarget, it was made LOCAL.
02d,20jul92,eve  Remove conditional compilation.
02c,14jul92,eve  added a pScsiXaction info in SCSI_PHYS_DEV structure to
		 maintain the data direction information to support
		 cache coherency in drivers.
02b,06jul92,eve  added declaration for extended sync functionalities.
02a,04jul92,jcf  cleaned up.
01l,26may92,rrr  the tree shuffle
01k,07apr92,yao  changed BYTE_ORDER to _BYTE_ORDER, BIG_ENDIAN to _BIG_ENDIAN.
01j,04oct91,rrr  passed through the ansification filter
		  -fixed #else and #endif
		  -changed READ, WRITE and UPDATE to O_RDONLY O_WRONLY O_RDWR
		  -changed VOID to void
		  -changed copyright notice
01i,07mar91,jcc  added SCSI_BLK_DEV_LIST and SCSI_BLK_DEV_NODE definitions;
    	    	 added SCSI_SWAB macro. added a few error codes.
01h,05oct90,shl  added ANSI function prototypes.
                 added copyright notice.
01g,02oct90,jcc  changed SEM_ID's to SEMAPHORE's in SCSI_PHYS_DEV and SCSI_CTRL
		 structures; modified structures to return variable length
		 REQUEST SENSE data; miscellaneous.
01f,20aug90,jcc  added conditional defines and typedefs for 4.0.2 compatibility.
01e,10aug90,dnw  changed scsiBusReset and scsiSelTimeOutCvt to VOIDFUNCPTR.
01d,07aug90,shl  moved function declarations to end of file.
01c,12jul90,jcc  misc. enhancements and changes.
01b,08jun90,jcc  added scsiMsgInAck to SCSI_CTRL, which routine is invoked
		 to accept incoming messages.
01a,17apr89,jcc  written.
*/

#ifndef __INCscsi2Libh
#define __INCscsi2Libh

#ifndef	_ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

#include "vwModNum.h"
#include "blkIo.h"
#include "seqIo.h"
#include "semLib.h"
#include "lstLib.h"
#include "msgQLib.h"
#include "rngLib.h"
#include "taskLib.h"
#include "wdLib.h"


/* SCSI direct-access device and general purpose command opcodes are enumerated.
 * NOTE: Some commands are optional and may not be supported by all devices.
 */

#define SCSI_OPCODE_TEST_UNIT_READY     ((UINT8) 0x00)  /* test unit ready */
#define SCSI_OPCODE_REZERO_UNIT         ((UINT8) 0x01)  /* rezero unit */
#define SCSI_OPCODE_REWIND		((UINT8) 0x01)  /* rewind */
#define SCSI_OPCODE_REQUEST_SENSE       ((UINT8) 0x03)  /* request sense */
#define SCSI_OPCODE_FORMAT_UNIT         ((UINT8) 0x04)  /* format unit */
#define SCSI_OPCODE_READ_BLOCK_LIMITS	((UINT8) 0x05)  /* read block limits */
#define SCSI_OPCODE_REASSIGN_BLOCKS     ((UINT8) 0x07)  /* reassign blocks */
#define SCSI_OPCODE_READ                ((UINT8) 0x08)  /* read */
#define SCSI_OPCODE_RECEIVE		((UINT8) 0x08)  /* receive */
#define SCSI_OPCODE_WRITE               ((UINT8) 0x0a)  /* write */
#define SCSI_OPCODE_PRINT               ((UINT8) 0x0a)  /* print */
#define SCSI_OPCODE_SEND		((UINT8) 0x0a)  /* send */
#define SCSI_OPCODE_SEEK                ((UINT8) 0x0b)  /* seek */
#define SCSI_OPCODE_TRACK_SELECT	((UINT8) 0x0b)  /* track select */
#define SCSI_OPCODE_SLEW_AND_PRINT	((UINT8) 0x0b)  /* slew and print */
#define SCSI_OPCODE_READ_REVERSE	((UINT8) 0x0f)  /* read reverse */
#define SCSI_OPCODE_WRITE_FILEMARKS	((UINT8) 0x10)  /* write filemarks */
#define SCSI_OPCODE_FLUSH_BUFFER	((UINT8) 0x10)  /* flush buffer */
#define SCSI_OPCODE_SPACE		((UINT8) 0x11)  /* space */
#define SCSI_OPCODE_INQUIRY             ((UINT8) 0x12)  /* inquiry */
#define SCSI_OPCODE_VERIFY_SEQ		((UINT8) 0x13)  /* seq. access verify */
#define SCSI_OPCODE_RECOVER_BUF_DATA	((UINT8) 0x14)  /* rec. buffered data */
#define SCSI_OPCODE_MODE_SELECT         ((UINT8) 0x15)  /* mode select */
#define SCSI_OPCODE_RESERVE             ((UINT8) 0x16)  /* reserve */
#define SCSI_OPCODE_RELEASE             ((UINT8) 0x17)  /* release */
#define SCSI_OPCODE_COPY		((UINT8) 0x18)  /* copy */
#define SCSI_OPCODE_ERASE		((UINT8) 0x19)  /* erase */
#define SCSI_OPCODE_MODE_SENSE          ((UINT8) 0x1a)  /* mode sense */
#define SCSI_OPCODE_START_STOP_UNIT     ((UINT8) 0x1b)  /* start/stop unit */
#define SCSI_OPCODE_LOAD_UNLOAD		((UINT8) 0x1b)  /* load/unload */
#define SCSI_OPCODE_STOP_PRINT		((UINT8) 0x1b)  /* stop print */
#define SCSI_OPCODE_RECV_DIAG_RESULTS	((UINT8) 0x1c)  /* recv diag results */
#define SCSI_OPCODE_SEND_DIAGNOSTIC     ((UINT8) 0x1d)  /* send diagnostic */
#define SCSI_OPCODE_CTRL_MEDIUM_REMOVAL	((UINT8) 0x1e)  /* ctrl med. removal */
#define SCSI_OPCODE_READ_CAPACITY       ((UINT8) 0x25)  /* read capacity */
#define SCSI_OPCODE_READ_EXT		((UINT8) 0x28)  /* read extended */
#define SCSI_OPCODE_WRITE_EXT      	((UINT8) 0x2a)  /* write extended */
#define SCSI_OPCODE_SEEK_EXT       	((UINT8) 0x2b)  /* seek extended */
#define SCSI_OPCODE_WRITE_AND_VERIFY    ((UINT8) 0x2e)  /* write and verify */
#define SCSI_OPCODE_VERIFY              ((UINT8) 0x2f)  /* verify */
#define SCSI_OPCODE_SEARCH_DATA_HIGH	((UINT8) 0x30)  /* search data high */
#define SCSI_OPCODE_SEARCH_DATA_EQUAL	((UINT8) 0x31)  /* search data equal */
#define SCSI_OPCODE_SEARCH_DATA_LOW	((UINT8) 0x32)  /* search data low */
#define SCSI_OPCODE_SET_LIMITS		((UINT8) 0x33)  /* set limits */
#define SCSI_OPCODE_READ_DEFECT_DATA    ((UINT8) 0x37)  /* read defect data */
#define SCSI_OPCODE_COMPARE    		((UINT8) 0x39)  /* compare */
#define SCSI_OPCODE_COPY_AND_VERIFY	((UINT8) 0x3a)  /* copy and verify */
#define SCSI_OPCODE_WRITE_BUFFER        ((UINT8) 0x3b)  /* write buffer */
#define SCSI_OPCODE_READ_BUFFER         ((UINT8) 0x3c)  /* read buffer */

/* SCSI general purpose sense keys are enumerated.
 * NOTE: Some sense keys are optional and may not be supported by all devices.
*/

#define SCSI_SENSE_KEY_NO_SENSE         0x00  /* no sense sense key */
#define SCSI_SENSE_KEY_RECOVERED_ERROR  0x01  /* recovered error sense key */
#define SCSI_SENSE_KEY_NOT_READY        0x02  /* not ready sense key */
#define SCSI_SENSE_KEY_MEDIUM_ERROR     0x03  /* medium error sense key */
#define SCSI_SENSE_KEY_HARDWARE_ERROR   0x04  /* hardware error sense key */
#define SCSI_SENSE_KEY_ILLEGAL_REQUEST  0x05  /* illegal request sense key */
#define SCSI_SENSE_KEY_UNIT_ATTENTION   0x06  /* unit attention sense key */
#define SCSI_SENSE_KEY_DATA_PROTECT     0x07  /* data protect sense key */
#define SCSI_SENSE_KEY_BLANK_CHECK      0x08  /* blank check sense key */
#define SCSI_SENSE_KEY_VENDOR_UNIQUE    0x09  /* vendor unique sense key */
#define SCSI_SENSE_KEY_COPY_ABORTED     0x0a  /* copy aborted sense key */
#define SCSI_SENSE_KEY_ABORTED_COMMAND  0x0b  /* aborted command sense key */
#define SCSI_SENSE_KEY_EQUAL            0x0c  /* key equal sense key */
#define SCSI_SENSE_KEY_VOLUME_OVERFLOW  0x0d  /* volume overflow sense key */
#define SCSI_SENSE_KEY_MISCOMPARE       0x0e  /* miscompare sense key */
#define SCSI_SENSE_KEY_RESERVED         0x0f  /* reserved sense key */

/* SCSI additional sense keys */

#define SCSI_ADD_SENSE_MEDIUM_CHANGED 	0x28  /* medium may have changed */
#define SCSI_ADD_SENSE_DEVICE_RESET     0x29  /* hardware or bus device reset */
#define SCSI_ADD_SENSE_WRITE_PROTECTED  0x27  /* write protected medium */
#define SCSI_ADD_SENSE_NO_MEDIUM 	0x3a  /* medium not present */


/* SCSI MODE header parameters */

#define SCSI_MODE_DEV_SPECIFIC_PARAM  	0x2   /* device specific parameter */
#define SCSI_DEV_SPECIFIC_WP_MASK	0x80  /* Write protect bit */

/* SCSI status byte codes are enumerated.
 * NOTE: Some status codes are optional and may not be supported by all devices.
*/

#define SCSI_STATUS_MASK            ((UINT8) 0x3e) /* mask vend uniq status */
#define SCSI_STATUS_GOOD            ((UINT8) 0x00) /* good status */
#define SCSI_STATUS_CHECK_CONDITION ((UINT8) 0x02) /* check condition status */
#define SCSI_STATUS_CONDITION_MET   ((UINT8) 0x04) /* condition met/good stat */
#define SCSI_STATUS_BUSY            ((UINT8) 0x08) /* busy status */
#define SCSI_STATUS_INTMED_GOOD     ((UINT8) 0x10) /* intermediate/good stat */
#define SCSI_STATUS_INTMED_COND_MET ((UINT8) 0x14) /* interm./cond. met stat */
#define SCSI_STATUS_RESERV_CONFLICT ((UINT8) 0x18) /* reservation conflict */
#define	SCSI_STATUS_CMD_TERMINATED  ((UINT8) 0x22) /* command terminated */
#define SCSI_STATUS_QUEUE_FULL      ((UINT8) 0x28) /* queue full status */

/* SCSI message codes are enumerated.
 * NOTE: Some message codes are optional and may not be supported by all
 *       devices.
*/

#define SCSI_MSG_COMMAND_COMPLETE   ((UINT8) 0x00) /* command complete msg. */
#define SCSI_MSG_EXTENDED_MESSAGE   ((UINT8) 0x01) /* extended message msg. */
#define SCSI_MSG_SAVE_DATA_POINTER  ((UINT8) 0x02) /* save data pointer msg. */
#define SCSI_MSG_RESTORE_POINTERS   ((UINT8) 0x03) /* restore pointers msg. */
#define SCSI_MSG_DISCONNECT         ((UINT8) 0x04) /* disconnect msg. */
#define SCSI_MSG_INITOR_DETECT_ERR  ((UINT8) 0x05) /* initor. detect err msg. */
#define SCSI_MSG_ABORT              ((UINT8) 0x06) /* abort msg. */
#define SCSI_MSG_MESSAGE_REJECT     ((UINT8) 0x07) /* message reject msg. */
#define SCSI_MSG_NO_OP              ((UINT8) 0x08) /* no operation msg. */
#define SCSI_MSG_MSG_PARITY_ERR     ((UINT8) 0x09) /* message parity err msg. */
#define SCSI_MSG_LINK_CMD_COMPLETE  ((UINT8) 0x0a) /* linked cmd. comp. msg. */
#define SCSI_MSG_LINK_CMD_FLAG_COMP ((UINT8) 0x0b) /* link cmd w/flag comp. */
#define SCSI_MSG_BUS_DEVICE_RESET   ((UINT8) 0x0c) /* bus device reset msg. */
#define SCSI_MSG_ABORT_TAG  	    ((UINT8) 0x0d) /* abort tag msg. */
#define	SCSI_MSG_CLEAR_QUEUE	    ((UINT8) 0x0e) /* clear queue msg. */
#define	SCSI_MSG_INITIATE_RECOVERY  ((UINT8) 0x0f) /* initiate recovery msg. */
#define	SCSI_MSG_RELEASE_RECOVERY   ((UINT8) 0x10) /* release recovery msg. */
#define	SCSI_MSG_TERMINATE_PROCESS  ((UINT8) 0x11) /* terminate i/o process */

#define SCSI_MSG_IDENTIFY 	    ((UINT8) 0x80) /* identify msg bit mask */
#define SCSI_MSG_IDENT_DISCONNECT   ((UINT8) 0x40) /* identify disconnect bit */
#define	SCSI_MSG_IDENT_LUN_MASK	    ((UINT8) 0x07) /* identify bit mask for LUN */

/* SCSI two-byte message codes */

#define	SCSI_MSG_SIMPLE_Q_TAG	    ((UINT8) 0x20) /* simple queue tag    */
#define SCSI_MSG_HEAD_OF_Q_TAG	    ((UINT8) 0x21) /* head of queue tag   */
#define	SCSI_MSG_ORDERED_Q_TAG	    ((UINT8) 0x22) /* ordered queue tag   */
#define	SCSI_MSG_IGNORE_WIDE_RESIDUE ((UINT8)0x23) /* ignore wide residue */

#define	SCSI_IS_TWO_BYTE_MSG(m)	    ((0x20 <= (m)) && ((m) < 0x30))

/* SCSI extended message codes */

#define SCSI_EXT_MSG_MODIFY_DATA_PTR ((UINT8) 0x00)  /* modify data pointer */
#define SCSI_EXT_MSG_SYNC_XFER_REQ   ((UINT8) 0x01)  /* sync. data xfer. req. */
#define SCSI_EXT_MSG_EXT_IDENTIFY    ((UINT8) 0x02)  /* extended identify */
#define SCSI_EXT_MSG_WIDE_XFER_REQ   ((UINT8) 0x03)  /* wide data xfer req. */

/* SCSI extended message lengths */

#define	SCSI_EXT_MSG_HDR_LENGTH	    	 2  /* generic part of ext msg */

#define SCSI_MODIFY_DATA_PTR_MSG_LENGTH  5  /* modify data pointer */
#define SCSI_SYNC_XFER_REQ_MSG_LENGTH    3  /* sync. data xfer. req. */
#define SCSI_WIDE_XFER_REQ_MSG_LENGTH    2  /* wide data xfer req. */

#define	SCSI_EXT_MSG_MAX_LENGTH	       256  /* max. possible length */

/* byte offsets within SCSI extended messages */

#define SCSI_EXT_MSG_LENGTH_BYTE    	1   /* # bytes to follow         */
#define SCSI_EXT_MSG_TYPE_BYTE 	    	2   /* defines type of ext. msg. */

#define SCSI_MODIFY_DATA_PTR_UU	    	3   /* argument (MSB) */
#define SCSI_MODIFY_DATA_PTR_UM	    	4
#define SCSI_MODIFY_DATA_PTR_LM	    	5
#define SCSI_MODIFY_DATA_PTR_LL	    	6   /* argument (LSB) */

#define	SCSI_SYNC_XFER_MSG_PERIOD   	3   /* min xfer period (x 4 ns) */
#define SCSI_SYNC_XFER_MSG_OFFSET   	4   /* max req/ack offset       */

#define SCSI_WIDE_XFER_MSG_WIDTH    	3   /* wide data transfer width */

#define	SCSI_EXT_IDENTIFY_SUB_LUN   	3   /* sub-logical unit number */

/* the largest SCSI ID and LUN a target drive can have */

#define SCSI_MIN_BUS_ID			0	/* min. bus ID under SCSI */
#define SCSI_MAX_BUS_ID			7	/* max. bus ID under SCSI */
#define SCSI_MIN_LUN			0	/* min. logical unit number */
#define SCSI_MAX_LUN			7	/* max. logical unit number */

#define	SCSI_MAX_TARGETS    	    	8
#define	SCSI_MAX_LUNS_PER_TARGET    	8
#define SCSI_MAX_PHYS_DEVS		(SCSI_MAX_TARGETS * 	    	\
					 SCSI_MAX_LUNS_PER_TARGET)

/* byte offsets and bitmasks within the data returned by an INQUIRY command */

#define	SCSI_INQUIRY_DEV_TYPE	    0	    /* device type and qualifier   */
#define	SCSI_INQUIRY_DEV_MODIFIER   1	    /* type modifier, removable    */
#define	SCSI_INQUIRY_VERSION	    2	    /* ISO, ECMA and ANSI versions */
#define	SCSI_INQUIRY_FORMAT 	    3	    /* reponse format, etc.        */
#define	SCSI_INQUIRY_ADD_LENGTH	    4	    /* additional length byte      */
#define	SCSI_INQUIRY_FLAGS  	    7	    /* miscellaneous flags (below) */
#define	SCSI_INQUIRY_VENDOR_ID	    8	    /* vendor ID                   */
#define	SCSI_INQUIRY_PRODUCT_ID	    16	    /* product ID                  */
#define	SCSI_INQUIRY_REV_LEVEL      32	    /* product revision level      */

#define	SCSI_INQUIRY_DEV_TYPE_MASK  0x1f    /* mask for device type (byte 0) */

#define	SCSI_INQUIRY_REMOVABLE_MASK 0x80    /* removable media flag (byte 1) */

#define	SCSI_INQUIRY_ANSI_VSN_MASK  0x07    /* ANSI version         (byte 2) */

#define	SCSI_INQUIRY_FORMAT_MASK    0x0f    /* response data format (byte 3) */

#define	SCSI_INQUIRY_CMD_QUEUE_MASK 0x02    /* command queuing flag (byte 7) */
#define SCSI_INQUIRY_WIDE_32_MASK   0x40    /* wide bus 32 flag     (byte 7) */
#define SCSI_INQUIRY_WIDE_16_MASK   0x20    /* wide bus 16 flag     (byte 7) */
#define	SCSI_INQUIRY_SYNC_XFER_MASK 0x10    /* synch. xfer flag     (byte 7) */

#define SCSI_INQUIRY_VENDOR_ID_LENGTH  	    8
#define SCSI_INQUIRY_PRODUCT_ID_LENGTH 	    16
#define SCSI_INQUIRY_REV_LEVEL_LENGTH	    4


/* device type enumeration */

#define SCSI_DEV_DIR_ACCESS	((UINT8) 0x00) /* direct-access dev (disk) */
#define SCSI_DEV_SEQ_ACCESS	((UINT8) 0x01) /* sequent'l-access dev (tape) */
#define SCSI_DEV_PRINTER	((UINT8) 0x02) /* printer dev */
#define SCSI_DEV_PROCESSOR	((UINT8) 0x03) /* processor dev */
#define SCSI_DEV_WORM		((UINT8) 0x04) /* write-once read-mult dev */
#define SCSI_DEV_RO_DIR_ACCESS	((UINT8) 0x05) /* read-only direct-access dev */
#define SCSI_LUN_NOT_PRESENT	((UINT8) 0x7f) /* logical unit not present */

/* miscellaneous SCSI constants, etc. */

#define	SCSI_SYNC_XFER_MAX_OFFSET   	0xff	/* in sync xfer request msg  */
#define SCSI_SYNC_XFER_MIN_PERIOD   	1  	/* shoot for highest speed   */
#define SCSI_SYNC_XFER_ASYNC_OFFSET	0   	/* special value for async.  */
#define SCSI_SYNC_XFER_ASYNC_PERIOD	0   	/* (irrelevant for async.)   */

#define SCSI_WIDE_XFER_SIZE_DEFAULT     1	/* 16 bit default wide width */
#define SCSI_WIDE_XFER_SIZE_NARROW      0 	/* 8 bit narrow width        */

#define SCSI_MAX_MSG_IN_BYTES		(2 + SCSI_EXT_MSG_MAX_LENGTH)
#define SCSI_MAX_MSG_OUT_BYTES		(2 + SCSI_EXT_MSG_MAX_LENGTH)

#define	SCSI_MAX_IDENT_MSG_LENGTH   	(1 + 2)	/* IDENTIFY + QUEUE TAG msg */

#define	SCSI_MAX_TAGS	    	    	256 	/* tags numbered 0 - 255 */

/* definitions relating to SCSI commands */

#define SCSI_MAX_CMD_LENGTH 12  /* maximum length in bytes of a SCSI command */

typedef UINT8 SCSI_COMMAND [SCSI_MAX_CMD_LENGTH];

#define SCSI_GROUP_0_CMD_LENGTH  6
#define SCSI_GROUP_1_CMD_LENGTH 10
#define SCSI_GROUP_5_CMD_LENGTH 12

/* some useful defines and structures for CCS commands */

#define SCSI_FORMAT_DATA_BIT		((UINT8) 0x10)
#define SCSI_COMPLETE_LIST_BIT		((UINT8) 0x08)

#define DEFAULT_INQUIRY_DATA_LENGTH	36
#define REQ_SENSE_ADD_LENGTH_BYTE	7
#define INQUIRY_ADD_LENGTH_BYTE		4
#define MODE_SENSE_ADD_LENGTH_BYTE	0

#define NON_EXT_SENSE_DATA_LENGTH	4

#define SCSI_SENSE_DATA_CLASS		0x70
#define SCSI_SENSE_DATA_CODE		0x0f

#define SCSI_SENSE_KEY_MASK		0x0f

#define SCSI_EXT_SENSE_CLASS		0x70
#define SCSI_EXT_SENSE_CODE		0x00

#define SPACE_CODE_DATABLK      0x00    	/* space data blocks  */
#define SPACE_CODE_FILEMARK     0x01    	/* space file-marks   */


/* scsiLib errno's */

#define S_scsiLib_DEV_NOT_READY		(M_scsiLib | 1)
#define S_scsiLib_WRITE_PROTECTED	(M_scsiLib | 2)
#define S_scsiLib_MEDIUM_ERROR		(M_scsiLib | 3)
#define S_scsiLib_HARDWARE_ERROR	(M_scsiLib | 4)
#define S_scsiLib_ILLEGAL_REQUEST	(M_scsiLib | 5)
#define S_scsiLib_BLANK_CHECK		(M_scsiLib | 6)
#define S_scsiLib_ABORTED_COMMAND	(M_scsiLib | 7)
#define S_scsiLib_VOLUME_OVERFLOW	(M_scsiLib | 8)
#define S_scsiLib_UNIT_ATTENTION	(M_scsiLib | 9)
#define S_scsiLib_SELECT_TIMEOUT	(M_scsiLib | 10)
#define S_scsiLib_LUN_NOT_PRESENT	(M_scsiLib | 11)
#define S_scsiLib_ILLEGAL_BUS_ID	(M_scsiLib | 12)
#define S_scsiLib_NO_CONTROLLER		(M_scsiLib | 13)
#define S_scsiLib_REQ_SENSE_ERROR	(M_scsiLib | 14)
#define S_scsiLib_DEV_UNSUPPORTED	(M_scsiLib | 15)
#define S_scsiLib_ILLEGAL_PARAMETER	(M_scsiLib | 16)
#define S_scsiLib_INVALID_PHASE		(M_scsiLib | 17)
#define S_scsiLib_ABORTED   		(M_scsiLib | 18)
#define S_scsiLib_ILLEGAL_OPERATION	(M_scsiLib | 19)
#define S_scsiLib_DEVICE_EXIST	    	(M_scsiLib | 20)
#define S_scsiLib_DISCONNECTED	    	(M_scsiLib | 21)
#define S_scsiLib_BUS_RESET	    	(M_scsiLib | 22)
#define	S_scsiLib_INVALID_TAG_TYPE  	(M_scsiLib | 23)
#define	S_scsiLib_SOFTWARE_ERROR    	(M_scsiLib | 24)
#define	S_scsiLib_NO_MORE_THREADS   	(M_scsiLib | 25)
#define	S_scsiLib_UNKNOWN_SENSE_DATA	(M_scsiLib | 26)
#define	S_scsiLib_INVALID_BLOCK_SIZE	(M_scsiLib | 27)

					/* default select timeout (usec) */
#define SCSI_DEF_SELECT_TIMEOUT		((UINT) 250000)

					/* default auto-config timeout (usec) */
#define SCSI_DEF_CONFIG_TIMEOUT		((UINT) 1000)

#define	SCSI_TIMEOUT_5SEC		((UINT) 5000000)
#define	SCSI_TIMEOUT_1SEC		((UINT) 1000000)
#define	SCSI_TIMEOUT_FULL		((UINT) 0xffffffff)

#define	SCSI_DEF_MIN_TIMEOUT	    	SCSI_TIMEOUT_1SEC
#define	SCSI_DEF_MAX_TIMEOUT	    	SCSI_TIMEOUT_FULL
					 
				/* default bus ID for a vxWorks SCSI port */

#define SCSI_DEF_CTRL_BUS_ID		7

/* parameters for SCSI manager task (also see specific controller drivers) */

#define SCSI_DEF_TASK_NAME    	    	"tScsiTask"
#define SCSI_DEF_TASK_OPTIONS  	    	VX_UNBREAKABLE
#define SCSI_DEF_TASK_STACK_SIZE	4000
#define SCSI_DEF_TASK_PRIORITY	    	5

/* parameters for SCSI manager queues, thread allocation, etc. */

#define	SCSI_DEF_MAX_THREADS	    	64  /* max total # threads allowed */
#define	SCSI_DEF_ALLOC_THREADS	    	16  /* # threads allocated at once */

#define	SCSI_DEF_EVENT_Q_SIZE	    	8   /* 2 should normally be enough ! */
#define	SCSI_DEF_REQUEST_Q_SIZE	    	SCSI_DEF_MAX_THREADS
#define	SCSI_DEF_REPLY_Q_SIZE	    	SCSI_DEF_MAX_THREADS
#define	SCSI_DEF_TIMEOUT_Q_SIZE	    	SCSI_DEF_MAX_THREADS

/*
 *  Cache flush threshold - the entire data cache will be cleared (read)
 *  or flushed (write) if the data buffer used for a SCSI command is this
 *  size or bigger.  Otherwise, just the buffer will be cleared/flushed.
 */
#define	SCSI_DEF_CACHE_FLUSH_THRESHOLD	2048


/* SCSI bus information transfer phases (MCI codes) */

#define	SCSI_NUM_XFER_PHASE 	    	8

#define SCSI_DATA_OUT_PHASE		0x0
#define SCSI_DATA_IN_PHASE		0x1
#define SCSI_COMMAND_PHASE		0x2
#define SCSI_STATUS_PHASE		0x3
#define SCSI_MSG_OUT_PHASE		0x6
#define SCSI_MSG_IN_PHASE		0x7


/*
 *  Structure to pass to ioctl call to execute an SCSI command
 */
/* SCSI command priority */

typedef UINT SCSI_PRIORITY;

#define	SCSI_THREAD_TASK_PRIORITY   ((UINT)-1) /* use current task priority */
					 
#define	SCSI_THREAD_MIN_PRIORITY    255	    /* same as min task priority */
#define	SCSI_THREAD_MAX_PRIORITY    0	    /* same as max task priority */

#define	SCSI_IS_HIGHER_PRIORITY(x,y)	(x < y)	    /* numerically lower  */
#define	SCSI_IS_LOWER_PRIORITY(x,y) 	(x > y)	    /* numerically higher */

/* SCSI tag type enumeration */

typedef enum	    	    	/* SCSI_TAG_TYPE */
    {
    SCSI_TAG_DEFAULT  = -1,	/* use default for device                */
    SCSI_TAG_UNTAGGED = 0,  	/* untagged command                      */
    SCSI_TAG_SENSE_RECOVERY,	/* untagged, even if tagged cmds present */
    SCSI_TAG_SIMPLE,	    	/* tagged with simple  queue tag         */
    SCSI_TAG_ORDERED,	    	/* tagged with ordered queue tag         */
    SCSI_TAG_HEAD_OF_Q	    	/* tagged with head-of-queue tag         */
    } SCSI_TAG_TYPE;

/* default tag type (all targets use this by default) */

#define	SCSI_DEF_TAG_TYPE   	SCSI_TAG_SIMPLE

/* SCSI transation structure */

typedef struct  /* SCSI_TRANSACTION - information about a SCSI transaction */
    {
    UINT8 * cmdAddress;		/* address of SCSI command bytes             */
    int     cmdLength;		/* length of command in bytes                */
    UINT8 * dataAddress;	/* address of data buffer                    */
    int     dataDirection;	/* direction of data transfer: O_{RD,WR}ONLY */
    int     dataLength;		/* length of data buffer in bytes (0 = none) */
    int     addLengthByte;      /* no longer used                            */
    UINT8   statusByte;		/* status byte returned from target          */
    UINT    cmdTimeout;		/* number of usec for this command timeout   */
    SCSI_TAG_TYPE tagType;  	/* tag type to use for this command          */
    SCSI_PRIORITY priority; 	/* priority of this transaction              */
    } SCSI_TRANSACTION;


/*
 *  Data structure and constants for "scsiTargetOptionsSet()"
 */
#define	SCSI_SET_OPT_ALL_TARGETS    ((UINT) -1)

#define SCSI_SET_OPT_BITMASK	    0x003f  /* mask for valid option bits   */
#define	SCSI_SET_OPT_TIMEOUT	    0x0001  /* bit values to select options */
#define	SCSI_SET_OPT_MESSAGES	    0x0002
#define	SCSI_SET_OPT_DISCONNECT	    0x0004
#define	SCSI_SET_OPT_XFER_PARAMS    0x0008
#define	SCSI_SET_OPT_TAG_PARAMS	    0x0010
#define	SCSI_SET_OPT_WIDE_PARAMS    0x0020

typedef struct	    	    	/* SCSI_OPTIONS - programmable options */
    {
    UINT          selTimeOut;	/* device select time-out (us)             */
    BOOL          messages; 	/* FALSE => do not use SCSI messages       */
    BOOL          disconnect;	/* FALSE => do not use disconnect          */
    UINT8         maxOffset;	/* max sync xfer offset (0 => async.)      */
    UINT8         minPeriod;	/* min sync xfer period (x 4 ns)           */
    SCSI_TAG_TYPE tagType;  	/* default tag type (see SCSI_TRANSACTION) */
    UINT          maxTags;  	/* max cmd tags available (0 => untagged)  */
    UINT8    	  xferWidth;	/* wide data transfer width in SCSI units  */
    } SCSI_OPTIONS;

/*
 *  Structure for results of READ CAPACITY command
 */
typedef struct  /* RD_CAP_DATA - results from READ CAPACITY */
    {
    int lastLogBlkAdrs;		/* address of last logical block on device */
    int blkLength;		/* block length of last logical block */
    } RD_CAP_DATA;

typedef struct  /* RD_BLOCK_LIMIT_DATA - results from READ BLOCK LIMITS */
    {
    int maxBlockLength;         /* maximum block length limit */
    UINT16 minBlockLength;      /* minimum block length limit */
    } RD_BLOCK_LIMIT_DATA;

/* HIDDEN */

/*
 *  SCSI Target and related structures
 */
typedef enum				/* Synchronous transfer state  */
    {
    SYNC_XFER_NOT_NEGOTIATED = 0,	/* no negotiation ever started */
    SYNC_XFER_REQUEST_PENDING,		/* attempting to send request  */
    SYNC_XFER_REQUEST_SENT,		/* waiting for target's reply  */
    SYNC_XFER_REPLY_PENDING,		/* attempting to send reply    */
    SYNC_XFER_NEGOTIATION_COMPLETE	/* all done (at least once)    */
    } SCSI_SYNC_XFER_STATE;

typedef enum				/* Synchronous transfer event      */
    {
    SYNC_XFER_RESET = 1,		/* reset synchronous transfer fsm  */
    SYNC_XFER_NEW_THREAD,		/* new thread is being activated   */
    SYNC_XFER_MSG_IN,			/* target has sent message in      */
    SYNC_XFER_MSG_OUT,			/* target has rcvd message out     */
    SYNC_XFER_MSG_REJECTED		/* target has rejected message out */
    } SCSI_SYNC_XFER_EVENT;

typedef enum				/* Wide data transfer state 	   */
    {
    WIDE_XFER_NOT_NEGOTIATED = 0,	/* no negotiation ever stared 	   */
    WIDE_XFER_REQUEST_PENDING,		/* attempting to send wide request */
    WIDE_XFER_REQUEST_SENT,		/* waiting for target's reply      */
    WIDE_XFER_REPLY_PENDING,		/* attempting to send reply	   */
    WIDE_XFER_NEGOTIATION_COMPLETE	/* negotiations done (min. once)   */
    } SCSI_WIDE_XFER_STATE;

typedef enum				/* Wide data transfer event        */
    {
    WIDE_XFER_RESET = 1,		/* reset wide transfer		   */
    WIDE_XFER_NEW_THREAD,		/* new thread is being activated   */
    WIDE_XFER_MSG_IN,			/* target has sent message in      */
    WIDE_XFER_MSG_OUT,			/* target has received message out */
    WIDE_XFER_MSG_REJECTED		/* target has rejected message out */
    } SCSI_WIDE_XFER_EVENT;

typedef struct			/* SCSI_TARGET - SCSI target information   */
    {
    int           scsiDevBusId;	/* device's address on SCSI bus            */
    UINT          selTimeOut;	/* device select time-out (us)             */
    BOOL          messages; 	/* FALSE => do not use SCSI messages       */
    BOOL          disconnect;	/* FALSE => do not use disconnect          */
    SCSI_TAG_TYPE tagType;  	/* default tag type (cmds may override)    */
    UINT          maxTags;	/* max cmd tags available (0 => untagged)  */
    UINT8         maxOffset;	/* maximum sync xfer offset (0 => async.)  */
    UINT8         minPeriod;	/* minimum sync xfer period (x 4 ns)       */
    UINT8         xferOffset;	/* current sync xfer offset (0 => async.)  */
    UINT8         xferPeriod;	/* current sync xfer period (x 4 ns)       */
    UINT8 	  xferWidth;	/* width in bits of the wide transfer      */
    UINT8         syncSupport;  /* supports synchronous transfers          */
    UINT8         wideSupport;  /* supports synchronous transfers          */ 
    SCSI_SYNC_XFER_STATE syncXferState;	/* state of sync xfer negotiation  */
    SCSI_WIDE_XFER_STATE wideXferState; /* state of wide xfer negotiation  */
    } SCSI_TARGET;


/*
 *  Structures and definitions related to SCSI tags
 */
typedef UINT SCSI_TAG;	    	/* has the values [0..255] or SCSI_TAG_NONE */

#define	SCSI_TAG_NONE	((SCSI_TAG) NONE)

typedef struct scsiTagInfo
    {
    UINT8 inUse; 	    	/* actually a BOOL (but want to save space) */
    } SCSI_TAG_INFO;


/*
 *  SCSI Physical Device and related structures
 */
typedef enum scsiDevEvent   	/* SCSI phys dev event type enumeration     */
    {
    SCSI_DEV_ACTIVATED = 1, 	/* thread using this dev has been activated */
    SCSI_DEV_DISCONNECTED,  	/* thread using this dev has disconnected   */
    SCSI_DEV_RECONNECTED,   	/* thread using this dev has reconnected    */
    SCSI_DEV_COMPLETED	    	/* thread using this dev has completed      */
    } SCSI_DEV_EVENT_TYPE;

typedef enum	    	    	/* SCSI nexus type enumeration */
    {
    SCSI_NEXUS_NONE = 0,    	/* no         nexus */
    SCSI_NEXUS_IT,	    	/* an I-T     nexus */
    SCSI_NEXUS_ITL,	    	/* an I-T-L   nexus */
    SCSI_NEXUS_ITLQ	    	/* an I-T-L-Q nexus */
    } SCSI_NEXUS_TYPE;

typedef struct 			/* SCSI_PHYS_DEV - SCSI physical device info */
    {
    SEM_ID       mutexSem;	/* semaphore for access to blk dev list      */
    struct scsiCtrl *pScsiCtrl;	/* ptr to dev's SCSI controller info         */
    SCSI_TARGET *pScsiTarget;	/* ptr to data for corresponding SCSI target */
    int          scsiDevLUN;	/* device's logical unit number              */
    UINT8        scsiDevType;	/* SCSI device type                          */
    BOOL         resetFlag;	/* set TRUE when dev reset sensed            */

    	    	    	    	/* product info from INQUIRY command         */
    char devVendorID  [SCSI_INQUIRY_VENDOR_ID_LENGTH  + 1];
    char devProductID [SCSI_INQUIRY_PRODUCT_ID_LENGTH + 1];
    char devRevLevel  [SCSI_INQUIRY_REV_LEVEL_LENGTH  + 1];

    BOOL  removable;		/* whether medium is removable               */
    SCSI_TAG_TYPE tagType;  	/* default tag type: cmds may over-ride      */
    UINT  nTags;	    	/* total number of tags for device           */
    UINT8 lastSenseKey;		/* last sense key returned by dev            */
    UINT8 lastAddSenseCode;	/* last additional sense code returned       */
    int   controlByte;		/* vendor unique control byte for commands   */
    int   numBlocks;		/* number of blocks on the physical device   */
    int   blockSize;		/* size of an SCSI disk sector               */
    int   maxVarBlockLimit;     /* maximum size of a variable block: seq dev */
    BOOL  extendedSense;	/* whether device returns extended sense     */
    UINT8 *pReqSenseData;	/* ptr to last REQ SENSE data returned       */
    int   reqSenseDataLength;	/* size of REQ SENSE data array              */
    LIST  blkDevList;		/* list of block devs created on device      */
    struct scsiSeqDev *pScsiSeqDev;/* ptr to SCSI seq dev; one per phys dev  */

    SCSI_NEXUS_TYPE nexus;  	/* type of current nexuses in progress       */
    UINT nTaggedNexus;	    	/* number of tagged nexuses in progress      */
    BOOL connected; 	    	/* currently connected to SCSI bus ?         */
    BOOL pendingCA;	        /* contingent allegiance condition pending ? */
    SCSI_NEXUS_TYPE savedNexus;	/* type of nexus prior to CA recovery        */
    LIST waitingThreads;    	/* list of threads waiting for device        */
    LIST activeThreads;	    	/* list of threads active on device          */
    SCSI_TAG curTag; 	    	/* currently-connected tag (or NONE)         */
    SCSI_TAG nextTag;	    	/* next free tag number (if any)             */
    UINT nFreeTags; 	    	/* number of free tags (if tags supported)   */
    SCSI_TAG_INFO   untagged; 	/* "tag" info for a single untagged thread   */
    SCSI_TAG_INFO * pTagInfo;	/* ptr to tag info table for tagged threads  */
    } SCSI_PHYS_DEV;


typedef SCSI_PHYS_DEV *SCSI_PHYS_DEV_ID;

/* SCSI Sequential device */

typedef struct scsiSeqDev	/* SCSI_SEQ_DEV - 
				   SCSI logical sequential device info */
    {
    SEQ_DEV seqDev;		/*  generic logical sequential device info */
    SCSI_PHYS_DEV *pScsiPhysDev;/* ptr to SCSI physical device info */
    } SCSI_SEQ_DEV;


/*
 *  SCSI Block Device and related structures
 */
typedef struct scsiBlkDev	/* SCSI_BLK_DEV -
				   SCSI logical block device info */
    {
    BLK_DEV blkDev;		/* generic logical block device info */
    SCSI_PHYS_DEV *pScsiPhysDev;/* ptr to SCSI physical device info */
    int numBlocks;		/* number of blocks on the logical device */
    int blockOffset;		/* address of first block on logical device */
    } SCSI_BLK_DEV;


typedef SCSI_BLK_DEV *SCSI_BLK_DEV_ID;

typedef struct scsiBlkDevNode
    {
    NODE blkDevNode;
    SCSI_BLK_DEV scsiBlkDev;
    } SCSI_BLK_DEV_NODE;


/*
 *  SCSI Controller and related structures
 */
typedef enum				/* Message In state                 */
    {
    SCSI_MSG_IN_NONE = 0,		/* no message in being assembled    */
    SCSI_MSG_IN_SECOND_BYTE,	    	/* next byte in is second of two    */
    SCSI_MSG_IN_EXT_MSG_LEN,		/* next byte in is ext msg length   */
    SCSI_MSG_IN_EXT_MSG_DATA		/* next byte(s) in are ext msg data */
    } SCSI_MSG_IN_STATE;


typedef enum				/* Message Out state        */
    {
    SCSI_MSG_OUT_NONE = 0,		/* no message out available */
    SCSI_MSG_OUT_PENDING,		/* message out not yet sent */
    SCSI_MSG_OUT_SENT			/* message out already sent */
    } SCSI_MSG_OUT_STATE;


typedef enum	    	    	    	/* Identification In state          */
    {
    SCSI_IDENT_IN_NONE = 0, 	    	/* no identification in progress    */
    SCSI_IDENT_IN_IDENT,    	    	/* next byte in is IDENTIFY msg     */
    SCSI_IDENT_IN_QUEUE_TAG		/* next bytes in are QUEUE TAG msg  */
    } SCSI_IDENT_IN_STATE;


typedef enum scsiCacheAction		/* Cache management actions         */
    {
    SCSI_CACHE_PRE_COMMAND = 0,		/* flush/invalidate before command  */
    SCSI_CACHE_POST_COMMAND		/* flush/invalidate after  command  */
    } SCSI_CACHE_ACTION;


#define	SCSI_BUS_NEGATE_ACK 	0x0001	/* bitmasks for scsiBusControl() */
#define	SCSI_BUS_ASSERT_ATN 	0x0002
#define	SCSI_BUS_DISCONNECT 	0x0004
#define	SCSI_BUS_RESET	    	0x0008

typedef struct scsiCtrl		/* SCSI_CTRL - generic SCSI controller info  */
    {
    SEM_ID  mutexSem;		/* semaphore for exclusive access            */
    SEM_ID  actionSem;    	/* sync with controller events or clients    */
    RING_ID eventQ;	    	/* ring buffer for controller events         */
    RING_ID timeoutQ;	    	/* ring buffer for request timeouts          */
    RING_ID requestQ;	    	/* ring buffer for client requests           */
    RING_ID replyQ;		/* ring buffer for client replies            */
    int     scsiMgrId;    	/* SCSI manager task ID                      */
    FUNCPTR scsiTransact;	/* function for managing a SCSI transaction  */
    VOIDFUNCPTR scsiEventProc;  /* function for processing controller events */
    FUNCPTR scsiThreadInit; 	/* function for initializing a new thread    */
    FUNCPTR scsiThreadActivate; /* function for (re)activating a thread      */
    FUNCPTR scsiThreadAbort;	/* function for aborting a thread            */
    FUNCPTR scsiDevSelect;	/* function for selecting a SCSI device      */
    FUNCPTR scsiBusControl;	/* function for misc SCSI bus control        */
    FUNCPTR scsiInfoXfer;	/* function for SCSI input/output transfers  */
    FUNCPTR scsiXferParamsQuery;/* function for getting sync xfer params     */
    FUNCPTR scsiXferParamsSet;  /* function for setting sync xfer params     */
    FUNCPTR scsiWideXferParamsSet;/* function for setting wide xfer params   */
    FUNCPTR scsiWideXferParamsQuery;/* function for getting Wide xfer params */
    UINT    maxBytesPerXfer;	/* upper bound of ctrl. transfer counter     */
    int     eventSize;	    	/* sizeof event msg (0 = use default)        */
    int     threadSize;	    	/* sizeof thread    (0 = use default)        */
    int     scsiCtrlBusId;	/* SCSI bus ID of this SCSI controller       */
    BOOL    disconnect;         /* globally enable / disable disconnect      */
    BOOL    syncXfer;		/* globally enable / disable sync xfer.      */
    BOOL    wideXfer;		/* globally enable / disable wide xfer.      */

    BOOL    active; 	    	/* there is an active SCSI thread            */
    int	    peerBusId;	    	/* ID of connected SCSI peer device          */
    UINT    nThreads;	    	/* total number of threads in existence      */
    LIST    freeThreads;   	/* list (set) of free threads                */
    UINT    nextDev;	    	/* phys dev index for round-robin scheduling */

    struct scsiThread * pIdentThread;	/* reserved identification thread    */
    struct scsiThread * pThread;    	/* currently active thread           */

    UINT               msgInLength;	/* message in bytes read so far      */
    UINT               msgOutLength;	/* pending message out length        */
    SCSI_MSG_IN_STATE  msgInState;  	/* status of incoming message        */
    SCSI_MSG_OUT_STATE msgOutState;	/* status of outgoing message        */

    UINT8 identBuf  [SCSI_MAX_IDENT_MSG_LENGTH];/* incoming ident msg buffer */
    UINT8 msgInBuf  [SCSI_MAX_MSG_IN_BYTES]; 	/* incoming message buffer   */
    UINT8 msgOutBuf [SCSI_MAX_MSG_OUT_BYTES];	/* outgoing message buffer   */

    SCSI_TARGET    targetArr  [SCSI_MAX_TARGETS];   /* attached SCSI targets */
    SCSI_PHYS_DEV *physDevArr [SCSI_MAX_PHYS_DEVS]; /* attached phys. devs.  */
    BOOL           cacheSnooping;	/* is hardware snooping enabled      */
    FUNCPTR scsiSpecialHandler;  /* negotiate transfer paramaters            */
    } SCSI_CTRL;


/*
 *  SCSI thread and related structures
 */
typedef enum	    	    	/* event types for "scsiMgrThreadEvent()"  */
    {
    SCSI_THREAD_EVENT_ACTIVATED = 1,	/* thread has been activated       */
    SCSI_THREAD_EVENT_DISCONNECTED, 	/* thread has disconnected         */
    SCSI_THREAD_EVENT_RECONNECTED,  	/* thread has reconnected          */
    SCSI_THREAD_EVENT_COMPLETED,    	/* thread has completed            */
    SCSI_THREAD_EVENT_DEFERRED	    	/* thread has been deferred        */
    } SCSI_THREAD_EVENT_TYPE;

typedef enum	    	    	/* SCSI thread state enumeration             */
    {
    SCSI_THREAD_INACTIVE = 0,   /* thread is not doing anything              */
    SCSI_THREAD_WAITING,    	/* thread is waiting for access to phys dev  */
    SCSI_THREAD_DISCONNECTED,	/* thread is disconnected from SCSI bus      */
    SCSI_THREAD_CONNECTING,  	/* thread is selecting target device         */
    SCSI_THREAD_IDENT_IN,   	/* thread is receiving identification msg(s) */
    SCSI_THREAD_IDENT_OUT,   	/* thread is sending identification msg(s)   */
    SCSI_THREAD_ESTABLISHED,	/* thread has established a SCSI nexus       */
    SCSI_THREAD_WAIT_DISCONNECT,/* thread is waiting for disconnection       */
    SCSI_THREAD_WAIT_COMPLETE,  /* thread is waiting for final disconnection */
    SCSI_THREAD_ABORTING,   	/* thread is aborting a normal connection    */
    SCSI_THREAD_IDENT_ABORTING,	/* thread is aborting identification msg in  */
    SCSI_THREAD_WAIT_ABORT    	/* thread has been aborted                   */
    } SCSI_THREAD_STATE;

typedef enum			/* status of incoming identification         */
    {
    SCSI_IDENT_INCOMPLETE,  	/* identification message not yet complete   */
    SCSI_IDENT_COMPLETE,    	/* identification successfully completed     */
    SCSI_IDENT_FAILED	    	/* identification incorrect: to be rejected  */
    } SCSI_IDENT_STATUS;

typedef enum	    	    	/* SCSI role type enumeration        */
    {
    SCSI_ROLE_INITIATOR = 0,	/* initiating a SCSI transaction     */
    SCSI_ROLE_TARGET,	    	/* target for a SCSI transaction     */
    SCSI_ROLE_IDENT_INIT,   	/* identifying a reselecting target  */
    SCSI_ROLE_IDENT_TARG    	/* identifying a selecting initiator */
    } SCSI_ROLE;

typedef struct scsiThread	/* SCSI_THREAD */
    {
    NODE           lstNode; 	/* node to support lists of threads         */
    SCSI_CTRL     *pScsiCtrl;	/* SCSI controller thread is using          */
    SCSI_TARGET   *pScsiTarget;	/* SCSI target device thread is using       */
    SCSI_PHYS_DEV *pScsiPhysDev;/* SCSI physical device thread is using     */
    MSG_Q_ID       replyQ;	/* reply from thread manager                */
    WDOG_ID   	   wdog;    	/* request timeout watchdog                 */
    SCSI_THREAD_STATE state;	/* thread's current state                   */
    SCSI_ROLE      role;    	/* SCSI role (initiator/target/ident)       */
    STATUS         status;  	/* completion status for thread request     */
    int            errNum;      /* error code for thread request            */
    UINT           tagNumber;	/* tag number used by this thread           */
    SCSI_TAG_TYPE  tagType; 	/* tag type used by this thread             */
    UINT           timeout; 	/* timeout period (ticks)                   */
    SCSI_PRIORITY  priority;	/* thread priority: 0 highest, 255 lowest   */
    int            dataDirection;   /* data direction: O_{RDONLY,WRONLY}    */
				/* identification msg to be sent (if any)   */
    UINT8  identMsg[SCSI_MAX_IDENT_MSG_LENGTH];
    UINT   identMsgLength;    	/* number of identification bytes to send   */
    UINT8 *cmdAddress;	    	/* SCSI command pointer                     */
    UINT   cmdLength;  	    	/* number of command bytes to transfer      */
    UINT8 *statusAddress;    	/* buffer for SCSI status byte              */
    UINT   statusLength;    	/* number of status bytes to transfer       */
    UINT8 *dataAddress;	    	/* SCSI data pointer         (init'l value) */
    UINT   dataLength;   	/* remaining data byte count (init'l value) */
    UINT8 *activeDataAddress;	/* SCSI data pointer         (active value) */
    UINT   activeDataLength;   	/* remaining data byte count (active value) */
    UINT8 *savedDataAddress;   	/* SCSI data pointer         (saved value)  */
    UINT   savedDataLength;	/* remaining data byte count (saved value)  */
    UINT   nBytesIdent;	    	/* number of ident bytes sent/received      */
    } SCSI_THREAD;


/*
 *  Structures used by clients to communicate with the SCSI manager task
 */
typedef enum				/* requests to SCSI manager 	    */
    {
    SCSI_REQUEST_ACTIVATE = 0,		/* activate initiator thread        */
    SCSI_REQUEST_WAIT_ACTIVATION,	/* wait activation of target thread */
    SCSI_REQUEST_TRANSFER,  	    	/* transfer data on target thread   */
    SCSI_REQUEST_DEACTIVATE,		/* deactivate target thread         */
    SCSI_REQUEST_REACTIVATE,		/* reactivate target thread         */
    SCSI_REQUEST_COMPLETE  	    	/* complete target thread           */
    } SCSI_REQUEST_TYPE;

typedef struct scsiRequest  	    	/* request message to SCSI manager  */
    {
    SCSI_REQUEST_TYPE type; 	    	/* type of request: as above        */
    SCSI_THREAD *     thread;	    	/* thread request corresponds to    */
    } SCSI_REQUEST;


typedef enum	    	    	    	/* replies from SCSI manager        */
    {
    SCSI_REPLY_COMPLETE = 0	    	/* thread has completed (or failed) */
    } SCSI_REPLY_TYPE;

typedef struct scsiReply    	    	/* reply message from SCSI manager  */
    {
    SCSI_REPLY_TYPE type;   	    	/* type of reply: as above          */
    SCSI_THREAD *   thread; 	    	/* thread reply corresponds to      */
    STATUS          status; 	    	/* status of last request           */
    int             errNum; 	    	/* errno, if status == ERROR        */
    } SCSI_REPLY;


/*
 *  SCSI manager timeout message structure
 */
typedef struct scsiTimeout
    {
    SCSI_THREAD * thread;
    } SCSI_TIMEOUT;


/*
 *  SCSI Controller Event message structure
 */
typedef enum	    	    	/* SCSI controller event enumeration         */
    {
    SCSI_EVENT_CONNECTED = 1,	/* (re)select cmd has successfully completed */
    SCSI_EVENT_DISCONNECTED,	/* target device has disconnected            */
    SCSI_EVENT_RESELECTED, 	/* controller has been reselected            */
    SCSI_EVENT_SELECTED, 	/* controller has been selected              */
    SCSI_EVENT_XFER_REQUEST,	/* target device has requested info xfer     */
    SCSI_EVENT_TIMEOUT,    	/* (re)selection or info xfer timed out      */
    SCSI_EVENT_BUS_RESET,   	/* the SCSI bus has been reset               */
    SCSI_EVENT_PARITY_ERR   	/* parity error detected                     */
    } SCSI_EVENT_TYPE;

typedef struct scsiEvent    	/* SCSI_EVENT */
    {
    /*
     *	Note: some fields may not be used, depending on the event type.
     *	If this structure gets much bigger, it should probably be made into
     *	a tagged union.
     */
    SCSI_EVENT_TYPE type;   	/* type of event: as above                */
    int   busId;  	    	/* bus ID of SCSI peer (con, resel, sel)  */
    int   nBytesIdent;	    	/* # bytes ident in/out (con, resel, sel) */
    UINT8 identMsg [SCSI_MAX_IDENT_MSG_LENGTH];	/* ident msg (resel, sel) */
    int   phase;    	    	/* requested SCSI bus phase  (xfer req)   */
    } SCSI_EVENT;
    
/* END_HIDDEN */


/* external variable declarations */

IMPORT BOOL scsiDebug;
IMPORT BOOL scsiIntsDebug;
IMPORT SCSI_CTRL *pSysScsiCtrl;

/* macros */

#define SCSI_MSG							\
	logMsg  						

#define SCSI_ERROR_MSG	    	    	    	    	    	    	\
    if (scsiErrors || scsiDebug)    	    	    	    	    	\
    	SCSI_MSG
    
#define SCSI_DEBUG_MSG							\
    if (scsiDebug)							\
	SCSI_MSG  						

#define SCSI_INT_DEBUG_MSG						\
    if (scsiIntsDebug)							\
	logMsg  						

/*******************************************************************************
*
* SCSI_SWAB - swap bytes on little-endian machines
*
* All fields which are defined as integers (short or long) by the SCSI spec
* and which are passed during the data phase should be pre- or post-processed
* by this macro. The macro does nothing on big-endian machines, but swaps
* bytes for little-endian machines. All SCSI quantities are big-endian.
*
* NOMANUAL
*/

#if (_BYTE_ORDER == _BIG_ENDIAN)
#define SCSI_SWAB(pBuffer, bufLength)
#else
#define SCSI_SWAB(pBuffer, bufLength)					\
    do									\
	{								\
	char temp;							\
	char *pHead = (char *) pBuffer;					\
	char *pTail = (char *) pBuffer + bufLength - 1;			\
									\
	while (pHead < pTail)						\
	    {								\
	    temp = *pTail;						\
	    *pTail-- = *pHead;						\
	    *pHead++ = temp;						\
	    }								\
	} while (FALSE)
#endif

/* global variables */

extern SCSI_FUNC_TBL * pScsiIfTbl;      /* scsiLib interface function table */

extern BOOL scsiErrors;                 /* enable SCSI error messages */
extern int scsiMgrActionSemOptions;
extern int scsiThreadReplyQOptions;
extern int scsiMgrEventQSize;
extern int scsiMgrTimeoutQSize;
extern int scsiMgrRequestQSize;
extern int scsiMgrReplyQSize;


/* function declarations */

#if defined(__STDC__) || defined(__cplusplus)

extern void   scsi2IfInit ();

/* SCSI block device driver functions */

extern BLK_DEV *scsiBlkDevCreate (SCSI_PHYS_DEV *pScsiPhysDev, int numBlocks,
				  int blockOffset);
extern void 	scsiBlkDevInit   (SCSI_BLK_DEV *pScsiBlkDev, int blksPerTrack,
				  int nHeads);
extern void 	scsiBlkDevShow   (SCSI_PHYS_DEV *pScsiPhysDev);
extern STATUS 	scsiRdSecs       (SCSI_BLK_DEV *pScsiBlkDev, int sector,
				  int numSecs, char *buffer);
extern STATUS 	scsiWrtSecs      (SCSI_BLK_DEV *pScsiBlkDev, int sector,
				  int numSecs, char *buffer);

/* general SCSI functions */

extern SCSI_PHYS_DEV *scsiPhysDevCreate (SCSI_CTRL *pScsiCtrl, int devBusId,
					 int devLUN, int reqSenseLength,
					 int devType, BOOL removable,
					 int numBlocks, int blockSize);
extern SCSI_PHYS_DEV *scsiPhysDevIdGet (SCSI_CTRL *pScsiCtrl, int devBusId,
				        int devLUN);
extern STATUS 	scsiPhysDevDelete (SCSI_PHYS_DEV *pScsiPhysDev);
extern STATUS 	scsiAutoConfig (SCSI_CTRL *pScsiCtrl);
extern STATUS 	scsiBusReset (SCSI_CTRL *pScsiCtrl);
extern STATUS 	scsiCmdBuild (SCSI_COMMAND scsiCmd, int *pCmdLength,
			      UINT8 opCode, int LUN, BOOL relAdrs,
			      int logBlockAdrs, int xferLength,
			      UINT8 controlByte);
extern STATUS 	scsiCtrlInit (SCSI_CTRL *pScsiCtrl);

extern SCSI_THREAD * scsiCtrlIdentThreadCreate (SCSI_CTRL *pScsiCtrl);
extern STATUS        scsiCtrlThreadInit        (SCSI_CTRL   * pScsiCtrl,
						SCSI_THREAD * pThread);
extern STATUS        scsiCtrlThreadActivate    (SCSI_CTRL   * pScsiCtrl,
						SCSI_THREAD * pThread);
extern void          scsiCtrlEvent             (SCSI_CTRL   * pScsiCtrl,
						SCSI_EVENT  * pEvent);
extern BOOL          scsiCtrlThreadAbort       (SCSI_CTRL   * pScsiCtrl,
						SCSI_THREAD * pThread);

extern STATUS 	scsiIoctl (SCSI_PHYS_DEV *pScsiPhysDev, int function, int arg);
extern STATUS 	scsiShow (SCSI_CTRL *pScsiCtrl);
extern STATUS 	scsiTransact (SCSI_PHYS_DEV *pScsiPhysDev,
			      SCSI_TRANSACTION *pScsiXaction);
extern STATUS   scsiTargetOptionsSet (SCSI_CTRL *pScsiCtrl, int devBusId,
				      SCSI_OPTIONS *pOptions, UINT which);
extern STATUS   scsiTargetOptionsGet (SCSI_CTRL *pScsiCtrl, int devBusId,
				      SCSI_OPTIONS *pOptions);
extern void     scsiTargetReset (SCSI_CTRL * pScsiCtrl, UINT busId);
extern void     scsiThreadListShow   (LIST * pList);
extern void     scsiThreadListIdShow (LIST * pList);

extern void     scsiPhysDevShow (SCSI_PHYS_DEV * pScsiPhysDev, 
				 BOOL showThreads, BOOL noHeader);

extern char *	scsiPhaseNameGet (int scsiPhase);
extern void     scsiCacheSnoopEnable (SCSI_CTRL * pScsiCtrl);
extern void     scsiCacheSnoopDisable (SCSI_CTRL * pScsiCtrl);

/* functions to execute specific SCSI commands */

extern STATUS 	scsiFormatUnit   (SCSI_PHYS_DEV *pScsiPhysDev,
				  BOOL cmpDefectList, int defListFormat,
				  int vendorUnique,   int interleave,
				  char *buffer,       int bufLength);
extern STATUS 	scsiInquiry      (SCSI_PHYS_DEV *pScsiPhysDev, char *buffer,
			          int bufLength);
extern STATUS 	scsiModeSelect   (SCSI_PHYS_DEV *pScsiPhysDev, int pageFormat,
				  int saveParams, char *buffer, int bufLength);
extern STATUS 	scsiModeSense    (SCSI_PHYS_DEV *pScsiPhysDev, int pageControl,
			          int pageCode, char *buffer, int bufLength);
extern STATUS 	scsiReadCapacity (SCSI_PHYS_DEV *pScsiPhysDev, int *pLastLBA,
				  int *pBlkLength);
extern STATUS 	scsiReqSense     (SCSI_PHYS_DEV *pScsiPhysDev, char *buffer,
			          int bufLength);
extern STATUS 	scsiTestUnitRdy  (SCSI_PHYS_DEV *pScsiPhysDev);

extern STATUS 	scsiStartStopUnit(SCSI_PHYS_DEV *pScsiPhysDev,  BOOL start);

extern STATUS 	scsiRelease      (SCSI_PHYS_DEV * pScsiPhysDev);

extern STATUS 	scsiReserve      (SCSI_PHYS_DEV * pScsiPhysDev);

/* Functions to execute SCSI sequential access commands */

extern SEQ_DEV *scsiSeqDevCreate (SCSI_PHYS_DEV *pScsiPhysDev);

extern STATUS scsiErase (SCSI_PHYS_DEV *pScsiPhysDev, BOOL longErase);

extern STATUS scsiTapeModeSelect (SCSI_PHYS_DEV *pScsiPhysDev, int pageFormat, 
			          int saveParams, char *buffer, int bufLength);

extern STATUS scsiTapeModeSense (SCSI_PHYS_DEV *pScsiPhysDev, int pageControl,
			         int pageCode, char *buffer, int bufLength);

extern STATUS scsiReadBlockLimits (SCSI_PHYS_DEV *pScsiPhysDev, 
				   int *pMaxBlockLength, 
				   UINT16 *pMinBlockLength);

extern STATUS scsiSeqReadBlockLimits (SCSI_SEQ_DEV *pScsiSeqDev, 
				   int *pMaxBlockLength, 
				   UINT16 *pMinBlockLength);

extern int scsiRdTape (SCSI_SEQ_DEV *pScsiSeqDev, UINT numBytes, 
			  char *buffer, BOOL fixedSize);

extern STATUS scsiWrtTape (SCSI_SEQ_DEV *pScsiSeqDev, int numBytes, 
			   char *buffer, BOOL fixedSize);

extern STATUS scsiRewind (SCSI_SEQ_DEV *pScsiSeqDev);

extern STATUS scsiReserveUnit (SCSI_SEQ_DEV *pScsiSeqDev);

extern STATUS scsiReleaseUnit (SCSI_SEQ_DEV *pScsiSeqDev);

extern STATUS scsiLoadUnit (SCSI_SEQ_DEV *pScsiSeqDev, BOOL load, BOOL reten,
								     BOOL eot);

extern STATUS scsiWrtFileMarks (SCSI_SEQ_DEV *pScsiSeqDev, int numMarks, 
			        BOOL shortMark);

extern STATUS scsiSpace (SCSI_SEQ_DEV *pScsiSeqDev, int count, int spaceCode);

extern STATUS scsiSeqStatusCheck (SCSI_SEQ_DEV *pScsiSeqDev);

STATUS scsiSeqIoctl (SCSI_SEQ_DEV * pScsiSeqDev, int function, int arg);

/* SCSI controller driver support functions (not callable by applications) */

extern int               scsiIdentMsgBuild (UINT8         *msg,
				            SCSI_PHYS_DEV *pScsiPhysDev,
				            SCSI_TAG_TYPE  tagType,
				            SCSI_TAG       tagNumber);

extern SCSI_IDENT_STATUS scsiIdentMsgParse (SCSI_CTRL      *pScsiCtrl,
					    UINT8          *msg,
					    int             msgLength,
					    SCSI_PHYS_DEV **ppScsiPhysDev,
					    SCSI_TAG       *pTagNum);

extern STATUS scsiMsgInComplete  (SCSI_CTRL *pScsiCtrl, SCSI_THREAD *pThread);
extern STATUS scsiMsgOutComplete (SCSI_CTRL *pScsiCtrl, SCSI_THREAD *pThread);
extern void   scsiMsgOutReject   (SCSI_CTRL *pScsiCtrl, SCSI_THREAD *pThread);

extern void   scsiSyncXferNegotiate (SCSI_CTRL           *pScsiCtrl,
    	    	    	    	     SCSI_TARGET         *pScsiTarget,
    	    	    	    	     SCSI_SYNC_XFER_EVENT eventType);
extern void   scsiWideXferNegotiate (SCSI_CTRL		 *pScsiCtrl,
    	    	    	    	     SCSI_TARGET         *pScsiTarget,
    	    	    	    	     SCSI_WIDE_XFER_EVENT eventType);


extern void   scsiMgr (SCSI_CTRL *pScsiCtrl);

extern STATUS scsiMgrEventNotify (SCSI_CTRL  *pScsiCtrl,
				  SCSI_EVENT *pEvent,
				  int         eventSize);

extern void   scsiMgrCtrlEvent (SCSI_CTRL *pScsiCtrl, SCSI_EVENT_TYPE type);
extern void   scsiMgrBusReset  (SCSI_CTRL   *pScsiCtrl);

extern void   scsiMgrThreadEvent (SCSI_THREAD *pThread,
				  SCSI_THREAD_EVENT_TYPE type);

extern SCSI_THREAD * scsiMgrPhysDevActiveThreadFind (SCSI_PHYS_DEV *pScsiPhysDev,
				                     SCSI_TAG       tagNumber);

extern STATUS       scsiMgrRequestExecute  (SCSI_CTRL    * pScsiCtrl,
                                            SCSI_REQUEST * pRequest,
                                            SCSI_REPLY   * pReply);
extern void         scsiMgrPhysDevTagInit  (SCSI_PHYS_DEV * pScsiPhysDev);

extern void   scsiBusResetNotify (SCSI_CTRL *pScsiCtrl);

extern STATUS scsiThreadInit (SCSI_THREAD *pThread);
extern void   scsiCacheSynchronize (SCSI_THREAD *pThread,
				    SCSI_CACHE_ACTION action);

#else	/* __STDC__ */

/* SCSI block device driver functions */

extern BLK_DEV * scsiBlkDevCreate ();
extern void 	 scsiBlkDevInit ();
extern void 	 scsiBlkDevShow ();
extern STATUS 	 scsiRdSecs ();
extern STATUS 	 scsiWrtSecs ();

/* general SCSI functions */

extern SCSI_PHYS_DEV *	scsiPhysDevCreate ();
extern SCSI_PHYS_DEV *	scsiPhysDevIdGet ();
extern STATUS 	scsiPhysDevDelete ();
extern STATUS 	scsiAutoConfig ();
extern STATUS 	scsiBusReset ();
extern STATUS 	scsiCmdBuild ();
extern STATUS 	scsiCtrlInit ();
extern SCSI_THREAD * scsiCtrlIdentThreadCreate ();
extern STATUS        scsiCtrlThreadInit ();
extern STATUS        scsiCtrlThreadActivate ();
extern void          scsiCtrlEvent ();
extern BOOL          scsiCtrlThreadAbort ();

extern STATUS 	scsiIoctl ();
extern STATUS 	scsiShow ();
extern STATUS 	scsiTransact ();
extern STATUS   scsiTargetOptionsSet ();
extern STATUS   scsiTargetOptionsGet ();
extern void     scsiTargetReset ();
extern void     scsiThreadListShow ();
extern void     scsiThreadListIdShow ();
extern void     scsiPhysDevShow ();
extern char *	scsiPhaseNameGet ();
extern void     scsiCacheSnoopEnable ();
extern void     scsiCacheSnoopDisable ();

/* functions to execute specific SCSI commands */

extern STATUS 	scsiFormatUnit ();
extern STATUS 	scsiInquiry ();
extern STATUS 	scsiModeSelect ();
extern STATUS 	scsiModeSense ();
extern STATUS 	scsiReadCapacity ();
extern STATUS 	scsiReqSense ();
extern STATUS 	scsiTestUnitRdy ();

/* SCSI controller driver support functions (not callable by applications) */

extern int               scsiIdentMsgBuild ();
extern SCSI_IDENT_STATUS scsiIdentMsgParse ();

extern STATUS  scsiMsgInComplete ();
extern STATUS  scsiMsgOutComplete ();
extern void    scsiMsgOutReject ();
extern void    scsiSyncXferNegotiate ();
extern void    scsiWideXferNegotiate ();

extern void    scsiMgr ();
extern STATUS  scsiMgrEventNotify ();
extern void    scsiMgrCtrlEvent ();
extern void    scsiMgrBusReset ();
extern void    scsiMgrThreadEvent ();
extern STATUS  scsiMgrRequestExecute ();
extern void    scsiMgrPhysDevTagInit ();
extern SCSI_THREAD *scsiMgrPhysDevActiveThreadFind ();

extern void    scsiBusResetNotify ();
extern STATUS  scsiThreadInit ();
extern void    scsiCacheSynchronize ();


#endif	/* __STDC__ */

#ifdef __cplusplus
}
#endif

#endif /* _ASMLANGUAGE */
#endif /* __INCscsi2Libh */
