h19359
s 00001/00001/00116
d D 1.6 05/02/25 15:23:12 boiarino 7 6
c *** empty log message ***
e
s 00003/00002/00114
d D 1.5 03/11/27 20:43:36 boiarino 6 5
c .
e
s 00001/00000/00115
d D 1.4 02/05/16 23:36:30 boiarino 5 4
c add bosLget
c 
e
s 00004/00000/00111
d D 1.3 01/11/19 15:55:46 boiarino 4 3
c add etNamind
c 
e
s 00004/00006/00107
d D 1.2 00/08/22 17:20:04 boiarino 3 1
c *** empty log message ***
e
s 00000/00000/00000
d R 1.2 70/01/01 00:00:02 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 clas/bosio/bosio.s/etbosio.h
e
s 00113/00000/00000
d D 1.1 00/08/10 11:09:56 boiarino 1 0
c date and time created 00/08/10 11:09:56 by boiarino
e
u
U
f e 0
t
T
I 1

/* etbosio.h */
 
#ifndef _ETBOSIO_

typedef struct Fpackhead *FpackheadPtr;
typedef struct Fpackhead
{
  unsigned long  nhead;  /* the number of long words in a header */
  unsigned long  name;   /* bank name NAME */
  unsigned long  namext; /* name extension NAMEXT */
  unsigned long  nr;     /* bank number NR */
  unsigned long  ncol;   /* the number of columns NCOL */
  unsigned long  nrow;   /* the number of rows NROW */
  unsigned long  code;   /* data segment code */
  unsigned long  nprev;  /* the number of data long words in previous bank */
  unsigned long  ndata;  /* the number of data long words in this bank NDATA */
  unsigned long  frmt;   /* format definition */
} Fpack;


#define STRLEN 256
I 6
D 7
#define NDDL   100
E 7
I 7
#define NDDL   200
E 7
E 6

typedef struct ddl
{
  int  nname;
  char name[9];
  int  nfmt;
  char fmt[STRLEN+1];
  int  lfmt;
  int  ncol;

  FpackheadPtr b_save;
  int          ind_save;
I 4
  int          afterNamind;
E 4

} DDL;

D 6
#define NDDL   100

E 6
typedef struct sys
{
  int nwords;
  int indsys;     /* IW[indsys] will be pointer to the system area */
  int nddl;
  DDL ddl[NDDL];
} ETSYS;


#ifdef	__cplusplus
extern "C" {
#endif


I 6
void etDDLInit(int nddl, DDL *ddl);

E 6
int  etSysalloc(ETSYS **sys);
int  etSysfree(ETSYS *sys);
int  etOpen(int *jw, int nbcs, ETSYS *sysptr);
int  etClose(int *jw);



void etinit_(int *jw, int *ndim);
int  etInit(int *jw, int nbcs);

void etnformat_(int *jw, char *name, char *fmt, int lename, int lenfmt);
int  etNformat(int *jw, char *name, char *fmt);

int  etndrop_(int *jw, char *name, int *nr, int lename);
int  etNdrop(int *jw, char *name, int nr);

int  etncreate_(int *jw, char *name, int *nr, int *ncol, int *nrow, int lename);
int  etNcreate(int *jw, char *name, int nr, int ncol, int nrow);

int  etnlink_(int *jw, char *name, int *nr, int lename);
int  etNlink(int *jw, char *name, int nr);

int  etnind_(int *jw, char *name, int lenname);
int  etNind(int *jw, char *name);

I 4
int  etnamind_(int *jw, char *name, int lenname);
int  etNamind(int *jw, char *name);

E 4
D 3
int  etnext_(int *jw, int *ind);
int  etNext(int *jw, int ind);
E 3
I 3
int  etnnext_(int *jw, int *ind);
int  etNnext(int *jw, int ind);
E 3

D 3
int  etnnr_(int *jw, int *ind);
int  etNnr(int *jw, int ind);
E 3
I 3
int  etnnum_(int *jw, int *ind);
int  etNnum(int *jw, int ind);
E 3

int  etncol_(int *jw, int *ind);
int  etNcol(int *jw, int ind);

int  etnrow_(int *jw, int *ind);
int  etNrow(int *jw, int ind);

int  etndata_(int *jw, int *ind);
int  etNdata(int *jw, int ind);

void etldrop_(int *jw, char *list, int lenlist);
void etLdrop(int *jw, char *list);

void etNgarb_(int *jw);
void etNgarb(int *jw);

void etlctl_(int *jw, char *opt, char *list, int lenopt, int lenlist);
int  etLctl(int *jw, char *opt, char *list);

I 5
int  etLget(int *jw, char *list, int *indlist);
E 5

D 3


E 3
#ifdef	__cplusplus
}
#endif



#define _ETBOSIO_
#endif
E 1
