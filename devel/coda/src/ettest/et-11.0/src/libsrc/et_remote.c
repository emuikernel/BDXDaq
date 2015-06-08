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
 *      Routines for remote ET system users.
 *
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#ifndef VXWORKS

#include <strings.h>
#include <sys/time.h>

#else

#include <time.h>
#include <vxWorks.h>
#include <taskLib.h>
#define getpid       taskIdSelf
#define UINT32_MAX  (4294967295U)

#endif


#include "et_private.h"
#include "et_network.h"

static int etr_station_getstuff(et_id *id, et_stat_id stat_id, int cmd, int *stuff, const char *routine);
static int etr_system_getstuff(et_id *id, int cmd, int *stuff, const char *routine);


/*****************************************************/
/*                      ET SYSTEM                    */
/*****************************************************/
int etr_open(et_sys_id *id, const char *et_filename, et_openconfig openconfig)
{
    et_open_config *config = (et_open_config *) openconfig;
    int sockfd=-1, length, bufsize, version, nselects;
    int vxdelay;
    int err=ET_OK, openerror=ET_OK, transfer[8], incoming[9];
    int port=0;
    uint32_t inetaddr = 0;
    et_id *etid;
    char *buf, *pbuf, ethost[ET_MAXHOSTNAMELEN];

    double          dstart, dnow, dtimeout;
    struct timespec sleeptime;
#if defined linux || defined __APPLE__
    struct timeval  start, now;
#else
    struct timespec start, now;
#endif

    /* system id */
    etid = (et_id *) *id;

    strcpy(ethost, config->host);

    /* keep track of starting time */
#if defined linux || defined __APPLE__
    gettimeofday(&start, NULL);
    dstart = start.tv_sec + 1.e-6*(start.tv_usec);
#else
    clock_gettime(CLOCK_REALTIME, &start);
    dstart = start.tv_sec + 1.e-9*(start.tv_nsec);
#endif

    /* 0.1 sec per sleep (10Hz) */
    sleeptime.tv_sec  = 0;
    sleeptime.tv_nsec = 100000000;
    vxdelay = 6;

    /* set minimum time to wait for connection to ET */
    /* if timeout == 0, wait "forever" */
    if ((config->timeout.tv_sec == 0) && (config->timeout.tv_nsec == 0)) {
        dtimeout = 1.e9; /* 31 years */
    }
    else {
        dtimeout = config->timeout.tv_sec + 1.e-9*(config->timeout.tv_nsec);
    }

    /* if port# & name of ET server specified */
    if (config->cast == ET_DIRECT) {
        port = config->serverport;
    }

    while(1) {
        if (port > 0) {
            /* make the network connection */
            if (inetaddr == 0) {
et_logmsg("INFO","etr_open: et_tcp_connect to host %s on port %d\n",ethost,port);
              if ( (sockfd = et_tcp_connect(ethost, (unsigned short)port)) >= 0) {
                  break;
              }
            }
            else {
et_logmsg("INFO","etr_open: et_tcp_connect2 to address %u on port %d\n",inetaddr,port);
              if ( (sockfd = et_tcp_connect2(inetaddr, (unsigned short)port)) >= 0) {
                  break;
              }
            }
        }
        else {
            /* else find port# & name of ET server by broad/multicasting */
et_logmsg("INFO","etr_open: calling et_findserver(file=%s, host=%s)\n",et_filename,ethost);
            if ( (openerror = et_findserver(et_filename, ethost, &port, &inetaddr, config)) == ET_OK) {
                printf("etr_open: calling et_findserver SUCCESS\n");
                continue;
            }
        }

        /* if user does NOT want to wait ... */
        if (config->wait == ET_OPEN_NOWAIT) {
            break;
        }

        /* see if the min time has elapsed, if so quit. */
#if defined linux || defined __APPLE__
        gettimeofday(&now, NULL);
        dnow = now.tv_sec + 1.e-6*(now.tv_usec);
#else
        clock_gettime(CLOCK_REALTIME, &now);
        dnow = now.tv_sec + 1.e-9*(now.tv_nsec);
#endif
        if (dtimeout < dnow - dstart) {
            break;
        }
#ifdef VXWORKS
        taskDelay(vxdelay);
#else
        nanosleep(&sleeptime, NULL);
#endif
    }

    /* check for errors */
    /*printf("etr_open: port %d sockfd = 0x%x\n",port,sockfd);*/
    if ((port == 0) || (sockfd < 0)) {
        if (openerror == ET_ERROR_TOOMANY) {
            if (etid->debug >= ET_DEBUG_ERROR) {
                et_logmsg("ERROR", "etr_open: too many ET systems of that name responded\n");
            }
            return ET_ERROR_TOOMANY;
        }
        else {
            if (etid->debug >= ET_DEBUG_ERROR) {
                et_logmsg("ERROR", "etr_open: cannot find or connect to ET system\n");
            }
        }
        return ET_ERROR_REMOTE;
    }
    else {
        if (etid->debug >= ET_DEBUG_INFO) {
            et_logmsg("INFO", "etr_open: ET system on %s, port# %d\n", ethost, port);
        }
    }

    /* Add data to id - locality, socket, port, and host, of the ET system */
    etid->locality = ET_REMOTE;
    etid->sockfd   = sockfd;
    etid->port     = port;
    strcpy(etid->ethost, ethost);

    /* find client's iov_max value */
#if defined VXWORKS || defined __APPLE__
    etid->iov_max = ET_IOV_MAX;
#else
    if ( (etid->iov_max = sysconf(_SC_IOV_MAX)) == -1) {
        /* set it to POSIX minimum by default (it always bombs on Linux) */
        etid->iov_max = ET_IOV_MAX;
    }
#endif

    /* magic numbers */
    transfer[0]  = htonl(ET_MAGIC_INT1);
    transfer[1]  = htonl(ET_MAGIC_INT2);
    transfer[2]  = htonl(ET_MAGIC_INT3);
    
    /* endian */
    transfer[3]  = htonl(etid->endian);

    /* length of ET system name */
    length = strlen(et_filename)+1;
    transfer[4] = htonl(length);
#ifdef _LP64
    transfer[5] = 1;
#else
    transfer[5] = 0;
#endif
    /* not used */
    transfer[6] = 0;
    transfer[7] = 0;

    /* put everything in one buffer, extra room in "transfer" */
    bufsize = sizeof(transfer) + length;
    if ( (pbuf = buf = (char *) malloc(bufsize)) == NULL) {
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_open, cannot allocate memory\n");
        }
        err = ET_ERROR_REMOTE;
        goto error;
    }
    memcpy(pbuf, transfer, sizeof(transfer));
    pbuf += sizeof(transfer);
    memcpy(pbuf,et_filename, length);

    /* write it to server */
    if (et_tcp_write(sockfd, (void *) buf, bufsize) != bufsize) {
        free(buf);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_open, write error\n");
        }
        err = ET_ERROR_WRITE;
        goto error;
    }
    free(buf);

    /* read the return */
    if (et_tcp_read(sockfd, (void *) &err, sizeof(err)) != sizeof(err)) {
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_open, read error\n");
        }
        err = ET_ERROR_READ;
        goto error;
    }
    err = ntohl(err);
    if (err != ET_OK) {
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_open: found the wrong ET system\n");
        }
        goto error;
    }

    if (et_tcp_read(sockfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_open, read error\n");
        }
        err = ET_ERROR_READ;
        goto error;
    }
    /* ET's endian */
    etid->systemendian = ntohl(incoming[0]);
    /* ET's total number of events */
    etid->nevents = ntohl(incoming[1]);
    /* ET's max event size */
    etid->esize = ET_64BIT_UINT(ntohl(incoming[2]),ntohl(incoming[3]));
    /* ET's version number */
    version = ntohl(incoming[4]);
    /* ET's number of selection integers */
    nselects = ntohl(incoming[5]);
    /* ET's language */
    etid->lang = ntohl(incoming[6]);
    /* ET's 64 or 32 bit exe. Note, because we communicate thru the server
     * and not thru shared mem, 64 & 32 bit programs can speak to each
     * other. It is possible to have a run-time error if a 64 bit app tries
     * to send large data to a 32 bit ET system, or if a 32 bit app tries to
     * read large data from a 64 bit ET system.
     */
    etid->bit64 = ntohl(incoming[7]);

    if (version != etid->version) {
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_open, ET system & user use different versions of ET\n");
        }
        err = ET_ERROR_REMOTE;
        goto error;
    }

    if (nselects != etid->nselects) {
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_open, ET system & user compiled with different ET_STATION_SELECT_INTS\n");
        }
        err = ET_ERROR_REMOTE;
        goto error;
    }

    /*printf("etr_open: We are OK\n");*/
    return ET_OK;

error:
    close (sockfd);
    return err;
}


/*****************************************************/
int etr_close(et_sys_id id)
{
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    int com;

    com = htonl(ET_NET_CLOSE);
    /* if communication with ET system fails, we've already been "closed" */
    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) &com, sizeof(com)) != sizeof(com)) {
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_close, write error\n");
        }
        /* return ET_ERROR_WRITE; */
    }

    close(sockfd);
    et_tcp_unlock(etid);
    et_id_destroy(id);

    return ET_OK;
}

/*****************************************************/
int etr_forcedclose(et_sys_id id)
{
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    int com;

    com = htonl(ET_NET_FCLOSE);
    /* if communication with ET system fails, we've already been "closed" */
    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) &com, sizeof(com)) != sizeof(com)) {
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_forcedclose, write error\n");
        }
        /* return ET_ERROR_WRITE; */
    }

    close(sockfd);
    et_tcp_unlock(etid);
    et_id_destroy(id);

    return ET_OK;
}

