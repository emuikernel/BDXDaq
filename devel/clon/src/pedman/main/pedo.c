/* Pedestal manager program
 *   by VHG  01.04.98
 *   update  04.04.98
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <errno.h>



/*sergey*/
#define NBCS 10000000
#include "bosio.h"
#include "bos.h"
#include "bcs.h"

#include "map_manager.h"
#include "pedo.h"

/* the number of channels in SC */
#define NSC_TOT 342
#define NSC_SEC 57

/* allocate BOS arrays */
/*sergey
BOSbank bcs_;
BOSbank wcs_;
*/

/*----------- get  main env variables ----------*/
char *getenv();
char *clonparms;
char *clonwsrc;

                       /* run parameters */

static char *msql_database = "clasrun";
static char *session;
static int run;
static int zero = 0;

int pflag = 0;
int flag = 0;

int ev_num;
int evtnum_min;
int evtnum_max;
int Crate_num; 
char map_filename[128];
char inputfile[128];
char *root_name;
char roc_tab_file[128];
FILE *fpec1, *fpec2, *fpcc, *fpsc, *fplac;
FILE *pedlog;
Crate_struct Crate[CRATE_COUNT] = { 
  
  /* CRATE DEFINITION: Store info for each crate here,
   *
   * Column:    Use:
   * 1          Crate Number.
   * 2          Offset = number of channels above pedestal that spar is set.
   * 3          Error count.
   * 4          skip flag - set with switch -skip.
   * 5          BosBank Name of RAW data. Should be RC## where ## is crate number
   * 6          Sparsification Table file name = croc##.spar.
   * 7          Translation Table filename.
   * 8          Low Slot: Slot number below which all cards are ADC's.(sergey: set to impossible 98)
   * 9          High Slot: Slot Number above which all cards are ADC's. (sergey: set to impossible 99)
   *10          Array of Slots.
   *11          Pointer to Demux lookup table.
   *
   * The two entries #8 and #9  are only used when no TT tables is found. 
   */
  {12,20,0,0,"RC12","croc07.spar","ROC12.tab",98,99,{0*25},NULL}, /* Crate 12  = CC & TAGT */
  {13,40,0,0,"RC13","croc06.spar","ROC13.tab",98,99,{0*25},NULL},  /* Crate 13 = SC & CALL */
  {14,15,0,0,"RC14","croc08.spar","ROC14.tab",98,99,{0*25},NULL}, /* Crate 14 = EC1  */
  {15,15,0,0,"RC15","croc11.spar","ROC15.tab",98,99,{0*25},NULL},  /* Crate 15 = EC2  */
  {16,100,0,0,"RC16","LAC.spar"  ,"ROC16.tab",98,99,{0*25},NULL}}; /* Crate 16 = LAC */

Map_struct Mcc[2] = {
  {NULL,CC_MAP_NAME,"pedestals","mean"} ,
  {NULL,CC_MAP_NAME,"pedestals","sigma"}};
Map_struct Mcc_id[2] = {
  {NULL,CC_MAP_NAME,"pedestals","sigma"} ,
  {NULL,CC_MAP_NAME,"pedestals","sigma"}};

Map_struct Mdsps[2] = {
  {NULL,PS_MAP_NAME,"pedestals","mean"} ,
  {NULL,PS_MAP_NAME,"pedestals","sigma"}};
Map_struct Mdsps_id[2] = {
  {NULL,PS_MAP_NAME,"pedestals","sigma"} ,
  {NULL,PS_MAP_NAME,"pedestals","sigma"}};

Map_struct Mdspc[2] = {
  {NULL,PC_MAP_NAME,"pedestals","mean"} ,
  {NULL,PC_MAP_NAME,"pedestals","sigma"}};
Map_struct Mdspc_id[2] = {
  {NULL,PC_MAP_NAME,"pedestals","sigma"} ,
  {NULL,PC_MAP_NAME,"pedestals","sigma"}};

Map_struct Mdstc[2] = {
  {NULL,TC_MAP_NAME,"pedestals","mean"} ,
  {NULL,TC_MAP_NAME,"pedestals","sigma"}};
Map_struct Mdstc_id[2] = {
  {NULL,TC_MAP_NAME,"pedestals","sigma"} ,
  {NULL,TC_MAP_NAME,"pedestals","sigma"}};

Map_struct Msc[4] = {
  {NULL,SC_MAP_NAME,"pedestals","left"} ,
  {NULL,SC_MAP_NAME,"pedu","left"},
  {NULL,SC_MAP_NAME,"pedestals","right"},
  {NULL,SC_MAP_NAME,"pedu","right"}};
Map_struct Msc_id[2] = {
  {NULL,SC_MAP_NAME,"pedu","right"} ,
  {NULL,SC_MAP_NAME,"pedu","right"}};


/* old start counter */
Map_struct Mst[2] = {
  {NULL,ST_MAP_NAME,"pedestals","mean"} ,
  {NULL,ST_MAP_NAME,"pedestals","sigma"}};
Map_struct Mst_id[2] = {
  {NULL,ST_MAP_NAME,"pedestals","sigma"} ,
  {NULL,ST_MAP_NAME,"pedestals","sigma"}};


/* new start counter */
Map_struct Mstn[2] = {
  {NULL,STN_MAP_NAME,"pedestals","mean"} ,
  {NULL,STN_MAP_NAME,"pedestals","sigma"}};
Map_struct Mstn_id[2] = {
  {NULL,STN_MAP_NAME,"pedestals","sigma"} ,
  {NULL,STN_MAP_NAME,"pedestals","sigma"}};



Map_struct Mec[12] = {
  {NULL,EC_MAP_NAME,"inner","u"} ,{NULL,EC_MAP_NAME,"inneru","u"},
  {NULL,EC_MAP_NAME,"inner","v"} ,{NULL,EC_MAP_NAME,"inneru","v"},
  {NULL,EC_MAP_NAME,"inner","w"} ,{NULL,EC_MAP_NAME,"inneru","w"},
  {NULL,EC_MAP_NAME,"outer","u"} ,{NULL,EC_MAP_NAME,"outeru","u"},
  {NULL,EC_MAP_NAME,"outer","v"} ,{NULL,EC_MAP_NAME,"outeru","v"},
  {NULL,EC_MAP_NAME,"outer","w"} ,{NULL,EC_MAP_NAME,"outeru","w"}};


Map_struct Mec_id[12] = {
  {NULL,EC_MAP_NAME,"inner","u"} ,{NULL,EC_MAP_NAME,"inneru","u"},
  {NULL,EC_MAP_NAME,"inner","v"} ,{NULL,EC_MAP_NAME,"inneru","v"},
  {NULL,EC_MAP_NAME,"inner","w"} ,{NULL,EC_MAP_NAME,"inneru","w"},
  {NULL,EC_MAP_NAME,"outer","u"} ,{NULL,EC_MAP_NAME,"outeru","u"},
  {NULL,EC_MAP_NAME,"outer","v"} ,{NULL,EC_MAP_NAME,"outeru","v"},
  {NULL,EC_MAP_NAME,"outer","w"} ,{NULL,EC_MAP_NAME,"outeru","w"}};


