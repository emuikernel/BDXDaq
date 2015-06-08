h38079
s 00115/00022/00152
d D 1.11 10/07/21 15:22:54 juang 11 10
c *** empty log message ***
e
s 00052/00010/00122
d D 1.10 10/07/20 14:51:18 juang 10 9
c *** empty log message ***
e
s 00001/00001/00131
d D 1.9 10/07/20 09:39:45 juang 9 8
c *** empty log message ***
e
s 00006/00003/00126
d D 1.8 10/07/20 09:04:53 juang 8 7
c *** empty log message ***
e
s 00012/00002/00117
d D 1.7 10/07/19 10:11:21 boiarino 7 6
c *** empty log message ***
e
s 00002/00002/00117
d D 1.6 10/07/18 23:45:07 boiarino 6 5
c *** empty log message ***
e
s 00003/00002/00116
d D 1.5 10/07/18 18:15:33 boiarino 5 4
c *** empty log message ***
e
s 00010/00004/00108
d D 1.4 10/07/16 13:37:34 juang 4 3
c *** empty log message ***
e
s 00030/00007/00082
d D 1.3 10/07/16 13:12:39 juang 3 2
c *** empty log message ***
e
s 00034/00009/00055
d D 1.2 10/07/16 11:59:02 boiarino 2 1
c *** empty log message ***
e
s 00064/00000/00000
d D 1.1 07/06/28 23:26:06 boiarino 1 0
c date and time created 07/06/28 23:26:06 by boiarino
e
u
U
f e 0
t
T
I 1

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
I 3
#define EINTERVAL 90000
D 9
#define THRESHOLD 0.37
E 9
I 9
D 10
#define THRESHOLD 0.4
E 10
I 10
#define WARNTHRESH 0.5   /*Dictates how low the count to reference ratio
						   can be before being flagged as a warning*/
#define ERRORTHRESH 0.2  /*Dictates how low the count to reference ratio
						   can be before being flagged as an error*/
I 11
typedef struct alarm
{
  char type[NAMLEN];
  char message[128];
  char number[4];
 
} alarm;
E 11
E 10
E 9
E 3

