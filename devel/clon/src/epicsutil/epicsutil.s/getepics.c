
/* getepics.c */

// EPICS CA includes
#include <stdio.h>
#include <epicsStdlib.h>
#include <string.h>

#include <alarm.h>
#include <cadef.h>
#include <epicsGetopt.h>

#include "tool_lib_1.h"

#include "epicsutil.h"

#define PEND_EVENT_SLICES 5     /* No. of pend_event slices for callback requests */

/* Different output formats */
typedef enum { plain, terse, all, specifiedDbr } OutputT;

/* Different request types */
typedef enum { get, callback } RequestT;

static int nConn = 0;           /* Number of connected PVs */
static int nRead = 0;           /* Number of channels that were read */
static int floatAsString = 0;   /* Flag: fetch floats as string */




/*********************************************************************************/
/*********************************************************************************/
/****************** from $EPICS_BASE/src/catools/caget.c *************************/

/*+**************************************************************************
 *
 * Function:	event_handler
 *
 * Description:	CA event_handler for request type callback
 * 		Allocates the dbr structure and copies the data
 *
 * Arg(s) In:	args  -  event handler args (see CA manual)
 *
 **************************************************************************-*/

void
event_handler(evargs args)
{
  pv *ppv;

  /* args.usr contains pointer to the 'ppv' from ca_create_subscription() call
  (see connaction_handler) */
  ppv = (pv *)args.usr;

  ppv->status = args.status;
  if (args.status == ECA_NORMAL)
  {
    ppv->dbrType = args.type;
    ppv->value   = calloc(1, dbr_size_n(args.type, args.count));
    memcpy(ppv->value, args.dbr, dbr_size_n(args.type, args.count));
    ppv->onceConnected = 1;
    nRead++;
  }
}

/*+**************************************************************************
 *
 * Function:	caget
 *
 * Description:	Issue read requests, wait for incoming data
 * 		and print the data according to the selected format
 *
 * Arg(s) In:	pvs       -  Pointer to an array of pv structures
 *              nPvs      -  Number of elements in the pvs array
 *              request   -  Request type
 *              format    -  Output format
 *              dbrType   -  Requested dbr type
 *              reqElems  -  Requested number of (array) elements
 *
 * Return(s):	Error code: 0 = OK, 1 = Error
 *
 **************************************************************************-*/

/* sergey: instead of printing values we'll fill channe1_val[] array; all error prints
   are commented out as well: we'll return '-9999' in case of any problem; only
   format='plain' is used here */
