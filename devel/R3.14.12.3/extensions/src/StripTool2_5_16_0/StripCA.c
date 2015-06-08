/*************************************************************************\
* Copyright (c) 1994-2004 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 1997-2003 Southeastern Universities Research Association,
* as Operator of Thomas Jefferson National Accelerator Facility.
* Copyright (c) 1997-2002 Deutches Elektronen-Synchrotron in der Helmholtz-
* Gemelnschaft (DESY).
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

#define DEBUG_DISCONNECT 0
#define DEBUG_ASSERT 0
#define PRINT_DESC_ERRORS

/* Keep in mind the interface does not respond for this time and that
   most of the search requests are at the beginning of the sequence */
#define RETRY_TIMEOUT 1.0

#include "StripDAQ.h"

#include <cadef.h>
#include <db_access.h>

typedef struct _StripDAQInfo
{
  Strip         strip;
  struct        _ChannelData
  {
    chid                        chan_id;
    evid                        event_id;
#ifndef PEND_DESC
    chid                        desc_chan_id;
#endif    
    double                      value;
    struct _StripDAQInfo        *this;
  } chan_data[STRIP_MAX_CURVES];
} StripDAQInfo;


/* ====== Prototypes ====== */
static void addfd_callback (void *, int, int);
static void timeout_callback (XtPointer, XtIntervalId *);
static void work_callback (XtPointer, int *, XtInputId *);
static void connect_callback (struct connection_handler_args);
static void info_callback (struct event_handler_args);
static void data_callback (struct event_handler_args);
static double get_value (void *);
#ifdef PEND_DESCRIPTION
static void getDescriptionRecord (char *name,char *description);
#else
static void requestDescRecord (StripCurve curve);
static void desc_connect_callback (struct connection_handler_args);
static void desc_info_callback (struct event_handler_args args);
#endif

/*
 * StripDAQ_initialize
 */
StripDAQ StripDAQ_initialize (Strip strip)
{
  StripDAQInfo  *sca = NULL;
  int           status;
  int           i;

  
  if ((sca = (StripDAQInfo *)calloc (sizeof (StripDAQInfo), 1)) != NULL)
  {
    sca->strip = strip;
    status = ca_task_initialize ();
    if (status != ECA_NORMAL)
    {
      SEVCHK (status, "StripDAQ: Channel Access initialization error");
      free (sca);
      sca = NULL;
    }
    else {
      Strip_addtimeout (strip, 0.1, timeout_callback, strip);
      ca_add_fd_registration (addfd_callback, sca);
      for (i = 0; i < STRIP_MAX_CURVES; i++)
      {
        sca->chan_data[i].this = sca;
        sca->chan_data[i].chan_id = NULL;
      }
    }
  }

  return (StripDAQ)sca;
}


/*
 * StripDAQ_terminate
 */
void StripDAQ_terminate (StripDAQ the_sca)
{
  ca_task_exit ();
}



/*
 * StripDAQ_request_connect
 *
 *      Requests connection for the specified curve.
 */
int StripDAQ_request_connect (StripCurve curve, void *the_sca)
{
  StripDAQInfo  *sca = (StripDAQInfo *)the_sca;
  int           i;
  int           ret_val;
#ifdef PEND_DESCRIPTION
  char *description=NULL; /* Albert */
#endif
  
  for (i = 0; i < STRIP_MAX_CURVES; i++)
    if (sca->chan_data[i].chan_id == NULL)
      break;
  
  if ((ret_val = (i < STRIP_MAX_CURVES)))
  {
    StripCurve_setattr (curve, STRIPCURVE_FUNCDATA, &sca->chan_data[i], 0);
#ifndef PEND_DESCRIPTION
    /* first search for the description field so it is likely to
       connect first */
    requestDescRecord(curve);
#endif
    /* search for the process variable */
    ret_val = ca_search_and_connect
      ((char *)StripCurve_getattr_val (curve, STRIPCURVE_NAME),
	  &sca->chan_data[i].chan_id,
	  connect_callback,
	  curve);
    if (ret_val != ECA_NORMAL)
    {
      SEVCHK (ret_val, "StripDAQ: Channel Access unable to connect\n");
      fprintf
        (stderr, "channel name: %s\n",
	    (char *)StripCurve_getattr_val (curve, STRIPCURVE_NAME));
      ret_val = 0;
    }
    else ret_val = 1;
  }

#ifdef PEND_DESCRIPTION
  /* KE: Should be inside the if above */
  /* get the description field using ca_pend_io */
  description=malloc(STRIP_MAX_NAME_CHAR); /* Albert */
  getDescriptionRecord(
    (char *)StripCurve_getattr_val (curve, STRIPCURVE_NAME),
    description);
  StripCurve_setattr (curve, STRIPCURVE_COMMENT, description, 0);
  free(description);
#endif

  ca_flush_io();
  
  return ret_val;
}


