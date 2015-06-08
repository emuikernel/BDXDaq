h43469
s 00001/00003/00016
d D 1.2 07/10/29 00:40:28 boiarino 2 1
c *** empty log message ***
e
s 00019/00000/00000
d D 1.1 07/10/29 00:27:07 boiarino 1 0
c date and time created 07/10/29 00:27:07 by boiarino
e
u
U
f e 0
t
T
I 1

/* epicsutil.h */


// max entries in file
#define MAX_ENTRY 5000

D 2
// max length of epics ca name
#define MAXCANAMELEN 60
E 2

#ifdef  __cplusplus
extern "C" {
#endif

D 2
int getepics(int nPvs, char canames[MAX_ENTRY][MAXCANAMELEN], float *valfloats);
E 2
I 2
int getepics(int nPvs, char *canames[MAX_ENTRY], float *valfloats);
E 2

#ifdef  __cplusplus
}
#endif
E 1
