/*----------------------------------------------------------------------------*
 *  Copyright (c) 1998        Southeastern Universities Research Association, *
 *                            Thomas Jefferson National Accelerator Facility  *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 *    Author:  Carl Timmer                                                    *
 *             timmer@jlab.org                   Jefferson Lab, MS-12H        *
 *             Phone: (757) 269-5130             12000 Jefferson Ave.         *
 *             Fax:   (757) 269-5800             Newport News, VA 23606       *
 *                                                                            *
 *----------------------------------------------------------------------------*
 *
 * Description:
 *      Routines dealing with network communications and byte swapping
 *
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>

#ifdef VXWORKS

#include <vxWorks.h>
#include <taskLib.h>
#include <sockLib.h>
#include <inetLib.h>
#include <hostLib.h>
#include <ioLib.h>
#include <time.h>
#include <net/uio.h>
#include <net/if_dl.h>

#else

#include <strings.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/utsname.h>
#ifdef __APPLE__
#include <ifaddrs.h>
#endif

#endif

#ifdef sun
#include <sys/filio.h>
#endif
#include "et_network.h"

#ifdef VXWORKS

static struct hostent h;
static struct sockaddr_in sin;
static char   *addrlist[2];
static char   *aliaslist[1];

struct hostent *gethostbyname(const char *hostName) 
{
  int host;
  if ((host = hostGetByName((char *)hostName))==ERROR) return(NULL);
  h.h_name = (char *)hostName;
  h.h_addr_list = addrlist;
  addrlist[0] = (char *) &sin.sin_addr;
  h.h_length = sizeof(sin.sin_addr);
  bzero ((char *)&sin, sizeof(sin));
  sin.sin_addr.s_addr = host;
  /* no aliases */
  aliaslist[0] = NULL;
  h.h_aliases = aliaslist;
  return (&h);
}


/** Implementation of strdup for vxWorks. */
char *strdup(const char *s1) {
    char *s;    
    if (s1 == NULL) return NULL;    
    if ((s = (char *) malloc(strlen(s1)+1)) == NULL) return NULL;    
    return strcpy(s, s1);
}

#endif


/*****************************************************/
static char *sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
  int debug = 1;
  static char str[128]; /* Unix domain is largest */

  switch (sa->sa_family) {
    case AF_INET: {
      struct sockaddr_in  *sin = (struct sockaddr_in *) sa;
#ifdef VXWORKS
      if (inet_ntoa(sin->sin_addr) == NULL) {
        if (debug) fprintf(stderr, "sock_ntop_host: error calling inet_ntoa\n");
        return(NULL);
      }
#else
      if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL) {
        if (debug) fprintf(stderr, "sock_ntop_host: %s\n", strerror(errno));
        return(NULL);
      }
#endif
      return(str);
    }

#if defined IPV6 && !defined VXWORKS
    case AF_INET6: {
      struct sockaddr_in6  *sin6 = (struct sockaddr_in6 *) sa;

      if (inet_ntop(AF_INET6, &sin6->sin6_addr, str, sizeof(str)) == NULL) {
        if (debug) fprintf(stderr, "sock_ntop_host: %s\n", strerror(errno));
        return(NULL);
      }
      return(str);
    }
#endif

    default:
      if (debug) fprintf(stderr, "sock_ntop_host: unknown AF_xxx: %d, len %d", sa->sa_family, salen);
  }
  return (NULL);
}

/*****************************************************/
/* 64 bit swap routine */
uint64_t et_ntoh64(uint64_t n) {
        uint64_t h;
        uint64_t tmp = ntohl(n & 0x00000000ffffffff);
        h = ntohl(n >> 32);
        h |= tmp << 32;
        return h;
}

/*****************************************************/
int et_tcp_listen(unsigned short port)
{
  int                 listenfd, err;
  const int           debug=0, on=1, size=ET_SOCKBUFSIZE /* bytes */;
  struct sockaddr_in  servaddr;

  err = listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (err < 0) {
    if (debug) fprintf(stderr, "et_tcp_listen: socket error\n");
    return err;
  }

  bzero((void *)&servaddr, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(port);

  err = setsockopt(listenfd, IPPROTO_TCP, TCP_NODELAY, (void *) &on, sizeof(on));
  if (err < 0) {
    close(listenfd);
    if (debug) fprintf(stderr, "et_tcp_listen: setsockopt error\n");
    return err;
  }
  err = setsockopt(listenfd, SOL_SOCKET, SO_SNDBUF, (void *) &size, sizeof(size));
  if (err < 0) {
    close(listenfd);
    if (debug) fprintf(stderr, "et_tcp_listen: setsockopt error\n");
    return err;
  }
  err = setsockopt(listenfd, SOL_SOCKET, SO_RCVBUF, (void *) &size, sizeof(size));
  if (err < 0) {
    close(listenfd);
    if (debug) fprintf(stderr, "et_tcp_listen: setsockopt error\n");
    return err;
  }

  err = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (void *) &on, sizeof(on));
  if (err < 0) {
    close(listenfd);
    if (debug) fprintf(stderr, "et_tcp_listen: setsockopt error\n");
    return err;
  }

  err = setsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE, (void *) &on, sizeof(on));
  if (err < 0) {
    close(listenfd);
    if (debug) fprintf(stderr, "et_tcp_listen: setsockopt error\n");
    return err;
  }

  err = bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
  if (err < 0) {
    close(listenfd);
    if (debug) fprintf(stderr, "et_tcp_listen: bind error\n");
    return err;
  }

  err = listen(listenfd, LISTENQ);
  if (err < 0) {
    close(listenfd);
    if (debug) fprintf(stderr, "et_tcp_listen: listen error\n");
    return err;
  }

  return(listenfd);
}

/*****************************************************/
int et_tcp_connect(const char *ip_address, unsigned short port)
{
  int                 sockfd, err;
  const int           debug=1, on=1, size=ET_SOCKBUFSIZE /* bytes */;
  struct sockaddr_in  servaddr;
#ifndef VXWORKS
  struct in_addr      **pptr;
  struct hostent      *hp;
#endif

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
     if (debug) fprintf(stderr, "et_tcp_connect: socket error, %s\n", strerror(errno));
     return -1;
  }

  err = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (void *) &on, sizeof(on));
  if (err < 0) {
    close(sockfd);
    if (debug) fprintf(stderr, "et_tcp_connect: setsockopt error\n");
    return err;
  }
  err = setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (void *) &size, sizeof(size));
  if (err < 0) {
    close(sockfd);
    if (debug) fprintf(stderr, "et_tcp_connect: setsockopt error\n");
    return err;
  }
  err = setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (void *) &size, sizeof(size));
  if (err < 0) {
    close(sockfd);
    if (debug) fprintf(stderr, "et_tcp_connect: setsockopt error\n");
    return err;
  }

  bzero((void *)&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port   = htons(port);

#ifdef VXWORKS

  servaddr.sin_addr.s_addr = hostGetByName((char *)ip_address);
  if (servaddr.sin_addr.s_addr == ERROR) {
    fprintf(stderr, "et_tcp_connect: unknown server address for host %s\n",ip_address);
    close(sockfd);
    return ERROR;
  }

  if (connect(sockfd,(struct sockaddr *) &servaddr, sizeof(servaddr)) == ERROR) {
    fprintf(stderr, "et_tcp_connect: error in connect\n");
    close(sockfd);
    return ERROR;
  }    

#else
/* BUGBUG gethostbyname in not reentrant, need to mutex protect it. */
  if ((hp = gethostbyname(ip_address)) == NULL) {
    close(sockfd);
    if (debug) fprintf(stderr, "et_tcp_connect: hostname error - %s\n", et_hstrerror(h_errno));
    return -1;
  }
  pptr = (struct in_addr **) hp->h_addr_list;

  for ( ; *pptr != NULL; pptr++) {
    memcpy(&servaddr.sin_addr, *pptr, sizeof(struct in_addr));
    if ((err = connect(sockfd, (SA *) &servaddr, sizeof(servaddr))) < 0) {
      if (debug) fprintf(stderr, "et_tcp_connect: error attempting to connect to server\n");
    }
    else {
      /*printf("et_tcp_connect: connected to server\n"); */
      break;
    }
  }
#endif  


/* for debugging, print out our port */
/*
{
  struct sockaddr_in localaddr;
  socklen_t          addrlen, len;
  unsigned short  portt;
  
  addrlen = sizeof(localaddr);
  getsockname(sockfd, (SA *) &localaddr, &addrlen);
  portt = ntohs(localaddr.sin_port);
  printf("My Port is %hu\n", portt);
}
*/
  
  if (err == -1) {
    close(sockfd);
    if (debug) fprintf(stderr, "et_tcp_connect: socket connect error, %s\n", strerror(errno));
    return -1;
  }
  
  return sockfd;
}

/*****************************************************/
int et_tcp_connect2(uint32_t inetaddr, unsigned short port)
{
  int                 sockfd, err;
  const int           debug=1, on=1, size=ET_SOCKBUFSIZE /* bytes */;
  struct sockaddr_in  servaddr;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
     if (debug) fprintf(stderr, "et_tcp_connect2: socket error, %s\n", strerror(errno));
     return -1;
  }

  err = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (void *) &on, sizeof(on));
  if (err < 0) {
    close(sockfd);
    if (debug) fprintf(stderr, "et_tcp_connect2: setsockopt error\n");
    return err;
  }
  err = setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (void *) &size, sizeof(size));
  if (err < 0) {
    close(sockfd);
    if (debug) fprintf(stderr, "et_tcp_connect2: setsockopt error\n");
    return err;
  }
  err = setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (void *) &size, sizeof(size));
  if (err < 0) {
    close(sockfd);
    if (debug) fprintf(stderr, "et_tcp_connect2: setsockopt error\n");
    return err;
  }

  bzero((void *)&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port   = htons(port);
  servaddr.sin_addr.s_addr = inetaddr;

#ifdef VXWORKS
  if (connect(sockfd,(struct sockaddr *) &servaddr, sizeof(servaddr)) == ERROR) {
    fprintf(stderr, "et_tcp_connect2: error attempting to connect to server\n");
    close(sockfd);
    return ERROR;
  }    
#else
  if ((err = connect(sockfd, (SA *) &servaddr, sizeof(servaddr))) < 0) {
    if (debug) fprintf(stderr, "et_tcp_connect2: error attempting to connect to server\n");
  }
#endif  
  
  if (err == -1) {
    close(sockfd);
    if (debug) fprintf(stderr, "et_tcp_connect2: socket connect error, %s\n", strerror(errno));
    return -1;
  }
  
  return sockfd;
}

/************************************************************/
int et_accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
  int  n;

