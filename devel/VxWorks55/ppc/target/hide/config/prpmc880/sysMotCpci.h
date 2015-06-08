/* sysMotCpci.h - Motorola CompactPCI Backpanel Devices and Board IDs */

/* Copyright 1999-2003 Wind River Systems, Inc. */
/* Copyright 1999-2003 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01a,31mar03,simon  Ported. (from ver 01b, prpmc610/sysMotCpci.h)
*/

/*
This file contains:
 - The CompactPCI subsystem vendor and board IDs for Motorola CompactPCI
boards
 - Motorola CompactPCI boards which participate in VxWorks shared
memory.
 - Motorola CompactPCI boards which participate in VxWorks Backpanel
networking
*/

#ifndef INCsysMotCpcih
#define INCsysMotCpcih

#ifdef __cplusplus
    extern "C" {
#endif

#define MOT_SUB_VNDR_ID_VAL 0x1057      /* Vendor = Motorola */

#define CPV3060_SUB_SYS_ID_VAL   0x4804 /* Subsystem = CPV3060 */
#define MCPN750_SUB_SYS_ID_VAL   0x4805 /* Subsystem = MCPN750 (Sitka) */
#define PRPMCBASE_SUB_SYS_ID_VAL 0x480e /* Subsystem = PrPMC Carrier */
#define PRPMC750_SUB_SYS_ID_VAL  0x480f /* Subsystem = PrPMC750 */
#define PRPMC600_SUB_SYS_ID_VAL  0x4810 /* Subsystem = PrPMC600 */
#define MCPN765_SUB_SYS_ID_VAL   0x4811 /* Subsystem = MCPN765 */
#define CPN5360_SUB_SYS_ID_VAL   0x4814 /* Subsystem = CPN5360 */
#define PRPMC800_SUB_SYS_ID_VAL  0x4817 /* Subsystem = PrPMC800 */
#define CPN5365_SUB_SYS_ID_VAL   0x4818 /* Subsystem = CPN5365 */
#define MCPN805_SUB_SYS_ID_VAL   0x481C   /* Subsystem = MCPN805 */
#define PRPMC610_SUB_SYS_ID_VAL  0x4820   /* Subsystem = PrPMC610 */
#define PRPMC880_SUB_SYS_ID_VAL  0x4823   /* Subsystem = PrPMC880 */

/*
 * The poll list should be shared among BSPs in order for inter-detection.
 * However, some BSPs may not have all the macros defined.  Thus it is
 * necessary to define them here, with the risk of inconsistency if one
 * is changed only within a BSP.
 */

#ifndef PCI_ID_BR_DEC21554
#  define PCI_ID_BR_DEC21554 0x00461011     /* ID Intel 21554 PCI bridge */
#endif

#ifndef PCI_ID_BR_DEC21555
#  define PCI_ID_BR_DEC21555 0xb5558086     /* ID Intel 21555 PCI bridge */
#endif

#ifndef PCI_ID_HARRIER
#  define PCI_ID_HARRIER 0x480B1057         /* ID Harrier PHB */
#endif

/* Motorola CompactPCI boards which participate in VxWorks shared memory. */

#define SYS_MOT_SM_ANCHOR_POLL_LIST \
    { PCI_ID_BR_DEC21554, \
     (CPV3060_SUB_SYS_ID_VAL << 16)   | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (MCPN750_SUB_SYS_ID_VAL << 16)   | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (PRPMCBASE_SUB_SYS_ID_VAL << 16) | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (PRPMC750_SUB_SYS_ID_VAL << 16)  | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (PRPMC600_SUB_SYS_ID_VAL << 16)  | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (MCPN765_SUB_SYS_ID_VAL << 16)   | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21555, \
      (MCPN765_SUB_SYS_ID_VAL << 16) | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (CPN5360_SUB_SYS_ID_VAL << 16)   | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (PRPMC800_SUB_SYS_ID_VAL << 16)  | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (CPN5365_SUB_SYS_ID_VAL << 16)   | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (MCPN805_SUB_SYS_ID_VAL << 16)   | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (PRPMC610_SUB_SYS_ID_VAL << 16)  | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (PRPMC880_SUB_SYS_ID_VAL << 16)  | MOT_SUB_VNDR_ID_VAL },

/*
 * The following list is for the CompactPCI Host to use when connecting
 * interrupt handlers to support each DEC2155x device interrupting on the
 * CompactPCI backpanel when using shared memory.
 */

#define SYS_MOT_SM_DEVICE_LIST \
    { PCI_ID_BR_DEC21554, \
     (CPV3060_SUB_SYS_ID_VAL << 16)   | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (MCPN750_SUB_SYS_ID_VAL << 16)   | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (PRPMCBASE_SUB_SYS_ID_VAL << 16) | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (PRPMC750_SUB_SYS_ID_VAL << 16)  | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (PRPMC600_SUB_SYS_ID_VAL << 16)  | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (MCPN765_SUB_SYS_ID_VAL << 16)   | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21555, \
      (MCPN765_SUB_SYS_ID_VAL << 16) | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (CPN5360_SUB_SYS_ID_VAL << 16)   | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (PRPMC800_SUB_SYS_ID_VAL << 16)  | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (CPN5365_SUB_SYS_ID_VAL << 16)   | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (MCPN805_SUB_SYS_ID_VAL << 16)   | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (PRPMC610_SUB_SYS_ID_VAL << 16)  | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (PRPMC880_SUB_SYS_ID_VAL << 16)  | MOT_SUB_VNDR_ID_VAL }     
     
#ifdef __cplusplus
    }
#endif

#ifndef _ASMLANGUAGE

    /* Shared memory anchor polling list */

    typedef struct sysSmAnchorPollingList
        {
        UINT devVend;
        UINT subIdVend;
        UINT barOffset;
        } SYS_SM_ANCHOR_POLLING_LIST;

#endif /* _ASMLANGUAGE */

#endif  /* INCsysMotCpcih */
