h38379
s 00000/00000/00000
d R 1.2 00/09/21 14:12:41 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/lib/bit_split.c
e
s 00023/00000/00000
d D 1.1 00/09/21 14:12:40 wolin 1 0
c date and time created 00/09/21 14:12:40 by wolin
e
u
U
f e 0
t
T
I 1
#include <bit_defs.h>


void bit_split (int word, int bitList[16]) 
{
	if( BIT00 & word  )   {bitList[0]=1;}  else  {bitList[0]=0;}
   	if( BIT01 & word  )   {bitList[1]=1;}  else  {bitList[1]=0;}
	if( BIT02 & word  )   {bitList[2]=1;}  else  {bitList[2]=0;}
	if( BIT03 & word  )   {bitList[3]=1;}  else  {bitList[3]=0;}
	if( BIT04 & word  )   {bitList[4]=1;}  else  {bitList[4]=0;}
	if( BIT05 & word  )   {bitList[5]=1;}  else  {bitList[5]=0;}
	if( BIT06 & word  )   {bitList[6]=1;}  else  {bitList[6]=0;}
	if( BIT07 & word  )   {bitList[7]=1;}  else  {bitList[7]=0;}
	if( BIT08 & word  )   {bitList[8]=1;}  else  {bitList[8]=0;}
	if( BIT09 & word  )   {bitList[9]=1;}  else  {bitList[9]=0;}
	if( BIT10 & word  )   {bitList[10]=1;}  else  {bitList[10]=0;}
	if( BIT11 & word  )   {bitList[11]=1;}  else  {bitList[11]=0;}
	if( BIT12 & word  )   {bitList[12]=1;}  else  {bitList[12]=0;}
   	if( BIT13 & word  )   {bitList[13]=1;}  else  {bitList[13]=0;}
        if( BIT14 & word  )   {bitList[14]=1;}  else  {bitList[14]=0;}
        if( BIT15 & word  )   {bitList[15]=1;}  else  {bitList[15]=0;}
}	 

E 1