again:
  if ((n = accept(fd, sa, salenptr)) < 0) {
#ifdef EPROTO
    if (errno == EPROTO || errno == ECONNABORTED)
#else
    if (errno == ECONNABORTED)
#endif
      goto again;
    else
      fprintf(stderr, "et_accept: error, errno = %d\n", errno);
  }
  return(n);
}

/*****************************************************/
ssize_t et_tcp_writev(int fd, struct iovec iov[], int nbufs, int iov_max)
{
  struct iovec *iovp;
  int n_write, n_sent, i;
  ssize_t cc, nbytes;
  
  /* determine total # of bytes to be sent */
  nbytes = 0;
  for (i=0; i < nbufs; i++) {
    nbytes += iov[i].iov_len;
  }
  
  n_sent = 0;
  while (n_sent < nbufs) {  
    n_write = ((nbufs - n_sent) >= iov_max)? iov_max : (nbufs - n_sent);
    iovp     = &iov[n_sent];
    n_sent  += n_write;
      
   retry:
    if ( (cc = writev(fd, iovp, n_write)) == -1) {
      if (errno == EWOULDBLOCK) {
        goto retry;
      }
      fprintf(stderr,"et_tcp_writev(%d,,%d) = writev(%d,,%d) = %d\n",
              fd,nbufs,fd,n_write,(int)cc);
      perror("et_tcp_writev");
      return(-1);
    }
  }
  return(nbytes);
}

/*****************************************************/
ssize_t et_tcp_write(int fd, const void *vptr, size_t n)
{
  size_t   nleft;
  ssize_t  nwritten;
  char     *ptr;

  ptr = (char *) vptr;
  nleft = n;
  
  while (nleft > 0) {
    if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
      if (errno == EINTR) {
        nwritten = 0;        /* and call write() again */
      }
      else {
        return(-1);        /* error */
      }
    }

    nleft -= nwritten;
    ptr   += nwritten;
  }
  return((ssize_t)n);
}

/*****************************************************/
ssize_t et_tcp_read(int fd, void *vptr, size_t n)
{
    size_t  nleft;
    ssize_t nread;
    char    *ptr;

    ptr = (char *) vptr;
    nleft = n;
  
    while (nleft > 0) {
        if ( (nread = read(fd, ptr, nleft)) < 0) {
          /*
            if (errno == EINTR)            fprintf(stderr, "call interrupted\n");
            else if (errno == EAGAIN)      fprintf(stderr, "non-blocking return, or socket timeout\n");
            else if (errno == EWOULDBLOCK) fprintf(stderr, "nonblocking return\n");
            else if (errno == EIO)         fprintf(stderr, "I/O error\n");
            else if (errno == EISDIR)      fprintf(stderr, "fd refers to directory\n");
            else if (errno == EBADF)       fprintf(stderr, "fd not valid or not open for reading\n");
            else if (errno == EINVAL)      fprintf(stderr, "fd not suitable for reading\n");
            else if (errno == EFAULT)      fprintf(stderr, "buffer is outside address space\n");
            else {perror("cMsgTcpRead");}
          */
            if (errno == EINTR) {
                nread = 0;        /* and call read() again */
            }
            else {
                return(-1);
            }
        } else if (nread == 0) {
            break;            /* EOF */
        }
    
        nleft -= nread;
        ptr   += nread;
    }
    return((ssize_t) (n - nleft));        /* return >= 0 */
}
/*****************************************************/
/**
 * Read 3 ints from a nonblocking socket which are returned.
 * If they cannot be read within .1 sec, forget about it
 * since the client is probably not a valide one. Takes
 * care of converting network to host byte order.
 * Return of -1 is error, 0 is OK.
 */
int et_tcp_read_3i_NB(int fd, int *i1, int *i2, int *i3)
{
    const int numBytes = 12;
    size_t  nleft = numBytes;
    ssize_t nread;
    char    buf[numBytes]; /* byte buffer */
    int     bufIndex = 0, loops = 10;
    struct timespec wait = {0, 10000000}; /* .01 sec */

    
    while (nleft > 0 && loops-- > 0) {

        if ( (nread = read(fd, &buf[bufIndex], nleft)) < 0) {
           
            if (errno != EWOULDBLOCK) {
                return -1;   /* error reading */
            }
            
            /* try again after a .01 sec delay */
            nanosleep(&wait,NULL);
            continue;
            
        } else if (nread == 0) {
            return -1;       /* EOF, but not done reading yet */
        }
    
        nleft -= nread;
        bufIndex += nread;
    }

    /* if we timed out ... */
    if (nleft > 0) {
        return -1;
    }

    /* This takes care of converting network byte order */
    if (i1 != NULL) *i1 = ((buf[0]<<24) | (buf[1]<<16) | (buf[2]<<8)  | buf[3]);
    if (i2 != NULL) *i2 = ((buf[4]<<24) | (buf[5]<<16) | (buf[6]<<8)  | buf[7]);
    if (i3 != NULL) *i3 = ((buf[8]<<24) | (buf[9]<<16) | (buf[10]<<8) | buf[11]);
    
    return 0;
}

/*****************************************************/
int et_byteorder(void)
{
  union {
    short  s;
    char   c[sizeof(short)];
  } un;

  un.s = 0x0102;
  if (sizeof(short) == 2) {
    if (un.c[0] == 1 && un.c[1] == 2) {
      return ET_ENDIAN_BIG;
    }
    else if (un.c[0] == 2 && un.c[1] == 1) {
      return ET_ENDIAN_LITTLE;
    }
    else {
      fprintf(stderr, "et_byteorder: unknown endian\n");
      return ET_ERROR;
    }
  }
  else {
    fprintf(stderr, "et_byteorder: sizeof(short) = %d\n", (int) sizeof(short));
    return ET_ERROR;
  }
}

/*****************************************************/
const char *et_hstrerror(int err)
{
  if (err == 0)
    return("no error");

  else if (err == HOST_NOT_FOUND)
    return("Unknown host");

  else if (err == TRY_AGAIN)
    return("Hostname lookup failure");

  else if (err == NO_RECOVERY)
    return("Unknown server error");

  else if (err == NO_DATA)
    return("No address associated with name");

  return("unknown error");
}

/******************************************************
 * Find out if node1 and node2 are the same machine.
 * 
 * Comparison of names can be risky as there can be more than one name
 * for a domain (as in cebaf.gov & jlab.org) or more that one name
 * for any host. Do a complete comparison by comparing the binary
 * address values.
 ******************************************************/
int et_nodesame(const char *node1, const char *node2)
{
#ifdef VXWORKS
  int nodeAddr1, nodeAddr2;

  if ((node1 == NULL) || (node2 == NULL)) {
    fprintf(stderr, "et_nodesame: bad argument\n");
    return ET_ERROR;
  }
    
  /* do a quick check of name against name, it may work */
  if (strcmp(node1, node2) == 0) return ET_NODE_SAME;

  nodeAddr1 = hostGetByName((char *)node1);
  if (nodeAddr1 == ERROR)
    return ET_ERROR;

  nodeAddr2 = hostGetByName((char *)node2);
  if (nodeAddr2 == ERROR)
    return ET_ERROR;

  if (nodeAddr1 == nodeAddr2)
    return ET_NODE_SAME;
  else
    return ET_NODE_DIFF;

#else
  struct hostent *hptr;
  char **pptr;
  /* save up to ET_MAXADDRESSES ip addresses for each node */
  struct in_addr   node1addrs[ET_MAXADDRESSES], node2addrs[ET_MAXADDRESSES];
  int              n1addrs=0, n2addrs=0, i, j;
  
  if ((node1 == NULL) || (node2 == NULL)) {
    fprintf(stderr, "et_nodesame: bad argument\n");
    return ET_ERROR;
  }
  
  /* do a quick check of name against name, it may work */
  if (strcmp(node1, node2) == 0) return ET_NODE_SAME;
    
  /* Since gethostbyname uses static data and gethostbyname_r
   * is buggy on linux, do things the hard way and save
   * the results into arrays so we don't overwrite data.
   */
   
  if ( (hptr = gethostbyname(node1)) == NULL) {
    fprintf(stderr, "et_nodesame: gethostbyname error\n");
    return ET_ERROR;
  }
  for (pptr = hptr->h_addr_list; *pptr != NULL; pptr++) {
    node1addrs[n1addrs++] = *((struct in_addr *) *pptr);
    if (n1addrs > ET_MAXADDRESSES-1) break;
  } 

  if ( (hptr = gethostbyname(node2)) == NULL) {
    fprintf(stderr, "et_nodesame: gethostbyname error\n");
    return ET_ERROR;
  }
  for (pptr = hptr->h_addr_list; *pptr != NULL; pptr++) {
    node2addrs[n2addrs++] = *((struct in_addr *) *pptr);
    if (n2addrs > ET_MAXADDRESSES-1) break;
  } 

  /* look through addresses for a match */
  for (i=0; i < n1addrs; i++) {
    for (j=0; j < n2addrs; j++) {
      if (node1addrs[i].s_addr == node2addrs[j].s_addr) {
        return ET_NODE_SAME;
      }
    }
  }
  
  return ET_NODE_DIFF;
#endif
}

/******************************************************
 * Find out if "host" is on the same node that this
 * routine is being executed on and can share mutexes (ET_LOCAL),
 * or is on the same node but cannot share them (ET_LOCAL_NOSHARE),
 * or is not on the same node (ET_REMOTE).
 ******************************************************/
int et_nodelocality(const char *host)
{
#ifdef VXWORKS
  int hostAddr1, hostAddr2;
  char name[ET_MAXHOSTNAMELEN];

  if (host == NULL) {
    fprintf(stderr, "et_nodelocality: bad argument\n");
    return ET_ERROR;
  }

  hostAddr1 = hostGetByName((char *)host);
  if(hostAddr1 == ERROR)
    return ET_ERROR;

  if(gethostname(name,ET_MAXHOSTNAMELEN))
    return ET_ERROR;

  hostAddr2 = hostGetByName(name);
  if(hostAddr2 == ERROR)
    return ET_ERROR;


  if(hostAddr1 == hostAddr2) 
    return ET_LOCAL;
  else
    return ET_REMOTE;

#else
  struct utsname myname;
  int status;
  
  if (host == NULL) {
    fprintf(stderr, "et_nodelocality: bad argument\n");
    return ET_ERROR;
  }

  /* find out the name of the machine we're on */
  if (uname(&myname) < 0) {
    fprintf(stderr, "et_nodelocality: cannot find hostname\n");
    return ET_ERROR;
  }
  
  if ( (status = et_nodesame(host, myname.nodename)) == ET_ERROR) {
    fprintf(stderr, "et_nodelocality: error in et_nodesame\n");
    return ET_ERROR;
  }
  else if (status == ET_NODE_SAME) {
    int shared = et_sharedmutex();
    /* if local operating system can share pthread mutexes ... */
    if (shared == ET_MUTEX_SHARE) {
      return ET_LOCAL;
    }
    else {
      return ET_LOCAL_NOSHARE;
    }
  }
  return ET_REMOTE;
#endif
}