/*****************************************************/
int etr_alive(et_sys_id id)
{
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    int alive, com;

    com = htonl(ET_NET_ALIVE);
    /* If ET system is NOT alive, or if ET system was killed and
     * restarted (breaking tcp connection), we'll get a write error
     * so signal ET system as dead.
     */
    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) &com, sizeof(com)) != sizeof(com)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_alive, write error\n");
        }
        return 0;
    }
    if (et_tcp_read(sockfd, &alive, sizeof(alive)) != sizeof(alive)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_alive, read error\n");
        }
        return 0;
    }

    et_tcp_unlock(etid);
    return ntohl(alive);
}

/*****************************************************/
/* This function behaves differently for a remote application
 * than for a local one. It is impossible to talk to a system
 * if it isn't yet alive or if it was killed and restarted,
 * since the tcp connection will have been nonexistant or destroyed.
 * Waiting and repeatedly trying any communication will be
 * an exercise in futility.
 */
int etr_wait_for_alive(et_sys_id id)
{
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    int returned, com;

    com = htonl(ET_NET_WAIT);
    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) &com, sizeof(com)) != sizeof(com)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_wait_for_alive, write error\n");
        }
        return ET_ERROR_WRITE;
    }
    if (et_tcp_read(sockfd, &returned, sizeof(returned)) != sizeof(returned)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_wait_for_alive, read error\n");
        }
        return ET_ERROR_READ;
    }
    et_tcp_unlock(etid);
    /* if communication with ET is successful, it's alive */
    return ET_OK;
}

/*****************************************************/
int etr_wakeup_attachment(et_sys_id id, et_att_id att)
{
    et_id *etid = (et_id *) id;
    int transfer[2], sockfd = etid->sockfd;

    transfer[0] = htonl(ET_NET_WAKE_ATT);
    transfer[1] = htonl(att);

    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_wakeup_attachment, write error\n");
        }
        return ET_ERROR_WRITE;
    }
    et_tcp_unlock(etid);
    /* always return ET_OK, so don't bother to send over network */
    return ET_OK;
}

/*****************************************************/
int etr_wakeup_all(et_sys_id id, et_stat_id stat_id)
{
    et_id *etid = (et_id *) id;
    int transfer[2], sockfd = etid->sockfd;

    transfer[0] = htonl(ET_NET_WAKE_ALL);
    transfer[1] = htonl(stat_id);

    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_wakeup_all, write error\n");
        }
        return ET_ERROR_WRITE;
    }
    et_tcp_unlock(etid);
    /* always return ET_OK, so don't bother to send over network */
    return ET_OK;
}

/*****************************************************/
/*                     ET STATION                    */
/*****************************************************/
int etr_station_create_at(et_sys_id id, et_stat_id *stat_id,const char *stat_name,
                          et_statconfig sconfig, int position, int parallelposition)
{
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    int i, err, bufsize, lengthname, lengthfname, lengthlib, lengthclass;
    int ints[15+ET_STATION_SELECT_INTS], transfer[2];
    et_stat_config *sc = (et_stat_config *) sconfig;
    char *buf, *pbuf;

    /* command */
    ints[0] = htonl(ET_NET_STAT_CRAT);

    /* station configuration data */
    ints[1] = htonl(sc->init);
    ints[2] = htonl(sc->flow_mode);
    ints[3] = htonl(sc->user_mode);
    ints[4] = htonl(sc->restore_mode);
    ints[5] = htonl(sc->block_mode);
    ints[6] = htonl(sc->prescale);
    ints[7] = htonl(sc->cue);
    ints[8] = htonl(sc->select_mode);
    for (i=0 ; i < ET_STATION_SELECT_INTS ; i++) {
        ints[9+i] = htonl(sc->select[i]);
    }
    lengthfname = strlen(sc->fname)+1;
    lengthlib   = strlen(sc->lib)+1;
    lengthclass = strlen(sc->classs)+1;
    ints[9 +ET_STATION_SELECT_INTS] = htonl(lengthfname);
    ints[10+ET_STATION_SELECT_INTS] = htonl(lengthlib);
    ints[11+ET_STATION_SELECT_INTS] = htonl(lengthclass);

    /* station name and position */
    lengthname = strlen(stat_name)+1;
    ints[12+ET_STATION_SELECT_INTS] = htonl(lengthname);
    ints[13+ET_STATION_SELECT_INTS] = htonl(position);
    ints[14+ET_STATION_SELECT_INTS] = htonl(parallelposition);

    /* send it all in one buffer */
    bufsize = sizeof(ints) + lengthname + lengthfname +
              lengthlib  + lengthclass;
    if ( (pbuf = buf = (char *) malloc(bufsize)) == NULL) {
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_create_at, cannot allocate memory\n");
        }
        return ET_ERROR_REMOTE;
    }

    memcpy(pbuf, ints, sizeof(ints));
    pbuf += sizeof(ints);
    /* strings come last */
    memcpy(pbuf, sc->fname, lengthfname);
    pbuf += lengthfname;
    memcpy(pbuf, sc->lib, lengthlib);
    pbuf += lengthlib;
    memcpy(pbuf, sc->classs, lengthclass);
    pbuf += lengthclass;
    memcpy(pbuf, stat_name, lengthname);

    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) buf, bufsize) != bufsize) {
        et_tcp_unlock(etid);
        free(buf);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_create_at, write error\n");
        }
        return ET_ERROR_WRITE;
    }
    free(buf);

    if (et_tcp_read(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_create_at, read error\n");
        }
        return ET_ERROR_READ;
    }
    et_tcp_unlock(etid);
    err = ntohl(transfer[0]);

    if ((err == ET_OK) || (err == ET_ERROR_EXISTS)) {
        *stat_id = ntohl(transfer[1]);
    }

    return err;
}

/*****************************************************/
int etr_station_create(et_sys_id id, et_stat_id *stat_id, const char *stat_name,
                       et_statconfig sconfig)
{
    return etr_station_create_at(id, stat_id, stat_name, sconfig, ET_END, ET_END);
}

/*****************************************************/
int etr_station_remove(et_sys_id id, et_stat_id stat_id)
{
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    int err, transfer[2];

    transfer[0] = htonl(ET_NET_STAT_RM);
    transfer[1] = htonl(stat_id);

    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_remove, write error\n");
        }
        return ET_ERROR_WRITE;
    }

    if (et_tcp_read(sockfd, (void *) &err, sizeof(err)) != sizeof(err)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_remove, read error\n");
        }
        return ET_ERROR_READ;
    }
    et_tcp_unlock(etid);

    return ntohl(err);
}


/******************************************************/
int etr_station_attach(et_sys_id id, et_stat_id stat_id, et_att_id *att)
{
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    int err, length, bufsize, incoming[2], transfer[4];
    char *buf, *pbuf, host[ET_MAXHOSTNAMELEN];

    /* find name of our host */
    if (et_defaulthost(host, ET_MAXHOSTNAMELEN) != ET_OK) {
        if (etid->debug >= ET_DEBUG_WARN) {
            et_logmsg("WARN", "etr_station_attach: cannot find hostname\n");
        }
        length = 0;
    }
    else {
        length = strlen(host)+1;
    }

    transfer[0] = htonl(ET_NET_STAT_ATT);
    transfer[1] = htonl(stat_id);
    transfer[2] = htonl(getpid());
    transfer[3] = htonl(length);

    /* send it all in one buffer */
    bufsize = sizeof(transfer) + length;
    if ( (pbuf = buf = (char *) malloc(bufsize)) == NULL) {
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_attach: cannot allocate memory\n");
        }
        return ET_ERROR_REMOTE;
    }
    memcpy(pbuf, transfer, sizeof(transfer));
    pbuf += sizeof(transfer);
    memcpy(pbuf, host, length);

    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) buf, bufsize) != bufsize) {
        et_tcp_unlock(etid);
        free(buf);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_attach, write error\n");
        }
        return ET_ERROR_WRITE;
    }
    free(buf);

    if (et_tcp_read(sockfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_attach, read error\n");

        }
        return ET_ERROR_READ;
    }
    et_tcp_unlock(etid);
    err = ntohl(incoming[0]);

    if (err == ET_OK) {
        if (att) {
            *att = ntohl(incoming[1]);
        }
    }

    return err;
}


/******************************************************/
/*  This routine is called by ET system and by users  */
int etr_station_detach(et_sys_id id, et_att_id att)
{
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    int err, transfer[2];

    transfer[0] = htonl(ET_NET_STAT_DET);
    transfer[1] = htonl(att);

    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_detach, write error\n");
        }
        return ET_ERROR_WRITE;
    }

    if (et_tcp_read(sockfd, (void *) &err, sizeof(err)) != sizeof(err)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_detach, read error\n");
        }
        return ET_ERROR_READ;
    }
    et_tcp_unlock(etid);

    return ntohl(err);
}


/*****************************************************/
int etr_station_setposition(et_sys_id id, et_stat_id stat_id, int position,
                            int parallelposition)
{
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    int err, transfer[4];

    transfer[0] = htonl(ET_NET_STAT_SPOS);
    transfer[1] = htonl(stat_id);
    transfer[2] = htonl(position);
    transfer[2] = htonl(parallelposition);

    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_setposition, write error\n");
        }
        return ET_ERROR_WRITE;
    }

    if (et_tcp_read(sockfd, (void *) &err, sizeof(err)) != sizeof(err)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_setposition, read error\n");
        }
        return ET_ERROR_READ;
    }
    et_tcp_unlock(etid);

    return ntohl(err);
}


