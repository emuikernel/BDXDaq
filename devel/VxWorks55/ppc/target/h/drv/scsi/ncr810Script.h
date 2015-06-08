/* ncr810Script.h - NCR 810 Script definition header */

/* Copyright 1984-1994 Wind River Systems, Inc. */

/*
modification history
--------------------
01c,28jan99,dat  Fixed HTH timeout, part of SPR 22597
01b,21nov96,dds  Added compiler directives to turn optmization off when
                 compiling the ncr810Shared data structure.
01a,25may95,jds	 Created script and driver header; adapted from ncr710 driver
*/

#ifndef __INCncr810Scripth
#define __INCncr810Scripth

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

#ifndef NCR_COMPILE


/*
 *  Entry points defined by PROCs in the script.
 */

extern ULONG ncr810Wait[];		/* wait for re-select or host cmd   */
extern ULONG ncr810InitStart[];		/* start new initiator thread       */
extern ULONG ncr810InitContinue[];	/* continue an initiator thread     */
extern ULONG ncr810TgtDisconnect[];	/* disconnect from SCSI bus         */

#if CPU_FAMILY==I960
#pragma align 1                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */


/*
 *  Structure giving parameters for block moves using the NCR 810's
 *  indirect addressing ("MOVE FROM") capability.
 */

typedef struct moveParams
    {
    ULONG  size;			/* # bytes to move to/from SCSI */
    UINT8 *addr;			/* where in memory they go/come */
    } MOVE_PARAMS;

/*
 *  Shared structure defining all information about a SCSI nexus which has
 *  to be visible to the script.  Note that this structure must not be
 *  changed without corresponding changes to the OFFSET_* definitions below.
 *
 *  IMPORTANT NOTE: Every element in this structure _must_ be aligned on
 *  a 4-byte boundary, otherwise the script will fail (illegal instruction).
 */
typedef struct ncr810Shared
    {
    ULONG       device;			/* target bus ID, sync xfer params */
    MOVE_PARAMS command;		/* command bytes sent to target    */
    MOVE_PARAMS data;			/* data bytes to/from target       */
    MOVE_PARAMS status;			/* command status byte from target */
    MOVE_PARAMS identOut;		/* outgoing identification msg     */
    MOVE_PARAMS identIn;		/* incoming identification msg     */
    MOVE_PARAMS msgOut;			/* message bytes sent to target    */
    MOVE_PARAMS msgIn;			/* message bytes recvd from target */
    MOVE_PARAMS msgInSecond;		/* second byte of multi-byte msg   */
    MOVE_PARAMS msgInRest;		/* remainder of multi-byte message */
    } NCR810_SHARED;

#if CPU_FAMILY==I960
#pragma align 0                 /* tell gcc960 not to optimize alignments */
#endif  /* CPU_FAMILY==I960 */

#endif	/* NCR_COMPILE */

/*
 *  The following offsets are used in the script.  They _MUST_ be kept in
 *  sync with the definition of the NCR810_NEXUS structure above.
 */
#define	SIZEOF_ULONG   	    	4
#define	SIZEOF_MOVE_PARAMS  	8

#define OFFSET_DEVICE	    	0
#define OFFSET_CMD  	    	(OFFSET_DEVICE          + SIZEOF_ULONG)
#define OFFSET_DATA 	    	(OFFSET_CMD             + SIZEOF_MOVE_PARAMS)
#define OFFSET_STATUS	    	(OFFSET_DATA            + SIZEOF_MOVE_PARAMS)
#define	OFFSET_IDENT_OUT    	(OFFSET_STATUS          + SIZEOF_MOVE_PARAMS)
#define	OFFSET_IDENT_IN    	(OFFSET_IDENT_OUT       + SIZEOF_MOVE_PARAMS)
#define OFFSET_MSG_OUT	    	(OFFSET_IDENT_IN        + SIZEOF_MOVE_PARAMS)
#define OFFSET_MSG_IN	    	(OFFSET_MSG_OUT         + SIZEOF_MOVE_PARAMS)
#define OFFSET_MSG_IN_SECOND	(OFFSET_MSG_IN          + SIZEOF_MOVE_PARAMS)
#define OFFSET_MSG_IN_REST	(OFFSET_MSG_IN_SECOND   + SIZEOF_MOVE_PARAMS)


/*
 *  Scratch registers used for data transfer to host
 *
 */
#define	HOST_FLAGS          scratcha0	/* host <-> SIOP: flags - see below  */

#define	MSG_OUT_STATE	    scratcha1	/* host <-> SIOP: pending,sent,none  */

#define	MSG_IN_STATE	    scratcha2	/* host <-> SIOP: pending,sent,none  */

#define	TARGET_BUS_ID	    scratcha3	/* host <-  SIOP: bus IDs asserted   */
#define	CURRENT_PHASE	    scratcha3   /* host <-  SIOP: phase in progress  */

/* Bitmasks for HOST_FLAGS */

#define	FLAGS_IDENTIFY	    0x01	/* assert ATN on select, to IDENTIFY */

/*
 *  These definitions provide meaningful aliases for use in "ncr810Lib.c".
 *  I don't like #defining names of structure members, but it's better than
 *  having "scratch0" and so forth all over the place ...  Be careful of
 *  clashes with ordinary variable names.
 */
