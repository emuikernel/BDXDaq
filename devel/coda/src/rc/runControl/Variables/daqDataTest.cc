#include "daqNetData.h"

char *runtype[]=
{"test","fscc","camac","complex","electron", "phnon","hallc1","hallc2",
"hallc3"};

main (int argc, char **argv)
{
  daqNetData send1 ("RCS", "allRunTypes", runtype, 9);
  daqNetData recv1;

  printf ("Data Send out is %s\n", (char *)send1);
  char *buffer;
  long size = 0;
  long datalen = 0;
  encodeNetData (send1, buffer, size);
  printf ("Size of encoded data is %d\n", size);
  decodeNetData (recv1, buffer, size);

  printf ("Data received is %s\n", (char *)recv1);
  delete []buffer;

  double rate = 324242323111.3232;
  daqNetData send2 ("RCS", "dataRate", rate);
  daqNetData recv2;

  encodeNetData (send2, buffer, size);
  printf ("Size of encoded data is %d\n", size);
  decodeNetData (recv2, buffer, size);

  printf ("Data received is %f\n", (double)recv2);
  delete []buffer;

  int number = 1009;
  
  daqNetData send3 ("RCS", "runNumber", number);
  daqNetData recv3;

  encodeNetData (send3, buffer, size);
  printf ("Size of encoded data is %d\n", size);
  decodeNetData (recv3, buffer, size);

  printf ("Data received is %d\n", (int)recv3);
  delete []buffer;


  int rs[50];
  for (int i = 0; i < 50; i++)
    rs[i] = i*40;
  
  daqNetData send4 ("RCS", "hist", rs, 50);
  daqNetData recv4;

  encodeNetData (send4, buffer, size);
  printf ("Size of encoded data is %d\n", size);
  decodeNetData (recv4, buffer, size);

  printf ("Data received is %s\n", (char *)recv4);
  delete []buffer;

  double rsd[100];
  for (i = 0; i < 80; i++)
    rsd[i] = i*0.1;

  daqNetData send5 ("RCS", "dhist", rsd, 80);
  daqNetData recv5;

  encodeNetData (send5, buffer, size);
  printf ("Size of encoded data is %d\n", size);
  decodeNetData (recv5, buffer, size);

  printf ("Data received is %s\n", (char *)recv5);
  delete []buffer;

  float rsf[100];
  for (i = 0; i < 65; i++)
    rsf[i] = i*10.11;

  daqNetData send6 ("RCS", "fhist", rsf, 65);
  daqNetData recv6;

  encodeNetData (send6, buffer, size);
  printf ("Size of encoded data is %d\n", size);
  decodeNetData (recv6, buffer, size);

  printf ("Data received is %s\n", (char *)recv6);
  delete []buffer;
}
