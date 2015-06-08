/*DECK ID>, REWINC. */
 
/* rewind remote file */
 
#if defined(IBMMVS) || defined(IBMVM)
#pragma linkage (rewinc,FORTRAN)
#endif
 
#include "netincl.h"
 
#if defined(IBMMVS) || defined(IBMVM)
void rewinc(iclptr,streamptr,retptrp)
#else
void rewinc(iclptr,streamptr,retptr)
#endif
int      *iclptr;      /* index to pointer to client handle         */
int      *streamptr;   /* pointer to stream_id                      */
#if defined(IBMMVS) || defined(IBMVM)
int     **retptrp;     /* pointer to pointer to return value        */
                       /* (0 means success , -1 means failure)      */
 {
   int     *retptr;
#endif
#if defined(UNIX) || defined(VAXVMS) || defined(MACII)
int      *retptr;      /* pointer to return value (0 means success  */
                       /* -1 means failure)                         */
 {
#endif
   CLIENT *clptr;
   char    *report = NULL;
   enum    clnt_stat  clnt_stat;
/* set timeout for RPC requests */
static struct timeval total_timeout = {120,5};
 
#if defined(IBMMVS) || defined(IBMVM)
   retptr = *retptrp;
#endif
   clptr = clntpointer[ *iclptr ];
   clnt_stat=clnt_call(clptr, REWIND, xdr_int, streamptr,
                       xdr_wrapstring, &report,
                       total_timeout);
   if(clnt_stat != RPC_SUCCESS)
      {fprintf(stderr,"  clnt_call for REWIND failed\n");
       clnt_perrno(clnt_stat);
       perror(" RPC");
       clnt_freeres(clptr, xdr_wrapstring, &report);
#ifdef MACII
       s_close( *((int *)((clptr)->cl_private)) );
#else
       close( *((int *)((clptr)->cl_private)) );
#endif
       clnt_destroy(clptr);
       *retptr =  -2;
       return;
      }
   if(report[0] == 'c')
      { *retptr = -1;
        fprintf(stderr,"SERVER: %s \n", report);
        clnt_freeres(clptr, xdr_wrapstring, &report);
        return;
      }
   clnt_freeres(clptr, xdr_wrapstring, &report);
   *retptr = 0;
 }
 
