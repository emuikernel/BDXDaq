#ifdef VXWORKS

#define SP_DEBUGx

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tdc890.h"

#include "v895.h"


/* readout options */
static int a24_a32;


/* CAEN v895 Discriminator base address and step */
/* first board has address DISCRADR, second DISCRADR+DISCRSTEP etc */
/* NBOARDS - max # of boards */

/*#define DISCRADR  0x100000 */
/*#define DISCRSTEP 0x100000 */
/*#define DISCRADR  0x010000 */
/*#define DISCRSTEP 0x010000 */
int DISCRADR  = 0x0100000;
int DISCRSTEP = 0x0100000;

#define F_NAME    100       /* length of config. file name */
#define STRLEN    250       /* length of str_tmp */
#define ROCLEN    20        /* length of ROC_name */
#define NBOARDS   40

#define CHECK_NUMBER_vs_ADDR \
  volatile V895 *v895 = (V895 *) addr; \
  int v895_number; \
  if( (v895_number = v895GetNumberByAddr(addr)) == -1 )   return(-1)

#define CHECK_CHANNEL_NUMBER \
  if( (channel < 0) || (channel > 15) )   return(-2)


/* Reference Table of DAC values and Output Signal Width */
/* measured by Scope and ROOT */
static int v895_DAC_Width [44] = 
  { 0,
/*   1    2    3    4    5    6    7    8    9    10  (index = ns) */
      0,   0,   0,   0,  44,  90, 114, 133, 149, 164,
    175, 185, 193, 199, 205, 210, 214, 218, 221, 224,
    227, 230, 232, 234, 236, 238, 239, 241, 242, 244,
    245, 246, 247, 248, 249, 250, 251, 252, 253, 253,
    254, 255, 255 };


/* Define global variables */
static UINT32  v895_BaseAddr  [NBOARDS];
static UINT16  v895_Threshold [NBOARDS][16];
static UINT16  v895_Width     [NBOARDS][2];
static UINT16  v895_Majority  [NBOARDS];
static UINT16  v895_Mask      [NBOARDS];
static int     v895_Nconf     [NBOARDS];
static int     v895_NNconf;
static int     v895_NN;



/* Function Prototypes */
void   v895Init            ();
int    v895SetConfig       (char *fname);
void   v895MonAll          ();
void   v895MonConfig       ();
void   v895MonBoard        (int board_number);
void   v895InitGlobals     ();
STATUS v895ScanCLAS        (int options, int *nboards, UINT32 *discradr);
int    v895ReadConfigFile  (char *fname);
int    v895DownloadAll     ();
int    v895DownloadConfig  ();

int    v895SetMask         (UINT32 addr, UINT16 mask);
int    v895EnableChannel   (UINT32 addr, int channel);
int    v895EnableAllChan   (UINT32 addr);
int    v895EnableAll       ();
int    v895DisableChannel  (UINT32 addr, int channel);
int    v895DisableAllChan  (UINT32 addr);
int    v895DisableAll      ();
int    v895SetThreshold    (UINT32 addr, int channel, int thresh);
int    v895ReadThreshold   (UINT32 addr, int channel);
int    v895SetUniThreshold (UINT32 addr, int thresh);
int    v895SetThresholdAll (int thresh);
int    v895SetWidth        (UINT32 addr, int channel, int width);
int    v895ReadWidth       (UINT32 addr, int channel);
int    v895SetUniWidth     (UINT32 addr, int width);
int    v895SetWidthAll     (int width);
int    v895SetMajority     (UINT32 addr, int majority_level);
int    v895ReadMajority    (UINT32 addr);
int    v895TestPulse       (UINT32 addr);
int    v895GetVersion      (UINT32 addr);
int    v895GetSerialN      (UINT32 addr);
int    v895GetManufactN    (UINT32 addr);
int    v895GetModuleType   (UINT32 addr);
int    v895GetFixedCode1   (UINT32 addr);
int    v895GetFixedCode2   (UINT32 addr);

