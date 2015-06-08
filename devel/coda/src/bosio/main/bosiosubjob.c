/***********************************************************************
 *
 *   PERMANENT SERVER FOR F-PACKAGE
 *      Z.Szkutnik 15.09.92.
 *
 * Versions available:
 *   1. Apollo                       (#define APOLLO)
 *   2. Silicon Graphics             (#define IRIX)
 *   3. DecStation                   (#define ULTRIX)
 *   4. DEC Alpha-OSF                (#define ALPHA)
 *   5. IBM AIX 3.2                  (#define AIX)
 *   6. HP-UX                        (#define HPUX)
 *   7. Sun Station                  (#define SunOS) serguei - doesn't work yet ...
 *   8. Linux                        (#define Linux) serguei - doesn't work yet ...
 *
 **********************************************************************/

#ifdef Linux
#define _GNU_SOURCE 1 /* This set's __USE_XOPEN to make rpc.h compatible on RH6.0 --Maurik*/
#endif

 
#include <stdio.h>

/*
#ifdef Linux
#define PORTMAP
#ifdef __STDC__
#undef __STDC__
#endif
#endif 
*/ 

#ifdef SunOS
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
 
#include <sys/socket.h>
#include <time.h>
#include <netdb.h>
 
#if defined(ULTRIX) || defined(ALPHA) || defined(HPUX)
#include <sys/wait.h>
#include <signal.h>
#else
#include <sys/signal.h>
#endif
 
#include <pwd.h>
#include <fcntl.h>
#include <errno.h>
 
#define  PROGNUM      0x20805901
#define  VERSNUM      1
#define  PORTNUM      1710
#define  CLOSE_SERVER 10
#define  CHECK_PORT   15
#define  SUBMIT       20

typedef struct
{
  char *User, *Password;
  char *Class, *Time_M, *Time_S;
} AUTH_T ;

/* =============== Global variables ============================= */
static int pipe_sockets[32], cur_soc;
static unsigned long cliprognums[32];
static short         cliports[32];
static int s;                           /* socket number */
static SVCXPRT *Transp;                 /* main transport handle */
static unsigned long numprog = 0x40000000;
static time_t   timevar;
/* =============================================================== */

static int xdr_auth(xdrsp, authp)
XDR *xdrsp;
AUTH_T *authp;
{
  if(!xdr_wrapstring(xdrsp,&(authp->User))) return 0;
  if(!xdr_wrapstring(xdrsp,&(authp->Password))) return 0;
  if(!xdr_wrapstring(xdrsp,&(authp->Class))) return 0;
  if(!xdr_wrapstring(xdrsp,&(authp->Time_M))) return 0;
  if(!xdr_wrapstring(xdrsp,&(authp->Time_S))) return 0;
  return 1;
}

#if defined(HPUX) || defined(Linux) || defined(Darwin)
struct passwd *getpwnam( const char * ) ;
#else
struct passwd *getpwnam( char * ) ;
#endif

#if defined(Linux)
char *crypt( const char * , const char * ) ;
#else
char *crypt( char * , char * ) ;
#endif

struct passwd *getUSER(name, password)
char *name , *password ;
{
  struct passwd *user ;
  char salt[4] , *encrypt ;
/*
 *  look up the user name in the password file /etc/passwd.
 */
  if((user = getpwnam(name)) == NULL) return(NULL) ;
/*
 *  copy the salt of the password
 */
  strncpy(salt, user -> pw_passwd, 2);
  salt[2] = '\0';
/*
 *   encrypt the specified password with this salt
 */
  encrypt = crypt(password, salt);
/*
 *   compare, if equal to encrypted password in system file.
 */
  if(strcmp(encrypt, user -> pw_passwd)) return(NULL) ;
/*
 *     password belongs to this user,
 *     so we can return user and group ID's
 */
  return(user) ;
}

char *setUSER(name, password)
char *name, *password;
{
  struct passwd *user ;
 
  if( ( user = getUSER( name , password ) ) == NULL )return(NULL) ;
 
  if( setgid( user -> pw_gid ) != 0 )return(NULL) ;
 
  if( setuid( user -> pw_uid ) != 0 )return(NULL) ;
 
  return( user -> pw_dir ) ;
}

