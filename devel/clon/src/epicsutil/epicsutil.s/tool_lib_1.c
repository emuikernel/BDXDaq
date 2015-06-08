
/* tool_lib_1.c - slightly modified copy of $EPICS_BASE/src/catools/tool_lib.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define epicsAlarmGLOBAL
#include <alarm.h>
#undef epicsAlarmGLOBAL
#include <epicsTime.h>
#include <cadef.h>

#include "tool_lib.h"

/* Time stamps for program start, previous value (used for relative resp.
 * incremental times with monitors) */
static epicsTimeStamp tsStart, tsPrevious;

static int tsInit = 0;               /* Flag: Timestamps init'd */

TimeT tsType = absolute;             /* Timestamp type flag (-riI options) */
IntFormatT outType = dec_;            /* For -0.. output format option */

char dblFormatStr[30] = "%g"; /* Format string to print doubles (-efg options) */
char timeFormatStr[30] = "%Y-%m-%d %H:%M:%S.%06f"; /* Time format string */

int enumAsNr = 0;        /* used for -n option - get DBF_ENUM as number */
double caTimeout = 10.0/*1.0*/;  /* wait time default */

#define TIMETEXTLEN 28          /* Length of timestamp text buffer */


void
sprint_long (char *ret, long val)
{
    long i, bit, skip=-1L;   /* used only for printing bits */
    switch (outType) {
    case hex_: sprintf(ret, "0x%lX", val); break;
    case oct_: sprintf(ret, "0o%lo", val); break;
    case bin_:
        for (i=31; i>=0 ; i--)
        {
            bit = (val>>i) & 0x1L;
            if (skip<0 && bit)
            {
                skip = 31 - i;          /* skip leading 0's */
                ret[i+1] = '\0';
            }
            if (skip >= 0)
            {
                ret[31-i-skip] = (bit) ? '1' : '0';
            }
        }
        break;
    default:  sprintf(ret, "%ld", val); /* decimal */
    }
}


/*+**************************************************************************
 *
 * Function:	val2str
 *
 * Description:	Print (convert) value to a string
 *
 * Arg(s) In:	v      -  Pointer to dbr_... structure
 *              type   -  Numeric dbr type
 *              index  -  Index of element to print (for arrays) 
 *
 * Return(s):	Pointer to static output string
 *
 **************************************************************************-*/

char *
val2str (const void *v, unsigned type, int index, float *valfloat)
{
    static char str[500];
    char ch;
    void *val_ptr;
    unsigned base_type;

    if (!dbr_type_is_valid(type))
    {
      strcpy (str, "*** invalid type");
      return str;
    }

    base_type = type % (LAST_TYPE+1);

    if (type == DBR_STSACK_STRING || type == DBR_CLASS_NAME)
        base_type = DBR_STRING;

    val_ptr = dbr_value_ptr(v, type);

	/*printf("index=%d base_type=%d\n",index,base_type);*/
    switch (base_type) {
    case DBR_STRING:
        sprintf(str, "%s",  ((dbr_string_t*) val_ptr)[index]);
        *valfloat = 0.0; /* must use 'str' */
        break;
    case DBR_FLOAT:
        sprintf(str, dblFormatStr, ((dbr_float_t*) val_ptr)[index]);
        *valfloat = ((dbr_float_t*) val_ptr)[index];
        break;
    case DBR_DOUBLE:	  
        sprintf(str, dblFormatStr, ((dbr_double_t*) val_ptr)[index]);
        *valfloat = ((dbr_double_t*) val_ptr)[index];
		/*
printf("DOUBLE [%2d] %f >%s< >%s<\n",index,*valfloat,dblFormatStr,str);
		*/
        break;
    case DBR_CHAR:
        ch = ((dbr_char_t*) val_ptr)[index];
        *valfloat = 0.0; /* must use 'str' */
        if(ch > 31 )
            sprintf(str, "%d \'%c\'",ch,ch);
        else
            sprintf(str, "%d",ch);
        break;
    case DBR_INT:
        sprint_long(str, ((dbr_int_t*) val_ptr)[index]);
        *valfloat = ((dbr_int_t*) val_ptr)[index];
        break;
    case DBR_LONG:
        sprint_long(str, ((dbr_long_t*) val_ptr)[index]);
        *valfloat = ((dbr_long_t*) val_ptr)[index];
        break;
    case DBR_ENUM:
    {
        dbr_enum_t *val = (dbr_enum_t *)val_ptr;
        *valfloat = 0.0; /* must use 'str' */
        if (dbr_type_is_GR(type))
            sprintf(str,"%s (%d)", 
                    ((struct dbr_gr_enum *)v)->strs[val[index]],val[index]);
        else if (dbr_type_is_CTRL(type))
            sprintf(str,"%s (%d)", 
                    ((struct dbr_ctrl_enum *)v)->strs[val[index]],val[index]);
        else
            sprintf(str, "%d", val[index]);
    }
    }
    return str;
}



