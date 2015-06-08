/*********************************************
 *
 *  FADC Internal Trigger FADC Configuration and Control
 *  Routines.
*
*  SVN: $Rev: 488 $
*
 */

#ifndef EIEIO
#define EIEIO
#endif

unsigned int
faItrigStatus(int id, int sFlag)
{
  unsigned int status, config, tdelay, twidth, wMask, wWidth, cMask, sum_th;
  int vers, disabled, mode;

  if(id==0) id=fadcID[0];

  if((id<=0) || (id>21) || (FAp[id] == NULL)) {
    printf("faItrigStatus: ERROR : FADC in slot %d is not initialized \n",id);
    return(ERROR);
  }

  /* Express Time in ns - 4ns/clk  */
  FALOCK;
  status =  vmeRead32(&(FAp[id]->hitsum_status))&0xffff;
  config =  vmeRead32(&(FAp[id]->hitsum_cfg))&0xffff;
  tdelay =  (vmeRead32(&(FAp[id]->hitsum_trig_delay))&0xffff)*FA_ADC_NS_PER_CLK;
  twidth =  (vmeRead32(&(FAp[id]->hitsum_trig_width))&0xffff)*FA_ADC_NS_PER_CLK;
  wMask  =  vmeRead32(&(FAp[id]->hitsum_window_bits))&0xffff;
  wWidth =  (vmeRead32(&(FAp[id]->hitsum_window_width))&0xffff)*FA_ADC_NS_PER_CLK;
  cMask  =  vmeRead32(&(FAp[id]->hitsum_coin_bits))&0xffff;
  sum_th =  vmeRead32(&(FAp[id]->hitsum_sum_thresh))&0xffff;
  FAUNLOCK;

  vers     = status&FA_ITRIG_VERSION_MASK;
  mode     = config&FA_ITRIG_MODE_MASK;
  disabled = config&FA_ITRIG_ENABLE_MASK;


  printf("\n FADC Internal Trigger (HITSUM) Configuration: \n");
  if(disabled)
    printf("     Hitsum Status = 0x%04x   Config = 0x%04x (mode = %d - Disabled)\n",status, config, mode);
  else
    printf("     Hitsum Status = 0x%04x   Config = 0x%04x (mode = %d - Enabled)\n",status, config, mode);
  printf("     Trigger Out  Delay = %6d ns  Width = %5d ns\n", tdelay, twidth);
  printf("     Window  Input Mask = 0x%04x     Width = %5d ns\n", wMask, wWidth);
  printf("     Coin    Input Mask = 0x%04x \n",cMask);
  printf("     Sum Mode Threshold = %d\n",sum_th);

  return(config);
}

/************************************************************
 *
 *  Setup Internal Triggering
 *   
 *   Four Modes of Operation (tmode)
 *     1) Table Mode
 *     2) Window Mode
 *     3) Coincidence Mode
 *     4) Sum Mode
 *
 *   wMask     = Mask of 16 channels to be enabled for Window Mode
 *   wWidth    = Width of trigger window before latching (in clocks)
 *   cMask     = Mask of 16 channels to be enabled for Coincidence Mode
 *   sumThresh = 10-12 bit threshold for Sum trigger to be latched 
 *   tTable    = pointer to trigger table (65536 values) to be loaded
 */
