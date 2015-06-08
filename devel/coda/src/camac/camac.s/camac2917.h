/*
   camac2917.h - copied from ks2917.h and adjusted for multiple
                 branches by Sergey Boyarinov
*/


/* typedefs */

typedef struct camac2917
{
  /* DMA registers */

  unsigned short cser;  /* ks2917_BASE+0x00 */
  unsigned short res02;
  unsigned short docr;  /* ks2917_BASE+0x04 */
  unsigned short sccr;  /* ks2917_BASE+0x06 */
  unsigned short res08;
  unsigned short mtc;   /* ks2917_BASE+0x0a */
  unsigned short machi; /* ks2917_BASE+0x0c */
  unsigned short maclo; /* ks2917_BASE+0x0e */

  unsigned short res10;
  unsigned short res12;
  unsigned short res14;
  unsigned short res16;
  unsigned short res18;
  unsigned short res1a;
  unsigned short res1c;
  unsigned short res1e;

  unsigned short res20;
  unsigned short res22;
  unsigned short res24;
  unsigned short res26;
  unsigned short res28;
  unsigned short res2a;
  unsigned short res2c;
  unsigned short res2e;

  unsigned short res30;
  unsigned short res32;
  unsigned short res34;
  unsigned short res36;
  unsigned short res38;
  unsigned short res3a;
  unsigned short res3c;
  unsigned short res3e;

  /* Interrupt control */

  unsigned short icr_lam;   /* ks2917_BASE+0x40 */
  unsigned short icr_done;  /* ks2917_BASE+0x42 */
  unsigned short icr_dma;   /* ks2917_BASE+0x44 */
  unsigned short icr_abort; /* ks2917_BASE+0x46 */
  unsigned short ivr_lam;   /* ks2917_BASE+0x48 */
  unsigned short ivr_done;  /* ks2917_BASE+0x4a */
  unsigned short ivr_dma;   /* ks2917_BASE+0x4c */
  unsigned short ivr_abort; /* ks2917_BASE+0x4e */

  unsigned short res50;
  unsigned short res52;
  unsigned short res54;
  unsigned short res56;
  unsigned short res58;
  unsigned short res5a;
  unsigned short res5c;
  unsigned short res5e;

  /* Other Registers */

  unsigned short amr;  /* ks2917_BASE+0x60 */
  unsigned short cmr;  /* ks2917_BASE+0x62 */
  unsigned short cmar; /* ks2917_BASE+0x64 */
  unsigned short cwc;  /* ks2917_BASE+0x66 */
  unsigned short srr;  /* ks2917_BASE+0x68 */
  unsigned short dlr;  /* ks2917_BASE+0x6a */
  unsigned short dhr;  /* ks2917_BASE+0x6c */
  unsigned short csr;  /* ks2917_BASE+0x6e */

} CAMAC2917;


/******************** Register Definitions **********************************/

#define ks2917_ws24 0
#define ks2917_ws16 2

#define ks2917_qm_stop   0x00
#define ks2917_qm_ignore 0x08
#define ks2917_qm_repeat 0x10
#define ks2917_qm_scan   0x18

#define ks2917_tm_single 0x00
#define ks2917_tm_block  0x20
#define ks2917_tm_inline 0x60

#define ks2917_halt 0x80
#define ks2917_jump 0xc0

#define ks2917_go    0x1
#define ks2917_write 0x20
#define ks2917_dma   0x40
#define ks2917_done  0x80
#define ks2917_rdy   0x100
#define ks2917_reset 0x1000
#define ks2917_timeo 0x2000
#define ks2917_error 0x8000

/************************* DMA register definitions **************************/

#define ks2917_sccr_abrt 0x10

#define ks2917_cser_coc  0x8000
#define ks2917_cser_ndt  0x2000
#define ks2917_cser_err  0x1000
#define ks2917_cser_act  0x800
#define ks2917_cser_rdy  0x100
#define ks2917_cser_code 0x1f
#define ks2917_cser_berr 0x09
#define ks2917_cser_abrt 0x11


/********************* Misc Definitions, Prototypes **************************/

#define MAX_CDREG_PATTERN 400 /* max 455=8192words buffer/18 words per cnaf */
#define KS2917_OFFSET_QSCAN 3	 /* offset to q scan naf */
#define KS2917_OFFSET_QSTOP 8	 /* offset to q stop naf */
#define KS2917_OFFSET_QREPEAT 13 /* offset to q repeat naf */



/********************************************/
/*********** FUNCTION PROTOTYPES ************/

void ccinit(int b);
void cdreg(int *ext, int b, int c, int n, int a);
void cfsa(int f, int ext, unsigned int *dat, unsigned int *q);
void cssa(int f, int ext, unsigned short *dat, unsigned int *q);
void cccz(int ext);
void cccc(int ext);
void ccci(int ext, int l);
void ctci(int ext, int *l);
void cccd(int ext, int l);
void ctcd(int ext, int *l);
void ctstatus(int ext, int *istat);
void ctstat(int *istat);
void ccgl(int lam, int l);
void ctgl(int ext, int *l);
void cdlam(int *ext, int b, int c, int n, int a, int inta[2]);
void cclc(int lam);
void cclm(int lam, int l);
void ctlm(int lam, int *l);
void lamIsr(int b);
void cclnk(int lam, VOIDFUNCPTR label);
void cculk(int lam, VOIDFUNCPTR label);
void ccrgl(int lam);
void cclwt(int lam);
void clpoll(int lam);
void camac2917scan();

