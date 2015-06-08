/****************************************************************
*
* header file for use of Struck SFI 340 with CODA crl
*
*                                DJA   March 1996
*
*****************************************************************/


volatile struct sfiStruct sfi;         /* global parameter block */

unsigned long sfiSeq2VmeFifoVal;

/* constants for fast access to fastbus functions */
/* all these addresses are found in the write-to-vme2seq-address-space */
volatile unsigned long *fastPrimDsr;
volatile unsigned long *fastPrimCsr;
volatile unsigned long *fastPrimDsrM;
volatile unsigned long *fastPrimCsrM;
volatile unsigned long *fastPrimHmDsr;
volatile unsigned long *fastPrimHmCsr;
volatile unsigned long *fastPrimHmDsrM;
volatile unsigned long *fastPrimHmCsrM;
volatile unsigned long *fastSecadR;
volatile unsigned long *fastSecadW;
volatile unsigned long *fastSecadRDis;
volatile unsigned long *fastSecadWDis;
volatile unsigned long *fastRndmR;
volatile unsigned long *fastRndmW;
volatile unsigned long *fastRndmRDis;
volatile unsigned long *fastRndmWDis;
volatile unsigned long *fastDiscon;
volatile unsigned long *fastDisconRm;
volatile unsigned long *fastStartFrdbWithClearWc;
volatile unsigned long *fastStartFrdb;
volatile unsigned long *fastStoreFrdbWc;
volatile unsigned long *fastStoreFrdbAp;
volatile unsigned long *fastLoadDmaAddressPointer;
volatile unsigned long *fastDisableRamMode;
volatile unsigned long *fastEnableRamSequencer;
volatile unsigned long *fastWriteSequencerOutReg;
volatile unsigned long *fastDisableSequencer;


/*======================================================================*/
/*
   Function     : SFI_ShowStatusReg

   In           : -
   Out          : -

   Description  : displays sfi sequencer/FASTBUS status register set

*/
/*======================================================================*/


void
SFI_ShowStatusReg1()
{
  printf("accessing address = %08lX\n",sfi.sequencerStatusReg);
  /*
  printf("Sequencer Status Register = %08lX\n",*sfi.sequencerStatusReg);
  */
}

void
SFI_ShowStatusReg()
{
  printf("Sequencer Status Register = %08lX\n",*sfi.sequencerStatusReg);
  printf("Last Sequencer KeyAddress = %08lX\n",*sfi.lastSequencerProtocolReg);
  printf("Fastbus Status Register1  = %08lX\n",*sfi.FastbusStatusReg1);
  printf("Fastbus Status Register2  = %08lX\n",*sfi.FastbusStatusReg2);
  printf("Fastbus Last Primary Addr = %08lX\n",*sfi.fastbusreadback);
}

/*======================================================================*/
/*
   Funktion     : GetFastbusPtr

   In           : keyAddress - function key address on SFI
   Out          : returns    - absolute memory address of this function

   Bemerkungen  :
*/
/*======================================================================*/

unsigned long *
GetFastbusPtr(unsigned long keyAddress)
{
  unsigned long address;

  address = (unsigned long) sfi.writeVme2SeqFifoBase;
  address += keyAddress;

  /* printf("generated address %08lX\n",address); */

  return ((unsigned long*) (address));
}

/*======================================================================*/
/*
   Funktion     : InitSFI

   In           : 32 bit Base address for SFI board
   Out          : -

   Bemerkungen  : initializes program parameters
*/
/*======================================================================*/


