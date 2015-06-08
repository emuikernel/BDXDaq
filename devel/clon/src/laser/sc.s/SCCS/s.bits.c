h17918
s 00011/00000/00031
d D 1.4 08/12/02 20:40:07 boiarino 5 4
c *** empty log message ***
e
s 00021/00203/00010
d D 1.3 03/07/21 14:15:23 sergpozd 4 3
c Completely new revision
e
s 00034/00034/00179
d D 1.2 03/04/22 10:07:21 sergpozd 3 1
c put static for local variables
e
s 00000/00000/00000
d R 1.2 03/03/03 14:04:17 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/sc/s/bits.c
e
s 00213/00000/00000
d D 1.1 03/03/03 14:04:16 boiarino 1 0
c date and time created 03/03/03 14:04:16 by boiarino
e
u
U
f e 0
t
T
I 5
#ifndef VXWORKS

E 5
I 4
/*
 * bits.c
 *
 */
E 4
I 1

D 4
/* bits.c */

E 4
#include <stdio.h>
D 4
/**************  bit defs for convenience ***********/
/* name them starting at zero                     */
E 4

D 4
#define  BIT00    0x000001    /* 1st bit or zeroth bit */
#define  BIT01    0x000002    /* 2nd bit               */
#define  BIT02    0x000004    /* 3rd bit               */
#define  BIT03    0x000008    /* 4 bit                 */
#define  BIT04    0x000010    /* 5 bit                 */
#define  BIT05    0x000020    /* 6 bit                 */
#define  BIT06    0x000040    /* 7 bit                 */
#define  BIT07    0x000080    /* 8 bit                 */
#define  BIT08    0x000100    /* 9 bit                 */
#define  BIT09    0x000200    /* 10 bit                */
#define  BIT10    0x000400    /* 11 bit                 */
#define  BIT11    0x000800    /* 12 bit                 */
#define  BIT12    0x001000    /* 13 bit                 */
#define  BIT13    0x002000    /* 14 bit                 */
#define  BIT14    0x004000    /* 15 bit                 */
#define  BIT15    0x008000    /* 16 bit  or 15 bit      */

#define  BIT16    0x010000    /* 17      */
#define  BIT17    0x020000    /* 18      */       
#define  BIT18    0x040000    /* 19      */       
#define  BIT19    0x080000    /* 20      */       

#define  BIT20    0x100000    /* 21      */
#define  BIT21    0x200000    /* 22      */
#define  BIT22    0x400000    /* 23      */
#define  BIT23    0x800000    /* 24      */

#define  BIT24   0x1000000    /* 25      */
#define  BIT25   0x2000000    /* 26      */
#define  BIT26   0x4000000    /* 27      */
#define  BIT27   0x8000000    /* 28      */

#define  BIT28  0x10000000    /* 29      */
#define  BIT29  0x20000000    /* 30      */
#define  BIT30  0x40000000    /* 31      */
#define  BIT31  0x80000000    /* 32      */



D 3
int  bit00  = 0 ;  /* 1st bit or zeroth bit */
int  bit01  = 0 ;  /* 2nd bit               */
int  bit02  = 0 ;  /* 3rd bit               */
int  bit03  = 0 ;  /* 4 bit                 */
int  bit04  = 0 ;  /* 5 bit                 */
int  bit05  = 0 ;  /* 6 bit                 */
int  bit06  = 0 ;  /* 7 bit                 */
int  bit07  = 0 ;  /* 8 bit                 */
int  bit08  = 0 ;  /* 9 bit                 */
int  bit09  = 0 ;  /* 10 bit                */
int  bit10  = 0 ;  /* 11 bit                 */
int  bit11  = 0 ;  /* 12 bit                 */
int  bit12  = 0 ;  /* 13 bit                 */
int  bit13  = 0 ;  /* 14 bit                 */
int  bit14  = 0 ;  /* 15 bit                 */
int  bit15  = 0 ;  /* 16 bit  or 15 bit      */
E 3
I 3
static int  bit00  = 0 ;  /* 1st bit or zeroth bit */
static int  bit01  = 0 ;  /* 2nd bit               */
static int  bit02  = 0 ;  /* 3rd bit               */
static int  bit03  = 0 ;  /* 4 bit                 */
static int  bit04  = 0 ;  /* 5 bit                 */
static int  bit05  = 0 ;  /* 6 bit                 */
static int  bit06  = 0 ;  /* 7 bit                 */
static int  bit07  = 0 ;  /* 8 bit                 */
static int  bit08  = 0 ;  /* 9 bit                 */
static int  bit09  = 0 ;  /* 10 bit                */
static int  bit10  = 0 ;  /* 11 bit                 */
static int  bit11  = 0 ;  /* 12 bit                 */
static int  bit12  = 0 ;  /* 13 bit                 */
static int  bit13  = 0 ;  /* 14 bit                 */
static int  bit14  = 0 ;  /* 15 bit                 */
static int  bit15  = 0 ;  /* 16 bit  or 15 bit      */
E 3

