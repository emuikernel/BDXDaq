/* universeDma.h - Tundra Universe chip DMA Interface Include File */

/* Copyright 1984-1997 Wind River Systems, Inc. */
/* Copyright 1996-1997 Motorola, Inc. */

/* 
modification history
--------------------
01a,10mar97,rcp   written.
*/


/*  Variable structure which can be used for Call Back funtion if an
    error occurred.  These values will be set to the final DMA Reg.
    if an error occurs in the sysVmeDmaInt ISR  */

struct stat_struct  {
   UINT32  finalStatus;  /* Final Status of DMA */
   UCHAR   *finalLadr,   /* Final Local Address if error */
           *finalVadr;   /* Final VMEbus Address if error */
   UINT32  finalCount;   /* Final Byte Count if error */
   };


/* Structure for Link-List operation */

struct univDma_ll  {
  volatile UINT32 dctl;
  volatile UINT32 dtbc;
  volatile UINT32 dla;
  volatile UINT32 reserve1;
  volatile UINT32 dva;
  volatile UINT32 reserve2;
  volatile UINT32 dcpp;
  volatile UINT32 reserve3;
};
#define UNIV_DMA_MAX_LL  20


/* universeDma Library function Prototypes */
void sysVmeDmaShow (void);    
void sysVmeDmaInt(void);
STATUS sysVmeDmaDone(int, int);
STATUS sysVmeDmaInit(int);
STATUS sysVmeDmaGet(UINT32, UINT32 *);
STATUS sysVmeDmaSet(UINT32, UINT32);
STATUS sysVmeDmaCopy(char *, char *, int, BOOL);
STATUS async_sysVmeDmaCopy(char *, char *, int, BOOL);
STATUS sysVmeDmaSend(UINT32, UINT32, int, BOOL);

/* Define UNIVERSE Chip base address and the PCI to DRAM Base address 
   This depends on the Architecture Configuration */

#ifndef MV5100
#define VME_DEV_SPACE       0x50000      /* Universe offset from Base PCI Memory Space */
#else
#define VME_DEV_SPACE       0x00000      /* Offset changed for MVME5100 BSP */
#endif

#ifndef EXTENDED_VME
#define CPU_PCI_MEM_ADRS    0xc1000000   /* PCI Memory Space as Seen from CPU */
#define PCI2DRAM_BASE_ADRS  0x80000000   /* CPU DRAM Base as seen from PCI Space */
#else
#define CPU_PCI_MEM_ADRS    0xfd000000
#define PCI2DRAM_BASE_ADRS  0x00000000
#endif

#define UNIVERSE_BASE_ADRS  (CPU_PCI_MEM_ADRS + VME_DEV_SPACE)


/* Local defines */
#define DGCS_ERROR_MASK   (0x00006e00)   /*  Mask all but status bits */
#define DGCS_VON_CLEAR    (0xff0fffff)   /*  Clear all but VON bits */
#define DGCS_VOFF_CLEAR   (0xfff0ffff)   /*  Clear all but VOFF bits */
#define DCTL_L2V_CLEAR    (0x7fffffff)   /*  Clear all but VDW bits */
#define DCTL_VDW_CLEAR    (0xff3fffff)   /*  Clear all but VDW bits */
#define DCTL_VAS_CLEAR    (0xfff8ffff)   /*  Clear all but VAS bits */
#define DCTL_SUPER_CLEAR  (0xffffcfff)   /*  Clear all but SUPER bits */
#define DCTL_PGM_CLEAR    (0xffff3fff)   /*  Clear all but PGM bits */
#define DCTL_VCT_CLEAR    (0xfffffeff)   /*  Clear all but VDW bits */
#define DCTL_64EN_CLEAR   (0xffffff7f)   /*  Clear all but VDW bits */

/* Enable all DMA interrupts except halt and stop.  */
#define DMA_INTERRUPTS   DGCS_INT_STOP | DGCS_INT_HALT |DGCS_INT_DONE |\
        DGCS_INT_LERR | DGCS_INT_VERR | DGCS_INT_P_ERR 

