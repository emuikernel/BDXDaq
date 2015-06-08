/*
 *  all_rocs.c
 *
 *  code to get downloaded to all rocs
 *
 *  E.Wolin, 10-jul-97
 *
 */

#if defined(VXWORKS) || defined(Linux_vme)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef VXWORKS

#include <vxWorks.h> 
#include <bootLib.h>
#include <logLib.h>
#include <usrLib.h>
#include <ioLib.h>
#include <remLib.h>
#include <envLib.h>
#include <errnoLib.h>


/* local memory */
#define BOOTLEN 1024
static char boot_line[BOOTLEN];
static BOOT_PARAMS boot_params;
static char targetname[32];
static char targetnameupper[32];
static char ipaddress[32];


/* local memory */
static char clashostname[20];
static char clasrocname[20];
static char temp[1024];


/* external functions */
extern char *targetName();
extern STATUS sysNvRamGet();
extern void printErrno(int);
extern BOOT_PARAMS sysBootParams;

#endif

/*---------------------------------------------------------------*/
/*******************************************************************************
 *
 * vmeRead8         - Read a 8bit register
 * vmeWrite8        - Write to a 8bit register
 * vmeRead16         - Read a 16bit register
 * vmeWrite16        - Write to a 16bit register
 * vmeRead32         - Read a 32bit register
 * vmeWrite32        - Write to a 16bit register
 *
 */
/*
unsigned char vmeRead8(volatile unsigned char *addr);
void vmeWrite8(volatile unsigned char *addr, unsigned char val);
unsigned short vmeRead16(volatile unsigned short *addr);
void vmeWrite16(volatile unsigned short *addr, unsigned short val);
unsigned int vmeRead32(volatile unsigned int *addr);
void vmeWrite32(volatile unsigned int *addr, unsigned int val);
*/

/* define some Macros for byte swapping */
#define LSWAP(x)        ((((x) & 0x000000ff) << 24) | \
                         (((x) & 0x0000ff00) <<  8) | \
                         (((x) & 0x00ff0000) >>  8) | \
                         (((x) & 0xff000000) >> 24))

#define SSWAP(x)        ((((x) & 0x00ff) << 8) | \
                         (((x) & 0xff00) >> 8))


unsigned char
vmeRead8(volatile unsigned char *addr)
{
  unsigned char rval;
  rval = *addr;
  return rval;
}

void
vmeWrite8(volatile unsigned char *addr, unsigned char val)
{
  *addr = val;
  return;
}

unsigned short
vmeRead16(volatile unsigned short *addr)
{
  unsigned short rval;
  rval = *addr;
#ifndef VXWORKS
  rval = SSWAP(rval);
#endif
  return rval;
}

void
vmeWrite16(volatile unsigned short *addr, unsigned short val)
{
#ifndef VXWORKS
  val = SSWAP(val);
#endif
  *addr = val;
  return;
}

unsigned int
vmeRead32(volatile unsigned int *addr)
{
  unsigned int rval;
  rval = *addr;
#ifndef VXWORKS
  rval = LSWAP(rval);
#endif
  return rval;
}

void
vmeWrite32(volatile unsigned int *addr, unsigned int val)
{
#ifndef VXWORKS
  val = LSWAP(val);
#endif
  *addr = val;
  return;
}

/***********************************/
/***********************************/
/***********************************/

#ifdef VXWORKS

/* returns local target name and stores name in global variable targetname (lower case vsn)
 *  ejw, 10-jul-97 
 */
char *
targetName()
{
  sysNvRamGet(boot_line,BOOTLEN,0);
  bootStringToStruct(boot_line,&boot_params);
  strcpy(targetname,boot_params.targetName);  

  return((char*)targetname);
}


/*---------------------------------------------------------------*/


/* returns local target name and stores name in global variable targetnameupper (upper case vsn)
 *  ejw, 10-jul-97 
 */
