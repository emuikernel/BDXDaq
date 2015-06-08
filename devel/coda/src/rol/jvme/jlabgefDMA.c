/*----------------------------------------------------------------------------*
 *  Copyright (c) 2009,2010   Southeastern Universities Research Association, *
 *                            Thomas Jefferson National Accelerator Facility  *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 *    Author:  Bryan Moffit                                                   *
 *             moffit@jlab.org                   Jefferson Lab, MS-12B3       *
 *             Phone: (757) 269-5660             12000 Jefferson Ave.         *
 *             Fax:   (757) 269-5800             Newport News, VA 23606       *
 *                                                                            *
 *----------------------------------------------------------------------------*
 *
 * Description:
 *     JLab DMA routines to compliment the GEFanuc API
 *
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "jvme.h"
#include "jlabgef.h"
#include "jlabgefDMA.h"
#include "gef/gefcmn_vme_tempe.h"
#include "tsi148.h"
#include "dmaPList.h"

/* Timers for DMA latency measurements */
#define TIMERS
#ifdef TIMERS
unsigned long long int dma_timer[10];
#endif /* TIMERS */

/* Some DMA Global Variables */
GEF_VME_DMA_HDL dmaLL_hdl;  /* GEF HDL for DMA Linked List buffer */
#define TSI148_DMA_MAX_LL 21
volatile tsi148DmaDescriptor_t dmaDescSample; /* Sample descriptor - contains preset attributes */
volatile tsi148DmaDescriptor_t *dmaDescList;  /* Pointer to Linked-List descriptors */
GEF_UINT32 dmaLL_totalwords = 0;

extern pthread_mutex_t tsi_mutex;

int
jlabgefDmaConfig
(
 unsigned int addrType, 
 unsigned int dataType, 
 unsigned int sstMode
 )
{
  
  /* Some default attributes */
  dmaDescSample.dsat  = TSI148_LCSR_DDAT_SUP; /* Supervisory Mode */
  dmaDescSample.dsat |= TSI148_LCSR_DDAT_TYP_VME; /* VME Source */

  switch(addrType)
    {
    case 0: /* A16 */
      dmaDescSample.dsat |= TSI148_LCSR_DSAT_AMODE_16;
      break;
    case 1: /* A24 */
      dmaDescSample.dsat |= TSI148_LCSR_DSAT_AMODE_24;
      break;
    case 2: /* A32 */
      dmaDescSample.dsat |= TSI148_LCSR_DSAT_AMODE_32;
      break;
    default:
      printf("Address mode addrType=%d is not supported\n",addrType);
      return ERROR;
    }

  switch(dataType)
    {
    case 0: /* D16 - SCT */
      dmaDescSample.dsat |= TSI148_LCSR_DDAT_DBW_16;
      dmaDescSample.dsat |= TSI148_LCSR_DDAT_TM_SCT;
      break;
    case 1: /* D32 - SCT */
      dmaDescSample.dsat |= TSI148_LCSR_DDAT_DBW_32;
      dmaDescSample.dsat |= TSI148_LCSR_DDAT_TM_SCT;
      break;
    case 2: /* BLK32 */
      dmaDescSample.dsat |= TSI148_LCSR_DDAT_DBW_32;
      dmaDescSample.dsat |= TSI148_LCSR_DDAT_TM_BLT;
      break;
    case 3: /* MBLK */
      /* 64bit Data Width by default, set 32bit for misaligned SCT transfers */
      dmaDescSample.dsat |= TSI148_LCSR_DDAT_DBW_32;
      dmaDescSample.dsat |= TSI148_LCSR_DDAT_TM_MBLT;
      break;
    case 4: /* 2eVME */
      /* 64bit Data Width by default, set 32bit for misaligned SCT transfers */
      dmaDescSample.dsat |= TSI148_LCSR_DDAT_DBW_32;
      dmaDescSample.dsat |= TSI148_LCSR_DDAT_TM_2eVME;
      break;
    case 5: /* 2eSST */
      /* 64bit Data Width by default, set 32bit for misaligned SCT transfers */
      dmaDescSample.dsat |= TSI148_LCSR_DDAT_DBW_32;
      dmaDescSample.dsat |= TSI148_LCSR_DDAT_TM_2eSST;
      switch(sstMode)
	{
	case 0: /* SST160 */
	  dmaDescSample.dsat |= TSI148_LCSR_DSAT_2eSSTM_160;
	  break;
	case 1: /* SST267 */
	  dmaDescSample.dsat |= TSI148_LCSR_DSAT_2eSSTM_267;
	  break;
	case 2: /* SST320 */
	  dmaDescSample.dsat |= TSI148_LCSR_DSAT_2eSSTM_320;
	  break;
	default: /* SST160 */
	  dmaDescSample.dsat |= TSI148_LCSR_DSAT_2eSSTM_160;
	}
      break;
    default:
      printf("Data type dataType=%d is not supported\n",dataType);
      return ERROR;
    }

  return OK;
}