/*+**************************************************************************
 *
 * Function:	dbr2str
 *
 * Description:	Print (convert) additional information contained in dbr_...
 *
 * Arg(s) In:	value  -  Pointer to dbr_... structure
 *              type   -  Numeric dbr type
 *
 * Return(s):	Pointer to static output string
 *
 **************************************************************************-*/

/* Definitions for sprintf format strings and matching argument lists */

#define FMT_TIME                                \
    "    Timestamp:      %s"

#define ARGS_TIME(T)                            \
    timeText

#define FMT_STS                                 \
    "    Status:         %s\n"                  \
    "    Severity:       %s"

#define ARGS_STS(T)                             \
    stat_to_str(((struct T *)value)->status),   \
    sevr_to_str(((struct T *)value)->severity)

#define ARGS_STS_UNSIGNED(T)                            \
    stat_to_str_unsigned(((struct T *)value)->status),  \
    sevr_to_str_unsigned(((struct T *)value)->severity)

#define FMT_ACK                                 \
    "    Ack transient?: %s\n"                  \
    "    Ack severity:   %s"

#define ARGS_ACK(T)                                     \
    ((struct T *)value)->ackt ? "YES" : "NO",           \
    sevr_to_str_unsigned(((struct T *)value)->acks)

#define FMT_UNITS                               \
    "    Units:          %s"

#define ARGS_UNITS(T)                           \
    ((struct T *)value)->units

#define FMT_PREC                                \
    "    Precision:      %d"

#define ARGS_PREC(T)                            \
    ((struct T *)value)->precision

#define FMT_GR(FMT)                             \
    "    Lo disp limit:  " #FMT "\n"            \
    "    Hi disp limit:  " #FMT "\n"            \
    "    Lo alarm limit: " #FMT "\n"            \
    "    Lo warn limit:  " #FMT "\n"            \
    "    Hi warn limit:  " #FMT "\n"            \
    "    Hi alarm limit: " #FMT

#define ARGS_GR(T,F)                                    \
    (F)((struct T *)value)->lower_disp_limit,           \
    (F)((struct T *)value)->upper_disp_limit,           \
    (F)((struct T *)value)->lower_alarm_limit,          \
    (F)((struct T *)value)->lower_warning_limit,        \
    (F)((struct T *)value)->upper_warning_limit,        \
    (F)((struct T *)value)->upper_alarm_limit

#define FMT_CTRL(FMT)                           \
    "    Lo ctrl limit:  " #FMT "\n"            \
    "    Hi ctrl limit:  " #FMT

#define ARGS_CTRL(T,F)                          \
    (F)((struct T *)value)->lower_ctrl_limit,   \
    (F)((struct T *)value)->upper_ctrl_limit


/* Definitions for the actual sprintf calls */

#define PRN_DBR_STS(T)                          \
    sprintf(str,                                \
            FMT_STS,                            \
            ARGS_STS(T))

