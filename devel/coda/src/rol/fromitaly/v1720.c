
/* v1720.c - CAEN FADC driver */

#if defined(VXWORKS) || defined(Linux_vme)

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#ifdef VXWORKS
#include <vxWorks.h>
#include <logLib.h>
#include <taskLib.h>
#include <intLib.h>
#include <iv.h>
#include <vxLib.h>
#else
#include "jvme.h"
#endif

#include "v1720.h"



/* Register Read/Write routines */
static unsigned short vmeRead16(volatile unsigned short *addr);
static void vmeWrite16(volatile unsigned short *addr, unsigned short val);
static unsigned int vmeRead32(volatile unsigned int *addr);
static void vmeWrite32(volatile unsigned int *addr, unsigned int val);




#ifdef VXWORKS
/* Define external Functions */
IMPORT  STATUS sysBusToLocalAdrs(int, char *, char **);
IMPORT  STATUS intDisconnect(int);
IMPORT  STATUS sysIntEnable(int);
IMPORT  STATUS sysIntDisable(int);
#endif

/* Macro to check id and c1190p */
#define CHECKID(id) {							\
    if((id<0) || (c1720p[id] == NULL)) {				\
      logMsg("%s: ERROR : ADC id %d not initialized \n",		\
	     (int)__FUNCTION__,id,3,4,5,6);				\
      return ERROR;							\
    }									\
  }

#ifdef VXWORKS
#define EIEIO    __asm__ volatile ("eieio")
#define SynC     __asm__ volatile ("sync")
#else
#define EIEIO    
#define SynC     
#endif

/* Mutex to hold of reads and writes from competing threads */
pthread_mutex_t c1720_mutex = PTHREAD_MUTEX_INITIALIZER;
#define LOCK_1720 {				\
    if(pthread_mutex_lock(&c1720_mutex)<0)	\
      perror("pthread_mutex_lock");	\
  }
#define UNLOCK_1720 {				\
    if(pthread_mutex_unlock(&c1720_mutex)<0)	\
      perror("pthread_mutex_unlock");	\
  }

unsigned int adcAddrOffset = 0;                /* Difference in CPU (USERSPACE) Base */

int Nc1720 = 0;                              /* Number of ADCs in crate */
volatile struct v1720_struct *c1720p[V1720_MAX_MODULES];    /* pointers to ADC memory map */



/* readout options */
static int cblt_not, a24_a32, sngl_blt_mblt, berr_fifo;

/* variables filled by DmaStart and used by DmaDone */
static int ndata_save[21];
static int extra_save[21];
static int size_save[21];

/*******************************************************************************
 *
 * v1720Init - Initialization
 *
 * RETURNS: the number of boards detected, or ERROR if the address is invalid
 *   or board is not present.
 */

int
v1720Init(UINT32 addr, UINT32 addr_inc, int nadc, int iFlag)
{
  int ii, jj, res=0, errFlag = 0;
  unsigned short rdata=0;
  int boardID = 0;
  unsigned int laddr;
  volatile struct v1720_ROM_struct *rp;

  /* Check for valid address */
  if(addr==0) 
  {
    printf("v1720Init: ERROR: Must specify a Bus (VME-based A32/A24) address for board 0\n");
    return(ERROR);
  }
  else if(addr < 0x00ffffff) /* A24 Addressing */
  {
    a24_a32=1;
    if((addr_inc==0)||(nadc==0)) nadc = 1; /* assume only one board to initialize */

    /* get the board address */
#ifdef VXWORKS
    res = sysBusToLocalAdrs(0x39,(char *)addr,(char **)&laddr);
    if (res != 0) 
	{
	  printf("v1720Init: ERROR in sysBusToLocalAdrs(0x39,0x%x,&laddr) \n",addr);
	  return(ERROR);
	}
    adcAddrOffset = (unsigned int)laddr-(unsigned int)addr;
#else
    res = vmeBusToLocalAdrs(0x39,(char *)addr,(char **)&laddr);
    if (res != 0) 
	{
	  printf("v1720Init: ERROR in vmeBusToLocalAdrs(0x39,0x%x,&laddr) \n",addr);
	  return(ERROR);
	}
    adcAddrOffset = (unsigned int)laddr-(unsigned int)addr;
#endif
  }
  else /* A32 Addressing */
  {
    a24_a32=2;
    if((addr_inc==0)||(nadc==0)) nadc = 1; /* assume only one board to initialize */

    /* get the board address */
#ifdef VXWORKS
    res = sysBusToLocalAdrs(0x09,(char *)addr,(char **)&laddr);
    if (res != 0) 
	{
	  printf("v1720Init: ERROR in sysBusToLocalAdrs(0x09,0x%x,&laddr) \n",addr);
	  return(ERROR);
	}
    adcAddrOffset = (unsigned int)laddr-(unsigned int)addr;
#else
    res = vmeBusToLocalAdrs(0x09,(char *)addr,(char **)&laddr);
    if (res != 0) 
	{
	  printf("v1720Init: ERROR in vmeBusToLocalAdrs(0x09,0x%x,&laddr) \n",addr);
	  return(ERROR);
	}
    adcAddrOffset = (unsigned int)laddr-(unsigned int)addr;
#endif
  }
 
  Nc1720 = 0;
  for(ii=0; ii<nadc; ii++) 
  {
    c1720p[ii] = (struct v1720_struct *)(laddr + ii*addr_inc);

  /* Check if Board exists at that address */
#ifdef VXWORKS
    res = vxMemProbe((char *) &(c1720p[ii]->roc_fpga_firmware_rev),0,2,(char *)&rdata);
#else
    res = vmeMemProbe((char *) &(c1720p[ii]->roc_fpga_firmware_rev),2,(char *)&rdata);
#endif
    if(res < 0 )
    /*       if(res < 0 || rdata==0xffff)  */
    {
      printf("v1720Init: ERROR: No addressable board at addr=0x%x\n",(UINT32) c1720p[ii]);
      c1720p[ii] = NULL;
      errFlag = 1;
      break;
    } 
    else 
    {
      /* Check if this is a Model 1720 */
      rp = (struct v1720_ROM_struct *)((UINT32)c1720p[ii] + V1720_ROM_OFFSET);
      boardID = ((vmeRead32(&(rp->board2))&(0xff))<<16) + 
	    ((vmeRead32(&(rp->board1))&(0xff))<<8) + 
	    (vmeRead32(&(rp->board0))&(0xff)); 
      if(boardID != V1720_BOARD_ID) 
	  {
	    printf("v1720Init: ERROR: Board ID does not match: 0x%x \n",boardID);
	    break;
	  }

      /* Check if this is the firmware we expect V1720_FIRMWARE_REV */
      if((rdata != V1720_FIRMWARE_REV)&&(rdata != (V1720_FIRMWARE_REV-1)))
	  {
	    printf("WARN: Firmware does not match: 0x%08x (expected 0x%08x)\n",
		   rdata,V1720_FIRMWARE_REV);
        break; 
	  } 
    }
    printf("Initialized ADC ID %2d at address 0x%08x \n",ii,(UINT32) c1720p[ii]);

    Nc1720++;
  }

  /* Disable/Clear all ADCs */
  for (ii=0; ii<Nc1720; ii++) 
  {
    vmeWrite32(&(c1720p[ii]->sw_reset),1);
    vmeWrite32(&(c1720p[ii]->sw_clear),1);
  }

#ifndef VXWORKS
  /*sergey: let jlabgefDMA know an offset between vmePhysMem and vmeUserMem */
  usrVmeUserToPhysOffset(adcAddrOffset);
#endif

  return(Nc1720);
}






