#ifndef VXWORKS

/*
 * bits.c
 *
 */

#include <stdio.h>

int
sbin_to_int(char *stringval)
{
  int j, res = 0;
  int bits[16] = {0,0,0,0,  0,0,0,0,  0,0,0,0,  0,0,0,0};
  
  sscanf(stringval,"%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d%1d",
	 &bits[15],&bits[14],&bits[13],&bits[12],
	 &bits[11],&bits[10],&bits[9], &bits[8],
	 &bits[7], &bits[6], &bits[5], &bits[4],
	 &bits[3], &bits[2], &bits[1], &bits[0]);
  
  for(j=0; j<16; j++)  res = res | (bits[j] << j);
  
  return(res);
}

int
bit_TST(int bit, int status)
{
  if((1 << bit) & status) return(1);
  else                    return(0);
}

#else

void
bits_dummy()
{
  return;
}

#endif
