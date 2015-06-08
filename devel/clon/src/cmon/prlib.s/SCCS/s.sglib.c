h55776
s 00104/00005/00414
d D 1.9 03/12/16 22:18:12 boiarino 10 9
c .
e
s 00003/00002/00416
d D 1.8 03/04/17 16:50:39 boiarino 9 8
c *** empty log message ***
e
s 00000/00001/00418
d D 1.7 02/05/01 00:06:45 boiarino 8 7
c minor
c 
e
s 00001/00000/00418
d D 1.6 02/04/30 23:58:57 boiarino 7 6
c minor
c 
e
s 00000/00008/00418
d D 1.5 02/04/30 17:06:16 boiarino 6 5
c move some macros to header file
c 
e
s 00012/01150/00414
d D 1.4 02/04/30 15:46:05 boiarino 5 4
c move low level functions to sgutil.c
c 
e
s 01043/00424/00521
d D 1.3 02/04/30 14:22:37 boiarino 4 3
c new version - a lot of changes
c 
c 
e
s 00000/00028/00945
d D 1.2 01/11/26 13:36:30 boiarino 3 1
c remove some printfs and some new code
c which will be included later
c 
e
s 00000/00000/00000
d R 1.2 01/11/19 18:57:06 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/cmon/prlib.s/sglib.c
e
s 00973/00000/00000
d D 1.1 01/11/19 18:57:05 boiarino 1 0
c date and time created 01/11/19 18:57:05 by boiarino
e
u
U
f e 0
t
T
I 1

D 4
/* sglib.c - Dave Heddle's segment finding algorithm */
E 4
I 4
D 5
/* sglib.c - segment finding library */
E 5
I 5
/* sglib.c - main functions of the segment finding library */
E 5
E 4

I 4

E 4
#include <stdio.h>
#include <string.h>
#include <limits.h>
I 10
#include "bos.h"
E 10
I 7
D 8
#include "uthbook.h"
E 8
E 7
I 4
#include "prlib.h"
E 4
#include "sglib.h"
#include "sdakeys.h"
SDAkeys sdakeys_;

D 4
#define LONGLONG_BIT 64
E 4
I 4
#define DEBUG 0
D 5
#define ABS(x)     ( (x) < 0 ? -(x) : (x) )
E 5
E 4

D 5
#ifdef HPUX
#define LONG_BIT 32
#endif
#ifdef Linux
#define LONG_BIT 32
#endif
E 5
I 5
D 6
#define GOODHIT \
  wire > 0 && wire < 193 && \
  layer > 0 && layer < 37 && layer != 5 && layer != 6
E 5

I 4
D 5
#define NW128 4 /* # of words in 128 bits */
#define NW192 6 /* # of words in 192 bits */

E 4
#define MIN(x,y) ((x) < (y) ? (x) : (y))
I 4
#define MAX(x,y) ((x) > (y) ? (x) : (y))
E 4

E 5
#define BREAKSHORT(sw, low, high) \
  high = (unsigned char)(((sw) >> 8) & 0x00FF); \
  low = (unsigned char)((sw) & 0x00FF)

E 6
D 5
#define GOODHIT \
  wire > 0 && wire < 193 && \
  layer > 0 && layer < 37 && layer != 5 && layer != 6

I 4
#define NADR   0xFFF
static unsigned int look[NADR];   /* 3 summed layers lookup table */
static unsigned int look3[NADR];  /* 3 row layers lookup table */
E 4

E 5
static unsigned long nlayer1[6][3][2];
static unsigned long nwires1[6][3][2];

I 4
D 5
#define NSHIFT  29
static int dshift[NSHIFT][5];
/*
  1 >>>  1  0  0  0  0
  2 >>>  0  0  1  0  0
  3 >>> -1  0 -1  0  1
  4 >>>  0  0  1  0  0
  5 >>>  0  0  0  1  0
  6 >>>  1  0  0 -1  0
  7 >>>  0  0  0  1  0
  8 >>>  0  1  0  0  0
  9 >>> -1 -1  0  0  1
 10 >>>  1  0  0  0  0
 11 >>>  0  1  0  0  0
 12 >>>  0  0  1  0  0
 13 >>>  0  0  0  1  0
 14 >>>  0  0  0  0  1
 15 >>>  0  0  0  1  1
 16 >>>  0  0  1  0  0
 17 >>>  0  1  0  0  0
 18 >>>  1  0  0  0  0
 19 >>> -1 -1  0  0  1
 20 >>>  0  0  0  1  0
 21 >>>  0  1  0  0  0
 22 >>>  1  0  0  0  0
 23 >>>  0  0  1  0  0
 24 >>> -1  0  0  0  1
 25 >>>  1  0  0  0  0
 26 >>>  0  1  0  0  0
 27 >>>  0  0  0  1  0
 28 >>>  0  0  1  0  0
*/
const unsigned int shift[NSHIFT][6] = {
                          /* l1,l2,l3,l4,l5,p02 */
                            { 0, 0, 0, 0, 0,  0 },
                            { 1, 0, 0, 0, 0,  1 },
                            { 1, 0, 1, 0, 0,  1 },
                            { 0, 0, 0, 0, 1,  0 },
                            { 0, 0, 1, 0, 1,  0 },
                            { 0, 0, 1, 1, 1,  0 },
                            { 1, 0, 1, 0, 1,  1 },
                            { 1, 0, 1, 1, 1,  1 },
                            { 1, 1, 1, 1, 1,  2 },
                            { 0, 0, 1, 1, 2,  0 },
                            { 1, 0, 1, 1, 2,  1 },
                            { 1, 1, 1, 1, 2,  2 },
                            { 1, 1, 2, 1, 2,  2 },
                            { 1, 1, 2, 2, 2,  2 },
                            { 1, 1, 2, 2, 3,  2 },
                            { 1, 1, 2, 3, 4,  2 },
                            { 1, 1, 3, 3, 4,  2 },
                            { 1, 2, 3, 3, 4,  3 },
                            { 2, 2, 3, 3, 4,  4 },
                            { 1, 1, 3, 3, 5,  2 },
                            { 1, 1, 3, 4, 5,  2 },
                            { 1, 2, 3, 4, 5,  3 },
                            { 2, 2, 3, 4, 5,  4 },
                            { 2, 2, 4, 4, 5,  4 },
                            { 1, 2, 4, 4, 6,  3 },
                            { 2, 2, 4, 4, 6,  4 },
                            { 2, 3, 4, 4, 6,  5 },
                            { 2, 3, 4, 5, 6,  5 },
                            { 2, 3, 5, 5, 6,  5 } };

E 4
/********************
  USEFUL BINARY OPS
*********************/

D 4
/* prints out an unsigned long in binary format
   send NULL for ktest to ignore */
E 4
I 4
/* prints out an unsigned long in binary format */
E 4

E 5
void
D 4
BinaryPrint(unsigned long k, unsigned long *ktest)
E 4
I 4
D 5
BinaryPrint32(unsigned long k)
E 4
{
D 4
  unsigned long jj;
  int           i;
  static int    lastbit;
  static unsigned long j = 1;
E 4
I 4
  int i, lastbit;
  unsigned long j, jj;
E 4

D 4
  if(j == 1)
  {
    lastbit = 8*sizeof(long) - 1;
    for (i = 0; i < lastbit; i++) j *= 2;
  }
E 4
I 4
  j = 1;
  lastbit = 8*sizeof(long) - 1;
  for(i = 0; i < lastbit; i++) j *= 2;
E 4

  jj = j;
  for(i = lastbit; i >=0; i--)
  {
D 4
    if(ktest)
    {
      if(*ktest & jj)
      {
        if(k & jj) fprintf(stdout, "*"); /* both */
        else fprintf(stdout, "X"); /* test only */
      }
      else
      {
        if(k & jj) fprintf(stdout, "|"); /* real on */
        else fprintf(stdout, "o"); /* real off */
      }
    }
    else if (k & jj) fprintf(stdout, "|");
    else fprintf(stdout, "o");
E 4
I 4
    if(k & jj) fprintf(stdout, "|");
    else       fprintf(stdout, "o");
E 4
    
    jj = jj >> 1;
  }

  return;
}


I 4

E 4
/*********************************/
/* Word192 and Word128 functions */
/*********************************/

void
PrintWord128(Word128 *hw)
{
  int i;
  unsigned long *w;

  w = (unsigned long *)hw->words;
  printf("128> ");
D 4
  for(i=3; i>=0; i--)
  {
    BinaryPrint(w[i], NULL);
    printf(" ");
  }
E 4
I 4
  for(i=3; i>=0; i--) {BinaryPrint32(w[i]); printf(" ");}
E 4
  printf("\n");

  return;
}
void
PrintWord192(Word192Ptr hw)
{
  int i;
  unsigned long *w;

I 4
  w = (unsigned long *)hw->words;
E 4
  printf("192> ");
D 4
  for (i=5; i>=0; i--)
  {
    BinaryPrint(hw->words[i], NULL);
    printf(" ");
  }
E 4
I 4
  for(i=5; i>=0; i--) {BinaryPrint32(w[i]); printf(" ");}
E 4
  printf("\n");

  return;
}

void
CopyWord128(Word128 *hws, Word128 *hwd)
{
D 4
  memcpy(hwd->words, hws->words, 2*sizeof(long long));
E 4
I 4
  int i;
  unsigned long *a = (unsigned long *)hws;
  unsigned long *b = (unsigned long *)hwd;
  for(i=0; i<4; i++) b[i] = a[i];

  /*memcpy(hwd->words, hws->words, 4*sizeof(long)); dol'she!*/

E 4
  return;
}
void
CopyWord192(Word192Ptr hws, Word192Ptr hwd)
{
D 4
  memcpy(hwd->words, hws->words, 6*sizeof(long));
E 4
I 4
  int i;
  unsigned long *a = (unsigned long *)hws;
  unsigned long *b = (unsigned long *)hwd;
  for(i=0; i<6; i++) b[i] = a[i];

  /*memcpy(hwd->words, hws->words, 6*sizeof(long)); dol'she!*/

E 4
  return;
}

