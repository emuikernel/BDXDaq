h59999
s 00050/00026/00375
d D 1.3 02/03/26 15:16:10 boiarino 4 3
c *** empty log message ***
e
s 00231/00022/00170
d D 1.2 02/02/13 18:41:24 boiarino 3 1
c new version - ready to be tested
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 18:54:36 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/l3.s/l3lib.c
e
s 00192/00000/00000
d D 1.1 01/11/19 18:54:35 boiarino 1 0
c date and time created 01/11/19 18:54:35 by boiarino
e
u
U
f e 0
t
T
I 1

/************************************************

  l3lib.c - level 3 (software) trigger library

  BUG report: boiarino@jlab.org

*************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "eclib.h"
#include "prlib.h"
#include "sdakeys.h"
SDAkeys sdakeys_;

#include "bos.h"

#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))
I 3
#define MAXSTRLEN   256
E 3


/*****************************************************/
/***************** START OF USER CODE ****************/
/*****************************************************/
/* these functions suppose to be called from coda_l3 */
/*****************************************************/

D 3
#define SIGNATURE 0x02000000
E 3
I 3
#define PRESCALE 0x80000000
I 4
#define ACCEPT (candsum&0x003F3F3F) && (!(candsum&0x00808080))
E 4
E 3

D 4
int nevents   = 0;
E 4
I 4
int nevents1  = 0;
int nevents2  = 0;
E 4
int nremove1  = 0;
int nremove2  = 0;
int nremove3  = 0;
int nelect[6] = {0,0,0,0,0,0};
int nwrong[6] = {0,0,0,0,0,0};
int nmiss[6]  = {0,0,0,0,0,0};
int secbit[7];

void
l3_reset()
{
  int i;

D 4
  nevents=0;
E 4
I 4
  nevents1=0;
  nevents2=0;
E 4
  nremove1=0;
  nremove2=0;
  nremove3=0;
  for(i=0; i<6; i++) nelect[i]=0;

  return;
}

void
l3_init(int runno)
{
  int i;

  for (i=0; i<6; i++) secbit[i]=1<<i;
I 3
D 4
  l3config(".");
E 4
I 4
  l3config("");
E 4
E 3
  i = l3_init1(runno);
  i = l3_init2(runno);
  i = l3_init3(runno);

  return;
}

I 3

/*
   input params:  jw - BOS array pointer
D 4
   output params: keepit =1 if event must be put into ET
                         =0 if event must be dumped into ET (recycled)
E 4
I 4
   output params: *keepit =1 if event must be put into ET
                          =0 if event must be dumped into ET (recycled)
E 4
   returns trigger mask
*/

