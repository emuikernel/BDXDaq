
/* dacgethostbynametest.c */

#ifndef VXWORKS

#include <stdio.h>

main()
{
  char ipaddress[20];

  dacgethostbyname("croctest1", ipaddress);

  if(strcmp("129.57.68.88",ipaddress))
  {
    printf("ERROR: UNAUTORIZED ACCESS FROM >%s<, goto accept() again\n","croctest1");
  }
  else
  {
    printf("Ok\n");
  }

  exit(0);
}

#else

void
dacgethostbynametest_dummy()
{
  return;
}

#endif
