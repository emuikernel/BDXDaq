/******************************************************************************
*
* DUMMY_source.h - header file for use with DUMMY ROL2 (see roc_component.c)
*
*******************************************************************************/
#ifndef __DUMMY_ROL__
#define __DUMMY_ROL__

/* Definition of a DUMMY trigger source */
static int DUMMY_handlers,DUMMYflag;
static int DUMMY_isAsync;

static unsigned long *INPUT;

static void 
dummytenable(int val)
{
}

static void 
dummytdisable(int val)
{
}

static unsigned long 
dummyttype()
{
  return(1);
}

static int 
dummyttest()
{
  if ((rol->input->list.c) > 0) return(1);
  return(0);
}

#define DUMMY_GET \
  if(input_event__) \
  { \
	EVENT_LENGTH = *rol->dabufpi++ - 1; \
    EVTYPE  = (((*rol->dabufpi)&0xff0000)>>16); \
	if(__the_event__) __the_event__->nevent = input_event__->nevent; \
    rol->dabufpi++; \
	INPUT = rol->dabufpi; \
  }

#define DUMMY_TEST dummyttest

#define DUMMY_INIT {DUMMY_handlers = 0;DUMMY_isAsync = 0;DUMMYflag = 0;}

#define DUMMY_ASYNC(code)  {printf("No Async mode is available for DUMMY\n"); \
                            DUMMY_handlers=1; DUMMY_isAsync = 0;}

#define DUMMY_SYNC(code)   {DUMMY_handlers=1; DUMMY_isAsync = 0;}

#define DUMMY_SETA(code) DUMMYflag = code;

#define DUMMY_SETS(code) DUMMYflag = code;

#define DUMMY_ENA(code,val) dummytenable(val);

#define DUMMY_DIS(code,val) dummytdisable(val);

#define DUMMY_CLRS(code) DUMMYflag = 0;

#define DUMMY_TTYPE dummyttype

#define DUMMY_START(val)   {;}

#define DUMMY_STOP(val)	 {dummytdisable(val);}

#define DUMMY_ENCODE(code) (code)


#endif

