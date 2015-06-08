/*  clockgen3660.h 
 * 
 *     by Serguei P.
 *     Last update 04/02/99
 */ 

#ifndef _CLOCKGEN3660_H
#define _CLOCKGEN3660_H


/* Library of functions */

int clkgen3660readCSR (int c, int n, int *q);
int clkgen3660writeCSR (int c, int n, int data, int *q);
int clkgen3660readPPC (int c, int n, int *q);
int clkgen3660readCPC (int c, int n, int *q);
int clkgen3660resetPrC (int c, int n, int *q);
int clkgen3660stepsPrC (int c, int n, int *q);
int clkgen3660resetALL (int c, int n, int *q);

/*   RAM functions   ------------------------------------------- */
int clkgen3660readRAP (int c, int n, int *q);
int clkgen3660writeRAP (int c, int n, int data, int *q);
int clkgen3660readFSD (int c, int n, int *q);
int clkgen3660writeFSD (int c, int n, int data, int *q);

/*   LAM functions   ------------------------------------------- */
int clkgen3660enbLAM (int c, int n, int *q);
int clkgen3660disLAM (int c, int n, int *q);
int clkgen3660testLR (int c, int n, int *q);
int clkgen3660testLS (int c, int n, int *q);
int clkgen3660clearLS (int c, int n, int *q);


#endif  /* _CLOCKGEN3660_H */