int jobsub(jobname, user, password, account, class, msgclass, timem, times, prognum)
char  *jobname , *user , *password , *account , *class , *msgclass,
      *timem , *times , *prognum;
{
  char path[128], *user_dir, userid[10], passwd[10], numprog[20], sock[5];
  int  i, sockets[2];
  if(pipe(sockets) < 0)
  {
    perror("opening socket pair");
    return(-1);
  }

#ifdef FPPATH
  /* take the pathname of bosioserver as defined in the compiler option */
  strcpy(path, FPPATH);
#else
  /* assume bosioserver resides in the current working directory */
  if(getcwd(path, 120) == NULL)
  {
    perror("pwd");
    return(-1);
  }
  strcat(path,"/bosioserver");
#endif
 
  strcpy(userid, user);
  strcpy(passwd, password);
  strcpy(numprog, prognum);
  fflush(stdout);
 
  switch( fork() )
  {
    case 0:   /* this is the child */
 
        close(s);                /* close parents socket... */
        close(sockets[0]);
        for(i = 0; i < 32; i++)  /* ...and all other sockets */
           if( svc_fds & ( 1 << i ) ) close(i);
        if( (user_dir= setUSER(userid, passwd)) == NULL ) exit(0);
        chdir(user_dir);
        sprintf(sock,"%d",sockets[1]);
        if( execl(path,path,numprog,sock,(char *)NULL) < 0 ) exit(0);
        break;
 
    case -1:     /* error in fork */
 
        perror("fork");
        close(sockets[1]);
        close(sockets[0]);
        return(-1);
    default:     /* this is the parent process */
        close(sockets[1]);
        pipe_sockets[cur_soc] = sockets[0];
#ifdef APOLLO
        /* make the pipe socket nonblocking */
        if( fcntl(sockets[0], F_SETFL, FNDELAY) < 0 )
        {
          close(sockets[0]);
          perror("fcntl");
          return(-1);
        }
#endif
        return 0;
  }
}
 
/* === main loop === */

static void svc_runS( transp)
SVCXPRT *transp;
{
  int readfds;
  register int i, j, count;
 
  for(;;)
  {
    readfds = svc_fds;
 
    /* free prognums blocked because of private server failures */
    for(j = 0; j < 32; j++)
    {
      if((cliports[j] == PORTNUM) && !(readfds & (1 << j)))
      {
        if(cliprognums[j] != (unsigned long)0)
        {
          cliports[j] = 0;
          cliprognums[j] = 0;
          close(pipe_sockets[j]);
          pipe_sockets[j] = 0;
        }
      }
    }

    if(readfds == 0)
    { 
      svc_unregister( PROGNUM,VERSNUM);
      printf("BOSIOSERVER: closed.\n");
      fflush(stdout);
      exit(0);
    }

#ifdef HPUX
    switch( select(32, &readfds, NULL, NULL, NULL) )
#else
    switch( select(32, (fd_set *)&readfds, NULL, NULL, NULL) )
#endif
    {
      case -1:
        if(errno == EINTR) continue;
        printf("BOSIOSERVER: error in select.\n");
        perror("select");
        fflush(stdout);
        svc_destroy(transp);
        break;
      case 0:
        break;
      default:
        svc_getreq(readfds);
    }
  }
}
 
#if defined(ULTRIX) || defined(ALPHA) || defined(HPUX)
void reapchild()
{
  union wait status;
#ifndef HPUX
  wait3(&status, WNOHANG, (struct rusage *)0);
#else
  wait3((int *)&status, WNOHANG, (int *)0);
#endif
}
#endif
 
