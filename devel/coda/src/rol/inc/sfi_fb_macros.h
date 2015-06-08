
/* sfi_fb_macros.h - Header file for SFI specific FASTBUS Macros as well
                   as prototypes for external FASTBUS routines and variables */

extern volatile struct sfiStruct sfi;
extern unsigned long sfiSeq2VmeFifoVal;
extern volatile unsigned long *fastPrimDsr;
extern volatile unsigned long *fastPrimCsr;
extern volatile unsigned long *fastPrimDsrM;
extern volatile unsigned long *fastPrimCsrM;
extern volatile unsigned long *fastPrimHmDsr;
extern volatile unsigned long *fastPrimHmCsr;
extern volatile unsigned long *fastPrimHmDsrM;
extern volatile unsigned long *fastPrimHmCsrM;
extern volatile unsigned long *fastSecadR;
extern volatile unsigned long *fastSecadW;
extern volatile unsigned long *fastSecadRDis;
extern volatile unsigned long *fastSecadWDis;
extern volatile unsigned long *fastRndmR;
extern volatile unsigned long *fastRndmW;
extern volatile unsigned long *fastRndmRDis;
extern volatile unsigned long *fastRndmWDis;
extern volatile unsigned long *fastDiscon;
extern volatile unsigned long *fastDisconRm;
extern volatile unsigned long *fastStartFrdbWithClearWc;
extern volatile unsigned long *fastStartFrdb;
extern volatile unsigned long *fastStoreFrdbWc;
extern volatile unsigned long *fastStoreFrdbAp;
extern volatile unsigned long *fastLoadDmaAddressPointer;
extern volatile unsigned long *fastDisableRamMode;
extern volatile unsigned long *fastEnableRamSequencer;
extern volatile unsigned long *fastWriteSequencerOutReg;
extern volatile unsigned long *fastDisableSequencer;

/* MACROS for SEQUENCER RAM Functions */
#define SFI_RAM_LOAD_ENABLE(a)   {*sfi.sequencerReset = 1;\
                                  *sfi.sequencerRamAddressReg = a;\
                                  *sfi.sequencerRamLoadEnable = 1;}

#define SFI_RAM_LOAD_DISABLE     {*sfi.sequencerRamLoadDisable = 1;\
                                  *sfi.sequencerEnable = 1;}

#define SFI_LOAD_ADDR(a) \
{ \
  if(a) \
  { \
    *fastLoadDmaAddressPointer = (a); \
  } \
  else \
  { \
    *fastLoadDmaAddressPointer = \
        ((unsigned long) (rol->dabufp) + sfi_cpu_mem_offset); \
  } \
}

#define SFI_WAIT   {taskDelay(1);}

#define SFI_SET_OUT_REG(m)     {*fastWriteSequencerOutReg = m;}
#define SFI_CLEAR_OUT_REG(m)   {*fastWriteSequencerOutReg = ((m)<<16);}


/* FASTBUS MACROS for RAM List Programming */
#define FBAC(p,s) { if(s) {*fastPrimCsr = p;*fastSecadW = s;}else{*fastPrimCsr = p;}}
#define FBAD(p,s) { if(s) {*fastPrimDsr = p;*fastSecadW = s;}else{*fastPrimDsr = p;}}
#define FBSAW(s)  {*fastSecadW = s;}

#define FBWC(p,s,d) { if(s) {*fastPrimCsr = p;*fastSecadW = s;*fastRndmWDis = d;}\
                            else\
                            {*fastPrimCsr = p;*fastRndmWDis = d;}}
#define FBWD(p,s,d) { if(s) {*fastPrimDsr = p;*fastSecadW = s;*fastRndmWDis = d;}\
                            else\
                            {*fastPrimDsr = p;*fastRndmWDis = d;}}

#define FBWCM(p,s,d) { if(s) {*fastPrimCsrM = p;*fastSecadW = s;*fastRndmWDis = d;}\
                            else\
                            {*fastPrimCsrM = p;*fastRndmWDis = d;}}
#define FBWDM(p,s,d) { if(s) {*fastPrimDsrM = p;*fastSecadW = s;*fastRndmWDis = d;}\
                            else\
                            {*fastPrimDsrM = p;*fastRndmWDis = d;}}

#define FBR    {*fastRndmR = 1;}
#define FBW(d) {*fastRndmW = d;}
#define FBREL  {*fastDiscon = 1;}

#define FBBR(c)    {*fastStartFrdbWithClearWc = (0x0a<<24)+c;}
#define FBBRF(c)   {*fastStartFrdbWithClearWc = (0x08<<24)+c;}
#define FBBRNC(c)  {*fastStartFrdb = (0x0a<<24)+c;}
#define FBBRNCF(c) {*fastStartFrdb = (0x08<<24)+c;}

#define FBSWC  {*fastStoreFrdbWc = 1;}
#define FBEND  {*fastDisableRamMode = 1;}