/*
 * StripDAQ_request_disconnect
 *
 *      Requests disconnection for the specified curve.
 *      Returns 0 if anything fails, otherwise 1
 */
int StripDAQ_request_disconnect (StripCurve curve, void *the_sca)
{
  struct _ChannelData   *cd;
  int                   ret_val = 1;

  cd = (struct _ChannelData *) StripCurve_getattr_val
    (curve, STRIPCURVE_FUNCDATA);

  /* this will happen if a non-CA curve is submitted for disconnect */
  if (!cd) return 1;

#if DEBUG_DISCONNECT
  fprintf(stderr,"StripDAQ_request_disconnect: %s\n",
    ca_name(cd->chan_id));
#endif

  if (cd->event_id != NULL)
  {
    if ((ret_val = ca_clear_event (cd->event_id)) != ECA_NORMAL)
    {
      SEVCHK
        (ret_val, "StripDAQ_request_disconnect: error in ca_clear_event");
      ret_val = 0;
    }
    else
    {
      cd->event_id = NULL;
    }
  }
  ca_flush_io();
  
  if (cd->chan_id != NULL)
  {
    /* **** ca_clear_channel() causes info to be printed to stdout **** */
#if DEBUG_ASSERT
    printf("StripDAQ_request_disconnect: ca_clear_channel: %s\n",
	ca_name(cd->chan_id)?ca_name(cd->chan_id):"NULL");
#endif
    if ((ret_val = ca_clear_channel (cd->chan_id)) != ECA_NORMAL)
    {
      SEVCHK
        (ret_val, "StripDAQ_request_disconnect: error in ca_clear_channel");
      ret_val = 0;
#if DEBUG_ASSERT
    printf("  Done\n");
#endif
    }
    else
    {
      cd->chan_id = NULL;
    }
  }

#ifndef PEND_DESC
  if (cd->desc_chan_id != NULL)
  {
    /* **** ca_clear_channel() causes info to be printed to stdout **** */
#if DEBUG_ASSERT
    printf("StripDAQ_request_disconnect: ca_clear_channel: %s\n",
	ca_name(cd->desc_chan_id)?ca_name(cd->desc_chan_id):"NULL");
#endif
    if ((ret_val = ca_clear_channel (cd->desc_chan_id)) != ECA_NORMAL)
    {
      SEVCHK
        (ret_val, "StripDAQ_request_disconnect: error in ca_clear_channel");
      ret_val = 0;
#if DEBUG_ASSERT
    printf("  Done\n");
#endif
    }
    else
    {
      cd->desc_chan_id = NULL;
    }
  }
#endif
  
  ca_flush_io();
#if DEBUG_DISCONNECT
  fprintf(stderr,"StripDAQ_request_disconnect: end\n");
#endif
  return ret_val;
}

/*
 * StripDAQ_retry_connections
 *
 *      Tries to reconnect to currently unconnected PVs.
 */