int
jlabgefDmaSend
(
 unsigned int locAdrs,   /* Userspace Local Address (NOT Pci Bus) */
 unsigned int vmeAdrs,   /* VME Bus local Address */
 int size       /* Size of the DMA Transfer in bytes*/
 )
{
  int offset;
  int tmp_dctl=0;
  int channel=0; /* hard-coded for now */
  int nbytes=0;

#ifdef TIMERS
  dma_timer[0] = rdtsc();
#endif /* TIMERS */

  if(!the_event) 
    {
      printf("jlabgefDmaSend: ERROR:  the_event pointer is invalid!\n");
      return ERROR;
    }

  if(!the_event->physMemBase)
    {
      printf("jlabgefDmaSend: ERROR: DMA Physical Memory has an invalid base address (0x%08x)",
	     (unsigned int)the_event->physMemBase);
      return ERROR;
    }

  /* Clear any previous exception */
  jlabgefClearException(1);

  /* Local addresses (in Userspace) need to be translated to the physical memory */
  /* Here's the offset between current buffer position and the event head */
  offset = (int)locAdrs - (int)the_event->partBaseAdr;
  dmaDescSample.ddal = (int)the_event->physMemBase + offset;

#ifdef DEBUG
  printf("locAdrs     = 0x%08x   partBaseAdr = 0x%08x\n",
	 locAdrs, the_event->partBaseAdr);
  printf("physMemBase = 0x%08x   offset      = 0x%08x\n\n",
	 the_event->physMemBase,offset);
#endif

  /* Source (VME) address */
  dmaDescSample.dsal = vmeAdrs;

  /* Calculate the bytes left in this buffer */
  nbytes = the_event->part->size - sizeof(DMANODE);
  nbytes = nbytes - (((int)dma_dabufp - (int)&(the_event->length))<<2);

  /* Make sure nbytes is realistic */
  if(nbytes<0)
    {
      printf("%s: ERROR: Space left in buffer is less than zero (%d). Quitting\n",
	     __FUNCTION__,nbytes);
      return ERROR;

    }

  /* Check the specified "size" vs. size left in buffer */
  /* ... if size==0, just use the space left in the buffer */
  if(size>nbytes) 
    {
      printf("jlabgefDmaSend: WARN: Specified number of DMA bytes (%d) is greater than \n",
	     size);
      printf("\tthe space left in the buffer (%d).  Using %d\n",nbytes,nbytes);
    }
  else if( (size !=0) && (size<=nbytes) )
    {
      nbytes = size;
    }

  dmaDescSample.dcnt = nbytes;

  /* Some defaults hardcoded for the DMA Control Register */
  tmp_dctl  = 0x00830000; /* Set Direct mode, VFAR and PFAR by default */
  tmp_dctl |= (TSI148_LCSR_DCTL_VBKS_2048 |
			 TSI148_LCSR_DCTL_PBKS_2048); /* 2048 VME/PCI Block Size */
  tmp_dctl |= (TSI148_LCSR_DCTL_VBOT_0 |
			 TSI148_LCSR_DCTL_PBOT_0); /* 0us VME/PCI Back-off */
  
#ifdef TIMERS
  dma_timer[1] = rdtsc();
#endif /* TIMERS */

  LOCK_TSI;
  pTempe->lcsr.dma[channel].dsal = LSWAP(dmaDescSample.dsal);
  /*   pTempe->lcsr.dma[channel].dsau = 0x0; */
  pTempe->lcsr.dma[channel].ddal = LSWAP(dmaDescSample.ddal);
  /*   pTempe->lcsr.dma[channel].ddau = 0x0; */
  pTempe->lcsr.dma[channel].dsat = LSWAP(dmaDescSample.dsat);
  /*   pTempe->lcsr.dma[channel].ddat = 0x0; */
  pTempe->lcsr.dma[channel].dcnt = LSWAP(dmaDescSample.dcnt);
  /*   pTempe->lcsr.dma[channel].ddbs = 0x0; */
  
  pTempe->lcsr.dma[channel].dctl = LSWAP(tmp_dctl);

  /* GO! */
  tmp_dctl |=  TSI148_LCSR_DCTL_DGO;
  pTempe->lcsr.dma[channel].dctl = LSWAP(tmp_dctl);

  UNLOCK_TSI;

#ifdef TIMERS
  dma_timer[2] = rdtsc();
#endif /* TIMERS */

  return OK;
}

