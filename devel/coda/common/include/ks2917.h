/*-----------------------------------------------------------------------------
 * Copyright (c) 1991,1992 Southeastern Universities Research Association,
 *                         Continuous Electron Beam Accelerator Facility
 *
 * This software was developed under a United States Government license
 * described in the NOTICE file included as part of this distribution.
 *
 * CEBAF Data Acquisition Group, 12000 Jefferson Ave., Newport News, VA 23606
 * Email: coda@cebaf.gov  Tel: (804) 249-7101  Fax: (804) 249-7363
 *-----------------------------------------------------------------------------
 * 
 * Description:
 *   A C macro implementation of the CAMAC standard routines 
 *   using C argument passing conventions.
 *	
 * Author:  Chip Watson   (macro adaptation, level C)
 *	    Graham Heyes  (original level B)
 *	    CEBAF Data Acquisition Group
 *
 * Revision History:
 *   $Log: ks2917.h,v $
 *   Revision 1.1.1.1  1996/08/21 19:20:57  heyes
 *   Imported sources
 *
*	  Revision 1.1  94/03/16  07:53:45  07:53:45  heyes (Graham Heyes)
*	  Initial revision
*	  
*	  Revision 1.1  94/03/15  11:48:01  11:48:01  heyes (Graham Heyes)
*	  Initial revision
*	  
 *	  Revision 1.3  1993/05/11  12:40:04  heyes
 *	  fix f==8 problem
 *
 *	  Revision 1.2  1993/04/06  20:07:11  watson
 *	  redid cssa, fixed ccci, cclm, ctstat
 *
 *	  Revision 1.1  1992/12/08  18:54:37  watson
 *	  Initial revision
 *
 */

/* Macros (all return void):
 *
 * CDREG(int *ext,int b,int c,int n,int a)   define external address 
 * CSSA(int f,int ext,int *dat,int *q)       short (16) single action 
 * CFSA(int f,int ext,int *dat,int *q)       long (24) single action 
 * CCCZ(int ext)			 crate initialize 
 * CCCC(int ext)			 crate clear 
 * CCCI(int ext,int l)			 set/reset crate inhibit 
 * CTCI(int ext,int *l)			 test crate inhibit 
 * CCCD(int ext,int l)			 ena/disable crate demand 
 * CTCD(int ext,int *l)			 test crate demand enabled
 * CCGL(int lam,int l)			 ena/disaable graded lam (NON STANDARD)
 * CTGL(int ext,int *l)			 test graded lam
 * CDLAM(int *lam,int b,int c,int n,int a,int inta[]) 	 define lam 
 * CCLC(int lam)			 clear lam (dataless function)
 * CCLM(int lam,int l)			 alternative lam clears 
 * CTLM(int lam,int *l)			 test lam 
 * CCRGL(int lam)			 re-enable graded lam (@end of isr)
 * CTSTAT(int *istat)			 test status of previous operation 
 */

#include "vxWorks.h"
#include "intLib.h"
#include "taskLib.h"

/******************** Register Definitions **********************************/

#define ks2917_amr  *(unsigned short *) 0xffffff60
#define ks2917_cmr  *(unsigned short *) 0xffffff62
#define ks2917_cmar *(unsigned short *) 0xffffff64
#define ks2917_cwc  *(unsigned short *) 0xffffff66
#define ks2917_srr  *(unsigned short *) 0xffffff68
#define ks2917_dlr  *(unsigned short *) 0xffffff6a
#define ks2917_dhr  *(unsigned short *) 0xffffff6c
#define ks2917_csr  *(unsigned short *) 0xffffff6e

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

#define ks2917_cser  *(unsigned short *) 0xffffff00
#define ks2917_docr  *(unsigned short *) 0xffffff04
#define ks2917_sccr  *(unsigned short *) 0xffffff06
#define ks2917_mtc   *(unsigned short *) 0xffffff0a
#define ks2917_machi *(unsigned short *) 0xffffff0c
#define ks2917_maclo *(unsigned short *) 0xffffff0e