Map_struct Mec1[4] = {
  {NULL,EC1_MAP_NAME,"pedestals","left"} ,
  {NULL,EC1_MAP_NAME,"pedu","left"},
  {NULL,EC1_MAP_NAME,"pedestals","right"},
  {NULL,EC1_MAP_NAME,"pedu","right"}};
Map_struct Mec1_id[2] = {
  {NULL,EC1_MAP_NAME,"pedu","right"} ,
  {NULL,EC1_MAP_NAME,"pedu","right"}};


int
main(int argc,char *argv[])
{
  int line_count;
  int Run_Number;
  char *chtype,*chplace,*chbank;
  int isec,ipd,ipl;
  char fma[100];
  char *config;

  /*sergey*/
  int status;
  BOSIO *descriptor;
 
  clonparms = getenv("CLON_PARMS"); 
  root_name = getenv("CLON_PARMS");  

  session="clasprod";

  evtnum_min=1100;
  evtnum_max=2100;
  
  sprintf(fma,"%s/pedman/log/pedman.log",clonparms);
  if(( pedlog = fopen(fma,"w"))==NULL)
    printf("can't open log file \n");
  
  /* decoding the command_line arguments */
  decode_arguments(&argc,argv);
  
  /* initializing the data stractures */
  init_crate(Crate);
  
  /* Start up the bos system. */
  status = bosInit(bcs_.iw,NBCS);

  if(access(inputfile,R_OK)==EOF)
  {
    /* Make sure input file exists and can be read.*/
    fprintf(pedlog,"Cannot open file %s for reading.\n",inputfile);
    fclose(pedlog);
    exit(1);
  }
  else
  {
    /* Open input file. */
    status = bosOpen(inputfile,(char*)"r",&descriptor);
  }
  
  /* Find the first event */
  find_first_event(descriptor, &Run_Number);
  /*printf("-----> Run number %d\n",Run_Number);*/

  if(Run_Number == 0)
  {
    fprintf(pedlog,"Bad header!, can't get run number");
  }

  /* Run through the data file and collect histograms.  */
  process_data_file(descriptor); 
  
  /* close data file*/
  bosClose(descriptor);

  creat_spar(Crate,CRATE_COUNT,Run_Number);
  
  creat_sig(Crate,CRATE_COUNT,Run_Number);
  
  fill_map(Crate,CRATE_COUNT);

  creat_map(Crate,CRATE_COUNT,Run_Number);  


  /** TCL program than will ask permission to update the spar files 
    and to run the program of reading the new spar files and updating the map files
    **/

  exit(0);
}
/*========================================================*/

/* function sets */

void
decode_arguments(int *argc,char **argv)
{ 
  /* Read the command line and set all the switches etc. */
  /* Macro to remove 1 item from argument list. */
#define REMOVE_ONE {(*argc)--;for(j=i;j<(*argc);j++)argv[j]=argv[j+1];i--;}
#define I_PLUS_PLUS if((i+1)<(*argc)){i++;}else{break;}
  
  int i,j,skip,kk;
  int cnum;
  
  /* set default parameters */
  int crate_ham=0; /* all 5 crates */
  int voffset = 10; /* mean + 10 channel */

  for(kk=0; kk<5; kk++) Crate[kk].skip = 0;

  /*sergey: temporary: remove LAC1
  Crate[4].skip=1;
*/

  if(*argc<2)
  {
    *argc=2;
    argv[1]="-help";
  }
  
  if((*argc)>1)strcpy(inputfile,argv[1]);
  else      strcpy(inputfile,"bos.evt");  
  
  for(i=1; i<(*argc); i++)
  {
    if(argv[i][0]=='-')
	{
	  if(strcmp(argv[i],"-from")==0)
	    {
	      I_PLUS_PLUS; 
	      sscanf(argv[i],"%d",&evtnum_min);
	      REMOVE_ONE;
	    }
	  else if(strcmp(argv[i],"-to")==0)
	    {
	      I_PLUS_PLUS; 
	      sscanf(argv[i],"%d",&evtnum_max);
	      REMOVE_ONE;
	    }
	  else if(strcmp(argv[i],"-ec1")==0)
	    {
	      I_PLUS_PLUS; 
	      sscanf(argv[i],"%d",&crate_ham);
	      if(crate_ham == 0)
		Crate[2].skip=1;
              else
		{
		  Crate[2].skip=0;
		  fprintf(pedlog,"EC first crate on the list\n");
		}
	      REMOVE_ONE;
	    }    
	  
 	  else if(strcmp(argv[i],"-ec2")==0)
	    {
	      I_PLUS_PLUS; 
	      sscanf(argv[i],"%d",&crate_ham);
	      if(crate_ham == 0)
		Crate[3].skip=1;
              else
		{
		  Crate[3].skip=0;
		  fprintf(pedlog,"EC second crate on the list\n");
		}
	      REMOVE_ONE;
	    }    
	  
 	  else if(strcmp(argv[i],"-cc1")==0)
	    {
	      I_PLUS_PLUS; 
	      sscanf(argv[i],"%d",&crate_ham);
	      if(crate_ham == 0)
		Crate[0].skip=1;
              else
		{
		  Crate[0].skip=0;
		  fprintf(pedlog,"CC + TAGT crate on the list\n");
		}
	      REMOVE_ONE;
	    }    
	  
 	  else if(strcmp(argv[i],"-sc1")==0)
	    {
	      I_PLUS_PLUS; 
	      sscanf(argv[i],"%d",&crate_ham);
	      if(crate_ham == 0)
		Crate[1].skip=1;
              else
		{
		  Crate[1].skip=0;
		  fprintf(pedlog,"SC + CALL crate on the list\n");
		}
	      REMOVE_ONE;
	    }    
 	  else if(strcmp(argv[i],"-lac1")==0)
	    {
	      I_PLUS_PLUS; 
	      sscanf(argv[i],"%d",&crate_ham);
	      if(crate_ham == 0)
		Crate[4].skip=1;
              else
		{
		  Crate[4].skip=0;
		  fprintf(pedlog,"LAC crate on the list\n");
		}
	      REMOVE_ONE;
	    }
	  else if(strcmp(argv[i],"-offset")==0)
	    {
	      I_PLUS_PLUS;
	      sscanf(argv[i],"%d",&voffset);
	      REMOVE_ONE;
	      for(j=0; j<CRATE_COUNT; j++)
          {
		    Crate[j].offset = voffset;
	      }
	    }
	  else if(strcmp(argv[i],"-help")==0|| strcmp(argv[i],"-h")==0)
	    {
	      printf("pedo [bos_file]  <-from>  <-to>  <-ec1...1->in, 0->out> <-offset> \n");
	      exit(1); 
  	    }
	  else
	    {
	      printf("Type pedo -help \n");
	      exit(1); 
	    }
	}
    }
  if (evtnum_max > evtnum_min) 
    ev_num = evtnum_max - evtnum_min;
  
  fprintf(pedlog,"%s %d %d   \n",inputfile,ev_num,voffset);

  printf("11\n");

  return;
}

