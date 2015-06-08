#ifndef _BIOFUN_

#ifdef	__cplusplus
extern "C" {
#endif

/*#define	getBOSIO()	(BOSIOptr) MALLOC(sizeof(BOSIO))*/
#define	getNET()	(Nptr) MALLOC(sizeof(BOSIONET))

/* bosmem.c */
void *MALLOC(size_t);
void FREE(void *);

  /* et interface */
int etRead(BOSIO *descriptor, int *ev, int et_size, int *size, int control[4]);
int etWrite(BOSIO *descriptor, int *ev);
int etFlush(BOSIO *descriptor);


/* ../src/bosopen.c: */
int bosopen_(char *file, char *flag, BOSIO **descriptor, int filen, int fllen);
int bosOpen(char *file, char *flag, BOSIO **descriptor);

int bosclose_(BOSIO *descriptor);
int bosClose(BOSIO *descriptor);

BOSIOptr fillDEFBOSIO(BOSIO *lun); 
BOSIOptr getDEFBOSIO();
char *cleanFN(char *file);
int parseListFN(char *FList, bosioFNList *flist);
char *getNextFN(BOSIOptr stream);
char *getSplitFN(char *file);
int bosOPEN(char *file, char *flag, BOSIO *descriptor);
int fpopen(BOSIO *descriptor, char *action);
int bosnres(BOSIO *descriptor, int client, int stream, int mode);

int bosIoctl(BOSIO *descriptor, char *option, void *arg);

char *bositoa(int number);
#define BOSREOPEN(d) bosOPEN( d->finame, d->flag, d)
#define bosGETFNAME(d) ((BOSIOptr)d)->finame

/* ../src/bosread.c: */
int bosread_(BOSIO *descriptor, int *jw, char *list, int lilen);
int bosRead(BOSIO *descriptor, int *jw, char *list);

int bosins(int unit, int *jw, char *list);

/* ../src/ddgetlogrec.c: */
int bosinn(BOSIO *descriptor, int *ntot, int *buf);
int ddGetLogRec(BOSIO *descriptor, int **bufp, int *evlen, int *freeflag);

/* ../src/bosin.c: */
int bosin(BOSIO *descriptor, int *ntot);

/* ../src/bosrewin.c: */
int bosrewind_(BOSIO *descriptor);
int bosRewind(BOSIO *descriptor);

/* ../src/boswrite.c: */
int boswrite_(BOSIO *descriptor, int *jw, char *list, int lilen);
int bosWrite(BOSIO *descriptor, int *jw, char *list);

/* bosnform.c: */
void bosnformat_(int *jw, char *name, char *fmt, int lename, int lenfmt);
int  bosNformat(int *jw, void *name, char *fmt);

/* ../src/bosout.c: */
int bosout(BOSIO *descriptor, int *buf);

/* ../src/ddwalk.c: */
int prphrec  (int *p);
int prrechead(int *p);
int prdathead(int *p);
int prdatbody(int *p);
int dumplogrec(int *p, int level);
int ddCheckRec(int *p);
int ddCheckBlock(int * p);

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
void closec(int *iclptr, int *streamptr, int **retptrp);
#else
void closec(int *iclptr, int *streamptr, int *retptr);
#endif

/* ../src/crewnd.c: */
void crewnd(int lunptr, int medium);

/* ../src/fparm.c: */
int  fparm_(char *string, int len);
int  FParm (char *string, BOSIO **descriptor);
void frbos_(int *jw, int *unit, char *list, int *err, int len); 
void fwbos_(int *jw, int *unit, char *list, int *err, int len); 
void fpstat_();
void fseqr_(char *daname, int *err, int len); 
void fseqw_(char *daname, int len);

void fswap_(unsigned int *, int *);
void etfswap(unsigned int *rec);

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

/* some new functions */

int et2bos(int *ev, int *jw, char *list);


#ifdef	__cplusplus
}
#endif

#define _BIOFUN_
#endif










