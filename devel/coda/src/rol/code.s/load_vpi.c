/*  load_vpi.c
 *
 *  originally pre_trigger.c, from Marchand, for 68K processor
 *
 * modified by E.Wolin, 28-may-1997
 * modified by vhg      06/13/1997            
 * modified by vhg      07/19/1997
 * modified by vhg      10/27/1997
 */

/* Sergey Boyarinov (Sep 2003): PPC version */

#ifdef VXWORKS

#include <stdio.h>
#include <vxWorks.h>
#include <vxLib.h>

#ifdef VXWORKSPPC
#define PRE_TRIGGER_BASE_ADDRS (short *) 0xfbffd000
#else
#define PRE_TRIGGER_BASE_ADDRS (short *) 0xffffd000
#endif

#define WIDTH_MIN 30
#define WIDTH_MAX 183
#define ADC_MIN 30
#define ADC_MAX 1300


/*----------------------------------------------------------------------*/

int
set_vpi_thresh(char board, short thresh)
{
  short testw;
  char *pthresh;
  short *ptemp;
  short rthresh;
  short readout;
  
  
  if( (thresh < ADC_MIN) || (thresh > ADC_MAX) )
  {
    printf("error: threshold out of allowed range(30-1300 mV) \n");
    return(ERROR);
  }
  
  rthresh = (short) (thresh*4095/787);
  rthresh &= 0x0fff;
  
  pthresh = (void *)( (int)PRE_TRIGGER_BASE_ADDRS);
  pthresh = (void *)((int)pthresh | (int)board*16);
  
  if(vxMemProbe(pthresh,VX_READ,sizeof(testw),(char *)&testw)!=OK)
  {
    printf("error: can not adress the board adress 0x%x\n",(int)pthresh);
    return(ERROR);
  }
  
  ptemp = (short *) pthresh;
  *ptemp = (short) rthresh;
  readout = (short) *ptemp;
  readout &= 0x0fff;
  
  if(rthresh != readout)
  {
    printf("error: write threshold failed \n");
    return(ERROR);
  } 
  
  printf("success: wrote threshold of %d mV in board %d \n",thresh,(int)board); 

  return(OK);
}

/*----------------------------------------------------------------------*/

int
set_vpi_width(char board, short width)
{
  short testw;
  char *pwidth;
  short *ptemp;
  short rwidth;
  short readout;

  if( (width < WIDTH_MIN) || (width > WIDTH_MAX) )
  {
    printf("error: width out of allowed range(30-163 ns) \n");
    return(ERROR);
  }
  
  rwidth = (short) ((width-30)*5/3);
  rwidth &= 0x00ff;
  
  pwidth = (void *)((int)PRE_TRIGGER_BASE_ADDRS+4);
  pwidth = (void *)((int)pwidth | (int)board*16);
  
  if(vxMemProbe(pwidth,VX_READ,sizeof(testw),(char *)&testw)!=OK)
  {
    printf("error: can not adress the board adress 0x%x\n",(int)pwidth);
    return(ERROR);
  }
  
  ptemp = (short *) pwidth;
  *ptemp = (short) rwidth;
  readout = (short) *ptemp;
  readout &= 0x00ff;
  
  if(rwidth != readout)
  {
    printf("error: write width failed \n");
    return(ERROR);
  }

  printf("success: wrote width of %d ns in board %d \n",width,(int)board);

  return(OK);
}


/*----------------------------------------------------------------------*/


int
mon_vpi_width(char board)
{
  short testw;
  char *pwidth;
  short prwidth;

  
  pwidth = (void *)((int)PRE_TRIGGER_BASE_ADDRS+4);
  pwidth = (void *)((int)pwidth | (int)board*16);
  
  if(vxMemProbe(pwidth,VX_READ,sizeof(testw),(char *)&testw)!=OK)
  {
    printf("error: can not adress the board adress 0x%x\n",(int)pwidth);
    return(ERROR);
  }
  
  testw &= 0x00ff;
  prwidth = (short) ((testw*3/5)+30); 

  printf("program monitored width of %d ns in board %d \n",
    (int)prwidth,(int)board);

  return(OK);
}


/*----------------------------------------------------------------------*/


int
mon_vpi_thresh(char board)
{
  short testw;
  char *pthresh;
  short prthresh;
  
  pthresh = (void *)( (int)PRE_TRIGGER_BASE_ADDRS);
  pthresh = (void *)((int)pthresh | (int)board*16);
  
  if(vxMemProbe(pthresh,VX_READ,sizeof(testw),(char *)&testw)!=OK)
  {
    printf("error: can not adress the board adress 0x%x\n",(int)pthresh);
    return(ERROR);
  } 
  
  testw &= 0x0fff;
  prthresh = (short) (testw*787/4095); 
  
  printf("program monitored threshold of %d mV in board %d \n",(int)prthresh,(int)board); 

  return(OK);
}


