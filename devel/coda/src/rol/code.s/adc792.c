/*
  adc792.c - CAEN v792 QDC library for the VxWorks OS

  BUG report: Sergey Boyarinov boiarino@jlab.org
*/

#ifdef VXWORKS

#include <stdio.h>
#include <string.h>
#include <vxWorks.h>
#include <logLib.h>
#include <taskLib.h>
#include <intLib.h>
#include <vxLib.h>

#include "adc792.h"
#include "ttutils.h"

#define NTICKS 1000 /* the number of ticks per second */

IMPORT STATUS sysBusToLocalAdrs(int, char *, char **);
int    sysClkRateGet();
STATUS sysClkRateSet(int);


/* sleep() for vxworks - delay n * 10(msec) */
static void
tsleep(int n)
{
  taskDelay ((sysClkRateGet() / NTICKS) * n);
}

/*****************************************************/
/**************** low level functions ****************/
/*****************************************************/

/* Logic control */

int
adc792setLogic(UINT32 addr, UINT16 value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1006);
  volatile UINT16 *rg1 = (UINT16 *) (addr + 0x1010);

  if((value) > 0x7FFF) return(-1);
  *reg = (value) & 0x0098;
  *rg1 = (value >> 8) & 0x0034;

  return(0);
}

int
adc792getLogic(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1006);
  volatile UINT16 *rg1 = (UINT16 *) (addr + 0x1010);

  *value = ((*reg) & 0x0098) + (((*rg1) & 0x0034) << 8);

  return(0);
}

int
adc792resetLogic(UINT32 addr, UINT16 value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1008);

  if((value) > 0x7FFF) return(-1);

  *reg = (value) & 0x0098;

  return(0);
}


/* Geting status */

int
adc792getStatus(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x100E);

  *value = (*reg) & 0x01FF;

  return(0);
}

int
adc792getBufferStatus(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1022);

  *value = (*reg) & 0x0006;

  return(0);
}

int
adc792getBoardType(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1022);

  *value = (*reg) & 0x00F0;

  return(0);
}


/* Control operations */

int
adc792setOperationMode(UINT32 addr, UINT16 value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1032);

  if((value) > 0x7FFF) return(-1);
  *reg = (value) & 0x7FFF;

  return(0);
}

int
adc792resetOperationMode(UINT32 addr, UINT16 value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1034);

  if((value) > 0x7FFF) return(-1);
  *reg = (value) & 0x7FFF;

  return(0);
}

int
adc792getOperationMode(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1032);

  *value = (*reg) & 0x7FFF;

  return(0);
}


/* Board addressing */

/* value suppose to be 0xXXXX0000, last four digits will be ignored */
int
adc792setVMEAddress(UINT32 addr, UINT32 value)
{
  volatile UINT16 *regH = (UINT16 *) (addr + 0x1012);
  volatile UINT16 *regL = (UINT16 *) (addr + 0x1014);
  UINT16 high, low;

  high = (value >> 24) & 0xFF;
  low = (value >> 16) & 0xFF;
  *regH = high;
  *regL = low;

  return(0);
}

/* value returns as 0xXXXX0000, last four digits will be zero */
int
adc792getVMEAddress(UINT32 addr, UINT32 *value)
{
  volatile UINT16 *regH = (UINT16 *) (addr + 0x1012);
  volatile UINT16 *regL = (UINT16 *) (addr + 0x1014);

  *value = (((*regH)&0xFF)<<24) + (((*regL)&0xFF)<<16);

  return(0);
}

/* for non-PAUX version only !!! */
int
adc792setGEOAddress(UINT32 addr, UINT16 value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1002);

  if(value > 21) return(-1);
  *reg = value & 0x1F;

  return(0);
}

int
adc792getGEOAddress(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1002);

  *value = (*reg)&0x1F;

  return(0);
}



/* Interrupt operations */

int
adc792setInterruptLevel(UINT32 addr, UINT16 value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x100A);

  if(value > 0x0007) return(-1);
  *reg = value & 0x0007;

  return(0);
}

int
adc792getInterruptLevel(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x100A);

  *value = (*reg) & 0x0007;

  return(0);
}

int
adc792setInterruptVector(UINT32 addr, UINT16 value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x100C);

  if(value > 0x00FF) return(-1);
  *reg = value & 0x00FF;

  return(0);
}

int
adc792getInterruptVector(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x100C);

  *value = (*reg) & 0x00FF;

  return(0);
}

