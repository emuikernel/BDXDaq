#ifndef _SGLIB_

/* sglib.h */

#ifdef	__cplusplus
extern "C" {
#endif

#include "sgutil.h"

/* function prototypes (only these will be called from outside) */

void sginit();
void sginitfun(PRSEC sgm);
void sglib(int *jw, int isec, int opt, int *nsgm, PRSEC *sgm);
void sgprint(PRSEC *sgm);
void sgroad(PRSEC *sgm, int ncl[3], unsigned char ird[3][2][nclmx]);

int sgtrigger(int *jw, int isec, int nsl, int layershifts[6]);
int sgremovenoise(int *jw, int isec, int nsl, int layershifts[6]);

#ifdef	__cplusplus
}
#endif

#define _SGLIB_
#endif


