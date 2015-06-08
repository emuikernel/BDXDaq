h59125
s 00031/00007/00184
d D 1.2 05/02/25 15:23:35 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/frconv.c
e
s 00191/00000/00000
d D 1.1 00/08/10 11:10:13 boiarino 1 0
c date and time created 00/08/10 11:10:13 by boiarino
e
u
U
f e 0
t
T
I 1
/*
D 3
     Conversion of complete record to format IFM (0, 1, 2, 3, 4 ...)
E 3
I 3
  frconv.c - conversion of complete record to format IFM (0, 1, 2, 3, 4 ...)
E 3
     (0 = local format, 1 2 3 4 are IEEE IBM VAX DECS formats)
     EXCEPT the 10 words of record key

     Convert all headers to local format, otherwise cannot be used !

 Serguei - let's convert the 10 words of record key also
I 3

   !!! called on 'read' only !!!
E 3
*/

#include <stdio.h>
I 3
#include <string.h>
#include <stdlib.h>
E 3

I 3
#undef DEBUG
E 3

I 3

E 3
/*#include "bosio.h"*/
/* --------------- Indexes in record segment header -------------------------- */
#define irPTRN  0  /* Pattern to recognize byte swapping                       */
#define irFORG  1  /* (format code: 1-IEEE,2-IBM,3-VAX,4-DEC)*16 + origin code */
#define irNRSG  2  /* #logrec*100 + #segment                                   */
#define irNAME1 3  /* event name - part 1   (Hollerith)                        */
#define irNAME2 4  /* event name - part 2   (Hollerith)                        */
#define irNRUN  5  /* run number                                               */
#define irNEVNT 6  /* event number                                             */
#define irNPHYS 7  /* # of several phys. records in one logical                */
#define irTRIG  8  /* Trig pattern                                             */
#define irCODE  9  /* segment code: 0-complete, 1-first, 2-middle, 3-last      */
#define irNWRD  10 /* the number of words in this record segment               */
#define RECHEADLEN 11 /* Record Header length                                  */
/* --------------------------------------------------------------------------- */
/* ----------------- Indexes in data segment header --------------------- */
#define idNHEAD 0  /* Number of header words                              */
#define idNAME1 1  /* Data bank name part1 (Hollerith)                    */
#define idNAME2 2  /* Data bank name part2 (Hollerith)                    */
#define idNSGM  3  /* Number of data bank                                 */
#define idNCOL  4  /* Number of columns                                   */
#define idNROW  5  /* Number of rows                                      */
#define idCODE  6  /* Data segment code ( see irCODE for description)     */
#define idNPREV 7  /* Number of data words in previous segments           */
#define idDATA  8  /* Number of data words in that segment                */
#define idFRMT  9  /* Beginning of FORMAT (Hollerith)                     */
/* ---------------------------------------------------------------------- */





#define MOD(x1,x2)        ( (x1) - (int)((x1)/(x2)) * (x2) )