int
adc792setInterruptEventNumber(UINT32 addr, UINT16 value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1020);

  if(value > 0x001F) return(-1);
  *reg = value & 0x001F;

  return(0);
}

int
adc792getInterruptEventNumber(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1020);

  *value = (*reg) & 0x001F;

  return(0);
}



/* Output Buffer operations */

int
adc792incrementWord32(UINT32 addr)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x102A);

  *reg = 0x1; /* arbitrary data */

  return(0);
}

int
adc792incrementEvent (UINT32 addr)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1028);

  *reg = 0x1; /* arbitrary data */

  return(0);
}

int
adc792readWord(UINT32 addr, UINT32 *value)
{
  volatile UINT32 *reg = (UINT32 *) addr;

  *value = *reg;

  return(0);
}
int
adc792readWord1(UINT32 addr, UINT32 i, UINT32 *value)
{
  volatile UINT32 *reg = (UINT32 *) (addr+4*i);
tsleep(1);
  *value = *reg;
tsleep(1);

  return(0);
}


/*
*
* RETURNS: 0(No Data) or  event # in FIFO
*/

int
adc792Dready(UINT32 addr)
{
  int nev = 0;
  UINT32 word;
  UINT16 value, stat = 0;
  
  adc792getStatus(addr, &value);
  stat = value & DREADY;
  if(stat)
  {
    adc792readEventCounter(addr, &word);
    nev = word;
    return(nev);
  }

  return(0);
}


int
adc792readEvent(UINT32 addr, UINT32 array[32], UINT32 *nwords, UINT32 *nevent)
{
  volatile UINT32 *reg = (UINT32 *) addr;
  UINT32 header, end, *buf;
  int i;

  header = *reg;
  /*logMsg("Event Header -> slot=%2d  key=%1d  crate=%3d  count=%2d\n",
 (header>>27)&0x1F,(header>>24)&0x7,(header>>16)&0xFF,(header>>8)&0x3F,0,0);*/

  /* check if buffer is empty */
  if(((header>>24)&0x7) == 6)
  {
    logMsg("Got NOT VALID DATUM (read access to the empty buffer)\n",
      0,0,0,0,0,0);
    *nwords = 0;
    return(0);
  }

  buf = array;
  for(i=0; i<(*nwords=(header>>8)&0x3F); i++)
  {
    *buf++ = *reg;
    /*logMsg("   Data [%2d] -> slot=%2d channel=%2d data=%4d (un=%1d ov=%1d)\n",i,
      (*(buf-1)>>27)&0x1F, (*(buf-1)>>16)&0x3F, *(buf-1)&0xFFF, (*(buf-1)>>13)&0x1,
      (*(buf-1)>>12)&0x1);*/
  }

  end = *reg;
  *nevent = end&0xFFFFFF;
  /*logMsg("Event End    -> slot=%2d  key=%1d  event_counter=%2d\n",
  (end>>27)&0x1F, (end>>24)&0x7, end&0xFFFFFF,0,0,0);*/

  return(0);
}


/* Fast Clear Window */
/* window[us] = 7[us] + (1/32)*value[us] */

int
adc792setFastClearWindow(UINT32 addr, UINT16 value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x102E);

  if(value > 0x03FF) return(-1);
  *reg = value & 0x03FF;

  return(0);
}

int
adc792getFastClearWindow(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x102E);

  *value = (*reg) & 0x03FF;

  return(0);
}


/* define crate number */

int
adc792setCrateNumber(UINT32 addr, UINT16 value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x103C);

  if(value > 0x00FF) return(-1);
  *reg = value&0x00FF;

  return(0);
}

int
adc792getCrateNumber(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x103C);

  *value = (*reg)&0x00FF;

  return(0);
}


/* DAQ test */

int
adc792writeTestEvent(UINT32 addr, UINT16 array[32])
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x103E);
  int i;

  for(i=0; i<16; i++)
  {
    *reg = array[i] > 0xFFF ? ((array[i]&0xFFF)+0x1000) : (array[i]&0xFFF);
    *reg = array[i+16] > 0xFFF ? ((array[i+16]&0xFFF)+0x1000) : (array[i+16]&0xFFF);
  }

  return(0);
}


/* Event Counter operations */

int
adc792resetEventCounter(UINT32 addr)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1040);

  *reg = 0x1; /* arbitrary data */

  return(0);
}

int
adc792readEventCounter(UINT32 addr, UINT32 *value)
{
  volatile UINT16 *regL = (UINT16 *) (addr + 0x1024);
  volatile UINT16 *regH = (UINT16 *) (addr + 0x1026);

  *value = (((*regH)&0x00FF)<<16) + (*regL);

  return(0);
}


