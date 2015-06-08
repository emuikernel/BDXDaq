h60566
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/openc.c
e
s 00702/00000/00000
d D 1.1 00/08/10 11:10:16 boiarino 1 0
c date and time created 00/08/10 11:10:16 by boiarino
e
u
U
f e 0
t
T
I 1
/*DECK ID>, OPENC. */
 
/* open remote file */
 
#define OPENC
#if defined(IBMMVS) || defined(IBMVM)
#pragma linkage (openc,FORTRAN)
#pragma linkage (delay,FORTRAN)
#endif
 
#include "netincl.h"
 
CLIENT * clntpointer[MAXSERV];
#ifdef MACII
#include <Files.h>
#endif
#include <ctype.h>
#if defined(APOLLO) || defined(ALLIANT)
/* on APOLLO and ALLIANT 'tolower' is a macro, not a function,
   and its behaviour is undefined if the argument is not in
   upper case, hence...                                       */
#ifndef TOLOWER
#define TOLOWER(x) isupper(x) ? tolower(x) : x
#endif
#else
#ifndef TOLOWER
#define TOLOWER(x) tolower(x)
#endif
#endif
 
#define FAILURE  { clptr=NULL; *iclptr=0; *strptr = 0; return; }
 
               /* XDR function for open request
                  ============================= */
int xdr_open(xdrsp, openp)
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
 
 /* XDR function for authentification data and job class
 ======================================================= */
int xdr_auth(xdrsp,authp)
XDR *xdrsp;
struct auth_t *authp;
{if(!xdr_wrapstring(xdrsp,&(authp->User))) return 0;
 if(!xdr_wrapstring(xdrsp,&(authp->Password))) return 0;
 if(!xdr_wrapstring(xdrsp,&(authp->Class))) return 0;
 if(!xdr_wrapstring(xdrsp,&(authp->Time_M))) return 0;
 if(!xdr_wrapstring(xdrsp,&(authp->Time_S))) return 0;
 return 1;
}
 
   /* get authentification data from netrc-file
      ========================================= */
int get_auth(machine, login, password)
char *machine, *login, *password;
 
