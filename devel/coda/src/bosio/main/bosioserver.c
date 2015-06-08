/***********************************************************************
 *
 *   PRIVATE SERVER FOR F-PACKAGE STARTED FROM SUBJOB
 *      Z.Szkutnik 10.09.92.
 *
 * Versions available:
 *   1. Apollo            (#define APOLLO)
 *   2. Silicon Graphics  (#define IRIX)
 *   3. DecStation        (#define ULTRIX)
 *   4. DEC Alpha-OSF     (#define ALPHA)
 *   5. IBM AIX 3.2       (#define AIX)
 *   6. HP-UX             (#define HPUX)
 *   7. Sun Station       (#define SunOS)
 *
 **********************************************************************/

#ifdef Linux
#define _GNU_SOURCE 1 /* This set's __USE_XOPEN to make rpc.h compatible on RH6.0 --Maurik*/
#endif
 
#include <stdio.h>
#include <time.h>
 
#ifdef SunOS	/* serguei - doesn't work yet ... */
#define PORTMAP
#ifdef __STDC__
#undef __STDC__
#endif
#endif
 
#ifdef APOLLO
#include "rpc.h"
#else
#include <rpc/rpc.h>
#endif
 
#include <string.h>
#include <errno.h>
 
#ifdef APOLLO
#include <sys/file.h>
#else
#include <unistd.h>
#endif
 
#ifndef SunOS
#define  MAXNAMELEN  128
#endif
 
#define  TIMEOUT_S  1800  /* timeout for select() in seconds         */
#define  BUFSIZE   32760  /* maximal number of bytes sent in one call*/
#define  FILNO        20  /* maximal number of opened files          */
#define  SENDSIZE  32760  /* NULL defaults size of TCP output buffer */
#define  RECESIZE  32760  /* NULL defaults size of TCP input buffer  */
#define  ONE            1
#define  VERSNUM        1
#define  OPEN           1
#define  CLOSE          2
#define  READ           3
#define  WRITE          4
#define  REWIND         5
#define  LOCK           6
#define  CLOSE_SERVER  10

 
struct data_t             /* input structure for xdr_binary  */
{ int           nbytes;           /* #bytes transmitted              */
  char         *buffer;
};
 
struct req_t              /* input structure for xdr_request */
{ int    stream;                  /* stream_id for I/O               */
  int    nbskip;                  /* #bytes to be skipped            */
  int    bufsize;                 /* #bytes to be read/written       */
};
 
struct write_t            /* input structure for xdr_write   */
{ int           stream;
  int           ibyte;            /* where to start writing          */
  int           nbwrite;          /* #bytes to be written            */
  char         *buffer;
};
 
struct open_t             /* input structure for xdr_open    */
{ char         *name;
  char         *options;
  int           ispc;             /* code for space units: 1=TRK,2=CYL,
                                     anything else='average blksize' */
  int           nr1;              /* primary space                   */
  int           nr2;              /* secondary space                 */
  int           keyunit;          /* 0 for non-keyed files, positive
                                     integer for keyed files, must be
                                     different for different files   */
};
 
struct lock_t             /* input structure for xdr_lock    */
{ char         *name;
  int           stream;
  int           mode;
  int           num;              /* for future use                  */
};
 
   static int xdr_request(xdrsp, datap)
/* =================================== XDR function for read requests */
 XDR *xdrsp;
 struct req_t *datap;
 {if(!xdr_int(xdrsp, &(datap->stream))) return 0;
  if(!xdr_int(xdrsp, &(datap->nbskip))) return 0;
  if(!xdr_int(xdrsp, &(datap->bufsize))) return 0;
  return 1;
 }
 
   static int xdr_binary(xdrsp, datap)
/* =================================== XDR function for raw bytes */
XDR *xdrsp;
struct data_t *datap;
{if(!xdr_int(xdrsp,&(datap->nbytes))) return 0;
 if(datap->nbytes < 0) return 1; /* no data follow */
 if(!xdr_opaque(xdrsp,datap->buffer,
                        (unsigned int)datap->nbytes)) return 0;
 return 1;
}
 
   static int xdr_write(xdrsp, writep)
