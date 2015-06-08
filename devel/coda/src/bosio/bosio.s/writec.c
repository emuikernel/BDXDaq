/*DECK ID>, WRITEC. */
 
/* write to remote file */
 
#if defined(IBMMVS) || defined(IBMVM)
#pragma linkage (writec,FORTRAN)
#endif
 
#include "netincl.h"
 
               /* XDR function for write request
                  ============================== */
int xdr_put(xdrsp, writep)
XDR *xdrsp;
struct write_t *writep;
{if(!xdr_int(xdrsp, &(writep->stream))) return 0;
 if(!xdr_int(xdrsp, &(writep->ibyte))) return 0;
 if(!xdr_int(xdrsp, &(writep->nbwrite))) return 0;
 if(!xdr_opaque(xdrsp,writep->buffer,
                   (unsigned int)writep->nbwrite)) return 0;
 return 1;
}
 
/* write bytes request */
 
#if defined(IBMMVS) || defined(IBMVM)
void writec(iclptr,streamptr,ibyteptr,sizeptr,buffer,retptrp)
#else
void writec(iclptr,streamptr,ibyteptr,sizeptr,buffer,retptr)
#endif
 
int      *iclptr;      /* index to pointer to client handle          */
int      *streamptr;   /* pointer to stream_id                       */
int      *ibyteptr;    /* where to start writing                     */
int      *sizeptr;     /* pointer to #bytes to be written            */
int      *buffer;      /* pointer to buffer                          */
#if defined(IBMMVS) || defined(IBMVM)
int     **retptrp;     /* pointer to pointer to return value         */
                       /* (0 means success, -1 means failure)        */
 {
   int             *retptr;
#endif
#if defined(UNIX) || defined(VAXVMS) || defined(MACII)
int      *retptr;      /* pointer to return value (0 means success,  */
                       /*                         -1 means failure)  */
 {
#endif
   CLIENT *clptr;
   char            *report = NULL;
   struct write_t   writed;
   enum    clnt_stat    clnt_stat;
/* set timeout for RPC requests */
static struct timeval total_timeout = {120,5};
 
#if defined(IBMMVS) || defined(IBMVM)
   retptr = *retptrp;
#endif
   clptr = clntpointer[ *iclptr ];
   writed.buffer = (char *) buffer;
   writed.stream = *streamptr;
   writed.ibyte  = *ibyteptr;
   writed.nbwrite = *sizeptr;
 
   clnt_stat=clnt_call(clptr, WRITE, xdr_put, &writed,
                       xdr_wrapstring, &report,
                       total_timeout);
   if(clnt_stat != RPC_SUCCESS)
      {fprintf(stderr,"  clnt_call for WRITE failed\n");
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
      {
      *retptr = -1;
      fprintf(stderr,"SERVER: %s \n", report);
      clnt_freeres(clptr, xdr_wrapstring, &report);
      return;
      }
   clnt_freeres(clptr, xdr_wrapstring, &report);
   *retptr = 0;
 }
 
