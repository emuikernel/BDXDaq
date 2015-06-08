//
// insert_msg_
//
//  fortran callable vsn of insert_msg
//
// usage:
//
//    status=insert_msg(char *name, char *facility, char *process, char *msgclass, 
//                      int severity, char *status, int *code, char *text);
//
//    returns 0 if ok, non-zero otherwise
//
//
//  ejw, 14-mar-97


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// system stuff
#include <stdlib.h>


// prototypes
extern "C"{
char *strdupf(char *fname, int flen);
int insert_msg(char *name, char *facility, char *process, char *msgclass, 
	       int severity, char *status, int code, char *text);
}


//----------------------------------------------------------------


extern "C" {
int *insert_msg_(char *fname, char *ffacility, char *fprocess, char *fmsgclass, 
		 int *fseverity, char *fstatus,
		 int *fcode, char *ftext, int l1, int l2, int l3, int l4, int l5, int l6){

  int err;

  // create c strings from fortran character variables
  char *name     = strdupf(fname,     l1);
  char *facility = strdupf(ffacility, l2);
  char *process  = strdupf(fprocess,  l3);
  char *msgclass = strdupf(fmsgclass, l4);
  char *status   = strdupf(fstatus,   l5);
  char *text     = strdupf(ftext,     l6);

  // insert message into database
  err=insert_msg(name,facility,process,msgclass,*fseverity,status,*fcode,text);

  // free memory
  free(name);
  free(facility);
  free(process);
  free(msgclass);
  free(status);
  free(text);

  // done
  return ((int *) err);

}
}

//----------------------------------------------------------------------


