/* set_tdc_mask.c
 *
 * Set mask into the all discriminators used in FC TDC calibration.
 * Return (-1) if something wrong.
 *
 *   written by:  SP, 12-Nov-1999
 *  modified by:  SP, 31-Aug-2000   < new slot added to CAMAC crate #1 >
 *  modified by:  SP, 6-Aug-2004    < branch number changed to "1" >
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <ca.h>

int main (int argc, char *argv[])
{
int  n[2][23]={{1,2,3,4,5,8,9,10,11,12,14,15,16,17,19,20,21,22,23},\
	       {1,2,3,4,5,6,8,9,10,11,12,14,15,16,17,19,20,21,22,23}};
int  N_of_slots[2]={19,20};
char *roc="camac1";
int  b=1, c, i, ii;
int  ext, lstat, x, q;
int  stat;
int  mask, mask_check;


 
  if ( argc != 2 ) {
    printf("Usage: set_tdc_mask mask \n");
    return (-1); }
  
  mask = strtoul(argv[1],NULL,0);
  /*
    mask = atoi(argv[1]);
    printf("mask = 0x%x, m_adr = 0x%x \n", mask, (int)&mask);
  */
  
  caopen(roc, &stat);
  if (stat!=1) {
    printf("Failed to establish connection with ROC = %s\n", roc);
    return (-1); }
  
  
  for (c=0; c<=1; c++) {
    for (i=0; i < N_of_slots[c]; i++) { 
      cdreg(&ext, b, c, n[c][i], 0);
      cfsa(16, ext, &mask, &q);
      /*
	ctstatus(ext,&lstat);
	x = (~lstat>>1)&1;
	printf("c=%d, n=%2d, q=%d  x=%d \n", c, n[c][i], q, x);
      */


      for (ii=0; ii<=10; ii++) {
	
	cfsa(0, ext, &mask_check, &q);
	
	if ( (mask_check != mask) && (ii<10) ) {
	  cfsa(16, ext, &mask, &q);
	}
	else if ( mask_check == mask ) {
	  ii=11;
	}
	else {
	  printf("\n Error: mask=0x%x did not write correctly", mask);
	  printf("\n        for crate #%d slot #%d \n\n", c, n[c][i]);
	  return (-1);
	}
      }
    }
  }
 
  printf("0\n");
  
  return (0);
}

