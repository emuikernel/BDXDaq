/* 
  fbutil.c - FASTBUS utilities

    (former hardiag.c written by V. Gyurjyan, CLAS Online group
    03.11.98
    modified 03.31.00
    modified 05.25.00)

  new functions:
    GetMaxEventLength

*/

#ifdef VXWORKS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sysLib.h>
#include <taskLib.h>

#include "coda.h"
#include "fbutil.h"

#define MIN(x,y) ((x) < (y) ? (x) : (y))

extern STATUS sysBusToLocalAdrs();
extern STATUS sysLocalToBusAdrs();

extern unsigned long sfiSeq2VmeFifoVal;
extern unsigned long fpac ();
extern unsigned long fpwc ();
extern unsigned long fprc ();
extern unsigned long fprel();
extern unsigned long fb_fwc_1();
extern unsigned sfi_error_decode();
extern unsigned fb_frdb_1();


/* globals - used by readout lists and corresponding functions, */
/* must be declared as 'extern' there */
struct brset adc81brset;
struct brset tdc77brset;
struct brset tdc77Sbrset;
struct brset tdc72brset;
struct brset tdc75brset;
struct hardmap map;
struct csr str;
int totalPTim1 = 0;
int totalPTim2 = 0;



/* for PRIMEX: returns 1 if ADC1881 do exist in specified slot */
int
isADC1881(int slot)
{
  if(slot<1||slot>25) return(0);
  return(map.map_adc[slot]);
}



/* statics - for the local file use only */

/*
   calculates maximum amount of bytes we can get from all FASTBUS boards
   in the current crate
 */
int
GetMaxEventLength(HARDMAP map, CSRS csr)
{
  int slot, nwords, maxhits;

  nwords = 2; /* count CODA header */
  for(slot=0; slot<NSLOT; slot++)
  {
    if(map.map_adc[slot])
    {
      nwords += Adc81Length;
    }
    else if(map.map_tdc72[slot]||map.map_tdc75[slot])
    {
      nwords += Tdc72Length;
    }
    else if(map.map_tdc77[slot]||map.map_tdc77S[slot])
    {
      /* get 'maxhits' from board registers setup */
      /*if(map.map_tdc77[slot])
      {
        maxhits = csr.csr18_1877 & 0xF;
      }
      else
      {
        maxhits = csr.csr18_1877S & 0xF;
      }
      if(maxhits==0)*/ maxhits = 16; /* use maximim just in case */

      nwords += MIN(Tdc77Length,(96*maxhits+1));
    }
  }

  return(nwords<<2); /* return length in bytes */
}

