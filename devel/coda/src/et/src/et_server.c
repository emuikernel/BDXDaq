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
 *      Routines for TCP and UDP "servers". Threads that listen on UDP sockets
 *	for users trying to find ET systems. Threads for establishing TCP
 *	communications with remote users.
 *
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/select.h>

#include "et_private.h"
#include "et_network.h"
#include "et_data.h"

/* Info passed to each thread serving a network ET client connection */
typedef struct et_threadinfo_t {
  int    connfd;	/* socket connection's fd */
  int    endian;	/* endian of server */
  int    endian_client;	/* endian of client */
  int    iov_max;	/* max iov structs for writev */
  et_id  *id;		/* for passing info on system running server */
} et_threadinfo;

/* prototypes */
static void *et_client_thread(void *arg);
static void *et_listen_thread(void *arg);
static void  et_command_loop(et_threadinfo *info);


/************************************************************
 * This thread starts one thread for each local IP address
 * (from one or more interfaces), each local subnet broadcast
 * address, and each desired multicast address. The aim is that
 * the ET system can be listening on each for a UDP packet
 * looking to find this ET system.
 ************************************************************/
void *et_cast_thread(void *arg)
{
  et_netthread	  *threadarg = (et_netthread *) arg;
  et_sys_config   *config = threadarg->config;
  et_id           *etid   = threadarg->id;
  int             i;
  pthread_attr_t  attr;
  char            unamehost[ET_MAXHOSTNAMELEN], unameaddr[ET_IPADDRSTRLEN];

  /* send signal to main thread that this thread has started */
  etid->race = -1;

  /* get thread attribute ready */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  /* thread for each network interface - listen for broadcasts */
  for (i=0; i<config->ifaddrs.count; i++) {
    /* allocate struct for passing info to listening thread */
    et_netthread *newarg = (et_netthread *) malloc(sizeof(et_netthread));
    if (newarg == NULL) {
      if (etid->debug >= ET_DEBUG_SEVERE) {
        et_logmsg("SEVERE", "et_cast_thread: cannot allocate memory\n");
      }
      exit(1);
    }
    /* ignore loopback address - don't need to listen on that */
    if (strcmp(config->ifaddrs.addr[i], "127.0.0.1") == 0) {
      free(newarg);
      continue;
    }
    /* each thread gets a slightly different arg passed to it */
    newarg->config = config;
    newarg->id = etid;
    newarg->cast = ET_BROADCAST;
    strcpy(newarg->host, config->ifnames.name[i]);
    strcpy(newarg->listenaddr, config->ifaddrs.addr[i]);
    strcpy(newarg->returnaddr, config->ifaddrs.addr[i]);

    pthread_create(&config->ifaddrs.tid[i], &attr, et_listen_thread, (void *) newarg);
  }

  /* get uname & addr of this host */
  if (et_defaulthost(unamehost, ET_MAXHOSTNAMELEN) != ET_OK) {
    strcpy(unamehost, "cannotfindhostname");
  }
  if (et_defaultaddress(unameaddr, ET_IPADDRSTRLEN) != ET_OK) {
    strcpy(unameaddr, "cannotfindaddr");
  }

  /* If we wanted to, we could specify the interface address to accept
   * the multicast packet on, but we'll forget about it for now.
   */

  /* thread for each multicast address */
  for (i=0; i<config->mcastaddrs.count; i++) {
    /* each thread gets a slightly different arg passed to it */
    et_netthread *newarg = (et_netthread *) malloc(sizeof(et_netthread));
    newarg->config = config;
    newarg->id = etid;
    newarg->cast = ET_MULTICAST;
    strcpy(newarg->host, unamehost);
    strcpy(newarg->listenaddr, config->mcastaddrs.addr[i]);
    strcpy(newarg->returnaddr, unameaddr);

    pthread_create(&config->mcastaddrs.tid[i], &attr, et_listen_thread, (void *) newarg);
  }

  /* thread for each subnet address */
  for (i=0; i<config->subnets.count; i++) {
    /* each thread gets a slightly different arg passed to it */
    et_netthread *newarg = (et_netthread *) malloc(sizeof(et_netthread));
    newarg->config = config;
    newarg->id = etid;
    newarg->cast = ET_BROADCAST;
    strcpy(newarg->host, unamehost);
    strcpy(newarg->listenaddr, config->subnets.addr[i]);
    strcpy(newarg->returnaddr, unameaddr);

    pthread_create(&config->subnets.tid[i], &attr, et_listen_thread, (void *) newarg);
  }

  return NULL;
}


/************************************************************/
static void *et_listen_thread(void *arg)
{
  et_netthread	     *threadarg = (et_netthread *) arg;
  et_sys_config      *config    = threadarg->config;
  et_id              *etid      = threadarg->id;
  int                cast       = threadarg->cast;
  char               *addr      = threadarg->listenaddr;
  char               *retaddr   = threadarg->returnaddr;
  char               *host      = threadarg->host;

  int                version, sockfd, nbytes, length, port;
  int                len_host, len_address, len_uname;
  int                host_len, address_len, uname_len;
  size_t             bufsize;
  char               *outbuf, *pbuf, inbuf[ET_FILENAME_LENGTH+4];
  char	             filename[ET_FILENAME_LENGTH];
  char               unamehost[ET_MAXHOSTNAMELEN];
  socklen_t          len;
  struct sockaddr_in cliaddr;
#ifdef sun
  int con;
#endif

  /* setup socket for receiving udp packets */
  sockfd = et_udpreceive(config->port, addr, cast);
  if (sockfd < 0) {
    if (etid->debug >= ET_DEBUG_SEVERE) {
      et_logmsg("SEVERE", "et_listen_thread: problem opening socket\n");
    }
    exit(1);
  }

#ifdef sun
  /* increase concurrency for this thread */
  con = thr_getconcurrency();
  thr_setconcurrency(con + 1);
#endif

  /* Prepare output buffer we send in answer to inquiries:
   * (1) ET version #,
   * (2) port of tcp server thread (not udp config->port),
   * (3) length of next string,
   * (4) fully qualified hostname of this interface address
   *     (from uname if multicasting),
   * (5) length of next string
   * (6) this interface's address in dotted-decimal form,
   * (7) length of next string,
   * (8) fully qualified hostname from "uname" (used as a
   *     general identifier of this host no matter which
   *     interface is used)
   */

  version  = htonl(etid->version);
  bufsize  = sizeof(version);

  port     = htonl((int) etid->sys->port);
  bufsize += sizeof(port);

  host_len = strlen(host)+1;
  len_host = htonl(host_len);
  bufsize += sizeof(len_host) + host_len;

  address_len = strlen(retaddr)+1;
  len_address = htonl(address_len);
  bufsize    += sizeof(len_address) + address_len;

  /* get uname of this host */
  if (et_defaulthost(unamehost, ET_MAXHOSTNAMELEN) != ET_OK) {
    len_uname = uname_len = 0;
  }
  else {
    uname_len = strlen(unamehost)+1;
    len_uname = htonl(uname_len);
  }
  bufsize += sizeof(len_uname) + uname_len;

  /* allocate packet's buffer */
  if ( (pbuf = outbuf = (char *) malloc(bufsize)) == NULL) {
    if (etid->debug >= ET_DEBUG_SEVERE) {
      et_logmsg("SEVERE", "et_listen_thread: cannot allocate memory\n");
    }
    exit(1);
  }

  /* put data into buffer */
  memcpy(pbuf, &version, sizeof(version));
  pbuf += sizeof(version);
  memcpy(pbuf, &port, sizeof(port));
  pbuf += sizeof(port);
  memcpy(pbuf, &len_host, sizeof(len_host));
  pbuf += sizeof(len_host);
  memcpy(pbuf, host, host_len);
  pbuf += host_len;
  memcpy(pbuf, &len_address, sizeof(len_address));
  pbuf += sizeof(len_address);
  memcpy(pbuf, retaddr, address_len);
  pbuf += address_len;
  if (uname_len) {
    memcpy(pbuf, &len_uname, sizeof(len_uname));
    pbuf += sizeof(len_uname);
    memcpy(pbuf, unamehost, uname_len);
  }

  /* release memory allocated in et_cast_thread */
  /* free(arg); */

  for ( ; ; ) {
    len = sizeof(cliaddr);

    /* read incoming data */
    nbytes = recvfrom(sockfd, (void *) inbuf, ET_FILENAME_LENGTH+4,
    			   0, (SA *) &cliaddr, &len);
    if (nbytes < 0) {
      if (etid->debug >= ET_DEBUG_ERROR) {
        et_logmsg("ERROR", "et_listen_thread: error in recvfrom\n");
      }
      /* try listening for another packet */
      continue;
    }

    /* decode the data:
     * (1) ET version #,
     * (2) length of string,
     * (3) ET file name
     */
    memcpy(&version, inbuf, sizeof(version));
    version = ntohl(version);
    /* check version number */
    if (version !=  etid->version) {
      /* wrong version, listen for next packet */
      continue;
    }
    memcpy(&length, inbuf+sizeof(version), sizeof(length));
    length = ntohl(length);
    /* Check to see if we have a reasonable length. There may
     * be problems here since ET version 3 did not send version
     * number in the packet.
     */
    if ((length < 1) || (length > ET_FILENAME_LENGTH)) {
      /* not proper format, listen for next packet */
      continue;
    }
    memcpy(filename, inbuf+sizeof(version)+sizeof(length), length);
/*
printf("et_listen_thread: received packet on %s @ %s for %s\n",
        host, addr, filename);
*/
    /* check if the ET system the client wants is ours */
    if (strcmp(filename, etid->sys->config.filename) == 0) {
      /* if we are the one the client is looking for, send a reply */
      sendto(sockfd, (void *) outbuf, bufsize, 0, (SA *) &cliaddr, len);
    }
  }
}

