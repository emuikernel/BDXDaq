
/* GE FANUC V7865 VME controller support */

#ifndef __FANUC_ROL__
#define __FANUC_ROL__


#include "../jvme/jlabgef.h"
#include "../jvme/jvme.h"

#include "../tirLib/tirLib.h"

volatile struct vme_tir *tir;
volatile struct vme_ts  *ts;

/* Define Hardware sources */
#define TIR_SOURCE 1
#define TS_SOURCE  2


#define TS_LEVEL 3 /*do we have it in one of the TS registers ???*/


static int VME_handlers;
static int VME_isAsync;
static int VMEflag;
#ifdef VXWORKS

void
VME_int_handler()
{
  /*logMsg("VME_int_handler reached\n",1,2,3,4,5,6);*/
  theIntHandler(VME_handlers);                   /* Call our handler */
}

#else

void
VME_int_handler(int arg)
{
  theIntHandler(VME_handlers);                   /* Call our handler */
}

#endif


static void
vmetinit(int code)
{
  switch(code)
  {
    case TIR_SOURCE:
      /* Open the default VME windows */
      vmeOpenDefaultWindows();
      /* Disable IRQ for VME Bus Errors */
      vmeDisableBERRIrq();
      /* Initialize VME Interrupt interface */
#ifdef POLLING_MODE
#ifdef TRIG_SUPV
      tirIntInit(TIR_ADDR, TIR_TS_POLL, 1);
#else
      tirIntInit(TIR_ADDR, TIR_EXT_POLL, 1);
#endif
#else
#ifdef TRIG_SUPV
      tirIntInit(TIR_ADDR, TIR_TS_INT, 1);
#else
      tirIntInit(TIR_ADDR, TIR_EXT_INT, 1);
#endif
#endif
      break;

    case TS_SOURCE:
      break;

    default:
      printf(" vmetriglink: ERROR: source type %d undefined\n",code);
      return;
  }
}

/* called at prestart (CTRIGRSA) */
static void
vmetriglink(int code, VOIDFUNCPTR isr)
{

  switch(code)
  {
    case TIR_SOURCE:
      /* Initialize VME Interrupt variables */
      tirClearIntCount();
      /* Connect User Trigger Routine */
      tirIntConnect(TIR_INT_VEC, isr, 0);
      break;

    case TS_SOURCE:
      if(ts)
      {
        ts->intVec = TIR_INT_VEC;
      }
      else
      {
        printf(" vmetriglink: ERROR: ts uninitialized\n");
        return;
      }

#ifdef VXWORKSPPC
      if((intDisconnect(INUM_TO_IVEC(TIR_INT_VEC)) !=0))
      {
        printf("Error disconnecting Interrupt\n");
      }
      intConnect(INUM_TO_IVEC(TIR_INT_VEC),isr,0);
#endif

      break;

    default:
      printf(" vmetriglink: ERROR: source type %d undefined\n",code);
      return;
  }

  printf("vmetriglink: register int handler 0x%08x\n",isr);fflush(stdout);
}

static void 
vmetenable(int code, unsigned int intMask)
{
  int lock_key, tirIntLevel;

  if(code == TIR_SOURCE)
  {
    tirIntEnable(TIR_CLEAR_COUNT); 
  }



  if(code == TS_SOURCE)
  {

/* interrupt mode */
#ifndef POLLING_MODE
  lock_key = intLock();
  sysIntEnable(TS_LEVEL);
#ifdef VXWORKSPPC
  intEnable(11);         /* Enable VME Interrupts on IBC chip (Radstone PPC604 only) */
#endif

#ifdef TRIG_SUPV
  /*Set TIR to accept Trigger Supervisor triggers*/
  ts->csr2 |= 0x1800;
#else
  /*Set TIR for External triggers*/
  ts->csr2 |= 0x0000;
#endif
  intUnlock(lock_key);


/* polling mode */
#else

#ifdef TRIG_SUPV
  /*Set TIR for Trigger Supervisor triggers*/
  ts->csr2 |= 0x1000;
#else
  /*Set TIR for External triggers*/
  ts->csr2 |= 0x0000;
#endif

#endif

  }


}

static void 
vmetdisable(int code, unsigned int intMask)
{
  if(code == TIR_SOURCE)
  {
    /*tirWrite(&tir->tir_csr,0x80);*/

    tirIntStatus(1);

    /* Disable Interrupts */
    tirIntDisable();
    tirIntDisconnect();

    tirIntReset();/* ??? bryan has no that */
  }

  if(code == TS_SOURCE)
  {
    ts->csr2 &= ~(0x1800);
  }
}

static void 
vmetack(int code, unsigned int intMask)
{
  if(code == TIR_SOURCE)
  {
    tirIntAck();
  }

  if(code == TS_SOURCE)
  {
    ts->lrocBufStatus = 0x100;
  }
}


/* read trigger type from TI boards or from TS */

static unsigned int 
vmettype(unsigned int code)
{
  unsigned int tt;
  unsigned short sval=0;

#ifdef TRIG_SUPV

  if(code == TIR_SOURCE)
  {
	/*
    tt = ((tirRead(&tir->tir_dat)&0x3c)>>2);
    syncFlag = tirRead(&tir->tir_dat)&0x01;
    lateFail = (tirRead(&tir->tir_dat)&0x02)>>1;
	*/
    tt = tirIntType(&syncFlag, &lateFail);
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


/* for polling mode only */
static int 
vmettest(unsigned int code)
{
  unsigned short sval=0;
  unsigned int   lval=0;
  int val=0;

  /*printf("vmettest reached, code=%d\n",code);*/

  if(code == TIR_SOURCE)
  {
    val = tirIntPoll();
    if(val)
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

/* define CODA readout list specific routines/definitions */

#define FANUC_TEST  vmettest

#define FANUC_INIT(code) {VME_handlers=0; VME_isAsync=0; VMEflag=0; vmetinit(code);}

#define FANUC_ASYNC(code)  {VME_handlers=1; VME_isAsync=1; vmetriglink(code,VME_int_handler);}

#define FANUC_SYNC(code)   {VME_handlers=1; VME_isAsync=0;}

#define FANUC_SETA(code) VMEflag = code;

#define FANUC_SETS(code) VMEflag = code;

#define FANUC_ENA(code,val)   vmetenable(code,val);

#define FANUC_DIS(code,val)   vmetdisable(code,val);

#define FANUC_ACK(code,val)   vmetack(code,val);

#define FANUC_CLRS(code) VMEflag = 0;

#define FANUC_TTYPE vmettype

#define FANUC_START(val)	 {;}

#define FANUC_STOP(val)	 {;}

#define FANUC_ENCODE(code) (code)

#endif
