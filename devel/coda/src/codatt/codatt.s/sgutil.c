
/* sgutil.c - drift chamber noise reduction and segment finding library */
/* based on David Heddle noise reduction algorithm code                 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "sgutil.h"

#define DEBUG  0
#define DEBUG1 0

#ifdef ALTIVEC

#define PRINT(_nn_) \
  printf("[%d] 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n", \
  _nn_,prn.x[0],prn.x[1],prn.x[2],prn.x[3],prn.x[4],prn.x[5],prn.x[6],prn.x[7])

#define SHIFTLEFT256_1(A) \
{ \
  vector unsigned char shiftFactor, negShiftFactor; \
  vector unsigned int vOdd, vEven; \
  shiftFactor = vec_splat_u8(1); \
  negShiftFactor = (vector unsigned char)vec_splat_s8(-1); \
  vEven = A[0]; \
  vOdd  = A[1]; \
  A[0]  = vec_or(vec_srl(vec_sro(vOdd, negShiftFactor), negShiftFactor), \
                 vec_sll(vEven, shiftFactor)); \
  A[1]  = vec_sll(vOdd, shiftFactor); \
}

#define SHIFTRIGHT256_1(A) \
{ \
  vector unsigned char shiftFactor, negShiftFactor; \
  vector unsigned int vOdd, vEven; \
  shiftFactor = vec_splat_u8(1); \
  negShiftFactor = (vector unsigned char)vec_splat_s8(-1); \
  vOdd  = A[1]; \
  vEven = A[0]; \
  A[1]  = vec_or(vec_srl(vOdd, shiftFactor), \
                   vec_sll(vec_slo(vEven, negShiftFactor), negShiftFactor)); \
  A[0]  = vec_srl(vEven, shiftFactor); \
}

typedef union prnt256
{
  vector unsigned int v256[2];
  unsigned int x[8];
} PRNT256;

#endif

#define ABS(x)     ( (x) < 0 ? -(x) : (x) )

#ifdef VXWORKS
#define LONG_BIT 32
#endif
#ifdef Linux
#define LONG_BIT 32
#endif
#ifdef HPUX
#define LONG_BIT 32
#endif

#define NW192 6 /* # of long words in 192 bits */
#define NW256 8 /* # of long words in 256 bits and TOTAL # in 192 */

#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define NADR   0xFFF
static unsigned int look[NADR];   /* 3 summed layers lookup table */
static unsigned int look3[NADR];  /* 3 row layers lookup table */

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

static unsigned int shift[NSHIFT][5];
const unsigned int shift_half[NSHIFT_HALF][5] = {
                          /* l1,l2,l3,l4,l5 */
                            { 1, 0, 0, 0, 0, },
                            { 1, 0, 1, 0, 0, },
                            { 0, 0, 0, 0, 1, },
                            { 0, 0, 1, 0, 1, },
                            { 0, 0, 1, 1, 1, },
                            { 1, 0, 1, 0, 1, },
                            { 1, 0, 1, 1, 1, },
                            { 1, 1, 1, 1, 1, },
                            { 0, 0, 1, 1, 2, },
                            { 1, 0, 1, 1, 2, },
                            { 1, 1, 1, 1, 2, },
                            { 1, 1, 2, 1, 2, },
                            { 1, 1, 2, 2, 2, },
                            { 1, 1, 2, 2, 3, },
                            { 1, 1, 2, 3, 4, },
                            { 1, 1, 3, 3, 4, },
                            { 1, 2, 3, 3, 4, },
                            { 2, 2, 3, 3, 4, },
                            { 1, 1, 3, 3, 5, },
                            { 1, 1, 3, 4, 5, },
                            { 1, 2, 3, 4, 5, },
                            { 2, 2, 3, 4, 5, },
                            { 2, 2, 4, 4, 5, },
                            { 1, 2, 4, 4, 6, },
                            { 2, 2, 4, 4, 6, },
                            { 2, 3, 4, 4, 6, },
                            { 2, 3, 4, 5, 6, },
                            { 2, 3, 5, 5, 6, } };

/********************
  USEFUL BINARY OPS
*********************/

/* prints out an unsigned long in binary format */

void
BinaryPrint32(unsigned long k)
{
  int i, lastbit;
  unsigned long j, jj;

  j = 1;
  lastbit = 8*sizeof(long) - 1;
  for(i = 0; i < lastbit; i++) j *= 2;

  jj = j;
  for(i = lastbit; i >=0; i--)
  {
    if(k & jj) fprintf(stdout, "|");
    else       fprintf(stdout, "o");
    
    jj = jj >> 1;
  }

  return;
}



/*********************/
/* Word192 functions */
/*********************/

void
PrintWord192(Word192Ptr hw)
{
  int i;
  unsigned long *w;
  w = (unsigned long *)hw->words;
  printf("192> ");
  for(i=5; i>=0; i--) {BinaryPrint32(w[i]); printf(" ");}
  printf("\n");
  /*
printf("hilo ");
BinaryPrint32(hw->hword); printf(" ");
BinaryPrint32(hw->lword); printf("\n");
  */
  return;
}