/* Pedestal operations (ADC only) */

int
adc792setPedestal(UINT32 addr, UINT16 value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1060);

  if(value > 0xFF) return(-1);
  *reg = value&0xFF;

  return(0);
}

int
adc792getPedestal(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1060);

  *value = (*reg)&0xFF;

  return(0);
}


/* Scale range operations (TDC only) */
/* 0xFF -> 35ps LSB, 140ns full scale range */
/* 0x1E -> 300ps LSB, 1200ns full scale range */
/* linear interpolation for intermediate values */

int
tdc775setResolution(UINT32 addr, UINT16 value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1060);

  if(value > 0xFF) return(-1);
  if(value < 0x1E) return(-2);
  *reg = value&0xFF;

  return(0);
}

int
tdc775getResolution(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1060);

  *value = (*reg)&0xFF;

  return(0);
}


/* memory test */

int
adc792setMemoryReadPointer(UINT32 addr, UINT16 value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1064);

  if(value > 0x07FF) return(-1);
  *reg = value&0x07FF;

  return(0);
}

int
adc792setMemoryWritePointer(UINT32 addr, UINT16 value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1036);

  if(value > 0x07FF) return(-1);
  *reg = value&0x07FF;

  return(0);
}

int
adc792writeMemoryWord32(UINT32 addr, UINT32 value)
{
  volatile UINT16 *regH = (UINT16 *) (addr + 0x1038);
  volatile UINT16 *regL = (UINT16 *) (addr + 0x103A);
  UINT16 high, low;

  low = value&0xFFFF;
  high = (value>>16)&0xFFFF;
/*printf("-> %04x %04x\n",high,low);*/

  *regH = high; /* first  !!! do not change the order !!! */
tsleep(1);
  *regL = low;  /* second !!! of these two operations !!! */

  return(0);
}


/* conversion for the test purposes */

int
adc792startConversion(UINT32 addr)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1068);

  *reg = 0x1; /* arbitrary data */

  return(0);
}


/* sliding constant */

int
adc792setSlideConstant(UINT32 addr, UINT16 value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x106A);

  if(value > 0x00FF) return(-1);
  *reg = value&0x00FF;

  return(0);
}

int
adc792getSlideConstant(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x106A);

  *value = (*reg)&0x00FF;

  return(0);
}


/* access to the value converted by ADC of the block A and B */

int
adc792getBlockA(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1070);

  *value = (*reg)&0x0FFF;

  return(0);
}

int
adc792getBlockB(UINT32 addr, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1072);

  *value = (*reg)&0x0FFF;

  return(0);
}


/* Threshold operations */

int
adc792setThreshold(UINT32 addr, UINT32 channel, UINT16 value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1080);

  if(channel < 0 || channel > 31) return(-1);
  if(value > 0x1FF) return(-2);
  reg[channel] = value;

  return(0);
}

int
adc792getThreshold(UINT32 addr, UINT32 channel, UINT16 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1080);

  if(channel < 0 || channel > 31) return(-1);
  *value = reg[channel];

  return(0);
}

int
adc792setThresholds(UINT32 addr, UINT16 *array)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1080);
  UINT16 *arr = array;
  int i;

  for(i=0; i<32; i++)
  {
    if(*array > 0x1FF) return(-1);
    *reg++ = *arr++;
  }

  return(0);
}

int
adc792getThresholds(UINT32 addr, UINT16 *array)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1080);
  UINT16 *arr = array;
  int i;

  for(i=0; i<32; i++)
  {
    *arr++ = *reg++;
  }

  return(0);
}



/* Firmware and ROM access */

int
adc792getFirmwareRevision(UINT32 addr, UINT32 *value)
{
  volatile UINT16 *reg = (UINT16 *) (addr + 0x1000);

  *value = *reg;

  return(0);
}

int
adc792getOUI(UINT32 addr, UINT32 *value)
{
  ADC792ROM *rom = (ADC792ROM *) (addr + ROM_OFFSET);

  *value = ((rom->OUI_3&0xff) << 16) +
           ((rom->OUI_2&0xff) << 8) + (rom->OUI_1&0xff);

  return(0);
}

int
adc792getVersion(UINT32 addr, UINT32 *value)
{
  ADC792ROM *rom = (ADC792ROM *) (addr + ROM_OFFSET);

  *value = rom->version&0xff;

  return(0);
}