D 3
int  bit16  = 0 ;  /*       */
int  bit17  = 0 ;  /*       */
int  bit18  = 0 ;  /*       */
int  bit19  = 0 ;  /*       */
E 3
I 3
static int  bit16  = 0 ;  /*       */
static int  bit17  = 0 ;  /*       */
static int  bit18  = 0 ;  /*       */
static int  bit19  = 0 ;  /*       */
E 3
 
D 3
int  bit20  = 0 ;  /*       */
int  bit21  = 0 ;  /*       */
int  bit22  = 0 ;  /*       */
int  bit23  = 0 ;  /*       */
E 3
I 3
static int  bit20  = 0 ;  /*       */
static int  bit21  = 0 ;  /*       */
static int  bit22  = 0 ;  /*       */
static int  bit23  = 0 ;  /*       */
E 4
I 4
int
sbin_to_int(char *stringval)
{
  int j, res = 0;
  int bits[16] = {0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0};
E 4
E 3
  
D 3
int  bit24  = 0 ;  /*       */
int  bit25  = 0 ;  /*       */
int  bit26  = 0 ;  /*       */
int  bit27  = 0 ;  /*       */
E 3
I 3
D 4
static int  bit24  = 0 ;  /*       */
static int  bit25  = 0 ;  /*       */
static int  bit26  = 0 ;  /*       */
static int  bit27  = 0 ;  /*       */
E 3

D 3
int  bit28  = 0 ;  /*       */
int  bit29  = 0 ;  /*       */
int  bit30  = 0 ;  /*       */
int  bit31  = 0 ;  /*       */
E 3
I 3
static int  bit28  = 0 ;  /*       */
static int  bit29  = 0 ;  /*       */
static int  bit30  = 0 ;  /*       */
static int  bit31  = 0 ;  /*       */
E 3


D 3
unsigned int my_bit_array[32] = {
E 3
I 3
static unsigned int my_bit_array[32] = {
E 3
  BIT00,BIT01,BIT02,BIT03,BIT04,BIT05,BIT06,BIT07,
  BIT08,BIT09,BIT10,BIT11,BIT12,BIT13,BIT14,BIT15,
  BIT16,BIT17,BIT18,BIT19,BIT20,BIT21,BIT22,BIT23,
  BIT24,BIT25,BIT26,BIT27,BIT28,BIT29,BIT30,BIT31
                                };


D 3
int
E 3
I 3
static int
E 3
bit_SET(int status, int debug)
{           
  if( BIT00 & status )  {bit00=1;}  else  {bit00=0;}
  if( BIT01 & status )  {bit01=1;}  else  {bit01=0;}
  if( BIT02 & status )  {bit02=1;}  else  {bit02=0;}
  if( BIT03 & status )  {bit03=1;}  else  {bit03=0;}
  if( BIT04 & status )  {bit04=1;}  else  {bit04=0;}
  if( BIT05 & status )  {bit05=1;}  else  {bit05=0;}
  if( BIT06 & status )  {bit06=1;}  else  {bit06=0;}
  if( BIT07 & status )  {bit07=1;}  else  {bit07=0;}
  if( BIT08 & status )  {bit08=1;}  else  {bit08=0;}
  if( BIT09 & status )  {bit09=1;}  else  {bit09=0;}
  if( BIT10 & status )  {bit10=1;}  else  {bit10=0;}
  if( BIT11 & status )  {bit11=1;}  else  {bit11=0;}
  if( BIT12 & status )  {bit12=1;}  else  {bit12=0;}
  if( BIT13 & status )  {bit13=1;}  else  {bit13=0;}
  if( BIT14 & status )  {bit14=1;}  else  {bit14=0;}
  if( BIT15 & status )  {bit15=1;}  else  {bit15=0;}

  if(debug)
  {
    printf(" %d%d%d%d %d%d%d%d %d%d%d%d %d%d%d%d\n",
      bit15,bit14,bit13,bit12,bit11,bit10,bit09,bit08,
      bit07,bit06,bit05,bit04,bit03,bit02,bit01,bit00);	
  }

  return(1);
E 4
I 4
  sscanf(stringval,"%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d",
	 &bits[15],&bits[14],&bits[13],&bits[12],
	 &bits[11],&bits[10],&bits[9], &bits[8],
	 &bits[7], &bits[6], &bits[5], &bits[4],
	 &bits[3], &bits[2], &bits[1], &bits[0]);
  
  for(j=0; j<16; j++)  res = res | (bits[j] << j);
  
  return(res);
E 4
}

D 4
 
E 4
int
D 4
bit_TST(int bit, int status, int debug)
{          
  int k;

  k = bit_SET(status, debug); 

  switch(bit)
  {
    case    0: if(debug)printf(" case 0");  return(bit00);  break;
    case    1: if(debug)printf(" case 1");  return(bit01);  break;
    case    2: if(debug)printf(" case 2");  return(bit02);  break;
    case    3: if(debug)printf(" case 3");  return(bit03);  break;
    case    4: if(debug)printf(" case 4");  return(bit04);  break;
    case    5: if(debug)printf(" case 5");  return(bit05);  break;
    case    6: if(debug)printf(" case 6");  return(bit06);  break;
    case    7: if(debug)printf(" case 7");  return(bit07);  break;
    case    8: if(debug)printf(" case 8");  return(bit08);  break;             
    case    9: if(debug)printf(" case 9");  return(bit09);  break;
    case   10: if(debug)printf(" case 10"); return(bit10);  break;
    case   11: if(debug)printf(" case 11"); return(bit11);  break;
    case   12: if(debug)printf(" case 12"); return(bit12);  break;
    case   13: if(debug)printf(" case 13"); return(bit13);  break;
    case   14: if(debug)printf(" case 14"); return(bit14);  break;
    case   15: if(debug)printf(" case 15"); return(bit15);  break;
    case   16: if(debug)printf(" case 16"); return(bit16);  break;
    case   17: if(debug)printf(" case 17"); return(bit17);  break;
    case   18: if(debug)printf(" case 18"); return(bit18);  break;
    case   19: if(debug)printf(" case 19"); return(bit19);  break;
    case   20: if(debug)printf(" case 20");  return(bit20);  break;
    case   21: if(debug)printf(" case 21");  return(bit21);  break;
    case   22: if(debug)printf(" case 22");  return(bit22);  break;
    case   23: if(debug)printf(" case 23");  return(bit23);  break;
    case   24: if(debug)printf(" case 24");  return(bit24);  break;
    case   25: if(debug)printf(" case 25");  return(bit25);  break;
    case   26: if(debug)printf(" case 26");  return(bit26);  break;
    case   27: if(debug)printf(" case 27");  return(bit27);  break;
    case   28: if(debug)printf(" case 28");  return(bit28);  break;             
    case   29: if(debug)printf(" case 29");  return(bit29);  break;
    case   30: if(debug)printf(" case 30");  return(bit30);  break;
    case   31: if(debug)printf(" case 31");  return(bit31);  break;
 
    default  :
      printf("  error illegal bit number 0-31 only \n");
      return(-4);
      break;
  }
}


int
sbin_to_int(char *stringval, int debug)
E 4
I 4
bit_TST(int bit, int status)
E 4
{
D 4
  int j,k,l;
  int bits[16]={0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0};

  /* checked the intialization looked okay for my_bit_array
  if(debug)
  {
    j=0;
    while(j < 32)
    {
      printf("bit #%d=%u\n",j,my_bit_array[j]);
      j++;
    }
  }
  */

  if(debug) printf("string=%s \n",stringval);

  sscanf(stringval,"%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d", 
	&bits[15],&bits[14],&bits[13],&bits[12],&bits[11],&bits[10],&bits[9],&bits[8],
	&bits[7], &bits[6], &bits[5], &bits[4], &bits[3], &bits[2], &bits[1],&bits[0]);
       
  j = 0;
  k = 0;
  while(j<16)
  {
    k = my_bit_array[j]*bits[j]+k;    
    if(debug)
    {
      printf(" bit%d =%d array= %d current k=%d\n",
        j,bits[j],my_bit_array[j],k);
    }
    j++;
  }

  return(k);   
E 4
I 4
  if((1 << bit) & status) return(1);
  else                    return(0);
E 4
}

I 5
#else

void
bits_dummy()
{
  return;
}

#endif
E 5
E 1
