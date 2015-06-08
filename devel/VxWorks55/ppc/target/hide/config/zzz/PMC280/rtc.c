/******************************************************************************
*              (c), Copyright, MOTOROLA  INC - ECC                  *
*                                                                             *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MOTOROLA , INC.        *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT *
* OF MOTOROLA   OR ANY THIRD PARTY. MOTOROLA   RESERVES THE RIGHT AT ITS SOLE DISCRETION*
* TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MOTOROLA .       *
* THIS CODE IS PROVIDED "AS IS". MOTOROLA  MAKES NO WARRANTIES, EXPRESS,*
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  *
******************************************************************************/

#include "stdio.h"
#include "vxWorks.h"
#include "time.h"
#include "stdlib.h"
#include "ctype.h"
#include "string.h"
#include "sysLib.h"
#include "intLib.h"
#include "pmc280.h"
#include "i2cDrv.h"

IMPORT void sysMsDelay      (UINT);
IMPORT bool frcI2CWrite(UINT8 devAdd,UINT8* regFile, unsigned int noBytes) ;
#define BCD_TO_BIN(val) ((val)=((val)&15) + ((val)>>4)*10)
#define BIN_TO_BCD(val) ((val)=(((val)/10)<<4) + (val)%10)

int isday(char*,int* );
int ismon(char*,int*);
int isdate(char*,int*);
int ishour(char*,int*);
int ismin(char*,int*);
int issec(char*,int*);
int isyear(char*,int* ,int*);
bool rtccall(void);

void frcGetTime( struct tm *);
void frcSyncClk (void);
void frcSyncClkApp (void);
void frcSetTime(struct tm time);
int settime(char *);
void gettime (void);
IMPORT void frcRTCRead(UINT8 *);
IMPORT void frcRTCWrite(UINT8 *);



int dayr,monr,dater,hourr,minr,secr,yearr,cenr;

void gettime(void)
{
  struct tm *time; 
  UINT8 realtime[10];
  char  *timeinascii;
  int i=0,var,add,wday;
  bool RTC;
     
    RTC=rtccall();
    if(RTC) {
           time=malloc(sizeof(struct tm));     
             frcRTCRead(realtime);          
     		time->tm_hour=BCD_TO_BIN(realtime[0]);
     		time->tm_min=BCD_TO_BIN(realtime[1]);    
     		time->tm_sec=BCD_TO_BIN(realtime[2]);
                     
     		time->tm_mday=BCD_TO_BIN(realtime[3]);
                      
     		wday=BCD_TO_BIN(realtime[6]);
     		if (wday==7)
       			time->tm_wday=0;
     			else time->tm_wday=wday;   
     			time->tm_mon=BCD_TO_BIN(realtime[4])-1;     
                 
    		 var=BCD_TO_BIN(realtime[7]);
                  
    		 if(var==19)
      			add=0;
     			else if(var==20)
        			  add=100;
         			    else add=200;
                
 
     		time->tm_year=BCD_TO_BIN(realtime[5])+add;
                      
     		time->tm_yday=0;
     		time->tm_isdst=0;                         
    		timeinascii=asctime(time);
    		printf("the ansi time is:");
    		while(timeinascii[i]!='\0') {     		
       			printf("%c",timeinascii[i]);          
       			i=i+1;
                } 
   	free(time);
     }  
}
  
int settime(char *temp){                                       
 /* char temp[50];*/
  int day,mon,date,hour,min,sec,year,cen;
  UINT8 buffer[10]; 
  int flag=0;
  bool RTC;
  day=mon=date=hour=0;
  min=sec=year=cen=0;
  dayr=monr=0;
  dater=hourr=0;
  minr=secr=yearr=cenr=0;
  RTC=rtccall();
  if(RTC){  
  /* gettime();
   printf("\nEnter the new date and time:");
   printf("\nThe format is www-mmm-dd-hh:mm:ss-yyyy\n "); 

   scanf("%s",temp);
*/
   if((temp[3]=='-')&&(temp[7]=='-')&&(temp[10]=='-')&&(temp[13]==':')&&(temp[16]==':')
       &&(temp[19]=='-'))
       { 
          day=isday(temp,&dayr);
	    if(day)
	       mon=ismon(temp,&monr);
		  if(mon)
		      date=isdate(temp,&dater);
			 if(date)                           
			       hour=ishour(temp,&hourr);
			       if(hour)
				 min=ismin(temp,&minr);
				    if(min)
				      sec=issec(temp,&secr);
					 if(sec)
					    year=isyear(temp,&yearr,&cenr);
						if(year)                                                                                                      
                                                   {if((dater==31)&&((monr==2)||(monr==4)||(monr==6)||(monr==9)||(monr==11))) 
                                                           flag=0;                                                          
                                                      else if(((yearr%4)==0)&&(monr==2)&&(dater==30))
                                                            flag=0;                                                                 
                                                       else if(((yearr%4)!=0)&&(monr==2)&&((dater==30)||(dater==29)))
                                                            flag=0;
                                                        else
						            flag=1;
                                                      }
                                                    else
                                                      flag=0;						

	}

    if(flag==0)
	 {printf("The system cannot accept the date and time entered\n");
	  }
    else
	 {          
	  buffer[0]= BIN_TO_BCD(hourr);         
	  buffer[1]= BIN_TO_BCD(minr);         
	  buffer[2]= BIN_TO_BCD(secr);        
	  buffer[3]= BIN_TO_BCD(dater);          
	  buffer[4]= BIN_TO_BCD(monr);
                
	  buffer[5]= BIN_TO_BCD(yearr);
                 
          buffer[6]= BIN_TO_BCD(dayr);
                           
	  buffer[7]= BIN_TO_BCD(cenr);         	 
          frcRTCWrite(buffer);
	  }
      }
return flag; 
}

