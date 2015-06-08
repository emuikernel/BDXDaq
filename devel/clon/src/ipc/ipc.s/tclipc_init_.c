/*
 *  tclipc_init_
 *
 *  fortran wrapper for tclipc_init
 *  also provides wrapper for tcl_evalfile
 *
 *  ejw 7-oct-96
 */

#define _POSIX_SOURCE 1
#define __EXTENSIONS__

#include <tcl.h>

char *strdupf(char *fname, int len);


/*----------------------------------------------------------------*/


int *tclinterp_init_(void){

  return( (int*) tclinterp_init());

}


/*----------------------------------------------------------------*/


int *tclipc_init_(int *finterp){

  Tcl_Interp *interp = (Tcl_Interp *) *finterp;

  return( (int*) tclipc_init(interp));

}


/*----------------------------------------------------------------*/


int *tcl_evalfile_(int *finterp, char *fname, int flen){

  Tcl_Interp *interp=(Tcl_Interp *) *finterp;
  char *name=strdupf(fname,flen);
  
  Tcl_EvalFile(interp,name);
  free(name);

}


/*----------------------------------------------------------------*/
