
/* dcrb.c */

#if defined(VXWORKS)

#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include <taskLib.h>
#include "dcrb.h"

#define SYNC()                { __asm__ volatile("eieio"); __asm__ volatile("sync"); }
#define DCRB_WRITE_REG(r, v)  { r = v; SYNC() }

#define DCRB_BASE_ADDR        0xBE0000
#define DCRB_BASE_ADDR_A32    0x08000000

DCRB_regs *pDCRB;
volatile unsigned int *pDCRBFifo;

void
DCRBSync()
{
  pDCRB->TriggerSource |= 0x800;
  SYNC()
}

void
DCRBTrig()
{
  pDCRB->TriggerSource |= 0x08;
  SYNC()
}

void
DCRBEnableTestPulse(unsigned char fce_mask, unsigned char dce_mask)
{
  pDCRB->TestPulseConfig = fce_mask | (dce_mask<<6);
}

void
DCRBUpdateDAC(unsigned short val)
{
  pDCRB->DACConfig = val;
}

void
DCRBInit(int threshold)
{
  unsigned int addr = DCRB_BASE_ADDR;
  sysBusToLocalAdrs(0x39, addr, &pDCRB);
  sysBusToLocalAdrs(0x09, DCRB_BASE_ADDR_A32, &pDCRBFifo);
        
  printf("DCRB Initializing @ 0x%08X\n", addr);
  printf("Board ID: %08X\n", pDCRB->BoardID);
  printf("Firmware Revision: %08X\n", pDCRB->FirmwareRev);        
        
  /* Setup A32 base address for event readout */
  DCRB_WRITE_REG(pDCRB->ADR32, ((DCRB_BASE_ADDR_A32>>16) & 0xFF80) | 0x0001)
        
  /* Define window start point (in ns units before trigger) */
  DCRB_WRITE_REG(pDCRB->LookBack, 4000)       /* 4us */
        
  /* Define window width (in ns units) */
  DCRB_WRITE_REG(pDCRB->WindowWidth, 4000)    /* 4us */
        
  /* Number of events per block */
  DCRB_WRITE_REG(pDCRB->BlockConfig, 1)       /* 1 event per block */
        
  /* TDC Channel dead-time in 8ns units */
  DCRB_WRITE_REG(pDCRB->TDCConfig, 4)         /* 32ns channel dead-time */
        
  /* Front-end discriminator threshold */
  DCRB_WRITE_REG(pDCRB->DACConfig, threshold) /* 2900: ~30mV threshold */

  /* Clock source         */
  DCRB_WRITE_REG(pDCRB->ClockConfig, 0)       /* use onboard */
        
  DCRB_WRITE_REG(pDCRB->ReadoutConfig, 1)     /* enable berr */

  /* Enable all TDC channels */
  DCRB_WRITE_REG(pDCRB->ChDisable[0], 0)
  DCRB_WRITE_REG(pDCRB->ChDisable[1], 0);
  DCRB_WRITE_REG(pDCRB->ChDisable[2], 0);

  /* Trigger source selection */
  /*DCRB_WRITE_REG(pDCRB->TriggerSource, 0)   /* software trigger only */
  DCRB_WRITE_REG(pDCRB->TriggerSource, 0x11)  /* High resolution trig 1 input */
        
  /* Event FIFO reset */
  DCRB_WRITE_REG(pDCRB->Reset, 1)
        
  /* Issue S/W sync */
  DCRBSync();
        
  /* Test input pulse muxes */
  /*DCRBEnableTestPulse(0x3f, 0); */          /* Front-end test */
  /*DCRBEnableTestPulse(0, 0x3f); */          /* DriftChamber Preamp test */
        
}

                

int
DCRBReadoutFIFO(unsigned int *gReadoutBuf, int *rlenbuf)
{
  volatile unsigned int *pA32Buf;
  int res, nbytes, quit = 0;

  nbytes = rlenbuf[0] << 2;
  rlenbuf[0] = 0;

  res = usrVme2MemDmaStart((UINT32)DCRB_BASE_ADDR_A32, (UINT32)gReadoutBuf, nbytes);
  if(res < 0) printf("usrVme2MemDmaStart() error: res = %d\n", res);

  res = usrVme2MemDmaDone();
  if(res < 0) printf("usrVme2MemDmaDone() error: res = %d\n", res);

  rlenbuf[0] = res >> 2;

  return(res);
}

void
DCRBStat()
{
  printf("VME slot: %d\n", pDCRB->Geo);
  printf("Clock Status: %08X\n", pDCRB->ClockConfig);
  printf("FIFO Word Count: %u\n", pDCRB->FifoWordCnt);
  printf("FIFO Event Count: %u\n", pDCRB->FifoEventCnt);
}

int
DCRBFifoEventCount()
{
  return(pDCRB->FifoEventCnt);
}

#else /* dummy version*/

void
dcrb_dummy()
{
  return;
}

#endif