/**************************************************************/
/* Return the host returned by "uname" (on vxworks use gethostname)  */
int et_getUname(char *host, int length)
{
#ifdef VXWORKS
  if (host == NULL || length < 2) {
    fprintf(stderr, "et_getUname: bad argument(s)\n");
    return ET_ERROR;
  }

  if (gethostname(host, length) < 0) return ET_ERROR;
  return ET_OK;

#else
  struct utsname myname;
  
  if (host == NULL || length < 2) {
    fprintf(stderr, "et_getUname: bad argument(s)\n");
    return ET_ERROR;
  }

  /* find out the name of the machine we're on */
  if (uname(&myname) < 0) {
    fprintf(stderr, "et_getUname: cannot find uname\n");
    return ET_ERROR;
  }

  /* return the null-teminated uname */
  strncpy(host, myname.nodename, length);
  host[length-1] = '\0';
  
  return ET_OK;
#endif
}

/**************************************************************/
/* Return the default fully qualified, canonical host name of this host  */
int et_defaulthost(char *host, int length)
{
#ifdef VXWORKS

  return et_getUname(host, length);

#else
  struct utsname myname;
  struct hostent *hptr;
  
  if (host == NULL || length < 2) {
    fprintf(stderr, "et_defaulthost: bad argument(s)\n");
    return ET_ERROR;
  }

  /* find out the name of the machine we're on */
  if (uname(&myname) < 0) {
    if ( (hptr = gethostbyname("localhost")) == NULL) {
      fprintf(stderr, "et_defaulthost: cannot find hostname\n");
      return ET_ERROR;
    }
  }
  if ( (hptr = gethostbyname(myname.nodename)) == NULL) {
    fprintf(stderr, "et_defaulthost: cannot find hostname\n");
    return ET_ERROR;
  }

  /* return the null-teminated canonical name */
  strncpy(host, hptr->h_name, length);
  host[length-1] = '\0';
  
  return ET_OK;
#endif
}

/**************************************************************/
/* Return the default dotted-decimal address of this host  */
int et_defaultaddress(char *address, int length)
{
#ifdef VXWORKS
  char name[ET_MAXHOSTNAMELEN];
  union {
    char ip[4];
    int  ipl;
  } u;

  if (address == NULL) {
    fprintf(stderr, "et_defaultaddress: bad argument\n");
    return ET_ERROR;
  }

  if(gethostname(name,ET_MAXHOSTNAMELEN))
    return ET_ERROR;

  u.ipl = hostGetByName(name);
  if(u.ipl == -1)
    return ET_ERROR;

  sprintf(address,"%d.%d.%d.%d",u.ip[0],u.ip[1],u.ip[2],u.ip[3]);
  
  return ET_OK;

#else
  struct utsname myname;
  struct hostent *hptr;
  char           **pptr, *val;
  
  if (address == NULL || length < 8) {
    fprintf(stderr, "et_defaultaddress: bad argument(s)\n");
    return ET_ERROR;
  }

  /* find out the name of the machine we're on */
  if (uname(&myname) < 0) {
    if ( (hptr = gethostbyname("localhost")) == NULL) {
      fprintf(stderr, "et_defaultaddress: cannot find hostname\n");
      return ET_ERROR;
    }
  }
  
  if ( (hptr = gethostbyname(myname.nodename)) == NULL) {
    fprintf(stderr, "et_defaultaddress: cannot find hostname\n");
    return ET_ERROR;
  }

  /* find address from hostent structure */
  pptr = hptr->h_addr_list;
  val  = inet_ntoa(*((struct in_addr *) *pptr));
  
  /* return the null-teminated dotted-decimal address */
  if (val == NULL) {
    return ET_ERROR;
  }
  strncpy(address, val, length);
  address[length-1] = '\0';
  
  return ET_OK;
#endif
}

/******************************************************
 * The following few routines are taken from R. Stevens book,
 * UNIX Network Programming, Chapter 16. A few changes have
 * been made to simply things and fit them into the ET system.
 * These are used for finding our broadcast addresses and
 * network interface hostnames.
 * Yes, we stole the code.
 ******************************************************/



/******************************************************/
#ifdef __APPLE__
struct ifi_info *et_get_ifi_info(int family, int doaliases)
{
  struct ifi_info    *ifi, *ifilast, *ifihead, **ifipnext;
  int                sockfd, len, lastlen, flags, myflags;
  char               *ptr, *buf, lastname[IFNAMSIZ], *cptr;
  struct ifconf      ifc;
  struct ifreq       *ifr, ifrcopy;
  struct sockaddr_in *sinptr;


  {
      struct ifaddrs *ifaddrsp;

      struct ifaddrs *theifaddrs;

      getifaddrs(&ifaddrsp);

      ifihead = NULL;
      ifi = NULL;
      ifilast=NULL;

      for (theifaddrs = ifaddrsp; theifaddrs->ifa_next != NULL; theifaddrs=theifaddrs->ifa_next) {
          ifr = (struct ifreq *) ptr;

          /* for next one in buffer */
          switch (theifaddrs->ifa_addr->sa_family) {
#ifdef IPV6
              case AF_INET6:
                  len = sizeof(struct sockaddr_in6);
                  break;
#endif
#if !defined linux && !defined __APPLE__
              case AF_LINK:
                  len = sizeof(struct sockaddr_dl);
                  break;
#endif
#if defined __APPLE__
              case AF_LINK:
                  len = sizeof(struct sockaddr);
                  continue;
#endif
              case AF_INET:
                  len = sizeof(struct sockaddr);
                  break;
              default:
                  len = sizeof(struct sockaddr);
                  continue;

          }

          myflags = 0;
          if ( (cptr = strchr(theifaddrs->ifa_name, ':')) != NULL) {
              *cptr = 0;    /* replace colon will null */
          }

          if (strncmp(lastname, theifaddrs->ifa_name, IFNAMSIZ) == 0) {
              if (doaliases == 0) {
                  /* continue;    already processed this interface */
              }

              myflags = IFI_ALIAS;
          }

          memcpy(lastname, theifaddrs->ifa_name, IFNAMSIZ);

          flags = theifaddrs->ifa_flags;

          /* ignore if interface not up */
          if ((flags & IFF_UP) == 0) {
              continue;
          }

          ifilast = ifi;

          ifi = calloc(1, sizeof(struct ifi_info));

          if (ifihead == NULL) ifihead = ifi;

          if (ifilast != NULL)
              ifilast->ifi_next = ifi;

          ifi->ifi_flags = flags;    /* IFF_xxx values */
          ifi->ifi_myflags = myflags;    /* IFI_xxx values */
          memcpy(ifi->ifi_name, theifaddrs->ifa_name, IFI_NAME);

          ifi->ifi_name[IFI_NAME-1] = '\0';

          sinptr = (struct sockaddr_in *) theifaddrs->ifa_addr;
          if (sinptr != NULL) {
              ifi->ifi_addr = calloc(1, sizeof(struct sockaddr_in));
              memcpy(ifi->ifi_addr, sinptr, sizeof(struct sockaddr_in));
          }

          sinptr = (struct sockaddr_in *) theifaddrs->ifa_broadaddr;

          if (sinptr != NULL) {
              ifi->ifi_brdaddr = calloc(1, sizeof(struct sockaddr));
              memcpy(ifi->ifi_brdaddr, sinptr, sizeof(struct sockaddr));
          }

      }
      freeifaddrs(ifaddrsp);
  }


  return(ifihead);    /* pointer to first structure in linked list */
}

#else