/*----------------------------------------------------------------------*/

int
set_vpi_mask(char board, short mask)
{
  short testw;
  char *pmask;
  short *ptemp;
  short rmask;
  short readout;

  
  rmask = (short) mask;
  
  pmask = (void *)((int)PRE_TRIGGER_BASE_ADDRS+8);
  pmask = (void *)((int)pmask | (int)board*16);
  
  if(vxMemProbe(pmask,VX_READ,sizeof(testw),(char *)&testw)!=OK)
  {
    printf("error: can not adress the board adress 0x%x\n",(int)pmask);
    return(ERROR);
  } 
  
  ptemp = (short *) pmask;
  *ptemp = (short) rmask;
  readout = (short) *ptemp;
  readout &= 0x0fff;
  

  if(rmask != readout)
  {
    printf("error: write maskold failed \n");
    return(ERROR);
  } 
  
  printf("success: wrote mask = %x  in board %d \n",mask,(int)board); 

  return(OK);
}


/*----------------------------------------------------------------------*/


int
mon_vpi_mask(char board)
{
  short testw;
  char *pmask;
  
  pmask = (void *)((int)PRE_TRIGGER_BASE_ADDRS+8);
  pmask = (void *)((int)pmask | (int)board*16);
  
  if(vxMemProbe(pmask,VX_READ,sizeof(testw),(char *)&testw)!=OK)
  {
    printf("error: can not adress the board adress 0x%x\n",(int)pmask);
    return(ERROR);
  } 
  
  testw &= 0x0fff;
  
  printf("program registrated mask = %x  in board %d \n",(int)testw,(int)board); 

  return(OK);
}


/*----------------------------------------------------------------------*/
int
mon_vpi_all2d(void)
{
  short testw;
  char *pthresh;
  short prthresh;
  char *pwidth;
  short prwidth;
  char *pmask;
  int i, board;
  int mean_thr = 0, mean_width = 0;
  int c_mtr = 0, c_wdr = 0;

  for (i=1;i<=9;i++)
  {
    board=i;

    /* Threshold read */

    pthresh = (void *)( (int)PRE_TRIGGER_BASE_ADDRS);
    pthresh = (void *)((int)pthresh | (int)board*16);
	
    if(vxMemProbe(pthresh,VX_READ,sizeof(testw),(char *)&testw)!=OK)
    { 
      printf("error: can not adress the board adress 0x%x\n",(int)pthresh);
      return(ERROR);
    }
	
    testw &= 0x0fff;
    prthresh = (short) (testw*787/4095); 
  
    printf("%6d",(int)prthresh); 
    mean_thr += (int)prthresh;
    c_mtr++;

    testw=0;

    /* width read */

    pwidth = (void *)((int)PRE_TRIGGER_BASE_ADDRS+4);
    pwidth = (void *)((int)pwidth | (int)board*16);
	
    if(vxMemProbe(pwidth,VX_READ,sizeof(testw),(char *)&testw)!=OK)
    { 
      printf("error: can not adress the board adress 0x%x\n",(int)pwidth);
      return(ERROR);
    }
	
    testw &= 0x00ff;
    prwidth = (short) ((testw*3/5)+30); 

    printf("%6d",(int)prwidth);
    mean_width += (int)prwidth;
    c_wdr++;

    testw=0;

    /* mask read */

    pmask = (void *)((int)PRE_TRIGGER_BASE_ADDRS+8);
    pmask = (void *)((int)pmask | (int)board*16);
	
    if(vxMemProbe(pmask,VX_READ,sizeof(testw),(char *)&testw)!=OK)
    { 
      printf("error: can not adress the board adress 0x%x\n",(int)pmask);
      return(ERROR);
    } 
	
    testw &= 0x0fff;
  
    printf("%6d\n",(int)testw); 
  }

  mean_thr = mean_thr/c_mtr;
  mean_width = mean_width/c_wdr;
  printf("%6d%6d%6d\n",mean_thr, mean_width,0);

  return (OK);
}


