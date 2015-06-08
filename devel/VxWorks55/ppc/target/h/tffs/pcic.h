
/*
 * $Log:   V:/pcic.h_v  $
   
      Rev 1.2   18 Aug 1996 13:47:52   amirban
   Comments
   
      Rev 1.1   31 Jul 1996 14:29:44   amirban
   5V and 12V Vpp
   
      Rev 1.0   20 Mar 1996 13:33:20   amirban
   Initial revision.
 */

/************************************************************************/
/*                                                                      */
/*		FAT-FTL Lite Software Development Kit			*/
/*		Copyright (C) M-Systems Ltd. 1995-1996			*/
/*									*/
/************************************************************************/


#ifndef PCIC_H
#define PCIC_H

#ifdef __cplusplus
extern "C" {
#endif

/* -----	Equates */

#define	BASE_PORT			0x3e0

#define INDEX_REG			BASE_PORT
#define	DATA_REG			(BASE_PORT + 1)

#define	VPP_WAIT_TICK			3	/* At least 100 msec. */

#define	NO_OF_WINDOWS			5

/* ----- Function index values */

#define	IDENTIFICATION			0
#define	INTERFACE			1
#define	POWER				2
#define	INTERRUPT			3
#define	CARD_STATUS_CHANGE		4
#define	CARD_STATUS_INTERRUPT		5
#define	ADDRESS_WINDOW_ENABLE		6
#define	WINDOW_0_START_LO		0x10
#define	WINDOW_0_START_HI		0x11
#define	WINDOW_0_STOP_LO		0x12
#define	WINDOW_0_STOP_HI		0x13
#define	WINDOW_0_ADDRESS_LO		0x14
#define	WINDOW_0_ADDRESS_HI		0x15

/* ----- Register bit mapping */

/*					Identification 	*/
#define	PCIC_STEP_A			0x82
#define	PCIC_STEP_B			0x83

/*					Interface 	*/
#define	CARD_DETECT_1			4
#define	CARD_DETECT_2			8
#define	MEMORY_WRITE_PROTECT		0x10
#define	CARD_READY			0x20
#define	CARD_POWER_ACTIVE		0x40
#define	VPP_VALID			0x80	/* Step A only */

/*					Power		*/
#define	VPP1_CONTROL			3
#define	VPP2_CONTROL			0xc
#define	CARD_POWER_ENABLE		0x10
#define	AUTO_POWER_SWITCH_ENABLE 	0x20
#define	DISABLE_RESUME_RESETDRV		0x40
#define	ENABLE_OUTPUTS			0x80

#define	VPP_ON_12V		     	0xa
#define VPP_ON_5V			5

/*					Interrupt	*/
#define	PC_CARD_TYPE			0x20
#define	PC_CARD_NOT_RESET		0x40

/*					Card Status Change	*/
#define	CARD_DETECT_CHANGE		8

/*					Card Status Interrupt	*/
#define	CARD_DETECT_ENABLE		8

/*					Address Window Enable	*/
#define	MEMORY_WINDOW_0_ENABLE		1
#define	MEMORY_WINDOW_1_ENABLE		2
#define	MEMORY_WINDOW_2_ENABLE		4
#define	MEMORY_WINDOW_3_ENABLE		8
#define	MEMORY_WINDOW_4_ENABLE		0x10
#define	MEMCS16_DECODE			0x20

/*					Window 0 Base Start High */
#define	DATA_16_BITS			0x80

/*					Window 0 Address High	*/
#define	REG_ACTIVE			0x40
#define	WRITE_PROTECT			0x80

#ifdef __cplusplus
}
#endif

#endif
