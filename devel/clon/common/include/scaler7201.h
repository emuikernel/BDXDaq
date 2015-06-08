
/* scaler7201.h */


typedef struct Struck7201DAT *DATPtr;
typedef struct Struck7201DAT
{
  unsigned usr2    : 1; /* D31 - user bit 2 */
  unsigned usr1    : 1; /* D30 - user bit 1 */
  unsigned bank    : 1; /* D29 - bank */
  unsigned channel : 5; /* D28-D24 - Channel Number */
  unsigned data    : 24; /* D23-D0 - Scaler Contents */
} DAT;


/* masks for control register */

#define ENABLE_EXT_NEXT   0x00010000
#define DISABLE_EXT_NEXT  0x01000000
#define ENABLE_EXT_CLEAR  0x00020000
#define DISABLE_EXT_CLEAR 0x02000000
#define ENABLE_EXT_DIS    0x00040000
#define DISABLE_EXT_DIS   0x04000000

#define ENABLE_MODE0      0x00000000
#define ENABLE_MODE1      0x00000004
#define ENABLE_MODE2      0x00000008
#define ENABLE_MODE3      0x0000000c
#define DISABLE_MODES     0x00000c00

#define ENABLE_TEST       0x00000020
#define DISABLE_TEST      0x00002000

#define ENABLE_25MHZ      0x00000010
#define DISABLE_25MHZ     0x00001000

#define ENABLE_IRQ        0x00f00000
#define ENABLE_IRQ0       0x00100000
#define ENABLE_IRQ1       0x00200000
#define ENABLE_IRQ2       0x00400000
#define ENABLE_IRQ3       0x00800000

#define DISABLE_IRQ       0xf0000000
#define DISABLE_IRQ0      0x10000000
#define DISABLE_IRQ1      0x20000000
#define DISABLE_IRQ2      0x40000000
#define DISABLE_IRQ3      0x80000000

/* masks for status register */

#define FIFO_FULL         0x00001000
#define FIFO_ALMOST_FULL  0x00000800
#define FIFO_HALF_FULL    0x00000400
#define FIFO_ALMOST_EMPTY 0x00000200
#define FIFO_EMPTY        0x00000100


/* functions */

int scaler7201readfifo(int addr);
void scaler7201writefifo(int addr, int value);
int scaler7201status(int addr);
void scaler7201control(int addr, int value);
void scaler7201mask(int addr, int value);
void scaler7201clear(int addr);
void scaler7201nextclock(int addr);
void scaler7201enablenextlogic(int addr);
void scaler7201disablenextlogic(int addr);
void scaler7201reset(int addr);
void scaler7201testclock(int addr);

void scaler7201ledon(int addr);
void scaler7201ledoff(int addr);
int scaler7201almostread(int addr, int *value);
int scaler7201readHLS(int addr, int *ring, int counter);
int scaler7201restore(int addr, int mask);
int scaler7201read(int addr, int *value);
void scaler7201test(int addr);




/* obsolete sfuff */



typedef struct Struck7201SR *SRPtr;
typedef struct Struck7201SR
{
  unsigned bit31 : 1;
  unsigned bit30 : 1;
  unsigned bit29 : 1;
  unsigned bit28 : 1;
  unsigned bit27 : 1;
  unsigned bit26 : 1;
  unsigned bit25 : 1;
  unsigned bit24 : 1;
  unsigned bit23 : 1;
  unsigned bit22 : 1;
  unsigned bit21 : 1;
  unsigned bit20 : 1;
  unsigned bit19 : 1;
  unsigned bit18 : 1;
  unsigned bit17 : 1;
  unsigned bit16 : 1;
  unsigned bit15 : 1;
  unsigned bit14 : 1;
  unsigned bit13 : 1;
  unsigned bit12 : 1;
  unsigned bit11 : 1;
  unsigned bit10 : 1;
  unsigned bit09 : 1;
  unsigned bit08 : 1;
  unsigned bit07 : 1;
  unsigned bit06 : 1;
  unsigned bit05 : 1;
  unsigned bit04 : 1;
  unsigned bit03 : 1;
  unsigned bit02 : 1;
  unsigned bit01 : 1;
  unsigned bit00 : 1;
} SR;

typedef struct Struck7201CR *CRPtr;
typedef struct Struck7201CR
{
  unsigned bit31 : 1;
  unsigned bit30 : 1;
  unsigned bit29 : 1;
  unsigned bit28 : 1;
  unsigned bit27 : 1;
  unsigned bit26 : 1;
  unsigned bit25 : 1;
  unsigned bit24 : 1;
  unsigned bit23 : 1;
  unsigned bit22 : 1;
  unsigned bit21 : 1;
  unsigned bit20 : 1;
  unsigned bit19 : 1;
  unsigned bit18 : 1;
  unsigned bit17 : 1;
  unsigned bit16 : 1;
  unsigned bit15 : 1;
  unsigned bit14 : 1;
  unsigned bit13 : 1;
  unsigned bit12 : 1;
  unsigned mode_off      : 2; /* D11-D10 - clear control input mode */
  unsigned fifo_test_off : 1; /* D9 - disable FIFO_TEST_MODE */
  unsigned led_off       : 1; /* D8 - reset user LED */
  unsigned bit07 : 1;
  unsigned bit06 : 1;
  unsigned bit05 : 1;
  unsigned bit04 : 1;
  unsigned mode_on       : 2; /* D3-D2 - set control input mode */
  unsigned fifo_test_on  : 1; /* D1 - enable FIFO_TEST_MODE */
  unsigned led_on        : 1; /* D0 - set user LED */
} CR;