int
adc792getBoardID(UINT32 addr, UINT32 *value)
{
  ADC792ROM *rom = (ADC792ROM *) (addr + ROM_OFFSET);

  *value = ((rom->ID_3&0xff) << 16) +
           ((rom->ID_2&0xff) << 8) + (rom->ID_1&0xff);

  return(0);
}

int
adc792getRevision(UINT32 addr, UINT32 *value)
{
  ADC792ROM *rom = (ADC792ROM *) (addr + ROM_OFFSET);

  *value = rom->revision&0xff;

  return(0);
}

int
adc792getSerialNumber(UINT32 addr, UINT32 *value)
{
  ADC792ROM *rom = (ADC792ROM *) (addr + ROM_OFFSET);

  *value = ((rom->NUM_2&0xff) << 8) + (rom->NUM_1&0xff);

  return(0);
}



/*****************************************************/
/**************** high level functions ***************/
/*****************************************************/


/* high level functions */



int
adc792printROM(UINT32 addr)
{
  UINT32 value;

  printf("ADC792 ROM information =====>\n");
  adc792getOUI(addr, &value);
  printf("  Manufacturer identifier (IEEE OUI): 0x%08x\n",value);
  adc792getVersion(addr, &value);
  printf("  Purchased version of the Mod.V792:  0x%08x\n",value);
  adc792getBoardID(addr, &value);
  printf("  Board identifier:                   %10d\n",value);
  adc792getRevision(addr, &value);
  printf("  Hardware revision identifier:       0x%08x\n",value);
  adc792getSerialNumber(addr, &value);
  printf("  Serial number:                      0x%08x\n",value);

  adc792getFirmwareRevision(addr, &value);
  printf("ADC792 Firmware Revision =====> 0x%08x\n\n",value);

  return(0);
}

int
adc792boardInit(UINT32 addr)
{
  adc792setLogic(addr, SOFT_RESET);
  adc792resetLogic(addr, SOFT_RESET);

  return(0);
}

int
adc792printStatus(UINT32 addr)
{

  /* read various registers */

  /*
  stat1 = c792p[id]->status1;
  stat2 = c792p[id]->status2;
  bit1 = c792p[id]->bitSet1;
  bit2 = c792p[id]->bitSet2;
  cntl1 = c792p[id]->control1;
  tmp1 = c792p[id]->evCountL;
  tmp2 = c792p[id]->evCountH;
  evCnt = (tmp2<<16) | tmp1 ;
  */

  /* print out status info */

  /*
  printf("STATUS for QDC id %d at base address 0x%08x \n",
    id,(unsigned int)c792p[id]);
  printf("----------------------------------------- \n");
  printf("Status  = 0x%04x 0x%04x\n",stat1,stat2);
  printf("BitSet  = 0x%04x 0x%04x\n",bit1,bit2);
  printf("Control = 0x%04x\n",cntl1);
  printf("Event Count = %d\n",evCnt);
  */

  return(0);
}


/*****************************************************/
/************* interrupt functions *******************/
/*****************************************************/

int
adc792intenable(int addr, unsigned int nev, int level, int vector)
{
  unsigned short value1,value2,value3;

  /* check for event count out of range */
  if((nev<=0) || (nev>31))
  {
    printf("adc792intenable: ERROR:  Event count %d is out of range(1-31)\n",
                    nev);
    return(-1);
  }

  adc792setInterruptLevel      (addr, (unsigned short)level);
  adc792setInterruptVector     (addr, (unsigned short)vector);
  adc792setInterruptEventNumber(addr, (unsigned short)nev);


  adc792getInterruptLevel      (addr, &value2);
  adc792getInterruptVector     (addr, &value3);
  printf("adc792intenable: addr=0x%08x nev=%d level=%d vector=%d\n",
                              addr,nev,value2,value3);
  adc792getInterruptEventNumber(addr, &value1);


  return(0);
}

int
adc792intdisable(int addr)
{
  adc792setInterruptEventNumber(addr, (unsigned short)0);

  return(0);
}

int
adc792intSave(int addr, unsigned int *mask0)
{
  unsigned short value;

  adc792getInterruptEventNumber(addr, &value);
  *mask0 = (unsigned int) value;

  adc792setInterruptEventNumber(addr, (unsigned short)0);
  return(0);
}

int
adc792intRestore(int addr, unsigned int mask0)
{
  adc792setInterruptEventNumber(addr, (unsigned short)mask0);
  return(0);
}



/***************************/
/* CLAS-SPECIFIC FUNCTIONS */
/***************************/

