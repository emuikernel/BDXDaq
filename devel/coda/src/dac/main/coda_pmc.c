
/* pmc_component.c - run at pmc board */

/* VXWORKS only; UNIX version is dummy */

#ifdef VXWORKS


#include <stdio.h>
#include <string.h>
#include <vxWorks.h>
#include <semLib.h>

#include "coda.h"
#include "tt.h"

#include "circbuf.h" /*'ttbosio.h' inside*/
#include "pmc.h"
#include "bigbuf.h"

#include "libtcp.h" 
#include "libdb.h" 

extern TTSPtr ttp; /* pointer to TTS structures for current roc */


/*sergey: should we use 'vxworks_task_delete' here ??? */

/* check if 'coda_proc' task exist; if it does kill it */
#define DELETE_PROC \
          iii = taskIdVerify(iProcStat); \
          /*printf("taskIdVerify returns %d\n",iii);*/ \
          if(iii==OK) /* check task status */ \
          { \
            iii = taskDelete(iProcStat); \
            printf("taskDelete(proc) returns %d\n",iii); \
            iProcStat = 0; \
            taskDelay(5*sysClkRateGet()); \
          }

#define DELETE_SUSPENDED_PROC \
          iii = taskIdVerify(iProcStat); \
          /*printf("taskIdVerify returns %d\n",iii);*/ \
          if(iii==OK) /* check task status */ \
          { \
            /* if task suspended, delete it */ \
            iii = taskIsSuspended(iProcStat); /* returns TRUE if suspended */ \
  	        if(iii==TRUE) \
            { \
              printf("INFO(coda_pmc): 'proc' is suspended, calling taskDelete(%d)\n", \
                iProcStat); \
              iii = taskDelete(iProcStat); \
              printf("taskDelete(proc) returns %d\n",iii); \
              iProcStat = 0; \
              taskDelay(5*sysClkRateGet()); \
	        } \
            else \
	        { \
              /*printf("INFO(coda_pmc): task is Ok\n")*/; \
	        } \
          }

/* check if 'coda_net' task exist; if it does kill it */
#define DELETE_NET \
          jjj = taskIdVerify(iNetStat); \
          /*printf("taskIdVerify returns %d\n",jjj);*/ \
          if(jjj==OK) /* check task status */ \
          { \
            jjj = taskDelete(iNetStat); \
            printf("taskDelete(net) returns %d\n",jjj); \
            iNetStat = 0; \
            taskDelay(5*sysClkRateGet()); \
          }

/* check if 'coda_net' task exist; if it does,
check it health and kill if it is suspended */
#define DELETE_SUSPENDED_NET \
          jjj = taskIdVerify(iNetStat); \
          /*printf("taskIdVerify returns %d\n",jjj);*/ \
          if(jjj==OK) /* check task status */ \
          { \
            /* if task suspended, delete it */ \
            jjj = taskIsSuspended(iNetStat); /* returns TRUE if suspended */ \
  	        if(jjj==TRUE) \
            { \
              printf("INFO(coda_pmc): 'net' is suspended, calling taskDelete(%d)\n", \
                iNetStat); \
              jjj = taskDelete(iNetStat); \
              printf("taskDelete(net) returns %d\n",jjj); \
              iNetStat = 0; \
              taskDelay(5*sysClkRateGet()); \
	        } \
            else \
	        { \
              /*printf("INFO(coda_pmc): task is Ok\n")*/; \
	        } \
          }



void
targetName_test()
{
  char *myname;

  myname = hostName();
  printf("hostName returns >%s<\n",myname);
  myname = targetName();
  printf("targetName returns >%s<\n",myname);
}





/****************/
/* main program */
/****************/


/* main_pmc.c - main CODA program running on secondary CPU */

/* General Purpose Register (GPR) is used for interprocessor
   syncronization */

/* the number of 'big' buffers */
#define NUM_SEND_BUFS        16



/* big buffers to be used between 'coda_proc' and 'coda_net' */
static BIGBUF *gbigBuffer1 = NULL;

/* net_thread structure, contains pointer to the gbigBuffer1 - to be used
 between 'coda_proc' and 'coda_net' */
static BIGNET big1;



volatile PMC *pmc; /* 'pmc->bignetptr' is structure for the first process, proc or net */
static int bufin[NWBOS+16], bufout[NWBOS+16];

void net_thread(BIGNET *bignetptr, unsigned int offset);
void proc_thread(BIGNET *bigprocptrin, unsigned int offsetin);

