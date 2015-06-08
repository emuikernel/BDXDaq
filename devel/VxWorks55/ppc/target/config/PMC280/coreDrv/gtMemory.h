/******************************************************************************
*              (c), Copyright, FORCE COMPUTERS INDIA Limited                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF FORCE COMPUTERS, INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF FORCE OR ANY THIRD PARTY. FORCE RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO FORCE COMPUTERS.       *
* THIS CODE IS PROVIDED "AS IS". FORCE COMPUTERS MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
*******************************************************************************

*******************************************************************************
* gtMemory.h - Memory control functions header file. 
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

#ifndef __INCgtMemoryh
#define __INCgtMemoryh

/* includes */
#include "gtCore.h"

/* defines */
#define MEM_DONT_MODIFY       0xffffffff
#define MEM_PARITY_SUPPORT    0x40000000

#define _8BIT                 0x00000000
#define _16BIT                0x00100000
#define _32BIT                0x00200000
#define _64BIT                0x00300000

/* typedefs */
typedef struct _gtDeviceParam 
{                             /* boundary values */
    unsigned int turnOff;     /* 0x0 - 0xf       */
    unsigned int acc2First;   /* 0x0 - 0x1f      */ 
    unsigned int acc2Next;    /* 0x0 - 0x1f      */
    unsigned int ale2Wr;      /* 0x0 - 0xf       */
    unsigned int wrLow;       /* 0x0 - 0xf       */
    unsigned int wrHigh;      /* 0x0 - 0xf       */
    unsigned int badrSkew;    /* 0x0 - 0x2       */  
    unsigned int DPEn;        /* 0x0 - 0x1       */
    unsigned int deviceWidth; /* in Bytes        */
} GT_DEVICE_PARAM;  

typedef enum _pciMemWindow{PCI_0_IO,PCI_0_MEM0,PCI_0_MEM1,PCI_0_MEM2,PCI_0_MEM3
                          } PCI_MEM_WINDOW;
typedef enum _memoryBank{BANK0, BANK1, BANK2, BANK3} MEMORY_BANK;
typedef enum _gtDevice  {DEVICE0,DEVICE1,DEVICE2,DEVICE3,BOOT_DEVICE} GT_DEVICE;

/* There are 21 fixed windows: PCI , devCS , CS (SDRAM) etc. for the CPU 
   decoding mechanism*/
typedef enum _memoryWindow {MEM_CS_0 = BIT0,MEM_CS_1 = BIT1,MEM_CS_2 = BIT2,
                            MEM_CS_3 = BIT3,MEM_DEVCS_0 = BIT4,
                            MEM_DEVCS_1 = BIT5,MEM_DEVCS_2 = BIT6,
                            MEM_DEVCS_3 = BIT7,MEM_BOOT_CS = BIT8,
                            MEM_PCI_0_IO = BIT9,MEM_PCI_0_MEM0 = BIT10,
                            MEM_PCI_0_MEM1 = BIT11,MEM_PCI_0_MEM2 = BIT12,
                            MEM_PCI_0_MEM3 = BIT13,MEM_PCI_1_IO = BIT14,
                            MEM_PCI_1_MEM0 = BIT15,MEM_PCI_1_MEM1 = BIT16,
                            MEM_PCI_1_MEM2 = BIT17,MEM_PCI_1_MEM3 = BIT18,
                            MEM_INTEGRATED_SRAM = BIT19,
                            MEM_INTERNAL_SPACE = BIT20,
                            MEM_ALL_WINDOWS = 0X1FFFFF
                           } MEMORY_WINDOW;       
typedef enum _memoryWindowStatus {MEM_WINDOW_ENABLED,MEM_WINDOW_DISABLED
                                 } MEMORY_WINDOW_STATUS;       
/* There are four possible windows that can be defined as protected */
typedef enum _memoryProtectWindow{MEM_WINDOW0,MEM_WINDOW1,MEM_WINDOW2,
                                  MEM_WINDOW3
                                 } MEMORY_PROTECT_WINDOW; 
/* When defining a protected window , this paramter indicates whether it
   is accessible or not */
typedef enum _memoryAccess       {MEM_ACCESS_ALLOWED, MEM_ACCESS_FORBIDDEN
                                 } MEMORY_ACCESS;