/*---------------------------------------------------------*/
void
init_crate(Crate_struct *Cr)
{
  /* Initialize the data structures for each crate. */
  
  int Crate_num,i,j,k;
  char strec1[100],strec2[100],strsc[100],strcc[100],strlac[100];
  
  /* creating raw root files */
  sprintf(strec1,"%s/pedman/raw/ec1_root.dat",clonparms);
  if((fpec1 = fopen(strec1,"w"))==NULL)
    fprintf(pedlog,"can't open ec1_root.dat\n");
  sprintf(strec2,"%s/pedman/raw/ec2_root.dat",clonparms);
  if((fpec2 = fopen(strec2,"w"))==NULL)
    fprintf(pedlog,"can't open ec2_root.dat\n");
  sprintf(strsc,"%s/pedman/raw/sc1_root.dat",clonparms);
  if((fpsc = fopen(strsc,"w"))==NULL)
    fprintf(pedlog,"can't open sc1_root.dat\n");
  sprintf(strcc,"%s/pedman/raw/cc1_root.dat",clonparms);
  if((fpcc = fopen(strcc,"w"))==NULL)
    fprintf(pedlog,"can't open cc1_root.dat\n");
  sprintf(strlac,"%s/pedman/raw/lac1_root.dat",clonparms);
  if((fplac = fopen(strlac,"w"))==NULL)
    fprintf(pedlog,"can't open lac1_root.dat\n");
  
  for(Crate_num=0; Crate_num<CRATE_COUNT; Crate_num++)
  {
    Cr[Crate_num].err_count = 0;
    for(i=0; i<25; i++)   /* Init data storage */
	{
      Cr[Crate_num].Slot[i].num=   -1;
      Cr[Crate_num].Slot[i].type = -1;
      for(j=0; j<64; j++)
	  {
	    Cr[Crate_num].Slot[i].Chan[j].hist = (int *)calloc(ev_num, sizeof (int));
	    Cr[Crate_num].Slot[i].Chan[j].data = 0; 
	    Cr[Crate_num].Slot[i].Fit[j].amp = 0; 
	    Cr[Crate_num].Slot[i].Fit[j].cent = 0; 
	    Cr[Crate_num].Slot[i].Fit[j].sigma = 0; 
	  }
    }

	/* use Translation Tables; if commented out, will run without TT */
    Cr[Crate_num].RC_look = NULL;
    if(!Cr[Crate_num].skip)
    {
      Cr[Crate_num].RC_look = alloc_decode_roc(0);
      strcpy(roc_tab_file,Cr[Crate_num].TT_file);
      printf("roc_tab_file >%s<\n",roc_tab_file);
      init_decode_roc(roc_tab_file,Cr[Crate_num].RC_look,NULL);
    } 


  }
}

/*---------------------------------------------------------*/
/*sergey: modified function*/
void
find_first_event(BOSIO *descriptor, int *runnum)
{
  /*
   * Uses GLOBAL variables:
   *   evtnum_min    = lowest event number requested from command line.
   */

  int status, head, iev;

  iev = 0;
  *runnum = -1;
  while(evtnum_min>iev)
  {
	status=bosRead(descriptor,bcs_.iw,(char*)"E");
    if(status != 0)
    {
      printf("status=%d after bosRead 1 !!!\n");
      break;
	}

	/* check for event type or class */
	head=bosNlink(bcs_.iw,(char*)"HEAD",0);
	if(head>0)
    {
      /*printf("Runnum=%d Event=%d\n",bcs_.iw[head+1],bcs_.iw[head+2]);*/
      iev = bcs_.iw[head+2];
	  /*
	  if((evtype!=-9999)&&(bcs_.iw[head+4]!=evtype))continue;    
      if((evclass!=-9999)&&(bcs_.iw[head+6]!=evclass))continue;
	  */
	}
    else
	{
      printf("NO HEAD BANK !!!!!!!!!!!!!\n");
    }

    bosLdrop(bcs_.iw,(char*)"E");
    bosNgarbage(bcs_.iw);

  } /* read loop */
  *runnum = bcs_.iw[head+1];

  return;
}


/*--------------------------------------------------------*/
void
process_data_file(BOSIO *descriptor)
{
  /* Run through the data file and collect histograms.  */
  
  int j, status, ind, indh, evtype;
  int ievt, ievtbad;
  int icode;
  int Crate_num;
  
  clasRC_t *RC = NULL;
  RAW_DATA_t R;


  status=bosRead(descriptor,bcs_.iw,(char*)"E");
  if(status != 0)
  {
    printf("status=%d after bosRead 2 !!!\n");
  }


  ievtbad = 0;
  for(ievt=evtnum_min; ievt<=evtnum_max; ievt++)
  {
    if( (indh = bosNlink(bcs_.iw,(char*)"HEAD",0)) <= 0)
    {
      printf("ERROR: ievt=%d: no HEAD bank (indh=%d) - exit\n",ievt,indh);
      exit(0);
    }
    evtype = bcs_.iw[indh+4];
    if((evtype>10)||(evtype<1))
    {
      /*printf("non-physics event - skip\n");*/
      continue;
	}

    for(Crate_num=0; Crate_num<CRATE_COUNT; Crate_num++)
    {
      if(Crate[Crate_num].skip) continue;
                 
      /*printf("1 >%4.4s<\n",Crate[Crate_num].BOSname);fflush(stdout);*/
      ind = bosNlink(bcs_.iw,Crate[Crate_num].BOSname,0);
      if(ind > 0)
      {
        RC =(clasRC_t *)&bcs_.iw[ind-6];

        /* Run through the entries in the bank. */
        for(j=0;j<RC->bank.nrow;j++)    
	    {
	      /* Decode the raw adc value. If minor error, continue with next  */
	      if(decode_raw_data(RC,j,Crate[Crate_num].RC_look,
			                      Crate[Crate_num].tdc_slot_low,
                                  Crate[Crate_num].tdc_slot_high,
                                  &R))
		  {
            /* if returns 1, then it was ADC header - not an error realy .. */
            /*printf("minor error in >%4.4s< continue\n",RC->bank.name);*/
	        continue;
		  }	    

	      if(Crate[Crate_num].Slot[R.islot-1].num != R.islot)
          { 
	        Crate[Crate_num].Slot[R.islot-1].num= R.islot;       
	        Crate[Crate_num].Slot[R.islot-1].type= R.itype;
	      }
	    
	    
	      if(R.itype != 1)
          { /* It's an ADC */
	        update_histogram(Crate,Crate_num,R.islot,R.ichan,R.idat);
	      }
	    }


      } /*  RC is NULL, OOPS, bad data. */
      else
      {	
	    if(ievtbad>10)
        {
	      fprintf(pedlog,"error: No raw data crate %d  event %d ! \n",
		    Crate[Crate_num].num,ievt);
	      Crate[Crate_num].err_count++;
	      if(Crate[Crate_num].err_count>7)
          {
	        fprintf(pedlog,"Change Pedman configuration and run the program without crate %d .\n Reminder! crate12 - CC, crate13 - SC , crate14 - EC1 \n crate15 - EC2 , crate16 - LAC",
		      Crate[Crate_num].num);
	        fclose(pedlog);
            printf("No raw data !!!\n");
	        exit(1);
	      }
	    }
		/*
	     Print a warning if the data was taken without RAW wrinting turned on. 
	     First 2 (4?) events are headers, so there is no RC stuff usualy not there.
           don't complain.
		*/
      }

      
    } /*Crate_num*/    


    /* Get the next event */
    bosLdrop(bcs_.iw,(char*)"E");
    bosNgarbage(bcs_.iw);

    status = bosRead(descriptor,bcs_.iw,(char*)"E");
    if(status<0)
    {
      printf("status=%d after bosRead 3 !!!\n");
      printf("End of file !!!\n");
      break;
	} 

  } /*ievt*/

  fclose(fpcc);
  fclose(fpsc);
  fclose(fpec1);
  fclose(fpec2);
  fclose(fplac);
}

