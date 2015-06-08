//---------------------------------------------------------------------------
//title: VME Debugger for XVME-655 
//version: Linux 1.1
//date: February 1998
//designer: Michael Wyrick                                                      
//programmer: Michael Wyrick
//project: VMELinux Project in association with Chesapeake Research
//platform: Linux 2.2.x, 2.4.x
//language: GCC 2.95, GCC 3.0
//module: 
//------------------------------------------------------------------------------  
//  Purpose:                                                                    
//  Docs:                                                                       
//------------------------------------------------------------------------------  
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <asm/io.h>
#include <malloc.h>

#include "unilib.h"
#include "commands.h"
#include "universe.h"
#include "vmeutils.h"

#define CMDT_NULL  0
#define CMDT_FUNC  1
#define CMDT_MODC  2
#define CMDT_MODI  3
#define CMDT_MODL  4
#define CMDT_MODB  5

#define DEFCNT 128

// Size of a DMA write file transfer
#define WRITEBUFSIZE    1024

int VMEspace = 1;        // 1 = A16, 2 = A24, 3 = A32

char TestValueC = 0;
int  TestValueI = 0;
long TestValueL = 0;

long FixedAdder = 0;
long VME_Valid_Base = 0;
long VME_Valid_Cnt  = 0;

typedef void (*func)(char *,int);

typedef struct {
  int  CmdType;
  char *command;
  int  NumParams;
  void *pointer;
} COMMAND;

COMMAND cmdTable[] = {
  {CMDT_FUNC,"QUIT",0,(void*)quit},
  {CMDT_FUNC,"Q",0,(void*)quit},
  {CMDT_FUNC,"EXIT",0,(void*)quit},
  {CMDT_FUNC,"E",0,(void*)quit},
  {CMDT_FUNC,"HELP",0,(void*)help},
  {CMDT_FUNC,"?",0,(void*)help},
  {CMDT_FUNC,"STATUS",0,(void*)status},
  {CMDT_FUNC,"REGS",0,(void*)regs},
  {CMDT_FUNC,"CLS",0,(void*)clear},

  {CMDT_FUNC,"D",2,(void*)Display},
  {CMDT_FUNC,"R",2,(void*)Display},
  {CMDT_FUNC,"RB",2,(void*)Display},
  {CMDT_FUNC,"T",2,(void*)DisplayMonitor},
  {CMDT_FUNC,"DW",2,(void*)DisplayWord},
  {CMDT_FUNC,"RW",2,(void*)DisplayWord},
  {CMDT_FUNC,"DL",2,(void*)DisplayLong},
  {CMDT_FUNC,"RL",2,(void*)DisplayLong},

  {CMDT_FUNC,"W",2,(void*)Write},
  {CMDT_FUNC,"WB",2,(void*)Write},
  {CMDT_FUNC,"WW",2,(void*)WriteWord},
  {CMDT_FUNC,"WL",2,(void*)WriteLong},

  {CMDT_FUNC,"RF",2,(void*)ReadFile},
  {CMDT_FUNC,"WF",2,(void*)WriteFile},
  {CMDT_FUNC,"WSRF",2,(void*)WriteSRecordsFile},

  {CMDT_FUNC,"SEEK",1,(void*)Seek},

  {CMDT_FUNC,"REGR",1,(void*)ReadReg},
  {CMDT_FUNC,"REGW",2,(void*)WriteReg},

  {CMDT_FUNC,"MAP",1,(void*)vmemap1},         

  {CMDT_FUNC,"TESTWRITE",1,(void*)TestWrite},         
  {CMDT_FUNC,"TESTREAD",1,(void*)TestRead},         
  {CMDT_FUNC,"TESTDMA",1,(void*)TestDMA},         
  {CMDT_FUNC,"TESTPROGRAMMED",1,(void*)TestProgrammed},         

  {CMDT_FUNC,"WINT", 1, (void*)wint},

  {CMDT_MODL,"FA",1,&FixedAdder},         

  {CMDT_NULL,"",0,NULL}               // End of Table
};  

unsigned long lastpnt = 0; 
unsigned long VMEMapping = 0; 

extern int quiet;
extern int default_mode;
extern int binary;

/*------------------------------------------------------------------------------
 * 
 *
 *------------------------------------------------------------------------mjw-*/
void clear(char *b,int l)
{
}

/*------------------------------------------------------------------------------
 * ValidAddress
 *   return 1 if the ptr is in the current Mapping
 *------------------------------------------------------------------------mjw-*/
int ValidAddress(long ptr)
{
  if ((ptr >= VME_Valid_Base) && 
      (ptr < VME_Valid_Base + VME_Valid_Cnt))
    return(1);
  else {
    printf("  An attempt was made to access an invalid address. %0lX",ptr);
    return(0);
  }  
}

//----------------------SREC-------------
#define ERROR 1
#define BUFFER_SIZE             4096
#define BYT(cp)                 ((Asc2Hex(cp[0]) << 4) | Asc2Hex(cp[1]))

static FILE *infile;
static char *buffer;
static char *goffset;
/*-------------------------------------------------------------------------*/
/*  Asc2Hex - convert a two byte ascii string to a byte                    */
/*-------------------------------------------------------------------------*/
static char Asc2Hex(char byt)
{
  if ((byt < 0x3A) && (byt > 0x2F))
    return(byt & 0x0F);
  if ((byt < 'G') && (byt > '@'))
    return((byt & 0x0F) + 9);
  if ((byt < 'g') && (byt > 0x60))
    return((byt & 0x0F) + 9);
  return(0xEE);  /* Return Error */
}

