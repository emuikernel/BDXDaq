
/* v1495.h */

#define V890_MAX_CHANNELS   128
#define V890_MAX_WORDS_PER_EVENT  34
#define V890_MAX_WORDS_PER_BOARD  3000

/* Define a Structure for access */
/* all addressing is A24/A32, all data D16 except scratch32 */
typedef struct v1495_struct
{
  /*0x0000*/ volatile unsigned short data[16384];    /* R/W */
  /*0x8000*/ volatile unsigned short control;        /* R/W */
  /*0x8002*/ volatile unsigned short status;         /* R */
  /*0x8004*/ volatile unsigned short intLevel;       /* R/W */
  /*0x8006*/ volatile unsigned short intVector;      /* R/W */
  /*0x8008*/ volatile unsigned short geoAddr;        /* R */
  /*0x800A*/ volatile unsigned short moduleReset;    /* W */
  /*0x800C*/ volatile unsigned short firmwareRev;    /* R */
  /*0x800E*/ volatile unsigned short selflashVME;    /* R/W */
  /*0x8010*/ volatile unsigned short flashVME;       /* R/W */
  /*0x8012*/ volatile unsigned short selflashUSER;   /* R/W */
  /*0x8014*/ volatile unsigned short flashUSER;      /* R/W */
  /*0x8016*/ volatile unsigned short configUSER;     /* R/W */
  /*0x8018*/ volatile unsigned short scratch16;      /* R/W */
  /*0x801A*/ volatile unsigned short res1[3];
  /*0x8020*/ volatile unsigned int scratch32;        /* R/W */
  /*0x8024*/ volatile unsigned short res2[110];
  /*0x8100*/ volatile unsigned short configROM[127]; /* R */

} V1495;

#define V1495_BOARD_ID   0x01 /*  */
