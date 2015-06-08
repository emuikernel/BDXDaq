/******************************************************************************
*
*  f1tdcLib.c  -  Driver library for JLAB config and readout of JLAB F1 TDC
*                   using a VxWorks 5.4 or later based Single Board computer. 
*
*  Author: David Abbott 
*          Jefferson Lab Data Acquisition Group
*          August 2003
*
*  Revision  1.0 - Initial Revision
*                    - Supports up to 20 F1 boards in a Crate
*                    - Programmed I/O and Block reads
*
*  Revision  1.1 - Support for JLAB Signal Distribution Card
*                              JLAB Backplane Dist. board
*                              Mulitblock Readout
*                              Global Set routines
*                              Support for new F1 firmware
*
*  Revision  1.2 - Bug fixes for use with MV5500 CPUs 
*                  Added Improved Status and new Event Flush routines
*                  Added more Global routines for Multiblock modes
*
*/

#ifdef VXWORKS

#define USE_IN_HALLB

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <logLib.h>
#include <taskLib.h>
#include <intLib.h>
#include <iv.h>
#include <semLib.h>
#include <vxLib.h>


/* Include TDC definitions */
#include "f1tdcLib.h"

/* Include DMA Library definintions */
#include "universeDma.h"

/* Define external Functions */
IMPORT  STATUS sysBusToLocalAdrs(int, char *, char **);
IMPORT  STATUS intDisconnect(int);
IMPORT  STATUS sysIntEnable(int);
IMPORT  STATUS sysIntDisable(int);

#define EIEIO    __asm__ volatile ("eieio")
#define SYNC     __asm__ volatile ("sync")

/* Define Interrupts variables */
BOOL              f1tdcIntRunning  = FALSE;                    /* running flag */
int               f1tdcIntID       = -1;                       /* id number of ADC generating interrupts */
LOCAL VOIDFUNCPTR f1tdcIntRoutine  = NULL;                     /* user interrupt service routine */
LOCAL int         f1tdcIntArg      = 0;                        /* arg to user routine */
LOCAL UINT32      f1tdcIntLevel    = F1_VME_INT_LEVEL;         /* default VME interrupt level */
LOCAL UINT32      f1tdcIntVec      = F1_VME_INT_VEC;           /* default interrupt Vector */

/* Define static default config data
    0 Hi Rez      - Synchronous 
    1 Hi Rez      - Non-Synchronous (Start reqd)
    2 Normal Rez  - Synchronous
    3 Normal Rez  - Non-Synchonous (Start reqd)
    4 Not Initialized - Read data from a file
*/
LOCAL int f1ConfigData[5][16] = {
  /* 60 ps res, 3.5us range, 1.0us window */
  0x0180, 0x8000, 0x407F, 0x407F, 0x407F, 0x407F,
  0x003F, 0xA6C0, 0x2079, 0x71A6, 0x1FAF,
  0x0000, 0x0000, 0x0000, 0x0000, 0x000C,
  /*
                                 0x0180, 0x8000, 0x407F, 0x407F, 0x407F, 0x407F, 
                                 0x003F, 0xA400, 0x31D2, 0x31D2, 0x1FBA, 
                                 0x0000, 0x0000, 0x0000, 0x0000, 0x000C,
  */

                                 0x0180, 0x8000, 0x407F, 0x407F, 0x407F, 0x407F, 
                                 0x003F, 0xA400, 0x31D2, 0x31D2, 0x1FBA, 
                                 0x0000, 0x0000, 0x0000, 0x0000, 0x0008,
  
  /* 120 ps res, 3.5us range, 1.0us window */
  0x0180, 0x0000, 0x4040, 0x4040, 0x4040, 0x4040,
  0x003F, 0xCE40, 0x2079, 0x71A6, 0x1FAF,
  0x0000, 0x0000, 0x0000, 0x0000, 0x000C,

  /*
                                 0x0180, 0x0000, 0x4040, 0x4040, 0x4040, 0x4040, 
                                 0x003F, 0xC880, 0x31D2, 0x31D2, 0x1FBA, 
                                 0x0000, 0x0000, 0x0000, 0x0000, 0x000C,
  */

                                 0x0180, 0x0000, 0x4040, 0x4040, 0x4040, 0x4040, 
                                 0x003F, 0xC880, 0x31D2, 0x31D2, 0x1FBA, 
                                 0x0000, 0x0000, 0x0000, 0x0000, 0x0008,


                                 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
                                 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 
                                 0x0000, 0x0000, 0x0000, 0x0000, 0x0000
};

/* Define global variables */
int nf1tdc = 0;                             /* Number of TDCs in Crate */
int f1tdcA32Base   = 0x08000000;            /* Minimum VME A32 Address for use by TDCs */
int f1tdcA32Offset = 0x08000000;            /* Difference in CPU A32 Base - VME A32 Base */
volatile struct f1tdc_struct *f1p[21];      /* pointers to TDC memory map */
volatile unsigned int *f1pd[21];            /* pointers to TDC FIFO memory */
volatile unsigned int *f1pmb;               /* pointer to Multblock window */
volatile struct f1SDC_struct *f1SDCp;       /* pointer to Signal Distribution Card */
int f1ID[20];                               /* array of slot numbers for TDCs */
int f1MaxSlot=0;                            /* Highest Slot hold an F1 */
int f1MinSlot=0;                            /* Lowest Slot holding an F1 */
int f1UseSDC=0;                             /* Flag=1 means F1 system controlled by SDC */
int f1UseBDC=0;                             /* Flag=1 means F1 system controlled by BDC */
int f1tdcIntCount = 0;                      /* Count of interrupts from TDC */



/*******************************************************************************
*
* f1Init - Initialize JLAB F1 TDC Library. 
*
*    if iFlag > 0xf then It is assumed that a JLAB signal distribution
*                   card is available and that iFlag contains the address
*               0xXXX0 Address bits  
*               0x000X Resolution flag - see below 
*
*    iFlag:  Flag to determine intial configuration load
*            0  - high resolution   - Trigger Matching mode
*            1  - high resolution   - Start-Stop Synchronous
*            2  - normal resolution - Trigger matching mode
*            3  - normal resolution - Start-Stop Synchronous
*            4-7 - reserved
*            
*            8-F Same modes as above but Boards are initialized
*                to use the Backplane Distribution Card (Front Panel disabled).
*
*
* RETURNS: OK, or ERROR if the address is invalid or a board is not present.
*/

