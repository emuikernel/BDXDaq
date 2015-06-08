
/* binary1.c - binary file to be downloaded by utload() */

void
binary1()
{
  int i;
  unsigned int *ptr0, *ptr;

  i = 0xF;

  ptr0 = (unsigned int *) 0x80002000;
  *ptr0 = 1;
  ptr = (unsigned int *) 0x80001000;

  while(1)
  {
    i++;
    if(i>0xFFFF) i=0;
    *ptr = i;

    ptr0 = (unsigned int *) 0x80002000;
    if((*ptr0) != 1) break;
  }

  return;
}