/******************************************************/
int etr_station_getposition(et_sys_id id, et_stat_id stat_id, int *position,
                            int *parallelposition)
{
    et_id *etid = (et_id *) id;
    int err, transfer[3], sockfd = etid->sockfd;

    transfer[0] = htonl(ET_NET_STAT_GPOS);
    transfer[1] = htonl(stat_id);

    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) transfer, 2*sizeof(int)) != 2*sizeof(int)) {
        et_tcp_unlock(id);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_getposition, write error\n");
        }
        return ET_ERROR_WRITE;
    }

    if (et_tcp_read(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_getposition, read error\n");
        }
        return ET_ERROR_READ;
    }
    et_tcp_unlock(etid);
    err = ntohl(transfer[0]);

    if (err == ET_OK) {
        if (position) {
            *position = ntohl(transfer[1]);
        }
        if (parallelposition) {
            *parallelposition = ntohl(transfer[2]);
        }
    }

    return err;
}


/******************************************************/
/*                STATION INFORMATION                 */
/******************************************************/
/* is this att attached to this station ? */
int etr_station_isattached(et_sys_id id, et_stat_id stat_id, et_att_id att)
{
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    int err, transfer[3];

    transfer[0] = htonl(ET_NET_STAT_ISAT);
    transfer[1] = htonl(stat_id);
    transfer[2] = htonl(att);

    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_isattached, write error\n");
        }
        return ET_ERROR_WRITE;
    }

    if (et_tcp_read(sockfd, (void *) &err, sizeof(err)) != sizeof(err)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_isattached: read error\n");
        }
        return ET_ERROR_READ;
    }
    et_tcp_unlock(etid);

    return ntohl(err);
}

/******************************************************/
int etr_station_exists(et_sys_id id, et_stat_id *stat_id, const char *stat_name)
{
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    int err, com, length, len, bufsize, transfer[2];
    char *buf, *pbuf;

    length = strlen(stat_name)+1;
    len    = htonl(length);
    com    = htonl(ET_NET_STAT_EX);

    /* send it all in one buffer */
    bufsize = sizeof(com)+sizeof(len)+length;
    if ( (pbuf = buf = (char *) malloc(bufsize)) == NULL) {
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_exists: cannot allocate memory\n");
        }
        return ET_ERROR_REMOTE;
    }
    memcpy(pbuf, &com, sizeof(com));
    pbuf += sizeof(com);
    memcpy(pbuf, &len, sizeof(len));
    pbuf += sizeof(len);
    memcpy(pbuf, stat_name, length);

    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) buf, bufsize) != bufsize) {
        et_tcp_unlock(etid);
        free(buf);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_exists, write error\n");
        }
        return ET_ERROR_WRITE;
    }
    free(buf);

    if (et_tcp_read(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_exists, read error\n");
        }
        return ET_ERROR_READ;
    }
    et_tcp_unlock(etid);
    err = ntohl(transfer[0]);

    if (err == 1) {
        if (stat_id) {
            *stat_id = ntohl(transfer[1]);
        }
    }

    return err;
}

/******************************************************/
int etr_station_setselectwords (et_sys_id id, et_stat_id stat_id, int select[])
{
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    int i, err, transfer[2+ET_STATION_SELECT_INTS];

    transfer[0] = htonl(ET_NET_STAT_SSW);
    transfer[1] = htonl(stat_id);
    for (i=0; i < ET_STATION_SELECT_INTS; i++) {
        transfer[i+2] = htonl(select[i]);
    }

    et_tcp_lock(id);
    if (et_tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(id);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_setselectwords, write error\n");
        }
        return ET_ERROR_WRITE;
    }

    if (et_tcp_read(sockfd, (void *) &err, sizeof(err)) != sizeof(err)) {
        et_tcp_unlock(id);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_setselectwords, read error\n");
        }
        return ET_ERROR_READ;
    }
    et_tcp_unlock(id);

    return ntohl(err);
}

/******************************************************/
int etr_station_getselectwords(et_sys_id id, et_stat_id stat_id, int select[])
{
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    int i, err, send[2], transfer[1+ET_STATION_SELECT_INTS];

    send[0] = htonl(ET_NET_STAT_GSW);
    send[1] = htonl(stat_id);

    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) send, sizeof(send)) != sizeof(send)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_getselectwords, write error\n");
        }
        return ET_ERROR_WRITE;
    }

    if (et_tcp_read(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_getselectwords, read error\n");
        }
        return ET_ERROR_READ;
    }
    et_tcp_unlock(etid);
    err = ntohl(transfer[0]);

    if (err == ET_OK) {
        if (select) {
            for (i=0; i<ET_STATION_SELECT_INTS; i++) {
                select[i] = ntohl(transfer[i+1]);
            }
        }
    }

    return err;
}

/******************************************************/
int etr_station_getlib(et_sys_id id, et_stat_id stat_id, char *lib)
{
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    int err, len, transfer[2];
    char libname[ET_FILENAME_LENGTH];

    transfer[0] = htonl(ET_NET_STAT_LIB);
    transfer[1] = htonl(stat_id);

    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_getlib, write error\n");
        }
        return ET_ERROR_WRITE;
    }

    if (et_tcp_read(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_getlib, read error\n");
        }
        return ET_ERROR_READ;
    }
    err = ntohl(transfer[0]);

    if (err == ET_OK) {
        len = ntohl(transfer[1]);
        if (et_tcp_read(sockfd, (void *) libname, len) != len) {
            et_tcp_unlock(etid);
            if (etid->debug >= ET_DEBUG_ERROR) {
                et_logmsg("ERROR", "etr_station_getlib, read error\n");
            }
            return ET_ERROR_READ;
        }
        if (lib) {
            strcpy(lib, libname);
        }
    }
    et_tcp_unlock(etid);

    return err;
}

/******************************************************/
int etr_station_getclass(et_sys_id id, et_stat_id stat_id, char *classs)
{
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    int err, len, transfer[2];
    char classname[ET_FILENAME_LENGTH];

    transfer[0] = htonl(ET_NET_STAT_CLASS);
    transfer[1] = htonl(stat_id);

    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_getclass, write error\n");
        }
        return ET_ERROR_WRITE;
    }

    if (et_tcp_read(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_getclass, read error\n");
        }
        return ET_ERROR_READ;
    }
    err = ntohl(transfer[0]);

    if (err == ET_OK) {
        len = ntohl(transfer[1]);
        if (et_tcp_read(sockfd, (void *) classname, len) != len) {
            et_tcp_unlock(etid);
            if (etid->debug >= ET_DEBUG_ERROR) {
                et_logmsg("ERROR", "etr_station_getclass, read error\n");
            }
            return ET_ERROR_READ;
        }
        if (classs) {
            strcpy(classs, classname);
        }
    }
    et_tcp_unlock(etid);

    return err;
}

/******************************************************/
int etr_station_getfunction(et_sys_id id, et_stat_id stat_id, char *function)
{
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    int err, len, transfer[2];
    char fname[ET_FUNCNAME_LENGTH];

    transfer[0] = htonl(ET_NET_STAT_FUNC);
    transfer[1] = htonl(stat_id);

    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_getfunction, write error\n");
        }
        return ET_ERROR_WRITE;
    }

    if (et_tcp_read(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_station_getfunction, read error\n");
        }
        return ET_ERROR_READ;
    }
    err = ntohl(transfer[0]);

    if (err == ET_OK) {
        len = ntohl(transfer[1]);
        if (et_tcp_read(sockfd, (void *) fname, len) != len) {
            et_tcp_unlock(etid);
            if (etid->debug >= ET_DEBUG_ERROR) {
                et_logmsg("ERROR", "etr_station_getfunction, read error\n");
            }
            return ET_ERROR_READ;
        }
        if (function) {
            strcpy(function, fname);
        }
    }
    et_tcp_unlock(etid);

    return err;
}


/*****************************************************
 * This gets "int" station parameter data
 *****************************************************/
static int etr_station_getstuff (et_id *id, et_stat_id stat_id, int cmd,
                                 int *stuff, const char *routine)
{
    int err, transfer[2], sockfd = id->sockfd;

    transfer[0] = htonl(cmd);
    transfer[1] = htonl(stat_id);

    et_tcp_lock(id);
    if (et_tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(id);
        if (id->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "%s, write error\n", routine);
        }
        return ET_ERROR_WRITE;
    }

    if (et_tcp_read(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(id);
        if (id->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "%s, read error\n", routine);
        }
        return ET_ERROR_READ;
    }
    et_tcp_unlock(id);
    err = ntohl(transfer[0]);

    if (err == ET_OK) {
        if (stuff) {
            *stuff = ntohl(transfer[1]);
        }
    }

    return err;
}

/******************************************************/
int etr_station_getattachments(et_sys_id id, et_stat_id stat_id, int *numatts)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_station_getstuff(etid, stat_id, ET_NET_STAT_GATTS, numatts, "etr_station_getattachments");
    return err;
}

/******************************************************/
int etr_station_getstatus(et_sys_id id, et_stat_id stat_id, int *status)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_station_getstuff(etid, stat_id, ET_NET_STAT_STATUS, status, "etr_station_getstatus");
    return err;
}

/*****************************************************/
int etr_station_getinputcount (et_sys_id id, et_stat_id stat_id, int *cnt)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_station_getstuff(etid, stat_id, ET_NET_STAT_INCNT, cnt, "etr_station_getinputcount");
    return err;
}