E 3
int
D 3
l3_event(int *jw)
E 3
I 3
D 4
l3_event(int *jw, int keepit)
E 4
I 4
l3_event(int *jw, int *keepit)
E 4
E 3
{
D 4
  int i, cand, nsec, candsum;
E 4
I 4
  int i, ind, cand, candsum;
E 4


D 3
  cand = l3_event1(jw);
  candsum = cand;
  if(!cand) nremove1++;
E 3
I 3
D 4
  keepit = 1; /* accept all events by default */
E 4
I 4
  /* accept all events by default */

  *keepit = 1;
E 4
  candsum = 0;
E 3


I 4
  /* accept physics events only */

  if((ind = etNlink(jw,"HEAD",0)) <= 0) return(candsum);
  /*printf("event no. %d\n",jw[ind+2]);*/
  if(jw[ind+4] >= 10) return(candsum); /* not physics event */


E 4
D 3
  cand = l3_event2(jw);
  if(!(cand&0x80))
E 3
I 3
  /* process selected alghorithms (maximum 3) */

  if(sdakeys_.l3a1)
E 3
  {
D 3
    for(i=0; i<6; i++)
E 3
I 3
    cand = l3_event1(jw);
    if(cand&0x80) nremove1++;
    candsum = cand;
  }

  if(sdakeys_.l3a2)
  {
    cand = l3_event2(jw);
    if(cand&0x80)
E 3
    {
D 3
  	  if(cand & secbit[i])
E 3
I 3
      nremove2++;
    }
    else
    {
      for(i=0; i<6; i++)
E 3
      {
D 3
        nelect[i]++;
E 3
I 3
    	  if(cand & secbit[i])
        {
          nelect[i]++;
        }
E 3
      }
    }
I 3
    candsum += (cand<<8);
E 3
  }
D 3
  else
E 3
I 3

  if(sdakeys_.l3a3)
E 3
  {
D 3
    nremove2++;
E 3
I 3
    cand = l3_event3(jw);
    if(cand&0x80) nremove3++;
    candsum += (cand<<16);
E 3
  }
D 3
  candsum += (cand<<8);
E 3


D 3
  cand = l3_event3(jw);
  if(!cand) nremove3++;
  candsum += (cand<<16);
E 3
I 3
  /* prescale if trigger mode */
/* COUNT EVENTS HERE OR USE EVENT NUMBER FROM 'HEAD' BANK ??? */
E 3

I 3
  if(sdakeys_.l3mode == *((int *)"TRIG"))
  {
D 4
    if(!((++nevents) % sdakeys_.l3prescale))
E 4
I 4
    if(!((++nevents1) % sdakeys_.l3prescale))
E 4
    {
      candsum += PRESCALE;
D 4
      keepit = 1;
E 4
I 4
      /*printf("l3_event: 0x%08x - prescaled\n",candsum);*/
      *keepit = 1;
E 4
    }
    else
    {
D 4
      if(!candsum) /* ?????????????????????????? */
E 4
I 4
      if(ACCEPT)
E 4
      {
D 4
        keepit = 0;
E 4
I 4
        /*printf("l3_event: 0x%08x - accepted\n",candsum);*/
        *keepit = 1;
E 4
      }
      else
      {
D 4
        keepit = 1;
E 4
I 4
        /*printf("l3_event: 0x%08x - rejected\n",candsum);*/
        *keepit = 0;
E 4
      }
    }
    /* add prescale factor to the 'candsum' */
    candsum += (sdakeys_.l3prescale << 24);
  }
E 3

D 3
  if(!((++nevents)%100000))
E 3
I 3

  /* print statistic */

D 4
  if(!((++nevents) % 100000))
E 4
I 4
  if(!((++nevents2) % 100000))
E 4
E 3
  {
	/*
D 3
    printf("\nLevel3 report: version %3d, events processed %7d\n",(SIGNATURE>>24)&0xff,nevents);
E 3
I 3
D 4
    printf("\nLevel3 report: events processed %7d\n",nevents);
E 4
I 4
    printf("\nLevel3 report: events processed %7d\n",nevents2);
E 4
E 3
    printf("   electron efficiency (3 algorithms): %4.1f%% %4.1f%% %4.1f%%\n",
D 4
           ((float)(nevents-nremove1)/(float)nevents)*100.,
           ((float)(nevents-nremove2)/(float)nevents)*100.,
           ((float)(nevents-nremove3)/(float)nevents)*100.);
E 4
I 4
           ((float)(nevents2-nremove1)/(float)nevents2)*100.,
           ((float)(nevents2-nremove2)/(float)nevents2)*100.,
           ((float)(nevents2-nremove3)/(float)nevents2)*100.);
E 4
    printf("   per sector (second algorithm): %6d %6d %6d %6d %6d %6d\n",
           nelect[0],nelect[1],nelect[2],nelect[3],nelect[4],nelect[5]);
    */
  }


D 3
/*printf("==>0x%08x\n",candsum + SIGNATURE);*/
  return(candsum + SIGNATURE);
E 3
I 3
D 4
  printf("==>0x%08x\n",candsum);
E 4
I 4
  /*printf("==>0x%08x\n",candsum);*/
E 4
  return(candsum);
E 3
}


/*************************************************/
/**************** END OF USER CODE ***************/
/*************************************************/


I 3

/*
  l3config() - reads l3lib configuration file 'l3lib.config'
     dir contains the name of the directory where file is located;
     if dir = "" file assume in $CLON_PARMS/cmon/l3lib/
 */