/* =================================== XDR function for write request */
XDR *xdrsp;
struct write_t *writep;
{if(!xdr_int(xdrsp, &(writep->stream))) return 0;
 if(!xdr_int(xdrsp, &(writep->ibyte))) return 0;
 if(!xdr_int(xdrsp, &(writep->nbwrite))) return 0;
 if(!xdr_opaque(xdrsp,writep->buffer,
                      (unsigned int)writep->nbwrite)) return 0;
 return 1;
}
 
   static int xdr_open(xdrsp, openp)
/* ================================= XDR function for open request */
XDR *xdrsp;
struct open_t *openp;
{if(!xdr_wrapstring(xdrsp, &(openp->name))) return 0;
 if(!xdr_wrapstring(xdrsp, &(openp->options))) return 0;
 if(!xdr_int(xdrsp, &(openp->ispc))) return 0;
 if(!xdr_int(xdrsp, &(openp->nr1))) return 0;
 if(!xdr_int(xdrsp, &(openp->nr2))) return 0;
 if(!xdr_int(xdrsp, &(openp->keyunit))) return 0;
 return 1;
}
 
   static int xdr_lock(xdrsp, lockp)
/* ================================= XDR function foor lock request */
XDR *xdrsp;
struct lock_t *lockp;
{if(!xdr_wrapstring(xdrsp, &(lockp->name))) return 0;
 if(!xdr_int(xdrsp, &(lockp->stream))) return 0;
 if(!xdr_int(xdrsp, &(lockp->mode))) return 0;
 if(!xdr_int(xdrsp, &(lockp->num))) return 0;
 return 1;
}
 
 
/* ================  Global variables   ========================= */
static FILE *log = NULL;
static unsigned long PROGNUM;
static int null_fds = 0;
/* ============================================================== */
 
   static void svc_runT(transp)
/* ============================== main loop waiting for requests */
SVCXPRT          *transp;
{ int  readfds;
  int  j,count,dtbsz = 32;
  int  i, ret, k;
  struct timeval  timeout;
 
  timeout.tv_sec = 120;    /* wait at most 2 min for a connection */
  timeout.tv_usec = 0;
 
again:
  readfds = svc_fds;
#ifdef HPUX
  switch ( select(dtbsz, &readfds, NULL, NULL, &timeout) )
#else
  switch ( select(dtbsz, (fd_set *)&readfds, NULL, NULL, &timeout) )
#endif
    { case 0:
        svc_unregister(PROGNUM,VERSNUM);
        fprintf(log,"timeout, no connection opened\n");
        exit(0);
      case -1:
        if(errno == EINTR) goto again;
        svc_unregister(PROGNUM,VERSNUM);
        fprintf(log,"bosioserver closed: error in first select\n");
        exit(0);
      default:
        svc_getreq(readfds);
        svc_destroy(transp); /* accept no other connection */
        break;
    }
 
  for(;;)
  { readfds = svc_fds;
    timeout.tv_sec = TIMEOUT_S;
    timeout.tv_usec = 0;
 
    if( (readfds - null_fds) == 0 )
       {svc_unregister(PROGNUM,VERSNUM);
        fprintf(log,"bosioserver closed: connection died\n");
        fflush(log);
        exit(0);
       }
 
#ifdef HPUX
    switch ( select(dtbsz, &readfds, NULL, NULL, &timeout) )
#else
    switch ( select(dtbsz, (fd_set *)&readfds, NULL, NULL, &timeout) )
#endif
    { case 0:
        for( j = 31; j >= 0; --j)
           if( readfds & (1 << j) )
             {
              shutdown(j,2);
              close(j);
             }
        svc_unregister(PROGNUM,VERSNUM);
        fprintf(log,"bosioserver closed: timeout\n");
        fflush(log);
        exit(0);
      case -1:
        if(errno == EINTR) continue;
        svc_unregister(PROGNUM,VERSNUM);
        fprintf(log,"bosioserver closed: error in select\n");
        exit(0);
      default:
        svc_getreq(readfds);
    }
  }
}
 
   static void dispatch(rqstp, transp)
/* ===================================  dispatch routine */
struct  svc_req  *rqstp;
SVCXPRT          *transp;
 