STATUS 
f1Init (UINT32 addr, UINT32 addr_inc, int ntdc, int iFlag)
{
  int ii, res, rdata, errFlag = 0;
  int boardID = 0;
  int maxSlot = 1;
  int minSlot = 21;
  unsigned int laddr, a32addr, a16addr;
  volatile struct f1tdc_struct *f1;

  
  /* Check for valid address */
  if(addr==0) {
    printf("f1Init: ERROR: Must specify a Bus (VME-based A24) address for TDC 0\n");
    return(ERROR);
  }else if(addr > 0x00ffffff) { /* A24 Addressing */
    printf("f1Init: ERROR: A32 Addressing not allowed for the F1 TDC\n");
    return(ERROR);
  }else{ /* A24 Addressing */
    if((addr_inc==0)||(ntdc==0))
      ntdc = 1; /* assume only one TDC to initialize */

    /* get the TDC address */
    res = sysBusToLocalAdrs(0x39,(char *)addr,(char **)&laddr);
    if (res != 0) {
      printf("f1Init: ERROR in sysBusToLocalAdrs(0x39,0x%x,&laddr) \n",addr);
      return(ERROR);
    }
  }

  /* Init Some Global variables */
  f1UseBDC = 0;
  f1UseSDC = 0;
  nf1tdc = 0;

  for (ii=0;ii<ntdc;ii++) {
    f1 = (struct f1tdc_struct *)(laddr + ii*addr_inc);
    /* Check if Board exists at that address */
    res = vxMemProbe((char *) &(f1->version),VX_READ,4,(char *)&rdata);
    if(res < 0) {
      printf("f1Init: ERROR: No addressable board at addr=0x%x\n",(UINT32) f1);
      errFlag = 1;
      break;
    } else {
      /* Check that it is an F1 board */
      if((rdata&F1_BOARD_MASK) != F1_BOARD_ID) {
	printf(" ERROR: Invalid Board ID: 0x%x\n",rdata);
        return(ERROR);
      }
      /* Check if this is board has a valid slot number */
      boardID =  ((f1->intr)&F1_SLOT_ID_MASK)>>16;
      if((boardID <= 0)||(boardID >21)) {
        printf(" ERROR: Board Slot ID is not in range: %d\n",boardID);
        return(ERROR);
      }
      f1p[boardID] = (struct f1tdc_struct *)(laddr + ii*addr_inc);
    }
    f1ID[nf1tdc] = boardID;
    if(boardID >= maxSlot) maxSlot = boardID;
    if(boardID <= minSlot) minSlot = boardID;

    nf1tdc++;
    printf("Initialized TDC %d  Slot # %d at address 0x%08x \n",ii,f1ID[ii],(UINT32) f1p[(f1ID[ii])]);
  }

  /* Check if we are using a JLAB Signal Distribution Card (SDC)
     or if we are using a Backplane Distribution Card (BDC)
     NOTE the SDC board only supports 5 F1 TDCs - so if there are
     more than 5 TDCs in the crate they can only be controlled by daisychaining 
     multiple SDCs together - or by using a Backplane distribution board (BDC)*/

  a16addr = iFlag&F1_SDC_MASK;
  if(a16addr) {
    res = sysBusToLocalAdrs(0x29,(char *)a16addr,(char **)&laddr);
    if (res != 0) {
      printf("f1Init: ERROR in sysBusToLocalAdrs(0x29,0x%x,&laddr) \n",a16addr);
      return(ERROR);
    }

    res = vxMemProbe((char *) laddr,VX_READ,2,(char *)&rdata);
    if(res < 0) {
      printf("f1Init: ERROR: No addressable SDC board at addr=0x%x\n",(UINT32) laddr);
    } else {
      f1SDCp = (struct f1SDC_struct *) laddr;
      f1SDCp->ctrl = F1_SDC_RESET;   /* Reset the Module */

      if(nf1tdc>5) {
	printf("WARN: A Single JLAB F1 Signal Distribution Module only supports 5 TDCs\n");
	printf("WARN: You must use multiple SDCs to support more TDCs - this must be configured in hardware\n");
      }
      printf("Using JLAB F1 Signal Distribution Module at address 0x%x\n",
	       (UINT32) f1SDCp); 
      f1UseSDC=1;
    }
  }
  if((iFlag&F1_BDC_MASK) && (f1UseSDC == 0)) {  /* Assume BDC exists */
    f1UseBDC = 1;
    printf("f1Init: JLAB F1 Backplane Distribution Card is Assumed in Use\n");
    printf("f1Init: Front Panel Inputs will be disabled. \n");
  }else{
    f1UseBDC = 0;
  }
  
  /* Reset all TDCs */
  for(ii=0;ii<nf1tdc;ii++) {
    f1p[f1ID[ii]]->csr  = F1_CSR_HARD_RESET;
  }
  taskDelay(30);

  /* Initialize Interrupt variables */
  f1tdcIntID = -1;
  f1tdcIntRunning = FALSE;
  f1tdcIntLevel = F1_VME_INT_LEVEL;
  f1tdcIntVec = F1_VME_INT_VEC;
  f1tdcIntRoutine = NULL;
  f1tdcIntArg = 0;

  /* Calculate the A32 Offset for use in Block Transfers */
    res = sysBusToLocalAdrs(0x09,(char *)f1tdcA32Base,(char **)&laddr);
    if (res != 0) {
      printf("f1Init: ERROR in sysBusToLocalAdrs(0x09,0x%x,&laddr) \n",f1tdcA32Base);
      return(ERROR);
    } else {
      f1tdcA32Offset = laddr - f1tdcA32Base;
    }


  /* Write configuration registers with default Config */
  for(ii=0;ii<nf1tdc;ii++) {
    f1ConfigWrite(f1ID[ii],(int *) &f1ConfigData[(iFlag&0x7)], F1_ALL_CHIPS);
    
    /* Program an A32 access address for this TDC's FIFO */
    a32addr = f1tdcA32Base + ii*F1_MAX_A32_MEM;
    res = sysBusToLocalAdrs(0x09,(char *)a32addr,(char **)&laddr);
    if (res != 0) {
      printf("f1Init: ERROR in sysBusToLocalAdrs(0x09,0x%x,&laddr) \n",a32addr);
      return(ERROR);
    }
    f1p[f1ID[ii]]->adr32 = (a32addr>>16) + 1;  /* Write the register and enable */
    f1pd[f1ID[ii]] = (unsigned int *)(laddr);  /* Set a pointer to the FIFO */

    if((f1UseSDC <= 0)&&(f1UseBDC <= 0))
      f1EnableClk(f1ID[ii]);
    f1ClearStatus(f1ID[ii],F1_ALL_CHIPS);
  }

  /* If there are more than 1 TDC in the crate then setup the Muliblock Address
     window. This must be the same on each board in the crate */
  if(nf1tdc > 1) {
    a32addr = f1tdcA32Base + (nf1tdc+1)*F1_MAX_A32_MEM; /* set MB base above individual board base */
    res = sysBusToLocalAdrs(0x09,(char *)a32addr,(char **)&laddr);
    if (res != 0) {
      printf("f1Init: ERROR in sysBusToLocalAdrs(0x09,0x%x,&laddr) \n",a32addr);
      return(ERROR);
    }
    f1pmb = (unsigned int *)(laddr);  /* Set a pointer to the FIFO */
    for (ii=0;ii<nf1tdc;ii++) {
      /* Write the register and enable */
      f1p[f1ID[ii]]->adr_mb = (a32addr+F1_MAX_A32MB_SIZE) + (a32addr>>16) + 1;
    }
    
    /* Set First Board and Last Board */
    f1MaxSlot = maxSlot;
    f1MinSlot = minSlot;
    f1p[minSlot]->ctrl |= F1_FIRST_BOARD;
    f1p[maxSlot]->ctrl |= F1_LAST_BOARD;
    
  }

  if(f1UseSDC) {  /* If we have a Signal Distribution Board. Set up to Use That */
    f1SDC_Config();
  }else if(f1UseBDC) { /* or Setup to use BDC */
    f1BDC_Config();
  }

  if(errFlag > 0) {
    printf("f1Init: ERROR: Unable to initialize all TDC Modules\n");
    if(nf1tdc > 0)
      printf("f1Init: %d TDC(s) successfully initialized\n",nf1tdc );
    return(ERROR);
  } else {
    return(OK);
  }
}