void
CLAStdc775Init(int nboards, unsigned long *tdcadr)
{
  int i, ii, ifull;
  unsigned short value, value0, value1, array0[32], array1[32], data16;
  unsigned short dat16[21][10];
  short tmp[5], tdata;
  unsigned int ww, wm, wr;
  int wo;

  printf("start v775 initialization\n");

  for(ii=0; ii<nboards; ii++)
  {
    adc792boardInit(tdcadr[ii]);

    /* Common Stop mode */
    adc792setOperationMode(tdcadr[ii], START_STOP);
    /* Common Start mode 
	   adc792resetOperationMode(tdcadr[ii], START_STOP);*/

    /* set resolution (0xFF for 35 ps, 0x1E for 300 ps) */
    value = 0xFF;
    tdc775setResolution(tdcadr[ii], value);
    tdc775getResolution(tdcadr[ii], &value);
    printf("Resolution per channel is 0x%x\n",value);

    /* disable overflow suppression */
    adc792setOperationMode(tdcadr[ii], OVER_RANGE);

    /* disable valid suppression */
    adc792setOperationMode(tdcadr[ii], VALID_CONTROL);

    adc792getOperationMode(tdcadr[ii], &value);
    if(value&START_STOP) printf("Common Stop mode\n");
    else                 printf("Common Start mode\n");
    if(value&OVER_RANGE) printf("NO Overflow suppression\n");
    else                 printf("Overflow suppression\n");
    if(value&VALID_CONTROL) printf("NO Valid suppression\n");
    else                    printf("Valid suppression\n");

    /* set thresholds */
    for(i=0; i<32; i++)
    {
      value = 0x1;
      adc792setThreshold(tdcadr[ii], i, value);
      adc792getThreshold(tdcadr[ii], i, &value);
      printf("Channel %2d, threshold = 0x%x\n",i,value);
    }

    value = 0;
    adc792setFastClearWindow(tdcadr[ii], value);
    adc792getFastClearWindow(tdcadr[ii], &value);
    printf("Fast Clear Window is %d microsec\n",(7+(value/32)));
  }

  return;
}


/* read several boards; that function called from rol1 */
int
CLAStdc775ReadEvent(int nboards, UINT32 *tdcadr, INT32 *tdcbuf,
                    INT32 *rlenbuf)
{
  UINT16 value;
  UINT32 word0;
  int i, jj, itdcbuf, tag, ch, tdc1buf[32], kk;

  itdcbuf = 0;
  for(jj=0; jj<nboards; jj++)
  {

    adc792getStatus(tdcadr[jj], &value);
    while(!(value&DREADY))
    {
logMsg("wait ...\n",1,2,3,4,5,6);
      adc792getStatus(tdcadr[jj], &value);
    }

    for(i=0; i<32; i++) tdc1buf[i] = 0;
    kk = 0;
    adc792getBufferStatus(tdcadr[jj],&value);
    while(!(value&BUFFER_EMPTY))
    {
      adc792readWord(tdcadr[jj], &word0);
      tag = (word0 >> 24) & 0x7;
      ch = (word0 >> 16) & 0x1F;
	  
logMsg("addr=0x%08x -> ch=%2d tag=%1d word0=0x%08x\n",
tdcadr[jj],ch,tag,word0,5,6);
	  
      kk ++;

      /* data words */
      if(tag==0) tdc1buf[ch] = word0 & 0xFFF;

      adc792getBufferStatus(tdcadr[jj],&value);
    }

logMsg("kk=%d\n",kk,2,3,4,5,6);

    for(i=0; i<32; i++)
    {
      tdcbuf[itdcbuf++] == tdc1buf[i];
      rlenbuf[jj] = 32;
    }

  }

  return(OK);
}
















/****************/
/*              */
/*   EXAMPLES   */
/*              */
/****************/

#define ADCADR 0xfad00000
#define TDCADR 0xfad00000
#define MEMSIZE 200


/* interrupt vector */
#define MYINT_VECTOR 0xf0
/* interrupt level (TI board has level 5) */
#define MYINT_LEVEL 6

static int *ring0, *ring1;

