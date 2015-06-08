/* Define a Structure for access to Discriminator */
/* All registers are 16 bits*/
typedef struct v2718_struct
{
  /*0x00*/ volatile unsigned short status;	          /* r/w; D16  */
  /*0x01*/ volatile unsigned short ctrl;		  /* r/w; D16  */
  /*0x02*/ volatile unsigned short firmware_version;      /* read only; D16  */
  /*0x03*/ volatile unsigned short firmware_download;
  /*0x04*/ volatile unsigned short flash_enable;
} V2718;

/* Function Prototypes */
void v2718Init(unsigned int BA);
int  v2718GetFirmwareRev();
int  v2718GetStatus();
int  v2718GetDisplayAddress();

#define UINT32 unsigned short
#define UINT16 unsigned short
#define INT32  int

