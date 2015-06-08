#pragma hdrstop
#include <condefs.h>
#include <stdio.h>
//---------------------------------------------------------------------------
#pragma argsused


/***************************************************************************/
/*  SRecord Loader for VME6U NOPI board.                                   */
/*    uses vme word writes and upper byte is 0 for DSP-2101 Boot code.     */
/*  MEMORY Tools for VME Testing of Dual Port Memory                       */
/*                                                                         */
/* 8/5/96 - Michael J. Wyrick Interferometrics, Inc.                       */
/*                                                                         */
/***************************************************************************/
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define ERROR 1
#define BUFFER_SIZE             4096
#define BYT(cp)                 ((Asc2Hex(cp[0]) << 4) | Asc2Hex(cp[1]))

static FILE *infile;
static char *buffer;
static char *goffset;

int size = 0;

unsigned char memory[10000];

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
void checkwrite(unsigned short *a,unsigned char val)
{ 
  memory[a] = val;
  size++;
}

/*-------------------------------------------------------------------------*/
/*  DecodeS1 - Decode and place in memory S1 type records                  */
/*     Checks for correct checksum value and will not write if incorrect   */
/*---------------------------------------------------------------------mjw-*/
static void DecodeS1(char *cp)
{
   unsigned short *address, val; 
   unsigned char *datastart, cnt, cntr, chksum;
   unsigned long addr;

   cnt = BYT(cp) - 3;          /* Get Length of Line */
   chksum = BYT(cp);
   cp += 2;

   addr = ComputeAddr(cp,2) << 1;
   printf("Address %X\n",addr);
   address = (unsigned short *)(addr + (unsigned long) goffset);

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
   for (cntr = 0; cntr < cnt; cntr++, datastart+=2, address++) {
      val = BYT(datastart);
      checkwrite(address,val);
   }
}

/*-------------------------------------------------------------------------*/
/*  dsp_srec                                                               */
/*      int srec(char *filename, char *offset)                             */
/*---------------------------------------------------------------------mjw-*/
int dsp_srec(char *filename, char *offset)
{
   char *cp;
   int flag, pcount;
   int count = 0;
   int errcount = 0;
   int scount[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

   printf("\nLinux DSP-21xx VME Word Write S-record Loader, Ver 0.1\n\n");
   buffer = (char *)malloc(BUFFER_SIZE);
   if (buffer == (char *) 0) {
      printf("\n\nLoad error: unable to get 4096 bytes of memory.\n\n\n");
      return(ERROR);
   }
   infile = fopen(filename, "r");
   if (infile == (FILE *) 0) {
      printf("\n\nLoad error: unable to open file [%#X]\n\n\n");
      free(buffer);
      return(ERROR);
   }

   goffset = offset;
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
                  DecodeS1(++cp);
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

//-----------------------------------------------------------------------------
//
//
//
//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  if (argc == 3)
    dsp_srec(argv[1],0);  // No offset yet
  else
    printf("  usage:  upload filename offset\n\n");
}
 