/*****************************************************/
int etr_station_getoutputcount (et_sys_id id, et_stat_id stat_id, int *cnt)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_station_getstuff(etid, stat_id, ET_NET_STAT_OUTCNT, cnt, "etr_station_getoutputcount");
    return err;
}

/*****************************************************/
int etr_station_getblock (et_sys_id id, et_stat_id stat_id, int *block)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_station_getstuff(etid, stat_id, ET_NET_STAT_GBLOCK, block, "etr_station_getblock");
    return err;
}

/*****************************************************/
int etr_station_getuser (et_sys_id id, et_stat_id stat_id, int *user)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_station_getstuff(etid, stat_id, ET_NET_STAT_GUSER, user, "etr_station_getuser");
    return err;
}

/*****************************************************/
int etr_station_getrestore (et_sys_id id, et_stat_id stat_id, int *restore)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_station_getstuff(etid, stat_id, ET_NET_STAT_GRESTORE, restore, "etr_station_getrestore");
    return err;
}

/*****************************************************/
int etr_station_getselect (et_sys_id id, et_stat_id stat_id, int *select)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_station_getstuff(etid, stat_id, ET_NET_STAT_GSELECT, select, "etr_station_getselect");
    return err;
}

/*****************************************************/
int etr_station_getcue (et_sys_id id, et_stat_id stat_id, int *cue)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_station_getstuff(etid, stat_id, ET_NET_STAT_GCUE, cue, "etr_station_getcue");
    return err;
}

/*****************************************************/
int etr_station_getprescale (et_sys_id id, et_stat_id stat_id, int *prescale)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_station_getstuff(etid, stat_id, ET_NET_STAT_GPRE, prescale, "etr_station_getprescale");
    return err;
}

/*****************************************************
 * This sets "int" station parameter data
 *****************************************************/
static int etr_station_setstuff (et_id *id, et_stat_id stat_id, int cmd,
                                 int value, const char *routine)
{
    int err, transfer[3], sockfd = id->sockfd;

    transfer[0] = htonl(cmd);
    transfer[1] = htonl(stat_id);
    transfer[2] = htonl(value);

    et_tcp_lock(id);
    if (et_tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(id);
        if (id->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "%s, write error\n", routine);
        }
        return ET_ERROR_WRITE;
    }

    if (et_tcp_read(sockfd, (void *) &err, sizeof(err)) != sizeof(err)) {
        et_tcp_unlock(id);
        if (id->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "%s, read error\n", routine);
        }
        return ET_ERROR_READ;
    }
    et_tcp_unlock(id);

    return ntohl(err);
}

/******************************************************/
int etr_station_setblock(et_sys_id id, et_stat_id stat_id, int block)
{
    et_id *etid = (et_id *) id;
    return etr_station_setstuff(etid, stat_id, ET_NET_STAT_SBLOCK,
                                block, "etr_station_setblock");
}

/******************************************************/
int etr_station_setuser(et_sys_id id, et_stat_id stat_id, int user)
{
    et_id *etid = (et_id *) id;
    return etr_station_setstuff(etid, stat_id, ET_NET_STAT_SUSER,
                                user, "etr_station_setuser");
}

/******************************************************/
int etr_station_setrestore(et_sys_id id, et_stat_id stat_id, int restore)
{
    et_id *etid = (et_id *) id;
    return etr_station_setstuff(etid, stat_id, ET_NET_STAT_SRESTORE,
                                restore, "etr_station_setrestore");
}

/******************************************************/
int etr_station_setprescale(et_sys_id id, et_stat_id stat_id, int prescale)
{
    et_id *etid = (et_id *) id;
    return etr_station_setstuff(etid, stat_id, ET_NET_STAT_SPRE,
                                prescale, "etr_station_setprescale");
}

/******************************************************/
int etr_station_setcue(et_sys_id id, et_stat_id stat_id, int cue)
{
    et_id *etid = (et_id *) id;
    return etr_station_setstuff(etid, stat_id, ET_NET_STAT_SCUE,
                                cue, "etr_station_setcue");
}


/*****************************************************
 * This gets "int" system parameter data
 *****************************************************/
static int etr_system_getstuff (et_id *id, int cmd, int *stuff, const char *routine)
{
    int err, transfer[2], sockfd = id->sockfd;

    transfer[0] = htonl(cmd);

    et_tcp_lock(id);
    if (et_tcp_write(sockfd, (void *) transfer, sizeof(int)) != sizeof(int)) {
        et_tcp_unlock(id);
        if (id->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "%s, write error\n", routine);
        }
        return ET_ERROR_WRITE;
    }

    if (et_tcp_read(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(id);
        if (id->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "%s, read error\n", routine);
        }
        return ET_ERROR_READ;
    }
    et_tcp_unlock(id);
    err = ntohl(transfer[0]);

    if (err == ET_OK) {
        if (stuff) {
            *stuff = ntohl(transfer[1]);
        }
    }

    return err;
}

/*****************************************************/
int etr_system_gettemps (et_sys_id id, int *temps)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_system_getstuff(etid, ET_NET_SYS_TMP, temps, "etr_system_gettemps");
    return err;
}

/*****************************************************/
int etr_system_gettempsmax (et_sys_id id, int *tempsmax)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_system_getstuff(etid, ET_NET_SYS_TMPMAX, tempsmax, "etr_system_gettempsmax");
    return err;
}

/*****************************************************/
int etr_system_getstations (et_sys_id id, int *stations)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_system_getstuff(etid, ET_NET_SYS_STAT, stations, "etr_system_getstations");
    return err;
}

/*****************************************************/
int etr_system_getstationsmax (et_sys_id id, int *stationsmax)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_system_getstuff(etid, ET_NET_SYS_STATMAX, stationsmax, "etr_system_getstationsmax");
    return err;
}

/*****************************************************/
int etr_system_getprocs (et_sys_id id, int *procs)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_system_getstuff(etid, ET_NET_SYS_PROC, procs, "etr_system_getprocs");
    return err;
}

/*****************************************************/
int etr_system_getprocsmax (et_sys_id id, int *procsmax)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_system_getstuff(etid, ET_NET_SYS_PROCMAX, procsmax, "etr_system_getprocsmax");
    return err;
}

/*****************************************************/
int etr_system_getattachments (et_sys_id id, int *atts)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_system_getstuff(etid, ET_NET_SYS_ATT, atts, "etr_system_getattachments");
    return err;
}

/*****************************************************/
int etr_system_getattsmax (et_sys_id id, int *attsmax)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_system_getstuff(etid, ET_NET_SYS_ATTMAX, attsmax, "etr_system_getattsmax");
    return err;
}

/*****************************************************/
int etr_system_getheartbeat (et_sys_id id, int *heartbeat)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_system_getstuff(etid, ET_NET_SYS_HBEAT, heartbeat, "etr_system_getheartbeat");
    return err;
}

/*****************************************************/
int etr_system_getpid (et_sys_id id, int *pid)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_system_getstuff(etid, ET_NET_SYS_PID, pid, "etr_system_getpid");
    return err;
}

/*****************************************************/
int etr_system_getgroupcount (et_sys_id id, int *groupCnt)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_system_getstuff(etid, ET_NET_SYS_GRP, groupCnt, "etr_system_getgroupcount");
    return err;
}

/*****************************************************
 * This gets 64 bit integer attachment data
 *****************************************************/
static int etr_attach_getstuff (et_id *id, et_att_id att_id, int cmd,
                                uint64_t *stuff,
                                const char *routine)
{
    int err, transfer[3], sockfd = id->sockfd;

    transfer[0] = htonl(cmd);
    transfer[1] = htonl(att_id);

    et_tcp_lock(id);
    if (et_tcp_write(sockfd, (void *) transfer, 2*sizeof(int)) != 2*sizeof(int)) {
        et_tcp_unlock(id);
        if (id->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "%s, write error\n", routine);
        }
        return ET_ERROR_WRITE;
    }

    if (et_tcp_read(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(id);
        if (id->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "%s, read error\n", routine);
        }
        return ET_ERROR_READ;
    }
    et_tcp_unlock(id);
    err = ntohl(transfer[0]);

    if (err == ET_OK) {
        if (stuff != NULL) {
            *stuff = ET_64BIT_UINT(ntohl(transfer[1]),ntohl(transfer[2]));
        }
    }

    return err;
}

/*****************************************************/
int etr_attach_geteventsput(et_sys_id id, et_att_id att_id,
                            uint64_t *events)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_attach_getstuff(etid, att_id, ET_NET_ATT_PUT,
                              events, "etr_attach_geteventsput");
    return err;
}

/*****************************************************/
int etr_attach_geteventsget(et_sys_id id, et_att_id att_id,
                            uint64_t *events)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_attach_getstuff(etid, att_id, ET_NET_ATT_GET,
                              events, "etr_attach_geteventsget");
    return err;
}

/*****************************************************/
int etr_attach_geteventsdump(et_sys_id id, et_att_id att_id,
                             uint64_t *events)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_attach_getstuff(etid, att_id, ET_NET_ATT_DUMP,
                              events, "etr_attach_geteventsdump");
    return err;
}

/*****************************************************/
int etr_attach_geteventsmake(et_sys_id id, et_att_id att_id,
                             uint64_t *events)
{
    int err;
    et_id *etid = (et_id *) id;

    err = etr_attach_getstuff(etid, att_id, ET_NET_ATT_MAKE,
                              events, "etr_attach_geteventsmake");
    return err;
}


/******************************************************/
/*                   ET EVENT STUFF                   */
/******************************************************/

