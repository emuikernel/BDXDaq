/******************************************************************************
*
* header file for use with General VME based  rols with CODA crl (version 2.0)
*   The trigger source for polling or interrupts is the TJNAF VME Trigger
*   Supervisor Register.
*
*                             DJA   July 1996
*
*******************************************************************************/
#ifndef __VME_ROL__
#define __VME_ROL__

static int VME_handlers,VMEflag;
static int VME_isAsync;
static unsigned long VME_prescale = 1;
static unsigned long VME_count = 0;

/* define various structures for use in accessing commonly
   used VME modules like the Trigger Supervisor, VME trigger interface
   Lecroy scalers etc...  */

#define VME_STD_SUP_ASC    	0x3f
#define VME_STD_SUP_PGM		0x3e
#define VME_STD_SUP_DAT		0x3d

#define VME_STD_USR_ASC	        0x3b
#define VME_STD_USR_PGM		0x3a
#define VME_STD_USR_DAT		0x39

#define VME_SHR_SUP_IO		0x2d
#define VME_SHR_USR_IO		0x29

#define VME_EXT_SUP_ASC	        0x0f
#define VME_EXT_SUP_PGM		0x0e
#define VME_EXT_SUP_DAT 	0x0d

#define VME_EXT_USR_ASC    	0x0b
#define VME_EXT_USR_PGM		0x0a
#define VME_EXT_USR_DAT		0x09

extern int sysBusToLocalAdrs(int space,long *localA,long **busA);

/*
struct vme_ts {
    unsigned long csr;       
    unsigned long trig;
    unsigned long roc;
    unsigned long sync;
    unsigned long test;
    unsigned long state;
    unsigned long blank_1;           
    unsigned long blank_2;           
    unsigned long prescale[8];
    unsigned long timer[5];
    unsigned long blank_3;           
    unsigned long blank_4;           
    unsigned long blank_5;           
    unsigned long sc_as;
    unsigned long scale_0a;
    unsigned long scale_1a;
    unsigned long blank_6;           
    unsigned long blank_7;          
    unsigned long scale_0b;
    unsigned long scale_1b;
  };
*/

volatile struct vme_ts {
    volatile unsigned long csr;       
    volatile unsigned long csr2;       
    volatile unsigned long trig;
    volatile unsigned long roc;
    volatile unsigned long sync;
    volatile unsigned long trigCount;
    volatile unsigned long trigData;
    volatile unsigned long lrocData;
    volatile unsigned long prescale[8];
    volatile unsigned long timer[5];
    volatile unsigned long intVec;
    volatile unsigned long rocBufStatus;
    volatile unsigned long lrocBufStatus;
    volatile unsigned long rocAckStatus;
    volatile unsigned long userData1;
    volatile unsigned long userData2;
    volatile unsigned long state;
    volatile unsigned long test;
    volatile unsigned long blank1;
    volatile unsigned long scalAssign;
    volatile unsigned long scalControl;
    volatile unsigned long scaler[18];
    volatile unsigned long scalEvent;
    volatile unsigned long scalLive1;
    volatile unsigned long scalLive2;
    volatile unsigned long id;
  } VME_TS;


struct vme_tir {
    unsigned short tir_csr;
    unsigned short tir_vec;
    unsigned short tir_dat;
    unsigned short tir_oport;
    unsigned short tir_iport;
  };


struct vme_dpm {
    long dpm_mem[32768];
    unsigned short dpm_addr;
    unsigned short dpm_mode;
  };

struct vme_scal {
    unsigned short reset;
    unsigned short blank1[7];
    unsigned short bim[8];
    unsigned short blank2[16];
    unsigned long preset[16];
    unsigned long scaler[16];
    unsigned short blank3[29];
    unsigned short id[3];
  };

volatile struct vme_ts  *ts;
struct vme_tir *tir[2];
struct vme_dpm *dpm;
struct vme_dpm *dpml;
struct vme_scal *vscal[32];
struct vme_scal *vlscal[32];

volatile unsigned long *tsmem;
unsigned long ts_memory[4096];
long *vme2_ir;





/******************************************************************/
/* trigger support routines for TIR module (former vme_triglib.h) */

