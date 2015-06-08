/****************************************************************************
 *
 *  fadc250Config.c  -  configuration library file for fADC250 board 
 *
 *  SP, 07-Nov-2013
 * Sergey Boyarinov Nov 2013 - simplify/adjust for Hall B
 *
 *  empty lines and line startes with # - will be ignored
 *  config file format:

CRATE             rocbcal1   <- ROC/crate name, usually IP name
FADC250_ALLSLOTS             <- just keyword - all settings after this line will be implemented
#                                              for all slots, till FADC250_SLOTS will be met
FADC250_SLOTS     3  8  15   <- slot_numbers - in which next settings will be implemented
#                                              till file ends or next FADC250_SLOTS will be met
FADC250_F_REV     0x02c1     <- firmware revision  (0x0 Bits:7-0)
FADC250_B_REV     0x0a03     <- board revision     (0x0 Bits:15-8)
FADC250_ID        0xfadc     <- board type         (0x0 Bits:31-16)

FADC250_MODE      1   <- process mode: 1-4  (0x10C Bits:2-0)
FADC250_W_OFFSET  50  <- number of ns back from trigger point. (0x120)
#                           (in Manual it is  PL=Trigger_Window(ns) * 250MHz)
FADC250_W_WIDTH   49  <- number of ns to include in trigger window. (0x11C)
#                           (in M:  PTW=Trigger_Window(ns) * 250MHz, minimum is 6)
FADC250_NSB       3   <- number of ns before trigger point to include in data processing. (0x124)
#                           This include the trigger Point. (minimum is 2 in all mode)
FADC250_NSA       6   <- number of ns after trigger point to include in data processing. (0x128)
#                           Minimum is (6 in mode 2) and ( 3 in mode 0 and 1).
#                           Number of sample report is 1 more for odd and 2 more for even NSA number.
FADC250_NPEAK     1   <- number of Pulses in Mode 2 and 3.  (0x10C Bits:6-5)

#                 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 - channels ##
FADC250_ADC_MASK  1  0  1  0  1  0  1  0  1  0  1  0  1  0  1  0   <- channel enable mask
FADC250_TRG_MASK  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1   <- trigger enable mask 

FADC250_TET       110        <- board Trigger Energy Threshold (TET), same for all 16 channels
FADC250_CH_TET    0    110   <- channel# and TET_value for this channel
FADC250_ALLCH_TET 111  222  2  3  4  5  6  7  8  9  10  11  12  13  14  15   <- 16 TETs (0x12C - 0x148)

FADC250_DAC       3300       <- board DAC, one and the same for all 16 channels
FADC250_CH_DAC    0    3300  <- channel# and DAC_value for this channel
FADC250_ALLCH_DAC 3300 3280 3310 3280 3310 3280 3310 3280 3300 3280 3300 3280 3310 3280 3310 3280 <- 16 DACs

FADC250_PED       210        <- board Pedestals, same for all channels
FADC250_CH_PED    0    210   <- channel# and Pedestal_value for this channel
FADC250_ALLCH_PED 210  220  210  215  215  220  220  210  210  215  215  220  220  210  215  220  <- 16 PEDs

FADC250_GAIN       0.5        <- board Gains, same for all channels
FADC250_CH_GAIN    0    0.5   <- channel# and Gain_value for this channel
FADC250_ALLCH_GAIN 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5  <- 16 GAINs



 cc -rdynamic -shared -o fadc250Config.so fadc250Config.c -I/home/halld/test_setup/coda/linuxvme/include /home/halld/test_setup/coda/linuxvme/jvme/libjvme.a /home/halld/test_setup/coda/linuxvme/fadcV2/libfadc.a -ldl -lpthread -lrt

 */

#if defined(VXWORKS) || defined(Linux_vme)

#include <stdio.h>
#include <string.h>

#include "fadc250Config.h"
#include "fadcLib.h"
#include "xxxConfig.h"

#undef DEBUG

static int active;

