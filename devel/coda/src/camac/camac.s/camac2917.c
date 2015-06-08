
#ifdef VXWORKS

/*
   camac2917.c - copied from libcamac.c and adjusted for multiple
                 branches by Sergey Boyarinov

 NOTE: using 'volatile' is VERY important in that file; use it whenever
       you are accessing VME board(s) registers !!!

 Functions (all return void):

  ccinit(int b)			 initialize branch 
  cdreg(int *ext,int b,int c,int n,int a)   define external address 
  cfsa(int f,int ext,int *dat,int *q)       long (24) single action 
  cssa(int f,int ext,short *dat,int *q)     short (16) single action 
  cccz(int ext)			 crate initialize 
  cccc(int ext)			 crate clear 
  ccci(int ext,int l)			 set/reset crate inhibit 
  ctci(int ext,int *l)			 test crate inhibit 
  cccd(int ext,int l)			 ena/disable crate demand 
  ctcd(int ext,int *l)			 test crate demand enabled
  ctstatus(int ext, int *istat)	 test status of previous operation 
  ctstat(int *istat)			   same, but for the last accessed branch 

  ccgl(int lam,int l)			 ena/disaable graded lam (NON STANDARD)
  ctgl(int ext,int *l)			 test graded lam
  cdlam(int *lam,int b,int c,int n,int a,int inta[]) 	 define lam 
  cclc(int lam)			 clear lam (dataless function)
  cclm(int lam,int l)			 alternative lam clears 
  ctlm(int lam,int *l)			 test lam 
  lamIsr(int b)                  interrupt handler
  cclnk(int lam,void *())		 link routine to lam 
  cculk(int lam,void *())		 unlink routine from lam 
  ccrgl(int lam)			 re-enable graded lam (@end of isr)
  cclwt(int lam)			 lam wait 
  clpoll(int lam)

  camac2917scan()
*/

#include <stdio.h>
#include <vxWorks.h>
#include <vxLib.h>
#include <intLib.h>
#include <taskLib.h>
#include <sigLib.h>
#include "vme.h"
#include "camac2917.h"

extern int printf();
extern STATUS sysIntEnable();

#ifdef VXWORKSPPC
extern STATUS intDisconnect();
extern STATUS intEnable();
extern int sysBusIntAck();
#endif

int sysBusToLocalAdrs();

/* use following flag to get debug messages */
#undef DEBUG

/* lock/unlock, guarantee only 1 caller at a time */
/* use NULL routines for performance */
int camacLockKey;
#define CAMAC_RAISE_IPL camacLockKey = intLock();
#define CAMAC_LOWER_IPL intUnlock(camacLockKey);
/*
#define CAMAC_RAISE_IPL ;
#define CAMAC_LOWER_IPL ;
*/


/* enables/disables the geting csr and retrieval of q in the routines
  cfsa(), cssa(), cfga(), and csga() */
#define GET_KS2917_CSR(csr,stat) stat = *csr;
#define DECODE_KS2917_CSR(stat,dat,q) \
  if(stat & (ks2917_error|ks2917_timeo)) \
  { \
    *(dat) = 0; \
    *(q) = 0; \
  } else \
    *(q) = stat&2 ? 0:1;
/*
#define GET_KS2917_CSR(a,b)
#define DECODE_KS2917_CSR(stat,dat,q)
*/


#define SET_BOARD_ADDRESS \
  int b; \
  volatile CAMAC2917 *ks2917; \
\
  /* extract branch number and cleanup 'b' field in 'ext' */ \
  b = ((ext) >> 29) & 0x7; \
  ks2917 = (CAMAC2917 *) ks2917base[b]; \
  (ext) &= 0x1FFFFFFF; \
  ks2917last = ks2917



/* the number of branches and branch addresses and interrupt vectors */
#define NBRANCH 8
static unsigned long KS2917ADR[NBRANCH]     = {0xFF00, 0xFE00, 0xFD00, 0xFC00,
                                               0xFB00, 0xFA00, 0xF900, 0xF800};
static unsigned int KS2917_LAM_VEC[NBRANCH] = {0xa0,   0xa4,   0xa8,   0xac,
                                               0xb0,   0xb4,   0xb8,   0xbc};


