/* sysMotVpd.h - Vital Product Data header file. */

/* Copyright 1999 Wind River Systems, Inc. */
/* Copyright 1999-2002 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01d,17may04,cak Added some missing FLASH offset definitions. 
01c,29aug02,cak Moved VPD_EEPROM_SIZE definition to board header file, updated
		packet types based on version 2.1 of the VPD Specification, and
		fixed spelling mistakes. 
01b,25apr02,cak Added L3 cache support.
01a,25apr02,scb Adapted from MVME5100 base.
*/

/*
DESCRIPTION
This file contains Vital Product Data definitions.
*/

#ifndef __INCsysMotVpdh
#define __INCsysMotVpdh

#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */


#define VPD_EYE_SIZE	8        /* size of eyecatcher */

#define VPD_DATA_SIZE   (VPD_EEPROM_SIZE - sizeof (VPD_HEADER))

/* vpd packet types */

#define VPD_PID_GI	0x00	/* guaranteed illegal */
#define VPD_PID_PID	0x01	/* product id */
#define VPD_PID_FAN	0x02	/* factory assembly number */
#define VPD_PID_SN	0x03	/* serial number */
#define VPD_PID_PCO	0x04	/* product configuration options */
#define VPD_PID_ICS	0x05	/* internal clock speed */
#define VPD_PID_ECS	0x06	/* external clock speed */
#define VPD_PID_RCS	0x07	/* reference clock speed */
#define VPD_PID_EA	0x08	/* ethernet MAC address */
#define VPD_PID_MT	0x09	/* microprocessor type */
#define VPD_PID_CRC	0x0a	/* eeprom crc */
#define VPD_PID_FMC	0x0b	/* flash memory configuration */
#define VPD_PID_VLSI	0x0c	/* vlsi revisions/versions */
#define VPD_PID_PCS	0x0d	/* PCI clock speed */
#define VPD_PID_L2	0x0e	/* L2 cache configuration */
#define VPD_PID_VPDR	0x0F	/* VPD revision */
#define VPD_PID_L3	0x19	/* L3 cache configuration */
#define VPD_PID_TERM	0x0ff	/* termination packet */

/* Flash Memory Configration offsets */

#define VPD_FMC_DDW	0x04	/* Device Data Width (16-bits) */
#define VPD_FMC_NOD	0x05	/* Number of Devices/Sockets present */
#define VPD_FMC_NOC     0x06    /* Number of Columns */
#define VPD_FMC_CW      0x07    /* Column Width in Bits */
#define VPD_FMC_WEDW    0x08    /* Write/Erase Data Width */
#define VPD_FMC_BANK	0x09	/* Bank # of FLASH Memory Array (0=Bank A) */
#define VPD_FMC_SPEED   0x0a    /* ROM Access Speed in Nanoseconds */
#define VPD_FMC_SIZE	0x0b	/* Total Bank Size 5=8MB, 6=16MB etc. */

/* L3 Cache Configuration Packet Field Identifiers */

#define VPD_L3C_PROC_NUM		0x00	/* corresponding processor */
#define VPD_L3C_SIZE			0x01	/* L3 cache size */
#define VPD_L3C_CLK_RATIO		0x02	/* L3 clock ratio */
#define VPD_L3C_CACHE_CLK_SMPL_PNT	0x03	/* cache clock sample point */
#define VPD_L3C_PROC_CLK_SMPL_PNT	0x04	/* proc clock sample point */
#define VPD_L3C_SMPL_PNT_OVERRIDE	0x05	/* L3 sample point override */
#define VPD_L3C_SRAM_CLK_CTRL		0x06	/* L3 non-int ratios clk adj */
#define VPD_L3C_SRAM_TYPE		0x07	/* SRAM type */
#define VPD_L3C_DATA_ERROR_DETECT	0x08	/* data parity checking */
#define VPD_L3C_ADDR_ERROR_DETECT	0x09	/* address parity checking */

typedef struct vpdHeader 
    {
    UCHAR eyeCatcher[VPD_EYE_SIZE];
    UINT16 size;
    } VPD_HEADER;

typedef struct vpd 
    {
    VPD_HEADER header;
    UCHAR packets[VPD_DATA_SIZE];
    } VPD;

typedef struct vpdPacket 
    {
    UCHAR type;
    UCHAR size;
    UCHAR data[1];   /* size depends on packet type */
    } VPD_PACKET;


#ifdef __cplusplus
   }
#endif /* __cplusplus */

#endif  /* __INCsysMotVpdh */
