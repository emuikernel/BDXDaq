
/* utload.c binary download utility */

#ifdef VXWORKS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*print/set memory, to be used by tcpClient for example*/
void
utprintmemory(unsigned int am, unsigned int offset, unsigned int mode)
{
  unsigned addr;
  volatile unsigned int   *ptr32;
  volatile unsigned short *ptr16;
  volatile unsigned char  *ptr08;

  if(am == 0) /*address modifier ==0 -> use 'offset' as absolute address*/
  {
    addr = offset;
  }
  else if(am == 0x09)
  {
    sysBusToLocalAdrs(0x09,offset,&addr);
  }
  else if(am == 0x39)
  {
    sysBusToLocalAdrs(0x39,offset&0xFFFFFF,&addr);
  }
  else if(am == 0x29)
  {
    sysBusToLocalAdrs(0x29,offset&0xFFFF,&addr);
  }
  else
  {
    return;
  }

  if(addr == 0xffffffff) return;

  if(mode==1)
  {
    ptr08 = (volatile unsigned char *)addr;
    printf("0x%02x\n",*ptr08);
  }
  else if(mode==2)
  {
    ptr16 = (volatile unsigned short *)addr;
    printf("0x%04x\n",*ptr16);
  }
  else
  {
    ptr32 = (volatile unsigned int *)addr;
    printf("0x%08x\n",*ptr32);
  }

  return;
}

void
utsetmemory(unsigned int am, unsigned int offset, unsigned int mode, unsigned int value)
{
  unsigned addr;
  volatile unsigned int   *ptr32;
  volatile unsigned short *ptr16;
  volatile unsigned char  *ptr08;

  if(am==0) /*address modifier ==0 -> use 'offset' as absolute address*/
  {
    addr = offset;
  }
  else if(am == 0x09)
  {
    sysBusToLocalAdrs(0x09,offset,&addr);
  }
  else if(am == 0x39)
  {
    sysBusToLocalAdrs(0x39,offset&0xFFFFFF,&addr);
  }
  else if(am == 0x29)
  {
    sysBusToLocalAdrs(0x29,offset&0xFFFF,&addr);
  }
  else
  {
    return;
  }

  if(addr == 0xffffffff) return;

  if(mode==1)
  {
    ptr08 = (volatile unsigned char *)addr;
    *ptr08 = value & 0xFF;
  }
  else if(mode==2)
  {
    ptr16 = (volatile unsigned short *)addr;
    *ptr16 = value & 0xFFFF;
  }
  else
  {
    ptr32 = (volatile unsigned int *)addr;
    *ptr32 = value & 0xFFFFFFFF;
  }

  return;
}

void
pciprint0()
{
  int i, aaa;

  for(i=0; i<0x250; i+=4)
  {
    pciConfigInLong(0,0x0,0,i,&aaa);
    printf("[0x%08x] 0x%08x\n",i,aaa);
  }

}

void
pciprint1()
{
  int i, aaa;

  for(i=0; i<0x250; i+=4)
  {
    pciConfigInLong(0,0x10,0,i,&aaa);
    printf("[0x%08x] 0x%08x\n",i,aaa);
  }

}


int
utload(char *fname, unsigned int address)
{
  FILE *fp;
  size_t res;
  unsigned int *ptr;

  if((fp = fopen(fname,"rb")) != NULL)
  {
    printf("downloading binary file >%s<\n",fname);

    ptr = (unsigned int *) address;
    res = fread(ptr, 1, 1000000, fp);

    printf("read %d bytes\n",res);

    fclose(fp);
  }
  else
  {
    printf("cannot download file >%s<\n",fname);
  }

  return(0);
}

void
utmemscan(unsigned int adr1, unsigned int adr2, unsigned int step)
{
  int status;
  unsigned short data;
  unsigned short *ptr;

  ptr = (unsigned short *) adr1;
  while(ptr < (unsigned short *)adr2)
  {
    status = vxMemProbe((char *)ptr, READ, 2, (char *)&data);
    if(status == OK)
    {
      /*printf("[0x%08x] +\n",ptr);*/
      /*if((*ptr==0x7c3043a6) || (*ptr=0xa643307c))*/
	  /*
      if((*ptr==0x1057480b) || (*ptr==0x480b1057) ||
         (*ptr==0x57100b48) || (*ptr==0x480b1057))
        printf("[0x%08x] = 0x%08x\n",ptr,*ptr);
	  */

      if((*ptr==0x8400) || (*ptr==0x0084)|| (*ptr==0x4800)|| (*ptr==0x0048))
      {
        printf("[0x%08x] = 0x%0x\n",ptr,*ptr);
        printf("  [0x%08x] = 0x%0x\n",(ptr+1),*(ptr+1));
	  }
    }
    /*else             printf("[0x%08x] -\n",ptr);*/
    ptr += step;
if(((int)ptr)%0x1000000 == 0) printf("[0x%08x] ..\n",ptr);
if(ptr < 0x100) return;
  }

  return;
}

#else /* no UNIX version */

void
utload_dummy()
{
  return;
}

#endif