#define DGCS_RESET_STAT DGCS_STOP | DGCS_HALT | DGCS_DONE |\
        DGCS_LERR | DGCS_VERR | DGCS_P_ERR

/* Create some define values for the error codes returned from
   the DMA system calls  */

#define DMA_SIZE_ERROR   1   /*  Size parameter was out of range */
#define DMA_LADR_ERROR   2   /*  PCI Addr. not even 3 bit boundary */
#define DMA_VADR_ERROR   3   /*  VME Addr. not even 3 bit boundary */
#define DMA_BUSY         4   /*  DMA Channel busy */

/* Parameters for the Set and Get Functions */
#define FRC_VME_DMA_LOCAL_MODE  1
#define FRC_VME_DMA_LOCAL_ACAP  2
#define FRC_VME_DMA_LOCAL_DCAP  3
#define FRC_VME_DMA_LOCAL_BSIZE 4   /* max LOCAL Block size */
#define FRC_VME_DMA_VME_MODE    10
#define FRC_VME_DMA_VME_ACAP    11  /* address mode on vmebus */
#define FRC_VME_DMA_VME_DCAP    12  /* data size on vmebus */
#define FRC_VME_DMA_VME_BSIZE   13  /* max VME block size */
#define FRC_VME_DMA_VME_DELAY   14  /* delay between VME blocks */
#define FRC_VME_DMA_VME_UCAP    15  /* User/Supervisor capabilities */
#define FRC_VME_DMA_VME_PCAP    16  /* Program/Data capabilities */

#define FRC_VME_DMA_DMA_PARAM   20
#define FRC_VME_DMA_VIC_BURST   30
#define FRC_VME_DMA_VIC_INTLV   31
#define FRC_VME_DMA_LOCAL_DCTL  32  /* Entire value of the DCTL Reg */
#define FRC_VME_DMA_LOCAL_DGCS  33  /* Entire value of the DGCS Reg */

/* address capabilities */
#define ACAP_A16        0
#define ACAP_A24        1
#define ACAP_A32        2

/* data capabilities */
#define DCAP_D8         1
#define DCAP_D16        2
#define DCAP_D32        3
#define DCAP_BLT        4
#define DCAP_MBLT       5
#define DCAP_D64        6

/* user/supervisor capabilities */
#define UCAP_USR        0
#define UCAP_SUP        1

/* program space/data space capabilities */
#define PCAP_DATA       0
#define PCAP_PGM        1

/* Values for the FRC_VME_DMA_VME_BSIZE (max VME block size) */

#define VME_BSIZE_DONE	0   /*  Transfer until done */
#define VME_BSIZE_256	1   /*  Transfer Size 256 bytes */
#define VME_BSIZE_512	2   /*  Transfer Size 512 bytes */
#define VME_BSIZE_1024	3   /*  Transfer Size 1024 bytes */
#define VME_BSIZE_2048	4   /*  Transfer Size 2048 bytes */
#define VME_BSIZE_4096	5   /*  Transfer Size 4096 bytes */
#define VME_BSIZE_8192	6   /*  Transfer Size 8192 bytes */
#define VME_BSIZE_16834	7   /*  Transfer Size 16834 bytes */
   
/* Values for the FRC_VME_DMA_VME_DELAY (delay between VME blocks) */
#define VME_DELAY_0	0   /*  0uSec wait until next Transfer */
#define VME_DELAY_16	1   /*  16uSec wait until next Transfer */
#define VME_DELAY_32	2   /*  32uSec wait until next Transfer */
#define VME_DELAY_64	3   /*  64uSec wait until next Transfer */
#define VME_DELAY_128	4   /*  128uSec wait until next Transfer */
#define VME_DELAY_256	5   /*  256uSec wait until next Transfer */
#define VME_DELAY_512	6   /*  512uSec wait until next Transfer */
#define VME_DELAY_1024	7   /*  1024uSec wait until next Transfer */
