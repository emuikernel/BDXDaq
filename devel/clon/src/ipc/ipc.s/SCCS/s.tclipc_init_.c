h31818
s 00004/00002/00050
d D 1.5 96/10/28 12:54:16 wolin 6 5
c No more ipc_tcl_interp...using T_CB_ARG
e
s 00004/00003/00048
d D 1.4 96/10/21 13:59:23 wolin 5 4
c Fixed passing fortran interp
e
s 00010/00000/00041
d D 1.3 96/10/16 09:38:40 wolin 4 3
c Added tclinterp_init
e
s 00018/00007/00023
d D 1.2 96/10/15 11:55:03 wolin 3 1
c Removed startup script from tclipc_init call
e
s 00000/00000/00000
d R 1.2 96/10/07 17:17:35 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libipc/s/tclipc_init_.c
c Name history : 1 0 s/tclipc_init_.c
e
s 00030/00000/00000
d D 1.1 96/10/07 17:17:34 wolin 1 0
c Initializes tcl interp, creates callback for tcl_request message, calls tcl_eval, sends back tcl_reply message
e
u
U
f e 0
t
T
I 1
/*
 *  tclipc_init_
 *
D 3
 *  fortran wrapper for tcl_ipc_init(tcl_startup_script)
E 3
I 3
 *  fortran wrapper for tclipc_init
 *  also provides wrapper for tcl_evalfile
E 3
 *
 *  ejw 7-oct-96
 */

#define _POSIX_SOURCE 1
#define __EXTENSIONS__

D 3
char *strdupf(char *, int);
E 3
I 3
#include <tcl.h>
E 3

I 3
D 5
extern Tcl_Interp *ipc_tcl_interp;
E 5
char *strdupf(char *fname, int len);
E 3

I 3

E 3
/*----------------------------------------------------------------*/


I 4
int *tclinterp_init_(void){

  return( (int*) tclinterp_init());

}


/*----------------------------------------------------------------*/


E 4
D 3
int *tclipc_init_(char *namef, int len){
E 3
I 3
D 6
int *tclipc_init_(void){
E 6
I 6
int *tclipc_init_(int *finterp){
E 6
E 3

D 3
  int status;
  char *name=strdupf(namef,len);
E 3
I 3
D 6
  return( (int*) tclipc_init());
E 6
I 6
  Tcl_Interp *interp = (Tcl_Interp *) *finterp;
E 6
E 3

I 6
  return( (int*) tclipc_init(interp));

E 6
D 3
  status=tclipc_init(name);
E 3
I 3
}


/*----------------------------------------------------------------*/


D 5
int *tcl_evalfile_(char *fname, int flen){
E 5
I 5
int *tcl_evalfile_(int *finterp, char *fname, int flen){
E 5

I 5
  Tcl_Interp *interp=(Tcl_Interp *) *finterp;
E 5
  char *name=strdupf(fname,flen);
D 5
  Tcl_EvalFile(ipc_tcl_interp,name);
E 5
I 5
  
  Tcl_EvalFile(interp,name);
E 5
E 3
  free(name);

D 3
  return( (int*) status);
E 3
}


/*----------------------------------------------------------------*/
E 1