VOIDFUNCPTR lamisrs[NBRANCH][25][8];
int taskidlam[NBRANCH][25][8];

static unsigned long ks2917base[NBRANCH];
static int camacStatus[NBRANCH];
static short cdreg_pattern[NBRANCH][MAX_CDREG_PATTERN];
volatile static CAMAC2917 *ks2917last; /* last used board pointer */

void
ccinit(int b)
{
  int i, status;
  unsigned short data;
  unsigned long ppc_offset;
  volatile CAMAC2917 *ks2917;

  /* check parameter */
  if(b < 0 || b >= NBRANCH)
  {
    printf("ccinit: ERROR: wrong branch number %d, must be from 0 to %d\n",
           b,NBRANCH-1);
    return;
  }

  /* get actual VME address */
  sysBusToLocalAdrs(0x29,0,&ppc_offset);
  ks2917base[b] = ppc_offset + KS2917ADR[b];
  ks2917 = (CAMAC2917 *) ks2917base[b];
  ks2917last = ks2917;
  printf("   ccinit: ks2917base[%1d]=0x%08x\n",b,ks2917base[b]);

  camacStatus[b] = 0;


  /*status = vxMemProbe((char *)&ks2917->csr,READ,2,(char *)&data);*/
  status = OK;


  if(status == OK)
  {
    printf("ccinit: INFO: vxMemProbe(0x%08x) returns %d\n",
           &ks2917->csr,status);
    ks2917->csr = ks2917_reset;
    ks2917->cmar = 0; /* preload halt's everywhere */


/* hung here !!! */
    for(i=0; i<8192; i++) ks2917->cmr = ks2917_halt;


    for(i=0; i<MAX_CDREG_PATTERN; i++) cdreg_pattern[b][i] = 0;
    ks2917->sccr = ks2917_sccr_abrt; /* abort any left over DMA */
    ks2917->amr = VME_AM_EXT_USR_DATA | 0x40; /*DMA address modifier | LWORD* */

goto skipInt;/* Sergey */
#ifdef VXWORKSPPC
    intDisconnect(INUM_TO_IVEC(KS2917_LAM_VEC[b]));
#endif
    intConnect((VOIDFUNCPTR *) INUM_TO_IVEC(KS2917_LAM_VEC[b]),
               (VOIDFUNCPTR) lamIsr, b);
    ks2917->ivr_lam = KS2917_LAM_VEC[b]; /* int vector */
    ks2917->icr_lam = 0x0a; /* disable ints, auto-clr, level 2 */
    sysIntEnable(2); /* enable bus interrupts on level 2 */
#ifdef VXWORKSPPC
    intEnable(11); /* enable VME interrupts on IBC chip */
#endif
skipInt:

    camacStatus[b] = 1;
  }
  else
  {
    printf("ccinit: ERROR: vxMemProbe(0x%08x) returns %d\n",
           &ks2917->csr,status);
  }

  return;
}



/* Macros */

#define KS2917_WHEN_RDY(statement) \
{ \
  int count; \
  if(!(ks2917->csr & ks2917_rdy)) \
  { \
    for(count=0; \
        (count<1000) && (!(ks2917->csr & (ks2917_rdy|ks2917_error|ks2917_timeo))); \
        count++) ; \
    if(ks2917->csr & ks2917_rdy) \
    { \
      /*printf("KS2917_WHEN_RDY: GOOOD: count=%d, csr=0x%04x, rdy=0x%04x\n", \
		count,ks2917->csr,ks2917_rdy);*/ \
      statement; \
    } \
    else \
    { \
      printf("KS2917_WHEN_RDY: ERROR: count=%d, csr=0x%04x, rdy=0x%04x\n", \
             count,ks2917->csr,ks2917_rdy); \
    } \
  } \
  else \
  { \
    statement; \
  } \
}

#define ks2917_wait_done \
{ int count; \
  if (!(ks2917->csr & (ks2917_done|ks2917_error|ks2917_timeo))) \
    for (count=0;(count<1000)&& \
         (!(ks2917->csr & (ks2917_done|ks2917_error|ks2917_timeo)));count++) ; \
}