#define ks2917_sccr_abrt 0x10

#define ks2917_cser_coc  0x8000
#define ks2917_cser_ndt  0x2000
#define ks2917_cser_err  0x1000
#define ks2917_cser_act  0x800
#define ks2917_cser_rdy  0x100
#define ks2917_cser_code 0x1f
#define ks2917_cser_berr 0x09
#define ks2917_cser_abrt 0x11


/************************* Interrupt control *********************************/

#define ks2917_icr_lam   *(unsigned short *) 0xffffff40
#define ks2917_icr_done  *(unsigned short *) 0xffffff42
#define ks2917_icr_dma   *(unsigned short *) 0xffffff44
#define ks2917_icr_abort *(unsigned short *) 0xffffff46

#define ks2917_ivr_lam   *(unsigned short *) 0xffffff48
#define ks2917_ivr_done  *(unsigned short *) 0xffffff4a
#define ks2917_ivr_dma   *(unsigned short *) 0xffffff4c
#define ks2917_ivr_abort *(unsigned short *) 0xffffff4e

#define KS2917_LAM_VEC 0xa0


/********************* Internal Macros ***************************************/

#define KS2917_WHEN_RDY(statement) \
{ int count; \
  if (!(ks2917_csr & ks2917_rdy)) { \
    for (count=0;(count<1000)&& \
         (!(ks2917_csr & (ks2917_rdy|ks2917_error|ks2917_timeo)));count++) ; \
    if (ks2917_csr & ks2917_rdy) {statement;} \
  } else \
    {statement;} \
}
#define ks2917_wait_done \
{ int count; \
  if (!(ks2917_csr & (ks2917_done|ks2917_error|ks2917_timeo))) \
    for (count=0;(count<1000)&& \
         (!(ks2917_csr & (ks2917_done|ks2917_error|ks2917_timeo)));count++) ; \
}
#define ks2917_wait_block \
{ int count,iwait; \
  if (!(ks2917_csr & (ks2917_done|ks2917_error|ks2917_timeo))) \
    for (count=0; \
         ( count<1000)&& \
           (!(ks2917_csr & (ks2917_done|ks2917_error|ks2917_timeo))); \
         count++) {\
      for (iwait=0;iwait<32;iwait++); }\
}
#define KS2917_READ16_NAF(ext,n,a,f,data) \
  ks2917_cmar = *((short *)&(ext)); \
  data = ks2917_cmr; /* fetch crate mask */ \
  ks2917_cmar = 0;   /* use 1st 3 words for operation */ \
  ks2917_cmr = data | ks2917_ws16; /* set for 16 bit read */\
  ks2917_cmr = (((n)<<9) | (a)<<5 | (f) ); \
  ks2917_cmr = ks2917_halt; \
  ks2917_cmar = 0; \
  ks2917_csr =  ks2917_go; \
  KS2917_WHEN_RDY(data = ks2917_dlr;) \
  ks2917_wait_done;
#define KS2917_WRITEBACK16_NAF(n,a,f,data) \
  ks2917_cmar = 1; \
  ks2917_cmr = (((n)<<9)|(a)<<5 |(f) );	\
  ks2917_cmar = 0; \
  ks2917_csr = ks2917_write + ks2917_go; \
  KS2917_WHEN_RDY(ks2917_dlr = data;) \
  ks2917_wait_done;
#define KS2917_READ24_NAF(ext,n,a,f,data) \
  ks2917_cmar = *((short *)&(ext)); \
  data = ks2917_cmr; /* fetch crate mask */ \
  ks2917_cmar = 0;   /* use 1st 3 words for operation */ \
  ks2917_cmr = data | ks2917_ws24; /* 24 bit read */\
  ks2917_cmr = (((n)<<9) | (a)<<5 | (f) ); \
  ks2917_cmr = ks2917_halt; \
  ks2917_cmar = 0; \
  ks2917_csr =  ks2917_go; \
  KS2917_WHEN_RDY(data = ks2917_dlr | (ks2917_dhr<<16);); \
  ks2917_wait_done;