void
CopyWord192(Word192Ptr hws, Word192Ptr hwd)
{
  int i;
  unsigned long *a = (unsigned long *)hws;
  unsigned long *b = (unsigned long *)hwd;
  for(i=0; i<NW256; i++) b[i] = a[i];
  return;
}

void
ANDWord192(Word192Ptr hwa, Word192Ptr hwb, Word192Ptr hwc)
{
  int i;
  unsigned long *a = (unsigned long *)hwa;
  unsigned long *b = (unsigned long *)hwb;
  unsigned long *c = (unsigned long *)hwc;
  for(i=0; i<NW256; i++) c[i] = a[i] & b[i];
  return;
}

void
ORWord192(Word192Ptr hwa, Word192Ptr hwb, Word192Ptr hwc)
{
  int i;
  unsigned long *a = (unsigned long *)hwa;
  unsigned long *b = (unsigned long *)hwb;
  unsigned long *c = (unsigned long *)hwc;
  for(i=0; i<NW256; i++) c[i] = a[i] | b[i];
  return;
}

void
XORWord192(Word192Ptr hwa, Word192Ptr hwb, Word192Ptr hwc)
{
  int i;
  unsigned long *a = (unsigned long *)hwa;
  unsigned long *b = (unsigned long *)hwb;
  unsigned long *c = (unsigned long *)hwc;
  for(i=0; i<NW256; i++) c[i] = a[i] ^ b[i];
  return;
}

#define RightShiftWord192(hw, n) \
{ \
  int ii, shift; \
  unsigned long *w = (unsigned long *)(hw)->words; \
  shift = LONG_BIT - n; \
\
(hw)->lword >>= n; \
(hw)->lword |= w[0] << shift; \
\
  for(ii=0; ii<5; ii++) \
  { \
    w[ii] >>= n; \
    w[ii] |= w[ii+1] << shift; \
  } \
  w[5] >>= n; \
\
w[5] |= (hw)->hword << shift; \
(hw)->hword >>= n; \
\
}

#define LeftShiftWord192(hw, n) \
{ \
  int ii, shift; \
  unsigned long *w = (unsigned long *)(hw)->words; \
  shift = LONG_BIT - n; \
\
(hw)->hword <<= n; \
(hw)->hword |= w[5] >> shift; \
\
  for(ii=5; ii>0; ii--) \
  { \
    w[ii] <<= n; \
    w[ii] |= w[ii-1] >> shift; \
  } \
  w[0] <<= n; \
\
w[0] |= (hw)->lword >> shift; \
(hw)->lword <<= n; \
\
}

/**************************/
/* start of special cases */

#define RIGHTSHIFT(hw,index,nbit) \
{ \
  int ii; \
  unsigned long *w = (unsigned long *)(hw)->words; \
\
(hw)->lword >>= nbit; \
(hw)->lword |= w[0] << (LONG_BIT-nbit); \
\
  for(ii=0; ii<index; ii++) \
  { \
    w[ii] >>= nbit; \
    w[ii] |= w[ii+1] << (LONG_BIT-nbit); \
  } \
  w[index] >>= nbit; \
\
w[index] |= (hw)->hword << (LONG_BIT-nbit); \
(hw)->hword >>= nbit; \
\
}

#define RightShiftWord192_00(hw) ;
#define RightShiftWord192_01(hw) RIGHTSHIFT(hw,5,1)
#define RightShiftWord192_02(hw) RIGHTSHIFT(hw,5,2)
#define RightShiftWord192_03(hw) RIGHTSHIFT(hw,5,3)


#define LEFTSHIFT(hw,index,nbit) \
{ \
  int ii; \
  unsigned long *w = (unsigned long *)(hw)->words; \
\
(hw)->hword <<= nbit; \
(hw)->hword |= w[index] >> (LONG_BIT-nbit); \
\
  for(ii=index; ii>0; ii--) \
  { \
    w[ii] <<= nbit; \
    w[ii] |= w[ii-1] >> (LONG_BIT-nbit); \
  } \
  w[0] <<= nbit; \
\
w[0] |= (hw)->lword >> (LONG_BIT-nbit); \
(hw)->lword <<= nbit; \
\
}

#define LeftShiftWord192_00(hw) ;
#define LeftShiftWord192_01(hw) LEFTSHIFT(hw,5,1)
#define LeftShiftWord192_02(hw) LEFTSHIFT(hw,5,2)
#define LeftShiftWord192_03(hw) LEFTSHIFT(hw,5,3)

/* end of special cases */
/************************/


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
ClearWord192(Word192Ptr hw)
{
  memset(hw->words,0,sizeof(Word192));/* dol'she?*/

  return;
}