/*
   creates a rocmap file
*/
struct hardmap
what_hardware(int pid, char *location)
{
  volatile unsigned long csr;
  FILE *fpout;
  char filename_out[111];
  int islot, i;
  int val;
  unsigned long ladr;
  struct hardmap temp_map;

  ladr = 0;
  sysBusToLocalAdrs(0x39, (void *)SFI_VME_ADDR, (void *)&ladr);
  fb_init_1(ladr);

  /*...............> loop over all the slots of the Fastbus crate */
  for(islot=0; islot<NSLOT; islot++)
  {
    /*................................> reset the map array elements */
    temp_map.map_adc[islot]     = 0;
    temp_map.map_tdc72[islot]   = 0;
    temp_map.map_tdc75[islot]   = 0;
    temp_map.map_tdc77[islot]   = 0;
    temp_map.map_tdc77S[islot]  = 0;
    temp_map.map_unknown[islot] = 0;
    temp_map.map_empty[islot]   = 0;

    if(fpac(islot,0) != 0) temp_map.map_empty[islot] = 1;
    fprel();
    /* sfi_error_decode(2); */

    /*................> master reseting is the same for ADC's and TDC's */
	if(fpwc(islot,0,0x40000000) != 0) temp_map.map_empty[islot] = 1; 

    /*...............................................> reading the CSR0 */ 
	if(fprc(islot,0,&csr) == 0)
    { 
	  val = csr & BOARD_ID_MASK;
	  if(val == B1881M || val == B1881)
      {
        temp_map.map_adc[islot] = 1;
        printf("what_hardware: ADC1881  at slot #%d\n",islot);
      }
	  else if(val == B1872 || val == B1872A)
      {
        temp_map.map_tdc72[islot] = 1;
        printf("what_hardware: TDC1872  at slot #%d\n",islot);
      }
	  else if(val == B1875 || val == B1875A)
      {
        temp_map.map_tdc75[islot] = 1;
        printf("what_hardware: TDC1875  at slot #%d\n",islot);
      }
	  else if(val == B1877)
      {
        temp_map.map_tdc77[islot] = 1;
        printf("what_hardware: TDC1877  at slot #%d\n",islot);
      }
	  else if(val == B1877S)
      {
        temp_map.map_tdc77S[islot] = 1;
        printf("what_hardware: TDC1877S at slot #%d\n",islot);
      }
	  else if(temp_map.map_empty[islot] == 0)
      {
        temp_map.map_unknown[islot] = 1;
        printf("what_hardware: unknown board at slot #%d, csr=0x%08x\n",
               islot,csr);
      }
	}
    else
    { 
	  sfi_error_decode(3);
	  temp_map.map_empty[islot] = 1;
    }
    fprel();
  }
  sprintf(filename_out,"%s/ROC%d.hardmap",location,pid);
  
  if( (fpout = fopen(filename_out,"w") ) == NULL)
  { 
    printf("what_hardware: ERROR: can't open output file \n") ;
  }

  fprintf(fpout,
    "Slot    ADC1881  TDC1872  TDC1875  TDC1877  TDC1877S  Unknown  Empty \n");
  fprintf(fpout,
    "-------------------------------------------------------------------- \n");
  for(i=0; i<NSLOT; i++)
  {
    fprintf(fpout,"%3d %8d %8d %8d %8d %8d %8d %8d \n",i,
	    temp_map.map_adc[i], temp_map.map_tdc72[i], temp_map.map_tdc75[i],
	    temp_map.map_tdc77[i], temp_map.map_tdc77S[i],
	    temp_map.map_unknown[i], temp_map.map_empty[i]);
  }
  
  if(fclose(fpout) !=0)
  {
    printf("what_hardware: ERROR closing the output file \n");
  }

  /*...............................> End reading hardware map */
  /*.....................................................................*/
  return(temp_map);
}


/*
   defines the status regiters
*/
struct csr
what_settings(int pid, char *location)
{
  FILE *fp;
  char filename_in[111];
  unsigned int bit;
  char ss[333];

  struct csr temp;

  unsigned int *pt; 
 
  temp.csr0_1877   = DefCsr0_1877;
  temp.csr1_1877   = DefCsr1_1877;
  temp.csr18_1877  = DefCsr18_1877;

  temp.csr0_1877S  = DefCsr0_1877S;
  temp.csr1_1877S  = DefCsr1_1877S;
  temp.csr18_1877S = DefCsr18_1877S;

  temp.csr0_1881   = DefCsr0_1881;
  temp.csr1_1881   = DefCsr1_1881;

  temp.csr0_1872   = DefCsr0_1872;

  sprintf(filename_in,"%s/ROC%d.hardcnf",location,pid);
  
  if((fp = fopen(filename_in, "r")) == NULL)
  {
    printf("Program is unable to read config file >%s<\n",filename_in);
    printf("Using the default settigs\n");
  }
  else
  {
    while(fscanf(fp, "%s", ss) != EOF)
    {
      if(strcmp(ss,"1877_CSR0") == 0)         pt = &temp.csr0_1877;
      else if(strcmp(ss,"1877_CSR1") == 0)    pt = &temp.csr1_1877;
      else if (strcmp(ss,"1877_CSR18") == 0)  pt = &temp.csr18_1877;      
      else if(strcmp(ss,"1877S_CSR0") == 0)   pt = &temp.csr0_1877S;
      else if(strcmp(ss,"1877S_CSR1") == 0)   pt = &temp.csr1_1877S;
      else if (strcmp(ss,"1877S_CSR18") == 0) pt = &temp.csr18_1877S;      
      else if(strcmp(ss,"1881_CSR0") == 0)    pt = &temp.csr0_1881;
      else if (strcmp(ss,"1881_CSR1") == 0)   pt = &temp.csr1_1881;      
      else if(strcmp(ss,"1872_CSR0") == 0)    pt = &temp.csr0_1872;

      while (strcmp(ss,"!") == 0) 
      {
	    fscanf(fp, "%d %s ", &bit, ss);
	    *pt = *pt << 1;
      	if( bit==0 || bit==1 ) *pt = *pt | bit;
	    bit = 0;
      }
    }
    fclose(fp);
    printf("For 1877S: CSR0 : 0x%08x, CSR1 : 0x%08x, CSR18 : 0x%08x\n",
           temp.csr0_1877S, temp.csr1_1877S, temp.csr18_1877S);
    printf("For 1881: CSR0 : 0x%08x, CSR1 : 0x%08x\n",
           temp.csr0_1881, temp.csr1_1881);
    printf("For 1872: CSR0 : 0x%08x\n",temp.csr0_1872);
  }

  return(temp);
}