int isday(char *val,int *dayr){
 int i,flag=0,k,l;
 const char DAYS[8][4]={"MON","TUE","WED","THU","FRI","SAT","SUN"};
 const char days[8][4]={"mon","tue","wed","thu","fri","sat","sun"}; 
 char day[4];
  for(i=0;i<3;i++)
     day[i]=val[i];
  day[3]='\0';

  for(i=0;i<7;i++)
   {
     k=strcmp(days[i],day);
     l=strcmp(DAYS[i],day);    
     if((k==0)||(l==0))
      { flag=1;
	*dayr=i+1;	
       }
    }
  return(flag);
 }

int ismon(char *val,int *monr)
{
 int i,flag=0,k,j,l,m;
 const char MONTHS[13][4]={"JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};
 const char months[13][4]={"jan","feb","mar","apr","may","jun","jul","aug","sep","oct","nov","dec"};
 char mon[4];
  
 *monr=0; 
 for(i=0,j=4;i<3;i++,j++)
  mon[i]=val[j];
  mon[3]='\0';

 for(k=0;k<12;k++)
  {
   l=strcmp(MONTHS[k],mon);
   m=strcmp(months[k],mon);  
   if((l==0)||(m==0))
    {
     flag=1;
     *monr=k+1;    
    }  
  }

  return(flag);
}

int isdate(char *val,int *dater)
{ int i,j,dat,flag;
  char date[4];

  *dater=0;
  for(i=0,j=8;i<2;i++,j++)
     date[i]=val[j];
   date[i]='\0';
   
   if((isdigit((int)date[0]))&&(isdigit((int)date[1])))
      flag=1;
     else
       flag=0;
   if(flag==1)
     { dat=atoi(date); 
        if ((dat>0)&&(dat<32))  
	  { 
             flag=1;
	    *dater=dat;	   
           }
        else
	    flag=0;
      }      
   return(flag);
 }

int ishour(char *val,int *hourr)
{ int i,j,dat,flag;
  char hour[4];
  
  for(i=0,j=11;i<2;i++,j++)
    hour[i]=val[j];
  hour[2]='\0';
   if((isdigit((int)hour[0]))&&(isdigit((int)hour[1])))
         flag=1;
   else
         flag=0;
 
   if(flag==1)
     { dat=atoi(hour);
       if((dat>=0)&&(dat<25))
            {
	     flag=1;	     
             *hourr=dat;	     
	     }
         else 
            flag=0;
       } 
  
    return(flag);
 }
 
int ismin(char *val,int *minr)
{ int i,j,dat,flag=0;
  char min[4];
  
  for(i=0,j=14;i<2;i++,j++)
    min[i]=val[j];
  min[2]='\0';
   if((isdigit((int)min[0]))&&(isdigit((int)min[1])))
         flag=1;
   else
         flag=0;
 
   if(flag==1)
     { dat=atoi(min);
       if((dat>=0)&&(dat<60))
            {
             flag=1; 
	     *minr=dat;
	    }
         else 
            flag=0;
       } 
  
    return(flag);
 }
 
int issec(char *val,int *secr)
{ int i,j,dat,flag=0;
  char sec[4];
   *secr=0;
  for(i=0,j=17;i<2;i++,j++)
    sec[i]=val[j];
  sec[2]='\0';
   if((isdigit((int)sec[0]))&&(isdigit((int)sec[1])))
         flag=1;
   else
         flag=0;
 
   if(flag==1)
     { dat=atoi(sec);
       if((dat>=0)&&(dat<60))
            {
             flag=1; 
	     *secr=dat;	     
	     }
	 else
	    flag=0;
       } 
  
    return(flag);
}

int isyear(char *val,int *yearr,int *cenr)
{
 int i,j,flag=0,year1,dat;
 char year[5];
 
 for(i=0,j=20;i<4;i++,j++)
   year[i]=val[j];
   year[4]='\0';
   if((isdigit((int)year[0]))&&(isdigit((int)year[1]))&&(isdigit((int)year[2]))
       &&(isdigit((int)year[3])))
	   flag=1;
     else
	  flag=0;
      if(flag==1)
	{ dat=atoi(year);
          *cenr=dat/100;
         
	  if((dat> 1900 ) && (dat <2200))
	    {flag=1;
	     year1=dat-1900;
	     if(year1>=200)
		*yearr=year1-200;
	       else if(year1>=100)
		 *yearr=year1-100;
		else if(year1<100)
		  *yearr=year1;	      
	       }
	    else
	      flag=0;
	  }
	  return(flag);
       }
                  
bool rtccall(){
bool RTC=false;
UINT8 regfile[1];
int i;
regfile[0]=0;
	
    for (i=0;i<2;i++){
    		sysMsDelay(500); 
    		GT_REG_WRITE(I2C_SOFT_RESET,0x0);
    		sysMsDelay(40);   /* Restart the I2C controller */
   
     		if(frcI2CWrite(RTC_I2C_ADDR,regfile,1))
        	 RTC=true;
               else
                 RTC=false;
	}
	
     if (!RTC)            
         printf("\nRTC device absent\n");

     return(RTC);
}