int
f1ConfigWrite(int id, int *config_data, int chipMask)
{
  int ii,jj, reg, clk=0, ext=0;
  int order[16] = { 15,10,  0,1,2,3,4,5,6,  8,9,  11,12,13,14,  7 };
  unsigned int rval;

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    printf("f1SetConfig: ERROR : TDC in slot %d is not initialized \n",id);
    return(ERROR);
  }

  if(chipMask == 0) { /* Assume all chips programmed the same */
    chipMask = F1_ALL_CHIPS;
  }

  /* Disable the clock if it is enabled */
  clk = (f1p[id]->ctrl)&F1_REF_CLK_PCB;
  ext = (f1p[id]->ctrl)&F1_REF_CLK_SEL;
  if(clk)
    f1p[id]->ctrl &= ~F1_REF_CLK_PCB;
  if(ext)
    f1p[id]->ctrl &= ~F1_REF_CLK_SEL;

  for(ii=0; ii<F1_MAX_TDC_CHIPS; ii++) {
    if((chipMask)&(1<<ii)) {
      for(jj=0; jj<16; jj++) /* write cfg data */
	{
	  reg = order[jj]; /* program reisters in the correct order */
	  if((reg>=2) && (reg<=5))
	    f1p[id]->config[ii][reg] = (config_data[reg]&F1_OFFSET_MASK);
	  else
	    f1p[id]->config[ii][reg] = config_data[reg];
	  /* read it back and check */
	  rval = (f1p[id]->config[ii][reg])&0xffff;
	}
    }
  }

  if(clk)
    f1p[id]->ctrl |= F1_REF_CLK_PCB;
  if(ext)
    f1p[id]->ctrl |= F1_REF_CLK_SEL;
  /* clear all latched status bits */
  for(ii=0;ii<F1_MAX_TDC_CHIPS;ii++) {
    if(chipMask&(1<<ii))
      f1p[id]->stat[ii] = F1_CHIP_CLEAR_STATUS;
  }

  return(OK);

}

int
f1SetConfig(int id, int iflag, int chipMask)
{

/* Set from four default configuations
    0 Hi Rez      - Synchronous
    1 Hi Rez      - Non-Synchronous
    2 Normal Rez  - Synchronous
    3 Normal Rez  - Non-Synchonous
    4 User specified (from file)
*/

  int ii,jj, reg, clk=0, ext=0;
  int order[16] = { 15,10,  0,1,2,3,4,5,6,  8,9,  11,12,13,14,  7 };
  unsigned int rval;

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    printf("f1SetConfig: ERROR : TDC in slot %d is not initialized \n",id);
    return(ERROR);
  }
  
  if((iflag<0)||(iflag>4)) {
    printf("f1SetConfig: ERROR: Invalid config number. Choose from 0-3 where\n");
    printf("             0 -> Hi Resolution (32 chan)     - Trigger Matching\n");
    printf("             1 -> Hi Resolution (32 chan)     - Non-Sync (Start reqd)\n");
    printf("             2 -> Normal Resolution (64 chan) - Trigger Matching\n");
    printf("             3 -> Normal Resolution (64 chan) - Non-Sync (Start reqd)\n");
    printf("             4 -> User specified from a file (call f1ConfigReadFile())\n");
    return (ERROR);
  }

  if(iflag==4) { /* check if there is valid config data there */
    if (f1ConfigData[iflag][0] == 0) {
      printf("f1SetConfig:ERROR: Invalid Config data\n");
      return(ERROR);
    }
  }

  if(chipMask == 0) { /* Assume all chips programmed the same */
    chipMask = F1_ALL_CHIPS;
  }

  /* Disable the clock if it is enabled */
  clk = (f1p[id]->ctrl)&F1_REF_CLK_PCB;
  ext = (f1p[id]->ctrl)&F1_REF_CLK_SEL;
  if(clk)
    f1p[id]->ctrl &= ~F1_REF_CLK_PCB;
  if(ext)
    f1p[id]->ctrl &= ~F1_REF_CLK_SEL;

  for(ii=0; ii<F1_MAX_TDC_CHIPS; ii++) {
    if((chipMask)&(1<<ii)) {
      for(jj=0; jj<16; jj++) /* write cfg data */
	{
	  reg = order[jj]; /* program reisters in the correct order */
	  if((reg>=2) && (reg<=5))
	    f1p[id]->config[ii][reg] = ((f1ConfigData[iflag][reg])&F1_OFFSET_MASK);
	  else
	    f1p[id]->config[ii][reg] = f1ConfigData[iflag][reg];
	  /* read it back and check */
	  rval = (f1p[id]->config[ii][reg])&0xffff;
	}
    }
  }
  if(clk)
    f1p[id]->ctrl |= F1_REF_CLK_PCB;
  if(ext)
    f1p[id]->ctrl |= F1_REF_CLK_SEL;

  return(OK);

}


int
f1ConfigRead(int id, int *config_data, int chipID)
{
  int jj, reg;

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    printf("f1ConfigRead: ERROR : TDC in slot %d is not initialized \n",id);
    return(ERROR);
  }
  
  if((chipID<0)||(chipID>=F1_MAX_TDC_CHIPS)) {
    printf("f1ConfigRead: ERROR : Invalid Chip ID %d (range 0-7)\n",chipID);
    return(ERROR);
  }

  for(jj=0; jj<16; jj++) { /* READ cfg data */
    config_data[jj] = f1p[id]->config[chipID][jj];
  }

  return(OK);

}

int
f1ConfigReadFile(char *filename)
{
  FILE *fd_1;
  unsigned int value, config[16];
  int ii, jj;

  if(filename == NULL) {
    printf("f1ConfigReadFile: ERROR: No Config file specified.\n");
    return(ERROR);
  }

  fd_1 = fopen(filename,"r");
  if(fd_1 > 0) {
    printf("Reading Data from file: %s\n",filename);
    jj = 4; /* location for file data */
    for(ii = 0; ii < 16; ii++) {
      fscanf(fd_1,"%lx",&value);
      f1ConfigData[jj][ii] = 0xFFFF & value;
      config[ii] = f1ConfigData[jj][ii]; 
      printf("ALL Chips: Reg %2d  =  %04x\n",ii,config[ii]);
    }        

    fclose(fd_1);
    
    return(OK);
  }else{
    printf("f1ConfigReadFile: ERROR opening file %s\n",filename);
    return(ERROR);
  }
}