/******************************************************************************
 * 
 * jlabgefDmaDone - Wait until a DMA finishes or times-out
 *
 * RETURNS: if successful, number of bytes transferred
 *          otherwise ERROR
 *
 */

int
jlabgefDmaDone
()
{
  unsigned int val=0,ii=0;
  int channel=0; 
  unsigned int timeout=100;
  unsigned int vmeExceptionAttribute=0;
  unsigned int veal;
  unsigned int dcnt;
  int status=OK;

#ifdef TIMERS
  dma_timer[3] = rdtsc();
#endif /* TIMERS */

  LOCK_TSI;

  /* Sergey: 0x1e000000 means we are waiting for VMEbus error, Abort, Pause or Done */
  val = jlabgefReadRegister(TEMPE_DSTA(channel));
  while( ((val&0x1e000000)==0) && (ii<timeout) )
  {
    val = jlabgefReadRegister(TEMPE_DSTA(channel));
    ii++;
  }

  if(ii>=timeout) 
  {
    printf("jlabgefDmaDone: DMA timed-out. DMA Status Register = 0x%08x\n",val);
    UNLOCK_TSI;
    jlabgefReadDMARegs();
    LOCK_TSI;
    status = ERROR;
  }

#ifdef TIMERS
  dma_timer[4] = rdtsc();
#endif /* TIMERS */

  /* check the VME Exception Attribute... 
     clear it if the DMA ended on BERR or 2eST (2e Slave Termination) */
  vmeExceptionAttribute = jlabgefReadRegister(TEMPE_VEAT);
  if( (vmeExceptionAttribute & TEMPE_VEAT_VES) && 
      ( (vmeExceptionAttribute & TEMPE_VEAT_BERR) ||
        (vmeExceptionAttribute & TEMPE_VEAT_2eST) )
      )
  {
    pTempe->lcsr.veat = LSWAP(TEMPE_VEAT_VESCL);

    if(status != ERROR)
	{
	  /* Read where the BERR occurred */
	  veal = jlabgefReadRegister(TEMPE_VEAL);
	  /* Return value is the difference between VEAL and the Starting Address */
	  status = veal - dmaDescSample.dsal;
	  if(status<0)
	  {
	    printf("jlabgefDmaDone: ERROR: VME Exception Address < DMA Source Address (0x%08x < 0x%08x)\n",
	       veal,dmaDescSample.dsal);
	    status = ERROR;
	  }
	}
  }
  else
  {
    /* DMA ended on DMA Count (No BERR), return original byte count */
    if(status != ERROR)
    {
	  status = dmaDescSample.dcnt;
	  /* check and make sure 0 bytes are left in the DMA Count register */
      dcnt = jlabgefReadRegister(TEMPE_DCNT(channel));
      if(dcnt != 0)
      {
	    printf("%s: ERROR: DMA terminated on master byte count,",__FUNCTION__);
	    printf("    however DCNT (%d) != 0 \n", dcnt);
	  }
    }
  }

  /* If we started a linked-list transaction, dmaLL_totalwords should be non-zero */
  if(dmaLL_totalwords>0)
  {
    status = dmaLL_totalwords<<2;
    dmaLL_totalwords=0;
  }

  UNLOCK_TSI;
#ifdef TIMERS
  dma_timer[5] = rdtsc();
#endif /* TIMERS */

  return status;
  
}

/******************************************************************************
 * 
 * jlabgefSetupDMALLBuf - Set up the system buffer (mapped to userspace)
 *                        necesary for DMA Linked-List Mode.
 *                        DMA registers for each DMA transaction will
 *                        be stored in this buffer.
 *
 * RETURNS: Status from gefVmeAllocDmaBuf
 *
 */

