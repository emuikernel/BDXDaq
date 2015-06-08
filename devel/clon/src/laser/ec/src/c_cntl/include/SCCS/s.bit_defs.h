h50753
s 00000/00000/00000
d R 1.2 00/09/21 14:12:20 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/include/bit_defs.h
e
s 00144/00000/00000
d D 1.1 00/09/21 14:12:19 wolin 1 0
c date and time created 00/09/21 14:12:19 by wolin
e
u
U
f e 0
t
T
I 1
/**************  bit defs for convenience ***********/
/* name them starting at zero                     */

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




#if defined(INIT) 


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

          int  bit16  = 0 ;  /*       */
          int  bit17  = 0 ;  /*       */
          int  bit18  = 0 ;  /*       */
          int  bit19  = 0 ;  /*       */
 
          int  bit20  = 0 ;  /*       */
          int  bit21  = 0 ;  /*       */
          int  bit22  = 0 ;  /*       */
          int  bit23  = 0 ;  /*       */
  
          int  bit24  = 0 ;  /*       */
          int  bit25  = 0 ;  /*       */
          int  bit26  = 0 ;  /*       */
          int  bit27  = 0 ;  /*       */
 
          int  bit28  = 0 ;  /*       */
          int  bit29  = 0 ;  /*       */
          int  bit30  = 0 ;  /*       */
          int  bit31  = 0 ;  /*       */
  

 
#elif !defined(INIT) 

        extern  int  bit00     ;  /* 1st bit or zeroth bit */
        extern  int  bit01     ;  /* 2nd bit               */
        extern  int  bit02     ;  /* 3rd bit               */
        extern  int  bit03     ;  /* 4 bit                 */
        extern  int  bit04     ;  /* 5 bit                 */
        extern  int  bit05     ;  /* 6 bit                 */
        extern  int  bit06     ;  /* 7 bit                 */
        extern  int  bit07     ;  /* 8 bit                 */
        extern  int  bit08     ;  /* 9 bit                 */
        extern  int  bit09     ;  /* 10 bit                */
        extern  int  bit10     ;  /* 11 bit                 */
        extern  int  bit11     ;  /* 12 bit                 */
        extern  int  bit12     ;  /* 13 bit                 */
        extern  int  bit13     ;  /* 14 bit                 */
        extern  int  bit14     ;  /* 15 bit                 */
        extern  int  bit15     ;  /* 16 bit  or 15 bit      */

        extern   int  bit16    ;  /*       */
        extern   int  bit17    ;  /*       */
        extern   int  bit18    ;  /*       */
        extern   int  bit19    ;  /*       */
 
        extern   int  bit20    ;  /*       */
        extern   int  bit21    ;  /*       */
        extern   int  bit22    ;  /*       */
        extern   int  bit23    ;  /*       */
  
        extern   int  bit24    ;  /*       */
        extern   int  bit25    ;  /*       */
        extern   int  bit26    ;  /*       */
        extern   int  bit27    ;  /*       */
 
        extern   int  bit28    ;  /*       */
        extern   int  bit29    ;  /*       */
        extern   int  bit30    ;  /*       */
        extern   int  bit31    ;  /*       */
  

  
#endif

               
/* ********
 to extract the bit info from int_value
 
     if( (BIT00) & int_value))   bit00=1 ;
     
 
 to set information in int_value  
   
     int_value=BIT00+int_value ;
     
*/      
/*************************************************************************/      



E 1
