/*
 * codatcl.c - rcframe and other commands implementation
 */

#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <errno.h>

#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <X11/StringDefs.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <tcl.h>
#include <tk.h>

#include "codaRegistry.h"



/*from old ns.c
note: clientData ignored*/
int
Codatcl_codaSendCmd (ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
  return coda_send(Tk_Display(Tk_MainWindow(interp)),argv[1],argv[2]);
}


/*from old ns.c
note: clientData ignored*/
int
Codatcl_codaRCFrame (ClientData clientData, Tcl_Interp *interp, int argc, char **argv)
{
  char tmp1[400];
  char tmp2[400];
  int wid;

  printf("Codatcl_codaRCFrame reached\n");fflush(stdout);

  if (argc != 3)
  {
    Tcl_SetResult(interp,"usage : rcframe ?widget? ?name?",TCL_STATIC);
    printf("Codatcl_codaRCFrame 0\n");fflush(stdout);
    return TCL_ERROR;
  }

  printf("Codatcl_codaRCFrame 1\n");fflush(stdout);

  sprintf(tmp1,"%s_WINDOW",argv[2]);
  
  wid = CODAGetAppWindow(Tk_Display(Tk_MainWindow(interp)),tmp1);
  
  if (wid == 0)
  {
    int ix;

    sprintf(tmp1,"t:%d %s",getpid(),argv[2]);
	
    coda_send(Tk_Display(Tk_MainWindow(interp)),"RUNCONTROL",tmp1);
	
    for(ix=0; ix<500; ix++)
    {
      sleep(1);
      sprintf(tmp1,"%s_WINDOW",argv[2]);
	  
      wid = CODAGetAppWindow(Tk_Display(Tk_MainWindow(interp)),tmp1);
	  
      if (wid) break;
    }

    if(wid == 0)
    {
      Tcl_SetResult(interp,"window not created",TCL_STATIC);
      return TCL_ERROR;
    }
  }
  sprintf(tmp2,"toplevel %s -use %d",argv[1],wid);
  
  return Tcl_Eval(interp,tmp2); 
}




/* Initialize the full package */
int
Codatcl_Init(Tcl_Interp *interp)
{  
  if (Tcl_PkgProvide(interp, "Codatcl", "1.0") != TCL_OK)
  {
    printf("ERRRRROR\n");
    return(TCL_ERROR);
  }
  else
  {
    printf("Set Codatcl version to 1.0\n");
  }

  Tcl_CreateCommand(interp, "coda_send", Codatcl_codaSendCmd,
		      (ClientData) NULL, (void (*)()) NULL);

  Tcl_CreateCommand(interp, "rcframe", Codatcl_codaRCFrame,
		      (ClientData)NULL, (void (*)())NULL);  

  return(TCL_OK);
}