/*-------------------------------------------------------------------------*/
/*  ComputeAddr - Get address from S Record                                */
/*-------------------------------------------------------------------------*/
static unsigned long ComputeAddr(char *cp, int siz)
{
  unsigned long retval;

  for (retval = 0L; siz > 0; siz--, cp += 2)
    retval = (retval << 8) | BYT(cp);

  return(retval);
}

/*-------------------------------------------------------------------------*/
/*  checkwrite - Write a word to memory and read it back to check that it  */
/*               wrote correctly.                                          */
/*---------------------------------------------------------------------mjw-*/
void checkwrite(unsigned long a,unsigned char val)
{ 
  int error;

  wb(a,val,&error);
  if (error)
    printf("BUS Error during Write at address %08lX\n",a);
}

/*-------------------------------------------------------------------------*/
/*  DecodeS1 - Decode and place in memory S1 type records                  */
/*     Checks for correct checksum value and will not write if incorrect   */
/*---------------------------------------------------------------------mjw-*/
static void DecodeS1(char *cp, long offset,long skip)
{
  long address;
  unsigned short val; 
  unsigned char *datastart, cnt, cntr, chksum;
  unsigned long addr;

  cnt = BYT(cp) - 3;          /* Get Length of Line */
  chksum = BYT(cp);
  cp += 2;

  addr = ComputeAddr(cp,2) * skip;
  printf("Address %lX\n",addr);
  address = (addr + (unsigned long) goffset);

  chksum += BYT(cp);          /* Checksum 1st address byte */
  cp += 2;
  chksum += BYT(cp);          /* Checksum 2nd address byte */
  cp += 2;

  datastart = (unsigned char *)cp;  /* Data starts here */

  /* Calc rest of checksum please */
  for (cntr = 0; cntr <= cnt; cntr++, cp += 2)
    chksum += BYT(cp);

  /* If checksum not valid, skip this line */
  if (chksum != (unsigned char) 0xFF) {
    printf("Checksum failed - Got %X\n",chksum);
    return; 
  }

  /* Every thing is cool, write data to memory */
  for (cntr = 0; cntr < cnt; cntr++, datastart+=2, address+=skip) {
    val = BYT(datastart);
    checkwrite(address+offset,val);
  }
}