struct ifi_info *et_get_ifi_info(int family, int doaliases)
{
  struct ifi_info    *ifi, *ifihead, **ifipnext;
  int                sockfd, len, lastlen, flags, myflags;
  char               *ptr, *buf, lastname[IFNAMSIZ], *cptr;
  struct ifconf      ifc;
  struct ifreq       *ifr, ifrcopy;
  struct sockaddr_in *sinptr;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    fprintf(stderr, "et_get_ifi_info: socket error, %s.\n", strerror(errno));
    return NULL;
  }

  /* initial buffer size guess */
  len = 10 * sizeof(struct ifreq);
  lastlen = 0;
  
  for ( ; ; ) {
    buf = malloc(len);
    ifc.ifc_len = len;
    ifc.ifc_buf = buf;
#if defined __APPLE__
    /* Newer BSD systems (>=4.4) do not return arrays of constant
       sized elements if SIOCGIFCONF is used. To get the "old-style"
       structures, OSIOCGIFCONF must be used. I found this from
       the following URL:
       http://www.omniorb-support.com/pipermail/omniorb-dev/2004-January/000134.html */
    if (ioctl(sockfd, OSIOCGIFCONF, &ifc) < 0) {
#elif defined VXWORKS
    if (ioctl(sockfd, SIOCGIFCONF, (int) &ifc) < 0) {
#else
    if (ioctl(sockfd, SIOCGIFCONF, &ifc) < 0) {
#endif
      if (errno != EINVAL || lastlen != 0) {
        fprintf(stderr, "et_get_ifi_info: ioctl error\n");
        close(sockfd);
        return NULL;
      }
    }
    else {
      if (ifc.ifc_len == lastlen) {
        /* success, len has not changed */
        break;
      }
      lastlen = ifc.ifc_len;
    }
    len += sizeof(struct ifreq);    /* increment */
    free(buf);
  }
  
  ifihead     = NULL;
  ifipnext    = &ifihead;
  lastname[0] = 0;

  for (ptr = buf; ptr < buf + ifc.ifc_len; ) {
    ifr = (struct ifreq *) ptr;

    switch (ifr->ifr_addr.sa_family) {
#ifdef IPV6
      case AF_INET6:    
        len = sizeof(struct sockaddr_in6);
        break;
#endif
#if !defined linux && !defined __APPLE__
      case AF_LINK:
        len = sizeof(struct sockaddr_dl);
        break;
#endif
      case AF_INET:
      default:
        len = sizeof(struct sockaddr);
        break;
    }
    
    /* for next one in buffer */
    /*printf(" Name= %s  Family =0x%x  len = %d\n",  ifr->ifr_name,ifr->ifr_addr.sa_family,len);*/
    ptr += sizeof(struct ifreq);

    /* ignore if not desired address family */
    if (ifr->ifr_addr.sa_family != family) {
      continue;
    }
    
    myflags = 0;
    if ( (cptr = strchr(ifr->ifr_name, ':')) != NULL) {
      *cptr = 0;    /* replace colon with null */
    }
    if (strncmp(lastname, ifr->ifr_name, IFNAMSIZ) == 0) {
      if (doaliases == 0) {
        continue;    /* already processed this interface */
      }
      myflags = IFI_ALIAS;
    }
    memcpy(lastname, ifr->ifr_name, IFNAMSIZ);

    ifrcopy = *ifr;
#ifdef VXWORKS
    ioctl(sockfd, SIOCGIFFLAGS, (int)&ifrcopy);
#else
    ioctl(sockfd, SIOCGIFFLAGS, &ifrcopy);
#endif
    flags = ifrcopy.ifr_flags;
    /* ignore if interface not up */
    if ((flags & IFF_UP) == 0) {
      continue;
    }

    ifi = calloc(1, sizeof(struct ifi_info));
    *ifipnext = ifi;               /* prev points to this new one */
    ifipnext  = &ifi->ifi_next;    /* pointer to next one goes here */

    ifi->ifi_flags = flags;        /* IFF_xxx values */
    ifi->ifi_myflags = myflags;    /* IFI_xxx values */
    memcpy(ifi->ifi_name, ifr->ifr_name, IFI_NAME);
    ifi->ifi_name[IFI_NAME-1] = '\0';

    switch (ifr->ifr_addr.sa_family) {
    case AF_INET:
      sinptr = (struct sockaddr_in *) &ifr->ifr_addr;
      if (ifi->ifi_addr == NULL) {
        ifi->ifi_addr = calloc(1, sizeof(struct sockaddr_in));
        memcpy(ifi->ifi_addr, sinptr, sizeof(struct sockaddr_in));

        if (flags & IFF_BROADCAST) {
#ifdef VXWORKS
          ioctl(sockfd, SIOCGIFBRDADDR, (int)&ifrcopy);
#else
          ioctl(sockfd, SIOCGIFBRDADDR, &ifrcopy);
#endif
          sinptr = (struct sockaddr_in *) &ifrcopy.ifr_broadaddr;
          ifi->ifi_brdaddr = calloc(1, sizeof(struct sockaddr_in));
/*printf("et_get_ifi_info: Broadcast addr = %s\n", inet_ntoa(sinptr->sin_addr));*/
          memcpy(ifi->ifi_brdaddr, sinptr, sizeof(struct sockaddr_in));
        }

        if (flags & IFF_POINTOPOINT) {
#ifdef VXWORKS
          ioctl(sockfd, SIOCGIFDSTADDR, (int)&ifrcopy);
#else
          ioctl(sockfd, SIOCGIFDSTADDR, &ifrcopy);
#endif
          sinptr = (struct sockaddr_in *) &ifrcopy.ifr_dstaddr;
          ifi->ifi_dstaddr = calloc(1, sizeof(struct sockaddr_in));
          memcpy(ifi->ifi_dstaddr, sinptr, sizeof(struct sockaddr_in));
        }
      }
      break;

    default:
      break;
    }
  }
  free(buf);
  close(sockfd);
  return(ifihead);        /* pointer to first structure in linked list */
}

#endif

/******************************************************/
void et_free_ifi_info(struct ifi_info *ifihead)
{
  struct ifi_info  *ifi, *ifinext;

  for (ifi = ifihead; ifi != NULL; ifi = ifinext) {
    if (ifi->ifi_addr != NULL) {
      free(ifi->ifi_addr);
    }
    if (ifi->ifi_brdaddr != NULL) {
      free(ifi->ifi_brdaddr);
    }
    if (ifi->ifi_dstaddr != NULL) {
      free(ifi->ifi_dstaddr);
    }
    ifinext = ifi->ifi_next;        /* can't fetch ifi_next after free() */
    free(ifi);                      /* the ifi_info{} itself */
  }
}

/**
 * This routine frees allocated memory of a linked list containing IP info.
 *
 * @param ipaddr pointer to first element of linked list to be freed
 *
 * @returns ET_OK if successful
 * @returns ET_ERROR if error
 * @returns ET_ERROR_NOMEM if no more memory
 */
void et_freeIpAddrs(et_ipaddr *ipaddr) {
  int i;
  et_ipaddr *next;
  
  while (ipaddr != NULL) {
    next = ipaddr->next;
    if (ipaddr->aliases != NULL) {
      for (i=0; i<ipaddr->aliasCount; i++) {
        free(ipaddr->aliases[i]);
      }
      free(ipaddr->aliases);
    }
    free(ipaddr);
    ipaddr = next;
  }
}

/**
 * This routine finds all IP addresses, their names & subnets of this host and
 * returns the data in the arguments.
 *
 * @param ipaddrs address of pointer to a struct to hold IP data in linked list.
 *                If NULL, nothing is returned here.
 * @param ipinfo  pointer to struct to hold IP data in a fixed size array for
 *                storage in shared memory. This array is ET_MAXADDRESSES in size. 
 *                If NULL, nothing is returned here.
 *
 * @returns ET_OK if successful
 * @returns ET_ERROR if error
 * @returns ET_ERROR_NOMEM if no more memory
 */
int et_getNetInfo(et_ipaddr **ipaddrs, et_netinfo *info)
{
  struct ifi_info       *ifi, *ifihead, *default_ifi=NULL;
  struct sockaddr       *sa;
  struct hostent        *hptr;
#ifdef VXWORKS
  int                   *vxaddr;
  char                  vxhost[ET_MAXHOSTNAMELEN];
#endif
  int                   i, debug=0;
  char                  **pptr, host[ET_MAXHOSTNAMELEN];
  et_ipaddr             *ipaddr=NULL, *prev=NULL, *first=NULL;
  
  
  /* get fully qualified default hostname of this host */
  et_defaulthost(host, ET_MAXHOSTNAMELEN);

  /* look through IPv4 interfaces */
  ifihead = ifi = et_get_ifi_info(AF_INET, 1);
  if (ifi == NULL) {
    fprintf(stderr, "et_getNetInfo: cannot find network interface info\n");
    return ET_ERROR;
  }

  for (;ifi != NULL; ifi = ifi->ifi_next) {
    /* ignore loopback interface */
    if (ifi->ifi_flags & IFF_LOOPBACK) {
      continue;
    }
    /* if the interface is up */
    if (ifi->ifi_flags & IFF_UP) {
    
      /* allocate space for IP data */
      ipaddr = (et_ipaddr *)calloc(1, sizeof(et_ipaddr));
      if (ipaddr == NULL) {
        et_freeIpAddrs(first);
        et_free_ifi_info(ifihead);
        fprintf(stderr, "et_getNetInfo: no memory\n");
        return ET_ERROR_NOMEM;
      }
      /* string IP address info structs in linked list */
      if (prev != NULL) {
        prev->next = ipaddr;
      }
      else {
        first = ipaddr;
      }
      prev = ipaddr;
   
      /* if there is an address listed ... */
      if ( (sa = ifi->ifi_addr) != NULL) {

#ifdef VXWORKS
        /* Internet address starts at the 4th byte of the sockaddr structure
           This corresponds to sa->sa_data[2] */
        vxaddr = (int *)&(sa->sa_data[2]);
        ipaddr->saddr = *((struct sockaddr_in *) vxaddr); /* copy it */
        if(hostGetByAddr(*vxaddr,vxhost) < 0) {
          et_freeIpAddrs(first);
          et_free_ifi_info(ifihead);
          fprintf(stderr,"et_getNetInfo: error in hostGetByAddr\n");
          return ET_ERROR;
        }

        hptr = gethostbyname(vxhost);
        
        /* if this is the default interface, mark it */
        if (strcmp(host, vxhost) == 0) {
          default_ifi = ifi;
        }
        
#else
        ipaddr->saddr = *((struct sockaddr_in *) sa); /* copy it */
        hptr = gethostbyaddr((const char *)&ipaddr->saddr.sin_addr,
                              sizeof(struct in_addr), AF_INET);
        if (hptr == NULL) {
          et_freeIpAddrs(first);
          et_free_ifi_info(ifihead);
          fprintf(stderr, "et_getNetInfo: error in gethostbyaddr\n");
          return ET_ERROR;
        }
        /* if this is the default interface, mark it */
        if (strcmp(host, hptr->h_name) == 0) {
          default_ifi = ifi;
        }
#endif
        /* copy canonical name ... */
        strncpy(ipaddr->canon, hptr->h_name, ET_MAXHOSTNAMELEN-1);
if (debug)
    printf("et_getNetInfo canon name: %s\n", hptr->h_name);
        
        /* copy aliases, but first count them */
        for (pptr= hptr->h_aliases; *pptr != NULL; pptr++) {
          ipaddr->aliasCount++;
        }
        
        if (ipaddr->aliasCount > 0) {
          ipaddr->aliases = (char **)calloc(ipaddr->aliasCount, sizeof(char *));
          if (ipaddr->aliases == NULL) {
            et_freeIpAddrs(first);
            et_free_ifi_info(ifihead);
            fprintf(stderr, "et_getNetInfo: no memory\n");
            return ET_ERROR_NOMEM;
          }
        }
        
        i=0;
        for (pptr= hptr->h_aliases; *pptr != NULL; pptr++) {
          ipaddr->aliases[i] = strdup(*pptr);
          if (ipaddr->aliases[i] == NULL) {
            ipaddr->aliasCount = i;
            et_freeIpAddrs(first);
            et_free_ifi_info(ifihead);
            fprintf(stderr, "et_getNetInfo: no memory\n");
            return ET_ERROR_NOMEM;
          }
          i++;
if (debug)
    printf("et_getNetInfo alias #%d  : %s\n", i, *pptr);
        }
       
        /* copy IP address - only 1 per loop */
        strncpy(ipaddr->addr, sock_ntop_host(sa, sizeof(*sa)), ET_IPADDRSTRLEN-1);
if (debug)
    printf("et_getNetInfo address   : %s\n", sock_ntop_host(sa, sizeof(*sa)));
      }

      /* if the interface is broadcast enabled */
      if ((ifi->ifi_flags & IFF_BROADCAST) > 0) {
        /* if there is a broadcast (subnet) address listed ... */
        if ( (sa = ifi->ifi_brdaddr) != NULL) {
          strncpy(ipaddr->broadcast, sock_ntop_host(sa, sizeof(*sa)), ET_IPADDRSTRLEN-1);
if (debug)
    printf("et_getNetInfo broadcast : %s\n", sock_ntop_host(sa, sizeof(*sa)));
        }
      }
      
    } /* if interface is up */
if (debug)
     printf("\n");
  } /* for each interface */
  
  /* free memory */
  et_free_ifi_info(ifihead);
  
  /* ************************ */
  /* send back data to caller */
  /* ************************ */
  
  /* copy everything into a fixed-size array of structures for use in shared memory */
  if (info != NULL) {
    int j, i=0;
    ipaddr = first;
    
    while (ipaddr != NULL) {
      /* look at no more than ET_MAXADDRESSES IP addresses */
      if (i >= ET_MAXADDRESSES) break;
      info->ipinfo[i].saddr.sin_addr.s_addr = ipaddr->saddr.sin_addr.s_addr;
      strcpy(info->ipinfo[i].addr, ipaddr->addr);
      strcpy(info->ipinfo[i].canon, ipaddr->canon);
      strcpy(info->ipinfo[i].broadcast, ipaddr->broadcast);
      for (j=0; j < ipaddr->aliasCount; j++) {
        /* look at no more than ET_MAXADDRESSES aliases */
        if (j >= ET_MAXADDRESSES) break;
        strcpy(info->ipinfo[i].aliases[j], ipaddr->aliases[j]);
      }
      info->ipinfo[i].aliasCount = j;
      
      i++;
      ipaddr = ipaddr->next;
    }
    info->count = i;
  }
  
  if (ipaddrs != NULL) {
    *ipaddrs = first;
  }
  else {
    et_freeIpAddrs(first);
  }
  
  return ET_OK;
}


/**
 * This routine frees allocated memory of a linked list containing broadcast addresses.
 *
 * @param addr pointer to first element of linked list to be freed
 *
 * @returns ET_OK if successful
 * @returns ET_ERROR if error
 * @returns ET_ERROR_NOMEM if no more memory
 */
void et_freeBroadcastAddrs(et_iplist *addr) {
  et_iplist *next;
  while (addr != NULL) {
    next = addr->next;
    free(addr);
    addr = next;
  }
}


/**
 * This routine finds all broadcast addresses, eliminates duplicates and
 * returns the data in the arguments.
 *
 * @param addrs   address of pointer to a struct to hold broadcast data in linked list.
 *                If NULL, nothing is returned here.
 * @param bcaddrs pointer to struct to hold broadcast data in a fixed size array for storage
 *                in shared memory. This array allows only ET_MAXADDRESSES number of addresses. 
 *                If NULL, nothing is returned here.
 *
 * @returns ET_OK if successful
 * @returns ET_ERROR if error
 * @returns ET_ERROR_NOMEM if no more memory
 */
int et_getBroadcastAddrs(et_iplist **addrs, et_ddipaddrs *bcaddrs)
{
  char  *p;
  int    index, count=0, skip, debug=0;
  struct ifi_info *ifi, *ifihead;
  struct sockaddr *sa;
  et_iplist    *baddr=NULL, *first=NULL, *prev=NULL, *paddr;
  
  
  /* look through IPv4 interfaces */
  ifihead = ifi = et_get_ifi_info(AF_INET, 1);
  if (ifi == NULL) {
    fprintf(stderr, "et_getBroadcastAddrs: cannot find network interface info\n");
    return ET_ERROR;
  }

  for (;ifi != NULL; ifi = ifi->ifi_next) {
    /* ignore loopback */
    if (ifi->ifi_flags & IFF_LOOPBACK) {
      continue;
    }
    
    /* if the interface is up & broadcast enabled */
    if ( (ifi->ifi_flags & IFF_UP) && ((ifi->ifi_flags & IFF_BROADCAST) > 0) ) {
    
      if ( (sa = ifi->ifi_brdaddr) != NULL) {
        p = sock_ntop_host(sa, sizeof(*sa));
        
        /* check to see if we already got this one */
        skip  = 0;
        index = 0;
        paddr = first;
        while (index++ < count) {
          if (strcmp(p, paddr->addr) == 0) {
            skip++;
            break;
          }
          paddr = paddr->next;
        }
        if (skip) continue;
        
        /* allocate space for broadcast data */
        baddr = (et_iplist *)calloc(1, sizeof(et_iplist));
        if (baddr == NULL) {
          et_freeBroadcastAddrs(first);
          et_free_ifi_info(ifihead);
          fprintf(stderr, "et_getBroadcastAddrs: no memory\n");
          return ET_ERROR_NOMEM;
        }

        /* string address structs in linked list */
        if (prev != NULL) {
          prev->next = baddr;
        }
        else {
          first = baddr;
        }
        prev = baddr;

        strncpy(baddr->addr, p, ET_IPADDRSTRLEN-1);
        count++;
if (debug)
    printf("et_getBroadcastAddrs broadcast : %s\n", sock_ntop_host(sa, sizeof(*sa)));
      }
    } /* if interface is up */
  } /* for each interface */
if (debug)
    printf("\n");
  
  /* free memory */
  et_free_ifi_info(ifihead);
  
  /* ************************ */
  /* send back data to caller */
  /* ************************ */
  
  /* copy everything into 1 structure for use in shared memory */
  if (bcaddrs != NULL) {
    paddr = first;
    bcaddrs->count = 0;
    
    while (paddr != NULL) {
      if (bcaddrs->count >= ET_MAXADDRESSES) break;
      strcpy(bcaddrs->addr[bcaddrs->count++], paddr->addr);
      paddr = paddr->next;
    }
  }
  
  if (addrs != NULL) {
     *addrs = first;
  }
  else {
    et_freeBroadcastAddrs(first);
  }
  
  return ET_OK;
}


/******************************************************/
/*  Find out if the operating system is Linux or not  */
int et_isLinux(void)
{
#ifdef VXWORKS

  return 0;

#else
  struct utsname mysystem;

  /* find out the operating system of the machine we're on */
  if (uname(&mysystem) < 0) {
    fprintf(stderr, "et_isLinux: cannot find system name\n");
    return ET_ERROR;
  }

  if (strcasecmp(mysystem.sysname, "linux") == 0) {
    return 1;
  }
  return 0;
#endif
}


/******************************************************/
int et_udpreceive(unsigned short port, const char *address, int cast)
{
  int                err, sockfd;
  const int          on = 1;
  struct in_addr     castaddr;
  struct sockaddr_in servaddr;

  /* put broad/multicast address into net-ordered binary form */
#ifdef VXWORKS
  if (inet_aton((char *)address, &castaddr) == INET_ATON_ERR) {
#else
  if (inet_aton(address, &castaddr) == INET_ATON_ERR) {
#endif
    fprintf(stderr, "et_udpreceive: inet_aton error\n");
    return -1;
  }

  bzero((void *)&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr   = castaddr;
  /*servaddr.sin_addr.s_addr = htonl(INADDR_ANY);*/
  servaddr.sin_port   = htons(port);

  /* create socket */
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    fprintf(stderr, "et_udpreceive: socket error\n");
    return -1;
  }
  
  /* allow multiple copies of this to run on same host */
  err = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *) &on, sizeof(on));
  if (err < 0) {
    fprintf(stderr, "et_udpreceive: setsockopt error\n");
    return err;
  }
  
  /* add to multicast group */
  if (cast == ET_MULTICAST) {
    struct ip_mreq     mreq;

    mreq.imr_multiaddr = castaddr;
    /* accept multicast over any interface */
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    err = setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *) &mreq, sizeof(mreq));
    if (err < 0) {
      fprintf(stderr, "et_udpreceive: setsockopt IP_ADD_MEMBERSHIP error\n");
      return err;
    }
  }
    
  /* only allow packets to this port & address to be received */
  
  err = bind(sockfd, (SA *) &servaddr, sizeof(servaddr));
  if (err < 0) {
    char errnostr[255];
    sprintf(errnostr,"err=%d ",errno);
    perror(errnostr);
    fprintf(stderr, "et_udpreceive: bind error\n");
    return err;
  }

  return sockfd;
}


