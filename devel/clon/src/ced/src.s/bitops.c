#include "ced.h"


/* -------- BitCount -------- */

int BitCount(unsigned x)

/* counts the number of on bits in x. */

{
  int b;

  for (b = 0; x != 0; x &= x-1)
      ++b;

  return b;
}


/* ------- Binary Print  -------*/

void BinaryPrint(unsigned long  k,
		 unsigned long *ktest)

/* prints out an unsigned long in binary format
   send NULL for ktest to ignore */

{
  unsigned long  jj;
  int            i;
  static int     lastbit;

  static unsigned long j = 1;

  if (j == 1) {
    lastbit = 8*sizeof(long) - 1;
    for (i = 0; i < lastbit; i++)
      j *= 2;
  }

  jj = j;
  for (i = lastbit; i >=0; i--) {

    if (ktest) {
      if (*ktest & jj) {
	if (k & jj)
	  fprintf(stdout, "*"); /* both */
	else
	  fprintf(stdout, "X"); /* test only */
      }
      else {
	if (k & jj)
	  fprintf(stdout, "|"); /* real on */
	else
	  fprintf(stdout, "o"); /* real off */
      }
    }

    else if (k & jj)
      fprintf(stdout, "|");
    else
      fprintf(stdout, "o");
    
    jj = jj >> 1;
  }
  
}


/*----- CompositeLong ------- */

unsigned long  CompositeLong(unsigned short high,
			     unsigned short low)

/* builds a long word from two shorts -- putting
   high in bits 16-31 and low in bits 0-15 */

{
  long      lw;

  lw = low + (((unsigned long)high) << 16);
  return(lw);
}

/*----- CompositeShort ------- */

unsigned short  CompositeShort(unsigned char high,
			       unsigned char low)

/* builds a long word from two chars -- putting
   high in bits 8-15 and low in bits 0-7 */

{
  short      sw;

  sw = low + (((unsigned short)high) << 8);
  return  sw;
}


/*----- LowWord ---------- */

unsigned short  LowWord(unsigned long lw)

/* returns the low word (i.e. bits 0..15) of a long word */

{
  long   mask = 0x0000FFFF;

  return((unsigned short)(lw & mask));
}

/*----- HighWord ---------- */

unsigned short  HighWord(unsigned long lw)

/* returns the high word (i.e. bits 16..31) of a long word */

{
  long   mask = 0x0000FFFF;

  return((unsigned short)((lw >> 16) & mask));
}

/*----- LowByte ---------- */

unsigned char  LowByte(unsigned short sw)

/* returns the low byte (i.e. bits 0..7) of a short word */

{
  short   mask = 0x00FF;

  return((unsigned char)(sw & mask));
}

/*----- HighByte ---------- */

unsigned char  HighByte(unsigned short sw)


/* returns the high byte (i.e. bits 8..15) of a short word */

{
  short   mask = 0x00FF;

  return((unsigned char)((sw >> 8) & mask));
}

/* ------- BreakLongWord ----------*/

void BreakLongWord(unsigned int     lw,
		   unsigned short  *high,
		   unsigned short  *low)

/* break the longword into a high and low short */

{
  *high = HighWord(lw);
  *low = LowWord(lw);
}

/* ------- BreakShort ----------*/

void BreakShort(unsigned short sw,
		unsigned char *low,
		unsigned char *high)

/* break the shortword into a high and low byte */


{
  *high = HighByte(sw);
  *low = LowByte(sw);
}


/* ------- LongWordBytes -------- */

void  LongWordBytes(unsigned long   lw,
		    unsigned char   *b3,
		    unsigned char   *b2,
		    unsigned char   *b1,
		    unsigned char   *b0)

{
  unsigned short  high, low;

  BreakLongWord(lw, &high, &low);

  BreakShort(high, b2, b3);
  BreakShort(low,  b0, b1);
}