/* vme_trigLib.c -- Primitive trigger control for VME CPUs using the TJNAF 
                     Trigger Supervisor interface card or Trigger Supervisor

 File : vme_trigLib.h

 Supported Interrupt/Trigger sources:

       code       Hardware type
        1     VME Trigger Supervisor Interface  (TIR)
        2     Trigger Supervisor (Direct via Branch 5)

 Routines:
	   void vmetriglink();       link isr to trigger
	   void vmetenable();        enable trigger
	   void vmetdisable();       disable trigger
       void vmetack();           Acknowledge/Reset trigger
	   char vmettype();          read trigger type
	   int  vmettest();          test for trigger  (POLL)

-----------------------------------------------------------------------------*/

/* Define Hardware sources */
#define TIR_SOURCE 1
#define TS_SOURCE  2

/* Define global variable for TIR board revision */
unsigned int tirVersion = 0; /* Default is an invalid Version ID */

/* Define TIR Version IDs (readback of a reset CSR register */
#define TIR_VERSION_1   0x05c0
#define TIR_VERSION_2   0x0580

/* Define default Interrupt vector and Levels for possible sources */
#define TIR_DEFAULT_ADDR 0x0ed0
#define TS_DEFAULT_ADDR  0xed0000
#define VME_VEC 0xec
#define TIR_LEVEL 5
#define TS_LEVEL 3

#ifdef VXWORKSPPC
#define INUM_TO_IVEC(intNum)    ((VOIDFUNCPTR *) (intNum))
#else
#define INUM_TO_IVEC(intNum)    ((VOIDFUNCPTR *) (intNum<<2))
#endif

int
tirInit(unsigned int tAddr)
{
  int stat = 0;
  unsigned long laddr;
  unsigned short rval;

  if(tAddr == 0)
  {
    tAddr = TIR_DEFAULT_ADDR;
  }

  stat = sysBusToLocalAdrs(0x29, (long *)tAddr, (long **)&laddr);
  if(stat != 0)
  {
    printf("tirInit: ERROR: Error in sysBusToLocalAdrs res=%d \n",stat);
  }
  else
  {
    printf("TIR address = 0x%x\n",laddr);
    tir[1] = (struct vme_tir *)laddr;
  }

  /* Check if TIR board is readable */
  stat = vxMemProbe((char *)laddr,0,2,(char *)&rval);
  if(stat != 0)
  {
    printf("tirInit: ERROR: TIR card not addressable\n");
    return(-1);
  }
  else
  {
    tir[1]->tir_csr = 0x80; /* Reset the board */
    tirVersion = (unsigned int)rval;
    printf("tirInit: tirVersion = 0x%x\n",tirVersion);
  }

  return(0);
}



static void 
vmetriglink(int code, VOIDFUNCPTR isr)
{
  switch(code)
  {
    case TIR_SOURCE:
      if(tir[1])
      {
        tir[1]->tir_csr = 0x80;
        tir[1]->tir_vec = VME_VEC;
      }
      else
      {
        printf(" vmetriglink: ERROR: tir[1] uninitialized\n");
        return;
      }
      break;

    case TS_SOURCE:
      if(ts)
      {
        ts->intVec = VME_VEC;
      }
      else
      {
        printf(" vmetriglink: ERROR: ts uninitialized\n");
        return;
      }
      break;

    default:
      printf(" vmetriglink: ERROR: source type %d undefined\n",code);
      return;
  }


#ifdef VXWORKSPPC
  if((intDisconnect(INUM_TO_IVEC(VME_VEC)) !=0))
  {
    printf("Error disconnecting Interrupt\n");
  }
#endif
  intConnect(INUM_TO_IVEC(VME_VEC),isr,0);

}

