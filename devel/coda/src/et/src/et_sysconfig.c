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
 *      Routines for ET system configuration
 *
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef VXWORKS
#include <strings.h>
#endif
#include "et_private.h"
#include "et_network.h"

/*****************************************************
 * Routines for setting and getting system configuration
 * parameters. Used to create systems..
 *****************************************************/

int et_system_config_init(et_sysconfig* sconfig)
{
  et_sys_config *sc;
  
  sc = (et_sys_config *) malloc(sizeof(et_sys_config));
  if (sc == NULL) {
    return ET_ERROR;
  }
  
  /* default configuration for a system */
  sc->nevents          = ET_SYSTEM_EVENTS;
  sc->event_size       = ET_SYSTEM_ESIZE;
  sc->ntemps           = ET_SYSTEM_NTEMPS;
  sc->nstations        = ET_SYSTEM_NSTATS;
  sc->nprocesses       = ET_PROCESSES_MAX;
  sc->nattachments     = ET_ATTACHMENTS_MAX;
  sc->port             = ET_BROADCAST_PORT;
  sc->serverport       = 0;
  sc->mcastaddrs.count = 0;
  
  /* Find our local subnets' broadcast addresses as
   * well as the interfaces' addresses and names.
   */
  if (et_netinfo(&sc->ifnames, &sc->ifaddrs, &sc->subnets) == ET_ERROR) {
    strcpy(sc->subnets.addr[0], ET_BROADCAST_ADDR);
    sc->subnets.count = 1;
  }
  
  /*
  if (et_defaultbroadcastaddr(sc->address) == ET_ERROR) {
    strcpy(sc->address, ET_BROADCAST_ADDR);
  }
  */
		  
 *sc->filename      = '\0';
  sc->init          = ET_STRUCT_OK;
  
  *sconfig = (et_sysconfig) sc;
  return ET_OK;
}

/*****************************************************/
int et_system_config_destroy(et_sysconfig sconfig)
{
  free((et_sys_config *) sconfig);
  return ET_OK;
}

/*****************************************************/
int et_system_config_setevents(et_sysconfig sconfig, int val)
{
  et_sys_config *sc = (et_sys_config *) sconfig;
  
  if (sc->init != ET_STRUCT_OK) {
    return ET_ERROR;
  }
  if (val < 1) {
    return ET_ERROR;
  }
  
  sc->nevents = val;
  /* number of temp events can't be more than the number of events */
  if (sc->ntemps > val) sc->ntemps = val;
  return ET_OK;
}

/*****************************************************/
int et_system_config_getevents(et_sysconfig sconfig, int *val)
{
  et_sys_config *sc = (et_sys_config *) sconfig;
  
  *val = sc->nevents;
  return ET_OK;
}

/*****************************************************/
int et_system_config_setsize(et_sysconfig sconfig, int val)
{
  et_sys_config *sc = (et_sys_config *) sconfig;
  
  if (sc->init != ET_STRUCT_OK) {
    return ET_ERROR;
  }
  if (val < 1) {
    return ET_ERROR;
  }
  
  sc->event_size = val;
  return ET_OK;
}

/*****************************************************/
int et_system_config_getsize(et_sysconfig sconfig, int *val)
{
  et_sys_config *sc = (et_sys_config *) sconfig;
  
  *val = sc->event_size;
  return ET_OK;
}

/*****************************************************/
int et_system_config_settemps(et_sysconfig sconfig, int val)
{
  et_sys_config *sc = (et_sys_config *) sconfig;
  
  if (sc->init != ET_STRUCT_OK) {
    return ET_ERROR;
  }
  if (val < 1) {
    return ET_ERROR;
  }
  
  sc->ntemps = val;
  return ET_OK;
}

/*****************************************************/
int et_system_config_gettemps(et_sysconfig sconfig, int *val)
{
  et_sys_config *sc = (et_sys_config *) sconfig;
  
  *val = sc->ntemps;
  return ET_OK;
}

/*****************************************************/
int et_system_config_setstations(et_sysconfig sconfig, int val)
{
  et_sys_config *sc = (et_sys_config *) sconfig;
  
  if (sc->init != ET_STRUCT_OK) {
    return ET_ERROR;
  }
  if (val < 1) {
    return ET_ERROR;
  }
  
  sc->nstations = val;
  return ET_OK;
}

/*****************************************************/
int et_system_config_getstations(et_sysconfig sconfig, int *val)
{
  et_sys_config *sc = (et_sys_config *) sconfig;
  
  *val = sc->nstations;
  return ET_OK;
}