/******************************************************
 * To talk to an ET system on another computer, we need
 * to find the TCP port number the server thread of that
 * ET system is listening on. There are a number of ways to
 * do this.
 *
 * First off:
 * The remote client (who is running this routine) either
 * says he doesn't know where the ET is (config->host = 
 * ET_HOST_REMOTE or ET_HOST_ANYWHERE), or he specifies
 * the host that the ET system is running on.
 *
 * If we don't know the host name we can broadcast and/or
 * multicast (UDP) to addresses which the ET system is
 * listening on (each address on a separate thread). In
 * this broad/multicast, we send the name of the ET system
 * that we want to find. The system which receives the
 * packet and has the same name, responds with the port #
 * and its host name from each interface address or
 * broad/multicast address that received the packet.
 * If we know the hostname, however, we send a udp packet
 * to ET systems listening on that host and UDP port. The
 * proper system should be the only one to respond with its
 * server TCP port.
 *
 * When a server responds to a broad/multicast and we've
 * specified ET_HOST_REMOTE, this routine must see whether
 * the server is really on a remote host and not the local one.
 * To make this determination, the server has also sent
 * his host name obtained by running "uname". That is compared
 * to the result of running "uname" on this host.
 *****************************************************/
 
/******************************************************
 * etname:    ET system file name
 * ethost:    returns name of ET system's host
 * config:    configuration passed to et_open
 * trys:      max # of times to broadcast UDP packet
 * waittime:  wait time for response to broad/multicast
 *****************************************************/
 
 /* structure for holding a single response to our broad/multicast */
typedef struct response_t {
  int   port;                     /**< ET system's TCP server port. */
  int   castType;                 /**< ET_BROADCAST or ET_MULTICAST (what this is a response to). */
  int   nameCount;                /**< Number of names and addresses. */
  uint32_t *addrs;                /**< Array of 32bit net byte ordered addresses (1 for each name). */
  char  uname[ET_MAXHOSTNAMELEN]; /**< Uname of sending host. */
  char  castIP[ET_IPADDRSTRLEN];  /**< Original broad/multicast IP addr. */
  char  **names;                  /**< Array of names corresponding to host (all for multicast, on subnet for broadcast). */
  struct response_t *next;        /**< Next response in linked list. */
} response;


static void freeAnswers(response *answer) {
  int i;
  response *next;
  
  while (answer != NULL) {
    next = answer->next;
    if (answer->names != NULL) {
      for (i=0; i<answer->nameCount; i++) {
        free(answer->names[i]);
      }
      free(answer->names);
    }
    free(answer->addrs);
    free(answer);
    answer = next;
  }
}


int et_findserver2(const char *etname, char *ethost, int *port, uint32_t *inetaddr,
                              et_open_config *config, int trys,
                              struct timeval *waittime)
{
  int          i, j, k, l, n, err, version, nameCount, castType, gotMatch=0, subnetCount=0;
  int          length, len_net, lastdelay, maxtrys=6, serverport=0, debug=0, magicInts[3];
  const int    on=1, timeincr[]={0,1,2,3,4,5};
  uint32_t     addr;
  et_iplist *baddr;
   