STATUS 
InitSFI(unsigned int SFIBaseAddr)
{
  unsigned int laddr;
  int res;

  /* Assume Default Base Address */
  if (SFIBaseAddr <= 0) SFIBaseAddr = SFI_VME_BASE_ADDR;
    
  /* Check for valid address */
  if(SFIBaseAddr < 0x00010000) /* A16 Addressing */
  {
    printf("InitSFI: ERROR: A16 Addressing not allowed for the SFI\n");
    return(ERROR);
  }
  else
  {
    if(SFIBaseAddr > 0x00ffffff)
    {
      /* 32 bit address - assume it is already translated */
      sfi.VMESlaveAddress = SFIBaseAddr;      
    }
    else
    {
      res = sysBusToLocalAdrs(0x39, (char *)SFIBaseAddr, (char **)&laddr);
      if(res != 0)
      {
        printf("InitSFI: ERROR in sysBusToLocalAdrs(0x39, 0x%x, &laddr)\n",
                SFIBaseAddr);
        return(ERROR);
      }
      sfi.VMESlaveAddress = laddr;
    }
  }


  /* Setup pointers */
  sfi.writeVme2SeqFifoBase = (unsigned long *)
    ( sfi.VMESlaveAddress + SFI_WRITE_VME2SEQ_FIFO_BASE);
  
  sfi.readSeq2VmeFifoBase = (unsigned long *)
    ( sfi.VMESlaveAddress + SFI_READ_SEQ2VME_FIFO_BASE);
  
  
  sfi.sequencerOutputFifoSize = 1024;
  
  sfi.sequencerReset = (unsigned long*) 
    ( sfi.VMESlaveAddress + SFI_SEQUENCER_RESET);
  
  sfi.sequencerEnable = (unsigned long*) 
    ( sfi.VMESlaveAddress + SFI_SEQUENCER_ENABLE);
  
  sfi.sequencerDisable = (unsigned long*) 
    ( sfi.VMESlaveAddress + SFI_SEQUENCER_DISABLE);
  
  sfi.sequencerRamLoadEnable = (unsigned long*) 
    ( sfi.VMESlaveAddress + SFI_SEQUENCER_RAM_LOAD_ENABLE);
  
  sfi.sequencerRamLoadDisable = (unsigned long*) 
    ( sfi.VMESlaveAddress + SFI_SEQUENCER_RAM_LOAD_DISABLE);
  
  sfi.sequencerTryAgain = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_SEQUENCER_TRY_AGAIN);
  
  sfi.readLocalFbAdBus = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_READ_LOCAL_FB_AD_BUS);
  
  sfi.readVme2SeqDataFifo = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_READ_VME2SEQ_DATA_FIFO);
  
  sfi.readSeqFifoFlags = (unsigned long *)
    ( sfi.VMESlaveAddress + SFI_SEQUENCER_FIFO_FLAG_AND_ECL_NIM_INPUT_REGISTER);
  
  
  sfi.writeAuxReg = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_WRITE_AUX_REGISTER);
  
  sfi.generateAuxB40Pulse = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_GENERATE_AUX_B40_PULSE);
  
  sfi.writeVmeOutSignalReg = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_WRITE_VME_OUT_SIGNAL_REGISTER);
  
  sfi.clearBothLca1TestReg = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_CLEAR_BOTH_LCA1_TEST_REGISTER);
  
  sfi.writeVmeTestReg = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_WRITE_VME_TEST_REGISTER);
  
  sfi.FastbusArbitrationLevelReg = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_FASTBUS_ARBITRATION_LEVEL_REGISTER);
  
  sfi.FastbusProtocolInlineReg = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_FASTBUS_PROTOCOL_INLINE_REGISTER);
  
  sfi.sequencerFifoFlagAndEclNimInputReg = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_SEQUENCER_FIFO_FLAG_AND_ECL_NIM_INPUT_REGISTER);
  
  sfi.nextSequencerRamAddressReg = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_NEXT_SEQUENCER_RAM_ADDRESS_REGISTER);
  
  sfi.lastSequencerProtocolReg = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_LAST_SEQUENCER_PROTOCOL_REGISTER);
  
  sfi.sequencerStatusReg = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_SEQUENCER_STATUS_REGISTER);
  
  sfi.FastbusStatusReg1 = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_FASTBUS_STATUS_REGISTER1);
  
  sfi.FastbusStatusReg2 = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_FASTBUS_STATUS_REGISTER2);
  
  sfi.fastbusreadback = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_READ_LAST_FB_PRIM_ADDR_REGISTER);

  sfi.FastbusTimeoutReg = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_FASTBUS_TIMEOUT_REGISTER);
  
  sfi.FastbusArbitrationLevelReg = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_FASTBUS_ARBITRATION_LEVEL_REGISTER);
  
  sfi.VmeIrqLevelAndVectorReg = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_VME_IRQ_LEVEL_AND_VECTOR_REGISTER);
  
  sfi.VmeIrqMaskReg = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_VME_IRQ_MASK_REGISTER);
  
  sfi.sequencerRamAddressReg = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_SEQUENCER_RAM_ADDRESS_REGISTER);
  
  sfi.resetRegisterGroupLca2 = (unsigned long *)
    (sfi.VMESlaveAddress + SFI_RESET_REGISTER_GROUP_LCA2);
  
  printf("resetRegisterGroupLca2 = %08lX\n",(unsigned long)
	 sfi.resetRegisterGroupLca2 );
  printf("FastbusTimeoutRegister = %08lX\n",(unsigned long)
	 sfi.FastbusTimeoutReg );
  printf("FastbusdArbitrationLevelReg = %08lX\n",(unsigned long)
	 sfi.FastbusArbitrationLevelReg);
  
  
  
  /* intialize global variables for fast FASTBUS access */
  fastPrimDsr     = GetFastbusPtr(PRIM_DSR);
  printf("fastPrimDsr = %08lX\n", (unsigned long) fastPrimDsr);
  fastPrimCsr     = GetFastbusPtr(PRIM_CSR);
  fastPrimDsrM    = GetFastbusPtr(PRIM_DSRM);
  fastPrimCsrM    = GetFastbusPtr(PRIM_CSRM);
  fastPrimHmDsr   = GetFastbusPtr(PRIM_HM_DSR);
  fastPrimHmCsr   = GetFastbusPtr(PRIM_HM_CSR);
  fastPrimHmDsrM  = GetFastbusPtr(PRIM_HM_DSRM);
  fastPrimHmCsrM  = GetFastbusPtr(PRIM_HM_CSRM);
  fastSecadR      = GetFastbusPtr(SECAD_R);
  fastSecadW      = GetFastbusPtr(SECAD_W);
  fastSecadRDis   = GetFastbusPtr(SECAD_R_DIS);
  fastSecadWDis   = GetFastbusPtr(SECAD_W_DIS);
  fastRndmR       = GetFastbusPtr(RNDM_R);
  fastRndmW       = GetFastbusPtr(RNDM_W);
  fastRndmRDis    = GetFastbusPtr(RNDM_R_DIS);
  fastRndmWDis    = GetFastbusPtr(RNDM_W_DIS);
  fastDiscon      = GetFastbusPtr(DISCON);
  fastDisconRm    = GetFastbusPtr(DISCON_RM);
  fastStartFrdbWithClearWc = GetFastbusPtr(START_FRDB_WITH_CLEAR_WORD_COUNTER);
  fastStartFrdb   = GetFastbusPtr(START_FRDB);
  fastStoreFrdbWc = GetFastbusPtr(STORE_FRDB_WC);
  fastStoreFrdbAp = GetFastbusPtr(STORE_FRDB_AP);
  fastLoadDmaAddressPointer = GetFastbusPtr(LOAD_DMA_ADDRESS_POINTER);
  /* initialize global variables for RAM LIST Sequencer functions */
  fastDisableRamMode = GetFastbusPtr(DISABLE_RAM_MODE);
  fastEnableRamSequencer = GetFastbusPtr(ENABLE_RAM_SEQUENCER);
  fastWriteSequencerOutReg = GetFastbusPtr(WRITE_SEQUENCER_OUT_REG);
  fastDisableSequencer = GetFastbusPtr(DISABLE_SEQUENCER);


  return(OK);
}


