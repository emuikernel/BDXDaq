/******************************************************************************
*
* header file for use Linux VME defined rols with CODA
*
*                             DJA   Nov 2000
*
* SVN: $Rev: 396 $
*
*******************************************************************************/
#ifndef __LINUXVME_ROL__
#define __LINUXVME_ROL__

static int LINUXVME_handlers,LINUXVMEflag;
static int LINUXVME_isAsync;
static unsigned int *LINUXVMEPollAddr = NULL;
static unsigned int LINUXVMEPollMask;
static unsigned int LINUXVMEPollValue;
static unsigned long LINUXVME_prescale = 1;
static unsigned long LINUXVME_count = 0;


/* Put any global user defined variables needed here for LINUXVME readout */
extern DMA_MEM_ID vmeOUT, vmeIN;

/*----------------------------------------------------------------------------
  linuxvme_trigLib.c -- Dummy trigger routines for GENERAL USER based ROLs

 File : linuxvme_trigLib.h

 Routines:
	   void linuxvmetenable();        enable trigger
	   void linuxvmetdisable();       disable trigger
	   char linuxvmettype();          return trigger type 
	   int  linuxvmettest();          test for trigger  (POLL Routine)
------------------------------------------------------------------------------*/

static void 
linuxvmetenable(int val)
{
  LINUXVMEflag = 1;
}

static void 
linuxvmetdisable(int val)
{
  LINUXVMEflag = 0;
}

static unsigned long 
linuxvmettype()
{
  return(1);
}

static int 
linuxvmettest()
{
  if(dmaPEmpty(vmeOUT)) 
    return (0);
  else
    return (1);
}



/* Define CODA readout list specific Macro routines/definitions */

#define LINUXVME_TEST  linuxvmettest

#define LINUXVME_INIT { LINUXVME_handlers =0;LINUXVME_isAsync = 0;LINUXVMEflag = 0;}

#define LINUXVME_ASYNC(code,id)  {printf("*** No Async mode is available for LINUXVME ***\n"); \
                              printf("linking sync LINUXVME trigger to id %d \n",id); \
			       LINUXVME_handlers = (id);LINUXVME_isAsync = 0;}

#define LINUXVME_SYNC(code,id)   {printf("linking sync LINUXVME trigger to id %d \n",id); \
			       LINUXVME_handlers = (id);LINUXVME_isAsync = 1;}

#define LINUXVME_SETA(code) LINUXVMEflag = code;

#define LINUXVME_SETS(code) LINUXVMEflag = code;

#define LINUXVME_ENA(code,val) linuxvmetenable(val);

#define LINUXVME_DIS(code,val) linuxvmetdisable(val);

#define LINUXVME_CLRS(code) LINUXVMEflag = 0;

#define LINUXVME_GETID(code) LINUXVME_handlers

#define LINUXVME_TTYPE linuxvmettype

#define LINUXVME_START(val)	 {;}

#define LINUXVME_STOP(val)	 {linuxvmetdisable(val);}

#define LINUXVME_ENCODE(code) (code)


#endif

