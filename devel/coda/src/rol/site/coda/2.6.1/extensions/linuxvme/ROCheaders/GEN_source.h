/******************************************************************************
*
* header file for use General USER defined rols with CODA crl (version 2.0)
*
*                             DJA   Nov 2000
*
* SVN: $Rev: 396 $
*
*******************************************************************************/
#ifndef __GEN_ROL__
#define __GEN_ROL__

#include "jvme.h"

static int GEN_handlers,GENflag;
static int GEN_isAsync;
static unsigned int *GENPollAddr = NULL;
static unsigned int GENPollMask;
static unsigned int GENPollValue;
static unsigned long GEN_prescale = 1;
static unsigned long GEN_count = 0;


/* Put any global user defined variables needed here for GEN readout */
extern DMA_MEM_ID vmeOUT, vmeIN;

/*----------------------------------------------------------------------------
  gen_trigLib.c -- Dummy trigger routines for GENERAL USER based ROLs

 File : gen_trigLib.h

 Routines:
	   void gentenable();        enable trigger
	   void gentdisable();       disable trigger
	   char genttype();          return trigger type 
	   int  genttest();          test for trigger  (POLL Routine)
------------------------------------------------------------------------------*/

static void 
gentenable(int val)
{
  GENflag = 1;
}

static void 
gentdisable(int val)
{
  GENflag = 0;
}

static unsigned long 
genttype()
{
  return(1);
}

static int 
genttest()
{
  if(dmaPEmpty(vmeOUT)) 
    {
      return (0);
    } 
  else
    {
      return (1);
    }
}


/* Define CODA readout list specific Macro routines/definitions */

#define GEN_TEST  genttest

#define GEN_INIT { GEN_handlers =0;GEN_isAsync = 0;GENflag = 0;}

#define GEN_ASYNC(code,id)  {printf("*** No Async mode is available for GEN ***\n"); \
                              printf("linking sync GEN trigger to id %d \n",id); \
			       GEN_handlers = (id);GEN_isAsync = 0;}

#define GEN_SYNC(code,id)   {printf("linking sync GEN trigger to id %d \n",id); \
			       GEN_handlers = (id);GEN_isAsync = 1;}

#define GEN_SETA(code) GENflag = code;

#define GEN_SETS(code) GENflag = code;

#define GEN_ENA(code,val) gentenable(val);

#define GEN_DIS(code,val) gentdisable(val);

#define GEN_CLRS(code) GENflag = 0;

#define GEN_GETID(code) GEN_handlers

#define GEN_TTYPE genttype

#define GEN_START(val)	 {;}

#define GEN_STOP(val)	 {gentdisable(val);}

#define GEN_ENCODE(code) (code)


#endif

