h01162
s 00000/00000/00000
d R 1.2 00/09/21 14:12:20 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 laser/ec/src/c_cntl/include/bit_defs_novalue.h
e
s 00052/00000/00000
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

       int BIT00  ;  /* 1st bit or zeroth bit */
       int BIT01  ;  /* 2nd bit               */
       int BIT02  ;  /* 3rd bit               */
       int BIT03  ;  /* 4 bit                 */
       int BIT04  ;  /* 5 bit                 */
       int BIT05  ;  /* 6 bit                 */
       int BIT06  ;  /* 7 bit                 */
       int BIT07  ;  /* 8 bit                 */
       int BIT08  ;  /* 9 bit                 */
       int BIT09  ;  /* 10 bit                */
       int BIT10  ;  /* 11 bit                 */
       int BIT11  ;  /* 12 bit                 */
       int BIT12  ;  /* 13 bit                 */
       int BIT13  ;  /* 14 bit                 */
       int BIT14  ;  /* 15 bit                 */
       int BIT15  ;  /* 16 bit  or 15 bit      */

       int bit00  ;  /* 1st bit or zeroth bit */
       int bit01  ;  /* 2nd bit               */
       int bit02  ;  /* 3rd bit               */
       int bit03  ;  /* 4 bit                 */
       int bit04  ;  /* 5 bit                 */
       int bit05  ;  /* 6 bit                 */
       int bit06  ;  /* 7 bit                 */
       int bit07  ;  /* 8 bit                 */
       int bit08  ;  /* 9 bit                 */
       int bit09  ;  /* 10 bit                */
       int bit10  ;  /* 11 bit                 */
       int bit11  ;  /* 12 bit                 */
       int bit12  ;  /* 13 bit                 */
       int bit13  ;  /* 14 bit                 */
       int bit14  ;  /* 15 bit                 */
       int bit15  ;  /* 16 bit  or 15 bit      */
              
/* ********
 to extract the bit info from int_value
 
     if( (BIT00) & int_value))   bit00=1 ;
     
 thnis is a bit compare where the result is
 the bitwise and of the two values
 
 to set information in int_value  
   
     int_value=BIT00+int_value ;
     
*/      
/*************************************************************************/      

E 1