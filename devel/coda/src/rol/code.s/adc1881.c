
/* adc1881.c - Lecroy 1881 ADC board library:

 - slightly modified and renamed sparsification thresholds setting
   and readback functions written V. Gyurjyan, CLAS Online group, 04.08.98:
     adc1881spar_file2mem  - former spar_init
     adc1881spar_board2mem - former read_hardware
     adc1881spar_crate2mem - former spar_hardread
     adc1881spar_mem2board - former write_hardware
     adc1881spar_mem2crate - former spar_hardwrite
     adc1881ped_dump       - former spar_close
     adc1881ped_measure    - former ped_measure
     adc1881ped_read       - former ped_hardread

 - set of new functions ...
     adc1881spar_mem2file
     adc1881spar_enable
     adc1881spar_disable
     adc1881spar_event

*/

#ifdef VXWORKS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <sysLib.h>
#include <taskLib.h>

#include "coda.h"
#include "fbutil.h"
#include "adc1881.h"


#define GETREGS1881(slot_m,csr0_m,csr1_m) \
  if((res=fprc(slot_m,0,&csr0_m)) == 0) \
  { \
    csr0_m = csr0_m & 0x0000FFFF; /* take off manufacturer id */ \
    /*printf("csr0->0x%08x\n",csr0_m);*/ \
  } \
  else \
    printf("GETREGS1881: ERROR: res(csr0)=%d\n",res); \
  if((res=fprc(slot_m,1,&csr1_m)) == 0) \
  { \
    /*printf("csr1->0x%08x\n",csr1_m);*/ \
  } \
  else \
    printf("GETREGS1881: ERROR: res(csr1)=%d\n",res)

#define PRINTREGS1881(slot_m,csr0_m,csr1_m) \
  if((res=fprc(slot_m,0,&csr0_m)) != 0) \
    printf("PRINTREGS1881: ERROR: res(csr0)=%d\n",res); \
  else if((res=fprc(slot_m,1,&csr1_m)) != 0) \
    printf("PRINTREGS1881: ERROR: res(csr1)=%d\n",res); \
  else \
    printf("[%2d] csr0->0x%08x csr1->0x%08x\n",slot_m,csr0_m,csr1_m)

#define SETREGS1881(slot_m,csr0_m,csr1_m) \
  res = fpwc((slot_m),0,0x40000000); /* master reset */ \
  if(res!=0) printf("SETREGS1881: ERROR1=%d\n",res); \
  res = fpwc((slot_m),0,(csr0_m)); \
  if(res!=0) printf("SETREGS1881: ERROR2=%d\n",res); \
  res = fpwc((slot_m),1,(csr1_m)); \
  if(res!=0) printf("SETREGS1881: ERROR3=%d\n",res); \
  res = fpwc((slot_m),7,BROADCAST_1881); \
  if(res!=0) printf("SETREGS1881: ERROR4=%d\n",res); \
  fprel(); \
  sfi_error_decode()

extern unsigned long fpwc();
extern unsigned long fprc();
extern unsigned long fprcm();
extern unsigned long fb_frdb_1();

/* local prototypes */
static int adc1881spar_board2mem(int sl);
static int adc1881spar_mem2board(int sl);
static int adc1881ped_measure(int slot, long count);


/* conf file information about the adc slots */
static unsigned long slot[MAX_LENGTH];
static unsigned long channel[MAX_LENGTH];
static unsigned long pedestal[65];
static int value[MAX_LENGTH];
static int offset[MAX_LENGTH];

/* output buffer */
static unsigned long ped_out[MAX_LENGTH];
static int slot_out[MAX_LENGTH];
static int channel_out[MAX_LENGTH];
static int spars_out[MAX_LENGTH];

/* this variable is needed to sort the monitored 
   information over all the slots in to one file */ 
static int slot_count; 


/* */
static int nevents[NSLOT][NCHAN];
static int sum    [NSLOT][NCHAN];
static int sum2   [NSLOT][NCHAN];


/* backward compartibility functions; to be used by */
/* pedman_dump.c, pedman_init.c, pedman_read.c and pedman_write.c */
int spar_init(char *filename) {
  return(adc1881spar_file2mem(filename));
}
int spar_hardread() {
  return(adc1881spar_crate2mem());
}
int spar_hardwrite() {
  return(adc1881spar_mem2crate());
}
int spar_close(char *clonparms, char *sname, int run, char *tname) {
 return(adc1881ped_dump(clonparms, sname, run, tname));
}
int ped_hardread() {
  return(adc1881ped_read());
}