/******************************************************
 * This routine is a little different than other
 * et_event_get/set routines as it must only
 * be used in a remote application. Use this in conjunction
 * with the mode flag of ET_NOALLOC in et_event(s)_new.
 * This allows the user to avoid allocating event data
 * memory, but instead to supply a buffer. This buffer
 * is given by the "data" argument in the following routine.
 * If the user does a "put" of an event with having set
 * the buffer, the user-supplied buffer is not freed.
 ******************************************************/
int et_event_setdatabuffer(et_sys_id id, et_event *pe, void *data)
{
    et_id *etid = (et_id *) id;

    if (etid->locality != ET_REMOTE) {
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "et_event_setdatapointer, user must be remote to use this routine\n");
        }
        return ET_ERROR;
    }
    if (data == NULL) {
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "et_event_setdatapointer, data argument cannot be null\n");
        }
        return ET_ERROR;
    }
    pe->pdata = data;
    return ET_OK;
}

/******************************************************/
int etr_event_new(et_sys_id id, et_att_id att, et_event **ev,
                  int mode, struct timespec *deltatime, size_t size)
{
    int err, transfer[7], incoming[3], wait, noalloc;
    size_t eventsize;
    uint64_t pevent;
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    et_event *newevent;

    /* Pick out wait & no-allocate parts of mode.
     * Value of wait is checked in et_event_new. */
    wait = mode & ET_WAIT_MASK;

    /* Do not allocate memory. Use buffer to be designated later. */
    noalloc = mode & ET_NOALLOC;

    transfer[0] = htonl(ET_NET_EV_NEW);
    transfer[1] = htonl(att);
    transfer[2] = htonl(wait);
    transfer[3] = htonl(ET_HIGHINT(size));
    transfer[4] = htonl(ET_LOWINT(size));
    transfer[5] = 0;
    transfer[6] = 0;
    if (deltatime) {
        transfer[5] = htonl(deltatime->tv_sec);
        transfer[6] = htonl(deltatime->tv_nsec);
    }

    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_event_new, write error\n");
        }
        return ET_ERROR_WRITE;
    }

    /* get back a pointer which for later use */
    if (et_tcp_read(sockfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_event_new, read error\n");
        }
        return ET_ERROR_READ;
    }
    et_tcp_unlock(etid);
    err    = ntohl(incoming[0]);
    pevent = ET_64BIT_UINT(ntohl(incoming[1]),ntohl(incoming[2]));

    if (err != ET_OK) {
        return err;
    }

    /* allocate memory for an event */
    if ((newevent = (et_event *) malloc(sizeof(et_event))) == NULL) {
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_event_new, cannot allocate memory\n");
        }
        return ET_ERROR_REMOTE;
    }

    /* initialize it */
    et_init_event(newevent);

    /* Allocate memory for event data. The default event size, even for
     * the remote client, is the standard size defined by the ET system.
     * If the remote client is interested in a larger size, then that
     * request is granted.In this manner, the remote user
     * asking for an event has at least the amount of memory to work
     * with that was originially specified on the ET system. The remote
     * client behaves just as a local one does.
     *
     * There's a problem if the ET sys is 64 bit and this app is 32 bit.
     * If the events on the ET sys are enormous, make sure we don't try
     * to allocate more than a reasonable size. In this case, only
     * allocate what was asked for.
     */
#ifndef _LP64

    if (etid->bit64) {
        /* if event size > ~1G, only allocate what's asked for */
        if (etid->esize > UINT32_MAX/5) {
            eventsize = size;
        }
        else {
            eventsize = (size_t)etid->esize;
        }
    }
    else {
        eventsize = (size_t)etid->esize;
    }
#else
    eventsize = (size_t)etid->esize;
#endif

    if (size > etid->esize) {
        eventsize = size;
        newevent->temp = ET_EVENT_TEMP;
    }

    /* if allocating memory as is normally the case ... */
    if (noalloc == 0) {
        if ((newevent->pdata = malloc(eventsize)) == NULL) {
            if (etid->debug >= ET_DEBUG_ERROR) {
                et_logmsg("ERROR", "etr_event_new, cannot allocate memory\n");
            }
            free(newevent);
            return ET_ERROR_REMOTE;
        }
    }
    /* else if user supplying a buffer ... */
    else {
        /* Take an element of the et_event structure that is unused
         * in a remote setting and use it to record the fact that
         * the user is supplying the data buffer. This way when a
         * "put" is done, ET will not try to free the memory.
         */
        newevent->owner = ET_NOALLOC;
    }
    newevent->length  = size;
    newevent->memsize = eventsize;
    newevent->pointer = pevent;
    newevent->modify  = ET_MODIFY;

    *ev = newevent;

    return ET_OK;
}

/******************************************************/
int etr_events_new(et_sys_id id, et_att_id att, et_event *evs[],
                   int mode, struct timespec *deltatime,
                   size_t size, int num, int *nread)
{
    int i, j, err, temp, nevents, transfer[8], wait, noalloc;
    size_t eventsize;
    uint64_t *pevents;
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    et_event **newevents;

    /* Allocate array of event pointers - store new events here
     * until copied to evs[] when all danger of error is past.
     */
    if ((newevents = (et_event **) calloc(num, sizeof(et_event *))) == NULL) {
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_events_new, cannot allocate memory\n");
        }
        return ET_ERROR_REMOTE;
    }

    /* Pick out wait & no-allocate parts of mode.
     * Value of wait is checked in et_events_new. */
    wait = mode & ET_WAIT_MASK;

    /* Do not allocate memory. Use buffer to be designated later. */
    noalloc = mode & ET_NOALLOC;

    transfer[0] = htonl(ET_NET_EVS_NEW);
    transfer[1] = htonl(att);
    transfer[2] = htonl(wait);
    transfer[3] = htonl(ET_HIGHINT(size));
    transfer[4] = htonl(ET_LOWINT(size));
    transfer[5] = htonl(num);
    transfer[6] = 0;
    transfer[7] = 0;

    if (deltatime) {
        transfer[6] = htonl(deltatime->tv_sec);
        transfer[7] = htonl(deltatime->tv_nsec);
    }

    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_events_new, write error\n");
        }
        free(newevents);
        return ET_ERROR_WRITE;
    }
    /*printf("etr_events_new: sent transfer array, will read err\n");*/

    if (et_tcp_read(sockfd, (void *) &err, sizeof(err)) != sizeof(err)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_events_new, read error\n");
        }
        free(newevents);
        return ET_ERROR_READ;
    }

    err = ntohl(err);
    /*printf("etr_events_new: err = %d\n", err);*/
    if (err < 0) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_events_new, error in server\n");
        }
        free(newevents);
        return err;
    }

    /* number of events to expect */
    nevents = err;
    /*printf("etr_events_new: num events coming = %d\n", nevents);*/

    /* allocate memory for event pointers */
    if ((pevents = (uint64_t *) calloc(nevents, sizeof(uint64_t))) == NULL) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_events_new, cannot allocate memory\n");
        }
        free(newevents);
        return ET_ERROR_REMOTE;
    }

    /* read array of event pointers */
    /*printf("etr_events_new: read in %d 64 bit event pointers\n", nevents);*/
    if (et_tcp_read(sockfd, (void *) pevents, nevents*sizeof(uint64_t)) !=
            nevents*sizeof(uint64_t) ) {

        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_events_new, read error\n");
        }
        free(pevents);
        free(newevents);
        return ET_ERROR_READ;
    }
    et_tcp_unlock(etid);

#ifndef _LP64

    if (etid->bit64) {
        /* if events size > ~1G, only allocate what's asked for */
        if (num*etid->esize > UINT32_MAX/5) {
            eventsize = size;
        }
        else {
            eventsize = (size_t)etid->esize;
        }
    }
    else {
        eventsize = (size_t)etid->esize;
    }
#else
    eventsize = (size_t)etid->esize;
#endif

    /* set size of an event's data memory */
    temp = ET_EVENT_NORMAL;
    if (size > etid->esize) {
        eventsize = size;
        temp = ET_EVENT_TEMP;
    }

    for (i=0; i < nevents; i++) {
        /* allocate memory for event */
        if ((newevents[i] = (et_event *) malloc(sizeof(et_event))) == NULL) {
            if (etid->debug >= ET_DEBUG_ERROR) {
                et_logmsg("ERROR", "etr_events_new, cannot allocate memory\n");
            }
            err = ET_ERROR_REMOTE;
            break;
        }
        /* initialize new event */
        et_init_event(newevents[i]);

        /* if allocating memory for event data as is normally the case ... */
        if (noalloc == 0) {
            if ((newevents[i]->pdata = (void *) malloc(eventsize)) == NULL) {
                if (etid->debug >= ET_DEBUG_ERROR) {
                    et_logmsg("ERROR", "etr_events_new, cannot allocate memory\n");
                }
                free(newevents[i]);
                err = ET_ERROR_REMOTE;
                break;
            }
        }
        /* else if user supplying a buffer ... */
        else {
            /* Take an element of the et_event structure that is unused
             * in a remote setting and use it to record the fact that
             * the user is supplying the data buffer. This way when a
             * "put" is done, ET will not try to free the memory.
             */
            newevents[i]->owner = ET_NOALLOC;
        }
        newevents[i]->length  = size;
        newevents[i]->memsize = eventsize;
        newevents[i]->modify  = ET_MODIFY;
        newevents[i]->pointer = ntoh64(pevents[i]);
        /*printf("etr_events_new:   pointer[%d] = 0x%llx\n",i,newevents[i]->pointer );*/
        newevents[i]->temp    = temp;
    }

    /* if error in above for loop ... */
    if (err < 0) {
        /* free up all allocated memory */
        for (j=0; j < i; j++) {
            if (noalloc == 0) {
                free(newevents[j]->pdata);
            }
            free(newevents[j]);
        }
        free(pevents);
        free(newevents);
        return err;
    }

    /* now that all is OK, copy into user's array of event pointers */
    for (i=0; i < nevents; i++) {
        evs[i] = newevents[i];
    }
    if (nread != NULL) {
        *nread = nevents;
    }
    free(pevents);
    free(newevents);

    return ET_OK;
}

