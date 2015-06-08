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
 *      Header for routines dealing with network communications
 *
 *----------------------------------------------------------------------------*/
 
#ifndef __et_network_h
#define __et_network_h

#ifdef VXWORKS
#include <ioLib.h>       /* writev */
#include <inetLib.h>    /* htonl stuff */
#else
#include <sys/uio.h>     /* writev */
#include <arpa/inet.h>	 /* htonl stuff */
#endif
#include <sys/types.h>	 /* basic system data types */
#include <sys/socket.h>	 /* basic socket definitions */
#include <netinet/in.h>	 /* sockaddr_in{} and other Internet defns */
#include <netinet/tcp.h> /* TCP_NODELAY def */
#include <net/if.h>	 /* find broacast addr */
#include <netdb.h>	 /* herrno */
#ifdef sun
#include <sys/sockio.h>  /* find broacast addr */
#else
#include <sys/ioctl.h>   /* find broacast addr */
#endif

#ifdef	__cplusplus
extern "C" {
#endif

#if defined sun || defined linux || defined VXWORKS
#  define socklen_t int
#endif

#ifdef linux
#ifndef _SC_IOV_MAX
#  define _SC_IOV_MAX _SC_T_IOV_MAX
#endif
#endif

#define	SA                  struct sockaddr
#define LISTENQ             10
#define ET_SOCKBUFSIZE      49640  /* multiple of 1460 - ethernet MSS */
#define ET_IOV_MAX          16     /* minimum for POSIX systems */

/* are 2 nodes the same or different? */
#define ET_NODE_SAME 0
#define ET_NODE_DIFF 1

/* The following is taken from R. Stevens book, UNIX Network Programming,
 * Chapter 16. Used for finding our broadcast address. Yes, we stole the code.
 */
#define	IFI_NAME	16	/* same as IFNAMSIZ in <net/if.h> */
#define	IFI_HADDR	 8	/* allow for 64-bit EUI-64 in future */
#define	IFI_ALIAS	 1	/* ifi_addr is an alias */

struct ifi_info {
  char    ifi_name[IFI_NAME];	/* interface name, null terminated */
  u_char  ifi_haddr[IFI_HADDR];	/* hardware address */
  u_short ifi_hlen;		/* #bytes in hardware address: 0, 6, 8 */
  short   ifi_flags;		/* IFF_xxx constants from <net/if.h> */
  short   ifi_myflags;		/* our own IFI_xxx flags */
  struct sockaddr  *ifi_addr;	/* primary address */
  struct sockaddr  *ifi_brdaddr;/* broadcast address */
  struct sockaddr  *ifi_dstaddr;/* destination address */
  struct ifi_info  *ifi_next;	/* next of these structures */
};

/* our prototypes */
extern int   tcp_listen(unsigned short port);
extern int   tcp_connect(const char *ip_address, unsigned short port);
extern int   Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);

extern int   tcp_read(int fd, void *vptr, int n);
extern int   tcp_write(int fd, const void *vptr, int n);
extern int   tcp_writev(int fd, struct iovec iov[], int nbufs, int iov_max);

extern int   et_CODAswap(long *src, long *dest, int nlongs, int same_endian);
extern int   et_byteorder(void);
extern const char *et_hstrerror(int err);
extern int   et_nodesame(const char *node1, const char *node2);
extern int   et_nodelocality(const char *host);
extern int   et_defaulthost(char *host, int length);
extern int   et_defaultaddress(char *address, int length);
extern int   et_defaultbroadcastaddr(char *baddr);
extern int   et_isLinux(void);
extern int   et_findserver2(const char *etname, char *ethost, unsigned short *port,
			et_open_config *config, int trys,
			struct timeval *waittime);

extern int   et_udpreceive(unsigned short port, const char *address, int cast);

#ifdef	__cplusplus
}
#endif

#endif