STATUS
v1720Config(int options)
{
  int ii, ifull;
  unsigned short value, value0, value1, array0[32], array1[32], data16;
  unsigned short dat16[21][10];
  short tmp[5], tdata;
  unsigned int ww, wm, wr;
  int wo;

  /* unpack options */
  /*cblt_not = (options>>24)&0xFF;*/
  /*a24_a32 = (options>>16)&0xFF;*/
  sngl_blt_mblt = (options>>8)&0xFF;
  berr_fifo = (options)&0xFF;

  /* do not use cblt any more
  if(cblt_not==0)
  {
    printf("  board-by-board readout\n");
  }
  else if(cblt_not==0x01)
  {
    printf("  chained readout\n");
  }
  else
  {
    printf("  unknown cblt_not mode, use board-by-board readout\n");
    cblt_not = 0x0;
  }
  */
  cblt_not = 0x0;

  /* done in init based on address provided
  if(a24_a32==0x01)
  {
    printf("  A24 addressing mode\n");
  }
  else if(a24_a32==0x02)
  {
    printf("  A32 addressing mode\n");
  }
  else
  {
    printf("  unknown addressing mode, use A24 addressing mode\n");
    a24_a32 = 0x01;
  }
  */

  if(sngl_blt_mblt==0x01)
  {
    printf("  D32 single word readout\n");
  }
  else if(sngl_blt_mblt==0x02)
  {
    printf("  D32 DMA (BLT) readout\n");
  }
  else if(sngl_blt_mblt==0x03)
  {
    printf("  D64 DMA (MBLT) readout\n");
  }
  else if(sngl_blt_mblt==0x04)
  {
    printf("  D64 DMA (2eVME) readout\n");
  }
  else if(sngl_blt_mblt==0x05)
  {
    printf("  D64 DMA (2eSST) readout\n");
  }
  else
  {
    printf("  unknown readout mode, use D32 single word readout\n");
    sngl_blt_mblt = 0x01;
  }

  if(berr_fifo==0x00)
  {
    printf("  Nwords method: use VME BUS error\n");
  }
  else if(berr_fifo==0x01)
  {
    printf("  Nwords method: use event fifo\n");
  }
  else
  {
    printf("  unknown Nwords method, use VME BUS error\n");
    berr_fifo = 0x00;
  }

  /*************************************/
  /* VME controller DMA initialization */
  usrVmeDmaInit();
  /* usrVmeDmaConfig params:
          first: 0 - A16,
                 1 - A24,
                 2 - A32
          second: 0 - D16 / SINGLE
                  1 - D32 / SINGLE
                  2 - D32 / BLOCK (BLT)  <- use this for DMA !
                  3 - D64 / BLOCK (MBLT) <- use this for DMA !

  */
  if(a24_a32==0x02) /* A32 */
  {
    usrVmeDmaConfig(2, sngl_blt_mblt);
  }
  else /* A24 */
  {
    usrVmeDmaConfig(1, sngl_blt_mblt);
  }



  printf("start v1720 initialization\n");

  /* general initialization */
  for(ii=0; ii<Nc1720; ii++) v1720Clear(ii);
  for(ii=0; ii<Nc1720; ii++) v1720Reset(ii);

  /* Program ADC for trigger matching mode 
  for(ii=0; ii<Nc1720; ii++)
  {
    v1720SetTriggerMatchingMode(ii);
  }
  */

  /****************************/
  /* setting boards parameters */

  for(ii=0; ii<Nc1720; ii++)
  {
    v1720SetAllChannels(ii,0,0,0,1,0,0);
    v1720SetBuffer(ii, 0x0A); /*1024 buffers 1K samples max size*/
    v1720SetAcquisition(ii, 0, 0, 0);
    v1720SetTriggerEnableMask(ii, 0, 1, 0, 0);/*(board,sw trigger,ext trigger,coinc level,chan mask)*/
    v1720SetFontPanelTriggerOutEnableMask(ii, 0, 1, 0); 
    v1720SetPostTrigger(ii, 16);
    v1720SetChannelEnableMask(ii, 0xFF);
    v1720SetBLTEventNumber(ii, 1);
  }







  /********************/
  /* readout settings */

  if(berr_fifo == 0x01)
  {
    /* enable event fifo */
    for(ii=0; ii<Nc1720; ii++)
    {
      /*v1720EventFifo(ii,1)*/;
    }
  }

  for(ii=0; ii<Nc1720; ii++)
  {
    ifull = v1720StatusFull(ii);
    printf("ifull=%d\n",ifull);

    printf("end v1720\n");
  }


  /* if using event fifo, disable bus error on BLT finish */
  if(berr_fifo == 0x01)
  {
    printf("disable VME BUS error for event fifo readout\n");
    for(ii=0; ii<Nc1720; ii++)
    {
      v1720SetBusError(ii,0);
	}
  }
  else
  {
    printf("enable VME BUS error\n");
    for(ii=0; ii<Nc1720; ii++)
    {
      v1720SetBusError(ii,1);
    }
  }

  /* enable 64-bit alignment */
  /* NOTE: 64-bit alignment must be enabled for any DMA readout,
  not only for MBLT(D64) but for BLT(D32) as well; some DMA engines
  (for example the one on mv5100) will cut off last 32-bit word if
  it is not even, for example if event size is equal to 137 D32 words,
  only 136 D32 words will be transfered */
  /* in addition universe library contains check for 64-bit alignment
	 in dmastart procedure, it will return error if ... */
  for(ii=0; ii<Nc1720; ii++)
  {
    v1720SetAlign64(ii,1);
  }

  /* set BLT Event Number Register
  for(ii=0; ii<Nc1720; ii++)
  {
    tdata = 1;
    v1720SetBLTEventNumber(ii, tdata);
    tdata = v1720GetBLTEventNumber(ii);
    printf("BLT Event Number set to %d\n",tdata);
  }
  */

  /* set maximum number of hits per event 
  for(ii=0; ii<Nc1720; ii++)
  {
    tdata = 64;
    v1720SetMaxNumberOfHitsPerEvent(ii, tdata);
    tdata = v1720GetMaxNumberOfHitsPerEvent(ii);
  }
  */  


  /* reset MCST flag in every board in case if it was set before
  for(ii=0; ii<Nc1720; ii++)
  {
    v1720ResetMCST(ii);
  }
*/

  /* set 'almost full' level and program output connector to signal on it 
  for(ii=0; ii<Nc1720; ii++)
  {
    v1720SetAlmostFullLevel(ii, 16384);
    tdata = v1720GetAlmostFullLevel(ii);

    v1720SetOutProg(ii,2);
    tdata = v1720GetOutProg(ii);
  }
*/

  /*
  fflush(stdout);
  taskDelay(10);
  */

  return;
}



