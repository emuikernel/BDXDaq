h31357
s 00011/00000/00146
d D 1.4 00/01/25 11:19:05 wolin 5 4
c Added real8 array
e
s 00001/00001/00145
d D 1.3 99/06/04 10:06:48 wolin 4 3
c No arg to flush
c 
e
s 00022/00000/00124
d D 1.2 99/06/04 09:56:49 wolin 3 1
c Added int,real arrays
c 
e
s 00000/00000/00000
d R 1.2 99/06/04 09:50:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 libipc/s/ipc_msg_.cc
e
s 00124/00000/00000
d D 1.1 99/06/04 09:50:01 wolin 1 0
c 
e
u
U
f b 
f e 0
t
T
I 1
// ipc_msg_.cc
//
//  creates ipc messages from fortran
//  assumes ipc_init already called
//
//  ejw, 3-jun-99


// for posix
#define _POSIX_SOURCE_ 1
#define __EXTENSIONS__


// for smartsockets
#include <rtworks/cxxipc.hxx>


// prototypes
extern "C" {
char *strdupf(char *fname, int len);
}


extern "C" {

//----------------------------------------------------------------


  int *ipc_msg_create_(char *typef, char *destf, int len1, int len2) {
    
    char *type    = strdupf(typef,len1);
    char *dest    = strdupf(destf,len2);

    T_IPC_MSG msg = TipcMsgCreate(TipcMtLookup(type));
    TipcMsgSetDest(msg,dest);

    free(type);
    free(dest);

    return(int*)msg;
  }
  

//----------------------------------------------------------------


  int *ipc_msg_add_string_(int *msgf, char *stringf, int len) {

    char *string = strdupf(stringf,len);

    TipcMsgAppendStr((T_IPC_MSG)*msgf,string);
    free(string);

    return((int*)-1);
  }


//----------------------------------------------------------------


  int *ipc_msg_add_int4_(int *msgf, int *int4) {

    TipcMsgAppendInt4((T_IPC_MSG)*msgf,*int4);

    return((int*)-1);
  }


//----------------------------------------------------------------


  int *ipc_msg_add_real4_(int *msgf, float *real4) {

    TipcMsgAppendReal4((T_IPC_MSG)*msgf,*real4);

    return((int*)-1);
  }


//----------------------------------------------------------------


  int *ipc_msg_add_real8_(int *msgf, double *real8) {

    TipcMsgAppendReal8((T_IPC_MSG)*msgf,*real8);

    return((int*)-1);
  }


//----------------------------------------------------------------


I 3
  int *ipc_msg_add_int4_array_(int *msgf, long *i4array, int *len) {

    TipcMsgAppendInt4Array((T_IPC_MSG)*msgf,i4array,*len);

    return((int*)-1);
  }


//----------------------------------------------------------------


  int *ipc_msg_add_real4_array_(int *msgf, float *r4array, int *len) {

    TipcMsgAppendReal4Array((T_IPC_MSG)*msgf,r4array,*len);

    return((int*)-1);
  }


//----------------------------------------------------------------


I 5
  int *ipc_msg_add_real8_array_(int *msgf, double *r8array, int *len) {

    TipcMsgAppendReal8Array((T_IPC_MSG)*msgf,r8array,*len);

    return((int*)-1);
  }


//----------------------------------------------------------------


E 5
E 3
  int *ipc_msg_send_(int *msgf) {

    TipcSrvMsgSend((T_IPC_MSG)*msgf,TRUE);

    return((int*)-1);
  }


//----------------------------------------------------------------

D 4
  int *ipc_msg_flush_(int *msgf) {
E 4
I 4
  int *ipc_msg_flush_() {
E 4

    TipcSrvFlush();

    return((int*)-1);
  }


//----------------------------------------------------------------

  int *ipc_msg_destroy_(int *msgf) {

    TipcMsgDestroy((T_IPC_MSG)*msgf);

    return((int*)-1);
  }


//----------------------------------------------------------------

} // extern "C"
E 1
