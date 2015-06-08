
/* vxWorks-like interface: avoids dmaPList and makes vxWorks-compartible API */

static UINT32 physMemBase;
static UINT32 userMemBase;
static UINT32 memSize = 0x100000; /*must be enough to take maximum event size from entire VME crate*/

/*
static UINT32 vmeUserToPhysOffset;
*/

void
usrVmeDmaInit()
{
  GEF_STATUS status;
  GEF_VME_DMA_HDL dma_hdl;
  GEF_MAP_PTR mapPtr;

  if(vmeHdl==NULL) 
  {
    printf("usrVmeDmaInit ERROR: vmeHdl undefined.\n");
    return;
  }
 /* Parameters:
 *    <GEF_VME_BUS_HDL> vmeHdl   - (I) Handle allocated by <gefVmeOpen>.
 *    GEF_UINT32 memSize         - (I) size of the buffer.
 *    <GEF_VME_DMA_HDL> *dma_hdl - (O) DMA handle of type <GEF_VME_DMA_HDL>.
 *    void **addr                - (O) User process address to allocated DMA Buffer.
 */
  status = gefVmeAllocDmaBuf (vmeHdl, memSize, &dma_hdl, &mapPtr);
  if(status != GEF_STATUS_SUCCESS) 
  {
    printf("usrVmeDmaInit ERROR: gefVmeAllocDmaBuf returned 0x%x\n",status);
    physMemBase = 0;
    userMemBase = 0;
    return;
  }

  physMemBase = (UINT32)dma_hdl->phys_addr;
  userMemBase = (UINT32)mapPtr;
  memset((char *)userMemBase,0,memSize);

  printf("usrVmeDmaInit: userMemBase = 0x%08x, physMemBase = 0x%08x, memSize = %d bytes\n",
    userMemBase, physMemBase, memSize);


  return;
}

void
usrVmeDmaMemory(UINT32 *pMemBase, UINT32 *uMemBase, UINT32 *mSize)
{
  *pMemBase = physMemBase;
  *uMemBase = userMemBase;
  *mSize = memSize;

  return;
}


static unsigned int addrType1;
static unsigned int dataType1;
static unsigned int sstMode1;

void
usrVmeDmaGetConfig(unsigned int *addrType, unsigned int *dataType, unsigned int *sstMode)
{
  *addrType = addrType1;
  *dataType = dataType1;
  *sstMode = sstMode1;

  return;
}

void
usrVmeDmaSetConfig(unsigned int addrType, unsigned int dataType, unsigned int sstMode)
{
  int i;

  addrType1 = addrType;
  dataType1 = dataType;
  sstMode1 = sstMode;

  jlabgefDmaConfig(addrType,dataType,sstMode);

  /* allocate buffer for DMA list descriptors and points 'dmaDescList' to it */
  jlabgefDmaAllocLLBuffer();

  /*sergey: NEED IT ??? copy 'dmaDescSample' in 'dmaDescList' array to be used by dma list*/
  for(i=0; i<TSI148_DMA_MAX_LL; i++)
  {
    dmaDescList[i] = dmaDescSample;
  }

  return;
}

/*
void
usrVmeUserToPhysOffset(unsigned int offset)
{
  vmeUserToPhysOffset = offset;
  return;
}
*/

/* modified jlabgefDmaSend() - AUGHTUNG: PARAMETERS ARE SWAPPED !!! */
/*
 vmeAdrs - VME user address (phys address)
 locAdrs - MEM user address (will be converted to phys address inside that routine)
 nbytes - size of the DMA Transfer in bytes
 */
