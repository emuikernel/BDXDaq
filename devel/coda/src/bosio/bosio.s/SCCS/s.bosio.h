h59566
s 00009/00003/00206
d D 1.3 05/02/25 15:22:29 boiarino 4 3
c *** empty log message ***
e
s 00011/00008/00198
d D 1.2 03/04/17 16:44:54 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/bosio.h
e
s 00206/00000/00000
d D 1.1 00/08/10 11:09:56 boiarino 1 0
c date and time created 00/08/10 11:09:56 by boiarino
e
u
U
f e 0
t
T
I 1
 
#ifndef _BOSIO_
/* bosio.h */

D 4
#ifndef Linux
#define	LOCAL	1
E 4
I 4


#if defined(Linux) || defined(SunOS_i86pc)
/* DEC format (little-endian IEEE) */
#define LOCAL   4
E 4
#else
D 4
#define	LOCAL	4
E 4
I 4
/* IEEE format */
#define LOCAL   1
E 4
#endif

I 4


E 4
#define	SWAP	1+256*(2+256*(3+256*4))
#define NSERV	10
#ifndef NUNITS
#define NUNITS 40
#endif

#define MAXFNLIST 256
 
typedef char *chptr;
 
/* define BOSIO structure */

typedef struct {
  long nFN;
  long curFN;
  char *FN[MAXFNLIST+1];
} bosioFNList;
 
typedef struct bosfilestruct *BOSIOptr;
typedef struct bosfilestruct {
  int	unit;	     /* file unit */
  int	recl;	     /* record length */
  int	action;	     /* 1-READ 2-WRITE 3-APPEND 4-UPDATE */
  int	status;	     /* 1-OLD 2-NEW */
  int	access;	     /* 1-SEQ 2-DIR */
  int	format;	     /* 0-LOCAL 1-IEEE 2-IBM 3-VAX 4-DEC */
  int	medium;	     /* 0-disk 1-unixtape 2-system level disk IO */
  int	nrec;	     /* number of records - primary space */
  int	stream;	     /* file descriptor or STREAM id for remote */
  int	client;	     /* 0 for local file or RPC CLIENT id for remote */
  int	recnum;	     /* Physical record number after the last SUCCESSFUL IO */
  int	innum;	     /* Physical record number after last read  operation */
  int	outnum;	     /* Physical record number after last write operation */
  int   nphysrec;    /* Physical record number under processing - just for statistic */
  int   nlogrec;     /* Logical  record number under processing - just for statistic */
  int   ndatrec;     /* Data  record number under processing - just for statistic */
  int   nsegm;       /* Logical segment number inside Physical record */
  int	cursta;	     /* 0-undef 1-reading 2-eofread 3-writing 4-eofwrite 5-rewind */
  int	convert;     /* 0-convert 1-inhibit conversion on output */
  int	recfmtw;     /*  */
  int	recfmtr;     /*  */
  int	iobptr;	     /* file pointer for remote file */
  chptr finame;	     /* file name */
  chptr honame;	     /* host name */
  chptr flag;	     /* Open flags */
  int   splitmb;     /*  */
  int   splitev;     /*  */
  int   reopen;      /* reopen flag */
  int  *bufp;        /* pointer to current IO buffer */ 
  int  *bufp1;       /* pointer to first IO buffer */ 
  int  *bufp2;       /* pointer to second IO buffer */ 
  int   ip;          /* Ukazatel` na record segment header v I/O bufere */  
  int   jp;          /* Ukazatel` na data header v I/O bufere */
  int   kp;          /* Ukazatel` na data v I/O bufere */
  int   rechead[11]; /* current record header */

  bosioFNList  flist; /* Structure containing parsed filelist to read */

  unsigned char recname[8]; /* record name */
  unsigned int runnum;      /* run number */
  unsigned int evntnum;     /* event number */
  unsigned int trig;        /* trigger pattern */
} BOSIO;