/*******************************************************************************
 *
 * v1720Status - Gives Status info on specified ADC
 *
 *
 * RETURNS: None
 */

STATUS
v1720Status(int id)
{
  int lock;
  int drdy=0, afull=0, bfull=0;
  int berr=0, testmode=0;
  int trigMatch=0, headers=0, adcerror[4];
  UINT16 statReg, cntlReg, afullLevel, bltEvents;
  UINT16 iLvl, iVec, evStored;
  UINT16 evCount, res=0;

  CHECKID(id);

  LOCK_1720;


  /*
  statReg = vmeRead32(&(c1720p[id]->status))&V1720_STATUS_MASK;
  cntlReg = vmeRead32(&(c1720p[id]->control))&V1720_CONTROL_MASK;
  afullLevel = vmeRead32(&(c1720p[id]->almostFullLevel));
  bltEvents = vmeRead32(&(c1720p[id]->bltEventNumber))&V1720_BLTEVNUM_MASK;
  
  iLvl = vmeRead32(&(c1720p[id]->intLevel))&V1720_INTLEVEL_MASK;
  iVec = vmeRead32(&(c1720p[id]->intVector))&V1720_INTVECTOR_MASK;
  evCount  = vmeRead32(&(c1720p[id]->evCount))&V1720_EVCOUNT_MASK;
  evStored = vmeRead32(&(c1720p[id]->evStored));
  UNLOCK_1720;

  drdy  = statReg&V1720_STATUS_DATA_READY;
  afull = statReg&V1720_STATUS_ALMOST_FULL;
  bfull = statReg&V1720_STATUS_BUFFER_FULL;

  trigMatch = statReg&V1720_STATUS_TRIG_MATCH;
  headers  = statReg&V1720_STATUS_HEADER_ENABLE;
  adcerror[0] = (statReg&V1720_STATUS_ERROR_0)>>6;
  adcerror[1] = (statReg&V1720_STATUS_ERROR_1)>>7;
  adcerror[2] = (statReg&V1720_STATUS_ERROR_2)>>8;
  adcerror[3] = (statReg&V1720_STATUS_ERROR_3)>>9;

  berr = cntlReg&V1720_BUSERROR_ENABLE;
  testmode = cntlReg&V1720_TESTMODE;

#ifdef VXWORKS
  printf("STATUS for v1720 ADC at base address 0x%x\n",(UINT32)c1720p[id]);
#else
  printf("STATUS for v1720 ADC at VME (USER) base address 0x%x (0x%x)\n",
	 (UINT32)c1720p[id] - adcAddrOffset, (UINT32)c1720p[id]);
#endif
  printf("---------------------------------------------- \n");

  if(iLvl>0) {
    printf(" Interrupts Enabled - Request level = %d words\n",afullLevel);
    printf(" VME Interrupt Level: %d   Vector: 0x%x \n",iLvl,iVec);
  } else {
    printf(" Interrupts Disabled\n");
  }
  printf("\n");

  printf("  Data Status \n");
  printf("    Events to transfer via BLT  = 0x%04x\n",bltEvents);
  if(bfull) {
    printf("    Status        = 0x%04x (Buffer Full)\n",statReg);
  } else if(afull) {
    printf("    Status        = 0x%04x (Almost Full)\n",statReg);
  }else if(drdy) {
    printf("    Status        = 0x%04x (Data Ready )\n",statReg);
  }else {
    printf("    Status        = 0x%04x (No Data)\n",statReg);
  }
  
  printf("    Events Stored = %d\n",evStored);
  if(evCount == 0xffff)
    printf("    Total Events  = (No Events taken)\n");
  else
    printf("    Total Events  = %d\n",evCount);
  printf("    Almost Full   = %d\n",afullLevel);

  
  printf("\n");

  printf("  ADC Mode/Status \n"); 

  res = v1720GetEdgeResolution(id);
  printf("    Resolution      : %d ps\n",res);

  if(testmode)
    printf("    Test Mode       : On\n");

  if(trigMatch)
    printf("    Trigger Matching: On\n");
  else
    printf("    Trigger Matching: Off\n");
  
  if(headers)
    printf("    ADC Headers/EOB : On\n");
  else
    printf("    ADC Headers/EOB : Off\n");

  if(berr)
    printf("    Bus Errors      : On\n");
  else
    printf("    Bus Errors      : Off\n");

  printf("    ADC Errors      : 0:%d   1:%d   2:%d   3:%d\n",
	 adcerror[0],adcerror[1],adcerror[2],adcerror[3]);
  printf("\n");
  */


  return OK;
}