void
adc792inthandler0(int addr)
{
  int i;
  UINT16 value;
  UINT32 word0, word1, nwords, nevent, buf[32];

  logMsg("adc792inthandler reached, addr=0x%08x\n",(int)addr,2,3,4,5,6);

  adc792getBufferStatus(addr,&value);
  while(!(value&BUFFER_EMPTY))
  {
    adc792readEvent(addr, buf, &nwords, &nevent);
    logMsg("adc792inthandler: nevent=%d data0=%d (UN=%1d OV=%1d V=%1d)\n",
         nevent,buf[0]&0xFFF,(buf[0]>>13)&1,(buf[0]>>12)&1,(buf[0]>>14)&1,6);
    adc792setOperationMode(addr, CLEAR_DATA);
    adc792resetOperationMode(addr, CLEAR_DATA);

    for(i=0; i<nwords; i++)
    {
      if(utRingWriteWord(ring0, buf[i]) < 0)
      {
        logMsg("adc792inthandler: ERROR: ring0 full\n",0,0,0,0,0,0);
      }
    }

    adc792getBufferStatus(addr,&value);
  }

  return;
}

void
adc792inthandler(void)
{
  unsigned int mask0;

  sysIntDisable(MYINT_LEVEL);
  adc792intSave(ADCADR, &mask0);

  logMsg("adc792inthandler reached, mask0=%d\n",mask0,0,0,0,0,0);
  adc792inthandler0(ADCADR);

  adc792intRestore(ADCADR, mask0);
  sysIntEnable(MYINT_LEVEL);

  return;
}



/*****************************************************/
/*************** test functions **********************/
/*****************************************************/

int
adc792_memory_test(UINT32 addr)
{
  UINT16 value0, value1, array0[32], array1[32];
  UINT32 word0, word1, radr, wadr;
  int i, fail;

  /* test random memory access */

  adc792getOperationMode(addr, &value0);
  adc792getBufferStatus(addr, &value1);
printf("1-> 0x%08x 0x%08x\n",value0,value1);

  /* turn on memory test mode */
  adc792setOperationMode(addr, MEMORY_TEST);

  adc792getOperationMode(addr, &value0);
  adc792getBufferStatus(addr, &value1);
printf("2-> 0x%08x 0x%08x\n",value0,value1);

  /* read pointer MUST be different from write pointer at ANY MOMENT */
  radr = 7;
  wadr = 9;
  /*printf("[-2] r=0x%08x w=0x%08x\n",radr,wadr);*/
  adc792setMemoryReadPointer(addr, radr);
  adc792setMemoryWritePointer(addr, wadr);
  /*printf("[-1] r=0x%08x w=0x%08x\n",radr,wadr);*/

  /* memory writing loop */
  printf("Writing ...\n");
  for(i=0; i<MEMSIZE; i++)
  {
    word0 = 0x000F000F + i*0xF000F;
	/*printf("word0=0x%08x\n",word0);*/
    wadr = i;
    /*printf("1[%2d]: r=0x%08x w=0x%08x\n",i,radr,wadr);*/
    adc792setMemoryWritePointer(addr,wadr);
    /*printf("2[%2d]: r=0x%08x w=0x%08x\n",i,radr,wadr);*/
    adc792writeMemoryWord32(addr,word0);
    if(i==5)
    {
      radr=2;
      /*printf("3[%2d]: r=0x%08x w=0x%08x\n",i,radr,wadr);*/
      adc792setMemoryReadPointer(addr, radr);
    }
  }


  /* memory reading and check loop */
  printf("Reading and checking ...\n");
  word0 = 0x000F000F;
  fail = 0;
  for(i=0; i<MEMSIZE; i++)
  {
    word0 = 0x000F000F + i*0xF000F;
    radr = i;
    /*printf("1[%2d]: r=0x%08x w=0x%08x\n",i,radr,wadr);*/
    adc792setMemoryReadPointer(addr,radr);
    /*printf("2[%2d]: r=0x%08x w=0x%08x\n",i,radr,wadr);*/
    adc792readWord1(addr, i, &word1);
    /*adc792readWord(addr, &word1);*/
    if(i==5)
    {
      wadr=2;
      /*printf("3[%2d]: r=0x%08x w=0x%08x\n",i,radr,wadr);*/
      adc792setMemoryWritePointer(addr, wadr);
    }
    if(word0 != word1)
    {
      fail = -1;
      printf("adr=%04d in=0x%08x out=0x%08x\n",i,word0,word1);
    }
  }

  /* turn off memory test mode */
  adc792resetOperationMode(addr, MEMORY_TEST);

  /* clear buffer */
  adc792setOperationMode(addr, CLEAR_DATA);
  adc792resetOperationMode(addr, CLEAR_DATA);

  adc792getOperationMode(addr, &value0);
  adc792getBufferStatus(addr, &value1);
printf("4-> 0x%08x 0x%08x\n",value0,value1);


  return(fail);
}