/*======================================================================*/
/*
   Funktion     : InitFastbus

   In           :       arbReg - value for arbitration level register
            timReg - value for arbitration timeout register
   Out          : -

   Bemerkungen  : reset fastbus sequencer and prepare for new action
*/
/*======================================================================*/



/*start mv6100 debugging
void
InitFastbus1(unsigned long arbReg, unsigned long timReg)
{
  unsigned long val32out;
  val32out = 0L;
  *sfi.sequencerReset = val32out; 
}
void
InitFastbus2(unsigned long arbReg, unsigned long timReg)
{
  *sfi.FastbusArbitrationLevelReg = arbReg;
}
void
InitFastbus3(unsigned long arbReg, unsigned long timReg)
{
  *sfi.FastbusTimeoutReg = timReg;
}
void
InitFastbus4(unsigned long arbReg, unsigned long timReg)
{
  unsigned long val32out;
  val32out = 0L;
  *sfi.sequencerEnable = val32out;
}
*/


void
InitFastbus(unsigned long arbReg, unsigned long timReg)
{
  unsigned long val32out;

  /* reset sequencer */ 
  val32out = 0L;
  *sfi.sequencerReset = val32out; 

  /* set arbitration level */
  *sfi.FastbusArbitrationLevelReg = arbReg;

  /* set timeout register  */
  *sfi.FastbusTimeoutReg = timReg;

  /* enable sequencer */
  *sfi.sequencerEnable = val32out;
}