int
usrVme2MemDmaStart(unsigned int vmeAdrs, unsigned int locAdrs, int nbytes)
{
  int offset;
  int tmp_dctl=0;
  int channel=0; /* hard-coded for now */

#ifdef TIMERS
  dma_timer[0] = rdtsc();
#endif /* TIMERS */

  if(!physMemBase)
  {
    printf("usrVme2MemDmaStart ERROR: DMA Physical Memory has an invalid base address (0x%08x)",
	  physMemBase);
    return(ERROR);
  }

  /* Clear any previous exception */
  jlabgefClearException(1);


  /************************/
  /* Source (VME) address */
  /*Sergey: convert userVmeAdrs to physVmeAdrs*/
  dmaDescSample.dsal = vmeAdrs/* - vmeUserToPhysOffset*/;

  /*
printf("usrVme2MemDmaStart: vmeAdrs = 0x%08x, dsal = 0x%08x\n",vmeAdrs,dmaDescSample.dsal);
  */


  /***********************/
  /* destination address */
  /* Local addresses (in Userspace) need to be translated to the physical memory */
  /* Here's the offset between current buffer position and the event head */
  offset = (int)locAdrs - (int)userMemBase;
  dmaDescSample.ddal = physMemBase + offset;
  /*
printf("usrVme2MemDmaStart: locAdrs     = 0x%08x   userMemBase = 0x%08x\n",locAdrs, userMemBase);
printf("usrVme2MemDmaStart: physMemBase = 0x%08x   offset      = 0x%08x\n\n",physMemBase,offset);
  */


  /***************************************************/
  /* Check the specified "nbytes" vs. size of memory */
  if(nbytes<=0 || nbytes>(memSize-offset)) 
  {
    printf("usrVme2MemDmaStart ERROR: the number of DMA bytes (%d), memSize=%d\n",
	     nbytes, memSize);
    return(ERROR);
  }
  dmaDescSample.dcnt = nbytes;
  /*
printf("usrVme2MemDmaStart: nbytes = %d\n",nbytes);
  */

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


/* redone from jlabgefDmaDone*/

#define NEW_DMADONE_VERSION

int
usrVme2MemDmaDone()
{
  unsigned int val=0,ii=0;
  int channel=0; 
  unsigned int timeout=10000;
  unsigned int excStatus=0;
  unsigned int excAddrL;
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
     clear it if the DMA ended on BERR or 2eSST (2e Slave Termination) */
  excStatus = jlabgefReadRegister(TEMPE_VEAT); /*TEMPE_VEAT_VES=0x80000000,TEMPE_VEAT_BERR=0x00080000,TEMPE_VEAT_2eST=0x00100000*/
  if( (excStatus & TEMPE_VEAT_VES) && ((excStatus & TEMPE_VEAT_BERR) || (excStatus & TEMPE_VEAT_2eST)) ) /*BERR*/
  {
    pTempe->lcsr.veat = LSWAP(TEMPE_VEAT_VESCL); /* clear the VME Exception Status (TEMPE_VEAT_VESCL=0x20000000)*/

    if(status != ERROR)
	{
	  excAddrL = jlabgefReadRegister(TEMPE_VEAL); /* the address where the Exception occurred */
#ifdef NEW_DMADONE_VERSION
	  /* Return value is the difference between VEAL and the Starting Address - the number of bytes transfered */
	  status = excAddrL - dmaDescSample.dsal;
#else
      status = (dmaStatus.dcsaLx - excAddrL) + dmaStatus.dcntx;
#endif
	  if(status<0)
	  {
	    printf("jlabgefDmaDone: ERROR: VME Exception Address < DMA Source Address (0x%08x < 0x%08x)\n",
	       excAddrL,dmaDescSample.dsal);
	    status = ERROR; /*Sergey: a la vxWorks: (dmaDescSample.dsal - excAddrL) + jlabgefReadRegister(TEMPE_DCNT(channel)); */
	  }
#ifndef NEW_DMADONE_VERSION
      else
	  {
        status = 0; /*sergey: a la vxWorks*/
        /*printf("jlabgefDmaDone1 returns %d bytes\n",status)*/;
	  }
#endif
	}
  }
  else /*no BERR*/
  {
    /* DMA ended on DMA Count (No BERR), return original byte count */
    if(status != ERROR)
    {
	  status = dmaDescSample.dcnt;
      dcnt = jlabgefReadRegister(TEMPE_DCNT(channel));
#ifdef NEW_DMADONE_VERSION
	  /* check and make sure 0 bytes are left in the DMA Count register */
      if(dcnt != 0)
      {
	    printf("%s: ERROR: DMA terminated on master byte count,",__FUNCTION__);
	    printf("    however DCNT (%d) != 0 \n", dcnt);
	  }
      else
	  {
        /*printf("jlabgefDmaDone2 returns %d bytes\n",status)*/;
	  }
#else
      /*sergey: return the number of bytes LEFT*/
      status = dcnt;
#endif

    }
    /* If we started a linked-list transaction, dmaLL_totalwords should be non-zero */
    /* linked-list can be used for no-BERR transfer only, so we check it here */
    if(dmaLL_totalwords>0)
    {
#ifdef NEW_DMADONE_VERSION
      status = dmaLL_totalwords<<2;
#else
      status = 0;
#endif
      dmaLL_totalwords = 0;
    }
  }

  UNLOCK_TSI;
#ifdef TIMERS
  dma_timer[5] = rdtsc();
#endif /* TIMERS */

  /*printf("jlabgefDmaDone3 returns %d bytes\n",status);*/

  return(status);
}



/* from jlabgefDmaSetupLL */
void
usrVme2MemDmaListSet(unsigned int *vmeAddr, unsigned int *locAddr, unsigned int *dmaSize, unsigned int numt)
{
  unsigned int dmabuf_base=0, offset=0;
  unsigned int dmaDesc_phys_base; /* Base address of the list in system memory */
  unsigned int size_incr=0; /* "current" size of the entire DMA */
  int idma;

  /* check to see if the linked list structure array has been init'd */
  if(dmaDescList == NULL) 
  {
    printf("jlabgefDMAListSet: ERROR: dmaDescList not initialized\n");
    return;
  }

  /* Get the base address (system) of the DMA Description buffer from the GEF Hdl */
  dmaDesc_phys_base = dmaHdl_to_PhysAddr(dmaLL_hdl);


  /* loop over list of descriptors setting parameters */
  dmaLL_totalwords = 0;
  for(idma=0; idma<numt; idma++) 
  {
    /* Set the vme (source) address */
    dmaDescList[idma].dsau = 0x0;
    dmaDescList[idma].dsal = LSWAP(vmeAddr[idma]/*-vmeUserToPhysOffset*/);

    /* Set the local (destination) address */
    dmaDescList[idma].ddau = 0x0;
    dmaDescList[idma].ddal = LSWAP( physMemBase + ((int)locAddr[idma]-(int)userMemBase) );

    /* Set the byte count */
    dmaDescList[idma].dcnt = LSWAP(dmaSize[idma]);
	dmaLL_totalwords += (dmaSize[idma]>>2);
    size_incr += dmaSize[idma];

    /* Set the address of the next descriptor, or NULL for the last one */
    dmaDescList[idma].dnlau = 0x0;
    if(idma<(numt-1))
	{
	  dmaDescList[idma].dnlal = LSWAP(dmaDesc_phys_base + (idma+1)*sizeof(tsi148DmaDescriptor_t));
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

	
    /* debug prints
    printf("dmaDescList[%d]:\n",idma);
    printf("locAddr = 0x%08x\n",(unsigned int)locAddr[idma]);
    printf("  DSAL = 0x%08x  DDAL = 0x%08x  DCNT = 0x%08x\n",
	     LSWAP(dmaDescList[idma].dsal),
	     LSWAP(dmaDescList[idma].ddal),
	     LSWAP(dmaDescList[idma].dcnt));
    printf(" DNLAL = 0x%08x  DSAT = 0x%08x\n",
	     LSWAP(dmaDescList[idma].dnlal),
	     LSWAP(dmaDescList[idma].dsat));
	*/
  }

  return;
}


/* from jlabgefDmaSendLL */
void
usrVmeDmaListStart()
{
  int channel=0; 
  unsigned int dctl = 0x0; 

  LOCK_TSI;
  /* Write the address of the first entry in the linked list to the TEMPE regs */
  pTempe->lcsr.dma[channel].dnlal = LSWAP(dmaHdl_to_PhysAddr(dmaLL_hdl));
  pTempe->lcsr.dma[channel].dnlau = 0x0;

  /* Set the CTL reg */
  dctl  = 0x00030000; /* Set Linked List mode, VFAR and PFAR by default */
  dctl |= (TSI148_LCSR_DCTL_VBKS_2048 | TSI148_LCSR_DCTL_PBKS_2048); /* 2048 VME/PCI Block Size */
  dctl |= (TSI148_LCSR_DCTL_VBOT_0 | TSI148_LCSR_DCTL_PBOT_0); /* 0us VME/PCI Back-off */

  pTempe->lcsr.dma[channel].dctl = LSWAP(dctl);

  /* Set the "DGO" Bit */
  dctl |= 0x02000000;

  pTempe->lcsr.dma[channel].dctl = LSWAP(dctl);
  UNLOCK_TSI;

  return;
}

void
usrVmeDmaShow()
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