int
adc792_daq_test(UINT32 addr)
{
  UINT16 value0, value1, array0[32], array1[32];
  UINT32 word0, word1, nwords, nevent, buf[32];
  int i, fail;

  fail = 0;

  /* Acquisition test */

  adc792getOperationMode(addr, &value0);
  adc792getBufferStatus(addr, &value1);
printf("1-> 0x%08x 0x%08x\n",value0,value1);

  /* turn on daq test mode,
  it resets the write pointer in the FIFO */
  adc792setOperationMode(addr, DAQ_TEST);

  /* turn off daq test mode,
  it resets the read pointer in the FIFO and releases the write pointer */
  adc792resetOperationMode(addr, DAQ_TEST);

  /* fill array */
  for(i=0; i<32; i++) array0[i] = 1000+i;

  /* write test event (32 words) */
  adc792writeTestEvent(addr, array0);

  /* turn on daq test mode,
  it resets again the write pointer in the FIFO and releases the read pointer */
  adc792setOperationMode(addr, DAQ_TEST);

  /* read event counter */
  adc792readEventCounter(addr, &nevent);
  printf("nevent1=%d\n",nevent);

  /* send now several GATE signals */
  adc792startConversion(addr);
  tsleep(1);
  adc792startConversion(addr);
  tsleep(1);
  adc792startConversion(addr);
  tsleep(1);

  /* read buffer */
  adc792readEvent(addr, buf, &nwords, &nevent);
  printf("nwords1=%d buf->0x%08x 0x%08x 0x%08x\n",nwords,buf[0],buf[1],buf[2]);
  adc792readEvent(addr, buf, &nwords, &nevent);
  printf("nwords2=%d\n",nwords);
  adc792readEvent(addr, buf, &nwords, &nevent);
  printf("nwords3=%d\n",nwords);
  printf("nevent2=%d\n",nevent);

  adc792readEventCounter(addr, &nevent);
  printf("nevent3=%d\n",nevent);

  /* turn off daq test mode */
  adc792resetOperationMode(addr, DAQ_TEST);

  /* clear buffer */
  adc792setOperationMode(addr, CLEAR_DATA);
  adc792resetOperationMode(addr, CLEAR_DATA);

  adc792getOperationMode(addr, &value0);
  adc792getBufferStatus(addr, &value1);
printf("2-> 0x%08x 0x%08x\n",value0,value1);

  return(fail);
}


void
adc792_test()
{
  UINT16 value0, value1, array0[32], array1[32];
  UINT32 addr, laddr, word0, word1;
  int i;


  addr = 0x00d00000; /* temporary */


  /* set system clock rate (seems can be from 10 to 1000 ...) */
  sysClkRateSet(NTICKS);
  printf("Clocks per second = %d\n",sysClkRateGet());

  /* get board address */
  if(addr==0)
  {
    printf("ERROR: Must specify an address for the board.\n");
    return;
  }
  else if(addr < 0x00ffffff) /* A24 Addressing */
  {
    if(sysBusToLocalAdrs(AM24N,(char *)addr,(char **)&laddr) != 0)
    {
      printf("ERROR in sysBusToLocalAdrs(AM24N,0x%x,&laddr) \n",addr);
      return;
    }
  }
  else /* A32 Addressing */
  {
    if(sysBusToLocalAdrs(AM32N,(char *)addr,(char **)&laddr) != 0)
    {
      printf("ERROR in sysBusToLocalAdrs(AM32N,0x%x,&laddr) \n",addr);
      return;
    }
  }
  printf("Looking for the board at address 0x%08x ... \n",laddr);

  /* check if board exists at that address */
  if(vxMemProbe((char *) laddr,0,4,(char *)&i) < 0)
  {
    printf("ERROR: No addressable board at addr=0x%x\n",(UINT32) laddr);
    return;
  }

  /* Check if this is right kind of board */
  adc792getBoardID(ADCADR, &word0);
  if(word0 != C792_BOARD_ID)
  {
    printf("ERROR: Board ID does not match: got %d, expected %d\n",
      word0,C792_BOARD_ID);
    return;
  }
  printf("Board found at address 0x%08x with board id %d\n",
      laddr,word0);



  printf("\n==> TEST started <==\n\n");

  /* board initialization */
  adc792boardInit(laddr);

  /* test ROM */
  adc792printROM(laddr);

  /* test thresholds */
  for(i=0; i<32; i++)
  {
    value0 = i+200;
    adc792setThreshold(laddr, i, value0);
    adc792getThreshold(laddr, i, &value1);
    /*printf("ch[%2d] set %d  get %d\n",i,value0,value1);*/
    if(value0 != value1)
    {
      printf("ERROR: threshold operation 1\n");
      return;
    }
  }
  for(i=0; i<32; i++) array0[i] = i+100;
  adc792setThresholds(laddr, array0);
  adc792getThresholds(laddr, array1);
  /*for(i=0; i<32; i++) printf("ch[%2d] set %d  get %d\n",i,array0[i],array1[i]);*/
  for(i=0; i<32; i++)
  {
    if(array0[i] != array1[i])
    {
      printf("ERROR: threshold operation 2\n");
      return;
    }
  }
  for(i=0; i<32; i++) array0[i] = 0;
  adc792setThresholds(laddr, array0);
  adc792getThresholds(laddr, array1);
  /*for(i=0; i<32; i++) printf("ch[%2d] set %d  get %d\n",i,array0[i],array1[i]);*/
  for(i=0; i<32; i++)
  {
    if(array0[i] != array1[i])
    {
      printf("ERROR: threshold operation 3\n");
      return;
    }
  }


  /* test memory */
  if(adc792_memory_test(laddr) < 0)
  {
    printf("ERROR: memory test failed\n");
    return;
  }
  

  /* test daq 
  if(adc792_daq_test(laddr) < 0)
  {
    printf("ERROR: daq test failed\n");
    return;
  }
  */
  /*
  test1(laddr);
  */

  printf("\n==> TEST completed OK <==\n\n");
  return;
}

