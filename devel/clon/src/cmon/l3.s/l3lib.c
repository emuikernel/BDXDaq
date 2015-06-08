
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

/*#include "bos.h"*/

#define MAX(a,b)    (a > b ? a : b)
#define MIN(a,b)    (a < b ? a : b)
#define ABS(x)      ((x) < 0 ? -(x) : (x))
#define MAXSTRLEN   256


/*****************************************************/
/***************** START OF USER CODE ****************/
/*****************************************************/
/* these functions suppose to be called from coda_l3 */
/*****************************************************/

#define PRESCALE 0x80000000
#define ACCEPT (candsum&0x003F3F3F) && (!(candsum&0x00808080))

int nevents1  = 0;
int nevents2  = 0;
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

  nevents1=0;
  nevents2=0;
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
  l3config("");
  i = l3_init1(runno);
  i = l3_init2(runno);
  i = l3_init3(runno);

  return;
}


/*
   input params:  jw - BOS array pointer
   output params: *keepit =1 if event must be put into ET
                          =0 if event must be dumped into ET (recycled)
   returns trigger mask
*/

int
l3_event(int *jw, int *keepit)
{
  int i, ind, cand, candsum;


  /* accept all events by default */

  *keepit = 1;
  candsum = 0;

return(0xffff);


  /* accept physics events only */

  if((ind = etNlink(jw,"HEAD",0)) <= 0) return(candsum);
  /*printf("event no. %d\n",jw[ind+2]);*/
  if(jw[ind+4] >= 10) return(candsum); /* not physics event */


  /* process selected alghorithms (maximum 3) */

  /*
  if(sdakeys_.l3a1)
  {
    cand = l3_event1(jw);
    if(cand&0x80) nremove1++;
    candsum = cand;
  }

  if(sdakeys_.l3a2)
  {
    cand = l3_event2(jw);
    if(cand&0x80)
    {
      nremove2++;
    }
    else
    {
      for(i=0; i<6; i++)
      {
    	  if(cand & secbit[i])
        {
          nelect[i]++;
        }
      }
    }
    candsum += (cand<<8);
  }
  */

  if(sdakeys_.l3a3)
  {
    cand = l3_event3(jw);
    if(cand&0x80) nremove3++;

/* Sergey: put it in first trigger place */
/*candsum += (cand<<16);*/

/* Sergey: ignore others*/
candsum = cand;

  }


  /* prescale if trigger mode */
/* COUNT EVENTS HERE OR USE EVENT NUMBER FROM 'HEAD' BANK ??? */

  if(sdakeys_.l3mode == *((int *)"TRIG"))
  {
    if(!((++nevents1) % sdakeys_.l3prescale))
    {
      candsum |= PRESCALE;
      /*printf("l3_event: 0x%08x - prescaled\n",candsum);*/
      *keepit = 1;
    }
    else
    {
      if(ACCEPT)
      {
        /*printf("l3_event: 0x%08x - accepted\n",candsum);*/
        *keepit = 1;
      }
      else
      {
        /*printf("l3_event: 0x%08x - rejected\n",candsum);*/
        *keepit = 0;
      }
    }
    /* add prescale factor to the 'candsum' */
    candsum += (sdakeys_.l3prescale << 24);
  }


  /* print statistic */

  if(!((++nevents2) % 100000))
  {
	/*
    printf("\nLevel3 report: events processed %7d\n",nevents2);
    printf("   electron efficiency (3 algorithms): %4.1f%% %4.1f%% %4.1f%%\n",
           ((float)(nevents2-nremove1)/(float)nevents2)*100.,
           ((float)(nevents2-nremove2)/(float)nevents2)*100.,
           ((float)(nevents2-nremove3)/(float)nevents2)*100.);
    printf("   per sector (second algorithm): %6d %6d %6d %6d %6d %6d\n",
           nelect[0],nelect[1],nelect[2],nelect[3],nelect[4],nelect[5]);
    */
  }


  /*printf("==>0x%08x\n",candsum);*/
  return(candsum);
}


/*************************************************/
/**************** END OF USER CODE ***************/
/*************************************************/



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

void
l3init(int runno)
{
  int i;

  if(sdakeys_.lanal[9] == 0 || sdakeys_.lanal[9] == 1) return; /* no Level 3 */

  for (i=0; i<6; i++) secbit[i]=1<<i;
  l3config(".");
  i = l3_init1(runno);
  i = l3_init2(runno);
  i = l3_init3(runno);

  return;
}


/*
 opt = 0 - no L3
 opt = 1 - use incoming L3 info from TGBI bank
 opt = 2 - call online L3 algorithm here

  returns 1 if good event, 0 if bad
*/

int
l3lib(int *jw, int *ifail)
{
  int ind, cand, ret, opt, keepit;

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
      cand = l3_event(jw, &keepit);
      /* second algorithm (Alex: if bit7=1 - bad event)
      if(!(cand&0x00008000)) ret = 1;*/
      /* third algorithm (mine: if bits0-5=0 - bad event) */
      ret = keepit;
      /*printf("l3lib: keep=%d cand=%08x -> %1d\n",keepit,cand,ret);*/
      break;
        
    default:
      printf ("l3lib: ERROR: unknown option = %d\n",opt);
  }
  /*bosNprint(jw,"TGBI",0);*/

  *ifail = ret;
  return(ret);
}