{FILE *fp;
 char line[81], string[20], *sp;
 int  i,count, length;
 
#ifdef MACII
 FInfo                       Info;
 short                       vRefNum,refNum = 0;
 char                        volName[256];
 
 getvol(volName, &vRefNum); /* get the name of the main volume */
 strcat(volName, ":netrc");
 if(( fp = fopen(volName,"r")) == NULL ) return 1;
       /* make netrc a bit more difficult to read */
 getfinfo(volName,refNum,&Info);
 Info.fdType = (OSType) 'PASS';
 Info.fdCreator = (OSType) '????';
 setfinfo(volName,refNum,&Info);
#endif
#ifdef IBMMVS
 if(( fp = fopen("PER.NETRC","r")) == NULL ) return 1;
#endif
#ifdef IBMVM
 if(( fp = fopen("DOT NETRC *","r")) == NULL ) return 1;
#endif
#ifdef UNIX
 sp = (char *)getenv( "HOME" );
 if(sp == NULL)
   {fprintf(stderr, "No HOME env. variable!\n");
    return 1;
   }
 strcpy(line, sp);
 strcat(line,"/.netrc");
 if(( fp = fopen(line,"r")) == NULL ) return 1;
#endif
#ifdef VAXVMS
 if(( fp = fopen("sys$login:netrc","r")) == NULL ) return 1;
#endif
 
 lcase(machine);
 strcpy(string,"machine ");
 while(1)
   {if( fgets(line,80,fp) == NULL )
      {fclose(fp);
       return 2;
      }
    length = strlen(line);
    for(count = 0; count <= length && line[count] == ' '; count++);
    if( count >= length - strlen(string) ) continue;
    for(i = count; i < count + strlen(string); i++)
                                      line[i] = TOLOWER( line[i] );
    if( strncmp( &line[count], string, strlen(string) ) ) continue;
    for( i = count + strlen(string);
                    i <= length && line[i] == ' '; i++ );
    if( i >= length - strlen(machine) ) continue;
    for( count = i; count < i + strlen(machine); count++)
                                   line[count] = TOLOWER( line[count] );
    if( strncmp(&line[i], machine, strlen(machine) ) ) continue;
    count = i + strlen(machine);
    if( line[count] != ' ' && line[count] != '\n' && line[count] != '\0')
       continue;
    else
       {count++;
        break;
       }
   }
 
 strcpy(string,"login ");
 while(1)
   {for(; count <= length && line[count] == ' '; count++);
    if( count >= length - 1 )
      if( fgets(line,80,fp) == NULL )
        {fclose(fp);
         return 3;
        }
      else
        {count = 0;
         length = strlen(line);
         continue;
        }
    if( count >= length - strlen(string) )
      {fclose(fp);
       return 4;
      }
    for(i = count; i < count + strlen(string); i++)
              line[i] = TOLOWER( line[i] );
    if( !strncmp(&line[count], string, strlen(string)) )
      {for( i = count + strlen(string);
            i <= length && line[i] ==' '; i++ );
       if( i >= length )
         {fclose(fp);
          return 4;
         }
       for( count = i; count <= length; count++)
          if( line[count] == ' ')
            {line[count] = '\0';
             break;
            }
       strcpy(login,&line[i]);
       if( sp = strchr(login,'\n') ) *sp = '\0' ;
       count++;
       break;
      }
    else
      {fclose(fp);
       return 4;
      }
   }
 
 strcpy(string,"password ");
 while(1)
   {for(; count <= length && line[count] == ' '; count++);
    if( count >= length - 1 )
      if( fgets(line,80,fp) == NULL )
        {fclose(fp);
         return 5;
        }
      else
        {count = 0;
         length = strlen(line);
         continue;
        }
    if( count >= length - strlen(string) )
      {fclose(fp);
       return 5;
      }
    for(i = count; i < count + strlen(string); i++)
                   line[i] = TOLOWER( line[i] );
    if( !strncmp(&line[count], string, strlen(string)) )
      {for( i = count + strlen(string);
            i <= length && line[i] ==' '; i++ );
       if( i >= length )
         {fclose(fp);
          return 6;
         }
       for( count = i; count <= length; count++)
          if( line[count] == ' ')
            {line[count] = '\0';
             break;
            }
       strcpy(password, &line[i]);
       if(sp = strchr(password,'\n')) *sp = '\0' ;
       fclose(fp);
       return 0;
      }
    else
      {fclose(fp);
       return 6;
      }
   }
}
 
/* open remote file */
 
#if defined(IBMMVS) || defined(IBMVM)
void openc(iclptrp,strptrp,name,serv_host,options,ispc,nr1,nr2)
int      **iclptrp;      /* pointer to
                            index to pointer to client handle      */
int      **strptrp;      /* pointer to pointer to stream_id        */
#else
void openc(iclptr,strptr,name,serv_host,options,ispc,nr1,nr2)
#endif
 
#if defined(UNIX)|| defined(VAXVMS)|| defined(MACII)
int      *iclptr;        /* index to pointer to client handle      */
int      *strptr;        /* pointer to stream_id                   */
#endif
char     *name;          /* file name                              */
char     *serv_host;     /* server host name                       */
char     *options;       /* open options: access, recfm, lrecl     */
int      *ispc;          /* pointer to space unit code:1=TRK,2=CYL,
                            anything else='average blksize'        */