void
NegateWord192(Word192Ptr hw)
{
  int i;
  unsigned long *w;
  w = hw->words;
  for(i=0; i<NW256; i++) w[i] = ~w[i];
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
/*                 TOP LEVEL FUNCTIONS                  */
/********************************************************/
/********************************************************/

void
AxialPlusStereoInit()
{
  int i, j;
  unsigned long k;
  unsigned char *p;

  /* create full shift table using shift_half */

  for(i=0; i<NSHIFT_HALF; i++)
  {
    for(j=0; j<5; j++)
    {
      shift[i][j] = - shift_half[NSHIFT_HALF-1-i][j];
      shift[i+NSHIFT_HALF+1][j] = shift_half[i][j];
    }
  }
  for(j=0; j<5; j++) shift[NSHIFT_HALF][j] =0;

  printf("\n\nShift table:\n");
  for(i=0; i<NSHIFT; i++)
  {
    printf("%3d >>> %2d %2d %2d %2d %2d\n",i,shift[i][0],
                          shift[i][1],shift[i][2],shift[i][3],shift[i][4]);
  }
  printf("\n\n");

  /* create differential shift table */

  for(j=0; j<5; j++) dshift[0][j] = 0;
  printf("\n\nDifferential shift table:\n");
  for(i=1; i<NSHIFT; i++)
  {
    for(j=0; j<5; j++)
    {
      dshift[i][j] = shift[i][j] - shift[i-1][j];
    }
    printf("%3d >>> %2d %2d %2d %2d %2d\n",i,dshift[i][0],
                          dshift[i][1],dshift[i][2],dshift[i][3],dshift[i][4]);
  }
  printf("\n\n");

  /* lookup table: 3 layers contains # hits in vertical columns */

  for(k=0; k<NADR; k++)
  {
    p = (unsigned char *)&look[k];
    p[0] = ((k&0x1)   )+((k&0x10)>>3)+((k&0x100)>>6)+((k&0x1000)>>9);
    p[1] = ((k&0x2)>>1)+((k&0x20)>>4)+((k&0x200)>>7)+((k&0x2000)>>10);
    p[2] = ((k&0x4)>>2)+((k&0x40)>>5)+((k&0x400)>>8)+((k&0x4000)>>11);
    p[3] = ((k&0x8)>>3)+((k&0x80)>>6)+((k&0x800)>>9)+((k&0x8000)>>12);
  }

  /*  lookup table: 3 layers contains row hits
     layer 2: 11 10  9  8
     layer 1:  7  6  5  4
     layer 0:  3  2  1  0 */

  for(k=0; k<NADR; k++)
  {
    p = (unsigned char *)&look3[k];
    p[0] = ((k&0x1)   )+((k&0x10)>>4)+((k&0x100)>>8);
    p[1] = ((k&0x2)>>1)+((k&0x20)>>5)+((k&0x200)>>9);
    p[2] = ((k&0x4)>>2)+((k&0x40)>>6)+((k&0x400)>>10);
    p[3] = ((k&0x8)>>3)+((k&0x80)>>7)+((k&0x800)>>11);
  }

  return;
}


/*******************/
/* 3 layers 4 bits */
/*******************/

#define ADR30 ((b0   )&0xF)  + ((b1&0xF)<<4)         + ((b2&0xF)<<8)
#define ADR31 ((b0>>4)&0xF)  + ((b1&0xF0)   )        + ((b2&0xF0)<<4)
#define ADR32 ((b0>>8)&0xF)  + ((b1&0xF00)>>4)       + ((b2&0xF00)   )
#define ADR33 ((b0>>12)&0xF) + ((b1&0xF000)>>8)      + ((b2&0xF000)>>4)
#define ADR34 ((b0>>16)&0xF) + ((b1&0xF0000)>>12)    + ((b2&0xF0000)>>8)
#define ADR35 ((b0>>20)&0xF) + ((b1&0xF00000)>>16)   + ((b2&0xF00000)>>12)
#define ADR36 ((b0>>24)&0xF) + ((b1&0xF000000)>>20)  + ((b2&0xF000000)>>16)
#define ADR37 ((b0>>28)&0xF) + ((b1&0xF0000000)>>24) + ((b2&0xF0000000)>>20)

/********************************/
/* 2 layers 4 bits (not in use) */
/********************************/

#define ADR20 ((b0   )&0xF)  + ((b1&0xF)<<4)
#define ADR21 ((b0>>4)&0xF)  + ((b1&0xF0)   )
#define ADR22 ((b0>>8)&0xF)  + ((b1&0xF00)>>4)
#define ADR23 ((b0>>12)&0xF) + ((b1&0xF000)>>8)
#define ADR24 ((b0>>16)&0xF) + ((b1&0xF0000)>>12)
#define ADR25 ((b0>>20)&0xF) + ((b1&0xF00000)>>16)
#define ADR26 ((b0>>24)&0xF) + ((b1&0xF000000)>>20)
#define ADR27 ((b0>>28)&0xF) + ((b1&0xF0000000)>>24)


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

#define SLMIN SLMIN3
#define RGMIN 5 /* was 6 */

/* search for bits with minimum 'SLMIN' hits */
/* XXX=128 or 192, YYY='st' or 'ax' */

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

/* the same, but 2-hit segments */
#define SUPERLAYER_2(XXX,YYY) \
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
    stat |= *mk##YYY++ = SLMIN2; \
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



#define NPEAK  100
#define NCAND  (NSHIFT*NGAP*2)


#define LOCALVAR1(XXX) \
  /* */ \
  int nsg, sgwire[NPEAK], sgmax[NPEAK], sgshift[NPEAK], sgphi[NPEAK]; \
  /* temporary input data storage and corresponding pointers */ \
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
  unsigned long *st5 = (unsigned long *)stwork[5].words; \
  /* filled up by TWOSLPROCESS, used by REGIONPROCESS */ \
  unsigned char tmp1[NSHIFT][XXX], tmp2[NSHIFT][XXX]; \
  unsigned int *pax = (unsigned int *)&tmp1[0][0]; \
  unsigned int *pst = (unsigned int *)&tmp2[0][0]; \
  Word##XXX mask1[NSHIFT], mask2[NSHIFT]; \
  unsigned long *mkax, *mkst; \
  int stat; \
  /* filled by REGIONPROCESS */ \
  int maxr[XXX], nlistr[XXX]; \
/* NCAND too big, local cannot be so big ???!!! use 'static' ... */ \
  static int listr[NCAND][XXX], phir[NCAND][XXX], nhitr[NCAND][XXX]; \
  /* other local variables */ \
  int i, j, k, m, n, itmp, imax, iwtmp, imaxs, nclust, stat3; \
  unsigned long b0, b1, b2, tmp


/******************************************************************************
 TWOSLPROCESS: bit map processing for 2 SL and match them

   1. copy axial[0..NLAY] -> axwork[0..NLAY]
   2. copy stereo[0..NLAY] -> stwork[0..NLAY]
   3. shift stwork[0..NLAY] to the left on NGAP bits
   4. loop over NSHIFT shifts in according to dshift[] table:
      - shifts every layer in both axial and stereo
      - calls SUPERLAYER macros for both axial and stereo
      - fills following arrays/variables:
          *pax (pointer to tmp1[][])
          *pst (pointer to tmp2[][])
          *mkax (pointer to mask1[])
          *mkst (pointer to mask2[])
          stat

******************************************************************************/
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
  for(i=0;i<NSHIFT;i++) for(j=0;j<XXX;j++) {tmp1[i][j]=tmp2[i][j]=0;} /* was NOT here */ \
  /*about 63micros*/ \
  for(i=0; i<NLAY; i++) \
  { \
    CopyWord##XXX(&axial[i], &axwork[i]); \
    CopyWord##XXX(&stereo[i], &stwork[i]); \
    LeftShiftWord##XXX(&stwork[i], NGAP); \
  } \
  for(i=1; i<NLAY; i++) \
  { \
    itmp = -shift[0][i-1]; \
    RightShiftWord##XXX(&axwork[i], itmp); \
    RightShiftWord##XXX(&stwork[i], itmp); \
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
    mkax = (unsigned long *)mask1[j].words; \
    mkst = (unsigned long *)mask2[j].words; \
    SUPERLAYER(XXX,ax); \
    SUPERLAYER(XXX,st); \
  } /* \
  if(DEBUG) \
  { \
    for(i=0; i<XXX; i++) \
	{ \
      printf("wire #%d: ",i); \
      for(j=0; j<NSHIFT; j++) \
      { \
        printf("%2d",tmp1[j][i]); \
      } \
      printf("\n"); \
	  } \
  }*/

/* 'almost' the same as TWOSLPROCESS - just to enforce 2-hit R1 stereo ... */
#define TWOSLPROCESS_2(XXX) \
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
  for(i=0;i<NSHIFT;i++) for(j=0;j<XXX;j++) {tmp1[i][j]=tmp2[i][j]=0;} /* was NOT here */ \
  /*about 63micros*/ \
  for(i=0; i<NLAY; i++) \
  { \
    CopyWord##XXX(&axial[i], &axwork[i]); \
    CopyWord##XXX(&stereo[i], &stwork[i]); \
    LeftShiftWord##XXX(&stwork[i], NGAP); \
  } \
  for(i=1; i<NLAY; i++) \
  { \
    itmp = -shift[0][i-1]; \
    RightShiftWord##XXX(&axwork[i], itmp); \
    RightShiftWord##XXX(&stwork[i], itmp); \
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
    mkax = (unsigned long *)mask1[j].words; \
    mkst = (unsigned long *)mask2[j].words; \
    SUPERLAYER(XXX,ax); \
    SUPERLAYER_2(XXX,st); \
  } \

/******************************************************************************
 REGIONPROCESS: 
   1. cleanup maxr[0..XXX] and nlistr[0..XXX]
   2. if(stat), loop over NGAP*2:
      - shift mask2[0..NSHIFT] (stereo) 1 bit to the right

******************************************************************************/
/*about 80micros*/ \
#define REGIONPROCESS(XXX) \
  for(i=0; i<XXX; i++) {maxr[i] = 0; nlistr[i] = 0;} \
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
        \
        stat3 = 0; \
		/*was: for(i=0; i<NW##XXX; i++) stat3 |= (*m1++) & (*m2++);*/ \
        for(i=0; i<NW##XXX; i++) stat3 |= (*m1++) || (*m2++); \
/*printf("09: stat3=%d\n",stat3);*/ \
        if(stat3) \
        { \
          for(m=0; m<XXX-(k+1); m++) /*was: 'XXX-k+1' - error ???*/ \
          { \
/*printf("10: j=%d m=%d tmp1=%d tmp2=%d\n",j,m,tmp1[j][m],tmp2[j][m+k+1]);*/ \
			/*was: if(tmp1[j][m]>2 && tmp2[j][m+k+1]>2)*/ /* if BOTH superlayers have segments */ \
			  if(tmp1[j][m]>=4/*2*/ || tmp2[j][m+k+1]>=4/*2*/) /* if AT LEAST one superlayer has a segment */ \
            { \
/*printf("100: tmp1=%d tmp2=%d\n",tmp1[j][m],tmp2[j][m+k+1]);*/ \
				if((itmp = tmp1[j][m]+tmp2[j][m+k+1]) >= 8/*3*//*RGMIN*/) \
              { \
/*printf("101: tmp1=%d tmp2=%d\n",tmp1[j][m],tmp2[j][m+k+1]);*/ \
                if(itmp > maxr[m] /*&& ABS(phir[nlistr[m]][m]-k) < 1*/) \
                { \
                  nlistr[m] = 0; \
                  maxr[m] = itmp; \
                } \
/*printf("11: m=%d NCAND=%d NSHIFT=%d NGAP=%d\n",m,NCAND,NSHIFT,NGAP);fflush(stdout);*/ \
                n = nlistr[m]; \
/*printf("12: n=%d, j=%d\n",n,j);fflush(stdout);*/ \
                listr[n][m] = j; \
/*printf("13\n");fflush(stdout);*/ \
                phir [n][m] = k; \
                nhitr[n][m] = itmp; \
                nlistr[m] ++; \
              } \
            } \
          } \
        } \
      } \
    } \
  } \