/************************************************************
 * et_netserver is a server for remote clients and for local
 * clients that cannot access the mutexes from shared memory
 * because the operating system does not support such sharing.
 **************************************************************/
void *et_netserver(void *arg)
{
  et_netthread	  *threadarg = (et_netthread *) arg;
  et_sys_config   *config = threadarg->config;
  et_id           *etid   = threadarg->id;
  int             listenfd=0, endian, iov_max;
  unsigned short  i, port=0, trylimit=2000;
  struct sockaddr_in cliaddr;
  socklen_t	  addrlen, len;
  pthread_t       tid;
  pthread_attr_t  attr;
  /* pointer to information to be passed to threads */
  et_threadinfo   *pinfo;

  /* find servers's endian value */
  if ( (endian = et_byteorder()) == ET_ERROR) {
    if (etid->debug >= ET_DEBUG_SEVERE) {
      et_logmsg("SEVERE", "et_netserver: strange byteorder\n");
    }
    exit(1);
  }

  /* find servers's iov_max value */
  if ( (iov_max = sysconf(_SC_IOV_MAX)) < 0) {
    /* set it to POSIX minimum by default (it always bombs on Linux) */
    iov_max = ET_IOV_MAX;
  }

  /* get thread attribute ready */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  /* open a listening socket */

  /* if a server port was explicitly specified, use it and nothing else */
  if (config->serverport > 0) {
    listenfd = tcp_listen(config->serverport);
    if (listenfd < 0) {
      if (etid->debug >= ET_DEBUG_SEVERE) {
	et_logmsg("SEVERE", "et_netserver: specified port is busy, cannot start server thread\n");
      }
      exit(1);
    }
    port = config->serverport;
  }
  /* else, start with default & keeping trying different port #s until one works */
  else {
    for (i=0; i < trylimit; i++) {
      listenfd = tcp_listen(ET_SERVER_PORT+i);
      if (listenfd < 0) {
	if (etid->debug >= ET_DEBUG_INFO) {
	  et_logmsg("INFO", "et_netserver: tried but could not listen on port %d\n", ET_SERVER_PORT+i);
	}
	continue;
      }
      else {
	port = ET_SERVER_PORT+i;
	break;
      }
    }
  }

  if (etid->debug >= ET_DEBUG_INFO) {
    et_logmsg("INFO", "et_netserver: am listening on port %hu\n", port);
  }

  if (listenfd < 0) {
    if (etid->debug >= ET_DEBUG_SEVERE) {
      et_logmsg("SEVERE", "et_netserver: all ports busy, cannot start server thread\n");
    }
    exit(1);
  }

  /* save this port # in the shared memory so clients can get to it */
  etid->sys->port = port;

  /* send signal that thread started */
  etid->race = -1;

  /* spawn threads to deal with each client */
  addrlen = sizeof(cliaddr);
  for ( ; ; ) {
    len = addrlen;

    /* allocate argument to pass to thread */
    pinfo = (et_threadinfo *) malloc(sizeof(et_threadinfo));
    if (pinfo == NULL) {
      if (etid->debug >= ET_DEBUG_SEVERE) {
        et_logmsg("SEVERE", "et_netserver: cannot allocate memory\n");
      }
      exit(1);
    }
    /* set values to pass on to thread */
    pinfo->endian  = endian;
    pinfo->iov_max = iov_max;
    pinfo->id      = etid;

    /* wait for connection to client */
    pinfo->connfd  = Accept(listenfd, (SA *) &cliaddr, &len);
    if (pinfo->connfd < 0) {
      if (etid->debug >= ET_DEBUG_ERROR) {
        et_logmsg("ERROR", "et_netserver: error accepting client connection\n");
      }
      free(pinfo);
      continue;
    }

    /* create thread to deal with client */
    pthread_create(&tid, &attr, et_client_thread, (void *) pinfo);
  }
}


/************************************************************/
static void *et_client_thread(void *arg)
{
  int  connfd, endian, err, length;
  int  outgoing[9], incoming[4];
  char et_name[ET_FILENAME_LENGTH];
  et_threadinfo    info;
  et_id            *etid;
#ifdef sun
  int  con;
#endif

  info   = *((et_threadinfo *) arg);
  connfd = info.connfd;
  etid   = info.id;
  free(arg);

  /* read data from client, extra room for future if necessary */
  if (tcp_read(connfd, incoming, sizeof(incoming)) != sizeof(incoming)) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_client_thread: read failure\n");
    }
    err = ET_ERROR_READ;
    goto error;
  }
  endian = ntohl(incoming[0]);
  length = ntohl(incoming[1]);

  /* store client endian info */
  info.endian_client = endian;

  /* read ET filename */
  if (tcp_read(connfd, et_name, length) != length) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_client_thread: read failure\n");
    }
    err = ET_ERROR_READ;
    goto error;
  }

  /*
   * Check to see if the ET system that the client is trying to
   * connect to is this one (as it should be).
   */
  if (strcmp(et_name, etid->sys->config.filename) != 0) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_client_thread: Sorry! May only connect to %s\n", etid->sys->config.filename);
    }
    err = ET_ERROR;
    goto error;
  }

  /* send ET system info back to client */
  outgoing[0] = htonl(ET_OK);
  outgoing[1] = htonl(info.endian);
  outgoing[2] = htonl(etid->sys->config.nevents);
  outgoing[3] = htonl(etid->sys->config.event_size);
  outgoing[4] = htonl(etid->version);
  outgoing[5] = htonl(etid->nselects);
  outgoing[6] = htonl(etid->lang);
  /* not used */
  outgoing[7] = 0;
  outgoing[8] = 0;

  if (tcp_write(connfd, (void *) outgoing, sizeof(outgoing)) != sizeof(outgoing)) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_client_thread: write failure\n");
    }
    close(connfd);
    pthread_exit(NULL);
  }

#ifdef sun
  /* increase concurrency for this thread */
  con = thr_getconcurrency();
  thr_setconcurrency(con + 1);
#endif

  /* wait for and process client requests */
  et_command_loop(&info);

  /* we are done with connected socket */
  close(connfd);

#ifdef sun
  /* decrease concurrency as this thread disappears */
  con = thr_getconcurrency();
  thr_setconcurrency(con - 1);
#endif

  /* quit thread */
  pthread_exit(NULL);

  error:
    /* if connection is NOT shut down, send reply */
    if (err == ET_ERROR) {
      err = htonl(err);
      if (tcp_write(connfd, (void *) &err, sizeof(err)) != sizeof(err)) {
	if (etid->debug >= ET_DEBUG_ERROR) {
	  et_logmsg("ERROR", "et_client_thread: write failure\n");
	}
      }
    }
    close(connfd);
    pthread_exit(NULL);
}