/*****************************************************/
int et_system_config_setprocs(et_sysconfig sconfig, int val)
{
  et_sys_config *sc = (et_sys_config *) sconfig;
  
  if (sc->init != ET_STRUCT_OK) {
    return ET_ERROR;
  }
  if ((val < 1) || (val > ET_PROCESSES_MAX)) {
    return ET_ERROR;
  }
  
  sc->nprocesses = val;
  return ET_OK;
}

/*****************************************************/
int et_system_config_getprocs(et_sysconfig sconfig, int *val)
{
  et_sys_config *sc = (et_sys_config *) sconfig;
  
  *val = sc->nprocesses;
  return ET_OK;
}

/*****************************************************/
int et_system_config_setattachments(et_sysconfig sconfig, int val)
{
  et_sys_config *sc = (et_sys_config *) sconfig;
  
  if (sc->init != ET_STRUCT_OK) {
    return ET_ERROR;
  }
  if ((val < 1) || (val > ET_ATTACHMENTS_MAX)) {
    return ET_ERROR;
  }
  
  sc->nattachments = val;
  return ET_OK;
}

/*****************************************************/
int et_system_config_getattachments(et_sysconfig sconfig, int *val)
{
  et_sys_config *sc = (et_sys_config *) sconfig;
  
  *val = sc->nattachments;
  return ET_OK;
}

/*****************************************************/
int et_system_config_setport(et_sysconfig sconfig, unsigned short val)
{
  et_sys_config *sc = (et_sys_config *) sconfig;
  
  if (sc->init != ET_STRUCT_OK) {
    return ET_ERROR;
  }
   
  if (val < 1024) {
    return ET_ERROR;
  }
  
  sc->port = val;
  return ET_OK;
}

/*****************************************************/
int et_system_config_getport(et_sysconfig sconfig, unsigned short *val)
{
  et_sys_config *sc = (et_sys_config *) sconfig;
  
  *val = sc->port;
  return ET_OK;
}

/*****************************************************/
int et_system_config_setserverport(et_sysconfig sconfig, unsigned short val)
{
  et_sys_config *sc = (et_sys_config *) sconfig;
  
  if (sc->init != ET_STRUCT_OK) {
    return ET_ERROR;
  }
   
  if (val < 1024) {
    return ET_ERROR;
  }
  
  sc->serverport = val;
  return ET_OK;
}

/*****************************************************/
int et_system_config_getserverport(et_sysconfig sconfig, unsigned short *val)
{
  et_sys_config *sc = (et_sys_config *) sconfig;
  
  *val = sc->serverport;
  return ET_OK;
}

/*****************************************************
 * This routine and the next are now obsolete and do
 * nothing. Included only for backwards compatibility.
 ****************************************************/
int et_system_config_setcast(et_sysconfig sconfig, int val)
{
  return ET_OK;
}

/*****************************************************/
int et_system_config_getcast(et_sysconfig sconfig, int *val)
{
  return ET_OK;
}

/*****************************************************
 * This routine & the next are deprecated but
 * included for backwards compatibility.
 ****************************************************/
int et_system_config_setaddress(et_sysconfig sconfig, const char *val)
{
  return et_system_config_addmulticast(sconfig, val);
}

/*****************************************************/
int et_system_config_getaddress(et_sysconfig sconfig, char *val)
{
  et_sys_config *sc = (et_sys_config *) sconfig;
  
  /* Return the first multicast address. If none, return the first
   * broadcast address. This is a hack for the sole purpose of
   * backward compatibility.
   */
  
  if (sc->mcastaddrs.count > 0) {
    strcpy(val, sc->mcastaddrs.addr[0]);
  }
  else {
    strcpy(val, sc->subnets.addr[0]);
  }
  
  return ET_OK;
}

/*****************************************************
 * This routine together with et_system_config_removemulticast
 * replaces et_system_config_setaddress.
 ****************************************************/