#define KS2917_WRITEBACK24_NAF(n,a,f,data) \
  ks2917_cmar = 1; \
  ks2917_cmr = (((n)<<9)|(a)<<5 |(f) );	\
  ks2917_cmar = 0; \
  ks2917_csr = ks2917_write + ks2917_go; \
  KS2917_WHEN_RDY(ks2917_dlr = data; ks2917_dhr = data>>16;); \
  ks2917_wait_done;


/******************* Conditional Compilation Macros **************************/

int camacLockKey;
#ifdef CAMAC_BUILD_NOIPL	/* option used for interrupt code */
#define CAMAC_RAISE_IPL ;	/* NULL routines for performance */
#define CAMAC_LOWER_IPL ;
#else				/* guarantee only 1 caller at a time */
#define CAMAC_RAISE_IPL camacLockKey = intLock();
#define CAMAC_LOWER_IPL intUnlock(camacLockKey);
#endif

#ifndef CAMAC_BUILD_NOSTATUS	/* ifdef don't return q status on cssa,cfsa */
#define GET_KS2917_CSR(csr,stat) stat = *csr;
#define DECODE_KS2917_CSR(stat,dat,q) \
  if (stat & (ks2917_error|ks2917_timeo)) { \
    *(dat) = 0; \
    *(q) = 0; \
  } else \
    *(q) = stat&2 ? 0:1;
#else
#define GET_KS2917_CSR(a,b)	/* don't get csr */
#define DECODE_KS2917_CSR(stat,dat,q) /* don't return q */
#endif


/********************* Misc Definitions, Prototypes ***************************/

void cdreg_1(int *ext,int b,int c,int n,int a,int i,int match);

#define MAX_CDREG_PATTERN 200
extern short cdreg_pattern[MAX_CDREG_PATTERN];
#define KS2917_OFFSET_QSCAN 3	/* offset to q scan naf */
#define KS2917_OFFSET_QSTOP 8	/* offset to q stop naf */
#define KS2917_OFFSET_QREPEAT 13/* offset to q repeat naf */


/************************ Standard Routine Macros ****************************/

#define CDREG(ext,b,c,n,a) \
{ short match, temp; \
  int i; \
  match = ((c)<<9) | ((n)<<4) | ((a)<<0); \
  for (i=1; ;i++) { /* low addresses reserved for temp NAF's */ \
    if (cdreg_pattern[i]==match) { \
      temp = ((n)<<9) | ((a)<<5); \
      *ext = ((18*i) << 16) | temp; \
      break; \
    } \
    if (cdreg_pattern[i]==0) { \
      cdreg_1(ext,b,c,n,a,i,match); \
      break; \
    } \
  } \
}

#define CFSA(f,ext,dat,q) \
{ \
  unsigned short stat; \
  register unsigned short *source, *csr; \
  csr = &(ks2917_csr); /* preload pointer to csr for speed */\
  CAMAC_RAISE_IPL; \
  if (f>0) { \
    ks2917_cmar = *((short *)&(ext)) + 1; \
    ks2917_cmr = *(((short *)&(ext))+1) | (f); \
    ks2917_cmar = *((short *)&(ext)); \
    if (f>=8) { \
      if (((f)&8) == 0) {	/* write operation */ \
        *csr = ks2917_write + ks2917_go; \
	KS2917_WHEN_RDY( \
	  ks2917_dlr = *(dat); ks2917_dhr = *((short *)(dat)); \
        ); \
        ks2917_wait_done; \
      } else { 			/* control operation */ \
        *csr = ks2917_go; \
        ks2917_wait_done; \
      } \
    } else { 			/* read operation */ \
      *csr = ks2917_go; \
      KS2917_WHEN_RDY( \
	*(dat) = ks2917_dlr;	/* fetch LS 16 bits of data */ \
        *((short *)(dat)) = ks2917_dhr & 0xff; /* overwrite hi wrd w/MS 16 */\
      ); \
    } \
    ks2917_cmar = *((short *)&(ext)) + 1; \
    ks2917_cmr = *(((short *)&(ext))+1); /* clear out the f bits */ \
  } else { \
    ks2917_cmar = *((short *)&(ext)); /* f=0 read is fastest code */\
    *csr = ks2917_go;	/* f=0 read operation */ \
    source = &ks2917_dlr; \
    KS2917_WHEN_RDY( \
      *((short *)(dat)+1) = *(source++);  /* fetch LS 16 bits of data */ \
      *((short *)(dat)) = *(source) & 0xff; /* fetch MS 16 */ \
    ); \
  } \
  GET_KS2917_CSR(csr,stat); /* get csr */\
  CAMAC_LOWER_IPL; \
  DECODE_KS2917_CSR(stat,dat,q); /* get q from csr image */\
}

