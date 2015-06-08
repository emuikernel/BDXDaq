
/* UNIX only */

#ifndef VXWORKS

/* ERfrag.c - CLAS-specific part of Event Recorder */

#include <stdio.h>
#include <pthread.h>
#ifdef __sun
#include <dlfcn.h>
#endif

#include "da.h"
#include "rc.h"
#include "coda.h"
#include "bosio.h"


extern objClass localobject; /* defined in er_component.c */
extern char *session; /* coda_component.c */

/* ET stuff */

#include <time.h> 
#include "et.h" 

#define ET_EVENT_ARRAY_SIZE 100
extern char	      et_name[ET_FILENAME_LENGTH];
extern et_stat_id et_statid;
extern et_sys_id  et_sys;
extern et_att_id  et_attach;
extern int        et_init = 0;

/* end of ET stuff */


#undef TRUE
#define TRUE 1

#undef FALSE
#define FALSE 0

#define TCL_PROC(name) int name (objClass object, Tcl_Interp *interp, int argc, char **argv)

typedef int (*IFUNCPTR) ();

typedef struct ERpriv *ERp;

typedef struct ERpriv {
  BOSIO *fd; /*int fd;*/
  int buffer_count;
  int log_EvDumpLevel;
  char output_type[128];
  char *current_file;
  unsigned int output_switch;
  void *mod_id;
  char mod_name[30];
  int  record_length;
  int  split;
  char *filename;
  pthread_t net_thread;
  objClass object;
  IFUNCPTR write_proc;
  IFUNCPTR close_proc;
  IFUNCPTR open_proc;
  int splitnb;
  int nevents;
  int nlongs;
  int nerrors;
  int nend;
  char *runConfig;
} ER_priv;

extern int PrestartCount = 0;

#define FILE_OPEN_PROC   0
#define FILE_WRITE_PROC  1
#define FILE_CLOSE_PROC  2
#define FILE_FLUSH_PROC  3


static char *ER_RunFileDir="/usr/local/clas/parms/run_files";
/*static char *ER_RunFileDir=CLON_PARMS"/run_files";*/
/*static char *ER_RunFileDir="/home/clasrun/zzz";*/

static char *ER_RunFile=NULL;
static char *ER_Session=NULL;
static char *ER_DataFileDir=NULL;
static char *ER_DataFile=NULL;
static FILE *ER_fh=NULL;
static int ER_nrun;
static int ER_nlongs;
static int ER_nevents;
static int ER_nerrors;
static int ER_DirLng;
static int trigstat[12];

int ER_WriteStat(objClass object, FILE *fd) ;
int ER_WriteEOR(objClass object, FILE *fd) ;