#define PRN_DBR_TIME(T)                                         \
    epicsTimeToStrftime(timeText, TIMETEXTLEN, timeFormatStr,   \
                        &(((struct T *)value)->stamp));         \
    sprintf(str,                                                \
            FMT_TIME "\n" FMT_STS,                              \
            ARGS_TIME(T), ARGS_STS(T))

#define PRN_DBR_GR(T,F,FMT)                             \
    sprintf(str,                                        \
            FMT_STS "\n" FMT_UNITS "\n" FMT_GR(FMT),    \
            ARGS_STS(T), ARGS_UNITS(T), ARGS_GR(T,F))

#define PRN_DBR_GR_PREC(T,F,FMT)                                        \
    sprintf(str,                                                        \
            FMT_STS "\n" FMT_UNITS "\n" FMT_PREC "\n" FMT_GR(FMT),      \
            ARGS_STS(T), ARGS_UNITS(T), ARGS_PREC(T), ARGS_GR(T,F))

#define PRN_DBR_CTRL(T,F,FMT)                                                   \
    sprintf(str,                                                                \
            FMT_STS "\n" FMT_UNITS "\n" FMT_GR(FMT) "\n" FMT_CTRL(FMT),         \
            ARGS_STS(T), ARGS_UNITS(T), ARGS_GR(T,F),    ARGS_CTRL(T,F))

#define PRN_DBR_CTRL_PREC(T,F,FMT)                                                      \
    sprintf(str,                                                                        \
            FMT_STS "\n" FMT_UNITS "\n" FMT_PREC "\n" FMT_GR(FMT) "\n" FMT_CTRL(FMT),   \
            ARGS_STS(T), ARGS_UNITS(T), ARGS_PREC(T), ARGS_GR(T,F),    ARGS_CTRL(T,F))

#define PRN_DBR_STSACK(T)                       \
    sprintf(str,                                \
            FMT_STS "\n" FMT_ACK,               \
            ARGS_STS_UNSIGNED(T), ARGS_ACK(T))

#define PRN_DBR_X_ENUM(T)                               \
    n = ((struct T *)value)->no_str;                    \
    PRN_DBR_STS(T);                                     \
    sprintf(str+strlen(str),                            \
                "\n    Enums:          (%2d)", n);      \
    for (i=0; i<n; i++)                                 \
        sprintf(str+strlen(str),                        \
                "\n                    [%2d] %s", i,    \
                ((struct T *)value)->strs[i]);


/* Make a good guess how long the dbr_... stuff might get as worst case */
#define DBR_PRINT_BUFFER_SIZE                                           \
      50                        /* timestamp */                         \
    + 2 * 30                    /* status / Severity */                 \
    + 2 * 30                    /* acks / ackt */                       \
    + 20 + MAX_UNITS_SIZE       /* units */                             \
    + 30                        /* precision */                         \
    + 6 * 45                    /* graphic limits */                    \
    + 2 * 45                    /* control limits */                    \
    + 30 + (MAX_ENUM_STATES * (20 + MAX_ENUM_STRING_SIZE)) /* enums */  \
    + 50                        /* just to be sure */