int
faItrigSetMode(int id, int tmode, unsigned int wMask, unsigned int wWidth,
	       unsigned int cMask, unsigned int sumThresh, unsigned int *tTable)
{
  int ii;
  unsigned int config, stat, wTime;

  if(id==0) id=fadcID[0];

  if((id<=0) || (id>21) || (FAp[id] == NULL)) {
    printf("faItrigSetMode: ERROR : FADC in slot %d is not initialized \n",id);
    return(ERROR);
  }

  /* Make sure we are not enabled or running */
  FALOCK;
  config = vmeRead32(&(FAp[id]->hitsum_cfg))&FA_ITRIG_CONFIG_MASK;
  if((config&FA_ITRIG_ENABLE_MASK) == 0) {
    printf("faItrigSetMode: ERROR: Internal triggers are enabled - Disable first\n");
    FAUNLOCK;
    return(ERROR);
  }

  if((tmode<1)||(tmode>4)) {
    printf("faItrigSetMode: ERROR: Trigger mode (%d) out of range (tmode = 1-4)\n",tmode);
    FAUNLOCK;
    return(ERROR);
  }

  /* Check if we need to load a trigger table */
  if(tTable != NULL) {
    printf("faItrigSetMode: Loading trigger table from address 0x%08x \n",(unsigned int) tTable);
    vmeWrite32(&(FAp[id]->s_adr), FA_SADR_AUTO_INCREMENT);
    vmeWrite32(&(FAp[id]->hitsum_pattern), 0);  /* Make sure address 0 is not a valid trigger */
    for(ii=1;ii<=0xffff;ii++) {
      if(tTable[ii]) 
	vmeWrite32(&(FAp[id]->hitsum_pattern), 1);
      else
	vmeWrite32(&(FAp[id]->hitsum_pattern), 0);
    }
  }

  switch(tmode) {
  case FA_ITRIG_SUM_MODE:
    /* Load Sum Threshhold if in range */
    if((sumThresh > 0)&&(sumThresh <= 0xffff)) {
      vmeWrite32(&(FAp[id]->hitsum_sum_thresh), sumThresh);
    }else{
      printf("faItrigSetMode: ERROR: Sum Threshold out of range (0<st<=0xffff)\n");
      FAUNLOCK;
      return(ERROR);
    }
    stat = (config&~FA_ITRIG_MODE_MASK) | FA_ITRIG_SUM_MODE;
    vmeWrite32(&(FAp[id]->hitsum_cfg), stat);
    printf("faItrigSetMode: Configure for SUM Mode (Threshold = 0x%x)\n",sumThresh);
    break;
  case FA_ITRIG_COIN_MODE:
    /* Set Coincidence Input Channels */
    if((cMask > 0)&&(cMask <= 0xffff)) {
      vmeWrite32(&(FAp[id]->hitsum_coin_bits), cMask);
    }else{
      printf("faItrigSetMode: ERROR: Coincidence channel mask out of range (0<cc<=0xffff)\n");
      FAUNLOCK;
      return(ERROR);
    }
    stat = (config&~FA_ITRIG_MODE_MASK) | FA_ITRIG_COIN_MODE;
    vmeWrite32(&(FAp[id]->hitsum_cfg), stat);
    printf("faItrigSetMode: Configure for COINCIDENCE Mode (channel mask = 0x%x)\n",cMask);
    break;
  case FA_ITRIG_WINDOW_MODE:
    /* Set Trigger Window width and channel mask */
    if((wMask > 0)&&(wMask <= 0xffff)) {
      vmeWrite32(&(FAp[id]->hitsum_window_bits), wMask);
    }else{
      printf("faItrigSetMode: ERROR: Trigger Window channel mask out of range (0<wc<=0xffff)\n");
      FAUNLOCK;
      return(ERROR);
    }
    if((wWidth > 0)&&(wWidth <= FA_ITRIG_WINDOW_MAX_WIDTH)) {
      vmeWrite32(&(FAp[id]->hitsum_window_width), wWidth);
      wTime = 4*wWidth;
    }else{
      printf("faItrigSetMode: ERROR: Trigger Window width out of range (0<ww<=0x200)\n");
      FAUNLOCK;
      return(ERROR);
    }
    stat = (config&~FA_ITRIG_MODE_MASK) | FA_ITRIG_WINDOW_MODE;
    vmeWrite32(&(FAp[id]->hitsum_cfg), stat);
    printf("faItrigSetMode: Configure for Trigger WINDOW Mode (channel mask = 0x%x, width = %d ns)\n",
	   cMask,wTime);

    break;
  case FA_ITRIG_TABLE_MODE:
    stat = (config&~FA_ITRIG_MODE_MASK) | FA_ITRIG_TABLE_MODE;
    vmeWrite32(&(FAp[id]->hitsum_cfg), stat);
    printf("faItrigSetMode: Configure for Trigger TABLE Mode\n");
  }

  FAUNLOCK;
  return(OK);
}

/************************************************************
 *
 *  Setup Internal Trigger Table 
 *    16 input channels can be latched to create a 16 bit
 *  lookup address (0x0001 - 0xffff) in memory. The value 0 or 1
 *  at that memory address determines if a trigger pulse will be
 *  generated (this is for Window or Table mode only)
 *
 *   table = pointer to an array of 65536 values (1 or 0) that
 *           will define a valid trigger or not.
 *      (if = NULL, then the default table is loaded - all
 *       input combinations will generate a trigger)
 */