/*******************/
/* low-level calls */


STATUS
v1720Clear(int id)
{
  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->sw_clear),1);
  UNLOCK_1720;
  return OK;
}

STATUS
v1720Reset(int id)
{
  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->sw_reset),1);
  UNLOCK_1720;
  return OK;
}



STATUS
v1720SetChannelZSThreshold(int id, int ch, int logic, int threshold)
{
  unsigned int mask;

  if(logic<0||logic>1||threshold<0||threshold>0xFFF)
  {
    printf("v1720SetChannelZSThreshold ERROR: bad parameter(s)\n");
    return ERROR;
  }

  mask = (logic<<31)+threshold;

  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->chan[ch].zs_thres),mask);
  UNLOCK_1720;
  return OK;
}


STATUS
v1720SetChannelZSNsamples(int id, int ch, int nsamples)
{
  unsigned int mask;

  mask = nsamples; /*must distinguish 2 cases - see manual ??*/

  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->chan[ch].zs_nsamp),mask);
  UNLOCK_1720;
  return OK;
}


STATUS
v1720SetChannelThreshold(int id, int ch, int threshold)
{
  unsigned int mask;

  if(threshold<0||threshold>0xFFF)
  {
    printf("v1720SetChannelThreshold ERROR: bad parameter(s)\n");
    return ERROR;
  }

  mask = threshold;

  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->chan[ch].threshold),mask);
  UNLOCK_1720;
  return OK;
}


STATUS
v1720SetChannelNdataOverUnderThreshold(int id, int ch, int ndata)
{
  unsigned int mask;

  if(ndata<0||ndata>0xFFF)
  {
    printf("v1720SetNdataOverUnderThreshold ERROR: bad parameter(s)\n");
    return ERROR;
  }

  mask = ndata;

  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->chan[ch].ndata_over_under_threshold),mask);
  UNLOCK_1720;
  return OK;
}


STATUS
v1720SetChannelDACOffset(int id, int ch, int offset)
{
  unsigned int mask;

  if(offset<0||offset>0xFFFF)
  {
    printf("v1720SetChannelDACOffset ERROR: bad parameter(s)\n");
    return ERROR;
  }

  mask = offset;

  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->chan[ch].dac_offset),mask);

  /*need to check status bit 2 ???*/

  UNLOCK_1720;
  return OK;
}

/*
  inputs:
     zero_supr: 0 - no zero suppression, 2 - ZLE(zero length encoded), 3 - ZS AMP(full suppression besed on amplitude)
     pack25: 0 - pack2.5 disabled, 1 - pack2.5 enabled
     trig_out: 0 - trigger out on input over threshold, 1 - under threshold
     mem_access: 0 - memory random access, 1 - sequential access
     test_pattern: 0 - test pattern generation disabled, 1 - enabled
     trig_overlap: 0 - trigger overlapping disabled, 1 - enabled    
 */
STATUS
v1720SetAllChannels(int id, int zero_supr, int pack25, int trig_out, int mem_access, int test_pattern, int trig_overlap)
{
  unsigned int mask;

  if(zero_supr<0||zero_supr==2||zero_supr>3||
     pack25<0||pack25>1||trig_out<0||trig_out>1||mem_access<0||mem_access>1||
     test_pattern<0||test_pattern>1||trig_overlap<0||trig_overlap>1)
  {
    printf("v1720SetAllChannels ERROR: bad parameter(s)\n");
    return ERROR;
  }

  mask = (zero_supr<<16)+(pack25<<11)+(trig_out<<6)+(mem_access<<4)+(test_pattern<<3)+(trig_overlap<<1);

  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->chan_config),mask);
  UNLOCK_1720;
  return OK;
}


STATUS
v1720SetBuffer(int id, int code)
{
  unsigned int mask;

  if(code<0||code>0x0A)
  {
    printf("v1720SetBuffer ERROR: bad parameter(s)\n");
    return ERROR;
  }

  mask = code;

  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->buffer_organization),mask);
  UNLOCK_1720;
  return OK;
}



/* frees first 'nblocks' outbut buffer memory blocks */
STATUS
v1720FreeBuffer(int id, int nblocks)
{
  unsigned int mask;

  if(nblocks<0||nblocks>0xFFF)
  {
    printf("v1720FreeBuffer ERROR: bad parameter(s)\n");
    return ERROR;
  }

  mask = nblocks;

  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->buffer_free),mask);
  UNLOCK_1720;
  return OK;
}

STATUS
v1720SetBufferSizePerEvent(int id, int nloc)
{
  unsigned int mask;

  mask = nloc;

  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->custom_size),mask);
  UNLOCK_1720;
  return OK;
}

/*
inputs:
 trig_count: 0-count accepted triggers, 1-count all triggers
 stop_run: 0-acquisition stop, 1-acquisition run
 mode: 0-register-controlled run mode, 1-s-in controlled run mode, 2-s-in gate mode, 3-multi-board sync mode
*/
STATUS
v1720SetAcquisition(int id, int trig_count, int stop_run, int mode)
{
  unsigned int mask;

  if(trig_count<0||trig_count>1||stop_run<0||stop_run>1||mode<0||mode>3)
  {
    printf("v1720SetAcquisition ERROR: bad parameter(s)\n");
    return ERROR;
  }

  mask = (trig_count<<3)+(stop_run<<2)+mode;

  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->acquisition_control),mask);
  UNLOCK_1720;
  return OK;
}


