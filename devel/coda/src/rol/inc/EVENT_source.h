/******************************************************************************
*
* header file for use with secondary rols with CODA (version 2.0) CRL
*
*                             DJA   April
*
*******************************************************************************/
#ifndef __EVENT_ROL__
#define __EVENT_ROL__

/* Definition of a EVENT trigger source */
static int EVENT_handlers, EVENTflag;
static int EVENT_isAsync;

static unsigned long *INPUT;

static void 
eventtenable(int val)
{
}

static void 
eventtdisable(int val)
{
}

static unsigned long 
eventttype()
{
  return(1);
}

static int 
eventttest()
{
  int tval=0;
  /*SERGEY if ((rol->input->list.c) > 0)*/ tval = 1;
  /*
printf("eventttest reached, tval=%d\n",tval);
  */
  return(tval);
}

#define EVENT_GET \
  if(input_event__) \
  { \
	EVENT_LENGTH = *rol->dabufpi++ - 1; \
    EVTYPE  = (((*rol->dabufpi)&0xff0000)>>16); \
	if(__the_event__) __the_event__->nevent = input_event__->nevent; \
    rol->dabufpi++; \
	INPUT = rol->dabufpi; \
  }

#define EVENT_TEST eventttest

#define EVENT_INIT {EVENT_handlers = 0;EVENT_isAsync = 0;EVENTflag = 0;}

#define EVENT_ASYNC(code)  {printf("No Async mode is available for EVENT\n"); \
                            EVENT_handlers=1; EVENT_isAsync = 0;}

#define EVENT_SYNC(code)   {EVENT_handlers=1; EVENT_isAsync = 0;}

#define EVENT_SETA(code) EVENTflag = code;

#define EVENT_SETS(code) EVENTflag = code;

#define EVENT_ENA(code,val) eventtenable(val);

#define EVENT_DIS(code,val) eventtdisable(val);

#define EVENT_CLRS(code) EVENTflag = 0;

#define EVENT_TTYPE eventttype

#define EVENT_START(val)   {;}

#define EVENT_STOP(val)	 {eventtdisable(val);}

#define EVENT_ENCODE(code) (code)


#endif