#define ks2917_wait_block \
{ int count,iwait; \
  if (!(ks2917->csr & (ks2917_done|ks2917_error|ks2917_timeo))) \
    for (count=0; \
         ( count<1000)&& \
           (!(ks2917->csr & (ks2917_done|ks2917_error|ks2917_timeo))); \
         count++) {\
      for (iwait=0;iwait<32;iwait++); }\
}

#define KS2917_READ16_NAF(ext,n,a,f,data) \
  ks2917->cmar = *((short *)&(ext)); \
  data = ks2917->cmr; /* fetch crate mask */ \
  ks2917->cmar = 0;   /* use 1st 3 words for operation */ \
  ks2917->cmr = data | ks2917_ws16; /* set for 16 bit read */\
  ks2917->cmr = (((n)<<9) | (a)<<5 | (f) ); \
  ks2917->cmr = ks2917_halt; \
  ks2917->cmar = 0; \
  ks2917->csr =  ks2917_go; \
  KS2917_WHEN_RDY(data = ks2917->dlr;) \
  ks2917_wait_done

#define KS2917_WRITEBACK16_NAF(n,a,f,data) \
  ks2917->cmar = 1; \
  ks2917->cmr = (((n)<<9)|(a)<<5 |(f) );	\
  ks2917->cmar = 0; \
  ks2917->csr = ks2917_write + ks2917_go; \
  KS2917_WHEN_RDY(ks2917->dlr = data;) \
  ks2917_wait_done

#define KS2917_READ24_NAF(ext,n,a,f,data) \
  ks2917->cmar = *((short *)&(ext)); \
  data = ks2917->cmr; /* fetch crate mask */ \
  ks2917->cmar = 0;   /* use 1st 3 words for operation */ \
  ks2917->cmr = data | ks2917_ws24; /* 24 bit read */\
  ks2917->cmr = (((n)<<9) | (a)<<5 | (f) ); \
  ks2917->cmr = ks2917_halt; \
  ks2917->cmar = 0; \
  ks2917->csr =  ks2917_go; \
  KS2917_WHEN_RDY(data = ks2917->dlr | (ks2917->dhr<<16);); \
  ks2917_wait_done

#define KS2917_WRITEBACK24_NAF(n,a,f,data) \
  ks2917->cmar = 1; \
  ks2917->cmr = (((n)<<9)|(a)<<5 |(f) );	\
  ks2917->cmar = 0; \
  ks2917->csr = ks2917_write + ks2917_go; \
  KS2917_WHEN_RDY(ks2917->dlr = data; ks2917->dhr = data>>16;); \
  ks2917_wait_done



/*************/
/* functions */
/*************/