int
faItrigInitTable(int id, unsigned int *table)
{
  int ii;
  unsigned int config;

  if(id==0) id=fadcID[0];

  if((id<=0) || (id>21) || (FAp[id] == NULL)) {
    printf("faItrigSetMode: ERROR : FADC in slot %d is not initialized \n",id);
    return(ERROR);
  }

  FALOCK;
  /* Check and make sure we are not running */
  config = vmeRead32(&(FAp[id]->hitsum_cfg));
  if((config&FA_ITRIG_ENABLE_MASK) !=  FA_ITRIG_DISABLED) {
    printf("faItrigInitTable: ERROR: Cannot update Trigger Table while trigger is Enabled\n");
    FAUNLOCK;
    return(ERROR);
  }


  if(table == NULL) { 
    /* Use default Initialization - all combinations of inputs will be a valid trigger */
    vmeWrite32(&(FAp[id]->s_adr), FA_SADR_AUTO_INCREMENT);
    vmeWrite32(&(FAp[id]->hitsum_pattern), 0);  /* Make sure address 0 is not a valid trigger */
    for(ii=1;ii<=0xffff;ii++) {
      vmeWrite32(&(FAp[id]->hitsum_pattern), 1);
    }

  }else{  /* Load specified table into hitsum FPGA */

    vmeWrite32(&(FAp[id]->s_adr), FA_SADR_AUTO_INCREMENT);
    vmeWrite32(&(FAp[id]->hitsum_pattern), 0);  /* Make sure address 0 is not a valid trigger */
    for(ii=1;ii<=0xffff;ii++) {
      if(table[ii]) 
	vmeWrite32(&(FAp[id]->hitsum_pattern), 1);
      else
	vmeWrite32(&(FAp[id]->hitsum_pattern), 0);
    }

  }

  FAUNLOCK;
  return(OK);
}




int
faItrigSetHBwidth(int id, unsigned short hbWidth, unsigned short hbMask)
{
  int ii;
  unsigned int config;

  if(id==0) id=fadcID[0];

  if((id<=0) || (id>21) || (FAp[id] == NULL)) {
    printf("faItrigSetHBwidth: ERROR : FADC in slot %d is not initialized \n",id);
    return(ERROR);
  }

  /* Check and make sure we are not running */
  FALOCK;
  config = vmeRead32(&(FAp[id]->hitsum_cfg));
  if((config&FA_ITRIG_ENABLE_MASK) !=  FA_ITRIG_DISABLED) {
    printf("faItrigSetHBwidth: ERROR: Cannot set HB widths while trigger is Enabled\n");
    FAUNLOCK;
    return(ERROR);
  }


  if(hbWidth==0)  hbWidth = 1;       /* Minimum Width allowed */
  if(hbMask==0)   hbMask  = 0xffff;  /* Set all Channels */

  for(ii=0;ii<FA_MAX_ADC_CHANNELS;ii++) {
    if((1<<ii)&hbMask) {
      vmeWrite32(&(FAp[id]->s_adr), ii);                  /* Set Channel */
      vmeWrite32(&(FAp[id]->hitsum_hit_width), hbWidth);  /* Set Value */
    }
  }

  FAUNLOCK;
  return(OK);
}

unsigned int
faItrigGetHBwidth(int id, unsigned int chan)
{
  unsigned int rval;

  if(id==0) id=fadcID[0];

  if((id<=0) || (id>21) || (FAp[id] == NULL)) {
    logMsg("faItrigGetHBwidth: ERROR : FADC in slot %d is not initialized \n",id,0,0,0,0,0);
    return(0xffffffff);
  }

  if(chan>15) {
    logMsg("faItrigGetHBwidth: ERROR : Channel # out of range (0-15)\n",0,0,0,0,0,0);
    return(0xffffffff);
  }
     
  FALOCK;
  vmeWrite32(&(FAp[id]->s_adr), chan);             /* Set Channel */
  EIEIO;    
  rval = vmeRead32(&(FAp[id]->hitsum_hit_width));  /* Get Value */
  FAUNLOCK;

  return(rval);
}

