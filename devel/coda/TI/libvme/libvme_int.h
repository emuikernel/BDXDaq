

#include <linux/timex.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#ifndef cycles_t
typedef unsigned long cycles_t;
#endif

typedef struct {
	int vector;
	int irq;
	int irq_count;
	int read_count;
	int status;
	cycles_t cycles;
}vme_int_data_t;


#ifndef VOIDFUNCPTR9
typedef void VOIDFUNCPTR9(int);
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int vmeIntConnect(VOIDFUNCPTR9 routine, int arg);
int vmeIntDisconnect(void);

#ifdef __cplusplus
}
#endif // __cplusplus