static void 
vmetenable(int code, unsigned int intMask)
{
  int lock_key;

#ifndef POLLING_MODE
  lock_key = intLock();

   if(code == TIR_SOURCE)
     sysIntEnable(TIR_LEVEL);
   if(code == TS_SOURCE)
     sysIntEnable(TS_LEVEL);
#ifdef VXWORKSPPC
    intEnable(11);         /* Enable VME Interrupts on IBC chip (Radstone PPC604 only) */
#endif

#ifdef TRIG_SUPV
  /*Set TIR to accept Trigger Supervisor triggers*/
  if(code == TIR_SOURCE)
    tir[1]->tir_csr = 0x6;
  if(code == TS_SOURCE)
    ts->csr2 |= 0x1800;
#else
  /*Set TIR for External triggers*/
  if(code == TIR_SOURCE)
    tir[1]->tir_csr = 0x7;
  if(code == TS_SOURCE)
    ts->csr2 |= 0x0000;
#endif
  intUnlock(lock_key);

#else

#ifdef TRIG_SUPV
  /*Set TIR for Trigger Supervisor triggers*/
  if(code == TIR_SOURCE)
    tir[1]->tir_csr = 0x2;
  if(code == TS_SOURCE)
    ts->csr2 |= 0x1000;
#else
  /*Set TIR for External triggers*/
  if(code == TIR_SOURCE)
    tir[1]->tir_csr = 0x3;
  if(code == TS_SOURCE)
    ts->csr2 |= 0x0000;
#endif

#endif
}

static void 
vmetdisable(int code,unsigned int intMask)
{
  if(code == TIR_SOURCE)
    tir[1]->tir_csr = 0x80;
  if(code == TS_SOURCE)
    ts->csr2 &= ~(0x1800);

}

static void 
vmetack(int code, unsigned int intMask)
{
  if(code == TIR_SOURCE)
    tir[1]->tir_dat = 0x8000;
  if(code == TS_SOURCE)
    ts->lrocBufStatus = 0x100;

}

static unsigned long 
vmettype(int code)
{
  unsigned long tt;
#ifdef TRIG_SUPV

  if(code == TIR_SOURCE)
  {
    tt = (((tir[1]->tir_dat)&0x3c)>>2);
    syncFlag = (tir[1]->tir_dat)&0x01;
    lateFail = ((tir[1]->tir_dat)&0x02)>>1;
  }

  if(code == TS_SOURCE)
  {
    /*Sergey: use 4 bits only for event type !!!
    tt = (((ts->lrocData)&0xfc)>>2);*/
    tt = (((ts->lrocData)&0x3c)>>2);

    syncFlag = (ts->lrocData)&0x01;
    lateFail = ((ts->lrocData)&0x02)>>1;
  }

#else
  /* Set default trigger type to 1 */
  tt=1;
#endif

  return(tt);
}

static int 
vmettest(int code)
{
  unsigned short sval=0;
  unsigned int   lval=0;

  if(code == TIR_SOURCE)
  {
    sval = tir[1]->tir_csr;
    if( (sval != 0xffff) && ((sval&0x8000) != 0) )
    {
      return(1);
    }
    else
    {
      return(0);
    }
  }

  if(code == TS_SOURCE)
  {
    lval = ts->lrocBufStatus;
    if( (lval != 0xffffffff) && ((lval&0xf) != 0) )
    {
      return(1);
    }
    else
    {
      return(0);
    }
  }

  return(0);
}

/******************************************************************/
/******************************************************************/




void VME_int_handler()
{
  theIntHandler(VME_handlers);                   /* Call our handler */
}

/* define CODA readout list specific routines/definitions */

#define VME_TEST  vmettest

#define VME_INIT { VME_handlers =0;VME_isAsync = 0;VMEflag = 0;}

#define VME_ASYNC(code,id)  {printf("Linking async VME trigger to id %d \n",id); \
			       VME_handlers = (id);VME_isAsync = 1;vmetriglink(code,VME_int_handler);}

#define VME_SYNC(code,id)   {printf("Linking sync VME trigger to id %d \n",id); \
			       VME_handlers = (id);VME_isAsync = 0;}

#define VME_SETA(code) VMEflag = code;

#define VME_SETS(code) VMEflag = code;

#define VME_ENA(code,val)   vmetenable(code,val);

#define VME_DIS(code,val)   vmetdisable(code,val);

#define VME_ACK(code,val)   vmetack(code,val);

#define VME_CLRS(code) VMEflag = 0;

#define VME_GETID(code) VME_handlers

#define VME_TTYPE vmettype

#define VME_START(val)	 {;}

#define VME_STOP(val)	 {;}

#define VME_ENCODE(code) (code)


#endif