/* === dispatch routine === */
void dispatch(rqstp, transp)
struct  svc_req  *rqstp;
SVCXPRT          *transp;
{
  AUTH_T   authd;
  unsigned long  numprog1, resnum;
  short    port;
  char     prognum[20], jobname[10], msgclass[2], jobclass[2];
  register int  ret, i, len;
  struct hostent *cln;
  struct sockaddr_in *cln_soc;
  char    *hostname;
  char    *inet_ntoa();
#ifndef APOLLO
  fd_set pipe_fd;
  struct timeval timeout;
#endif
  authd.User     = NULL;
  authd.Password = NULL;
  authd.Class    = NULL;
  authd.Time_M   = NULL;
  authd.Time_S   = NULL;
  cur_soc = transp->xp_sock;

  switch(rqstp->rq_proc)
  {
    case NULLPROC:
      svc_sendreply(transp, xdr_void, 0);
      break;
 
    case SUBMIT:
#ifdef AIX
      if(!svc_getargs(transp,xdr_auth,(caddr_t)&authd))
#else
      if(!svc_getargs(transp,xdr_auth,&authd))
#endif
      {
        svcerr_decode(transp);
        break;
      }
      cln_soc = svc_getcaller(transp);
      cln = gethostbyaddr((char *) &cln_soc->sin_addr,
                           sizeof(struct in_addr),
                           cln_soc->sin_family);
      if(cln == NULL)
      {
        hostname = inet_ntoa(cln_soc->sin_addr);
      }
      else
      {
        hostname = cln->h_name;
      }
      time(&timevar);
      printf("User %s connected from %s at %s",
               authd.User, hostname, ctime(&timevar));
      fflush(stdout);
      ++numprog;
      if(numprog >= 0x5fffffff) numprog = 0x40000000;

      sprintf(prognum,"0x%x",numprog);

      if(jobsub(jobname,authd.User,authd.Password,"00010221",
           jobclass,msgclass,authd.Time_M,authd.Time_S, prognum) < 0)
      {
        resnum = 0;
      }
      else
      {
        resnum = numprog;
        cliprognums[transp->xp_sock] = numprog;
      }
 
done:

#ifdef AIX
      svc_sendreply(transp,xdr_u_long,(caddr_t)&resnum);
      svc_freeargs(transp,xdr_wrapstring,(caddr_t)&(authd.User));
      svc_freeargs(transp,xdr_wrapstring,(caddr_t)&(authd.Password));
      svc_freeargs(transp,xdr_wrapstring,(caddr_t)&(authd.Class));
      svc_freeargs(transp,xdr_wrapstring,(caddr_t)&(authd.Time_M));
      svc_freeargs(transp,xdr_wrapstring,(caddr_t)&(authd.Time_S));
#else
      svc_sendreply(transp,xdr_u_long,&resnum);
      svc_freeargs(transp,xdr_wrapstring,&(authd.User));
      svc_freeargs(transp,xdr_wrapstring,&(authd.Password));
      svc_freeargs(transp,xdr_wrapstring,&(authd.Class));
      svc_freeargs(transp,xdr_wrapstring,&(authd.Time_M));
      svc_freeargs(transp,xdr_wrapstring,&(authd.Time_S));
#endif
      break;
 
    case CHECK_PORT:
#ifdef AIX
      if(!svc_getargs(transp,xdr_u_long,(caddr_t)&numprog1))
#else
      if(!svc_getargs(transp,xdr_u_long,&numprog1))
#endif
      {
        svcerr_decode(transp);
        break;
      }
#ifdef APOLLO
      errno = 0;
      read(pipe_sockets[transp->xp_sock], (char *)&port, 2);
      if(errno == EWOULDBLOCK)
      {
        port = PORTNUM;
      }
      else
      {
        close(pipe_sockets[transp->xp_sock]);
        pipe_sockets[transp->xp_sock] = 0;
      }
#else
      FD_ZERO(&pipe_fd);
      FD_SET(pipe_sockets[transp->xp_sock], &pipe_fd);
      timeout.tv_sec = 0;
      timeout.tv_usec = 0;
#ifdef HPUX
      if( select(pipe_sockets[transp->xp_sock] + 1, (int *)&pipe_fd, NULL,NULL,
                &timeout) == 1 )
#else
      if( select(pipe_sockets[transp->xp_sock] + 1, &pipe_fd, NULL,NULL,
                &timeout) == 1 )
#endif
      {
        read(pipe_sockets[transp->xp_sock], (char *)&port, 2);
        close(pipe_sockets[transp->xp_sock]);
        pipe_sockets[transp->xp_sock] = 0;
      }
      else
      {
        port = PORTNUM;
      }
#endif
      cliports[transp->xp_sock] = port;
      if(port == PORTNUM)
      {
        port = 0;
      }
      else
      {
        cliprognums[transp->xp_sock] = 0;
        cliports[transp->xp_sock] = 0;
      }
#ifdef AIX
      svc_sendreply(transp,xdr_short,(caddr_t)&port);
#else
      svc_sendreply(transp,xdr_short,&port);
#endif
      break;

    case CLOSE_SERVER:
      svc_sendreply(transp, xdr_void, NULL);
      svc_destroy(Transp);
      break;

    default:
      svcerr_noproc(transp);
      return;
  }
}

/* ======= */
 
