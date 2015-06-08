
#include <bit_defs.h>


 
int bit_SET(int status, int debug)
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
       
	 
	if(debug)printf(" %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d \n ",
	bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
	
     
 return(1);
}