void
cdreg(int *ext, int b, int c, int n, int a)
{
  short match, temp;
  int i;
  volatile CAMAC2917 *ks2917;

  ks2917 = (CAMAC2917 *) ks2917base[b];
  ks2917last = ks2917;

  /* 'match' contains following fields:

 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
-------------------------------------------------
|          |   c    |      n       |     a      |


     'ext' contains following fields:

 31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
-------------------------------------------------------------------------------------------------
|   b    |                   18*i               |     |      n       |     a     |              |


  where:
    b - branch number (0-7)
    c - crate number (0-7)
    n - station
    a - subaddress

  */

  match = (c<<9) | (n<<4) | (a<<0);
  for(i=1; ; i++)
  { /* low addresses reserved for temp NAF's */
    if(cdreg_pattern[b][i] == match)
    {
      temp = (n<<9) | (a<<5);
      *ext = (b<<29) | ((18*i) << 16) | temp;
#ifdef DEBUG
      printf("cdreg: old match, *ext=0x%08x\n",*ext);
#endif
      break;
    }

    /* Sergey: temporary solution for the long camac branch: */
    /* if we are out of space in 'cdreg_pattern', clean up last element */
    /* in it, it will be used again and again for all new cnaf's */
    if(i == (MAX_CDREG_PATTERN-2))
    {
#ifdef DEBUG
      printf("cdreg: reach max=%d\n",i);
#endif
      cdreg_pattern[b][i]=0;
    }

    if(cdreg_pattern[b][i]==0)
    {
      if(!(camacStatus[b]) || (i==(MAX_CDREG_PATTERN-1)) || ((b)&0xfffffff8) ||
         ((c)&(0xfffffff8)) || ((n)&(0xffffffe0)) || ((a)&(0xfffffff0)) )
      {
        *ext = -1;
#ifdef DEBUG
        printf("cdreg: ERROR: camacStatus[%1d]=%d i=%d b=0x%x c=0x%x n=0x%x a=0x%x\n",
               b,camacStatus[b],i,b,c,n,a);
        printf("cdreg: ERROR: returns *ext=0x%08x\n",*ext);
#endif
      }
      else
      {
        cdreg_pattern[b][i] = match;
        temp = ((n)<<9) | ((a)<<5); 
        *ext = (b<<29) | ((18*i) << 16) | temp; 
        CAMAC_RAISE_IPL; 

        ks2917->cmar = 18*i; /* jump to address in list memory */ 
        ks2917->cmr = (c)<<8 | ks2917_ws24; /* single word I/O */
        ks2917->cmr = temp; 
        ks2917->cmr = ks2917_halt; 
        ks2917->cmr = (c)<<8 | ks2917_tm_block | ks2917_qm_scan; /* q scan */ 
        ks2917->cmr = temp; 
        ks2917->cmr = -1; /* LS transfer length */ 
        ks2917->cmr = -1; /* MS transfer length */ 
        ks2917->cmr = ks2917_halt; 
        ks2917->cmr = (c)<<8 | ks2917_tm_block | ks2917_qm_stop; /* q stop */ 
        ks2917->cmr = temp; 
        ks2917->cmr = -1; /* LS transfer length */ 
        ks2917->cmr = -1; /* MS transfer length */
        ks2917->cmr = ks2917_halt;
        ks2917->cmr = (c)<<8 | ks2917_tm_block | ks2917_qm_repeat;/* q repeat */
        ks2917->cmr = temp;
        ks2917->cmr = -1; /* LS transfer length */
        ks2917->cmr = -1; /* MS transfer length */
        ks2917->cmr = ks2917_halt;
        CAMAC_LOWER_IPL;
      }

#ifdef DEBUG
      printf("cdreg: new match, b=%d c=%d n=%d a=%d i=%d\n",b,c,n,a,i);
      printf("cdreg: new match, *ext=0x%08x\n",*ext);
#endif

      break;
    }
  }

}

void
cfsa(int f, int ext, unsigned int *dat, unsigned int *q)
{
  unsigned short stat;
  volatile unsigned short *source;
  volatile unsigned short *csr;
  SET_BOARD_ADDRESS;

#ifdef DEBUG
  printf("cfsa: f=%d ext=0x%08x -> b=%d\n",f,ext,b);
#endif

  csr = (volatile unsigned short *) &(ks2917->csr); /* preload pointer to csr for speed */
  CAMAC_RAISE_IPL;
  if(f>0)
  {
    ks2917->cmar = *( (short *)&(ext) ) + 1;
    ks2917->cmr = *(( (short *)&(ext) ) + 1) | (f);
    ks2917->cmar = *( (short *)&(ext) );
    if(f>=8)
    {
      if(((f)&8) == 0) /* write operation */
      {
        *csr = ks2917_write + ks2917_go;
        KS2917_WHEN_RDY(ks2917->dlr = *(dat); ks2917->dhr = *((short *)(dat)););
        ks2917_wait_done;
      }
      else /* control operation */
      {
        *csr = ks2917_go;
        ks2917_wait_done;
      }
    }
    else /* read operation */
    {
      *csr = ks2917_go;
      KS2917_WHEN_RDY( *(dat) = ks2917->dlr;	/* fetch LS 16 bits of data */
                       *((short *)(dat)) = ks2917->dhr & 0xff; /* overwrite hi wrd w/MS 16 */
      );
    }
    ks2917->cmar = *((short *)&(ext)) + 1;
    ks2917->cmr = *(((short *)&(ext)) + 1); /* clear out the f bits */
  }
  else
  {
    ks2917->cmar = *((short *)&(ext)); /* f=0 read is fastest code */
#ifdef DEBUG
    printf("cfsa: ks2917->cmar=0x%04x\n",ks2917->cmar);
#endif
    *csr = ks2917_go;	/* f=0 read operation */
    source = (volatile unsigned short *) &ks2917->dlr;
    KS2917_WHEN_RDY( *((short *)(dat)+1) = *(source++);  /* fetch LS 16 bits of data */
                     *((short *)(dat)) = *(source) & 0xff; /* fetch MS 16 */
    );
  }
  GET_KS2917_CSR(csr,stat); /* get csr */
  CAMAC_LOWER_IPL;
  DECODE_KS2917_CSR(stat,dat,q); /* get q from csr image */

#ifdef DEBUG
  printf("cfsa: *dat=%d *q=%d\n",*dat,*q);
#endif
}