void
ANDWord128(Word128 *hwa, Word128 *hwb, Word128 *hwc)
{
  int i;
D 4
  unsigned long long *a = (unsigned long long *)hwa;
  unsigned long long *b = (unsigned long long *)hwb;
  unsigned long long *c = (unsigned long long *)hwc;
  for(i=0; i<2; i++) *c++ = (*a++) & (*b++);
E 4
I 4
  unsigned long *a = (unsigned long *)hwa;
  unsigned long *b = (unsigned long *)hwb;
  unsigned long *c = (unsigned long *)hwc;
  for(i=0; i<4; i++) c[i] = a[i] & b[i];
E 4

  return;
}
void
ANDWord192(Word192Ptr hwa, Word192Ptr hwb, Word192Ptr hwc)
{
  int i;
D 4
  unsigned long long *a = (unsigned long long *)hwa;
  unsigned long long *b = (unsigned long long *)hwb;
  unsigned long long *c = (unsigned long long *)hwc;
  for(i=0; i<3; i++) *c++ = (*a++) & (*b++);
  /*for(i=0; i<6; i++) hwc->words[i] = hwa->words[i] & hwb->words[i];*/
E 4
I 4
  unsigned long *a = (unsigned long *)hwa;
  unsigned long *b = (unsigned long *)hwb;
  unsigned long *c = (unsigned long *)hwc;
  for(i=0; i<6; i++) c[i] = a[i] & b[i];
E 4
  return;
}

void
ORWord128(Word128 *hwa, Word128 *hwb, Word128 *hwc)
{
  int i;
D 4
  unsigned long long *a = (unsigned long long *)hwa;
  unsigned long long *b = (unsigned long long *)hwb;
  unsigned long long *c = (unsigned long long *)hwc;
  for(i=0; i<2; i++) *c++ = (*a++) | (*b++);
E 4
I 4
  unsigned long *a = (unsigned long *)hwa;
  unsigned long *b = (unsigned long *)hwb;
  unsigned long *c = (unsigned long *)hwc;
  for(i=0; i<4; i++) c[i] = a[i] | b[i];
E 4

  return;
}
void
ORWord192(Word192Ptr hwa, Word192Ptr hwb, Word192Ptr hwc)
{
  int i;
D 4
  unsigned long long *a = (unsigned long long *)hwa;
  unsigned long long *b = (unsigned long long *)hwb;
  unsigned long long *c = (unsigned long long *)hwc;
  for(i=0; i<3; i++) *c++ = (*a++) | (*b++);
  /*for(i=0; i<6; i++) hwc->words[i] = hwa->words[i] | hwb->words[i];*/
E 4
I 4
  unsigned long *a = (unsigned long *)hwa;
  unsigned long *b = (unsigned long *)hwb;
  unsigned long *c = (unsigned long *)hwc;
  for(i=0; i<6; i++) c[i] = a[i] | b[i];
E 4
  return;
}

void
XORWord128(Word128 *hwa, Word128 *hwb, Word128 *hwc)
{
  int i;
D 4
  unsigned long long *a = (unsigned long long *)hwa;
  unsigned long long *b = (unsigned long long *)hwb;
  unsigned long long *c = (unsigned long long *)hwc;
  for(i=0; i<2; i++) *c++ = (*a++) ^ (*b++);
E 4
I 4
  unsigned long *a = (unsigned long *)hwa;
  unsigned long *b = (unsigned long *)hwb;
  unsigned long *c = (unsigned long *)hwc;
  for(i=0; i<4; i++) c[i] = a[i] ^ b[i];
E 4

  return;
}
void
XORWord192(Word192Ptr hwa, Word192Ptr hwb, Word192Ptr hwc)
{
  int i;
D 4
  unsigned long long *a = (unsigned long long *)hwa;
  unsigned long long *b = (unsigned long long *)hwb;
  unsigned long long *c = (unsigned long long *)hwc;
  for(i=0; i<3; i++) *c++ = (*a++) ^ (*b++);
  /*for(i=0; i<6; i++) hwc->words[i] = hwa->words[i] ^ hwb->words[i];*/
E 4
I 4
  unsigned long *a = (unsigned long *)hwa;
  unsigned long *b = (unsigned long *)hwb;
  unsigned long *c = (unsigned long *)hwc;
  for(i=0; i<6; i++) c[i] = a[i] ^ b[i];
E 4
  return;
}