/*============================================================= */
unsigned int
create_spds_mask(Hardmap rocmap)
{
  int spds_mask;
  int i;
  
  spds_mask = 0x00000000;

  /*...............> Create spds_mask */
  for(i=0; i<NSLOT; i++)
  {
    if(rocmap->map_adc[i])
    {
      spds_mask |= (0x00000001 << i);
    }
    else if(rocmap->map_tdc72[i] || rocmap->map_tdc75[i])
    { 
      spds_mask |= (0x00000001 << i);
    } 
    else if(rocmap->map_tdc77[i] || rocmap->map_tdc77S[i])
    { 
      spds_mask |= (0x00000001 << i);
    } 
  }

  /*printf ("spds_mask: 0x%x\n",spds_mask); DEBUG */
  return(spds_mask);
}



/*
   if blk==1 - use blockread,
         ==0 - read single boards
*/

struct brset
program(int arr[], int blk, Csr roccsr,
         void (*program_module) (int, char *,  Csr roccsr))
{
  int i, temp_first, temp_last;
  struct brset bread;

  bread.readflag = 0;

  for(i=0; i<NSLOT; i++)
  {
    bread.lastsing[i] = 0;
    bread.brlength[i] = 0;
  }

  if(blk==1) /* blockread is allowed */
  {
    for(i=0; i<NSLOT; i++)
    {
      if(arr[i])
      {
        bread.readflag = 1;

        /* i=0 case */
        if(i==0 && arr[i+1])
        {
          (*program_module)(i,"first",roccsr);
          temp_first = i;
          continue;
        }
        else if(i==0 && !arr[i+1])
        {
          (*program_module)(i,"single",roccsr);
          bread.lastsing[i] = SINGLE;
          continue;
        }
      
        /* i=25 case */
        if(i==25 && arr[i-1])
        {
          (*program_module)(i,"last",roccsr);
          bread.lastsing[i] = LAST;
          temp_last = i;
          bread.brlength[i] = temp_last - temp_first + 1;
          temp_last = 0;
          temp_first = 0;
          continue;
        }
        else if(i==25 && !arr[i-1])
        {
          (*program_module)(i,"single",roccsr);
          bread.lastsing[i] = SINGLE;
          continue;
        }

        /* all the other cases */
        if(arr[i-1] && arr[i+1])
        {
          (*program_module)(i,"middle",roccsr);
        }
        else if(!arr[i-1] && arr[i+1])
        {
          (*program_module)(i,"first",roccsr);
          temp_first = i;
        }
        else if(arr[i-1] && !arr[i+1])
        {
          (*program_module)(i,"last",roccsr);
          bread.lastsing[i] = LAST;
          temp_last = i;
          bread.brlength[i] = temp_last - temp_first + 1;
          temp_last = 0;
          temp_first = 0;
        }
        else if(!arr[i-1] && !arr[i+1])
        {
          (*program_module)(i,"single",roccsr);
          bread.lastsing[i] = SINGLE;
        }
      }
    }
  }
  else /* single boards readout */
  {
    for(i=0; i<NSLOT; i++)
    {
      if(arr[i])
      {
        bread.readflag = 1;
        (*program_module)(i,"single", roccsr);
        bread.lastsing[i] = SINGLE;
      }
    }
  }

  return(bread);
}


#define MULTIBLOCK_CONFIG \
  if(strcmp(str,"single")==0)      this_csr0 |= BYPASS; \
  else if(strcmp(str,"first")==0)  this_csr0 |= END_LINK; \
  else if(strcmp(str,"middle")==0) this_csr0 |= MIDDLE_LINK; \
  else if(strcmp(str,"last")==0)   this_csr0 |= PRIMARY_LINK; \
  sadr = 0; \
  fpwc((int)i,sadr,0x40000000); /* master reset */ \
  fpwc((int)i,sadr,this_csr0)