int
caget(pv *pvs, int nPvs, RequestT request, OutputT format,
      chtype dbrType, unsigned long reqElems, float *valfloatarray)
{
  unsigned int i;
  int n, result;
  float valfloat;


/* sergey: enforce all ENUM types to be reported as integers rather then strings */
enumAsNr = 1;


  for(n=0; n<nPvs; n++)
  {
    /* Set up pvs structure */
    /* -------------------- */

    /* Get natural type and array count */
    pvs[n].nElems  = ca_element_count(pvs[n].ch_id);
    pvs[n].dbfType = ca_field_type(pvs[n].ch_id);

    /* Set up value structures */
    if (format != specifiedDbr)
    {
      dbrType = dbf_type_to_DBR_TIME(pvs[n].dbfType); /* Use native type */
      if (dbr_type_is_ENUM(dbrType))                  /* Enums honour -n option */
      {
        if (enumAsNr) dbrType = DBR_TIME_INT;
        else          dbrType = DBR_TIME_STRING;
      }
      else if (floatAsString &&
                 (dbr_type_is_FLOAT(dbrType) || dbr_type_is_DOUBLE(dbrType)))
      {
        dbrType = DBR_TIME_STRING;
      }
    }

    /* Adjust array count */
    if (reqElems == 0 || pvs[n].nElems < reqElems) reqElems = pvs[n].nElems;

    /* Remember dbrType and reqElems */
    pvs[n].dbrType  = dbrType;
    pvs[n].reqElems = reqElems;

    /* Issue CA request */
    /* ---------------- */

    if (ca_state(pvs[n].ch_id) == cs_conn)
    {
      nConn++;
      pvs[n].onceConnected = 1;
      if (request == callback) /* Event handler will allocate value */
      {
        result = ca_array_get_callback(dbrType,
                                       reqElems,
                                       pvs[n].ch_id,
                                       event_handler,
                                       (void*)&pvs[n]);
      }
      else /* Allocate value structure */
      {
        pvs[n].value = calloc(1, dbr_size_n(dbrType, reqElems));
        result = ca_array_get(dbrType,
                              reqElems,
                              pvs[n].ch_id,
                              pvs[n].value);
      }
      pvs[n].status = result;
    }
    else
    {
      pvs[n].status = ECA_DISCONN;
    }
  }
  if (!nConn) return(1);              /* No connection? We're done. */

  /* Wait for completion */
  /* ------------------- */

  result = ca_pend_io(caTimeout);
  if (result == ECA_TIMEOUT)
	fprintf(stderr, "Read operation timed out: some PV data was not read.\n");

  if (request == callback)    /* Also wait for callbacks */
  {
    double slice = caTimeout / PEND_EVENT_SLICES;
    for (n = 0; n < PEND_EVENT_SLICES; n++)
    {
      ca_pend_event(slice);
      if (nRead >= nConn) break;
    }
    if (nRead < nConn)
            fprintf(stderr, "Read operation timed out: some PV data was not read.\n");
  }

  /* Print the data */
  /* -------------- */

  for(n=0; n<nPvs; n++)
  {
    reqElems = pvs[n].reqElems;

    switch (format)
    {
        case plain:             /* Emulate old caget behaviour */
            if (reqElems <= 1) /*printf("%-30s ", pvs[n].name)*/;
            else               /*printf("%s", pvs[n].name)*/;
        case terse:
            if (pvs[n].status == ECA_DISCONN)
			  /*printf("*** not connected\n")*/;
            else if (pvs[n].status == ECA_NORDACCESS)
			  /*printf("*** no read access\n")*/;
            else if (pvs[n].status != ECA_NORMAL)
			  /*printf("*** CA error %s\n", ca_message(pvs[n].status))*/;
            else if (pvs[n].value == 0)
			  /*printf("*** no data available (timeout)\n")*/;
            else
            {
              char *ss;
              if (reqElems > 1) /*printf(" %lu ", reqElems)*/;
              for (i=0; i<reqElems; ++i)
			  {
                ss = val2str(pvs[n].value, pvs[n].dbrType, i, &valfloat);
                /*printf("%s ", ss)*/;
			  }
              /*printf("\n")*/;
              if(reqElems == 1) valfloatarray[n] = valfloat; /* no CA arrays here */
            }
            break;
        case all:
            print_time_val_sts(&pvs[n], reqElems, valfloatarray);
            break;
        case specifiedDbr:
            printf("%s\n", pvs[n].name);
            if (pvs[n].status == ECA_DISCONN)
                printf("    *** not connected\n");
            else if (pvs[n].status == ECA_NORDACCESS)
                printf("    *** no read access\n");
            else if (pvs[n].status != ECA_NORMAL)
                printf("    *** CA error %s\n", ca_message(pvs[n].status));
            else
            {
              printf("    Data type:      %s (native: %s)\n",
                       dbr_type_to_text(pvs[n].dbrType),
                       dbf_type_to_text(pvs[n].dbfType));
              if (pvs[n].dbrType == DBR_CLASS_NAME)
			  {
                printf("    Class Name:     %s\n",
                           *((dbr_string_t*)dbr_value_ptr(pvs[n].value,
                                                          pvs[n].dbrType)));
			  }
              else
              {
                printf("    Element count:  %lu\n"
                           "    Value:          ",
                           reqElems);
                for (i=0; i<reqElems; ++i)       /* Print value(s) */
                        printf("%s ", val2str(pvs[n].value, pvs[n].dbrType, i, &valfloat));
                printf("\n");

                /* Extended type extra info */
                if (pvs[n].dbrType > DBR_DOUBLE)
                        printf("%s\n", dbr2str(pvs[n].value, pvs[n].dbrType));
                   
              }
            }
            break;
        default :
            break;
    }
  }
  return(0);
}


/*********************************************************/
/*********************************************************/
/*********************************************************/

/*
input:
 nPvs - the number of PVs
 */

int
getepics(int nPvs, char *canames[MAX_ENTRY], float *valfloats)
{
  int n = 0;
  int result;
  OutputT format = plain;     /* User specified format */
  RequestT request = get;     /* User specified request type */
  int count = 0;              /* 0 = not specified by -# option */
  int type = -1;              /* getopt() data type argument */
  pv *pvs = 0;                /* Array of PV structures */

  if(nPvs < 1)
  {
    fprintf(stderr, "No pv name specified. ('get_epics -h' for help.)\n");
    return(1);
  }

  /* Start up Channel Access */

  result = ca_context_create(ca_disable_preemptive_callback);
  if(result != ECA_NORMAL)
  {
    fprintf(stderr, "CA error %s occurred while trying "
            "to start channel access '%s'.\n", ca_message(result), pvs[n].name);
    return(1);
  }

  /* Allocate PV structure array */
  pvs = (pv *) calloc(nPvs, sizeof(pv));
  if (!pvs)
  {
    fprintf(stderr, "Memory allocation for channel structures failed.\n");
    return(1);
  }

  /* Connect channels */
  for(n=0; n<nPvs; n++)
  {
    pvs[n].name = (char *)&canames[n][0];       /* Copy PV names */
  }
  connect_pvs(pvs, nPvs);

  /* Read and print data */
  result = caget(pvs, nPvs, request, format, type, count, valfloats);

  /* Shut down Channel Access */
  ca_context_destroy();

  return(0);
}

