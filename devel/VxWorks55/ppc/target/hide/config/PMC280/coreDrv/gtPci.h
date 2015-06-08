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
* gtPci.h - PCI functions header file 
*
* DESCRIPTION:
*       None.
*
* DEPENDENCIES:
*       None.
*
******************************************************************************/

#ifndef __INCgtPcih
#define __INCgtPcih

/* includes */
#include "gtCore.h"

/* Macros */

/* The next Macros configurate the initiator board (PCI_SELF) or any any agent 
   on the PCI to become: MASTER, response to MEMORY transactions , response to
   IO transactions or TWO both MEMORY_IO transactions. Those configuration
   are for PCI0 only. */
   
#define GT_PCI_0_MASTER_ENABLE(deviceNumber) frcPci0WriteConfigReg(             \
          PCI_STATUS_AND_COMMAND,deviceNumber,PCI_MASTER_ENABLE |              \
          frcPci0ReadConfigReg(PCI_STATUS_AND_COMMAND,deviceNumber) )

#define GT_PCI_0_MASTER_DISABLE(deviceNumber) frcPci0WriteConfigReg(            \
          PCI_STATUS_AND_COMMAND,deviceNumber,~PCI_MASTER_ENABLE &             \
          frcPci0ReadConfigReg(PCI_STATUS_AND_COMMAND,deviceNumber) )
                                                                               
#define GT_PCI_0_MEMORY_ENABLE(deviceNumber) frcPci0WriteConfigReg(             \
          PCI_STATUS_AND_COMMAND,deviceNumber,PCI_MEMORY_ENABLE |              \
          frcPci0ReadConfigReg(PCI_STATUS_AND_COMMAND,deviceNumber) )
                                                                               
#define GT_PCI_0_IO_ENABLE(deviceNumber) frcPci0WriteConfigReg(                 \
          PCI_STATUS_AND_COMMAND,deviceNumber,PCI_I_O_ENABLE |                 \
          frcPci0ReadConfigReg(PCI_STATUS_AND_COMMAND,deviceNumber) )
                                                                               
#define GT_PCI_0_SLAVE_ENABLE(deviceNumber) frcPci0WriteConfigReg(              \
          PCI_STATUS_AND_COMMAND,deviceNumber,PCI_MEMORY_ENABLE |              \
          PCI_I_O_ENABLE | frcPci0ReadConfigReg(PCI_STATUS_AND_COMMAND,         \
                                               deviceNumber) )                 
                                                                               
#define GT_PCI_0_DISABLE(deviceNumber) frcPci0WriteConfigReg(                   \
          PCI_STATUS_AND_COMMAND,deviceNumber,0xfffffff8  &                    \
          frcPci0ReadConfigReg(PCI_STATUS_AND_COMMAND,deviceNumber))            
                                                                               
#define PCI_MASTER_ENABLE		                   BIT2  
#define	PCI_MEMORY_ENABLE		                   BIT1  
#define	PCI_I_O_ENABLE  		                   BIT0  
#define PCI_SELF                                   32
/* Definitions to clarify the code, not to be used specifically */
#define PCI_MASTER_ABORT                           BIT8
#define PCI_VENDOR_ID                              0xffff
#define PCI_REVISION_ID                            0xff
#define PCI_DEVICE_ID                              0xffff0000
#define PCI_ERROR_CODE                             0xffffffff
#define PCI_CONFIG_ENABLE                          BIT31
#define PCI_TYPE0                                  NO_BIT
#define PCI_TYPE1                                  BIT0
#define PCI_OPERATION_MODE                         (BIT4|BIT5) 

/********************************************************/
/* Definition for the access regions (from slave side). */
/********************************************************/

/* enables the access control mechanism */
#define PCI_ACCESS_CONTROL_WINDOW_ENABLE           BIT0
/* PCI master always asserts REQ64*. This feature is Only relevant if 
   the target is the peer PCI interface (P2P gtMemory transaction) */
#define PCI_REQ64_ENABLE                           BIT1
#ifdef INCLUDE_SNOOP_SUPPORT
    /* Cache Coherency support.    
       NOTE: Only relevant if the target is SDRAM .
             In PCIX mode - if bit[30] in the attribute phase
             (no Snoop) is asserted, these bit are ignored. */
    #define PCI_NON_COHERENT_REGION                NO_BIT
    #define PCI_CACHE_COHERENT_WT_REGION           BIT2
    #define PCI_CACHE_COHERENT_WB_REGION           BIT3