/*============================================================= */
void
_adc1881(int i, char *str, Csr roccsr)
{
  unsigned int this_csr0;
  unsigned int sadr;

  this_csr0 = roccsr->csr0_1881;
  MULTIBLOCK_CONFIG;
  printf("_adc1881 ===>%d %s csr0=%x (%x) csr1=%x\n",
          i,str,roccsr->csr0_1881,this_csr0,roccsr->csr1_1881);        

  /* csr1 register */
  sadr = 1;   
  fpwc((int)i,sadr,roccsr->csr1_1881);

  /* csr7 register */
  sadr = 7;  
  fpwc((int)i,sadr,BROADCAST_1881);

  fprel(); 
  sfi_error_decode();
}

/*============================================================= */
void
_tdc1877(int i, char *str, Csr roccsr)
{    
  unsigned int this_csr0;
  unsigned int sadr;

  this_csr0 = roccsr->csr0_1877;
  MULTIBLOCK_CONFIG;
  printf("_tdc1877 ===>%d %s csr0=%x (%x) csr18=%x csr1=%x\n",
       i,str,roccsr->csr0_1877,this_csr0,roccsr->csr18_1877,roccsr->csr1_1877);

  sadr = 18;   
  fpwc((int)i,sadr,roccsr->csr18_1877);

  sadr = 1;  
  fpwc((int)i,sadr,roccsr->csr1_1877);   

  sadr = 7 ;
  fpwc((int)i,sadr,BROADCAST_1877);

  fprel();
  sfi_error_decode();
}
  
/*============================================================= */
void
_tdc1877S(int i, char *str, Csr roccsr)
{    
  unsigned int this_csr0;
  unsigned int sadr;

  this_csr0 = roccsr->csr0_1877S;
  MULTIBLOCK_CONFIG;
  printf("DEBUG === IN THE _tdc1877S ==>%d %s csr0=%x (%x) csr18=%x csr1=%x\n",
    i,str,roccsr->csr0_1877S,this_csr0,roccsr->csr18_1877S,roccsr->csr1_1877S);

  sadr = 18;   
  fpwc((int)i,sadr,roccsr->csr18_1877S);

  sadr = 1;  
  fpwc((int)i,sadr,roccsr->csr1_1877S);   
    
  sadr = 7 ;
  fpwc((int)i,sadr,BROADCAST_1877);
    
  fprel();
  sfi_error_decode();
}
  
/*============================================================= */
void
_tdc1872(int i, char *str, Csr roccsr)
{   
  unsigned int this_csr0;
  unsigned int sadr;

  printf("_tdc1872 ===>%d %s csr0=%x\n",i,str,roccsr->csr0_1872);
  sadr = 0;
  fpwc((int)i,sadr,0x08000000);
  /* must be 950ns delay here !!! see manual page 4-2 */
  fpwc((int)i,sadr,0x40000000);
  fpwc((int)i,sadr,roccsr->csr0_1872);

  /*
  fb_fwc_1((int)i,sadr,0x08000000,1,1,0,1,0,1,1);
  fb_fwc_1(0,sadr,0x40000000,1,1,1,1,0,1,1);
  fb_fwc_1(0,sadr,roccsr->csr0_1872,1,1,1,1,0,1,1);
  */

  fprel(); 
  sfi_error_decode();
}

/*=============================================================
   if blk==1 or 2 - use block read
         ==0 - read as single boards
*/
void
program_hardware(Hardmap pmap, Csr roccsr, int blkrd)
{
  int blk;

  blk = blkrd;
  if(blk>1) blk = 1;

  adc81brset = program(pmap->map_adc,     blk, roccsr, _adc1881);
  tdc77brset = program(pmap->map_tdc77,   blk, roccsr, _tdc1877);
  tdc77Sbrset = program(pmap->map_tdc77S, blk, roccsr, _tdc1877S);
  tdc72brset = program(pmap->map_tdc72,   blk, roccsr, _tdc1872);
  tdc75brset = program(pmap->map_tdc75,   blk, roccsr, _tdc1872);
}

















/*=============================================================
   program boards for 64-bit transfer
*/

