
/* trconfig.c - reads configuration file */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "dclib.h"
#include "sdakeys.h"
SDAkeys sdakeys_;

void
trgetconfig_(SDAkeys *keys)
{
  trgetconfig(keys);
}

void
trgetconfig(SDAkeys *keys)
{
  memcpy((char *)keys, (char *)&sdakeys_, sizeof(SDAkeys));
  return;
}

void
trconfig_()
{
 trconfig();
}

void
trconfig()
{
  FILE *fd;
  char *parm, fname[256];
  char key[7], key1[7], key2[7], str[100];
  int i, j;


  /* Some defaults */

  for(i=0; i<10; i++)
  {
    sdakeys_.isw[i]   = 0;
    sdakeys_.lprnt[i] = 0;
  }
  for(i=0; i<36; i++)
  {
    sdakeys_.lout[i] = 0;
  }
  sdakeys_.zkine[0] = 0.;
  sdakeys_.zgrid[0] = 0.;


  /* get params directory */

  if((parm = getenv("CLON_PARMS")) == NULL)
  {
    printf("trconfig: environment variable CLON_PARMS is not set - exit.\n");
    exit(0);
  }


  /* config file */

  if((fd = fopen("cmon.config","r")) == NULL)
  {
    sprintf(fname,"%s/cmon/cmon.config",parm);
  }
  else
  {
    fclose(fd);
    sprintf(fname,"%s","./cmon.config");
  }
  printf("trconfig: config file >%s<\n",fname);


  /* read cards */

  if(fd = fopen(fname,"r"))
  {
    do
    {
      if(!fgets(str,99,fd)) break;
      if(str[0] == 'C' && !isalpha(str[1])) continue;
      if(!strncmp(str,"STOP",4)) break;
      printf("%s",str);

      if(!strncmp(str,"MODE",4))
      {
        {
          sscanf(&str[4],"%s",key);
          key[0]=key[1]; key[1]=key[2]; key[2]=key[3]; key[3]=key[4];
          for(i=3; i>0; i--) if(key[i] == '\0') key[i] = ' ';
        }
        strncpy((char *)&sdakeys_.lmode,"    ",4);
        strncpy((char *)&sdakeys_.lmode,&key[0],4);
      }
      else if(!strncmp(str,"REVB",4))
      {
        sscanf(&str[4],"%s",sdakeys_.inputfile);
        printf("    input file >%s<\n",sdakeys_.inputfile);
      }
      else if(!strncmp(str,"WDST",4))
      {
        sscanf(&str[4],"%s",sdakeys_.outputfile);
        printf("    output file >%s<\n",sdakeys_.outputfile);
      }
      else if(!strncmp(str,"GBOS",4))
      {
        sscanf(&str[4],"%s",sdakeys_.geomfile);
        printf("    geometry file >%s<\n",sdakeys_.geomfile);
      }
      else if(!strncmp(str,"DICT",4))
      {
        sscanf(&str[4],"%s",sdakeys_.dictfile);
        printf("    dictionary file >%s<\n",sdakeys_.dictfile);
      }
      else if(!strncmp(str,"BTOR",4))
      {
        sscanf(&str[4],"%s",sdakeys_.btorfile);
        printf("    torus field file >%s<\n",sdakeys_.btorfile);
      }
      else if(!strncmp(str,"BMIN",4))
      {
        sscanf(&str[4],"%s",sdakeys_.bminfile);
        printf("    minitorus field >%s<\n",sdakeys_.bminfile);
      }
      else if(!strncmp(str,"BPOL",4))
      {
        sscanf(&str[4],"%s",sdakeys_.bpolfile);
        printf("    pol. target field >%s<\n",sdakeys_.bpolfile);
      }
      else if(!strncmp(str,"CBOS",4))
      {
        sscanf(&str[4],"%s",sdakeys_.cbosfile);
        printf("    calibration file >%s<\n",sdakeys_.cbosfile);
      }
      else if(!strncmp(str,"DEBU",4))
      {
        for(i=0; i<10; i++) sdakeys_.debu[i][0] = '\0';
        sscanf(&str[4],"%s %s %s %s %s %s %s %s %s %s",&sdakeys_.debu[0][0],
               &sdakeys_.debu[1][0],&sdakeys_.debu[2][0],&sdakeys_.debu[3][0],
               &sdakeys_.debu[4][0],&sdakeys_.debu[5][0],&sdakeys_.debu[6][0],
               &sdakeys_.debu[7][0],&sdakeys_.debu[8][0],&sdakeys_.debu[9][0]);
        sdakeys_.ndebu = 0;
        for(i=0; i<10; i++)
        {
          if(sdakeys_.debu[i][0] != '\0') sdakeys_.ndebu ++;
          else break;
          printf("    debu[%d] >%s<\n",i,(char *)&sdakeys_.debu[i][0]);
        }
        printf("  ndebu=%d\n",sdakeys_.ndebu);
      }
      else if(!strncmp(str,"TRIG",4))
      {
        sscanf(&str[4],"%d %d %d %d %d %d %d %d %d %d",&sdakeys_.ltrig[0],
               &sdakeys_.ltrig[1],&sdakeys_.ltrig[2],&sdakeys_.ltrig[3],
               &sdakeys_.ltrig[4],&sdakeys_.ltrig[5],&sdakeys_.ltrig[6],
               &sdakeys_.ltrig[7],&sdakeys_.ltrig[8],&sdakeys_.ltrig[9]);
      }
      else if(!strncmp(str,"ANAL",4))
      {
        sscanf(&str[4],"%d %d %d %d %d %d %d %d %d %d",&sdakeys_.lanal[0],
               &sdakeys_.lanal[1],&sdakeys_.lanal[2],&sdakeys_.lanal[3],
               &sdakeys_.lanal[4],&sdakeys_.lanal[5],&sdakeys_.lanal[6],
               &sdakeys_.lanal[7],&sdakeys_.lanal[8],&sdakeys_.lanal[9]);
      }
      else if(!strncmp(str,"PRNT",4))
      {
        sscanf(&str[4],"%d %d %d %d %d %d %d %d %d %d",&sdakeys_.lprnt[0],
               &sdakeys_.lprnt[1],&sdakeys_.lprnt[2],&sdakeys_.lprnt[3],
               &sdakeys_.lprnt[4],&sdakeys_.lprnt[5],&sdakeys_.lprnt[6],
               &sdakeys_.lprnt[7],&sdakeys_.lprnt[8],&sdakeys_.lprnt[9]);
      }
      else if(!strncmp(str,"SWIT",4))
      {
        sscanf(&str[4],"%d %d %d %d %d %d %d %d %d %d",&sdakeys_.isw[0],
               &sdakeys_.isw[1],&sdakeys_.isw[2],&sdakeys_.isw[3],
               &sdakeys_.isw[4],&sdakeys_.isw[5],&sdakeys_.isw[6],
               &sdakeys_.isw[7],&sdakeys_.isw[8],&sdakeys_.isw[9]);
      }
      else if(!strncmp(str,"TIME",4))
      {
        sscanf(&str[4],"%f %f %f %f %f %f %f %f %f %f",&sdakeys_.time[0],
               &sdakeys_.time[1],&sdakeys_.time[2],&sdakeys_.time[3],
               &sdakeys_.time[4],&sdakeys_.time[5],&sdakeys_.time[6],
               &sdakeys_.time[7],&sdakeys_.time[8],&sdakeys_.time[9]);
      }
      else if(!strncmp(str,"RNDM",4))
      {
        sscanf(&str[4],"%d",&sdakeys_.iseed);
      }
      else if(!strncmp(str,"GRID",4))
      {
        sscanf(&str[4],"%f %f %f %f %f %f %f %f %f %f",&sdakeys_.zgrid[0],
               &sdakeys_.zgrid[1],&sdakeys_.zgrid[2],&sdakeys_.zgrid[3],
               &sdakeys_.zgrid[4],&sdakeys_.zgrid[5],&sdakeys_.zgrid[6],
               &sdakeys_.zgrid[7],&sdakeys_.zgrid[8],&sdakeys_.zgrid[9]);
      }
      else if(!strncmp(str,"TARG",4))
      {
        sscanf(&str[4],"%f %f %f %f %f %f %f %f %f %f",&sdakeys_.ztarg[0],
               &sdakeys_.ztarg[1],&sdakeys_.ztarg[2],&sdakeys_.ztarg[3],
               &sdakeys_.ztarg[4],&sdakeys_.ztarg[5],&sdakeys_.ztarg[6],
               &sdakeys_.ztarg[7],&sdakeys_.ztarg[8],&sdakeys_.ztarg[9]);
      }
      else if(!strncmp(str,"BEAM",4))
      {
        sscanf(&str[4],"%f %f %f %f %f %f %f %f %f %f",&sdakeys_.zbeam[0],
               &sdakeys_.zbeam[1],&sdakeys_.zbeam[2],&sdakeys_.zbeam[3],
               &sdakeys_.zbeam[4],&sdakeys_.zbeam[5],&sdakeys_.zbeam[6],
               &sdakeys_.zbeam[7],&sdakeys_.zbeam[8],&sdakeys_.zbeam[9]);
      }
      else if(!strncmp(str,"STEP",4))
      {
        sscanf(&str[4],"%f %f %f %f %f %f %f %f %f %f",&sdakeys_.zstep[0],
               &sdakeys_.zstep[1],&sdakeys_.zstep[2],&sdakeys_.zstep[3],
               &sdakeys_.zstep[4],&sdakeys_.zstep[5],&sdakeys_.zstep[6],
               &sdakeys_.zstep[7],&sdakeys_.zstep[8],&sdakeys_.zstep[9]);
      }
      else if(!strncmp(str,"KINE",4))
      {
        sscanf(&str[4],"%f %f %f %f %f %f %f %f %f %f",&sdakeys_.zkine[0],
               &sdakeys_.zkine[1],&sdakeys_.zkine[2],&sdakeys_.zkine[3],
               &sdakeys_.zkine[4],&sdakeys_.zkine[5],&sdakeys_.zkine[6],
               &sdakeys_.zkine[7],&sdakeys_.zkine[8],&sdakeys_.zkine[9]);
      }
      else if(!strncmp(str,"DERR",4))
      {
        sscanf(&str[4],"%f %f %f %f %f %f %f %f %f %f",&sdakeys_.zderr[0],
               &sdakeys_.zderr[1],&sdakeys_.zderr[2],&sdakeys_.zderr[3],
               &sdakeys_.zderr[4],&sdakeys_.zderr[5],&sdakeys_.zderr[6],
               &sdakeys_.zderr[7],&sdakeys_.zderr[8],&sdakeys_.zderr[9]);
      }
      else if(!strncmp(str,"CUTS",4))
      {
        sscanf(&str[4],"%f %f %f %f %f %f %f %f %f %f",&sdakeys_.zcut[0],
               &sdakeys_.zcut[1],&sdakeys_.zcut[2],&sdakeys_.zcut[3],
               &sdakeys_.zcut[4],&sdakeys_.zcut[5],&sdakeys_.zcut[6],
               &sdakeys_.zcut[7],&sdakeys_.zcut[8],&sdakeys_.zcut[9]);
      }
      else if(!strncmp(str,"BGRD",4))
      {
        sscanf(&str[4],"%f %f %f %f %f %f %f %f %f %f %f %f %f %f",
               &sdakeys_.zbgrd[0],&sdakeys_.zbgrd[1],&sdakeys_.zbgrd[2],
               &sdakeys_.zbgrd[3],&sdakeys_.zbgrd[4],&sdakeys_.zbgrd[5],
               &sdakeys_.zbgrd[6],&sdakeys_.zbgrd[7],&sdakeys_.zbgrd[8],
               &sdakeys_.zbgrd[9],&sdakeys_.zbgrd[10],&sdakeys_.zbgrd[11],
               &sdakeys_.zbgrd[12],&sdakeys_.zbgrd[13]);
      }
      else if(!strncmp(str,"TFIT",4))
      {
        sscanf(&str[4],"%f %f %f %f %f %f %f %f %f %f",&sdakeys_.ztfit[0],
               &sdakeys_.ztfit[1],&sdakeys_.ztfit[2],&sdakeys_.ztfit[3],
               &sdakeys_.ztfit[4],&sdakeys_.ztfit[5],&sdakeys_.ztfit[6],
               &sdakeys_.ztfit[7],&sdakeys_.ztfit[8],&sdakeys_.ztfit[9]);
      }
      else if(!strncmp(str,"LOUT",4))
      {
        sscanf(&str[4],
               "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d"
               "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
               &sdakeys_.lout[0],&sdakeys_.lout[1],&sdakeys_.lout[2],
               &sdakeys_.lout[3],&sdakeys_.lout[4],&sdakeys_.lout[5],
               &sdakeys_.lout[6],&sdakeys_.lout[7],&sdakeys_.lout[8],
               &sdakeys_.lout[9],&sdakeys_.lout[10],&sdakeys_.lout[11],
               &sdakeys_.lout[12],&sdakeys_.lout[13],&sdakeys_.lout[14],
               &sdakeys_.lout[15],&sdakeys_.lout[16],&sdakeys_.lout[17],
               &sdakeys_.lout[18],&sdakeys_.lout[19],&sdakeys_.lout[20],
               &sdakeys_.lout[21],&sdakeys_.lout[22],&sdakeys_.lout[23],
               &sdakeys_.lout[24],&sdakeys_.lout[25],&sdakeys_.lout[26],
               &sdakeys_.lout[27],&sdakeys_.lout[28],&sdakeys_.lout[29],
               &sdakeys_.lout[30],&sdakeys_.lout[31],&sdakeys_.lout[32],
               &sdakeys_.lout[33],&sdakeys_.lout[34],&sdakeys_.lout[35]);
      }
      else if(!strncmp(str,"MAGN",4))
      {
        sscanf(&str[4],"%s %f %f %f %f %f %f %f %f %f",key,
               &sdakeys_.zmagn[1],&sdakeys_.zmagn[2],&sdakeys_.zmagn[3],
               &sdakeys_.zmagn[4],&sdakeys_.zmagn[5],&sdakeys_.zmagn[6],
               &sdakeys_.zmagn[7],&sdakeys_.zmagn[8],&sdakeys_.zmagn[9]);
        {
          key[0]=key[1]; key[1]=key[2]; key[2]=key[3]; key[3]=key[4];
          for(i=3; i>0; i--) if(key[i] == '\0') key[i] = ' ';
        }
        strncpy((char *)&sdakeys_.zmagn[0],"    ",4);
        strncpy((char *)&sdakeys_.zmagn[0],&key[0],4);
      }
      else if(!strncmp(str,"GEOM",4))
      {
        sscanf(&str[4],"%s %d %d %d %d %d %d %d %d %d",key,
               &sdakeys_.lgeom[1],&sdakeys_.lgeom[2],&sdakeys_.lgeom[3],
               &sdakeys_.lgeom[4],&sdakeys_.lgeom[5],&sdakeys_.lgeom[6],
               &sdakeys_.lgeom[7],&sdakeys_.lgeom[8],&sdakeys_.lgeom[9]);
        {
          key[0]=key[1]; key[1]=key[2]; key[2]=key[3]; key[3]=key[4];
          for(i=3; i>0; i--) if(key[i] == '\0') key[i] = ' ';
        }
        strncpy((char *)&sdakeys_.lgeom[0],"    ",4);
        strncpy((char *)&sdakeys_.lgeom[0],&key[0],4);
      }
      else if(!strncmp(str,"EVBU",4))
      {
        sscanf(&str[4],"%s %s %s %s %s %s %s %s %s %s",key,key1,key2,
               &sdakeys_.levb[3],
               &sdakeys_.levb[4],&sdakeys_.levb[5],&sdakeys_.levb[6],
               &sdakeys_.levb[7],&sdakeys_.levb[8],&sdakeys_.levb[9]);
        {
          key[0]=key[1]; key[1]=key[2]; key[2]=key[3]; key[3]=key[4];
          for(i=3; i>0; i--) if(key[i] == '\0') key[i] = ' ';
          key1[0]=key1[1]; key1[1]=key1[2]; key1[2]=key1[3]; key1[3]=key1[4];
          for(i=3; i>0; i--) if(key1[i] == '\0') key1[i] = ' ';
          key2[0]=key2[1]; key2[1]=key2[2]; key2[2]=key2[3]; key2[3]=key2[4];
          for(i=3; i>0; i--) if(key2[i] == '\0') key2[i] = ' ';
        }
        strncpy((char *)&sdakeys_.levb[0],"    ",4);
        strncpy((char *)&sdakeys_.levb[0],&key[0],4);
        strncpy((char *)&sdakeys_.levb[1],"    ",4);
        strncpy((char *)&sdakeys_.levb[1],&key1[0],4);
        strncpy((char *)&sdakeys_.levb[2],"    ",4);
        strncpy((char *)&sdakeys_.levb[2],&key2[0],4);
      }
      else if(!strncmp(str,"CALI",4))
      {
        sscanf(&str[4],"%s %d %d %d %d %d %d %d %d %d",key,
               &sdakeys_.lcali[1],&sdakeys_.lcali[2],&sdakeys_.lcali[3],
               &sdakeys_.lcali[4],&sdakeys_.lcali[5],&sdakeys_.lcali[6],
               &sdakeys_.lcali[7],&sdakeys_.lcali[8],&sdakeys_.lcali[9]);
        {
          key[0]=key[1]; key[1]=key[2]; key[2]=key[3]; key[3]=key[4];
          for(i=3; i>0; i--) if(key[i] == '\0') key[i] = ' ';
        }
        strncpy((char *)&sdakeys_.lcali[0],"    ",4);
        strncpy((char *)&sdakeys_.lcali[0],&key[0],4);
      }
      else if(!strncmp(str,"MATX",4))
      {
        sscanf(&str[4],"%s %d %d %d %d %d %d %d %d %d",key,
               &sdakeys_.lmatx[1],&sdakeys_.lmatx[2],&sdakeys_.lmatx[3],
               &sdakeys_.lmatx[4],&sdakeys_.lmatx[5],&sdakeys_.lmatx[6],
               &sdakeys_.lmatx[7],&sdakeys_.lmatx[8],&sdakeys_.lmatx[9]);
        {
          key[0]=key[1]; key[1]=key[2]; key[2]=key[3]; key[3]=key[4];
          for(i=3; i>0; i--) if(key[i] == '\0') key[i] = ' ';
        }
        strncpy((char *)&sdakeys_.lmatx[0],"    ",4);
        strncpy((char *)&sdakeys_.lmatx[0],&key[0],4);
      }
      else
      {
        printf("unknown key - exit.\n"); /* ignore comment lines and unknown keys */
        exit(0);
      }

    } while(1 == 1);

    fclose(fd);
  }

  /* Interpret some DATA cards */

  if(!strncmp("TABL",(char *)&sdakeys_.zmagn,4)) sdakeys_.mgtyp = 0;
  if(!strncmp("TORU",(char *)&sdakeys_.zmagn,4)) sdakeys_.mgtyp = 1;
  if(!strncmp("MINT",(char *)&sdakeys_.zmagn,4)) sdakeys_.mgtyp = 2;
  if(!strncmp("TOPM",(char *)&sdakeys_.zmagn,4)) sdakeys_.mgtyp = 3;
  if(!strncmp("UNIF",(char *)&sdakeys_.zmagn,4)) sdakeys_.mgtyp = 4;
  if(!strncmp("NONE",(char *)&sdakeys_.zmagn,4)) sdakeys_.mgtyp = 5;

  sdakeys_.ievt = 0;
  sdakeys_.Nevt = sdakeys_.ltrig[1];
  sdakeys_.Ndbg = sdakeys_.ltrig[2];
  sdakeys_.isim = 0;
  sdakeys_.ifsim = 0;

  if(sdakeys_.ztfit[5] == 0.) sdakeys_.ztfit[5] = 39.;
  if(sdakeys_.ztfit[4] == 0.)
  {
    sdakeys_.ztfit[4] = 1.;
    if(sdakeys_.zmagn[3] != 0.) sdakeys_.ztfit[4] = 4.;
  }

  return;
}


