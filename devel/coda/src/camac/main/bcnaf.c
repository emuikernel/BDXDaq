/*
   bcnaf.c - copied from cnaf.c and adjusted for multiple branches
             by Sergey Boyarinov
 */

#include <stdio.h>

#ifdef VXWORKS

#include "vxWorks.h"

void
bcnafr(int b, int c, int n, int a, int f)
{
  int ext, d, q, lstat, x, stat;

  if((f>=16) && (f<24)) /* write function -- data required */
  {
    printf("ERROR: f=%d - write function - use bcnafw() !!!\n",f);
  }

  printf("b,c,n,a,f=%d %d %d %d %d\n",b,c,n,a,f);

  cdreg(&ext,b,c,n,a);
  cfsa(f,ext,&d,&q);
  ctstatus(ext,&lstat);

  x = (~lstat>>1)&1;
  if(f<=7)
  {
    printf("lstat=0x%08x -> dec:%d    hex:0x%x    q:%d  x:%d\n",lstat,d,d,q,x);
  }
  else
  {
    printf("lstat=0x%08x -> q:%d  x:%d\n",lstat,q,x);
  }

  return;
}

void
bcnafw(int b, int c, int n, int a, int f, int d)
{
  int ext, q, lstat, x, stat;

  printf("b,c,n,a,f=%d %d %d %d %d\n",b,c,n,a,f);

  cdreg(&ext,b,c,n,a);
  cfsa(f,ext,&d,&q);
  ctstatus(ext,&lstat);

  x = (~lstat>>1)&1;
  if(f<=7)
  {
    printf("lstat=0x%08x -> dec:%d    hex:0x%x    q:%d  x:%d\n",lstat,d,d,q,x);
  }
  else
  {
    printf("lstat=0x%08x -> q:%d  x:%d\n",lstat,q,x);
  }

  return;
}

void
bcnaf(int b, int c, int n, int a, int f)
{
  bcnafr(b, c, n, a, f);
}

#else

#include <stdlib.h>
#include <ca.h>

int
main(int argc, char *argv[])
{
  int ext, b, c, n, a, f, d, q, lstat, x;
  int stat, nfields;
  char line[80];

  if(argc < 2)
  {
    printf("Usage: bcnaf target [b c n a f [d]]\n");
    return(0);
  }

  caopen(argv[1],&stat);
  if(stat != 1)
  {
    printf("failed to establish connection to target machine %s\n",argv[1]);
    printf("Usage: bcnaf target [b c n a f [d]]\n");
    return(0);
  }

  if((argc == 7) || (argc == 8))
  {
    sprintf(line,"%s %s %s %s %s\0",argv[2],argv[3],argv[4],argv[5],argv[6]);
    if(sscanf(line,"%i%i%i%i%i",&b,&c,&n,&a,&f) != 5)
    {
      printf("bad arguments: %s\n",line);
      printf("Usage: bcnaf target [b c n a f [d]]\n");
      return(0);
    }

    if((f>=16) && (f<24)) /* write function -- data required */
    {
      if(argc != 8)
      {
        printf("bcnaf data missing, 0 assumed\n");
        d = 0;
      }
      else
      {
        d = strtoul(argv[7],NULL,0);
      }
    }
    else if(argc != 7)
    {
      printf("bcnaf data argument ignored (non-write function)\n");
    }

    cdreg(&ext,b,c,n,a);
    cfsa(f,ext,&d,&q);
    ctstatus(ext,&lstat);

    x = (~lstat>>1)&1;
    if(f<=7)
    {
      printf("lstat=0x%08x -> dec:%d    hex:%x    q:%d  x:%d\n",lstat,d,d,q,x);
    }
    else
    {
      printf("q:%d  x:%d\n",q,x);
    }
  }
  else
  {
    printf("Invalid number of arguments\n");
    printf("Usage: bcnaf target [b c n a f [d]]\n");
  }

  exit(0);
}

#endif
