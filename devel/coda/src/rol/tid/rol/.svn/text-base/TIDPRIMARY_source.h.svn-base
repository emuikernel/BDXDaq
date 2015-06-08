/******************************************************************************
*
* header file for use Linux VME defined rols with CODA
*
*                             DJA   Nov 2000
*
* SVN: $Rev: 396 $
*
*******************************************************************************/
#ifndef __TIDPRIMARY_ROL__
#define __TIDPRIMARY_ROL__

static int TIDPRIMARY_handlers,TIDPRIMARYflag;
static int TIDPRIMARY_isAsync;
static unsigned int *TIDPRIMARYPollAddr = NULL;
static unsigned int TIDPRIMARYPollMask;
static unsigned int TIDPRIMARYPollValue;
static unsigned long TIDPRIMARY_prescale = 1;
static unsigned long TIDPRIMARY_count = 0;


/* Put any global user defined variables needed here for TIDPRIMARY readout */
extern DMA_MEM_ID vmeOUT, vmeIN;

/*----------------------------------------------------------------------------
  tidprimary_trigLib.c -- Dummy trigger routines for GENERAL USER based ROLs

 File : tidprimary_trigLib.h

 Routines:
	   void tidprimarytenable();        enable trigger
	   void tidprimarytdisable();       disable trigger
	   char tidprimaryttype();          return trigger type 
	   int  tidprimaryttest();          test for trigger  (POLL Routine)
------------------------------------------------------------------------------*/

static void 
tidprimarytenable(int val)
{
  TIDPRIMARYflag = 1;
}

static void 
tidprimarytdisable(int val)
{
  TIDPRIMARYflag = 0;
}

static unsigned long 
tidprimaryttype()
{
  return(1);
}

static int 
tidprimaryttest()
{
  if(dmaPEmpty(vmeOUT)) 
    return (0);
  else
    return (1);
}



/* Define CODA readout list specific Macro routines/definitions */

#define TIDPRIMARY_TEST  tidprimaryttest

#define TIDPRIMARY_INIT { TIDPRIMARY_handlers =0;TIDPRIMARY_isAsync = 0;TIDPRIMARYflag = 0;}

#define TIDPRIMARY_ASYNC(code,id)  {printf("*** No Async mode is available for TIDPRIMARY ***\n"); \
                              printf("linking sync TIDPRIMARY trigger to id %d \n",id); \
			       TIDPRIMARY_handlers = (id);TIDPRIMARY_isAsync = 0;}

#define TIDPRIMARY_SYNC(code,id)   {printf("linking sync TIDPRIMARY trigger to id %d \n",id); \
			       TIDPRIMARY_handlers = (id);TIDPRIMARY_isAsync = 1;}

#define TIDPRIMARY_SETA(code) TIDPRIMARYflag = code;

#define TIDPRIMARY_SETS(code) TIDPRIMARYflag = code;

#define TIDPRIMARY_ENA(code,val) tidprimarytenable(val);

#define TIDPRIMARY_DIS(code,val) tidprimarytdisable(val);

#define TIDPRIMARY_CLRS(code) TIDPRIMARYflag = 0;

#define TIDPRIMARY_GETID(code) TIDPRIMARY_handlers

#define TIDPRIMARY_TTYPE tidprimaryttype

#define TIDPRIMARY_START(val)	 {;}

#define TIDPRIMARY_STOP(val)	 {tidprimarytdisable(val);}

#define TIDPRIMARY_ENCODE(code) (code)


#endif

