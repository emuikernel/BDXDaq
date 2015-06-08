/* ppcProc.h - PowerPC Processor Version and Revision Definitions */

/* Copyright 2003, Motorola Inc. All Rights Reserved */

/*
modification history
--------------------
01b,21apr03,scb  Checked into common source base
01a,09apr03,yyz  Copied from MV6100 base version 01c.
*/

/*
This file contains the version and revision definitions for the 
PowerPC family of processors, as found in the PVR (Processor Version
Register).  Also contained here are the macros used to extract the
version and revision values from the PVR.
*/

#ifndef __INCppcProch
#define INCppcProch

#ifdef __cplusplus
   extern "C" {
#endif

/* CPU Version and Revision Macros */

#define CPU_TYPE            ((vxPvrGet() >> 16) & 0xffff)
#define CPU_REV             (vxPvrGet() & 0xffff)

/* CPU Versions */

#define	CPU_TYPE_601		0x01	/* PPC 601 CPU */
#define	CPU_TYPE_602		0x02	/* PPC 602 CPU */
#define	CPU_TYPE_603		0x03	/* PPC 603 CPU */
#define	CPU_TYPE_603E		0x06	/* PPC 603e CPU */
#define	CPU_TYPE_603P		0x07	/* PPC 603p CPU */
#define	CPU_TYPE_750		0x08	/* PPC 750 CPU */
#define	CPU_TYPE_604		0x04	/* PPC 604 CPU */
#define	CPU_TYPE_604E		0x09	/* PPC 604e CPU */
#define	CPU_TYPE_604R		0x0A	/* PPC 604r CPU */
#define	CPU_TYPE_7400		0x0C	/* PPC 7400 "MAX" CPU */
#define	CPU_TYPE_7410		0x800C	/* PPC 7410 "NITRO" CPU */
#define	CPU_TYPE_7450		0x8000	/* PPC 7450 "VGer" CPU */
#define	CPU_TYPE_74X5		0x8001	/* PPC 7445/55 "Apollo6" CPUs */
#define CPU_TYPE_74X7           0x8002  /* PPC 7447/57 "Apollo7" CPUs */

/* CPU Revisions */

/* PPC 7400 "Max" */

#define CPU_7400_REV_1_1	0x0101	/* Rev 1.1 */
#define CPU_7400_REV_2_0	0x0200	/* Rev 2.0 */
#define CPU_7400_REV_2_2	0x0202	/* Rev 2.2 */
#define CPU_7400_REV_2_6	0x0206	/* Rev 2.6 */
#define CPU_7400_REV_2_7	0x0207	/* Rev 2.7 */
#define CPU_7400_REV_2_8	0x0208	/* Rev 2.8 */
#define CPU_7400_REV_2_9	0x0209	/* Rev 2.9 */

/* PPC 7410 "Nitro" */

#define	CPU_7410_REV_1_0	0x1100	/* Rev 1.0 */
#define CPU_7410_REV_1_2	0x1102	/* Rev 1.2 */
#define CPU_7410_REV_3_3	0x1103	/* Rev 3.3 */
#define CPU_7410_REV_3_4	0x1104	/* Rev 3.4 */
 
/* PPC 7450 "VGer" */

#define CPU_7450_REV_1_0	0x0100	/* Rev 1.0 */
#define CPU_7450_REV_1_1	0x0101	/* Rev 1.1 */
#define CPU_7450_REV_1_2	0x0102	/* Rev 1.2 */
#define CPU_7450_REV_2_0	0x0200	/* Rev 2.0, 2.02, 2.03, and 2.04 */
#define CPU_7450_REV_2_1	0x0201	/* Rev 2.1 */

/* PPC 7445 & 7455 "Apollo6" */

#define CPU_7455_REV_2_1	0x0201	/* PPC 7455 Rev 2.1 CPU */
#define CPU_7455_REV_3_3	0x0303  /* PPC 7455 Rev 3.3 CPU */
#define CPU_7445_REV_2_1	0x0201  /* PPC 7445 Rev 2.1 CPU */
#define CPU_7445_REV_3_5	0x0303  /* PPC 7445 Rev 3.5 CPU */

/* PPC 7447 & 7457 "Apollo7" */

#define CPU_7457_REV_1_0	0x0100  /* PPC 7457 Rev 1.0 CPU */
#define CPU_7457_REV_1_1	0x0101  /* PPC 7457 Rev 1.1 CPU */

#ifdef __cplusplus
    }
#endif

#endif /* __INCppcProch */
