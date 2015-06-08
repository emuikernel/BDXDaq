
#define NO_DP_ASK


//-----------------------------------------------------------------------------
// Copyright (c) 1994,1995 Southeastern Universities Research Association,
//                         Continuous Electron Beam Accelerator Facility
//
// This software was developed under a United States Government license
// described in the NOTICE file included as part of this distribution.
//
// CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
//       coda@cebaf.gov  Tel: (804) 249-7030     Fax: (804) 249-5800
//-----------------------------------------------------------------------------
//
// Description:
//      CODA remote component communication routines
//
// Author:  
//      Jie Chen
//      CEBAF Data Acquisition Group
//
// Revision History:
//   $Log: codaCompClnt.cc,v $
//   Revision 1.18  1998/11/05 20:11:38  heyes
//   reverse status updating to use UDP, fix other stuff
//
//   Revision 1.17  1998/10/16 15:08:51  heyes
//   order list of components by type
//
//   Revision 1.16  1998/06/02 19:51:44  heyes
//   fixed rcServer
//
//   Revision 1.15  1997/10/24 12:23:18  heyes
//   fix dplite bug
//
//   Revision 1.14  1997/09/19 17:57:20  heyes
//   longer timeouts
//
//   Revision 1.13  1997/09/16 12:28:41  heyes
//   make comms async
//
//   Revision 1.12  1997/09/11 16:07:12  heyes
//   fix DP_cmd
//
//   Revision 1.11  1997/08/25 15:57:30  heyes
//   use dplite.h
//
//   Revision 1.10  1997/06/13 21:30:43  heyes
//   for marki
//
//   Revision 1.9  1997/05/20 15:12:26  heyes
//   Always DP_ask for status
//
//   Revision 1.8  1997/02/25 19:27:19  heyes
//   added zap command
//
//   Revision 1.7  1997/02/03 13:47:31  heyes
//   add ask command
//
//   Revision 1.6  1996/11/27 15:06:09  chen
//   fix CDEV and Set internal itcl component state before doing state transition
//
//   Revision 1.5  1996/10/31 15:56:05  chen
//   Fixing boot stage bug + reorganize code
//
//   Revision 1.4  1996/10/28 20:32:07  heyes
//   timeout on DP_ask
//
//   Revision 1.3  1996/10/28 14:23:01  heyes
//   communication with ROCs changed
//
//   Revision 1.2  1996/10/14 20:02:46  heyes
//   changed message system
//
//   Revision 1.1.1.1  1996/10/11 13:39:19  chen
//   run control source
//
//
#include <stdio.h>
#include <string.h>
#include <daqConst.h>
#include <daqState.h>

/*sergey
#include <dplite.h> 
*/

/***************************************/
/***************************************/


/* NOTE: DP_ask use 'process' table to get host and port
   to communicate with; port number set in the end of boot
   process (constructor ?); we can use the same idea ???!!! */

/* dpnslib.c:

    int res;
    char tmp[200],host[100],port[100];
    m_result *result;
    m_row    row;
 
    sprintf(tmp,"select host,port from process where name='%s'",rpc_argv[0]);
    res = msqlDoQuery( tmp); 
    if (res < 0) {
      Tcl_AppendResult(interp,"DP_ask (msql error : ", msqlErrMsg," )", (char *) NULL);
      return TCL_ERROR;
    }   
    result = msqlStoreResult();

    if (msqlNumRows(result) == 0) {
      Tcl_AppendResult(interp,"DP_ask (server ", rpc_argv[0]," not in database)", (char *) NULL);
      msqlFreeResult(result);
      return TCL_ERROR;
    }
    row = msqlFetchRow(result);

    strcpy(host,row[0]);
    strcpy(port,row[1]);

    msqlFreeResult(result);

 */



/***************************************/
/***************************************/
/* Sergey: tcpClient as DP replacement */