/* following same as CFSA, but only transfers 16 bits */

#define CSSA(f,ext,dat,q) \
{ \
  unsigned short stat, temp; \
  register unsigned short *source, *csr; \
  csr = &(ks2917_csr); /* preload pointer to csr for speed */\
  CAMAC_RAISE_IPL; \
  if (f>0) { \
    if (f>=8) { \
      if (((f)&8) == 0) {	/* write operation */ \
	ks2917_cmar = *((short *)&(ext)); \
        temp = ks2917_cmr;      /* fetch crate mask */ \
        ks2917_cmar = 0;        /* use 1st 3 words */ \
        ks2917_cmr = temp | ks2917_ws16; /* 16 bit write */ \
        ks2917_cmr = *(((short *)&(ext))+1) | (f); \
        ks2917_cmar = 0; \
        *csr = ks2917_write + ks2917_go; \
	KS2917_WHEN_RDY( \
	  ks2917_dlr = *(dat); \
        ); \
        ks2917_wait_done; \
      } else { 			/* control operation */ \
        ks2917_cmar = *((short *)&(ext)) + 1; \
        ks2917_cmr = *(((short *)&(ext))+1) | (f); \
        ks2917_cmar = *((short *)&(ext)); \
        *csr = ks2917_go; \
        ks2917_wait_done; \
        ks2917_cmar = *((short *)&(ext)) + 1; \
        ks2917_cmr = *(((short *)&(ext))+1); /* clear out the f bits */ \
      } \
    } else { 			/* read operation */ \
      *csr = ks2917_go; \
      KS2917_WHEN_RDY( \
	*(dat) = ks2917_dlr;	/* fetch LS 16 bits of data */ \
      ); \
      ks2917_cmar = *((short *)&(ext)) + 1; \
      ks2917_cmr = *(((short *)&(ext))+1); /* clear out the f bits */ \
    } \
  } else { \
    ks2917_cmar = *((short *)&(ext)); /* f=0 read is fastest code */\
    *csr = ks2917_go;	/* f=0 read operation */ \
    KS2917_WHEN_RDY( \
      *(dat) = ks2917_dlr;  /* fetch LS 16 bits of data */ \
    ); \
  } \
  GET_KS2917_CSR(csr,stat); /* get csr */\
  CAMAC_LOWER_IPL; \
  DECODE_KS2917_CSR(stat,dat,q); /* get q from csr image */\
}

#define CCCZ(ext) \
{ \
  short temp; \
  CAMAC_RAISE_IPL; \
  KS2917_READ16_NAF(ext,30,0,1,temp); \
  KS2917_WRITEBACK16_NAF(30,0,17,temp|1); \
  CAMAC_LOWER_IPL; \
}

#define CCCC(ext) \
{ \
  short temp; \
  CAMAC_RAISE_IPL; \
  KS2917_READ16_NAF(ext,30,0,1,temp); \
  KS2917_WRITEBACK16_NAF(30,0,17,temp|2); \
  CAMAC_LOWER_IPL; \
}