/*======================================================================*/
/*
   Function     : sfi_pulse

   In           : pmode - setup pulse mode
                          0 - default (set and unset output)
                          1 - set output
                          2 - unset output
                  pmask - mask for which outputs to pulse 
		  pcount - number of pulses to generate

   Out          : -

   Description  : General utility to manipulate the SFI output register
*/
/*======================================================================*/
void
sfi_pulse(unsigned long pmode, unsigned long pmask, unsigned long pcount)
{
  int ii, setMask, clearMask;

  if(pmask)
  {
    setMask = pmask;
    clearMask = pmask<<16;
    switch(pmode)
    {
      case 0:
        for(ii=0; ii<pcount; ii++)
        {
          *sfi.writeVmeOutSignalReg = setMask;
          *sfi.writeVmeOutSignalReg = clearMask;
        }
        break;
      case 1:
        *sfi.writeVmeOutSignalReg = setMask;
        break;
      case 2:
        *sfi.writeVmeOutSignalReg = clearMask;
        break;
      default:
    }
  }
}

/*======================================================================*/
/*
   Function     : sfi_error_decode

   In           : pflag - set various output/reset options
                          0 - default 
                          1 - suppress messages
                          2 - force reset/enable of sequencer
                          3 - suppress logMsg and reset/enable
   Out          : -

   Description  : Decode error from last sequencer failure , reset the
                  sequencer and pend a message to LogMsg in VxWorks.
*/
/*======================================================================*/