int    v895GetNumberByAddr (UINT32 addr);
UINT32 v895GetAddrByNumber (int board_number);



/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

void
v895Init()
{
  unsigned int discradr[NBOARDS];

  v895InitGlobals();
  v895ScanCLAS(CLAS_A24_D32_BERR, &v895_NN, discradr);
  v895DownloadAll();
}


int
v895SetConfig(char *fname)
{
  int res;
  if( (res = v895ReadConfigFile(fname)) < 0 )   return(res);
  return(v895DownloadConfig());
}


#define V895_MON_PRINT(Dk0,DNN,Dkk) \
  int kk, jj; \
  for(kk=(Dk0); kk<(DNN); kk++) \
  { \
    printf("v895[0x%08x]:  V=%d  SN=%d  ManufacN=%d  Mod.Type=%d  FC1=0x%02x  FC2=0x%02x\n", \
	   v895_BaseAddr[(Dkk)], \
	   v895GetVersion(v895_BaseAddr[(Dkk)]), \
	   v895GetSerialN(v895_BaseAddr[(Dkk)]), \
	   v895GetManufactN(v895_BaseAddr[(Dkk)]), \
	   v895GetModuleType(v895_BaseAddr[(Dkk)]), \
	   v895GetFixedCode1(v895_BaseAddr[(Dkk)]), \
	   v895GetFixedCode2(v895_BaseAddr[(Dkk)])); \
    printf("                   majority=%d    mask=0x%04x   width[0-7]=%d   width[8-15]=%d", \
	   v895_Majority[(Dkk)], \
	   v895_Mask[(Dkk)], \
	   v895_Width[(Dkk)][0], \
	   v895_Width[(Dkk)][1]); \
    for(jj=0;jj<16;jj++) \
    { \
      if( jj%4 == 0 )  printf("\n                   "); \
      printf("thrsh[%2d]=%3d  ", jj, v895_Threshold[(Dkk)][jj]); \
   } \
    printf("\n"); \
  }


void
v895MonAll()
{
  V895_MON_PRINT(0,v895_NN,kk);
}


void
v895MonConfig()
{
  V895_MON_PRINT(0,v895_NNconf,v895_Nconf[kk]);
}


void
v895MonBoard(int board_number)
{
  V895_MON_PRINT(board_number,(board_number+1),kk);
}


void
v895InitGlobals()
{
  int ii, jj;

  for(ii=0; ii<NBOARDS; ii++)
  {
    for(jj=0;jj<16;jj++)  v895_Threshold [ii][jj] = 20;
    for(jj=0;jj<2;jj++)   v895_Width     [ii][jj] = 40;
    v895_BaseAddr [ii] = 0xffffff00;
    v895_Majority [ii] = 0;
    v895_Mask     [ii] = 0xffff;
    v895_Nconf    [ii] = -1;
  }
  v895_NNconf = 0;
  v895_NN     = 0;
}