char *
targetNameUpper()
{
  int i;

  /* get name in lower case */
  strcpy(targetnameupper,targetName());

  /* convert */
  for(i=0; i<strlen(targetnameupper); i++) {
    targetnameupper[i]=toupper(targetnameupper[i]);
  }

  return((char*)targetnameupper);
}


/*---------------------------------------------------------------*/


/* returns local ip address in char format (e.g. "123.45.67.89")
 *  ejw, 10-jul-97 
 */
char *
ipAddress()
{
  sysNvRamGet(boot_line,BOOTLEN,0);
  bootStringToStruct(boot_line,&boot_params);
  strcpy(ipaddress,boot_params.ead);  
  *(strchr(ipaddress,':'))='\0';

  return((char*)ipaddress);
}


/*---------------------------------------------------------------*/


/* returns clas roc name using target name in format "host ROC" */
/*  strips off suffix "-fe" from target name */
char *
clasHostName()
{
  char *p;

  strcpy(clashostname,targetName());
  if((p=strstr(clashostname,"-fe"))!=NULL)p[0]='\0';
  strcat(clashostname," ROC");

  return((char*)clashostname);
}


/*---------------------------------------------------------------*/


/* returns clas roc name using env var in format "host ROC" */
/*  strips off suffix "-fe" from target name */
char *
clasRocName()
{
  char *p;
  char tgt[20];

  strcpy(tgt,targetName());
  if((p=strstr(tgt,"-fe"))!=NULL)p[0]='\0';

  strcpy(clasrocname,getenv(tgt));
  strcat(clasrocname," ROC");

  return((char*)clasrocname);
}


/*---------------------------------------------------------------*/


/* returns clas roc name in format "host TS" */
char *
clasTsName()
{
  char *p;

  strcpy(clashostname,targetName());
  if((p=strstr(clashostname,"-fe"))!=NULL)p[0]='\0';
  strcat(clashostname," TS");

  return((char*)clashostname);
}


/*---------------------------------------------------------------*/


/* for testing (char*) variables and functions...ejw, 10-jul-97 */
void
echo(char *x)
{
  printf("%s\n",x);
}


/*---------------------------------------------------------------*/


/* dumps len words starting at addr to stdout in integer format...ejw, 23-may-97 */
void
vme_dumpi(int addr, int len)
{
  int i;
  int *p = (int *)addr;

  if(len<=0)return;
  for(i=0; i<len; i++) {
    printf("%d ",*(p+i));
  }
  printf("\n");
}


/*---------------------------------------------------------------*/


/* dumps len words starting at addr to stdout in hex format...ejw, 23-may-97 */
void
vme_dumpx(int addr, int len)
{
  int i;
  int *p = (int *)addr;

  if(len<=0)return;
  for(i=0; i<len; i++) {
    printf("%x ",*(p+i));
  }
  printf("\n");
}


/*---------------------------------------------------------------*/


/* dumps len shorts starting at addr to stdout in integer format...ejw, 10-jul-97 */
void
vme_dumpsi(int addr, int len)
{
  int i;
  short *p = (short *)addr;

  if(len<=0)return;
  for(i=0; i<len; i++) {
    printf("%d ",*(p+i));
  }
  printf("\n");
}


/*---------------------------------------------------------------*/


/* dumps len shorts starting at addr to stdout in hex format...ejw, 10-jul-97 */
void
vme_dumpsx(int addr, int len)
{
  int i;
  short *p = (short *)addr;

  if(len<=0)return;
  for(i=0; i<len; i++) {
    printf("%x ",*(p+i));
  }
  printf("\n");
}


/*---------------------------------------------------------------*/


/* loads len integer words from stdin into addr...ejw, 23-may-97 */
void
vme_loadi(int addr, int len)
{
  int i;
  int *p = (int *)addr;

  if(len<=0)return;
  for(i=0; i<len; i++) {
    scanf("%d",(p+i));
  }
}


/*---------------------------------------------------------------*/


/* loads len hex words from stdin into addr...ejw, 23-may-97 */
void
vme_loadx(int addr, int len)
{
  int i;
  int *p = (int *)addr;

  if(len<=0)return;
  for(i=0; i<len; i++) {
    scanf("%x",(p+i));
  }
}


