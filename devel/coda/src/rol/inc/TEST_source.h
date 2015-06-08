/******************************************************************************
*
* header file for use with UNIX rols with CODA crl (version 2.0)
*
*                             DJA   March 1996
*
*******************************************************************************/
#ifndef __TEST_ROL__
#define __TEST_ROL__

static int TEST_handlers,TESTflag;
static int TEST_isAsync;
static unsigned long TEST_prescale = 1;
static unsigned long TEST_count = 0;

struct vme_ts {
    unsigned long csr;       
    unsigned long trig;
    unsigned long roc;
    unsigned long sync;
    unsigned long test;
    unsigned long state;
    unsigned long blank_1;           /* no register */
    unsigned long blank_2;           /* no register */
    unsigned long prescale[8];
    unsigned long timer[5];
    unsigned long blank_3;           /* no register */
    unsigned long blank_4;           /* no register */
    unsigned long blank_5;           /* no register */
    unsigned long sc_as;
    unsigned long scale_0a;
    unsigned long scale_1a;
    unsigned long blank_6;           /* no register */
    unsigned long blank_7;           /* no register */
    unsigned long scale_0b;
    unsigned long scale_1b;
  };

struct vme_tir {
    unsigned short tir_csr;
    unsigned short tir_vec;
    unsigned short tir_dat;
    unsigned short tir_oport;
    unsigned short tir_iport;
  };


struct vme_ts  *ts;
struct vme_tir *tir[2];

long *tsmem;
long ts_memory[4096];
long preset_count;

long *vme2_ir;


/*----------------------------------------------------------------------------*/
 /* test_trigLib.c -- Dummy trigger routines for UNIX based ROLs

 File : test_trigLib.h

 Routines:
	   void testtenable();        enable trigger
	   void testtdisable();       disable trigger
	   char testttype();          read trigger type
	   int  testttest();          test for trigger (POLL)
------------------------------------------------------------------------------*/


static void 
testtenable(int code, int val)
{
  TESTflag = 1;
}

static void 
testtdisable(int code, int val)
{
  TESTflag = 0;
}

static unsigned long 
testttype(int code)
{
  return(1);
}

static int 
testttest(int code)
{

  TEST_count++;

  if(TESTflag && ((TEST_count%TEST_prescale) == 0))
    return(1);
  else
    return(0);
}


/* define CODA readout list specific routines/definitions */


#define TEST_TEST  testttest

#define TEST_INIT { TEST_handlers =0;TEST_isAsync = 0;TESTflag = 0;}

#define TEST_ASYNC(code)  {printf("No Async mode is available for TEST\n"); \
                           TEST_handlers=1; TEST_isAsync = 0;}

#define TEST_SYNC(code)   {TEST_handlers=1; TEST_isAsync = 0;}

#define TEST_SETA(code) TESTflag = code;

#define TEST_SETS(code) TESTflag = code;

#define TEST_ENA(code,val) testtenable(code,val);

#define TEST_DIS(code,val) testtdisable(code,val);

#define TEST_CLRS(code) TESTflag = 0;

#define TEST_TTYPE testttype

#define TEST_START(val)	 {;}

#define TEST_STOP(val)	 {;}

#define TEST_ENCODE(code) (code)


#endif