STATUS
v895ScanCLAS(int options, int *nboards, UINT32 *discradr)
{
  volatile V895 *v895;
  int ii, res, rdata, errFlag = 0;
  int iboards, boardID = 0;
  unsigned int offset;


  /* unpack options */

  a24_a32 = (options>>16)&0xFF;

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



  /* get offset: use address modifier 0x09 for A32, 0x39 for A24 and anything else */
  if(a24_a32==0x02)
  {
    sysBusToLocalAdrs(0x09,V895_A32_ADDR,&offset);
  }
  else
  {
    sysBusToLocalAdrs(0x39,0,&offset);
  }
  printf("v895ScanCLAS: offset = 0x%08x\n",offset);


  iboards = 0;
  for(ii=0; ii<NBOARDS; ii++)
  {
    /* Check if Board exists at that address */
/*
-> vxMemProbe(0xfa0100fa, 0x0, 2, &yy)
value = 0 = 0x0
*/
    v895 = (V895 *) (offset + DISCRADR + ii*DISCRSTEP);
    res = vxMemProbe((char *) &(v895->module_type), 0, 2, (char *) &rdata);
    if(res < 0)
    {
      printf("v895ScanCLAS: No addressable board at addr=0x%x\n", (UINT32) v895);
      *nboards = iboards;
      return(OK);
    }
    else
    {
      printf("v895ScanCLAS: Found addressable board at addr=0x%x\n", (UINT32) v895);

      /* Check if this is a Model v895 */
      boardID = v895->module_type;
      if(boardID != V895_BOARD_ID)
      {
        printf("v895ScanCLAS: Manufacturer/Module type does not match,\n");
	printf("              expected  = 0x%08x\n              read back = 0x%08x\n\n",
	       V895_BOARD_ID, boardID);
        *nboards = iboards;
        return(OK);
      }
    }
    iboards ++;
    v895_BaseAddr[ii] = discradr[ii] = (UINT32) v895;

#ifdef SP_DEBUG
    printf("v895ScanCLAS: init v895 Discriminator number %d at address 0x%08x\n",
	   iboards, discradr[ii]);
    printf ("  Fixed code                  = 0x%04x  at 0x%08x\n",
	    v895->fixed_code, &v895->fixed_code);
    printf ("  Manufacturer & Module type  = 0x%04x  at 0x%08x\n",
	    v895->module_type, &v895->module_type);
    printf ("  Version & Serial number     = 0x%04x  at 0x%08x\n",
	    v895->serial_number, &v895->serial_number);
#endif
  }

  return(ERROR);
}