main()
{
  struct sockaddr_in name;
  int    namelen, i, j;
#if defined(ULTRIX) || defined(ALPHA)
  struct sigvec sv;
  bzero((char *)&sv, sizeof(sv));
  sv.sv_mask = 0;
  sv.sv_handler = reapchild;
  sigvec(SIGCHLD, &sv, (struct sigvec *)0);
#else
#ifdef Darwin
  signal(SIGCHLD, SIG_IGN);
#else
  signal(SIGCLD, SIG_IGN);
#endif
#endif
 
  bzero((char *) cliprognums, 32 * sizeof(unsigned long) );
  bzero((char *) cliports, 32 * sizeof(short) );
  bzero((char *) pipe_sockets, 32 * sizeof(int) );
 
  /* we must use socket() to define an Internet domain socket with AF_INET;
     we are using the SOCK_STREAM transport */

  if((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf(" BOSIOSERVER: Cannot open socket.\n");
    exit(1);
  }

  /* to bind an address to the socket, we must fill the sockaddr_in
     structure, including protocol family, port, and socket selection;
     the use of the INADDR_ANY socket address tells the OS to use any
     user address it likes */

  /* SunOS, /usr/include/netinet/in.h:

   Internet address
        This definition contains obsolete fields for compatibility
        with SunOS 3.x and 4.2bsd.  The presence of subnets renders
        divisions into fixed fields misleading at best.  New code
        should use only the s_addr field.

struct in_addr {
        union {
                struct { u_char s_b1, s_b2, s_b3, s_b4; } S_un_b;
                struct { u_short s_w1, s_w2; } S_un_w;
                u_long S_addr;
        } S_un;
#define	s_addr	S_un.S_addr


   Socket address, internet style.

struct sockaddr_in {
        short   sin_family;
        u_short sin_port;
        struct  in_addr sin_addr;
        char    sin_zero[8];
};

*/

  /* complete the socket structure */

  namelen = sizeof(name);
  bzero((char *)&name,namelen);
  name.sin_family =      AF_INET;
  name.sin_port =        htons(PORTNUM); /* put in net order */
  name.sin_addr.s_addr = INADDR_ANY;
  /*
  cln = gethostbyname(...) and then
  memcpy((char *)&(name.sin_addr.s_addr),(char *)cln->h_addr_list[0],
	     (size_t)cln->h_length);
  */

  /* bind the socket to the port number */

  if(bind(s, (struct sockaddr *)&name, namelen) < 0)
  {
    printf(" BOSIOSERVER: Cannot bind the socket.\n");
    close(s);
    exit(1);
  }

/* fragment from "man svctcp_create" on SunOS:

  File Descriptors
     Transport independent RPC uses TLI as its  transport  inter-
     face instead of sockets.

     Some of the routines described  in  this  section  (such  as
     clnttcp_create()) take a pointer to a file descriptor as one
     of the parameters.  If the user wants the file descriptor to
     be  a  socket,  then  the application will have to be linked
     with  both  librpcsoc  and  libnsl.   If  the  user   passed
     RPC_ANYSOCK  as  the file descriptor, and the application is
     linked with libnsl only, then the routine will return a  TLI
     file descriptor and not a socket.

 BUT THERE IS NO librpcsoc on SunOS !!! - Serguei */

  if((Transp = svctcp_create(s,NULL,NULL)) == NULL) /* working if RPC_ANYSOCK instead s */
  {
    printf(" BOSIOSERVER: Cannot create an RPC server\n");
    close(s);
    exit(1);
  }
  else
  {
/*    
printf("transport's file descriptor==%d transport's port=%d\n",
            Transp->xp_fd,Transp->xp_port);
*/
  }

  if(!svc_register(Transp, PROGNUM, VERSNUM, dispatch, 0))
  {
    printf("BOSIOSERVER: Cannot register RPC service\n");
    svc_destroy(Transp);
    exit(1);
  }

  printf("BOSIOSERVER: listening...\n");
  fflush(stdout);
  /* Fork the daemon and return to the user. setpgrp() to disconnect
     the daemon from the user's control terminal. */
  setpgrp();
  switch(fork())
  {
    case -1:
      perror("fork");
      printf("BOSIOSERVER:Unable to fork daemon.\n");
      exit(1);
    case 0:
      /* the daemon comes here */
      freopen("bosioserver.log","a",stdout);
      time(&timevar);
      printf("\nBOSIOSERVER started at %s",ctime(&timevar));
#ifdef AIX
      close((int)stdin);
      close((int)stderr);
#else
      close(stdin);
      close(stderr);
#endif
      svc_runS(Transp);       /* never returns */
      printf("BOSIOSERVER: Should never reach this point\n");
      fflush(stdout);
      svc_unregister(PROGNUM,VERSNUM);
      svc_destroy(Transp);
      exit(1);
    default:
      /* parent process */
      exit(0);
  }
}
 