/*-------------------------------------------------------------------------*/
/*  dsp_srec                                                               */
/*      int srec(char *filename, char *offset)                             */
/*---------------------------------------------------------------------mjw-*/
int dsp_srec(char *filename, long offset, long skip)
{
  char *cp;
  int flag, pcount;
  int count = 0;
  int errcount = 0;
  int scount[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  buffer = (char *)malloc(BUFFER_SIZE);
  if (buffer == (char *) 0) {
    printf("\n\nLoad error: unable to get 4096 bytes of memory.\n\n\n");
    return(ERROR);
  }
  infile = fopen(filename, "r");
  if (infile == (FILE *) 0) {
    printf("\n\nLoad error: unable to open file\n\n\n");
    free(buffer);
    return(ERROR);
  }

  flag = pcount = 0;
  while ((fgets(buffer, BUFFER_SIZE, infile) != (char *) 0) && (flag == 0)) {
    cp = strtok(buffer, " \t\f\n");
    if (cp == (char *) 0)
      continue;

    if (cp[0] != 'S') {
      errcount++;
      continue;
    }

    count++;
    cp++;           /* point to the record type */
    switch (*cp) {
    case '0':       /* optional introduction record. */
      scount[0]++;
      break;
    case '1':       /* address count is 2 bytes long */
      scount[1]++;
      DecodeS1(++cp,offset,skip);
      break;
    case '2':       /* address count is 3 bytes long */
      scount[2]++;
      break;
    case '3':       /* address is 4 bytes long */
      scount[3]++;
      break;
    case '9':       /* End-Of-File record */
      scount[4]++;
      flag = 1;
      break;
    } /* switch */
  } /* while */

  fclose(infile);
  free(buffer);
  printf("\n%d lines processed\n",count);
  printf("S0 - %d\nS1 - %d\nS2 - %d\nS3 - %d\nS9 - %d\n\n\n",
         scount[0], scount[1], scount[2], scount[3], scount[9]);
  return(count);
}

//----------------------SREC-------------
void WriteSRecordsFile(char *b,int l)
{
  int num;
  long pnt,skip;
  char fname[255];

//  num = sscanf(b,"%lx %s %lx",&pnt,&fname,&skip);
  num = sscanf(b,"%lx %s %lx",&pnt,fname,&skip);

  pnt += FixedAdder;

  if ((num < 2) || (num > 3)) {
    printf("Usage: WriteSRecordFile address filename [skip]");
  } else {
    if (num == 2)
      skip = 1;
    if (ValidAddress(pnt)) {
      dsp_srec(fname,pnt,skip);
    }
  }  
}

/*------------------------------------------------------------------------------
 * WriteFile
 *
 *------------------------------------------------------------------------mjw-*/
void WriteFile(char *b,int l)
{
  int num,c;
  long pnt;
  char fname[255];
  FILE *fin;
  int error,n,num_written;
  unsigned int *dma_buffer;

  num = sscanf(b,"%lx %s",&pnt,fname);

  pnt += FixedAdder;

  if (num != 2) {
    printf("Usage: WriteFile address filename");
  } else {
    if (ValidAddress(pnt)) {
      fin = fopen(fname,"rb");
      if (!fin) {
        printf("Can't open file %s\n",fname);
      } else {
        // Programmed IO
        if (default_mode == MODE_PROGRAMMED) {
          while ((c = getc(fin)) != EOF) {
            wb(pnt++,c,&error);
            if (error) {
              printf("Bus Error during Writefile.\n");
              continue;
            }
          }
          if (!quiet)
            printf("Done Writing file to Memory");
        } else {
          // DMA
          dma_buffer = (unsigned int *)malloc(WRITEBUFSIZE);
          ioctl(vme_handle,IOCTL_SET_MODE,MODE_DMA);
          lseek(vme_handle,pnt,SEEK_SET);

          n = 1;   // Start the while loop on a good note please
          while (n) {
            // get data from file
            n = fread(dma_buffer,1,WRITEBUFSIZE,fin);
            // Do The DMA Transfer
            if (n > 0) {
              num_written = write(vme_handle,dma_buffer,n);
              if (num_written < 1) {
                printf("BUS Error during Write file DMA Transfer\n");
              }
            }
          }
          free(dma_buffer);
          ioctl(vme_handle,IOCTL_SET_MODE,MODE_PROGRAMMED);
        }  
      }
    }
  }  
}

/*------------------------------------------------------------------------------
 * Display 
 *   usage: display, addr, number of times
 *------------------------------------------------------------------------jsh-*/
void DisplayMonitor(char *b, int l)
{
  unsigned int x;
  for (x=0;x<200;x++) {
    Display(b, 1);
    printf("   %02X\n",l);
  }
}

/*------------------------------------------------------------------------------
 * Display 
 *   usage: display addr count
 *------------------------------------------------------------------------mjw-*/
void Display(char *b,int l)
{
  unsigned char *dma_buffer;
  long pnt;
  unsigned int count,x,num,n,a[16],y;
  int error;
  unsigned v;

  num = sscanf(b,"%lx %x",&pnt,&count);

  pnt += FixedAdder;

  if (num < 1) {
    pnt = lastpnt;
    num = 1;
  }

  if (num == 1) {
    count = DEFCNT;
    num = 2;
  }

  if (num != 2) {
    printf("Usage: d address [count]");
  } else {
    if (ValidAddress(pnt)) {
      if (default_mode == MODE_PROGRAMMED) {
        lastpnt = pnt + count;
        for (x=0;x<count;x++) {
          if (!binary) {
            if (x % 16 == 0)
              printf("%08lX: ",x+pnt);
            if (x % 16 == 8)
              printf("- ");

            v = rb(pnt+x,&error);
            if (!error) {
              printf("%02X ",v);
              a[x % 16] = v;
            } else {
              printf("** ");
              a[x % 16] = 0;
            }

            // Show the character values to the right.
            if (x % 16 == 15) {
              printf(": ");
              for (y=0;y<16;y++) {
                if ( (a[y] > 31) && (a[y] < 127) )
                  printf("%c",a[y]);
                else
                  printf(".");
              }
              printf("\n");
            }
          } else {
            v = rb(pnt+x,&error);  
            if (!error)
              printf("%c",v);
            else
              printf("*");
          }                                 
        }  
      } else {  // end Mode Programmed
        if (!quiet)
          printf("--> Doing Read via DMA <--\n");
        dma_buffer = (unsigned char *)malloc(count);
        lastpnt = pnt + count;
        // Do The DMA Transfer
        ioctl(vme_handle,IOCTL_SET_MODE,MODE_DMA);
        lseek(vme_handle,pnt,SEEK_SET);
        n = read(vme_handle,dma_buffer,count);
        ioctl(vme_handle,IOCTL_SET_MODE,MODE_PROGRAMMED);

        if (n < 1) {
          printf("BUS Error\n");
        } else {
          // Print the Results
          for (x=0;x<n;x++) {
            if (!binary) {
              if (x % 16 == 0)
                printf("%08lX: ",x+pnt);
              if (x % 16 == 8)
                printf("- ");
              printf("%02X ",(unsigned char)dma_buffer[x]);
              if (x % 16 == 15)
                printf("\n");
            } else {
              printf("%c",(unsigned char)dma_buffer[x]);
            }  
          }  
        }  
        free(dma_buffer);
      } // end Mode DMA
    }
  }  
}   

/*------------------------------------------------------------------------------
 * DisplayWord 
 *   usage: display addr count
 *------------------------------------------------------------------------mjw-*/
void DisplayWord(char *b,int l)
{
  unsigned short *dma_buffer;
  long pnt;
  unsigned int count,x,num,n;
  int error;
  unsigned short v;

  num = sscanf(b,"%lx %x",&pnt,&count);

  pnt *= 2;
  pnt += FixedAdder;

  if (num < 1) {
    pnt = lastpnt;
    num = 1;
  }

  if (num == 1) {
    count = DEFCNT;
    num = 2;
  }

  if (num != 2) {
    printf("Usage: dw address [count]");
  } else {
//    count /= 2;  
    if (ValidAddress(pnt)) {
      if (default_mode == MODE_PROGRAMMED) {
        lastpnt = pnt + count * 2;
        for (x=0;x<count;x++) {
          if (!binary) {
            if (x % 8 == 0) {
              printf("%08lX: ",(x*2)+pnt);
              printf("%08lX: ",(x+((pnt-FixedAdder)/2)));
            }
            if (x % 8 == 4)
              printf("- ");

            v = rw(x*2+pnt,&error);  
            if (!error)
              printf("%04X ",v);
            else
              printf("**** ");

            if (x % 8 == 7)
              printf("\n");
          } else {
            v = rw(pnt+x*2,&error);  
            if (!error)
              printf("%c%c",(v&0x00FF),((v&0xFF00)>>8));
            else
              printf("**");
          }
        }
      } else {  // end Mode Programmed
        if (!quiet)
          printf("--> Doing Read via DMA <--\n");
        dma_buffer = (unsigned short *)malloc(count);
        lastpnt = pnt + count * 2;
        // Do The DMA Transfer
        ioctl(vme_handle,IOCTL_SET_MODE,MODE_DMA);
        lseek(vme_handle,pnt,SEEK_SET);
        n = read(vme_handle,dma_buffer,count*2);
        ioctl(vme_handle,IOCTL_SET_MODE,MODE_PROGRAMMED);

        if (n < 1) {
          printf("BUS Error\n");
        } else {
          // Print the Results
          for (x=0;x<(n/2);x++) {
            if (!binary) {
              if (x % 8 == 0)
                printf("%08lX: ",(x+pnt));
              if (x % 8 == 4)
                printf("- ");
              printf("%04X ",(unsigned short)dma_buffer[x]);
              if (x % 8 == 7)
                printf("\n");
            } else {
              v = dma_buffer[x];
              printf("%c%c",(v&0x00FF),((v&0xFF00)>>8));
            }
          }  
        }  
        free(dma_buffer);
      } // end Mode DMA
    }
  }
}

/*------------------------------------------------------------------------------
 * DisplayLong
 *   usage: display addr count
 *------------------------------------------------------------------------mjw-*/
void DisplayLong(char *b,int l)
{
  unsigned int *dma_buffer;
  long pnt;
  unsigned int count,x,num,n;
  int error;
  unsigned long v;

  num = sscanf(b,"%lx %x",&pnt,&count);

  pnt += FixedAdder;

  if (num < 1) {
    pnt = lastpnt;
    num = 1;
  }

  if (num == 1) {
    count = DEFCNT;
    num = 2;
  }

  if (num != 2) {
    printf("Usage: dl address [count]");
  } else {
    count /= 4;  
    if (ValidAddress(pnt)) {
      if (default_mode == MODE_PROGRAMMED) {
        lastpnt = pnt + count*4;
        for (x=0;x<count;x++) {
          if (!binary) {
            if (x % 4 == 0)
              printf("%08lX: ",(x*4+pnt));
            if (x % 4 == 2)
              printf("- ");
            v = rl(x*4+pnt,&error);  
            if (!error)
              printf("%08lX ",v);
            else
              printf("******** ");
            if (x % 4 == 3)
              printf("\n");

          } else {
            v = rl(x*4+pnt,&error);  
            if (!error)
              printf("%c%c%c%c",  (char)(v&0x000000FF), (char) ((v&0x0000FF00)>>8),
                     (char)((v&0x00FF0000)>>16), (char)((v&0xFF000000)>>24));
            else
              printf("****");
          }  
        }         
      } else {
        if (!quiet)
          printf("--> Doing Read via DMA <--\n");
        dma_buffer = (unsigned int *)malloc(count);
        lastpnt = pnt + count * 4;
        // Do The DMA Transfer
        ioctl(vme_handle,IOCTL_SET_MODE,MODE_DMA);
        lseek(vme_handle,pnt,SEEK_SET);
        n = read(vme_handle,dma_buffer,count*4);
        ioctl(vme_handle,IOCTL_SET_MODE,MODE_PROGRAMMED);

        if (n < 1) {
          printf("BUS Error\n");
        } else {
          for (x=0;x<(n/4);x++) {
            if (!binary) {
              if (x % 4 == 0)
                printf("%08lX: ",(x*4+pnt));
              if (x % 4 == 2)
                printf("- ");
              printf("%08X ",(unsigned int)dma_buffer[x]);
              if (x % 4 == 3)
                printf("\n");
            } else {
              v = dma_buffer[x];
              printf("%c%c%c%c",  (char)(v&0x000000FF),  (char)((v&0x0000FF00)>>8),
                     (char)((v&0x00FF0000)>>16), (char)((v&0xFF000000)>>24));
            }
          }   
        }    
      }
    }
  }
}

/*------------------------------------------------------------------------------
 * Write 
 *   usage: Write addr value
 *------------------------------------------------------------------------mjw-*/
void Write(char *b,int l)
{
  long pnt,num,cnt,x;
  unsigned char v;
  int error;

  num = sscanf(b,"%lx %hhx %lx",&pnt,&v,&cnt);

  pnt += FixedAdder;

  if (num < 2) {
    printf("Usage: w address value [count]");
  } else {
    if (num < 3)
      cnt = 1;
    if (ValidAddress(pnt)) {
      for (x=0;x<cnt;x++) {
        wb(pnt++,v,&error);
        if (error)
          printf("BUS Error during Write at address %08lX\n",pnt-1);
      } 
    }
  }  
}

/*------------------------------------------------------------------------------
 * WriteWord 
 *   usage: Write addr value
 *------------------------------------------------------------------------mjw-*/
void WriteWord(char *b,int l)
{
  long pnt,num,cnt,x;
  unsigned short v;
  int error;

  num = sscanf(b,"%lx %hx %lx",&pnt,&v,&cnt);

  pnt *= 2;            // word addressing
  pnt += FixedAdder;

  if (num < 2) {
    printf("Usage: ww address value [count]");
  } else {
    if (num < 3)
      cnt = 1;
    if (ValidAddress(pnt)) {
      for (x=0;x<cnt;x++) {
        ww(pnt,v,&error);
        pnt += 2;         
        if (error)
          printf("BUS Error during Write at address %08lX\n",pnt-2);
      } 
    }
  }  
}

/*------------------------------------------------------------------------------
 * WriteLong 
 *   usage: Write addr value
 *------------------------------------------------------------------------mjw-*/
void WriteLong(char *b,int l)
{
  long pnt,num,cnt,x;
  unsigned long v;
  int error;

  num = sscanf(b,"%lx %lx %lx",&pnt,&v,&cnt);

  pnt += FixedAdder;

  if (num < 2) {
    printf("Usage: wl address value [count]");
  } else {
    if (num < 3)
      cnt = 1;
    if (ValidAddress(pnt)) {
      for (x=0;x<cnt;x++) {
        wl(pnt,v,&error);
        pnt += 4;         
        if (error)
          printf("BUS Error during Write at address %08lX\n",pnt-4);
      } 
    }
  }  
}

/*------------------------------------------------------------------------------
 * Seek
 *
 *------------------------------------------------------------------------mjw-*/
void Seek(char *b,int l)
{
  unsigned int num;
  long ptr;

  num = sscanf(b,"%lx",&ptr);

  ptr += FixedAdder;

  if (ValidAddress(ptr)) {
    if (num != 1) {
      printf("Seek usage:  SEEK address\n");
    } else {
      lseek(vme_handle,ptr,SEEK_SET);
    } 
  }
}                                                                               

/*------------------------------------------------------------------------------
 * wint
 *  wait for VME interrupt
 *------------------------------------------------------------------------mjw-*/
void wint(char *b, int l)
{
  unsigned int a,num;
  num = sscanf(b, "%x", &a);

  if (num != 1) {
    printf("wint usage:  wint v\n");
    printf("  where v = 0 is don't wait for ints\n");
    printf("        v = n (vme interrupt) is wait for ints before read\n");  
  } else {
    ioctl(vme_handle,IOCTL_SET_WINT,a);    // Set WINT flag
  }     
}  

/*------------------------------------------------------------------------------
 * WriteReg
 *
 *------------------------------------------------------------------------mjw-*/
void WriteReg(char *b,int l)
{
  unsigned int num,value;
  long reg;

  num = sscanf(b,"%lx %x",&reg,&value);

  if (num != 2) {
    printf("REGW usage:  REGW register value\n");    
  } else {
    printf("Universe Reg Assigned %04lX = %08X\n",reg,value);  
    WriteUniReg(reg,value);
  }
}                                                                               

/*------------------------------------------------------------------------------
 * ReadReg
 *
 *------------------------------------------------------------------------mjw-*/
void ReadReg(char *b,int l)
{
  unsigned int num,value;
  long reg;

  num = sscanf(b,"%lx",&reg);

  if (num != 1) {
    printf("REGR usage:  REGW register\n");
  } else {
    value = ReadUniReg(reg);
    printf("Universe Reg %04lX = %08X\n",reg,value);  
  }
}                                                                               

/*------------------------------------------------------------------------------
 * ReadFile
 *
 *------------------------------------------------------------------------mjw-*/
void ReadFile(char *b,int l)
{
  int num,c,n;
  long pnt,len,i = 0;
  char fname[255];
  FILE *fout;
  int error;
  unsigned int *dma_buffer;

  num = sscanf(b,"%lx %li %s",&pnt,&len,fname);

  pnt += FixedAdder;

  if (num != 3) {
    printf("Usage: ReadFile address len filename");
  } else {
    if (ValidAddress(pnt)) {
      fout = fopen(fname,"wb");
      if (!fout) {
        printf("Can't open file %s\n",fname);
      } else {
        if (default_mode == MODE_PROGRAMMED) {
          // Do it via PROGRAMMED IO
          while (i++ < len) {
            c = rb(pnt++,&error);
            if (error) {
              printf("Bus Error during Readfile. File may be truncated.\n");
              len = 0;
            } else
              putc(c,fout);
          }
          // VIA DMA
        } else {
          if (!quiet)
            printf("--> Doing ReadFile via DMA <--\n");
          dma_buffer = (unsigned int *)malloc(len);
          // Do The DMA Transfer
          ioctl(vme_handle,IOCTL_SET_MODE,MODE_DMA);
          lseek(vme_handle,pnt,SEEK_SET);
          n = read(vme_handle,dma_buffer,len);
          ioctl(vme_handle,IOCTL_SET_MODE,MODE_PROGRAMMED);
          if (n < 1) {
            printf("BUS Error during DMA Transfer\n");
          } else {
            while (i++ < len)
              putc(dma_buffer[i],fout);
          }
          free(dma_buffer);
        }    

        fclose(fout);
        if (!quiet)
          printf("Done Reading Memory to a File\n");
      } 
    }
  }  
}

/*------------------------------------------------------------------------------
 * TestDMA
 *
 *------------------------------------------------------------------------mjw-*/
/*
void TestDMA(char *b,int l)
{
  int num;
  unsigned int pci_pnt,vme_pnt,len, val, temp;
  char testme[] = 0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
      0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27;
      
  num = sscanf(b,"%lx %lx %lx",&pci_pnt,&vme_pnt,&len);

  if (num == 0) {
    pci_pnt = 0;
    vme_pnt = 0xa5000000;
    len     = 16;
    num     = 3;
  }
  
  if (num != 3) {
    printf("Usage: TestDMA pciaddr vmeaddr len");
  } else {

    val = ReadUniReg(PCI_CSR);    // Lets Clear any error flags
    WriteUniReg(PCI_CSR,val | 0xF8000000);
  
    WriteUniReg(DCTL,0x80820100);  // Setup Control Reg
    WriteUniReg(DTBC,len);         // Setup Length
    WriteUniReg(DLA,pci_pnt);      // Setup PCI Address
    WriteUniReg(DVA,vme_pnt);      // Setup VME Address
    WriteUniReg(DGCS,0x80006F00);  // GO
 
    val = ReadUniReg(DGCS); 
  
    printf("DMA General Control Reg Value   = %08X\n",val);
    temp = val & 0x00008000;
    if (temp)
      printf("    Active\n");
    temp = val & 0x00004000;
    if (temp)
      printf("    Stopped Flag\n");
    temp = val & 0x00002000;
    if (temp)
      printf("    Halted Flag\n");
    temp = val & 0x00000800;
    if (temp)
      printf("    Transfers Complete\n");
    temp = val & 0x00000400;
    if (temp)
      printf("    PCI Bus Error\n");
    temp = val & 0x00000200;
    if (temp)
      printf("    VME Bus Error\n");
    temp = val & 0x00000100;
    if (temp)
      printf("    Protocol Error\n");
      
    val = ReadUniReg(PCI_CSR); 
  
    printf("PCI Configuration Space Control = %08X\n",val);
    temp = val & 0x80000000;
    if (temp)
      printf("    Parity Error\n");
    temp = val & 0x40000000;
    if (temp)
      printf("    Signalled SERR#\n");
    temp = val & 0x20000000;
    if (temp)
      printf("    Received Master-Abort\n");
    temp = val & 0x10000000;
    if (temp)
      printf("    Reveiced Target-Abort\n");
    temp = val & 0x08000000;
    if (temp)
      printf("    Signalled Target-Abort\n");
  }  
}
*/

/*------------------------------------------------------------------------------
 * TestDMA
 *
 *------------------------------------------------------------------------mjw-*/
void TestDMA(char *b,int l)
{
  int num;
  unsigned int vme_pnt,len,n,x;
  char buf[16];

  num = sscanf(b,"%x %x",&vme_pnt,&len);

  if (num == 0) {
    vme_pnt = 0x8000;
    len     = 16;
    num     = 2;
  }

  if (num != 2) {
    printf("Usage: TestDMA offset len");
    printf("  This will read len bytes via DMA from address offset");
  } else {

    printf("Read Test.");
    ioctl(vme_handle,IOCTL_SET_MODE,MODE_DMA);
    lseek(vme_handle,vme_pnt,SEEK_SET);
    n = read(vme_handle,buf,len);
    ioctl(vme_handle,IOCTL_SET_MODE,MODE_PROGRAMMED);

    for (x=0;x<n;x++) {
      if (x % 16 == 0)
        printf("%08X: ",x);
      if (x % 16 == 8)
        printf("- ");
      printf("%02X ",(unsigned char)buf[x]);
      if (x % 16 == 15)
        printf("\n");
    }

//    printf("Write Test.");
//    ioctl(vme_handle,IOCTL_SET_MODE,MODE_DMA);
//    lseek(vme_handle,vme_pnt,SEEK_SET);
//    write(vme_handle,buf,len);
//    ioctl(vme_handle,IOCTL_SET_MODE,MODE_PROGRAMMED);
  }
}

/*------------------------------------------------------------------------------
 * TestProgrammed
 *
 *------------------------------------------------------------------------mjw-*/
void TestProgrammed(char *b,int l)
{
  int num;
  unsigned int vme_pnt,len, n, x;
  char buf[16];

  num = sscanf(b,"%x %x",&vme_pnt,&len);

  if (num == 0) {
    vme_pnt = 0x8000;
    len     = 16;
    num     = 2;
  }

  if (num != 2) {
    printf("Usage: TestDMA offset len");
    printf("  This will read len bytes via DMA from address offset");
  } else {

    printf("Read Test.");
    ioctl(vme_handle,IOCTL_SET_MODE,MODE_PROGRAMMED);
    lseek(vme_handle,vme_pnt,SEEK_SET);
    n = read(vme_handle,buf,len);

    for (x=0;x<n;x++) {
      if (x % 16 == 0)
        printf("%08X: ",x);
      if (x % 16 == 8)
        printf("- ");
      printf("%02X ",(unsigned char)buf[x]);
      if (x % 16 == 15)
        printf("\n");
    }

    printf("Write Test.");
    ioctl(vme_handle,IOCTL_SET_MODE,MODE_PROGRAMMED);
    lseek(vme_handle,vme_pnt,SEEK_SET);
    write(vme_handle,buf,len);
  }
}

/*------------------------------------------------------------------------------
 * TestRead 
 *
 *------------------------------------------------------------------------mjw-*/
void TestRead(char *b,int l)
{
  unsigned int pnt,len,x,n,num;
  unsigned char block[1024];

  num = sscanf(b,"%x %x",&pnt,&len);

  if (num != 2) {
    len = 16;
    pnt = 0;
  }

  pnt += FixedAdder;

  if (ValidAddress(pnt)) {
    lseek(vme_handle,pnt,SEEK_SET);
    n = read(vme_handle,block,len);

    if (n) {
      for (x=0;x<n;x++) {
        if (x % 16 == 0)
          printf("%08X: ",x+pnt);
        if (x % 16 == 8)
          printf("- ");
        printf("%02X ",block[x]);
        if (x % 16 == 15)
          printf("\n");
      }
    } else {
      printf(" Read Failed.\n");
    }  

    printf("Test Read Done..\n");
  }
}

/*------------------------------------------------------------------------------
 * TestWrite 
 *
 *------------------------------------------------------------------------mjw-*/
void TestWrite(char *b,int l)
{
  long pnt,num,x;         
  int error;

  num = sscanf(b,"%lx",&pnt);

  if (num != 1) {
    pnt = 0;
  }

  pnt += FixedAdder;

  if (ValidAddress(pnt)) {
    for (x=0;x<16;x++) {
      wb(pnt+x,0xAB,&error);
    }  

    printf("Test Write Done..\n");
  }
}

/*-----------------------------------------------------------------------------
 * regs 
 *
 *------------------------------------------------------------------------mjw-*/
void regs(char *b,int l)
{
  unsigned int val;
  unsigned int temp;

  printf("Registers:\n");

  val = ReadUniReg(LMISC);
  printf("  LMISC    = %08X\n",val);
  temp = (val & 0xF0000000) >> 28;
  if (temp == 0)
    printf("    CRT    = Disabled\n");
  else if (temp == 1)
    printf("    CRT    = 128us\n");
  else if (temp == 2)
    printf("    CRT    = 256us\n");
  else if (temp == 3)
    printf("    CRT    = 512us\n");
  else if (temp == 4)
    printf("    CRT    = 1024us\n");
  else if (temp == 5)
    printf("    CRT    = 2048us\n");
  else if (temp == 6)
    printf("    CRT    = 4096us\n");
  else if (temp == 7)
    printf("    CRT    = Reserved\n");

  temp = (val & 0x0F000000) >> 24;
  if (temp == 0)
    printf("    CWT    = Disabled\n");
  else if (temp == 1)
    printf("    CWT    = 16 PCI Clocks\n");
  else if (temp == 2)
    printf("    CWT    = 32 PCI Clocks\n");
  else if (temp == 3)
    printf("    CWT    = 64 PCI Clocks\n");
  else if (temp == 4)
    printf("    CWT    = 128 PCI Clocks\n");
  else if (temp == 5)
    printf("    CWT    = 256 PCI Clocks\n");
  else if (temp == 6)
    printf("    CWT    = 512 PCI Clocks\n");
  else if (temp == 7)
    printf("    CWT    = Reserved\n");

  val = ReadUniReg(LSI0_CTL);
  printf("  LSIO_CTL = %08X\n",val);
  temp = (val & 0x80000000) >> 31;
  if (temp == 0)
    printf("    EN     = Disabled\n");
  else if (temp == 1)
    printf("    EN     = Enabled\n");

  temp = val & 0x40000000;
  if (temp)
    printf("    PWEN   = Enabled\n");
  else
    printf("    PWEN   = Disabled\n");
  temp = (val & 0x00C00000) >> 22;
  if (temp == 0)
    printf("    VDW    = 8 bit\n");
  else if (temp == 1)
    printf("    VDW    = 16 Bit\n");
  else if (temp == 2)
    printf("    VDW    = 32 Bit\n");
  else if (temp == 3)
    printf("    VDW    = 64 Bit\n");

  temp = (val & 0x70000) >> 16;
  if (temp == 0)
    printf("    VAS    = A16\n");
  else if (temp == 1)
    printf("    VAS    = A24\n");
  else if (temp == 2)
    printf("    VAS    = A32\n");
  else if (temp == 3)
    printf("    VAS    = Reserved\n");
  else if (temp == 4)
    printf("    VAS    = Reserved\n");
  else if (temp == 5)
    printf("    VAS    = CR/CSR\n");
  else if (temp == 6)
    printf("    VAS    = User 1\n");
  else if (temp == 7)
    printf("    VAS    = User 2\n");

  temp = (val & 0xC000) >> 14;
  if (temp == 0)
    printf("    PGM    = Data\n");
  else if (temp == 1)
    printf("    PGM    = Program\n");
  else if (temp == 2)
    printf("    PGM    = Reserved\n");
  else if (temp == 3)
    printf("    PGM    = Reserved\n");

  temp = (val & 0x3000) >> 12;
  if (temp == 0)
    printf("    SUPER  = Non-Privileged\n");
  else if (temp == 1)
    printf("    SUPER  = Supervisor\n");

  temp = (val & 0x0100) >> 8;
  if (temp == 0)
    printf("    VCT    = Single Cycles Only\n");
  else if (temp == 1)
    printf("    VCT    = Block Transfers OK\n");

  temp = (val & 0x03);
  if (temp == 0)
    printf("    LAS    = PCI Bus Memory Space\n");
  else if (temp == 1)
    printf("    LAS    = PCI Bus I/O Space\n");
  else if (temp == 2)
    printf("    LAS    = PCI Bus Type 1 Config Space\n");
  else if (temp == 3)
    printf("    LAS    = Reserved\n");

  printf("  LSIO_BS  = %08X\n",ReadUniReg(LSI0_BS));
  printf("  LSIO_BD  = %08X\n",ReadUniReg(LSI0_BD));
  printf("  LSIO_TO  = %08X\n",ReadUniReg(LSI0_TO));
}  

/*------------------------------------------------------------------------------
 *  vmespace1
 *
 *------------------------------------------------------------------------mjw-*/
void vmespace1(char *b,int l)
{
  int num,temp;

  num = sscanf(b,"%x",&temp);

  if (num == 0) {
    printf("  VME Address Space: ");
    if (VMEspace == 1) {
      printf("A16\n");
    } else if (VMEspace == 2) {
      printf("A24\n");
    } else if (VMEspace == 3) {
      printf("A32\n");
    }
  } else if (num == 1) {
    if (temp == 1) {
      printf("Setting to A16\n");
//      vmespace(VME_A16);
    } else if (temp == 2) {
      printf("Setting to A24\n");
//      vmespace(VME_A24);
    } else if (temp == 3) {
      printf("Setting to A32\n");
//      vmespace(VME_A32);
    } else
      printf("Invalid Value for VMEspace\n");
  } else {
    printf("\nUsage: vmespace space\n");
    printf("  where if space = 1, VME Address Space = A16\n");
    printf("  where if space = 2, VME Address Space = A24\n");
    printf("  where if space = 3, VME Address Space = A32\n");
  }  
}

/*------------------------------------------------------------------------------
 * vmemap1 
 *
 *------------------------------------------------------------------------mjw-*/
void vmemap1(char *b,int l)
{
  int num;
  long temp,count,space,size,type;
  char vmespc = 0;

  num = sscanf(b,"%lx %lx %lx %lx %lx",&temp,&count,&space,&size,&type);

  if (num != 5) {
    printf("Usage: map address count space size type\n");
    printf("  where address is VME Address to set Universe image to\n");
    printf("    Space = 0 CR/CSR    Space = 1 A16\n");
    printf("    Space = 2 A24       Space = 3 A32\n\n");
    printf("    Size  = 1 8 bit     Size  = 2 16 bit\n");
    printf("    Size  = 3 32 bit    Size  = 4 64 bit\n\n");
    printf("    Type  = 0 USR/DATA  Type  = 1 USR/PRG\n");
    printf("    Type  = 2 SUP/DATA  Type  = 3 SUP/PRG\n");
  } else {
    if (!quiet)
      printf("Setting PCI Slave Image to address %08lX\n",temp);

    // VME Size
    if (size == 1) {
      vmespc = VME_SIZE_8;         
    } else if (size == 2) {
      vmespc = VME_SIZE_16;        
    } else if (size == 3) {
      vmespc = VME_SIZE_32;        
    } else if (size == 4) {
      vmespc = VME_SIZE_64;        
    } else
      printf(" Bad Size\n");
    FixedAdder = temp;                           

    vmespc |= space;

    switch (type) {
    case 0:
      vmespc &= ~VME_SUP_USR;
      vmespc &= ~VME_PRG_DATA;
      break;
    case 1:
      vmespc &= ~VME_SUP_USR;
      vmespc |= VME_PRG_DATA;
      break;
    case 2:
      vmespc |= VME_SUP_USR;
      vmespc &= ~VME_PRG_DATA;
      break;
    case 3:
      vmespc |= VME_SUP_USR;
      vmespc |= VME_PRG_DATA;
      break;
    }
    VME_Valid_Base = temp;
    VME_Valid_Cnt  = count;
    vmemap(temp,count,vmespc);
  }
}

/*------------------------------------------------------------------------------
 * quit 
 *
 *------------------------------------------------------------------------mjw-*/
void quit(char *b,int l)
{
  shutdown_vmelib();
  if (!quiet)
    printf("Exiting\n\n");
  exit(0);
}

/*------------------------------------------------------------------------------
 * help
 *
 *------------------------------------------------------------------------mjw-*/
void help(char *b,int l)
{
  int x = 0;
  int done = 0;

  printf("Commands:\n"); 
  while (!done) {
    printf("%10s ",cmdTable[x].command);
    if ((x+1) % 6 == 0)
      printf("\n");
    if (cmdTable[x++].CmdType == CMDT_NULL)
      done = 1;
  } // While
}

/*------------------------------------------------------------------------------
 * status 
 *
 *------------------------------------------------------------------------mjw-*/
void status(char *b,int l)
{
  printf("Program Status:\n");
  printf("  Fixed Adder: %08lX\n",FixedAdder);
}

/*-----------------------------------------------------------------------------
 *  ProcessCommand
 *
 *------------------------------------------------------------------------mjw-*/
void ProcessCommand(char *buffer,int len)
{
  int done = 0;
  int cmdi = 0;
  COMMAND *cpnt;
  char *cmd;
  char *params = 0;
  int x,plen = 0;
  long temp;

  cmd = buffer;
  for (x=0;x < len;x++) {
    if (buffer[x] == ' ') {
      buffer[x] = 0;             // NULL Term the string
      params = &buffer[x+ 1];
      plen = len - x;
      break;
    }
  } 

  while (!done) {
    if (!strcmp(cmd,cmdTable[cmdi].command)) {
      done = 1;
      cpnt = &cmdTable[cmdi];
      switch (cpnt->CmdType) {
      case CMDT_NULL:
        break;
      case CMDT_FUNC:
        func(cpnt->pointer)(params,plen);
        break;
      case CMDT_MODC:
        sscanf(params,"%li",&temp);
        *(char*)(cpnt->pointer) = temp;
        break;  
      case CMDT_MODI:
        sscanf(params,"%li",&temp);
        *(int*)(cpnt->pointer) = temp;
        break;  
      case CMDT_MODL:
        sscanf(params,"%lx",&temp);
        *(long*)(cpnt->pointer) = temp;
        break;  
      case CMDT_MODB:
        if (!strcmp(params,"ON")) {
          *(int*)(cpnt->pointer) = 1;
        } else if (!strcmp(params,"TRUE")) {
          *(int*)(cpnt->pointer) = 1;
        } else if (!strcmp(params,"OFF")) {
          *(int*)(cpnt->pointer) = 0;
        } else if (!strcmp(params,"FALSE")) {
          *(int*)(cpnt->pointer) = 0;
        } else {
          sscanf(params,"%li",&temp);
          *(int*)(cpnt->pointer) = temp;
        }  
        break;  
      } // switch
    }
    if (cmdTable[cmdi++].CmdType == CMDT_NULL) {
      done = 1;
      printf("Unknown Command\n");
    }
  } // While
}