{   int               stream,ret,count,ret_len,len,i,k;
    struct data_t    *datap,data;
    static char       name[MAXNAMELEN],space[80],buffer[BUFSIZE];
    static char       opt[MAXNAMELEN];
    char             *report, *strptr;
    struct req_t     *reqp,req;
    struct write_t   *writep,writed;
    struct open_t    *openp,opend;
    struct lock_t    *lockp,lockd;
 
    switch (rqstp->rq_proc)
        {
    case NULLPROC:
        if (!svc_sendreply(transp, xdr_void, 0))
                 fprintf(log, "NULLPROC: Cannot reply to RPC call.\n");
        break;
 
    case OPEN:
        openp = &opend;
        openp->name = name;
        openp->options = opt;
#ifdef AIX
        if (!svc_getargs(transp, xdr_open, (caddr_t)openp))
#else
        if (!svc_getargs(transp, xdr_open, openp))
#endif
           { svcerr_decode(transp);
             break;
           }
 
        stream = (int) fopen(openp->name,openp->options);
        if(stream == 0) perror("OPEN");
 
back:
#ifdef AIX
        if (!svc_sendreply(transp, xdr_int, (caddr_t)&stream))
#else
        if (!svc_sendreply(transp, xdr_int, &stream))
#endif
                   fprintf(log, "OPEN: Cannot reply to RPC call.\n");
        break;
 
    case READ:
        reqp = &req;
#ifdef AIX
        if(!svc_getargs(transp, xdr_request, (caddr_t)reqp))
#else
        if(!svc_getargs(transp, xdr_request, reqp))
#endif
           { svcerr_decode(transp);
             break;
           }
 
        datap = &data;
 
        {long offset, current, request;
           int whence;
 
           request = (long)reqp->nbskip;
           current = ftell((FILE *)reqp->stream);
           offset = request;
           whence = SEEK_CUR;
 
           if( offset < 0)
              { offset = current + request;
                whence = SEEK_SET;}
 
        if(fseek((FILE *)reqp->stream, offset, whence))
          ret_len = -3;
        else
          ret_len = 0;
       }
 
        if(ret_len == (long)0)
         {ret_len=(long)fread((void *)buffer, ONE,(size_t)reqp->bufsize,
                         (FILE *)reqp->stream);
          if(ferror((FILE *)reqp->stream))
             { perror("READ problem");
               clearerr( (FILE *)reqp->stream );
               ret_len = -2;
             }
         }
        datap->nbytes = (int) ret_len;
        datap->buffer = buffer;
#ifdef AIX
        if(!svc_sendreply(transp, xdr_binary, (caddr_t)datap))
#else
        if(!svc_sendreply(transp, xdr_binary, datap))
#endif
                     fprintf(log, "READ: Cannot reply to RPC call.\n");
        break;
 
    case WRITE:
        writep = &writed;
        writep->buffer = buffer;
#ifdef AIX
        if (!svc_getargs(transp,xdr_write,(caddr_t)writep))
#else
        if (!svc_getargs(transp,xdr_write,writep))
#endif
           { svcerr_decode(transp);
             break;
           }
 
        if(fseek((FILE *)writep->stream,(long)writep->ibyte-1, SEEK_SET))
           ret_len = -3;
        else
           ret_len = 0;
 
        if(ret_len == (long)0)
               fwrite(buffer, ONE, (size_t)writep->nbwrite,
                                    (FILE *)writep->stream);
        if(ferror( (FILE *)writep->stream) || (ret_len == -3) )
             {report = "cannot write bytes";
              perror("WRITE problem");
              clearerr( (FILE *)writep->stream );
             }
        else
             report = "bytes written";
#ifdef AIX
        if (!svc_sendreply(transp, xdr_wrapstring, (caddr_t)&report))
#else
        if (!svc_sendreply(transp, xdr_wrapstring, &report))
#endif
                    fprintf(log, "WRITE: Cannot reply to RPC call.\n");
        break;
 
    case REWIND:
#ifdef AIX
        if (!svc_getargs(transp, xdr_int, (caddr_t)&stream))
#else
        if (!svc_getargs(transp, xdr_int, &stream))
#endif
           { svcerr_decode(transp);
             break;
           }
        report = "remote file rewound";
        rewind((FILE *)stream);
#ifdef AIX
        if (!svc_sendreply(transp, xdr_wrapstring, (caddr_t)&report))
#else
        if (!svc_sendreply(transp, xdr_wrapstring, &report))
#endif
                   fprintf(log, "REWIND: Cannot reply to RPC call.\n");
        break;
 
    case CLOSE:
#ifdef AIX
        if (!svc_getargs(transp, xdr_int, (caddr_t)&stream))
#else
        if (!svc_getargs(transp, xdr_int, &stream))
#endif
           { svcerr_decode(transp);
             break;
           }
        if(fclose((FILE *) stream) == EOF)
             report="cannot close file";
        else
             report = "remote file closed";
 
#ifdef AIX
        if (!svc_sendreply(transp, xdr_wrapstring, (caddr_t)&report))
#else
        if (!svc_sendreply(transp, xdr_wrapstring, &report))
#endif
                   fprintf(log, "CLOSE: Cannot reply to RPC call.\n");
        break;
 
    case LOCK:
        lockp = &lockd;
        lockp->name = name;
#ifdef AIX
        if (!svc_getargs(transp, xdr_lock, (caddr_t)lockp))
#else
        if (!svc_getargs(transp, xdr_lock, lockp))
#endif
           { svcerr_decode(transp);
             break;
           }
        ret = 0;
        len = strlen(lockp->name);
 
        if(lockp->mode == 1)  /* lock */
           {
#ifdef APOLLO
            if(flock(fileno((FILE *)lockd.stream), LOCK_EX | LOCK_NB)
                           == -1)
#else
            if(lockf(fileno((FILE *)lockd.stream), F_TLOCK,0) == -1)
#endif
               {ret = 1;
                perror("LOCK");
               }
           }
        else                  /* unlock */
           {
 
#ifdef APOLLO
            if(flock(fileno((FILE *)lockd.stream), LOCK_UN) == -1)
#else
            if(lockf(fileno((FILE *)lockd.stream), F_ULOCK,0) == -1)
#endif
               {ret = 1;
                perror("UNLOCK");
               }
           }
#ifdef AIX
        if (!svc_sendreply(transp, xdr_int, (caddr_t)&ret))
#else
        if (!svc_sendreply(transp, xdr_int, &ret))
#endif
                   fprintf(log, "LOCK: Cannot reply to RPC call.\n");
        break;
 
    case CLOSE_SERVER:
        if (!svc_sendreply(transp, xdr_void, NULL))
             fprintf(log, "CLOSE_SERVER: Cannot reply to RPC call.\n");
        svc_unregister(PROGNUM,VERSNUM);
        fprintf(log,"bosioserver closed\n");
        fflush(log);
        exit(0);
 
    default:
        svcerr_noproc(transp);
        return;
        }
}
 
   main(argc,argv)
