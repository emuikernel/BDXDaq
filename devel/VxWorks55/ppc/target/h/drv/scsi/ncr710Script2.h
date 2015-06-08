/* ncr710Script.h - NCR 710 Script definition header */

/* Copyright 1984-1992 Wind River Systems, Inc. */

/*
modification history
--------------------
03c,03jun94,ihw  corrected problem with NCR_COMPILE macro
03b,27may94,ihw  documented prior to release
03a,02may94,ihw  major modifications to work with new SCSI architecture
    	    	    supports tagged commands
02a,18feb94,ihw  modified for enhanced SCSI library: multiple initiators,
                    disconnect/reconnect and synchronous transfer supported
01i,22sep92,rrr  added support for c++
01h,01sep92,ccc	 fixed warnings.
01g,03jul92,eve  change NCRBUG to NCR_COMPILER_BUG
01f,03jul92,eve  Merge new header
01e,26jun92,ccc  change ASMLANGUAGE to _ASMLANGUAGE.
40b,26may92,rrr  the tree shuffle
40a,12nov91,ccc  SPECIAL VERSION FOR 5.0.2 68040 RELEASE.
02a,26oct91,eve  Add byte definition to turnoff/on the timeout in the script.
01a,23oct91,eve	 Created script and driver header
*/

#ifndef __INCncr710Script2h
#define __INCncr710Script2h

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _ASMLANGUAGE

#ifndef NCR_COMPILE

/*
 *  Entry points defined by PROCs in the script.
 */

extern ULONG ncr710Wait[];		/* wait for re-select or host cmd   */
extern ULONG ncr710InitStart[];		/* start new initiator thread       */
extern ULONG ncr710InitContinue[];	/* continue an initiator thread     */
extern ULONG ncr710TgtDisconnect[];	/* disconnect from SCSI bus         */

/*
 *  Structure giving parameters for block moves using the NCR 710's
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
typedef struct ncr710Shared
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
    } NCR710_SHARED;

#endif	/* NCR_COMPILE */

/*
 *  The following offsets are used in the script.  They _MUST_ be kept in
 *  sync with the definition of the NCR710_NEXUS structure above.
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
#define	HOST_FLAGS          scratch0	/* host <-> SIOP: flags - see below  */

#define	MSG_OUT_STATE	    scratch1	/* host <-> SIOP: pending,sent,none  */

#define	MSG_IN_STATE	    scratch2	/* host <-> SIOP: pending,sent,none  */

#define	TARGET_BUS_ID	    scratch3	/* host <-  SIOP: bus IDs asserted   */
#define	CURRENT_PHASE	    scratch3    /* host <-  SIOP: phase in progress  */

/* Bitmasks for HOST_FLAGS */

#define	FLAGS_IDENTIFY	    0x01	/* assert ATN on select, to IDENTIFY */

/*
 *  These definitions provide meaningful aliases for use in "ncr710Lib.c".
 *  I don't like #defining names of structure members, but it's better than
 *  having "scratch0" and so forth all over the place ...  Be careful of
 *  clashes with ordinary variable names.
 */
#define	nHostFlags   	    scratch0	/* (r/w) flag bits as defined above  */
#define	nMsgOutState	    scratch1	/* (r/w) state of message out buffer */
#define	nMsgInState 	    scratch2	/* (r/w) state of message in  buffer */
#define	nBusIdBits  	    scratch3	/* (r-o) bus ID bits set on reselect */
#define	nBusPhase    	    scratch3	/* (r-o) most recent phase; mismatch */


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
#define	NCR710_READY		     0	/* ready to start new host command  */
#define	NCR710_SCRIPT_ABORTED	     1	/* have aborted a running script    */

#define	NCR710_MESSAGE_OUT_SENT	    10	/* have successfully sent a message */
#define	NCR710_MESSAGE_IN_RECVD	    11	/* have received a complete message */
#define	NCR710_EXT_MESSAGE_SIZE     12	/* have read size of extended msg   */
#define	NCR710_NO_MSG_OUT   	    13	/* msg out req. with no msg pending */

#define	NCR710_CMD_COMPLETE	    20	/* SCSI command completed           */
#define	NCR710_DISCONNECTED   	    21	/* target has disconnected          */
#define	NCR710_RESELECTED	    22	/* have been reselected by target   */
#define	NCR710_SELECTED	    	    23	/* have been selected as a target   */

#define	NCR710_SPURIOUS_CMD 	    30	/* spurious interrupt of resel wait */
#define	NCR710_ILLEGAL_PHASE	    31	/* target requested phase 4 or 5    */
#define	NCR710_NO_IDENTIFY  	    32	/* no IDENTIFY after reselection    */

/*
 *  Note: the following are not generated by the script, but it seems
 *  sensible to define them here for continuity.
 */
#define	NCR710_PHASE_MISMATCH	    50	/* unexpected phase change occurred  */
#define	NCR710_SCSI_TIMEOUT 	    51	/* select, reselect or xfer timeout  */
#define	NCR710_SCSI_BUS_RESET	    52	/* SCSI bus has been reset           */
#define	NCR710_UNEXPECTED_DISCON    53	/* target disconnected unexpectedly  */

#define	NCR710_SINGLE_STEP  	    60	/* single-step completed             */

#define	NCR710_FATAL_ERROR  	    99	/* bus error, illegal opcode, etc.   */


/*
 *  NCR 710 register bit definitions
 *
 *  Note: there are similar definitions in "ncr710.h".
 */
#define	SBCL_SEL    	    	0x10	/* 1 => SCSI SEL signal asserted  */
#define	SBCL_ATN    	    	0x08	/* 1 => SCSI ATN signal asserted  */

#define	SCNTL0_TARGET_MODE  	0x01	/* 1 => SIOP is in target mode    */

#define	SCNTL1_CONNECTED    	0x10	/* 1 => SIOP is connected to SCSI */

#define	SIEN_MISMATCH_ATN   	0x80	/* 1 => mismatch/ATN intr enabled */

#define	CTEST2_SIGNAL	    	0x40	/* 1 => signal pending from host  */

#define	CTEST7_NO_TIMEOUT	0x10	/* 1 => disable all SCSI timeouts */


#endif  /* _ASMLANGUAGE */

#ifdef __cplusplus
}
#endif

#endif /* __INCncr710Script2h */
