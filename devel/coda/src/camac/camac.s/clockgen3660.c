
#ifdef VXWORKS

/*  clockgen3660.c ( for loading in VxWorks )
 * 
 *     A little library for Programmable Clock Generator 3660.
 * 
 *     by Serguei P.
 *     Last update 04/02/99
 */ 

#include <vxWorks.h> 
#include <stdio.h>
#include <ca.h>
#include "genROCparm.h"
#include "clockgen3660.h"


/*---------------------------------------------------------------*/
/*--------------     Clock Generator 3660     -------------------*/
/*---------------------------------------------------------------*/

int clkgen3660readCSR (int c, int n, int *q)
{     
  int ext, data=0;

  cdreg(&ext, BB, c, n, 0);
  cfsa(0, ext, &data, q);
  return(data);
}

int clkgen3660writeCSR (int c, int n, int data, int *q)
{     
  int ext;

  cdreg(&ext, BB, c, n, 0);
  cfsa(16, ext, &data, q);
  return(data);
}

int clkgen3660readPPC (int c, int n, int *q)
{     
  int ext, data=0;

  cdreg(&ext, BB, c, n, 1);
  cfsa(0, ext, &data, q);
  return(data);
}

int clkgen3660readCPC (int c, int n, int *q)
{     
  int ext, data=0;

  cdreg(&ext, BB, c, n, 2);
  cfsa(0, ext, &data, q);
  return(data);
}

int clkgen3660resetPrC (int c, int n, int *q)
{     
  int ext, data=0;

  cdreg(&ext, BB, c, n, 0);
  cfsa(25, ext, &data, q);
  return(data);
}

int clkgen3660stepsPrC (int c, int n, int *q)
{     
  int ext, data=0;

  cdreg(&ext, BB, c, n, 1);
  cfsa(25, ext, &data, q);
  return(data);
}

/*   Places the module in Inactive state, clears the LAM Status, 
     LAM Request, Inhibit, and CSR   --------------------------- */

int clkgen3660resetALL (int c, int n, int *q)
{     
  int ext, data=0;

  cdreg(&ext, BB, c, n, 2);
  cfsa(25, ext, &data, q);
  return(data);
}

/*   RAM functions   ------------------------------------------- */

int clkgen3660readRAP (int c, int n, int *q)
{     
  int ext, data=0;

  cdreg(&ext, BB, c, n, 0);
  cfsa(1, ext, &data, q);
  return(data);
}

int clkgen3660writeRAP (int c, int n, int data, int *q)
{     
  int ext;

  cdreg(&ext, BB, c, n, 0);
  cfsa(17, ext, &data, q);
  return(data);
}

int clkgen3660readFSD (int c, int n, int *q)
{     
  int ext, data=0;

  cdreg(&ext, BB, c, n, 1);
  cfsa(1, ext, &data, q);
  return(data);
}

int clkgen3660writeFSD (int c, int n, int data, int *q)
{     
  int ext;

  cdreg(&ext, BB, c, n, 1);
  cfsa(17, ext, &data, q);
  return(data);
}

/*   LAM functions   ------------------------------------------- */

int clkgen3660enbLAM (int c, int n, int *q)
{     
  int ext, data=0;

  cdreg(&ext, BB, c, n, 0);
  cfsa(26, ext, &data, q);
  return(data);
}

int clkgen3660disLAM (int c, int n, int *q)
{     
  int ext, data=0;

  cdreg(&ext, BB, c, n, 0);
  cfsa(24, ext, &data, q);
  return(data);
}

int clkgen3660testLR (int c, int n, int *q)
{     
  int ext, data=0;

  cdreg(&ext, BB, c, n, 0);
  cfsa(8, ext, &data, q);
  return(data);
}

int clkgen3660testLS (int c, int n, int *q)
{     
  int ext, data=0;

  cdreg(&ext, BB, c, n, 0);
  cfsa(27, ext, &data, q);
  return(data);
}

