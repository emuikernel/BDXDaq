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

#ifdef VXWORKS
#include <vxWorks.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#ifdef VXWORKS
#include <taskLib.h>
#include <sockLib.h>
#include <inetLib.h>
#include <hostLib.h>
#include <ioLib.h>
#include <time.h>
#include <net/uio.h>
#include <net/if_dl.h>
#else
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/utsname.h>
#endif
#include <netdb.h>
#ifdef sun
#include <sys/filio.h>
#endif
#include "et_private.h"
#include "et_network.h"

/* prototypes */
static struct ifi_info *get_ifi_info(int family, int doaliases);
static void free_ifi_info(struct ifi_info *ifihead);

/*****************************************************/
int tcp_listen(unsigned short port)
{
  int                 listenfd, err;
  const int           debug=0, on=1, size=ET_SOCKBUFSIZE /* bytes */;
  struct sockaddr_in  servaddr;

  err = listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (err < 0) {
    if (debug) fprintf(stderr, "tcp_listen: socket error\n");
    return err;
  }

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family      = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port        = htons(port);

  err = setsockopt(listenfd, IPPROTO_TCP, TCP_NODELAY, (const void *) &on, sizeof(on));
  if (err < 0) {
    close(listenfd);
    if (debug) fprintf(stderr, "tcp_listen: setsockopt error\n");
    return err;
  }
  err = setsockopt(listenfd, SOL_SOCKET, SO_SNDBUF, (const void *) &size, sizeof(size));
  if (err < 0) {
    close(listenfd);
    if (debug) fprintf(stderr, "tcp_listen: setsockopt error\n");
    return err;
  }
  err = setsockopt(listenfd, SOL_SOCKET, SO_RCVBUF, (const void *) &size, sizeof(size));
  if (err < 0) {
    close(listenfd);
    if (debug) fprintf(stderr, "tcp_listen: setsockopt error\n");
    return err;
  }

  err = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &on, sizeof(on));
  if (err < 0) {
    close(listenfd);
    if (debug) fprintf(stderr, "tcp_listen: setsockopt error\n");
    return err;
  }

  err = setsockopt(listenfd, SOL_SOCKET, SO_KEEPALIVE, (const void *) &on, sizeof(on));
  if (err < 0) {
    close(listenfd);
    if (debug) fprintf(stderr, "tcp_listen: setsockopt error\n");
    return err;
  }

  err = bind(listenfd, (SA *) &servaddr, sizeof(servaddr));
  if (err < 0) {
    close(listenfd);
    if (debug) fprintf(stderr, "tcp_listen: bind error\n");
    return err;
  }

  err = listen(listenfd, LISTENQ);
  if (err < 0) {
    close(listenfd);
    if (debug) fprintf(stderr, "tcp_listen: listen error\n");
    return err;
  }

  return(listenfd);
}

/*****************************************************/
int tcp_connect(const char *ip_address, unsigned short port)
{
  int                 sockfd, err;
  const int           debug=1, on=1, size=ET_SOCKBUFSIZE /* bytes */;
  struct sockaddr_in  servaddr;
  struct in_addr      **pptr;
  struct hostent      *hp;
	
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
     if (debug) fprintf(stderr, "tcp_connect: socket error, %s\n", strerror(errno));
     return -1;
  }
	
  err = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (const void *) &on, sizeof(on));
  if (err < 0) {
    close(sockfd);
    if (debug) fprintf(stderr, "tcp_connect: setsockopt error\n");
    return err;
  }
  err = setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (const void *) &size, sizeof(size));
  if (err < 0) {
    close(sockfd);
    if (debug) fprintf(stderr, "tcp_connect: setsockopt error\n");
    return err;
  }
  err = setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (const void *) &size, sizeof(size));
  if (err < 0) {
    close(sockfd);
    if (debug) fprintf(stderr, "tcp_connect: setsockopt error\n");
    return err;
  }
	
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port   = htons(port);

#ifdef VXWORKS

  servaddr.sin_addr.s_addr = hostGetByName(ip_address);
  if(servaddr.sin_addr.s_addr == ERROR) {
    fprintf(stderr, "tcp_connect: unknown server address for host %s\n",ip_address);
    close(sockfd);
    return ERROR;
  }

  if(connect(sockfd,(struct sockaddr *) &servaddr, sizeof(servaddr)) == ERROR) {
    fprintf(stderr, "tcp_connect: error in connect\n");
    close(sockfd);
    return ERROR;
  }    

#else	
  if ((hp = gethostbyname(ip_address)) == NULL) {
    close(sockfd);
    if (debug) fprintf(stderr, "tcp_connect: hostname error - %s\n", et_hstrerror(h_errno));
    return -1;
  }
  pptr = (struct in_addr **) hp->h_addr_list;

  for ( ; *pptr != NULL; pptr++) {
    memcpy(&servaddr.sin_addr, *pptr, sizeof(struct in_addr));
    if ((err = connect(sockfd, (SA *) &servaddr, sizeof(servaddr))) < 0) {
      if (debug) fprintf(stderr, "tcp_connect: error attempting to connect to server\n");
    }
    else {
      /*printf("tcp_connect: connected to server\n"); */
      break;
    }
  }
#endif  


/* for debugging, print out our port */
/*
{
  struct sockaddr_in localaddr;
  socklen_t	  addrlen, len;
  unsigned short  portt;
  
  addrlen = sizeof(localaddr);
  getsockname(sockfd, (SA *) &localaddr, &addrlen);
  portt = ntohs(localaddr.sin_port);
  printf("My Port is %hu\n", portt);
}
*/	
  
  if (err == -1) {
    close(sockfd);
    if (debug) fprintf(stderr, "tcp_connect: socket connect error, %s\n", strerror(errno));
    return -1;
  }
  
  return sockfd;
}

/************************************************************/
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
  int  n;

again:
  if ((n = accept(fd, sa, salenptr)) < 0) {
#ifdef	EPROTO
    if (errno == EPROTO || errno == ECONNABORTED)
#else
    if (errno == ECONNABORTED)
#endif
      goto again;
    else
      fprintf(stderr, "Accept: error, errno = %d\n", errno);
  }
  return(n);
}