void
coda_pmc()
{
  int socketnum, port;
  unsigned int pmcadr;
  int iProcStat, iNetStat, cmd;
  int proc_on_pmc, net_on_pmc;
  MYSQL *dbsock = NULL;
  MYSQL_RES *result;
  int numRows;
  char tmp[256], *myname;
  int iii, jjj, ret;
  char host[128];

  /* allocate 'big' buffers */
  gbigBuffer1 = bb_new(NUM_SEND_BUFS, SEND_BUF_SIZE);
  if(gbigBuffer1 == NULL)
  {
    printf("coda_pmc: ERROR in bb_new: Buffer1 allocation FAILED\n");
    return(0);
  }
  else
  {
    printf("coda_pmc: big buffer1 allocated\n");
  }




  /*************************************************/
  /* update database: let others know we are here  */
  /* we will insert '0xffffffff' in 'StructAddr' field now */
  myname = targetName();
  printf("myname >%s<\n",myname);
  dbsock = dbConnect(getenv("MYSQL_HOST"), "daq");
  /* trying to select our name from 'PMCs' table */
  sprintf(tmp,"SELECT name FROM PMCs WHERE name='%s'",myname);
  if(mysql_query(dbsock, tmp) != 0)
  {
	printf("mysql error (%s)\n",mysql_error(dbsock));
    return(ERROR);
  }
  /* gets results from previous query */
  /* we assume that numRows=0 if our Name does not exist,
     or numRows=1 if it does exist */
  if( !(result = mysql_store_result(dbsock)) )
  {
    printf("ERROR in mysql_store_result (%)\n",mysql_error(dbsock));
    return(ERROR);
  }
  else
  {
    numRows = mysql_num_rows(result);
    mysql_free_result(result);
    /* NOTE: for VXWORKS 'Host' the same as 'Name' */
    /*printf("nrow=%d\n",numRows);*/
    if(numRows == 0)
    {
      sprintf(tmp,"INSERT INTO PMCs (name,Host,StructAddr) VALUES ('%s','%s',%d)",
        myname,myname,0xffffffff);
    }
    else if(numRows == 1)
    {
      sprintf(tmp,"UPDATE PMCs SET Host='%s',StructAddr=%d WHERE name='%s'",myname,0xffffffff,myname);
    }
    else
    {
      printf("ERROR: unknown nrow=%d",numRows);
      return(ERROR);
    }

    if(mysql_query(dbsock, tmp) != 0)
    {
	  printf("ERROR\n");
      return(ERROR);
    }
    else
    {
      printf("Query >%s< succeeded\n",tmp);
    }
  }

  dbDisconnect(dbsock);

  /* just wait 10 seconds */
  taskDelay(sysClkRateGet()*10);






#ifdef PMCOFFSET



#ifdef USE_PCI_BRIDGE_REGISTER

  /* this program usualy starts from boot script; at that time we will
	 cleanup GPR and wait for its value become non-zero */
  usrWriteGPR(0);
  /* wait for 'pmc' structure address to be passed from host board; */
  /* it is 'localpmc' in roc_component.c */
  while((pmcadr=usrReadGPR())==0)
  {
	/*printf("waiting for structure address from host ..\n");*/
    /*Sergey: when parameter was 10, PMC280's frcPciShow crashed !!!*/ 
    taskDelay(100);
  }
#else  /* use database */

  /* wait until 'StructAddr' in 'PMCs' table become positive */
  printf("Waiting for StructAddr from host board ..\n");
  while(1)
  {
    /* wait before next enquiry */
    taskDelay(sysClkRateGet()*3);

    dbsock = dbConnect(getenv("MYSQL_HOST"), "daq");
    sprintf(tmp,"SELECT StructAddr from PMCs WHERE name='%s'",myname);
    if(dbGetInt(dbsock, tmp, &pmcadr)==ERROR)
    {
      printf("ERROR: cannot select StructAddr from PMCs for name=%s\n",myname);
      dbDisconnect(dbsock);
      return(ERROR);
    }
    else
    {
      if(pmcadr!=0xffffffff)
      {
        dbDisconnect(dbsock);
        break;
	  }
    }
    dbDisconnect(dbsock);
  }

  printf("StructAddr set to 0x%08x\n",pmcadr);
#endif


  pmc = (PMC *) (pmcadr+PMCOFFSET);
  printf("coda_pmc: received structure address is 0x%08x -> 0x%08x\n",
    pmcadr,pmc);

  /* start main loop */
  while(1)
  {
    /*printf("waiting for the command from host ..\n");*/
    taskDelay(sysClkRateGet());
    cmd = pmc->cmd & DC_CMD_MASK;
    /*printf("cmd=0x%08x(addr=0x%08x)\n",cmd,pmc);*/

    switch(cmd)
    {
      case DC_DOWNLOAD:
        printf("Download command received\n");

        /* add offset to the 'bignet' pointer inside 'pmc' structure */
        printf(" -> bignet at 0x%08x\n",pmc->bignetptr);
        pmc->bignetptr = (((int)(pmc->bignetptr)) + PMCOFFSET);
        printf(" <- bignet at 0x%08x\n",pmc->bignetptr);

        printf("bignet at 0x%08x, bignet.gbigBuffer at 0x%08x -> 0x%08x\n",
          pmc->bignetptr, &(pmc->bignetptr->gbigBuffer),
          (&(pmc->bignetptr->gbigBuffer))+PMCOFFSET);

        proc_on_pmc = pmc->bignetptr->proc_on_pmc;
        net_on_pmc = pmc->bignetptr->net_on_pmc;
        printf("proc_on_pmc=%d, net_on_pmc=%d\n",
          proc_on_pmc,net_on_pmc);

        if(proc_on_pmc)
        {
          printf("rocID=%d\n",pmc->bignetptr->rocid);
          proc_download(pmc->bignetptr->rolname,
                        pmc->bignetptr->rolparams,
                        pmc->bignetptr->rocid);
        }

        pmc->csrr = DC_DOWNLOADED;
        pmc->cmd = 0; /* always cleanup command register after command processed */
        break;

      case DC_PRESTART:
        printf("Prestart command received\n");

        /* copy all info from first 'bigbuf' into local 'bigbuf' structure,
        we may need it over there */
        big1.failure = 0;
        big1.doclose = 0;
        big1.token_interval = pmc->bignetptr->token_interval;
        big1.proc_on_pmc = pmc->bignetptr->proc_on_pmc;
        big1.net_on_pmc = pmc->bignetptr->net_on_pmc;
        big1.rocid = pmc->bignetptr->rocid;

        big1.gbigBuffer = gbigBuffer1; /* input is previous's output */
        big1.gbigBuffer1 = NULL; /* no output big buffers */


        printf("port=%d (0x%08x)\n",pmc->bignetptr->port,
          &(pmc->bignetptr->port));
        printf("host >%s<\n",pmc->bignetptr->host);

        if(proc_on_pmc)
        {
          vxworks_task_delete("coda_proc");
          iProcStat = 0;
          vxworks_task_delete("coda_net");
          iNetStat = 0;
		  /*
          DELETE_SUSPENDED_PROC;
          DELETE_SUSPENDED_NET;
		  */

          printf("rocID=%d\n",big1.rocid);
          proc_prestart(big1.rocid);

          bb_init(&gbigBuffer1); /*init output buffers*/

          /* set output big buffers for 'coda_proc' */
          pmc->bignetptr->gbigBuffer1 = gbigBuffer1;

          bb_init(&(pmc->bignetptr->gbigBuffer)); /*init input buffers (done in ROC ???)*/
          pmc->bignetptr->failure = 0;


          /* Spawn the Process Thread */
          iProcStat = taskSpawn("coda_proc", 115, 0, 500000, proc_thread,
                              pmc->bignetptr, PMCOFFSET, 0,0,0,0,0,0,0,0);
          printf("taskSpawn(\"coda_proc\",...) returns %d\n",iProcStat);




          /* create a TCP connection to EB */
          ret = rocOpenLink(pmc->bignetptr->roc_name, pmc->bignetptr->eb_name, host, &port, &socketnum);
          if(ret<0)
		  {
            printf("coda_pmc ERROR: CANNOT ESTABLISH TCP TO EB !!!\n");
            pmc->cmd = 0; /* always cleanup command register after command processed */
            break;
		  }
          if(socketnum>0)
	      {
            pmc->bignetptr->socket = socketnum;
            pmc->bignetptr->port = port;
            strcpy((char *)pmc->bignetptr->host, host);

            memcpy((char *)big1.host, (char *)pmc->bignetptr->host, 128);
            big1.port = pmc->bignetptr->port;
            big1.socket = pmc->bignetptr->socket;
	      }
          else
		  {
            printf("coda_pmc: FATAL ERROR(1): cannot open TCP to EB\n");
		  }

          bb_init(&big1.gbigBuffer); /*init input buffers (done above ???)*/
          big1.failure = 0;

          /* Spawn the Network Thread */
          iNetStat = taskSpawn("coda_net", 120, 0, 500000, net_thread,
                                &big1,  0,  0,0,0,0,0,0,0,0);
          printf("taskSpawn(\"coda_net\",...) returns %d\n",iNetStat);

        }
        else if(net_on_pmc)
        {
          vxworks_task_delete("coda_net");
          iNetStat = 0;
          /*DELETE_SUSPENDED_NET;*/

          bb_init(&(pmc->bignetptr->gbigBuffer)); /*init input buffers (done in ROC ???)*/
          pmc->bignetptr->failure = 0;

          /* create a TCP connection to EB */
          ret = rocOpenLink(pmc->bignetptr->roc_name, pmc->bignetptr->eb_name, host, &port, &socketnum);
          if(ret<0)
		  {
            printf("coda_pmc ERROR: CANNOT ESTABLISH TCP TO EB !!!\n");
            pmc->cmd = 0; /* always cleanup command register after command processed */
            break;
		  }
          if(socketnum>0)
	      {
            pmc->bignetptr->socket = socketnum;
            pmc->bignetptr->port = port;
            strcpy((char *)pmc->bignetptr->host, host);

            memcpy((char *)big1.host, (char *)pmc->bignetptr->host, 128);
            big1.port = pmc->bignetptr->port;
            big1.socket = pmc->bignetptr->socket;
	      }
          else
		  {
            printf("coda_pmc: FATAL ERROR(1): cannot open TCP to EB\n");
		  }
          printf("socket=%d, port=%d\n",
            pmc->bignetptr->socket,pmc->bignetptr->port);


          /* Spawn the Network Thread */
          iNetStat = taskSpawn("coda_net", 120, 0, 500000, net_thread,
                                pmc->bignetptr, PMCOFFSET, 0,0,0,0,0,0,0,0);
          printf("taskSpawn(\"coda_net\",...) returns %d\n",iNetStat);

        }

        pmc->csrr = DC_PRESTARTED;
        pmc->cmd = 0; /* always cleanup command register after command processed */
        break;

      case DC_GO:
        printf("Go command received: net=%d proc=%d\n",net_on_pmc,proc_on_pmc);
        if(proc_on_pmc)
        {
          printf("calls 'proc_go', rocid=%d\n",big1.rocid);
          proc_go(big1.rocid);
        }
        pmc->csrr = DC_ACTIVE;
        pmc->cmd = 0; /* always cleanup command register after command processed */
        break;

      case DC_END:
        printf("End command received\n");

        taskDelay(5*sysClkRateGet()); /* give it time for graceful exit */
		/*
        DELETE_PROC;
        DELETE_NET;
		*/
        pmc->csrr = DC_ENDED;
        pmc->cmd = 0; /* always cleanup command register after command processed */
        break;

      case DC_RESET:
        printf("Reset command received\n");

        vxworks_task_delete("coda_proc");
        vxworks_task_delete("coda_net");
		/*
        DELETE_PROC;
        DELETE_NET;
		*/
        pmc->csrr = DC_ENDED;
        pmc->cmd = 0; /* always cleanup command register after command processed */
        break;

      default:
        /*printf("coda_pmc: waiting for the command: csr=0x%08x\n",pmc->csrr);*/
        taskDelay(sysClkRateGet());
    }

    /*pmc->heart = 1;*/ /* tell host board we are alive */
  }

  printf("coda_pmc: EXIT\n");

#endif

  exit(0);
}


/* CODA transitions (DUMMY here) */

int
codaDownload(char *conf)
{
  codaUpdateStatus("downloaded");
  return(OK);
}

int
codaPrestart()
{
  codaUpdateStatus("prestarted");
  return(OK);
}

int
codaGo()
{
  codaUpdateStatus("active");
  return(OK);
}

int
codaEnd()
{
  codaUpdateStatus("downloaded");
  return(OK);  
}

int
codaPause()
{
  codaUpdateStatus("paused");
  return(OK);
}

int
codaExit()
{
  codaUpdateStatus("configured");
  return(OK);
}



#else

/* dummy UNIX version */

int
main()
{
  printf("coda_pmc does exist for VxWorks only\n");
}

#endif