int
FPACK_open_file(ERp erp)
{
  objClass object = localobject;

  int  status, ind;
  long clk;
  char tmp[1000];

  for(ind=0; ind<12; ind++) trigstat[ind] = 0; /* cleanup trigger statistic */

/*
printf("ERfrag: object=%u &object->private=%u\n",object,&object->private);
printf("runnumber=%u\n",object->runNumber);
printf("splitmb=%u\n",erp->splitnb);
printf("filename=%c%c...\n",erp->filename[1],erp->filename[2]);
*/


/* following piece from er_component.c - August 19, 1997 - Serguei */


  if (erp->filename[0] == '@') {
    char temp[500];
    printf("Executing file : %s\n",erp->filename);
      sprintf(temp,"%s %d %d $config %d",
	      &erp->filename[1],
	      object->runNumber, 
	      object->runType,
	      erp->splitnb);


/* Sergey: !!!!!!!!!!!!! MUST BE REDONE WITHOUT TCL !!!!!!!!!!!!!!!
    if (Tcl_VarEval(interp,temp,NULL) != TCL_OK) {
      printf ("ERROR", "Filename generation script %s failed",erp->filename);
      return TCL_ERROR;
      }
    if (erp->current_file)
      ckfree(erp->current_file);
    erp->current_file = strdup(interp->result);
*/


  } else {
    if (erp->current_file)
      ckfree(erp->current_file);

    erp->current_file = ckalloc((strlen(erp->filename)+100));
    sprintf(erp->current_file, erp->filename, object->runNumber,erp->splitnb);
  }

  /* end of piece from er_component.c */



  fprintf(stderr,"====>> FILE name given by ER is \"%s\" Run # %i\n",
		   erp->current_file,object->runNumber);
  if (strlen(erp->current_file) == 0) {
	fprintf(stderr,"====>> ERROR OPENING FPACK FILE : FILE_NAME EMPTY !!!!\n");
	return TCL_ERROR;
  }

  ER_nlongs   = ER_nevents   = ER_nerrors = 0;
  erp->nlongs = erp->nevents = erp->nerrors = erp->splitnb = 0;
  ER_nrun = object->runNumber;
  

  if(ER_Session != NULL) free(ER_Session);
  if(ER_RunFile != NULL) free(ER_RunFile);
  ER_Session = strdup(session);

  ER_RunFile = malloc(256);
  sprintf (ER_RunFile,"%s/runfile_%s_%6.6d.txt",ER_RunFileDir,ER_Session,ER_nrun);
  if ( (ER_fh = fopen (ER_RunFile,"w")) == NULL ) {
	perror ("FPACK_open_file: ");
	printf ("FPACK_open_file: ERROR : Can't open run summary file %s\n",ER_RunFile);
	return TCL_ERROR;
  } else printf ("FPACK_open_file: Open run summary file %s\n",ER_RunFile);

  clk=time(NULL);
  fprintf(ER_fh,
	"#\n"
	"#  %s\n"
	"#  %s\n"
	"#  Written by ER, contains list of all \n"
	"#  files produced during run %d for session <%s>\n\n"
	"#  blank lines and comment lines are ignored\n\n"
	"*BOR*     this is UNIX time\n"
	"  %u\n\n"
	"*FILES*\n"
	"#  dir                   filename                 nlong   nevent   nerror\n"
	"#  ---                   --------                 -----   ------   ------\n",
		  ER_RunFile,ctime(&clk),ER_nrun,ER_Session,clk);
  fflush(ER_fh);

  if (erp->current_file) {
    ckfree(erp->current_file);
   } 
  erp->current_file = ckalloc(1000);

  sprintf(erp->current_file,erp->filename,object->runNumber);
  fprintf(stderr,"====>> FILE name would be \"%s\" \n",erp->current_file);
  sprintf(tmp,"OPEN UNIT=11 FILE='%s' WRITE RECL=%d SPLITMB=%d RAW SEQ NEW BINARY",
	  erp->current_file,
	  erp->record_length,
	  erp->split/1024/1024);

  fprintf(stderr,"====>> Exec FPACK Command \"%s\"\n",tmp);
  if( (status = FParm(tmp,&erp->fd)) !=0)
  {
	sprintf(tmp,"FParm status %d \n",status);
	fprintf(stderr,tmp);
	erp->fd = (BOSIO *)(-1);
	return TCL_ERROR;
  }

  ckfree(erp->current_file);
  erp->current_file = strdup (((BOSIOptr)erp->fd)->finame);






  /* TCL_ HERE !!!!!!!!!!!!!!!!!!!!! */
  if(Tcl_VarEval(object->interp,"regexp -indices {[^/]*$} \"",
     erp->current_file,"\" file",NULL) != TCL_OK) return(TCL_ERROR);
  if(*(object->interp->result) == '0')
  {
	printf("FPACK_open_file: ERROR: Can't match file name in '%s'\n",
      erp->current_file);
	ER_nlongs = ER_nevents = ER_nerrors = 0;
	return(TCL_ERROR);
  }
  Tcl_Eval(object->interp,"set file");
  ind = atol(object->interp->result);
  ER_DataFileDir = malloc(ind+2);
  strncpy(ER_DataFileDir,erp->current_file,ind);
  ER_DataFileDir[ind]=0;
  ER_DirLng=ind;





  return(TCL_OK);
}

int
ER_WriteStat(objClass object, FILE *fd)
{
  ER_DataFile = ((ERp)object->private)->current_file + ER_DirLng;

  fprintf(fd,"%s\t%s\t\t\t%d\t%d\t%d\n"
		   ,ER_DataFileDir, ER_DataFile, ER_nlongs, ER_nevents, ER_nerrors);

  ER_nlongs = ER_nevents = ER_nerrors = 0;
  fflush (fd);
  
  return 0;
}

int
ER_WriteEOR(objClass object, FILE *fd)
{
  ER_DataFile = ((ERp)object->private)->current_file + ER_DirLng;

  fprintf(fd,
    "\n"
	"*L1STAT*    1       2       3       4       5       6       7       8       9      10      11      12\n"
	"      %7u %7u %7u %7u %7u %7u %7u %7u %7u %7u %7u %7u\n\n"
	"*EOR*     this is UNIX time\n"
	"  %u\n\n",trigstat[0],trigstat[1],trigstat[2],trigstat[3],trigstat[4],trigstat[5],
     trigstat[6],trigstat[7],trigstat[8],trigstat[9],trigstat[10],trigstat[11],time(NULL));

  fflush (fd);
  
  return 0;
}