STATUS
v1720GenerateSoftwareTrigger(int id)
{
  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->sw_trigger),1);
  UNLOCK_1720;
  return OK;
}

/*
 inputs:
 sw_trig: 0-softare trigger disabled, 1-software trigger enabled
 ext_trig 0-external trigger disabled, 1-external trigger enabled
 coinc_level: 1-at least 2 channels above threshold required to generate trigger, 2-at least 3, and so on
 chan_mask: 8-bit mask enabling/disabling channel triggers, bit 0 is channel 0, bit 7 is channel 7
 */
STATUS
v1720SetTriggerEnableMask(int id, int sw_trig, int ext_trig, int coinc_level, int chan_mask)
{
  unsigned int mask;

  if(sw_trig<0||sw_trig>1||ext_trig<0||ext_trig>1||coinc_level<0||coinc_level>7||chan_mask<0||chan_mask>0xFF)
  {
    printf("v1720SetTriggerEnableMask ERROR: bad parameter(s)\n");
    return ERROR;
  }

  mask = (sw_trig<<31)+(ext_trig<<30)+(coinc_level<<24)+chan_mask;

  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->trigger_source_enable_mask),mask);
  UNLOCK_1720;
  return OK;
}

STATUS
v1720SetFontPanelTriggerOutEnableMask(int id, int sw_trig, int ext_trig, int chan_mask)
{
  unsigned int mask;

  if(sw_trig<0||sw_trig>1||ext_trig<0||ext_trig>1||chan_mask<0||chan_mask>0xFF)
  {
    printf("v1720SetFontPanelTriggerOutEnableMask ERROR: bad parameter(s)\n");
    return ERROR;
  }

  mask = (sw_trig<<31)+(ext_trig<<30)+chan_mask;

  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->front_panel_trigger_out_enable_mask),mask);
  UNLOCK_1720;
  return OK;
}

STATUS
v1720SetPostTrigger(int id, int mask)
{
  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->post_trigger_setting),mask);
  UNLOCK_1720;
  return OK;
}

STATUS
v1720SetFrontPanelIOData(int id, int data)
{
  unsigned int mask;

  if(data<0||data>0xFFFF)
  {
    printf("v1720SetFontPanelIOData ERROR: bad parameter(s)\n");
    return ERROR;
  }

  mask = data;

  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->front_panel_io_data),mask);
  UNLOCK_1720;
  return OK;
}

/*
STATUS
v1720SetFrontPanelIO(int id, int mode, int trg_out, int pattern_latch, int )
{
  unsigned int mask;

  if(data<0||data>0xFFFF)
  {
    printf("v1720SetFontPanelIO ERROR: bad parameter(s)\n");
    return ERROR;
  }

  mask = data;

  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->front_panel_io_control),mask);
  UNLOCK_1720;
  return OK;
}
*/

STATUS
v1720SetChannelEnableMask(int id, int mask)
{
  if(mask<0||mask>0xFF)
  {
    printf("v1720SetChannelEnableMask ERROR: bad parameter(s)\n");
    return ERROR;
  }

  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->channel_enable_mask),mask);
  UNLOCK_1720;
  return OK;
}

STATUS
v1720SetMonitorDAC(int id, int mask)
{
  if(mask<0||mask>0xFFF)
  {
    printf("v1720SetMonitorDAC ERROR: bad parameter(s)\n");
    return ERROR;
  }

  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->monitor_dac),mask);
  UNLOCK_1720;
  return OK;
}

STATUS
v1720SetMonitorMode(int id, int mask)
{
  if(mask<0||mask>4)
  {
    printf("v1720SetMonitorMode ERROR: bad parameter(s)\n");
    return ERROR;
  }

  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->monitor_mode),mask);
  UNLOCK_1720;
  return OK;
}

/*****/





STATUS
v1720SetGeoAddress(int id, UINT16 geoaddr)
{
  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->geo_address),geoaddr & 0x1F);
  UNLOCK_1720;
  return OK;
}

int
v1720GetGeoAddress(int id)
{
  int rval;

  CHECKID(id);
  LOCK_1720;
  rval = vmeRead32(&(c1720p[id]->geo_address)) & 0x1F;
  UNLOCK_1720;
  return rval;
}



STATUS
v1720SetBLTEventNumber(int id, int nevents)
{
  if(nevents<0||nevents>0xFF)
  {
    printf("v1720SetBLTEventNumber ERROR: bad parameter(s)\n");
    return ERROR;
  }

  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->blt_event_number), nevents);
  UNLOCK_1720;
  return OK;
}

int
v1720GetBLTEventNumber(int id)
{
  int rval;

  CHECKID(id);
  LOCK_1720;
  rval = vmeRead32(&(c1720p[id]->blt_event_number)) & 0xFF;
  UNLOCK_1720;
  return rval;
}




/*******************************************************************************
 * RETURNS: 1 if Full, 0 if Not Full
 */

int
v1720StatusFull(int id)
{
  int res;

  CHECKID(id);

  LOCK_1720;
  res = vmeRead32(&(c1720p[id]->vme_status)) & V1720_STATUS_BUFFER_FULL;
  UNLOCK_1720;

  return(res);
}

int
v1720GetBusError(int id)
{
  int res;

  CHECKID(id);

  LOCK_1720;
  res = vmeRead32(&(c1720p[id]->vme_status)) & V1720_STATUS_BERR_FLAG;
  UNLOCK_1720;

  return(res);
}


/******************************************************************************
 * RETURNS: 0(No Data) or the number of events
 */

int
v1720Dready(int id)
{
  UINT16 stat=0, nevents;
  
  CHECKID(id);

  LOCK_1720;
  stat = vmeRead32(&(c1720p[id]->vme_status)) & V1720_STATUS_DATA_READY;
  if(stat)
  {
    nevents = vmeRead32(&(c1720p[id]->event_stored));
    UNLOCK_1720;
    return(nevents);
  }
  else
  {
    UNLOCK_1720;
    return(0);
  }
}