void
l3config(char *dir)
{
  FILE *fd;
  char *parm, *sss, tmp[MAXSTRLEN], fname[MAXSTRLEN];
  char key[7], str[100];
  int i, j, len;

  printf("l3config: dir>%s< len=%d\n",dir,strlen(dir));

  len = strlen(dir);
  strncpy(tmp,dir,MAXSTRLEN);
  sss = tmp;
  i = 0;
  while(*sss++ == ' ' && i<len) i++; /* skip spaces in the begining */
  if(i==len)
  {
    printf("l3config: i=%d len=%d\n",i,len);
    printf("l3config: use default directory in CLON_PARMS area\n");
    if((parm = getenv("CLON_PARMS")) == NULL)
    {
      printf("l3config: env. variable CLON_PARMS is not set - exit.\n");
      exit(0);
    }
    sprintf(fname,"%s/cmon/l3lib/l3lib.config",parm);
  }
  else
  {
    sss--;
    printf("l3config 2: sss>%s< len=%d\n",sss,strlen(sss));
    len = strlen(sss);
    while(sss[len-1]==' ') len--; /* remove trailing spaces */
    if(sss[len-1]=='/')
    {
      sss[len] = '\0';
    }
    else
    {
      sss[len++] = '/';
      sss[len] = '\0';
    }
    printf("l3config: sss>%s< len=%d\n",sss,strlen(sss));
    printf("l3config: looking in directory >%s<\n",sss);
    sprintf(fname,"%sl3lib.config",sss);
  }

  printf("l3config: using config file >%s<\n",fname);
  if((fd = fopen(fname,"r")) == NULL)
  {
    printf("l3config: config file >%s< does not exist - exit.\n",fname);
    exit(0);
  }
  else
  {
    fclose(fd);
  }


  /* Some defaults */

  sdakeys_.l3mode = *((int *)"TAGG");
  sdakeys_.l3prescale = 1;
  sdakeys_.l3a1 = 0;
  sdakeys_.l3a2 = 0;
  sdakeys_.l3a3 = 0;
  for(i=0; i<10; i++)
  {
    sdakeys_.l3p1[i] = 0.0;
    sdakeys_.l3p2[i] = 0.0;
    sdakeys_.l3p3[i] = 0.0;
  }


  /* read cards */

  if(fd = fopen(fname,"r"))
  {
    do
    {
      if(!fgets(str,99,fd)) break;
      if(str[0] == '#') continue; /* skip comment line */
      if(!strncmp(str,"STOP",4)) break;
      printf("%s",str);

      if(!strncmp(str,"L3MD",4))
      {
        {
          sscanf(&str[4],"%s",key);
          for(i=3; i>0; i--) if(key[i] == '\0') key[i] = ' ';
        }
        strncpy((char *)&sdakeys_.l3mode,"    ",4);
        strncpy((char *)&sdakeys_.l3mode,&key[0],4);
        printf("l3config: l3mode>%4.4s<\n",(char *)&sdakeys_.l3mode);
      }
      else if(!strncmp(str,"L3PR",4))
      {
        sscanf(&str[4],"%d",&sdakeys_.l3prescale);
        printf("l3config: l3prescale=%d\n",sdakeys_.l3prescale);
      }
      else if(!strncmp(str,"L3P1",4))
      {
        sscanf(&str[4],"%i %f %f %f %f %f %f %f %f %f %f",
               &sdakeys_.l3a1,&sdakeys_.l3p1[0],
               &sdakeys_.l3p1[1],&sdakeys_.l3p1[2],&sdakeys_.l3p1[3],
               &sdakeys_.l3p1[4],&sdakeys_.l3p1[5],&sdakeys_.l3p1[6],
               &sdakeys_.l3p1[7],&sdakeys_.l3p1[8],&sdakeys_.l3p1[9]);
        printf("l3config: L3P1: %i\n",sdakeys_.l3a1);
      }
      else if(!strncmp(str,"L3P2",4))
      {
        sscanf(&str[4],"%i %f %f %f %f %f %f %f %f %f %f",
               &sdakeys_.l3a2,&sdakeys_.l3p2[0],
               &sdakeys_.l3p2[1],&sdakeys_.l3p2[2],&sdakeys_.l3p2[3],
               &sdakeys_.l3p2[4],&sdakeys_.l3p2[5],&sdakeys_.l3p2[6],
               &sdakeys_.l3p2[7],&sdakeys_.l3p2[8],&sdakeys_.l3p2[9]);
        printf("l3config: L3P2: %i %f %f %f %f\n",
          sdakeys_.l3a2,sdakeys_.l3p2[0],sdakeys_.l3p2[1],
          sdakeys_.l3p2[2],sdakeys_.l3p2[3]);
      }
      else if(!strncmp(str,"L3P3",4))
      {
        sscanf(&str[4],"%i %f %f %f %f %f %f %f %f %f %f",
               &sdakeys_.l3a3,&sdakeys_.l3p3[0],
               &sdakeys_.l3p3[1],&sdakeys_.l3p3[2],&sdakeys_.l3p3[3],
               &sdakeys_.l3p3[4],&sdakeys_.l3p3[5],&sdakeys_.l3p3[6],
               &sdakeys_.l3p3[7],&sdakeys_.l3p3[8],&sdakeys_.l3p3[9]);
        printf("l3config: L3P3: %i\n",sdakeys_.l3a3);
      }
      else
      {
        printf("unknown key - exit.\n"); /* ignore comment lines and unknown keys */
        exit(0);
      }

    } while(1 == 1);

    fclose(fd);
  }
  else
  {
    printf("l3config: error opening config file - exit\n");
    exit(0);
  }

  return;
}


