/******************************************************************************
*
* header file for use with General VME based  rols with CODA crl (version 2.0)
*   The trigger source for polling or interrupts is the TJNAF VME Trigger
*   Supervisor Register.
*
*                             DJA   July 1996
*
*******************************************************************************/


/* define various structures for use in accessing commonly
   used VME modules like the Trigger Supervisor, VME trigger interface
   Lecroy scalers etc...  */

#ifdef TS_VERSION_1
struct vme_ts {
    unsigned long csr;       
    unsigned long trig;
    unsigned long roc;
    unsigned long sync;
    unsigned long test;
    unsigned long state;
    unsigned long blank_1;           
    unsigned long blank_2;           
    unsigned long prescale[8];
    unsigned long timer[5];
    unsigned long blank_3;           
    unsigned long blank_4;           
    unsigned long blank_5;           
    unsigned long sc_as;
    unsigned long scale_0a;
    unsigned long scale_1a;
    unsigned long blank_6;           
    unsigned long blank_7;          
    unsigned long scale_0b;
    unsigned long scale_1b;
  };

#else

volatile struct vme_ts {
    volatile unsigned long csr;       
    volatile unsigned long csr2;       
    volatile unsigned long trig;
    volatile unsigned long roc;
    volatile unsigned long sync;
    volatile unsigned long trigCount;
    volatile unsigned long trigData;
    volatile unsigned long lrocData;
    volatile unsigned long prescale[8];
    volatile unsigned long timer[5];
    volatile unsigned long intVec;
    volatile unsigned long rocBufStatus;
    volatile unsigned long lrocBufStatus;
    volatile unsigned long rocAckStatus;
    volatile unsigned long userData1;
    volatile unsigned long userData2;
    volatile unsigned long state;
    volatile unsigned long test;
    volatile unsigned long blank1;
    volatile unsigned long scalAssign;
    volatile unsigned long scalControl;
    volatile unsigned long scaler[18];
    volatile unsigned long scalEvent;
    volatile unsigned long scalLive1;
    volatile unsigned long scalLive2;
    volatile unsigned long id;
  } VME_TS;

#endif

struct vme_tir {
    unsigned short tir_csr;
    unsigned short tir_vec;
    unsigned short tir_dat;
    unsigned short tir_oport;
    unsigned short tir_iport;
  };


/* Define Interrupt Vector and VME level */
#define TIR_DEFAULT_ADDR   0x0ed0
#define TS_DEFAULT_ADDR    0xed0000
#define VME_INT_VEC   0xec
#define VME_INT_LEVEL 5
#define MAX_EVENT_LENGTH  60000

#define VME_POLLING   0
#define VME_INTERRUPT 1

/* Define Hardware Interrupt sources */
#define TIR_SOURCE 1
#define TS_SOURCE  2


#define VME_TIR_VERSION_MASK  0x05c0 
#define VME_TIR_VERSION_1     0x05c0
#define VME_TIR_VERSION_2     0x0580


extern volatile struct vme_ts  *tsP;
extern volatile struct vme_tir *tir[2];

extern  int sysBusToLocalAdrs(int, char *, char **);
extern  int intDisconnect();
extern  int sysIntEnable(int);

extern unsigned int vmeIntCount;
extern unsigned int vmeNeedAck;
extern unsigned int vmeLateFail, vmeSyncFlag;


/*Macros */


/* prototypes */
void vmeUserTrigger(int arg);
int vmeIntStatus();
int vmeUserGetCount();


