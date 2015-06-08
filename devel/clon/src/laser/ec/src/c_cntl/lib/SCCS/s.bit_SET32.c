h47118
s 00000/00000/00000
d R 1.2 00/09/21 14:12:40 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/lib/bit_SET32.c
e
s 00052/00000/00000
d D 1.1 00/09/21 14:12:39 wolin 1 0
c date and time created 00/09/21 14:12:39 by wolin
e
u
U
f e 0
t
T
I 1

#include <bit_defs.h>


 
int bit_SET32(int status, int debug)
{        
 
           
        if( BIT00 & status  )   {bit00=1;}  else  {bit00=0;}
   	if( BIT01 & status  )   {bit01=1;}  else  {bit01=0;}
	if( BIT02 & status  )   {bit02=1;}  else  {bit02=0;}
	if( BIT03 & status  )   {bit03=1;}  else  {bit03=0;}
	if( BIT04 & status  )   {bit04=1;}  else  {bit04=0;}
	if( BIT05 & status  )   {bit05=1;}  else  {bit05=0;}
	if( BIT06 & status  )   {bit06=1;}  else  {bit06=0;}
	if( BIT07 & status  )   {bit07=1;}  else  {bit07=0;}
	if( BIT08 & status  )   {bit08=1;}  else  {bit08=0;}
	if( BIT09 & status  )   {bit09=1;}  else  {bit09=0;}
	if( BIT10 & status  )   {bit10=1;}  else  {bit10=0;}
	if( BIT11 & status  )   {bit11=1;}  else  {bit11=0;}
	if( BIT12 & status  )   {bit12=1;}  else  {bit12=0;}
   	if( BIT13 & status  )   {bit13=1;}  else  {bit13=0;}
        if( BIT14 & status  )   {bit14=1;}  else  {bit14=0;}
        if( BIT15 & status  )   {bit15=1;}  else  {bit15=0;} 

        if( BIT16 & status  )   {bit16=1;}  else  {bit16=0;}
        if( BIT17 & status  )   {bit17=1;}  else  {bit17=0;}
        if( BIT18 & status  )   {bit18=1;}  else  {bit18=0;}
        if( BIT19 & status  )   {bit19=1;}  else  {bit19=0;}
        if( BIT20 & status  )   {bit20=1;}  else  {bit20=0;}
        if( BIT21 & status  )   {bit21=1;}  else  {bit21=0;}
        if( BIT22 & status  )   {bit22=1;}  else  {bit22=0;}
        if( BIT23 & status  )   {bit23=1;}  else  {bit23=0;}
        if( BIT24 & status  )   {bit24=1;}  else  {bit24=0;}
        if( BIT25 & status  )   {bit25=1;}  else  {bit25=0;}
        if( BIT26 & status  )   {bit26=1;}  else  {bit26=0;}
        if( BIT27 & status  )   {bit27=1;}  else  {bit27=0;}
        if( BIT28 & status  )   {bit28=1;}  else  {bit28=0;}
        if( BIT29 & status  )   {bit29=1;}  else  {bit29=0;}
        if( BIT30 & status  )   {bit30=1;}  else  {bit30=0;}
        if( BIT31 & status  )   {bit31=1;}  else  {bit31=0;}
       
       
	 
	if(debug)printf(" %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d \n ",
	bit31,bit30,bit29,bit28,bit27,bit26,bit25,bit24,bit23,bit22,bit21,bit20,bit19,bit18,bit17,bit16,	      
	bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
	
     
 return(1);
}
E 1