/* FPACK Options */
#define BIOS_UNIT     BIOstream->unit     /* file unit                                    */
#define BIOS_RECL     BIOstream->recl     /* record length                                */
#define BIOS_FINAME   BIOstream->finame   /* file name                                    */
#define BIOS_HONAME   BIOstream->honame   /* host name                                    */
#define BIOS_ACTION   BIOstream->action   /* 1-READ 2-WRITE 3-APPEND 4-UPDATE             */
#define BIOS_STATUS   BIOstream->status   /* 1-OLD 2-NEW                                  */
#define BIOS_ACCESS   BIOstream->access   /* 1-SEQ 2-DIR                                  */
#define BIOS_FORMAT   BIOstream->format   /* 0-LOCAL 1-IEEE 2-IBM 3-VAX 4-DEC             */
#define BIOS_SPLITMB  BIOstream->splitmb  /*                                              */
#define BIOS_SPLITEV  BIOstream->splitev  /*                                              */
#define BIOS_MEDIUM   BIOstream->medium   /* 0-disk 1-unixtape 2-system level disk IO     */
#define BIOS_CONVERT  BIOstream->convert  /* 0-convert 1-inhibit conversion on output     */
#define BIOS_NREC     BIOstream->nrec     /* number of records - primary space            */   
#define BIOS_FLAG     BIOstream->flag     /* Save ov open flags                           */   
/* Internal control words */                     
#define BIOS_STREAM   BIOstream->stream   /* file descriptor or STREAM id for remote      */
#define BIOS_CLIENT   BIOstream->client   /* 0 for local file or RPC CLIENT id for remote */
#define BIOS_RECNUM   BIOstream->recnum   /* Physical rec. num. after the last SUCCESSFUL IO */
#define BIOS_INNUM    BIOstream->innum    /* Physical rec. num. after last read operation */
#define BIOS_OUTNUM   BIOstream->outnum   /* Physical rec. num. after last write operation*/
#define BIOS_NPHYSREC BIOstream->nphysrec /* Physical rec. num. Under processing          */
#define BIOS_NLOGREC  BIOstream->nlogrec  /* Logical  rec. num. Under processing          */
#define BIOS_NDATREC  BIOstream->ndatrec  /* Data  rec. num. Under processing             */
#define BIOS_NSEGM    BIOstream->nsegm    /* Logical segment number inside Physical record*/
#define BIOS_CURSTA   BIOstream->cursta   /* 0-undef 1-reading 2-eofread 3-writing 4-eofwrite 5-rewind */
#define BIOS_BUFP     BIOstream->bufp     /* Pointer to current I/O buffer                */
#define BIOS_BUFP1    BIOstream->bufp1    /* Pointer to first I/O buffer                  */
#define BIOS_BUFP2    BIOstream->bufp2    /* Pointer to second I/O buffer                 */
#define      IP       BIOstream->ip       /* Ukazatel` na record segment header v I/O bufere */
#define      JP       BIOstream->jp       /* Ukazatel` na data header v I/O bufere        */
#define      KP       BIOstream->kp       /* Ukazatel` na data v I/O bufere               */
#define BIOS_RECHEAD  BIOstream->rechead  /* pointer to current record header             */
#define BIOS_RECFMTW  BIOstream->recfmtw  /*                                              */
#define BIOS_RECFMTR  BIOstream->recfmtr  /*                                              */
#define BIOS_REOPEN   BIOstream->reopen   /*                                              */

/* INDEX definition */

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


