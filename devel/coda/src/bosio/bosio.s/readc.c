/*DECK ID>, READC. */
 
/* read from remote file */
 
#if defined(IBMMVS) || defined(IBMVM)
#pragma linkage (readc,FORTRAN)
#endif
 
#include "netincl.h"
 
              /* XDR function for read request
                 ============================= */
 int xdr_get(xdrsp, datap)
 XDR *xdrsp;
 struct req_t *datap;
 {
  if(!xdr_int(xdrsp, &(datap->stream))) return 0;
  if(!xdr_int(xdrsp, &(datap->nbskip))) return 0;
  if(!xdr_int(xdrsp, &(datap->bufsize))) return 0;
  return 1;
 }
 
              /* XDR function for raw bytes
                 ========================== */
 int xdr_raw(xdrsp, datap)
 XDR *xdrsp;
 struct data_t *datap;
 {if(!xdr_int(xdrsp, &(datap->nbytes))) return 0;
  if(datap->nbytes < 0) return 1; /* no data follow */
  if(!xdr_opaque(xdrsp,datap->buffer,
                    (unsigned int)datap->nbytes)) return 0;
  return 1;
 }
 
/* read bytes request */
 
#if defined(IBMMVS) || defined(IBMVM)
void readc(iclptr,streamptr,skipptr,sizeptr,bufferp,retptrp)
#else
void readc(iclptr,streamptr,skipptr,sizeptr,buffer,retptr)
#endif
int      *iclptr;        /* index to pointer to client handle       */
int      *streamptr;     /* pointer to stream_id                    */
int      *skipptr;       /* pointer to #bytes to be skipped         */
int      *sizeptr;       /* pointer to #bytes to be read            */
#if defined(IBMMVS) || defined(IBMVM)
int     **bufferp;       /* pointer to pointer to buffer            */
int     **retptrp;       /* pointer to pointer to #bytes read       */
 
 {
   int *buffer;
   int *retptr;
#endif
#if defined(UNIX) || defined(VAXVMS) || defined(MACII)
int      *buffer;        /* pointer to buffer                       */
int      *retptr;        /* pointer to #bytes read (return value)   */
 
 {
#endif
   CLIENT *clptr;
   struct data_t data;
   struct req_t  req;
   enum    clnt_stat     clnt_stat;
/* set timeout for RPC requests */
static struct timeval total_timeout = {120,5};
 
#if defined(IBMMVS) || defined(IBMVM)
   retptr = *retptrp;
   buffer = *bufferp;
#endif
   data.buffer = (char *) buffer;
   req.stream = *streamptr;
   req.nbskip = *skipptr;
   req.bufsize = *sizeptr;
   clptr = clntpointer[ *iclptr ];
 
   clnt_stat=clnt_call(clptr, READ, xdr_get, &req,
                       xdr_raw, &data,
                       total_timeout);
   if(clnt_stat != RPC_SUCCESS)
        {fprintf(stderr,"  clnt_call for READ failed\n");
         clnt_perrno(clnt_stat);
         perror(" RPC");
#ifdef MACII
         s_close( *((int *)((clptr)->cl_private)) );
#else
         close( *((int *)((clptr)->cl_private)) );
#endif
         clnt_destroy(clptr);
         *retptr = -1;
         return;
        }
   *retptr = data.nbytes;
 }
 
