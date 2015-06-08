/* sysMotVpd.h - Vital Product Data header file. */

/* Copyright 1999 Wind River Systems, Inc. */
/* Copyright 1999-2000 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01b,16aug00,dmw  Wind River coding standards review.
01a,13mar00,dmw  Written (from version 01a of mcpn765/vpd.h).
*/

/*
DESCRIPTION
This file contains Vital Product Data definitions.
*/

#ifndef INCvpdh
#define INCvpdh

#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */


#define VPD_EYE_SIZE	8        /* size of eyecatcher */

#define VPD_EEPROM_SIZE 256     /* size of a serial eeprom */
#define VPD_DATA_SIZE   (VPD_EEPROM_SIZE - sizeof (VPD_HEADER))

/* vpd packet types */

#define VPD_PID_GI    0x00      /* guaranteed illegal */
#define VPD_PID_PID   0x01      /* product id */
#define VPD_PID_FAN   0x02      /* factory assembly number */
#define VPD_PID_SN    0x03      /* serial number */
#define VPD_PID_PCO   0x04      /* product configuration options */
#define VPD_PID_ICS   0x05      /* internal clock speed */
#define VPD_PID_ECS   0x06      /* external clock speed */
#define VPD_PID_RCS   0x07      /* reference clock speed */
#define VPD_PID_EA    0x08      /* ethernet address */
#define VPD_PID_MT    0x09      /* microprocessor type */
#define VPD_PID_CRC   0x0a      /* eeprom crc */
#define VPD_PID_FMC   0x0b      /* flash memory configuration */
#define VPD_PID_VLSI  0x0c      /* vlsi revisions/versions */
#define VPD_PID_PCS   0x0d      /* PCI clock speed */
#define VPD_PID_L2    0x0e      /* l2 cache configuration */
#define VPD_PID_TERM  0x0ff     /* termination packet */

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

#endif  /* INCvpdh */