/* enable sparsification for all ADCs in 'adc' list */

int
adc1881spar_enable(Csr roccsr, int *adc)
{
  int slot, res;
  unsigned int csr0, csr1;

  printf("adc1881spar_enable reached\n");
  for(slot=0; slot<NSLOT; slot++)
  {
    if(adc[slot]==1) /* if board is adc */
    {
      GETREGS1881(slot,csr0,csr1);
      csr1 = (csr1 & 0xBFFFFFFF) + 0x40000000; /* set sparsification bit */
      SETREGS1881(slot,csr0,csr1);
      PRINTREGS1881(slot,csr0,csr1);
    }
  }

  return(0);
}


/* disable sparsification for all ADCs in 'adc' list */

int
adc1881spar_disable(Csr roccsr, int *adc)
{
  int slot, channel, res;
  unsigned int csr0, csr1;

  printf("adc1881spar_disable reached\n");
  for(slot=0; slot<NSLOT; slot++)
  {
    if(adc[slot]==1) /* if board is adc */
    {
      GETREGS1881(slot,csr0,csr1);
      csr1 = csr1 & 0xBFFFFFFF; /* reset sparsification bit */
      SETREGS1881(slot,csr0,csr1);
      PRINTREGS1881(slot,csr0,csr1);
    }
    for(channel=0; channel<NCHAN; channel++)
    {
      nevents[slot][channel] = 0;
      sum    [slot][channel] = 0;
      sum2   [slot][channel] = 0;
    }
  }

  return(0);
}

/* process one event pointed by 'buf' and fills local static arrays;
   adc[slot#] =1 if adc1881, =0 otherwise */

int
adc1881spar_event(int *adc, unsigned int *buffer, int nw)
{
  int i, slot, slot_old, channel, data;
  unsigned int *buf;

  slot_old = -1;
  buf = buffer;
  for(i=0; i<nw; i++)
  {
    slot = ((*buf)>>27)&0x1F;
	/*logMsg("slot=%d\n",slot,0,0,0,0,0);*/
    if(adc[slot]==1) /* if board is adc */
    {
	  /*gMsg("adc\n",0,0,0,0,0,0);*/
      if(slot != slot_old) /* header word */
      {
        slot_old = slot;
      }
      else                 /* data word */
      {
        channel = ((*buf)>>17)&0x3F;
        data = (*buf)&0x1FFF;
		/*gMsg("ch %d data %d\n",channel,data);*/
        nevents[slot][channel] ++;
        sum[slot][channel] += data;
        sum2[slot][channel] += data*data;
/*if(slot==25 && channel<5) printf("data=%d -> sum2[%2d][%2d]=%d\n",data,slot,channel,sum2[slot][channel]);*/
      }
    }
/*if(slot==25 && channel==5) printf("\n");*/

    buf++;
  }

  return(0);
}


/* calculate pedestal 'mean' and 'rms' for every channel
   and write results to the file
   $CLON_PARMS/pedman/archive/<rocname>_<session>_<runnum>.ped
*/

int
adc1881spar_mem2file(int *adc, char *tname, char *sname, int runnum, int offset)
{
  FILE *fp;
  char *clonparms = "/usr/local/clas/parms"; /* should getenv() it */
  char filename[1024];
  int slot, channel;
  int tmp, mean;
  float rms;

  sprintf(filename,"%s/pedman/archive/%s_%s_%06d.ped",
             clonparms,tname,sname,runnum);

  if((fp = fopen(filename,"r") ) != NULL)
  {
    printf("adc1881spar_mem2file: ERROR: file >%s< exists - do nothing\n",
           filename);
    fclose(fp);
    return(0);
  }

  if((fp = fopen(filename,"w") ) != NULL)
  {
    printf("adc1881spar_mem2file: open output file >%s<\n",filename);

    for(slot=0; slot<NSLOT; slot++)
    {
      if(adc[slot]==1) /* if board is adc */
      {
        for(channel=0; channel<NCHAN; channel++)
        {
          if(nevents[slot][channel] > 0)
          {
            mean = sum[slot][channel] / nevents[slot][channel];
            tmp = sum2[slot][channel] / nevents[slot][channel] - mean * mean;
/*printf("tmp=%d\n",tmp);*/
            if(tmp > 0)
            {
              rms = sqrt((float)tmp);
/*printf("rms1=%f\n",rms);*/
            }
            else if(tmp < 0)
            {
              rms = sqrt((float)(-tmp));
/*printf("rms2=%f\n",rms);*/
            }
            else
            {
              rms = 0.0;
            }
          }
          else
          {
            mean = 0;
            rms = 0.0;
          }
          fprintf(fp,"%2d %3d %5d %6.3f %3d\n",slot,channel,mean,rms,offset);
        }
      }
    }

    if(fclose(fp) != 0)
    {
      printf("adc1881spar_mem2file: ERROR closing the output file >%s<\n",
             filename);
    }
  }
  else
  {
    printf("adc1881spar_mem2file: ERROR: can not open output file >%s<\n",
           filename);
  }

  return(0);
}