/*---------------------------------------------------------------*/


int
find_tag_line(FILE *file, char *tag, char buffer[], int buflen)
{
  /* start at beginning of file */
  fseek(file,0,SEEK_SET);

  /* search for tag */
  for(;;) {
    if(fgets(buffer,buflen,file)==NULL)return(1);
    if(strncmp(buffer,tag,strlen(tag))==0)return(0);
  }
}


/*---------------------------------------------------------------*/


int
get_next_line(FILE *file, char buffer[], int buflen)
{
  for(;;) {
    if(fgets(buffer,buflen,file)==NULL)return(1);

    /* blank or whitespace only */
    if(strlen(buffer)==0)continue;
    if(strlen(buffer)==strspn(buffer," \t\n\r\b"))continue;

    /* found end of data */
    if(strncmp(buffer,"*",1)==0)return(1);

    /* comment */
    if(strncmp(buffer,"#",1)==0)continue;
    if(strncmp(buffer+strspn(buffer," \t\n\r\b"),"#",1)==0)continue;

    /* found a real data line */
    return(0);
  }
}


/*---------------------------------------------------------------*/


/*  not sure what this does...ejw */
int
calib_load(int *p, int *q, int *s, int *t, int *u)
{
  FILE *fp;
  fp = fopen("/home/clasrun/rol/calib.parms","r");
  fscanf(fp," %d  %d %d %d %d", p, q, s, t, u);
  fclose(fp);
  
  return(0);
}


/*---------------------------------------------------------------*/


/* sets env var from existing env and suffix...ejw, 4-may-2001 */
void
env_putenv(char *envname, char *envin, char *suffix)
{
  sprintf(temp,"%s=%s%s",envname,getenv(envin),suffix);
  putenv(temp);
  return;
}


/*---------------------------------------------------------------*/

/*
 *  envGetRemote.c  -  Retrieve Environment Variable from Boot host and 
 *                     set locally.
 *
 *  Author: David Abbott 
 *          Jefferson Lab Data Acquisition Group
 *          May 2001
 *
 *  Revision  1.0 - Initial Revision
 *
 *  Inputs:       envName  -   Environment variable name (required)
 *                remHost  -   Remote host to query (default - VxWorks boot host)
 *                remUser  -   Remote user (default - VxWorks boot user)
 *
 *  Returns:    OK or ERROR if env variable not set
 */


/* Max Environment variable length (bytes) */ 
#define EGR_MAX_ENV_LENGTH     1024

/* Default port for cmd deamon on remote host (/etc/services) */
#define EGR_HOST_DEFAULT_PORT   514


