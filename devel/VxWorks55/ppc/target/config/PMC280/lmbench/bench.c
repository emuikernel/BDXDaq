#include <taskLib.h>
#include "bench.h"

IMPORT void	vxTimeBaseSet (UINT32 tbu, UINT32 tbl);
IMPORT void	vxTimeBaseGet (UINT32 * pTbu, UINT32 * pTbl);
/*IMPORT int bwMem(char *buf,int	nbytes,char	*what,void	(*pCopy)(void *bp1, void *bp2, int size));*/
IMPORT int latMemRead(char 	*addr,int len,int stride);

/*vijay added*/
#ifndef DEC_CLK_TO_INC 
    #define DEC_CLK_TO_INC	4
#endif /*DEC_CLK_TO_INC*/

#ifndef  DEFAULT_DEC_CLK_FREQ
#define	DEFAULT_DEC_CLK_FREQ	133000000
#endif /*8DEFAULT_DEC_CLK_FREQ*/

#define DEC_CLOCK_FREQ  DEFAULT_DEC_CLK_FREQ

#define DIVI_SEC        (DEC_CLOCK_FREQ/DEC_CLK_TO_INC)
#define DIVI_USEC       (DEC_CLOCK_FREQ/(DEC_CLK_TO_INC*1000000))

UINT32	tbStart_lm = 0;

void tbReset_lmbench(void)
{
    unsigned int tbStart = 0;
    
    vxTimeBaseSet(tbStart, tbStart);
}

#if 0
int gettimeofday
    (
    struct timeval *tp, 
    void *ptr
    )
    {
    UINT32      tbLo, tbHi;

    vxTimeBaseGet (&tbHi, &tbLo);
    tp->tv_sec = tbHi*(0xffffffff / (DIVI_SEC*DIVI_SEC)) +
                  (tbLo / DIVI_SEC);
    tp->tv_usec = (tbHi*(0xffffffff / (DIVI_USEC*DIVI_USEC)) +
                   (tbLo / DIVI_USEC)) - (tp->tv_sec * 1000000);

    return 0;
    }
#endif

/*dependent functions added by vijay*/
int gettimeofday_lmbench(struct timeval *tp, struct timezone *ptr )
{
#if 0
    UINT32      tbLo, tbHi;
    
    vxTimeBaseGet (&tbHi, &tbLo);

    tbLo -= tbStart_lm;
    
    tp->tv_sec = tbLo / (double)DIVI_SEC;
    tp->tv_usec = (tbLo - (tp->tv_sec * DIVI_SEC)) / DIVI_USEC;

  /*  return tp->tv_sec;*/
  
   UINT32      tbLo, tbHi;

    vxTimeBaseGet (&tbHi, &tbLo);
    tp->tv_sec = tbHi*(0xffffffff / (DIVI_SEC*DIVI_SEC)) +
                  (tbLo / DIVI_SEC);
    tp->tv_usec = (tbHi*(0xffffffff / (DIVI_USEC*DIVI_USEC)) +
                   (tbLo / DIVI_USEC)) - (tp->tv_sec * 1000000);

    return 0;
  #endif
  UINT32      tbLo, tbHi;
    double	tv;
    
    vxTimeBaseGet (&tbHi, &tbLo);
    
    tv = tbLo + tbHi * (double)0x100000000ULL;
    
    tp->tv_sec = tv / (double)DIVI_SEC;
    tp->tv_usec = (tv - ((double)tp->tv_sec * DIVI_SEC)) / (double)DIVI_USEC;

    return tp->tv_sec;
}

int getpagesize_lmbench()
{
    return 0x1000;
}


#if 0
int frcLmbench(unsigned int size)
{
   /*bwMem calculates the memory bandwidth*/
   int *ptr = (int* )malloc(size);  

   printf("Executing the bwMem\n");
   bwMem (ptr,size,"rd");
   
   printf("Executing the latMemRead\n");
   latMemRead (ptr,size,32);
   free(ptr);
   
}
#endif /*#if 0*/

#define LMBENCH_MAX_PERF_SIZE  8*1024*1024
int frcLmbench(void)
{
    long bufsize;
    unsigned long *pBufx; 
    void	(*pCopy)(void *bp1, void *bp2, int size) = NULL;
    void	(*pCopy1)(void *bp1, void *bp2, int size) = NULL;
    printf( "\nLMBENCH Memory Performance test.\n" );

    if( (pBufx= malloc( LMBENCH_MAX_PERF_SIZE )) == NULL )
    {
       printf( "malloc error\n" );
       return -1;
    }

    printf( "pBufx= %08lx\n", (unsigned long)pBufx );

    tbReset_lmbench();
    bufsize= 1024;
    printf("Executing the bwMem -read\n");
    while( bufsize<= LMBENCH_MAX_PERF_SIZE )
    {       
        bwMem (pBufx,bufsize,"rd",pCopy);
        bufsize= bufsize*2;   
        taskDelay(3);
    }
    
    tbReset_lmbench();
    pCopy = NULL;
    printf("Executing the bwMem -write\n");
    bufsize= 1024;
    while( bufsize<= LMBENCH_MAX_PERF_SIZE )
    {       
        bwMem (pBufx,bufsize,"wr",pCopy);
        bufsize= bufsize*2;   
        taskDelay(3);
    }

    tbReset_lmbench();
    pCopy = NULL;
    printf("Executing the bwMem -readwrite\n");
    bufsize= 1024;
    while( bufsize<= LMBENCH_MAX_PERF_SIZE )
    {       
        bwMem (pBufx,bufsize,"rdwr",pCopy);
        bufsize= bufsize*2;   
        taskDelay(3);
    }

    tbReset_lmbench();
    pCopy = NULL;
    printf("Executing the bwMem -copy\n");
    bufsize= 1024;
    while( bufsize<= LMBENCH_MAX_PERF_SIZE )
    {       
        bwMem (pBufx,bufsize,"cp",pCopy);
        bufsize= bufsize*2;   
        taskDelay(3);
    }

    tbReset_lmbench();
    pCopy = NULL;
    printf("Executing the bwMem -copy\n");
    bufsize= 1024;
    while( bufsize<= LMBENCH_MAX_PERF_SIZE )
    {       
        bwMem (pBufx,bufsize,"cp",pCopy1);
        bufsize= bufsize*2;   
        taskDelay(3);
    }

       bufsize= 0x00200000;           
       latMemRead ((char *)pBufx,bufsize,32);         

    free( pBufx );
    return 0;
}

