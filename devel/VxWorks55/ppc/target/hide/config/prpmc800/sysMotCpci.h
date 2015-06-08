/* sysMotCpci.h - Motorola CompactPCI Backpanel Devices and Board IDs */

/* Copyright 1999-2001 Wind River Systems, Inc. */
/* Copyright 1999-2001 Motorola, Inc., All Rights Reserved */

/*
modification history
--------------------
01c,10oct01,scb  Add BAR offset to polling list.
01b,17oct00,dmw  Updated PrPMC800 subsystem vendor.
01a,31aug00,dmw  Written (from version 01e of mcpn765/sysMotCpci.h).
*/

/*
This file contains:
 - The CompactPCI subsystem vendor and board IDs for Motorola CompactPCI boards
 - Motorola CompactPCI boards which participate in VxWorks shared memory.
 - Motorola CompactPCI boards which participate in VxWorks Backpanel networking

IMPORTANT NOTE/WARNING: 
To provide for support of additional board types in shared memory
configurations, in particular the prpmc800, the sysSmAnchorPollingList
structure was changed to include an additional field in each entry
(representing the BAR offset through which to querry for the shared
memory anchor).  In addition, the typdef definition for
SYS_SM_ANCHOR_POLLING_LIST has been moved from the board header file
into this file.  If you are compiling a BSP and encounter an error
because of the duplicate declaration for SYS_SM_ANCHOR_POLLING_LIST
you simply need to remove the definition from the board header file
and maintain the definition which appears in this file.  Older BSPs
which have only two elements per entry for SYS_SM_ANCHOR_POLLING_LIST
instead of three as has currently been defined should need no other
modification for a clean compile.
*/

#ifndef INCsysMotCpcih
#define INCsysMotCpcih

#ifdef __cplusplus
    extern "C" {
#endif

/* CompactPCI subsystem vendor and board IDs for Motorola CompactPCI boards */

#define MOT_SUB_VNDR_ID_VAL      0x1057   /* Vendor = Motorola */
#define CPV3060_SUB_SYS_ID_VAL   0x4804   /* Subsystem = CPV3060 */
#define MCPN750_SUB_SYS_ID_VAL   0x4805   /* Subsystem = MCPN750 (Sitka) */
#define PRPMCBASE_SUB_SYS_ID_VAL 0x480e   /* Subsystem = PrPMC Carrier */
#define PRPMC750_SUB_SYS_ID_VAL	 0x480f	  /* Subsystem = PrPMC750 */
#define PRPMC600_SUB_SYS_ID_VAL  0x4810   /* Subsystem = PrPMC600 */
#define MCPN765_SUB_SYS_ID_VAL   0x4811   /* Subsystem = MCPN765 */
#define PRPMC800_SUB_SYS_ID_VAL  0x4817   /* Subsystem = PrPMC800 */

/* Motorola CompactPCI boards which participate in VxWorks shared memory. */

#define SYS_MOT_SM_ANCHOR_POLL_LIST \
    { PCI_ID_BR_DEC21554, \
      ((MCPN750_SUB_SYS_ID_VAL << 16)  | MOT_SUB_VNDR_ID_VAL ), \
      (PCI_CFG_BASE_ADDRESS_0) },\
    { PCI_ID_BR_DEC21554, \
      ((PRPMCBASE_SUB_SYS_ID_VAL << 16) | MOT_SUB_VNDR_ID_VAL ), \
      (PCI_CFG_BASE_ADDRESS_0) },\
    { PCI_ID_BR_DEC21554, \
      ((CPV3060_SUB_SYS_ID_VAL << 16)   | MOT_SUB_VNDR_ID_VAL ), \
      (PCI_CFG_BASE_ADDRESS_0) },\
    { PCI_ID_BR_DEC21554, \
      ((MCPN765_SUB_SYS_ID_VAL << 16)   | MOT_SUB_VNDR_ID_VAL ), \
      (PCI_CFG_BASE_ADDRESS_0) },\
    { PCI_ID_BR_DEC21554, \
      ((PRPMC800_SUB_SYS_ID_VAL << 16)  | MOT_SUB_VNDR_ID_VAL ), \
      (PCI_CFG_BASE_ADDRESS_1) },\
    { PCI_ID_HARRIER, \
      ((PRPMC800_SUB_SYS_ID_VAL << 16)  | MOT_SUB_VNDR_ID_VAL ), \
      (PCI_CFG_BASE_ADDRESS_1) },

/*
 * The following list is for the CompactPCI Host to use when connecting
 * interrupt handlers to support each DEC2155x device interrupting on the
 * CompactPCI backpanel when using shared memory.
 */

#define SYS_MOT_SM_DEVICE_LIST \
    { PCI_ID_BR_DEC21554, \
     (MCPN750_SUB_SYS_ID_VAL << 16)   | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (PRPMCBASE_SUB_SYS_ID_VAL << 16) | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (CPV3060_SUB_SYS_ID_VAL << 16)   | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (MCPN765_SUB_SYS_ID_VAL << 16)   | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_BR_DEC21554, \
     (PRPMC800_SUB_SYS_ID_VAL << 16)  | MOT_SUB_VNDR_ID_VAL },\
    { PCI_ID_HARRIER, \
     (PRPMC800_SUB_SYS_ID_VAL << 16)  | MOT_SUB_VNDR_ID_VAL },

#ifndef _ASMLANGUAGE

    /* Shared memory anchor polling list */

    typedef struct sysSmAnchorPollingList
        {
        UINT devVend;
        UINT subIdVend;
        UINT barOffset;
        } SYS_SM_ANCHOR_POLLING_LIST;
#endif /* _ASMLANGUAGE */

#ifdef __cplusplus
    }
#endif

#endif	/* INCsysMotCpcih */