/*============================================================= */
void
_adc1881_64(int i, char *str, Csr roccsr)
{
  unsigned int this_csr0;
  unsigned int sadr;

  this_csr0 = roccsr->csr0_1881;
  MULTIBLOCK_CONFIG;
  printf("_adc1881 ===>%d %s csr0=%x (%x) csr1=%x\n",
          i,str,roccsr->csr0_1881,this_csr0,roccsr->csr1_1881);        

  /* csr1 register */
  sadr = 1;   
  fpwc((int)i,sadr,roccsr->csr1_1881);

  /* csr7 register */
  sadr = 7;  
  fpwc((int)i,sadr,BROADCAST_1881);

  fprel(); 
  sfi_error_decode();
}

/*============================================================= */
void
_tdc1877_64(int i, char *str, Csr roccsr)
{    
  unsigned int this_csr0;
  unsigned int sadr;

  this_csr0 = roccsr->csr0_1877;
  MULTIBLOCK_CONFIG;
  printf("_tdc1877 ===>%d %s csr0=%x (%x) csr18=%x csr1=%x\n",
       i,str,roccsr->csr0_1877,this_csr0,roccsr->csr18_1877,roccsr->csr1_1877);

  sadr = 18;   
  fpwc((int)i,sadr,roccsr->csr18_1877);

  sadr = 1;  
  fpwc((int)i,sadr,roccsr->csr1_1877);   

  sadr = 7 ;
  fpwc((int)i,sadr,BROADCAST_1877);

  fprel();
  sfi_error_decode();
}
  
/*============================================================= */
void
_tdc1877S_64(int i, char *str, Csr roccsr)
{    
  unsigned int this_csr0;
  unsigned int sadr;

  this_csr0 = roccsr->csr0_1877S;
  MULTIBLOCK_CONFIG;
  printf("DEBUG === IN THE _tdc1877S ==>%d %s csr0=%x (%x) csr18=%x csr1=%x\n",
    i,str,roccsr->csr0_1877S,this_csr0,roccsr->csr18_1877S,roccsr->csr1_1877S);

  sadr = 18;   
  fpwc((int)i,sadr,roccsr->csr18_1877S);

  sadr = 1;  
  fpwc((int)i,sadr,roccsr->csr1_1877S);   
    
  sadr = 7 ;
  fpwc((int)i,sadr,BROADCAST_1877);
    
  fprel();
  sfi_error_decode();
}

#define PROGRAM_MODULE(i_m,pos_m) \
  if(pmap->map_adc[i_m])         _adc1881_64(i_m,pos_m,roccsr); \
  else if(pmap->map_tdc77[i_m])  _tdc1877_64(i_m,pos_m,roccsr); \
  else if(pmap->map_tdc77S[i_m]) _tdc1877S_64(i_m,pos_m,roccsr); \
  else printf("ERROR IN PROGRAM_MODULE !!!\n")

void
program_hardware_64(Hardmap pmap, Csr roccsr, int low64slot, int high64slot)
{
  int i;

  printf("program_hardware_64 reached\n");

  if(low64slot == high64slot)
  {
    PROGRAM_MODULE(low64slot,"single");
  }
  else
  {
    PROGRAM_MODULE(low64slot,"first");

    for(i=low64slot+1; i<=high64slot-1; i++)
    {
      PROGRAM_MODULE(i,"middle");
    }

    PROGRAM_MODULE(high64slot,"last");
  }
}
















/* TIME PROFILE functions - for DAQ performance tests */

/* sets execution time for 1st and 2nd readout lists (microsec) */
int
fbutilProfileSetExecutionTime1(int time)
{
  totalPTim1 = time*25;
  return(totalPTim1/25);
}
int
fbutilProfileGetExecutionTime1()
{
  return(totalPTim1/25);
}

int
fbutilProfileSetExecutionTime2(int time)
{
  totalPTim2 = time*25;
  return(totalPTim2/25);
}
int
fbutilProfileGetExecutionTime2()
{
  return(totalPTim2/25);
}


void
test_profile()
{
  PROFILE_VAR;
  int i, j, k;

  logMsg("sysClkRateGet returns %d\n",sysClkRateGet(),0,0,0,0,0);
  logMsg("set execution time to %d microsec\n",
    fbutilProfileSetExecutionTime1(90),0,0,0,0,0);

  for(i=0; i<5; i++)
  {
    logMsg("i=%d\n",i,0,0,0,0,0);

    PROFILE_START;
    for(j=0; j<3000; j++) k = k*k;
    PROFILE_STOP(5);
  }

  return;
}

#else /* no UNIX version */

void
fbutil_dummy()
{
  return;
}

#endif
