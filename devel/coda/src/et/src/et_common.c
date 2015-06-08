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
 *      Routines for initializations of processes,
 *	stations, events, eventlists, statistics & system ids. Also routines
 *	for opening ET systems and dealing with heartbeats.
 *
 *----------------------------------------------------------------------------*/
 

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include "et_private.h"
#include "et_network.h"

/*****************************************************/
/*             ERROR REPORTING                       */
/*****************************************************/

#ifndef WITH_DALOGMSG
void et_logmsg (char *sev, char *fmt, ...)
{
  va_list ap;
  char temp[500];
  
  va_start(ap, fmt);
  vsprintf(temp, fmt, ap);

  printf("et %s: %s", sev, temp);
  va_end(ap);
}
#endif

/*****************************************************/
/*             ET STARTUP & SHUTDOWN                 */
/*****************************************************/

/******************************************************
 * Decide whether we are looking for the ET system locally,
 * locally on Linux or some other non-mutex-sharing operating
 * system, or remotely.
 *****************************************************/
int et_findlocality(const char *filename, et_openconfig openconfig)
{     
  char ethost[ET_MAXHOSTNAMELEN];
  et_open_config *config = (et_open_config *) openconfig;
    
  /* if local client opens ET system as remote (thru server) ...
   * This option is for those applications (such as system
   * monitoring) that only want to communication through
   * an ET system's server and not map its shared mem.
   */
  if (config->mode == ET_HOST_AS_REMOTE) {
    return ET_REMOTE;
  }
  
  /* else if ET system host name is unknown and remote ... */
  else if (strcmp(config->host, ET_HOST_REMOTE) == 0) {
    return ET_REMOTE;
  }
  
  /* else if ET system is on local host */
  else if ((strcmp(config->host, ET_HOST_LOCAL) == 0) ||
           (strcmp(config->host, "localhost")   == 0))  {
    /* if local operating system can share pthread mutexes ... */
    if (sysconf(_SC_THREAD_PROCESS_SHARED) == 1) {
      return ET_LOCAL;
    }
    else {
      return ET_LOCAL_NOSHARE;
    }
  }
  
  /* else if ET system host name is unknown and maybe anywhere ... */
  else if (strcmp(config->host, ET_HOST_ANYWHERE) == 0) {
    int err;
    unsigned short port;
    struct timeval waittime;
    
    waittime.tv_sec  = 0;
    waittime.tv_usec = 10000; /* 0.1 sec */
    
    /* send only 1 broadcast with a 0.01 sec wait */
    err = et_findserver2(filename, ethost, &port, config, 1, &waittime);
    if ((err == ET_ERROR) || (err == ET_ERROR_TIMEOUT)) {
      et_logmsg("ERROR", "et_findlocality, cannot find ET system\n");
      return err;
    }
    else if (err == ET_ERROR_TOOMANY) {
      /* many systems responded */
      et_logmsg("ERROR", "et_findlocality, multiple ET systems reponded\n");
      return err;
    }
    
    return et_nodelocality(ethost);
  }
  
  /* else ET system host name is given ... */
  else {
    return et_nodelocality(config->host);
  }
}
 
