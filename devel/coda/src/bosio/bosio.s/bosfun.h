#ifndef _BOSFUN_

#ifdef	__cplusplus
extern "C" {
#endif

/* ../src/bosinit.c: */
int bosInit(int *jw, int ndim);
void bnames(int namax);
void bnres();
int berr(char *who, int error);

/* ../src/boslctl.c: */
int bosLctl(int *jw, char *opt, char *list);

/* ../src/bosldrop.c: */
int bosLdrop(int *jw, char *list);

/* ../src/boslname.c: */
int boslname_(int *jw, int *n, char *list, int lenlist);
int bosLname(int *jw, int n, char *list);

/* ../src/bosncrea.c: */
int bosncreate_(int *jw, char *name, int *nr, int *ncol, int *nrow, int lename);
int bosNcreate(int *jw, char *name, int nr, int ncol, int nrow);

/* ../src/bosndrop.c: */
int bosndrop_(int *jw, char *name, int *nr, int lename);
int bosNdrop(int *jw, char *name, int nr);

/* ../src/bosnform.c: */
int bosNformat(int *jw, void *name, char *fmt);

/* ../src/bosngarb.c: */
int bosNgarbage(int *jw);

/* ../src/bosnlink.c: */
int bosnlink_(int *jw, char *name, int *nr, int lename);
int bosNlink(int *jw, char *name, int nr);

/* ../src/bosNamind.c: */
int bosnamind_(int *jw, char *name, int namlen);
int bosNamind(int *jw, char *name);
int mamind_(int *jw, char *name, int namlen);
int mamind(int *jw, int nama);

/* ../src/boswcrea.c: */
int wwbanc_(int *jw, int *id, int *ncol, int *nrow);
int wbanc(int *jw, int *id, int ncol, int nrow);
int boswcreate_(int *jw, int *id, int *ncol, int *nrow);
int bosWcreate(int *jw, int *id, int ncol, int nrow);

/* ../src/boswdrop.c: */
int boswdrop_(int *jw, int *id);
int bosWdrop(int *jw, int *id);

/* ../src/boswgarb.c: */
int boswgarbage_(int *jw);
int bosWgarbage(int *jw);

/* ../src/boswprin.c: */
int bosprint(int *jw, int id);

/* ../src/lcase.c: */
int lcase(char *charptr);

/* ../src/listn1.c: */
int listn1(int *jw, char *list, int *jls, int *nls);

/* ../src/listn2.c: */
int listn2(int *jw, char *list, int *ils, int *jls, int *nls);

/* ../src/namen.c: */
int namen(int *jw, int nama);

/* ../src/namez.c: */
int namez(int *jw, int nama);

/* ../src/nindex.c: */
int nindex(int *jw, int nr, int nami, int *indi, int *indj);

/* ../src/bosinit.c */
void bosinit_(int *jw, int *ndim);

/* ../src/boslctl.c: */
void boslctl_(int *jw, char *opt, char *list, int lenopt, int lenlist);

/* ../src/bosldrop.c: */
void bosldrop_(int *jw, char *list, int lenlist);

/* ../src/boslprin.c: */
void boslprint_(int *jw, char *list, int lenlist);

/* ../src/bosldump.c: */
int bosLdump(int *jw, char *list, int **outptr, int **fmtptr);
int bosLget(int *jw, char *list, int *outptr);

/* ../src/bosnform.c: */
void bosnformat_(int *jw, char *name, char *fmt, int lename, int lenfmt);

/* ../src/bosngarb.c: */
void bosngarbage_(int *jw);

/* ../src/bosnprin.c: */
void bosnprint_(int *jw, char *name, int *nr, int lenname);
void bosNprint(int *jw, char *name, int nr);

/* ../src/boswprin.c: */
void boswprint_(int *jw, int *id);
void bosWprint(int *jw, int id);

#ifdef	__cplusplus
}
#endif

#define _BOSFUN_
#endif

