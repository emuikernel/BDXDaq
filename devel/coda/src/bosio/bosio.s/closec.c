/*DECK ID>, CLOSEC. */
 
/* close remote file */
 
#if defined(IBMMVS) || defined(IBMVM)
#pragma linkage (closec,FORTRAN)
#endif
 
#include "netincl.h"
 
#if defined(IBMMVS) || defined(IBMVM)
void closec(iclptr,streamptr,retptrp)
#else
void closec(iclptr,streamptr,retptr)
#endif
 
int      *iclptr;      /* index to pointer to client handle         */
int      *streamptr;   /* pointer to stream_id                      */
#if defined(IBMMVS) || defined(IBMVM)
int     **retptrp;     /* pointer to pointer to return value        */
                       /* (0 means success,-1 means failure,        */
                       /* on input 1 closes connection              */
 {
   int     *retptr;
#endif
#if defined(UNIX) || defined(VAXVMS) || defined(MACII)
int      *retptr;      /* pointer to return value (0 means success  */
                       /* -1 means failure)  (on input 1 closes     */
                       /*                          connection)      */
 {
#endif
   CLIENT *clptr;      /* pointer to client handle                  */
   char    *report = NULL;
   int      flag;
   enum    clnt_stat  clnt_stat;
/* set timeout for RPC requests */
static struct timeval total_timeout = {120,5};
 
#if defined(IBMMVS) || defined(IBMVM)
   retptr = *retptrp;
#endif
   clptr = clntpointer[*iclptr];
   flag = *retptr;
   *retptr = 0;
   clnt_stat=clnt_call(clptr, CLOSE, xdr_int, streamptr,
                       xdr_wrapstring, &report,
                       total_timeout);
   if(clnt_stat != RPC_SUCCESS)
      {fprintf(stderr,"  clnt_call for CLOSE failed\n");
       clnt_perrno(clnt_stat);
       perror(" RPC");
       *retptr =  -1;
      }
   if(report != NULL)
      {if(report[0] == 'c')
         {*retptr = -1;
          fprintf(stderr,"SERVER: %s \n", report);
         }
       clnt_freeres(clptr, xdr_wrapstring, &report);
      }
   if(flag == 1)
      {clnt_stat=clnt_call(clptr, CLOSE_SERVER, xdr_void, NULL,
                           xdr_void, NULL,total_timeout);
       if(clnt_stat != RPC_SUCCESS)
         {fprintf(stderr,"  clnt_call for CLOSE_SERVER failed\n");
          clnt_perrno(clnt_stat);
#ifdef MACII
          s_close( *((int *)((clptr)->cl_private)) );
#else
          close( *((int *)((clptr)->cl_private)) );
#endif
          clnt_destroy(clptr);
          clptr = NULL;
          return;
         }
     /* Some RPC implementations (e.g. on IBM MVS ) do not close the
        socket within clnt_destroy(). Socket descriptor is the first
        element of the structure ct_data (pointed to by cl_private)
        defined and used internally in clnt_tcp.c. Hence, ...     */
#ifdef MACII
       s_close( *((int *)((clptr)->cl_private)) );
#else
       close( *((int *)((clptr)->cl_private)) );
#endif
       clnt_destroy(clptr);
       clptr=NULL;
  /*     fprintf(stderr,"disconnected\n");
       fflush(stderr); */
 
       clntpointer[ *iclptr ] = NULL;
      }
 }
 
 