void
D 7
main()
E 7
I 7
main(int argc, char **argv)
E 7
{
  BOSIO *handle;

D 2
  int iw[NBCS], tmp1=1, iret, ttt, nevents=0;
E 2
I 2
  int iw[NBCS], ind, tmp1=1, iret, ttt, nevents=0;
D 10
  int i, nsets, status;
E 10
I 10
D 11
  int i, j, k, nsets, status, errors, warnings, alarms;
E 11
I 11
  int i, j, nsets, status, errors, warnings, alarms;
E 11
E 10
D 3
  int counts[NSETS];
E 3
I 3
  int counts[NSETS], reference[NSETS];
E 3
  char setnames[NSETS][NAMLEN];
I 10
D 11
  char errorlist[NSETS][100];
  char warnlist[NSETS][100];
E 10
I 7
  char fname[128];
E 11
I 11
  /*char errorlist[NSETS][100];
  char warnlist[NSETS][100];*/
  char fname[128], name[16],number[4];
  alarm errorlist[NSETS],warnlist[NSETS];
E 11
E 7
I 3
  float ratio;
I 11


E 11
I 7
  printf("dmlibtest reached; *.dmcfg files are read from current directory\n");
D 10
 
E 10
I 10

E 10
  if(argc != 2)
  {
    printf("Usage: dmlibtest <fpack_filename>\n");
    exit(1);
  }
E 7
E 3
E 2

I 7
  strncpy(fname,argv[1],127);

E 7
  bosInit(iw,NBCS);
D 2
  iret = bosOpen("/work/boiarino/clas_055043.A00","r",&handle);
E 2
I 2
D 7
  iret = bosOpen("bigfile","r",&handle);
E 7
I 7
  iret = bosOpen(fname,"r",&handle);
E 7
E 2
  if(iret)
  {
    printf("bosOpen returns %d - exit\n",iret);
    exit(0);
  }

D 2
  dmconfig("");
E 2
I 2
D 5
  dmconfig("/usr/clas/devel/clon/src/prtr");
E 5
I 5
  /*dmconfig("/usr/clas/devel/clon/src/prtr");*/
  dmconfig(".");
E 5
E 2
D 11

E 11
  while((iret=bosRead(handle,iw,"E")) == 0)
  {

    /* select RUNEVENT records only */
    /*printf("kname >%8.8s< ...\n",&handle->rechead[3]);*/
D 2
    if( !strncmp((char *)&handle->rechead[3],"RUNEVENT",8) )
E 2
I 2
    /*if( !strncmp((char *)&handle->rechead[3],"RUNEVENT",8) )*/
E 2
	{
      /*printf("kname >%8.8s< !!!\n",&handle->rechead[3]);*/
D 2
      nevents ++;
E 2
I 2
      if((ind = etNlink(iw,"HEAD",0)) != 0)
      {
        if(iw[ind+4] < 10) /* physics event */
        {
		  /*printf("HEAD: our type = %d, coda type = %d\n",iw[ind+4],iw[ind+6]);*/
          if(iw[ind+6]<16)
		  {
            nevents ++;
D 6
			/*start_timer();*/
E 6
I 6
			start_timer();
E 6
            dmlib(iw);
I 6
			stop_timer(90000);
I 11
		
E 11
E 6
E 2

D 2
start_timer();
      dmlib(iw);
stop_timer(10000);

E 2
I 2
D 3
            if(!(nevents%90000))
E 3
I 3
            if(!(nevents%EINTERVAL))
E 3
            {
D 3
              printf("Event %d\n",nevents);
E 3
I 3
			  printf("Event %d\n",nevents);
I 11

E 11
E 3
              /*dmsetsprint();*/
D 11
              status = dmsetsoutput(&nsets,counts,setnames);
E 11
I 11
			  status = dmsetsoutput(&nsets,counts,setnames);
E 11
              if(status) printf("Actual number of sets was %d, cut to %d\n",
D 11
								status,nsets);
E 11
I 11
			  status,nsets);
E 11
I 10

D 11
			  	
E 10
D 4
              for(i=0; i<nsets; i++)
E 4
I 4
			  /*  for(i=0; i<nsets; i++)
E 4
			  {
D 3
                printf("name >%10.10s< count %7d\n",setnames[i],counts[i]);
E 3
I 3
				printf("name >%10.10s< count %7d\n",setnames[i],counts[i]);
E 11
I 11
			  /*for(i=0;i<nsets;i++){
				printf("%s\n", setnames[i]);
E 11
E 3
D 4
			  }
E 4
I 4
				}*/
I 11
			 
E 11
E 4
I 3
			  /*COPY TO REFERENCE AFTER FIRST EVENT INTERVAL*/
			  if(nevents == EINTERVAL){
				for(i=0; i<nsets; i++){
				  reference[i]=counts[i];
				  dmsetsreset();
				}
			  }
			  /*CHECK FOR ALARMS IN GROUPS*/
			  if( (!(nevents%EINTERVAL)) && nevents!=EINTERVAL && nevents!=0){
I 10
D 11
				errors = 0;    
				warnings = 0;
				                    /*errors and warnings used as counters as
									  well as warnlist and errorlist indices*/
	   
E 11
I 11
				errors = 0;    /*errors and warnings used as counters*/
				warnings = 0;  /*Also used as indices for warnlist and errorlist*/
E 11
				alarms = 0;

E 10
				for(i=0; i<nsets; i++){
I 8

E 8
D 4
				  ratio = (float) counts[i]/ (float) reference[i];
E 4
I 4
				  if(reference[i]!=0){
					ratio = (float) counts[i]/ (float) reference[i];
				  }
D 8
				  else{ ratio=1;}
E 8
I 8
				  else{
					ratio=1;
				  }
E 8
				   
D 5
				  printf("name >%10.10s< count %7d  ratio %4f\n",setnames[i],counts[i],ratio);
E 5
I 5
D 8
				  printf("name >%s< count %7d  ratio %4f\n",setnames[i],counts[i],ratio);
E 8
I 8
D 10
				   printf("name >%s< count %7d  ratio %4f\n",setnames[i],counts[i],ratio);
E 8
E 5
				   
E 10
I 10
				  //printf("name >%s< count %7d  ratio %4f\n",setnames[i],counts[i],ratio);
E 10
E 4

D 10
				  if( ratio <THRESHOLD && reference[i] > 300){
D 4
					  printf("WARNING DROP in %11s, drop of %f percent\n",setnames[i],100*(1-ratio));
E 4
I 4
D 8
					  printf("WARNING DROP in%9s, drop of %f percent\n",setnames[i],100*(1-ratio));
E 8
I 8
					  printf("WARNING DROP in %9s, drop of %f percent\n",setnames[i],100*(1-ratio));
E 10
I 10
				  if( ratio <WARNTHRESH && ratio > ERRORTHRESH && reference[i] > 300){
D 11
					sprintf(warnlist[warnings],"WARNING DROP in %9s, drop of %f percent\n",setnames[i],100*(1-ratio));
E 11
I 11
					sprintf(warnlist[warnings].message,"WARNING DROP in %9s, drop of %f percent\n",setnames[i],100*(1-ratio));
					strcpy(warnlist[warnings].type, strtok(setnames[i], "_"));
					strcpy(warnlist[warnings].number,strtok(NULL,"_"));
E 11
					warnings++;
E 10
E 8
E 4
				  }
D 10
				  if(ratio>(2-THRESHOLD) && reference[i]>300){
					  printf("WARNING JUMP in %11s, jump of %f percent\n",setnames[i],100*(ratio-1));
E 10
I 10

				  if(ratio>(2-WARNTHRESH) && ratio < (2-ERRORTHRESH) && reference[i]>300){
D 11
					sprintf(warnlist[warnings],"WARNING JUMP in %11s, jump of %f percent\n",setnames[i],100*(ratio-1));
E 11
I 11
					sprintf(warnlist[warnings].message,"WARNING JUMP in %11s, jump of %f percent\n",setnames[i],100*(ratio-1));
					strcpy(warnlist[warnings].type, strtok(setnames[i], "_"));
					strcpy(warnlist[warnings].number,strtok(NULL,"_"));
E 11
					warnings++;
E 10
				  }
I 10

				  if(ratio<= ERRORTHRESH && reference[i]>300){
D 11
					sprintf(errorlist[errors],"ERROR DROP in %11s, drop of %f percent\n",setnames[i],100*(1-ratio));
E 11
I 11
					sprintf(errorlist[errors].message,"ERROR DROP in %11s, drop of %f percent\n",setnames[i],100*(1-ratio));
					strcpy(errorlist[errors].type, strtok(setnames[i], "_"));
					strcpy(errorlist[warnings].number,strtok(NULL,"_"));
E 11
					errors++;
				  }

				  if(ratio>=(2-ERRORTHRESH) &&  reference[i]>300){
D 11
					sprintf(errorlist[errors],"WARNING JUMP in %11s, jump of %f percent\n",setnames[i],100*(ratio-1));
E 11
I 11
					sprintf(errorlist[errors].message,"WARNING JUMP in %11s, jump of %f percent\n",setnames[i],100*(ratio-1));
					strcpy(errorlist[errors].type, strtok(setnames[i], "_"));
					strcpy(errorlist[warnings].number,strtok(NULL,"_"));
E 11
					errors++;
				  }
E 10
				}
E 3
D 10
              dmsetsreset();
E 10
I 10
				alarms = errors + warnings;

				if(alarms > 200){
				  printf("WARNING: OVER 200 ALARMS\n");
				}

				else{
				  for(i=0;i<errors;i++){
D 11
				   	printf("%s",errorlist[i]);
E 11
I 11
					
				   	if(strcmp(errorlist[i].type,"DCLVSUPPLY")==0 
					   || strcmp(errorlist[i].type,"DCADBCRATE")==0 
					   || strcmp(errorlist[i].type,"DCTDCCRATE")==0){

					   printf("%s",errorlist[i].message);

					   for(j=0;j<errors;j++){

						 if(strcmp(errorlist[i].type,"DCLVSUPPLY")==0){
						   if(strcmp(errorlist[j].type,"DCFUSE")==0
							  && strcmp(errorlist[j].number,errorlist[i].number)==0){
							 strcpy(errorlist[j].type, "ignore");
						   }
						 }
						 if(strcmp(errorlist[i].type,"DCADBCRATE")==0){
						   if(strcmp(errorlist[j].type,"DCADBBOARD")==0
							  && strcmp(errorlist[j].number,errorlist[i].number)==0){
							 strcpy(errorlist[j].type, "ignore");
						   }
						 }
						 if(strcmp(errorlist[i].type,"DCTDCCRATE")==0){
						   if(strcmp(errorlist[j].type,"DCTDCBOARD")==0
							  && strcmp(errorlist[j].number,errorlist[i].number)==0){
							 strcpy(errorlist[j].type, "ignore");
						   }
						 }
					   }
					}
				
E 11
				  }
I 11
				  for(i=0;i<errors;i++){
E 11

I 11
					if(strcmp(errorlist[i].type,"DCFUSE")==0 
					   || strcmp(errorlist[i].type,"DCADBBOARD")==0 
					   || strcmp(errorlist[i].type,"DCTDCBOARD")==0){

					   printf("%s",errorlist[i].message);
					}
				  }

E 11
				  for(i=0;i<warnings;i++){
D 11
				  	printf("%s",warnlist[i]);
E 11
I 11
				  
					if(strcmp(warnlist[i].type,"DCLVSUPPLY")==0 
					   || strcmp(warnlist[i].type,"DCADBCRATE")==0 
					   || strcmp(warnlist[i].type,"DCTDCCRATE")==0){

					   printf("%s",warnlist[i].message);

					   for(j=0;j<warnings;j++){

						 if(strcmp(warnlist[i].type,"DCLVSUPPLY")==0){
						   if(strcmp(warnlist[j].type,"DCFUSE")==0
							  && strcmp(warnlist[j].number,warnlist[i].number)==0){
							 strcpy(warnlist[j].type, "ignore");
						   }
						 }
						 if(strcmp(warnlist[i].type,"DCADBCRATE")==0){
						   if(strcmp(warnlist[j].type,"DCADBBOARD")==0
							  && strcmp(warnlist[j].number,warnlist[i].number)==0){
							 strcpy(warnlist[j].type, "ignore");
						   }
						 }
						 if(strcmp(warnlist[i].type,"DCTDCCRATE")==0){
						   if(strcmp(warnlist[j].type,"DCTDCBOARD")==0
							  && strcmp(warnlist[j].number,warnlist[i].number)==0){
							 strcpy(warnlist[j].type, "ignore");
						   }
						 }
					   }
					}

E 11
				  }
I 11

				  for(i=0;i<warnings;i++){

					if(strcmp(warnlist[i].type,"DCFUSE")==0 
					   || strcmp(warnlist[i].type,"DCADBBOARD")==0 
					   || strcmp(warnlist[i].type,"DCTDCBOARD")==0){

					   printf("%s",warnlist[i].message);
					}
				  }

E 11
				}

				dmsetsreset();
E 10
D 3
			}
E 3
I 3
			  }
E 3
D 6
			/*stop_timer(90000);*/
E 6
I 3
			}
E 3
		  }
		}
	  }
E 2
	}
D 3

E 3
    if((iret=etLdrop(iw,"E")) < 0)
    {
      printf("Error in bosLdrop, number %d\n",iret);
      exit(0);
    }
  }

  iret = bosClose(handle);

  printf("nevents=%d, time=%d\n",nevents,ttt);

  exit(0);
D 3
}
E 3
I 3
  }

E 3
E 1