/*-------------------------------------------------------------*/
void
update_histogram(Crate_struct *Crate,int Crate_num,
                 int islot,int ichan,int idat)
{
  
  int i;
  /*
if( islot==0) 
    printf("HOPLA==> crate = %d slot = %d chan = %d data = %d \n", Crate_num,islot,ichan,idat); */
      if(idat > MIN && idat < MAX) {
  Crate[Crate_num].Slot[islot-1].Chan[ichan].data += idat;
  Crate[Crate_num].Slot[islot-1].Fit[ichan].amp++;	
  /* DEBUG PRINTOUT 
   printf(" Debug -> idat = %d  crate_num = %d \n",idat, Crate_num);
  */
    }

  

  /* 
     printf("%d %d  \n",Crate[Crate_num].Slot[islot-1].Fit[ichan].amp,Crate[Crate_num].Slot[islot-1].Chan[ichan].data);
     */
  switch (Crate_num)
  {
  case 0:
    fprintf(fpcc,"%d %d %d %d \n",Crate[Crate_num].num,islot,ichan,idat);
    if(idat > MIN && idat < MAX) {
      Crate[Crate_num].Slot[islot-1].Chan[ichan].hist[Crate[Crate_num].Slot[islot-1].Fit[ichan].amp]=idat;
      
    }
    break;
  case 1:
    fprintf(fpsc,"%d %d %d %d \n",Crate[Crate_num].num,islot,ichan,idat);
    if(idat > MIN && idat < MAX) {
      Crate[Crate_num].Slot[islot-1].Chan[ichan].hist[Crate[Crate_num].Slot[islot-1].Fit[ichan].amp]=idat;

    }
    break;
  case 2:
    fprintf(fpec1,"%d %d %d %d \n",Crate[Crate_num].num,islot,ichan,idat);
    if(idat > MIN && idat < MAX) {
      Crate[Crate_num].Slot[islot-1].Chan[ichan].hist[Crate[Crate_num].Slot[islot-1].Fit[ichan].amp]=idat;
      
    }
    break;
  case 3:
    fprintf(fpec2,"%d %d %d %d \n",Crate[Crate_num].num,islot,ichan,idat);
    if(idat > MIN && idat < MAX) {
      Crate[Crate_num].Slot[islot-1].Chan[ichan].hist[Crate[Crate_num].Slot[islot-1].Fit[ichan].amp]=idat;
      
    }
    break;
  case 4:
    fprintf(fplac,"%d %d %d %d \n",Crate[Crate_num].num,islot,ichan,idat);
    if(idat > MIN && idat < MAX) {
      Crate[Crate_num].Slot[islot-1].Chan[ichan].hist[Crate[Crate_num].Slot[islot-1].Fit[ichan].amp]=idat;
      
    }
    break;
  }
  
}


/*--------------------------------------------------------------*/
void
creat_spar(Crate_struct *Cr,int CR_num, int rnum)
{
  /* This creates the spar files with the run number */
  
  int Crate_num,i,j,k;
  
  /* For each crate... */    
  for(Crate_num=0;Crate_num< CR_num;Crate_num++)
  { 
    /* If not asked to skip... */
    if(Cr[Crate_num].skip)
    {
	  continue;
    }

    for(i=1; i<=25; i++)
    {      
      /* Check all the slots. */  
      if(Cr[Crate_num].Slot[25-i].type==0)
      { 
	    /* It's an initialized ADC. Use it. */
	    /* Check all channels for each slot. */

	    for(j=0; j<64; j++)
        {
	      /*
	      if(Cr[Crate_num].Slot[25-i].Chan[j].data <MIN && Cr[Crate_num].Slot[25-i].Chan[j].data > MAX){  
	        fprintf(pedlog," data = %d  Slot %d, Channel %d \n",Cr[Crate_num].Slot[25-i].Chan[j].data, 25-i,j);
	      }else{
          */

	      switch (Cr[Crate_num].num)
          {
	        case 12:
	        case 13:
	        case 14:
	        case 15:
	        case 16:

	          /* calculating the mean */
	          if(Cr[Crate_num].Slot[25-i].Fit[j].amp != 0)
			  {
	            Cr[Crate_num].Slot[25-i].Fit[j].cent = 
		          Cr[Crate_num].Slot[25-i].Chan[j].data /
                        Cr[Crate_num].Slot[25-i].Fit[j].amp;
			  }
              else
			  {
                Cr[Crate_num].Slot[25-i].Fit[j].cent = 999;
			  }
			  /*
			  printf("crate %2d slot %2d ch %2d -> data=%d amp=%d -> cent=%d\n",
                     Cr[Crate_num].num,Cr[Crate_num].Slot[25-i].num,j,
					 Cr[Crate_num].Slot[25-i].Chan[j].data,
					 Cr[Crate_num].Slot[25-i].Fit[j].amp,
					 Cr[Crate_num].Slot[25-i].Fit[j].cent);
			  */
	          break;

	      }
	  
	    }
      }
    }
  }
}