  /* encoding & decoding packets */
  char  *pbuf, buffer[4096]; /* should be way more than enough */
  char  outbuf[ET_FILENAME_LENGTH+1+5*sizeof(int)];
  char  localhost[ET_MAXHOSTNAMELEN];
  char  localuname[ET_MAXHOSTNAMELEN];
  char  specifiedhost[ET_MAXHOSTNAMELEN];
  char  unqualifiedhost[ET_MAXHOSTNAMELEN];

  int   numresponses=0, remoteresponses=0;
  response *answer, *answer_first=NULL, *answer_prev=NULL, *first_remote=NULL;

  /* socket & select stuff */
  struct in_addr     castaddr;
  struct sockaddr_in servaddr, cliaddr;
  socklen_t          len;
  int                sockfd, numsockets=0, biggestsockfd=-1;
  struct timespec    delaytime;
  struct timeval     tv;
  fd_set             rset;
  
  /* structure for holding info to send out a packet on a socket */
  struct senddata {
    int sockfd;
    struct sockaddr_in servaddr;
  };
  struct senddata *send;
   
  /* check args */
  if ((ethost == NULL) || (port == NULL)) {
    fprintf(stderr, "et_findserver: invalid (null) arguments\n");
    return ET_ERROR;
  }
  
  /* count # of subnet addrs */
  baddr = config->bcastaddrs;
  while (baddr != NULL) {
      subnetCount++;
      baddr = baddr->next;
  }
  
  /* allocate space to store all our outgoing stuff */
  send = (struct senddata *) calloc(subnetCount + config->mcastaddrs.count,
                                    sizeof(struct senddata));
  if (send == NULL) {
    fprintf(stderr, "et_findserver: cannot allocate memory\n");
    return ET_ERROR;
  } 
  
  /* find local uname */
  if (et_getUname(localuname, ET_MAXHOSTNAMELEN) != ET_OK) {
    /* nothing will match this */
    strcpy(localuname, "...");
  }
  
  /* get fully qualified canonical name of this host (obtained with uname or "localhost" if that fails) */
  et_defaulthost(localhost, ET_MAXHOSTNAMELEN);
  
  /* If the host is local or we know its name... */
  if ((strcmp(config->host, ET_HOST_REMOTE) != 0) &&
      (strcmp(config->host, ET_HOST_ANYWHERE) != 0)) {

    /* if it's local, find address associated with uname */
    if ((strcmp(config->host, ET_HOST_LOCAL) == 0) ||
        (strcmp(config->host, "localhost")   == 0))  {
      
      /* store canonical name of local host to contact */
      strcpy(specifiedhost, localhost);
    }
    /* else if we know its name ... */
    else {
      /* store name of specified host to contact */
      strcpy(specifiedhost, config->host);
    }
    
    /* get an unqualified host for later comparisons */
    {
      char *p, *q;
      q = strdup(specifiedhost);
      p = strtok(q, ".");
      if (p != NULL)
        strcpy(unqualifiedhost, p);
      free(q);
    }
    
if (debug)
    printf("et_findserver: specifed host = %s, unqualifiedhost = %s\n", specifiedhost, unqualifiedhost);
    
    /* We need 1 socket for each subnet if broadcasting */
    if ((config->cast == ET_BROADCAST) ||
        (config->cast == ET_BROADANDMULTICAST)) {
        
      /* for each listed broadcast address ... */
      baddr = config->bcastaddrs;
      while (baddr != NULL) {
          /* put address into net-ordered binary form */
          if (inet_aton(baddr->addr, &castaddr) == INET_ATON_ERR) {
            fprintf(stderr, "et_findserver: inet_aton error net_addr = %s\n",  baddr->addr);
            for (j=0; j<numsockets; j++) {
              close(send[j].sockfd);
            }
            free(send);
            return ET_ERROR;
          }

if (debug)
    printf("et_findserver: send broadcast packet to %s on port %d\n", baddr->addr, config->udpport);

          /* server's location */
          bzero((void *)&servaddr, sizeof(servaddr));
          servaddr.sin_family = AF_INET;
          servaddr.sin_addr   = castaddr;
          servaddr.sin_port   = htons((unsigned short)config->udpport);

          /* create socket */
          sockfd = socket(AF_INET, SOCK_DGRAM, 0);
          if (sockfd < 0) {
            fprintf(stderr, "et_findserver: socket error\n");
            for (j=0; j<numsockets; j++) {
              close(send[j].sockfd);
            }
            free(send);
            return ET_ERROR;
          }

          /* make this a broadcast socket */
          err = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (void *) &on, sizeof(on));
          if (err < 0) {
            fprintf(stderr, "et_findserver: setsockopt SO_BROADCAST error\n");
            close(sockfd);
            for (j=0; j<numsockets; j++) {
              close(send[j].sockfd);
            }
            free(send);
            return ET_ERROR;
          }

          /* for sending packet and for select */
          send[numsockets].sockfd   = sockfd;
          send[numsockets].servaddr = servaddr;
          numsockets++;
          if (biggestsockfd < sockfd) biggestsockfd = sockfd;
          FD_SET(sockfd, &rset);

          baddr = baddr->next;
      }
    }