int
v895ReadConfigFile (char *fname)
{
  FILE   *ff;
  int    ii, jj, ch, kk = -1;
  char   str_tmp[STRLEN], keyword[ROCLEN];
  char   host[ROCLEN], ROC_name[ROCLEN];
  int    nameLen, start_conf = 0;
  UINT32 addr;
  int    w[2], maj, msk[16], ch_n, ch_thr;
  UINT32 Read_BaseAddr  [NBOARDS];
  UINT16 Read_Threshold [NBOARDS][16];
  int    Read_ch_Thresh [NBOARDS][16];
  int    Read_N_Thresh  [NBOARDS];
  UINT16 Read_Width     [NBOARDS][2];
  UINT16 Read_Majority  [NBOARDS];
  UINT16 Read_Mask      [NBOARDS];
  int    Read_Nconf     [NBOARDS];


  /* Init tmp variables */
  for(ii=0; ii<NBOARDS; ii++)
  {
    for(jj=0;jj<16;jj++)  Read_Threshold [ii][jj] = 0;
    for(jj=0;jj<16;jj++)  Read_ch_Thresh [ii][jj] = 0;
    for(jj=0;jj<2;jj++)   Read_Width     [ii][jj] = 0;
    Read_BaseAddr [ii] = 0;
    Read_Majority [ii] = 0;
    Read_Mask     [ii] = 0;
    Read_Nconf    [ii] = 0;
    Read_N_Thresh [ii] = 0;
  }


  /* Open config file */
  if ( (ff = fopen(fname,"r")) == NULL)
  {
    printf("\nReadConfigFile: Can't open config file\n%s\n\n",fname);           return(-2);
  }

  gethostname(host, &nameLen);
  sprintf(ROC_name, "<%s>", host);

#ifdef SP_DEBUG
  printf("\n       host=%s;    nameLen=%d \n", host, nameLen);
  printf("   ROC_name=%s; \n", ROC_name);
#endif


  /* Parsing of config file */
  while ((ch = getc(ff)) != EOF)
  {
    if     ( ch == '#' || ch == ' ' || ch == '\t' )
    {
      while (getc(ff) != '\n') {}
    }
    else if( ch == '\n' ) {}
    else
    {
      ungetc(ch,ff);
      fgets(str_tmp, STRLEN, ff);
      sscanf (str_tmp, "%s", keyword);

      if(strcmp(keyword,"CRATE") == 0)
      {
	if( (strstr(str_tmp,ROC_name) != NULL) && (kk == -1) )  start_conf = 1;
	else                                                    start_conf = 0;

#ifdef SP_DEBUG
	printf("\n    str_tmp=%s;\n", str_tmp);
	printf("    keyword=%s;\n", keyword);
	printf(" start_conf=%d;\n\n", start_conf);
#endif
      }
      else if(start_conf == 1)
      {
	if(strcmp(keyword,"BOARD") == 0)
	{
	  kk++;
	  sscanf (str_tmp, "%*s%*s %x %d %d %d", &addr, &w[0], &w[1], &maj);

#ifdef SP_DEBUG
	  printf(" v895_READ [kk=%d] at addr=0x%08x  w0=%d  w1=%d  maj=%d\n",
		 kk, addr, w[0], w[1], maj);
#endif
	  if( (Read_Nconf[kk] = v895GetNumberByAddr(addr)) == -1 )
	  {
	    printf("\nReadConfigFile: Wrong address, 0x%08x\n\n",addr);         return(-3);
	  }
	  Read_BaseAddr [Read_Nconf[kk]]    = addr;
	  Read_Width    [Read_Nconf[kk]][0] = w[0];
	  Read_Width    [Read_Nconf[kk]][1] = w[1];
	  Read_Majority [Read_Nconf[kk]]    = maj;
	}
	else if( (strcmp(keyword,"MASK") == 0) && (kk >= 0) )
	{
	  sscanf (str_tmp, "%*s %d %d %d %d %d %d %d %d
                              %d %d %d %d %d %d %d %d",
		  &msk[ 0], &msk[ 1], &msk[ 2], &msk[ 3],
		  &msk[ 4], &msk[ 5], &msk[ 6], &msk[ 7],
		  &msk[ 8], &msk[ 9], &msk[10], &msk[11],
		  &msk[12], &msk[13], &msk[14], &msk[15]);
	  for(jj=0; jj<16; jj++)
	  {
	    if( (msk[jj] < 0) || (msk[jj] > 1) )
	    {
	      printf("\nReadConfigFile: Wrong mask bit value, %d\n\n",msk[jj]); return(-4);
	    }

	    Read_Mask [Read_Nconf[kk]] |= (msk[jj]<<jj);
	  }

#ifdef SP_DEBUG
	  printf(" v895_READ_Mask[0x%08x] = 0x%04x \n",
		 Read_BaseAddr [Read_Nconf[kk]], Read_Mask [Read_Nconf[kk]]);
#endif
	}
	else if( (strcmp(keyword,"CHANNEL") == 0) && (kk >= 0) )
	{
	  sscanf (str_tmp, "%*s %d %d", &ch_n, &ch_thr);

	  if( (ch_n < 0) || (ch_n > 15) )
	  {
	    printf("\nReadConfigFile: Wrong channel number, %d\n\n",ch_n);      return(-5);
	  }
	  Read_Threshold [Read_Nconf[kk]][ch_n] = ch_thr;
	  Read_ch_Thresh [Read_Nconf[kk]][Read_N_Thresh[Read_Nconf[kk]]] = ch_n;
	  Read_N_Thresh  [Read_Nconf[kk]] ++;

#ifdef SP_DEBUG
	  printf(" v895_READ_Threshold[0x%08x][%2d] = %3d \n",
		 Read_BaseAddr [Read_Nconf[kk]],ch_n,Read_Threshold [Read_Nconf[kk]][ch_n]);
#endif
	}
	else
	{
	  printf("ReadConfigFile: Unknown Field or Missed Field in\n");
	  printf("   %s \n", fname);
	  printf("   str_tmp=%s", str_tmp);
	  printf("   keyword=%s \n\n", keyword);                                return(-6);
	}
      }
    }
  }
  fclose(ff);

  v895_NNconf = kk + 1;


  /* Prepare global variables for Download */
  for(ii=0; ii<v895_NNconf; ii++)
  {
    for(jj=0;jj<Read_N_Thresh[Read_Nconf[ii]];jj++)
      v895_Threshold   [Read_Nconf[ii]] [Read_ch_Thresh[Read_Nconf[ii]][jj]] =
	Read_Threshold [Read_Nconf[ii]] [Read_ch_Thresh[Read_Nconf[ii]][jj]];
    v895_Width    [Read_Nconf[ii]][0] = Read_Width    [Read_Nconf[ii]][0];
    v895_Width    [Read_Nconf[ii]][1] = Read_Width    [Read_Nconf[ii]][1];
    v895_BaseAddr [Read_Nconf[ii]]    = Read_BaseAddr [Read_Nconf[ii]];
    v895_Majority [Read_Nconf[ii]]    = Read_Majority [Read_Nconf[ii]];
    v895_Mask     [Read_Nconf[ii]]    = Read_Mask     [Read_Nconf[ii]];
    v895_Nconf    [ii]                = Read_Nconf    [ii];
  }


  return(kk);
}


