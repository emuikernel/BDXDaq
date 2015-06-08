/******************************************************************

 g0helicity.c    R. Michaels   July 29, 2002

      modified 6-mar-2006 by Sergey Boyarinov
 This code performs the following tasks as would be

 expected of an analysis code:

   1. Reads the helicity info -- readHelicity()
      In this case the helicity comes from a text file 
      for test purposes, but one can imagine it coming
      from the datastream.

   2. Loads the shift register to calibrate it. 
      The shift register is a word of NBIT bits of 0's and 1's.
      After loading is done, helicity predictions are available
      as global variables like "present_helicity".
        -- loadHelicity(int)
            returns 0  if loading not finished yet
            returns 1  if loading finished (helicity available)

   3. If the helicity is wrong, the code sets up recovery.
      by setting global variable recovery_flag.

  Other routines used:

   ranBit(unsigned int *seed)  -- returns helicity based on seed value
                                 also modifies the seed (arg)
   getSeed()  -- returns seed value based on string of NBIT bits.

******************************************************************/




#define NDELAY 2         /* number of quartets between */
						 /* present reading and present helicity  */
						 /* (i.e. the delay in reporting the helicity) */

/* Functions to perform */
void forceRecovery();
int loadHelicity(int, unsigned int *, unsigned int *);
int ranBit(unsigned int *seed);
int ranBit0(unsigned int *seed);
unsigned int getSeed();

/* static variables */
#define NBIT 24
static int hbits[NBIT];       /* The NBIT shift register */
static int recovery_flag = 1; /* flag to determine if we need to recover */
						      /* from an error (1) or not (0) */


void
forceRecovery()
{
  recovery_flag = 1;
}


/*************************************************************
 loadHelicity() - loads the helicity to determine the seed.

   on first NBIT calls it just fills an array hbits[NBIT] with
   helicity information from data stream and returns 0;

   on (NBIT+1)th call it does all necessary calculations and
   returns 1 telling main program that it is ready to predict

   so after loading (nb==NBIT), predicted helicities are available

*************************************************************/
int
loadHelicity(int input_helicity, unsigned int *iseed, unsigned int *iseed_earlier)
{
  static int nb;
  unsigned int iseed0, iseed_earlier0;
  int tmp, i;

  if(recovery_flag) nb = 0;
  recovery_flag = 0;

  if(nb < NBIT)
  {
    hbits[nb] = input_helicity;
    nb++;
    return(0);
  }
  else if(nb == NBIT) /* Have finished loading */
  {
    /* obtain the seed value from a collection of NBIT bits */
    iseed_earlier0 = getSeed();

    /* get predicted reading (must coinside with current readout) */
    /* skipping 24 times since we read them already ??? */
    /* why (NBIT+1) ?? maybe because of former main() .. */
    for(i=0; i<NBIT/*+1*/; i++) tmp = ranBit(&iseed_earlier0);

    /* get prediction for present helicity */
    iseed0 = iseed_earlier0;
    for(i=0; i<NDELAY; i++) tmp = ranBit(&iseed0);

    nb++;
    printf("iseed_earlier=0x%08x iseed=0x%08x\n",iseed_earlier0,iseed0);

    *iseed = iseed0;
    *iseed_earlier = iseed_earlier0;

    return(1);
  }
}
 



/*************************************************************
// This is the random bit generator according to the G0
// algorithm described in "G0 Helicity Digital Controls" by 
// E. Stangland, R. Flood, H. Dong, July 2002.
// Argument:
//        ranseed = seed value for random number. 
//                  This value gets modified.
// Return value:

//        helicity (0 or 1)
*************************************************************/
int
ranBit(unsigned int *ranseed)
{
  static int IB1 = 0x1;           /* Bit 1 */
  static int IB3 = 0x4;           /* Bit 3 */
  static int IB4 = 0x8;           /* Bit 4 */
  static int IB24 = 0x800000;     /* Bit 24 */
  static int MASK;
  unsigned int seed;

  MASK = IB1+IB3+IB4+IB24;
  seed = *ranseed;
  if(seed & IB24)
  {    
    seed = ((seed^MASK)<<1) | IB1;
    *ranseed = seed;
    return(1);
  }
  else
  {
    seed <<= 1;
    *ranseed = seed;
    return(0);
  }
}

/* following function does NOT modifying *ranseed */
int
ranBit0(unsigned int *ranseed)
{
  static int IB1 = 0x1;           /* Bit 1 */
  static int IB3 = 0x4;           /* Bit 3 */
  static int IB4 = 0x8;           /* Bit 4 */
  static int IB24 = 0x800000;     /* Bit 24 */
  static int MASK;
  unsigned int seed;

  MASK = IB1+IB3+IB4+IB24;
  seed = *ranseed;
  if(seed & IB24)
  {    
    seed = ((seed^MASK)<<1) | IB1;
    /**ranseed = seed;*/
    return(1);
  }
  else
  {
    seed <<= 1;
    /**ranseed = seed;*/
    return(0);
  }
}



/*************************************************************
// getSeed
// Obtain the seed value from a collection of NBIT bits.
// This code is the inverse of ranBit.
// Input:
//       int hbits[NBIT]  -- global array of bits of shift register
// Return:
//       seed value
*************************************************************/
unsigned int
getSeed()
{
  int seedbits[NBIT];
  unsigned int ranseed = 0;
  int i;

  if(NBIT != 24)
  {
    printf("getSeed: ERROR: NBIT is not 24.  This is unexpected.\n");
    printf("getSeed: Code failure...\n");
    return(0);
  }
  for(i=0; i<20; i++) seedbits[23-i] = hbits[i];
  seedbits[3] = hbits[20]^seedbits[23];
  seedbits[2] = hbits[21]^seedbits[22]^seedbits[23];
  seedbits[1] = hbits[22]^seedbits[21]^seedbits[22];
  seedbits[0] = hbits[23]^seedbits[20]^seedbits[21]^seedbits[23];
  for(i=NBIT-1; i>=0; i--) ranseed = ranseed<<1|(seedbits[i]&1);
  ranseed = ranseed&0xFFFFFF;
  return(ranseed);
}