void 
f1ConfigShow(int id, int chipMask)
{
  unsigned int refcnt;
  unsigned int exponent;
  unsigned int refclkdiv;
  unsigned int hsdiv;
  unsigned int trigwin;
  unsigned int triglat;
  unsigned int t_offset;
  int ii,ii_chip, sync;
  float factor;
  float tframe;
  float bin_size;
  float full_range;
  float window;
  float latency;
  
  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    printf("f1ConfigShow: ERROR : TDC in slot %d is not initialized \n",id);
    return;
  }

  if(chipMask == 0) /* Print out config info for all chips */
    chipMask = F1_ALL_CHIPS;


  for(ii_chip = 0; ii_chip < F1_MAX_TDC_CHIPS; ii_chip++) {
    if((chipMask)&(1<<ii_chip)) {

      printf("\n ---------------- Chip %d ----------------",ii_chip);
      for(ii = 0; ii < 16; ii++) {
	if((ii%8)==0) printf("\n");
	printf("0x%04x  ",((f1p[id]->config[ii_chip][ii])&0xffff));
      }
      printf("\n");
      
      if( (f1p[id]->config[ii_chip][1]) & 0x8000 ) {
	printf("High Resolution mode\n");
	factor = 0.5;
      }else{
	printf("Normal Resolution mode\n");
	factor = 1.;
      }   
      
      if( (f1p[id]->config[ii_chip][15]) & 0x4 ) {
	sync=1;
	printf("Synchronous mode\n");
      } else {
	sync=0;
	printf("Non-synchronous mode (Start required)\n");
      }
      
      refcnt = ( (f1p[id]->config[ii_chip][7]) >> 6 ) & 0x1FF;
      tframe = (float)(25 * (refcnt +2 ));
      printf("refcnt = %d   tframe (ns) = %.1f\n",refcnt,tframe);
      
      exponent =  ( (f1p[id]->config[ii_chip][10]) >> 8 ) & 0x7;
      refclkdiv = 1;
      for(ii = 0; ii < exponent; ii++)
	refclkdiv = 2 * refclkdiv;
      hsdiv = (f1p[id]->config[ii_chip][10]) & 0xFF;
      bin_size = factor * (25./152.) * ( (float)refclkdiv )/( (float)hsdiv );
      full_range = 65536 * bin_size;
      printf("refclkdiv = %d   hsdiv = %d   bin_size (ns) = %.4f   full_range (ns) = %.1f\n",
	     refclkdiv,hsdiv,bin_size,full_range);
      
      trigwin = (f1p[id]->config[ii_chip][8])&0xffff;
      triglat = (f1p[id]->config[ii_chip][9])&0xffff; 	
      window = ((float)trigwin) * bin_size/factor;	
      latency = ((float)triglat) * bin_size/factor;	
      printf("trigwin = %d   triglat = %d   window (ns) = %.1f   latency (ns) = %.1f\n",
	     trigwin,triglat,window,latency);

      if(sync) {
	t_offset = (unsigned int) (tframe/bin_size);
	printf("Rollover count = %d\n",t_offset);
      } else {
	t_offset = 65536;
	printf("Rollover count = N/A (Full Range - 65536)\n");

      }
    }
   }	
  
}


void
f1Status(int id, int sflag)
{
  int ii, jj;
  unsigned int a32Base, ambMin, ambMax;
  unsigned int csr, ctrl, count, level, intr, addr32, addrMB;

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    printf("f1Status: ERROR : TDC in slot %d is not initialized \n",id);
    return;
  }

  csr    = (f1p[id]->csr)&F1_CSR_MASK;
  ctrl   = (f1p[id]->ctrl)&F1_CONTROL_MASK;
  count  = (f1p[id]->ev_count)&F1_EVENT_COUNT_MASK;
  level  = (f1p[id]->ev_level)&F1_EVENT_LEVEL_MASK;
  intr   = f1p[id]->intr;
  addr32 = f1p[id]->adr32;
  a32Base = (addr32&F1_A32_ADDR_MASK)<<16;
  addrMB = f1p[id]->adr_mb;
  ambMin =  (addrMB&F1_AMB_MIN_MASK)<<16;
  ambMax =  (addrMB&F1_AMB_MAX_MASK);


  printf("\nSTATUS for TDC in slot %d at base address 0x%x \n",id,(UINT32) f1p[id]);
  printf("-------------------------------------------------- \n");

  printf(" Firmware Revsion/ID = 0x%x\n",(f1p[id]->version)&0xffff);
  if(addrMB&F1_AMB_ENABLE) {
    printf(" Alternate VME Addressing: Multiblock Enabled\n");
    if(addr32&F1_A32_ENABLE)
      printf("   A32 Enabled at VME base 0x%08x or CPU addr 0x%08x\n",a32Base,(UINT32) f1pd[id]);
    else
      printf("   A32 Disabled\n");
    
    printf("   Muliblock VME Address Range 0x%08x - 0x%08x\n",ambMin,ambMax);
  }else{
    printf(" Alternate VME Addressing: Multiblock Disabled\n");
    if(addr32&F1_A32_ENABLE)
      printf("   A32 Enabled at VME base 0x%08x or CPU addr 0x%08x\n",a32Base,(UINT32) f1pd[id]);
    else
      printf("   A32 Disabled\n");
  }

  if(ctrl&F1_INT_ENABLE_MASK) {
    printf("\n  Interrupts ENABLED: ");
    if(ctrl&F1_EVENT_LEVEL_INT) printf("EventLevel(%d)",level);
    if(ctrl&F1_ERROR_INT) printf("Errors ");
    printf("\n");
    printf("  VME INT Vector = 0x%x  Level = %d\n",(intr&F1_INT_VEC_MASK),((intr&F1_INT_LEVEL_MASK)>>8));
  }

  printf("\n Configuration: \n");
  if(f1UseSDC)
    printf("  USING Signal Distribution card at 0x%x\n",(UINT32)f1SDCp);
  if(f1UseBDC)
    printf("  USING Backplane Distribution Card\n");
  if(ctrl&F1_FB_SEL)
    printf("   Control Inputs from Backplane\n");
  else
    printf("   Control Inputs from Front Panel\n");

  if(ctrl&F1_REF_CLK_PCB)
    printf("   Internal Clock ON\n");
  else
    printf("   Internal Clock OFF\n");

  if(ctrl&F1_REF_CLK_SEL)
    printf("   Use EXTERNAL Clock\n");
  else
    printf("   Use INTERNAL Clock\n");

  if(ctrl&F1_ENABLE_BERR)
    printf("   Bus Error ENABLED\n");
  else
    printf("   Bus Error DISABLED\n");

  if(ctrl&F1_ENABLE_MULTIBLOCK) {
    if(ctrl&F1_FIRST_BOARD)
      printf("   MultiBlock transfer ENABLED (First Board)\n");
    else if(ctrl&F1_LAST_BOARD)
      printf("   MultiBlock transfer ENABLED (Last Board)\n");
    else
      printf("   MultiBlock transfer ENABLED\n");
  } else {
    printf("   MultiBlock transfer DISABLED\n");
  }

  if(ctrl&F1_ENABLE_SOFT_TRIG)
    printf("   Software Triggers ENABLED\n");


  printf("\n");
  if(csr&F1_CSR_ERROR)
    printf("  CSR     Register = 0x%08x - Error Condition\n",csr);
  else
    printf("  CSR     Register = 0x%08x \n",csr);
  printf("  Control Register = 0x%08x \n",ctrl);
  printf("  Events in FIFO   = %d  (Block level = %d)\n",count,level);

  /* Print out Chip Status */
  f1ChipStatus(id, 0);

  if(sflag) { /* Print out Chip configuration */
    f1ConfigShow(id,F1_ALL_CHIPS);
  }

}