char *
dbr2str(const void *value, unsigned type)
{
    static char str[DBR_PRINT_BUFFER_SIZE];
    char timeText[TIMETEXTLEN];
    int n, i;

    switch (type) {
    case DBR_STRING:   /* no additional information for basic data types */
    case DBR_INT:
    case DBR_FLOAT:
    case DBR_ENUM:
    case DBR_CHAR:
    case DBR_LONG:
    case DBR_DOUBLE: break;

    case DBR_CTRL_STRING:       /* see db_access.h: not implemented */
    case DBR_GR_STRING:         /* see db_access.h: not implemented */
    case DBR_STS_STRING:  PRN_DBR_STS(dbr_sts_string); break;
    case DBR_STS_SHORT:   PRN_DBR_STS(dbr_sts_short); break;
    case DBR_STS_FLOAT:   PRN_DBR_STS(dbr_sts_float); break;
    case DBR_STS_ENUM:    PRN_DBR_STS(dbr_sts_enum); break;
    case DBR_STS_CHAR:    PRN_DBR_STS(dbr_sts_char); break;
    case DBR_STS_LONG:    PRN_DBR_STS(dbr_sts_long); break;
    case DBR_STS_DOUBLE:  PRN_DBR_STS(dbr_sts_double); break;

    case DBR_TIME_STRING: PRN_DBR_TIME(dbr_time_string); break;
    case DBR_TIME_SHORT:  PRN_DBR_TIME(dbr_time_short); break;
    case DBR_TIME_FLOAT:  PRN_DBR_TIME(dbr_time_float); break;
    case DBR_TIME_ENUM:   PRN_DBR_TIME(dbr_time_enum); break;
    case DBR_TIME_CHAR:   PRN_DBR_TIME(dbr_time_char); break;
    case DBR_TIME_LONG:   PRN_DBR_TIME(dbr_time_long); break;
    case DBR_TIME_DOUBLE: PRN_DBR_TIME(dbr_time_double); break;

    case DBR_GR_CHAR:
        PRN_DBR_GR(dbr_gr_char, char, %8d); break;
    case DBR_GR_INT:
        PRN_DBR_GR(dbr_gr_int,  int,  %8d); break;
    case DBR_GR_LONG:
        PRN_DBR_GR(dbr_gr_long, long int, %8ld); break;
    case DBR_GR_FLOAT:
        PRN_DBR_GR_PREC(dbr_gr_float,  float, %g); break;
    case DBR_GR_DOUBLE:
        PRN_DBR_GR_PREC(dbr_gr_double, double, %g); break;
    case DBR_GR_ENUM:
        PRN_DBR_X_ENUM(dbr_gr_enum); break;
    case DBR_CTRL_CHAR:
        PRN_DBR_CTRL(dbr_ctrl_char,   char,     %8d); break;
    case DBR_CTRL_INT:
        PRN_DBR_CTRL(dbr_ctrl_int,    int,      %8d); break;
    case DBR_CTRL_LONG:
        PRN_DBR_CTRL(dbr_ctrl_long,   long int, %8ld); break;
    case DBR_CTRL_FLOAT:
        PRN_DBR_CTRL_PREC(dbr_ctrl_float,  float,  %g); break;
    case DBR_CTRL_DOUBLE:
        PRN_DBR_CTRL_PREC(dbr_ctrl_double, double, %g); break;
    case DBR_CTRL_ENUM:
        PRN_DBR_X_ENUM(dbr_ctrl_enum); break;
    case DBR_STSACK_STRING:
        PRN_DBR_STSACK(dbr_stsack_string); break;
    default : strcpy (str, "can't print data type");
    }
    return str;
}





/*+**************************************************************************
 *
 * Function:	print_time_val_sts
 *
 * Description:	Print (to stdout) one wide output line
 *              (name, timestamp, value, status, severity)
 *
 * Arg(s) In:	pv      -  Pointer to pv structure
 *              nElems  -  Number of elements (array)
 *
 **************************************************************************-*/
 