int et_system_config_addmulticast(et_sysconfig sconfig, const char *val)
{
  et_sys_config *sc = (et_sys_config *) sconfig;
  size_t len;
  int  firstnumber, i;
  char firstint[4];
  
  if (sc->init != ET_STRUCT_OK) {
    return ET_ERROR;
  }
  
  if (val == NULL) {
    return ET_ERROR;
  }
  
  if ((strlen(val) >= ET_IPADDRSTRLEN) || (strlen(val) < 7)) {
    return ET_ERROR;
  }
  
  /* The address is in dotted-decimal form. If the first
   * number is between 224-239, its a multicast address.
   */
  len = strcspn(val, ".");
  strncpy(firstint, val, len);
  firstint[len] = '\0';
  firstnumber = atoi(firstint);
  
  if ((firstnumber < 224) || (firstnumber > 239)) {
    fprintf(stderr, "et_open_config_addmulticast: bad value for multicast address\n");
    return ET_ERROR;
  }
  
  /* Once here, it's probably an address of the right value.
   * Don't add it to the list if it's already there or if
   * there's no more room on the list.
   */
  for (i=0; i < sc->mcastaddrs.count; i++) {
    if (strcmp(val, sc->mcastaddrs.addr[i]) == 0) {
      /* it's already in the list */
      return ET_OK;
    }
  }
  if (sc->mcastaddrs.count == ET_MAXADDRESSES) {
    /* list is already full */
    return ET_ERROR;
  }
  
  strcpy(sc->mcastaddrs.addr[sc->mcastaddrs.count++], val);
  
  return ET_OK;
}

/*****************************************************/
int et_system_config_removemulticast(et_sysconfig sconfig, const char *val)
{
  et_sys_config *sc = (et_sys_config *) sconfig;
  size_t len;
  int  firstnumber, i, j;
  char firstint[4];
  
  if (sc->init != ET_STRUCT_OK) {
    return ET_ERROR;
  }
  
  if (val == NULL) {
    return ET_ERROR;
  }
  
  if ((strlen(val) >= ET_IPADDRSTRLEN) || (strlen(val) < 7)) {
    return ET_ERROR;
  }
  
  /* The address is in dotted-decimal form. If the first
   * number is between 224-239, its a multicast address.
   */
  len = strcspn(val, ".");
  strncpy(firstint, val, len);
  firstint[len] = '\0';
  firstnumber = atoi(firstint);
  
  if ((firstnumber < 224) || (firstnumber > 239)) {
    fprintf(stderr, "et_open_config_addmulticast: bad value for multicast address\n");
    return ET_ERROR;
  }
  
  /* Once here, it's probably an address of the right value.
   * Remove it from the list only if it's already there.
   */
  for (i=0; i < sc->mcastaddrs.count; i++) {
    if (strcmp(val, sc->mcastaddrs.addr[i]) == 0) {
      /* It's in the list. Remove it and move later elements up. */
      for (j=i+1; j < sc->mcastaddrs.count; j++) {
        strcpy(sc->mcastaddrs.addr[j-1], sc->mcastaddrs.addr[j]);
      }
      sc->mcastaddrs.count--;
      return ET_OK;
    }
  }
    
  return ET_OK;
}


/*****************************************************/
int et_system_config_setfile(et_sysconfig sconfig, const char *val)
{
  et_sys_config *sc = (et_sys_config *) sconfig;
  
  if (sc->init != ET_STRUCT_OK) {
    return ET_ERROR;
  }
  if (val == NULL) {
    return ET_ERROR;
  }
  if (strlen(val) > ET_FILENAME_LENGTH - 1) {
    return ET_ERROR;
  }
  
  strcpy(sc->filename, val);

  return ET_OK;
}

/*****************************************************/
int et_system_config_getfile(et_sysconfig sconfig, char *val)
{
  et_sys_config *sc = (et_sys_config *) sconfig;
  
  strcpy(val, sc->filename);
  return ET_OK;
}

/******************************************************/
/* Routines for setting/getting system information    */
/* from an existing system.                           */
/******************************************************/

int et_system_getlocality(et_sys_id id, int *locality)
{
  et_id *etid = (et_id *) id;
  
  if (locality != NULL) {
     *locality = etid->locality;
     return ET_OK;
  }
  return ET_ERROR;
} 

/******************************************************/
int et_system_setdebug(et_sys_id id, int debug)
{
  et_id *etid = (et_id *) id;
  
  if ((debug != ET_DEBUG_NONE)  && (debug != ET_DEBUG_SEVERE) &&
      (debug != ET_DEBUG_ERROR) && (debug != ET_DEBUG_WARN)   &&
      (debug != ET_DEBUG_INFO))  {
     return ET_ERROR;
  }
  etid->debug = debug;
  return ET_OK;
} 

/******************************************************/
int et_system_getdebug(et_sys_id id, int *debug)
{
  et_id *etid = (et_id *) id;
  
  if (debug != NULL) {
     *debug = etid->debug;
     return ET_OK;
  }
  return ET_ERROR;
} 

/******************************************************/
int et_system_getnumevents(et_sys_id id, int *numevents)
{
  et_id *etid = (et_id *) id;
  
  if (numevents != NULL) {
     *numevents = etid->nevents;
     return ET_OK;
  }
  return ET_ERROR;
} 