/*----------------------------------------------------*/
/*--------------------------------------------------------------*/
void
creat_sig(Crate_struct *Cr,int CR_num, int rnum)
{
  
  /* This creates the spar files with the run number */
  
  int Crate_num,i,j,k;
  static int cc_close_flag,sc_close_flag,ec1_close_flag,ec2_close_flag,lac_close_flag;
  char ec1[111],ec2[111],cc[111],sc[111],lac[111];
  double xach = 0.;
  FILE *cc_spar, *sc_spar, *ec1_spar, *ec2_spar, *lac_spar, *fd;

  sprintf(cc,"%s/pedman/archive/cc1_%s_%06d.ped",clonparms,session,rnum);
  sprintf(sc,"%s/pedman/archive/sc1_%s_%06d.ped",clonparms,session,rnum);
  sprintf(ec1,"%s/pedman/archive/ec1_%s_%06d.ped",clonparms,session,rnum);
  sprintf(ec2,"%s/pedman/archive/ec2_%s_%06d.ped",clonparms,session,rnum);
  sprintf(lac,"%s/pedman/archive/lac1_%s_%06d.ped",clonparms,session,rnum);

    
  /* For each crate... */    
  for(Crate_num=0;Crate_num< CR_num;Crate_num++)
  {
    /* If not asked to skip... */

    /* creating spar files */
    if(Cr[Crate_num].skip == 0)
    {
	
	  switch (Cr[Crate_num].num)
      {
	    case 12:
	      if((cc_spar = fopen(cc,"w"))==NULL)
	        fprintf(pedlog,"can't open %s file\n",cc);
	      cc_close_flag = 1;
	      break;

	    case 13:
	      if((sc_spar = fopen(sc,"w"))==NULL)
	        fprintf(pedlog,"can't open %s file\n",sc);
	      sc_close_flag = 1;
	      break;

	    case 14:
	      if((ec1_spar = fopen(ec1,"w"))==NULL)
	        fprintf(pedlog,"can't open %s file\n",ec1);
	      ec1_close_flag = 1;
	      break;

	    case 15:
	      if((ec2_spar = fopen(ec2,"w"))==NULL)
	        fprintf(pedlog,"can't open %s file\n",ec2);
	      ec2_close_flag = 1;
	      break;

	    case 16:
	      if((lac_spar = fopen(lac,"w"))==NULL)
	        fprintf(pedlog,"can't open %s file\n",lac);
	      lac_close_flag = 1;
	      break;
	  }
    
      for(i=1; i<=25; i++)
      {
	    /* Check all the slots. */
	    /*printf("%d %d\n",Cr[Crate_num].Slot[25-i].num,Cr[Crate_num].Slot[25-i].type);*/
	    if(Cr[Crate_num].Slot[25-i].type==0)
        { 
	      /* It's an initialized ADC. Use it. */
	      /* Check all channels for each slot. */
	  
	      for(j=0; j<64; j++)
          {
	        /*
	        if(Cr[Crate_num].Slot[25-i].Chan[j].data <MIN && Cr[Crate_num].Slot[25-i].Chan[j].data > MAX){  
	          fprintf(pedlog,"No data for Slot %d, Channel %d \n",i,j);
	       }else{
           */

	        switch (Cr[Crate_num].num)
            {
	          case 12:
	          case 13:
	          case 14:
	          case 15:
	          case 16:
		        for(k=1; k<=Crate[Crate_num].Slot[25-i].Fit[j].amp; k++)
		        {
		          xach += ((Cr[Crate_num].Slot[25-i].Chan[j].hist[k] - Cr[Crate_num].Slot[25-i].Fit[j].cent) *
			               (Cr[Crate_num].Slot[25-i].Chan[j].hist[k] - Cr[Crate_num].Slot[25-i].Fit[j].cent));
		        }
                if(Crate[Crate_num].Slot[25-i].Fit[j].amp != 0)
                  Cr[Crate_num].Slot[25-i].Fit[j].sigma = sqrt(xach/Crate[Crate_num].Slot[25-i].Fit[j].amp);
                else
                  Cr[Crate_num].Slot[25-i].Fit[j].sigma = 99.999;

		        xach = 0;
                break;
		    }

	        switch (Cr[Crate_num].num)
            {
	          case 12:
                fd = cc_spar;
		        break;

	          case 13:
                fd = sc_spar;
		        break;

	          case 14:
                fd = ec1_spar;
		        break;

	          case 15:
                fd = ec2_spar;
		        break;

	          case 16:
                fd = lac_spar;
		        break;
	        }

		    fprintf(fd,"%2d %2d %4d %6.3f %3d\n",Cr[Crate_num].Slot[25-i].num,j,
                  Cr[Crate_num].Slot[25-i].Fit[j].cent,
                  Cr[Crate_num].Slot[25-i].Fit[j].sigma,
                  Cr[Crate_num].offset);
			/*
		    printf("crate %2d slot %2d ch %2d -> cent=%4d sigma=%6.3f offset=%3d\n",
                         Cr[Crate_num].num,
                         Cr[Crate_num].Slot[25-i].num,
                         j,
                         Cr[Crate_num].Slot[25-i].Fit[j].cent,
                         Cr[Crate_num].Slot[25-i].Fit[j].sigma,
                         Cr[Crate_num].offset); 
			*/

	    
	      } /*for*/
	    } /*if*/
      } /*for*/
    } /*if*/      
  } /*for*/

  fprintf(pedlog,"\nBOS readout is done\n");
  fclose(pedlog);
    
  if(cc_close_flag  == 1) fclose(cc_spar);
  if(sc_close_flag  == 1) fclose(sc_spar);
  if(ec1_close_flag == 1) fclose(ec1_spar);
  if(ec2_close_flag == 1) fclose(ec2_spar);
  if(lac_close_flag == 1) fclose(lac_spar);

  printf("ped files are created \n");
}


/*------------------------------------------------*/
/* create detector-based files in 'Tfiles' directory */
int
put_values_in_map(Map_struct *M, Map_struct *Mid, int maplen, int num_m,
                  int Rnum, char *root_name)
{
  /*
   * Put values in to the map. We already
   * know root_name from call to 
   * get_values_from_map
   *
   */
  
  int i,j;
  int oldsect = -1;
  char mapfile[128];
  char textfile[128];
  char *point;
  FILE *fp, *fecb;

  
  sprintf(textfile,"%s/pedman/Tfiles/%s_%s_%06d.txt",root_name,M[1].mapfile,session,Rnum);
  if (!(fp = fopen(textfile,"w"))) printf("Unable to open textfile\n");
  
  switch (num_m)
  {
  case 2:
    fprintf(fp,"\n#  ID         Sector         Pedestal         Sigma \n"); 
    if(strcmp(M[1].mapfile,"cc.trans") == 0 )
    {
      fprintf(fp,"*SECTORS*\n"); 
      fprintf(fp,"1 2 3 4 5 6 \n");

      for(j=0; j<maplen; j++)
      {
        if ((int)Mid[1].arr[j] != oldsect)
        {
	      fprintf(fp,"*SECTOR%d*\n",(int)Mid[1].arr[j]+1); 
          oldsect =  (int)Mid[1].arr[j];
        }

        fprintf(fp,"%6d %6d %6d %13f \n",(int)Mid[0].arr[j],(int)Mid[1].arr[j],(int)M[0].arr[j],M[1].arr[j]);
      }
    }
    else
	{   
      fprintf(fp,"*SECTORS*\n"); 
      fprintf(fp,"0\n");
      
      for(j=0;j<maplen;j++)
      {
	    if ((int)Mid[1].arr[j] != oldsect)
        {
	      fprintf(fp,"\n*SECTOR%d*\n",(int)Mid[1].arr[j]); 
          oldsect =  (int)Mid[1].arr[j];
	    }
	
	    fprintf(fp,"%6d %6d %6d %13f \n",(int)Mid[0].arr[j],(int)Mid[1].arr[j],(int)M[0].arr[j],M[1].arr[j]);
      }
    }

    for(i=0;i<2;i++)
    {
      free(M[i].arr);
      M[i].arr=NULL;
      free(Mid[i].arr);
      Mid[i].arr=NULL;
    }

    break;

  case 4:
       fprintf(fp,"\n# ID   Sector   Pedestal_left   Sigma_left   Pedestal_right   Sigma_right\n"); 

       if(strcmp(M[1].mapfile,"lac.trans") == 0 )fprintf(fp,"*SECTORS*\n 1 2 \n");
       else fprintf(fp,"*SECTORS*\n 1 2 3 4 5 6 \n");

       for(j=0;j<maplen;j++)
       {
         if ((int)Mid[1].arr[j] != oldsect)
         {
	       fprintf(fp,"\n*SECTOR%d*\n",(int)Mid[1].arr[j]+1); 
           oldsect =  (int)Mid[1].arr[j];
         }

	     fprintf(fp,"%6d %6d %6d %13f %6d %13f \n",(int)Mid[0].arr[j],(int)Mid[1].arr[j],(int)M[0].arr[j],M[1].arr[j],(int)M[2].arr[j],M[3].arr[j]);
       }
       for(i=0; i<4; i++)
       {
	     free(M[i].arr);
	     M[i].arr=NULL;
       }
       for(i=0; i<2; i++)
       {
         free(Mid[i].arr);
         Mid[i].arr = NULL;
       }

       break;

  case 6:
    sprintf(textfile,"%s/pedman/Tfiles/test_%s_%d.txt",root_name,M[1].mapfile,Rnum);

    fprintf(fp,"\n#  ID     Sector    Pedestal    Sigma \n"); 
    fprintf(fp,"*SECTORS*\n"); 
    fprintf(fp,"1 2 3 4 5 6 \n");
    for(j=0;j<maplen;j++)
    {
      if ((int)Mid[6].arr[j] != oldsect)
      {
	    fprintf(fp,"*SECTOR%d*\n",(int)Mid[6].arr[j]+1); 
        oldsect =  (int)Mid[6].arr[j];
      }
 
      /* output for the bos bank */
      fprintf(fp,"%6d %6d %6d %13f \n",(int)Mid[0].arr[j],(int)Mid[6].arr[j],(int)M[0].arr[j],M[6].arr[j]);
      fprintf(fp,"%6d %6d %6d %13f \n",(int)Mid[1].arr[j],(int)Mid[7].arr[j],(int)M[1].arr[j],M[7].arr[j]);
      fprintf(fp,"%6d %6d %6d %13f \n",(int)Mid[2].arr[j],(int)Mid[8].arr[j],(int)M[2].arr[j],M[8].arr[j]);
      fprintf(fp,"%6d %6d %6d %13f \n",(int)Mid[3].arr[j],(int)Mid[9].arr[j],(int)M[3].arr[j],M[9].arr[j]);
      fprintf(fp,"%6d %6d %6d %13f \n",(int)Mid[4].arr[j],(int)Mid[10].arr[j],(int)M[4].arr[j],M[10].arr[j]);
      fprintf(fp,"%6d %6d %6d %13f \n",(int)Mid[5].arr[j],(int)Mid[11].arr[j],(int)M[5].arr[j],M[11].arr[j]);
    }
    
    break;

  }

  fclose(fp);
  return(0);
}
  