int
adc1881spar_file2mem(char *fname)
{
  FILE *fp;
  char *clonparms = "/usr/local/clas/parms"; /* should getenv() it */
  char filename[1024];
  float sigma;
  int i, j, nread;

  /* unused slot mark NO_ADC */

  for(i=0; i<=(MAX_LENGTH-1); i++)
  {
    slot[i]     = NO_ADC;
    slot_out[i] = NO_ADC;
    channel[i]  = NO_ADC;
    value[i]    = NO_ADC;
    offset[i]   = NO_ADC;
  }

  sprintf(filename,"%s/pedman/archive/%s",clonparms,fname);

  if((fp = fopen(filename,"r") ) != NULL)
  {
    printf("adc1881spar_file2mem: open input file >%s<\n",filename);

    i=0;
    while(1)
    {
      nread = fscanf(fp,"%d %d %d %f %d\n",
          &slot[i],&channel[i],&value[i],&sigma,&offset[i]);
      /*printf("nread=%d -> %d %d %d %f %d\n",nread,
        slot[i],channel[i],value[i],sigma,offset[i]);*/
      if(nread == EOF)
      {
        printf("adc1881spar_file2mem: end-of-file\n");
        break;
      }
      if(nread != 5)
      {
        printf("adc1881spar_file2mem: ERROR reading file: nread=%d\n",nread);
        break;
      }
      i++;
    }

    if(fclose(fp) != 0)
    {
      printf("adc1881spar_file2mem: ERROR closing the input file >%s<\n",
             filename);
    }
  }
  else
  {
    printf("adc1881spar_file2mem: ERROR: can not open input file >%s<\n",
           filename);
  }

  return(0);
}



/* -------------------------------------------------- */

static int
adc1881spar_board2mem(int sl)
{
  static int i, j;
  unsigned int the_addr, th;

  slot_count++;

  fpwc(sl, 0, 0x40000000); /* master reset */
  sfi_error_decode();

  for(i=0; i<64; i++) 
  {
    the_addr = BASE_ADDR + i; /* read the hardware */ 
    fprc(sl, the_addr, &th); 
    sfi_error_decode();
    j=slot_count*64 + i;
    slot_out[j] = sl;
    channel_out[j] = i;
    spars_out[j] = th;
  }

  fprel(); 
  fpwc(sl, 0, 0x00000104); /* enable gate and wait */
  sfi_error_decode();
  fpwc(sl, 1, 0x40000080); /* enable sparsification */
  sfi_error_decode();
  fpwc(sl, 7, BROADCAST_1881); /* set broadcast class */
  sfi_error_decode();
  fprel();

  return(0);
}

/*-----------------------------------------------*/

static int
adc1881spar_mem2board(int sl)
{
  static int i, jw;
  unsigned int the_addr, th;

  slot_count++;
  fpwc(sl, 0, 0x40000000); /* master reset */
  sfi_error_decode();

  for(i=0; i<64; i++) 
  {
    the_addr = BASE_ADDR + i; /* write in to the hardware */ 
    jw = slot_count*64 + i;
    sl = slot[jw];
    th = value[jw] + offset[jw];
    fpwc(sl, the_addr, th); 
    sfi_error_decode();
  }    

  fprel(); 
  fpwc(sl, 0, 0x00000104); /* enable gate and wait */
  sfi_error_decode();
  fpwc(sl, 1, 0x40000080); /* enable sparsification */
  sfi_error_decode();
  fpwc(sl, 7, BROADCAST_1881); /* set broadcast class */
  sfi_error_decode();
  fprel();

  return(0);
}

