/*
   tdc1176 LeCroy VME board - Sergey Boyarinov

  TIPS:

  while( tdc1176ready(TDCADR) ) {;}
  rol->dabufp += tdc1176read(TDCADR,rol->dabufp);

 
 */

#ifdef VXWORKS

#include <vxWorks.h> 
#include <stdio.h>

typedef struct LeCroy1176CSR *CSRPtr;
typedef struct LeCroy1176CSR
{
  unsigned empty        : 2; /* D31-D30 - not implemented */
  unsigned full         : 1; /* D29 - Event Buffer Full Flag */
  unsigned count        : 5; /* D28-D24 - Event Counter */
  unsigned clear        : 1; /* D23 - Clear (write only, reads 0) */
  unsigned roip         : 1; /* D22 - Readout In Progress (read only) */
  unsigned clr_acq      : 1; /* D21 - CLR_ACQ (read only) */
  unsigned disable      : 1; /* D20 - Disable Acquisition */
  unsigned enable       : 1; /* D19 - Enable AUX Fast Clear */
  unsigned fast_clear   : 3; /* D18-D16 - Fast Clear Window */
  unsigned acq_mode     : 1; /* D15 - Acquisition Mode */
  unsigned trig_mode    : 1; /* D14 - Trigger Mode */
  unsigned rising_edge  : 1; /* D13 - Hit Rising Edge Registration */
  unsigned falling_edge : 1; /* D12 - Hit Falling Edge Registration */
  unsigned enable_hit   : 1; /* D11 - Enable AUX Common Hit */
  unsigned start        : 3; /* D10-D8 - Common Start Acquisition Timeout */
  unsigned test         : 8; /* D7-D0 - Tester Field */
} CSR;

typedef struct LeCroy1176TDC *TDCPtr;
typedef struct LeCroy1176TDC
{
  unsigned empty   : 8; /* D31-D24 - always zero */
  unsigned last    : 1; /* D23 - zero means last word */
  unsigned zero    : 1; /* D22 - always zero */
  unsigned valid   : 1; /* D21 - equal 0 for empry event */
  unsigned channel : 4; /* D20-D17 - Channel Number */
  unsigned edge    : 1; /* D16 - edge: 0-Falling(Trailing),1-Rising(Leading) */
  unsigned data    : 16; /* D15-D0 - Time Data */
} TDC;

#define NCSR 0x8000   /* offset for status register */

/* base address A23-A16 could be set by switches on a board */

/*---------------------------------------------------------------*/
/*---------------------------- tdc1176 --------------------------*/
/*---------------------------------------------------------------*/

/* test module at base address addr */
void tdc1176test(int addr)
{
  unsigned char *bufptr;
  bufptr = (unsigned char *) (addr + NCSR);

  /* Initiate CSR */

  bufptr[0] = 0x00; /* Reset Event Counter */
  bufptr[1] = 0x00; /* Set FCW lus, enable ACQ */
  bufptr[2] = 0x40; /* Common Stop, En Rise, Fall */
  bufptr[3] = 0xfe; /* Test Cycle: 8 Pulses, 1000ns to odd and even channels */

  /* Issue a Master Reset */

  bufptr[1] = 0x80; /* write a one to CSR bit 23 */

  /* execute one cycle of internal test */

  bufptr[3] = 0xff;

}

/* reset module at base address addr */
void tdc1176init(int addr)
{
  unsigned char *buf8;
  buf8 = (unsigned char *) (addr + NCSR);

  buf8[0] = 0x00; /* Reset Event Counter */
  buf8[1] = 0x00; /* Set FCW lus, enable ACQ */
  buf8[2] = 0xd0; /* Common Start, Pulse With Each Hit, Rising Hits Only */
  buf8[3] = 0x00; /* disable test */

  buf8[1] = 0x80; /* Master Reset */

  return;
}

/* is conversion complete in module at base address addr ? */
/*       return(1) if complete                             */
/*       return(0) if not complete                         */
unsigned int tdc1176ready(int addr)
{
  CSR *csr;

  csr = (CSRPtr) (addr + NCSR);

  return(csr->roip);
}

/* read data from module at base address addr to buffer */
unsigned int tdc1176read(int addr, unsigned int *buffer)
{
  unsigned int len;
  unsigned int *bufin, *bufout;
  unsigned char *buf8;

  bufout = buffer;
  bufin  = (unsigned int *) addr;
  buf8   = (unsigned char *) (addr+NCSR);

  buf8[0] = 0x00; /* Reset Event Counter */

  len = 0;

  do
  {
    len++;
    *bufout++ = *bufin++;
  } while( (*(bufout-1) & 0x00800000) ); /* read while bit 23 != 0 */

  return(len);
}

/* print data */
void tdc1176printest(int addr)
{

  TDC *tdc;
  unsigned int i, len, buffer[512];

  len = tdc1176read(addr, buffer);

  printf("Data(hex)     Edge      Channel     Time(bins)\n");
  for(i=0; i<len; i++)
  {
    tdc = (TDC *) &buffer[i];
    printf("%08x        %d         %2d          %5d\n",
            buffer[i],tdc->edge,tdc->channel,tdc->data);
  }

  return;
}

#else /* no UNIX version */

void
tdc1176_dummy()
{
  return;
}

#endif


