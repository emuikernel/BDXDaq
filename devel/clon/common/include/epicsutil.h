
/* epicsutil.h */


// max entries in file
#define MAX_ENTRY 5000


#ifdef  __cplusplus
extern "C" {
#endif

int getepics(int nPvs, char *canames[MAX_ENTRY], float *valfloats);

#ifdef  __cplusplus
}
#endif
