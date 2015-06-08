

/* dmlibtest.c - test program for dmlib.c */

/*
cc -I../bosio/bosio.s dmlib.c -I../codatt dmlib.c \
../bosio/SunOS/lib/libbosio.a ../codatt/libcodatt.a \
../cmon/SunOS/lib/libcmon.a -lnsl -lsocket -lthread \
-lposix4 -lresolv -ldl -lm -laio
*/


#include <stdio.h>
#include "bosio.h"
#include "bos.h"
#include "dmlib.h"

#define NBCS 700000
#define EINTERVAL 90000
#define WARNTHRESH 0.35   /*Dictates how low the count to reference ratio
						   can be before being flagged as a warning*/
#define ERRORTHRESH 0.2  /*Dictates how low the count to reference ratio
						   can be before being flagged as an error*/
typedef struct alarm
{
  char type[NAMLEN];
  char message[128];
  char alarm[8];
  char number[4];
  int sector, high, low;
 
} alarm;

void
main(int argc, char **argv)
{
  BOSIO *handle;
  FILE *pFile;

  int iw[NBCS], ind, tmp1=1, iret, ttt, nevents=0;
  int i, j, nsets, status, alarms, sector, layer,errordrop,errorjump,warnjump,warndrop;
  int counts[NSETS], reference[NSETS];
  char setnames[NSETS][NAMLEN];
  char fname[128], name[16], dmcfg[128], first[128], bankname[128], line[128];
  char temp[128];
  alarm alarmlist[NSETS];
  float ratio;


  printf("dmlibtest reached; *.dmcfg files are read from current directory\n");

  if(argc != 2)
  {
    printf("Usage: dmlibtest <fpack_filename>\n");
    exit(1);
  }

  strncpy(fname,argv[1],127);

  bosInit(iw,NBCS);
  iret = bosOpen(fname,"r",&handle);
  if(iret)
  {
    printf("bosOpen returns %d - exit\n",iret);
    exit(0);
  }

  /*dmconfig("/usr/clas/devel/clon/src/prtr");*/
  dmconfig(".");
  while((iret=bosRead(handle,iw,"E")) == 0)
  {

    /* select RUNEVENT records only */
    /*printf("kname >%8.8s< ...\n",&handle->rechead[3]);*/
    /*if( !strncmp((char *)&handle->rechead[3],"RUNEVENT",8) )*/
	{
      /*printf("kname >%8.8s< !!!\n",&handle->rechead[3]);*/
      if((ind = etNlink(iw,"HEAD",0)) != 0)
      {
        if(iw[ind+4] < 10) /* physics event */
        {
		  /*printf("HEAD: our type = %d, coda type = %d\n",iw[ind+4],iw[ind+6]);*/
          if(iw[ind+6]<16)
		  {
            nevents ++;
			start_timer();
            dmlib(iw);
			stop_timer(90000);
		

            if(!(nevents%EINTERVAL))
            {
			  printf("Event %d\n",nevents);

              /*dmsetsprint();*/
			  status = dmsetsoutput(&nsets,counts,setnames);
              if(status) printf("Actual number of sets was %d, cut to %d\n",
			  status,nsets);

			  /*for(i=0;i<nsets;i++){
				printf("%s\n", setnames[i]);
				}*/
			 
			  /*COPY TO REFERENCE AFTER FIRST EVENT INTERVAL*/
			  if(nevents == EINTERVAL){
				for(i=0; i<nsets; i++){
				  reference[i]=counts[i];
				  dmsetsreset();
				}
			  }
			  /*CHECK FOR ALARMS IN GROUPS AND SETS VALUES*/
			  if( (!(nevents%EINTERVAL)) && nevents!=EINTERVAL && nevents!=0){
				alarms = 0;

				for(i=0; i<nsets; i++){

				  if(reference[i]!=0){
					ratio = (float) counts[i]/ (float) reference[i];
				  }
				  else{
					ratio=1;
				  }
				  warndrop=0;
				  warnjump=0;
				  errordrop=0;
				  errorjump=0;
				  
				  /*FINDS ERROR AND MARKS TYPE*/
				  if( ratio <WARNTHRESH && ratio > ERRORTHRESH && reference[i] > 300){
					warndrop=1;
				  }
				  if(ratio>(2-WARNTHRESH) && ratio < (2-ERRORTHRESH) && reference[i]>300){
					warnjump=1;
				  }
				  if(ratio<= ERRORTHRESH && reference[i]>300){
					errordrop =1;
				  }
				  if(ratio>=(2-ERRORTHRESH) &&  reference[i]>300){
					errorjump=1;
				  }
				  

				  /*PROCESSES ERRORS AND STORES THEM INTO ALARMLIST*/
				  if(warndrop==1 ||
					 warnjump==1 ||
					 errordrop==1||
					 errorjump==1){
					
					if(warndrop==1){
					  sprintf(alarmlist[alarms].message,"WARNING DROP in %9s, drop of %f percent\n",setnames[i],100*(1-ratio));
					  strcpy(alarmlist[alarms].alarm, "WARN");		
					}
					if(warnjump==1){
					  sprintf(alarmlist[alarms].message,"WARNING JUMP in %11s, jump of %f percent\n",setnames[i],100*(ratio-1));
					  strcpy(alarmlist[alarms].alarm, "WARN");			
					}
					
					if(errordrop==1){
					  sprintf(alarmlist[alarms].message,"ERROR DROP in %11s, drop of %f percent\n",setnames[i],100*(1-ratio));
					  strcpy(alarmlist[alarms].alarm, "ERROR");		
					}
					
					if(errorjump==1){
					  sprintf(alarmlist[alarms].message,"ERROR JUMP in %11s, jump of %f percent\n",setnames[i],100*(ratio-1));
					  strcpy(alarmlist[alarms].alarm, "ERROR");
					}
					
					sprintf(dmcfg, "%s.dmcfg", setnames[i]);
					strcpy(alarmlist[alarms].type, strtok(setnames[i], "_"));
					strcpy(alarmlist[alarms].number,strtok(NULL,"_"));
					
					pFile = fopen(dmcfg,"r");
					
					if(pFile!=NULL){
					  
					  alarmlist[alarms].high = 0;
					  alarmlist[alarms].low = 50;
					  
					  while(!feof(pFile))
						{
						  
						  fgets(line,128,pFile);
						  strcpy(temp, line);
						  strcpy(first, strtok(temp, " "));
						  
						  
						  if(strcmp(first, "BANK")==0){
							
							strcpy(first,strtok(line," "));
							strcpy(bankname,strtok(NULL," "));
							sector = atoi(strtok(NULL, " "));
							layer = atoi(strtok(NULL, " "));
							
							alarmlist[alarms].sector = sector;
							
							if(layer > alarmlist[alarms].high){
							  alarmlist[alarms].high = layer;
							}
							if(layer < alarmlist[alarms].low){
							  alarmlist[alarms]. low = layer;
							}
						  }
						  
						}
					  
					  fclose(pFile);
					}
					else{
					  
					  printf("could not open file.\n");
					  
					}
					
					alarms++;
					
				  }
				  						    
				}
				

				if(alarms > 200){
				  printf("WARNING: OVER 200 ALARMS\n");
				}

				else{

				  /*ALARM PRIORITY CHECK*/
				  for(i=0;i<alarms;i++){
					
				   	if(strcmp(alarmlist[i].type,"DCLVSUPPLY")==0 
					   || strcmp(alarmlist[i].type,"DCADBCRATE")==0 
					   || strcmp(alarmlist[i].type,"DCTDCCRATE")==0){

					   for(j=0;j<alarms;j++){

						 /*CHECKS LV SUPPLY CONDITIONS*/
						 if(strcmp(alarmlist[i].type,"DCLVSUPPLY")==0){
						   
							 if(strcmp(alarmlist[j].type,"DCFUSE")==0
								&& strcmp(alarmlist[j].number,alarmlist[i].number)==0){
							   strcpy(alarmlist[j].alarm, "ignore");
							   
							 }
							 /*REMOVES ADB CRATES CORRESPONDING TO LVSUPPLY*/
							 if(strcmp(alarmlist[j].type, "DCADBCRATE")==0
								&& alarmlist[j].high <= alarmlist[i].high
								&& alarmlist[j].low >= alarmlist[i].low
								&& alarmlist[j].sector == alarmlist[i].sector
								){
							   
								  
							   if(strcmp(alarmlist[i].alarm,"ERROR")==0){
								 strcpy(alarmlist[j].alarm, "ignore");
							   }
							   if(strcmp(alarmlist[i].alarm,"WARN")==0
								  && strcmp(alarmlist[j].alarm,"WARN")==0){
								   strcpy(alarmlist[j].alarm, "ignore");
							   }

							 }



						 }
						 /*REMOVES ADB BOARDS ALARMS IF THEIR RESPECTIVE CRATE ERRORS*/
						 if(strcmp(alarmlist[i].type,"DCADBCRATE")==0){
						   
							 if(strcmp(alarmlist[j].type,"DCADBBOARD")==0
								&& strcmp(alarmlist[j].number,alarmlist[i].number)==0){

							  /* BIG NOTE: once implementation is in place for TDC
								  crates and LV supply overlap the following 3 if 
								  statements should be used in place of lines
								  following MARKED comments. These loops are used to
							      remove overlapping alarm warnings*/

							   if(strcmp(alarmlist[i].alarm,"ERROR")==0){
								 strcpy(alarmlist[j].alarm, "ignore");
							   }
							   if(strcmp(alarmlist[i].alarm,"WARN")==0
								  && strcmp(alarmlist[j].alarm,"WARN")==0){
								 strcpy(alarmlist[j].alarm, "ignore");
							   } 
							   if(strcmp(alarmlist[i].alarm, "ignore")==0){
								  strcpy(alarmlist[j].alarm, "ignore");
							   }
							   
							 }
						   
						 }
						 /*REMOVES TDC BOARDS ALARMS IF THEIR RESPECTIVE CRATE ERRORS*/
						 if(strcmp(alarmlist[i].type,"DCTDCCRATE")==0){
  
							 if(strcmp(alarmlist[j].type,"DCTDCBOARD")==0
								&& strcmp(alarmlist[j].number,alarmlist[i].number)==0){
							   
							   /*MARKED*/ strcpy(alarmlist[j].alarm, "ignore");
							   
							 }

						 }
					   }
					}
				
				  }
				
				  
				  /*PRINT MESSAGES*/
				  for(i=0;i<alarms;i++){
					if(strcmp(alarmlist[i].alarm, "ignore") !=0
					   &&  strcmp(alarmlist[i].alarm,"ERROR")==0 ){
					  printf("%s",alarmlist[i].message);
					}
				  }
				  for(i=0;i<alarms;i++){
					if(strcmp(alarmlist[i].alarm, "ignore") !=0
					   &&  strcmp(alarmlist[i].alarm,"WARN")==0){
					  printf("%s",alarmlist[i].message);
					}
				  }
				  

				}

				dmsetsreset();
			  }
			}
		  }
		}
	  }
	}
    if((iret=etLdrop(iw,"E")) < 0)
    {
      printf("Error in bosLdrop, number %d\n",iret);
      exit(0);
    }
  }

  iret = bosClose(handle);

  printf("nevents=%d, time=%d\n",nevents,ttt);

  exit(0);
  }