int clkgen3660clearLS (int c, int n, int *q)
{     
  int ext, data=0;

  cdreg(&ext, BB, c, n, 0);
  /*cfsa1(10, ext, &data, q);*/
  cfsa(10, ext, &data, q);
  return(data);
}

/*---------------------------------------------------------------*/
/*------------   Tests Clock Generator 3660   -------------------*/
/*---------------------------------------------------------------*/

void clocktest1()
{
  int c=2, n=19;
  int data, q;

/* Place 3660 in Inactive state; clear LR, LS, Inh. and CSR -----*/
  data = clkgen3660resetALL (c, n, &q);
  printf("clkgen3660resetALL  ");
  printf("       data=0x%8x, (d)=%8d, q=%d \n", data, data, q);
  
/* Set the 3660 RAM Address Pointer (RAP) to zero ---------------*/
  data = clkgen3660writeRAP (c, n, 0, &q);
  printf("clkgen3660writeRAP  ");
  printf("       data=0x%8x, (d)=%8d, q=%d \n", data, data, q);

/* Write program to RAM (1 step = 4 words) ----------------------*/
  data = clkgen3660writeFSD (c, n, 10000, &q);
  printf("clkgen3660writeFSD-1");
  printf("       data=0x%8x, (d)=%8d, q=%d \n", data, data, q);

  data = clkgen3660writeFSD (c, n, 0x81, &q);
  printf("clkgen3660writeFSD-2");
  printf("       data=0x%8x, (d)=%8d, q=%d \n", data, data, q);

  data = clkgen3660writeFSD (c, n, 100, &q);
  printf("clkgen3660writeFSD-3");
  printf("       data=0x%8x, (d)=%8d, q=%d \n", data, data, q);

  data = clkgen3660writeFSD (c, n, 0, &q);
  printf("clkgen3660writeFSD-4");
  printf("       data=0x%8x, (d)=%8d, q=%d \n", data, data, q);

/* Initialize the 3660 RAM Address Pointer to first step --------*/
  data = clkgen3660writeRAP (c, n, 0, &q);
  printf("clkgen3660writeRAP  ");
  printf("       data=0x%8x, (d)=%8d, q=%d \n", data, data, q);
  
/* Activate the 3660 --------------------------------------------*/ 
  /*
	CSR should be written once with all necessary option inside.
                                       The following does not work:
  clkgen3660writeCSR (c, n, 0x4, &q);
  clkgen3660writeCSR (c, n, 0x1, &q);
  clkgen3660writeCSR (c, n, 0x40, &q);
  clkgen3660writeCSR (c, n, 0x80, &q);
                                        It should be written like:
  clkgen3660writeCSR (c, n, 0xc5, &q);
  */
  data = clkgen3660writeCSR (c, n, 0x85, &q);
  printf("clkgen3660writeCSR  ");
  printf("       data=0x%8x, (d)=%8d, q=%d \n", data, data, q);
}

/*---------------------------------------------------------------*/