int StripDAQ_retry_connections (StripDAQ the_sca, Display *display)
{
  StripDAQInfo *sca = (StripDAQInfo *)the_sca;
  int ret_val = 0;
  int found = 0;
  int i;
  const char *pvname = NULL;
  chid retryChid;
  int status;
  
  /* Check if all channels are connected */
  for (i = 0; i < STRIP_MAX_CURVES; i++)
  {
    if (sca->chan_data[i].chan_id &&
	ca_state(sca->chan_data[i].chan_id) != cs_conn)
    {
	pvname=ca_name(sca->chan_data[i].chan_id);
	if(!pvname) continue;
	found=1;
	break;
    }
  }
  if(!found)
  {
    XBell (display,50);
    return -1;
  }
  
  /* Search */
    status=ca_search_and_connect(pvname,&retryChid,NULL,NULL);
    if(status != ECA_NORMAL)
    {
	fprintf(stderr,"StripDAQ_retry_connections: ca_search failed "
	  "for %s: %s\n",
	  pvname, ca_message(status));
	ret_val=-1;
    }
    
  /* Wait.  The searches will only continue for this time.  Keep the
   * time short as the interface is frozen, and most of the searches
   * occur at the start of the sequence.  Testing indicated:
   *
   * RETRY_TIMEOUT Searches
   *      30          15
   *       5          10
   *       3           9
   *       2           9
   *       1           8
   *
   * but this may vary owing to tuning and may change with new releases.
   */
    ca_pend_io(RETRY_TIMEOUT);
    
  /* Clear the channel */
    status = ca_clear_channel(retryChid);
    if(status != ECA_NORMAL)
    {
	fprintf(stderr,"StripDAQ_retry_connections: ca_clear_channel failed "
	  "for %s: %s\n",
	  pvname, ca_message(status));
	ret_val=-1;
    }

    return ret_val;
}

/*
 * addfd_callback
 *
 *      Add new file descriptors to select upon.
 *      Remove old file descriptors from selection.
 */
static void addfd_callback (void *data, int fd, int active)
{
  StripDAQInfo  *strip_ca = (StripDAQInfo *)data;
  if (active)
    Strip_addfd (strip_ca->strip, fd, work_callback, (XtPointer)strip_ca);
  else
    Strip_clearfd (strip_ca->strip, fd);
}


/*
 * work_callback
 *
 *      Gives control to Channel Access for a while.
 */
static void work_callback (XtPointer      BOGUS(1),
                           int            *BOGUS(2),
                           XtInputId      *BOGUS(3))
{
#if 0
  /* KE: ca_pend_event will block the program unnecessarily for
     STRIP_CA_PEND_TIMEOUT, whether there is anything to do or
     not. ca_poll will do all pending tasks, then exit.  Setting
     STRIP_CA_PEND_TIMEOUT to 1e-12 is equivalent to using ca_poll. */
  ca_pend_event (STRIP_CA_PEND_TIMEOUT);
#else
  ca_poll();
#endif  
}

/*
 * timeout_callback
 */
static void timeout_callback (XtPointer ptr, XtIntervalId *pId)
{
  Strip strip = (Strip) ptr;
#if 0
  /* KE: ca_pend_event will block the program unnecessarily for
     STRIP_CA_PEND_TIMEOUT, whether there is anything to do or
     not. ca_poll will do all pending tasks, then exit.  Setting
     STRIP_CA_PEND_TIMEOUT to 1e-12 is equivalent to using ca_poll. */
  ca_pend_event (STRIP_CA_PEND_TIMEOUT);
#else
  ca_poll();
#endif  
  Strip_addtimeout ( strip, 0.1, timeout_callback, strip );
}

/*
 * connect_callback
 */