/* tcpClient.c - TCP client example */ 
/* DESCRIPTION This file contains the client-side of the VxWorks TCP example code. 
   The example code demonstrates the usage of several BSD 4.4-style socket routine calls. */ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>

#define TRUE 1
#define FALSE 0
#define OK 0
#define ERROR -1
#define STD_IN stdin


#include "libtcp.h"
#include "libdb.h"

static void
alarmHandler(int sig)
{
  printf("codaCompClnt: timeout exit from tcpClient\n");fflush(stdout);
  signal(sig, alarmHandler);
  return;
}

/**************************************************************************** 
* * tcpClient - send requests to server over a TCP socket 
* * This routine connects over a TCP socket to a server, and sends a 
* user-provided message to the server.
* * This routine may be invoked as follows: 
* -> tcpClient("remoteSystemName","This is my message");  
* * RETURNS: OK, or ERROR if the message could not be sent to the server. */ 

int
tcpClient(char *name, char *message)
{
  struct hostent *hptr;
  TREQUEST myRequest; /* request to send to server */ 
  struct sockaddr_in serverAddr; /* server's socket address */ 
  int sockAddrSize; /* size of socket address structure */ 
  int sFd; /* socket file descriptor */ 
  int mlen; /* length of message */
  int rBytes;
  int i, portnum = 0;
  char hostname[128];
  MYSQL *dbsock;
  char tmp[1000], temp[100];

  /* create client's socket */ 
  if((sFd = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
  {
    perror("socket"); 
    return(ERROR); 
  } 

  /* bind not required - port number is dynamic */ 
  /* build server socket address */ 
  sockAddrSize = sizeof (struct sockaddr_in); 
  memset((char *) &serverAddr, 0, sockAddrSize); 
  serverAddr.sin_family = AF_INET; 


  /* get port and host from DB; if no record in DB for <target name> - exit */
  dbsock = dbConnect(getenv("MYSQL_HOST"), getenv("EXPID"));

  /* use 'inuse' field */
  sprintf(tmp,"SELECT inuse FROM process WHERE name='%s'",name);
#ifdef DEBUG_MSGS
  printf("DB select: >%s<\n",tmp);
#endif
  if(dbGetInt(dbsock, tmp, &portnum)==ERROR) return(ERROR);

  sprintf(tmp,"SELECT host FROM process WHERE name='%s'",name);
#ifdef DEBUG_MSGS
  printf("DB select: >%s<\n",tmp);
#endif
  if(dbGetStr(dbsock, tmp, hostname)==ERROR) return(ERROR);

  dbDisconnect(dbsock);
#ifdef DEBUG_MSGS
  printf("hostnamee=>%s< portnum=%d\n",hostname,portnum);
#endif
  serverAddr.sin_port = htons(portnum);

  hptr = gethostbyname(hostname);
  if(hptr == NULL)
  {
    printf("unknown hostname %s \n",hostname); 
    close(sFd);
    exit(1);
  }
  else
  {
    memcpy(&serverAddr.sin_addr,*(hptr->h_addr_list),sizeof(sizeof(struct in_addr)));
  }

  
  signal(SIGALRM,alarmHandler);
  alarm(3); /* in 3 seconds connect call will be interrupted if did not finished */
  /*
  printf("calling connect ..\n");fflush(stdout);
  */
  /* connect to server */ 
  if(connect (sFd, (struct sockaddr *) &serverAddr, sockAddrSize) == ERROR)
  {
    perror("connect"); 
    close(sFd); 
    return(ERROR); 
  }
  /* 
  printf(".. connected !\n");fflush(stdout);
  */
  alarm(0); /* clear alarm so it will not interrupt us */

  /* build request, prompting user for message */ 

  myRequest.msgLen = strlen(message);
  sprintf(myRequest.message,"%s\0",message);
  myRequest.reply = FALSE;

  /* convert integers to network byte order */
  myRequest.msgLen = htonl(myRequest.msgLen);
  myRequest.reply = htonl(myRequest.reply);

#ifdef DEBUG_MSGS
  printf(" Sending %d bytes: %s\n",myRequest.msgLen, myRequest.message);
#endif

  /* send request to server */ 
  if(write(sFd, (char *) &myRequest, sizeof (myRequest)) == ERROR)
  {
    perror("write"); 
    close(sFd); 
    return(ERROR);
  } 

  close(sFd); 

  return(OK); 
} 





/***************************************/
/***************************************/
/***************************************/





#ifdef USE_TK
#include <rcTclInterface.h>
#endif

#include "codaCompClnt.h"

int compTimeOut = 20;  // mseconds

#define   CODA_SCRIPT_LENGTH 256


#if defined (_CODA_2_0_T) || defined (_CODA_2_0)


#ifdef NO_DP_ASK
/* dummy DP initialization */
int
DP_cmd_init(char *)
{
  return(0);
}
#endif

int
codaDaCreate(char *name, int version, int id, char *type, char *host, int interval)
{
  int state = 0;
  int status;
  char temp [1000];
  char res[1000];

#ifdef DEBUG_MSGS
  printf("codaDaCreate reached\n");
#endif

  // codaDaReport second arg is 1 means "really ask"
  // printf("here in codaDaCreate for %s\n",name);
  if((state = codaDaReport (name, interval)) == CODA_DISCONNECTED)
  {
    return(state);
  }
  else if(state >= CODA_BOOTED)
  {
#ifdef NO_DP_ASK
    status = CODA_SUCCESS;
#else
    sprintf(temp,"setup_rcudp %s %d000; \n", name,interval);
    status = DP_cmd(name,temp,res,compTimeOut);
#ifdef DEBUG_MSGS
    printf("codaDaCreate: DP_cmd(%s) returns status=%d\n",temp,status);
#endif
#endif
    if(status != CODA_SUCCESS)
    {
	  printf("%s Failed to start UDP : %s\n",name,res);
      return(CODA_DISCONNECTED);
    }
    return(CODA_BOOTED);
  }
  else
  {
    printf("%s disconnected\n",name );
    return(CODA_DISCONNECTED);
  }

}

int
codaDaConnect(char *name, int version, int id, char *type, char *host)
{
printf("codaDaConnect reached\n");
exit(0);
  return(CODA_BOOTED);
}

int
codaDaRemove(char *name)
{
printf("codaDaRemove reached\n");
exit(0);
  return(CODA_DORMANT);
}

int
codaDaDownload(char *name, char *paramL)
{
  int status = 0;
  char temp[CODA_SCRIPT_LENGTH];

#ifdef DEBUG_MSGS
  printf("codaDaDownload reached\n");
#endif

#ifdef NO_DP_ASK
  sprintf(temp,"download %s",paramL);
  status = tcpClient(name,temp);

/* give component time to update DB with new status ! 
sleep(1);*/

#else
  sprintf(temp,"%s download %s",name,paramL);
  status = DP_cmd_async(name,temp);
#endif

  if(status == CODA_SUCCESS)
  {
    return(CODA_DOWNLOADING);
  }
  else
  {
    return(CODA_DORMANT);
  }
}

int
codaDaPrestart(char *name, int rn, int rt)
{
  int status = 0;
  char temp [CODA_SCRIPT_LENGTH];

#ifdef NO_DP_ASK
  status = tcpClient(name,"prestart");
#else
  sprintf(temp,"%s prestart",name);
  status = DP_cmd_async(name,temp);
#endif

  if(status == CODA_SUCCESS)
  {
    return(CODA_PRESTARTING);
  }
  else
  {
    return(CODA_DORMANT);
  }
}

int
codaDaEnd(char *name, int special)
{
  int status = 0;
  char temp [CODA_SCRIPT_LENGTH];

#ifdef NO_DP_ASK
  status = tcpClient(name,"end");
#else
  sprintf(temp,"%s end",name);
  status = DP_cmd_async(name,temp);
#endif
  
  if(status == CODA_SUCCESS)
  {
    return(CODA_ENDING);
  }
  else
  {
    return(CODA_DORMANT);
  }
}

int
codaDaPause(char *name, int mask)
{
  int status = 0;
  char temp [CODA_SCRIPT_LENGTH];

#ifdef NO_DP_ASK
  status = tcpClient(name,"pause");
#else
  sprintf(temp,"%s pause",name);
  status = DP_cmd_async(name,temp);
#endif
  
  if(status == CODA_SUCCESS)
  {
    return(CODA_PAUSING);
  }
  else
  {
    return(CODA_DORMANT);
  }
}

int
codaDaGo(char *name, int mask)
{
  int status = 0;
  char temp [CODA_SCRIPT_LENGTH];

#ifdef DEBUG_MSGS
  printf("codaDaGo reached, name >%s<, mask=0x%08x\n",name,mask);
#endif

#ifdef NO_DP_ASK
  status = tcpClient(name,"go");
#else
  sprintf (temp, "%s go",name);
  status = DP_cmd_async(name,temp);
#endif
  
  if(status == CODA_SUCCESS)
  {
    return(CODA_ACTIVATING);
  }
  else
  {
    return(CODA_DORMANT);
  }
}

int
codaAskComponent(char *command, char **res)
{
  int status = 0;
  char temp [CODA_SCRIPT_LENGTH];
  static char result[1000];;
  *res = result;

  strcpy(temp,command);
printf("codaAskComponent >%s<\n",temp);
exit(0);
  strtok(temp," ");

#ifdef NO_DP_ASK
  printf("codaAskComponent: do not know howto ..\n");
#else
  status = DP_cmd(temp,command,result,4);
#endif

  return(status);
}

int
codaDaConfigure(char *name, int type)
{
printf("codaDaConfigure reached\n");
exit(0);
  return(CODA_CONFIGURED);
}

int
codaDaReset(char *name)
{
  int status = 0;
  char temp [CODA_SCRIPT_LENGTH];

#ifdef NO_DP_ASK
  status = tcpClient(name,"exit");
#else
  sprintf (temp, "dp_after 1000 %s exit", name);
  status = DP_cmd_async(name,temp);
#endif
      
  return(status);
}
    
int
codaDaTerminate(char *name)
{
printf("codaDaTerminate reached\n");
exit(0);

  return(codaDaReset(name));
}

int
codaDaZap(char *name)
{
  int status = 0;
  char temp [CODA_SCRIPT_LENGTH];

printf("codaDaZap reached\n");
exit(0);

  sprintf (temp, "dp_after 1000 %s zap", name);
#ifdef NO_DP_ASK
  printf("codaDaZap: do not know howto ..\n");
#else
  status = DP_cmd_async(name,temp);
#endif
  return(status);
}

int
codaDaReport(char *name, int how)
{
  int status = 0;
  char temp [CODA_SCRIPT_LENGTH];
  char res[1000];
  char res2[1000];
  MYSQL *dbsock;
  char tmp[1000];

#ifdef DEBUG_MSGS
  printf("codaDaReport reached\n");
#endif

#ifdef NO_DP_ASK
  /* get component status from DB */
  dbsock = dbConnect(getenv("MYSQL_HOST"), getenv("EXPID"));
  sprintf(tmp,"SELECT state FROM process WHERE name='%s'",name);
#ifdef DEBUG_MSGS
  printf("DB select: >%s<\n",tmp);
#endif
  if(dbGetStr(dbsock, tmp, res)==ERROR) status = CODA_ERROR;
  else                                  status = CODA_SUCCESS;
  dbDisconnect(dbsock);
#else
  sprintf (temp, "%s status",name);
  status = DP_cmd(name,temp,res,compTimeOut);
#endif

#ifdef DEBUG_MSGS
  printf("codaDaReport: name>%s< state>%s<\n",name,res);
#endif

  if(status == CODA_SUCCESS)
  {
#ifndef NO_DP_ASK
    sprintf (temp, "set_udpRate %d000",how);
#ifdef DEBUG_MSGS
    printf("codaDaReport, set rate to %d ",how);
#endif
    status = DP_cmd(name,temp,res2,compTimeOut);
#ifdef DEBUG_MSGS
    printf(" Seconds , cmd result is %s\n",res2);
#endif
#endif
    if (::strcmp (res, "dormant") == 0) {
      return CODA_DORMANT;
    }
    else if (::strcmp (res, "booting") == 0) {
      return CODA_BOOTING;
    }
    else if (::strcmp (res, "booted") == 0) {
      return CODA_BOOTED;
    }
    else if (::strcmp (res, "configuring") == 0) {
      return CODA_CONFIGURING;
    }
    else if (::strcmp (res, "configured") == 0) {
      return CODA_CONFIGURED;
    }
    else if (::strcmp (res, "downloading") == 0) {
      return CODA_DOWNLOADING;
    }														  
    else if (::strcmp (res, "downloaded") == 0) {
      return CODA_DOWNLOADED;
    }
    else if (::strcmp (res, "prestarting") == 0) {
      return CODA_PRESTARTING;
    }
    else if (::strcmp (res, "paused") == 0) {
      return CODA_PAUSED;
    }
    else if (::strcmp (res, "activating") == 0) {
      return CODA_ACTIVATING;
    }
    else if (::strcmp (res, "active") == 0) {
      return CODA_ACTIVE;
    }
    else if (::strcmp (res, "ending") == 0) {
      return CODA_ENDING;
    }
    else if (::strcmp (res, "prestarted") == 0) {
      return CODA_PRESTARTED;
    }
    else if (::strcmp (res, "resetting") == 0) {
      return CODA_RESETTING;
    }
  }
  else if (status == -5) {
    return CODA_BUSY;
  } else {
    return CODA_DISCONNECTED;
  }
}

int
codaDaReport2 (char* name)
{
  int status = 0;
  char temp [CODA_SCRIPT_LENGTH];
  char res[1000];

printf("codaDaReport2 reached\n");
exit(0);

  sprintf (temp, "%s status", name);
#ifdef NO_DP_ASK
  printf("codaDaReport: do not know howto .. (2)\n");
#else
  status = DP_cmd(name,temp,res,compTimeOut);
#endif
  printf("codaDaReport2 status %d result %s\n",status);

  if (status == CODA_SUCCESS) {
    if (::strcmp (res, "dormant") == 0) {
      return CODA_DORMANT;
    }
    else if (::strcmp (res, "booting") == 0) {
      return CODA_BOOTING;
    }
    else if (::strcmp (res, "booted") == 0) {
      return CODA_BOOTED;
    }
    else if (::strcmp (res, "configuring") == 0) {
      return CODA_CONFIGURING;
    }
    else if (::strcmp (res, "configured") == 0) {
      return CODA_CONFIGURED;
    }
    else if (::strcmp (res, "downloading") == 0) {
      return CODA_DOWNLOADING;
    }
    else if (::strcmp (res, "downloaded") == 0) {
      return CODA_DOWNLOADED;
    }
    else if (::strcmp (res, "prestarting") == 0) {
      return CODA_PRESTARTING;
    }
    else if (::strcmp (res, "paused") == 0) {
      return CODA_PAUSED;
    }
    else if (::strcmp (res, "activating") == 0) {
      return CODA_ACTIVATING;
    }
    else if (::strcmp (res, "active") == 0) {
      return CODA_ACTIVE;
    }
    else if (::strcmp (res, "ending") == 0) {
      return CODA_ENDING;
    }
    else if (::strcmp (res, "prestarted") == 0) {
      return CODA_PRESTARTED;
    }
    else if (::strcmp (res, "resetting") == 0) {
      return CODA_RESETTING;
    }
  }
  else if (status == -5) {
    return CODA_BUSY;
  } else {
    return CODA_DISCONNECTED;
  }
}

int
codaDaReadInt(char *name, char *attr, long *value)
{
  int status = 0;
  char temp [CODA_SCRIPT_LENGTH];
  char res[1000];

printf("codaDaReadInt reached\n");
exit(0);
  
  sprintf (temp, "%s configure -%s", 
	    name, attr);

  printf ("%s configure -%s\n", 
	    name, attr);

#ifdef NO_DP_ASK
  printf("codaDaReadInt: do not know howto ..\n");
#else
  status = DP_cmd(name,temp,res,10);
#endif

  if (status == CODA_SUCCESS) {
    ::strncpy (temp, res, sizeof (temp));
    char token0[64], token1[64];
    int  result;
    if (::sscanf (temp, "%s %s %d", token0, token1, &result) >= 3) 
      *value = result;
    else
      *value = 0;
  }
  else
    *value = 0;

  return status;
}


int
codaDaWriteInt (char* name, char* attr, long* value)
{
  return CODA_SUCCESS;
}

int
codaCreateDaComp (char* name)
{
  return CODA_SUCCESS;
}

int
codaDeleteDaComp (char* name)
{
  return CODA_SUCCESS;
}

int
codaDaCompConfigure (char* name)
{
  return CODA_SUCCESS;
}

int
codaDaCompSetState (char* name, int st)
{
  //sergey: it called, do not know why ...
#ifdef DEBUG_MSGS
  printf("Sergey: DUMMY codaDaCompSetState reached\n");
#endif
  return CODA_SUCCESS;
}

#else

extern "C" {
int daCreate     (char* name, int version, int id, 
			 char* type, char* host);
int daConnect    (char* name, int version, int id,
			 char* type, char* host);
int daRemove     (char* name);
int daDownload   (char* name, char* paramL);
int daPrestart   (char* name, int rn, int rt);
int daEnd        (char* name, int special);
int daPause      (char* name, int mask);
int daGo         (char* name, int mask);
int daConfigure  (char* name, int type);
int daReset      (char* name);
int daTerminate  (char* name);
int daReport     (char* name);
int daReadInt    (char* name, char* attr, long* value);
int daWriteInt   (char* name, char* attr, long* value);
int daZap        (char* name);
};

int
codaDaCreate (char* name, int version, int id, 
	      char* type, char* host)
{
  return daCreate (name, version, id, type, host);
}

int
codaDaConnect (char* name, int version, int id,
	       char* type, char* host)
{
  return daConnect (name, version, id, type, host);
}

int
codaDaRemove (char* name)
{
  return daRemove (name);
}

int
codaDaDownload (char* name, char* paramL)
{
  return daDownload (name, paramL);
}

int
codaDaPrestart (char* name, int rn, int rt)
{
  return daPrestart (name, rn, rt);
}

int
codaDaEnd (char* name, int special)
{
  return daEnd (name, special);
}

int
codaDaPause (char* name, int mask)
{
  return daPause (name, mask);
}

int 
codaDaGo (char* name, int mask)
{
  return daGo (name, mask);
}

int
codaDaConfigure (char* name, int type)
{
  return daConfigure (name, type);
}

int
codaDaReset (char* name)
{
  return daReset (name);
}

int
codaDaTerminate (char* name)
{
  return daTerminate (name);
}

int
codaDaReport (char* name, int how)
{
  return daReport (name);
}

int
codaDaReadInt (char* name, char* attr, long* value)
{
  return daReadInt (name, attr, value);
}

int
codaDaWriteInt (char* name, char* attr, long* value)
{
  return daWriteInt (name, attr, value);
}

int
codaDaZap (char* name)
{
  return daZap (name);
}
#endif