#endif /* INCLUDE_SNOOP_SUPPORT */
/*   Access Protect  */                               
#define PCI_ACCESS_FORBBIDEN                       BIT4
/*   Write Protect   */                               
#define PCI_WRITE_FORBBIDEN                        BIT5
/*   PCI slave Data Swap Control   */                 
#define PCI_NO_SWAP                                BIT6
#define PCI_BYTE_AND_WORD_SWAP                     BIT7
#define PCI_WORD_SWAP                              (BIT7|BIT6)
/*   Max burst control. 
    Specifies the maximum burst size for a single transac-
    tion between a PCI slave and the other interfaces   
    NOTE: In case of cache coherent region(bit[3:2] are
          0x1 or 0x2), the maximum burst is 32 bytes
          regardless of MBurst setting */
#define PCI_MAX_BURST_32                           NO_BIT
#define PCI_MAX_BURST_64                           BIT8  
#define PCI_MAX_BURST_128                          BIT9
/*  Typical PCI read transaction Size. Defines the amount
    of data the slave prefetch from the target unit.
    Only relevant to conventional PCI mode.  */
#define PCI_READ_TRANSACTION_SIZE_32_BYTES         NO_BIT
#define PCI_READ_TRANSACTION_SIZE_64_BYTES         BIT10
#define PCI_READ_TRANSACTION_SIZE_128_BYTES        BIT11
#define PCI_READ_TRANSACTION_SIZE_256_BYTES        (BIT10|BIT11)

/* typedefs */

typedef enum _pciAccessRegions{REGION0,REGION1,REGION2,REGION3,REGION4,REGION5
                              } PCI_ACCESS_REGIONS;

typedef enum _pciBusMode      {CONVENTIONAL_PCI,PCI_X_66MHZ,PCI_X_100MHZ,
                               PCI_X_133MHZ
                              } PCI_BUS_MODE;

typedef enum _pciAgentPrio    {LOW_AGENT_PRIO, HI_AGENT_PRIO
                              } PCI_AGENT_PRIO;

typedef enum _pciAgentPark    {PARK_ON_AGENT, DONT_PARK_ON_AGENT
                              } PCI_AGENT_PARK;

typedef enum _pciBusFreq      {PCI_33MHZ, PCI_66MHZ
                              } PCI_BUS_FREQ;

typedef enum _pciBarMapping   {PCI_MEMORY_BAR, PCI_IO_BAR, PCI_NO_MAPPING
                              } PCI_BAR_MAPPING;

typedef enum _pciBarType      {PCI_32BIT_BAR = 0x0, PCI_RESERVED = 0x01,
                               PCI_64BIT_BAR = 0x2
                              } PCI_BAR_TYPE;

/* BAR structure */
typedef struct _pciBar
{
    unsigned int function;
    unsigned int barOffset;
    unsigned int barBaseLow;
    unsigned int barBaseHigh;
    unsigned int barSizeLow;
    unsigned int barSizeHigh;
    /* The 'barBaseAddr' is a 64-bit variable
       that will contain the TOTAL base address 
       value achived by combining both the 'barBaseLow'
       and the 'barBaseHigh' parameters as follows:
       
       BIT: 63          31         0
            |           |         |
            barBaseHigh barBaseLow */
    unsigned long long barBaseAddr;
    /* The 'barSize' is a 64-bit variable
       that will contain the TOTAL size achived 
       by combining both the 'barSizeLow' and 
       the 'barSizeHigh' parameters as follows:
       
       BIT: 63          31         0
            |           |         |
            barSizeHigh barSizeLow 
            
       NOTE: The total size described above
             is AFTER the size calculation as 
             described in PCI spec rev2.2 */       
    unsigned long long barSize;
    bool               isPrefetchable;
    PCI_BAR_TYPE       barType;
    PCI_BAR_MAPPING    barMapping;

} PCI_BAR;

/* Device information structure */
typedef struct _pciDevice
{
    /* Device specific information */
    char         type[30];
    unsigned int deviceNum;
    unsigned int venID;                                            
    unsigned int deviceID;
    unsigned int revisionID;
    bool         isMultiFunction;
    bool         isFastB2BCapable;
    PCI_BUS_FREQ busFrequency;
    PCI_BAR      pciBar[43]; /* BARs list */
} PCI_DEVICE;