static void connect_callback (struct connection_handler_args args)
{
  StripCurve            curve;
  struct _ChannelData   *cd;
  int                   status;

  curve = (StripCurve)(ca_puser (args.chid));
  cd = (struct _ChannelData *)StripCurve_getattr_val
    (curve, STRIPCURVE_FUNCDATA);

  switch (ca_state (args.chid))
  {
  case cs_never_conn:
    fprintf (stderr,
	"%s StripDAQ connect_callback: IOC not found for %s\n",
	timeStamp(),ca_name(args.chid)?ca_name(args.chid):"Name Unknown");
    cd->chan_id = NULL;
    cd->event_id = NULL;
    Strip_freecurve (cd->this->strip, curve);
    break;
    
  case cs_prev_conn:
    fprintf (stderr,
	"%s StripDAQ connect_callback: IOC unavailable for %s\n",
	timeStamp(),ca_name(args.chid)?ca_name(args.chid):"Name Unknown");
#if DEBUG_DISCONNECT
    fprintf
	(stderr,
	  "  cd->chan_id=%x cd->event_id=%x\n",
	  cd->chan_id, cd->event_id);
#endif
    Strip_setwaiting (cd->this->strip, curve);
    break;
    
  case cs_conn:
    /* now connected, so get the control info if this is first time */
    if (cd->event_id == 0)
    {
	status = ca_get_callback
	  (DBR_CTRL_DOUBLE, cd->chan_id, info_callback, curve);
	if (status != ECA_NORMAL)
	{
	  SEVCHK
	    (status, "StripDAQ connect_callback: error in ca_get_callback");
	  Strip_freecurve (cd->this->strip, curve);
	}
    } else {
	fprintf (stderr,
	  "%s StripDAQ connect_callback: IOC reconnected for %s\n",
	  timeStamp(),ca_name(args.chid)?ca_name(args.chid):"Name Unknown");
    }
    break;
    
  case cs_closed:
    fprintf (stderr,
	"%s StripDAQ connect_callback: IOC connection closed for %s\n",
	timeStamp(),ca_name(args.chid)?ca_name(args.chid):"Name Unknown");
    break;
  }
  
  fflush (stderr);
  
  ca_flush_io();
}

/*
 * info_callback
 */
static void info_callback (struct event_handler_args args)
{
  StripCurve                    curve;
  struct _ChannelData           *cd;
  struct dbr_ctrl_double        *ctrl;
  int                           status;
  double                        low, hi;

  curve = (StripCurve)(ca_puser (args.chid));
  cd = (struct _ChannelData *)StripCurve_getattr_val
    (curve, STRIPCURVE_FUNCDATA);

  if (args.status != ECA_NORMAL)
  {
    fprintf
      (stderr,
	  "StripDAQ info_callback:\n"
	  "  [%s] get: %s\n",
	  ca_name(cd->chan_id),
#if 0
	  ca_message_text[CA_EXTRACT_MSG_NO(args.status)]);
#else    
        ca_message(CA_EXTRACT_MSG_NO(args.status)));
#endif    
  }
  else
  {
    ctrl = (struct dbr_ctrl_double *)args.dbr;

    low = ctrl->lower_disp_limit;
    hi = ctrl->upper_disp_limit;

    if (hi <= low)
    {
      low = ctrl->lower_ctrl_limit;
      hi = ctrl->upper_ctrl_limit;
      if (hi <= low)
      {
        if (ctrl->value == 0)
        {
          hi = 100;
          low = -hi;
        }
        else
        {
          low = ctrl->value - (ctrl->value / 10.0);
          hi = ctrl->value + (ctrl->value / 10.0);
        }
      }
    }

    if (!StripCurve_getstat (curve, STRIPCURVE_EGU_SET))
      StripCurve_setattr (curve, STRIPCURVE_EGU, ctrl->units, 0);
    if (!StripCurve_getstat (curve, STRIPCURVE_PRECISION_SET))
      StripCurve_setattr (curve, STRIPCURVE_PRECISION, ctrl->precision, 0);
    if (!StripCurve_getstat (curve, STRIPCURVE_MIN_SET))
      StripCurve_setattr (curve, STRIPCURVE_MIN, low, 0);
    if (!StripCurve_getstat (curve, STRIPCURVE_MAX_SET))
      StripCurve_setattr (curve, STRIPCURVE_MAX, hi, 0);

    status = ca_add_event
      (DBR_STS_DOUBLE, cd->chan_id, data_callback, curve, &cd->event_id);
    if (status != ECA_NORMAL)
    {
      SEVCHK
        (status, "StripDAQ info_callback: error in ca_get_callback");
      Strip_freecurve (cd->this->strip, curve);
    }
  }
  
  ca_flush_io();
}


/*
 * data_callback
 */