static int          nfadc;                        /* Number of FADC250s */
static FADC250_CONF fa250[NBOARD+1];


#define SCAN_MSK \
	args = sscanf (str_tmp, "%*s %d %d %d %d %d %d %d %d   \
                                     %d %d %d %d %d %d %d %d", \
		       &msk[ 0], &msk[ 1], &msk[ 2], &msk[ 3], \
		       &msk[ 4], &msk[ 5], &msk[ 6], &msk[ 7], \
		       &msk[ 8], &msk[ 9], &msk[10], &msk[11], \
		       &msk[12], &msk[13], &msk[14], &msk[15])

#define SCAN_FMSK \
	args = sscanf (str_tmp, "%*s %f %f %f %f %f %f %f %f   \
                                     %f %f %f %f %f %f %f %f", \
		       &fmsk[ 0], &fmsk[ 1], &fmsk[ 2], &fmsk[ 3], \
		       &fmsk[ 4], &fmsk[ 5], &fmsk[ 6], &fmsk[ 7], \
		       &fmsk[ 8], &fmsk[ 9], &fmsk[10], &fmsk[11], \
		       &fmsk[12], &fmsk[13], &fmsk[14], &fmsk[15])

#define GET_READ_MSK \
	SCAN_MSK; \
	ui1 = 0; \
	for(jj=0; jj<NCHAN; jj++) \
	{ \
	  if((msk[jj] < 0) || (msk[jj] > 1)) \
	  { \
	    printf("\nReadConfigFile: Wrong mask bit value, %d\n\n",msk[jj]); return(-6); \
	  } \
	  if(strcmp(keyword,"FADC250_ADC_MASK") == 0) msk[jj] = ~(msk[jj])&0x1; \
	  ui1 |= (msk[jj]<<jj); \
	}


int
fadc250Config(char *fname)
{
  int res;
  char string[10]; /*dummy, will not be used*/

  /* faInit() must be called by now; get the number of boards from there */
  nfadc = faGetNfadc();
  printf("fadc250Config: nfadc=%d\n",nfadc);

/*
  if(strlen(fname) > 0) 
  {
    fadc250UploadAll(string, 0);
  }
  else 
  {
    fadc250InitGlobals();
  }
 */
 fadc250InitGlobals();
  /* reading and parsing config file */
  if( (res = fadc250ReadConfigFile(fname)) < 0 )
  {
    printf("ERROR in fadc250Config: fadc250ReadConfigFile() returns %d\n",res);
    return(res);
  }

  /* download to all boards */
  fadc250DownloadAll();

  return(0);
}

void
fadc250InitGlobals()
{
  int ii, jj;

  printf("fadc250InitGlobals reached\n");

  /*nfadc = 0;*/
  nfadc = faGetNfadc();
  for(jj=0; jj<NBOARD; jj++)
  {
    fa250[jj].mode      = 1;
    fa250[jj].winOffset = 300;
    fa250[jj].winWidth  = 100;
    fa250[jj].nsa       = 6;
    fa250[jj].nsb       = 3;
    fa250[jj].npeak     = 1;
    fa250[jj].chDisMask = 0x0;
    fa250[jj].trigMask  = 0xffff;
    fa250[jj].thrIgnoreMask = 0;

    for(ii=0; ii<NCHAN; ii++)
    {
      fa250[jj].thr[ii] = 110;
      fa250[jj].dac[ii] = 3300;
      fa250[jj].ped[ii] = 0.;
      fa250[jj].gain[ii] = 0.5;
    }
  }
}