/******************************************************/
int et_system_geteventsize(et_sys_id id, int *eventsize)
{
  et_id *etid = (et_id *) id;
  
  if (eventsize != NULL) {
     *eventsize = etid->esize;
     return ET_OK;
  }
  return ET_ERROR;
} 

/******************************************************/
int et_system_gettempsmax(et_sys_id id, int *tempsmax)
{
  et_id *etid = (et_id *) id;
  
  if (etid->locality != ET_LOCAL) {
    return etr_system_gettempsmax(id, tempsmax);
  }

  if (tempsmax != NULL) {
     *tempsmax = etid->sys->config.ntemps;
     return ET_OK;
  }
  return ET_ERROR;
}

/******************************************************/
int et_system_getstationsmax(et_sys_id id, int *stationsmax)
{
  et_id *etid = (et_id *) id;
  
  if (etid->locality != ET_LOCAL) {
    return etr_system_getstationsmax(id, stationsmax);
  }

  if (stationsmax != NULL) {
     *stationsmax = etid->sys->config.nstations;
     return ET_OK;
  }
  return ET_ERROR;
}

/******************************************************/
int et_system_getprocsmax(et_sys_id id, int *procsmax)
{
  et_id *etid = (et_id *) id;
  
  if (etid->locality != ET_LOCAL) {
    return etr_system_getprocsmax(id, procsmax);
  }

  if (procsmax != NULL) {
     *procsmax = etid->sys->config.nprocesses;
     return ET_OK;
  }
  return ET_ERROR;
}

/******************************************************/
int et_system_getattsmax(et_sys_id id, int *attsmax)
{
  et_id *etid = (et_id *) id;
  
  if (etid->locality != ET_LOCAL) {
    return etr_system_getattsmax(id, attsmax);
  }

  if (attsmax != NULL) {
     *attsmax = etid->sys->config.nattachments;
     return ET_OK;
  }
  return ET_ERROR;
}

/******************************************************/
int et_system_getheartbeat(et_sys_id id, int *heartbeat)
{
  et_id *etid = (et_id *) id;
  
  if (etid->locality != ET_LOCAL) {
    return etr_system_getheartbeat(id, heartbeat);
  }

  if (heartbeat != NULL) {
     *heartbeat = etid->sys->heartbeat;
     return ET_OK;
  }
  return ET_ERROR;
}

/******************************************************/
int et_system_getpid(et_sys_id id, pid_t *pid)
{
  et_id *etid = (et_id *) id;
  
  if (etid->locality != ET_LOCAL) {
    return etr_system_getpid(id, (int *) pid);
  }

  if (pid != NULL) {
     *pid = etid->sys->mainpid;
     return ET_OK;
  }
  return ET_ERROR;
}

/******************************************************/
int et_system_getprocs(et_sys_id id, int *procs)
{
  et_id *etid = (et_id *) id;
  
  if (etid->locality != ET_LOCAL) {
    return etr_system_getprocs(id, procs);
  }

  if (procs != NULL) {
     *procs = etid->sys->nprocesses;
     return ET_OK;
  }
  return ET_ERROR;
}

/******************************************************/
int et_system_getattachments(et_sys_id id, int *atts)
{
  et_id *etid = (et_id *) id;
  
  if (etid->locality != ET_LOCAL) {
    return etr_system_getattachments(id, atts);
  }

  if (atts != NULL) {
     *atts = etid->sys->nattachments;
     return ET_OK;
  }
  return ET_ERROR;
}

/******************************************************/
int et_system_getstations(et_sys_id id, int *stations)
{
  et_id *etid = (et_id *) id;
  
  if (etid->locality != ET_LOCAL) {
    return etr_system_getstations(id, stations);
  }

  if (stations != NULL) {
     *stations = etid->sys->nstations;
     return ET_OK;
  }
  return ET_ERROR;
}

/******************************************************/
int et_system_gettemps(et_sys_id id, int *temps)
{
  et_id *etid = (et_id *) id;
  
  if (etid->locality != ET_LOCAL) {
    return etr_system_gettemps(id, temps);
  }

  if (temps != NULL) {
     *temps = etid->sys->ntemps;
     return ET_OK;
  }
  return ET_ERROR;
}

/******************************************************/
int et_system_getserverport(et_sys_id id, unsigned short *port)
{
  et_id *etid = (et_id *) id;

  if (port != NULL) {
    *port = etid->port;
    return ET_OK;
  }
  return ET_ERROR;
}

/******************************************************/
int et_system_gethost(et_sys_id id, char *host)
{
  et_id *etid = (et_id *) id;

  if (host != NULL) {
    strcpy(host, etid->ethost);
    return ET_OK;
  }
  return ET_ERROR;
}