static void data_callback (struct event_handler_args args)
{
  StripCurve                    curve;
  struct _ChannelData           *cd;
  struct dbr_sts_double         *sts;

  curve = (StripCurve)ca_puser (args.chid);
  cd = (struct _ChannelData *)StripCurve_getattr_val
    (curve, STRIPCURVE_FUNCDATA);

  if (args.status != ECA_NORMAL)
  {
    fprintf
      (stderr,
       "StripDAQ data_callback:\n"
       "  [%s] get: %s\n",
       ca_name(cd->chan_id),
       ca_message_text[CA_EXTRACT_MSG_NO(args.status)]);
    Strip_setwaiting (cd->this->strip, curve);
  }
  else
  {
    if (StripCurve_getstat (curve, STRIPCURVE_WAITING))
    {
      StripCurve_setattr
        (curve, STRIPCURVE_SAMPLEFUNC, get_value, 0);
      Strip_setconnected (cd->this->strip, curve);
    }
    sts = (struct dbr_sts_double *)args.dbr;
    cd->value = sts->value;
  }
}


/*
 * get_value
 *
 *      Returns the current value specified by the CurveData passed in.
 */
static double get_value (void *data)
{
  struct _ChannelData   *cd = (struct _ChannelData *)data;

  return cd->value;
}


#ifdef PEND_DESCRIPTION
/*
 * getDescriptionRecord
 *
 *      Searches and waits for the description
 */
static void getDescriptionRecord (char *name, char *description)
{
  int status;
  chid id;
  static char desc_name[64];
  char *ptr;
  
  /* construct the name */
  memset(desc_name,0,64);
  strcpy(desc_name,name);
  ptr=strchr(desc_name,'.');
  if(ptr) *ptr='\0';
  strcat(desc_name,".DESC");

  /* Check validity and initialize to blank */
  if(!description) {
#ifdef PRINT_DESC_ERRORS      
    fprintf(stderr,"Description array is NULL\n");
    return;
#endif    
  }
  *description ='\0';
  
  status = ca_search(desc_name, &id);
  if (status != ECA_NORMAL) {
#ifdef PRINT_DESC_ERRORS      
    SEVCHK(status,"     Search for description field failed\n");
    fprintf(stderr,"%s: Search for description field failed\n",desc_name);
#endif    
    *description=0;
    return;      
  }
  
  status = ca_pend_io(1.0);	
  if (status != ECA_NORMAL) 
    {
#ifdef PRINT_DESC_ERRORS      
      SEVCHK(status,"     Search for description field timed out\n");
      fprintf(stderr,"%s: Search for description field timed out\n",desc_name);
      *description=0;
#endif    
      return;          
    }	
  
  status = ca_array_get (DBR_STRING,1,id,description);
  if (status != ECA_NORMAL) 
  {
#ifdef PRINT_DESC_ERRORS      
    SEVCHK(status,"     Get description field failed\n");
    fprintf(stderr,"%s: Get description field failed\n",desc_name);
    *description=0;
#endif    
    return;           
  }
  
  status= ca_pend_io(1.0);
  if (status != ECA_NORMAL)  
    {
#ifdef PRINT_DESC_ERRORS      
      SEVCHK(status,"     Get for description field timed out\n");
      fprintf(stderr,"%s: Get for description field timed out\n",name);
#endif    
      *description=0;
      return;     
    }

}
#endif  /* #ifdef PEND_DESCRIPTION */

#ifndef PEND_DESCRIPTION
/*
 * getDescriptionRecord
 *
 *      Searches for the description with callback
 */
static void requestDescRecord (StripCurve curve)
{
  int status;
  static char desc_name[64];
  char *name = (char *)StripCurve_getattr_val (curve, STRIPCURVE_NAME);
  struct _ChannelData *cd = (struct _ChannelData *)StripCurve_getattr_val
    (curve, STRIPCURVE_FUNCDATA);
  char *ptr;

  /* construct the name */
  memset(desc_name,0,64);
  strcpy(desc_name,name);
  ptr=strchr(desc_name,'.');
  if(ptr) *ptr='\0';
  strcat(desc_name,".DESC");

  /* search */
  cd->desc_chan_id = NULL;
#if DEBUG_ASSERT
    printf("requestDescRecord: ca_search_and_connect: %s\n",
	desc_name?desc_name:"NULL");
#endif
  status = ca_search_and_connect (desc_name, &cd->desc_chan_id,
    desc_connect_callback, curve);
  if (status != ECA_NORMAL) {
#ifdef PRINT_DESC_ERRORS      
    SEVCHK(status,"     Search for description field failed\n");
    fprintf(stderr,"%s: Search for description field failed\n", desc_name);
#endif    
  }
}