/******************************************************/
int etr_events_new_group(et_sys_id id, et_att_id att, et_event *evs[],
                   int mode, struct timespec *deltatime,
                   size_t size, int num, int group, int *nread)
{
    int i, j, err, temp, nevents, transfer[9], wait, noalloc;
    size_t eventsize;
    uint64_t *pevents;
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    et_event **newevents;

    /* Allocate array of event pointers - store new events here
     * until copied to evs[] when all danger of error is past.
     */
    if ((newevents = (et_event **) calloc(num, sizeof(et_event *))) == NULL) {
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_events_new, cannot allocate memory\n");
        }
        return ET_ERROR_REMOTE;
    }

    /* Pick out wait & no-allocate parts of mode.
     * Value of wait is checked in et_events_new. */
    wait = mode & ET_WAIT_MASK;

    /* Do not allocate memory. Use buffer to be designated later. */
    noalloc = mode & ET_NOALLOC;

    transfer[0] = htonl(ET_NET_EVS_NEW_GRP);
    transfer[1] = htonl(att);
    transfer[2] = htonl(wait);
    transfer[3] = htonl(ET_HIGHINT(size));
    transfer[4] = htonl(ET_LOWINT(size));
    transfer[5] = htonl(num);
    transfer[6] = htonl(group);
    transfer[7] = 0;
    transfer[8] = 0;

    if (deltatime) {
        transfer[7] = htonl(deltatime->tv_sec);
        transfer[8] = htonl(deltatime->tv_nsec);
    }

    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_events_new, write error\n");
        }
        free(newevents);
        return ET_ERROR_WRITE;
    }
    /*printf("etr_events_new: sent transfer array, will read err\n");*/

    if (et_tcp_read(sockfd, (void *) &err, sizeof(err)) != sizeof(err)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_events_new, read error\n");
        }
        free(newevents);
        return ET_ERROR_READ;
    }

    err = ntohl(err);
    /*printf("etr_events_new: err = %d\n", err);*/
    if (err < 0) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_events_new, error in server\n");
        }
        free(newevents);
        return err;
    }

    /* number of events to expect */
    nevents = err;
    /*printf("etr_events_new: num events coming = %d\n", nevents);*/

    /* allocate memory for event pointers */
    if ((pevents = (uint64_t *) calloc(nevents, sizeof(uint64_t))) == NULL) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_events_new, cannot allocate memory\n");
        }
        free(newevents);
        return ET_ERROR_REMOTE;
    }

    /* read array of event pointers */
    /*printf("etr_events_new: read in %d 64 bit event pointers\n", nevents);*/
    if (et_tcp_read(sockfd, (void *) pevents, nevents*sizeof(uint64_t)) !=
            nevents*sizeof(uint64_t) ) {

        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_events_new, read error\n");
        }
        free(pevents);
        free(newevents);
        return ET_ERROR_READ;
    }
    et_tcp_unlock(etid);

#ifndef _LP64

    if (etid->bit64) {
        /* if events size > ~1G, only allocate what's asked for */
        if (num*etid->esize > UINT32_MAX/5) {
            eventsize = size;
        }
        else {
            eventsize = (size_t)etid->esize;
        }
    }
    else {
        eventsize = (size_t)etid->esize;
    }
#else
    eventsize = (size_t)etid->esize;
#endif

    /* set size of an event's data memory */
    temp = ET_EVENT_NORMAL;
    if (size > etid->esize) {
        eventsize = size;
        temp = ET_EVENT_TEMP;
    }

    for (i=0; i < nevents; i++) {
        /* allocate memory for event */
        if ((newevents[i] = (et_event *) malloc(sizeof(et_event))) == NULL) {
            if (etid->debug >= ET_DEBUG_ERROR) {
                et_logmsg("ERROR", "etr_events_new, cannot allocate memory\n");
            }
            err = ET_ERROR_REMOTE;
            break;
        }
        /* initialize new event */
        et_init_event(newevents[i]);
        /*newevents[i]->group = group;*/

        /* if allocating memory for event data as is normally the case ... */
        if (noalloc == 0) {
            if ((newevents[i]->pdata = (void *) malloc(eventsize)) == NULL) {
                if (etid->debug >= ET_DEBUG_ERROR) {
                    et_logmsg("ERROR", "etr_events_new, cannot allocate memory\n");
                }
                free(newevents[i]);
                err = ET_ERROR_REMOTE;
                break;
            }
        }
        /* else if user supplying a buffer ... */
        else {
            /* Take an element of the et_event structure that is unused
             * in a remote setting and use it to record the fact that
             * the user is supplying the data buffer. This way when a
             * "put" is done, ET will not try to free the memory.
             */
            newevents[i]->owner = ET_NOALLOC;
        }
        newevents[i]->length  = size;
        newevents[i]->memsize = eventsize;
        newevents[i]->modify  = ET_MODIFY;
        newevents[i]->pointer = ntoh64(pevents[i]);
        /*printf("etr_events_new:   pointer[%d] = 0x%llx\n",i,newevents[i]->pointer );*/
        newevents[i]->temp    = temp;
    }

    /* if error in above for loop ... */
    if (err < 0) {
        /* free up all allocated memory */
        for (j=0; j < i; j++) {
            if (noalloc == 0) {
                free(newevents[j]->pdata);
            }
            free(newevents[j]);
        }
        free(pevents);
        free(newevents);
        return err;
    }

    /* now that all is OK, copy into user's array of event pointers */
    for (i=0; i < nevents; i++) {
        evs[i] = newevents[i];
    }
    if (nread != NULL) {
        *nread = nevents;
    }
    free(pevents);
    free(newevents);

    return ET_OK;
}

/******************************************************/
int etr_event_get(et_sys_id id, et_att_id att, et_event **ev,
                  int mode, struct timespec *deltatime)
{
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    int i, err, modify, wait;
    int transfer[6], header[9+ET_STATION_SELECT_INTS];
    uint64_t len;
    size_t eventsize=0;
    et_event *newevent;

    /* Pick out wait & modify parts of mode.
     * Value of wait is checked in et_event_get. */
    wait = mode & ET_WAIT_MASK;

    /* Modifying the whole event has precedence over modifying
     * only the header should the user specify both.
     */
    modify = mode & ET_MODIFY;
    if (modify == 0) {
        modify = mode & ET_MODIFY_HEADER;
    }

    transfer[0] = htonl(ET_NET_EV_GET);
    transfer[1] = htonl(att);
    transfer[2] = htonl(wait);
    transfer[3] = htonl(modify | (mode & ET_DUMP));
    transfer[4] = 0;
    transfer[5] = 0;
    if (deltatime) {
        transfer[4] = htonl(deltatime->tv_sec);
        transfer[5] = htonl(deltatime->tv_nsec);
    }

    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_event_get, write error\n");
        }
        return ET_ERROR_WRITE;
    }

    if (et_tcp_read(sockfd, (void *) &err, sizeof(err)) != sizeof(err)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_event_get, read error\n");
        }
        return ET_ERROR_READ;
    }
    err = ntohl(err);
    if (err != ET_OK) {
        et_tcp_unlock(etid);
        return err;
    }

    if (et_tcp_read(sockfd, (void *) header, sizeof(header)) != sizeof(header)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_event_get, read error\n");
        }
        return ET_ERROR_READ;
    }

    /* allocate memory for an event */
    if ((newevent = (et_event *) malloc(sizeof(et_event))) == NULL) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_event_get, cannot allocate memory\n");
        }
        return ET_ERROR_REMOTE;
    }

    /* initialize new event */
    et_init_event(newevent);

    newevent->length = len = ET_64BIT_UINT(ntohl(header[0]),ntohl(header[1]));
    newevent->memsize      = ET_64BIT_UINT(ntohl(header[2]),ntohl(header[3]));
    newevent->priority     = ntohl(header[4]) & ET_PRIORITY_MASK;
    newevent->datastatus   =(ntohl(header[4]) & ET_DATA_MASK) >> ET_DATA_SHIFT;
    newevent->pointer      = ET_64BIT_UINT(ntohl(header[5]),ntohl(header[6]));
    newevent->byteorder    = header[7];
    for (i=0; i < ET_STATION_SELECT_INTS; i++) {
        newevent->control[i] = ntohl(header[i+9]);
    }
    newevent->modify = modify;

#ifndef _LP64

    if (etid->bit64) {
        /* if event size > ~1G don't allocate all that space, only enough to hold data */
        if (newevent->memsize > UINT32_MAX/5) {
            eventsize = len;
        }
        else {
            eventsize = (size_t) newevent->memsize;
        }
    }
    else {
        eventsize = (size_t) newevent->memsize;
    }
#else
    eventsize = (size_t) newevent->memsize;