void
sfi_error_decode(int pflag)
{
  unsigned long seqStatus, seqKeyAddr;
  unsigned long fbStatus1=0, fbStatus2=0, paddr=-1;
  int ss, fb1=0, fb2=0;
  int reset = 0;
  int enable = 0;

  /* First check Sequencer Status Register for error type */
  seqKeyAddr = *sfi.lastSequencerProtocolReg;
  seqStatus  = *sfi.sequencerStatusReg & 0xffff;
  switch(seqStatus)
  {
   case 0x8001:
   case 0xA001:
     /* logMsg("SFI_SEQ_INFO: Sequencer OK \n",0,0,0,0,0); */
     break;
   case 0x8000:
   case 0xA000:
     if(!(pflag&1)) logMsg("SFI_SEQ_ERR: Sequencer not Enabled\n",0,0,0,0,0,0);
     break;
   case 0x8011:
   case 0xA011:
     if(!(pflag&1)) logMsg("SFI_SEQ_ERR: Invalid Sequencer KeyAddress 0x%x\n",seqKeyAddr,0,0,0,0,0);
     reset = 1;
     enable = 1;
     break;
   case 0xc010:     
     if(!(pflag&1)) logMsg("SFI_CMD_ERR: Sequencer Command error: status=0x%x command = 0x%x\n",seqStatus,seqKeyAddr,0,0,0,0);
     reset = 1;
     enable = 1;
     break;
   case 0xc020:
     fb1=1;
     fbStatus1 = *sfi.FastbusStatusReg1 & 0xfff;
     paddr = *sfi.fastbusreadback;
     break;
   case 0xc040:
     fb2=1;
     fbStatus2 = *sfi.FastbusStatusReg2 & 0xffff;
     paddr = *sfi.fastbusreadback;
     break;
   case 0xc080:
     fb2=1;
     fbStatus2 = *sfi.FastbusStatusReg2 & 0xffff;
     paddr = *sfi.fastbusreadback;
     break;
   default:
     if(!(pflag&1)) logMsg("SFI_SEQ_ERR: Unknown Sequencer error: status=0x%x\n",seqStatus,0,0,0,0,0);
     break;
   }

  /* Now Determine if this is an Addressing Error or Data error */
  if(fb1) /* Addressing Error */
  {
    switch((fbStatus1 & 0x28F))
    {
     case 0x200:
       if(!(pflag&1)) logMsg("SFI_ADR_ERR: Fastbus AK timout paddr=%d \n",paddr,0,0,0,0,0);
       break;
     case 0x002:
       if(!(pflag&1)) logMsg("SFI_ADR_ERR: Fastbus Arbitration timeout \n",0,0,0,0,0,0);
       break;
     case 0x001:
       if(!(pflag&1)) logMsg("SFI_ADR_ERR: Fastbus AS-AK lock already exists\n",0,0,0,0,0,0);
       break;
     case 0x004:
       if(!(pflag&1)) logMsg("SFI_ADR_ERR: Fastbus Master long timout\n",0,0,0,0,0,0);
       break;
     case 0x008:
       if(!(pflag&1)) logMsg("SFI_ADR_ERR: Fastbus Set AS timeout (WT High)\n",0,0,0,0,0,0);
       break;
     case 0x080:
       ss = (fbStatus1&0x70)>>4;
       if(!(pflag&1)) logMsg("SFI_ADR_ERR: Fastbus SS!=0 paddr=%d SS=%d\n",paddr,ss,0,0,0,0);
       break;
     default:
       if(!(pflag&1)) logMsg("SFI_ADR_ERR: Fastbus Address Error status=%x \n",fbStatus1,0,0,0,0,0);
     }
     reset = 1;
     enable = 1;
   }
   else if(fb2) /* Data Error */
   {
     switch((fbStatus2 & 0x300F))
     {
     case 0x0001:
       if(!(pflag&1)) logMsg("SFI_DAT_ERR: Fastbus No AS-AK lock paddr=%d \n",paddr,0,0,0,0,0);
       break;
     case 0x0004:
       if(!(pflag&1)) logMsg("SFI_DAT_ERR: Fastbus DS timeout (WT High)\n",0,0,0,0,0,0);
       break;
     case 0x0008:
       if(!(pflag&1)) logMsg("SFI_DAT_ERR: Fastbus DK long timeout\n",0,0,0,0,0,0);
       break;
     case 0x1000:
       if(!(pflag&1)) logMsg("SFI_DAT_ERR: Fastbus timeout during DMA\n",0,0,0,0,0,0);
       break;
     case 0x2000:
       if(!(pflag&1)) logMsg("SFI_DAT_ERR: VME Bus timeout during DMA\n",0,0,0,0,0,0);
       break;
     default:
       if(!(pflag&1)) logMsg("SFI_DAT_ERR: Fastbus Data error paddr=%d, status=0x%x\n",paddr,fbStatus2,0,0,0,0);
     }
     reset = 1;
     enable = 1;
   }

  if(pflag&0x2)
  {
    reset = 1;
    enable = 1;
    if(!(pflag&1)) logMsg("** Resetting Sequencer ** \n",0,0,0,0,0,0);
  }
   
  /* reset sequencer */ 
  if(reset) *sfi.sequencerReset = 1; 

  /* enable sequencer */
  if(enable) *sfi.sequencerEnable = 1;

  return;
}

