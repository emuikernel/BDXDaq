h23526
s 00001/00001/00170
d D 1.4 08/06/08 23:36:59 boiarino 5 4
c NET -> BOSIONET
c 
e
s 00002/00001/00169
d D 1.3 05/02/25 15:22:20 boiarino 4 3
c *** empty log message ***
e
s 00049/00031/00121
d D 1.2 03/04/17 16:44:39 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/biofun.h
e
s 00152/00000/00000
d D 1.1 00/08/10 11:09:55 boiarino 1 0
c date and time created 00/08/10 11:09:55 by boiarino
e
u
U
f e 0
t
T
I 1
#ifndef _BIOFUN_

#ifdef	__cplusplus
extern "C" {
#endif

D 3
#define	getBOSIO()	(BOSIOptr) MALLOC(sizeof(BOSIO))
E 3
I 3
/*#define	getBOSIO()	(BOSIOptr) MALLOC(sizeof(BOSIO))*/
E 3
D 5
#define	getNET()	(Nptr) MALLOC(sizeof(NET))
E 5
I 5
#define	getNET()	(Nptr) MALLOC(sizeof(BOSIONET))
E 5

I 3
/* bosmem.c */
void *MALLOC(size_t);
void FREE(void *);

  /* et interface */
int etRead(BOSIO *descriptor, int *ev, int et_size, int *size, int control[4]);
int etWrite(BOSIO *descriptor, int *ev);
int etFlush(BOSIO *descriptor);


E 3
/* ../src/bosopen.c: */
D 3
int bosopen_(char *file, char *flag, int *descriptor, int filen, int fllen);
int bosOpen(char *file, char *flag, int *descriptor);
BOSIOptr  getDEFBOSIO();
BOSIOptr fillDEFBOSIO(BOSIOptr lun); 
E 3
I 3
int bosopen_(char *file, char *flag, BOSIO **descriptor, int filen, int fllen);
int bosOpen(char *file, char *flag, BOSIO **descriptor);

int bosclose_(BOSIO *descriptor);
int bosClose(BOSIO *descriptor);

BOSIOptr fillDEFBOSIO(BOSIO *lun); 
BOSIOptr getDEFBOSIO();
char *cleanFN(char *file);
int parseListFN(char *FList, bosioFNList *flist);
char *getNextFN(BOSIOptr stream);
E 3
char *getSplitFN(char *file);
D 3
int bosOPEN(char *file, char *flag, int *descriptor);
#define BOSREOPEN(d) bosOPEN( ((BOSIOptr)d)->finame, ((BOSIOptr)d)->flag, &d)
int fpopen(int descriptor, char *action);
int bosnres(int descriptor, int client, int stream, int mode);
int bosclose_(int *descriptor);
int bosClose(int descriptor);
int bosIoctl(int descriptor, char *option, void *arg);
char *bositoa(int number);      /* integer -> char string */
E 3
I 3
int bosOPEN(char *file, char *flag, BOSIO *descriptor);
int fpopen(BOSIO *descriptor, char *action);
int bosnres(BOSIO *descriptor, int client, int stream, int mode);

int bosIoctl(BOSIO *descriptor, char *option, void *arg);

char *bositoa(int number);
#define BOSREOPEN(d) bosOPEN( d->finame, d->flag, d)
E 3
#define bosGETFNAME(d) ((BOSIOptr)d)->finame

/* ../src/bosread.c: */
D 3
int bosread_(int *descriptor, int *jw, char *list, int lilen);
int bosRead(int descriptor, int *jw, char *list);
int bosins(int descriptor, int *jw, char *list);
E 3
I 3
int bosread_(BOSIO *descriptor, int *jw, char *list, int lilen);
int bosRead(BOSIO *descriptor, int *jw, char *list);
E 3

I 3
int bosins(int unit, int *jw, char *list);

E 3
/* ../src/ddgetlogrec.c: */
D 3
int ddGetLogRec(int descriptor, int **bufp, int *evlen, int *freeflag);
E 3
I 3
int bosinn(BOSIO *descriptor, int *ntot, int *buf);
int ddGetLogRec(BOSIO *descriptor, int **bufp, int *evlen, int *freeflag);
E 3

/* ../src/bosin.c: */
D 3
int bosin(int descriptor, int *ntot);
E 3
I 3
int bosin(BOSIO *descriptor, int *ntot);
E 3

/* ../src/bosrewin.c: */
D 3
int bosrewind_(int *descriptor);
int bosRewind(int descriptor);
E 3
I 3
int bosrewind_(BOSIO *descriptor);
int bosRewind(BOSIO *descriptor);
E 3

/* ../src/boswrite.c: */
D 3
int boswrite_(int *descriptor, int *jw, char *list, int lilen);
int bosWrite(int descriptor, int *jw, char *list);
E 3
I 3
int boswrite_(BOSIO *descriptor, int *jw, char *list, int lilen);
int bosWrite(BOSIO *descriptor, int *jw, char *list);
E 3

I 3
/* bosnform.c: */
void bosnformat_(int *jw, char *name, char *fmt, int lename, int lenfmt);
int  bosNformat(int *jw, void *name, char *fmt);

E 3
/* ../src/bosout.c: */
D 3
int bosout(int descriptor, int *buf);
E 3
I 3
int bosout(BOSIO *descriptor, int *buf);
E 3

/* ../src/ddwalk.c: */
int prphrec  (int *p);
int prrechead(int *p);
int prdathead(int *p);
int prdatbody(int *p);
int dumplogrec(int *p, int level);
int ddCheckRec(int *p);
int ddCheckBlock(int * p);

D 3
/* ../src/ddwrite.c: */
int ddWrite(int descriptor, int, int *);
int ddFlush(int descriptor);


E 3
/* ../src/cget.c: */
int cget(int *lunptr, int *ircn, int *medium, int *ntot, int *buffer);

/* ../src/copen.c: */
int copen(int *stream, char *filename, char *options, int medium, int blksize,
          int *bufptr, int reopen);

/* ../src/cput.c: */
int cput(int stream, int ircn, int ntot, int *buffer, int medium);

/* ../src/cclose.c: */
void cclose(int lunptr, int bufptr, int medium);

/* ../src/closec.c: */
#if defined(IBMMVS) || defined(IBMVM)
D 3
void closec(int *iclptr,int *streamptr,int **retptrp);
E 3
I 3
void closec(int *iclptr, int *streamptr, int **retptrp);
E 3
#else
D 3
void closec(int *iclptr,int *streamptr,int *retptr);
E 3
I 3
void closec(int *iclptr, int *streamptr, int *retptr);
E 3
#endif

/* ../src/crewnd.c: */
void crewnd(int lunptr, int medium);

/* ../src/fparm.c: */
int  fparm_(char *string, int len);
D 3
int  FParm (char *string, int *dsc);
E 3
I 3
int  FParm (char *string, BOSIO **descriptor);
E 3
void frbos_(int *jw, int *unit, char *list, int *err, int len); 
void fwbos_(int *jw, int *unit, char *list, int *err, int len); 
void fpstat_();
void fseqr_(char *daname, int *err, int len); 
void fseqw_(char *daname, int len);

D 4
void fswap_(int *, int *);
E 4
I 4
void fswap_(unsigned int *, int *);
void etfswap(unsigned int *rec);
E 4

/* ../src/openc.c: */
#if defined(IBMMVS) || defined(IBMVM)
void openc(int **iclptrp,int **strptrp,char *name,char *serv_host,
		   char *options,int *ispc,int *nr1,int *nr2);
#else
void openc(int *iclptr,int *strptr,char *name,char *serv_host,
		   char *options,int *ispc,int *nr1,int *nr2);
#endif

/* ../src/readc.c: */
#if defined(IBMMVS) || defined(IBMVM)
void readc(int *iclptr,int *streamptr,int *skipptr,int *sizeptr,
		   int **bufferp,int **retptrp);
#else
void readc(int *iclptr,int *streamptr,int *skipptr,int *sizeptr,
		   int *buffer,int *retptr);
#endif

/* ../src/rewinc.c: */
#if defined(IBMMVS) || defined(IBMVM)
void rewinc(int *iclptr,int *streamptr,int **retptrp);
#else
void rewinc(int *iclptr,int *streamptr,int *retptr);
#endif

/* ../src/stopsc.c: */
#if defined(IBMMVS) || defined(IBMVM)
void stopsc(int *iclptr, int **retptrp);
#else
void stopsc(int *iclptr, int *retptr);
#endif

/* ../src/writec.c: */
#if defined(IBMMVS) || defined(IBMVM)
void writec(int *iclptr,int *streamptr,int *ibyteptr,int *sizeptr,
			int *buffer,int **retptrp);
#else
void writec(int *iclptr,int *streamptr,int *ibyteptr,int *sizeptr,
			int *buffer,int *retptr);
#endif

#ifdef	__cplusplus
}
#endif

#define _BIOFUN_
#endif










E 1