/* initialization */

E 3
D 4
int
l3init(int run)
E 4
I 4
void
l3init(int runno)
E 4
{
D 4
  l3_init(run);
  return(0);
E 4
I 4
  int i;

  if(sdakeys_.lanal[9] == 0 || sdakeys_.lanal[9] == 1) return; /* no Level 3 */

  for (i=0; i<6; i++) secbit[i]=1<<i;
  l3config(".");
  i = l3_init1(runno);
  i = l3_init2(runno);
  i = l3_init3(runno);

  return;
E 4
}

I 3

E 3
/*
 opt = 0 - no L3
 opt = 1 - use incoming L3 info from TGBI bank
 opt = 2 - call online L3 algorithm here

  returns 1 if good event, 0 if bad
*/

int
l3lib(int *jw, int *ifail)
{
D 3
  int ind, cand, ret, opt;
E 3
I 3
  int ind, cand, ret, opt, keepit;
E 3

  opt = sdakeys_.lanal[9]; /* get option from 'config' file */

  ret = 0;
  switch(opt)
  {
    case 0: /* consider all events as good */
      ret = 1;
      break;
    
    case 1: /* search for L3 info in TGBI bank */
      if( (ind=etNlink(jw,"TGBI",0)) > 0)
      {
        if(etNcol(jw,ind) >= 5)
        {
          if(jw[ind+4] & 0x00003F00) ret = 1;
        }
        else
        {
          ret = 1; /* consider event as good if short TGBI */
        }
	  }
      else
      {
        ret = 1; /* consider event as good if no TGBI */
      }
      break;

    case 2: /* execute L3 algorithm here */
D 3
      cand = l3_event(jw);
E 3
I 3
D 4
      cand = l3_event(jw, keepit);
E 4
I 4
      cand = l3_event(jw, &keepit);
E 4
E 3
      /* second algorithm (Alex: if bit7=1 - bad event)
      if(!(cand&0x00008000)) ret = 1;*/
      /* third algorithm (mine: if bits0-5=0 - bad event) */
D 4
      if(cand&0x003F0000) ret = 1;
      printf("===> %08x -> %1d\n",cand,ret);
E 4
I 4
      ret = keepit;
      /*printf("l3lib: keep=%d cand=%08x -> %1d\n",keepit,cand,ret);*/
E 4
      break;
        
    default:
      printf ("l3lib: ERROR: unknown option = %d\n",opt);
  }
  /*bosNprint(jw,"TGBI",0);*/

  *ifail = ret;
  return(ret);
}


E 1
