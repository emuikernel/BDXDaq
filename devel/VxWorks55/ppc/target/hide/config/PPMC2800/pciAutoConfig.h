/******************************************************************************
*              (c), Copyright, MOTOROLA  INC  - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

 * pciAutoConfig.h - PCI autoConfuration */

/* Copyright 1998 Motorola, Inc. All Rights Reserved */

/*
modification history
--------------------
01a,24apr98,scb  created by Motorola.
*/

/*
Description:

Contains structure and defines which support PCI automatic configuration.
*/


#ifndef	INCpciAutoConfigh
#define	INCpciAutoConfigh

#ifdef __cplusplus
extern "C" {
#endif

/* miscellaneous defines */

#define PCIARYSIZE 64           /* Maximum number of PCI array entries */
#define INCPCILIST 0            /* Index of included PCI function list */
#define EXCPCILIST 1            /* Index of excluded PCI function list */
#define MAXPCILIST 2            /* Number of lists in PCI function array */

#define MAXPCIDEV 32            /* Number of PCI devices on a bus */
#define MAXPCIFUNC 8            /* Number of PCI functions on a device */

/* structures */

/*
 * PCI location structure
 *
 * Warning: Do not modify the size of this structure.  This entire
 * structure has been typecast as UINT throughout the code in order to
 * maintain compatibility with existing code.
 */
typedef struct _PCI_LOC {
   unsigned enable:     1;
   unsigned reserved:   7;
   unsigned bus:        8;
   unsigned device:     5;
   unsigned function:   3;
   unsigned reg_number: 6;
   unsigned offset:     2;
} PCI_LOC;

/* PCI identification structure */
typedef struct _PCI_ID {
   PCI_LOC loc;
   UINT devVend;
} PCI_ID;

typedef struct pciUnique
    {
    UINT pciMem32;                  /* 32 bit memory space allocation ptr */
    UINT pciMem20;                  /* 20 bit memory space allocation ptr */
    UINT pciIo32;                   /* 32 bit io space allocation pointer */
    UINT pciIo16;                   /* 16 bit io space allocation pointer */
    UINT pciIncludes;               /* Size of the include function list */
    UINT pciExcludes;               /* Size of the exclude function list */
    PCI_LOC pciList[PCIARYSIZE];    /* PCI descriptor list area */
    /*
     * The size of the above array depends on the number of PCI device
     * functions present in the system.  This allocation simply reserves
     * a block of memory for the PCI function list.
     */
    } _PCI_UNIQUE;

#ifdef __cplusplus
}
#endif

#endif	/* INCpciAutoConfigh */