STATUS
envGetRemote(char *envName, char *remHost, char *remUser)
{
  int rfd, stat;
  int envlen, nbytes = 0;

  char rhost[BOOT_HOST_LEN], ruser[BOOT_USR_LEN];
  char rstring[128];
  char res[EGR_MAX_ENV_LENGTH];
  char envS[EGR_MAX_ENV_LENGTH];

  /* Get remote host info */
  if(remHost == NULL)
  {
    strcpy(rhost,sysBootParams.hostName);
    /*printf("envGetRemote1: rhost >%s<\n",rhost);*/
  }
  else
  {
    strcpy(rhost,remHost);
    /*printf("envGetRemote2: rhost >%s<\n",rhost);*/
  }

  /* Get remote user info */
  if(remUser == NULL)
  {
    strcpy(ruser,sysBootParams.usr);
    /*printf("envGetRemote3: ruser >%s<\n",ruser);*/
  }
  else
  {
    strcpy(ruser,remUser);
    /*printf("envGetRemote4: ruser >%s<\n",ruser);*/
  }

  /* Construct the remote command */
  sprintf(rstring,"echo $%s",envName);
  envlen = strlen(envName);
  /* Set local user to the same as remote user */
  iam(ruser,NULL);
  printf("envGetRemote: host >%s<  user >%s<  cmd >%s<\n",rhost,ruser,rstring);

  /* Issue the remote command */
  rfd = rcmd(rhost,EGR_HOST_DEFAULT_PORT,ruser,ruser,rstring,0);

  if(rfd > 0) /* Read the result */
  {
    nbytes = read(rfd,res,EGR_MAX_ENV_LENGTH);

    /* Make sure result string is not too long */
    if(nbytes >= (EGR_MAX_ENV_LENGTH-envlen))
    {
      printf("envGetRemote: ERROR: env result string too long\n");
      close(rfd);
      return(ERROR);
    }
    else
    {
      res[nbytes-1] = 0;  /* remove newline */
      res[nbytes] = 0;
    }

    /* Check for undefined variable */
    if(strstr(res,"Undefined") != NULL)
    {
      printf("%s\n",res);
      close(rfd);
      return(ERROR);
    }

    /* Create string for putenv and set */
    sprintf(envS,"%s=%s",envName,res); 
    stat = putenv(envS);
    if(stat)
    {
      printf("envGetRemote: ERROR in putenv()\n");
      close(rfd);
      return(ERROR);
    }
    else
    {
      printf("envGetRemote: set >%s< to >%s<\n",envName,res);
    }

  }
  else  /* Print the error */
  {
    printf("envGetRemote: ERROR: ");
    printErrno(errno);
    return(ERROR);
  }

  /* Close file descriptor */
  close(rfd);

  return(OK);
}

STATUS
getFromHost(char *rstring, char *remHost, char *remUser, char output[EGR_MAX_ENV_LENGTH])
{
  int rfd, stat;
  int envlen, nbytes = 0;

  char rhost[BOOT_HOST_LEN], ruser[BOOT_USR_LEN];
  char res[EGR_MAX_ENV_LENGTH];
  char envS[EGR_MAX_ENV_LENGTH];

  /* Get remote host info */
  if(remHost == NULL)
  {
    strcpy(rhost,sysBootParams.hostName);
    /*printf("getFromHost1: rhost >%s<\n",rhost);*/
  }
  else
  {
    strcpy(rhost,remHost);
    /*printf("getFromHost2: rhost >%s<\n",rhost);*/
  }

  /* Get remote user info */
  if(remUser == NULL)
  {
    strcpy(ruser,sysBootParams.usr);
    /*printf("getFromHost3: ruser >%s<\n",ruser);*/
  }
  else
  {
    strcpy(ruser,remUser);
    /*printf("getFromHost4: ruser >%s<\n",ruser);*/
  }


  envlen = strlen(rstring);
  /* Set local user to the same as remote user */
  iam(ruser,NULL);
  /*printf("getFromHost: host >%s<  user >%s<  cmd >%s<\n",rhost,ruser,rstring);*/


  /* Issue the remote command */
  rfd = rcmd(rhost,EGR_HOST_DEFAULT_PORT,ruser,ruser,rstring,0);

  if(rfd > 0) /* Read the result */
  {
    nbytes = read(rfd,res,EGR_MAX_ENV_LENGTH);

    /* Make sure result string is not too long */
    if(nbytes >= (EGR_MAX_ENV_LENGTH-envlen))
    {
      printf("getFromHost: ERROR: env result string too long\n");
      close(rfd);
      return(ERROR);
    }
    else
    {
      res[nbytes-1] = 0;  /* remove newline */
      res[nbytes] = 0;
    }

    /* Check for undefined variable */
    if(strstr(res,"Undefined") != NULL)
    {
      printf("%s\n",res);
      close(rfd);
      return(ERROR);
    }

    /* Create output string */
    sprintf(output,"%s",res);
    /*printf("output >%s<",output);*/

  }
  else  /* Print the error */
  {
    printf("getFromHost: ERROR: ");
    printErrno(errno);
    return(ERROR);
  }

  /* Close file descriptor */
  close(rfd);

  return(OK);
}

#endif /*VXWORKS*/

/*---------------------------------------------------------------*/

#else /* no UNIX version */

void
all_rocs_dummy()
{
  return;
}

#endif
