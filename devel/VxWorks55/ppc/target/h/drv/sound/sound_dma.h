/* sound_dma.h - i8237Dma helper functions */

/* Copyright 1999-2002 Wind River Systems, Inc. */

/*
modification history
--------------------
01b,26apr02,dat  Adding cplusplus protection, SPR 75017
01a,03oct99,spm  created.
*/

#ifndef _INCsound_dmah
#define _INCsound_dmah

#include "drv/dma/i8237Dma.h"
#include "cacheLib.h"
#include "sysLib.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * DMA transfers are limited to the lower 16MB of _physical_ memory.  
 *
 */
#define MAX_DMA_ADDRESS 0xFFFFFF

/* DMA page registers
 */
#define DMA_PAGE_0              0x87
#define DMA_PAGE_1              0x83
#define DMA_PAGE_2              0x81
#define DMA_PAGE_3              0x82
#define DMA_PAGE_5              0x8B
#define DMA_PAGE_6              0x89
#define DMA_PAGE_7              0x8A

/* enable/disable a DMA channel
 */
static __inline__ void enable_dma (unsigned int dmanr)
{
  if (dmanr <= 3)
    sysOutByte (DMA1_MASK_SINGLE, dmanr);
  else
    sysOutByte (DMA2_MASK_SINGLE, dmanr & 3);
}

static __inline__ void disable_dma (unsigned int dmanr)
{
  if (dmanr <= 3)
    sysOutByte (DMA1_MASK_SINGLE, dmanr | 4);
  else
    sysOutByte (DMA2_MASK_SINGLE, (dmanr & 3) | 4);
}


/* Clear the 'DMA Pointer Flip Flop'.
 * Write 0 for LSB/MSB, 1 for MSB/LSB access.
 */
static __inline__ void clear_dma_ff (unsigned int dmanr)
{
  if (dmanr <= 3)
    sysOutByte (DMA1_CLEAR_FF, 0);
  else
    sysOutByte (DMA2_CLEAR_FF, 0);
}


/* set mode for a DMA channel
 */
static __inline__ void set_dma_mode (unsigned int dmanr, char mode)
{
  if (dmanr <= 3)
    sysOutByte (DMA1_MODE, mode | dmanr);
  else
    sysOutByte (DMA2_MODE, mode | (dmanr&3));
}


/* Set the page register bits of the transfer address.
 */
static __inline__ void set_dma_page (unsigned int dmanr, char pagenr)
{
  switch (dmanr)
  {
    case 0:
      sysOutByte (DMA_PAGE_0, pagenr);
      break;
    case 1:
      sysOutByte (DMA_PAGE_1, pagenr);
      break;
    case 2:
      sysOutByte (DMA_PAGE_2, pagenr);
      break;
    case 3:
      sysOutByte (DMA_PAGE_3, pagenr);
      break;
    case 5:
      sysOutByte (DMA_PAGE_5, pagenr);
      break;
    case 6:
      sysOutByte (DMA_PAGE_6, pagenr);
      break;
    case 7:
      sysOutByte (DMA_PAGE_7, pagenr);
      break;
  }
}


/* Set transfer address & page bits for a DMA channel.
 */
static __inline__ void set_dma_addr (unsigned int dmanr, unsigned int a)
{
  set_dma_page (dmanr, a>>16);

  if (dmanr <= 3)
  {
    sysOutByte (((dmanr&3)<<1) + DMA1_BASE, a & 0xff);
    sysOutByte (((dmanr&3)<<1) + DMA1_BASE, (a>>8) & 0xff);
  }
  else
  {
    sysOutByte (((dmanr&3)<<2) + DMA2_BASE, (a>>1) & 0xff);
    sysOutByte (((dmanr&3)<<2) + DMA2_BASE, (a>>9) & 0xff);
  }
}


/* Set transfer size (max 64k for DMA1..3, 128k for DMA5..7) for
 * a DMA channel.
 */
static __inline__ void set_dma_count(unsigned int dmanr, unsigned int count)
{
  count--;
  if (dmanr <= 3)
  {
    sysOutByte (((dmanr&3)<<1) + 1 + DMA1_BASE, count & 0xff);
    sysOutByte (((dmanr&3)<<1) + 1 + DMA1_BASE, (count>>8) & 0xff);
  }
  else
  {
    sysOutByte (((dmanr&3)<<2) + 2 + DMA2_BASE, (count>>1) & 0xff);
    sysOutByte (((dmanr&3)<<2) + 2 + DMA2_BASE, (count>>9) & 0xff);
  }
}

static __inline__ int get_dma_residue(unsigned int dmanr)
{
  unsigned int io_port =
    (dmanr<=3) ? ((dmanr&3)<<1) + 1 + DMA1_BASE
               : ((dmanr&3)<<2) + 2 + DMA2_BASE;
  /* using short to get 16-bit wrap around */
  unsigned short count;

  count = 1 + (unsigned char)sysInByte (io_port);
  count += (unsigned char)sysInByte (io_port) << 8;

  return (dmanr<=3) ? count : (count<<1);
}

/* Don't use the vxWorks dmaSetup
 */
#define dmaSetup __inline__dmaSetup

/* Instead, use this one.
 */
static __inline__ void dmaSetup (int mode, char *buf, int size, int dma)
{
  disable_dma (dma);
  sysDelay ();
  clear_dma_ff (dma);
  sysDelay ();
  set_dma_mode (dma, mode);
  set_dma_addr (dma, (int)buf);
  set_dma_count (dma, size);
  sysDelay ();
  enable_dma (dma);
}

static __inline__ int dmaResidue (int dma)
{
  int residue;

  disable_dma (dma);
  clear_dma_ff (dma);
  residue = get_dma_residue (dma);
  enable_dma (dma);

  return residue;
}

#ifdef __cplusplus
}
#endif

#endif /* __INCsound_dmah */