void 
f1GStatus()
{
  int ii;

  for (ii=0;ii<nf1tdc;ii++) {
    f1Status(f1ID[ii],0);
  }

}

void
f1ChipStatus(int id, int pflag)
{

  int ii, reg, lock, latch, stat;
  unsigned short chipstat[F1_MAX_TDC_CHIPS];

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    printf("f1Status: ERROR : TDC in slot %d is not initialized \n",id);
    return;
  }

  for(ii=0;ii<F1_MAX_TDC_CHIPS;ii++) {
    chipstat[ii]  = f1p[id]->stat[ii];
  }

  printf("\n CHIP Status: \n");
  for(ii=0; ii<F1_MAX_TDC_CHIPS; ii++) {
    reg = chipstat[ii]&0xffff;
    lock = reg&0x1;
    stat = reg&0x7e;
    latch = (reg&0x1f00)>>8;
    if(!(reg&F1_CHIP_INITIALIZED)) {
      printf("   CHIP %d  Reg = 0x%04x -  NOT Initialized  \n",ii,reg);
    }else{
      if(lock==0) {
	printf("   CHIP %d  Reg = 0x%04x ** Resolution NOT Locked **\n",ii,reg);
      }else if(latch) {
	printf("   CHIP %d  Reg = 0x%04x ** Check Latched Status **\n",ii,reg);
	if(pflag) {
	  if(latch&F1_CHIP_RES_LOCKED)
	    printf("        Resolution Lock Failed\n");
	  if(latch&F1_CHIP_HITFIFO_OVERFLOW)
	    printf("        Hit FIFO Overflow occurred\n");
	  if(latch&F1_CHIP_TRIGFIFO_OVERFLOW)
	    printf("        Trigger FIFO Overflow occured\n");
	  if(latch&F1_CHIP_OUTFIFO_OVERFLOW)
	    printf("        Output FIFO Overflow occured\n");
	  if(latch&F1_CHIP_EXTFIFO_FULL)
	    printf("        External FIFO Full occured\n");
	}
      }else{
	printf("   CHIP %d  Reg = 0x%04x - OK\n",ii,reg);
	if(pflag) {
	  if(stat&F1_CHIP_HITFIFO_OVERFLOW)
	    printf("        Hit FIFO has Overflowed\n");
	  if(stat&F1_CHIP_TRIGFIFO_OVERFLOW)
	    printf("        Trigger has FIFO Overflowed\n");
	  if(stat&F1_CHIP_OUTFIFO_OVERFLOW)
	    printf("        Output has FIFO Overflowed\n");
	  if(stat&F1_CHIP_EXTFIFO_FULL)
	    printf("        External FIFO is Full\n");
	  if(stat&F1_CHIP_EXTFIFO_ALMOST_FULL)
	    printf("        External FIFO Almost Full (Busy Asserted)\n");
	  if((stat&F1_CHIP_EXTFIFO_EMPTY) == 0)
	    printf("        External FIFO NOT Empty\n");
	}
      }	
    }

  }      
}


/**************************************************************************************
 *
 *  f1ReadEvent - General Event readout routine
 *
 *    id    - Slot number of module to read
 *    data  - local memory address to place data
 *    nwrds - Max number of words to transfer
 *    rflag - Readout Flag
 *              0 - programmed I/O from the specified board
 *              1 - DMA transfer using Universe DMA Engine 
 *                    (DMA VME transfer Mode must be setup prior)
 *              2 - Multiblock DMA transfer (Multiblock must be enabled
 *                     and daisychain in place or BDC being used)
 */
int
f1ReadEvent(int id, volatile UINT32 *data, int nwrds, int rflag)
{
  int ii, evnum, trigtime, chip, chan;
  int stat, retVal, xferCount;
  int dCnt, berr=0;
  unsigned int head, tail, val;
  unsigned int vmeAdr;

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    logMsg("f1ReadEvent: ERROR : TDC in slot %d is not initialized \n",id,0,0,0,0,0);
    return(ERROR);
  }

  if(data==NULL) {
    logMsg("f1ReadEvent: ERROR: Invalid Destination address\n",0,0,0,0,0,0);
    return(ERROR);
  }

  if(nwrds <= 0) nwrds=F1_MAX_TDC_CHANNELS*F1_MAX_HITS_PER_CHANNEL;

  if(rflag >= 1) { /* Block Transfers */
    
    /* Don't bother checking if there is a valid event. Just blast data out of the 
       FIFO Valid or Invalid 
       Also assume that the Universe DMA programming is setup. */

    if(rflag == 2) { /* Multiblock Mode */
      if(((f1p[id]->ctrl)&F1_FIRST_BOARD)==0) {
	logMsg("f1ReadEvent: ERROR: TDC in slot %d is not First Board\n",id,0,0,0,0,0);
	return(ERROR);
      }
#ifdef USE_IN_HALLB
      vmeAdr = (unsigned int)(f1pmb);
#else
      vmeAdr = (unsigned int)(f1pmb) - f1tdcA32Offset;
#endif
    }else{
#ifdef USE_IN_HALLB
      vmeAdr = (unsigned int)(f1pd[id]);
#else
      vmeAdr = (unsigned int)(f1pd[id]) - f1tdcA32Offset;
#endif
    }
#ifdef USE_IN_HALLB
    retVal = usrVme2MemDmaStart(vmeAdr, (UINT32)data, (nwrds<<2));
#else
    retVal = sysVmeDmaSend((UINT32)data, vmeAdr, (nwrds<<2), 0);
#endif
    if(retVal < 0) {
      logMsg("f1ReadEvent: ERROR in DMA transfer Initialization 0x%x\n",retVal,0,0,0,0,0);
      return(retVal);
    }
    /* Wait until Done or Error */
#ifdef USE_IN_HALLB
    retVal = usrVme2MemDmaDone();
#else
    retVal = sysVmeDmaDone(10000,1);
#endif
    if(retVal != 0) {
      /* Check to see that Bus error was generated by TDC */
      if(rflag == 2) {
	stat = (f1p[f1MaxSlot]->csr)&F1_CSR_BERR_STATUS;  /* from Last TDC */
      }else{
	stat = (f1p[id]->csr)&F1_CSR_BERR_STATUS;  /* from TDC id */
      }
      if((retVal>0) && (stat)) {
	xferCount = (nwrds - (retVal>>2));  /* Number of Longwords transfered */
        return(xferCount); /* Return number of data words transfered */
      }else{
	logMsg("f1ReadEvent: DMA transfer terminated by unknown BUS Error\n",0,0,0,0,0,0);
	return(ERROR);
      }
    } else { /* Block Error finished without Bus Error */
      logMsg("f1ReadEvent: WARN: DMA transfer terminated by word count 0x%x\n",
        nwrds,0,0,0,0,0);
      return(nwrds);
    }

  } else {  /*Programmed IO */

    /* Check if data available */
    if(((f1p[id]->ev_count)&F1_EVENT_COUNT_MASK)==0) {
      logMsg("f1ReadEvent: ERROR: FIFO Empty\n",0,0,0,0,0,0);
      return(0);
    }

   /* Check if Bus Errors are enabled. If so then disable for reading */
    berr = (f1p[id]->ctrl)&F1_ENABLE_BERR;
    if(berr)
      f1p[id]->ctrl &= ~F1_ENABLE_BERR;

    dCnt = 0;
    /* Read Header - */
    head = (unsigned int) *f1pd[id];
    if((head&F1_HT_DATA_MASK) != F1_HEAD_DATA) {
      logMsg("f1ReadEvent: ERROR: Invalid Header Word 0x%08x\n",head,0,0,0,0,0);
      return(ERROR);
    }else{
      evnum = (head&F1_HT_EVENT_MASK)>>16;
      trigtime = (head&F1_HT_TRIG_MASK)>>7;
      data[dCnt] = head;
      dCnt++;
    }

    ii=0;
    while(ii<nwrds) {
      data[ii+1] = (unsigned int) *f1pd[id];
      if((data[ii+1]&F1_HT_DATA_MASK) == F1_TAIL_DATA)
	break;
      ii++;
    }
    ii++;
    dCnt += ii;

    if(dCnt%2) {   /* if Odd number of words then read once more to get filler */
      val = (unsigned int) *f1pd[id];
      if(val&F1_DATA_SLOT_MASK)
	logMsg("f1ReadData: ERROR: Invalid filler word 0x%08x\n",val,0,0,0,0,0);
    }

    if(berr)
      f1p[id]->ctrl |= F1_ENABLE_BERR;

    return(dCnt);
  }

  return(OK);
}


