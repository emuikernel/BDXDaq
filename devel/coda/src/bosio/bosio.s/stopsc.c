/*DECK ID>, STOPSC. */
 
/* close private server request */
 
#if defined(IBMMVS) || defined(IBMVM)
#pragma linkage (stopsc,FORTRAN)
#endif
 
#include "netincl.h"
 
#if defined(IBMMVS) || defined(IBMVM)
void stopsc(iclptr, retptrp)
int      *iclptr;      /* index to pointer to client handle         */
int     **retptrp;     /* pointer to pointer to return value        */
                       /* (0 means success, -1 means failure)       */
 {
   int    *retptr;
#else
void stopsc(iclptr, retptr)
#endif
 
#if defined(UNIX) || defined(VAXVMS) || defined(MACII)
int      *iclptr;      /* index to pointer to client handle         */
int      *retptr;      /* pointer to return value (0 means success  */
                       /* -1 means failure)                         */
 {
#endif
   CLIENT  *clptr;
   enum    clnt_stat   clnt_stat;
/* set timeout for RPC requests */
static struct timeval total_timeout = {120,5};
 
#if defined(IBMMVS) || defined(IBMVM)
   retptr = *retptrp;
#endif
   clptr = clntpointer[ *iclptr ];
   clnt_stat=clnt_call(clptr, CLOSE_SERVER, xdr_void, NULL,
                       xdr_void, NULL,total_timeout);
   if(clnt_stat != RPC_SUCCESS)
      {fprintf(stderr,"  clnt_call for CLOSE_SERVER failed\n");
       clnt_perrno(clnt_stat);
       perror(" RPC");
#ifdef MACII
       s_close( *((int *)((clptr)->cl_private)) );
#else
       close( *((int *)((clptr)->cl_private)) );
#endif
       clnt_destroy(clptr);
       clptr = NULL;
       clntpointer[ *iclptr ] = NULL;
       *retptr =  -1;
       return;
      }
   *retptr = 0;
#ifdef MACII
   s_close( *((int *)((clptr)->cl_private)) );
#else
   close( *((int *)((clptr)->cl_private)) );
#endif
   clnt_destroy(clptr);
   clptr = NULL;
   clntpointer[ *iclptr ] = NULL;
 /*  fprintf(stderr,"disconnected\n");
   fflush(stderr); */
 }
 