int
adc792_test1(UINT32 addr)
{
  UINT16 value;
  UINT32 word0, word1, nwords, nevent, buf[32];
  int i, j;

  /* reset board */
  adc792boardInit(addr);

  /* Common Stop mode */
  adc792setOperationMode(addr, START_STOP);
  

  /* Common Start mode 
  adc792resetOperationMode(addr, START_STOP);
  */

  /* set resolution (0xFF for 35 ps, 0x1E for 300 ps) */
  value = 0xFF;
  tdc775setResolution(addr, value);
  tdc775getResolution(addr, &value);
  printf("Resolution per channel is 0x%x\n",value);

  /* disable overflow suppression */
  adc792setOperationMode(addr, OVER_RANGE);

  /* disable valid suppression */
  adc792setOperationMode(addr, VALID_CONTROL);

  adc792getOperationMode(addr, &value);
  if(value&START_STOP) printf("Common Stop mode\n");
  else                 printf("Common Start mode\n");
  if(value&OVER_RANGE) printf("NO Overflow suppression\n");
  else                 printf("Overflow suppression\n");
  if(value&VALID_CONTROL) printf("NO Valid suppression\n");
  else                    printf("Valid suppression\n");


value = 0x1;
adc792setThreshold(addr, 0, value);
adc792getThreshold(addr, 0, &value);
printf("Threshold = 0x%x\n",value);





/* WITHOUT INTERRUPTS */


  for(i=0; i<1; i++)
  {
    adc792getBufferStatus(addr,&value);
    if(!(value&BUFFER_EMPTY))
    {
      adc792readEvent(addr, buf, &nwords, &nevent);
      printf("nevent=%d data0=%d (UN=%1d OV=%1d V=%1d)\n",
         nevent,buf[0]&0xFFF,(buf[0]>>13)&1,(buf[0]>>12)&1,(buf[0]>>14)&1);
      adc792setOperationMode(addr, CLEAR_DATA);
      adc792resetOperationMode(addr, CLEAR_DATA);
    }

    for(j=0; j<10; j++)
    {
      taskDelay (sysClkRateGet());
    }

  }



/* WITH INTERRUPTS 

  ring0 = utRingCreate(64000);

  utIntInit((VOIDFUNCPTR)adc792inthandler,0,MYINT_LEVEL,MYINT_VECTOR);
  adc792intenable(addr, 1, MYINT_LEVEL,MYINT_VECTOR);
  for(i=0; i<10; i++)
  {
    taskDelay (sysClkRateGet());
  }
  adc792intdisable(addr);

  utRingStatusPrint(ring0);
*/

  return(0);
}



void
adc792_ttt()
{
  int i, j;

  for(i=0; i<10; i++)
  {
    adc792_daq_test(ADCADR);

    for(j=0; j<10; j++)
    {
      taskDelay (sysClkRateGet());
    }

  }
  /*test1(ADCADR);*/
  return;
}

#else /* no UNIX version */

void
adc792_dummy()
{
  return;
}

#endif












