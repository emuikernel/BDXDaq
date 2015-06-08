/* remove following after including into main library file */

/*REMOVE*/

#ifdef VXWORKS
#include <vxWorks.h>
#include <vxLib.h>
#include <logLib.h>
#else
#include "jvme/jvme.h"
#endif

#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "vmeDSClib1.h"

/* Internal (static) Register Read/Write prototypes */
static unsigned int dscRead(volatile unsigned int *addr);
static void dscWrite(volatile unsigned int *addr, unsigned int val);

/* Global variables */
int Ndsc = 0;                                     /* Number of DSCs in Crate */
volatile struct dsc_struct *dscp[DSC_MAX_BOARDS]; /* pointers to DSC memory map */
int dscID[DSC_MAX_BOARDS];                        /* array of slot numbers for DSCs */
unsigned int dscMemOffset = 0;       /* Offset between VME and Local address space */

/* Macro to check for module initialization */
#define CHECKID(id) {							\
    if(id==0) id=dscID[0];						\
    if((id<0) || (id>21) || (dscp[id] == NULL))				\
      {									\
	logMsg("%s: ERROR: DSC in slot %d is not initialized \n",	\
	       __FUNCTION__,id,3,4,5,6);			\
	return ERROR;							\
      }									\
  }

/* Mutex to guard flexio read/writes */
pthread_mutex_t   dscMutex = PTHREAD_MUTEX_INITIALIZER;
#define DSCLOCK     pthread_mutex_lock(&dscMutex);
#define DSCUNLOCK   pthread_mutex_unlock(&dscMutex);

/*REMOVE*/

/* variables filled by DmaStart and used by DmaDone */
static int ndata_save[21];

/*******************************************************************
 *   Function : dsc2ReadBoardDmaStart
 *                      
 *   Function : issues software trigger and Dma data from one scaler
 *              board in according to rflag
 *                                                    
 *   Parameters :  UINT32 id    - Module slot number
 *                 UINT32 *data - local memory address to place data
 *                 UINT32 nwrds - Max number of words to transfer
 *                 UINT32 rflag - Readout flag
 *                                 bit 0 - TRG scalers
 *                                 bit 1 - TDC scalers
 *                                 bit 2 - TRG VME scalers
 *                                 bit 3 - TDC VME scalers
 *                                 bit 4 - Reference Scaler
 *                                 bit 5 - Reference VME scaler
 *                                 bit 6 - Latch VME before read
 *                                 bit 7 - Latch GATED before read
 *                                                    
 *   Returns -1 if Error, Number of words transferred if OK.
 *                                                    
 *******************************************************************/

int
dsc2ReadBoardDmaStart(UINT32 id, volatile UINT32 *data, int nwrds, int rflag)
{
  int dCnt=0, res;
  int softTrigger = rflag & 0xFF;
  volatile UINT32 *vmeAdr;

  CHECKID(id);

  if(data==NULL) 
  {
    logMsg("%s: ERROR: Invalid Destination address\n",__FUNCTION__,0,0,0,0,0);
    return(ERROR);
  }
  
  /* calculate the length of event */

  dCnt ++; /* header word always there */
  if(softTrigger && DSC_READOUT_TRG)    dCnt+=16;
  if(softTrigger && DSC_READOUT_TDC)    dCnt+=16;
  if(softTrigger && DSC_READOUT_TRGVME) dCnt+=16;
  if(softTrigger && DSC_READOUT_TDCVME) dCnt+=16;
  if(softTrigger && DSC_READOUT_REF)    dCnt++;
  if(softTrigger && DSC_READOUT_REFVME) dCnt++;
  if(dCnt%2) dCnt++; /* if not aligned to 64bit, board will send us extra filler */

  /*128-bit alignment for 2eSST: must be done in VME FPGA (ask Ed) */
  /* if 2eSST (or 2eVME ?) dCnt += ... */

  if(dCnt > nwrds)
  {
    logMsg("%s: ERROR: buffer too small (%d < %d)\n",__FUNCTION__,nwrds,dCnt,0,0,0);
    return(ERROR);
  }

  ndata_save[id] = dCnt;

  DSCLOCK;

  /* start moving event to FIFO */
  vmeWrite32(&dscp[id]->softTrigger,softTrigger);

  /* do not need to check if event is ready, because it takes less then 2us to form event */

  /* fifo address: use highest byte from board switches */
  vmeAdr = ((unsigned int)(dscp[id]->vmeadr))&0xFF000000;

  /* start Dma */
  res = usrVme2MemDmaStart( (UINT32 *)vmeAdr, (UINT32 *)data, (dCnt<<2));
  if(res < 0)
  {
    logMsg("v1720ReadEventDmaStart: ERROR: usrVme2MemDmaStart returned %d\n",
           res,0,0,0,0,0);
  }

  DSCUNLOCK;

  return(dCnt);
}


int
dsc2ReadBoardDmaDone(int id)
{
  if((res = usrVme2MemDmaDone()) < 0)
  {
    logMsg("%s: ERROR: usrVme2MemDmaDone returned = %d\n",__FUNCTION__,
             res,3,4,5,6);
    return(0);
  }
  else
  {
    mdata = ndata_save[id] - (res>>2);
	  /*
logMsg("%s: res=%d ndata_save=%d -> mdata=%d\n",__FUNCTION__,res,ndata_save[id],mdata,5,6);
	  */

    if( (res>4) || ((mdata%2)!=0) )
    {
      logMsg("%s: WRONG: res=%d (ndata_save=%d => was %d), mdata=%d\n",__FUNCTION__,
          res,ndata_save[id],ndata_save[id]<<2,mdata,6);
    }

	/*
    logMsg("%s: INFO: usrVme2MemDmaDone returned = %d (%d)\n",__FUNCTION__,res,mdata,4,5,6);
	*/
  }

  return(mdata);
}