/*****************************************************/
int tcp_writev(int fd, struct iovec iov[], int nbufs, int iov_max)
{
  struct iovec *iovp;
  int n_write, n_sent, nbytes, cc, i;
  
  /* determine total # of bytes to be sent */
  nbytes = 0;
  for (i=0; i < nbufs; i++) {
    nbytes += iov[i].iov_len;
  }
  
  n_sent = 0;
  while (n_sent < nbufs) {  
    n_write = ((nbufs - n_sent) >= iov_max)?iov_max:(nbufs - n_sent);
    iovp     = &iov[n_sent];
    n_sent  += n_write;
      
   retry:
    if ( (cc = writev(fd, iovp, n_write)) == -1) {
      if (errno == EWOULDBLOCK) {
        goto retry;
      }
      fprintf(stderr,"tcp_writev(%d,,%d) = writev(%d,,%d) = %d\n",
		fd,nbufs,fd,n_write,cc);
      perror("tcp_writev");
      return(-1);
    }
  }
  return(nbytes);
}

/*****************************************************/
int tcp_write(int fd, const void *vptr, int n)
{
  int		nleft;
  int		nwritten;
  const char	*ptr;

  ptr = (char *) vptr;
  nleft = n;
  
  while (nleft > 0) {
    if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
      if (errno == EINTR) {
        nwritten = 0;		/* and call write() again */
      }
      else {
        return(-1);		/* error */
      }
    }

    nleft -= nwritten;
    ptr   += nwritten;
  }
  return(n);
}

/*****************************************************/
int tcp_read(int fd, void *vptr, int n)
{
  int	nleft;
  int	nread;
  char	*ptr;

  ptr = (char *) vptr;
  nleft = n;
  
  while (nleft > 0) {
    if ( (nread = read(fd, ptr, nleft)) < 0) {
      if (errno == EINTR) {
        nread = 0;		/* and call read() again */
      }
      else {
        return(-1);
      }
    } else if (nread == 0) {
      break;			/* EOF */
    }
    
    nleft -= nread;
    ptr   += nread;
  }
  return(n - nleft);		/* return >= 0 */
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
    fprintf(stderr, "et_byteorder: sizeof(short) = %d\n", sizeof(short));
    return ET_ERROR;
  }
}

/*****************************************************/
const char *et_hstrerror(int err)
{
	if (err == 0)
		return("no error");

	if (err == HOST_NOT_FOUND)
		return("Unknown host");

	if (err == TRY_AGAIN)
		return("Hostname lookup failure");

	if (err == NO_RECOVERY)
		return("Unknown server error");

	if (err == NO_DATA)
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
    
  nodeAddr1 = hostGetByName(node1);
  if(nodeAddr1 == ERROR)
    return ET_ERROR;

  nodeAddr2 = hostGetByName(node2);
  if(nodeAddr2 == ERROR)
    return ET_ERROR;

  if(nodeAddr1 == nodeAddr2)
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
    
  /* Since gethostbyname uses static data and linux does NOT
   * define gethostbyname_r, do things the hard way and save
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

  hostAddr1 = hostGetByName(host);
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
    /* can local operating system share pthread mutexes between processes? */
    if (sysconf(_SC_THREAD_PROCESS_SHARED) == 1) {
      return ET_LOCAL;
    }
    return ET_LOCAL_NOSHARE;
  }
  return ET_REMOTE;
#endif
}