#endif

    if (!(mode & ET_NOALLOC)) {

        if ((newevent->pdata = malloc(eventsize)) == NULL) {
            et_tcp_unlock(etid);
            if (etid->debug >= ET_DEBUG_ERROR) {
                et_logmsg("ERROR", "etr_event_get, cannot allocate memory\n");
            }
            free(newevent);
            return ET_ERROR_REMOTE;
        }

        /* read data */
        if (len > 0) {
            if (et_tcp_read(sockfd, newevent->pdata, len) != len) {
                et_tcp_unlock(etid);
                if (etid->debug >= ET_DEBUG_ERROR) {
                    et_logmsg("ERROR", "etr_event_get, read error\n");
                }
                free(newevent->pdata);
                free(newevent);
                return ET_ERROR_READ;
            }
        }
    }
    et_tcp_unlock(etid);

    *ev = newevent;
    return ET_OK;
}

/******************************************************/
int etr_events_get(et_sys_id id, et_att_id att, et_event *evs[],
                   int mode, struct timespec *deltatime, int num, int *nread)
{
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    int i, j, nevents, err, error, modify, wait;
    int incoming[2], transfer[7], header[9+ET_STATION_SELECT_INTS];
    uint64_t size, len;
    size_t eventsize=0;
    et_event **newevents;

    /* Allocate array of event pointers - store new events here
     * until copied to evs[] when all danger of error is past.
     */
    if ((newevents = (et_event **) calloc(num, sizeof(et_event *))) == NULL) {
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_events_get, cannot allocate memory\n");
        }
        return ET_ERROR_REMOTE;
    }

    /* Pick out wait & modify parts of mode.
     * Value of wait is checked in et_events_get. */
    wait = mode & ET_WAIT_MASK;

    /* Modifying the whole event has precedence over modifying
     * only the header should the user specify both.
     */
    modify = mode & ET_MODIFY;
    if (modify == 0) {
        modify = mode & ET_MODIFY_HEADER;
    }

    transfer[0] = htonl(ET_NET_EVS_GET);
    transfer[1] = htonl(att);
    transfer[2] = htonl(wait);
    transfer[3] = htonl(modify | (mode & ET_DUMP));
    transfer[4] = htonl(num);
    transfer[5] = 0;
    transfer[6] = 0;

    if (deltatime) {
        transfer[5] = htonl(deltatime->tv_sec);
        transfer[6] = htonl(deltatime->tv_nsec);
    }

    et_tcp_lock(etid);
    if (et_tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_events_get, write error\n");
        }
        free(newevents);
        return ET_ERROR_WRITE;
    }

    if (et_tcp_read(sockfd, (void *) &err, sizeof(err)) != sizeof(err)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_events_get, read error\n");
        }
        free(newevents);
        return ET_ERROR_READ;
    }
    err = ntohl(err);
    if (err < 0) {
        et_tcp_unlock(etid);
        free(newevents);
        return err;
    }
    nevents = err;

    /* read total size of data to come - in bytes */
    if (et_tcp_read(sockfd, (void *) incoming, sizeof(incoming)) != sizeof(incoming)) {
        et_tcp_unlock(etid);
        if (etid->debug >= ET_DEBUG_ERROR) {
            et_logmsg("ERROR", "etr_events_get, read error\n");
        }
        free(newevents);
        return ET_ERROR_READ;
    }
    size = ET_64BIT_UINT(ntohl(incoming[0]),ntohl(incoming[1]));

    error = ET_OK;

    for (i=0; i < nevents; i++) {

        if (et_tcp_read(sockfd, (void *) header, sizeof(header)) != sizeof(header)) {
            if (etid->debug >= ET_DEBUG_ERROR) {
                et_logmsg("ERROR", "etr_events_get, read error\n");
            }
            error = ET_ERROR_READ;
            break;
        }

        /* allocate memory for event */
        if ((newevents[i] = (et_event *) malloc(sizeof(et_event))) == NULL) {
            if (etid->debug >= ET_DEBUG_ERROR) {
                et_logmsg("ERROR", "etr_events_get, cannot allocate memory\n");
            }
            err = ET_ERROR_REMOTE;
            break;
        }

        /* initialize new event */
        et_init_event(newevents[i]);

        newevents[i]->length = len = ET_64BIT_UINT(ntohl(header[0]),ntohl(header[1]));
        newevents[i]->memsize      = ET_64BIT_UINT(ntohl(header[2]),ntohl(header[3]));
        newevents[i]->priority     = ntohl(header[4]) & ET_PRIORITY_MASK;
        newevents[i]->datastatus   =(ntohl(header[4]) & ET_DATA_MASK) >> ET_DATA_SHIFT;
        newevents[i]->pointer      = ET_64BIT_UINT(ntohl(header[5]),ntohl(header[6]));
        /*printf("Got %llx, high = %x, low = %x\n", newevents[i]->pointer,
                                                  ntohl(header[5]),  ntohl(header[6]));
        */
        /*printf("Event %d: len = %llu, memsize = %llu\n", i, len,  newevents[i]->memsize);*/
        newevents[i]->byteorder    = header[7];
        for (j=0; j < ET_STATION_SELECT_INTS; j++) {
            newevents[i]->control[j] = ntohl(header[j+9]);
        }
        newevents[i]->modify = modify;

#ifndef _LP64
        /* The server will not send events that are too big for us,
         * we'll get an error above.
         */
        if (etid->bit64) {
            /* if event size > ~1G don't allocate all that space, only enough to hold data */
            if (newevents[i]->memsize > UINT32_MAX/5) {
                eventsize = len;
            }
            else {
                eventsize = (size_t) newevents[i]->memsize;
            }
        }
        else {
            eventsize = (size_t) newevents[i]->memsize;
        }
#else
        eventsize = (size_t) newevents[i]->memsize;
#endif

        if (!(mode & ET_NOALLOC)) {
            if ((newevents[i]->pdata = (void *) malloc(eventsize)) == NULL) {
                if (etid->debug >= ET_DEBUG_ERROR) {
                    et_logmsg("ERROR", "etr_events_get, cannot allocate memory\n");
                }
                free(newevents[i]);
                error = ET_ERROR_REMOTE;
                break;
            }

            if (len > 0) {
                if (et_tcp_read(sockfd, newevents[i]->pdata, len) != len) {
                    if (etid->debug >= ET_DEBUG_ERROR) {
                        et_logmsg("ERROR", "etr_events_get, read error\n");
                    }
                    free(newevents[i]->pdata);
                    free(newevents[i]);
                    error = ET_ERROR_READ;
                    break;
                }
            }
        }
    }
    et_tcp_unlock(etid);

    if (error != ET_OK) {
        /* free up all allocated memory */
        for (j=0; j < i; j++) {
            free(newevents[j]->pdata);
            free(newevents[j]);
        }
        free(newevents);
        return error;
    }

    /* now that all is OK, copy into user's array of event pointers */
    for (i=0; i < nevents; i++) {
        evs[i] = newevents[i];
    }
    if (nread != NULL) {
        *nread = nevents;
    }
    free(newevents);

    return ET_OK;
}

/******************************************************/
int etr_event_put(et_sys_id id, et_att_id att, et_event *ev)
{
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    int i, err=ET_OK, iov_bufs, transfer[9+ET_STATION_SELECT_INTS];
    struct iovec iov[2];

    /* if we're changing an event or writing a new event ... */
    if (ev->modify > 0) {
        /* Do not send back datastatus bits since they
         * only change if this remote client dies.
         */
        transfer[0] = htonl(ET_NET_EV_PUT);
        transfer[1] = htonl(att);
        transfer[2] = htonl(ET_HIGHINT((uint64_t) ev->pointer));
        transfer[3] = htonl(ET_LOWINT((uint64_t) ev->pointer));
        transfer[4] = htonl(ET_HIGHINT(ev->length));
        transfer[5] = htonl(ET_LOWINT(ev->length));
        transfer[6] = htonl(ev->priority | ev->datastatus << ET_DATA_SHIFT);
        transfer[7] = ev->byteorder;
        transfer[8] = 0; /* not used */
        for (i=0; i < ET_STATION_SELECT_INTS; i++) {
            transfer[i+9] = htonl(ev->control[i]);
        }

        /* send header if modifying header or whole event */
        iov[0].iov_base = (void *) transfer;
        iov[0].iov_len  = sizeof(transfer);
        iov_bufs = 1;

        /* send data only if modifying whole event */
        if (ev->modify == ET_MODIFY) {
            /* The data pointer might be null if using ET_NOALLOC
             * option in et_event(s)_new and the user never supplied
             * a data buffer.
             */
            if (ev->pdata == NULL) {
                if (etid->debug >= ET_DEBUG_ERROR) {
                    et_logmsg("ERROR", "etr_event_put, bad pointer to data\n");
                }
                return ET_ERROR_REMOTE;
            }
            iov[1].iov_base = ev->pdata;
            iov[1].iov_len  = (size_t)ev->length;
            iov_bufs++;
        }

        /* write data */
        et_tcp_lock(etid);
        if (et_tcp_writev(sockfd, iov, iov_bufs, 16) == -1) {
            et_tcp_unlock(etid);
            if (etid->debug >= ET_DEBUG_ERROR) {
                et_logmsg("ERROR", "etr_event_put, write error\n");
            }
            return ET_ERROR_WRITE;
        }

        if (et_tcp_read(sockfd, (void *) &err, sizeof(err)) != sizeof(err)) {
            et_tcp_unlock(etid);
            if (etid->debug >= ET_DEBUG_ERROR) {
                et_logmsg("ERROR", "etr_event_put, read error\n");
            }
            free(ev->pdata);
            free(ev);
            return ET_ERROR_READ;
        }
        et_tcp_unlock(etid);
        err = ntohl(err);
    }

    /* If event data buffer was malloced, then free it,
     * if supplied by the user, do not free it. */
    if (ev->owner != ET_NOALLOC) {
        free(ev->pdata);
    }
    free(ev);

    return err;
}

