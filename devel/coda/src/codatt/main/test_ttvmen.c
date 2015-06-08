
/* test_ttvmen.c - reads and process ascii buffer printed by ttvmen at error */

#include <stdio.h>

#include "ttbosio.h"
#include "coda.h"
#include "tt.h"

int
main()
{
  FILE *fd;
  unsigned int *bufin, *bufout;
  int nin, nout;
  TTSPtr ttp = NULL;
  int i;
  char str[132];
  unsigned int addr, data;


  bufin = malloc(1000000);
  if(bufin==NULL)
  {
    printf("Cannot allocate bufin - exit\n");
    exit(0);
  }

  bufout = malloc(1000000);
  if(bufout==NULL)
  {
    printf("Cannot allocate bufout - exit\n");
    exit(0);
  }

  fd = fopen("sc2.dat","r");
  if(fd==NULL)
  {
    printf("Cannot open file - exit\n");
    exit(0);
  }


  nin = 1;
  while(fgets(str,132,fd) != NULL)
  {
    if(str[0] == '#') continue;
    if(!strncmp(str,"[xxx",4)) break;

    str[29] = '\0';
    sscanf(&str[19],"%x",&data);
	
    bufin[nin++] = data;
    /*printf("str->%s<, data=0x%08x\n",&str[19],data);*/
  }
  bufin[0] = nin-1;
  printf("---------------> nin=%d\n",nin);

  fclose(fd);


  printf("downloading DDL table ...\n");
  clonbanks();
  printf(" ... done.\n");

  printf("downloading translation table for roc=%d (ttp=0x%08x)\n",22,ttp);
  ttp = TT_LoadROCTT(22, ttp);
  printf(" ... done.\n");

  bufout[0] = NWBOS;
  bosinit(&bufout[2],bufout[2]);

  TT_TranslateVMEBank(bufin, bufout, ttp);

  exit(0);
}