/* all prints are commented out: uncomment if want to debug */
#define PRN_TIME_VAL_STS(TYPE,TYPE_ENUM)                                        \
    printAbs = 0;                                                               \
                                                                                \
    switch (tsType) {                                                           \
    case relative:                                                              \
        ptsRef = &tsStart;                                                      \
        break;                                                                  \
    case incremental:                                                           \
        ptsRef = &tsPrevious;                                                   \
        break;                                                                  \
    case incrementalByChan:                                                     \
        ptsRef = &pv->tsPrevious;                                               \
        break;                                                                  \
    default :                                                                   \
        printAbs = 1;                                                           \
    }                                                                           \
                                                                                \
    if (!printAbs) {                                                            \
        if (pv->firstStampPrinted)                                              \
        {                                                                       \
            /*printf("%10.4fs ", epicsTimeDiffInSeconds(                        \
			  &(((struct TYPE *)value)->stamp), ptsRef) )*/;                    \
        } else {                    /* First stamp is always absolute */        \
            printAbs = 1;                                                       \
            pv->firstStampPrinted = 1;                                          \
        }                                                                       \
    }                                                                           \
                                                                                \
    if (tsType == incrementalByChan)                                            \
        pv->tsPrevious = ((struct TYPE *)value)->stamp;                         \
                                                                                \
    if (printAbs) {                                                             \
        epicsTimeToStrftime(timeText, TIMETEXTLEN, timeFormatStr,               \
                            &(((struct TYPE *)value)->stamp));                  \
        /*printf("%s ", timeText)*/;                                            \
    }                                                                           \
                                                                                \
    tsPrevious = ((struct TYPE *)value)->stamp;                                 \
                                                                                \
                             /* Print count if array */                         \
    if ( nElems > 1 ) /*printf("%lu ", nElems)*/;                               \
                             /* Print Values */                                 \
    for (i=0; i<nElems; ++i) {                                                  \
        valstr = val2str(value, TYPE_ENUM, i, &valfloatarray[i]);               \
          /*if(nElems>1) printf ("[%3d] >>>%s<<<\n",i,valstr); */                                      \
    }                                                                           \
                             /* Print Status, Severity - if not NO_ALARM */     \
    if ( ((struct TYPE *)value)->status || ((struct TYPE *)value)->severity )   \
    {                                                                           \
      /*printf("ERRORRRRRRRRRRR: 0x%08x 0x%08x\n",                                \
                 ((struct TYPE *)value)->status,                                \
				 ((struct TYPE *)value)->severity)*/;                            \
        /*printf("%s %s\n",                                                     \
               stat_to_str(((struct TYPE *)value)->status),                     \
               sevr_to_str(((struct TYPE *)value)->severity))*/;                \
    } else {                                                                    \
        /*printf("\n")*/;                                                       \
    }



char *
print_time_val_sts (pv* pv, unsigned long nElems, float *valfloatarray)
{
  char timeText[TIMETEXTLEN];
  int i, printAbs;
  void *value; 
  epicsTimeStamp *ptsRef = &tsStart;
  epicsTimeStamp tsNow;
  char *valstr;

  value = pv->value;

  if (!tsInit)                /* Initialize start timestamp */
  {
    epicsTimeGetCurrent(&tsStart);
    tsPrevious = tsStart;
    tsInit = 1;
  }

  epicsTimeGetCurrent(&tsNow);
  epicsTimeToStrftime(timeText, TIMETEXTLEN, timeFormatStr, &tsNow);

  if (!pv->onceConnected)
  {
    valfloatarray[0] = -9999.0;
    printf("%-30s ", pv->name);
    printf("*** Not connected (PV not found)\n");
  }
  else if (pv->status == ECA_DISCONN)
  {
    valfloatarray[0] = -9999.0;
    printf("%-30s ", pv->name);
    printf("%s *** disconnected\n", timeText);
  }
  else if (pv->status == ECA_NORDACCESS)
  {
    valfloatarray[0] = -9999.0;
    printf("%-30s ", pv->name);
    printf("%s *** no read access\n", timeText);
  }
  else if (pv->status != ECA_NORMAL)
  {
    valfloatarray[0] = -9999.0;
    printf("%-30s ", pv->name);
    printf("%s *** CA error %s\n", timeText, ca_message(pv->status));
  }
  else if (pv->value == 0)
  {
    valfloatarray[0] = -9999.0;
    printf("%-30s ", pv->name);
    printf("%s *** no data available (timeout)\n", timeText);
  }
  else
  {
	/*
if(!strcmp(pv->name,"jscaler.SCALER_0")) printf(">>>>> %-30s >%d<\n",pv->name,pv->dbrType);
	*/
    switch (pv->dbrType)
    {
      case DBR_TIME_STRING:
          PRN_TIME_VAL_STS(dbr_time_string, DBR_TIME_STRING);
          break;
      case DBR_TIME_SHORT:
          PRN_TIME_VAL_STS(dbr_time_short, DBR_TIME_SHORT);
          break;
      case DBR_TIME_FLOAT:
          PRN_TIME_VAL_STS(dbr_time_float, DBR_TIME_FLOAT);
          break;
      case DBR_TIME_ENUM:
          PRN_TIME_VAL_STS(dbr_time_enum, DBR_TIME_ENUM);
          break;
      case DBR_TIME_CHAR:
          PRN_TIME_VAL_STS(dbr_time_char, DBR_TIME_CHAR);
          break;
      case DBR_TIME_LONG:
          PRN_TIME_VAL_STS(dbr_time_long, DBR_TIME_LONG);
          break;
      case DBR_TIME_DOUBLE:
          PRN_TIME_VAL_STS(dbr_time_double, DBR_TIME_DOUBLE);
		  /*
if(!strcmp(pv->name,"jscaler.SCALER_0")) printf("DOUBLES\n");
		  */
         break;
      default: printf("can't print data type\n");
    }
  }

  return(valstr);
}



