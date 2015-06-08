/*----------------------------------------------------------------------------*
 *  Copyright (c) 1998        Southeastern Universities Research Association, *
 *                            Thomas Jefferson National Accelerator Facility  *
 *                                                                            *
 *    This software was developed under a United States Government license    *
 *    described in the NOTICE file included as part of this distribution.     *
 *                                                                            *
 *    Author:  Carl Timmer                                                    *
 *             timmer@jlab.org                   Jefferson Lab, MS-12H        *
 *             Phone: (757) 269-5130             12000 Jefferson Ave.         *
 *             Fax:   (757) 269-5800             Newport News, VA 23606       *
 *                                                                            *
 *----------------------------------------------------------------------------*
 *
 * Description:
 *      Routines for handling 64-bit integers.
 *
 *----------------------------------------------------------------------------*/
 

#include <stdio.h>
#include <string.h>

#include "et_private.h"

/*****************************************************/
/*                64 BIT COUNTERS                    */
/*****************************************************/

/*
 * Big integers are currently implemented as a structure
 * of two unsigned ints (et_bigint). These routines implement
 * addition, subtraction, equality, mod, initialization, & printing.
 * They are meant to count large, positive numbers.
 */
et_bigint et_bigint_add(et_bigint bi, unsigned int add)
{
  if (bi.lowint > UINT_MAX - add) {
    bi.highint++;
  }
  bi.lowint += add;
  return bi;
}

et_bigint et_bigint_sub(et_bigint bi, unsigned int sub)
{
  if (bi.lowint < sub) {
    if (bi.highint < 1) {
      /* lower limit on bi value is 0 */
      bi.lowint = 0;
    }
    else {
      bi.highint--;
      bi.lowint = (UINT_MAX - sub) + bi.lowint;
    }
  }
  else {
    bi.lowint -= sub;
  }
  
  return bi;
}

int et_bigint_equal(et_bigint bi_1, et_bigint bi_2)
{
  if ((bi_1.lowint == bi_2.lowint) && (bi_1.highint == bi_2.highint)) {
    return 1;
  }
  return 0;
}

double et_bigint_diff(et_bigint bi_1,  et_bigint bi_2)
{
  double bi;
    
  bi = (((double)bi_1.highint - bi_2.highint) * ((double)UINT_MAX + 1))
        + bi_1.lowint - bi_2.lowint;
    
  return bi;
}

unsigned int et_bigint_mod(et_bigint bi, unsigned int mod)
{
  return (((bi.highint % mod) + (bi.lowint % mod)) % mod);
}

void et_bigint_init(et_bigint *bi)
{
  bi->highint = 0;
  bi->lowint  = 0;
}

void et_bigint_print(et_bigint bi, const char *format)
{
  double big;
  
  big = ((double)bi.highint * ((double)UINT_MAX + 1)) + bi.lowint;
  printf(format, big);
}

void et_bigint_string(et_bigint bi, char *string)
{
  double big;
  
  if (bi.highint) {
    big = ((double)bi.highint * ((double)UINT_MAX + 1)) + bi.lowint;
    sprintf(string, "%.0f", big);
  }
  else {
    sprintf(string, "%u", bi.lowint);
  }
}