\
  if(DEBUG1) \
  { \
	printf("region->\n"); /*always here*/ \
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

#define REGIONPROCESS_2(XXX) \
  for(i=0; i<XXX; i++) {maxr[i] = 0; nlistr[i] = 0;} \
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
        \
        stat3 = 0; \
		/*was: for(i=0; i<NW##XXX; i++) stat3 |= (*m1++) & (*m2++);*/ \
        for(i=0; i<NW##XXX; i++) stat3 |= (*m1++) || (*m2++); \
/*printf("09: stat3=%d\n",stat3);*/ \
        if(stat3) \
        { \
			for(m=0; m<XXX-(k+1); m++) /*was: 'XXX-k+1' - error ???*/ \
          { \
/*if(m>22&&m<27) \
printf("10: j=%d m=%d tmp1=%d tmp2=%d\n",j,m,tmp1[j][m],tmp2[j][m+k+1]);*/ \
			/*was: if(tmp1[j][m]>2 && tmp2[j][m+k+1]>2)*/ /* if BOT superlayers have segments */ \
			if(tmp1[j][m]>=2 || tmp2[j][m+k+1]>=2) /* if AT LEAST one superlayer has a segment */ \
            { \
              if((itmp = tmp1[j][m]+tmp2[j][m+k+1]) >= 2/*was RGMIN*/) \
              { \
/*if(itmp>192) printf("101: tmp1=%d tmp2=%d\n",tmp1[j][m],tmp2[j][m+k+1]);*/ \
                if(itmp > maxr[m] /*&& ABS(phir[nlistr[m]][m]-k) < 1*/) \
                { \
                  nlistr[m] = 0; \
                  maxr[m] = itmp; \
                } \
/*printf("11: m=%d NCAND=%d NSHIFT=%d NGAP=%d\n",m,NCAND,NSHIFT,NGAP);fflush(stdout);*/ \
                n = nlistr[m]; \
/*printf("12: n=%d, j=%d\n",n,j);fflush(stdout);*/ \
                listr[n][m] = j; \
/*printf("13\n");fflush(stdout);*/ \
                phir [n][m] = k; \
                nhitr[n][m] = itmp; \
                nlistr[m] ++; \
              } \
            } \
          } \
        } \
      } \
    } \
  } \
