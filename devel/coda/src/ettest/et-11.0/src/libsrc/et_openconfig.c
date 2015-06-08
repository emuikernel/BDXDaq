/*----------------------------------------------------------------------------*
 *  Copyright (c) 2002        Southeastern Universities Research Association, *
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
 *      Routines for defining a configuration for opening ET systems.
 *
 *----------------------------------------------------------------------------*/
 

#include <stdio.h>
#include <string.h>

#include "et_private.h"
#include "et_network.h"

/*****************************************************/
/*          OPEN SYSTEM CONFIGURATION                */
/*****************************************************/

int et_open_config_init(et_openconfig *sconfig)
{
  et_open_config *sc;
  
  sc = (et_open_config *) malloc(sizeof(et_open_config));
  if (sc == NULL) {
    return ET_ERROR;
  }
  
  /* default configuration for a station */
  sc->wait             = ET_OPEN_NOWAIT;
  sc->cast             = ET_BROADCAST;
  sc->ttl              = ET_MULTICAST_TTL;
  sc->mode             = ET_HOST_AS_LOCAL;
  sc->debug_default    = ET_DEBUG_ERROR;
  sc->udpport          = ET_BROADCAST_PORT;
  sc->multiport        = ET_MULTICAST_PORT;
  sc->serverport       = ET_SERVER_PORT;
  sc->timeout.tv_sec   = 0;
  sc->timeout.tv_nsec  = 0;
  strcpy(sc->host, ET_HOST_LOCAL);
  sc->policy           = ET_POLICY_FIRST; /* use first response to broad/multicast */
  sc->mcastaddrs.count = 0;
  
  /* find our local subnets' broadcast addresses */
  if (et_getBroadcastAddrs(&sc->bcastaddrs, NULL) == ET_ERROR) {
    sc->bcastaddrs = NULL;
  }
  
  if (et_getNetInfo(&sc->netinfo, NULL) != ET_OK) {
    sc->netinfo = NULL;
printf("et_open_config_init: error in et_getNetInfo\n");
  }
  
  sc->init = ET_STRUCT_OK;
  
  *sconfig = (et_openconfig) sc;
  return ET_OK;
}

/*****************************************************/
int et_open_config_destroy(et_openconfig sconfig)
{
  et_open_config *sc = (et_open_config *)sconfig;
  
  if (sc == NULL) return ET_OK;
  
  /* first, free network info (linked list) */
  et_freeIpAddrs(sc->netinfo);
  
  /* next, free broadcast info (linked list) */
  et_freeBroadcastAddrs(sc->bcastaddrs);
  
  free(sc);
  return ET_OK;
}

/*****************************************************/
int et_open_config_setwait(et_openconfig sconfig, int val)
{
  et_open_config *sc = (et_open_config *) sconfig;
  
  if (sc->init != ET_STRUCT_OK) {
    return ET_ERROR;
  }
   
  if ((val != ET_OPEN_NOWAIT) &&
      (val != ET_OPEN_WAIT))     {
    return ET_ERROR;
  }
  
  sc->wait = val;
  return ET_OK;
}

/*****************************************************/
int et_open_config_getwait(et_openconfig sconfig, int *val)
{
  et_open_config *sc = (et_open_config *) sconfig;
  
  *val = sc->wait;
  return ET_OK;
}

/*****************************************************/
int et_open_config_setcast(et_openconfig sconfig, int val)
{
  et_open_config *sc = (et_open_config *) sconfig;
  
  if (sc->init != ET_STRUCT_OK) {
    return ET_ERROR;
  }
   
  if ((val != ET_MULTICAST) &&
      (val != ET_BROADCAST) &&
      (val != ET_BROADANDMULTICAST) &&
      (val != ET_DIRECT))  {
    return ET_ERROR;
  }
  
  sc->cast = val;
  return ET_OK;
}

/*****************************************************/
int et_open_config_getcast(et_openconfig sconfig, int *val)
{
  et_open_config *sc = (et_open_config *) sconfig;
  
  *val = sc->cast;
  return ET_OK;
}

/*****************************************************/
int et_open_config_setTTL(et_openconfig sconfig, int val)
{
  et_open_config *sc = (et_open_config *) sconfig;
  
  if (sc->init != ET_STRUCT_OK) {
    return ET_ERROR;
  }
   
  if ((val < 0) || (val > 254))     {
    return ET_ERROR;
  }
  
  sc->ttl = val;
  return ET_OK;
}

/*****************************************************/
int et_open_config_getTTL(et_openconfig sconfig, int *val)
{
  et_open_config *sc = (et_open_config *) sconfig;
  
  *val = sc->ttl;
  return ET_OK;
}

/*****************************************************/
int et_open_config_setmode(et_openconfig sconfig, int val)
{
  et_open_config *sc = (et_open_config *) sconfig;
  
  if (sc->init != ET_STRUCT_OK) {
    return ET_ERROR;
  }
   
  if ((val != ET_HOST_AS_LOCAL) &&
      (val != ET_HOST_AS_REMOTE))     {
    return ET_ERROR;
  }
  
  sc->mode = val;
  return ET_OK;
}