/************************************************************/
static void et_command_loop(et_threadinfo *info)
{
  int i, connfd, command, err, error, nevents_max, event_size, iov_max;
  int *histogram=NULL, *header=NULL, attaches[ET_ATTACHMENTS_MAX];
  et_event     **events = NULL;
  struct iovec *iov;
  et_id        *etid = info->id;
  et_sys_id     id = (et_sys_id) info->id;

  connfd      = info->connfd;
  iov_max     = info->iov_max;
  event_size  = etid->sys->config.event_size;
  nevents_max = etid->sys->config.nevents;

  /*
   * Keep track of all the attachments this client makes
   * as they may need to be detached if the client dies
   * without cleanly disconnecting itself. Detaching
   * takes care of all events that were sent to clients
   * as events to be modified, but were never put back.
   */
  for (i=0; i < ET_ATTACHMENTS_MAX; i++) {
    attaches[i] = -1;
  }

  /*
   * Allocate things ahead of time so it doesn't need
   * to be done again with each remote command.
   */
  if ( (iov = (struct iovec *) calloc((2*nevents_max)+1, sizeof(struct iovec))) == NULL) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_command_loop: cannot allocate memory\n");
    }
    return;
  }

  if ( (header = (int *) calloc(nevents_max, (6+ET_STATION_SELECT_INTS)*sizeof(int))) == NULL) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_command_loop: cannot allocate memory\n");
    }
    free(iov);
    return;
  }

  /* allocate channels for each event + zero events + 1 more for err */
  if ( (histogram = (int *) calloc(nevents_max+2, sizeof(int))) == NULL) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_command_loop: cannot allocate memory\n");
    }
    free(iov); free(header);
    return;
  }

  if ( (events = (et_event **) calloc(nevents_max, sizeof(et_event *))) == NULL) {
    if (etid->debug >= ET_DEBUG_ERROR) {
      et_logmsg("ERROR", "et_command_loop: cannot allocate memory\n");
    }
    free(iov); free(header); free(histogram);
    return;
  }

  /* The Command Loop ... */
  while (1) {

    /* first, read the remote command */
    if (tcp_read(connfd, &command, sizeof(command)) != sizeof(command)) {
      /*if (etid->debug >= ET_DEBUG_ERROR) {
        et_logmsg("ERROR", "et_command_loop: error reading command\n");
      }*/
      goto end;
    }
    command = ntohl(command);

    /* init error value */
    error = ET_OK;

    /* Since there are so many commands, break up things up a bit */
    if (command < ET_NET_STAT_GATTS) {
      /* Notice that for the commands ending in "_L", which is the local
       * Linux stuff, no swapping (ntohl or htonl) is necessary since it's
       * all local communication.
       */

      switch (command) {

        case  ET_NET_EV_GET_L:
        {
          et_att_id  att;
	  int wait, incoming[4], transfer[2];
	  struct timespec deltatime;
	  et_event *event;

          if (tcp_read(connfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
            goto end;
          }

          att  = incoming[0];
          wait = incoming[1];

	  if (wait == ET_TIMED) {
            deltatime.tv_sec  = incoming[2];
            deltatime.tv_nsec = incoming[3];
            err = et_event_get(id, att, &event, wait, &deltatime);
	  }
	  else if (wait == ET_SLEEP) {
	    /* There's a problem if we have a remote client that is waiting
	     * for another event by sleeping and the events stop flowing. In
	     * that case, the client can be killed and the ET system does NOT
	     * know about it. Since this thread will be stuck in et_event_get,
	     * it will not immediately detect the break in the socket - at least
	     * not until event start flowing again. To circumvent this, implement
	     * ET_SLEEP by repeats of ET_TIMED every couple seconds to allow
	     * detection of broken socket between calls to et_event_get.
	     */
	    struct timeval timeout;
	    fd_set myset;
            etid->sys->attach[att].sleep = ET_ATT_SLEEP;
	    FD_ZERO(&myset);
	    err = ET_ERROR_TIMEOUT;

            /* 3 second timed wait */
	    deltatime.tv_sec  = 3;
            deltatime.tv_nsec = 0;

	    while (err == ET_ERROR_TIMEOUT) {
	      /* Linux modifies timeout, so reset each round */
	      timeout.tv_sec  = 0;
	      timeout.tv_usec = 0;
	      FD_SET(connfd, &myset);

              /* before waiting 3 sec in "get", check here if we're to wake up */
	      if (etid->sys->attach[att].quit == ET_ATT_QUIT) {
	          etid->sys->attach[att].sleep = ET_ATT_NOSLEEP;
                  etid->sys->attach[att].quit  = ET_ATT_CONTINUE;
		  err = ET_ERROR_WAKEUP;
		  break;
	      }
	      
	      err = et_event_get(id, att, &event, ET_TIMED, &deltatime);

	      /* Async call to select to see if this socket is still open.
	       * Ready to read only if socket error in this case.
	       */
	      if (err == ET_ERROR_TIMEOUT) {
		if (select(connfd + 1, &myset, NULL, NULL, &timeout) > 0) {
	          goto end;
		}
	      }
	      else {
	        etid->sys->attach[att].sleep = ET_ATT_NOSLEEP;
                etid->sys->attach[att].quit  = ET_ATT_CONTINUE;
	      }
	      
	    }
	  }
	  else {
	    err = et_event_get(id, att, &event, wait, NULL);
	  }

	  transfer[0] = err;
	  transfer[1] = (int) event;

	  if (tcp_write(connfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
            goto end;
          }
	}
	break;

        case  ET_NET_EVS_GET_L:
        {
          et_att_id  att;
	  int wait, num, nevents, incoming[5];
          struct iovec iov[2];
	  struct timespec deltatime;

          if (tcp_read(connfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
            goto end;
          }
          att  = incoming[0];
          wait = incoming[1];
          num  = incoming[2];

	  if (wait == ET_TIMED) {
            deltatime.tv_sec  = incoming[3];
            deltatime.tv_nsec = incoming[4];
	    err = et_events_get(id, att, events, wait, &deltatime, num, &nevents);
	  }
	  else if (wait == ET_SLEEP) {
	    /* There's a problem if we have a Linux client that is waiting
	     * for another event by sleeping and the events stop flowing. In
	     * that case, the client can be killed and the ET system does NOT
	     * know about it. Since this thread will be stuck in et_events_get,
	     * it will not immediately detect the break in the socket - at least
	     * not until event start flowing again. To circumvent this, implement
	     * ET_SLEEP by repeats of ET_TIMED every couple seconds to allow
	     * detection of broken socket between calls to et_events_get.
	     */
	    struct timeval timeout;
	    fd_set myset;
            etid->sys->attach[att].sleep = ET_ATT_SLEEP;
	    FD_ZERO(&myset);
	    err = ET_ERROR_TIMEOUT;

            /* 3 second timed wait */
	    deltatime.tv_sec  = 3;
            deltatime.tv_nsec = 0;

	    while (err == ET_ERROR_TIMEOUT) {
	      /* Linux modifies timeout, so reset each round */
	      timeout.tv_sec  = 0;
	      timeout.tv_usec = 0;
	      FD_SET(connfd, &myset);

              /* before waiting 3 sec in "get", check here if we're to wake up */
	      if (etid->sys->attach[att].quit == ET_ATT_QUIT) {
	          etid->sys->attach[att].sleep = ET_ATT_NOSLEEP;
                  etid->sys->attach[att].quit  = ET_ATT_CONTINUE;
		  err = ET_ERROR_WAKEUP;
		  break;
	      }
	      
	      err = et_events_get(id, att, events, ET_TIMED, &deltatime, num, &nevents);

	      /* Async call to select to see if this socket is still open.
	       * Ready to read only if socket error in this case.
	       */
	      if (err == ET_ERROR_TIMEOUT) {
		if (select(connfd + 1, &myset, NULL, NULL, &timeout) > 0) {
	          goto end;
		}
	      }
	      else {
	        etid->sys->attach[att].sleep = ET_ATT_NOSLEEP;
                etid->sys->attach[att].quit  = ET_ATT_CONTINUE;
	      }
	    }
	  }
	  else {
	    err = et_events_get(id, att, events, wait, NULL, num, &nevents);
	  }

	  if (err < 0) {
	    if (tcp_write(connfd, (void *) &err, sizeof(err)) != sizeof(err)) {
              goto end;
            }
	    break;
	  }

	  iov[0].iov_base = (void *) &nevents;
	  iov[0].iov_len  = sizeof(nevents);
	  iov[1].iov_base = (void *) events;
	  iov[1].iov_len  = nevents*sizeof(et_event *);

	  if (tcp_writev(connfd, iov, 2, iov_max) == -1) {
            goto end;
	  }
	}
	break;

        case  ET_NET_EV_PUT_L:
        {
	  int incoming[2];
	  et_event    *pe;
          et_att_id   att;

	  if (tcp_read(connfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
            goto end;
	  }

	  att = incoming[0];
	  pe  = (et_event *) incoming[1];

	  err = et_event_put(id, att, pe);

          if (tcp_write(connfd, (void *) &err, sizeof(err)) != sizeof(err)) {
            goto end;
          }
	}
	break;

        case  ET_NET_EVS_PUT_L:
	{
	  int len, nevents, incoming[2];
          et_att_id att;

	  if (tcp_read(connfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
            goto end;
	  }
	  att     = incoming[0];
	  nevents = incoming[1];
	  len = nevents*sizeof(int);

	  if (tcp_read(connfd, (void *) events, len) != len) {
            goto end;
	  }

	  err = et_events_put(id, att, events, nevents);

          if (tcp_write(connfd, (void *) &err, sizeof(err)) != sizeof(err)) {
            goto end;
          }
	}
	break;

        case  ET_NET_EV_NEW_L:
        {
          et_att_id  att;
	  int mode, size, incoming[5], transfer[2];
	  struct timespec deltatime;
	  et_event *event;

          if (tcp_read(connfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
            goto end;
          }

          att  = incoming[0];
          mode = incoming[1];
	  size = incoming[2];

	  if (mode == ET_TIMED) {
            deltatime.tv_sec  = incoming[3];
            deltatime.tv_nsec = incoming[4];
            err = et_event_new(id, att, &event, mode, &deltatime, size);
	  }
	  else if (mode == ET_SLEEP) {
	    /* There's a problem if we have a remote client that is waiting
	     * for a new event by sleeping and the events stop flowing. In
	     * that case, the client can be killed and the ET system does NOT
	     * know about it. Since this thread will be stuck in et_event_new,
	     * it will not immediately detect the break in the socket - at least
	     * not until event start flowing again. To circumvent this, implement
	     * ET_SLEEP by repeats of ET_TIMED every couple seconds to allow
	     * detection of broken socket between calls to et_event_new.
	     */
	    struct timeval timeout;
	    fd_set myset;
            etid->sys->attach[att].sleep = ET_ATT_SLEEP;
	    FD_ZERO(&myset);
	    err = ET_ERROR_TIMEOUT;

            /* 3 second timed wait */
	    deltatime.tv_sec  = 3;
            deltatime.tv_nsec = 0;

	    while (err == ET_ERROR_TIMEOUT) {
	      /* Linux modifies timeout, so reset each round */
	      timeout.tv_sec  = 0;
	      timeout.tv_usec = 0;
	      FD_SET(connfd, &myset);

              /* before waiting 3 sec in "get", check here if we're to wake up */
	      if (etid->sys->attach[att].quit == ET_ATT_QUIT) {
	          etid->sys->attach[att].sleep = ET_ATT_NOSLEEP;
                  etid->sys->attach[att].quit  = ET_ATT_CONTINUE;
		  err = ET_ERROR_WAKEUP;
		  break;
	      }
	      
	      err = et_event_new(id, att, &event, ET_TIMED, &deltatime, size);

	      /* Async call to select to see if this socket is still open.
	       * Ready to read only if socket error in this case.
	       */
	      if (err == ET_ERROR_TIMEOUT) {
		if (select(connfd + 1, &myset, NULL, NULL, &timeout) > 0) {
	          goto end;
		}
	      }
	      else {
	        etid->sys->attach[att].sleep = ET_ATT_NOSLEEP;
                etid->sys->attach[att].quit  = ET_ATT_CONTINUE;
	      }
	    }
	  }
	  else {
	    err = et_event_new(id, att, &event, mode, NULL, size);
	  }

	  transfer[0] = err;
	  transfer[1] = (int) event;

	  if (tcp_write(connfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
            goto end;
          }
	}
	break;

        case  ET_NET_EVS_NEW_L:
        {
          et_att_id  att;
	  int mode, size, nevents, num, incoming[6];
          struct iovec iov[2];
	  struct timespec deltatime;

          if (tcp_read(connfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
            goto end;
          }

          att  = incoming[0];
          mode = incoming[1];
          size = incoming[2];
	  num  = incoming[3];

	  if (mode == ET_TIMED) {
            deltatime.tv_sec  = incoming[4];
            deltatime.tv_nsec = incoming[5];
            err = et_events_new(id, att, events, mode, &deltatime, size, num, &nevents);
	  }
	  else if (mode == ET_SLEEP) {
	    /* There's a problem if we have a remote client that is waiting
	     * for a new event by sleeping and the events stop flowing. In
	     * that case, the client can be killed and the ET system does NOT
	     * know about it. Since this thread will be stuck in et_events_new,
	     * it will not immediately detect the break in the socket - at least
	     * not until event start flowing again. To circumvent this, implement
	     * ET_SLEEP by repeats of ET_TIMED every couple seconds to allow
	     * detection of broken socket between calls to et_events_new.
	     */
	    struct timeval timeout;
	    fd_set myset;
            etid->sys->attach[att].sleep = ET_ATT_SLEEP;
	    FD_ZERO(&myset);
	    err = ET_ERROR_TIMEOUT;

            /* 3 second timed wait */
	    deltatime.tv_sec  = 3;
            deltatime.tv_nsec = 0;

	    while (err == ET_ERROR_TIMEOUT) {
	      /* Linux modifies timeout, so reset each round */
	      timeout.tv_sec  = 0;
	      timeout.tv_usec = 0;
	      FD_SET(connfd, &myset);

              /* before waiting 3 sec in "get", check here if we're to wake up */
	      if (etid->sys->attach[att].quit == ET_ATT_QUIT) {
	          etid->sys->attach[att].sleep = ET_ATT_NOSLEEP;
                  etid->sys->attach[att].quit  = ET_ATT_CONTINUE;
		  err = ET_ERROR_WAKEUP;
		  break;
	      }
	      
	      err = et_events_new(id, att, events, ET_TIMED, &deltatime, size, num, &nevents);

	      /* Async call to select to see if this socket is still open.
	       * Ready to read only if socket error in this case.
	       */
	      if (err == ET_ERROR_TIMEOUT) {
		if (select(connfd + 1, &myset, NULL, NULL, &timeout) > 0) {
	          goto end;
		}
	      }
	      else {
	        etid->sys->attach[att].sleep = ET_ATT_NOSLEEP;
                etid->sys->attach[att].quit  = ET_ATT_CONTINUE;
	      }
	    }
	  }
	  else {
	    err = et_events_new(id, att, events, mode, NULL, size, num, &nevents);
	  }

	  if (err < 0) {
	    if (tcp_write(connfd, (void *) &err, sizeof(err)) != sizeof(err)) {
              goto end;
            }
	    break;
	  }

	  iov[0].iov_base = (void *) &nevents;
	  iov[0].iov_len  = sizeof(nevents);
	  iov[1].iov_base = (void *) events;
	  iov[1].iov_len  = nevents*sizeof(et_event *);

	  if (tcp_writev(connfd, iov, 2, iov_max) == -1) {
            goto end;
	  }
	}
	break;

        case  ET_NET_EV_DUMP_L:
        {
	  int incoming[2];
	  et_event    *pe;
          et_att_id   att;

	  if (tcp_read(connfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
            goto end;
	  }

	  att = incoming[0];
	  pe  = (et_event *) incoming[1];

	  err = et_event_dump(id, att, pe);

          if (tcp_write(connfd, (void *) &err, sizeof(err)) != sizeof(err)) {
            goto end;
          }
	}
	break;

        case  ET_NET_EVS_DUMP_L:
	{
	  int len, nevents, incoming[2];
          et_att_id att;

	  if (tcp_read(connfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
            goto end;
	  }
	  att     = incoming[0];
	  nevents = incoming[1];
	  len = nevents*sizeof(int);

	  if (tcp_read(connfd, (void *) events, len) != len) {
            goto end;
	  }

	  err = et_events_dump(id, att, events, nevents);

          if (tcp_write(connfd, (void *) &err, sizeof(err)) != sizeof(err)) {
            goto end;
          }
	}
	break;

        case  ET_NET_EV_GET:
        {
          et_att_id  att;
	  int i, wait, modify, dumpEvents, incoming[5], header[7+ET_STATION_SELECT_INTS];
	  struct timespec deltatime;
          struct iovec iov[2];
	  et_event *event;

          if (tcp_read(connfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
            goto end;
          }
          att    = ntohl(incoming[0]);
          wait   = ntohl(incoming[1]);
          modify = ntohl(incoming[2]);
	  /* Do we put or dump events back into ET system? */
	  dumpEvents = modify & ET_DUMP;
	  /* Are we going to modify the event or its header remotely? */
          modify &= (ET_MODIFY | ET_MODIFY_HEADER);
	  
	  if (wait == ET_TIMED) {
            deltatime.tv_sec  = ntohl(incoming[3]);
            deltatime.tv_nsec = ntohl(incoming[4]);
            err = et_event_get(id, att, &event, wait, &deltatime);
	  }
	  else if (wait == ET_SLEEP) {
	    /* There's a problem if we have a remote client that is waiting
	     * for another event by sleeping and the events stop flowing. In
	     * that case, the client can be killed and the ET system does NOT
	     * know about it. Since this thread will be stuck in et_event_get,
	     * it will not immediately detect the break in the socket - at least
	     * not until event start flowing again. To circumvent this, implement
	     * ET_SLEEP by repeats of ET_TIMED every couple seconds to allow
	     * detection of broken socket between calls to et_event_get.
	     */
	    struct timeval timeout;
	    fd_set myset;
            etid->sys->attach[att].sleep = ET_ATT_SLEEP;
	    FD_ZERO(&myset);
	    err = ET_ERROR_TIMEOUT;

            /* 3 second timed wait */
	    deltatime.tv_sec  = 3;
            deltatime.tv_nsec = 0;

	    while (err == ET_ERROR_TIMEOUT) {
	      /* Linux modifies timeout, so reset each round */
	      timeout.tv_sec  = 0;
	      timeout.tv_usec = 0;
	      FD_SET(connfd, &myset);

              /* before waiting 3 sec in "get", check here if we're to wake up */
	      if (etid->sys->attach[att].quit == ET_ATT_QUIT) {
	          etid->sys->attach[att].sleep = ET_ATT_NOSLEEP;
                  etid->sys->attach[att].quit  = ET_ATT_CONTINUE;
		  err = ET_ERROR_WAKEUP;
		  break;
	      }
	      
	      err = et_event_get(id, att, &event, ET_TIMED, &deltatime);

	      /* Async call to select to see if this socket is still open.
	       * Ready to read only if socket error in this case.
	       */
	      if (err == ET_ERROR_TIMEOUT) {
		if (select(connfd + 1, &myset, NULL, NULL, &timeout) > 0) {
	          goto end;
		}
	      }
	      else {
	        etid->sys->attach[att].sleep = ET_ATT_NOSLEEP;
                etid->sys->attach[att].quit  = ET_ATT_CONTINUE;
	      }
	    }
	  }
	  else {
	    err = et_event_get(id, att, &event, wait, NULL);
	  }
	  header[0] = htonl(err);
	  if (err < ET_OK) {
	    if (tcp_write(connfd, (void *) header, sizeof(header[0])) != sizeof(header[0])) {
              goto end;
            }
	    break;
	  }
	  /* keep track of how this event is to be modified */
	  event->modify = modify;

	  header[1] = htonl(event->length);
	  header[2] = htonl(event->memsize);
	  /* send the priority & datastatus together and save space */
	  header[3] = htonl(event->priority |
			    event->datastatus << ET_DATA_SHIFT);
	  header[4] = (int) htonl((unsigned int)event);
	  header[5] = event->byteorder;
	  header[6] = 0; /* not used */
	  for (i=0; i < ET_STATION_SELECT_INTS; i++) {
	    header[i+7] = htonl(event->control[i]);
	  }

	  iov[0].iov_base = (void *) header;
	  iov[0].iov_len  = sizeof(header);
	  iov[1].iov_base = event->pdata;
	  iov[1].iov_len  = event->length;

	  /* write data */
	  if (tcp_writev(connfd, iov, 2, iov_max) == -1) {
            goto end;
	  }

	  if (modify == 0) {
	    if (dumpEvents) {
	      et_event_dump(id, att, event);
	    }
	    else {
	      et_event_put(id, att, event);
	    }
	  }
	}
	break;

        case  ET_NET_EVS_GET:
        {
          et_att_id  att;
	  int i, j, wait, num, modify, nevents, index, size, dumpEvents;
	  int headersize, incoming[6], outgoing[2];
	  struct timespec deltatime;

          if (tcp_read(connfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
            goto end;
          }
          att    = ntohl(incoming[0]);
          wait   = ntohl(incoming[1]);
          modify = ntohl(incoming[2]);
          num    = ntohl(incoming[3]);
	  /* Do we put or dump events back into ET system? */
	  dumpEvents = modify & ET_DUMP;
	  /* Are we going to modify the event or its header remotely? */
          modify &= (ET_MODIFY | ET_MODIFY_HEADER);

	  if (wait == ET_TIMED) {
            deltatime.tv_sec  = ntohl(incoming[4]);
            deltatime.tv_nsec = ntohl(incoming[5]);
	    err = et_events_get(id, att, events, wait, &deltatime, num, &nevents);
	  }
	  else if (wait == ET_SLEEP) {
	    /* There's a problem if we have a remote client that is waiting
	     * for another event by sleeping and the events stop flowing. In
	     * that case, the client can be killed and the ET system does NOT
	     * know about it. Since this thread will be stuck in et_events_get,
	     * it will not immediately detect the break in the socket - at least
	     * not until event start flowing again. To circumvent this, implement
	     * ET_SLEEP by repeats of ET_TIMED every couple seconds to allow
	     * detection of broken socket between calls to et_events_get.
	     */
	    struct timeval timeout;
	    fd_set myset;
            etid->sys->attach[att].sleep = ET_ATT_SLEEP;
	    FD_ZERO(&myset);
	    err = ET_ERROR_TIMEOUT;

            /* 3 second timed wait */
	    deltatime.tv_sec  = 3;
            deltatime.tv_nsec = 0;

	    while (err == ET_ERROR_TIMEOUT) {
	      /* Linux modifies timeout, so reset each round */
	      timeout.tv_sec  = 0;
	      timeout.tv_usec = 0;
	      FD_SET(connfd, &myset);
	      
              /* before waiting 3 sec in "get", check here if we're to wake up */
	      if (etid->sys->attach[att].quit == ET_ATT_QUIT) {
	          etid->sys->attach[att].sleep = ET_ATT_NOSLEEP;
                  etid->sys->attach[att].quit  = ET_ATT_CONTINUE;
		  err = ET_ERROR_WAKEUP;
		  break;
	      }
	      
	      err = et_events_get(id, att, events, ET_TIMED, &deltatime, num, &nevents);

	      /* Async call to select to see if this socket is still open.
	       * Ready to read only if socket error in this case.
	       */
	      if (err == ET_ERROR_TIMEOUT) {
		if (select(connfd + 1, &myset, NULL, NULL, &timeout) > 0) {
	          goto end;
		}
	      }
	      else {
	        etid->sys->attach[att].sleep = ET_ATT_NOSLEEP;
                etid->sys->attach[att].quit  = ET_ATT_CONTINUE;
	      }
	    }
	  }
	  else {
	    err = et_events_get(id, att, events, wait, NULL, num, &nevents);
	  }

	  if (err < 0) {
	    err = htonl(err);
	    if (tcp_write(connfd, (void *) &err, sizeof(err)) != sizeof(err)) {
              goto end;
            }
	    break;
	  }

          headersize = sizeof(int)*(6+ET_STATION_SELECT_INTS);
	  size = nevents*headersize;
          for (j=0; j < nevents; j++) {
            size += events[j]->length;
          }

	  outgoing[0] = htonl(nevents);
          outgoing[1] = htonl(size);
	  iov[0].iov_base = (void *) outgoing;
	  iov[0].iov_len  = sizeof(outgoing);
	  index=0;

	  for (i=0; i < nevents; i++) {
	    /* keep track of how this event is to be modified */
	    events[i]->modify = modify;

	    header[index]   = htonl(events[i]->length);
	    header[index+1] = htonl(events[i]->memsize);
	    header[index+2] = htonl(events[i]->priority |
				    events[i]->datastatus << ET_DATA_SHIFT);
	    header[index+3] = (int) htonl((unsigned int)events[i]);
	    header[index+4] = events[i]->byteorder;
	    header[index+5] = 0; /* not used */
	    for (j=0; j < ET_STATION_SELECT_INTS; j++) {
	      header[index+6+j] = htonl(events[i]->control[j]);
	    }
	    iov[2*i+1].iov_base = (void *) &header[index];
	    iov[2*i+1].iov_len  = headersize;
	    iov[2*i+2].iov_base = events[i]->pdata;
	    iov[2*i+2].iov_len  = events[i]->length;
	    index += (6+ET_STATION_SELECT_INTS);
	  }

	  if (tcp_writev(connfd, iov, 2*nevents+1, iov_max) == -1) {
            goto end;
	  }

	  if (modify == 0) {
	    if (dumpEvents) {
	      et_events_dump(id, att, events, nevents);
	    }
	    else {
	      et_events_put(id, att, events, nevents);
	    }
	  }
	}
	break;

        case  ET_NET_EV_PUT:
        {
	  int i, incoming[6+ET_STATION_SELECT_INTS];
	  et_event    *pe;
          et_att_id   att;

	  if (tcp_read(connfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
            goto end;
	  }

	  att              = ntohl(incoming[0]);
	  pe  = (et_event *) ntohl(incoming[1]);
	  pe->length       = ntohl(incoming[2]);
	  pe->priority     = ntohl(incoming[3]) & ET_PRIORITY_MASK;
	  pe->datastatus   =(ntohl(incoming[3]) & ET_DATA_MASK) >> ET_DATA_SHIFT;
	  pe->byteorder    = incoming[4];
	  for (i=0; i < ET_STATION_SELECT_INTS; i++) {
	    pe->control[i] = ntohl(incoming[i+6]);
	  }
	  /* only read data if modifying everything */
	  if (pe->modify == ET_MODIFY) {
	    if (tcp_read(connfd, pe->pdata, pe->length) != pe->length) {
              goto end;
	    }
          }

	  err = et_event_put(id, att, pe);

	  err = htonl(err);
          if (tcp_write(connfd, (void *) &err, sizeof(err)) != sizeof(err)) {
            goto end;
          }
	}
	break;

        case  ET_NET_EVS_PUT:
	{
	  int i, j, len, nevents, size;
	  int incoming[3], header[5+ET_STATION_SELECT_INTS];
          et_att_id  att;

	  if (tcp_read(connfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
            goto end;
	  }
	  att     = ntohl(incoming[0]);
	  nevents = ntohl(incoming[1]);
	  size    = ntohl(incoming[2]);

	  for (i=0; i < nevents; i++) {
	    if (tcp_read(connfd, (void *) header, sizeof(header)) != sizeof(header)) {
              goto end;
	    }

	    events[i]  = (et_event *) ntohl(header[0]);
	    events[i]->length = len = ntohl(header[1]);
	    events[i]->priority     = ntohl(header[2]) & ET_PRIORITY_MASK;
	    events[i]->datastatus   =(ntohl(header[2]) & ET_DATA_MASK) >> ET_DATA_SHIFT;
	    events[i]->byteorder    = header[3];
	    for (j=0; j < ET_STATION_SELECT_INTS; j++) {
	      events[i]->control[j] = ntohl(header[j+5]);
	    }
	    /* only read data if modifying everything */
	    if (events[i]->modify == ET_MODIFY) {
	      if (tcp_read(connfd, events[i]->pdata, len) != len) {
        	goto end;
	      }
	    }
	  }
	  err = et_events_put(id, att, events, nevents);

	  err = htonl(err);
          if (tcp_write(connfd, (void *) &err, sizeof(err)) != sizeof(err)) {
            goto end;
          }
	}
	break;

        case  ET_NET_EV_NEW:
        {
          et_att_id att;
	  int mode, size, incoming[5], transfer[2];
	  struct timespec deltatime;
	  et_event *pe = NULL;

          if (tcp_read(connfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
            goto end;
          }

          att  = ntohl(incoming[0]);
          mode = ntohl(incoming[1]);
	  size = ntohl(incoming[2]);

	  if (mode == ET_TIMED) {
            deltatime.tv_sec  = ntohl(incoming[3]);
            deltatime.tv_nsec = ntohl(incoming[4]);
            err = et_event_new(id, att, &pe, mode, &deltatime, size);
	  }
	  else if (mode == ET_SLEEP) {
	    /* There's a problem if we have a remote client that is waiting
	     * for a new event by sleeping and the events stop flowing. In
	     * that case, the client can be killed and the ET system does NOT
	     * know about it. Since this thread will be stuck in et_event_new,
	     * it will not immediately detect the break in the socket - at least
	     * not until events start flowing again. To circumvent this, implement
	     * ET_SLEEP by repeats of ET_TIMED every couple seconds to allow
	     * detection of broken socket between calls to et_event_new.
	     */
	    struct timeval timeout;
	    fd_set myset;
            etid->sys->attach[att].sleep = ET_ATT_SLEEP;
	    FD_ZERO(&myset);
	    err = ET_ERROR_TIMEOUT;

            /* 3 second timed wait */
	    deltatime.tv_sec  = 3;
            deltatime.tv_nsec = 0;

	    while (err == ET_ERROR_TIMEOUT) {
	      /* Linux modifies timeout, so reset each round */
	      timeout.tv_sec  = 0;
	      timeout.tv_usec = 0;
	      FD_SET(connfd, &myset);

	      if (etid->sys->attach[att].quit == ET_ATT_QUIT) {
	          etid->sys->attach[att].sleep = ET_ATT_NOSLEEP;
                  etid->sys->attach[att].quit  = ET_ATT_CONTINUE;
		  err = ET_ERROR_WAKEUP;
		  break;
	      }
	      
	      err = et_event_new(id, att, &pe, ET_TIMED, &deltatime, size);

	      /* Async call to select to see if this socket is still open.
	       * Ready to read only if socket error in this case.
	       */
	      if (err == ET_ERROR_TIMEOUT) {
		if (select(connfd + 1, &myset, NULL, NULL, &timeout) > 0) {
	          goto end;
		}
	      }
	      else {
	        etid->sys->attach[att].sleep = ET_ATT_NOSLEEP;
                etid->sys->attach[att].quit  = ET_ATT_CONTINUE;
	      }
	    }
	  }
	  else {
	    err = et_event_new(id, att, &pe, mode, NULL, size);
	  }
	  /* keep track of how this event is to be modified */
          if (err == ET_OK) pe->modify = ET_MODIFY;

	  transfer[0] = htonl(err);
	  transfer[1] = (int) htonl((unsigned int)pe);

	  if (tcp_write(connfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
            goto end;
          }
	}

	break;

        case  ET_NET_EVS_NEW:
        {
          et_att_id  att;
	  int i, mode, size, nevents, num, nevents_net;
	  int incoming[6];
	  struct timespec deltatime;
          struct iovec iov[2];

          if (tcp_read(connfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
            goto end;
          }

          att  = ntohl(incoming[0]);
          mode = ntohl(incoming[1]);
          size = ntohl(incoming[2]);
	  num  = ntohl(incoming[3]);

	  if (mode == ET_TIMED) {
            deltatime.tv_sec  = ntohl(incoming[4]);
            deltatime.tv_nsec = ntohl(incoming[5]);
            err = et_events_new(id, att, events, mode, &deltatime, size, num, &nevents);
	  }
	  else if (mode == ET_SLEEP) {
	    /* There's a problem if we have a remote client that is waiting
	     * for a new event by sleeping and the events stop flowing. In
	     * that case, the client can be killed and the ET system does NOT
	     * know about it. Since this thread will be stuck in et_events_new,
	     * it will not immediately detect the break in the socket - at least
	     * not until event start flowing again. To circumvent this, implement
	     * ET_SLEEP by repeats of ET_TIMED every couple seconds to allow
	     * detection of broken socket between calls to et_events_new.
	     */
	    struct timeval timeout;
	    fd_set myset;
            etid->sys->attach[att].sleep = ET_ATT_SLEEP;
	    FD_ZERO(&myset);
	    err = ET_ERROR_TIMEOUT;

            /* 3 second timed wait */
	    deltatime.tv_sec  = 3;
            deltatime.tv_nsec = 0;

	    while (err == ET_ERROR_TIMEOUT) {
	      /* Linux modifies timeout, so reset each round */
	      timeout.tv_sec  = 0;
	      timeout.tv_usec = 0;
	      FD_SET(connfd, &myset);

              /* before waiting 3 sec in "get", check here if we're to wake up */
	      if (etid->sys->attach[att].quit == ET_ATT_QUIT) {
	          etid->sys->attach[att].sleep = ET_ATT_NOSLEEP;
                  etid->sys->attach[att].quit  = ET_ATT_CONTINUE;
		  err = ET_ERROR_WAKEUP;
		  break;
	      }
	      
	      err = et_events_new(id, att, events, ET_TIMED, &deltatime, size, num, &nevents);

	      /* Async call to select to see if this socket is still open.
	       * Ready to read only if socket error in this case.
	       */
	      if (err == ET_ERROR_TIMEOUT) {
		if (select(connfd + 1, &myset, NULL, NULL, &timeout) > 0) {
	          goto end;
		}
	      }
	      else {
	        etid->sys->attach[att].sleep = ET_ATT_NOSLEEP;
                etid->sys->attach[att].quit  = ET_ATT_CONTINUE;
	      }
	    }
	  }
	  else {
	    err = et_events_new(id, att, events, mode, NULL, size, num, &nevents);
	  }

	  if (err < 0) {
	    err = htonl(err);
	    if (tcp_write(connfd, (void *) &err, sizeof(err)) != sizeof(err)) {
              goto end;
            }
	    break;
	  }

	  for (i=0; i < nevents; i++) {
	    /* keep track of how this event is to be modified */
            events[i]->modify = ET_MODIFY;
	    events[i] = (et_event *) htonl((unsigned int)events[i]);
	  }

	  nevents_net = htonl(nevents);
	  iov[0].iov_base = (void *) &nevents_net;
	  iov[0].iov_len  = sizeof(nevents_net);
	  iov[1].iov_base = (void *) events;
	  iov[1].iov_len  = nevents*sizeof(et_event *);

	  if (tcp_writev(connfd, iov, 2, iov_max) == -1) {
            goto end;
	  }
	}
	break;

        case  ET_NET_EV_DUMP:
        {
	  int incoming[2];
	  et_event    *pe;
          et_att_id   att;

	  if (tcp_read(connfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
            goto end;
	  }

	  att = ntohl(incoming[0]);
	  pe  = (et_event *) ntohl(incoming[1]);

	  err = et_event_dump(id, att, pe);

	  err = htonl(err);
          if (tcp_write(connfd, (void *) &err, sizeof(err)) != sizeof(err)) {
            goto end;
          }
	}
	break;

        case  ET_NET_EVS_DUMP:
	{
	  int i, nevents, incoming[2];
          et_att_id  att;

	  if (tcp_read(connfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
            goto end;
	  }
	  att     = ntohl(incoming[0]);
	  nevents = ntohl(incoming[1]);

	  if (tcp_read(connfd, (void *) events, nevents*sizeof(int)) != nevents*sizeof(int)) {
            goto end;
	  }

	  for (i=0; i < nevents; i++) {
	    events[i] = (et_event *) ntohl((unsigned int) events[i]);
	  }

	  err = et_events_dump(id, att, events, nevents);

	  err = htonl(err);
          if (tcp_write(connfd, (void *) &err, sizeof(err)) != sizeof(err)) {
            goto end;
          }
	}
	break;

        case  ET_NET_ALIVE:
        {
	  /* we must be alive by definition as this is in the ET process */
          err = htonl(1);
          if (tcp_write(connfd, (void *) &err, sizeof(err)) != sizeof(err)) {
            goto end;
          }
        }
        break;

        case  ET_NET_WAIT:
        {
	  /* We must be alive by definition as this is in the ET process,
	   * hence there is no need to call et_wait_for_alive(id)
	   */
          err = htonl(ET_OK);
          if (tcp_write(connfd, (void *) &err, sizeof(err)) != sizeof(err)) {
            goto end;
          }
	}
        break;

        case  ET_NET_CLOSE:
        case  ET_NET_FCLOSE:
        {
	  int errnet;

          errnet = htonl(ET_OK);
          tcp_write(connfd, (void *) &errnet, sizeof(errnet));

	  /* detach all attachments */
	  for (i=0; i <ET_ATTACHMENTS_MAX ; i++) {
	    if (attaches[i] > -1) {
	      et_station_detach(id, attaches[i]);
	    }
	  }

          if (etid->debug >= ET_DEBUG_INFO) {
              et_logmsg("INFO", "et_command_loop: remote client closing\n");
          }

	  free(iov); free(header); free(histogram); free(events);
	  return;
 	}
        break;

        case  ET_NET_WAKE_ATT:
        {
          et_att_id  att;

          if (tcp_read(connfd, (void *) &att, sizeof(att)) != sizeof(att)) {
            goto end;
          }
          att = ntohl(att);

          et_wakeup_attachment(id, att);
	}
        break;

        case  ET_NET_WAKE_ALL:
        {
          et_stat_id stat_id;

          if (tcp_read(connfd, (void *) &stat_id, sizeof(stat_id)) != sizeof(stat_id)) {
            goto end;
          }
          stat_id = ntohl(stat_id);

          et_wakeup_all(id, stat_id);
	}
        break;

        case  ET_NET_STAT_ATT:
        {
          et_stat_id  stat_id;
          et_att_id   att;
	  int length, outgoing[2], transfer[3];
	  char host[ET_MAXHOSTNAMELEN];
	  pid_t pid;

          if (tcp_read(connfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
            goto end;
          }
          stat_id = ntohl(transfer[0]);
          pid     = ntohl(transfer[1]);
          length  = ntohl(transfer[2]);

	  if (length > 0) {
            if (tcp_read(connfd, (void *) host, length) != length) {
              goto end;
            }
	  }

          err = et_station_attach(id, stat_id, &att);
	  /* keep track of all attachments */
	  if (err == ET_OK) {
	    attaches[att] = att;
	    /*
	     * Register, in shared memory, that this attachment
	     * is to a process "pid" running on "host".
	     */
	    etid->sys->attach[att].pid = pid;
	    if (length > 0) {
	      strcpy(etid->sys->attach[att].host, host);
	    }
	  }

	  outgoing[0] = htonl(err);
	  outgoing[1] = htonl(att);
          if (tcp_write(connfd, (void *) outgoing, sizeof(outgoing)) != sizeof(outgoing)) {
            goto end;
          }

        }
        break;

        case  ET_NET_STAT_DET:
        {
          et_att_id  att;

	  if (tcp_read(connfd, (void *) &att, sizeof(att)) != sizeof(att)) {
            goto end;
          }
          att = ntohl(att);

          err = et_station_detach(id, att);
	  /* keep track of all detachments */
	  if (err == ET_OK) {
	    attaches[att] = -1;
	  }

          err = htonl(err);
	  if (tcp_write(connfd, (void *) &err, sizeof(err)) != sizeof(err)) {
            goto end;
          }

	}
	break;


        case  ET_NET_STAT_CRAT:
	{
          et_stat_config sc;
          et_stat_id     stat_id;
	  char stat_name[ET_STATNAME_LENGTH];
	  int incoming[14+ET_STATION_SELECT_INTS], transfer[2];
          int i, position, pposition, lengthname, lengthfname, lengthlib, lengthclass;

          if (tcp_read(connfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
            goto end;
          }

	  sc.init         = ntohl(incoming[0]);
	  sc.flow_mode    = ntohl(incoming[1]);
	  sc.user_mode    = ntohl(incoming[2]);
	  sc.restore_mode = ntohl(incoming[3]);
	  sc.block_mode   = ntohl(incoming[4]);
	  sc.prescale     = ntohl(incoming[5]);
	  sc.cue          = ntohl(incoming[6]);
	  sc.select_mode  = ntohl(incoming[7]);
	  for (i=0 ; i < ET_STATION_SELECT_INTS ; i++) {
	    sc.select[i]  = ntohl(incoming[8+i]);
	  }
	  lengthfname = ntohl(incoming[8 +ET_STATION_SELECT_INTS]);
	  lengthlib   = ntohl(incoming[9 +ET_STATION_SELECT_INTS]);
	  lengthclass = ntohl(incoming[10+ET_STATION_SELECT_INTS]);
	  lengthname  = ntohl(incoming[11+ET_STATION_SELECT_INTS]);
	  position    = ntohl(incoming[12+ET_STATION_SELECT_INTS]);
	  pposition   = ntohl(incoming[13+ET_STATION_SELECT_INTS]);

	  if (tcp_read(connfd, (void *) sc.fname, lengthfname) != lengthfname) {
	    goto end;
	  }
	  if (tcp_read(connfd, (void *) sc.lib, lengthlib) != lengthlib) {
	    goto end;
	  }
	  if (tcp_read(connfd, (void *) sc.classs, lengthclass) != lengthclass) {
	    goto end;
	  }
          if (tcp_read(connfd, (void *) stat_name, lengthname) != lengthname) {
            goto end;
          }

          err = et_station_create_at(id, &stat_id, stat_name,
	                            (et_statconfig) &sc, position, pposition);

          transfer[0] = htonl(err);
          transfer[1] = htonl(stat_id);
          if (tcp_write(connfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
            goto end;
          }
	}
	break;


        case  ET_NET_STAT_RM:
	{
          et_stat_id stat_id;

          if (tcp_read(connfd, (void *) &stat_id, sizeof(stat_id)) != sizeof(stat_id)) {
            goto end;
          }
          stat_id = ntohl(stat_id);

          err = et_station_remove(id, stat_id);

	  err = htonl(err);
          if (tcp_write(connfd, (void *) &err, sizeof(err)) != sizeof(err)) {
           goto end;
          }
	}
	break;

        case  ET_NET_STAT_SPOS:
	{
          et_stat_id  stat_id;
          int  position, pposition, transfer[3];

	  if (tcp_read(connfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
            goto end;
          }
          stat_id   = ntohl(transfer[0]);
          position  = ntohl(transfer[1]);
          pposition = ntohl(transfer[2]);

          err = et_station_setposition(id, stat_id, position, pposition);

	  err = htonl(err);
          if (tcp_write(connfd, (void *) &err, sizeof(err)) != sizeof(err)) {
            goto end;
          }
	}
	break;

        case  ET_NET_STAT_GPOS:
	{
          et_stat_id  stat_id;
          int  position, pposition, transfer[3];

	  if (tcp_read(connfd, (void *) &stat_id, sizeof(stat_id)) != sizeof(stat_id)) {
            goto end;
	  }
	  stat_id = ntohl(stat_id);

          err = et_station_getposition(id, stat_id, &position, &pposition);

	  transfer[0] = htonl(err);
	  transfer[1] = htonl(position);
	  transfer[2] = htonl(pposition);
	  if (tcp_write(connfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
            goto end;
	  }
	}
	break;

        case  ET_NET_STAT_ISAT:
	{
          et_stat_id  stat_id;
          et_att_id   att;
	  int  transfer[2];

	  if (tcp_read(connfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
            goto end;
          }
          stat_id = ntohl(transfer[0]);
          att     = ntohl(transfer[1]);

          err = et_station_isattached(id, stat_id, att);

	  err = htonl(err);
          if (tcp_write(connfd, (void *) &err, sizeof(err)) != sizeof(err)) {
            goto end;
          }
	}
	break;

       case  ET_NET_STAT_EX:
	{
          et_stat_id  stat_id;
	  char stat_name[ET_STATNAME_LENGTH];
	  int  length, transfer[2];

          if (tcp_read(connfd, (void *) &length, sizeof(length)) != sizeof(length)) {
            goto end;
          }
          length = ntohl(length);
          if (tcp_read(connfd, (void *) stat_name, length) != length) {
            goto end;
          }

          err = et_station_exists(id, &stat_id, stat_name);

	  transfer[0] = htonl(err);
	  transfer[1] = htonl(stat_id);
          if (tcp_write(connfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
            goto end;
          }
	}
	break;

        case  ET_NET_STAT_SSW:
	{
          et_stat_id  stat_id;
	  int i, incoming[1+ET_STATION_SELECT_INTS];

          if (tcp_read(connfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
            goto end;
          }

	  stat_id = ntohl(incoming[0]);
	  for (i=0 ; i < ET_STATION_SELECT_INTS; i++) {
	    incoming[i] = ntohl(incoming[1+i]);
	  }
	  
          err = et_station_setselectwords(id, stat_id, incoming);

          err = htonl(err);
          if (tcp_write(connfd, (void *) &err, sizeof(err)) != sizeof(err)) {
            goto end;
          }
	}
	break;


        case  ET_NET_STAT_GSW:
        {
          et_stat_id  stat_id;
	  int i, sw[1+ET_STATION_SELECT_INTS];

	  if (tcp_read(connfd, (void *) &stat_id, sizeof(stat_id)) != sizeof(stat_id)) {
            goto end;
	  }
          stat_id = ntohl(stat_id);

	  err = et_station_getselectwords(id, stat_id, &sw[1]);

	  sw[0] = htonl(err);
	  for (i=1; i <= ET_STATION_SELECT_INTS; i++) {
	    sw[i] = htonl(sw[i]);
	  }
	  if (tcp_write(connfd, (void *) sw, sizeof(sw)) != sizeof(sw)) {
            goto end;
          }
	}
	break;

        case  ET_NET_STAT_LIB:
        case  ET_NET_STAT_FUNC:
        case  ET_NET_STAT_CLASS:
        {
          et_stat_id  stat_id;
	  size_t len, size;
	  int transfer[2];
          /* make sure buf is larger than class, lib or func name + 2 integers
	   * (no matter how big ints may get)
	   */
	  char *name, buf[128+ET_FILENAME_LENGTH];

	  if (tcp_read(connfd, (void *) &stat_id, sizeof(stat_id)) != sizeof(stat_id)) {
            goto end;
	  }
          stat_id = ntohl(stat_id);

	  name = buf + sizeof(transfer);
	  if (command == ET_NET_STAT_LIB) {
	    err = et_station_getlib(id, stat_id, name);
	  }
	  else if (command == ET_NET_STAT_FUNC) {
	    err = et_station_getfunction(id, stat_id, name);
	  }
	  else  {
	    err = et_station_getclass(id, stat_id, name);
	  }

	  if (err != ET_OK) {
	    transfer[0] = htonl(err);
	    transfer[1] = 0;
	    if (tcp_write(connfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
              goto end;
            }
	  }
	  else {
	    len = strlen(name) + 1;
	    transfer[0] = htonl(err);
	    transfer[1] = htonl(len);
	    memcpy(buf, transfer, sizeof(transfer));
	    size = sizeof(transfer) + len;
	    if (tcp_write(connfd, (void *) buf, size) != size) {
              goto end;
            }
	  }
	}
	break;

        default :
	 ;
      } /* switch(command) */
    }   /* if (command < ET_NET_STAT_GATTS) */

    /* the following commands get values associated with stations */
    else if (command < ET_NET_STAT_SBLOCK) {

      et_stat_id  stat_id;
      int  val, transfer[2];

      if (tcp_read(connfd, (void *) &stat_id, sizeof(stat_id)) != sizeof(stat_id)) {
        goto end;
      }
      stat_id = ntohl(stat_id);

      switch (command) {
        case  ET_NET_STAT_GATTS:
          err = et_station_getattachments(id, stat_id, &val);
          break;
	case  ET_NET_STAT_STATUS:
          err = et_station_getstatus(id, stat_id, &val);
          break;
	case  ET_NET_STAT_INCNT:
          err = et_station_getinputcount(id, stat_id, &val);
          break;
	case  ET_NET_STAT_OUTCNT:
          err = et_station_getoutputcount(id, stat_id, &val);
          break;
	case  ET_NET_STAT_GBLOCK:
          err = et_station_getblock(id, stat_id, &val);
          break;
	case  ET_NET_STAT_GUSER:
          err = et_station_getuser(id, stat_id, &val);
          break;
	case  ET_NET_STAT_GRESTORE:
          err = et_station_getrestore(id, stat_id, &val);
          break;
	case  ET_NET_STAT_GPRE:
          err = et_station_getprescale(id, stat_id, &val);
          break;
	case  ET_NET_STAT_GCUE:
          err = et_station_getcue(id, stat_id, &val);
          break;
	case  ET_NET_STAT_GSELECT:
          err = et_station_getselect(id, stat_id, &val);
          break;
	default:
          if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "et_command_loop: bad command value\n");
          }
          goto end;
      }
      
      transfer[0] = htonl(err);
      transfer[1] = htonl(val);
      if (tcp_write(connfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        goto end;
      }
    }
    
    
    /* the following commands set values associated with stations */
    else if (command < ET_NET_ATT_PUT) {

      et_stat_id  stat_id;
      int  val, incoming[2];

      if (tcp_read(connfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
        goto end;
      }
      stat_id = ntohl(incoming[0]);
      val     = ntohl(incoming[1]);

      switch (command) {
        case  ET_NET_STAT_SBLOCK:
          err = et_station_setblock(id, stat_id, val);
          break;
        case  ET_NET_STAT_SUSER:
          err = et_station_setuser(id, stat_id, val);
          break;
        case  ET_NET_STAT_SRESTORE:
          err = et_station_setrestore(id, stat_id, val);
          break;
        case  ET_NET_STAT_SPRE:
          err = et_station_setprescale(id, stat_id, val);
          break;
        case  ET_NET_STAT_SCUE:
          err = et_station_setcue(id, stat_id, val);
          break;
        default:
          if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "et_command_loop: bad command value\n");
          }
          goto end;
      }
      
      err = htonl(err);
      if (tcp_write(connfd, (void *) &err, sizeof(err)) != sizeof(err)) {
        goto end;
      }
    }
    
    
    /* the following commands get values associated with attachments */
    else if (command < ET_NET_SYS_TMP) {
      et_att_id  att_id;
      int  highint=0, lowint=0, transfer[3];

      if (tcp_read(connfd, (void *) &att_id, sizeof(att_id)) != sizeof(att_id)) {
        goto end;
      }
      att_id = ntohl(att_id);

      switch (command) {
        case  ET_NET_ATT_PUT:
          err = et_attach_geteventsput(id, att_id, &highint, &lowint);
          break;
        case  ET_NET_ATT_GET:
          err = et_attach_geteventsget(id, att_id, &highint, &lowint);
          break;
        case  ET_NET_ATT_DUMP:
          err = et_attach_geteventsdump(id, att_id, &highint, &lowint);
          break;
        case  ET_NET_ATT_MAKE:
          err = et_attach_geteventsmake(id, att_id, &highint, &lowint);
          break;
        default:
          if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "et_command_loop: bad command value\n");
          }
          goto end;
      }
      
      transfer[0] = htonl(err);
      transfer[1] = htonl(highint);
      transfer[2] = htonl(lowint);
      if (tcp_write(connfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        goto end;
      }
    }


    /* the following commands get values associated with the system */
    else if (command <= ET_NET_SYS_PID) {

      int  val, transfer[2];
      pid_t pid; /* int in Linux, long in Solaris */

      switch (command) {
        case  ET_NET_SYS_TMP:
          err = et_system_gettemps(id, &val);
          break;
        case  ET_NET_SYS_TMPMAX:
          err = et_system_gettempsmax(id, &val);
          break;
        case  ET_NET_SYS_STAT:
          err = et_system_getstations(id, &val);
          break;
        case  ET_NET_SYS_STATMAX:
          err = et_system_getstationsmax(id, &val);
          break;
        case  ET_NET_SYS_PROC:
          err = et_system_getprocs(id, &val);
          break;
        case  ET_NET_SYS_PROCMAX:
          err = et_system_getprocsmax(id, &val);
          break;
        case  ET_NET_SYS_ATT:
          err = et_system_getattachments(id, &val);
          break;
        case  ET_NET_SYS_ATTMAX:
          err = et_system_getattsmax(id, &val);
          break;
        case  ET_NET_SYS_HBEAT:
          err = et_system_getheartbeat(id, &val);
          break;
        case  ET_NET_SYS_PID:
          err = et_system_getpid(id, &pid);
	  val = pid;
          break;
        default:
          if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "et_command_loop: bad command value\n");
          }
          goto end;
      }
      
      transfer[0] = htonl(err);
      transfer[1] = htonl(val);
      if (tcp_write(connfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        goto end;
      }
    }

    /* the following commands distribute data about this ET system over the network */
    else if (command <= ET_NET_SYS_HIST) {
      if (command == ET_NET_SYS_DATA) {
	struct iovec iov[5];
	int    outgoing[2], size;
        
	err = ET_OK;
	
	if (et_data_sys(etid, &iov[1]) != ET_OK) {
	  err = ET_ERROR;
	}
	else if (et_data_stats(etid, &iov[2]) != ET_OK) {
	  err = ET_ERROR;
	  free(iov[1].iov_base);
	}
	else if (et_data_atts(etid, &iov[3]) != ET_OK) {
	  err = ET_ERROR;
	  free(iov[1].iov_base);
	  free(iov[2].iov_base);
	}
	else if (et_data_procs(etid, &iov[4]) != ET_OK) {
	  err = ET_ERROR;
	  free(iov[1].iov_base);
	  free(iov[2].iov_base);
	  free(iov[3].iov_base);
	}

        if (err != ET_OK) {
	  err = htonl(ET_ERROR);
          if (tcp_write(connfd, (void *) &err, sizeof(err)) != sizeof(err)) {
            goto end;
          }
	}
	else {
	  /*
	   * The first bit of data we send will be the error followed by the
	   * total size - in bytes - of the data that is to follow. This
	   * allows the receiver to read that first bit of data and then to
	   * allocate a buffer of the size necessary to hold all the real data.
	   */
	  size = iov[1].iov_len + iov[2].iov_len + iov[3].iov_len + iov[4].iov_len;
	  outgoing[0] = htonl(ET_OK);
	  outgoing[1] = htonl(size);
	  iov[0].iov_base = (void *) outgoing;
	  iov[0].iov_len  = sizeof(outgoing);

	  if (tcp_writev(connfd, iov, 5, iov_max) == -1) {
	    free(iov[1].iov_base);
	    free(iov[2].iov_base);
	    free(iov[3].iov_base);
	    free(iov[4].iov_base);
            goto end;
	  }

	  /* free buffers allocated in "et_data_ ..." routines */
	  free(iov[1].iov_base);
	  free(iov[2].iov_base);
	  free(iov[3].iov_base);
	  free(iov[4].iov_base);
	}
      }

      /* send histogram data */
      else if (command == ET_NET_SYS_HIST) {
	err = et_data_gethistogram(id, histogram+1, nevents_max+1);
	/* sneak error code into histogram array for convenience in writing */
	histogram[0] = err;

	if (err != ET_OK) {
	  histogram[0] = htonl(histogram[0]);
          if (tcp_write(connfd, (void *) histogram, sizeof(int)) != sizeof(int)) {
            goto end;
          }
	}
	else {
          /* swap data */
	  for (i=0; i < nevents_max+2; i++) {
	    histogram[i] = htonl(histogram[i]);
	  }
          if (tcp_write(connfd, (void *) histogram, sizeof(int)*(nevents_max+2)) !=
	      sizeof(int)*(nevents_max+2)) {
            goto end;
          }
	}
      }

    }

    else {
      if (etid->debug >= ET_DEBUG_ERROR) {
        et_logmsg("ERROR", "et_command_loop: bad command value\n");
      }
      goto end;
    }

  } /* while(1) */

  /* we only end up down here if there's an error */
  end:
    /*
     * The client has crashed, therefore we must detach all
     * attachments or risked stopping the ET system. The client
     * will not be asking for or processing any more events.
     */
    for (i=0; i <ET_ATTACHMENTS_MAX ; i++) {
      if (attaches[i] > -1) {
	et_station_detach(id, attaches[i]);
      }
    }

    if (etid->debug >= ET_DEBUG_WARN) {
        et_logmsg("WARN", "et_command_loop: remote client connection broken\n");
    }

    free(iov); free(header); free(histogram); free(events);
    return;
}
