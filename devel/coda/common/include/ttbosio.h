
#ifndef _TTBOSIO_
#define _TTBOSIO_

/* ttbosio.h */

#ifdef Linux
#define NWBOS 262144/*65536*/
#else
/*#define NWBOS (65536/4)*/ /*(MAX_EVENT_LENGTH/4)*/
#define NWBOS 262144
#endif

/* types */

#define STRLENGTH 257
#define FMTLENGTH 9
#define NTTDDL      200

typedef struct ttddl
{
  int  nname;
  char name[9];
  int  nfmt;
  char fmt[FMTLENGTH+1];
  int  lfmt;
  int  ncol;
} TTDDL;

#ifdef	__cplusplus
extern "C" {
#endif

/* function prototupes */

void bosinit(int *, int);
int  bosleftspace(int *);
int  bosNopen(int *, int, int, int, int);
int  bosNclose(int *, int, int, int);

void clonbanks();
int  bosMgetid(char *name);
int  bosMgetname(int id, char name[9]);
int  bosMgetncol(char *name);
int  bosMgetlfmt(char *name);
int  bosMmsg(int *jw, char *bankname, int banknum, char *message);
void bosMinit(int *jw);
int  bosMlink(int *jw, char *name, int nr);
int  bosMopen(int *jw, char *name, int nr, int ncol, int nrow);
int  bosMclose(int *jw, int ind, int ncol, int nrow);

void clonbanks_();
int  bosMgetid_(char *name);
int  bosMgetname_(int id, char name[9]);
int  bosMgetncol_(char *name);
int  bosMgetlfmt_(char *name);
int  bosMmsg_(int *jw, char *bankname, int banknum, char *message);
void bosMinit_(int *jw);
int  bosMlink_(int *jw, char *name, int nr);
int  bosMopen_(int *jw, char *name, int nr, int ncol, int nrow);
int  bosMclose_(int *jw, int ind, int ncol, int nrow);

#ifdef	__cplusplus
}
#endif

#endif