/**************************************************************/
/* Return the default fully qualified host name of this host  */
int et_defaulthost(char *host, int length)
{
#ifdef VXWORKS
  if (host == NULL) {
    fprintf(stderr, "et_defaulthost: bad argument\n");
    return ET_ERROR;
  }

  if (gethostname(host, length) < 0) return ET_ERROR;
  return ET_OK;

#else
  struct utsname myname;
  struct hostent *hptr;
  
  if (host == NULL) {
    fprintf(stderr, "et_defaulthost: bad argument\n");
    return ET_ERROR;
  }

  /* find out the name of the machine we're on */
  if (uname(&myname) < 0) {
    fprintf(stderr, "et_defaulthost: cannot find hostname\n");
    return ET_ERROR;
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
  
  if (address == NULL) {
    fprintf(stderr, "et_defaultaddress: bad argument\n");
    return ET_ERROR;
  }

  /* find out the name of the machine we're on */
  if (uname(&myname) < 0) {
    fprintf(stderr, "et_defaultaddress: cannot find hostname\n");
    return ET_ERROR;
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

/******************************************************
 * et_defaultbroadcastaddr
 * 
 * It is possible to get a list of addresses, then take
 * that list and pick the address which corresponds to
 * the canonical name of the host we're on. However, that
 * runs into problems when 2 domain names refer to the same
 * domain (cebaf.gov & jlab.org). It's possible (check out
 * clon10) that the canonical name of the host and the name
 * of the host given by gethostbyaddr, using the address
 * given by gethostbyname, are different because of this.
 * In this case one could compare names without the domains.
 * 
 ******************************************************/
  
int et_defaultbroadcastaddr(char *baddr)
{
#ifdef VXWORKS

  char name[ET_MAXHOSTNAMELEN];
  union {
    char ip[4];
    int  ipl;
  } u;

  if(gethostname(name,ET_MAXHOSTNAMELEN))
    return ET_ERROR;

  u.ipl = hostGetByName(name);
  if(u.ipl == -1)
    return ET_ERROR;

  u.ip[2] = 0xff;
  u.ip[3] = 0xff;

  sprintf(baddr,"%d.%d.%d.%d",u.ip[0],u.ip[1],u.ip[2],u.ip[3]);
  
  return ET_OK;

#else
  /* hold data from gethostbyname call since it returns static struct */
  struct myhost {
    char            name[ET_MAXHOSTNAMELEN];	/* canonical name */
    struct in_addr  ipaddress[ET_MAXADDRESSES];	/* binary 32bit IPv4 addr */
  } me;
  
  struct ifi_info	*ifi, *ifihead;
  struct sockaddr	*sa;
  struct sockaddr_in	*baddr_in;
  int			i, naddrs, gotmatch=0, gotbackup=0;
  struct utsname	myname;
  struct hostent	*hptr;
  char			**pptr, *val;
 
  /* find out the name of the machine we're on */
  if (uname(&myname) < 0) {
    fprintf(stderr, "et_defaultbroadcastaddr: cannot find hostname\n");
    return ET_ERROR;
  }
  /* get more data about host - ip addresses, aliases, canonical name */
  if ( (hptr = gethostbyname(myname.nodename)) == NULL) {
    fprintf(stderr, "et_defaultbroadcastaddr: cannot find host info\n");
    return ET_ERROR;
  }
  
  strncpy(me.name, hptr->h_name, ET_MAXHOSTNAMELEN);
  me.name[ET_MAXHOSTNAMELEN-1] = '\0';
  
  /* store static info */
  naddrs=0;
  for (pptr = hptr->h_addr_list; *pptr != NULL; pptr++) {
    me.ipaddress[naddrs++] = *((struct in_addr *) *pptr);
    if (naddrs >= ET_MAXADDRESSES) break;
    /* printf("address = %s\n", inet_ntoa(*((struct in_addr *) *pptr))); */
  }
  
  /* look through ET_MAXADDRESSES IPv4 interfaces at most */
  ifihead = ifi = get_ifi_info(AF_INET, ET_MAXADDRESSES);
  if (ifi == NULL) {
    fprintf(stderr, "et_defaultbroadcastaddr: cannot find network interface info\n");
    return ET_ERROR;
  }

  /* See if any of the stored addresses match the interfaces' addresses.
   * If so, compare the hosts' canonical names. If they're the same, we
   * got a match. If not, check to see if the unqualified names are the
   * same. Use this as a backup match in case a full match isn't found.
   */
  for (i=0; i<naddrs; i++) {
  
    for (ifi = ifihead;ifi != NULL; ifi = ifi->ifi_next) {
      
      /* if the interface is up and there's a broadcast address ... */
      if (((ifi->ifi_flags & IFF_UP) > 0) &&
          ((ifi->ifi_flags & IFF_BROADCAST) > 0)) {
	
	/* if there is an address listed ... */
	if ( (sa = ifi->ifi_addr) != NULL) {
	      baddr_in = (struct sockaddr_in *) sa;
	      
	      /* if one of our host's addrs matches the interface addr ... */
	      if (me.ipaddress[i].s_addr == baddr_in->sin_addr.s_addr) {
        	  
		  /* get info listed under this address */
		  hptr = gethostbyaddr((const char *)&baddr_in->sin_addr, sizeof(struct in_addr), AF_INET);

		  /* See if the name associated with this address is the
		   * same as our host's name. If not, see if names without
		   * domains match. If so, store the broadcast address
		   * as a backup option in case we don't find a match.
		   */
		  if (strcmp(me.name, hptr->h_name) != 0) {
		    /* if unqualified names are different, no match */
		    size_t len1, len2;
		    len1 = strcspn(me.name, ".");
		    len2 = strcspn(hptr->h_name, ".");
		    if (len1 != len2) {
		      continue;
		    }
		    
		    if (strncmp(me.name, hptr->h_name, len1) != 0) {
		      continue;
		    }
		    
		    /* unqualified names are same, store broadcast addr */
		    if ( (sa = ifi->ifi_brdaddr) != NULL) {
		      baddr_in = (struct sockaddr_in *) sa;
		      val = inet_ntoa(baddr_in->sin_addr);
        	      strncpy(baddr, val, ET_IPADDRSTRLEN);
/* printf("et_defaultbroadcastaddr: backup broadcast addr = %s\n", val); */
		      gotbackup = 1;
		    }
		    continue;
		  }
/* printf("et_defaultbroadcastaddr: hostname = %s, interface name = %s\n", me.name, hptr->h_name); */

		  /* We get here if this host's canonical name is the
		   * same as the name corresponding to this interface
		   */

		  if ( (sa = ifi->ifi_brdaddr) != NULL) {
		    baddr_in = (struct sockaddr_in *) sa;
		    val = inet_ntoa(baddr_in->sin_addr);
        	    strncpy(baddr, val, ET_IPADDRSTRLEN);
/* printf("et_defaultbroadcastaddr: broadcast addr: %s\n",val); */
		    gotmatch = 1;
		    break;
		  }
	      }
	} /* if there is an address listed ... */
      } /* if the interface is up and there's a broadcast address ... */
      
      else {
	continue;
      }
      
    } /* loop thru interfaces */
    
    if (gotmatch) {
      break;
    }
  } /* loop thru addresses */
  
  /* free memory */
  free_ifi_info(ifihead);
  
  if (gotmatch) {
    return ET_OK;
  }
  else if (gotbackup) {
    return ET_OK;
  }
  return ET_ERROR;
#endif
}


/******************************************************/
static struct ifi_info *get_ifi_info(int family, int doaliases)
{
  struct ifi_info	*ifi, *ifihead, **ifipnext;
  int			sockfd, len, lastlen, flags, myflags;
  char			*ptr, *buf, lastname[IFNAMSIZ], *cptr;
  struct ifconf		ifc;
  struct ifreq		*ifr, ifrcopy;
  struct sockaddr_in	*sinptr;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    fprintf(stderr, "get_ifi_info: socket error, %s.\n", strerror(errno));
    return NULL;
  }

  /* initial buffer size guess */
  len = 10 * sizeof(struct ifreq);
  lastlen = 0;
  
  for ( ; ; ) {
    buf = malloc(len);
    ifc.ifc_len = len;
    ifc.ifc_buf = buf;
    if (ioctl(sockfd, SIOCGIFCONF, &ifc) < 0) {
      if (errno != EINVAL || lastlen != 0) {
        fprintf(stderr, "get_ifi_info: ioctl error\n");
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
    len += sizeof(struct ifreq);	/* increment */
    free(buf);
  }
  
  ifihead     = NULL;
  ifipnext    = &ifihead;
  lastname[0] = 0;

  /*  printf(" Getting network interface info buf = 0x%x length = %d :\n",(int) &buf[0], (int) ifc.ifc_len);*/

  for (ptr = buf; ptr < buf + ifc.ifc_len; ) {
    ifr = (struct ifreq *) ptr;

    switch (ifr->ifr_addr.sa_family) {
#ifdef IPV6
      case AF_INET6:	
	len = sizeof(struct sockaddr_in6);
	break;
#endif
#ifndef linux
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
    /* printf(" Name= %s  Family =0x%x  len = %d\n",  ifr->ifr_name,ifr->ifr_addr.sa_family,len);*/
    ptr += sizeof(ifr->ifr_name) + len;

    /* ignore if not desired address family */
    if (ifr->ifr_addr.sa_family != family) {
      continue;
    }
    
    myflags = 0;
    if ( (cptr = strchr(ifr->ifr_name, ':')) != NULL) {
      *cptr = 0;	/* replace colon will null */
    }
    if (strncmp(lastname, ifr->ifr_name, IFNAMSIZ) == 0) {
      if (doaliases == 0) {
	continue;	/* already processed this interface */
      }
      myflags = IFI_ALIAS;
    }
    memcpy(lastname, ifr->ifr_name, IFNAMSIZ);

    ifrcopy = *ifr;
    ioctl(sockfd, SIOCGIFFLAGS, &ifrcopy);
    flags = ifrcopy.ifr_flags;
    /* ignore if interface not up */
    if ((flags & IFF_UP) == 0) {
	continue;
    }

    ifi = calloc(1, sizeof(struct ifi_info));
    *ifipnext = ifi;		/* prev points to this new one */
    ifipnext  = &ifi->ifi_next;	/* pointer to next one goes here */

    ifi->ifi_flags = flags;	/* IFF_xxx values */
    ifi->ifi_myflags = myflags;	/* IFI_xxx values */
    memcpy(ifi->ifi_name, ifr->ifr_name, IFI_NAME);
    ifi->ifi_name[IFI_NAME-1] = '\0';

    switch (ifr->ifr_addr.sa_family) {
    case AF_INET:
      sinptr = (struct sockaddr_in *) &ifr->ifr_addr;
      if (ifi->ifi_addr == NULL) {
	ifi->ifi_addr = calloc(1, sizeof(struct sockaddr_in));
	memcpy(ifi->ifi_addr, sinptr, sizeof(struct sockaddr_in));

	if (flags & IFF_BROADCAST) {
	  ioctl(sockfd, SIOCGIFBRDADDR, &ifrcopy);
	  sinptr = (struct sockaddr_in *) &ifrcopy.ifr_broadaddr;
	  ifi->ifi_brdaddr = calloc(1, sizeof(struct sockaddr_in));
	  memcpy(ifi->ifi_brdaddr, sinptr, sizeof(struct sockaddr_in));
	}

	if (flags & IFF_POINTOPOINT) {
	  ioctl(sockfd, SIOCGIFDSTADDR, &ifrcopy);
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
  return(ifihead);	/* pointer to first structure in linked list */
}


/******************************************************/
static void free_ifi_info(struct ifi_info *ifihead)
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
    ifinext = ifi->ifi_next;	/* can't fetch ifi_next after free() */
    free(ifi);			/* the ifi_info{} itself */
  }
}


/*******************************************************************/
/* Find all network interfaces, their names & subnets on this host */	
int et_netinfo (et_ifnames *ifnames, et_ifaddrs *ifaddrs, et_subnets *nets)
{
  struct ifi_info	*ifi, *ifihead, *default_ifi=NULL;
  struct sockaddr	*sa;
  struct sockaddr_in	*addr_in;
  struct hostent	*hptr;
  int                   *vxaddr;
  int			i, alreadyhavesubnet=0;
  int                   namecnt=0, addrcnt=0, netcnt=0;
  char			**pptr, *val, vxhost[ET_MAXHOSTNAMELEN], host[ET_MAXHOSTNAMELEN];
  /* help put subnets in order with default first */
  et_subnets            subnets;
  
  /* get fully qualified default hostname of this host */
  et_defaulthost(host, ET_MAXHOSTNAMELEN);

  

  /* look through ET_MAXADDRESSES IPv4 interfaces at most */
  ifihead = ifi = get_ifi_info(AF_INET, ET_MAXADDRESSES);
  if (ifi == NULL) {
    fprintf(stderr, "et_netinfo: cannot find network interface info\n");
    return ET_ERROR;
  }

  for (;ifi != NULL; ifi = ifi->ifi_next) {
    /* ignore loopback interface */
    if (ifi->ifi_flags & IFF_LOOPBACK) {
      continue;
    }
    /* if the interface is up */
    if (ifi->ifi_flags & IFF_UP) {
      /* if there is an address listed ... */
      if ( (sa = ifi->ifi_addr) != NULL) {
#ifdef VXWORKS
	/* Ethernet address starts at the 4th byte of the sockaddr structure
           This corresponds to sa->sa_data[2] */
	vxaddr = (int *)&(sa->sa_data[2]);
	if(hostGetByAddr(*vxaddr,vxhost) < 0) {
	  fprintf(stderr,"et_netinfo: error in hostGetByAddr\n");
	  return ET_ERROR;
	}

	hptr = gethostbyname(vxhost);
	
	/* if this is the default interface, mark it */
	if (strcmp(host, vxhost) == 0) {
	  default_ifi = ifi;
	}
	
#else
        addr_in = (struct sockaddr_in *) sa;
	hptr = gethostbyaddr((const char *)&addr_in->sin_addr,
	                      sizeof(struct in_addr), AF_INET);

	if (hptr == NULL) {
          fprintf(stderr, "et_netinfo: error in gethostbyaddr.\n");
          free_ifi_info(ifihead);
          return ET_ERROR;
	}
	/* if this is the default interface, mark it */
	if (strcmp(host, hptr->h_name) == 0) {
	  default_ifi = ifi;
	}
#endif
	
	/* if we're only interested in hostnames ... */
	if ((ifnames != NULL) && (ifaddrs == NULL)) {
	  strncpy(ifnames->name[namecnt], hptr->h_name, ET_MAXHOSTNAMELEN-1);
	  ifnames->name[namecnt][ET_MAXHOSTNAMELEN-1] = '\0';
	  namecnt++;
	}
	/* else if we're interested in IP addresses ... */
	else if (ifaddrs != NULL) {
	  /* there may be many IP addresses listed under each interface */
	  for (pptr = hptr->h_addr_list; *pptr != NULL; pptr++) {
/* printf("et_netinfo: address = %s\n", inet_ntoa(*((struct in_addr *) *pptr))); */
            strncpy(ifaddrs->addr[addrcnt],
	            inet_ntoa(*((struct in_addr *) *pptr)), ET_IPADDRSTRLEN-1); 
	    ifaddrs->addr[addrcnt][ET_IPADDRSTRLEN-1] = '\0';
	    addrcnt++;
	    /* If interested in both IP addresses and hostnames, make two
	     * lists in which a list of IP addresses has its corresponding
	     * hostname listed in another.
	     */
	    if (ifnames != NULL) {
	      strncpy(ifnames->name[namecnt], hptr->h_name, ET_MAXHOSTNAMELEN-1);
	      ifnames->name[namecnt][ET_MAXHOSTNAMELEN-1] = '\0';
	      namecnt++;
/* printf("et_netinfo:   corresponding hostname = %s\n", hptr->h_name); */
            }
	  }
	}
	
      }

      /* if the interface is broadcast enabled */
      if ((ifi->ifi_flags & IFF_BROADCAST) > 0) {
	/* if there is a broadcast (subnet) address listed ... */
	if ( (sa = ifi->ifi_brdaddr) != NULL) {
	  addr_in = (struct sockaddr_in *) sa;
	  if (nets != NULL) {
	    val = inet_ntoa(addr_in->sin_addr);
	    if (val == NULL) {
              fprintf(stderr, "et_netinfo: error in inet_ntoa\n");
              free_ifi_info(ifihead);
              return ET_ERROR;
	    }
	    
	    /* check to see if we already got this subnet */
	    for (i=0; i<netcnt; i++) {
	      if (strcmp(val, nets->addr[i]) == 0) {
	        /* we already got this one, go to next interface */
		alreadyhavesubnet=1;
		break;
	      }
	    }
	    if (alreadyhavesubnet) {
	      alreadyhavesubnet = 0;
	      continue;
	    }
	    
	    strncpy(nets->addr[netcnt], val, ET_IPADDRSTRLEN-1);
	    nets->addr[netcnt][ET_IPADDRSTRLEN-1] = '\0';
	    strcpy(subnets.addr[netcnt], nets->addr[netcnt]);
	    
	    /* if this is the default interface, put its subnet first */
	    if ((ifi == default_ifi) && (netcnt > 0)) {
	      strcpy(nets->addr[0], subnets.addr[netcnt]);
	      strcpy(nets->addr[netcnt], subnets.addr[0]);
	    }
	    
 	    netcnt++;
	  }
        }
      }
      
    }
  }
    
  if (ifnames != NULL)  ifnames->count = namecnt;
  if (ifaddrs != NULL)  ifaddrs->count = addrcnt;
  if (nets    != NULL)  nets->count    = netcnt;
  
  /* free memory */
  free_ifi_info(ifihead);
  
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

  if ((strcmp(mysystem.sysname, "linux") == 0) ||
      (strcmp(mysystem.sysname, "Linux") == 0) ||
      (strcmp(mysystem.sysname, "LINUX") == 0)    ) {
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
  if (inet_aton(address, &castaddr) == INET_ATON_ERR) {
    fprintf(stderr, "et_udpreceive: inet_aton error\n");
    return -1;
  }

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr   = castaddr;
  servaddr.sin_port   = htons(port);

  /* create socket */
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    fprintf(stderr, "et_udpreceive: socket error\n");
    return -1;
  }
  
  /* allow multiple copies of this to run on same host */
  err = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &on, sizeof(on));
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
    err = setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const void *) &mreq, sizeof(mreq));
    if (err < 0) {
      fprintf(stderr, "et_udpreceive: setsockopt IP_ADD_MEMBERSHIP error\n");
      return err;
    }
  }
    
  /* only allow packets to this port & address to be received */
  err = bind(sockfd, (SA *) &servaddr, sizeof(servaddr));
  if (err < 0) {
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
 * etname:	ET system file name
 * ethost:	returns name of ET system's host
 * config:	configuration passed to et_open
 * trys:	max # of times to broadcast UDP packet
 * waittime:	wait time for response to broad/multicast
 *****************************************************/
int et_findserver2(const char *etname, char *ethost, unsigned short *port,
                              et_open_config *config, int trys,
                              struct timeval *waittime)
{
  int              i, j, k, err, n, version;
  int              length, len_net, lastdelay, maxtrys=6, serverport=0;
  const int        on=1, timeincr[]={0,1,2,3,4,5};
  
  /* encoding & decoding packets */
  char             *pbuf, buffer[1500]; /* full ethernet packet */
  char             outbuf[ET_FILENAME_LENGTH+16];
  char             localhost[ET_MAXHOSTNAMELEN];
  char             specifiedhost[ET_MAXHOSTNAMELEN];

  /* structure for holding a single response to our broad/multicast */
  struct response {
    unsigned short port;
    int            sockfd;
    char           host[ET_MAXHOSTNAMELEN];
    char           unamehost[ET_MAXHOSTNAMELEN];
    char           address[ET_IPADDRSTRLEN];
  };
  /* allocate space for 30 answers by default */
#define MAX_ANSWERS 30
  int numresponses=0, remoteresponses=0, firstremoteresponse=-1;
  struct response answers[MAX_ANSWERS];

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
  
  /* allocate space to store all our outgoing stuff */
  send = (struct senddata *) calloc(config->subnets.count + config->mcastaddrs.count,
                                    sizeof(struct senddata));
  if (send == NULL) {
    fprintf(stderr, "et_findserver: cannot allocate memory\n");
    return ET_ERROR;
  } 
  
  /* get fully qualified default hostname of this host */
  et_defaulthost(localhost, ET_MAXHOSTNAMELEN);
  
  /* If the host is not remote or anywhere out there. If it's
   * local or we know its name, send a UDP packet to it alone.
   */
  /* printf("et_findserver2: Host = %s \n",config->host); */
  if ((strcmp(config->host, ET_HOST_REMOTE) != 0) &&
      (strcmp(config->host, ET_HOST_ANYWHERE) != 0)) {
    
    /* Find host's address */
    struct in_addr  ipaddress;	/* binary 32bit IPv4 addr */
    struct hostent  *hptr=NULL;

    /* if it's local, find address associated with uname */
    if ((strcmp(config->host, ET_HOST_LOCAL) == 0) ||
        (strcmp(config->host, "localhost")   == 0))  {
      
      /* get host address */
      if ( (hptr = gethostbyname(localhost)) == NULL) {
	fprintf(stderr, "et_findserver: cannot find host info\n");
        free(send);
	return ET_ERROR;
      }
    }
    /* else if we know its name ... */
    else {
      /* get host address */
      if ( (hptr = gethostbyname(config->host)) == NULL) {
	fprintf(stderr, "et_findserver: cannot find host info\n");
        free(send);
	return ET_ERROR;
      }
    }
    
    /* store canonical name of specified host to contact */
    strcpy(specifiedhost, hptr->h_name);
    
   /*printf("et_findserver2: send single packet to %s\n", hptr->h_name);*/

    /* always send udp packet directly to udpport, local hostname */
    ipaddress = *((struct in_addr *) *hptr->h_addr_list);

    /* server's location */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr   = ipaddress;
    servaddr.sin_port   = htons(config->udpport);

    /* create socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
      fprintf(stderr, "et_findserver: socket error\n");
      return ET_ERROR;
    }
    
    /* for sending packet and for select */
    send[numsockets].sockfd   = sockfd;
    send[numsockets].servaddr = servaddr;
    numsockets++;
    biggestsockfd = sockfd;
    FD_SET(sockfd, &rset);
    
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

/*  printf("et_findserver: send multicast packet to %s\n", config->mcastaddrs.addr[i]);*/
	/* server's location */
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr   = castaddr;
	servaddr.sin_port   = htons(config->multiport);

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
	  
	  err = setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, (const void *) &ttl, sizeof(ttl));  
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
  
  /* else if the host name is not specified, and it's either
   * remote or anywhere out there, broad/multicast to find it
   */
  else {
    /*printf("et_findserver2: No host name config->cast=%d\n",config->cast);*/
    /* We need 1 socket for each actived subnet if broadcasting */
    if ((config->cast == ET_BROADCAST) ||
	(config->cast == ET_BROADANDMULTICAST)) {
      /* for each listed subnet ... */
      for (i=0; i < config->subnets.count; i++) {
	/* if subnet is to be used ... */
	if (config->activated & 1<<i) {
	  /* put address into net-ordered binary form */
	  if (inet_aton(config->subnets.addr[i], &castaddr) == INET_ATON_ERR) {
	    fprintf(stderr, "et_findserver: inet_aton error net_addr = %s\n", config->subnets.addr[i]);
            for (j=0; j<numsockets; j++) {
	      close(send[j].sockfd);
	    }
            free(send);
	    return ET_ERROR;
	  }

/*    printf("et_findserver: send broadcast packet to %s, port %hu\n",
        config->subnets.addr[i], config->udpport); */
	  /* server's location */
	  bzero(&servaddr, sizeof(servaddr));
	  servaddr.sin_family = AF_INET;
	  servaddr.sin_addr   = castaddr;
	  servaddr.sin_port   = htons(config->udpport);

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
	  err = setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (const void *) &on, sizeof(on));
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

	}
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

/*  printf("et_findserver: send multicast packet to %s\n", config->mcastaddrs.addr[i]);*/
	/* server's location */
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr   = castaddr;
	servaddr.sin_port   = htons(config->multiport);

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
	  
	  err = setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, (const void *) &ttl, sizeof(ttl));  
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
   * (1) ET version #,
   * (2) ET file name length (includes null)
   * (3) ET file name
   *
   * Sending version # first (starting with ET version 4) allows
   * version 3 clients to send to version 4 ET systems which
   * are expecting a filename to come first. Since there will
   * be no match, they won't respond. That is what we want anyway,
   * as they are different versions of ET.
   */
  pbuf = outbuf;
  version = htonl(ET_VERSION);
  memcpy(pbuf, &version, sizeof(version));
  pbuf   += sizeof(version); 
  length  = strlen(etname)+1;
  len_net = htonl(length);
  /* length of etname string */
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
	    return ET_ERROR;
	  }
	  
	  /* limit the # of responses */
	  if (numresponses >= MAX_ANSWERS) break;

	  /* see if there's another packet to be read on this socket */
	  bytes = 0;
	  ioctl(send[j].sockfd, FIONREAD, (void *) &bytes);
	  
	  /* decode packet from ET system:
	   * (1) ET version #,
	   * (2) port of tcp server thread (not udp config->port),
	   * (3) length of next string,
	   * (4) fully qualified hostname of this interface address,
	   * (5) length of next string,
	   * (6) this interface address in dotted-decimal form,
	   * (7) length of next string,
	   * (8) fully qualified hostname from "uname" (used as a
	   *     general identifier of this host no matter which
	   *     interface is used)
	   */
	  answers[numresponses].sockfd = send[j].sockfd;
	  
	  pbuf = buffer;
	  memcpy(&version, pbuf, sizeof(version));
	  version = ntohl(version);
	  pbuf += sizeof(version);

	  memcpy(&serverport, pbuf, sizeof(serverport));
	  serverport = ntohl(serverport);
	  if ((serverport < 1) || (serverport > 65536)) {
	    /* nonsense, if another packet there, try it */
	    if (bytes > 0) {
	      goto anotherpacket;
	    }
	    /* if no other packet, try next socket */
	    continue;
	  }
	  answers[numresponses].port = (unsigned short) serverport;
	  pbuf += sizeof(serverport);

	  memcpy(&length, pbuf, sizeof(length));
	  length = ntohl(length);
	  if ((length < 1) || (length > ET_MAXHOSTNAMELEN)) {
	    if (bytes > 0) {
	      goto anotherpacket;
	    }
	    continue;
	  }
	  pbuf += sizeof(length);
	  memcpy(answers[numresponses].host, pbuf, length);
	  pbuf += length;

	  memcpy(&length, pbuf, sizeof(length));
	  length = ntohl(length);
	  if ((length < 1) || (length > ET_IPADDRSTRLEN)) {
	    if (bytes > 0) {
	      goto anotherpacket;
	    }
	    continue;
	  }
	  pbuf += sizeof(length);
	  memcpy(answers[numresponses].address, pbuf, length);
	  pbuf += length;

	  memcpy(&length, pbuf, sizeof(length));
	  length = ntohl(length);
	  if ((length < 1) || (length > ET_MAXHOSTNAMELEN)) {
	    if (bytes > 0) {
	      goto anotherpacket;
	    }
	    continue;
	  }
	  pbuf += sizeof(length);
	  memcpy(answers[numresponses].unamehost, pbuf, length);
	  
/*  printf("et_findserver: RESPONSE from %s at %hu with addr = %s and uname = %s\n",
        answers[numresponses].host, answers[numresponses].port,
        answers[numresponses].address, answers[numresponses].unamehost);*/

          numresponses++;
	  
	  /* see if there's another packet to be read on this socket */
	  if (bytes > 0) {
	    goto anotherpacket;
	  }
	  
        } /* for each socket */
        
	
	/* If host is local or we know its name. If we're multicasting,
	 * there may be many responses. Look only at the response that
	 * matches the host's name.
	 */
	if ((strcmp(config->host, ET_HOST_REMOTE) != 0) &&
	    (strcmp(config->host, ET_HOST_ANYWHERE) != 0)) {
          /* analyze the answers/responses */
          for (j=0; j < numresponses; j++) {
            /* The problem here is if we are multicasting, another ET
	     * system of the same name may respond, but we're interested
	     * only in the one on the specified host.
	     */
            if (strcmp(specifiedhost, answers[j].host) == 0) {
/*  printf("et_findserver: got a match to local or specific\n");*/
	      for (k=0; k<numsockets; k++) {
		close(send[k].sockfd);
	      }
	      free(send);
	      strcpy(ethost, answers[j].host);
	      *port = answers[j].port;
              return ET_OK;
	    }
	  }
	}

	/* If the host may be anywhere (local or remote) ...  */
	else if (strcmp(config->host, ET_HOST_ANYWHERE) == 0) {
	  /* if our policy is to return an error for more than 1 response ... */
	  if ((config->policy == ET_POLICY_ERROR) && (numresponses > 1)) {
	    /* print error message and return */
	    fprintf(stderr, "et_findserver: %d responses to broad/multicast -\n",
		    numresponses);
	    for (j=0; j < numresponses; j++) {
	      fprintf(stderr, "  RESPONSE %d from %s at %hu\n",
		      j+1, answers[j].host, answers[j].port);
	    }
	    for (k=0; k<numsockets; k++) {
	      close(send[k].sockfd);
	    }
	    free(send);
	    return ET_ERROR_TOOMANY;
	  }

          /* analyze the answers/responses */
          for (j=0; j < numresponses; j++) {

	    /* If our policy is to take the first response do so. If our
             * policy is ET_POLICY_ERROR, we can also return the first as
             * we can only be here if there's been just 1 response.
             */
	    if ((config->policy == ET_POLICY_FIRST) ||
	        (config->policy == ET_POLICY_ERROR))  {
/*  printf("et_findserver: got a match to .anywhere, first or error policy\n");*/
	      for (k=0; k<numsockets; k++) {
		close(send[k].sockfd);
	      }
	      free(send);
	      strcpy(ethost, answers[j].host);
	      *port = answers[j].port;
              return ET_OK;
	    }
	    /* else if our policy is to take the first local response ... */
	    else if (config->policy == ET_POLICY_LOCAL) {
	      if (strcmp(localhost, answers[j].unamehost) == 0) {
/*  printf("et_findserver: got a match to .anywhere, local policy\n");*/
		for (k=0; k<numsockets; k++) {
		  close(send[k].sockfd);
		}
		free(send);
		strcpy(ethost, answers[j].host);
		*port = answers[j].port;
        	return ET_OK;
	      }
	      /* If we went through all responses without finding
	       * a local one, pick the first one we received.
	       */
	      if (j == numresponses-1) {
/*   printf("et_findserver: got a match to .anywhere, nothing local available\n");*/
		for (k=0; k<numsockets; k++) {
		  close(send[k].sockfd);
		}
		free(send);
		strcpy(ethost, answers[0].host);
		*port = answers[0].port;
        	return ET_OK;
	      }
	    }
	  }
	  
	}

	/* if user did not specify host name, and it must be remote ... */
        else if (strcmp(config->host, ET_HOST_REMOTE) == 0) {
          /* analyze the answers/responses */
          for (j=0; j < numresponses; j++) {
            /* The problem here is if we are broadcasting, a local ET
	    * system of the same name may respond, but we're interested
	    * only in the remote systems. Weed out the local system.
	    */
            if (strcmp(localhost, answers[j].unamehost) == 0) {
	      continue;
	    }
	    remoteresponses++;
	    if (firstremoteresponse == -1) {
	      firstremoteresponse = j;
	    }
	    /* The ET_POLICY_LOCAL doesn't make any sense here so
	     * default to ET_POLICY_FIRST
	     */
	    if ((config->policy == ET_POLICY_FIRST) ||
	        (config->policy == ET_POLICY_LOCAL))  {
/*  printf("et_findserver: got a match to .remote, first or local policy\n");*/
	      for (k=0; k<numsockets; k++) {
		close(send[k].sockfd);
	      }
	      free(send);
	      strcpy(ethost, answers[j].host);
	      *port = answers[j].port;
              return ET_OK;
	    }
	  }
	  
	  if (config->policy == ET_POLICY_ERROR) {
	    if (remoteresponses == 1) {
/* printf("et_findserver: got a match to .remote, error policy\n");*/
	      for (k=0; k<numsockets; k++) {
		close(send[k].sockfd);
	      }
	      free(send);
	      strcpy(ethost, answers[firstremoteresponse].host);
              *port = answers[firstremoteresponse].port;
              return ET_OK;
	    }
	    else if (remoteresponses > 1) {
	      /* too many proper responses, return error */
	      fprintf(stderr, "et_findserver: %d responses to broad/multicast -\n",
		      numresponses);
	      for (j=0; j < numresponses; j++) {
        	if (strcmp(localhost, answers[j].unamehost) == 0) {
	          continue;
		}
		fprintf(stderr, "  RESPONSE %d from %s at %hu\n",
			j+1, answers[j].host, answers[j].port);
	      }
	      for (k=0; k<numsockets; k++) {
		close(send[k].sockfd);
	      }
	      free(send);
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
  /*printf(" No response here\n");*/
  free(send);
  return ET_ERROR_TIMEOUT;
}


/******************************************************
 * This routine is used with etr_open to find tcp host & port.
 *****************************************************/
int et_findserver(const char *etname, char *ethost, unsigned short *port,
                  et_open_config *config)
{
  struct timeval waittime;
  /* wait 0.01 seconds before calling select */
  waittime.tv_sec  = 0;
  waittime.tv_usec = 10000;
  
  return et_findserver2(etname, ethost, port, config, 4, &waittime);
}

/******************************************************
 * This routine is used to quickly see if an ET system
 * is alive by trying to get a response from its UDP
 * broad/multicast thread. This is used only when the
 * ET system is local.
 *****************************************************/
int et_responds(const char *etname)
{
  unsigned short port;
  char		 ethost[ET_MAXHOSTNAMELEN];
  et_openconfig  openconfig; /* opaque structure */
  et_open_config *config;    /* real structure   */
  
  /* by default we'll broadcast to uname subnet */
  et_open_config_init(&openconfig);
  config = (et_open_config *) openconfig;
  
  /* make sure we contact a LOCAL et system of this name! */
  strcpy(config->host, ET_HOST_LOCAL);
  
  /* send only 1 broadcast with a default maximum .01 sec wait */
  if (et_findserver2(etname, ethost, &port, config, 1, NULL) == ET_OK) {
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

/*
 *-------------------------------------------------------------------------*
 *   Routine to take a Data buffer in CODA format and perform a
 *   a byte swap based on the data type in the Bank structure headers.
 *
 *   Data is maniputlated in the existing buffer so that function
 *   irreversably mangles the data.
 *   
 *-------------------------------------------------------------------------*
 */
 
void CODAswap(long *cbuf, int nlongs)
{
    int   ii, jj, blen, dtype;
    long  lwd, *lp;
    short shd, *sp;

    ii = 0;
    while (ii<nlongs) {
      lp = (long *)&cbuf[ii];
      blen = cbuf[ii] - 1;
      dtype = ((cbuf[ii+1])&0xff00)>>8;
      lwd = ET_LSWAP(*lp);    /* Swap the length      */
      *lp++ = lwd;         
      lwd = ET_LSWAP(*lp);    /* Swap the bank header */
      *lp++ = lwd;       
      ii += 2;
      if(dtype != DT_BANK) {
	switch(dtswap[dtype]) {
	case 0:
	  /* No swap */
	  ii += blen;
	  break;
	case 1:
	  /* short swap */
	  sp = (short *)&cbuf[ii];
	  for(jj=0; jj<(blen<<1); jj++) {
	    shd = ET_SSWAP(*sp);
	    *sp++ = shd;
	  }
	  ii += blen;
	  break;
	case 2:
          /* long swap */
	  lp = (long *)&cbuf[ii];
	  for(jj=0; jj<blen; jj++) {
	    lwd = ET_LSWAP(*lp);
	    *lp++ = lwd;
	  }
	  ii += blen;
	  break;
	case 3:
	  /* double swap */
	  lp = (long *)&cbuf[ii];
	  for(jj=0; jj<blen; jj++) {
	    lwd = ET_LSWAP(*lp);
	    *lp++ = lwd;
	  }
	  ii += blen;
	  break;
	default:
	  /* No swap */
	  ii += blen;
	}
      }
    }
 return;
}

/******************************************************
 * This is another version of CODAswap and allows
 * specifying separate source and destination buffers
 * as well as a flag telling whether the data was
 * written from a same or different endian machine
 * (i.e. do we swap bank info or not). In the first
 * case, set same_endian = 1, else 0.
 ******************************************************/
int et_CODAswap(long *src, long *dest, int nlongs, int same_endian)
{
  int   i, j, blen, dtype;
  long  *lp, *lpd;
  short *sp, *spd;

  /* long pointers */
  if (dest == NULL) {
    dest = src;
  }

  i = 0;

  while (i < nlongs) {
    lp  = &src[i];
    lpd = &dest[i];

    if (same_endian) {
      blen  = src[i] - 1;
      dtype = ((src[i+1])&0xff00)>>8;
    }
    else {
      blen  = ET_LSWAP(src[i]) - 1;
      dtype = ((ET_LSWAP(src[i+1]))&0xff00)>>8;
    }

    *lpd++ = ET_LSWAP(*lp);  /* Swap length */
     lp++;
    *lpd   = ET_LSWAP(*lp);  /* Swap bank header */

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
	  *spd++ = ET_SSWAP(*sp);
	}
	i += blen;
	break;
	
      case 2:
	/* long swap */
	lp  = &src[i];
	lpd = &dest[i];
	for(j=0; j<blen; j++, lp++) {
	  *lpd++ = ET_LSWAP(*lp);
	}
	i += blen;
	break;
	
      case 3:
	/* double swap */
	lp  = &src[i];
	lpd = &dest[i];
	for(j=0; j<blen; j++, lp++) {
	  *lpd++ = ET_LSWAP(*lp);
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
