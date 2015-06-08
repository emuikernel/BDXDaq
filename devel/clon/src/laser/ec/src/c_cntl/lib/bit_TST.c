
#include <bit_defs.h>


 
int bit_TST(int bit, int status, int debug)
{          
  int k;

   k=bit_SET(status, debug); 

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
  
    default  : printf("  error illigal bit number \n"); return(-4); break;
    }
}
