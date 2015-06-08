/*
 *  ppcTimer.h  - Header file for PowerPC hardware timer library
 *
 */


/* Define the Default Clock freqency for the memory Bus
   then the time base is DEC_CLOCK_FREQ/4  counts/sec */
/*#define DEC_CLOCK_FREQ   133333332   for MV6100 MV5500 */
#define DEC_CLOCK_FREQ  100000000  /*  for MV2400 MV5100 */
/*#define DEC_CLOCK_FREQ  66666666    for MV2300 MV2600 MV2700 */

/* PPC is big endian so put High word first to make it look like a long long */
typedef struct ppcTB {
  unsigned int tbH;
  unsigned int tbL;
} PPCTB;


/* prototypes */
void ppcTimeBaseGet(PPCTB *tb);
unsigned int ppcTimeBaseFreqGet();
int ppcTimeBaseBusSpdSet(unsigned int freq);
void ppcTimeBaseZero();
double ppcTimeBaseDuration(PPCTB *t1, PPCTB *t2);
void ppcUsDelay (unsigned int delay);