#define V895_DOWNLOAD(DNN,Dkk) \
  int kk, jj; \
  for(kk=0; kk<(DNN); kk++) \
  { \
    v895SetMask     (v895_BaseAddr[(Dkk)], v895_Mask[(Dkk)]); \
    v895SetMajority (v895_BaseAddr[(Dkk)], v895_Majority[(Dkk)]); \
    v895SetWidth    (v895_BaseAddr[(Dkk)], 0, v895_Width[(Dkk)][0]); \
    v895SetWidth    (v895_BaseAddr[(Dkk)], 8, v895_Width[(Dkk)][1]); \
    for(jj=0;jj<16;jj++) \
      v895SetThreshold (v895_BaseAddr[(Dkk)],jj,v895_Threshold[(Dkk)][jj]); \
  } \
  return(0)


int
v895DownloadAll()
{
  V895_DOWNLOAD(v895_NN,kk);
}


int
v895DownloadConfig()
{
  V895_DOWNLOAD(v895_NNconf,v895_Nconf[kk]);
}


int
v895SetMask(UINT32 addr, UINT16 mask)
{
  CHECK_NUMBER_vs_ADDR;
  v895->pattern_inhibit = v895_Mask[v895_number] = mask;
  return(0);
}


int
v895EnableChannel(UINT32 addr, int channel)
{
  CHECK_NUMBER_vs_ADDR;
  CHECK_CHANNEL_NUMBER;
  v895_Mask[v895_number] |= (1 << channel);
  v895->pattern_inhibit   = v895_Mask[v895_number];
  /*  printf("v895_Mask[%d] = 0x%04x \n", v895_number, v895_Mask[v895_number]); */
  return(0);
}


int
v895EnableAllChan(UINT32 addr)
{
  CHECK_NUMBER_vs_ADDR;
  v895->pattern_inhibit = v895_Mask[v895_number] = V895_ENABLE_ALL;
  return(0);
}


int
v895EnableAll()
{
  int kk;
  for(kk=0; kk<v895_NN; kk++)
    v895EnableAllChan(v895_BaseAddr[kk]);
  return(0);
}


int
v895DisableChannel(UINT32 addr, int channel)
{
  CHECK_NUMBER_vs_ADDR;
  CHECK_CHANNEL_NUMBER;
  v895_Mask[v895_number] &= ~(1 << channel);
  v895->pattern_inhibit   = v895_Mask[v895_number];
  return(0);
}


int
v895DisableAllChan(UINT32 addr)
{
  CHECK_NUMBER_vs_ADDR;
  v895->pattern_inhibit = v895_Mask[v895_number] = 0x0;
  return(0);
}


int
v895DisableAll()
{
  int kk;
  for(kk=0; kk<v895_NN; kk++)
    v895DisableAllChan(v895_BaseAddr[kk]);
  return(0);
}