int      *nr1;           /* pointer to primary space               */
int      *nr2;           /* pointer to secondary space             */
{
  CLIENT  *clptr;
#if defined(IBMMVS) || defined(IBMVM)
  int     *strptr;
  int     *iclptr;
#endif
  char                        filename[MAXNAMELEN],space1[80],c2[2];
  char                        hostname[MAXNAMELEN],space2[80],c3[2];
#if defined(ULTRIX) || defined(ALPHA) || defined(IBMVM) || defined(IBMMVS)
  char                        space3[80];
  unsigned int                address;
#endif
  char                        openopt[MAXNAMELEN],*sp,*spt,*spr,c1[5];
  int     namelen;
  int     nlen;
  int     optlen;
  struct  hostent            *hp;
  struct  sockaddr_in         server_addr;
  int                         sock,ntry,count,wsk;
  struct  open_t              opend;
  struct  auth_t              authd;
  unsigned int                numprog;/* PROGNUM received from server*/
  short                       port;   /* private port */
  enum    clnt_stat           clnt_stat;
#ifdef MACII
  FInfo                       Info;
  short                       refNum = 0;
  extern void s_shutdown(void);
  static int sflag = 1;
#endif
/* set timeout for RPC requests */
static struct timeval total_timeout = {120,5};
 
#ifdef MACII
  /* install "at exit" routine if not yet installed */
  if(sflag)
    if(atexit(s_shutdown))
      fprintf(stderr," Cannot install the s_shutdown exit routine\n");
  sflag = 0;
#endif
#if defined(MACII) || defined(VAXVMS)
  /* redirect stderr to fpacknet.log if not yet done */
  if( log == (FILE *)NULL )
  {
    log = freopen("fpacknet.log", "w", stderr);
#endif
#ifdef MACII
    /* set file attributes to make it visible to the Finder
       refNum=0 means in this context the current directory */
    getfinfo("fpacknet.log",refNum,&Info);
    Info.fdType = (OSType) 'TEXT';
    Info.fdCreator = (OSType) '????';
    setfinfo("fpacknet.log",refNum,&Info);
#endif
#if defined(MACII) || defined(VAXVMS)
    fprintf(log, "         OUTPUT FROM F-PACK NETWORK ROUTINES:\n\n ");
    fflush(log);
  }
#endif
#if defined(IBMMVS) || defined(IBMVM)
  strptr = *strptrp;
  iclptr = *iclptrp;
#endif
 
  /* set default values to job class and time for MVS */
 
  authd.Class  = c1;
  authd.Time_M = c2;
  authd.Time_S = c3;
  strcpy(authd.Class, "E");
  strcpy(authd.Time_M,"0");
  strcpy(authd.Time_S,"0");
 
  namelen = strlen(name);
  nlen = strlen(serv_host);
  optlen = strlen(options);
 
  /* analyse string parameters */
  if( namelen >= MAXNAMELEN )
  {
    fprintf(stderr,"Filename too long\n");
    return;
  }
  strncpy(filename,name,namelen);
  filename[namelen] = '\0';
 
  if( nlen >= MAXNAMELEN )
  {
    fprintf(stderr,"Server hostname too long \n");
    return;
  }
  strncpy(hostname,serv_host,nlen);
  hostname[nlen] = '\0';
 
  if(sp = strchr(hostname,'$'))
  {/* extract job class and time for MVS */
    *sp = '\0';
    ++sp;
    if( (*sp < '0')||(*sp > '9') ) /* job class given */
    {
      authd.Class = sp;
      if(spr = strchr(sp,'$'))         /* time given      */
      {
        *spr = '\0';
        authd.Time_M = ++spr;
        if( !(spt = strchr(spr,'.')) )
        {
          fprintf(stderr,"Wrong structure of Time parameter\n");
          FAILURE
        }
        else
        {
          *spt = '\0';
          authd.Time_S = ++spt;
        }
      }
      else           /* only job class given */
        *(++sp) = '\0';
    }
    else                           /* only time given      */
    {
      authd.Time_M = sp;
      if( !(spt=strchr(sp,'.')) )
      {
        fprintf(stderr,"Wrong structure of Time parameter\n");
        FAILURE
      }
      else
      {
        *spt = '\0';
        authd.Time_S = ++spt;
      }
    }
  }
  if( optlen >= MAXNAMELEN )
  {
    fprintf(stderr,"Option string too long\n");
    FAILURE
  }
  strncpy(openopt,options,optlen);
  openopt[optlen] = '\0';
 
  /* extract the SHR option (and possibly some other options)
  if( sp = strstr(filename,"::") )
  {
    strcat(openopt, sp);
    *sp = '\0';
  }                 */
  opend.name = filename;
  opend.options = openopt;
  opend.ispc = *ispc;
  opend.nr1 = *nr1;
  opend.nr2 = *nr2;
  opend.keyunit = *strptr;
 
  if(*iclptr == 0)
  {
 
   /* Connect to permanent server and start server job */
 
    if(sp = strchr(filename,'@'))
    {
      *sp = '\0';
      authd.User = ++sp;
      if(spt =strchr(sp,'@'))
      {
        *spt = '\0';
        authd.Password = ++spt;
      }
      else
      {
        fprintf(stderr," password missing in filename \n");
        FAILURE
      }
    }
    else
    {
      authd.User = space1;
      authd.Password = space2;
      if( wsk=get_auth(hostname,authd.User,authd.Password) )
      {
        fprintf(stderr," no userid given in filename \n");
        switch(wsk)
        {
          case 1: fprintf(stderr,"cannot open netrc-file\n");
                  break;
          case 2: fprintf(stderr,"no entry for %s in netrc-file\n",hostname);
                  break;
          case 3: fprintf(stderr,"no entry for userid in netrc-file\n");
                  break;
          case 4: fprintf(stderr,"invalid entry for userid in netrc-file\n");
                  break;
          case 5: fprintf(stderr,"no entry for passwd in netrc-file\n");
                  break;
          case 6: fprintf(stderr,"invalid entry for passwd in netrc-file\n");
                  break;
          default:break;
        }
        FAILURE
      }
      /* strip trailing blanks */
      for(count = strlen(space1)-1; ;count--)
        if(space1[count] == ' ' && count >=0) space1[count] = '\0';
      else
        break;
      for(count = strlen(space2)-1; ;count--)
        if(space2[count] == ' ' && count >=0) space2[count] = '\0';
      else
        break;
    }
 
     /* Get the internet address of the server host */
 
    if( !strcmp(hostname,"apollo") || !strcmp(hostname,"APOLLO") )
                              strcpy(hostname,APOSERV);
    sock = RPC_ANYSOCK;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(MAINPORT);
#if defined(ULTRIX) || defined(ALPHA) || defined(IBMVM) || defined(IBMMVS)
    /* DEC and IBM implementation of gethostbyname() does not treat
       dotted IP addresses properly. Hence, these addresses are
       analysed here and the address is resolved from the dotted
       notation and put into the structure as an explicit
       unsigned integer. */
 
    for(wsk = 0, sp = hostname; *sp != '\0'; sp++)
      if( (*sp < '0' || *sp > '9') && *sp != '.' ) wsk = 1;
 
    if(wsk)
    {/* not a dotted IP address */
#endif
      hp=gethostbyname(hostname);
      if(hp == NULL)
      {
        fprintf(stderr,"can't get addr for %s \n",hostname);
        FAILURE
      }
      bcopy(hp->h_addr, (caddr_t)&server_addr.sin_addr,hp->h_length);
#if defined(ULTRIX) || defined(ALPHA) || defined(IBMVM) || defined(IBMMVS)
    }
    else
    {/* dotted IP address */
      count = 0;
      strcpy(space3,hostname);
      sp = strtok(space3,".");
      if(sp == NULL)
      {
        fprintf(stderr,"no dot found in the IP address \n");
        FAILURE
      }
      do
      {
        *((char *)&address + count) = strtol(sp, &sp, 10);
        count++;
      }
      while((sp = strtok(NULL,".")) != NULL && count < 4);
      server_addr.sin_addr.s_addr = address;
    }
#endif
    /* Open TCP connection to permanent server */
 
    for(count = 0;; count++)
    {
      clptr = clnttcp_create(&server_addr,PERMNUM,1,&sock,NULL,NULL);
      if(clptr == NULL)
      {
        if(count < 3)
        {
#ifdef IBMMVS
          delay(83000);    /* or call a local equivalent */
#endif
#ifdef IBMVM
          system("CP SLEEP 2 SEC");
#endif
#if defined(UNIX) || defined(VAXVMS) || defined(MACII)
          printf("CP SLEEP 2 SEC\n");
          sleep(2);
#endif
          sock = RPC_ANYSOCK;
          continue;
        }
        fprintf(stderr,"FPSERVER on %s not responding.\n",hostname);
        FAILURE
      }
      else
      {
        fprintf(stderr,"\nConnected to FPSERVER on %s\n",hostname);
        break;
      }
    }
 
    /* Request private server to be started    */
 
    clnt_stat=clnt_call(clptr, SUBMIT, xdr_auth, &authd,
                       xdr_u_long, &numprog, total_timeout);
    if(clnt_stat != RPC_SUCCESS)
    {
      fprintf(stderr,"  clnt_call for SUBMIT failed\n");
      clnt_perrno(clnt_stat);
      perror(" RPC");
#ifdef MACII
      s_close(sock);
#else
      close(sock);
#endif
      clnt_destroy(clptr);
      FAILURE
    }
    if(numprog == 0)
    {
      fprintf(stderr,"Cannot submit the server job.\n");
#ifdef MACII
      s_close(sock);
#else
      close(sock);
#endif
      clnt_destroy(clptr);
      FAILURE
    }
 
    if( numprog != PERMNUM )
      {      /* host is not MacII,                      */
             /* open a TCP connection to private server */
 
       switch(*authd.Class)
         {case 'A': ntry = 75;
                    break;
          default : ntry = 25;
                    break;
         }
 
       count = 0;
       port=0;
       while(port == 0)          /* get port number */
       {
         ++count;
         clnt_stat=clnt_call(clptr,CHECK_PORT,xdr_u_long,&numprog,
                                xdr_short,&port,total_timeout);
         if(clnt_stat != RPC_SUCCESS)
         {
           fprintf(stderr," clnt_call for CHECK_PORT failed\n");
           clnt_perrno(clnt_stat);
           perror(" RPC");
#ifdef MACII
           s_close(sock);
#else
           close(sock);
#endif
           clnt_destroy(clptr);
           FAILURE
         }
         if(port) break;
         if(count > ntry)
         {
           fprintf(stderr," Can't connect the private server.\n");
#ifdef MACII
           s_close(sock);
#else
           close(sock);
#endif
           clnt_destroy(clptr);
           FAILURE
         }
         else
#ifdef IBMMVS
           delay(83000);
#endif
#ifdef IBMVM
           system("CP SLEEP 2 SEC");
#endif
#if defined(UNIX) || defined(VAXVMS) || defined(MACII)
           sleep(2);
#endif
         }
        /* Some RPC implementations (e.g. on IBM MVS) do not close
         the socket within clnt_destroy(). This can lead to crashes
         of the server! Hence... */
#ifdef MACII
         s_close(sock);
#else
         close(sock);
#endif
         clnt_destroy(clptr);  /* Close connection to permanent server */
 
         server_addr.sin_port=htons(port);
         sock = RPC_ANYSOCK;
         clptr = clnttcp_create(&server_addr,numprog,VERSNUM,&sock,
                                   SENDSIZE,RECESIZE);
         if(clptr == NULL)
         {
           fprintf(stderr,"private server on %s not responding.\n",hostname);
           FAILURE
         }
         else
           fprintf(stderr,"Connected to private server after %d trials.\n",
                                                                   count);
       }
 
      /* by now we have a client pointer stored in clptr;
       find a slot for it in clntpointer[]. */
 
       for( *iclptr = 1; clntpointer[*iclptr] != NULL; (*iclptr)++ );
       if( *iclptr >= MAXSERV ) FAILURE
       clntpointer[ *iclptr ] = clptr;
     }
     else /* *iclptr != 0 */
       clptr = clntpointer[*iclptr];
 
  /* open file */
 
  if(*strptr < 0) total_timeout.tv_sec = 1200;	/* remote stage requested */
 
  for(;;)
  {
    clnt_stat=clnt_call(clptr, OPEN, xdr_open, &opend,xdr_int, strptr,
                        total_timeout);
    if(clnt_stat != RPC_SUCCESS)
    {
      fprintf(stderr,"  clnt_call for OPEN failed\n");
      clnt_perrno(clnt_stat);
      perror(" RPC");
#ifdef MACII
      s_close( *((int *)((clptr)->cl_private)) );
#else
      close( *((int *)((clptr)->cl_private)) );
#endif
      clnt_destroy(clptr);
      FAILURE;
    }
    if(*strptr == 0)
    {
      fprintf(stderr,"SERVER: Open failed for %s \n",filename);
      return;
    }
    if(*strptr == -13)
    {
            /* This is specific for connections to servers running under
             IBM/MVS/VM. -13 means there is already a stream opened to
             the file in the update mode. In that case no other stream
             can be opened to the file from the same process. The
             behaviour is undefined. The data base e.g. typically gets
             corrupted. Just another pecularity of IBM mainframe
             systems ...  */
      fprintf(stderr,"openc: file %s cannot be opened for update \n",
                                     filename);
      fprintf(stderr,"       or is being currently updated. Wait 10s\n");
      fflush(stderr);
#ifdef IBMMVS
      delay(415000);
#endif
#ifdef IBMVM
      system("CP SLEEP 10 SEC");
#endif
#if defined(UNIX) || defined(VAXVMS) || defined(MACII)
      sleep(10);
#endif
      continue;
    }
    else
    {
      return;
    }
  }
}
 
E 1
