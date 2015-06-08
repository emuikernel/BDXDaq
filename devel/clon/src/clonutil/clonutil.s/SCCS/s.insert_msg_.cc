h43989
s 00005/00005/00066
d D 1.4 97/05/23 11:27:44 wolin 5 4
c Now using text tag instead of message tag
e
s 00012/00011/00059
d D 1.3 97/04/24 15:26:48 wolin 4 3
c Added new status field, modified severity
e
s 00012/00010/00058
d D 1.2 97/03/27 16:16:17 wolin 3 1
c Added name arg
e
s 00000/00000/00000
d R 1.2 97/03/25 09:54:42 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 2 1 libutil/s/insert_msg_.cc
c Name history : 1 0 s/insert_msg_.cc
e
s 00068/00000/00000
d D 1.1 97/03/25 09:54:41 wolin 1 0
c Inserts message into coda message logging system
e
u
U
f e 0
t
T
I 1
//
// insert_msg_
//
//  fortran callable vsn of insert_msg
//
// usage:
//
D 3
//    status=insert_msg(char *facility, char *process, char *msgclass, char *severity, 
E 3
I 3
D 4
//    status=insert_msg(char *name, char *facility, char *process, char *msgclass, char *severity, 
E 3
//                         int *code, char *text);
E 4
I 4
//    status=insert_msg(char *name, char *facility, char *process, char *msgclass, 
//                      int severity, char *status, int *code, char *text);
E 4
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
D 3
int insert_msg(char *facility, char *process, char *msgclass, char *severity, 
E 3
I 3
D 4
int insert_msg(char *name, char *facility, char *process, char *msgclass, char *severity, 
E 3
		   int code, char *message);
E 4
I 4
int insert_msg(char *name, char *facility, char *process, char *msgclass, 
D 5
	       int severity, char *status, int code, char *message);
E 5
I 5
	       int severity, char *status, int code, char *text);
E 5
E 4
}


//----------------------------------------------------------------


extern "C" {
D 3
int *insert_msg_(char *ffacility, char *fprocess, char *fmsgclass, char *fseverity, 
		   int *fcode, char *fmessage, int l1, int l2, int l3, int l4, int l5){
E 3
I 3
D 4
int *insert_msg_(char *fname, char *ffacility, char *fprocess, char *fmsgclass, char *fseverity, 
		   int *fcode, char *fmessage, int l1, int l2, int l3, int l4, int l5, int l6){
E 4
I 4
int *insert_msg_(char *fname, char *ffacility, char *fprocess, char *fmsgclass, 
		 int *fseverity, char *fstatus,
D 5
		 int *fcode, char *fmessage, int l1, int l2, int l3, int l4, int l5, int l6){
E 5
I 5
		 int *fcode, char *ftext, int l1, int l2, int l3, int l4, int l5, int l6){
E 5
E 4
E 3

D 4
  int status;
E 4
I 4
  int err;
E 4

  // create c strings from fortran character variables
D 3
  char *facility = strdupf(ffacility, l1);
  char *process  = strdupf(fprocess,  l2);
  char *msgclass = strdupf(fmsgclass, l3);
  char *severity = strdupf(fseverity, l4);
  char *message  = strdupf(fmessage,  l5);
E 3
I 3
  char *name     = strdupf(fname,     l1);
  char *facility = strdupf(ffacility, l2);
  char *process  = strdupf(fprocess,  l3);
  char *msgclass = strdupf(fmsgclass, l4);
D 4
  char *severity = strdupf(fseverity, l5);
E 4
I 4
  char *status   = strdupf(fstatus,   l5);
E 4
D 5
  char *message  = strdupf(fmessage,  l6);
E 5
I 5
  char *text     = strdupf(ftext,     l6);
E 5
E 3

  // insert message into database
D 3
  status=insert_msg(facility,process,msgclass,severity,*fcode,message);
E 3
I 3
D 4
  status=insert_msg(name,facility,process,msgclass,severity,*fcode,message);
E 4
I 4
D 5
  err=insert_msg(name,facility,process,msgclass,*fseverity,status,*fcode,message);
E 5
I 5
  err=insert_msg(name,facility,process,msgclass,*fseverity,status,*fcode,text);
E 5
E 4
E 3

  // free memory
I 3
  free(name);
E 3
  free(facility);
  free(process);
  free(msgclass);
D 4
  free(severity);
E 4
I 4
  free(status);
E 4
D 5
  free(message);
E 5
I 5
  free(text);
E 5

  // done
D 4
  return ((int *) status);
E 4
I 4
  return ((int *) err);
E 4

}
}

//----------------------------------------------------------------------


E 1