/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
int
mon_vpi_all2(void)
{
  short testw;
  char *pthresh;
  short prthresh;
  char *pwidth;
  short prwidth;
  char *pmask;
  int i, board;

  for(i=1; i<=9; i++)
  {
    board=i;

    /* Threshold read */

    pthresh = (void *)( (int)PRE_TRIGGER_BASE_ADDRS);
    pthresh = (void *)((int)pthresh | (int)board*16);
	
    if(vxMemProbe(pthresh,VX_READ,sizeof(testw),(char *)&testw)!=OK)
    {
      printf("error: can not adress the board adress 0x%x\n",(int)pthresh);
      return(ERROR);
    } 
	
    testw &= 0x0fff;
    prthresh = (short) (testw*787/4095); 
  
    printf(" %6d", (int)prthresh);
    testw=0;

    /* width read */

    pwidth = (void *)((int)PRE_TRIGGER_BASE_ADDRS+4);
    pwidth = (void *)((int)pwidth | (int)board*16);
	
    if(vxMemProbe(pwidth,VX_READ,sizeof(testw),(char *)&testw)!=OK)
    {
      printf("error: can not adress the board adress 0x%x\n",(int)pwidth);
      return(ERROR);
    }
	
    testw &= 0x00ff;
    prwidth = (short) ((testw*3/5)+30); 

    printf(" %6d", (int)prwidth);

    testw=0;

   /* mask read */

    pmask = (void *)((int)PRE_TRIGGER_BASE_ADDRS+8);
    pmask = (void *)((int)pmask | (int)board*16);
	
    if(vxMemProbe(pmask,VX_READ,sizeof(testw),(char *)&testw)!=OK)
    {
      printf("error: can not adress the board adress 0x%x\n",(int)pmask);
      return(ERROR);
    }
	
    testw &= 0x0fff;
    printf(" %6d \n", (int)testw);
  }

  return (OK);
}


/*----------------------------------------------------------------------*/


int
mon_vpi_all(void)
{
  int i;

  for(i=1;i<=9;i++)
  {
    mon_vpi_thresh(i);
    mon_vpi_width(i);
    mon_vpi_mask(i);
  }

  return(OK);
}

/*______________________________________________________________________*/

int
set_vpi_all(short t, short w, short m)
{
  int i;

  for(i=1;i<=9;i++)
  {
    set_vpi_thresh(i,t);
    set_vpi_width(i,w);
    set_vpi_mask(i,m);
  }

  return(OK);
}





/*FAST LOAD (Sergey B. 6-mar-2008) _____________________________________________________*/


int
set_vpi_all_pack(unsigned int twm1, unsigned int twm2, unsigned int twm3,
                 unsigned int twm4, unsigned int twm5, unsigned int twm6,
                 unsigned int twm7, unsigned int twm8, unsigned int twm9)
{
  short t[9],w[9],m[9];

  /* unpack */
  t[0] = twm1 & 0x3FF;
  t[1] = twm2 & 0x3FF;
  t[2] = twm3 & 0x3FF;
  t[3] = twm4 & 0x3FF;
  t[4] = twm5 & 0x3FF;
  t[5] = twm6 & 0x3FF;
  t[6] = twm7 & 0x3FF;
  t[7] = twm8 & 0x3FF;
  t[8] = twm9 & 0x3FF;

  w[0] = (twm1>>10) & 0x3FF;
  w[1] = (twm2>>10) & 0x3FF;
  w[2] = (twm3>>10) & 0x3FF;
  w[3] = (twm4>>10) & 0x3FF;
  w[4] = (twm5>>10) & 0x3FF;
  w[5] = (twm6>>10) & 0x3FF;
  w[6] = (twm7>>10) & 0x3FF;
  w[7] = (twm8>>10) & 0x3FF;
  w[8] = (twm9>>10) & 0x3FF;

  m[0] = (twm1>>20) & 0x3FF;
  m[1] = (twm2>>20) & 0x3FF;
  m[2] = (twm3>>20) & 0x3FF;
  m[3] = (twm4>>20) & 0x3FF;
  m[4] = (twm5>>20) & 0x3FF;
  m[5] = (twm6>>20) & 0x3FF;
  m[6] = (twm7>>20) & 0x3FF;
  m[7] = (twm8>>20) & 0x3FF;
  m[8] = (twm9>>20) & 0x3FF;

  set_vpi_thresh(1,t[0]);
  set_vpi_thresh(2,t[1]);
  set_vpi_thresh(3,t[2]);
  set_vpi_thresh(4,t[3]);
  set_vpi_thresh(5,t[4]);
  set_vpi_thresh(6,t[5]);
  set_vpi_thresh(7,t[6]);
  set_vpi_thresh(8,t[7]);
  set_vpi_thresh(9,t[8]);

  set_vpi_width(1,w[0]);
  set_vpi_width(2,w[1]);
  set_vpi_width(3,w[2]);
  set_vpi_width(4,w[3]);
  set_vpi_width(5,w[4]);
  set_vpi_width(6,w[5]);
  set_vpi_width(7,w[6]);
  set_vpi_width(8,w[7]);
  set_vpi_width(9,w[8]);

  set_vpi_mask(1,m[0]);
  set_vpi_mask(2,m[1]);
  set_vpi_mask(3,m[2]);
  set_vpi_mask(4,m[3]);
  set_vpi_mask(5,m[4]);
  set_vpi_mask(6,m[5]);
  set_vpi_mask(7,m[6]);
  set_vpi_mask(8,m[7]);
  set_vpi_mask(9,m[8]);

  return(OK);
}


/*______________________________________________________________________*/

#else /* no UNIX version */

void
load_vpi_dummy()
{
  return;
}

#endif