int
jlabgefDmaAllocLLBuffer
()
{
  GEF_STATUS status;
  void *mapPtr;
  int listsize = TSI148_DMA_MAX_LL*sizeof(tsi148DmaDescriptor_t);
  
  /* Allocate some system memory - mapped to userspace*/
  status = gefVmeAllocDmaBuf (vmeHdl, listsize, &dmaLL_hdl, (GEF_MAP_PTR *) &mapPtr);
  if (status != GEF_SUCCESS)
    {
      printf("jlabgefSetupDMALLBuf: ERROR: gefVmeAllocDmaBuf returned %x\n",status);
      return ERROR;
    }
  
  /* Impose the DmaDescriptor structure on it, TSI148_DMA_MAX_LL times */
  dmaDescList = (tsi148DmaDescriptor_t *)mapPtr;

  return OK;
}

/******************************************************************************
 * 
 * jlabgefDmaFreeLLBuf - Free up the system buffer allocated by 
 *                       jlabgefSetupDMALLBuf
 *
 * RETURNS: OK if Buffer was successfully freed,
 *          ERROR otherwise.
 *
 */

int
jlabgefDmaFreeLLBuffer
()
{
  GEF_STATUS status;
  int retval;

  status = gefVmeFreeDmaBuf(dmaLL_hdl);
  if (status != GEF_SUCCESS)
    {
      printf("jlabgefFreeDMALLBuf: ERROR: gefVmeFreeDmaBuf returned %x\n",status);
      retval = ERROR;
    }
  
  return OK;
}

/**********************************************************************
 *
 * jlabgefDmaSetupLL - Setup the DMA Linked List
 *
 *  INPUTS:
 *    locAddrBase    - Userspace Local Address Base to send DMA
 *    vmeAddr        - Array of VME addresses
 *    dmaSize        - Array of sizes of each DMA, in bytes
 *    numt           - Number of DMA (size of arrays).
 *
 *  RETURNS: 0 if successful, -1 otherwise
 */

void
jlabgefDmaSetupLL
(
 unsigned int locAddrBase,
 unsigned int *vmeAddr,
 unsigned int *dmaSize,
 unsigned int numt
 )
{
  unsigned int localAddr[TSI148_DMA_MAX_LL];
  unsigned int dmabuf_base=0, offset=0;
  unsigned int total_words=0;
  unsigned int dmaDesc_phys_base; /* Base address of the list in system memory */
  unsigned int size_incr=0; /* "current" size of the entire DMA */
  int idma;

  /* check to see if the linked list structure array has been init'd */
  if (dmaDescList == NULL) 
    {
      printf("jlabgefDMAListSet: ERROR: dmaDescList not initialized\n");
      return;
    }

  dmaLL_totalwords=0;

  /* Get the base address (system) of the DMA Description buffer from the GEF Hdl */
  dmaDesc_phys_base = dmaHdl_to_PhysAddr(dmaLL_hdl);

  /* Calculate localAddr array values based on buffer partition base, current location
     of dma_dabufp and the dma size for each linked list member */
  offset = (int)locAddrBase - (int)the_event->partBaseAdr;
  localAddr[0] = (int)the_event->physMemBase + offset;

  total_words = dmaSize[0]>>2;

  for (idma=0;idma<numt;idma++) 
    {
      if(idma!=0) 
	{
	  localAddr[idma] = localAddr[idma-1] + dmaSize[idma-1];
	  total_words += (dmaSize[idma]>>2);
	}

      /* Set the vme (source) address */
      dmaDescList[idma].dsau = 0x0;
      dmaDescList[idma].dsal = LSWAP(vmeAddr[idma]);
      
      /* Set the local (destination) address */
      dmaDescList[idma].ddau = 0x0;
      dmaDescList[idma].ddal = LSWAP(localAddr[idma]);
      
      /* Set the byte count */
      dmaDescList[idma].dcnt = LSWAP(dmaSize[idma]);
      size_incr += dmaSize[idma];

      /* Set the address of the next descriptor */
      dmaDescList[idma].dnlau = 0x0;
      if( idma < (numt-1) ) 
	{
	  dmaDescList[idma].dnlal = 
	    LSWAP(dmaDesc_phys_base + (idma+1)*sizeof(tsi148DmaDescriptor_t));
	}
      else
	{
	  dmaDescList[idma].dnlal = LSWAP(TSI148_LCSR_DNLAL_LLA); /* Last Link Address (LLA) */
	}

      /* Still need to set the Source & Destination Attributes, 2eSST Broadcast select */
      /* These we'll assume to be 0x0 for now */
      dmaDescList[idma].ddat = 0x0;
      dmaDescList[idma].ddbs = 0x0;

      /* Get the DSAT from how was made from jlabgefDmaConfig */
      dmaDescList[idma].dsat = LSWAP(dmaDescSample.dsat);

/* #define DEBUGLL */
#ifdef DEBUGLL
      printf("dmaDescList[%d]:\n",idma);
      printf("locAddrBase = 0x%08x\n",(unsigned int)locAddrBase);
      printf("  DSAL = 0x%08x  DDAL = 0x%08x  DCNT = 0x%08x\n",
	     LSWAP(dmaDescList[idma].dsal),
	     LSWAP(dmaDescList[idma].ddal),
	     LSWAP(dmaDescList[idma].dcnt));
      printf(" DNLAL = 0x%08x  DSAT = 0x%08x\n",
	     LSWAP(dmaDescList[idma].dnlal),
	     LSWAP(dmaDescList[idma].dsat));
#endif      

    }
  dmaLL_totalwords=total_words;

}