    /* if also configured for multicast, send that too */
    if ((config->cast == ET_MULTICAST) ||
        (config->cast == ET_BROADANDMULTICAST)) {
      /* for each listed address ... */
      for (i=0; i < config->mcastaddrs.count; i++) {
        /* put address into net-ordered binary form */
        if (inet_aton(config->mcastaddrs.addr[i], &castaddr) == INET_ATON_ERR) {
          fprintf(stderr, "et_findserver: inet_aton error\n");
          for (j=0; j<numsockets; j++) {
            close(send[j].sockfd);
          }
          free(send);
          return ET_ERROR;
        }

if (debug)
    printf("et_findserver: send multicast packet to %s on port %d\n", config->mcastaddrs.addr[i],
                                                                      config->multiport);

        /* server's location */
        bzero((void *)&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr   = castaddr;
        servaddr.sin_port   = htons((unsigned short)config->multiport);

        /* create socket */
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
          fprintf(stderr, "et_findserver: socket error\n");
          for (j=0; j<numsockets; j++) {
            close(send[j].sockfd);
          }
          free(send);
          return ET_ERROR;
        }

        /* Set the scope of the multicast, but don't bother
         * if ttl = 1 since that's the default.
         */
        if (config->ttl != 1) {
          unsigned char ttl = config->ttl;
          
          err = setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, (void *) &ttl, sizeof(ttl));  
          if (err < 0){
            fprintf(stderr, "et_findserver: setsockopt IP_MULTICAST_TTL error\n");
            close(sockfd);
            for (j=0; j<numsockets; j++) {
              close(send[j].sockfd);
            }
            free(send);
            return ET_ERROR;
          }
        }
        
        /* for sending packet and for select */
        send[numsockets].sockfd   = sockfd;
        send[numsockets].servaddr = servaddr;
        numsockets++;
        if (biggestsockfd < sockfd) biggestsockfd = sockfd;
        FD_SET(sockfd, &rset);
      }
    }
  }
  
  /* else if the host name is not specified, and it's either
   * remote or anywhere out there, broad/multicast to find it
   */
  else {
    /* We need 1 socket for each subnet if broadcasting */
    if ((config->cast == ET_BROADCAST) ||
        (config->cast == ET_BROADANDMULTICAST)) {

      /* for each listed broadcast address ... */
      et_iplist *baddr = config->bcastaddrs;
      while (baddr != NULL) {
          /* put address into net-ordered binary form */
          if (inet_aton(baddr->addr, &castaddr) == INET_ATON_ERR) {
            fprintf(stderr, "et_findserver: inet_aton error net_addr = %s\n",  baddr->addr);
            for (j=0; j<numsockets; j++) {
              close(send[j].sockfd);
            }
            free(send);
            return ET_ERROR;
          }

if (debug)
    printf("et_findserver: send broadcast packet to %s on port %d\n", baddr->addr, config->udpport);

          /* server's location */
          bzero((void *)&servaddr, sizeof(servaddr));
          servaddr.sin_family = AF_INET;
          servaddr.sin_addr   = castaddr;
          servaddr.sin_port   = htons((unsigned short)config->udpport);

          /* create socket */
          sockfd = socket(AF_INET, SOCK_DGRAM, 0);
          if (sockfd < 0) {
            fprintf(stderr, "et_findserver: socket error\n");
            for (j=0; j<numsockets; j++) {
              close(send[j].sockfd);
            }
            free(send);
            return ET_ERROR;
          }

          /* make this a broadcast socket */
          err = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (void *) &on, sizeof(on));
          if (err < 0) {
            fprintf(stderr, "et_findserver: setsockopt SO_BROADCAST error\n");
            close(sockfd);
            for (j=0; j<numsockets; j++) {
              close(send[j].sockfd);
            }
            free(send);
            return ET_ERROR;
          }

          /* for sending packet and for select */
          send[numsockets].sockfd   = sockfd;
          send[numsockets].servaddr = servaddr;
          if (biggestsockfd < sockfd) biggestsockfd = sockfd;
          numsockets++;
          FD_SET(sockfd, &rset);

          baddr = baddr->next;
      }
    }

    /* We need 1 socket for each multicast address if multicasting */
    if ((config->cast == ET_MULTICAST) ||
        (config->cast == ET_BROADANDMULTICAST)) {
      /* for each listed address ... */
      for (i=0; i < config->mcastaddrs.count; i++) {
        /* put address into net-ordered binary form */
        if (inet_aton(config->mcastaddrs.addr[i], &castaddr) == INET_ATON_ERR) {
          fprintf(stderr, "et_findserver: inet_aton error\n");
          for (j=0; j<numsockets; j++) {
            close(send[j].sockfd);
          }
          free(send);
          return ET_ERROR;
        }

if (debug)
    printf("et_findserver: send multicast packet to %s on port %d\n", config->mcastaddrs.addr[i],
                                                                      config->multiport);

        /* server's location */
        bzero((void *)&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr   = castaddr;
        servaddr.sin_port   = htons((unsigned short)config->multiport);

        /* create socket */
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
          fprintf(stderr, "et_findserver: socket error\n");
          for (j=0; j<numsockets; j++) {
            close(send[j].sockfd);
          }
          free(send);
          return ET_ERROR;
        }

        /* Set the scope of the multicast, but don't bother
         * if ttl = 1 since that's the default.
         */
        if (config->ttl != 1) {
          unsigned char ttl = config->ttl;
          
          err = setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, (void *) &ttl, sizeof(ttl));  
          if (err < 0){
            fprintf(stderr, "et_findserver: setsockopt IP_MULTICAST_TTL error\n");
            close(sockfd);
            for (j=0; j<numsockets; j++) {
              close(send[j].sockfd);
            }
            free(send);
            return ET_ERROR;
          }
        }
        
        /* for sending packet and for select */
        send[numsockets].sockfd   = sockfd;
        send[numsockets].servaddr = servaddr;
        if (biggestsockfd < sockfd) biggestsockfd = sockfd;
        numsockets++;
        FD_SET(sockfd, &rset);
        
      }
    }
  } 
  
  /* time to wait for responses to broad/multicasting */
  if (waittime != NULL) {
    delaytime.tv_sec  = waittime->tv_sec;
    delaytime.tv_nsec = 1000*waittime->tv_usec;
  }
  else {
    delaytime.tv_sec  = 0;
    delaytime.tv_nsec = 0;
  }
  lastdelay = delaytime.tv_sec;
  
  /* make select return after 0.01 seconds */
  tv.tv_sec  = 0;
  tv.tv_usec = 10000; /* 0.01 sec */
  
  /* max # of broadcasts is maxtrys */
  if ((trys > maxtrys) || (trys < 1)) {
    trys = maxtrys;
  }
     
  /* Prepare output buffer that we send to servers:
   * (1) ET magic numbers (3 ints),
   * (2) ET version #,
   * (3) ET file name length (includes null)
   * (4) ET file name
   */
  pbuf = outbuf;

  /* magic numbers */
  magicInts[0] = htonl(ET_MAGIC_INT1);
  magicInts[1] = htonl(ET_MAGIC_INT2);
  magicInts[2] = htonl(ET_MAGIC_INT3);
  memcpy(pbuf, magicInts, sizeof(magicInts));
  pbuf += sizeof(magicInts);

  /* ET major version number */
  version = htonl(ET_VERSION);
  memcpy(pbuf, &version, sizeof(version));
  pbuf   += sizeof(version);
  
  /* length of ET system name string */
  length  = strlen(etname)+1;
  len_net = htonl(length);
  memcpy(pbuf, &len_net, sizeof(len_net));
  pbuf += sizeof(len_net);
  
  /* name of the ET system we want to open */
  memcpy(pbuf, etname, length);
  
  /* zero socket-set for select */
  FD_ZERO(&rset);
  
  for (i=0; i < trys; i++) {
    /* send out packets */
    for (j=0; j<numsockets; j++) {
      sendto(send[j].sockfd, (void *) outbuf, sizeof(outbuf), 0, (SA *) &send[j].servaddr, sizeof(servaddr));
    }
    
    /* Increase waiting time for response each round. */
    delaytime.tv_sec = lastdelay + timeincr[i];
    lastdelay = delaytime.tv_sec;
    
    /* reset "rset" value each time select is called */
    for (j=0; j<numsockets; j++) {
      FD_SET(send[j].sockfd, &rset);
    }
    
    /* Linux reportedly changes tv in "select" so reset it each round */
    tv.tv_sec  = 0;
    tv.tv_usec = 10000; /* 0.01 sec */
    /* wait for responses from ET systems */
#ifdef VXWORKS
    taskDelay(lastdelay*60);
#else
    nanosleep(&delaytime, NULL);
#endif
    /* select */
    err = select(biggestsockfd + 1, &rset, NULL, NULL, &tv);
        
    /* if select error ... */
    if (err == -1) {
      fprintf(stderr, "et_findserver: select error\n");
      for (j=0; j<numsockets; j++) {
        close(send[j].sockfd);
      }
      free(send);
      return ET_ERROR;
    }
    /* else if select times out, rebroadcast with longer waiting time */
    else if (err == 0) {
      continue;
    }
    
    /* else if we have some response(s) ... */
    else {
        for(j=0; j < numsockets; j++) {
          int bytes;
          
          if (FD_ISSET(send[j].sockfd, &rset) == 0) {
            /* this socket is not ready for reading */
            continue;
          }
          
anotherpacket:

          /* get back a packet from a server */
          len = sizeof(cliaddr);
          n = recvfrom(send[j].sockfd, (void *) buffer, sizeof(buffer), 0, (SA *) &cliaddr, &len);

          /* if error ... */
          if (n < 0) {
            fprintf(stderr, "et_findserver: recvfrom error, %s\n", strerror(errno));
            for (k=0; k<numsockets; k++) {
              close(send[k].sockfd);
            }
            free(send);
            /* free up all answers */
            freeAnswers(answer_first);
            return ET_ERROR;
          }
          
          /* allocate space for single response */
          answer = (response *) calloc(1, sizeof(response));
          if (answer == NULL) {
            fprintf(stderr, "et_findserver: out of memory\n");
            for (k=0; k<numsockets; k++) {
              close(send[k].sockfd);
            }
            free(send);
            freeAnswers(answer_first);
            return ET_ERROR_NOMEM;
          }
          
          /* string answer structs in linked list */
          if (answer_prev != NULL) {
            answer_prev->next = answer;
          }
          else {
            answer_first = answer;
          }

          /* see if there's another packet to be read on this socket */
          bytes = 0;
#ifdef VXWORKS
          ioctl(send[j].sockfd, FIONREAD, (int) &bytes);
#else
          ioctl(send[j].sockfd, FIONREAD, (void *) &bytes);
#endif
          
          /* decode packet from ET system:
           * (0)  ET magic numbers (3 ints)
           * (1)  ET version #
           * (2)  port of tcp server thread (not udp config->port)
           * (3)  ET_BROADCAST or ET_MULTICAST (int)
           * (4)  length of next string
           * (5)    broadcast address (dotted-dec) if broadcast received or
           *        multicast address (dotted-dec) if multicast received
           *        (see int #3)
           * (6)  length of next string
           * (7)    hostname given by "uname" (used as a general
           *        identifier of this host no matter which interface is used)
           * (8)  number of names for this IP addr starting with canonical
           * (9)    32bit, net-byte ordered IPv4 address assoc with following name
           * (10)   length of next string
           * (11)       first name = canonical
           * (12)   32bit, net-byte ordered IPv4 address assoc with following name
           * (13)   length of next string
           * (14)       first alias ...
           *
           * All aliases are sent here.
           */
           
          pbuf = buffer;
          
          do {
            /* get ET magic #s */
            memcpy(magicInts, pbuf, sizeof(magicInts));
            pbuf += sizeof(magicInts);
              
            /* if the wrong magic numbers, reject it */
            if (ntohl(magicInts[0]) != ET_MAGIC_INT1 ||
                ntohl(magicInts[1]) != ET_MAGIC_INT2 ||
                ntohl(magicInts[2]) != ET_MAGIC_INT3)  {
                free(answer);
                break;
            }
         
            /* get ET system's major version # */
            memcpy(&version, pbuf, sizeof(version));
            version = ntohl(version);
            pbuf += sizeof(version);

            /* if the wrong version ET system is responding, reject it */
            if (version != ET_VERSION) {
              free(answer);
              break;
            }

            /* get ET system's TCP port */
            memcpy(&serverport, pbuf, sizeof(serverport));
            serverport = ntohl(serverport);
            if ((serverport < 1) || (serverport > 65536)) {
              free(answer);
              break;
            }
            answer->port = serverport;
            pbuf += sizeof(serverport);

            /* broad or multi cast? might be both for java. */
            memcpy(&castType, pbuf, sizeof(castType));
            castType = ntohl(castType);
            if ((castType != ET_BROADCAST) &&
                (castType != ET_MULTICAST) &&
                (castType != ET_BROADANDMULTICAST)) {
              free(answer);
              break;
            }
            answer->castType = castType;
            pbuf += sizeof(castType);

            /* get broad/multicast IP original packet sent to */
            memcpy(&length, pbuf, sizeof(length));
            length = ntohl(length);
            if ((length < 1) || (length > ET_IPADDRSTRLEN)) {
              free(answer);
              break;
            }
            pbuf += sizeof(length);
            memcpy(answer->castIP, pbuf, length);
            pbuf += length;

            /* get ET system's uname */
            memcpy(&length, pbuf, sizeof(length));
            length = ntohl(length);
            if ((length < 1) || (length > ET_MAXHOSTNAMELEN)) {
              free(answer);
              break;
            }
            pbuf += sizeof(length);
            memcpy(answer->uname, pbuf, length);
            pbuf += length;

            /* get number of host names */
            memcpy(&nameCount, pbuf, sizeof(nameCount));
            nameCount = ntohl(nameCount);
            if ((nameCount < 0) || (nameCount > 20)) {
              free(answer);
              break;
            }
            answer->nameCount = nameCount;
            pbuf += sizeof(nameCount);

            /* allocate mem - arrays of ints & char *'s */
            answer->addrs = (uint32_t *)calloc(nameCount, sizeof(uint32_t));
            answer->names = (char **)calloc(nameCount, sizeof(char *));
            if (answer->addrs == NULL || answer->names == NULL) {
              for (k=0; k<numsockets; k++) {
                close(send[k].sockfd);
              }
              free(send);
              free(answer);
              freeAnswers(answer_first);
              return ET_ERROR_NOMEM;
            }

            /* read in all the names & addrs */          
            for (k=0; k < nameCount; k++) {

              memcpy(&addr, pbuf, sizeof(addr));
               /* do not swap the addr as it must be network byte ordered */
              answer->addrs[k] = addr;
              pbuf += sizeof(addr);

              memcpy(&length, pbuf, sizeof(length));
              length = ntohl(length);
              if ((length < 1) || (length > ET_MAXHOSTNAMELEN)) {
                for (l=0; l < k; l++) {
                  free(answer->names[l]);
                }
                free(answer->names);
                free(answer->addrs);
                free(answer);
                break;
              }
              pbuf += sizeof(length);
              
              answer->names[k] = strdup(pbuf); /* ending NULL is sent so we're OK */
              if (answer->names[k] == NULL) {
                for (l=0; l<numsockets; l++) {
                  close(send[l].sockfd);
                }
                for (l=0; l < k; l++) {
                  free(answer->names[l]);
                }
                free(answer->names);
                free(answer->addrs);
                free(answer);
                freeAnswers(answer_first);
                return ET_ERROR_NOMEM;
              }
              answer->names[k][length-1] = '\0';
              pbuf += length;
            }

if (debug)
    printf("et_findserver: RESPONSE from %s at %d with addr = %s and uname = %s\n",
            answer->names[0], answer->port, answer->castIP, answer->uname);

            numresponses++;
            
          } while(0);
          
          /* Now that we have a real answer, make that a part of the list */
          answer_prev = answer;
          
          /* see if there's another packet to be read on this socket */
          if (bytes > 0) {
            goto anotherpacket;
          }
          
        } /* for each socket (j) */
        
        
        /* If host is local or we know its name. There may be many responses.
         * Look only at the response that matches the host's name.
         */
        if ((strcmp(config->host, ET_HOST_REMOTE) != 0) &&
            (strcmp(config->host, ET_HOST_ANYWHERE) != 0)) {
          /* analyze the answers/responses */
          answer = answer_first;
          while (answer != NULL) {
            /* The problem here is another ET system of the
             * same name may respond, but we're interested
             * only in the one on the specified host.
             */
            for (n=0; n < answer->nameCount; n++) {
              if ((strcmp(specifiedhost, answer->names[n])   == 0) ||
                  (strcmp(unqualifiedhost, answer->names[n]) == 0))  {
                gotMatch = 1;
                break;
              }
            }
            
            if (gotMatch) {
if (debug)
    printf("et_findserver: got a match to local or specific: %s matched either %s or %s\n",
            answer->names[n],  specifiedhost, unqualifiedhost);

              for (k=0; k<numsockets; k++) {
                close(send[k].sockfd);
              }
              free(send);
               strcpy(ethost, answer->names[n]);
              *port = answer->port;
              *inetaddr = answer->addrs[n];
              freeAnswers(answer_first);
              return ET_OK;
            }
            
            answer = answer->next;
          }
        }

        /* If the host may be anywhere (local or remote) ...  */
        else if (strcmp(config->host, ET_HOST_ANYWHERE) == 0) {
          /* if our policy is to return an error for more than 1 response ... */
          if ((config->policy == ET_POLICY_ERROR) && (numresponses > 1)) {
            /* print error message and return */
if (debug)
    fprintf(stderr, "et_findserver: %d responses to broad/multicast -\n", numresponses);
            j = 1;
            answer = answer_first;
            while (answer != NULL) {
              fprintf(stderr, "  RESPONSE %d from %s at %d\n",
                      j++, answer->names[0], answer->port);
              answer = answer->next;
            }
            for (k=0; k<numsockets; k++) {
              close(send[k].sockfd);
            }
            free(send);
            freeAnswers(answer_first);
            return ET_ERROR_TOOMANY;
          }

          /* analyze the answers/responses */
          j = 0;
          answer = answer_first;
          while (answer != NULL) {
            /* If our policy is to take the first response do so. If our
             * policy is ET_POLICY_ERROR, we can also return the first as
             * we can only be here if there's been just 1 response.
             */
            if ((config->policy == ET_POLICY_FIRST) ||
                (config->policy == ET_POLICY_ERROR))  {
if (debug)
    printf("et_findserver: got a match to .anywhere, first or error policy\n");
              for (k=0; k<numsockets; k++) {
                close(send[k].sockfd);
              }
              free(send);
              strcpy(ethost, answer->names[0]);
              *port = answer->port;
              *inetaddr = answer->addrs[0];
              freeAnswers(answer_first);
              return ET_OK;
            }
            /* else if our policy is to take the first local response ... */
            else if (config->policy == ET_POLICY_LOCAL) {
              if (strcmp(localuname, answer->uname) == 0) {
if (debug)
    printf("et_findserver: got a uname match to .anywhere, local policy\n");
                for (k=0; k<numsockets; k++) {
                  close(send[k].sockfd);
                }
                free(send);
                strcpy(ethost, answer->names[0]);
                *port = answer->port;
                *inetaddr = answer->addrs[0];
                freeAnswers(answer_first);
                return ET_OK;
              }
              
              /* If we went through all responses without finding
               * a local one, pick the first one we received.
               */
              if (++j == numresponses-1) {
if (debug)
    printf("et_findserver: got a match to .anywhere, nothing local available\n");
                for (k=0; k<numsockets; k++) {
                  close(send[k].sockfd);
                }
                free(send);
                strcpy(ethost, answer_first->names[0]);
                *port = answer_first->port;
                *inetaddr = answer->addrs[0];
                freeAnswers(answer_first);
                return ET_OK;
              }
            }
            answer = answer->next;
          }
        }

        /* if user did not specify host name, and it must be remote ... */
        else if (strcmp(config->host, ET_HOST_REMOTE) == 0) {
          /* analyze the answers/responses */
          answer = answer_first;
          while (answer != NULL) {
            /* The problem here is if we are broadcasting, a local ET
            * system of the same name may respond, but we're interested
            * only in the remote systems. Weed out the local system.
            */
            if (strcmp(localuname, answer->uname) == 0) {
              continue;
            }
            remoteresponses++;
            if (first_remote == NULL) {
              first_remote = answer;
            }
            /* The ET_POLICY_LOCAL doesn't make any sense here so
             * default to ET_POLICY_FIRST
             */
            if ((config->policy == ET_POLICY_FIRST) ||
                (config->policy == ET_POLICY_LOCAL))  {
if (debug)
     printf("et_findserver: got a match to .remote, first or local policy\n");
              for (k=0; k<numsockets; k++) {
                close(send[k].sockfd);
              }
              free(send);
              strcpy(ethost, answer->names[0]);
              *port = answer->port;
              *inetaddr = answer->addrs[0];
              freeAnswers(answer_first);
              return ET_OK;
            }
            answer = answer->next;
          }
          
          /* If we're here, we do NOT have a first/local policy with at least
           * 1 remote response.
           */
          
          if (config->policy == ET_POLICY_ERROR) {
            if (remoteresponses == 1) {
if (debug)
    printf("et_findserver: got a match to .remote, error policy\n");
              for (k=0; k<numsockets; k++) {
                close(send[k].sockfd);
              }
              free(send);
              strcpy(ethost, first_remote->names[0]);
              *port = first_remote->port;
              *inetaddr = answer->addrs[0];
              freeAnswers(answer_first);
              return ET_OK;
            }
            else if (remoteresponses > 1) {
              /* too many proper responses, return error */
              fprintf(stderr, "et_findserver: %d responses to broad/multicast -\n",
                      numresponses);
              j = 0;
              answer = answer_first;
              while (answer != NULL) {
                if (strcmp(localuname, answer->uname) == 0) {
                  continue;
                }
                fprintf(stderr, "  RESPONSE %d from %s at %d\n",
                        j++, answer->names[0], answer->port);
                answer = answer->next;
              }
              for (k=0; k<numsockets; k++) {
                close(send[k].sockfd);
              }
              free(send);
              freeAnswers(answer_first);
              return ET_ERROR_TOOMANY;
            }
          }
        }

    } /* else if we have some response(s) ... */
  } /* try another broadcast */
  
  /* if we're here, we got no response from any matching ET system */
  for (k=0; k<numsockets; k++) {
    close(send[k].sockfd);
  }