void
cfsa1(int f, int ext, unsigned int *dat, unsigned int *q)
{
  unsigned short stat;
  volatile unsigned short *source;
  volatile unsigned short *csr;
  SET_BOARD_ADDRESS;


  csr = (volatile unsigned short *) &(ks2917->csr); /* preload pointer to csr for speed */
logMsg("cfsa: f=%d ext=0x%08x -> b=%d (0x%08x 0x%08x 0x%08x)\n",f,ext,b,ks2917,csr,ks2917->cmar);
  CAMAC_RAISE_IPL;
  if(f>0)
  {
logMsg("cfsa 1\n",1,2,3,4,5,6);
    ks2917->cmar = *( (short *)&(ext) ) + 1;
logMsg("cfsa 2\n",1,2,3,4,5,6);
    ks2917->cmr = *(( (short *)&(ext) ) + 1) | (f);
logMsg("cfsa 3\n",1,2,3,4,5,6);
    ks2917->cmar = *( (short *)&(ext) ); /* hung here ! */
logMsg("cfsa 4\n",1,2,3,4,5,6);
return;
    if(f>=8)
    {
logMsg("cfsa 5\n",1,2,3,4,5,6);
      if(((f)&8) == 0) /* write operation */
      {
logMsg("cfsa 6\n",1,2,3,4,5,6);
return;
        *csr = ks2917_write + ks2917_go;
        KS2917_WHEN_RDY(ks2917->dlr = *(dat); ks2917->dhr = *((short *)(dat)););
        ks2917_wait_done;
      }
      else /* control operation */
      {
logMsg("cfsa 7\n",1,2,3,4,5,6);
return;
        *csr = ks2917_go;
        ks2917_wait_done;
      }
    }
    else /* read operation */
    {
      *csr = ks2917_go;
      KS2917_WHEN_RDY( *(dat) = ks2917->dlr;	/* fetch LS 16 bits of data */
                       *((short *)(dat)) = ks2917->dhr & 0xff; /* overwrite hi wrd w/MS 16 */
      );
    }
    ks2917->cmar = *((short *)&(ext)) + 1;
    ks2917->cmr = *(((short *)&(ext)) + 1); /* clear out the f bits */
  }
  else
  {
    ks2917->cmar = *((short *)&(ext)); /* f=0 read is fastest code */
#ifdef DEBUG
    printf("cfsa: ks2917->cmar=0x%04x\n",ks2917->cmar);
#endif
    *csr = ks2917_go;	/* f=0 read operation */
    source = (volatile unsigned short *) &ks2917->dlr;
    KS2917_WHEN_RDY( *((short *)(dat)+1) = *(source++);  /* fetch LS 16 bits of data */
                     *((short *)(dat)) = *(source) & 0xff; /* fetch MS 16 */
    );
  }
  GET_KS2917_CSR(csr,stat); /* get csr */
  CAMAC_LOWER_IPL;
  DECODE_KS2917_CSR(stat,dat,q); /* get q from csr image */


  logMsg("cfsa: *dat=%d *q=%d\n",*dat,*q,3,4,5,6);
}