D 3
void frconv_(int *IREC, int *IFM, int *IER)
E 3
I 3
void
frconv_(int *IREC, int *IFM, int *IER)
E 3
{
  static int LOCAL,NFMTH,NFMTR,NFMTS,NFMTA,NFMTK,KFM,JFM,I,J,K;
  static int INEXT,IHF,NHF,NFMT,NSP,NSC,tmp1,tmp2;
  static int IFMTH[3],IFMTR[3],IFMTS[3],IFMTA[1],IFMT[200];
  /* Serguei - begin */
  static int IFMTK[3];
  /* Serguei - end */
  static int start = 1;

  if(start)
  {
D 3
#ifndef Linux
    LOCAL = 1;    /* IEEE format */
#else
E 3
I 3
#if defined(Linux) || defined(SunOS_i86pc)
E 3
    LOCAL = 4;    /* DEC format (little-endian IEEE) */
I 3
#else
    LOCAL = 1;    /* IEEE format */
E 3
#endif
    ftctoi_("(I)",     IFMTH,&NFMTH,3);
    ftctoi_("3I,2A,6I",IFMTR,&NFMTR,8);
    ftctoi_("I,2A,6I", IFMTS,&NFMTS,7);
    ftctoi_("A",       IFMTA,&NFMTA,1);
    /* Serguei - begin */
    ftctoi_("3I,2A,5I",IFMTK,&NFMTK,8);
    /* Serguei - end */
    start = 0;
  }

  /* reset error flag */
  *IER = 0;
  /* KFM is target word format */
  KFM = *IFM;
  if(KFM == 0) KFM = LOCAL;
  JFM = IREC[3] / 16;
  /* convert 2-WORD record header */
  tmp1 = 2; tmp2 = 0;
  fcnvr_(&JFM,&KFM,&IREC[0],&tmp1,&tmp2,IFMTH,&NFMTH);
  /* init pointer I to record segment header */
  I = 2;

  /* test end-of-record */
a10:
  if(I == IREC[1]) return;
D 3
  /*
E 3
I 3

#ifdef DEBUG
E 3
  printf("I=%4d IREC[0]=%4d IREC[1]=%4d IREC[I+10]=%4d name >%4.4s%4.4s< ievt=%d\n",
        I,IREC[0],IREC[1],IREC[I+10],&IREC[I+irNAME1],&IREC[I+irNAME2],IREC[I+irNEVNT]);
D 3
  */
E 3
I 3
#endif

E 3
  /* pointer to next segment in same record */
  INEXT = (I+11) + IREC[I+10];
  /* formal data tests */
  if(I > IREC[1])
  {
    printf("frconv: error 1\n");
    goto a90;
  }
  if(I+11 > IREC[1])
  {
    printf("frconv: error 2\n");
    goto a90;
  }
  /* get word format of record */
  JFM = IREC[I+1] / 16;
  /* skip to next segment for diagonal term (no conversion) */
  if(JFM == KFM) goto a30;
  /* word format conversion of segment header from JFM to KFM */
  /* first 10 words not converted, only 11th */
  tmp1 = 1; tmp2 = 0;
  fcnvr_(&JFM,&KFM,&IREC[I+10],&tmp1,&tmp2,IFMTH,&NFMTH);
  /* change internal word format flag */
  IREC[I+1] = 16*KFM + MOD(IREC[I+1],16);
  /* Serguei - begin */
  /* convert first 10 words of the record header */
  tmp1 = 10; tmp2 = 0;
  fcnvr_(&JFM,&KFM,&IREC[I],&tmp1,&tmp2,IFMTK,&NFMTK);
  /* Serguei - end */
  /* pointer J to first data segment header */
  J = I + 11;

  /* next data segment header */
a20:

  if(J == INEXT) goto a30;

  /* check data segment pointer for formal errors */
  if(IREC[J] < 10 || J+IREC[J] > IREC[1])
  {
    printf("frconv: error 3: %d < 10   or   %d > %d\n",IREC[J],J+IREC[J],IREC[1]);
    goto a90;
  }
  /* convert data segment header at J */
  tmp1 = 9; tmp2 = 0;
  fcnvr_(&JFM,&KFM,&IREC[J],&tmp1,&tmp2,IFMTS,&NFMTS);
  /* indices for format code */
  IHF = J + 10;
  NHF = IREC[J]-9;
  if(JFM == LOCAL)
  {
I 3
#ifdef DEBUG
    printf("format1\n");
#endif
E 3
    /* convert format in local repr. into integers */
    fthtoi_(&IREC[IHF-1],&NHF,          IFMT,&NFMT);
    /* convert format code to target repr */
    tmp1 = 0;
    fcnvr_(&JFM,&KFM,&IREC[IHF-1],&NHF,&tmp1,IFMTA,&NFMTA);
  }
  else if(KFM == LOCAL)
  {
    /* convert format code to target repr */
    tmp1 = 0;
I 3
#ifdef DEBUG
    printf("format21 >%4.4s<\n",&IREC[IHF-1]);
#endif
E 3
    fcnvr_(&JFM,&KFM,&IREC[IHF-1],&NHF,&tmp1,IFMTA,&NFMTA);
    /* convert format in local repr. into integers */
I 3
#ifdef DEBUG
    printf("format22 >%4.4s<\n",&IREC[IHF-1]);
#endif
E 3
    fthtoi_(&IREC[IHF-1],&NHF,          IFMT,&NFMT);
I 3
#ifdef DEBUG
    printf("format23 >%4.4s<\n",&IREC[IHF-1]);
#endif
E 3
  }
  else
  {
I 3
#ifdef DEBUG
    printf("format3\n");
#endif
E 3
    /* convert format code to local repr */
    tmp1 = 0;
    fcnvr_(&JFM,&LOCAL,&IREC[IHF-1],&NHF,&tmp1,IFMTA,&NFMTA);
    /* convert format in local repr. into integers */
    fthtoi_(&IREC[IHF-1],&NHF,          IFMT,&NFMT);
    /* convert format code to target repr */
    tmp1 = 0;
    fcnvr_(&LOCAL,&KFM,&IREC[IHF-1],&NHF,&tmp1,IFMTA,&NFMTA);
  }
  /* number of words in previous/current subsegments */
  NSP = IREC[J+7];
  NSC = IREC[J+8];
  /* pointer K to data */
  K = J + IREC[J];
  if(K == INEXT) goto a30;
  /* check data pointer */
  fcnvr_(&JFM,&KFM,&IREC[K],&NSC,&NSP,IFMT,&NFMT);
  J = J + IREC[J] + IREC[J+8];
  goto a20;

  /* next record segment */
a30:
  I=INEXT;
  goto a10;

a90:
  printf("frconv: Bad FPACK-record: wrong internal pointers I=%d J=%d K=%d\n",I,J,K);
  *IER=1;

  return;
}

E 1