/******************************************************/
int etr_events_put(et_sys_id id, et_att_id att, et_event *evs[], int num)
{
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    int i, j, err, iov_init, iov_bufs, nevents, transfer[5];
    int index, headersize, *header = NULL;
    uint64_t bytes;
    struct iovec *iov = NULL;

    err        = ET_OK;
    nevents    = 0;
    iov_init   = 0;
    iov_bufs   = 0;
    bytes      = 0ULL;
    index      = 0;
    headersize = (7+ET_STATION_SELECT_INTS)*sizeof(int);

    for (i=0; i < num; i++) {
        /* if modifying an event ... */
        if (evs[i]->modify > 0) {
            /* if first time thru ... */
            if (iov_init == 0)
            {
                iov_init++;
                index = 0;
                if ( (iov = (struct iovec *) calloc(2*num+1, sizeof(struct iovec))) == NULL) {
                    if (etid->debug >= ET_DEBUG_ERROR) {
                        et_logmsg("ERROR", "etr_events_put, cannot allocate memory\n");
                    }
                    return ET_ERROR_REMOTE;
                }
                if ( (header = (int *) calloc(num, headersize)) == NULL) {
                    if (etid->debug >= ET_DEBUG_ERROR) {
                        et_logmsg("ERROR", "etr_events_put, cannot allocate memory\n");
                    }
                    free(iov);
                    return ET_ERROR_REMOTE;
                }
                transfer[0] = htonl(ET_NET_EVS_PUT);
                transfer[1] = htonl(att);
                iov[iov_bufs].iov_base = (void *) transfer;
                iov[iov_bufs].iov_len  = sizeof(transfer);
                iov_bufs++;
            }

            header[index]   = htonl(ET_HIGHINT((uint64_t) evs[i]->pointer));
            header[index+1] = htonl(ET_LOWINT((uint64_t) evs[i]->pointer));
            header[index+2] = htonl(ET_HIGHINT(evs[i]->length));
            header[index+3] = htonl(ET_LOWINT(evs[i]->length));
            header[index+4] = htonl(evs[i]->priority |
                                    evs[i]->datastatus << ET_DATA_SHIFT);
            header[index+5] = evs[i]->byteorder;
            header[index+6] = 0; /* not used */
            for (j=0; j < ET_STATION_SELECT_INTS; j++) {
                header[index+7+j] = htonl(evs[i]->control[j]);
            }
            /*
            printf("etr_events_put:   [0]  = %x\n", ntohl(header[index]));
            printf("etr_events_put:   [1]  = %x\n", ntohl(header[index+1]));
            printf("etr_events_put:   [2]  = %x\n", ntohl(header[index+2]));
            printf("etr_events_put:   [3]  = %x\n", ntohl(header[index+3]));
            printf("etr_events_put:   [4]  = %d\n", ntohl(header[index+4]));
            printf("etr_events_put:   [5]  = 0x%x\n", ntohl(header[index+5]));
            printf("etr_events_put:   [6]  = %d\n", ntohl(header[index+6]));
            printf("etr_events_put:   [7]  = %d\n", ntohl(header[index+7]));
            printf("etr_events_put:   [8]  = %d\n", ntohl(header[index+8]));
            printf("etr_events_put:   [9]  = %d\n", ntohl(header[index+9]));
            printf("etr_events_put:   [10] = %d\n", ntohl(header[index+10]));
            */
            /* send header if modifying header or whole event */
            iov[iov_bufs].iov_base = (void *) &header[index];
            iov[iov_bufs].iov_len  = headersize;
            iov_bufs++;
            bytes += headersize;

            /* send data only if modifying whole event */
            if (evs[i]->modify == ET_MODIFY) {
                /* The data pointer might be null if using ET_NOALLOC
                 * option in et_event(s)_new and the user never supplied
                 * a data buffer.
                 */
                if (evs[i]->pdata == NULL) {
                    if (etid->debug >= ET_DEBUG_ERROR)
                    {
                        et_logmsg("ERROR", "etr_events_put, bad pointer to data\n");
                    }
                    free(iov);
                    free(header);
                    return ET_ERROR_REMOTE;
                }
                iov[iov_bufs].iov_base = evs[i]->pdata;
                iov[iov_bufs].iov_len  = (size_t) evs[i]->length;
                iov_bufs++;
                bytes += evs[i]->length;
            }
            nevents++;
            index += (7+ET_STATION_SELECT_INTS);
            /*printf("etr_events_put: num events stored in header and iov, %d\n", nevents);*/
        }
    }

    if (nevents > 0) {
        /* send # of events & total # bytes */
        transfer[2] = htonl(nevents);
        transfer[3] = htonl(ET_HIGHINT(bytes));
        transfer[4] = htonl(ET_LOWINT(bytes));

        et_tcp_lock(etid);
        if (et_tcp_writev(sockfd, iov, iov_bufs, 16) == -1) {
            et_tcp_unlock(etid);
            if (etid->debug >= ET_DEBUG_ERROR) {
                et_logmsg("ERROR", "etr_events_put, write error\n");
            }
            free(iov);
            free(header);
            return ET_ERROR_WRITE;
        }
        free(iov);
        free(header);

        if (et_tcp_read(sockfd, (void *) &err, sizeof(err)) != sizeof(err)) {
            if (etid->debug >= ET_DEBUG_ERROR) {
                et_logmsg("ERROR", "etr_events_put, read error\n");
            }
            err = ET_ERROR_READ;
        }
        else {
            err = ntohl(err);
        }
        et_tcp_unlock(etid);
    }


    for (i=0; i < num; i++) {
        /* if event data buffer was malloced, then free it,
         * if supplied by the user, do not free it. */
        if (evs[i]->owner != ET_NOALLOC) {
            free(evs[i]->pdata);
        }
        free(evs[i]);
    }

    return err;
}

/******************************************************/
/* Dumping an event means we don't have to bother
 * sending any data, just its pointer.
 */
int etr_event_dump(et_sys_id id, et_att_id att, et_event *ev)
{
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    int err=ET_OK, transfer[4];

    /* If we've told the ET system that we're changing an event
     * or writing a new event, send back some info.
     */
    if (ev->modify > 0) {
        transfer[0] = htonl(ET_NET_EV_DUMP);
        transfer[1] = htonl(att);
        transfer[2] = htonl(ET_HIGHINT((uint64_t) ev->pointer));
        transfer[3] = htonl(ET_LOWINT((uint64_t) ev->pointer));

        /* write data */
        et_tcp_lock(etid);
        if (et_tcp_write(sockfd, (void *) transfer, sizeof(transfer)) != sizeof(transfer)) {
            et_tcp_unlock(etid);
            if (etid->debug >= ET_DEBUG_ERROR) {
                et_logmsg("ERROR", "etr_event_dump, write error\n");
            }
            return ET_ERROR_WRITE;
        }

        if (et_tcp_read(sockfd, (void *) &err, sizeof(err)) != sizeof(err)) {
            et_tcp_unlock(etid);
            if (etid->debug >= ET_DEBUG_ERROR) {
                et_logmsg("ERROR", "etr_event_dump, read error\n");
            }
            free(ev->pdata);
            free(ev);
            return ET_ERROR_READ;
        }
        et_tcp_unlock(etid);
        err = ntohl(err);
    }

    free(ev->pdata);
    free(ev);

    return err;
}

/******************************************************/
int etr_events_dump(et_sys_id id, et_att_id att, et_event *evs[], int num)
{
    et_id *etid = (et_id *) id;
    int sockfd = etid->sockfd;
    int i, err, iov_init, count, index;
    int *transfer = NULL;

    err      = ET_OK;
    index    = 0;
    count    = 0;
    iov_init = 0;

    for (i=0; i < num; i++) {
        /* if modifying an event ... */
        if (evs[i]->modify > 0) {
            /* if first time thru ... */
            if (iov_init == 0) {
                iov_init++;
                if ( (transfer = (int *) calloc(2*num+3, sizeof(int))) == NULL) {
                    if (etid->debug >= ET_DEBUG_ERROR) {
                        et_logmsg("ERROR", "etr_events_dump, cannot allocate memory\n");
                    }
                    return ET_ERROR_REMOTE;
                }

                transfer[0] = htonl(ET_NET_EVS_DUMP);
                transfer[1] = htonl(att);
                index = 3;
            }

            transfer[index++] = htonl(ET_HIGHINT((uint64_t) evs[i]->pointer));
            transfer[index++] = htonl(ET_LOWINT((uint64_t) evs[i]->pointer));

            count++;
        }
    }

    if (count > 0) {
        transfer[2] = htonl(count);

        et_tcp_lock(etid);
        if (et_tcp_write(sockfd, (void *) transfer, (2*count+3)*sizeof(int)) !=
                (2*count+3)*sizeof(int)) {
            et_tcp_unlock(etid);
            if (etid->debug >= ET_DEBUG_ERROR) {
                et_logmsg("ERROR", "etr_events_dump, write error\n");
            }
            free(transfer);
            return ET_ERROR_WRITE;
        }

        free(transfer);

        if (et_tcp_read(sockfd, (void *) &err, sizeof(err)) != sizeof(err)) {
            if (etid->debug >= ET_DEBUG_ERROR) {
                et_logmsg("ERROR", "etr_events_dump, read error\n");
            }
            err = ET_ERROR_READ;
        }
        else {
            err = ntohl(err);
        }
        et_tcp_unlock(etid);
    }


    for (i=0; i < num; i++) {
        free(evs[i]->pdata);
        free(evs[i]);
    }

    return err;
}