#define CCCI(ext,l) \
{ \
  short temp; \
  CAMAC_RAISE_IPL; \
  KS2917_READ16_NAF(ext,30,0,1,temp); \
  temp = l ? temp | 4 : temp & ~4; \
  KS2917_WRITEBACK16_NAF(30,0,17,temp); \
  CAMAC_LOWER_IPL; \
}

#define CTCI(ext,l) \
{ short temp; \
  CAMAC_RAISE_IPL; \
  KS2917_READ16_NAF(ext,30,0,1,temp); \
  CAMAC_LOWER_IPL; \
  *(l) = (temp>>2) & 1; \
}

#define CCCD(ext,l) \
{ short temp; \
  CAMAC_RAISE_IPL; \
  KS2917_READ16_NAF(ext,30,0,1,temp); \
  temp = l ? temp | 0x100 : temp & ~0x100; \
  KS2917_WRITEBACK16_NAF(30,0,17,temp); \
  CAMAC_LOWER_IPL; \
}

#define CTCD(ext,l) \
{ short temp; \
  CAMAC_RAISE_IPL; \
  KS2917_READ16_NAF(ext,30,0,1,temp); \
  CAMAC_LOWER_IPL; \
  if (temp & 0x100) \
    *(l) = 1; \
  else \
    *(l) = 0; \
}

#define CCGL(lam,l) \
{ int temp; \
  CAMAC_RAISE_IPL; \
  KS2917_READ24_NAF(lam,30,13,1,temp); /* read crate controller lam mask*/ \
  if (l) { \
    temp = temp | (1<<((((lam)>>9)-1) & 31)); \
  } else { \
    temp = temp & ~(1<<((((lam)>>9)-1) & 31)); \
  } \
  KS2917_WRITEBACK24_NAF(30,13,17,temp); /* write lam mask */ \
  CAMAC_LOWER_IPL; \
}
  
#define CTGL(ext,l) \
{ short temp; \
  CAMAC_RAISE_IPL; \
  KS2917_READ16_NAF(ext,30,0,1,temp); \
  CAMAC_LOWER_IPL; \
  *(l) = (temp>>15) & 1; \
}

#define CDLAM(ext,b,c,n,a,inta) \
{ cdreg(ext,b,c,n,a);  /* ignore inta[] */ }
     
#define CCLM(lam,l) \
{ int temp; \
  if (l) { \
    CFSA(26,(lam),&temp,&temp); \
  } else { \
    CFSA(24,(lam),&temp,&temp); \
  } \
}

#define CCLC(lam) \
{ int temp; \
  CFSA(10,(lam),&temp,&temp); \
}

#define CTLM(lam,l) \
{ int temp1,temp2; \
  CFSA(8,(lam),&temp1,&temp2); \
  /* *(l)=(ks2917_csr>>2)&1; */ *(l)=temp2;\
}

#define CCRGL(lam) \
{ ks2917_icr_lam = 0x1a; }

#define CTSTAT(istat) \
{ register unsigned short temp; temp = ks2917_csr; \
  *(istat) = ((temp>>1) & 3) | ((temp>>8) & 0xb0); \
} 

#define CLPOLL(lam) \
{ int ll; \
  for (ll=0;!ll;) ctlm(lam,&ll); \
}

/*************************** Block I/O Macros **********************************/