/* =============== */
   int argc;
   char *argv[];
{
  SVCXPRT *transp;
  char    *strstop;
  int      i;
  int      pipe_soc, count;
  short    port;
  struct   sockaddr_in name;
  int      namelen;
 
  log = freopen("fpnet.log", "w",stderr);
  if(log == NULL) log = stderr;
 
  if((PROGNUM = (unsigned long)strtol(argv[1], &strstop, 0))
                 == (unsigned long)NULL )
       { fprintf(log,"%s \n","Cannot decode PROGNUM");
         exit(-1);
       }
  else
    fprintf(log,"PROGNUM to be registered is %x \n", PROGNUM);
 
  pipe_soc = (int)strtol(argv[2], &strstop, 0);
 
  transp = svctcp_create(RPC_ANYSOCK, SENDSIZE, RECESIZE);
  if (transp == NULL)
       {fprintf(log,"Cannot create an RPC transport handle.\n");
        exit(1);
       }
  if (!svc_register(transp,PROGNUM, VERSNUM, dispatch, 0))
       {fprintf(log,"Cannot register FPACK service\n");
        svc_destroy(transp);
        exit(1);
       }
  fprintf(log,"bosioserver listening\n");
  fflush(log);
 
  namelen = sizeof(struct sockaddr_in);
  if( getsockname(transp->xp_sock, &name, &namelen) )
       {fprintf(log,"Cannot get socket address \n");
        svc_destroy(transp);
        exit(1);
       }
  port = ntohs(name.sin_port);
  if( write( pipe_soc, (void *)&port, 2) != 2)
    {fprintf(log,"cannot write to pipe socket\n");
     perror("write");
     close( pipe_soc );
     svc_destroy(transp);
     exit(1);
    }
  close( pipe_soc );
 
  svc_runT(transp);       /* never returns */
  fprintf(log,"should never reach this point\n");
  svc_unregister(PROGNUM,VERSNUM);
  exit(1);
}
 
