
/* fbutil.h - FASTBUS utilities header file */

#define BOARD_ID_MASK  0xffff0000
#define B1872A  0x10360000
#define B1875A  0x10370000
#define B1877   0x103c0000
#define B1877S  0x103d0000
#define B1881M  0x104f0000
#define B1881   0x104b0000
#define B1872   0x10480000
#define B1875   0x10490000

#define SFI_VME_ADDR     0xe00000
#define IRQ_SOURCE_MASK  0x10
#define RAM_ADDR         0x100
#define WC_MASK          0xffffff
#define PHYS             0x10000000
#define SYNC             0x20000000
#define MEK              0x00000001 

/* !!! currently only BROADCAST_1881 in use !!! */

/* 1877/1877S broadcast class will be 0 */
#define BROADCAST_1877  1
#define BROADCAST_1877_CODE  0x5

/* 1881 broadcast class will be 0 */
#define BROADCAST_1881  1
#define BROADCAST_1881_CODE  0x5
/* 1881 broadcast class will be 1 
#define BROADCAST_1881  2
#define BROADCAST_1881_CODE  0x15*/

#define BYPASS       0x0
#define PRIMARY_LINK 0x800
#define END_LINK     0x1000
#define MIDDLE_LINK  0x1800

#define SINGLE 1
#define LAST   2
#define NSLOT 26

#define DefCsr0_1877       0x0
#define DefCsr1_1877       0x0
#define DefCsr18_1877      0x0
#define DefCsr0_1877S      0x0
#define DefCsr1_1877S      0x0
#define DefCsr18_1877S     0x0
#define DefCsr0_1881       0x0
#define DefCsr1_1881       0x0
#define DefCsr0_1872       0x0

/* maximum number of words can be obtained from boards */
#define Adc81Length 85
#define AdcWordLength Adc81Length /* for compartibility */
#define Tdc72Length 84
#define Tdc77Length 1537 /* assuming 96 chans with 16 hits each */

typedef struct brset *Brset;
typedef struct brset
{
  int lastsing[NSLOT];
  int brlength[NSLOT];
  int readflag;
} BRSET;

typedef struct hardmap *Hardmap;
typedef struct hardmap
{
  int map_adc[NSLOT];
  int map_tdc72[NSLOT];
  int map_tdc75[NSLOT]; 
  int map_tdc77[NSLOT]; 
  int map_tdc77S[NSLOT]; 
  int map_unknown[NSLOT]; 
  int map_empty[NSLOT];
} HARDMAP;


typedef struct csr *Csr;
typedef struct csr
{
  unsigned int csr0_1877;
  unsigned int csr1_1877;
  unsigned int csr18_1877;

  unsigned int csr0_1877S;
  unsigned int csr1_1877S;
  unsigned int csr18_1877S;

  unsigned int csr0_1881;
  unsigned int csr1_1881;

  unsigned int csr0_1872;
} CSRS;



/* function prototypes */

int GetMaxEventLength(HARDMAP map, CSRS csr);
struct hardmap what_hardware(int pid, char *location);
struct csr what_settings(int pid, char *location);
unsigned int create_spds_mask(Hardmap rocmap);
struct brset program(int arr[], int blk, Csr roccsr,
         void (*program_module) (int, char *,  Csr roccsr));

void _adc1881(int i, char *str, Csr roccsr);
void _tdc1877(int i, char *str, Csr roccsr);
void _tdc1877S(int i, char *str, Csr roccsr);
void _tdc1872(int i, char *str, Csr roccsr);

void program_hardware(Hardmap pmap, Csr roccsr, int blockread);
void program_hardware_64(Hardmap pmap, Csr roccsr, int low64slot,
                         int high64slot);