/*-----------------------------------------------*/

int
adc1881spar_crate2mem()
{
  unsigned long ladr;
  int slot_old;
  int j;

  slot_old = -1;
  slot_count = -1;

  ladr = 0;

  sysBusToLocalAdrs(0x39, (char *)SFI_VME_ADDR, (char **)&ladr);

  fb_init_1(ladr);

  for(j=0; j<=(MAX_LENGTH-1); j++)
  {
    if(slot[j] != slot_old && slot[j] != NO_ADC) 
    {
      slot_old = slot[j];     
      adc1881spar_board2mem(slot[j]);
    }
  }

  return(0);
}

/*-----------------------------------------------*/

int
adc1881spar_mem2crate()
{
  unsigned long ladr;
  int slot_old;
  int j;

  slot_old = -1;
  slot_count = -1;


  ladr = 0;
  
  sysBusToLocalAdrs(0x39, (char *)SFI_VME_ADDR, (char **)&ladr);
  
  fb_init_1(ladr);

  for(j=0; j<=(MAX_LENGTH-1); j++)
  {
    if(slot[j] != slot_old && slot[j] != NO_ADC) 
    {
      slot_old = slot[j];
      adc1881spar_mem2board(slot[j]);
    }
  }

  return(0);
}





/***********************************************************************
 read pedestal measurement results from memory and write it into the file
 $CLON_PARMS/pedman/spar_archive/<rocname>_<session>_<runnum>_hard.spar
************************************************************************/

int
adc1881ped_dump(char *clonparms, char *sname, int run, char *tname)
{
  FILE *fpout;
  int i;
  char filename[111];

  sprintf(filename,"%s/pedman/spar_archive/%s_%s_%06d_hard.spar",
             clonparms,tname,sname,run);

  if( (fpout = fopen(filename,"w") ) != NULL)
  {
    for(i=0; i<=(MAX_LENGTH-1); i++)
    {
      if(slot[i] != NO_ADC)
      {
        fprintf(fpout,"%6d %6d %6d %6d\n",slot_out[i], channel_out[i],spars_out[i],ped_out[i]);     
      }
    }

    if(fclose(fpout) !=0)
    {
      printf("Error closing the output file >%s<\n",filename);
    }
  }
  else
  {
    printf("adc1881ped_dump: ERROR: can not open output file >%s<\n",filename);
  }

  return(0);
}




/***********************************************************************
 measure pedestals using internal gate and put results into the memory
************************************************************************/

int
adc1881ped_read()
{
  int j, slot_old;
  unsigned long ladr, res, dmaptr, lenb, rb;

  slot_old = -1;
  slot_count = -1;

  /* Initialize SFI interface and other variables. Get local address *
   * from which to access the SFI (A24/D32 VME slave AM = 0x39       */
  res = (unsigned long) sysBusToLocalAdrs(0x39,(char *)SFI_VME_ADDR,(char **)&ladr);
  if(res != 0)
  {
    printf("Error Initializing SFI res=%d\n",res);
    return(1);
  }
  else
  {
    InitSFI(ladr);
    InitFastbus(0x20,0x33);
  }

  for(j=0; j<=(MAX_LENGTH-1); j++)
  {
    if(slot[j] != slot_old && slot[j] != NO_ADC)
    {
      slot_old = slot[j];     
      adc1881ped_measure(slot[j],NEVENT);

      /****** Reset and put sparsification back */
      res = fpwc(slot[j],0,0x00001104);
      if(res != 0)
      { 
        printf("ERROR: Program ADC CSR 0\n");
      }

      res = fpwc(slot[j],1,0x40000042);
      if(res != 0)
      { 
        printf("ERROR: Program ADC CSR 1\n");
      }
      fpwc(slot[j], 7, BROADCAST_1881); /* set broadcast class */

      sfi_error_decode();
      fprel();
    }
  }

  return(0);
}