int
f1PrintEvent(int id, int rflag)
{
  int ii, evnum, trigtime, MAXWORDS=64*8;
  int dCnt, berr=0;
  unsigned int head, tail, val;

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    printf("f1PrintEvent: ERROR : TDC in slot %d is not initialized \n");
    return(ERROR);
  }

  /* Check if data available */
  if(((f1p[id]->ev_count)&F1_EVENT_COUNT_MASK)==0) {
    printf("f1PrintEvent: ERROR: FIFO Empty\n");
    return(0);
  }

  /* Check if Bus Errors are enabled. If so then disable for reading */
  berr = (f1p[id]->ctrl)&F1_ENABLE_BERR;
  if(berr)
    f1p[id]->ctrl &= ~F1_ENABLE_BERR;
  
  dCnt = 0;
  /* Read Header - */
  head = (unsigned int) *f1pd[id];

  /* check if valid data */
  if((head&F1_DATA_SLOT_MASK) == F1_DATA_INVALID) {
    printf("f1PrintEvent: ERROR: Invalid Data from FIFO 0x%08x\n",head);
    return(ERROR);
  }

  if((head&F1_HT_DATA_MASK) != F1_HEAD_DATA) {
    printf("f1PrintEvent: ERROR: Invalid Header Word 0x%08x\n",head);
    return(ERROR);
  }else{
    printf("TDC DATA for Module in Slot %d\n",id);
    evnum = (head&F1_HT_EVENT_MASK)>>16;
    trigtime = (head&F1_HT_TRIG_MASK)>>7;
    dCnt++;
    printf("  Header  = 0x%08x   Event = %d   Trigger Time = %d ",head,evnum,trigtime); 
  }

  ii=0;
  while(ii<MAXWORDS) {
    if((ii%5) == 0) printf("\n    ");
    val = (unsigned int) *f1pd[id];
    if((val&F1_HT_DATA_MASK) == F1_TAIL_DATA) {
      tail = val;
      ii++;
      printf("\n");
      break;
    }else{
      printf("  0x%08x",val);
      ii++;
    }
  }
  dCnt += ii;

  if(dCnt%2) {   /* if Odd number of words then read once more to get filler */
    val = (unsigned int) *f1pd[id];
    if(val&F1_DATA_SLOT_MASK)
      printf("f1PrintData: ERROR: Invalid filler word 0x%08x\n",val);

    printf("  Trailer = 0x%08x   Word Count = %d  Filler = 0x%08x\n",tail,dCnt,val);
  }else{
    printf("  Trailer = 0x%08x   Word Count = %d\n",tail,dCnt);
  }

  if(berr)
    f1p[id]->ctrl |= F1_ENABLE_BERR;
  
  return(dCnt);
  
}

/* Routine to flush a partial event from the FIFO. Read until a valid trailer is found */
int
f1FlushEvent(int id)
{
  int ii, evnum, trigtime, MAXWORDS=64*8;
  int berr=0;
  unsigned int tail, val;

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    logMsg("f1FlushEvent: ERROR : TDC in slot %d is not initialized \n",0,0,0,0,0,0);
    return(ERROR);
  }

  /* Check if data available - If not then just issue a Clear */
  if(((f1p[id]->ev_count)&F1_EVENT_COUNT_MASK)==0) {
    f1Clear(id);
    return(0);
  }

  /* Check if Bus Errors are enabled. If so then disable for reading */
  berr = (f1p[id]->ctrl)&F1_ENABLE_BERR;
  if(berr)
    f1p[id]->ctrl &= ~F1_ENABLE_BERR;
  

  ii=0;
  while(ii<MAXWORDS) {
    val = (unsigned int) *f1pd[id];
    if((val&F1_HT_DATA_MASK) == F1_TAIL_DATA) {
      tail = val;
      ii++;
      break;
    }else{
      ii++;
    }
  }

  if(ii%2) {   /* if Odd number of words then read once more to get filler */
    val = (unsigned int) *f1pd[id];
    ii++;
    if(val&F1_DATA_SLOT_MASK)
      logMsg("f1FlushData: ERROR: Invalid filler word 0x%08x\n",val,0,0,0,0,0);

  }

  if(berr)
    f1p[id]->ctrl |= F1_ENABLE_BERR;
  
  return(ii);
  
}



void
f1Clear(int id)
{

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    logMsg("f1Clear: ERROR : TDC in slot %d is not initialized \n",id,0,0,0,0,0);
    return;
  }
  
  f1p[id]->csr = F1_CSR_SOFT_RESET;

}
void
f1GClear()
{
  int ii;

  for(ii=0;ii<nf1tdc;ii++)
    f1p[f1ID[ii]]->csr = F1_CSR_SOFT_RESET;

}


void
f1ClearStatus(int id, unsigned int chipMask)
{
  
  int ii;

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    logMsg("f1ClearStatus: ERROR : TDC in slot %d is not initialized \n",id,0,0,0,0,0);
    return;
  }

  /* Default clear all chips latched status bits */
  if(chipMask<=0) chipMask=F1_ALL_CHIPS;
  
  for(ii=0;ii<F1_MAX_TDC_CHIPS;ii++) {
    if(chipMask&(1<<ii)) {
      f1p[id]->stat[ii] = F1_CHIP_CLEAR_STATUS;
    }
  }
}
void
f1GClearStatus(unsigned int chipMask)
{
  int ii, id;

  for(ii=0;ii<nf1tdc;ii++) {
    id = f1ID[ii];
    f1ClearStatus(id,chipMask);
  }
}