/* ERRORS definition */
#define EBIO_BASEERRNUM     -999
#define EBIO_INTERNAL       EBIO_BASEERRNUM+0  /* BOSIO Internal error              */
#define EBIO_CORREVENT      EBIO_BASEERRNUM+1  /* Corrupted event                   */
#define EBIO_SKIPTRAILGARB  EBIO_BASEERRNUM+2  /* Trailing garbage skipped          */
#define EBIO_EOFREAD                      -22  /* Trying to read past end of file   */
#define EBIO_WRONGSTATUS                  -33  /* Wrong file status != 1 (not read) */
#define EBIO_EOF                          -1   /* End of file encountered           */
#define EBIO_WRONGSEGMCODE  EBIO_BASEERRNUM+3  /* Wrong segment code <0 or >3       */
#define EBIO_NOTCONTCODE    EBIO_BASEERRNUM+4  /* Segment continuation expected     */
#define EBIO_BANKHEADINTEGR EBIO_BASEERRNUM+5  /* Bank header fails integrity check */
#define EBIO_NODISKSPACE    EBIO_BASEERRNUM+6  /*  */
#define EBIO_WRITEERROR     EBIO_BASEERRNUM+7  /*  */
#define EBIO_PERMDENIED     EBIO_BASEERRNUM+8  /*  */
#define EBIO_FILEEXISTS     EBIO_BASEERRNUM+9  /*  */
#define EBIO_FILENOTEXISTS  EBIO_BASEERRNUM+10 /*  */
#define EBIO_READERROR      EBIO_BASEERRNUM+11 /*  */
#define EBIO_UNKNOWNOPTION  EBIO_BASEERRNUM+12 /*  */
#define EBIO_WRONGPARAM     EBIO_BASEERRNUM+13 /*  */
#define EBIO_IOERROR        EBIO_BASEERRNUM+14 /* I/O error reported by system      */
#define EBIO_NOFREEUNITS    EBIO_BASEERRNUM+15 /* No more available units           */
#define EBIO_CONNOTRECOV    EBIO_BASEERRNUM+16 /* Failed to recover connection      */
#define EBIO_NOCONNECTION   EBIO_BASEERRNUM+17 /* No connection to server           */
#define EBIO_DEADCHANNEL    EBIO_BASEERRNUM+18 /* Dead network channel              */
#define EBIO_NEWFILEOPEN    EBIO_BASEERRNUM+19 /* New file open due to SPLITMB limit reached */
#define EBIO_SEEKERROR      EBIO_BASEERRNUM+20 /* Error seeking record in file      */
#define EBIO_UNITINUSE      EBIO_BASEERRNUM+21 /* This unit already in use          */
#define EBIO_NOETSPACE      EBIO_BASEERRNUM+22 /* Size of ET buffer not enough      */
#define EBIO_NOBOSSPACE     EBIO_BASEERRNUM+23 /* Size of BOS array not enough      */

#define BOSIOERR(num) ((num>=EBIO_BASEERRNUM&&num<=EBIO_BASEERRNUM+21)||num==-22||num==-33||num==-1)

#define NEXTIP(ip) ip+bufin[ip+irNWRD]+RECHEADLEN
/* Ukazatel` na nachalo sled. segmenta Log. zapisi
   ili sleduyuschey zapisi v I/O buffere */

#define NEXTJP(jp) jp+bufin[jp+idNHEAD]+bufin[jp+idDATA] 
/* Ukazatel` na nachalo sled. segmenta dannyh v I/O bufere */

#define nextjp(jp) jp+ev[jp+idNHEAD]+ev[jp+idDATA] 
/* Ukazatel` na nachalo sled. segmenta dannyh v ET bufere */

/* define a Ncommon data type */
 
typedef struct ncommon *Nptr;
D 3
typedef struct ncommon {
	chptr	honame;	/* name of the remote host */
	int	client;	/* RPC CLIENT id for the host */
	int	nfiles;	/* the number of files opened on the host */
	} NET;
E 3
I 3
typedef struct ncommon
{
  chptr	honame;	/* name of the remote host */
  int	client;	/* RPC CLIENT id for the host */
  int	nfiles;	/* the number of files opened on the host */
} NET;
E 3
 

I 3
/*
E 3
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
D 3

E 3
I 3
*/
E 3
 
D 3
/* backward compatibility */
 
E 3
I 3
/* prototypes */
E 3
#include "biofun.h" 
I 3

/* backward compatibility */
E 3
#include "bosfor.h"
 
#define _BOSIO_
#endif
E 1