typedef struct _pciSelfBars
{
    unsigned int SCS0Base;
    unsigned int SCS0BaseHigh;
    unsigned int SCS0Size;
    unsigned int SCS1Base;
    unsigned int SCS1BaseHigh;
    unsigned int SCS1Size;
    unsigned int SCS2Base;
    unsigned int SCS2BaseHigh;
    unsigned int SCS2Size;
    unsigned int SCS3Base;
    unsigned int SCS3BaseHigh;
    unsigned int SCS3Size;
    unsigned int internalMemBase;
    unsigned int internalMemBaseHigh;
    unsigned int internalIOBase;
    unsigned int CS0Base;
    unsigned int CS0BaseHigh;
    unsigned int CS0Size;
    unsigned int CS1Base;
    unsigned int CS1BaseHigh;
    unsigned int CS1Size;
    unsigned int CS2Base;
    unsigned int CS2BaseHigh;
    unsigned int CS2Size;
    unsigned int CS3Base;
    unsigned int CS3BaseHigh;
    unsigned int CS3Size;
    unsigned int CSBootBase;
    unsigned int CSBootBaseHigh;
    unsigned int CSBootSize;
    unsigned int P2PMem0Base;
    unsigned int P2PMem0BaseHigh;
    unsigned int P2PMem0Size;
    unsigned int P2PMem1Base;
    unsigned int P2PMem1BasEHigh;
    unsigned int P2PMem1Size;
    unsigned int P2PIOBase;
    unsigned int P2PIOSize;
#ifdef INCLUDE_CPU_MAPPING 
    unsigned int CPUBase;
    unsigned int CPUBaseHigh;
    unsigned int CPUSize;
#endif /* INCLUDE_CPU_MAPPING */
} PCI_SELF_BARS;

/* Returns the pci bus mode (PCI-X or conventional PCI) */
PCI_BUS_MODE gtPci0GetBusMode(void);

/* read/write configuration registers on local PCI bus. */
void _frcPci0WriteConfigReg(unsigned int,unsigned int,
                          unsigned int, unsigned int,
                          unsigned int data);
void frcPci0WriteConfigReg(unsigned int regOffset,unsigned int pciDevNum,
                          unsigned int data);

unsigned int _frcPci0ReadConfigReg(unsigned int,unsigned int,
                                 unsigned int, unsigned int);
unsigned int frcPci0ReadConfigReg(unsigned int regOffset,
                                  unsigned int pciDevNum);

/* read/write configuration registers on another PCI bus. */
void gtPci0OverBridgeWriteConfigReg(unsigned int regOffset,
                                    unsigned int pciDevNum,
                                    unsigned int busNum,unsigned int data);

unsigned int gtPci0OverBridgeReadConfigReg(unsigned int regOffset,
                                           unsigned int pciDevNum,
                                           unsigned int busNum);

/* Performs full scane on both PCI and returns all detail possible on the 
   agents which exist on the bus. */

/*      Master`s gtMemory space   */
void gtPci0MapIOspace     (unsigned int pci0IoBase  ,unsigned int pci0IoSize);
void gtPci0MapMemory0space(unsigned int pci0Mem0Base,unsigned int pci0Mem0Size);
void gtPci0MapMemory1space(unsigned int pci0Mem1Base,unsigned int pci0Mem1Size);
void gtPci0MapMemory2space(unsigned int pci0Mem2Base,unsigned int pci0Mem2Size);
void gtPci0MapMemory3space(unsigned int pci0Mem3Base,unsigned int pci0Mem3Size);

unsigned int gtPci0GetIOspaceBase(void);
unsigned int gtPci0GetIOspaceSize(void);
unsigned int gtPci0GetMemory0Base(void);
unsigned int gtPci0GetMemory0Size(void);
unsigned int gtPci0GetMemory1Base(void);
unsigned int gtPci0GetMemory1Size(void);
unsigned int gtPci0GetMemory2Base(void);
unsigned int gtPci0GetMemory2Size(void);
unsigned int gtPci0GetMemory3Base(void);
unsigned int gtPci0GetMemory3Size(void);

/*      Slave`s gtMemory space   */

void gtPci0ScanSelfBars(PCI_SELF_BARS *pci0SelfBars);

void gtPci0MapInternalRegSpace  (unsigned int pci0InternalBase);
void gtPci0MapInternalRegIOSpace(unsigned int pci0InternalBase);

void gtPci0MapMemoryBank0(unsigned int pci0Dram0Base,
                          unsigned int pci0Dram0Size);
void gtPci0MapMemoryBank1(unsigned int pci0Dram1Base,
                          unsigned int pci0Dram1Size);
void gtPci0MapMemoryBank2(unsigned int pci0Dram2Base,
                          unsigned int pci0Dram2Size);
void gtPci0MapMemoryBank3(unsigned int pci0Dram3Base,
                          unsigned int pci0Dram3Size);

void gtPci0MapDevice0MemorySpace(unsigned int pci0Dev0Base,
                                 unsigned int pci0Dev0Size);
void gtPci0MapDevice1MemorySpace(unsigned int pci0Dev1Base,
                                 unsigned int pci0Dev1Size);
void gtPci0MapDevice2MemorySpace(unsigned int pci0Dev2Base,
                                 unsigned int pci0Dev2Size);
void gtPci0MapDevice3MemorySpace(unsigned int pci0Dev3Base,
                                 unsigned int pci0Dev3Size);
void gtPci0MapBootDeviceMemorySpace(unsigned int pci0DevBootBase,
                                    unsigned int pci0DevBootSize);