/*returns the number of words in the next event*/
int
v1720GetNextEventSize(int id)
{
  UINT32 ret;
  
  CHECKID(id);

  LOCK_1720;
  ret = vmeRead32(&(c1720p[id]->event_size));
  /*ret++; /*until explained Andrea: seems not needed*/
  UNLOCK_1720;

  return(ret);
}



/******************************************************************************
 *
 * v1720SetBusError - Enable/Disable Bus Errors (to finish a block transfer,
 *                   or on an empty buffer read)
 *                    flag = 0 : Disable
 *                           1 : Enable
 *
 * RETURNS: 0 if successful, ERROR otherwise.
 */

int
v1720SetBusError(int id, UINT32 flag)
{
  unsigned int reg;

  CHECKID(id);

  if(flag<0||flag>1)
  {
    printf("v1720SetBusError ERROR: Invalid flag = %d",flag);
    return ERROR;
  }

  LOCK_1720;

  reg = vmeRead32(&(c1720p[id]->vme_control));

  if(flag == 1)
  {
    printf("set BUSerror\n");
	reg = reg | V1720_BERR_ENABLE;
  }
  else if(flag == 0)
  {
    printf("reset BUSerror\n");
	reg = reg & ~V1720_BERR_ENABLE;
  }

  vmeWrite32(&(c1720p[id]->vme_control),reg);

  UNLOCK_1720;

  return(0);
}

/******************************************************************************
 *
 * v1720SetAlign64 - Enable/Disable 64 bit alignment for block transfers
 *                   flag = 0 : Disable
 *                          1 : Enable
 *
 * RETURNS: 0 if successful, ERROR otherwise.
 */

int
v1720SetAlign64(int id, UINT32 flag)
{
  unsigned int reg;

  CHECKID(id);

  if(flag<0||flag>1)
  {
    printf("v1720SetAlign64 ERROR: Invalid flag = %d",flag);
    return ERROR;
  }

  LOCK_1720;

  reg = vmeRead32(&(c1720p[id]->vme_control));

  if(flag == 1)
  {
    printf("set Align64\n");
	reg = reg | V1720_ALIGN64;
  }
  else if(flag == 0)
  {
    printf("reset Aligh64\n");
	reg = reg & ~V1720_ALIGN64;
  }

  vmeWrite32(&(c1720p[id]->vme_control),reg);

  UNLOCK_1720;

  return(0);
}




int
v1720SetRelocation(int id, UINT32 flag)
{
  unsigned int reg;

  CHECKID(id);

  if(flag<0||flag>1)
  {
    printf("v1720SetRelocation ERROR: Invalid fg = %d",flag);
    return ERROR;
  }

  LOCK_1720;

  reg = vmeRead32(&(c1720p[id]->vme_control));

  if(flag == 1)
  {
    printf("set RELOC\n");
	reg = reg | V1720_RELOC;
  }
  else if(flag == 0)
  {
    printf("reset RELOC\n");
	reg = reg & ~V1720_RELOC;
  }

  vmeWrite32(&(c1720p[id]->vme_control),reg);

  UNLOCK_1720;

  return(0);
}




int
v1720SetBLTRange(int id, UINT32 flag)
{
  unsigned int reg;

  CHECKID(id);

  if(flag<0||flag>1)
  {
    printf("v1720SetBLTRange ERROR: Invalid fg = %d",flag);
    return ERROR;
  }

  LOCK_1720;

  reg = vmeRead32(&(c1720p[id]->vme_control));

  if(flag == 1)
  {
    printf("set BLT_RANGE\n");
	reg = reg | V1720_BLT_RANGE;
  }
  else if(flag == 0)
  {
    printf("reset BLT_RANGE\n");
	reg = reg & ~V1720_BLT_RANGE;
  }

  vmeWrite32(&(c1720p[id]->vme_control),reg);

  UNLOCK_1720;

  return(0);
}



STATUS
v1720SetRelocationAddress(int id, UINT32 address)
{
  if(address<0||address>0xFFFF)
  {
    printf("v1720SetRelocationAddress ERROR: bad parameter(s)\n");
    return ERROR;
  }

  CHECKID(id);
  LOCK_1720;
  vmeWrite32(&(c1720p[id]->relocation_address), address);
  UNLOCK_1720;
  return OK;
}

unsigned int
v1720GetRelocationAddress(int id)
{
  UINT32 ret;
  CHECKID(id);
  LOCK_1720;
  ret = vmeRead32(&(c1720p[id]->relocation_address));
  UNLOCK_1720;
  return(ret);
}







/*sergey: my mid-  and top-level functions*/

int
v1720ReadBoard(int iadc, UINT32 *tdata)
{
  UINT32 *output = tdata;
  int i, ndata;

  /* get event length in words */
  ndata = v1720GetNextEventSize(iadc);

  logMsg("v1720ReadBoard: ndata=%d\n",ndata,2,3,4,5,6);

  for(i=0; i<ndata; i++)
  {
    *output++ = vmeRead32(&(c1720p[iadc]->data[0]));
    if(i<6||i>(ndata-7)) logMsg("v1720ReadBoard:  [%d] 0x%08x\n",i,*(output-1),3,4,5,6);
  }
  
  return(ndata);
}