/* reading and parsing config file */
int
fadc250ReadConfigFile(char *filename)
{
  FILE   *fd;
  char   fname[FNLEN] = { "" };  /* config file name */
  int    ii, jj, ch;
  char   str_tmp[STRLEN], str2[STRLEN], keyword[ROCLEN];
  char   host[ROCLEN], ROC_name[ROCLEN];
  int    args, i1, i2, msk[16];
  int    slot, slot1, slot2, chan;
  unsigned int  ui1, ui2;
  float f1, fmsk[16];
  char *getenv();
  char *clonparms;
  char *expid;


  clonparms = getenv("CLON_PARMS");
  expid = getenv("EXPID");
  if(strlen(filename)!=0) /* filename specified */
  {
    if ( filename[0]=='/' || (filename[0]=='.' && filename[1]=='/') )
	{
      sprintf(fname, "%s", filename);
	}
    else
	{
      sprintf(fname, "%s/fadc250/%s", clonparms, filename);
	}

    if((fd=fopen(fname,"r")) == NULL)
    {
      printf("\nReadConfigFile: Can't open config file >%s<\n",fname);
      return(-1);
    }
  }
  else /* filename does not specified */
  {
    /* obtain our hostname */
    gethostname(host,ROCLEN);
    sprintf(fname, "%s/fadc250/%s.cnf", clonparms, host);
    if((fd=fopen(fname,"r")) == NULL)
    {
      sprintf(fname, "%s/fadc250/%s.cnf", clonparms, expid);
      if((fd=fopen(fname,"r")) == NULL)
      {
        printf("\nReadConfigFile: Can't open config file >%s<\n",fname);
        return(-2);
	  }
	}

  }

  printf("\nReadConfigFile: Using configuration file >%s<\n",fname);


  /* Parsing of config file */
  active = 0;
  while ((ch = getc(fd)) != EOF)
  {
    if ( ch == '#' || ch == ' ' || ch == '\t' )
    {
      while (getc(fd) != '\n') {}
    }
    else if( ch == '\n' ) {}
    else
    {
      ungetc(ch,fd);
      fgets(str_tmp, STRLEN, fd);
      sscanf (str_tmp, "%s %s", keyword, ROC_name);
#ifdef DEBUG
      printf("\nfgets returns %s so keyword=%s\n\n",str_tmp,keyword);
#endif

      /* Start parsing real config inputs */
      if(strcmp(keyword,"FADC250_CRATE") == 0)
      {
	    if(strcmp(ROC_name,host) == 0)
        {
	      printf("\nReadConfigFile: crate = %s  host = %s - activated\n",ROC_name,host);
          active = 1;
        }
	    else if(strcmp(ROC_name,"all") == 0)
		{
	      printf("\nReadConfigFile: crate = %s  host = %s - activated\n",ROC_name,host);
          active = 1;
		}
        else
		{
	      printf("\nReadConfigFile: crate = %s  host = %s - disactivated\n",ROC_name,host);
          active = 0;
		}
      }

      else if(active && ((strcmp(keyword,"FADC250_SLOT")==0) || (strcmp(keyword,"FADC250_SLOTS")==0)))
      {
        sscanf (str_tmp, "%*s %s", str2);
        /*printf("str2=%s\n",str2);*/
        if(isdigit(str2[0]))
        {
          slot1 = atoi(str2);
          slot2 = slot1 + 1;
          if(slot1<2 && slot1>21)
          {
            printf("\nReadConfigFile: Wrong slot number %d\n\n",slot1);
            return(-4);
          }
        }
        else if(!strcmp(str2,"all"))
        {
          slot1 = 0;
          slot2 = NBOARD;
        }
        else
        {
          printf("\nReadConfigFile: Wrong slot >%s<, must be 'all' or actual slot number\n\n",str2);
          return(-4);
        }
        /*printf("slot1=%d slot2=%d\n",slot1,slot2);*/
	  }

      else if(active && (strcmp(keyword,"FADC250_MODE") == 0))
      {
        sscanf (str_tmp, "%*s %d", &i1);
	    for(slot=slot1; slot<slot2; slot++) fa250[slot].mode = i1;
      }

      else if(active && (strcmp(keyword,"FADC250_W_OFFSET") == 0))
      {
        sscanf (str_tmp, "%*s %d", &i1);
	    for(slot=slot1; slot<slot2; slot++) fa250[slot].winOffset = i1/4;
      }

      else if(active && (strcmp(keyword,"FADC250_W_WIDTH") == 0))
      {
        sscanf (str_tmp, "%*s %d", &i1);
	    for(slot=slot1; slot<slot2; slot++) fa250[slot].winWidth = i1/4;
      }

      else if(active && (strcmp(keyword,"FADC250_NSA") == 0))
      {
        sscanf (str_tmp, "%*s %d", &i1);
	    for(slot=slot1; slot<slot2; slot++) fa250[slot].nsa = i1/4;
      }

      else if(active && (strcmp(keyword,"FADC250_NSB") == 0))
      {
        sscanf (str_tmp, "%*s %d", &i1);
	    for(slot=slot1; slot<slot2; slot++) fa250[slot].nsb = i1/4;
      }

      else if(active && (strcmp(keyword,"FADC250_NPEAK") == 0))
      {
        sscanf (str_tmp, "%*s %d", &i1);
	    for(slot=slot1; slot<slot2; slot++) fa250[slot].npeak = i1;
      }

      else if(active && (strcmp(keyword,"FADC250_ADC_MASK") == 0))
      {
	    GET_READ_MSK;
	    for(slot=slot1; slot<slot2; slot++) fa250[slot].chDisMask = ui1;
#ifdef DEBUG
	    printf("\nReadConfigFile: %s = 0x%04x \n",keyword,ui1);
#endif
      }

      else if(active && (strcmp(keyword,"FADC250_TRG_MASK") == 0))
      {
	    GET_READ_MSK;
	    for(slot=slot1; slot<slot2; slot++) fa250[slot].trigMask = ui1;
#ifdef DEBUG
	    printf("\nReadConfigFile: %s = 0x%04x \n",keyword,ui1);
#endif
      }

      else if(active && (strcmp(keyword,"FADC250_TET_IGNORE_MASK") == 0))
      {
	    GET_READ_MSK;
	    for(slot=slot1; slot<slot2; slot++) fa250[slot].thrIgnoreMask = ui1;
#ifdef DEBUG
	    printf("\nReadConfigFile: %s = 0x%04x \n",keyword,ui1);
#endif
      }

      else if(active && (strcmp(keyword,"FADC250_TET") == 0))
      {
        sscanf (str_tmp, "%*s %d", &ui1);
        for(slot=slot1; slot<slot2; slot++) for(ii=0; ii<NCHAN; ii++) fa250[slot].thr[ii] = ui1;
      }

      else if(active && (strcmp(keyword,"FADC250_CH_TET") == 0))
      {
        sscanf (str_tmp, "%*s %d %d", &chan, &ui1);
        if((chan<0) || (chan>NCHAN))
        {
	      printf("\nReadConfigFile: Wrong channel number %d, %s\n",chan,str_tmp);
	      return(-7);
        }
        for(slot=slot1; slot<slot2; slot++) fa250[slot].thr[chan] = ui1;
      }

      else if(active && (strcmp(keyword,"FADC250_ALLCH_TET") == 0))
      {
	    SCAN_MSK;
	    if(args != 16)
        {
	      printf("\nReadConfigFile: Wrong argument's number %d, should be 16\n\n",args);
          return(-8);
        }
	    for(slot=slot1; slot<slot2; slot++) for(ii=0; ii<NCHAN; ii++) fa250[slot].thr[ii] = msk[ii];
      }

      else if(active && (strcmp(keyword,"FADC250_DAC") == 0))
      {
        sscanf (str_tmp, "%*s %d", &ui1);
	    for(slot=slot1; slot<slot2; slot++) for(ii=0; ii<NCHAN; ii++) fa250[slot].dac[ii] = ui1;
      }

      else if(active && (strcmp(keyword,"FADC250_CH_DAC") == 0))
      {
        sscanf (str_tmp, "%*s %d %d", &chan, &ui1);
        if((chan<0) || (chan>NCHAN))
        {
	      printf("\nReadConfigFile: Wrong channel number %d, %s\n",chan,str_tmp);
	      return(-7);
        }
	    for(slot=slot1; slot<slot2; slot++) fa250[slot].dac[chan] = ui1;
      }

      else if(active && (strcmp(keyword,"FADC250_ALLCH_DAC") == 0))
      {
	    SCAN_MSK;
	    if(args != 16)
        {
	      printf("\nReadConfigFile: Wrong argument's number %d, should be 16\n\n",args);
          return(-8);
        }
	    for(slot=slot1; slot<slot2; slot++) for(ii=0; ii<NCHAN; ii++) fa250[slot].dac[ii] = msk[ii];
      }

      else if(active && (strcmp(keyword,"FADC250_PED") == 0))
      {
        sscanf (str_tmp, "%*s %f", &f1);
	    for(slot=slot1; slot<slot2; slot++) for(ii=0; ii<NCHAN; ii++) fa250[slot].ped[ii] = f1;
      }

      else if(active && (strcmp(keyword,"FADC250_CH_PED") == 0))
      {
        sscanf (str_tmp, "%*s %d %f", &chan, &f1);
        if((chan<0) || (chan>NCHAN))
        {
	      printf("\nReadConfigFile: Wrong channel number %d, %s\n",chan,str_tmp);
	      return(-7);
        }
	    for(slot=slot1; slot<slot2; slot++) fa250[slot].ped[chan] = f1;
      }

      else if(active && (strcmp(keyword,"FADC250_ALLCH_PED") == 0))
      {
	    SCAN_FMSK;
	    if(args != 16)
        {
	      printf("\nReadConfigFile: Wrong argument's number %d, should be 16\n\n",args);
          return(-8);
        }
	    for(slot=slot1; slot<slot2; slot++) for(ii=0; ii<NCHAN; ii++) fa250[slot].ped[ii] = fmsk[ii];
      }

      else if(active && (strcmp(keyword,"FADC250_GAIN") == 0))
      {
        sscanf (str_tmp, "%*s %f", &f1);
	    for(slot=slot1; slot<slot2; slot++) for(ii=0; ii<NCHAN; ii++) fa250[slot].gain[ii] = f1;
      }

      else if(active && (strcmp(keyword,"FADC250_CH_GAIN") == 0))
      {
        sscanf (str_tmp, "%*s %d %f", &chan, &f1);
        if((chan<0) || (chan>NCHAN))
        {
	      printf("\nReadConfigFile: Wrong channel number %d, %s\n",chan,str_tmp);
	      return(-7);
        }
	    for(slot=slot1; slot<slot2; slot++) fa250[slot].gain[chan] = f1;
      }

      else if(active && (strcmp(keyword,"FADC250_ALLCH_GAIN") == 0))
      {
	    SCAN_FMSK;
	    if(args != 16)
        {
	      printf("\nReadConfigFile: Wrong argument's number %d, should be 16\n\n",args);
          return(-8);
        }
	    for(slot=slot1; slot<slot2; slot++) for(ii=0; ii<NCHAN; ii++) fa250[slot].gain[ii] = fmsk[ii];
      }

      else
	  {
        ; /* unknown key - do nothing */
	  }

    }
  }
  fclose(fd);

  return(0);
}


