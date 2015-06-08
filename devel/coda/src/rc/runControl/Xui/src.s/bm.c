/* ---------------------------------------------------------------------- */

#include <stdio.h>

/* Boyer-Moore */

#define XSIZE BUFSIZ
#define ASIZE BUFSIZ
#define MAX(a,b)	(a>b ? a: b)

#define OUTPUT(i)	{ return i; }

/* below taken from http://www.dir.univ-rouen.fr/~charras/string/node14.html,
** slightly modified */

static void   PRE_BC(char *x, int m, int bm_bc[]) {
    int a, j;
   
    for (a=0; a < ASIZE; a++) bm_bc[a]=m;
    for (j=0; j < m-1; j++) bm_bc[x[j]]=m-j-1;
   }
   
   
   /* Preprocessing of the Good Suffix function shift. */
static void   PRE_GS(char *x, int m, int bm_gs[]) {
    int i, j, p, f[XSIZE];
   
    memset(bm_gs, 0, (m+1)*sizeof(int));
    f[m]=j=m+1;
    for (i=m; i > 0; i--) {
       while (j <= m && x[i-1] != x[j-1]) {
          if (bm_gs[j] == 0) bm_gs[j]=j-i;
          j=f[j];
       }
       f[i-1]=--j;
    }
    p=f[0];
    for (j=0; j <= m; ++j) {
       if (bm_gs[j] == 0) bm_gs[j]=p;
       if (j == p) p=f[p];
    }
   }
   
   
   /* Boyer-Moore string matching algorithm. */
int BM(char *y, char *x, int n, int m) {
    int i, j, bm_gs[XSIZE], bm_bc[ASIZE];
   
    /* Preprocessing */
    PRE_GS(x, m, bm_gs);
    PRE_BC(x, m, bm_bc);
   
    i=0;
    while(i <= n-m) {
      for (j=m-1; j >= 0 && y[i+j]==x[j]; --j);
      if (j < 0){
        OUTPUT(i);	/* return on first match */
	/* bugbug? RWM         i+=bm_gs[j+1]; */
      }
      else {
        i+=MAX(bm_gs[j+1],bm_bc[y[i+j]]-m+j+1);
      }   
    }
    OUTPUT(-1);
   }

/* ---------------------------------------------------------------------- */

/* usage: BM (fulltext, pattern, strlen(fulltext), strlen(pattern)); */
/* returns: first index, or -1 on error */

#undef TEST

#ifdef TEST
void main(int argc, char **argv)
{
	#define S1 "hello"
	#define S2 "alphahellobagel"
	#define S3 "lp"
	#define S4 "qj"
	#define S5 "l"

	int i;
	i = BM(S1,S2,strlen(S1),strlen(S2));
	fprintf(stderr,"%d\n",i);
	i = BM(S2,S1,strlen(S2),strlen(S1));
	fprintf(stderr,"%d\n",i);
	i = BM(S2,S3,strlen(S2),strlen(S3));
	fprintf(stderr,"%d\n",i);
	i = BM(S2,S4,strlen(S2),strlen(S4));
	fprintf(stderr,"%d\n",i);
	i = BM(S2,S5,strlen(S2),strlen(S5));
	fprintf(stderr,"%d\n",i);
}

#endif