\
  if(DEBUG) \
  { \
	printf("region->\n"); /*always here*/ \
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
        /* was: if(ABS(sgwire[i]-m)<4 && ABS(sgphi[i]-phir[j][m])<4) */ \
        if(ABS(sgwire[i]-m)<6 && ABS(sgphi[i]-phir[j][m])<20) \
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
  if(DEBUG1) \
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
    if(DEBUG1) printf("-----> nclust=%d (imax=%d), axial:",nclust,imax); \
    /* first layer */ \
    if(CheckBitWord##XXX(&axial[0],imax)) \
    { \
      clust->ax[nclust].la[0].nhit = 1; \
      clust->ax[nclust].la[0].iw[0] = imax; \
      if(DEBUG1) printf(" %3d",clust->ax[nclust].la[0].iw[0]); \
      clust->ax[nclust].ird = imax - shift[sgshift[nclust]][1]; \
    } \
    else \
    { \
      clust->ax[nclust].la[0].nhit = 0; \
      if(DEBUG1) printf(" %3d",0); \
      clust->ax[nclust].ird = 254; /*mark by non-existing wire number*/ \
    } \
    /* five other layers */ \
    for(i=0; i<5; i++) \
    { \
      iwtmp = imax - shift[sgshift[nclust]][i]; \
	  if(CheckBitWord##XXX(&axial[i+1],iwtmp)) \
      { \
        clust->ax[nclust].la[i+1].nhit = 1; \
        clust->ax[nclust].la[i+1].iw[0] = iwtmp; \
        if(DEBUG1) printf(" %3d",clust->ax[nclust].la[i+1].iw[0]); \
if(clust->ax[nclust].ird == 254) clust->ax[nclust].ird = iwtmp; /*imax ???*/ \
      } \
      else \
      { \
        clust->ax[nclust].la[i+1].nhit = 0; \
        if(DEBUG1) printf(" %3d",0); \
      } \
    } \
    if(DEBUG1) printf("  stereo:"); \
    imaxs = imax - (NGAP-1) + sgphi[nclust]; \
    if(CheckBitWord##XXX(&stereo[0],imaxs)) \
    { \
      clust->st[nclust].la[0].nhit = 1; \
      clust->st[nclust].la[0].iw[0] = imaxs; \
      if(DEBUG1) printf(" %3d",clust->st[nclust].la[0].iw[0]); \
      clust->st[nclust].ird = imaxs - shift[sgshift[nclust]][1]; \
    } \
    else \
    { \
      clust->st[nclust].la[0].nhit = 0; \
      if(DEBUG1) printf(" %3d",0); \
      clust->st[nclust].ird = 254; /*mark by non-existing wire number*/ \
    } \
    for(i=0; i<5; i++) \
    { \
      iwtmp = imaxs - shift[sgshift[nclust]][i]; \
	  if(CheckBitWord##XXX(&stereo[i+1],iwtmp)) \
      { \
        clust->st[nclust].la[i+1].nhit = 1; \
        clust->st[nclust].la[i+1].iw[0] = iwtmp; \
        if(DEBUG1) printf(" %3d",clust->st[nclust].la[i+1].iw[0]); \
if(clust->st[nclust].ird == 254) clust->st[nclust].ird = iwtmp; /*imax ???*/ \
      } \
      else \
      { \
        clust->st[nclust].la[i+1].nhit = 0; \
        if(DEBUG1) printf(" %3d",0); \
      } \
    } \
    if(DEBUG1) printf("\n"); \
  } \
  *ncl = clust->nax = nclust


/* 160micros */
void
SegmentSearch192(Word192 axial[6], Word192 stereo[6], int *ncl, PRRG *clust)
{
  LOCALVAR1(192);

  TWOSLPROCESS(192);
  REGIONPROCESS(192);
  PEAKSEARCH(192);
  SEGMENTLIST(192);

  return;
}

/* just to enforce 2-hit segments in R1 stereo ... */
void
SegmentSearch128(Word192 axial[6], Word192 stereo[6], int *ncl, PRRG *clust)
{
  LOCALVAR1(192);

  TWOSLPROCESS_2(192);
  REGIONPROCESS_2(192);
  PEAKSEARCH(192);
  SEGMENTLIST(192);

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
  {
    for(i=0; i<6; i++) /* need to be tested */
    {
      b1 = (*w0)&(*w1);
      b0 = (*w0++)^(*w1++);
      b1 = (b1)^((b0)&(*w2));
      b0 = (b0)^(*w2++);

      tmp = (b0)&(*w3);
      b2 = (b1)&tmp;
      b1 = (b1)^tmp;
      b0 = (b0)^(*w3++);

      *out++ = SLMIN2; /*was SLMIN3*/
    }
  }
  else
  {
    for(i=0; i<6; i++)
    {
      b1 = (*w0)&(*w1);
      b0 = (*w0++)^(*w1++);
      b1 = (b1)^((b0)&(*w2));
      b0 = (b0)^(*w2++);

      tmp = (b0)&(*w3);
      b2 = (b1)&tmp;
      b1 = (b1)^tmp;
      b0 = (b0)^(*w3++);

      tmp = (b0)&(*w4);
      b2 = (b2)^((b1)&tmp);
      b1 = (b1)^tmp;
      b0 = (b0)^(*w4++);

      tmp = (b0)&(*w5);
      b2 = (b2)^((b1)&tmp);
      b1 = (b1)^tmp;
      b0 = (b0)^(*w5++);
      
      *out++ = SLMIN3;
    }
  }

  return;
}


/********************* RemoveNoise ***************************
  Input:
    Word192  data[6] - actual data with noise 
    int      nlayers - number of layers in this superlayer
    int      nwires  - max number of wires in a layer in this superlayer 
    int     *layershifts - 
  Output:
    Word192  data[6] - cleaned data
**************************************************************/

int
RemoveNoise(Word192 data[6], int nlayers, int nwires, int *layershifts)
{
  int i, itmp;
  Word192 workspace[6], wrk;
  Word192 Lsegments, Rsegments; /* cumulative left & right segments */


  /**************************/
  /* look for segment areas */

  /* copy data[] to workspace[] and shift the data for each layer
     according to the layer shift (i.e the buckets) in both
     left and right directions */


  /*memcpy(workspace, data, nlayers*sizeof(Word192));*/
  for(i=0; i<nlayers; i++)
  {
    CopyWord192(&data[i], &workspace[i]);
  }


  for(i=1; i<nlayers; i++) BleedRightWord192(&workspace[i], layershifts[i]);
  SuperLayerSum(workspace, &Lsegments, nlayers);


  /*memcpy(workspace, data, nlayers*sizeof(Word192));*/
  for(i=0; i<nlayers; i++)
  {
    CopyWord192(&data[i], &workspace[i]);
  }


  for(i=1; i<nlayers; i++) BleedLeftWord192(&workspace[i], layershifts[i]);
  SuperLayerSum(workspace, &Rsegments, nlayers);


  /************************/
  /* now remove the noise */

  /* first, set workspace[0] to contain overlap of both sets of segments
  NOTE: the first layer (layer 0) NEVER has a layer shift */

  ORWord192(&Lsegments, &Rsegments, &workspace[0]);
/*
printf("LRsegm\n");
PrintWord192(&workspace[0]);
*/
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




#ifdef ALTIVEC

void
avec()
{
  /* causes the compiler to allocate the global variable x
     on a 16-byte boundary:
  int x __attribute__ ((aligned (16))) = 0;
  */

  int i, j __attribute__ ((aligned (16))); /* causes the compiler to allocate
                                            variables on a 16-byte boundary */
  PRNT256 prn;
  unsigned int tst[8] __attribute__ ((aligned (16)));
  vector unsigned char shiftFactor, negShiftFactor;
  vector unsigned int vEven;
  vector unsigned int vOdd;

  printf("alignment: int=%d long=%d vector int=%d vector long=%d\n",
    __alignof__(unsigned int),  __alignof__(unsigned long),
    __alignof__(vector unsigned int), __alignof__(vector unsigned long));

  printf("addresses: 0x%08x 0x%08x 0x%08x 0x%08x\n",
    (int)&vEven, (int)&vOdd, (int)tst, (int)prn.v256);

  tst[0] = 0xf0010001;
  tst[1] = 0xf0020002;
  tst[2] = 0xf0040004;
  tst[3] = 0xf0080008;
  tst[4] = 0xf0010010;
  tst[5] = 0xf0012012;
  tst[6] = 0xf0014014;
  tst[7] = 0xf0018018;
  memcpy((char *)prn.x, (char *)tst, 8*sizeof(unsigned int));

  PRINT(1);
  SHIFTLEFT256_1(prn.v256);
  for(i=0; i<100000; i++) j=i;
  PRINT(2);
  SHIFTLEFT256_1(prn.v256);
  PRINT(3);
  SHIFTRIGHT256_1(prn.v256);
  PRINT(4);

  return;
}

#endif


/* general shift test */
void
sgtest1()
{
  unsigned int tst[6];
  Word192 word1;
  unsigned long *w1 = (unsigned long *)word1.words;

  tst[0] = 0xf0010001;
  tst[1] = 0xf0020002;
  tst[2] = 0xf0040004;
  tst[3] = 0xf0080008;
  tst[4] = 0xf0010010;
  tst[5] = 0xf0012012;

  ClearWord192(&word1);
  PrintWord192(&word1);

  memcpy((char *)w1, (char *)tst, 24);
  PrintWord192(&word1);

  RightShiftWord192_01(&word1);
  PrintWord192(&word1);

  LeftShiftWord192_01(&word1);
  PrintWord192(&word1);

  LeftShiftWord192_01(&word1);
  PrintWord192(&word1);

  RightShiftWord192_01(&word1);
  PrintWord192(&word1);

  return;
}

void
sgtest2()
{
  static int layershifts[6] = {0, 2, 3, 3, 4, 4};
  unsigned int tst0,tst1,tst2,tst3,tst4,tst5,tst6,tst7,tst8,tst9,tst10,tst11;
  Word192 data0[6], data1[6];
  int nclust;
  PRRG clust;
  unsigned long *w11 = (unsigned long *)data1[5].words;
  unsigned long *w10 = (unsigned long *)data1[4].words;
  unsigned long *w9 =  (unsigned long *)data1[3].words;
  unsigned long *w8 =  (unsigned long *)data1[2].words;
  unsigned long *w7 =  (unsigned long *)data1[1].words;
  unsigned long *w6 =  (unsigned long *)data1[0].words;
  unsigned long *w5 =  (unsigned long *)data0[5].words;
  unsigned long *w4 =  (unsigned long *)data0[4].words;
  unsigned long *w3 =  (unsigned long *)data0[3].words;
  unsigned long *w2 =  (unsigned long *)data0[2].words;
  unsigned long *w1 =  (unsigned long *)data0[1].words;
  unsigned long *w0 =  (unsigned long *)data0[0].words;

  AxialPlusStereoInit();

  ClearWord192(&data1[5]);
  ClearWord192(&data1[4]);
  ClearWord192(&data1[3]);
  ClearWord192(&data1[2]);
  ClearWord192(&data1[1]);
  ClearWord192(&data1[0]);

  ClearWord192(&data0[5]);
  ClearWord192(&data0[4]);
  ClearWord192(&data0[3]);
  ClearWord192(&data0[2]);
  ClearWord192(&data0[1]);
  ClearWord192(&data0[0]);






  /*
  tst11 = 0x0000101f;
  tst10 = 0x00000006;
  tst9 =  0x10000006;
  tst8 =  0x00000006;
  tst7 =  0x01000004;
  tst6 =  0x00000004;

  tst5 =  0x00001006;
  tst4 =  0x00000002;
  tst3 =  0x10000002;
  tst2 =  0x00000004;
  tst1 =  0x01000004;
  tst0 =  0x00000004;
  */
  /*
  tst11 = 0x000101f0;
  tst10 = 0x00000060;
  tst9 =  0x00000060;
  tst8 =  0x00000000;
  tst7 =  0x10000040;
  tst6 =  0x00000040;

  tst5 =  0x00001006;
  tst4 =  0x00000002;
  tst3 =  0x10000000;
  tst2 =  0x00000006;
  tst1 =  0x01000004;
  tst0 =  0x0000000c;
  */
  tst11 = 0x000101f0;
  tst10 = 0x00000000;
  tst9 =  0x00000060;
  tst8 =  0x00000000;
  tst7 =  0x10000040;
  tst6 =  0x00000040;

  tst5 =  0x00001006;
  tst4 =  0x00000002;
  tst3 =  0x10000000;
  tst2 =  0x00000000;
  tst1 =  0x01000004;
  tst0 =  0x0000000c;




  memcpy((char *)&w11[2], (char *)&tst11, 4);
  memcpy((char *)&w10[2], (char *)&tst10, 4);
  memcpy((char *)&w9[2],  (char *)&tst9, 4);
  memcpy((char *)&w8[2],  (char *)&tst8, 4);
  memcpy((char *)&w7[2],  (char *)&tst7, 4);
  memcpy((char *)&w6[2],  (char *)&tst6, 4);
  memcpy((char *)&w5[2],  (char *)&tst5, 4);
  memcpy((char *)&w4[2],  (char *)&tst4, 4);
  memcpy((char *)&w3[2],  (char *)&tst3, 4);
  memcpy((char *)&w2[2],  (char *)&tst2, 4);
  memcpy((char *)&w1[2],  (char *)&tst1, 4);
  memcpy((char *)&w0[2],  (char *)&tst0, 4);



  printf("\nDATA\n");
  PrintWord192(&data1[5]);
  PrintWord192(&data1[4]);
  PrintWord192(&data1[3]);
  PrintWord192(&data1[2]);
  PrintWord192(&data1[1]);
  PrintWord192(&data1[0]);
  printf("\n");
  PrintWord192(&data0[5]);
  PrintWord192(&data0[4]);
  PrintWord192(&data0[3]);
  PrintWord192(&data0[2]);
  PrintWord192(&data0[1]);
  PrintWord192(&data0[0]);

  RemoveNoise(data0, 6, 192, layershifts);
  RemoveNoise(data1, 6, 192, layershifts);

  printf("\nNOISE REMOVED\n");
  PrintWord192(&data1[5]);
  PrintWord192(&data1[4]);
  PrintWord192(&data1[3]);
  PrintWord192(&data1[2]);
  PrintWord192(&data1[1]);
  PrintWord192(&data1[0]);
  printf("\n");
  PrintWord192(&data0[5]);
  PrintWord192(&data0[4]);
  PrintWord192(&data0[3]);
  PrintWord192(&data0[2]);
  PrintWord192(&data0[1]);
  PrintWord192(&data0[0]);
  printf("\n\n");
  
  SegmentSearch192(data0, data1, &nclust, &clust);
  /*  
  printf("\nSEGMENTS FOUND\n");
  PrintWord192(&data1[5]);
  PrintWord192(&data1[4]);
  PrintWord192(&data1[3]);
  PrintWord192(&data1[2]);
  PrintWord192(&data1[1]);
  PrintWord192(&data1[0]);
  printf("\n");
  PrintWord192(&data0[5]);
  PrintWord192(&data0[4]);
  PrintWord192(&data0[3]);
  PrintWord192(&data0[2]);
  PrintWord192(&data0[1]);
  PrintWord192(&data0[0]);
  */
  printf("\n\n");
  return;
}