/******************************************************/
int et_open(et_sys_id *id, const char *filename, et_openconfig openconfig)
{     
  int             status, auto_open=0, err, locality;
  et_open_config *config;
  et_openconfig   auto_config = NULL;
  int             def_debug;

  if (openconfig == NULL) {
    auto_open = 1;
    if (et_open_config_init(&auto_config) == ET_ERROR) {
      et_logmsg("ERROR", "et_open, null arg for openconfig, cannot use default\n");
      return ET_ERROR;
    }
    openconfig = auto_config;
  }

  config = (et_open_config *) openconfig;

  err = ET_OK;
  /* argument checking */
  if ((filename == NULL) || (config->init != ET_STRUCT_OK)) {
    et_logmsg("ERROR", "et_open, bad argument\n");
    err = ET_ERROR;
  }
  else if (strlen(filename) > ET_FILENAME_LENGTH - 1) {
    et_logmsg("ERROR", "et_open, ET name too long\n");
    err = ET_ERROR;
  }

  if (err != ET_OK) {
    if (auto_open == 1) {
      et_open_config_destroy(auto_config);
    }
    return err;
  }
  
  /* initialize id */
  if (et_id_init(id) != ET_OK) {
    et_logmsg("ERROR", "et_open, cannot initialize id\n");
    return ET_ERROR;
  }

  if (et_open_config_getdebugdefault(openconfig, &def_debug) != ET_OK) {
    def_debug = ET_DEBUG_ERROR;
  }
  et_system_setdebug(*id, def_debug);

  
  /* Decide whether we are looking for the ET system locally,
   * locally on Linux or some other non-mutex-sharing operating
   * system, remotely, or anywhere.
   */
  locality = et_findlocality(filename, openconfig);
  /* if host is local on SUN ... */
  if (locality == ET_LOCAL) {
    status = etl_open(id, filename, openconfig);
  }
  /* else if host is remote ... */
  else if (locality == ET_REMOTE) {
    status = etr_open(id, filename, openconfig);
  }
  /* else if host is local on Linux ... */
  else if (locality == ET_LOCAL_NOSHARE) {
    status = etn_open(id, filename, openconfig);
  }
  /* else if too many systems responded and we have return error policy ... */
  else if ((locality == ET_ERROR_TOOMANY) && (config->policy == ET_POLICY_ERROR)) {
    if (auto_open == 1) {
      et_open_config_destroy(auto_config);
    }
    et_logmsg("ERROR", "et_open: too many ET systems of that name responded\n");
    return ET_ERROR;
  }
  /* else did not find ET system by broad/multicasting. In
   * this case, try to open a local system first then remote.
   */
  else {
    /* if we can share pthread mutexes between processes ... */
    if (sysconf(_SC_THREAD_PROCESS_SHARED) == 1) {
      status = etl_open(id, filename, openconfig);
    }
    else {
      status = etn_open(id, filename, openconfig);
    }
    if (status != ET_OK) {
      status = etr_open(id, filename, openconfig);
    }
  }
  
  if (status != ET_OK) {
    et_id_destroy(*id);
  }
  
  if (auto_open == 1) {
    et_open_config_destroy(auto_config);
  }

  return status;
}
 

/******************************************************/
int et_close(et_sys_id id)
{
  et_id *etid = (et_id *) id;
  
  if (etid->locality == ET_REMOTE) {
    return etr_close(id);
  }
  else if (etid->locality == ET_LOCAL_NOSHARE) {
    return etn_close(id);
  }
  return etl_close(id);
}

/******************************************************/
int et_forcedclose(et_sys_id id)
{
  et_id *etid = (et_id *) id;
  
  if (etid->locality == ET_REMOTE) {
    return etr_forcedclose(id);
  }
  else if (etid->locality == ET_LOCAL_NOSHARE) {
    return etn_forcedclose(id);
  }
  return etl_forcedclose(id);
}

/*****************************************************/
/*                  HEARTBEAT STUFF                  */
/*****************************************************/

int et_alive(et_sys_id id)
{
  et_id *etid = (et_id *) id;
  
  if (etid->locality == ET_REMOTE) {
    return etr_alive(id);
  }
  else if (etid->locality == ET_LOCAL_NOSHARE) {
    return etn_alive(id);
  }
  return etl_alive(id);
}

/******************************************************/
int et_wait_for_alive(et_sys_id id)
{
  et_id *etid = (et_id *) id;
  
  if (etid->locality == ET_REMOTE) {
    return etr_wait_for_alive(id);
  }
  else if (etid->locality == ET_LOCAL_NOSHARE) {
    return etn_wait_for_alive(id);
  }
  return etl_wait_for_alive(id);
} 