void
f1Reset(int id, int iFlag)
{
  unsigned int a32addr, addrMB;

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    logMsg("f1Reset: ERROR : TDC in slot %d is not initialized \n",id,0,0,0,0,0);
    return;
  }

  if((iFlag<0)||(iFlag>3)) iFlag=2;  /* Default to Normal Syncronous */

  a32addr = f1p[id]->adr32;
  addrMB  = f1p[id]->adr_mb;

  f1p[id]->csr = F1_CSR_HARD_RESET;
  taskDelay(30);

  f1ConfigWrite(id, (int *) &f1ConfigData[iFlag], F1_ALL_CHIPS);
    
  f1p[id]->adr32  = a32addr;
  f1p[id]->adr_mb = addrMB ;

  if((f1UseSDC == 0)&&(f1UseBDC == 0))
    f1EnableClk(id);

  f1ClearStatus(id,F1_ALL_CHIPS);

}


void
f1Trig(int id)
{

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    logMsg("f1Trig: ERROR : TDC in slot %d is not initialized \n",id,0,0,0,0,0);
    return;
  }

  if((f1p[id]->ctrl)&(F1_ENABLE_SOFT_TRIG))
    f1p[id]->csr = F1_CSR_TRIGGER;
  else
    logMsg("f1Trig: ERROR: Software Triggers not enabled",0,0,0,0,0,0);

}

/* Return Event count for TDC in slot id */
int
f1Dready(int id)
{

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    logMsg("f1Dready: ERROR : TDC in slot %d is not initialized \n",id,0,0,0,0,0);
    return(ERROR);
  }
  
  return((f1p[id]->ev_count)&F1_EVENT_COUNT_MASK);
}

/* Return Slot mask for modules with data avaialable */
int
f1DataScan(int pflag)
{
  int ii, count, id, dmask=0;

  for(ii=0;ii<nf1tdc;ii++) {
    id = f1ID[ii];
    count = (f1p[id]->ev_count)&F1_EVENT_COUNT_MASK;
    if(count)
      dmask |= (1<<id);

    if(pflag) printf(" TDC %2d Slot %2d  Count=%d\n",ii,id,count);
  }
  
  return(dmask);
}


int
f1GetRez(int id)
{
  int ii, rez = 0;

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    logMsg("f1GetRez: ERROR : TDC in slot %d is not initialized \n",id,0,0,0,0,0);
    return(ERROR);
  }
  
  for(ii=0;ii<F1_MAX_TDC_CHIPS;ii++) {
    if((f1p[id]->config[ii][1])&F1_HIREZ_MODE)
      rez |= (1<<ii);
  }

  return(rez);

}

int
f1SetWindow(int id, int window, int latency, int chipMask)
{
  int ii, jj, enMask;
  int exponent, refclkdiv, hsdiv;
  int tframe, winMax, latMax;
  unsigned int config[16];

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    logMsg("f1SetWindow: ERROR : TDC in slot %d is not initialized \n",id,0,0,0,0,0);
    return(ERROR);
  }

  if(chipMask<=0) chipMask = F1_ALL_CHIPS;
  enMask = f1Enabled(id);
  
  for(ii=0;ii<F1_MAX_TDC_CHIPS;ii++) {
    if(chipMask&(1<<ii)) {
      f1ConfigRead(id,config,ii);

      /* Check if window and latency are OK */
      tframe = 25*(((config[7]>>6)&0x1ff) + 2);
      winMax = (tframe*(0.4));
      latMax = (tframe*(0.9));

      if((window>winMax)||(window<=0)) {
	logMsg("f1SetWindow: Trig Window out of range. Set to %d ns\n",winMax,0,0,0,0,0);
	window = winMax;
      }
      if((latency>latMax)||(latency<=0)) {
	logMsg("f1SetWindow: Trig Latency Out of range. Set to %d ns\n",winMax,0,0,0,0,0);
	latency = winMax;
      }
	
      exponent =  ((config[10])>>8)&0x7;
      refclkdiv = 1;
      for(jj = 0; jj<exponent;jj++)
	refclkdiv = 2 * refclkdiv;
      hsdiv = (config[10])&0xFF;
      config[8] = (int) ((float)(152*hsdiv*window)/(float)(25*refclkdiv));
      config[9] = (int) ((float)(152*hsdiv*latency)/(float)(25*refclkdiv));

      f1ConfigWrite(id,config,(1<<ii));
    } 
  }

  f1ClearStatus(id, F1_ALL_CHIPS);  /* Clear any latched status bits */

  if(enMask)
    f1EnableData(id,enMask);     /* renable any chips that were enabled */

  return(OK);

}

void
f1GSetWindow(int window, int latency, int chipMask)
{
  int id, ii;

  for(ii=0;ii<nf1tdc;ii++) {
    id = f1ID[ii];
    f1SetWindow(id,window,latency,chipMask);
  }

}


int
f1WriteControl(int id, unsigned int val)
{

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    logMsg("f1WriteControl: ERROR : TDC in slot %d is not initialized \n",id,0,0,0,0,0);
    return(ERROR);
  }
  
  f1p[id]->ctrl = val;

  return((f1p[id]->ctrl)&F1_CONTROL_MASK);
}

void
f1GWriteControl(unsigned int val)
{
  int id, ii;
  
  for(ii=0;ii<nf1tdc;ii++) {
    id = f1ID[ii];
    f1WriteControl(id, val);
  }
}



int
f1Enabled(int id)
{
  int res=0;

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    logMsg("f1Enabled: ERROR : TDC in slot %d is not initialized \n",id,0,0,0,0,0);
    return(ERROR);
  }

  res = ((f1p[id]->ctrl)&0xff0000)>>16;
  
  return(res);  /* Return the output FIFO enable mask */
}


int
f1EnableData(int id, int chipMask)
{
  int ii, jj, mask=0;
  unsigned int reg, rval;

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    logMsg("f1EnableData: ERROR : TDC in slot %d is not initialized \n",id,0,0,0,0,0);
    return(ERROR);
  }
  
  if(chipMask<=0) chipMask = F1_ALL_CHIPS; /* Enable all Chips */

  /* Enable FIFOs for each chip */
  mask = (chipMask&0xff)<<16;
  f1p[id]->ctrl |= mask;

  /* Enable Edges for each chip */
  for(ii=0;ii<F1_MAX_TDC_CHIPS;ii++) {
    if((chipMask)&(1<<ii)) {
      for(jj=2;jj<6;jj++) {
	reg = (f1p[id]->config[ii][jj])&0xffff;
	reg = reg | F1_ENABLE_EDGES;
	f1p[id]->config[ii][jj] = reg;
	/* read it back and check */
	rval = (f1p[id]->config[ii][jj])&0xffff;
	if(rval != reg) {
	  logMsg("f1EnableData: Error writing Config (%x != %x) \n",rval,reg,0,0,0,0);
	}
      }
    }
  }

  return(OK);
}
void
f1GEnableData(int chipMask)
{
  int ii, id;

  for(ii=0;ii<nf1tdc;ii++) {
    id = f1ID[ii];
    f1EnableData(id,chipMask);
  }

}

void
f1EnableClk(int id)
{

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    logMsg("f1EnableClk: ERROR : TDC in slot %d is not initialized \n",id,0,0,0,0,0);
    return;
  }
  
  f1p[id]->ctrl |= F1_REF_CLK_PCB;

}

void
f1DisableClk(int id)
{

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    logMsg("f1DisableClk: ERROR : TDC in slot %d is not initialized \n",id,0,0,0,0,0);
    return;
  }
  
  f1p[id]->ctrl &= ~F1_REF_CLK_PCB;

}

