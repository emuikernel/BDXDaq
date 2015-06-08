/******************************************************************************
*******************************************************************************
Sergey  ***********************************************************************
*******************************************************************************
*******************************************************************************

muxDevStart failed for device entry 0!

example:

  usrMem2MemDmaCopy(0,0x10000000,0x11000000,100)

*/


#define UINT32 unsigned int

#define NO_BIT          0x00000000
#define BIT0            0x00000001
#define BIT1            0x00000002
#define BIT2            0x00000004
#define BIT3            0x00000008
#define BIT4            0x00000010
#define BIT5            0x00000020
#define BIT6            0x00000040
#define BIT7            0x00000080
#define BIT8            0x00000100
#define BIT9            0x00000200
#define BIT10           0x00000400
#define BIT11           0x00000800
#define BIT12           0x00001000
#define BIT13           0x00002000
#define BIT14           0x00004000
#define BIT15           0x00008000
#define BIT16           0x00010000
#define BIT17           0x00020000
#define BIT18           0x00040000
#define BIT19           0x00080000
#define BIT20           0x00100000
#define BIT21           0x00200000
#define BIT22           0x00400000
#define BIT23           0x00800000
#define BIT24           0x01000000
#define BIT25           0x02000000
#define BIT26           0x04000000
#define BIT27           0x08000000
#define BIT28           0x10000000
#define BIT29           0x20000000
#define BIT30           0x40000000
#define BIT31           0x80000000

#define DMA_BLOCK_TRANSFER_MODE		            BIT11
#define DMA_SOURCE_ADDR_IN_PCI0	                    BIT21

#define DMA_DTL_32BYTES			            (BIT6|BIT7)

#define DMA_SRC_DRAM    0x0
#define DMA_SRC_PCI     0x2

#define DMA_TGT_DRAM    0x0
#define DMA_TGT_PCI     0x2





typedef enum _dmaEngine
{
  DMA_ENG_0,
  DMA_ENG_1,
  DMA_ENG_2,
  DMA_ENG_3
} DMA_ENGINE;

typedef enum _dmaStatus
{
  DMA_CHANNEL_IDLE,
  DMA_CHANNEL_BUSY,
  DMA_NO_SUCH_CHANNEL,
  DMA_OK,
  DMA_GENERAL_ERROR
} DMA_STATUS;

typedef struct _dmaRecord
{
  unsigned int ByteCnt;
  unsigned int SrcAdd;
  unsigned int DestAdd;
  unsigned int NextRecPtr;
} DMA_RECORD;

DMA_STATUS gtDmaStart(unsigned int chan,
               unsigned int sourceAddr, unsigned int destAddr, unsigned int numOfBytes,
               unsigned int command);



void
usrDmaInit()
{
  sysOutLong(0xf1000a00 , 0x00000000);
  sysOutLong(0xf1000a04 , 0x00000000);

/* Sergey: set WB cache coherency for CS[1], otherwise DMA is not copying whole block */
/* must understand it better !!!!!!!!!!!!!!! */
  sysOutLong(0xf1000a08 , 0x00002e00); /* MEM BAR */

  sysOutLong(0xf1000a0c , 0x1fff0000); /* MEM size */
  sysOutLong(0xf1000a10 , 0x00001903); /* PCI BAR */
  sysOutLong(0xf1000a14 , 0x1fff0000); /* PCI size */
  sysOutLong(0xf1000a18 , 0x00000000);
  sysOutLong(0xf1000a1c , 0x00000000);

  sysOutLong(0xf1000840 , 0x80080ac3); /* chan 0 */
  sysOutLong(0xf1000844 , 0x80080ac3); /* chan 1 */
  sysOutLong(0xf1000848 , 0x80080ac3); /* chan 2 */
  sysOutLong(0xf100084c , 0x80080ac3); /* chan 3 */

  sysOutLong(0xf1000a80 , 0x00000000);
}


void
myfill()
{
  int i;
  for(i=0; i<100; i++) sysOutLong((0x10000000+(i*4)) , 0x00012345);
  for(i=0; i<100; i++) sysOutLong((0xd0000000+(i*4)) , 0x54321000);
}