void clocktest2()
{
  int c=2, n=19;
  int data, q;

/* Place 3660 in Inactive state; clear LR, LS, Inh. and CSR -----*/
  printf("       Before    &q=%8x  \n", (unsigned int)&q);
  data = clkgen3660resetALL (c, n, &q);
  printf("clkgen3660resetALL  ");
  printf("  &q=%8x", (unsigned int)&q);
  printf("       data=0x%8x, (d)=%8d, q=%d \n", data, data, q);
  
/* Set the 3660 RAM Address Pointer (RAP) to zero ---------------*/
  data = clkgen3660writeRAP (c, n, 0, &q);
  printf("clkgen3660writeRAP  ");
  printf("  &q=%8x", (unsigned int)&q);
  printf("       data=0x%8x, (d)=%8d, q=%d \n", data, data, q);

/* Write program to RAM (1 step = 4 words) ----------------------*/
/* --------  Step # 1  ----------------------------------------- */
  data = clkgen3660writeFSD (c, n, 40000, &q);
  printf("clkgen3660writeFSD-1");
  printf("  &q=%8x", (unsigned int)&q);
  printf("       data=0x%8x, (d)=%8d, q=%d \n", data, data, q);

  data = clkgen3660writeFSD (c, n, 0x11, &q);
  printf("clkgen3660writeFSD-2");
  printf("  &q=%8x", (unsigned int)&q);
  printf("       data=0x%8x, (d)=%8d, q=%d \n", data, data, q);

  data = clkgen3660writeFSD (c, n, 20, &q);
  printf("clkgen3660writeFSD-3");
  printf("  &q=%8x", (unsigned int)&q);
  printf("       data=0x%8x, (d)=%8d, q=%d \n", data, data, q);

  data = clkgen3660writeFSD (c, n, 0, &q);
  printf("clkgen3660writeFSD-4");
  printf("  &q=%8x", (unsigned int)&q);
  printf("       data=0x%8x, (d)=%8d, q=%d \n", data, data, q);

/* --------  Step # 2  ----------------------------------------- */
  data = clkgen3660writeFSD (c, n, 40000, &q);
  printf("clkgen3660writeFSD-1");
  printf("  &q=%8x", (unsigned int)&q);
  printf("       data=0x%8x, (d)=%8d, q=%d \n", data, data, q);

  data = clkgen3660writeFSD (c, n, 0x81, &q);
  printf("clkgen3660writeFSD-2");
  printf("  &q=%8x", (unsigned int)&q);
  printf("       data=0x%8x, (d)=%8d, q=%d \n", data, data, q);

  data = clkgen3660writeFSD (c, n, 25, &q);
  printf("clkgen3660writeFSD-3");
  printf("  &q=%8x", (unsigned int)&q);
  printf("       data=0x%8x, (d)=%8d, q=%d \n", data, data, q);

  data = clkgen3660writeFSD (c, n, 0, &q);
  printf("clkgen3660writeFSD-4");
  printf("  &q=%8x", (unsigned int)&q);
  printf("       data=0x%8x, (d)=%8d, q=%d \n", data, data, q);

/* Initialize the 3660 RAM Address Pointer to first step --------*/
  data = clkgen3660writeRAP (c, n, 0, &q);
  printf("clkgen3660writeRAP  ");
  printf("  &q=%8x", (unsigned int)&q);
  printf("       data=0x%8x, (d)=%8d, q=%d \n", data, data, q);
  
/* Activate the 3660 --------------------------------------------*/ 
  data = clkgen3660writeCSR (c, n, 0xc5, &q);
  printf("clkgen3660writeCSR  ");
  printf("  &q=%8x", (unsigned int)&q);
  printf("       data=0x%8x, (d)=%8d, q=%d \n", data, data, q);
}

/*---------------------------------------------------------------*/

void sttt()
{
int d, qq;
int c=2, n=19;

  qq=0;
  
  d=clkgen3660readPPC(c,n,&qq);
  printf("clkgen3660readPPC     d= %8d,  &q=%8x,  q=%d \n", \
	 d, (unsigned int)&qq, qq);
  
  d=clkgen3660readCPC(c,n,&qq);
  printf("clkgen3660readCPC     d= %8d,  &q=%8x,  q=%d \n", \
	 d, (unsigned int)&qq, qq);
  
  d=clkgen3660testLS(c,n,&qq);
  printf("clkgen3660testLS      d= %8d,  &q=%8x,  q=%d \n", \
	 d, (unsigned int)&qq, qq);
  
  d=clkgen3660testLR(c,n,&qq);
  printf("clkgen3660testLR      d= %8d,  &q=%8x,  q=%d \n", \
	 d, (unsigned int)&qq, qq);
  
}

/*---------------------------------------------------------------*/

void stt2()
{
int d, qq;
int c=2, n=19;

  d=clkgen3660readCSR (c,n,&qq);
  printf("clkgen3660readCSR      d= %8x,  &q=%8x,  q=%d \n", \
	 d, (unsigned int)&qq, qq);

}

#else

clockgen3660_dummy()
{
  return;
}

#endif
