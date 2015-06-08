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


  int *ipc_msg_add_real8_array_(int *msgf, double *r8array, int *len) {

    TipcMsgAppendReal8Array((T_IPC_MSG)*msgf,r8array,*len);

    return((int*)-1);
  }


//----------------------------------------------------------------


  int *ipc_msg_send_(int *msgf) {

    TipcSrvMsgSend((T_IPC_MSG)*msgf,TRUE);

    return((int*)-1);
  }


//----------------------------------------------------------------

  int *ipc_msg_flush_() {

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
