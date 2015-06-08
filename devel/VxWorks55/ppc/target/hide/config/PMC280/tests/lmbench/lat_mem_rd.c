/*
 * lat_mem_rd.c - measure memory load latency
 *
 * usage: lat_mem_rd size-in-MB stride [stride ...]
 *
 * Copyright (c) 1994 Larry McVoy.  Distributed under the FSF GPL with
 * additional restriction that results may published only if
 * (1) the benchmark is unmodified, and
 * (2) the version in the sccsid below is included in the report.
 * Support for this development by Sun Microsystems is gratefully acknowledged.
 */

#include "bench.h"
#define N       1000000	/* Don't change this */
#define STRIDE  (512/sizeof(char *))
#define	MEMTRIES	4
#define	LOWER	512
void	loads(char *addr, int range, int stride);
int	step(int k);

int latMemRead
	(
	char 	*addr,
        int     len,
	int	stride
	)
{
	int	range;
      printf("Size of memory block Read in MB \t Latency in Nano seconds\n");
	for (range = LOWER; range <= len; range = step(range)) 
		loads(addr, range, stride);
	return(0);
}

void
loads(char *addr, int range, int stride)
{
	register char **p = 0 /* lint */;
        int     i;
	int	tries = 0;
	int	result = 0x7fffffff;

     	if (stride & (sizeof(char *) - 1)) {
		printf("lat_mem_rd: stride must be aligned.\n");
		return;
	}
	
	if (range < stride) {
		return;
	}

        tbReset_lmbench();
	/*
	 * First create a list of pointers.
	 *
	 * This used to go forwards, we want to go backwards to try and defeat
	 * HP's fetch ahead.
	 *
	 * We really need to do a random pattern once we are doing one hit per 
	 * page.
	 */
	for (i = range - stride; i >= 0; i -= stride) {
		char	*next;

		p = (char **)&addr[i];
		if (i < stride) {
			next = &addr[range - stride];
		} else {
			next = &addr[i - stride];
		}
		*p = next;
	}

	/*
	 * Now walk them and time it.
	 */
        for (tries = 0; tries < MEMTRIES; ++tries) {
                /* time loop with loads */
#define	ONE	p = (char **)*p;
#define	FIVE	ONE ONE ONE ONE ONE
#define	TEN	FIVE FIVE
#define	FIFTY	TEN TEN TEN TEN TEN
#define	HUNDRED	FIFTY FIFTY
		i = N;
                start(0);
                while (i > 0) {
			HUNDRED
			HUNDRED
			HUNDRED
			HUNDRED
			HUNDRED
			HUNDRED
			HUNDRED
			HUNDRED
			HUNDRED
			HUNDRED
			i -= 1000;
                }
		i = stop(0,0);
		use_pointer((void *)p);
		if (i < result) {
			result = i;
		}
	}
	/*
	 * We want to get to nanoseconds / load.  We don't want to
	 * lose any precision in the process.  What we have is the
	 * milliseconds it took to do N loads, where N is 1 million,
	 * and we expect that each load took between 10 and 2000
	 * nanoseconds.
	 *
	 * We want just the memory latency time, not including the
	 * time to execute the load instruction.  We allow one clock
	 * for the instruction itself.  So we need to subtract off
	 * N * clk nanoseconds.
	 *
	 * lmbench 2.0 - do the subtration later, in the summary.
	 * Doing it here was problematic.
	 *
	 * XXX - we do not account for loop overhead here.
	 */
	result *= 1000;			/* convert to nanoseconds */
	result /= N;				/* nanosecs per load */
	
	fprintf(stderr, "      %.5f \t\t\t\t %d\n", range / (1024. * 1024), result);
}

int
step(int k)
{
	if (k < 1024) {
		k = k * 2;
        } else if (k < 4*1024) {
		k += 1024;
        } else if (k < 32*1024) {
		k += 2048;
        } else if (k < 64*1024) {
		k += 4096;
        } else if (k < 128*1024) {
		k += 8192;
        } else if (k < 256*1024) {
		k += 16384;
        } else if (k < 512*1024) {
		k += 32*1024;
	} else if (k < 4<<20) {
		k += 512 * 1024;
	} else if (k < 8<<20) {
		k += 1<<20;
	} else if (k < 20<<20) {
		k += 2<<20;
	} else {
		k += 10<<20;
	}
	return (k);
}

int
loads_i(char *addr, int range, int stride)
{
	register char **p = 0 /* lint */;
        int     i;
	int	tries = 0;
	int	result = 0x7fffffff;

     	if (stride & (sizeof(char *) - 1)) {
		printf("lat_mem_rd: stride must be aligned.\n");
		return -1;
	}
	
	if (range < stride) {
		return -1;
	}

	/*
	 * First create a list of pointers.
	 *
	 * This used to go forwards, we want to go backwards to try and defeat
	 * HP's fetch ahead.
	 *
	 * We really need to do a random pattern once we are doing one hit per 
	 * page.
	 */
	for (i = range - stride; i >= 0; i -= stride) {
		char	*next;

		p = (char **)&addr[i];
		if (i < stride) {
			next = &addr[range - stride];
		} else {
			next = &addr[i - stride];
		}
		*p = next;
	}

	/*
	 * Now walk them and time it.
	 */
        for (tries = 0; tries < MEMTRIES; ++tries) {
                /* time loop with loads */
#define	ONE	p = (char **)*p;
#define	FIVE	ONE ONE ONE ONE ONE
#define	TEN	FIVE FIVE
#define	FIFTY	TEN TEN TEN TEN TEN
#define	HUNDRED	FIFTY FIFTY
		i = N;
                start(0);
                while (i > 0) {
			HUNDRED
			HUNDRED
			HUNDRED
			HUNDRED
			HUNDRED
			HUNDRED
			HUNDRED
			HUNDRED
			HUNDRED
			HUNDRED
			i -= 1000;
                }
		i = stop(0,0);
		use_pointer((void *)p);
		if (i < result) {
			result = i;
		}
	}
	/*
	 * We want to get to nanoseconds / load.  We don't want to
	 * lose any precision in the process.  What we have is the
	 * milliseconds it took to do N loads, where N is 1 million,
	 * and we expect that each load took between 10 and 2000
	 * nanoseconds.
	 *
	 * We want just the memory latency time, not including the
	 * time to execute the load instruction.  We allow one clock
	 * for the instruction itself.  So we need to subtract off
	 * N * clk nanoseconds.
	 *
	 * lmbench 2.0 - do the subtration later, in the summary.
	 * Doing it here was problematic.
	 *
	 * XXX - we do not account for loop overhead here.
	 */
	result *= 1000;			/* convert to nanoseconds */
	result /= N;				/* nanosecs per load */

	return result;
}