int
FPACK_close_file(ERp erp)
{
  objClass object = localobject;

  int stat;

  printf("FPACK_close_file reached\n");
  if(erp->fd == (BOSIO *)(-1) || erp->fd == NULL)
  {
	puts ("FPACK file already closed or haven't been opened.");
	return TCL_ERROR;
  }

  stat = etFlush(erp->fd);

  if (stat != 0)
  {
    printf("ER: Error in etFlush %i\n",stat);
    fprintf(stderr,"Error in etFlush %i\n",stat);
    return TCL_ERROR;
  }

  ER_WriteStat(object,ER_fh);
  ER_WriteEOR(object,ER_fh);
  fclose(ER_fh);        ER_fh=NULL;
  free(ER_DataFileDir); ER_DataFileDir=NULL;
  free(ER_Session);     ER_Session=NULL;
  free(ER_RunFile);     ER_RunFile=NULL;


  fprintf(stderr,"====>> Exec FPACK Command \"CLOSE UNIT=11\"\n");
  if ( (stat = FParm("CLOSE UNIT=11",&erp->fd)) !=0)
  {
	char tmp[200];
	printf("ER: Error in FParm, status %d \n",stat);
	sprintf(tmp,"FParm status %d \n",stat);
	fprintf(stderr,tmp);
	return TCL_ERROR;
  }

  erp->fd = (BOSIO *)(-1);

  return(TCL_OK);
}




#define NLOOP 1000
/*#define NLOOP 100 */