void
f1EnableSoftTrig(int id)
{

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    logMsg("f1EnableSoftTrig: ERROR : TDC in slot %d is not initialized \n",id,0,0,0,0,0);
    return;
  }
  
  f1p[id]->ctrl |= F1_ENABLE_SOFT_TRIG;

}

void
f1DisableSoftTrig(int id)
{

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    logMsg("f1DisableSoftTrig: ERROR : TDC in slot %d is not initialized \n",id,0,0,0,0,0);
    return;
  }
  
  f1p[id]->ctrl &= ~F1_ENABLE_SOFT_TRIG;

}

void
f1EnableBusError(int id)
{

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    logMsg("f1EnableBusError: ERROR : TDC in slot %d is not initialized \n",id,0,0,0,0,0);
    return;
  }
  
  f1p[id]->ctrl |= F1_ENABLE_BERR;

}
void
f1GEnableBusError()
{
  int ii;

  for(ii=0;ii<nf1tdc;ii++)
    f1p[f1ID[ii]]->ctrl |= F1_ENABLE_BERR;

}


void
f1DisableBusError(int id)
{

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    logMsg("f1DisableBusError: ERROR : TDC in slot %d is not initialized \n",id,0,0,0,0,0);
    return;
  }
  
  f1p[id]->ctrl &= ~F1_ENABLE_BERR;

}


int
f1SetBlockLevel(int id, int level)
{

  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    logMsg("f1SetBlockLevel: ERROR : TDC in slot %d is not initialized \n",id,0,0,0,0,0);
    return(ERROR);
  }
  
  f1p[id]->ev_level = level;

  return((f1p[id]->ev_level)&F1_EVENT_LEVEL_MASK);

}
void
f1GSetBlockLevel(int level)
{
  int ii;

  for(ii=0;ii<nf1tdc;ii++)
    f1p[f1ID[ii]]->ev_level = level;

}

void
f1SetInputPort(int id, int fb)
{
  if(id==0) id=f1ID[0];

  if((id<=0) || (id>21) || (f1p[id] == NULL)) {
    logMsg("f1SetInputPort: ERROR : TDC in slot %d is not initialized \n",id,0,0,0,0,0);
    return;
  }

  if(fb)
    f1p[id]->ctrl |= F1_FB_SEL;   /* Set to Backplane */
  else
    f1p[id]->ctrl &= ~F1_FB_SEL; /* Set to Front Panel - Default */
}


void
f1EnableMultiBlock()
{
  int ii, id;

  if((nf1tdc <= 1) || (f1p[f1ID[0]] == NULL)) {
    logMsg("f1EnableMultiBlock: ERROR : Cannot Enable MultiBlock mode \n",0,0,0,0,0,0);
    return;
  }
  
  for(ii=0;ii<nf1tdc;ii++) {
    id = f1ID[ii];
    f1p[id]->ctrl |= F1_ENABLE_MULTIBLOCK;
    f1DisableBusError(id);
    if(id == f1MinSlot) 
      f1p[id]->ctrl |= F1_FIRST_BOARD;
    if(id == f1MaxSlot) {
      f1p[id]->ctrl |= F1_LAST_BOARD;
      f1EnableBusError(id);   /* Enable Bus Error only on Last Board */
    }
  }

}

void
f1DisableMultiBlock()
{
  int ii;

  if((nf1tdc <= 1) || (f1p[f1ID[0]] == NULL)) {
    logMsg("f1DisableMultiBlock: ERROR : Cannot Disable MultiBlock Mode\n",0,0,0,0,0,0);
    return;
  }
  
  for(ii=0;ii<nf1tdc;ii++)
    f1p[f1ID[ii]]->ctrl &= ~F1_ENABLE_MULTIBLOCK;

}


/***************************************************************************************
   JLAB F1 Signal Distribution Card (SDC) Routines

*/

STATUS 
f1SDC_Config()
{
  int ii;

  if((f1UseSDC <= 0) || (f1SDCp == NULL)) {
    logMsg("f1SDC_Config: ERROR : Cannot Configure F1 Signal Board \n",0,0,0,0,0,0);
    return(ERROR);
  }

  /* Set up for distributing Clock to all modules
       Front Panel
       Internal Clock off 
       Use External Clock          */
  for(ii=0; ii<nf1tdc; ii++) {
    f1p[f1ID[ii]]->ctrl &= ~F1_FB_SEL;
    f1p[f1ID[ii]]->ctrl &= ~F1_REF_CLK_PCB;
    f1p[f1ID[ii]]->ctrl |= F1_REF_CLK_SEL;
  }

  /* Enable SDC Clock and Sync Reset */
  f1SDCp->csr = F1_SDC_INTERNAL_CLK | F1_SDC_INTERNAL_SYNC ;


  return(OK);

}

void
f1SDC_Sync()
{

  if(f1SDCp == NULL) {
    logMsg("f1SDC_Sync: ERROR : No F1 SDC available \n",0,0,0,0,0,0);
    return;
  }
  
  f1SDCp->ctrl = F1_SDC_SYNC_RESET;

}

/***************************************************************************************
   JLAB F1 Backplane Distribution Card (BDC) Routines

*/

STATUS 
f1BDC_Config()
{
  int ii;

  if(f1UseBDC <= 0) {
    logMsg("f1BDC_Config: ERROR : Cannot Configure F1 Backplane Board \n",0,0,0,0,0,0);
    return(ERROR);
  }

  /* Set up for distributing Clock to all modules and triggers from Backplane
       Backpanel Triggers
       Internal Clock off 
       Use External Clock          */
  for(ii=0; ii<nf1tdc; ii++) {
    f1p[f1ID[ii]]->ctrl |= F1_FB_SEL;
    f1p[f1ID[ii]]->ctrl &= ~F1_REF_CLK_PCB;
    f1p[f1ID[ii]]->ctrl |= F1_REF_CLK_SEL;
  }

  /* Enable Spare Output on 1st TDC */
  f1p[f1ID[0]]->ctrl |= F1_ENABLE_SPARE_OUT;

  return(OK);

}

void
f1BDC_Sync()
{

  if(f1UseBDC <= 0) {
    logMsg("f1BDC_Sync: ERROR : No F1 BDC available \n",0,0,0,0,0,0);
    return;
  }

  /* Strobe Spare Output on 1st TDC */
  f1p[f1ID[0]]->csr = F1_CSR_PULSE_SPARE_OUT;

}



/* Test routines */

#define TEST_MAX_WORDS   32768

void
f1TestRead()
{
  int ii, retval, scanval;
  int mask = 0;
  unsigned int data[TEST_MAX_WORDS*4];

  for(ii=0;ii<nf1tdc;ii++) {
    mask |= (1<<(f1ID[ii]));
  }

  printf("F1 TDC Mask = 0x%x\n",mask);
    
  while(1) {
  
    scanval = f1DataScan(0);
    
    if(scanval == mask) {
      retval = f1ReadEvent(f1MinSlot,data,TEST_MAX_WORDS,2);
      if(retval<=0) 
	printf("Error in reading data retval=%d\n",retval);
    } else {
      taskDelay(1);
    }

  }

}

#else /* no UNIX version */

void
f1tdcLib_dummy()
{
  return;
}

#endif