/******************************************************************************
 * 
 * jlabgefDmaSendLL - Start a DMA Linked List mode transfer
 *
 * RETURNS: N/A
 *
 */

void
jlabgefDmaSendLL
()
{
  int channel=0; 
  unsigned int dctl = 0x0; 

  LOCK_TSI;
  /* Write the address of the first entry in the linked list to the TEMPE regs */
  pTempe->lcsr.dma[channel].dnlal = LSWAP(dmaHdl_to_PhysAddr(dmaLL_hdl));
  pTempe->lcsr.dma[channel].dnlau = 0x0;

  /* Set the CTL reg */
  dctl  = 0x00030000; /* Set Linked List mode, VFAR and PFAR by default */
  dctl |= (TSI148_LCSR_DCTL_VBKS_2048 |
			 TSI148_LCSR_DCTL_PBKS_2048); /* 2048 VME/PCI Block Size */
  dctl |= (TSI148_LCSR_DCTL_VBOT_0 |
			 TSI148_LCSR_DCTL_PBOT_0); /* 0us VME/PCI Back-off */

  pTempe->lcsr.dma[channel].dctl = LSWAP(dctl);


  /* Set the "DGO" Bit */
  dctl |= 0x02000000;

  pTempe->lcsr.dma[channel].dctl = LSWAP(dctl);
  UNLOCK_TSI;

}

/******************************************************************************
 * 
 * jlabgefReadDMARegs - Print out a bunch of dma registers from the tempe chip
 *
 * RETURNS: N/A
 *
 */

void
jlabgefReadDMARegs
()
{
  int channel=0; 

  printf("\n");

  LOCK_TSI;

  printf("control                       [dctl]  = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].dctl));
  printf("status                        [dsta]  = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].dsta));

  printf("Current source address (high) [dcsau] = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].dcsau));
  printf("Current source address (low)  [dcsal] = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].dcsal));

  printf("Current dest address (high)   [dcdau] = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].dcdau));
  printf("Current dest address (low)    [dcdal] = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].dcdal));

  printf("Current link address (high)   [dclau] = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].dclau));
  printf("Current link address (low)    [dclal] = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].dclal));

  printf("source address (high)         [dsau]  = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].dsau));
  printf("source address (low)          [dsal]  = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].dsal));

  printf("dest address (high)           [ddau]  = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].ddau));
  printf("dest address (low)            [ddal]  = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].ddal));

  printf("source attributes             [dsat]  = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].dsat));
  printf("destination attributes        [ddat]  = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].ddat));

  printf("next link address (high)      [dnlau] = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].dnlau));
  printf("next link address (low)       [dnlal] = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].dnlal));

  printf("count (byte)                  [dcnt]  = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].dcnt));
  printf("2eSST broadcast select        [ddbs]  = 0x%08x\n",LSWAP(pTempe->lcsr.dma[channel].ddbs));


  UNLOCK_TSI;

  printf("\n");
}


#include "usrvme.c"