int 
FPACK_write_event(ERp erp, int flag)
{
  int  nloop, stat1, stat2, stat3, stat4, stat5, err=FALSE;
  char tmp[2000];
  et_event *pe[ET_EVENT_ARRAY_SIZE];
  int control_0, nevents, i, len, *ev;

  struct timespec timeout;
  timeout.tv_sec  = 0;
  timeout.tv_nsec = 20000000; /* 20 msec */

  if(flag != 0)
  {
    printf("FPACK_write_event: flag=%d received\n",flag);
  }

  if(erp->fd == (BOSIO *)(-1))   /* This means we closed file already */
  {
    printf("ER: file closed already\n");
	return(1);
  }


  if (!et_alive(et_sys))
  {
    printf("ER: ERROR: Not attached to ET system\n");
    return(0);
  }
  control_0 = 0; /* in case following et_events_get() will not successful */
                 /* we have to be sure that control_0 != 20; otherwise    */
                 /* present function will return 0 ( see the end of the   */
                 /* present function ), and we're dead ...                */

  nloop = 0;
  do
  {
    if ((nloop++) > NLOOP)
    {
	  /*printf("nloop=%d\n",nloop);*/

      /* input parameter flag==1 means that we are exiting, so here we
      have to cleanup whatever necessary and return 0; currently I
      do not know what else to cleanup so just return 0 */
      if(flag==1)
      {
        printf("FPACK_write_event: flag==1(1) so return 0\n",flag);fflush(stdout);
        return(0);
	  }

      return(1);
    }

    /*stat1 = et_events_get(et_sys, et_attach, pe, ET_TIMED, &timeout, ET_EVENT_ARRAY_SIZE, &nevents);*/
    stat1 = et_events_get(et_sys, et_attach, pe, ET_ASYNC, NULL, ET_EVENT_ARRAY_SIZE, &nevents);

    /* fprintf(stderr, "debug: got %d events from ET (status = %d) \n", nevents, stat1); */

    if (nevents > 0 && stat1 == ET_OK)
    {
      /* fprintf(stderr, "debug: got %d events from ET (status = %d) \n", nevents, stat1);
      printf("nevents=%d\n",nevents);*/
      control_0 = pe[nevents-1]->control[0];

      stat3 = stat4 = stat5 = 0;

      for(i=0; i<nevents; i++)
      {
        et_event_getdata(pe[i],(void **) &ev); /* set pointer 'ev' to the data area */

        if( (pe[i]->length>>2) < 11 )
        {
          printf("Control event :  0x%8.8X  0x%8.8X  0x%8.8X  0x%8.8X\n",ev[0],ev[1],ev[2],ev[3]);
        }
        else if(1/* (stat3 = ddCheckRec(ev)) == 0*/ )
        {
          { int j; for(j=0; j<12; j++) trigstat[j] += (pe[i]->control[3] >> j) & 1; }
		  /*
recordPrint(ev);
		  */
          stat4 = etWrite(erp->fd,ev);

          if (stat4 == EBIO_NEWFILEOPEN)
          {
	        stat5 = 1; /* set new file flag */
	      }
        }
        if (erp->log_EvDumpLevel > 0)
        {
          printf("\nFIFO Entry: W1=0x%8.8X B1=0x%2.2X W2=0x%8.8X B2=0x%2.2X Len=%d Ptr=0x%8.8X\n",
		  pe[i]->control[0], (unsigned int)pe[i]->control[1], 
		  pe[i]->control[2], (unsigned int)pe[i]->control[3],
		  (pe[i]->length>>2), ev);
        }
        dumplogrec(ev,erp->log_EvDumpLevel);
        if (stat3 != 0 || (stat4 != EBIO_NEWFILEOPEN && stat4 != 0) )
        {
          ER_nerrors++;
          erp->nerrors++;
          printf ("Event %i is corrupted : stat3 = %i , stat4 = %i\n",ev[6],stat3,stat4);
          dumplogrec(ev,1);

        }
        else
        {
          len = pe[i]->length>>2;
          erp->object->nlongs += len;
          erp->object->nevents++;
          erp->nevents++;
          erp->nlongs += len;

          ER_nevents++;
          ER_nlongs   += len;
        }
      } /* for over nevents */

      if (stat5)
      {
        ER_WriteStat(erp->object,ER_fh); /* write statistic if new file was opened */

        if(erp->current_file)
        {
          ckfree(erp->current_file);
        }
        erp->current_file = strdup (((BOSIOptr)erp->fd)->finame);
        stat4 = stat5 = 0;
      }

      stat2 = et_events_put(et_sys, et_attach, pe, nevents);
      if (stat2 != ET_OK)
      {
        sprintf(tmp," error in et_events_put() status = %i",stat2);
        fprintf(stderr,tmp);
        printf("ER: error in et_event_put(): status = %d\n",stat2);
        err=TRUE;
      }
      if (control_0 == 20)
      {
        printf("ERfrag: end of run\n");
        break;
      }
    }
    else
    {
      if (stat1 == ET_OK)
      {
        /* OK but there were no events. */ 
        ; /* NOOP, try again */
      }
      else if (stat1 == ET_ERROR_EMPTY)
      {
        ; /* NOOP */ /* usleep(1); */
        /* printf("BOS: no (more) ET events.\n"); */
      }
       else if (stat1 == ET_ERROR_BUSY)
      {
        /* printf("ER_flush: ET is busy, wait and try again.\n");*/
        ; /* NOOP */ /* usleep(1); */
      }
      else
      {
        /* force end by setting nend, or net_thread hangs on event get */
        erp->nend--; /* ETOGO NET !!! */
        if (stat1 == ET_ERROR_DEAD)
        {
          /* it's possible to check et_alive to see if ET system died */
          sprintf(tmp,"forcing writer to quit read, status = %i \n",stat1);
          fprintf(stderr,tmp);
          err=TRUE;
        }
        else
        {
          sprintf(tmp,"error in et_event_get status = %i \n",stat1);
          fprintf(stderr,tmp);
          err=TRUE;
        }
      }
    }

  } while ((stat1 >= ET_OK) || 
	   (stat1==ET_ERROR_EMPTY) || 
	   (stat1==ET_ERROR_BUSY) || 
	   (stat1==ET_ERROR_TIMEOUT));

  if (control_0 == 17) /* prestart */
  {
    PrestartCount++;
  }
  if (control_0 == 20) /* end */
  {
    erp->nend--;
    printf("Got End event, Need %d more\n",erp->nend);
  }

  if (erp->nend <= 0)
  { 
    return(0);
  }
  else
  {
    /* input parameter flag==1 means that we are exiting, so here we
    have to cleanup whatever necessary and return 0; currently I
    do not know what else to cleanup so just return 0 */
    if(flag==1)
    {
      printf("FPACK_write_event: flag==1(2) so return 0\n",flag);fflush(stdout);
      return(0);
	}

    return(1);
  }
}


#else /* dummy VXWORKS */

void
FPACK_file_vxworks_dummy()
{
  return;
}

#endif
