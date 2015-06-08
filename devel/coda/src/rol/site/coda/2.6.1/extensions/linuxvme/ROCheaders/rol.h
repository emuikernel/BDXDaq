/*----------------------------------------------------------------------------*
 *  Copyright (c) 1991, 1992  Southeastern Universities Research Association, *
 *                            Continuous Electron Beam Accelerator Facility   *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 * CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606 *
 *      heyes@cebaf.gov   Tel: (804) 249-7030    Fax: (804) 249-7363          *
 *----------------------------------------------------------------------------*
 * Discription: follows this header.
 *
 * Author:
 *	Graham Heyes
 *	CEBAF Data Acquisition Group
 *
 * Revision History:
 *      $Log: rol.h,v $
 *      Revision 1.5  1998/07/15 18:38:35  abbottd
 *       Return status on partCreate calls.
 *
 *      Revision 1.4  1998/01/23 15:27:53  heyes
 *      commit LINUX changes for Carl
 *
 *      Revision 1.3  1997/12/03 15:06:40  heyes
 *      support for multithread
 *
 *      Revision 1.2  1997/05/19 18:51:22  abbottd
 *      various changes and enhancements over several months
 *
 *      Revision 1.1.1.1  1996/08/21 19:20:56  heyes
 *      Imported sources
 *
 *	  Revision 1.2  94/03/16  11:00:33  11:00:33  heyes (Graham Heyes)
 *	  auto replace
 *	  
 *	  Revision 1.1  94/03/16  10:01:10  10:01:10  heyes (Graham Heyes)
 *	  Initial revision
 *	  
 *
 * SVN: $Rev: 396 $
 *
 *----------------------------------------------------------------------------*/
#ifndef _ROLDEFINED
#define _ROLDEFINED


typedef void (*VOIDFUNCPTR) ();
typedef int (*FUNCPTR) ();

#ifdef VXWORKS
typedef unsigned long time_t;
#endif

typedef struct semaphore *SEM_ID;


static void __download ();
static void __prestart ();
static void __end ();
static void __pause ();
static void __go ();
static void __done ();
static void __status ();

static int theIntHandler ();

/* Include the memory partition structures and routines */
#include "libpart.h"

/* The following structure is used per readout list to store it's global data */
#include "rolInt.h"     

/* Define external variables and internal rol structures */
extern ROLPARAMS rolP;
static rolParam rol;

/* Identify external variables from roc_component and VxWorks kernel */

extern int global_env[];
extern long global_env_depth;
extern char *global_routine[100];

extern long data_tx_mode;

#ifdef VXWORKSPPC
extern int cacheInvalidate();
extern int cacheFlush();
#endif

static int syncFlag;
static int lateFail;

#include "BankTools.h"

/* Disable Readout List error Recovery for Now */
/*#define RECOVER {if (global_env_depth>=0) global_env_depth--; \
  longjmp(global_env[global_env_depth+1],global_code);} */
#define RECOVER  return

#include "trigger_dispatch.h"

/* Maximum Event Length in bytes */
#ifndef MAX_EVENT_LENGTH
#define MAX_EVENT_LENGTH 2048
#endif

/* Maximum number of buffers in the Pool */
#ifndef MAX_EVENT_POOL
#define MAX_EVENT_POOL 512
#endif


/* Define Default ROL Name */
#ifndef ROL_NAME__
#define ROL_NAME__ "Un-named"
#endif

static char rol_name__[40];
static char temp_string__[132];


static void __poll()
{
    CDOPOLL;
}

void INIT_NAME(rolp)
     rolParam rolp;
{
      if ((rolp->daproc != DA_DONE_PROC)&&(rolp->daproc != DA_POLL_PROC)) 
	printf("rolp->daproc = %d\n",rolp->daproc);

      switch(rolp->daproc) {
      case DA_INIT_PROC:
	{
	  char name[40];
	  rol = rolp;
	  rolp->inited = 1;
	  strcpy(rol_name__, ROL_NAME__);
	  rolp->listName = rol_name__;

	  printf("Init - Initializing new rol structures for %s\n",rol_name__);

	  strcpy(name, rolp->listName);
	  strcat(name, ":pool");
	  rolp->pool  = partCreate(name, MAX_EVENT_LENGTH , MAX_EVENT_POOL,1);
	  if (rolp->pool == 0) {
	    rolp->inited = -1;
	    break;
	  }

	  strcpy(name, rolp->listName);
	  strcat(name, ":dispatch");
	  rolp->dispatch  = partCreate(name, 0, 32, 0);
	  if (rolp->dispatch == 0) {
	    rolp->inited = -1;
	    break;
	  }

	  strcpy(name, rolp->listName);
	  strcat(name, ":dispQ");
	  rolp->dispQ = partCreate(name, 0, 0, 0);
	  if (rolp->dispQ == 0) {
	    rolp->inited = -1;
	    break;
	  }

	  rolp->inited = 1;
	  printf("Init - Done\n");
	  break;
	}
      case DA_FREE_PROC:
	  rolp->inited = 0;
	break;
      case DA_DOWNLOAD_PROC:
	__download();
	break;
      case DA_PRESTART_PROC:
	__prestart();
	break;
      case DA_PAUSE_PROC:
	__pause();
	break;
      case DA_END_PROC:
	__end();
	break;
      case DA_GO_PROC:
	__go();
	break;
      case DA_POLL_PROC:
	__poll();
	break;
      case DA_DONE_PROC:
	__done();
	break;
      default:
	printf("WARN: unsupported rol action = %d\n",rolp->daproc);
	break;
      }
}

#endif