if (debug)
    printf("et_findserver: no valid response received\n");
  free(send);
  freeAnswers(answer_first);
  
  return ET_ERROR_TIMEOUT;
}


/******************************************************
 * This routine is used with etr_open to find tcp host & port.
 *****************************************************/
int et_findserver(const char *etname, char *ethost, int *port,
                  uint32_t *inetaddr, et_open_config *config)
{
  struct timeval waittime;
  /* wait 0.1 seconds before calling select */
  waittime.tv_sec  = 0;
  waittime.tv_usec = 100000;
  
  return et_findserver2(etname, ethost, port, inetaddr, config, 2, &waittime);
}

/******************************************************
 * This routine is used to quickly see if an ET system
 * is alive by trying to get a response from its UDP
 * broad/multicast thread. This is used only when the
 * ET system is local.
 *****************************************************/
int et_responds(const char *etname)
{
  int   port;
  char  ethost[ET_MAXHOSTNAMELEN];
  uint32_t inetaddr;
  et_openconfig  openconfig; /* opaque structure */
  et_open_config *config;    /* real structure   */
  
  /* by default we'll broadcast to uname subnet */
  et_open_config_init(&openconfig);
  config = (et_open_config *) openconfig;
  
  /* make sure we contact a LOCAL et system of this name! */
  strcpy(config->host, ET_HOST_LOCAL);
  
  /* send only 1 broadcast with a default maximum .01 sec wait */
  if (et_findserver2(etname, ethost, &port, &inetaddr, config, 1, NULL) == ET_OK) {
    /* got a response */
    return 1;
  }
  
  /* no response */
  return 0;
}

/*****************************************************
 * See da.h for the following definitions. This include file is not
 * included here because it drags a horrid lot of irrelevant stuff
 * with it.
 ****************************************************/
#define DT_BANK    0x10
#define DT_LONG    0x01
#define DT_SHORT   0x05
#define DT_BYTE    0x07
#define DT_CHAR    0x03

static int dtswap[] = {0,2,2,0,1,1,0,0, 3,2,3,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                       0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                       0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                       0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                       0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                       0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                       0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
                       0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0};


/******************************************************
 * This is another version of CODAswap and allows
 * specifying separate source and destination buffers
 * as well as a flag telling whether the data was
 * written from a same or different endian machine
 * (i.e. do we swap bank info or not). In the first
 * case, set same_endian = 1, else 0.
 ******************************************************/
int et_CODAswap(int *src, int *dest, int nints, int same_endian)
{
  int   i, j, blen, dtype;
  int   *lp, *lpd;
  short *sp, *spd;

  /* int pointers */
  if (dest == NULL) {
    dest = src;
  }

  i = 0;

  while (i < nints) {
    lp  = &src[i];
    lpd = &dest[i];

    if (same_endian) {
      blen  = src[i] - 1;
      dtype = ((src[i+1])&0xff00)>>8;
    }
    else {
      blen  = ET_SWAP32(src[i]) - 1;
      dtype = ((ET_SWAP32(src[i+1]))&0xff00)>>8;
    }

    *lpd++ = ET_SWAP32(*lp);  /* Swap length */
     lp++;
    *lpd   = ET_SWAP32(*lp);  /* Swap bank header */

    i += 2;

    if(dtype != DT_BANK) {
      switch(dtswap[dtype]) {
      case 0:
        /* No swap */
        i += blen;
        break;
        
      case 1:
        /* short swap */
        sp  = (short *)&src[i];
        spd = (short *)&dest[i];
        for(j=0; j<(blen<<1); j++, sp++) {
          *spd++ = ET_SWAP16(*sp);
        }
        i += blen;
        break;
        
      case 2:
        /* int swap */
        lp  = &src[i];
        lpd = &dest[i];
        for(j=0; j<blen; j++, lp++) {
          *lpd++ = ET_SWAP32(*lp);
        }
        i += blen;
        break;
        
      case 3:
        /* double swap */
        lp  = &src[i];
        lpd = &dest[i];
        for(j=0; j<blen; j++, lp++) {
          *lpd++ = ET_SWAP32(*lp);
        }
        i += blen;
        break;
        
      default:
        /* No swap */
        i += blen;
      }
    }
  }
  return ET_OK;
}