/*+**************************************************************************
 *
 * Function:	create_pvs
 *
 * Description:	Creates an arbitrary number of PVs
 *
 * Arg(s) In:	pvs   -  Pointer to an array of pv structures
 *              nPvs  -  Number of elements in the pvs array
 *              pCB   -  Connection state change callback
 *
 * Arg(s) Out:	none
 *
 * Return(s):	Error code:
 *                  0  -  All PVs created
 *                  1  -  Some PV(s) not created
 *
 **************************************************************************-*/

int
create_pvs (pv* pvs, int nPvs, caCh *pCB)
{
  int n;
  int result;
  int returncode = 0;
                                 /* Issue channel connections */
  for(n=0; n<nPvs; n++)
  {
	/*printf("create_pvs: [%2d] name >%s<\n",n,pvs[n].name);*/
    result = ca_create_channel( pvs[n].name,
                                pCB,
                                &pvs[n],
                                CA_PRIORITY,
                                &pvs[n].ch_id);
    if(result != ECA_NORMAL)
    {
      printf("CA error %s occurred while trying "
              "to create channel '%s'.\n", ca_message(result), pvs[n].name);
      fprintf(stderr, "CA error %s occurred while trying "
              "to create channel '%s'.\n", ca_message(result), pvs[n].name);
      pvs[n].status = result;
      returncode = 1;
    }
  }

  return(returncode);
}

/*+**************************************************************************
 *
 * Function:	connect_pvs
 *
 * Description:	Connects an arbitrary number of PVs
 *
 * Arg(s) In:	pvs   -  Pointer to an array of pv structures
 *              nPvs  -  Number of elements in the pvs array
 *
 * Arg(s) Out:	none
 *
 * Return(s):	Error code:
 *                  0  -  All PVs connected
 *                  1  -  Some PV(s) not connected
 *
 **************************************************************************-*/
/* sergey: comment out prints */
int
connect_pvs (pv* pvs, int nPvs)
{
  int returncode = create_pvs ( pvs, nPvs, 0);
  if ( returncode == 0 )
  {
    /* Wait for channels to connect */
    int result = ca_pend_io (caTimeout);
    if (result == ECA_TIMEOUT)
    {
      if (nPvs > 1)
      {
printf("Channel connect timed out: some PV(s) not found.\n");
        /*fprintf(stderr, "Channel connect timed out: some PV(s) not found.\n")*/;
      }
      else
      {
printf("Channel connect timed out: '%s' not found.\n", pvs[0].name);
		/*fprintf(stderr, "Channel connect timed out: '%s' not found.\n", pvs[0].name)*/;
      }
      returncode = 1;
    }
  }
  return(returncode);
}