void
faItrigPrintHBwidth(int id)
{
  int ii;
  unsigned int wval[16];

  if(id==0) id=fadcID[0];

  if((id<=0) || (id>21) || (FAp[id] == NULL)) {
    printf("faItrigPrintHBwidth: ERROR : FADC in slot %d is not initialized \n",id);
    return;
  }

  FALOCK;
  vmeWrite32(&(FAp[id]->s_adr), FA_SADR_AUTO_INCREMENT);
  EIEIO;
  for(ii=0;ii<FA_MAX_ADC_CHANNELS;ii++) {
    wval[ii] = (vmeRead32(&(FAp[id]->hitsum_hit_width)) + 1)*FA_ADC_NS_PER_CLK;  /* Get Values */
  }
  FAUNLOCK;

  printf(" HitBit widths for FADC Inputs in slot %d:",id);
  for(ii=0;ii<FA_MAX_ADC_CHANNELS;ii++) {
    if((ii%4)==0) printf("\n");
    printf("Chan %2d: %3d ns  ",(ii+1),wval[ii]);
  }
  printf("\n");

}


unsigned int
faItrigOutConfig(int id, unsigned short itrigDelay, unsigned short itrigWidth)
{
  unsigned int retval=0;
  unsigned short s1, s2;

  if(id==0) id=fadcID[0];

  if((id<=0) || (id>21) || (FAp[id] == NULL)) {
    logMsg("faItrigConfig: ERROR : FADC in slot %d is not initialized \n",id,0,0,0,0,0);
    return(0xffffffff);
  }

  if(itrigDelay>FA_ITRIG_MAX_DELAY) itrigDelay = FA_ITRIG_MAX_DELAY;
  if(itrigWidth>FA_ITRIG_MAX_WIDTH) itrigWidth = FA_ITRIG_MAX_WIDTH;

  FALOCK;
  if(itrigWidth)
    vmeWrite32(&(FAp[id]->hitsum_trig_width), itrigWidth);
  if(itrigDelay)
    vmeWrite32(&(FAp[id]->hitsum_trig_delay), itrigDelay);

  EIEIO;
  s1 = vmeRead32(&(FAp[id]->hitsum_trig_delay))&0xffff;
  s2 = vmeRead32(&(FAp[id]->hitsum_trig_width))&0xffff;
  retval = (s2<<16)|s1;
  FAUNLOCK;

  return(retval);

}

void
faItrigEnable(int id)
{
  unsigned int rval;

  if(id==0) id=fadcID[0];

  if((id<=0) || (id>21) || (FAp[id] == NULL)) {
    logMsg("faItrigEnable: ERROR : FADC in slot %d is not initialized \n",id,0,0,0,0,0);
    return;
  }
  
  FALOCK;
  rval = vmeRead32(&(FAp[id]->hitsum_cfg));
  rval &= ~(FA_ITRIG_DISABLED);
  
  vmeWrite32(&(FAp[id]->hitsum_cfg), rval);
  FAUNLOCK;
}

void
faItrigDisable(int id)
{
  unsigned int rval;

  if(id==0) id=fadcID[0];

  if((id<=0) || (id>21) || (FAp[id] == NULL)) {
    logMsg("faItrigDisable: ERROR : FADC in slot %d is not initialized \n",id,0,0,0,0,0);
    return;
  }

  FALOCK;
  rval = vmeRead32(&(FAp[id]->hitsum_cfg));
  rval |= FA_ITRIG_DISABLED;
  
  vmeWrite32(&(FAp[id]->hitsum_cfg), rval);
  FAUNLOCK;
}


int
faItrigGetTableVal (int id, unsigned short pMask)
{
  unsigned int rval;

  if(id==0) id=fadcID[0];

  if((id<=0) || (id>21) || (FAp[id] == NULL)) {
    logMsg("faItrigTableStatus: ERROR : FADC in slot %d is not initialized \n",id,0,0,0,0,0);
    return(ERROR);
  }

  FALOCK;
  vmeWrite32(&(FAp[id]->s_adr), pMask);
  EIEIO; /* Make sure write comes before read */
  rval = vmeRead32(&(FAp[id]->hitsum_pattern));
  FAUNLOCK;
  
  if(rval)
    return(1);
  else
    return(0);
}