/***************************************************************************
 * The thresholds values can be programmed in a range from -1 mV to -255 mV
 * with 1 mV steps, writing an integer number between 1 and 255.
 */
int
v895SetThreshold(UINT32 addr, int channel, int thresh)
{
  CHECK_NUMBER_vs_ADDR;
  CHECK_CHANNEL_NUMBER;
  if(thresh < 1)    thresh = 1;
  if(thresh > 255)  thresh = 255;
  v895->threshold[channel] = v895_Threshold[v895_number][channel] = thresh;
  return(0);
}


int
v895ReadThreshold(UINT32 addr, int channel)
{
  CHECK_NUMBER_vs_ADDR;
  CHECK_CHANNEL_NUMBER;
  return(v895_Threshold[v895_number][channel]);
}


int
v895SetUniThreshold(UINT32 addr, int thresh)
{
  int jj;
  CHECK_NUMBER_vs_ADDR;
  if(thresh < 1)    thresh = 1;
  if(thresh > 255)  thresh = 255;
  for(jj=0; jj<16; jj++)
    v895->threshold[jj] = v895_Threshold[v895_number][jj] = thresh;
  return(0);
}


int
v895SetThresholdAll(int thresh)
{
  int kk;
  if(thresh < 1)    thresh = 1;
  if(thresh > 255)  thresh = 255;
  for(kk=0; kk<v895_NN; kk++)
    v895SetUniThreshold(v895_BaseAddr[kk], thresh);
  return(0);
}


/*************************************************************************
 * The width value can be adjusted in the range from 5 ns to 40 ns,
 * writing an integer number between 0 and 255 into the register.
 * The set value corresponds to the width as follows:
 * 255 leads to a 40 ns pulse duration, 0 leads to a 5 ns pulse duration,
 * with a non-linear relation for intermediate values.
 * Nevertheless we used linear equation to get intermediate values !!!
 */
int
v895SetWidth(UINT32 addr, int channel, int width)
{
  int data, grp;
  CHECK_NUMBER_vs_ADDR;
  CHECK_CHANNEL_NUMBER;
  if(width < 4)  width = 4;
  if(width > 43) width = 43;

  grp  = channel / 8;
  v895_Width[v895_number][grp] = width;
  v895->width[grp]             = v895_DAC_Width[width];

  return(0);
}


int
v895ReadWidth(UINT32 addr, int channel)
{
  CHECK_NUMBER_vs_ADDR;
  CHECK_CHANNEL_NUMBER;
  return(v895_Width[v895_number][channel/8]);
}


int
v895SetUniWidth(UINT32 addr, int width)
{
  if(v895SetWidth(addr,0,width) != 0) return(-1);
  if(v895SetWidth(addr,8,width) != 0) return(-2);
  return(0);
}


int
v895SetWidthAll(int width)
{
  int kk;
  for(kk=0; kk<v895_NN; kk++)
    v895SetUniWidth(v895_BaseAddr[kk], width);
  return(0);
}


/***************************************************************************
 * Majority output provides a standard NIM signal if the number of channels
 * over threshold exceeds the programmed majority level (MAJLEV).
 *
 * MAJLEV can be programmed between 1 and 16 for Internal Majority Logic
 *                      and between 1 and 20 for External Majority Logic,
 *
 * writing a proper value (MAJTHR) in the Majority threshold register,
 * valid values range between 0 and 255 :
 *
 *   MAJTHR = NINT[(MAJLEV*50 - 25)/4], where NINT is the Nearest Integer.
 */
int
v895SetMajority(UINT32 addr, int majority_level)
{
  int data;
  CHECK_NUMBER_vs_ADDR;
  if(majority_level < 1)  majority_level = 1;
  if(majority_level > 20) majority_level = 20;

  data = (majority_level*50 - 25) / 4;
  if(   ((majority_level*50 - 25) % 4) >= 2)   data++;

  v895_Majority[v895_number] = majority_level;
  v895->majority_thresh      = data;

  return(0);
}