/* When defining a protected window , this paramter indicates whether it
   is accessible for writing or not */
typedef enum _memoryAccessWrite  {MEM_WRITE_ALLOWED, MEM_WRITE_FORBIDDEN
                                 } MEMORY_ACCESS_WRITE;
/* When defining a protected window , this paramter indicates whether caching
   is alowed or forbidden */
typedef enum _memoryCacheProtect {MEM_CACHE_ALLOWED, MEM_CACHE_FORBIDDEN
                                 } MEMORY_CACHE_PROTECT;
/* functions */

/* Returns base address and size for a given BANK or GT_DEVICE */ 
unsigned int gtMemoryGetBankBaseAddress  (MEMORY_BANK bank);
unsigned int gtMemoryGetDeviceBaseAddress(GT_DEVICE device);
unsigned int gtMemoryGetBankSize         (MEMORY_BANK bank);
unsigned int gtMemoryGetDeviceSize       (GT_DEVICE device);
unsigned int gtMemoryGetDeviceWidth      (GT_DEVICE device);

/* when given base Address and size Set new WINDOW for CS_X (SDRAM).
   (X = 0,1,2 or 3*/
void gtMemoryMapBank0(unsigned int bank0Base,unsigned int bank0Size);
void gtMemoryMapBank1(unsigned int bank1Base,unsigned int bank1Size);
void gtMemoryMapBank2(unsigned int bank2Base,unsigned int bank2Size);
void gtMemoryMapBank3(unsigned int bank3Base,unsigned int bank3Size);

/* when given base Address and size Set new BAR for devCS_X. 
   (X = 0,1,2,3 or BOOT*/
void gtMemoryMapDevice0Space(unsigned int device0Base,unsigned int device0Size);
void gtMemoryMapDevice1Space(unsigned int device1Base,unsigned int device1Size);
void gtMemoryMapDevice2Space(unsigned int device2Base,unsigned int device2Size);
void gtMemoryMapDevice3Space(unsigned int device3Base,unsigned int device3Size);
void gtMemoryMapDeviceBootSpace(unsigned int bootDeviceBase,
                                unsigned int bootDeviceSize);

/* Change the Internal Register Base Address to a new given Address. */
void gtMemoryMapInternalRegistersSpace(unsigned int internalRegBase);

/* Returns internal Register Space Base Address. */
unsigned int gtMemoryGetInternalRegistersSpace(void);

/* Disable or enable one of the 21 windows dedicated for the CPU 
   decoding mechanism */
void gtMemoryDisableWindow(MEMORY_WINDOW window);
void gtMemoryEnableWindow (MEMORY_WINDOW window);
MEMORY_WINDOW_STATUS gtMemoryGetMemWindowStatus(MEMORY_WINDOW window);

/* Configurate the protection feature to a given space. */
void gtMemorySetProtectRegion(MEMORY_PROTECT_WINDOW window,
                              MEMORY_ACCESS gtMemoryAccess,
                              MEMORY_ACCESS_WRITE gtMemoryWrite,
                              MEMORY_CACHE_PROTECT cacheProtection,
                              unsigned int baseAddress,
                              unsigned int size);

/* Disable the protection by the function 'gtMemorySetProtectRegion' */
void gtMemoryDisableProtectRegion(MEMORY_PROTECT_WINDOW window);

/* Returns the integrated SRAM Base Address. */
unsigned int gtMemoryGetInternalSramBaseAddr(void);

/* Set new base address for the integrated SRAM. */
void gtMemorySetInternalSramBaseAddr(unsigned int sramBaseAddress);

/* Writes a new remap value to the remap register */
unsigned int gtMemorySetPciRemapValue(PCI_MEM_WINDOW memoryWindow,
                                      unsigned int remapValueHigh,
                                      unsigned int remapValueLow);

/* Extarcts the device's parameters */
bool gtMemoryGetDeviceParam(GT_DEVICE_PARAM *deviceParam, GT_DEVICE deviceNum);

/* Sets new parameters for a device */
bool gtMemorySetDeviceParam(GT_DEVICE_PARAM *deviceParam, GT_DEVICE deviceNum);

#endif /* __INCgtMemoryh */