int
v1720ReadBoardDmaStart(int ib, UINT32 *tdata)
{
  volatile UINT32 *vmeAdr;
  int mdata, res;
  int i, nbytes;

  if(berr_fifo == 0x01) /*Berr=0 Fifo=1*/
  {
    /* FIFO: get event length in words */
    ndata_save[ib] = v1720GetNextEventSize(ib);
    /*
    logMsg("v1720ReadBoardDmaStart: INFO: event fifo reports %d words\n",
           ndata_save[ib],0,0,0,0,0);
	*/
    
   
    
  }
  else /*Berr*/
  {
    ndata_save[ib] = V1720_MAX_WORDS_PER_BOARD;
    mdata = 0;
    /*
    logMsg("v1720ReadBoardDmaStart: INFO: trying to DMA %d words\n",
           ndata_save[ib],0,0,0,0,0);
    */
  }

  /*usrVmeDmaReset();*/

  if(berr_fifo == 0x01) /*FIF0*/
  {
    if(sngl_blt_mblt >= 0x04) /* 128 bit alignment */ /*2eVME or 2eSST*/
	{
      extra_save[ib] = (4-(ndata_save[ib]%4));
      if(extra_save[ib]==4) extra_save[ib]=0;
	}
	else /* 64 bit alignment */
	{
	  /*
      extra_save[ib] = ndata_save[ib]%2;
	  */
      if( (ndata_save[ib]%2) != 0 ) extra_save[ib] = 1;
      else                          extra_save[ib] = 0;
	}
	/*
    logMsg("v1720ReadBoardDmaStart: ndata_save=%d extra_save=%d\n",
      ndata_save[ib],extra_save[ib],3,4,5,6);
	*/

    nbytes = ((ndata_save[ib]+extra_save[ib])<<2); /*multiply by 4, since V1720 has 32bit=4bite data words*/
  }
  else
  {
    nbytes = (ndata_save[ib]<<2); /*multiply by 4*/
  }


  
  /*vmeAdr = (unsigned int)(c1720p[ib]);*/

/*Andrea*/
/*ASSUME THAT v1720SetBLTRange(ii,1) WAS CALLED!!!*/
  /* vmeAdr = (unsigned int)(c1720p[ib]);*/
  vmeAdr = ((unsigned int)(c1720p[ib]))&0xFF000000;
  /* printf("VME ADDR: %#x, nbytes: %i \n",vmeAdr,nbytes);*/
/*
logMsg("vmeAdr=0x%08x\n",vmeAdr,2,3,4,5,6);
*/
  res = usrVme2MemDmaStart( (UINT32 *)vmeAdr, (UINT32 *)tdata, nbytes);

  if(res < 0)
  {
    logMsg("v1720ReadEventDmaStart: ERROR: usrVme2MemDmaStart returned %d\n",
           res,0,0,0,0,0);
  }
  /*else
  {
    logMsg("v1720ReadBoardDmaStart: usrVme2MemDmaStart returned %d\n",
      res,0,0,0,0,0);
  }
  */

  return(res);
}

int
v1720ReadBoardDmaDone(int ib)
{
  int mdata, res;
  int i;

  if(berr_fifo == 0x01)
  {
    /* check if transfer is completed; returns zero or ERROR  */
    if((res = usrVme2MemDmaDone()) < 0)
    {
      logMsg("v1720ReadBoardDmaDone: ERROR: usrVme2MemDmaDone returned = %d\n",
             res,2,3,4,5,6);
      return(0);
    }
    else
    {
      mdata = (ndata_save[ib]+extra_save[ib]) - (res>>2);
	  /*
logMsg("v1720ReadBoardDmaDone: res=%d ndata_save=%d extra_save=%d -> mdata=%d\n",
res,ndata_save[ib],extra_save[ib],mdata,5,6);
	  */

      if( (res>4) || ((mdata%2)!=0) )
      {
        logMsg("v1720ReadBoardDmaDone: WRONG: res=%d (ndata_save=%d, extra_save=%d => was %d), mdata=%d\n",
          res,ndata_save[ib],extra_save[ib],((ndata_save[ib]+extra_save[ib])<<2),mdata,6);
      }

	  /*
      logMsg("v1720ReadBoardDmaDone: INFO: usrVme2MemDmaDone returned = %d (%d)\n",
             res,mdata,3,4,5,6);
	  */
    }
  }
  else
  {
    /* check if transfer is completed; returns zero or ERROR  */
    if((res = usrVme2MemDmaDone()) < 0)
    {
      logMsg("v1720ReadBoardDmaDone: ERROR: usrVme2MemDmaDone returned = %d\n",
             res,2,3,4,5,6);
      return(0);
    }
    else if(res == 0)
    {
      logMsg("v1720ReadBoardDmaDone: ERROR: usrVme2MemDmaDone returned = %d\n",
             res,2,3,4,5,6);
      logMsg("v1720ReadBoardDmaDone: ERROR: we are expecting VME BUS error !\n",
             res,2,3,4,5,6);
      return(0);
    }
    else
    {
      mdata = ndata_save[ib] - (res>>2);
	  if(mdata>999)
        logMsg("v1720ReadBoardDmaDone: INFO: usrVme2MemDmaDone returned = %d (%d)\n",
               res,mdata,3,4,5,6);
    }
  }

  return(mdata);
}


/* generic readout for v1720 ADC boards */
/* time profiling data for 2 boards 550 bytes event size */
int
v1720ReadStart(INT32 *adcbuf, INT32 *rlenbuf)
{
  int ii, jj, nev, iadcbuf, itmp1;
  int nn[21];
  unsigned short tdata;

/* whole routine: 42 usec */

/* part1: 6 usec */

  if(Nc1720==0)
  {
    logMsg("v1720ReadStart: ERROR: Nc1720=%d\n",Nc1720,2,3,4,5,6);
  }


  for(jj=0; jj<Nc1720; jj++)
  {
    /* check if board is full */
    if(v1720StatusFull(jj))
    {
      logMsg("ERROR: [%2d] board is full - clear (Nc1720=%d)\n",
        jj,Nc1720,3,4,5,6);
      /* clear board, otherwise cannot use it any more ! */
      v1720Clear(jj);
    }

    /* check the number of events */
    nn[jj] = nev = v1720Dready(jj);
    if(nev == 0)
    {
      logMsg("v1720ReadStart: [%2d] not ready !\n",jj,2,3,4,5,6);
    }

    /* Trigger Supervisor has 6 event buffer, but we can get 7
	   if 'parallel' readout is in use */
    if(nev > 7)      
    {
	  logMsg("v1720readStart: ERROR: [%2d] nev=%d\n",jj,nev,3,4,5,6);
	}
  }




  /* part2: 36 usec */

  /* readout */
  iadcbuf = 0;
  for(jj=0; jj<Nc1720; jj++)
  {

    if(sngl_blt_mblt == 0x01)
    {
      rlenbuf[jj] = v1720ReadBoard(jj,&adcbuf[iadcbuf]);
    }
    else
    {
      /* 18usec x 2boards = 36Usec */
      v1720ReadBoardDmaStart(jj,&adcbuf[iadcbuf]);
      rlenbuf[jj] = v1720ReadBoardDmaDone(jj);
      /*printf("rlenbuf[%i]: %i \n",jj,rlenbuf[jj]);*/
    }

    if(rlenbuf[jj] <= 0)
    {
      logMsg("[%2d] ERROR: v1720ReadEvent[Dma] returns %d\n",
        jj,rlenbuf[jj],3,4,5,6);
    }
    else
    {
      iadcbuf += rlenbuf[jj];
    }

  }

  return(OK);
}