void gtPci0MapP2pMem0Space(unsigned int pci0P2pMem0Base, 
                           unsigned int pci0P2pMem0Size);
void gtPci0MapP2pMem1Space(unsigned int pci0P2pMem1Base, 
                           unsigned int pci0P2pMem1Size);
void gtPci0MapP2pIoSpace  (unsigned int pci0P2pIoBase, 
                           unsigned int pci0P2pIoSize);

#ifdef INCLUDE_CPU_MAPPING 
void gtPci0MapCPUspace(unsigned int pci0CpuBase,unsigned int pci0CpuSize);
#endif /* INCLUDE_CPU_MAPPING */

/* DAC mapping function .
   The following function are similar to the regular function 
   ( without DAC suffix ) only it updates also the high base address 
   corresponding to address bits[63:32].*/

     /* PCI 0 */
void gtPci0MapMemoryBank0DAC(unsigned int pci0Dram0BaseLow,
                             unsigned int pci0Dram0BaseHigh,
                             unsigned int pci0Dram0Size);
void gtPci0MapMemoryBank1DAC(unsigned int pci0Dram1BaseLow,
                             unsigned int pci0Dram1BaseHigh,
                             unsigned int pci0Dram1Size);
void gtPci0MapMemoryBank2DAC(unsigned int pci0Dram2BaseLow,
                             unsigned int pci0Dram2BaseHigh,
                             unsigned int pci0Dram2Size);
void gtPci0MapMemoryBank3DAC(unsigned int pci0Dram3BaseLow,
                             unsigned int pci0Dram3BaseHigh,
                             unsigned int pci0Dram3Size);
void gtPci0MapDevice0MemorySpaceDAC(unsigned int pci0Dev0BaseLow,
                                    unsigned int pci0Dev0BaseHigh,
                                    unsigned int pci0Dev0Size);
void gtPci0MapDevice1MemorySpaceDAC(unsigned int pci0Dev1BaseLow,
                                    unsigned int pci0Dev1BaseHigh,
                                    unsigned int pci0Dev1Size);
void gtPci0MapDevice2MemorySpaceDAC(unsigned int pci0Dev2BaseLow,
                                    unsigned int pci0Dev2BaseHigh,
                                    unsigned int pci0Dev2Size);
void gtPci0MapDevice3MemorySpaceDAC(unsigned int pci0Dev3BaseLow,
                                    unsigned int pci0Dev3BaseHigh,
                                    unsigned int pci0Dev3Size);
void gtPci0MapBootDeviceMemorySpaceDAC(unsigned int pci0DevBootBaseLow,
                                       unsigned int pci0DevBootBaseHigh,
                                       unsigned int pci0DevBootSize);
void gtPci0MapP2pMem0SpaceDAC(unsigned int pci0P2pMem0BaseLow, 
                              unsigned int pci0P2pMem0BaseHigh, 
                              unsigned int pci0P2pMem0Size);
void gtPci0MapP2pMem1SpaceDAC(unsigned int pci0P2pMem1BaseLow, 
                              unsigned int pci0P2pMem1BaseHigh, 
                              unsigned int pci0P2pMem1Size);
#ifdef INCLUDE_CPU_MAPPING 
void gtPci0MapCPUspaceDAC(unsigned int pci0CpuBaseLow,
                          unsigned int pci0CpuBaseHigh,
                          unsigned int pci0CpuSize);
#endif /* INCLUDE_CPU_MAPPING */


bool  gtPci0SetRegionFeatures(PCI_ACCESS_REGIONS region,unsigned int features, 
                              unsigned int baseAddress,unsigned int regionSize);
bool pci0SetRegionFeaturesDAC(PCI_ACCESS_REGIONS region,
                                unsigned int features, 
                                unsigned int baseAddressLow,
                                unsigned int baseAddressHigh,
                                unsigned int regionSize);
void  gtPci0DisableAccessRegion(PCI_ACCESS_REGIONS region);

/* PCI arbiter */

void gtPci0ArbiterEnable(void);
void gtPci0ArbiterDisable(void);
void gtPci0ParkingDisable(PCI_AGENT_PARK internalAgent,
                          PCI_AGENT_PARK externalAgent0,
                          PCI_AGENT_PARK externalAgent1,
                          PCI_AGENT_PARK externalAgent2,
                          PCI_AGENT_PARK externalAgent3,
                          PCI_AGENT_PARK externalAgent4,
                          PCI_AGENT_PARK externalAgent5);
bool gtPci0EnableBrokenAgentDetection(unsigned char brokenValue);
void gtPci0DisableBrokenAgentDetection(void);

/* PCI-to-PCI (P2P) */

void gtPci0P2PConfig(unsigned int SecondBusLow,unsigned int SecondBusHigh,
                     unsigned int busNum,unsigned int devNum);

#endif /* __INCgtPcih */
