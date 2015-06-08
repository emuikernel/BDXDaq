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


*******************************************************************************
* pci.target.h - Header File for :
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       None.
* Sept 18th 2002
* Added _ethernetdevice structure
******************************************************************************/

#ifndef __INCpciTargeth
#define __INCpciTargeth

 /* OS Dependent/ HW Platform specific defines.
  * Change these for your target.
  *
  * You need to look at the HW Schematic to identify the CPU
  * Interrupt Line for each PCI Interrupt Line. Then you need
  * to look at the OS configuration to decide the CPU Interrupt
  * Line to CPU Interrupt Vector Number mapping. This then needs
  * to be translated into to the Interrupt Vector.
  *
  * The following defines consolidates all the above transforms
  * into a single PCI Interrupt Line to Interrupt Vector map.
  */

/* includes */
#include <stdio.h>

struct _ethernetdevice
{
 int deviceid;
 int vendorid;
 int deviceno;
 int functionno;
 int busno;
};
/* defines  */
/* Stuff specified by the standard. */
#define PCI_MAX_BUS     0
#define PCI_BRIDGE_DEVICE   31 /* Dev 31 is used for bridge control */
#define PCI_MAX_DEVICE    30

#define PCI_INTR_A     1
#define PCI_INTR_B     2
#define PCI_INTR_C     3
#define PCI_INTR_D     4

/* Some specialized header types */
#define PCI_MultiFunction_Device 0x80
#define PCI_Device_HeaderType  0x00


/* PCI Configuration space registers take up 256 bytes.
 * The first 64 bytes are of fixed format. The following is
 * device specific. The meaning of each register is based on
 * register size.
 *
 * PCI System Architecture: Chapter 17: pg 330.
 */
#define PCI_REG_VendorID   0x00
#define PCI_REG_DeviceID   0x02
#define PCI_REG_Command    0x04
#define PCI_REG_Status    0x06
#define PCI_REG_RevisionID   0x08
#define PCI_REG_ClassCode   0x0A
#define PCI_REG_CacheLineSize  0x0C
#define PCI_REG_LatencyTimer  0x0D
#define PCI_REG_HeaderType   0x0E
#define PCI_REG_BIST    0x0F
/* The following registers are defined ONLY for Header Type == 0 */
#define PCI_REG_BaseAddress_0  0x10
#define PCI_REG_BaseAddress_1  0x14
#define PCI_REG_CardbusCISPointer 0x28
#define PCI_REG_SubsysVendorID  0x2C
#define PCI_REG_SubsysID   0x2E
#define PCI_REG_ROMBaseAddress  0x30
#define PCI_REG_IntrLine   0x3C
#define PCI_REG_IntrPin    0x3D
#define PCI_REG_MinGnt    0x3E
#define PCI_REG_MaxLat    0x3F

/* Some register specific bit selectors */
#define PCI_REG_HeaderType_MultiFunc 0x0080
#define PCI_REG_HeaderType_Type   0x007F
#define PCI_REG_Command_IOSpace   0x0001
#define PCI_REG_Command_MemSpace  0x0002
#define PCI_REG_Command_BusMaster  0x0004
#define PCI_REG_Command_MWI       0x010 /* rahamim4 - was 100 */
#define PCI_REG_BaseAddress_IO   0x0001
#define PCI_REG_BaseAddress_MemType  0x0006
#define PCI_REG_BaseAddress_IOAddr  (~0x0003)
#define PCI_REG_BaseAddress_MemAddr  (~0x000F)

#define TARGET_INTR_VEC(intNum)  ((void *) INUM_TO_IVEC (intNum))

 /* HW Platform specific defines. Change these for your target.
  *
  * The following assumes that your system uses method 1 to
  * access the PCI Configuration Space registers.
  */

/* If this is greater than 1, rewrite the pciscan() function */
#define TARGET_MAX_BUS    1

 /* On some hardware, reading the local host interface causes
  * the PCI Bus interface to hang. These defines ensure that
  * the scan functions will skip reading the local host interface
  * configuration space registers.
  */
#define TARGET_CPU_BUS    0
#define TARGET_CPU_DEVICE   0
 /* This is the PCI Address to use to read the CPU's PCI Config
  * Space. This is an alias that will not hang the CPU.
  */