/* Sergey: redefine bits 21-24 in 0xf1000840: BAR1 will be used
   to address memory and BAR2 to address PCI (assume PCI0);
   we have local memory and PCI-mapped host memory both at
   address 0, so this is the way to distinguish between them */

#define DMA_SOURCE_ADDR_IN_MEM	                    BIT21
#define DMA_SOURCE_ADDR_IN_PCI	                    BIT22
#define DMA_DEST_ADDR_IN_MEM		                BIT23
#define DMA_DEST_ADDR_IN_PCI		                BIT24

/*
-------------usrDmaInit---------< called from usrConfig.c
myfill
usrMem2MemDmaStart(0,0x10000000,0x11000000,104)
usrPci2MemDmaStart(0,0x10000000,0x11000000,108)
*/

int
usrMem2MemDmaStart(UINT32 chan,UINT32 *srcAddr, UINT32 *dstAddr, UINT32 nbytes)
{
  unsigned int command = 0;

  command = command | DMA_SOURCE_ADDR_IN_MEM;
  command = command | DMA_DEST_ADDR_IN_MEM;

  gtDmaStart(chan,(UINT32)srcAddr,(UINT32)dstAddr,nbytes,command);

  return(0);
}

int
usrPci2MemDmaStart(UINT32 chan,UINT32 *srcAddr, UINT32 *dstAddr, UINT32 nbytes)
{
  unsigned int command = 0;

  command = command | DMA_SOURCE_ADDR_IN_PCI;
  command = command | DMA_DEST_ADDR_IN_MEM;

  gtDmaStart(chan,(UINT32)srcAddr,(UINT32)dstAddr,nbytes,command);

  return(0);
}

int
usrMem2PciDmaStart(UINT32 chan,UINT32 *srcAddr, UINT32 *dstAddr, UINT32 nbytes)
{
  unsigned int command = 0;

  command = command | DMA_SOURCE_ADDR_IN_MEM;
  command = command | DMA_DEST_ADDR_IN_PCI;

  gtDmaStart(chan,(UINT32)srcAddr,(UINT32)dstAddr,nbytes,command);

  return(0);
}

int
usrPci2PciDmaStart(UINT32 chan,UINT32 *srcAddr, UINT32 *dstAddr, UINT32 nbytes)
{
  unsigned int command = 0;

  command = command | DMA_SOURCE_ADDR_IN_PCI;
  command = command | DMA_DEST_ADDR_IN_PCI;

  gtDmaStart(chan,(UINT32)srcAddr,(UINT32)dstAddr,nbytes,command);

  return(0);
}


/* TODO !!!!!!!!!!!!!!!!!!!!!!!!!! */
int
usrDmaDone(UINT32 chan)
{
  int status, res;
  /*int dmaStatus;*/                /* OK when DMA completes successfully */
  /*unsigned int dmaErrorCode;*/    /* Error code when dmaStatus is ERROR */
  /*
  dmaStatus = dmaChanCntl->dmaStatus;
  dmaErrorCode = dmaChanCntl->errSel;

  if(dmaStatus == ERROR && dmaErrorCode == 0) status = 1;
  else                                        status = dmaStatus;
  */

  if(gtDmaIsChannelActive((DMA_ENGINE)chan)) status = 1; /* busy */ 
  else                                       status = 0; /* done */

  return(status);
}


/* temporary: simple syncronization mechanizm */

/* PMC bridge general purpose register access */
/* always swap !!! */


/*from pciDevConfig.c (BAD ! must be in config.h for example so everybody
can use it) */
#define PCI_UNUSED_DEVICE_NO                     0x1F

int
usrReadGPR()
{
  UINT32 data, offset;

  offset = 0x24;
  /*pciConfigInLong(0,0,0,offset,&data);*/
  pciConfigInLong(0,PCI_UNUSED_DEVICE_NO,0,offset,&data);

  /*printf("usrReadGPR: data=0x%08x\n",data);*/

  return(data);
}

int
usrWriteGPR(UINT32 data)
{
  UINT32 offset;

  offset = 0x24;
  /*pciConfigOutLong(0,0,0,offset,data);*/
  pciConfigOutLong(0,PCI_UNUSED_DEVICE_NO,0,offset,data);
  /*
  printf("usrWriteGPR: data=0x%08x\n",data);
  */
  return(0);
}