#define CFBLOCK(f,extb,intc,cb,offset) \
{ short list; \
  int i, *datap; \
  register unsigned short *csr; \
  CAMAC_RAISE_IPL; \
  csr = &(ks2917_csr); /* preload pointer to csr for speed */ \
  list = *((short *)&((extb)[0])) + offset; /* address of command list */ \
  ks2917_cmar = list + 1;       /* address of naf */ \
  ks2917_cmr = *(((short *)&((extb)[0]))+1) | (f); /* encoded naf */ \
  ks2917_cmr = -(cb)[0];	/* 2's complement transfer length */ \
  ks2917_cmar = list;		/* reset to start of list */ \
  ks2917_machi = ((unsigned int)intc >> 16) + 0x80; \
  ks2917_maclo = (unsigned int)intc; \
  ks2917_mtc = 2 * (cb)[0];	/* DMA transfer length (16 bit words) */ \
  ks2917_sccr = 0;		/* release soft abort */ \
  ks2917_cser = ks2917_cser_coc | ks2917_cser_ndt | ks2917_cser_err ; \
  if (f>=8) { \
    if (f&8) {			/* control operation */ \
      *csr = ks2917_go; \
      ks2917_wait_block; \
    } else {			/* write operation */ \
      ks2917_docr = 0x8010;	/* set DMA engine for writes */ \
      ks2917_sccr = 0x80;	/* start DMA engine */ \
      *csr = ks2917_write + ks2917_go + ks2917_dma; \
      ks2917_wait_block; \
    } \
  } else {			/* read operation */ \
    ks2917_docr = 0x8090;	/* set DMA engine for reads */ \
    ks2917_sccr = 0x80;		/* start DMA engine */ \
    *csr = ks2917_go + ks2917_dma; \
    ks2917_wait_block; \
  } \
  ks2917_sccr = ks2917_sccr_abrt; /* abort any left over DMA */ \
  cb[1] = (unsigned short)((cb)[0] + ks2917_cwc); \
  datap = intc;			/* clear high bytes */ \
  for (i=0;i<cb[1];i++) *(char *)(datap++) = 0; \
  CAMAC_LOWER_IPL; \
}

#define CSBLOCK(f,extb,intc,cb,offset) \
{ short list, temp, *datap; \
  int i; \
  register unsigned short *csr; \
  CAMAC_RAISE_IPL; \
  csr = &(ks2917_csr); /* preload pointer to csr for speed */ \
  list = *((short *)&((extb)[0])) + offset; /* address of command list */ \
  ks2917_cmar = list; \
  temp = ks2917_cmr;            /* get 24 bit command */ \
  ks2917_cmar = list; \
  ks2917_cmr = temp | ks2917_ws16; /* set to 16 bit mode */ \
  ks2917_cmr = *(((short *)&((extb)[0]))+1) | (f); /* encoded naf */ \
  ks2917_cmr = -(cb)[0];	/* 2's complement transfer length */ \
  ks2917_cmar = list;		/* reset to start of list */ \
  datap = (short *)intc;	/* load DMA transfer address */ \
  ks2917_machi = ((unsigned int)datap >> 16) + 0x80; \
  ks2917_maclo = (unsigned int)datap; \
  ks2917_mtc = (cb)[0];		/* DMA transfer length (16 bit words) */ \
  ks2917_sccr = 0;		/* release soft abort */ \
  ks2917_cser = ks2917_cser_coc | ks2917_cser_ndt | ks2917_cser_err ; \
  if (f>=8) { \
    if (f&8) {			/* control operation */ \
      *csr = ks2917_go; \
      ks2917_wait_block; \
    } else {			/* write operation */ \
      ks2917_docr = 0x8010;	/* set DMA engine for writes */ \
      ks2917_sccr = 0x80;	/* start DMA engine */ \
      *csr = ks2917_write + ks2917_go + ks2917_dma; \
      ks2917_wait_block; \
    } \
  } else {			/* read operation */ \
    ks2917_docr = 0x8090;	/* set DMA engine for reads */ \
    ks2917_sccr = 0x80;		/* start DMA engine */ \
    *csr = ks2917_go + ks2917_dma; \
    ks2917_wait_block; \
  } \
  ks2917_sccr = ks2917_sccr_abrt; /* abort any left over DMA */ \
  ks2917_cmar = list; \
  ks2917_cmr = temp;		/* set back to 24 bit mode */ \
  cb[1] = (unsigned short)((cb)[0] + ks2917_cwc); \
  CAMAC_LOWER_IPL; \
}