/* following same as CFSA, but only transfers 16 bits */
void
cssa(int f, int ext, unsigned short *dat, unsigned int *q)
{
  unsigned short stat, temp;
  volatile unsigned short *csr;
  SET_BOARD_ADDRESS;

  csr = (volatile unsigned short *) &(ks2917->csr); /* preload pointer to csr for speed */
  CAMAC_RAISE_IPL;
  if(f>0)
  {
    if(f>=8)
    {
      if(((f)&8) == 0)
      { /* write operation */
        ks2917->cmar = *((short *)&(ext));
        temp = ks2917->cmr;      /* fetch crate mask */
        ks2917->cmar = 0;        /* use 1st 3 words */
        ks2917->cmr = temp | ks2917_ws16; /* 16 bit write */
        ks2917->cmr = *(((short *)&(ext))+1) | (f);
        ks2917->cmar = 0;
        *csr = ks2917_write + ks2917_go;
        KS2917_WHEN_RDY( ks2917->dlr = *(dat); );
        ks2917_wait_done;
      }
      else
      { 			/* control operation */
        ks2917->cmar = *((short *)&(ext)) + 1;
        ks2917->cmr = *(((short *)&(ext))+1) | (f);
        ks2917->cmar = *((short *)&(ext));
        *csr = ks2917_go;
        ks2917_wait_done;
        ks2917->cmar = *((short *)&(ext)) + 1;
        ks2917->cmr = *(((short *)&(ext))+1); /* clear out the f bits */
      }
    }
    else
    { 			/* read operation */
      *csr = ks2917_go;
      KS2917_WHEN_RDY( *(dat) = ks2917->dlr;	); /* fetch LS 16 bits of data */
      ks2917->cmar = *((short *)&(ext)) + 1;
      ks2917->cmr = *(((short *)&(ext))+1); /* clear out the f bits */
    }
  }
  else
  {
    ks2917->cmar = *((short *)&(ext)); /* f=0 read is fastest code */
    *csr = ks2917_go;	/* f=0 read operation */
    KS2917_WHEN_RDY( *(dat) = ks2917->dlr; ); /* fetch LS 16 bits of data */
  }
  GET_KS2917_CSR(csr,stat); /* get csr */
  CAMAC_LOWER_IPL;
  DECODE_KS2917_CSR(stat,dat,q); /* get q from csr image */
}

void
cccz(int ext)
{
  short temp;
  SET_BOARD_ADDRESS;

  CAMAC_RAISE_IPL;
  KS2917_READ16_NAF(ext,30,0,1,temp);
  KS2917_WRITEBACK16_NAF(30,0,17,temp|1);
  CAMAC_LOWER_IPL;
}

void
cccc(int ext)
{
  short temp;
  SET_BOARD_ADDRESS;

  CAMAC_RAISE_IPL;
  KS2917_READ16_NAF(ext,30,0,1,temp);
  KS2917_WRITEBACK16_NAF(30,0,17,temp|2);
  CAMAC_LOWER_IPL;
}

void
ccci(int ext, int l)
{
  short temp;
  SET_BOARD_ADDRESS;

  CAMAC_RAISE_IPL;
  KS2917_READ16_NAF(ext,30,0,1,temp);
  temp = l ? temp | 4 : temp & ~4;
  KS2917_WRITEBACK16_NAF(30,0,17,temp);
  CAMAC_LOWER_IPL;
}

void
ctci(int ext, int *l)
{
  short temp;
  SET_BOARD_ADDRESS;

  CAMAC_RAISE_IPL;
  KS2917_READ16_NAF(ext,30,0,1,temp);
  CAMAC_LOWER_IPL;
  *l = (temp>>2) & 1;
}

void
cccd(int ext, int l)
{
  short temp;
  SET_BOARD_ADDRESS;

  CAMAC_RAISE_IPL;
  KS2917_READ16_NAF(ext,30,0,1,temp);
  temp = l ? temp | 0x100 : temp & ~0x100;
  KS2917_WRITEBACK16_NAF(30,0,17,temp);
  CAMAC_LOWER_IPL;
}

void
ctcd(int ext, int *l)
{
  short temp;
  SET_BOARD_ADDRESS;

  CAMAC_RAISE_IPL;
  KS2917_READ16_NAF(ext,30,0,1,temp);
  CAMAC_LOWER_IPL;
  if(temp & 0x100) *l = 1;
  else             *l = 0;
}