#define TARGET_CPU_ALIAS_BUS  0
#define TARGET_CPU_ALIAS_DEVICE  0


/* Convert bus/device/func to PCI Configuration Address for Mechanism 1.
 * Note: bus denotes a _network_ in the PCI inter-network.
 *   bus/device denotes a unique node on the PCI inter-network.
 *   function denotes a specific function in a node.
 * Register's MUST be read at 4-byte boundaries. The Register index is in
 * terms of byte offsets.
 */
#define PCI_CFG_ADDR(bus,device,addr)        \
 (0x80000000 |       /* Enable PCI Config cycle */ \
 ((bus & 0xFF) << 16) |  /* Set the bus ID          */ \
 ((device & 0x1F) << 11) | /* Set the device ID       */ \
 (addr & 0xFC) )    /* Set the register ID     */
#define PCI_CFG_MF_ADDR(bus,device,function,addr)   \
 (PCI_CFG_ADDR(bus,device,addr) |      \
 ((function & 0x07) << 8)) /* Set the function ID */


 /* Translate PCI data representation to HOST format */
#if  (_BYTE_ORDER != _BIG_ENDIAN)

#define PCI_DATA(data) (data)

#else

#define PCI_DATA(data) (\
   (((data) & 0xFF000000) >> 24) | \
   (((data) & 0x00FF0000) >>  8) | \
   (((data) & 0x0000FF00) <<  8) | \
   (((data) & 0x000000FF) << 24) )

#endif

/* Function declarations */
void  gtPciCpuDevNum  (int *bus, int *device);
void  *gtPciIntrToVecNum  (int intrLine, int bus, int device);
int  gtPciGetMaxBusNo (void);


/* pci.target.h API list */
#if PCI_TARGET_SPECIFIC_FUNCTIONS
/* Get PCI Bus/Device IDs for CPU */
void gtPciCpuDevNum(int *bus, int *device)
{
 *bus = TARGET_CPU_BUS;
 *device = TARGET_CPU_DEVICE;
}

/* Get the maximum number of busses in the system */
int gtPciGetMaxBusNo(void)
{
 return TARGET_MAX_BUS;
}

/* Translate PCI Interrupt line to interrupt vector number */
void *gtPciIntrToVecNum (
 int intrLine,  /* PCI Intr Line (A = 1, B = 2, C = 3, D = 4) */
 int bus,
 int device)
{
    extern void panic ();
    void *vector = NULL;

    switch (intrLine)
    {
    case PCI_INTR_A:
        vector = TARGET_INTR_VEC(PCI0_INTR_A_VEC);
        if (vector < 0)
        {
            printf ("pci_IntrLineToVecNum: interrupt num %d not connected\n",
                    intrLine);
            panic ();
            while (1) {}; /* Never return */
        }
        return vector;

    case PCI_INTR_B:
        vector = TARGET_INTR_VEC(PCI0_INTR_B_VEC);
        if (vector < 0)
        {
            printf ("pci_IntrLineToVecNum: interrupt num %d not connected\n",
                    intrLine);
            panic ();
            while (1) {}; /* Never return */
         }
         return vector;

    case PCI_INTR_C:
        vector = TARGET_INTR_VEC(PCI0_INTR_C_VEC);
        if (vector < 0)
        {
            printf ("pci_IntrLineToVecNum: interrupt num %d not connected\n",
                     intrLine);
            panic ();
            while (1) {}; /* Never return */
        }
        return vector;
 
    case PCI_INTR_D:
        vector = TARGET_INTR_VEC(PCI0_INTR_D_VEC);
        if (vector < 0)
        {
            printf ("pci_IntrLineToVecNum: interrupt num %d not connected\n",
                    intrLine);
            panic ();
            while (1) {}; /* Never return */
        }
        return vector;

    default:
        printf ("pci_IntrLineToVecNum: bad interrupt num %d\n",intrLine);
        panic ();
        while (1) {}; /* Never return */
    }
}
#endif /* PCI_TARGET_SPECIFIC_FUNCTIONS */



#endif /* __INCpciTargeth */

