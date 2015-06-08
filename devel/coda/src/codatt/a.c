
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* crate, slot, channel */
static int ttpad[4][22][128];
static int ttbanknum[4][22][128];

int
rorcReadTable()
{
  FILE *fd;
  int i, ret;
  char tmp[256];
  char bankname[10];
  char notinuse[128];
  int banknum, pad, crate, slot, channel;

  for(crate=0; crate<4; crate++)
  {
    for(slot=0; slot<23; slot++)
	{
      for(channel=0; channel<128; channel++)
	  {
        ttpad[crate][slot][channel] = 0;
        ttbanknum[crate][slot][channel] = 0;
	  }
	}
  }

  fd = fopen("/usr/local/clas/parms/TT/TPC0.tab","r");
  if(fd==NULL)
  {
    printf("ERROR 1 in rorcReadTable\n");
    return(-1);
  }

  while(fgets(tmp,255,fd)!=NULL)
  {
    if(tmp[0]=='#')
    {
      printf("comment: %s",tmp);fflush(stdout);
      continue; /* skip comments */
	}
    ret = sscanf(tmp,"%s %d %d %d %d %d %s",
      bankname,&banknum,&pad,&crate,&slot,&channel,notinuse);
    if(ret != 7)
    {
      printf("ERROR in TPC table: ret=%d, tmp >%s<\n",ret,tmp);
	}
    else
	{
      /* fill the lookup table */
      printf("ret=%d: >%s< %d %d %d %d %d >%s<\n",
        ret,bankname,banknum,pad,crate,slot,channel,notinuse);
      for(i=0; i<16; i++)
	  {
        ttpad[crate][slot][channel+i] = pad+i;
        ttbanknum[crate][slot][channel+i] = banknum;
	  }
	}
  }

  fclose(fd);

  /* print whole table */
  printf("\n TPC translation table\n");
  i = 0;
  for(crate=0; crate<4; crate++)
  {
    for(slot=0; slot<23; slot++)
	{
      printf(" line#   crate   slot   channel  ->   banknum   pad\n");
      for(channel=0; channel<128; channel++)
	  {
        printf("[%5d]   %2d      %2d      %3d            %1d  %6d\n",i,crate,slot,channel,
          ttbanknum[crate][slot][channel],ttpad[crate][slot][channel]);
        i++;
	  }
	}
  }



  return(0);
}

int
main()
{
  rorcReadTable();
  exit(0);
}