D 4
void
RightShiftWord128(Word128 *hw, int n)
{
  int i, shift;
  unsigned long *w = (unsigned long *)hw->words;

  shift = LONG_BIT - n;
  for(i=0; i<3; i++)
  {
    w[i] >>= n;
    w[i] |= w[i+1] << shift;
  }
  w[3] >>= n;

  return;
E 4
I 4
#define RightShiftWord128(hw, n) \
{ \
  int ii, shift; \
  unsigned long *w = (unsigned long *)(hw)->words; \
  shift = LONG_BIT - n; \
  for(ii=0; ii<3; ii++) \
  { \
    w[ii] >>= n; \
    w[ii] |= w[ii+1] << shift; \
  } \
  w[3] >>= n; \
E 4
}
D 4
void
RightShiftWord192(Word192Ptr hw, int n)
{
  int i, shift;
  unsigned long *w = (unsigned long *)hw->words;
E 4
I 4
#define RightShiftWord192(hw, n) \
{ \
  int ii, shift; \
  unsigned long *w = (unsigned long *)(hw)->words; \
  shift = LONG_BIT - n; \
  for(ii=0; ii<5; ii++) \
  { \
    w[ii] >>= n; \
    w[ii] |= w[ii+1] << shift; \
  } \
  w[5] >>= n; \
}
E 4

D 4
  shift = LONG_BIT - n;
  for(i=0; i<5; i++)
  {
    w[i] >>= n;
    w[i] |= w[i+1] << shift;
  }
  w[5] >>= n;

  return;
E 4
I 4
#define LeftShiftWord128(hw, n) \
{ \
  int ii, shift; \
  unsigned long *w = (unsigned long *)(hw)->words; \
  shift = LONG_BIT - n; \
  for(ii=3; ii>0; ii--) \
  { \
    w[ii] <<= n; \
    w[ii] |= w[ii-1] >> shift; \
  } \
  w[0] <<= n; \
E 4
}
I 4
#define LeftShiftWord192(hw, n) \
{ \
  int ii, shift; \
  unsigned long *w = (unsigned long *)(hw)->words; \
  shift = LONG_BIT - n; \
  for(ii=5; ii>0; ii--) \
  { \
    w[ii] <<= n; \
    w[ii] |= w[ii-1] >> shift; \
  } \
  w[0] <<= n; \
}
E 4

D 4
void
LeftShiftWord128(Word128 *hw, int n)
{
  int i, shift;
  unsigned long *w = (unsigned long *)hw->words;

  shift = LONG_BIT - n;
  for(i=3; i>0; i--)
  {
    w[i] <<= n;
    w[i] |= w[i-1] >> shift;
  }
  w[0] <<= n;

  return;
E 4
I 4
/**************************/
/* start of special cases */
#define RIGHTSHIFT(hw,index,nbit) \
{ \
  int ii; \
  unsigned long *w = (unsigned long *)(hw)->words; \
  for(ii=0; ii<index; ii++) \
  { \
    w[ii] >>= nbit; \
    w[ii] |= w[ii+1] << (LONG_BIT-nbit); \
  } \
  w[index] >>= nbit; \
E 4
}
D 4
void
LeftShiftWord192(Word192Ptr hw, int n)
{
  int i, shift;
  unsigned long *w = (unsigned long *)hw->words;
E 4
I 4
#define RightShiftWord192_00(hw) ;
#define RightShiftWord192_01(hw) RIGHTSHIFT(hw,5,1)
#define RightShiftWord192_02(hw) RIGHTSHIFT(hw,5,2)
#define RightShiftWord192_03(hw) RIGHTSHIFT(hw,5,3)
#define RightShiftWord128_00(hw) ;
#define RightShiftWord128_01(hw) RIGHTSHIFT(hw,3,1)
#define RightShiftWord128_02(hw) RIGHTSHIFT(hw,3,2)
#define RightShiftWord128_03(hw) RIGHTSHIFT(hw,3,3)
E 4

D 4
  shift = LONG_BIT - n;
  for(i=5; i>0; i--)
  {
    w[i] <<= n;
    w[i] |= w[i-1] >> shift;
  }
  w[0] <<= n;

  return;
E 4
I 4
#define LEFTSHIFT(hw,index,nbit) \
{ \
  int ii; \
  unsigned long *w = (unsigned long *)(hw)->words; \
  for(ii=index; ii>0; ii--) \
  { \
    w[ii] <<= nbit; \
    w[ii] |= w[ii-1] >> (LONG_BIT-nbit); \
  } \
  w[0] <<= nbit; \
E 4
}
I 4
#define LeftShiftWord192_00(hw) ;
#define LeftShiftWord192_01(hw) LEFTSHIFT(hw,5,1)
#define LeftShiftWord192_02(hw) LEFTSHIFT(hw,5,2)
#define LeftShiftWord192_03(hw) LEFTSHIFT(hw,5,3)
#define LeftShiftWord128_00(hw) ;
#define LeftShiftWord128_01(hw) LEFTSHIFT(hw,3,1)
#define LeftShiftWord128_02(hw) LEFTSHIFT(hw,3,2)
#define LeftShiftWord128_03(hw) LEFTSHIFT(hw,3,3)
/* end of special cases */
/************************/
E 4

I 4

E 4
Boolean
CheckBitWord128(Word128 *hw, int n)
{
  unsigned long *w = (unsigned long *)hw->words;
  int whatword, whatbit;
  unsigned long mask = 1;

  whatword = n / LONG_BIT;
  whatbit = n % LONG_BIT;
  /*if((whatword < 0) || (whatword >= 4)) return(False);*/
  mask <<= whatbit;

  return((w[whatword] & mask) == mask);
}
Boolean
CheckBitWord192(Word192Ptr hw, int n)
{
  unsigned long *w = (unsigned long *)hw->words;
  int whatword, whatbit;
  unsigned long mask = 1;

  whatword = n / LONG_BIT;
  whatbit = n % LONG_BIT;
  /*if((whatword < 0) || (whatword >= 6)) return(False);*/
  mask <<= whatbit;

  return((w[whatword] & mask) == mask);
}

void
SetBitWord128(Word128 *hw, int n)
{
  unsigned long *w = (unsigned long *)hw->words;
  int whatword, whatbit;
  unsigned long mask = 1;

  whatword = n / LONG_BIT;
  whatbit = n % LONG_BIT;
  /*if((whatword < 0) || (whatword >= 4)) return;*/
  mask <<= whatbit;
  w[whatword] |= mask;

  return;
}
void
SetBitWord192(Word192Ptr hw, int n)
{
  unsigned long *w = (unsigned long *)hw->words;
  int whatword, whatbit;
  unsigned long mask = 1;

  whatword = n / LONG_BIT;
  whatbit = n % LONG_BIT;
  /*if((whatword < 0) || (whatword >= 6)) return;*/
  mask <<= whatbit;
  w[whatword] |= mask;

  return;
}

void
ClearWord128(Word128 *hw)
{
D 4
  memset(hw->words,0,2*sizeof(long long));
E 4
I 4
  memset(hw->words,0,4*sizeof(long));/* dol'she?*/
E 4

  return;
}
void
ClearWord192(Word192Ptr hw)
{
D 4
  memset(hw->words,0,6*sizeof(long));
E 4
I 4
  memset(hw->words,0,6*sizeof(long));/* dol'she?*/
E 4

  return;
}

void
NegateWord128(Word128 *hw)
{
  int i;
D 4
  unsigned long long *w = hw->words;
E 4
I 4
  unsigned long *w;
E 4

D 4
  for(i=0; i<2; i++) w[i] = ~w[i];
E 4
I 4
  w = hw->words;
  for(i=0; i<4; i++) w[i] = ~w[i];
E 4

  return;
}
void
NegateWord192(Word192Ptr hw)
{
D 4
  unsigned long *w;
E 4
  int i;
I 4
  unsigned long *w;
E 4

  w = hw->words;
  for(i=0; i<6; i++) w[i] = ~w[i];

  return;
}

void
BleedRightWord192(Word192Ptr hw, int n)
{
  int j, m, k;
  Word192 thw;

  CopyWord192(hw, &thw);
  if(n < 4)
  {
    for(j=0; j<n; j++)
    {
      RightShiftWord192(&thw, 1);
      ORWord192(hw, &thw, hw);
    }
    return;
  }

  m = (n+1)/2;
  k = n-m;

  for(j=0; j<m; j++)
  {
    RightShiftWord192(&thw, 1);
    ORWord192(hw, &thw, hw);
  }
  
  CopyWord192(hw, &thw);
  RightShiftWord192(&thw, k);
  ORWord192(hw, &thw, hw);

  return;
}

void
BleedLeftWord192(Word192Ptr hw, int n)
{
  int j, m, k;
  Word192 thw;

  CopyWord192(hw, &thw);
  if(n < 4)
  {
    for(j=0; j<n; j++)
    {
      LeftShiftWord192(&thw, 1);
      ORWord192(hw, &thw, hw);
    }
    return;
  }

  m = (n+1)/2;
  k = n-m;

  for(j=0; j<m; j++)
  {
    LeftShiftWord192(&thw, 1);
    ORWord192(hw, &thw, hw);
  }

  CopyWord192(hw, &thw);
  LeftShiftWord192(&thw, k);
  ORWord192(hw, &thw, hw);

  return;
}

/********************************************************/
/********************************************************/

/***********************/
/* TOP LEVEL FUNCTIONS */
/***********************/


D 4
/* OLD ALGORITHM from Dave Haddle - GENERIC BUT SLOW */

E 4
void
D 4
SuperLayerSum(Word192 data[6], Word192Ptr output, int nlayers,
              int missingLayers)
E 4
I 4
AxialPlusStereoInit()
E 4
{
D 4
  int i, j, numcheck;
  Word192 workspace[7], misses[6];
E 4
I 4
  unsigned long k;
  unsigned char *p;
E 4

D 4
  /* set all bits in misses[] to 1 */
  memset(misses, 0xFF, nlayers*sizeof(Word192));
E 4
I 4
  /* 3 layers contains # hits in vertical columns */
E 4

D 4
  /* segments start out as copy of first layer */
  CopyWord192(&data[0], output);
E 4
I 4
  for(k=0; k<NADR; k++)
  {
    p = (unsigned char *)&look[k];
    p[0] = ((k&0x1)   )+((k&0x10)>>3)+((k&0x100)>>6)+((k&0x1000)>>9);
    p[1] = ((k&0x2)>>1)+((k&0x20)>>4)+((k&0x200)>>7)+((k&0x2000)>>10);
    p[2] = ((k&0x4)>>2)+((k&0x40)>>5)+((k&0x400)>>8)+((k&0x4000)>>11);
    p[3] = ((k&0x8)>>3)+((k&0x80)>>6)+((k&0x800)>>9)+((k&0x8000)>>12);
  }
E 4

D 4
  /* now AND the other layers, which have been shifted to accomodate
   the buckets */
  for(i=0; i<nlayers;) /* no increment here ! */
E 4
I 4
  /* 3 layers contains row hits
     layer 2: 11 10  9  8
     layer 1:  7  6  5  4
     layer 0:  3  2  1  0 */

  for(k=0; k<NADR; k++)
E 4
  {
D 4
    if(i>0) ANDWord192(output, &data[i], output);
E 4
I 4
    p = (unsigned char *)&look3[k];
    p[0] = ((k&0x1)   )+((k&0x10)>>4)+((k&0x100)>>8);
    p[1] = ((k&0x2)>>1)+((k&0x20)>>5)+((k&0x200)>>9);
    p[2] = ((k&0x4)>>2)+((k&0x40)>>6)+((k&0x400)>>10);
    p[3] = ((k&0x8)>>3)+((k&0x80)>>7)+((k&0x800)>>11);
  }
E 4

D 4
    /* Now take missing layers into account. missingLayers
    is the max number of missing layers allowed. However
    there is no need to check more misses the layer that we
    are presently investigating */
    /* note: from this step 'i' is the "NEXT" layer */
    if(++i < missingLayers) numcheck = i;
    else                    numcheck = missingLayers;
E 4
I 4
  return;
}
E 4

D 4
    /* note: numcheck is always > 0 unless a level shift is set
    to zero which is unlikely. (in which case segments will
    be unnecessarily copied onto workspace[0] and back again.
    The algorithm still would work. I have chosen to save the
    check on (numcheck > 0) since it shouldn't happen */
E 4

D 4
    CopyWord192(output, &workspace[0]);
    for(j=0; j<numcheck; j++)
    {
      /* first step: use whatever misses are left for this j */
      ORWord192(&workspace[j], &misses[j], &workspace[j+1]);
E 4
I 4
/*******************/
/* 3 layers 4 bits */
/*******************/
E 4

D 4
      /* second step: remove used up misses */
      ANDWord192(&misses[j], &workspace[j], &misses[j]);
    }
E 4
I 4
#define ADR30 ((b0   )&0xF)  + ((b1&0xF)<<4)         + ((b2&0xF)<<8)
#define ADR31 ((b0>>4)&0xF)  + ((b1&0xF0)   )        + ((b2&0xF0)<<4)
#define ADR32 ((b0>>8)&0xF)  + ((b1&0xF00)>>4)       + ((b2&0xF00)   )
#define ADR33 ((b0>>12)&0xF) + ((b1&0xF000)>>8)      + ((b2&0xF000)>>4)
#define ADR34 ((b0>>16)&0xF) + ((b1&0xF0000)>>12)    + ((b2&0xF0000)>>8)
#define ADR35 ((b0>>20)&0xF) + ((b1&0xF00000)>>16)   + ((b2&0xF00000)>>12)
#define ADR36 ((b0>>24)&0xF) + ((b1&0xF000000)>>20)  + ((b2&0xF000000)>>16)
#define ADR37 ((b0>>28)&0xF) + ((b1&0xF0000000)>>24) + ((b2&0xF0000000)>>20)
E 4

D 4
    CopyWord192(&workspace[numcheck], output);
  } /* end of layer loop */
E 4
I 4
/*******************/
/* 2 layers 4 bits */
/*******************/
E 4

D 4
  return;
}
E 4
I 4
#define ADR20 ((b0   )&0xF)  + ((b1&0xF)<<4)
#define ADR21 ((b0>>4)&0xF)  + ((b1&0xF0)   )
#define ADR22 ((b0>>8)&0xF)  + ((b1&0xF00)>>4)
#define ADR23 ((b0>>12)&0xF) + ((b1&0xF000)>>8)
#define ADR24 ((b0>>16)&0xF) + ((b1&0xF0000)>>12)
#define ADR25 ((b0>>20)&0xF) + ((b1&0xF00000)>>16)
#define ADR26 ((b0>>24)&0xF) + ((b1&0xF000000)>>20)
#define ADR27 ((b0>>28)&0xF) + ((b1&0xF0000000)>>24)
E 4


D 4
/* search for bits with minimum 3 hits */
E 4
I 4
#define SLMIN2 (b1|b2)
unsigned int
slmin2(unsigned int b0, unsigned int b1, unsigned int b2)
{
  return(b1|b2);
}
#define SLMIN3 ((b0&b1)|b2)
unsigned int
slmin3(unsigned int b0, unsigned int b1, unsigned int b2)
{
  return((b0&b1)|b2);
}
#define SLMIN4 (b2)
unsigned int
slmin4(unsigned int b0, unsigned int b1, unsigned int b2)
{
  return(b2);
}
#define RGMIN8 (b3)
unsigned int
rgmin8(unsigned int b0, unsigned int b1, unsigned int b2, unsigned int b3)
{
  return(b3);
}
E 4

D 4
#define PAVEL \
	  /* algorthm from Pavel Degtiarenko */ \
      /* max 10 */ \
      b0a = (*w0)^(*w1); \
      b1a = (*w0)&(*w1); \
      /* max 11 */ \
      b0b = (b0a)^(*w2); \
      b1b = (b1a)^((b0a)&(*w2)); \
      /* max 100 */ \
      b0a = (b0b)^(*w3); \
      b1a = (b1b)^((b0b)&(*w3)); \
      b2a = (b1b)&((b0b)&(*w3)); \
      /* max 101 */ \
      b0b = (b0a)^(*w4); \
      b1b = (b1a)^((b0a)&(*w4)); \
      b2b = (b2a)^((b1a)&((b0a)&(*w4))); \
      /* max 110 */ \
      b0a = (b0b)^(*w5); \
      b1a = (b1b)^((b0b)&(*w5)); \
      b2a = (b2b)^((b1b)&((b0b)&(*w5))); \
      /* trigger Nbit>2 */ \
      *out = (b2a)|((b1a)&(b0a))
E 4
I 4
#define SLMIN SLMIN3
#define RGMIN 6
E 4

D 4
#define STUPID \
      *out = (*w0)&(*w1)&(*w2) | (*w0)&(*w1)&(*w3) | (*w0)&(*w1)&(*w4) | \
             (*w0)&(*w1)&(*w5) | (*w0)&(*w2)&(*w3) | (*w0)&(*w2)&(*w4) | \
             (*w0)&(*w2)&(*w5) | (*w0)&(*w3)&(*w4) | (*w0)&(*w3)&(*w5) | \
             (*w0)&(*w4)&(*w5) | (*w1)&(*w2)&(*w3) | (*w1)&(*w2)&(*w4) | \
             (*w1)&(*w2)&(*w5) | (*w1)&(*w3)&(*w4) | (*w1)&(*w3)&(*w5) | \
             (*w1)&(*w4)&(*w5) | (*w2)&(*w3)&(*w4) | (*w2)&(*w3)&(*w5) | \
             (*w2)&(*w4)&(*w5) | (*w3)&(*w4)&(*w5)
E 4

D 4
void
SuperLayerSum192(Word192 data[6], Word192Ptr output, int nlayers,
                 int missingLayers)
{
  int i;
  unsigned long long b0a, b1a, b2a, b0b, b1b, b2b;
  unsigned long long *out = (unsigned long long *)output->words;
  unsigned long long *w0 = (unsigned long long *)data[0].words;
  unsigned long long *w1 = (unsigned long long *)data[1].words;
  unsigned long long *w2 = (unsigned long long *)data[2].words;
  unsigned long long *w3 = (unsigned long long *)data[3].words;
  unsigned long long *w4 = (unsigned long long *)data[4].words;
  unsigned long long *w5 = (unsigned long long *)data[5].words;
E 4
I 4
/* search for bits with minimum ... hits */
/* XXX=128 or 192, YYY='st' or 'ax' */
E 4

D 4
  if(nlayers==4)
  {
    for(i=0; i<3; i++)
    {
      *out = (*w0)&(*w1)&(*w2) | (*w0)&(*w1)&(*w3) | (*w0)&(*w2)&(*w3) |
             (*w1)&(*w2)&(*w3);
      out++; w0++; w1++; w2++; w3++;
    }
  }
  else
  {
    for(i=0; i<3; i++)
    {
      /*STUPID;*/
      PAVEL;
      out++; w0++; w1++; w2++; w3++; w4++; w5++;
    }
  }
E 4
I 4
#define SUPERLAYER(XXX,YYY) \
{ \
  for(i=0; i<NW##XXX; i++) \
  { \
    b1 = (*YYY##0)&(*YYY##1); \
    b0 = (*YYY##0)^(*YYY##1); \
    b1 = (b1)^((b0)&(*YYY##2)); \
    b0 = (b0)^(*YYY##2); \
    tmp = (b0)&(*YYY##3); \
    b2 = (b1)&tmp; \
    b1 = (b1)^tmp; \
    b0 = (b0)^(*YYY##3); \
    tmp = (b0)&(*YYY##4); \
    b2 = (b2)^((b1)&tmp); \
    b1 = (b1)^tmp; \
    b0 = (b0)^(*YYY##4); \
    tmp = (b0)&(*YYY##5); \
    b2 = (b2)^((b1)&tmp); \
    b1 = (b1)^tmp; \
    b0 = (b0)^(*YYY##5); \
    stat |= *mk##YYY++ = SLMIN; \
    *p##YYY++ = look[ADR30]; \
    *p##YYY++ = look[ADR31]; \
    *p##YYY++ = look[ADR32]; \
    *p##YYY++ = look[ADR33]; \
    *p##YYY++ = look[ADR34]; \
    *p##YYY++ = look[ADR35]; \
    *p##YYY++ = look[ADR36]; \
    *p##YYY++ = look[ADR37]; \
    YYY##0++; YYY##1++; YYY##2++; YYY##3++; YYY##4++; YYY##5++; \
  } \
  YYY##0 -= NW##XXX; \
  YYY##1 -= NW##XXX; \
  YYY##2 -= NW##XXX; \
  YYY##3 -= NW##XXX; \
  YYY##4 -= NW##XXX; \
  YYY##5 -= NW##XXX; \
}
E 4

D 4
  return;
E 4
I 4
/* layers 0-2 */
#define SUPERLAYER02(XXX,YYY) \
{ \
  for(i=0; i<NW##XXX; i++) \
  { \
    b1 = (*YYY##0)&(*YYY##1); \
    b0 = (*YYY##0)^(*YYY##1); \
    b1 = (b1)^((b0)&(*YYY##2)); \
    b0 = (b0)^(*YYY##2); \
    *p02##YYY++ = look[ADR20]; \
    *p02##YYY++ = look[ADR21]; \
    *p02##YYY++ = look[ADR22]; \
    *p02##YYY++ = look[ADR23]; \
    *p02##YYY++ = look[ADR24]; \
    *p02##YYY++ = look[ADR25]; \
    *p02##YYY++ = look[ADR26]; \
    *p02##YYY++ = look[ADR27]; \
    YYY##0++; YYY##1++; YYY##2++; YYY##3++; YYY##4++; YYY##5++; \
  } \
  YYY##0 -= NW##XXX; \
  YYY##1 -= NW##XXX; \
  YYY##2 -= NW##XXX; \
  YYY##3 -= NW##XXX; \
  YYY##4 -= NW##XXX; \
  YYY##5 -= NW##XXX; \
E 4
}


D 4
void
SuperLayerSum128(Word128 data[6], Word128 *output, int nlayers,
                 int missingLayers)
{
  int i;
  unsigned long long b0a, b1a, b2a, b0b, b1b, b2b;
  unsigned long long *out = (unsigned long long *)output->words;
  unsigned long long *w0 = (unsigned long long *)data[0].words;
  unsigned long long *w1 = (unsigned long long *)data[1].words;
  unsigned long long *w2 = (unsigned long long *)data[2].words;
  unsigned long long *w3 = (unsigned long long *)data[3].words;
  unsigned long long *w4 = (unsigned long long *)data[4].words;
  unsigned long long *w5 = (unsigned long long *)data[5].words;
E 4

D 4
  if(nlayers==4)
  {
    for(i=0; i<2; i++)
    {
      *out = (*w0)&(*w1)&(*w2) | (*w0)&(*w1)&(*w3) | (*w0)&(*w2)&(*w3) |
             (*w1)&(*w2)&(*w3);
      out++; w0++; w1++; w2++; w3++;
    }
  }
  else
  {
    for(i=0; i<2; i++)
    {
      /*STUPID;*/
      PAVEL;
      out++; w0++; w1++; w2++; w3++; w4++; w5++;
    }
  }
E 4

D 4
  return;
}
E 4
I 4
#define NPEAK  100
#define NCAND  NSHIFT*NGAP*2
E 4


I 4
#define LOCALVAR1(XXX) \
  unsigned char ax02[6][XXX], st02[6][XXX]; \
  unsigned int *p02ax = (unsigned int *)&ax02[0][0]; \
  unsigned int *p02st = (unsigned int *)&st02[0][0]; \
  int i, j, k, m, itmp, imax, iwtmp, imaxs, nclust; \
  unsigned long b0, b1, b2, tmp; \
  int stat = 0, stat3; \
  unsigned char tmp1[NSHIFT][XXX], tmp2[NSHIFT][XXX]; \
  unsigned int *pax = (unsigned int *)&tmp1[0][0]; \
  unsigned int *pst = (unsigned int *)&tmp2[0][0]; \
  int maxr[XXX], nlistr[XXX], listr[NCAND][XXX], phir[NCAND][XXX], \
      nhitr[NCAND][XXX]; \
  int nsg, sgwire[NPEAK], sgmax[NPEAK], sgshift[NPEAK], sgphi[NPEAK]; \
  Word##XXX mask3, mask1[NSHIFT], mask2[NSHIFT]; \
  unsigned long *mkax = (unsigned long *)mask1[0].words; \
  unsigned long *mkst = (unsigned long *)mask2[0].words; \
  Word##XXX axwork[6], stwork[6]; \
  unsigned long *ax0 = (unsigned long *)axwork[0].words; \
  unsigned long *ax1 = (unsigned long *)axwork[1].words; \
  unsigned long *ax2 = (unsigned long *)axwork[2].words; \
  unsigned long *ax3 = (unsigned long *)axwork[3].words; \
  unsigned long *ax4 = (unsigned long *)axwork[4].words; \
  unsigned long *ax5 = (unsigned long *)axwork[5].words; \
  unsigned long *st0 = (unsigned long *)stwork[0].words; \
  unsigned long *st1 = (unsigned long *)stwork[1].words; \
  unsigned long *st2 = (unsigned long *)stwork[2].words; \
  unsigned long *st3 = (unsigned long *)stwork[3].words; \
  unsigned long *st4 = (unsigned long *)stwork[4].words; \
  unsigned long *st5 = (unsigned long *)stwork[5].words
E 4

D 4
/*********************************************/
/* search for segments in 128-bit superlayer */
E 4

I 4
/* bit map processing for 2 SL and match them */
#define TWOSLPROCESS(XXX) \
  if(DEBUG) \
  { \
    printf("stereo\n"); \
    PrintWord##XXX(&stereo[5]); \
    PrintWord##XXX(&stereo[4]); \
    PrintWord##XXX(&stereo[3]); \
    PrintWord##XXX(&stereo[2]); \
    PrintWord##XXX(&stereo[1]); \
    PrintWord##XXX(&stereo[0]); \
    printf("axial\n"); \
    PrintWord##XXX(&axial[5]); \
    PrintWord##XXX(&axial[4]); \
    PrintWord##XXX(&axial[3]); \
    PrintWord##XXX(&axial[2]); \
    PrintWord##XXX(&axial[1]); \
    PrintWord##XXX(&axial[0]); \
  } \
\
  /* fill ax02[][] and st02[][]  \
  for(i=0; i<2; i++) \
  { \
    CopyWord##XXX(&axial[i], &axwork[i]); \
	CopyWord##XXX(&stereo[i], &stwork[i]); \
    LeftShiftWord##XXX(&stwork[i], NGAP); \
  } \
  SUPERLAYER02(XXX,ax); \
  SUPERLAYER02(XXX,st); \
  LeftShiftWord##XXX##_01(&axwork[1]); \
  LeftShiftWord##XXX##_01(&stwork[1]); \
  SUPERLAYER02(XXX,ax); \
  SUPERLAYER02(XXX,st); \
  LeftShiftWord##XXX##_01(&axwork[2]); \
  LeftShiftWord##XXX##_01(&stwork[2]); \
  SUPERLAYER02(XXX,ax); \
  SUPERLAYER02(XXX,st); \
  LeftShiftWord##XXX##_01(&axwork[2]); \
  LeftShiftWord##XXX##_01(&stwork[2]); \
  SUPERLAYER02(XXX,ax); \
  SUPERLAYER02(XXX,st); \
  LeftShiftWord##XXX##_01(&axwork[1]); \
  LeftShiftWord##XXX##_01(&stwork[1]); \
  SUPERLAYER02(XXX,ax); \
  SUPERLAYER02(XXX,st); \
  LeftShiftWord##XXX##_01(&axwork[2]); \
  LeftShiftWord##XXX##_01(&stwork[2]); \
  SUPERLAYER02(XXX,ax); \
  SUPERLAYER02(XXX,st);*/ \
\
  /*about 63micros*/ \
  for(i=0; i<XXX; i++) {maxr[i] = 0; nlistr[i] = 0;} \
  /*memcpy((char *)axwork,(char *)axial,NLAY*sizeof(Word##XXX)); \
  memcpy((char *)stwork,(char *)stereo,NLAY*sizeof(Word##XXX));*/ \
  for(i=0; i<NLAY; i++) \
  { \
    CopyWord##XXX(&axial[i], &axwork[i]); \
	CopyWord##XXX(&stereo[i], &stwork[i]); \
    LeftShiftWord##XXX(&stwork[i], NGAP); \
  } \
\
  stat = 0; \
  for(j=0; j<NSHIFT; j++) \
  { \
    for(i=1; i<6; i++) \
    { \
      itmp = dshift[j][i-1]; \
      if(itmp==0) \
      { \
        ; \
	  } \
      else if(itmp>0) \
      { \
        LeftShiftWord##XXX##_01(&axwork[i]); \
        LeftShiftWord##XXX##_01(&stwork[i]); \
      } \
      else \
      { \
        RightShiftWord##XXX##_01(&axwork[i]); \
        RightShiftWord##XXX##_01(&stwork[i]); \
      } \
    } \
    SUPERLAYER(XXX,ax); \
    SUPERLAYER(XXX,st); \
  } \
E 4

D 4
#define PRINT1(macro_mess) \
  printf(macro_mess); \
  PrintWord128(&workspace[5]); \
  PrintWord128(&workspace[4]); \
  PrintWord128(&workspace[3]); \
  PrintWord128(&workspace[2]); \
  PrintWord128(&workspace[1]); \
  PrintWord128(&workspace[0]); \
  PrintWord128(output)
E 4

I 4
#define SHIFTS(XXX)
/*#include "shifts.h"*/
E 4

D 4
#define NSHIFT 29
E 4
I 4
  /*about 80micros*/ \
#define TWOSLPROCESS1(XXX) \
  if(stat) \
  { \
    for(k=0; k<NGAP*2; k++) \
    { \
      for(j=0; j<NSHIFT; j++) /*about 20micros for k-loop*/ \
      { \
        RightShiftWord##XXX##_01(&mask2[j]); \
      } \
      for(j=0; j<NSHIFT; j++) \
      { \
        unsigned long *m1 = (unsigned long *)mask1[j].words; \
        unsigned long *m2 = (unsigned long *)mask2[j].words; \
        /*unsigned long *m3 = (unsigned long *)mask3.words;*/ \
        \
        stat3 = 0; \
        for(i=0; i<NW##XXX; i++) stat3 |= /**m3++ =*/ (*m1++) & (*m2++); \
        if(stat3) \
        { \
          for(m=0; m<XXX; m++) \
          { \
            /*if(CheckBitWord##XXX(&mask3,m)) {*/ \
            if(tmp1[j][m]>2 && tmp2[j][m+k+1]>2) \
            { \
            if((itmp = tmp1[j][m]+tmp2[j][m+k+1]) >= RGMIN) \
            { \
				if(itmp > maxr[m] /*&& ABS(phir[nlistr[m]][m]-k) < 1*/) \
              { \
                nlistr[m] = 0; \
                maxr[m] = itmp; \
              } \
              listr[nlistr[m]][m] = j; \
              phir[nlistr[m]][m] = k; \
              nhitr[nlistr[m]][m] = itmp; \
              nlistr[m] ++; \
            } \
            } \
            /*}*/ \
          } \
        } \
      } \
    } \
  } \
\
  if(DEBUG) \
  { \
    printf("region->\n"); \
    for(m=0; m<XXX; m++) \
    { \
      if(nlistr[m]>0) \
	  { \
        printf("wire # %3d - %2d segments (max=%d): ",m,nlistr[m],maxr[m]); \
        for(j=0; j<nlistr[m]; j++) printf(" %2d(%2d,%1d)", \
                    nhitr[j][m],listr[j][m],phir[j][m]); \
        printf("\n"); \
      } \
    } \
  }
E 4

I 4
/* search for the peaks */
#define PEAKSEARCH(XXX) \
  nsg = 0; \
  for(m=0; m<XXX; m++) \
  { \
    if(nsg>=NPEAK) break; \
    for(j=0; j<nlistr[m]; j++) \
    { \
      if(nsg>=NPEAK) break; \
      k = 1; \
      for(i=0; i<nsg; i++) /* loop over existing peaks */ \
      { \
        if(ABS(sgwire[i]-m)<4 && ABS(sgphi[i]-phir[j][m])<4) \
        { \
          /* replace by new one if bigger */ \
          if(sgmax[i]<nhitr[j][m]) \
          { \
            sgwire[i] = m; \
            sgmax[i]   = nhitr[j][m]; \
            sgshift[i] = listr[j][m]; \
            sgphi[i]   = phir[j][m]; \
          } \
          k = 0; \
          break; \
        } \
      } \
      if(k) \
      { \
        sgwire[nsg]  = m; \
        sgmax[nsg]   = nhitr[j][m]; \
        sgshift[nsg] = listr[j][m]; \
        sgphi[nsg]   = phir[j][m]; \
        nsg ++; \
        break; \
      } \
    } \
  } \
  if(DEBUG) \
  { \
    printf("nsg=%d; wire#  max   shift  phi\n",nsg); \
    for(i=0; i<nsg; i++) printf(" %6d %6d %6d %6d\n", \
                         sgwire[i],sgmax[i],sgshift[i],sgphi[i]); \
  }

/* create list of segments */
#define SEGMENTLIST(XXX) \
  for(nclust=0; nclust<MIN(nsg,NCR); nclust++) \
  { \
    imax = sgwire[nclust]; \
    if(DEBUG) printf("-----> nclust=%d, axial:",nclust); \
    if(CheckBitWord##XXX(&axial[0],imax)) \
    { \
      clust->ax[nclust].la[0].nhit = 1; \
      clust->ax[nclust].la[0].iw[0] = imax; \
      if(DEBUG) printf(" %3d",clust->ax[nclust].la[0].iw[0]); \
    } \
    else \
    { \
      clust->ax[nclust].la[0].nhit = 0; \
      if(DEBUG) printf(" %3d",0); \
    } \
    clust->ax[nclust].ird = imax - shift[sgshift[nclust]][1]; \
    for(i=0; i<5; i++) \
    { \
      iwtmp = imax - shift[sgshift[nclust]][i]; \
	  if(CheckBitWord##XXX(&axial[i+1],iwtmp)) \
      { \
        clust->ax[nclust].la[i+1].nhit = 1; \
        clust->ax[nclust].la[i+1].iw[0] = iwtmp; \
        if(DEBUG) printf(" %3d",clust->ax[nclust].la[i+1].iw[0]); \
      } \
      else \
      { \
        clust->ax[nclust].la[i+1].nhit = 0; \
        if(DEBUG) printf(" %3d",0); \
      } \
    } \
    if(DEBUG) printf("  stereo:"); \
    imaxs = imax - (NGAP-1) + sgphi[nclust]; \
    if(CheckBitWord##XXX(&stereo[0],imaxs)) \
    { \
      clust->st[nclust].la[0].nhit = 1; \
      clust->st[nclust].la[0].iw[0] = imaxs; \
      if(DEBUG) printf(" %3d",clust->st[nclust].la[0].iw[0]); \
    } \
    else \
    { \
      clust->st[nclust].la[0].nhit = 0; \
      if(DEBUG) printf(" %3d",0); \
    } \
    clust->st[nclust].ird = imaxs - shift[sgshift[nclust]][1]; \
    for(i=0; i<5; i++) \
    { \
      iwtmp = imaxs - shift[sgshift[nclust]][i]; \
	  if(CheckBitWord##XXX(&stereo[i+1],iwtmp)) \
      { \
        clust->st[nclust].la[i+1].nhit = 1; \
        clust->st[nclust].la[i+1].iw[0] = iwtmp; \
        if(DEBUG) printf(" %3d",clust->st[nclust].la[i+1].iw[0]); \
      } \
      else \
      { \
        clust->st[nclust].la[i+1].nhit = 0; \
        if(DEBUG) printf(" %3d",0); \
      } \
    } \
    if(DEBUG) printf("\n"); \
  } \
  *ncl = clust->nax = nclust


E 4
void
D 4
SegmentSearch128(Word128 data[6], Word128 *segments, int nlayers,
                 int missingLayers)
E 4
I 4
SegmentSearch128(Word128 axial[6], Word128 stereo[6], int *ncl, PRRG *clust)
E 4
{
D 4
  int i, j, i5, i4, i3, i2, i1;
  int list[128][NSHIFT], nlist[128];
  Word128 workspace[6], temp;
  const unsigned int shift[NSHIFT][5] = {
                            { 0, 0, 0, 0, 0},
                            { 1, 0, 0, 0, 0},
                            { 1, 0, 1, 0, 0},
                            { 0, 0, 0, 0, 1},
                            { 0, 0, 1, 0, 1},
                            { 0, 0, 1, 1, 1},
                            { 1, 0, 1, 0, 1},
                            { 1, 0, 1, 1, 1},
                            { 1, 1, 1, 1, 1},
                            { 0, 0, 1, 1, 2},
                            { 1, 0, 1, 1, 2},
                            { 1, 1, 1, 1, 2},
                            { 1, 1, 2, 1, 2},
                            { 1, 1, 2, 2, 2},
                            { 1, 1, 2, 2, 3},
                            { 1, 1, 2, 3, 4},
                            { 1, 1, 3, 3, 4},
                            { 1, 2, 3, 3, 4},
                            { 2, 2, 3, 3, 4},
                            { 1, 1, 3, 3, 5},
                            { 1, 1, 3, 4, 5},
                            { 1, 2, 3, 4, 5},
                            { 2, 2, 3, 4, 5},
                            { 2, 2, 4, 4, 5},
                            { 1, 2, 4, 4, 6},
                            { 2, 2, 4, 4, 6},
                            { 2, 3, 4, 4, 6},
                            { 2, 3, 4, 5, 6},
                            { 2, 3, 5, 5, 6} };
E 4
I 4
  LOCALVAR1(128);
E 4

D 4
PrintWord128(&data[5]);
PrintWord128(&data[4]);
PrintWord128(&data[3]);
PrintWord128(&data[2]);
PrintWord128(&data[1]);
PrintWord128(&data[0]);
E 4
I 4
  TWOSLPROCESS(128);
  /*SHIFTS(128);*/
  TWOSLPROCESS1(128);
  PEAKSEARCH(128);
  SEGMENTLIST(128);
E 4

I 4
  return;
}
E 4

I 4
/* 160micros */
void
SegmentSearch192(Word192 axial[6], Word192 stereo[6], int *ncl, PRRG *clust)
{
  LOCALVAR1(192);
E 4

I 4
  TWOSLPROCESS(192);
  /*SHIFTS(192);*/
  TWOSLPROCESS1(192);
  PEAKSEARCH(192);
  SEGMENTLIST(192);
E 4

D 4
  ClearWord128(segments);
  for(i=0; i<128; i++) nlist[i] = 0;
  for(j=0; j<NSHIFT; j++)
E 4
I 4
  return;
}

void
SuperLayerSum(Word192 data[6], Word192Ptr output, int nlayers)
{
  int i;
  unsigned long b0, b1, b2, tmp;
  unsigned long *out = (unsigned long *)output->words;
  unsigned long *w0 = (unsigned long *)data[0].words;
  unsigned long *w1 = (unsigned long *)data[1].words;
  unsigned long *w2 = (unsigned long *)data[2].words;
  unsigned long *w3 = (unsigned long *)data[3].words;
  unsigned long *w4 = (unsigned long *)data[4].words;
  unsigned long *w5 = (unsigned long *)data[5].words;

  if(nlayers==4)
E 4
  {
D 4
    for(i=0; i<nlayers; i++) CopyWord128(&data[i], &workspace[i]);
    for(i=0; i<5; i++)
E 4
I 4
    for(i=0; i<6; i++) /* need to be tested */
E 4
    {
D 4
      if(shift[j][i]) LeftShiftWord128(&workspace[i+1], shift[j][i]);
E 4
I 4
      b1 = (*w0)&(*w1);
      b0 = (*w0++)^(*w1++);
      b1 = (b1)^((b0)&(*w2));
      b0 = (b0)^(*w2++);

      tmp = (b0)&(*w3);
      b2 = (b1)&tmp;
      b1 = (b1)^tmp;
      b0 = (b0)^(*w3++);

      *out++ = SLMIN3;
E 4
    }
D 4
    SuperLayerSum128(workspace, &temp, 6, 3);
    for(i=0; i<128; i++)
    {
      if(CheckBitWord128(&temp,i)) list[i][nlist[i]++] = j;
    }
    ORWord128(&temp,segments,segments);
E 4
  }
I 4
  else
  {
    for(i=0; i<6; i++)
    {
      b1 = (*w0)&(*w1);
      b0 = (*w0++)^(*w1++);
      b1 = (b1)^((b0)&(*w2));
      b0 = (b0)^(*w2++);
E 4

I 4
      tmp = (b0)&(*w3);
      b2 = (b1)&tmp;
      b1 = (b1)^tmp;
      b0 = (b0)^(*w3++);
E 4

D 4
  /*  
  for(i=0; i<128; i++)
    if(nlist[i]>0)
	{
      printf("wire # %3d - %2d segments: ",i,nlist[i]);
      for(j=0; j<nlist[i]; j++) printf(" %2d",list[i][j]);
      printf("\n");
E 4
I 4
      tmp = (b0)&(*w4);
      b2 = (b2)^((b1)&tmp);
      b1 = (b1)^tmp;
      b0 = (b0)^(*w4++);

      tmp = (b0)&(*w5);
      b2 = (b2)^((b1)&tmp);
      b1 = (b1)^tmp;
      b0 = (b0)^(*w5++);
      
      *out++ = SLMIN3;
E 4
    }
D 4
  */
E 4
I 4
  }
E 4

  return;
}


/********************* RemoveNoise ***************************
  Input:
    Word192  data[6] - actual data with noise 
    int      nlayers - number of layers in this superlayer
D 4
    int      nwires  - max number of wires in a layer in this superlayer
    int      missingLayers - 
E 4
I 4
    int      nwires  - max number of wires in a layer in this superlayer 
E 4
    int     *layershifts - 
  Output:
    Word192  data[6] - cleaned data
**************************************************************/

int
D 4
RemoveNoise(Word192 data[6], int nlayers, int nwires,
            int missingLayers, int *layershifts)
E 4
I 4
RemoveNoise(Word192 data[6], int nlayers, int nwires, int *layershifts)
E 4
{
  int i, itmp;
  Word192 workspace[6], wrk;
  Word192 Lsegments, Rsegments; /* cumulative left & right segments */


  /**************************/
  /* look for segment areas */

  /* copy data[] to workspace[] and shift the data for each layer
     according to the layer shift (i.e the buckets) in both
     left and right directions */

  memcpy(workspace, data, nlayers*sizeof(Word192));
  for(i=1; i<nlayers; i++) BleedRightWord192(&workspace[i], layershifts[i]);
D 4
  SuperLayerSum192(workspace, &Lsegments, nlayers, missingLayers);
E 4
I 4
  SuperLayerSum(workspace, &Lsegments, nlayers);
E 4

  memcpy(workspace, data, nlayers*sizeof(Word192));
  for(i=1; i<nlayers; i++) BleedLeftWord192(&workspace[i], layershifts[i]);
D 4
  SuperLayerSum192(workspace, &Rsegments, nlayers, missingLayers);
E 4
I 4
  SuperLayerSum(workspace, &Rsegments, nlayers);
E 4


  /************************/
  /* now remove the noise */

  /* first, set workspace[0] to contain overlap of both sets of segments
  NOTE: the first layer (layer 0) NEVER has a layer shift.*/

  ORWord192(&Lsegments, &Rsegments, &workspace[0]);
I 4
/*
printf("LRsegm\n");
PrintWord192(&workspace[0]);
*/
E 4
D 3
printf("LRsegm\n");
PrintWord192(&workspace[0]);
E 3
  ANDWord192(&data[0], &workspace[0], &data[0]);

  for(i=1; i<nlayers; i++)
  {
    /* copy segments onto a given layer and bleed to create left
    and right buckets */

    CopyWord192(&Lsegments, &workspace[i]);
    BleedLeftWord192(&workspace[i], layershifts[i]);

    CopyWord192(&Rsegments, &wrk);
    BleedRightWord192(&wrk, layershifts[i]);

    /* combine left and right buckets */
    ORWord192(&workspace[i], &wrk, &workspace[i]);

    /* now get overlap of original data with buckets */
    ANDWord192(&data[i], &workspace[i], &data[i]);
  }

  /*
  printf("workspace\n");
  for(i=0; i<6; i++) PrintWord192(&workspace[i]);
  */

  /* check if we found at least something in the superlayer */
  for(i=0; i<6; i++) if(workspace[0].words[i] & 0xFFFFFFFF) return(1);

  return(0);
}



/********************************************/
/* main functions to be called from outside */
/********************************************/

void
E 5
sginit()
{
D 4
  int i, sec, rgn, supl, numlay, numwire;
E 4
I 4
  int i, j, sec, rgn, supl, numlay, numwire;
E 4

I 4
  /* number of layers/wires */
E 4
  for(sec=0; sec<6; sec++)
  {
    for(rgn=0; rgn<3; rgn++)
    {
      for(supl=0; supl<2; supl++)
      {
        if(rgn==0 && supl==0) numlay=4;
        else                  numlay=6;
        if(rgn==0)
        {
          if(supl==0) numwire=130;
          else        numwire=142;
        }
        else
        {
          if(supl==0) numwire=189;
          else        numwire=192;
        }

        nlayer1[sec][rgn][supl] = numlay;
        nwires1[sec][rgn][supl] = numwire;
      }
    }
  }

I 4
D 5
  /* create differential shift table */
  for(j=0; j<5; j++) dshift[0][j] = 0;
  for(i=1; i<NSHIFT; i++)
  {
    for(j=0; j<5; j++)
    {
      dshift[i][j] = shift[i][j] - shift[i-1][j];
    }
    if(DEBUG) printf("%3d >>> %2d %2d %2d %2d %2d\n",i,dshift[i][0],
                          dshift[i][1],dshift[i][2],dshift[i][3],dshift[i][4]);
  }

  /* fill lookup tables */
E 5
I 5
  /* create differential shift table and fill lookup tables */
E 5
  AxialPlusStereoInit();
I 9
  printf("sginit completed\n");fflush(stdout);
E 9

E 4
  return;
}

I 5

/* DO WE NEED THAT ??? */
E 5
D 4


E 4
void
D 4
test1(int n)
E 4
I 4
sggen()
E 4
{
D 4
  int i;
  long *ww, w[1000000], tmp;
  ww = w;
  for(i=0; i<n; i++) {tmp = (*ww) << 10; *ww++ = tmp;}
E 4
I 4
  int i, j;
  FILE *fd;

  printf("sggen reached\n");

  /* create shifts.h file */
  if((fd=fopen("shifts.h","w")) != NULL)
  {
    fprintf(fd,"#define SHIFTS(XXX) \\\n");
    fprintf(fd,"\\\n/* shifts.h - generated automaticaly */\\\n\\\n");
    fprintf(fd,
      "stat1|=SuperLayer##XXX(axwork,&mask1[ 0],tmp1[ 0]); \\\n");
    fprintf(fd,
      "stat2|=SuperLayer##XXX(stwork,&mask2[ 0],tmp2[ 0]); \\\n");
    fprintf(fd,"\\\n");
I 5
/*
E 5
    for(i=1; i<NSHIFT; i++)
    {
      for(j=0; j<5; j++)
      {
        if(dshift[i][j] > 0)
        {
          fprintf(fd,"LeftShiftWord##XXX##_%02d(&axwork[%1d]); \\\n",
            dshift[i][j],j+1);
          fprintf(fd,"LeftShiftWord##XXX##_%02d(&stwork[%1d]); \\\n",
            dshift[i][j],j+1);
        }
        else if(dshift[i][j] < 0)
        {
          fprintf(fd,"RightShiftWord##XXX##_%02d(&axwork[%1d]); \\\n",
            -dshift[i][j],j+1);
          fprintf(fd,"RightShiftWord##XXX##_%02d(&stwork[%1d]); \\\n",
            -dshift[i][j],j+1);
        }
      }
      fprintf(fd,
        "stat1|=SuperLayer##XXX(axwork,&mask1[%2d],tmp1[%2d]); \\\n",i,i);
      fprintf(fd,
        "stat2|=SuperLayer##XXX(stwork,&mask2[%2d],tmp2[%2d]); \\\n",i,i);
      fprintf(fd,"\\\n");
      printf("%3d >>> %2d %2d %2d %2d %2d\n",i,dshift[i][0],
                          dshift[i][1],dshift[i][2],dshift[i][3],dshift[i][4]);
    }
I 5
*/
E 5
    fclose(fd);
  }

E 4
  return;
}

I 4
/* NOT COMPLETED ... */
E 4
void
D 4
test2(int n)
E 4
I 4
sginitfun(PRSEC sgm)
E 4
{
D 4
  int i;
  long long *ww, w[1000000], tmp;
  ww = w;
  for(i=0; i<n; i++) {tmp = (*ww) << 10; *ww++ = tmp;}
E 4
I 4
  int irg, iax, ist, itmp;

  for(irg=0; irg<3; irg++)
  {
    iax = sdakeys_.lanal[irg*2+2];
    ist = sdakeys_.lanal[irg*2+3];
    if(irg==0)
    {
      itmp = iax;
      iax = ist;
      ist = itmp;
    }
	/*
    printf("irg=%d\n",irg); fflush(stdout);
    printf("  adr=0x%08x\n",(int)&slmin4); fflush(stdout);
    printf("  0x%08x\n",(int)slmin4); fflush(stdout);
    printf("    adr=0x%08x\n",(int)&sgm.rg[irg].axmin); fflush(stdout);
    printf("    0x%08x\n",(int)sgm.rg[irg].axmin); fflush(stdout);
    */
I 5

/*
E 5
    sgm.rg[irg].axmin = slmin4;
    sgm.rg[irg].stmin = slmin4;
    sgm.rg[irg].rgmin = rgmin8;
I 5
*/
E 5

	/*
    if(iax<3)       sgm.rg[irg].axmin = slmin2;
    else if(iax==3) sgm.rg[irg].axmin = slmin3;
    else if(iax>3)  sgm.rg[irg].axmin = slmin4;
    if(ist<3)       sgm.rg[irg].stmin = slmin2;
    else if(ist==3) sgm.rg[irg].stmin = slmin3;
    else if(ist>3)  sgm.rg[irg].stmin = slmin4;
    */
  }

E 4
  return;
}





D 4
/* return ifail=0 if bad sector, =1 if good */
E 4

I 4
#define GETEV \
    /* copy data to bitmap arrays for segment finding */ \
    memset(data,0,36*sizeof(Word192)); \
    hits0 = bosDC0; \
    for(i=0; i<bosNumDC0; i++) \
    { \
      BREAKSHORT(hits0->id, wire, layer); \
      if(GOODHIT) \
      { \
        /* layer is 1..36 and must be converted */ \
        wire--; \
        layer--; \
        rgn = layer / 12; \
        supl = (layer % 12) / 6;  /* the 0..1 variety */ \
        lay = layer % 6; \
        /*printf("set: rgn=%1d supl=%1d lay=%2d wire=%3d -> 1\n", \
        rgn,supl,lay,wire);*/ \
        SetBitWord192(&data[rgn][supl][lay], wire); \
      } \
      hits0++; \
    }


I 10
/* rewrites all DC banks removing single hits */
int
sgremovenoise(int *jw, int isec, int nsl, int layershifts[6])
{
  unsigned int tempDC[10000];
  Word192 data[3][2][6];  /* rgn, supl, layer */
  DC0DataPtr hits0, hits1, bosDC0;
  int bosNumDC0, i, slcount, ret, sec, ind, nd, rgn, supl, lay, nlay, nwire;
  int iax, ih;
  unsigned char layer, wire;

  /* check if bank exist and have a data */
  sec = isec-1;
  if((ind = etNlink(jw,"DC0 ",sec+1)) <= 0) return;
  if((nd=etNdata(jw,ind)) <= 0) return;

  /* set data pointers */
  bosNumDC0 = nd * sizeof(int) / sizeof(DC0Data);
  bosDC0 = (DC0DataPtr)&jw[ind];


  GETEV;

  /* remove noise */
  if(DEBUG) printf("1: isec %d\n",isec);
  slcount = 6;
  for(rgn=0; rgn<3; rgn++)
  {
    if(DEBUG) printf("rgn=%d\n",rgn);
    for(supl=0; supl<2; supl++)
    {
      nlay  = nlayer1[sec][rgn][supl];
      nwire = nwires1[sec][rgn][supl];

      /*
      printf("befor\n");
      for(lay=0; lay<6; lay++) PrintWord192(&data[rgn][supl][lay]);
      */

      ret = RemoveNoise(data[rgn][supl], nlay, nwire, layershifts);

      if(ret == 0) slcount --; /* bag SuperLayer */
      if(slcount < nsl) /* the number of SL required */
      {
        /*printf("bad sector %d\n",sec+1);*/
        /*return(1);*/
      }
      /*
      printf("after ret=%d\n",ret);
      for(lay=0; lay<6; lay++) PrintWord192(&data[rgn][supl][lay]);
      */

    }
  }

  /* cleanup original data banks if noise removing */
  hits0 = bosDC0;
  for(i=0; i< bosNumDC0; i++)
  {
    BREAKSHORT(hits0->id, wire, layer);
    if(GOODHIT)
    {
      /* layer is 1..36 and must be converted */
      wire--;
      layer--;
      rgn = layer / 12;
      supl = (layer % 12) / 6;  /* the 0..1 variety */
      lay = layer % 6;

      /* returns 'False' if corresponding bit 'wire' unset */
      /* in that case will set id=0 - will be ignored by GETDCDATA */
      if(!CheckBitWord192(&data[rgn][supl][lay], wire)) hits0->id = 0;
    }
    hits0++;
  }

  /* recreate DC0 bank */
  hits0 = hits1 = bosDC0;
  for(i=0; i< bosNumDC0; i++)
  {
    if(hits0->id > 0)
    {
      *hits1++ = *hits0;
    }
    hits0++;
  }

  nd = ( (int)hits1-(int)&jw[ind]+1 ) / 4;
  memcpy((char *)tempDC, (char *)&jw[ind], nd * 4);
  etNdrop(jw,"DC0 ",isec);
  ind = etNcreate(jw,"DC0 ",isec,2,nd);
  memcpy((char *)&jw[ind], (char *)tempDC, nd * 4);

  return(slcount);
}



E 10
/* segment finding for one sector

      opt = 1 - noise removing only
          = 2 - segment finding only
          = 3 - noise removing and then segment finding
*/

E 4
void
D 4
sglib(int *jw, int isec, int *ifail)
E 4
I 4
sglib(int *jw, int isec, int opt, int *nsgm, PRSEC *sgm)
E 4
{
D 4
/* get it from sdakeys ? rgn supl */
  static int missinglayers[3][2] = { {3, 3}, {3, 3}, {3, 3} };
  static int layershifts[6] = {0, 2, 3, 3, 4, 4};
E 4
I 4
  /* get it from segment dictionary - take 'max' segment */
  /*static int layershifts[6] = {0, 2, 3, 3, 4, 4};*/
  static int layershifts[6] = {0, 5, 6, 7, 8, 9};
E 4

  Word192 data[3][2][6];  /* rgn, supl, layer */
  DC0DataPtr hits0, bosDC0;
  int bosNumDC0, i, slcount, ret, sec, ind, nd, rgn, supl, lay, nlay, nwire;
I 4
  int iax, ih;
E 4
  unsigned char layer, wire;

D 4
  /*
test1(100000);
test2( 50000);
  */

  *ifail = 0;
E 4
I 4
  *nsgm = 0;
E 4
  sec = isec-1;
I 4
  sgm->rg[0].nax = sgm->rg[1].nax = sgm->rg[2].nax = 0;
  sgm->rg[0].nst = sgm->rg[1].nst = sgm->rg[2].nst = -1;

E 4
  /* check if bank exist and have a data */
  if((ind = etNlink(jw,"DC0 ",sec+1)) <= 0) return;
  if((nd=etNdata(jw,ind)) <= 0) return;

  /* set data pointers */
  bosNumDC0 = nd * sizeof(int) / sizeof(DC0Data);
  bosDC0 = (DC0DataPtr)&jw[ind];

D 4
  /* cleanup bitmap arrays */
  memset(data,0,36*sizeof(Word192));

  /* copy data to bitmap arrays for segment finding */
  hits0 = bosDC0;
  for(i=0; i<bosNumDC0; i++)
E 4
I 4
  if(opt==1 || opt==3)
E 4
  {
D 4
    BREAKSHORT(hits0->id, wire, layer);
    if(GOODHIT)
E 4
I 4
    GETEV;

    /* remove noise */
D 10
    if(DEBUG) printf("isec %d\n",isec);
E 10
I 10
    if(DEBUG) printf("2: isec %d\n",isec);
E 10
    slcount = 6;
    for(rgn=0; rgn<3; rgn++)
E 4
    {
D 4
      /* layer is 1..36 and must be converted */
      wire--;
      layer--;
      rgn = layer / 12;
      supl = (layer % 12) / 6;  /* the 0..1 variety */
      lay = layer % 6;
      /*printf("set: rgn=%1d supl=%1d lay=%2d wire=%3d -> 1\n",rgn,supl,lay,wire);*/
      SetBitWord192(&data[rgn][supl][lay], wire);
E 4
I 4
      if(DEBUG) printf("rgn=%d\n",rgn);
      for(supl=0; supl<2; supl++)
      {
        nlay  = nlayer1[sec][rgn][supl];
        nwire = nwires1[sec][rgn][supl];

        /*
        printf("befor\n");
        for(lay=0; lay<6; lay++) PrintWord192(&data[rgn][supl][lay]);
        */

        ret = RemoveNoise(data[rgn][supl], nlay, nwire, layershifts);

        if(ret == 0) slcount --; /* bag SuperLayer */
        if(slcount < (int)sdakeys_.zcut[2])
        {
          /*printf("bad sector %d\n",sec+1);*/
D 10
return;
E 10
I 10
          return;
E 10
        }
        /*
        printf("after ret=%d\n",ret);
        for(lay=0; lay<6; lay++) PrintWord192(&data[rgn][supl][lay]);
        */

      }
E 4
    }
D 4
    hits0++;
E 4
I 4

    /* cleanup original data banks if noise removing */
    hits0 = bosDC0;
    for(i=0; i< bosNumDC0; i++)
    {
      BREAKSHORT(hits0->id, wire, layer);
      if(GOODHIT)
      {
        /* layer is 1..36 and must be converted */
        wire--;
        layer--;
        rgn = layer / 12;
        supl = (layer % 12) / 6;  /* the 0..1 variety */
        lay = layer % 6;

        /* returns 'False' if corresponding bit 'wire' unset */
        /* in that case will set id=0 - will be ignored by GETDCDATA */
        if(!CheckBitWord192(&data[rgn][supl][lay], wire)) hits0->id = 0;
      }
      hits0++;
    }
E 4
  }

D 3
printf("isec %d\n",isec);

E 3
D 4
  /* find segments */
  slcount = 6;
  for(rgn=0; rgn<3; rgn++)
E 4
I 4

  /* segment finding */
  if(opt==2 || opt==3)
E 4
  {
D 3
printf("rgn=%d\n",rgn);
E 3
D 4
    for(supl=0; supl<2; supl++)
E 4
I 4
    GETEV;

    /* segments finding */
D 10
    if(DEBUG) printf("isec %d\n",isec);
    for(rgn=2; rgn>=0; rgn--) /* is it better to use 128 for reg1 ??? */
E 10
I 10
    if(DEBUG) printf("3: isec %d\n",isec);
    for(rgn=2; rgn>=0; rgn--)
E 10
E 4
    {
D 4
      nlay  = nlayer1[sec][rgn][supl];
      nwire = nwires1[sec][rgn][supl];
E 4
I 4
      int nclust;
D 9
      if(DEBUG) printf("rgn=%d\n",rgn);
E 9
I 9
      if(DEBUG) {printf("rgn=%d\n",rgn);fflush(stdout);}
E 9
E 4

D 4
      /*
      printf("befor\n");
      for(lay=0; lay<6; lay++) PrintWord192(&data[rgn][supl][lay]);
      */
E 4
I 4
      SegmentSearch192(data[rgn][0], data[rgn][1], &nclust, &sgm->rg[rgn]);
E 4

D 4
      ret = RemoveNoise(data[rgn][supl], nlay, nwire, missinglayers[rgn][supl],
                        layershifts);
E 4
I 4
D 10
      if(nclust<=0) return;
E 10
I 10
      /*if(nclust<=0) return;*/
E 10
      *nsgm = 1;
    }
E 4

D 4

D 3
if(rgn==2 && supl==1)
{

Word128 segments, work0[6], work1[6];

printf("supl=0\n");
for(i=0; i<6; i++)
{
  memcpy((char *)&work0[i], (char *)&data[rgn][0][i], sizeof(Word128));
}
SegmentSearch128(work0, &segments, nlay, missinglayers[rgn][0]);

printf("supl=1\n");
for(i=0; i<6; i++)
{
  memcpy((char *)&work1[i], (char *)&data[rgn][1][i], sizeof(Word128));
}
SegmentSearch128(work1, &segments, nlay, missinglayers[rgn][1]);

}



E 3
      if(ret == 0) slcount --; /* bag SuperLayer */
      if(slcount < (int)sdakeys_.zcut[2])
E 4
I 4
    /* add TDC info to the found segments (14micros) */
    hits0 = bosDC0;
    for(i=0; i< bosNumDC0; i++)
    {
      BREAKSHORT(hits0->id, wire, layer);
      if(GOODHIT)
E 4
      {
D 4
        /*printf("bad sector %d\n",sec+1);*/
        return;
      }
      /*
      printf("after ret=%d\n",ret);
      for(lay=0; lay<6; lay++) PrintWord192(&data[rgn][supl][lay]);
      */
E 4
I 4
        wire--;
        layer--;
        rgn = layer / 12;
        supl = (layer % 12) / 6;
        lay = layer % 6;
E 4

I 4
        if(supl==0)
        {
          for(iax=0; iax<sgm->rg[rgn].nax; iax++)
          {
            for(ih=0; ih<sgm->rg[rgn].ax[iax].la[lay].nhit; ih++)
            {
              if(wire==sgm->rg[rgn].ax[iax].la[lay].iw[ih])
              {
                sgm->rg[rgn].ax[iax].la[lay].tdc[ih] = hits0->tdc;
              }
            }
          }
        }
        else
        {
          for(iax=0; iax<sgm->rg[rgn].nax; iax++)
          {
            for(ih=0; ih<sgm->rg[rgn].st[iax].la[lay].nhit; ih++)
            {
              if(wire==sgm->rg[rgn].st[iax].la[lay].iw[ih])
              {
                sgm->rg[rgn].st[iax].la[lay].tdc[ih] = hits0->tdc;
              }
            }
          }
        }
      }
      hits0++;
E 4
    }
I 4

E 4
  }

D 4
  /* cleanup original data banks */
  hits0 = bosDC0;
  for(i=0; i< bosNumDC0; i++)
E 4
I 4
  return;
}



/* print results for the one sector */

void
sgprint(PRSEC *sgm)
{
  int rgn, iax, il, ih;

  if(!DEBUG) return;

  printf("sgprint =============== wire from 1 here !!! ==============\n");
  for(rgn=0; rgn<3; rgn++)
E 4
  {
D 4
    BREAKSHORT(hits0->id, wire, layer);
    if(GOODHIT)
E 4
I 4
    printf(" region %d\n",rgn);
    for(iax=0; iax<sgm->rg[rgn].nax; iax++)
E 4
    {
D 4
      /* layer is 1..36 and must be converted */
      wire--;
      layer--;
      rgn = layer / 12;
      supl = (layer % 12) / 6;  /* the 0..1 variety */
      lay = layer % 6;

      /* returns 'False' if corresponding bit 'wire' unset */
      /* in that case will set id=0 - will be ignored by GETDCDATA */
      if(!CheckBitWord192(&data[rgn][supl][lay], wire)) hits0->id = 0;
E 4
I 4
      printf("   stereo: road %d\n",sgm->rg[rgn].st[iax].ird+1);
      for(il=NLAY-1; il>=0; il--)
      {
        printf("     la=%1d, nh=%1d:",il,sgm->rg[rgn].st[iax].la[il].nhit);
        for(ih=0; ih<sgm->rg[rgn].st[iax].la[il].nhit; ih++)
        {
          printf(" %3d-%4d",sgm->rg[rgn].st[iax].la[il].iw[ih]+1,
                            sgm->rg[rgn].st[iax].la[il].tdc[ih]);
        }
        printf("\n");
      }
      printf("   axial: road %d\n",sgm->rg[rgn].ax[iax].ird+1);
      for(il=NLAY-1; il>=0; il--)
      {
        printf("     la=%1d, nh=%1d:",il,sgm->rg[rgn].ax[iax].la[il].nhit);
        for(ih=0; ih<sgm->rg[rgn].ax[iax].la[il].nhit; ih++)
        {
          printf(" %3d-%4d",sgm->rg[rgn].ax[iax].la[il].iw[ih]+1,
                            sgm->rg[rgn].ax[iax].la[il].tdc[ih]);
        }
        printf("\n");
      }
E 4
    }
D 4
    hits0++;
E 4
I 4

E 4
  }

D 4
  *ifail = 1;
E 4
I 4
  return;
}
E 4

I 4

D 9
/* fill read finding structures, SWAPING region 1 */
E 9
I 9
/* fill road finding structures, SWAPING region 1 */
E 9

void
sgroad(PRSEC *sgm, int ncl[3], unsigned char ird[3][2][nclmx])
{
  int rgn, iax, il, ih, itmp;

  if(DEBUG) printf("sgroad ======= wire from 1 here !!! ==============\n");
  for(rgn=0; rgn<3; rgn++)
  {
    ncl[rgn] = sgm->rg[rgn].nax;
    if(DEBUG) printf(" region number = %d,   #clusters = %d\n",rgn,ncl[rgn]);
    for(iax=0; iax<ncl[rgn]; iax++)
    {
      ird[rgn][0][iax] = sgm->rg[rgn].ax[iax].ird+1;
      ird[rgn][1][iax] = sgm->rg[rgn].st[iax].ird+1;
      if(rgn==0)
      {
        itmp = ird[rgn][0][iax];
        ird[rgn][0][iax] = ird[rgn][1][iax];
        ird[rgn][1][iax] = itmp;
      }
      if(DEBUG) printf("  %2d: (%3d %3d)\n",
                       iax,ird[rgn][0][iax],ird[rgn][1][iax]);
    }
  }

E 4
  return;
}


I 4

E 4

E 1