/*
 * desc_connect_callback
 */
static void desc_connect_callback (struct connection_handler_args args)
{
  StripCurve            curve;
  struct _ChannelData   *cd;
  int                   status;
  
  curve = (StripCurve)(ca_puser (args.chid));
  cd = (struct _ChannelData *)StripCurve_getattr_val
    (curve, STRIPCURVE_FUNCDATA);
  
  switch (ca_state (args.chid))
  {
  case cs_never_conn:
#ifdef PRINT_DESC_ERRORS      
    fprintf (stderr, "StripDAQ desc_connect_callback: ioc not found\n");
#endif
    break;
    
  case cs_prev_conn:
#ifdef PRINT_DESC_ERRORS      
    fprintf
	(stderr,
	  "StripDAQ desc_connect_callback: IOC unavailable for %s\n",
	  ca_name(cd->desc_chan_id)?ca_name(cd->desc_chan_id):"NULL");
#endif
    break;
    
  case cs_conn:
    /* now connected, so get the desc string  */
#if DEBUG_ASSERT
    printf("desc_connect_callback: ca_get_callback: %s\n",
	ca_name(cd->desc_chan_id)?ca_name(cd->desc_chan_id):"NULL");
#endif
    status = ca_get_callback (DBR_STRING, cd->desc_chan_id,
	desc_info_callback, curve);
    if (status != ECA_NORMAL)
    {
	SEVCHK (status,
	  "StripDAQ desc_connect_callback: error in ca_get_callback");
    }
    break;
    
  case cs_closed:
#ifdef PRINT_DESC_ERRORS      
    fprintf (stderr, "StripDAQ desc_connect_callback: invalid chid\n");
#endif
    break;
  }
  
  fflush (stderr);
  
  ca_flush_io();
}

/*
 * info_callback
 */
static void desc_info_callback (struct event_handler_args args)
{
  StripCurve                    curve;
  struct _ChannelData           *cd;
  char                          *desc;
  int                           status;

  curve = (StripCurve)(ca_puser (args.chid));
  cd = (struct _ChannelData *)StripCurve_getattr_val
    (curve, STRIPCURVE_FUNCDATA);

  if (args.status != ECA_NORMAL)
  {
    fprintf
      (stderr,
	  "StripDAQ desc_info_callback:\n"
	  "  [%s] get DESC: %s\n",
	  ca_name(cd->chan_id),
#if 0
	  ca_message_text[CA_EXTRACT_MSG_NO(args.status)]);
#else    
        ca_message(CA_EXTRACT_MSG_NO(args.status)));
#endif    
  }
  else
  {
    /* get the description */
    desc = (char *)args.dbr;
    StripCurve_setattr (curve, STRIPCURVE_COMMENT, desc, 0);

    /* set the description to be connected */
    Strip_setdescconnected (cd->this->strip, curve);

    /* clear the description channel, we are through */
#if DEBUG_ASSERT
    printf("desc_info_callback: ca_clear_channel: %s\n",
	ca_name(cd->desc_chan_id)?ca_name(cd->desc_chan_id):"NULL");
#endif
    if ((status = ca_clear_channel (cd->desc_chan_id)) != ECA_NORMAL)
    {
      SEVCHK (status, "desc_info_callback: error in ca_clear_channel");
    }
    else
    {
      cd->desc_chan_id = NULL;
    }
#if DEBUG_ASSERT
    printf("  Done\n");
#endif

  }
}
#endif  /* #ifndef PEND_DESCRIPTION */

/* **************************** Emacs Editing Sequences ***************** */
/* Local Variables: */
/* tab-width: 6 */
/* c-basic-offset: 2 */
/* c-comment-only-line-offset: 0 */
/* c-indent-comments-syntactically-p: t */
/* c-label-minimum-indentation: 1 */
/* c-file-offsets: ((substatement-open . 0) (label . 2) */
/* (brace-entry-open . 0) (label .2) (arglist-intro . +) */
/* (arglist-cont-nonempty . c-lineup-arglist) ) */
/* End: */