/* download setting into all found FADCs */
int
fadc250DownloadAll()
{
  int slot, ii, jj, nsamples, threshold;
  float ped;
#if 0
  int updateThresholds = 1;
#endif

  printf("\n\nfadc250DownloadAll reached, nfadc=%d\n",nfadc);
  for(jj=0; jj<nfadc; jj++)
  {
    slot = faSlot(jj);
/*
    printf("\n\nfadc250DownloadAll: FA_SLOT=%d\n",slot);

    printf("params: mode=%d winOffset=%d winWidth=%d nsb=%d nsa=%d npeak=%d\n",
            fa250[slot].mode,
		    fa250[slot].winOffset,
		    fa250[slot].winWidth,
		    fa250[slot].nsb,
		    fa250[slot].nsa,
		    fa250[slot].npeak);
*/
    faSetProcMode(slot,
		    fa250[slot].mode,
		    fa250[slot].winOffset,
		    fa250[slot].winWidth,
		    fa250[slot].nsb,
		    fa250[slot].nsa,
		    fa250[slot].npeak, 0);
    faChanDisable(slot, fa250[jj].chDisMask);

    faThresholdIgnore(slot, fa250[jj].thrIgnoreMask);

    for(ii=0; ii<NCHAN; ii++)
    {
/*      printf("\n\nfadc250DownloadAll: channel=%d\n",ii); */

	   faSetDAC(slot, fa250[slot].dac[ii], (1<<ii));
      faSetChannelGain(slot, ii, fa250[slot].gain[ii]);



#if 0
      if(fa250[slot].ped[ii] < 1.)      /* pedestal was not specified in config file */
	  {
#endif
	     faSetThreshold(slot, fa250[slot].thr[ii],(1<<ii));
        faSetChannelPedestal(slot, ii, (int)fa250[slot].ped[ii]);	  
#if 0
	  }
	  else
	  {
        nsamples = fa250[slot].nsb + fa250[slot].nsa;
        printf("fadc250DownloadAll: nsamples=%d\n",nsamples);
        ped = fa250[slot].ped[ii] * ((float)nsamples);
        printf("fadc250DownloadAll: ped=%f(was %f) (will load %d)\n",ped,fa250[slot].ped[ii],(int)ped);
        faSetChannelPedestal(slot, ii, (int)ped);

        if(updateThresholds)
        {
          threshold = faGetChThreshold(slot, ii);
          /* if threshold=0, don't add pedestal since user is disabling zero suppression */
          if(threshold) threshold += (int)fa250[slot].ped[ii];
          faSetChThreshold(slot, ii, threshold);
        }
	  }
#endif

    }
  }

  return(0);
}



