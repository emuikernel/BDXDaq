
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "fadcLib.h"


#ifdef Linux_vme
#define OK 0
/*
 * File:
 *    fadc250peds.c
 *
 * Description:
 *    JLab Flash ADC pedestal measurement (HPS firmware)
 *
 *

HPS UNIX:

 cd $CLON_PARMS/peds
 fadc250peds rocXX.ped
*/


#include "jvme.h"

#define FADC_ADDR (3<<19)
#define NFADC     16
#define SKIPSS
#define DIST_ADDR 0x0

DMA_MEM_ID vmeIN, vmeIN2, vmeOUT;
#define MAX_NUM_EVENTS    400
#define MAX_SIZE_EVENTS   1024*10      /* Size in Bytes */

extern int fadcA32Base;
extern int nfadc;
char *progName;

void Usage();

int 
main(int argc, char *argv[]) 
{
  GEF_STATUS status;
  char *filename;
  int inputchar=10;
  int ch, ifa=0;
  unsigned int cfw=0;
  FILE *f;
  fa250Ped ped;

  printf("\nJLAB fadc pedestal measurement\n");
  printf("----------------------------\n");

  progName = argv[0];

  if(argc != 2)
  {
    printf(" ERROR: Must specify one argument\n");
    Usage();
    exit(-1);
  }
  else
    filename = argv[1];

  status = vmeOpenDefaultWindows();

  fadcA32Base=0x09000000;

  int iFlag = (DIST_ADDR)<<10;
  /* Sync Source */
  iFlag |= (1<<0); /* P2 */
  /* Trigger Source */
  iFlag |= (1<<2); // VXS Input Trigger
  /* Clock Source */
  iFlag |= (0<<5); // Internal Clock Source

  iFlag |= (1<<18); // Skip firmware check
/*     iFlag |= (1<<16); // Skip initialization */

#ifdef SKIPSS
  faInit((unsigned int)(FADC_ADDR),(1<<19),NFADC+2,iFlag);
#else
  faInit((unsigned int)(FADC_ADDR),(1<<19),NFADC,iFlag);
#endif

  fadc250Config("");

  if(nfadc==0)
  {
    printf(" Unable to initialize any FADCs.\n");
    goto CLOSE;
  }
  
  f = fopen(filename, "wt");

  for(ifa=0; ifa<nfadc; ifa++)
  {
    for(ch=0; ch<16; ch++)
	 {
      if(faMeasureChannelPedestal(faSlot(ifa), ch, &ped) != OK)
		{
        printf(" Unabled to measure pedestal on slot %d, ch %d...\n", faSlot(ifa), ch);
        fclose(f);
        goto CLOSE;
		}
		if(f)
		  fprintf(f, "%2d %2d %7.3f %7.3f %2d\n", faSlot(ifa), ch, ped.avg, ped.rms, 0);
    }
  }
  if(f)
    fclose(f);
  else
    printf(" Unable to open pedestal file %s\n", filename);

CLOSE:

    status = vmeCloseDefaultWindows();
    if (status != GEF_SUCCESS)
    {
      printf("vmeCloseDefaultWindows failed: code 0x%08x\n",status);
      return -1;
    }

    exit(0);
}


void
Usage()
{
  printf("\n");
  printf("%s <pedestal filename>\n",progName);
  printf("\n");
}





#else
int main(){
return 0;
}

#endif