/* use DMA list */
int
v1720ReadListStart(INT32 *adcbuf, INT32 *rlenbuf)
{
  int ii, jj, nev;
  int iadcbuf;
  static int *destination[V1720_MAX_MODULES];

  /*
TIMER_VAR;
  */
  if(Nc1720<2)
  {
    logMsg("v1720ReadListStart: ERROR: Nc1720=%d\n",Nc1720,2,3,4,5,6);
  }

  for(jj=0; jj<Nc1720; jj++)
  {
    /* check if board is full */
    if(v1720StatusFull(jj))
    {
      logMsg("ERROR: [%2d] board is full - clear (Nc1720=%d)\n",
        jj,Nc1720,3,4,5,6);
      /* clear board, otherwise cannot use it any more ! */
      v1720Clear(jj);
    }

    /* check the number of events */
    nev = v1720Dready(jj);
    if(nev == 0)
    {
      logMsg("v1720ReadListStart: [%2d] not ready !\n",jj,2,3,4,5,6);
    }

    /* Trigger Supervisor has 6 event buffer, but we can get 7
	   if 'parallel' readout is in use */
    if(nev > 7)
    {
	  logMsg("v1720ReadListStart: ERROR: [%2d] nev=%d\n",jj,nev,3,4,5,6);
	}
  }


  if(sngl_blt_mblt == 0x01)
  {
    logMsg("ERROR: DMA method must be used: sngl_blt_mblt=%d\n",
        sngl_blt_mblt,2,3,4,5,6);
    return(ERROR);
  }


  /* readout settings */
  if(berr_fifo == 0x01) /* use FIFO reaout */
  {
    iadcbuf = 0;
    for(jj=0; jj<Nc1720; jj++)
    {
      /* get event length in words */
      ndata_save[jj] = v1720GetNextEventSize(jj);

      if(sngl_blt_mblt >= 0x04) /* 128 bit alignment */
	  {
        extra_save[jj] = (4-(ndata_save[jj]%4));
        if(extra_save[jj]==4) extra_save[jj]=0;
	  }
	  else /* 64 bit alignment */
	  {
        if( (ndata_save[jj]%2) != 0 ) extra_save[jj] = 1;
        else                          extra_save[jj] = 0;
	  }

      size_save[jj] = (ndata_save[jj]+extra_save[jj])<<2;
      rlenbuf[jj] = ndata_save[jj]+extra_save[jj];

      destination[jj] = &adcbuf[iadcbuf];

      iadcbuf += rlenbuf[jj];

	/*
logMsg("[%d] ask=%d (%d bytes), got=%d (0x%08x to 0x%08x)\n",
 jj,ndata_save[jj]+extra_save[jj],size_save[jj],rlenbuf[jj],(unsigned int)c1720p[jj],destination[jj]);
	*/

    }
  }
  else /* use BERR readout */
  {
    iadcbuf = 0;
    for(jj=0; jj<Nc1720; jj++)
    {
      ndata_save[jj] = V1720_MAX_WORDS_PER_BOARD;
      extra_save[jj] = 0;

      size_save[jj] = (ndata_save[jj])<<2;
      rlenbuf[jj] = ndata_save[jj];

      destination[jj] = &adcbuf[iadcbuf];

      iadcbuf += rlenbuf[jj];
	} 
  }



  /*
{
  TIMER_NORMALIZE;
  TIMER_START;
}
*/

  /* set DMA list */
  {
    unsigned int adcadr[21];
    for(ii=0; ii<Nc1720; ii++) adcadr[ii] = (unsigned int)c1720p[ii];
    usrVme2MemDmaListSet(adcadr, destination, size_save, Nc1720);
  }

  /* start DMA list */
  usrVmeDmaListStart();

  /*
{
  TIMER_STOP(100000,-1);
}
*/
  return(OK);
}

int
v1720ReadDone()
{
  int res;

  
  res = usrVme2MemDmaDone();
  

  /* for BERR need something like following:
  if(berr_fifo == 0x0)
  {
mdata = ndata_save[ib] - (res>>2);
  }
  */

  return(res/*OK*/);
}



/*******************************************************************************
 *
 * vmeRead16         - Read a 16bit register
 * vmeWrite16        - Write to a 16bit register
 * vmeRead32         - Read a 32bit register
 * vmeWrite32        - Write to a 16bit register
 *
 */

static unsigned short
vmeRead16(volatile unsigned short *addr)
{
  unsigned short rval;
  rval = *addr;
#ifndef VXWORKS
  rval = SSWAP(rval);
#endif
  return rval;
}

static void
vmeWrite16(volatile unsigned short *addr, unsigned short val)
{
#ifndef VXWORKS
  val = SSWAP(val);
#endif
  *addr = val;
  return;
}

static unsigned int
vmeRead32(volatile unsigned int *addr)
{
  unsigned int rval;
  rval = *addr;
#ifndef VXWORKS
  rval = LSWAP(rval);
#endif
  return rval;
}

static void
vmeWrite32(volatile unsigned int *addr, unsigned int val)
{
#ifndef VXWORKS
  val = LSWAP(val);
#endif
  *addr = val;
  return;
}


#else /* dummy version*/

void
v1720Lib()
{
  return;
}

#endif