/* upload setting from all found FADCs */
int
fadc250UploadAll(char *string, int length)
{
 
  int slot, i, ii, jj, len1, len2;
  char *str, sss[1024];
  unsigned int tmp, val[FA_MAX_ADC_CHANNELS], adcChanEnabled;
  unsigned short sval[FA_MAX_ADC_CHANNELS];
  float fval[FA_MAX_ADC_CHANNELS];
  printf("fadc250UploadAll reached %s %i \n",string,length); fflush(stdout);
  for(jj=0; jj<nfadc; jj++)
  {
    slot = faSlot(jj);

    faGetProcMode(slot,
		    &fa250[slot].mode,
		    &fa250[slot].winOffset,
		    &fa250[slot].winWidth,
		    &fa250[slot].nsb,
		    &fa250[slot].nsa,
		    &fa250[slot].npeak);


    fa250[slot].chDisMask = faGetChanMask(slot);
    fa250[slot].thrIgnoreMask = faGetThresholdIgnoreMask(slot);
    for(i=0;i<FA_MAX_ADC_CHANNELS;i++)
    {
      fa250[slot].dac[i] = faGetChannelDAC(slot, i);

      fa250[slot].ped[i] = faGetChannelPedestal(slot, i);
      fa250[slot].ped[i] = ((float)fa250[slot].ped[i])/(fa250[slot].nsa+fa250[slot].nsb); /* go back from integral to amplitude */

      fa250[slot].thr[i] = faGetChThreshold(slot, i);
		if(fa250[slot].thr[i] > 0)
 	     fa250[slot].thr[i] = fa250[slot].thr[i] - (int)fa250[slot].ped[i]; /* MUST SUBTRACT PEDESTAL TO BE CONSISTENT WITH DOWNLOADED THRESHOLD */

      fa250[slot].gain[i] = faGetChannelGain(slot, i);
    }
  }


  if(length)
  {
    str = string;
    str[0] = '\0';

    for(jj=0; jj<nfadc; jj++)
    {
      slot = faSlot(jj);

      sprintf(sss,"FADC250_SLOT %d\n",slot);
      ADD_TO_STRING;

      sprintf(sss,"FADC250_MODE %d\n",      fa250[slot].mode);
      ADD_TO_STRING;
	  sprintf(sss,"FADC250_W_OFFSET %d\n",  fa250[slot].winOffset*FA_ADC_NS_PER_CLK);
      ADD_TO_STRING;
	  sprintf(sss,"FADC250_W_WIDTH  %d\n",  fa250[slot].winWidth*FA_ADC_NS_PER_CLK);
      ADD_TO_STRING;
	  sprintf(sss,"FADC250_NSA %d\n",       fa250[slot].nsa*FA_ADC_NS_PER_CLK);
      ADD_TO_STRING;
	  sprintf(sss,"FADC250_NSB %d\n",       fa250[slot].nsb*FA_ADC_NS_PER_CLK);
      ADD_TO_STRING;
	  sprintf(sss,"FADC250_NPEAK %d\n",     fa250[slot].npeak);
      ADD_TO_STRING;

      adcChanEnabled = 0xFFFF^fa250[slot].chDisMask;
	  sprintf(sss,"FADC250_ADC_MASK");
      ADD_TO_STRING;
      for(i=0; i<16; i++)
	  {
        sprintf(sss," %d",(adcChanEnabled>>(15-i))&0x1);
        ADD_TO_STRING;
      }
      sprintf(sss,"\n");
      ADD_TO_STRING;

      sprintf(sss,"FADC250_TET_IGNORE_MASK");
      ADD_TO_STRING;
      for(i=0; i<16; i++)
      {
        sprintf(sss," %d",(fa250[jj].thrIgnoreMask>>(15-i))&0x1);
        ADD_TO_STRING;
      }
      sprintf(sss,"\n");
      ADD_TO_STRING;

      sprintf(sss,"FADC250_ALLCH_DAC");
      ADD_TO_STRING;

      for(i=0; i<16; i++)
      {
        sprintf(sss," %d",fa250[slot].dac[i]);
        ADD_TO_STRING;
      }
      sprintf(sss,"\n");
      ADD_TO_STRING;

      sprintf(sss,"FADC250_ALLCH_PED");
      ADD_TO_STRING;
      for(i=0; i<16; i++)
	  {
        sprintf(sss," %7.3f", fa250[slot].ped[i]);
        ADD_TO_STRING;
      }
      sprintf(sss,"\n");
      ADD_TO_STRING;

      sprintf(sss,"FADC250_ALLCH_TET");
      ADD_TO_STRING;
      for(i=0; i<16; i++)
	  {
        sprintf(sss," %d",fa250[slot].thr[i]);
        ADD_TO_STRING;
      }
      sprintf(sss,"\n");
      ADD_TO_STRING;

      sprintf(sss,"FADC250_ALLCH_GAIN");
      ADD_TO_STRING;
      for(i=0; i<16; i++)
	  {
        sprintf(sss," %7.3f",fa250[slot].gain[i]);
        ADD_TO_STRING;
      }
      sprintf(sss,"\n");
      ADD_TO_STRING;
    }

    CLOSE_STRING;
  }

}



/* print board registers; if slot is zero, print all boards */
void
fadc250Mon(int slot)
{
  int id, start, end, kk, jj;


  nfadc = faGetNfadc();
  if(slot==0)
  {
    start = 0;
    end = nfadc;
  }
  else if((id = faId(slot)) >= 0)
  {
    start = id;
    end = start + 1;
  }
  else
  {
    return;
  }

  printf("nfadc=%d\n",nfadc);
  for(kk=start; kk<end; kk++)
  {
    faStatus(faSlot(kk),0);
  }

  return;
}


#else /* dummy version*/

void
fadc250Config_dummy()
{
  return;
}

#endif