/*****************************************************/
int et_open_config_getmode(et_openconfig sconfig, int *val)
{
  et_open_config *sc = (et_open_config *) sconfig;
  
  *val = sc->mode;
  return ET_OK;
}
/*****************************************************/
int et_open_config_setdebugdefault(et_openconfig sconfig, int val)
{
  et_open_config *sc = (et_open_config *) sconfig;
  
  if (sc->init != ET_STRUCT_OK) {
    return ET_ERROR;
  }
   
  if ((val != ET_DEBUG_NONE)  && (val != ET_DEBUG_SEVERE) &&
      (val != ET_DEBUG_ERROR) && (val != ET_DEBUG_WARN)   &&
      (val != ET_DEBUG_INFO))  {
     return ET_ERROR;
  }
  sc->debug_default = val;
  return ET_OK;
}

/*****************************************************/
int et_open_config_getdebugdefault(et_openconfig sconfig, int *val)
{
  et_open_config *sc = (et_open_config *) sconfig;
  
  *val = sc->debug_default;
  return ET_OK;
}

/*****************************************************/
int et_open_config_setport(et_openconfig sconfig, int val)
{
  et_open_config *sc = (et_open_config *) sconfig;
  
  if (sc->init != ET_STRUCT_OK) {
    return ET_ERROR;
  }
   
  if (val < 1024 || val > 65535) {
    return ET_ERROR;
  }
  
  sc->udpport = val;
  return ET_OK;
}

/*****************************************************/
int et_open_config_getport(et_openconfig sconfig, int *val)
{
  et_open_config *sc = (et_open_config *) sconfig;
  
  *val = sc->udpport;
  return ET_OK;
}

/*****************************************************/
int et_open_config_setmultiport(et_openconfig sconfig, int val)
{
  et_open_config *sc = (et_open_config *) sconfig;
  
  if (sc->init != ET_STRUCT_OK) {
    return ET_ERROR;
  }
   
  if (val < 1024 || val > 65535) {
    return ET_ERROR;
  }
  
  sc->multiport = val;
  return ET_OK;
}

/*****************************************************/
int et_open_config_getmultiport(et_openconfig sconfig, int *val)
{
  et_open_config *sc = (et_open_config *) sconfig;
  
  *val = sc->multiport;
  return ET_OK;
}

/*****************************************************/
int et_open_config_setserverport(et_openconfig sconfig, int val)
{
  et_open_config *sc = (et_open_config *) sconfig;
  
  if (sc->init != ET_STRUCT_OK) {
    return ET_ERROR;
  }
   
  if (val < 1024 || val > 65535) {
    return ET_ERROR;
  }
  
  sc->serverport = val;
  return ET_OK;
}

/*****************************************************/
int et_open_config_getserverport(et_openconfig sconfig, int *val)
{
  et_open_config *sc = (et_open_config *) sconfig;
  
  *val = sc->serverport;
  return ET_OK;
}

/*****************************************************/
int et_open_config_settimeout(et_openconfig sconfig, struct timespec val)
{
  et_open_config *sc = (et_open_config *) sconfig;
  
  if (sc->init != ET_STRUCT_OK) {
    return ET_ERROR;
  }
    
  sc->timeout = val;
  return ET_OK;
}

/*****************************************************/
int et_open_config_gettimeout(et_openconfig sconfig, struct timespec *val)
{
  et_open_config *sc = (et_open_config *) sconfig;
  
  *val = sc->timeout;
  return ET_OK;
}

/*****************************************************/
int et_open_config_addmulticast(et_openconfig sconfig, const char *val)
{
  et_open_config *sc = (et_open_config *) sconfig;
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
int et_open_config_removemulticast(et_openconfig sconfig, const char *val)
{
  et_open_config *sc = (et_open_config *) sconfig;
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
int et_open_config_setpolicy(et_openconfig sconfig, int val)
{
  et_open_config *sc = (et_open_config *) sconfig;
  
  if (sc->init != ET_STRUCT_OK) {
    return ET_ERROR;
  }
   
  if ((val != ET_POLICY_FIRST) &&
      (val != ET_POLICY_LOCAL) &&
      (val != ET_POLICY_ERROR))  {
    return ET_ERROR;
  }
  
  sc->policy = val;
  return ET_OK;
}

/*****************************************************/
int et_open_config_getpolicy(et_openconfig sconfig, int *val)
{
  et_open_config *sc = (et_open_config *) sconfig;
  
  *val = sc->policy;
  return ET_OK;
}

/*****************************************************/
int et_open_config_sethost(et_openconfig sconfig, const char *val)
{
  et_open_config *sc = (et_open_config *) sconfig;
  
  if (sc->init != ET_STRUCT_OK) {
    return ET_ERROR;
  }
  
  if (val == NULL) {
    return ET_ERROR;
  }
  
  if (strlen(val) > ET_FUNCNAME_LENGTH-1) {
    return ET_ERROR;
  }
  
  strcpy(sc->host, val);
  return ET_OK;
}

/*****************************************************/
int et_open_config_gethost(et_openconfig sconfig, char *val)
{
  et_open_config *sc = (et_open_config *) sconfig;
  
  strcpy(val, sc->host);
  return ET_OK;
}