void
ctstatus(int ext, int *istat)
{
  unsigned short temp;
  SET_BOARD_ADDRESS;

#ifdef DEBUG
  printf("ctstatus: ext=0x%08x\n",ext);
#endif

  if(camacStatus[b]==0)
  {
    printf("ctstatus: ERROR: branch %d does not exist\n",b);
    printf("ctstatus: ext=0x%08x\n",ext);
    *istat = 0x80;
    printf("ctstatus: *istat=0x%08x\n",*istat);
    return;
  }

  /* csr bits: 0-go, 1-NO-Q, 2-NO-X, 12-RST, 13-TMO, 15-ERR */
  temp = ks2917->csr;

  /* *istat bits: 0-NO-Q, 1-NO-X, 4-RST, 5-TMO, 7-ERR */
  *istat = ((temp>>1) & 3) | ((temp>>8) & 0xb0);

#ifdef DEBUG
  printf("ctstatus: *istat=0x%08x\n",*istat);
#endif
}


/* following function has no 'ext' in argument list so it will use
last accessed board address; it may create a problem if several
jobs accessing camac in the same time ... */
/* keep it just for backward compartibility */

void
ctstat(int *istat)
{
  unsigned short temp;

  /*printf("ctstat reached\n");*/
  temp = ks2917last->csr;
  *istat = ((temp>>1) & 3) | ((temp>>8) & 0xb0);

  /*printf("ctstat: *istat=0x%08x\n",*istat);*/
}






/*******************************/
/*******************************/
/* interrupt-related functions */


void
ccgl(int lam, int l)
{
  int temp;

int ext = lam;
SET_BOARD_ADDRESS;

  CAMAC_RAISE_IPL;
  KS2917_READ24_NAF(lam,30,13,1,temp); /* read crate controller lam mask */
  if(l)
  {
    temp = temp | (1<<((((lam)>>9)-1) & 31));
  }
  else
  {
    temp = temp & ~(1<<((((lam)>>9)-1) & 31));
  }
  KS2917_WRITEBACK24_NAF(30,13,17,temp); /* write lam mask */
  CAMAC_LOWER_IPL;
}
  
void
ctgl(int ext, int *l)
{
  short temp;
  SET_BOARD_ADDRESS;

  CAMAC_RAISE_IPL;
  KS2917_READ16_NAF(ext,30,0,1,temp);
  CAMAC_LOWER_IPL;
  *l = (temp>>15) & 1;
}

void
cdlam(int *ext, int b, int c, int n, int a, int inta[2])
{
  cdreg(ext,b,c,n,a);  /* ignore inta[] */
}

void
cclc(int lam)
{
  int temp;
  cfsa(10,(lam),&temp,&temp);
}

void
cclm(int lam, int l)
{
  unsigned int temp;

  if(l)
  {
    cfsa(26,(lam),&temp,&temp);
  }
  else
  {
    cfsa(24,(lam),&temp,&temp);
  }
}

void
ctlm(int lam, int *l)
{
  int temp1, temp2;

  cfsa(8,(lam),&temp1,&temp2);
  *l=temp2;
}

void
lamIsr(int b)
{
  int ii, jj, pattern=0;
  volatile CAMAC2917 *ks2917;

  ks2917 = (CAMAC2917 *) ks2917base[b];

#ifdef VXWORKSPPC
  sysBusIntAck(2);      /* Acknowledge VME Interrupt on IBC Chip */
#endif
  for(ii=0; ii<8; ii++)		/* loop over crates */
  {
    if(ks2917->srr & (1<<ii))
    {
      ks2917->cmar = 0;
      ks2917->cmr = ks2917_ws24 + (ii<<8); /* read LAM pattern */
      ks2917->cmr = ((30<<9) |(12<<5)| 1); /* N=30 A=12 F=1 */
      ks2917->cmr = ks2917_halt;
      ks2917->cmar = 0;
      ks2917->csr =  ks2917_go;
      KS2917_WHEN_RDY(pattern = ks2917->dlr + (ks2917->dhr<<16););
      ks2917_wait_done;

      ks2917->cmar = 0;
      ks2917->cmr = ks2917_ws24 + (ii<<8); /* read LAM mask */
      ks2917->cmr = ((30<<9) |(13<<5)| 1); /* N=30 A=12 F=1 */
      ks2917->cmr = ks2917_halt;
      ks2917->cmar = 0;
      ks2917->csr =  ks2917_go;
      KS2917_WHEN_RDY(pattern &= (ks2917->dlr + (ks2917->dhr<<16)););
      ks2917_wait_done;
      for(jj=0; jj<25; jj++) /* loop over slots */
      {
        if((pattern&(1<<jj))&&(lamisrs[b][jj][ii]!=0))
        {
          (* lamisrs[b][jj][ii])();
          taskResume(taskidlam[b][jj][ii]);
        }
      }
    }
  }
}