static int
adc1881ped_measure(int slt, long count)
{
  unsigned long datascan, scanmask, ii, iz, len;
  static unsigned long adc[MAX_LENGTH];
  unsigned long moduleID;
  unsigned long dmaptr, lenb, rb;
  unsigned long res, sl, chan;
  int i, job;

  slot_count++;
  for(i=0; i<65; i++) pedestal[i] = 0;

  scanmask = (1<<slt);
  len = MAX_LENGTH;
  if(count <= 0) count= 1;

  /* SFI uses VME AM = 0x0b or 0x09 (Non-privledged  Data transfer)     *
   * to transfer fastbus data to local CPU memory                  */
  if((res = sysLocalToBusAdrs(0x09,(char *)adc,(char **)&dmaptr)) != 0)
  {
    printf("Error Initializing SFI res=%d\n",res);
    return(1);
  }
  else
  {
    printf("Block Read data stored at address 0x%x ,dmaptr = 0x%x\n",&adc[0],dmaptr);
  }

  /****** Reset/Clear ADC *********/
  res = fpwc(slt,0,0x40000000);
  if(res != 0)
  { 
    printf("ERROR: Reset ADC\n");
    goto fooy;
  }

  /****** Program ADC *******/
  res = fpwc(slt,0,0x00000104);
  if(res != 0)
  { 
    printf("ERROR: Program ADC CSR 0\n");
    goto fooy;
  }

  res = fpwc(slt,1,0x20000040);
  if(res != 0)
  { 
    printf("ERROR: Program ADC CSR 1\n");
    goto fooy;
  }

  /****** Read ADC Module ID *********/
  res = fprc(slt,0,&moduleID);
  if(res != 0)
  { 
    printf("ERROR: Read ADC ID\n");
    goto fooy;
  }
  else
  {
    printf("Module ID = 0x%x\n",moduleID);
  }

  /****** Readout Loop *******/
  printf("Executing Readout loop %d times...\n",count);
  for(iz=0; iz<count; iz++)
  {
    res = fpwc(slt,0,0x80);
    if(res != 0)
    { 
      printf("ERROR: Test Gate ADC\n");
      goto fooy;
    }

    ii=0;
    datascan = 0;
    while((ii<10) && ((datascan&scanmask) != scanmask))
    {
      res = fprcm(9,0,&datascan);
      if(res != 0)
      { 
        printf("ERROR: Sparse Data Scan\n");
        goto fooy;
      }
      ii++;
    }

    if(ii<10)
    {
      res = fpwc(slt,0,0x400);
      if(res != 0)
      { 
        printf("ERROR: Load Next Event\n");
        goto fooy;
      }
      
      lenb = len<<2;
      res = fb_frdb_1(slt,0,dmaptr,lenb,&rb,1,0,1,0,0x0a,0,0,1);
      if((rb > (lenb+4))||(res != 0))
      {
        printf("ERROR: Block Read   res = 0x%x maxbytes = %d returnBytes = %d \n",res,lenb,rb);
        goto fooy;
      }
    }

    for(i=0; i<65; i++)
    {
      sl = (adc[i]>>27)&~(MEK_1881<<5);
      chan = (adc[i]<<9)>>26;

      pedestal[i] += (adc[i] & ~(MEK_1881<<14));

      /*printf("slot = %d channel = %d pedestal = %d \n",sl,chan,pedestal[i]);*/
    }
    /*   taskDelay(30);*/ /* wait a little before next trigger */
    /*    printf("=============================================\n");*/
  } /* End for(iz=0....) */

  for(i=1; i<65; i++)
  {
    pedestal[i] = pedestal[i]/count;
    job = slot_count*64+(i-1);
    ped_out[job] = pedestal[i];
    /*printf("slot = %d channel = %d pedestal = %d count = %d \n",slt,job,ped_out[job],count);*/
  }
  printf("Done with %d loop(s)\n",iz);
  fprel();

  return(0);
  
fooy:

  sfi_error_decode();
  fprel();

  return(1);
}



/*-----------------------------------------------*/

void
adc1881test()
{
  int slt = 5;
  unsigned long res, word;

  res = fprc(slt,0,&word);
  printf("res=%d\n",res);
  printf("word=0x%08x\n",word);

  fpwc(slt, 1, 0x00000040);

  res = fprc(slt,1,&word);
  printf("res=%d\n",res);
  printf("word=0x%08x\n",word);

  fpwc(slt, 1, 0x40000040);
  
  res = fprc(slt,1,&word);
  printf("res=%d\n",res);
  printf("word=0x%08x\n",word);

  fpwc(slt, 1, 0x00000040);

  res = fprc(slt,1,&word);
  printf("res=%d\n",res);
  printf("word=0x%08x\n",word);

  return;
}

#else /* no UNIX version */

void
adc1881_dummy()
{
  return;
}

#endif