int
v895ReadMajority(UINT32 addr)
{
  CHECK_NUMBER_vs_ADDR;
  return(v895_Majority[v895_number]);
}


int
v895TestPulse(UINT32 addr)
{
  CHECK_NUMBER_vs_ADDR;
  v895->test_pulse =1;
  return(0);
}


int
v895GetVersion(UINT32 addr)
{
  CHECK_NUMBER_vs_ADDR;
  return((v895->serial_number & V895_VERSION) >> 12);
}


int
v895GetSerialN(UINT32 addr)
{
  CHECK_NUMBER_vs_ADDR;
  return(v895->serial_number & V895_SERIAL_NUMBER);
}


int
v895GetManufactN(UINT32 addr)
{
  CHECK_NUMBER_vs_ADDR;
  return((v895->module_type & V895_MANUFACTURER_NUMBER) >> 10);
}


int
v895GetModuleType(UINT32 addr)
{
  CHECK_NUMBER_vs_ADDR;
  return(v895->module_type & V895_MODULE_TYPE);
}


int
v895GetFixedCode1(UINT32 addr)
{
  CHECK_NUMBER_vs_ADDR;
  return(v895->fixed_code & V895_FIXED_CODE_1);
}


int
v895GetFixedCode2(UINT32 addr)
{
  CHECK_NUMBER_vs_ADDR;
  return((v895->fixed_code & V895_FIXED_CODE_2) >> 8);
}


int
v895GetNumberByAddr(UINT32 addr)
{
  int ii, v895_number = -1;
  for(ii=0; ii<v895_NN; ii++)
    if(v895_BaseAddr[ii] == addr ) return(ii);
  return(v895_number);
}


UINT32
v895GetAddrByNumber(int board_number)
{
  UINT32 addr = 0xffffffff;
  if((board_number >= 0) && (board_number < v895_NN))
    return(v895_BaseAddr[board_number]);
  return(addr);
}


void
Set_DISCRADR(UINT32 addr)
{
  DISCRADR = addr & 0xffffff;
}


void
Set_DISCRSTEP(UINT32 addr)
{
  DISCRSTEP = addr & 0xffffff;
}





void
v895prn(int Nboard, int channel)
{
  char *ss = "     ";
  printf(" v895 [n=%d][ch=%d]  at 0x%08x  t=0x%02x  w=0x%02x  mj=0x%02x  m=0x%04x\n",
	 Nboard, channel,
	 v895_BaseAddr[Nboard],
	 v895_Threshold[Nboard][channel],
	 v895_Width[Nboard][channel/8],
	 v895_Majority[Nboard],
	 v895_Mask[Nboard]);
  printf ("%s Fixed code #1  = 0x%02x     Fixed code #2 = 0x%02x\n", ss,
	  v895GetFixedCode1(v895_BaseAddr[Nboard]),
	  v895GetFixedCode2(v895_BaseAddr[Nboard]));
  printf ("%s Manufacturer N = %d        Module type   = %d\n", ss,
	  v895GetManufactN(v895_BaseAddr[Nboard]),
	  v895GetModuleType(v895_BaseAddr[Nboard]));
  printf ("%s Version        = %d        Serial number = %d\n", ss,
	  v895GetVersion(v895_BaseAddr[Nboard]),
	  v895GetSerialN(v895_BaseAddr[Nboard]));
}


void
v895xxx(UINT32 addr)
{
  volatile V895 *v895 = (V895 *) addr;
  return;
}

void
SP_11()
{
  static int dd;

  printf (" 1 dd=%d\n", dd);
  dd++;
  printf (" 2 dd=%d\n", dd);
  dd++;
  printf (" 3 dd=%d\n\n", dd);
}

void
SP_22()
{
  SP_11();
  SP_11();
  SP_11();
}





#endif

