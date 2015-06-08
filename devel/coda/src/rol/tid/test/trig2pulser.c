/*
 * File:
 *    trig2pulser.c
 *
 * Description:
 *    attempt to Pulse trigger 2 at a specified frequency
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include "jvme.h"
#include "tidLib.h"

/* trig2 thread */
static void trig2loop(void);
pthread_attr_t trig2thread_attr;
pthread_t      trig2thread;

unsigned int rate=30;

int 
main(int argc, char *argv[]) 
{
  int stat;

  if(argc<2)
    rate = 30;
  if(argc==2)
    rate = atoi(argv[1]);

  printf("\nvTID/TS Trig2 Pulser (%d Hz)\n",rate);
  printf("----------------------------\n");

  vmeOpenDefaultWindows();

  /* Set the TIR structure pointer */
  tidInit(21<<19,0,0,0);

  tidSetTriggerSource(4,0);

  tidEnableFiber( (1<<0) | (1<<5) );
  tidSetBusyInputMask(TID_ENABLEBITS_BUSY_ENABLE_HFBR6);
  
  /* Set sync source as HFBR1 */
  tidSetSyncSource(1);
  
  tidTrigLinkReset();
  tidSyncReset();
  

  int inputchar=10;
  printf("Press <Enter> to generate a trigger\n");
  getchar();

  /* Launch thread here */
  stat = pthread_create(&trig2thread,
		       NULL,
		       (void*(*)(void *)) trig2loop,
		       (void *)NULL);
  if(stat!=0)
    {
      printf("Unable to start trig2loop thread\n");
      printf("\t pthread_create returned: %d\n",stat);
      goto CLOSE;
    }

  
  while(1 && (inputchar==10))
    {
      printf("1 and <Enter> to End\n");
      inputchar = getchar();
    }

  void *res;
  if(trig2thread)
    {
      if(pthread_cancel(trig2thread)<0) 
	perror("pthread_cancel");
      if(pthread_join(trig2thread,&res)<0)
	perror("pthread_join");
      if (res == PTHREAD_CANCELED)
	printf("%s: trig2loop thread canceled\n",__FUNCTION__);
      else
	printf("%s: ERROR: trig2loop NOT canceled\n",__FUNCTION__);
    }

  goto CLOSE;

 CLOSE:

  vmeCloseDefaultWindows();

  exit(0);
}

static void
trig2loop(void)
{

  printf("%s thread started\n",__FUNCTION__);

  while(1)
    {
      vmeBusLock();
      tidTrigger2();
      vmeBusUnlock();

      pthread_testcancel();

      usleep((useconds_t) (1/rate)*10^6);

      pthread_testcancel();
    }

}
