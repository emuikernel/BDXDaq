
/* etbosio.h */
 
#ifndef _ETBOSIO_

typedef struct Fpackhead *FpackheadPtr;
typedef struct Fpackhead
{
  unsigned int  nhead;  /* the number of long words in a header */
  unsigned int  name;   /* bank name NAME */
  unsigned int  namext; /* name extension NAMEXT */
  unsigned int  nr;     /* bank number NR */
  unsigned int  ncol;   /* the number of columns NCOL */
  unsigned int  nrow;   /* the number of rows NROW */
  unsigned int  code;   /* data segment code */
  unsigned int  nprev;  /* the number of data long words in previous bank */
  unsigned int  ndata;  /* the number of data long words in this bank NDATA */
  unsigned int  frmt;   /* format definition */
} Fpack;


#define STRLEN 256
#define NDDL   200

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
  int          afterNamind;

} DDL;

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


void etDDLInit(int nddl, DDL *ddl);

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

int  etnamind_(int *jw, char *name, int lenname);
int  etNamind(int *jw, char *name);

int  etnnext_(int *jw, int *ind);
int  etNnext(int *jw, int ind);

int  etnnum_(int *jw, int *ind);
int  etNnum(int *jw, int ind);

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

int  etLget(int *jw, char *list, int *indlist);

#ifdef	__cplusplus
}
#endif



#define _ETBOSIO_
#endif