/*-----------------------------------------------------------*/
int
fill_map(Crate_struct *Cr,int CR_num)
{    
    int i,j,Crate_num;
    int isec,isd1,isd2,ind;
    char *bankname;
    int itmp;
    int dont_copy;
    int ilac = -1;  
    
    /* For each crate... */    
    for(Crate_num=0; Crate_num<CR_num; Crate_num++)
    {
      /* If not asked to skip... */
      if(Cr[Crate_num].skip) continue;
      
      switch (Cr[Crate_num].num)
      {
        case 12:

	    if(Cr[Crate_num].RC_look==NULL) /* No TT, no cookie. */
		{
	      fprintf(stderr,
		    "Don't have translation tables. Can't put values in map.\n");
	      exit(1);
	    }
	


	/*piece 1 
	for(i=0;i<2;i++){
	  if((Mcc[i].arr = calloc(216,sizeof(float)))==NULL){
	    fprintf(stderr,"Could not allocate memory for MAP \n");
	  exit(2);
	  }
	  if((Mcc_id[i].arr = calloc(216,sizeof(float)))==NULL){
	    fprintf(stderr,"Could not allocate memory for MAP \n");
	    exit(2);
	  }
	}*/



	for(i=0;i<2;i++){
	  if((Mdsps[i].arr = calloc(8,sizeof(float)))==NULL){
	    fprintf(stderr,"Could not allocate memory for MAP \n");
	    exit(2);
	  }
	  if((Mdsps_id[i].arr = calloc(8,sizeof(float)))==NULL){
	    fprintf(stderr,"Could not allocate memory for MAP \n");
	    exit(2);
	  }
	}
	
	for(i=0;i<2;i++){
	  if((Mdspc[i].arr = calloc(6,sizeof(float)))==NULL){
	    fprintf(stderr,"Could not allocate memory for MAP \n");
	    exit(2);
	  }
	  if((Mdspc_id[i].arr = calloc(6,sizeof(float)))==NULL){
	    fprintf(stderr,"Could not allocate memory for MAP \n");
	    exit(2);
	  }
	}
	
	/* old dstc 1
	for(i=0;i<2;i++){
	  if((Mdstc[i].arr = calloc(7,sizeof(float)))==NULL){
	    fprintf(stderr,"Could not allocate memory for MAP \n");
	    exit(2);
	  }
	  if((Mdstc_id[i].arr = calloc(7,sizeof(float)))==NULL){
	    fprintf(stderr,"Could not allocate memory for MAP \n");
	    exit(2);
	  }
	}
	*/

    /* new start counter */
	for(i=0; i<2; i++)
    {
	  if((Mstn[i].arr = calloc(24,sizeof(float)))==NULL)
      {
	    fprintf(stderr,"Could not allocate memory for STN MAP \n");
	    exit(2);
	  }
	  if((Mstn_id[i].arr = calloc(24,sizeof(float)))==NULL)
      {
	    fprintf(stderr,"Could not allocate memory for STN MAP \n");
	    exit(2);
	  }
	}

	break;
	
      case 13:

	if(Cr[Crate_num].RC_look==NULL){ /* No TT, no cookie. */
	  fprintf(stderr,"Don't have translation tables. Can't put values in map.\n");
	  exit(1);
	}
	
	for(i=0;i<4;i++){
	  if((Msc[i].arr = calloc(NSC_TOT,sizeof(float)))==NULL){
	    fprintf(stderr,"Could not allocate memory for MAP \n");
	    exit(2);
	  }
	}
	for(i=0;i<2;i++){
	  if((Msc_id[i].arr = calloc(NSC_TOT,sizeof(float)))==NULL){
	    fprintf(stderr,"Could not allocate memory for MAP \n");
	    exit(2);
	  }      
	}

    /* old start counter */
	for(i=0;i<2;i++){
	  if((Mst[i].arr = calloc(6,sizeof(float)))==NULL){
	    fprintf(stderr,"Could not allocate memory for MAP \n");
	    exit(2);
	  }
	  if((Mst_id[i].arr = calloc(6,sizeof(float)))==NULL){
	    fprintf(stderr,"Could not allocate memory for MAP \n");
	    exit(2);
	  }
	}

	break;
	
      case 14:
	if(flag == 1) break;
	if(Cr[Crate_num].RC_look==NULL){ /* No TT, no cookie. */
	  fprintf(stderr,"Don't have translation tables. Can't put values in map.\n");
	  exit(1);
	}
	for(i=0;i<12;i++){
	  if((Mec[i].arr = calloc(216,sizeof(float)))==NULL){
	    fprintf(stderr,"Could not allocate memory for MAP \n");
	    exit(2);
	  } 
	  if((Mec_id[i].arr = calloc(216,sizeof(float)))==NULL){
	    fprintf(stderr,"Could not allocate memory for MAP \n");
	    exit(2);
	  }
	}
	flag = 1;
	break;
	
      case 15:

	if(Cr[Crate_num].RC_look==NULL){ /* No TT, no cookie. */
	  fprintf(stderr,"Don't have translation tables. Can't put values in map.\n");
	  exit(1);
	}

	/*piece 1 */
	for(i=0;i<2;i++){
	  if((Mcc[i].arr = calloc(216,sizeof(float)))==NULL){
	    fprintf(stderr,"Could not allocate memory for MAP \n");
	  exit(2);
	  }
	  if((Mcc_id[i].arr = calloc(216,sizeof(float)))==NULL){
	    fprintf(stderr,"Could not allocate memory for MAP \n");
	    exit(2);
	  }
	}

    /* new dstc 1 */
	for(i=0;i<2;i++){
	  if((Mdstc[i].arr = calloc(7,sizeof(float)))==NULL){
	    fprintf(stderr,"Could not allocate memory for MAP \n");
	    exit(2);
	  }
	  if((Mdstc_id[i].arr = calloc(7,sizeof(float)))==NULL){
	    fprintf(stderr,"Could not allocate memory for MAP \n");
	    exit(2);
	  }
	}


	if(flag == 1) break;


	for(i=0;i<12;i++){
	  if((Mec[i].arr = calloc(216,sizeof(float)))==NULL){
	    fprintf(stderr,"Could not allocate memory for MAP \n");
	    exit(2);
	  } 
	  if((Mec_id[i].arr = calloc(216,sizeof(float)))==NULL){
	    fprintf(stderr,"Could not allocate memory for MAP \n");
	    exit(2);
	  }
	}


	flag = 1;
	break;
	
      case 16:
	for(i=0;i<4;i++){
	  if((Mec1[i].arr = calloc(256,sizeof(float)))==NULL){
	    fprintf(stderr,"Could not allocate memory for MAP \n");
	    exit(2);
	  }
	}
	for(i=0;i<2;i++){
	  if((Mec1_id[i].arr = calloc(256,sizeof(float)))==NULL){
	    fprintf(stderr,"Could not allocate memory for MAP \n");
	    exit(2);
	  }
	}      
	break;
      }
      continue;       
    }
    
    
    /* For each crate... */    
    for(Crate_num=0; Crate_num<CR_num; Crate_num++)
    { 
      /* If not asked to skip... */
      if(Cr[Crate_num].skip) continue;
      
      for(i=0; i<25; i++)                  /* Check all the slots. */
	  {
	    if(Cr[Crate_num].Slot[i].type==0)  /* It's an initialized ADC. Use it. */
		{
	      for(j=0; j<64; j++)              /* Check all channels for each slot. */
	      {
	    
	    if(Cr[Crate_num].RC_look->mod[i+1]==NULL)
        {
	      fprintf(stderr,"Requested a module that is not in TT table: (%d,%d) \n",
		      i+1,j);
	      exit(1);
	    }	    
	    
	    bankname = Cr[Crate_num].RC_look->mod[i+1]->ch[j].bankname;
	    isec=Cr[Crate_num].RC_look->mod[i+1]->ch[j].sector-1;
	    isd1=Cr[Crate_num].RC_look->mod[i+1]->ch[j].layer-1;
	    isd2 =Cr[Crate_num].RC_look->mod[i+1]->ch[j].slab-1;
	    ind =Cr[Crate_num].RC_look->mod[i+1]->ch[j].place;


	  	if(isd2<0)continue;    /* Not in TT */
	    if(isec>5 || isd2<0)
        {
	      fprintf(stderr,
		    "Bad data encountered while sorting: (%d,%d)->(%d,%d,%d)\n",
		    i,j,isec,isd2,ind);
	      exit(1);	
	    }
	  
	  switch (Cr[Crate_num].num)
      {
	    
	  case 12:
	    /*piece 2    if(strncmp(bankname,"CC",2) == 0 ) {
	      if(isd2>35 || isd2<0)continue;
	      if(ind == 1 ) {  
		if(isec*36+isd2>216){
	      fprintf(stderr,"Sorting error, bad numbers in TT.\n");
	      exit(1);
		}
		Mcc[0].arr[isec*36+isd2] = (float)Cr[Crate_num].Slot[i].Fit[j].cent;
		Mcc[1].arr[isec*36+isd2] = (float)Cr[Crate_num].Slot[i].Fit[j].sigma;
		Mcc_id[0].arr[isec*36+isd2] = (isd1+1)*256+(isd2+1);
		Mcc_id[1].arr[isec*36+isd2] = isec;
	      }
	    }
	    else*/ if (strncmp(bankname,"DSPS",4) == 0 ) {
	      if(isd2>7 || isd2<0)continue;
	      if(ind == 1 ) {              /* this is an ADC */
		Mdsps[0].arr[isd2] = (float)Cr[Crate_num].Slot[i].Fit[j].cent;
		Mdsps[1].arr[isd2] = (float)Cr[Crate_num].Slot[i].Fit[j].sigma;
		Mdsps_id[0].arr[isd2] = (isd1+1)*256+(isd2+1);
		Mdsps_id[1].arr[isd2] = isec+1;
	      }
	    }
	    else if (strncmp(bankname,"DSPC",4) == 0 ) {
	    if(isd2>5 || isd2<0)continue;
	      if(isd2 == 0) {              /* this is an ADC */
		Mdspc[0].arr[ind-1] = (float)Cr[Crate_num].Slot[i].Fit[j].cent;
		Mdspc[1].arr[ind-1] = (float)Cr[Crate_num].Slot[i].Fit[j].sigma;
		Mdspc_id[0].arr[ind-1] = (isd1+1)*256+(isd2+1);
		Mdspc_id[1].arr[ind-1] = isec+1;
	      }
	    }
	    /* old dstc 2 else if (strncmp(bankname,"DSTC",4) == 0 ) {
	    if(isd2>6 || isd2<0)continue;
	      if(isd2 == 0) {
		Mdstc[0].arr[ind-1] = (float)Cr[Crate_num].Slot[i].Fit[j].cent;
		Mdstc[1].arr[ind-1] = (float)Cr[Crate_num].Slot[i].Fit[j].sigma;
		Mdstc_id[0].arr[ind-1] = (isd1+1)*256+(isd2+1);
		Mdstc_id[1].arr[ind-1] = isec+1;
	      }
		}*/

        /* new start counter */
	    else if (strncmp(bankname,"STN1",4) == 0)
        { /* this bank contains ADCs only */
	      if(isd2>23 || isd2<0)continue;

		  Mstn[0].arr[isd2] = (float)Cr[Crate_num].Slot[i].Fit[j].cent;
		  Mstn[1].arr[isd2] = (float)Cr[Crate_num].Slot[i].Fit[j].sigma;
	      Mstn_id[0].arr[isd2] = isd2+1;
	      Mstn_id[1].arr[isd2] = isec+1;
	    }

	    else continue;

	    break;
	    
	  case 13:
	    if(strncmp(bankname,"SC",2) == 0) {
	    if(isec<0)continue;      /* Must be from a different bank, like STMP */    
	    if(isd2>=NSC_SEC)continue;    
	    
	    if(isec*NSC_SEC+isd2 > NSC_TOT)continue; 
	    
	    if(ind==1||ind==3){ /* It's indeed an ADC */ 
	      itmp = ind-1;
	      
	      Msc[itmp  ].arr[isec*NSC_SEC+isd2] = (float)Cr[Crate_num].Slot[i].Fit[j].cent;
	      Msc[itmp+1].arr[isec*NSC_SEC+isd2] = (float)Cr[Crate_num].Slot[i].Fit[j].sigma;
	      if(ind ==1) {
			Msc_id[0].arr[isec*NSC_SEC+isd2] = /*(isd1+1)*256*/+(isd2+1); /* sergey: for SC do not use highest id's byte */ 
		Msc_id[1].arr[isec*NSC_SEC+isd2] = isec;
	      }
	    }
	    
	    }

        /* old start counter */
	    else if (strncmp(bankname,"ST",2) == 0)
        {
	      if(isd2>5 || isd2<0)continue;
	      if(ind == 1)
          {              /* this is an ADC */
		    Mst[0].arr[isd2] = (float)Cr[Crate_num].Slot[i].Fit[j].cent;
		    Mst[1].arr[isd2] = (float)Cr[Crate_num].Slot[i].Fit[j].sigma;
		    Mst_id[0].arr[isd2] = (isd1+1)*256+(isd2+1);
		    Mst_id[1].arr[isd2] = isec+1;
	      }
	    }

	    else continue;
	    
	    break;
	  case 15:
	    /*piece 2*/if(strncmp(bankname,"CC",2) == 0 )
        {
	      if(isd2>35 || isd2<0)continue;
	      if(ind == 1 )
          {              /* this is an ADC */   
		    if(isec*36+isd2>216)
            {
	          fprintf(stderr,"Sorting error, bad numbers in TT.\n");
	          exit(1);
		    }
		    Mcc[0].arr[isec*36+isd2] = (float)Cr[Crate_num].Slot[i].Fit[j].cent;
		    Mcc[1].arr[isec*36+isd2] = (float)Cr[Crate_num].Slot[i].Fit[j].sigma;
		    Mcc_id[0].arr[isec*36+isd2] = (isd1+1)*256+(isd2+1);
		    Mcc_id[1].arr[isec*36+isd2] = isec;
	      }
	    }
	    /* new dstc 2*/ else if (strncmp(bankname,"DSTC",4) == 0 )
        {
	      if(isd2>6 || isd2<0)continue;
	      if(isd2 == 0)
          {
		    Mdstc[0].arr[ind-1] = (float)Cr[Crate_num].Slot[i].Fit[j].cent;
		    Mdstc[1].arr[ind-1] = (float)Cr[Crate_num].Slot[i].Fit[j].sigma;
		    Mdstc_id[0].arr[ind-1] = (isd1+1)*256+(isd2+1);
		    Mdstc_id[1].arr[ind-1] = isec+1;
	      }
		}



		/*no break here !*/
      case 14:
	    if(strncmp(bankname,"EC",2)!=0) continue;
	    if(isd2>35 || isd2<0)continue; /* Must be from a different bank */
	    if(isd1<0  || isd1>5)continue; /* Error ? */
	    
	    
	    if(isec*36+isd2 > 216){
	      fprintf(stderr,"Sorting error, bad numbers in TT.\n");
	      exit(1);
	    }
	    /*	    itmp = isd1*2;*/
            itmp = isd1;
	    Mec[itmp].arr[isec*36+isd2] = (float)Cr[Crate_num].Slot[i].Fit[j].cent;
	    Mec[itmp+6 ].arr[isec*36+isd2] = (float)Cr[Crate_num].Slot[i].Fit[j].sigma; 
            Mec_id[itmp].arr[isec*36+isd2] = (isd1+1)*256+(isd2+1);
            Mec_id[itmp+6].arr[isec*36+isd2] = isec;
	    break;
	  case 16:
	    if(strncmp(bankname,"EC1",3)!=0) continue;
	    if(isec<0) continue; 
            if(isd2>40) continue;
	    if(ind==1||ind==3){ /* It's indeed an ADC */ 
	      itmp = ind-1;
	      if ( isd1<2 && isd1>=0 )
		ilac = isec *128 + (isd1*24 + isd2) ;
	      else if ( isd1>1 && isd1<4 )
		ilac = isec *128 + (isd1*24 + 16 + isd2) ;
	      else printf("error reading the isd1\n");
	      if (ilac >256 ) continue;
	      Mec1[itmp].arr[ilac] = (float)Cr[Crate_num].Slot[i].Fit[j].cent;
	      Mec1[itmp+1].arr[ilac] =  (float)Cr[Crate_num].Slot[i].Fit[j].sigma; 
	      if(ind == 1) {
		Mec1_id[0].arr[ilac] = (isd1+1)*256+(isd2+1);
		Mec1_id[1].arr[ilac] = isec;
		
	      }
	      /*
		printf("%f %f %f %f %f %f \n",Mec1_id[0].arr[ilac],Mec1_id[1].arr[ilac],Mec1[0].arr[ilac], Mec1[1].arr[ilac],Mec1[2].arr[ilac], Mec1[3].arr[ilac]); 
		printf("%d %d %d %d \n",isec,ind,isd1,isd2);
		*/
	    } 
	    break;
	  }
	  continue;       
	}
      }
    }
  }
}

	
/*--------------------------------------------------------------*/
void
creat_map(Crate_struct *Cr,int CR_num, int rnum)
{

  int Crate_num;
  
  /* For each crate... */    
  for(Crate_num=0;Crate_num< CR_num;Crate_num++)
  { 
    /* If not asked to skip... */
    if(Cr[Crate_num].skip) continue;
    
    switch (Cr[Crate_num].num) {
    case 12:
      /* piece 3 put_values_in_map(Mcc,Mcc_id,216,2,rnum,root_name); */
      put_values_in_map(Mdspc,Mdspc_id,6,2,rnum,root_name); 
      /* old dstc 3 put_values_in_map(Mdstc,Mdstc_id,7,2,rnum,root_name);*/
      put_values_in_map(Mdsps,Mdsps_id,8,2,rnum,root_name); 
      put_values_in_map(Mstn,Mstn_id,24,2,rnum,root_name); /* new ST */
      break;

    case 13:
      put_values_in_map(Msc,Msc_id,NSC_TOT,4,rnum,root_name); 
      put_values_in_map(Mst,Mst_id,6,2,rnum,root_name); /* old ST */
      break;
      
    case 14:
      if(pflag == 1) break;
      put_values_in_map(Mec,Mec_id,216,6,rnum,root_name);
      pflag = 1;  
      break;

    case 15:
      /* piece 3*/ put_values_in_map(Mcc,Mcc_id,216,2,rnum,root_name);
	  /* new dstc 3*/put_values_in_map(Mdstc,Mdstc_id,7,2,rnum,root_name); 
      if(pflag == 1) break;
      put_values_in_map(Mec,Mec_id,216,6,rnum,root_name); 
      pflag = 1;  
      break;

    case 16:
      put_values_in_map(Mec1,Mec1_id,256,4,rnum,root_name); 
      break;
    }
    continue;       
  }
  return;
}