#define	nHostFlags   	    scratcha0	/* (r/w) flag bits as defined above  */
#define	nMsgOutState	    scratcha1	/* (r/w) state of message out buffer */
#define	nMsgInState 	    scratcha2	/* (r/w) state of message in  buffer */
#define	nBusIdBits  	    scratcha3	/* (r-o) bus ID bits set on reselect */
#define	nBusPhase    	    scratcha3	/* (r-o) most recent phase; mismatch */


/*
 *  SCSI phases - defined here to avoid including "scsiLib.h".  Must match !
 */
#define	PHASE_NONE  	      0xff	/* no info transfer pending */
#define PHASE_DATA_OUT       	 0	/* data out (to target)     */
#define PHASE_DATA_IN        	 1	/* data in (from target)    */
#define PHASE_COMMAND       	 2	/* command (to target)      */
#define PHASE_STATUS        	 3	/* status (from target)     */
#define PHASE_MSG_OUT        	 6	/* message out (to target)  */
#define PHASE_MSG_IN         	 7	/* message in (from target) */

/*
 *  SCSI messages - defined here to avoid including "scsiLib.h".  Must match !
 */
#define M_CMD_COMPLETE          0x00	/* command complete msg. */
#define M_EXT_MSG               0x01	/* extended message msg. */
#define M_DISCONNECT            0x04	/* disconnect msg.       */
#define M_ABORT                	0x06	/* abort msg.            */
#define M_NO_OP                 0x08	/* no operation msg.     */
#define M_ABORT_TAG            	0x0d	/* abort tag msg.        */

/*
 *  SCSI message in status - must match SCSI_MSG_IN_STATE enum in "scsiLib.h"
 */
#define	M_IN_NONE  	    	0	/* no message in in progress     */
#define	M_IN_SECOND_BYTE    	1   	/* next byte is second of two    */
#define	M_IN_EXT_MSG_LEN	2	/* next byte is ext msg length   */
#define	M_IN_EXT_MSG_DATA    	3	/* next byte(s) are ext msg data */

/*
 *  SCSI message out status - must match SCSI_MSG_OUT_STATE enum in "scsiLib.h"
 */
#define	M_OUT_NONE  	    	0	/* no message out in progress */
#define	M_OUT_PENDING	    	1	/* message out ready to send  */
#define	M_OUT_SENT  	    	2	/* message out has been sent  */


/*
 *  Script status interrupt codes
 */
#define	NCR810_READY		     0	/* ready to start new host command  */
#define	NCR810_SCRIPT_ABORTED	     1	/* have aborted a running script    */

#define	NCR810_MESSAGE_OUT_SENT	    10	/* have successfully sent a message */
#define	NCR810_MESSAGE_IN_RECVD	    11	/* have received a complete message */
#define	NCR810_EXT_MESSAGE_SIZE     12	/* have read size of extended msg   */
#define	NCR810_NO_MSG_OUT   	    13	/* msg out req. with no msg pending */

#define	NCR810_CMD_COMPLETE	    20	/* SCSI command completed           */
#define	NCR810_DISCONNECTED   	    21	/* target has disconnected          */
#define	NCR810_RESELECTED	    22	/* have been reselected by target   */
#define	NCR810_SELECTED	    	    23	/* have been selected as a target   */

#define	NCR810_SPURIOUS_CMD 	    30	/* spurious interrupt of resel wait */
#define	NCR810_ILLEGAL_PHASE	    31	/* target requested phase 4 or 5    */
#define	NCR810_NO_IDENTIFY  	    32	/* no IDENTIFY after reselection    */

/*
 *  Note: the following are not generated by the script, but it seems
 *  sensible to define them here for continuity.
 */
#define	NCR810_PHASE_MISMATCH	    50	/* unexpected phase change occurred  */
#define	NCR810_SCSI_TIMEOUT 	    51	/* select, reselect or xfer timeout  */
#define NCR810_HANDSHAKE_TIMEOUT    54 	/* handshake timeout */
#define	NCR810_SCSI_BUS_RESET	    52	/* SCSI bus has been reset           */
#define	NCR810_UNEXPECTED_DISCON    53	/* target disconnected unexpectedly  */

#define	NCR810_SINGLE_STEP  	    60	/* single-step completed             */

#define	NCR810_FATAL_ERROR  	    99	/* bus error, illegal opcode, etc.   */


/*
 *  NCR 810 register bit definitions
 *
 *  Note: there are similar definitions in "ncr810.h".
 */
#define	SBCL_SEL    	    	0x10	/* 1 => SCSI SEL signal asserted  */
#define	SBCL_ATN    	    	0x08	/* 1 => SCSI ATN signal asserted  */

#define	SCNTL0_TARGET_MODE  	0x01	/* 1 => SIOP is in target mode    */

#define	SCNTL1_CONNECTED    	0x10	/* 1 => SIOP is connected to SCSI */

#define	SIEN0_MISMATCH_ATN   	0x80	/* 1 => mismatch/ATN intr enabled */

#define	CTEST2_SIGNAL	    	0x40	/* 1 => signal pending from host  */

#define STIME0_HTH_MASK 	0xf0	/* Handshake-to-handshake timeout mask*/
#define STIME0_SEL_MASK		0x0f	/* Select timeout bit-mask */
#define STIME0_HTH_TIMEOUT	0xd0	/* Handshake-to-handshake timeout valu*/
#define STIME0_SEL_TIMEOUT	0x0d	/* Select timeout value */

#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCncr810Scripth */
