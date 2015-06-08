
/* iclib.h - header file for iclib.c */

#define length_ic 529
#define ehitmin   0.001
#define egrplmin  0.005
#define egrpmin   0.005
#define ihalf     23
#define MaxRaw    1000
#define MaxX      23
#define MaxY      23
#define ADCg      0.0001
#define TDCch     0.05
#define MaxHits   500
#define MaxGroup  20


/* functions */

int icinit(int runnum);
int tclib(int *jw);