void
cclnk(int lam, VOIDFUNCPTR label)
{
  int crate, slot;

int ext = lam;
SET_BOARD_ADDRESS;

  ks2917->cmar = *((short *)&(lam));
  crate = (ks2917->cmr>>8) & 7;
  slot = ((lam>>9) & 31) - 1;
  lamisrs[b][slot][crate] = label;
  taskidlam[b][slot][crate] = taskIdSelf();
}

void
cculk(int lam, VOIDFUNCPTR label)
{
  int crate, slot;

int ext = lam;
SET_BOARD_ADDRESS;

  ks2917->cmar = *((short *)&(lam));
  crate = (ks2917->cmr>>8) & 7;
  slot = ((lam>>9) & 31) - 1;
  lamisrs[b][slot][crate] = 0;
}
  
void
ccrgl(int lam)
{

int ext = lam;
SET_BOARD_ADDRESS;

  ks2917->icr_lam = 0x1a;
}

void
cclwt(int lam)
{
  taskSuspend(0);
}

void
clpoll(int lam)
{
  int ll;

  for(ll=0; !ll; ) ctlm(lam,&ll);
}






/*********************** Multiple Action Routines ***************************/
/*********** obsoleted: cfga(), cfmad(), cfubc(), cfubr(), cfubl() **********/
/*********** obsoleted: csga(), csmad(), csubc(), csubr(), csubl() **********/
/****************************************************************************/


/********************** Diagnostics *************************************/

void
camac2917scan()
{
  int status1, status2, b;
  unsigned short data;
  unsigned long ppc_offset;
  volatile CAMAC2917 *ks2917;

  /* get VME address offset */
  sysBusToLocalAdrs(0x29,0,&ppc_offset);

  /* scan over all branches */
  printf("\n\n----------------------------------------------------------\n");
  printf("--- looking for  KS2917 interface boards -------------------\n\n");
  for(b=0; b<NBRANCH; b++)
  {
    ks2917 = (CAMAC2917 *) (ppc_offset + KS2917ADR[b]);
    status1 = vxMemProbe((char *)&ks2917->cser,READ,2,(char *)&data);
    status2 = vxMemProbe((char *)&ks2917->csr,READ,2,(char *)&data);
    if(status1 == OK && status2 == OK)
    {
      printf("---> branch %1d found\n",b);
      ccinit(b);

      printf("   cser: %8x",data);
      if (data & ks2917_cser_coc) printf(" COC");
      if (data & ks2917_cser_ndt) printf(" NDT");
      if (data & ks2917_cser_err) printf(" ERR");
      if (data & ks2917_cser_act) printf(" ACT");
      if ((data & ks2917_cser_code)==ks2917_cser_berr) printf(" BERR");
      if ((data & ks2917_cser_code)==ks2917_cser_abrt) printf(" SOFT_ABRT");
      printf("\n");

      printf("    csr: %8x",data);
      if (data & ks2917_go) printf(" GO");
      if (data & ks2917_write) printf(" WRITE");
      if (data & ks2917_dma) printf(" DMA");
      if (data & ks2917_done) printf(" DONE");
      if (data & ks2917_rdy) printf(" RDY");
      if (data & ks2917_timeo) printf(" TIMEO");
      if (data & ks2917_error) printf(" ERROR");
      printf("\n");

      printf("    mtc: %8x\n",ks2917->mtc);
      printf("  machi: %8x\n",ks2917->machi);
      printf("  maclo: %8x\n",ks2917->maclo);
      printf("    cwc: %8x\n",ks2917->cwc);
      printf("   cmar: %8x\n",ks2917->cmar);
    }
    else
    {
      printf("---> branch %1d does not installed\n",b);
      continue;
    }
  }
}



#else

void
camac2917_dummy()
{
  return;
}

#endif


