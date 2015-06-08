/*
  tidEMload.c
   - "Emergency" JTAG loading via VME for the TID
*/


#include <stdio.h>
#include <string.h>
#include "jvme.h"
#include "tidLib.h"

extern unsigned int tidRead(volatile unsigned int *addr);
extern void tidWrite(volatile unsigned int *addr, unsigned int val);
extern volatile struct TID_A24RegStruct *TIDp;

void EMload(char *filename);

int
main(int argc, char *argv[])
{
  int stat;
  char *filename;
  
  printf("\nTID Emergency JTAG Loading via VME\n");
  printf("----------------------------\n");

  if(argc<2)
    {
      filename = "tids.svf";
    }
  else 
    {
      filename = argv[1];
    }

  printf("\n svg filename to be used = %s\n",filename);

  stat = vmeOpenDefaultWindows();
  if(stat != OK)
    goto CLOSE;

  tidInit((21<<19),0,0,0);


  printf("Press emergency load the JTAG to the TID via VME.\n");
  getchar();
  EMload(filename);

 CLOSE:
  vmeCloseDefaultWindows();

  return OK;
}

void 
Emergency(unsigned int jtagType, unsigned int numBits, unsigned long *jtagData)
{
/*   unsigned long *laddr; */
  unsigned int iloop, iword, ibit;
  unsigned long shData;

  /*  int numWord, i;
      printf("type: %x, num of Bits: %x, data: \n",jtagType, numBits);
      numWord = (numBits-1)/32+1;
      for (i=0; i<numWord; i++)
      {
      printf("%08x",jtagData[numWord-i-1]);
      }
      printf("\n");
  */

  // A24 buffer write
  //  sysBusToLocalAdrs(0x39,TID_REG_ADDR+0x0FFFC,&laddr);	//Emergency A24 address

  //  printf(" \n laddr is %08x\n",laddr);

/*   laddr = 0x90a8fffc;  // for MVME6100 controller */
  //  printf("\n laddr is redefined as %08x \n",laddr);

  if (jtagType == 0) //JTAG reset, TMS high for 5 clcoks, and low for 1 clock;
    {
      for (iloop=0; iloop<5; iloop++)
	{
	  tidWrite(&TIDp->eJTAGLoad,1);
/* 	  *laddr = 1; */
	}
      tidWrite(&TIDp->eJTAGLoad,0);
/*       *laddr = 0; */
    }
  else if (jtagType == 1) // JTAG instruction shift
    {
      // Shift_IR header:
      tidWrite(&TIDp->eJTAGLoad,0);
      tidWrite(&TIDp->eJTAGLoad,1);
      tidWrite(&TIDp->eJTAGLoad,1);
      tidWrite(&TIDp->eJTAGLoad,0);
      tidWrite(&TIDp->eJTAGLoad,0);
/*       *laddr = 0; */
/*       *laddr = 1; */
/*       *laddr = 1; */
/*       *laddr = 0; */
/*       *laddr = 0; */
      for (iloop =0; iloop <numBits; iloop++)
	{ 
	  iword = iloop/32;
	  ibit = iloop%32;
	  shData = ((jtagData[iword] >> ibit )<<1) &0x2;
	  if (iloop == numBits -1) shData = shData +1;  //set the TMS high for last bit to exit Shift_IR
	  tidWrite(&TIDp->eJTAGLoad, shData);
/* 	  *laddr = shData; */
	}
      // shift _IR tail
      tidWrite(&TIDp->eJTAGLoad,1);
      tidWrite(&TIDp->eJTAGLoad,0);
/*       *laddr = 1;  // update instruction register */
/*       *laddr = 0;  // back to the Run_test/Idle */
    }
  else if (jtagType == 2)  // JTAG data shift
    {
      //shift_DR header
      tidWrite(&TIDp->eJTAGLoad,0);
      tidWrite(&TIDp->eJTAGLoad,1);
      tidWrite(&TIDp->eJTAGLoad,0);
      tidWrite(&TIDp->eJTAGLoad,0);
/*       *laddr = 0; */
/*       *laddr = 1; */
/*       *laddr = 0; */
/*       *laddr = 0; */
      for (iloop =0; iloop <numBits; iloop++)
	{ 
	  iword = iloop/32;
	  ibit = iloop%32;
	  shData = ((jtagData[iword] >> ibit )<<1) &0x2;
	  if (iloop == numBits -1) shData = shData +1;  //set the TMS high for last bit to exit Shift_DR
	  tidWrite(&TIDp->eJTAGLoad, shData);
/* 	  *laddr = shData; */
	}
      // shift _DR tail
      tidWrite(&TIDp->eJTAGLoad,1);
      tidWrite(&TIDp->eJTAGLoad,0);
/*       *laddr = 1;  // update Data_Register */
/*       *laddr = 0;  // back to the Run_test/Idle */
    }
  else if (jtagType == 3) // JTAG instruction shift, stop at IR-PAUSE state, though, it started from IDLE
    {
      // Shift_IR header:
      tidWrite(&TIDp->eJTAGLoad,0);
      tidWrite(&TIDp->eJTAGLoad,1);
      tidWrite(&TIDp->eJTAGLoad,1);
      tidWrite(&TIDp->eJTAGLoad,0);
      tidWrite(&TIDp->eJTAGLoad,0);
/*       *laddr = 0; */
/*       *laddr = 1; */
/*       *laddr = 1; */
/*       *laddr = 0; */
/*       *laddr = 0; */
      for (iloop =0; iloop <numBits; iloop++)
	{ 
	  iword = iloop/32;
	  ibit = iloop%32;
	  shData = ((jtagData[iword] >> ibit )<<1) &0x2;
	  if (iloop == numBits -1) shData = shData +1;  //set the TMS high for last bit to exit Shift_IR
	  tidWrite(&TIDp->eJTAGLoad, shData);
/* 	  *laddr = shData; */
	}
      // shift _IR tail
      tidWrite(&TIDp->eJTAGLoad,0);
      tidWrite(&TIDp->eJTAGLoad,0);
/*       *laddr = 0;  // update instruction register */
/*       *laddr = 0;  // back to the Run_test/Idle */
    }
  else if (jtagType == 4)  // JTAG data shift, start from IR-PAUSE, end at IDLE
    {
      //shift_DR header
      tidWrite(&TIDp->eJTAGLoad,1);
      tidWrite(&TIDp->eJTAGLoad,1);
      tidWrite(&TIDp->eJTAGLoad,1);
      tidWrite(&TIDp->eJTAGLoad,0);
      tidWrite(&TIDp->eJTAGLoad,0);
/*       *laddr = 1;  //to EXIT2_IR */
/*       *laddr = 1;  //to UPDATE_IR */
/*       *laddr = 1;  //to SELECT-DR_SCAN */
/*       *laddr = 0; */
/*       *laddr = 0; */
      for (iloop =0; iloop <numBits; iloop++)
	{ 
	  iword = iloop/32;
	  ibit = iloop%32;
	  shData = ((jtagData[iword] >> ibit )<<1) &0x2;
	  if (iloop == numBits -1) shData = shData +1;  //set the TMS high for last bit to exit Shift_DR
	  tidWrite(&TIDp->eJTAGLoad, shData);
/* 	  *laddr = shData; */
	}
      // shift _DR tail
      tidWrite(&TIDp->eJTAGLoad,1);
      tidWrite(&TIDp->eJTAGLoad,0);
/*       *laddr = 1;  // update Data_Register */
/*       *laddr = 0;  // back to the Run_test/Idle */
    }
  else
    {
      printf( "\n JTAG type %d unrecognized \n",jtagType);
    }

  //  printf (" \n Emergency command executed \n");
}

void 
Parse(char *buf,int *Count,char **Word)
{
  *Word = buf;
  *Count = 0;
  while(*buf != '\0')  
    {
      while ((*buf==' ') || (*buf=='\t') || (*buf=='\n') || (*buf=='"')) *(buf++)='\0';
      if ((*buf != '\n') && (*buf != '\0'))  
	{
	  Word[(*Count)++] = buf;
	}
      while ((*buf!=' ')&&(*buf!='\0')&&(*buf!='\n')&&(*buf!='\t')&&(*buf!='"')) 
	{
	  buf++;
	}
    }
  *buf = '\0';
}

void 
EMload(char *filename)
{
  unsigned long ShiftData[64], lineRead;
  unsigned int jtagType, jtagBit, iloop;
  FILE *svfFile;
  int byteRead;
  char bufRead[1024],bufRead2[256];
  unsigned int sndData[256];
  char *Word[16], *lastn;
  unsigned int nbits, nbytes, extrType, i, Count, nWords, nlines;

  //A24 Address modifier redefined
#ifdef VXWORKS
  sysTempeSetAM(2,0x19);
#else
  vmeSetA24AM(0x19);
#endif
  printf("%s: A24 memory map is set to AM = 0x19 \n",__FUNCTION__);

  //open the file:
  svfFile = fopen(filename,"r");
  if(svfFile==NULL)
    {
      perror("fopen");
      printf("%s: ERROR: Unable to open file %s\n",__FUNCTION__,filename);
      return;
    }
  printf("\n File is open \n");

  //PROM JTAG reset/Idle
  Emergency(0,0,ShiftData);
  printf("%s: Emergency PROM JTAG reset IDLE \n",__FUNCTION__);


  //initialization
  extrType = 0;
  lineRead=0;

  //  for (nlines=0; nlines<200; nlines++)
  while (fgets(bufRead,256,svfFile) != NULL)
    { 
      lineRead +=1;
      if (lineRead%1000 ==0) printf(" Lines read: %d out of 787000 \n",lineRead);
      //    fgets(bufRead,256,svfFile);
      if (((bufRead[0] == '/')&&(bufRead[1] == '/')) || (bufRead[0] == '!'))
	{
	  //	printf(" comment lines: %c%c \n",bufRead[0],bufRead[1]);
	}
      else
	{
	  if (strrchr(bufRead,';') ==0) 
	    {
	      do 
		{
		  lastn =strrchr(bufRead,'\n');
		  if (lastn !=0) lastn[0]='\0';
		  if (fgets(bufRead2,256,svfFile) != NULL)
		    {
		      strcat(bufRead,bufRead2);
		    }
		  else
		    {
		      printf("\n \n  !!! End of file Reached !!! \n \n");
		      return;
		    }
		} 
	      while (strrchr(bufRead,';') == 0);  //do while loop
	    }  //end of if
	
	  // begin to parse the data bufRead
	  Parse(bufRead,&Count,&(Word[0]));
	  if (strcmp(Word[0],"SDR") == 0)
	    {
	      sscanf(Word[1],"%d",&nbits);
	      nbytes = (nbits-1)/8+1;
	      if (strcmp(Word[2],"TDI") == 0)
		{
		  for (i=0; i<nbytes; i++)
		    {
		      sscanf (&Word[3][2*(nbytes-i-1)+1],"%2x",&sndData[i]);
		      //  printf("Word: %c%c, data: %x \n",Word[3][2*(nbytes-i)-1],Word[3][2*(nbytes-i)],sndData[i]);
		    }
		  nWords = (nbits-1)/32+1;
		  for (i=0; i<nWords; i++)
		    {
		      ShiftData[i] = ((sndData[i*4+3]<<24)&0xff000000) + ((sndData[i*4+2]<<16)&0xff0000) + ((sndData[i*4+1]<<8)&0xff00) + (sndData[i*4]&0xff);
		    }
		  //printf("Word[3]: %s \n",Word[3]);
		  //printf("sndData: %2x %2x %2x %2x, ShiftData: %08x \n",sndData[3],sndData[2],sndData[1],sndData[0], ShiftData[0]);
		  Emergency(2+extrType,nbits,ShiftData);
		}
	    }
	  else if (strcmp(Word[0],"SIR") == 0)
	    {
	      sscanf(Word[1],"%d",&nbits);
	      nbytes = (nbits-1)/8+1;
	      if (strcmp(Word[2],"TDI") == 0)
		{
		  for (i=0; i<nbytes; i++)
		    {
		      sscanf (&Word[3][2*(nbytes-i)-1],"%2x",&sndData[i]);
		      //  printf("Word: %c%c, data: %x \n",Word[3][2*(nbytes-i)-1],Word[3][2*(nbytes-i)],sndData[i]);
		    }
		  nWords = (nbits-1)/32+1;
		  for (i=0; i<nWords; i++)
		    {
		      ShiftData[i] = ((sndData[i*4+3]<<24)&0xff000000) + ((sndData[i*4+2]<<16)&0xff0000) + ((sndData[i*4+1]<<8)&0xff00) + (sndData[i*4]&0xff);
		    }
		  //printf("Word[3]: %s \n",Word[3]);
		  //printf("sndData: %2x %2x %2x %2x, ShiftData: %08x \n",sndData[3],sndData[2],sndData[1],sndData[0], ShiftData[0]);
		  Emergency(1+extrType,nbits,ShiftData);
		}
	    }
	  else if (strcmp(Word[0],"RUNTEST") == 0)
	    {
	      sscanf(Word[1],"%d",&nbits);
	      //	    printf("RUNTEST delay: %d \n",nbits);
/* 	      cpuDelay(nbits*45);   //delay, assuming that the CPU is at 45 MHz */
	      int time = (nbits/1000)+1;
	      taskDelay(time);   //delay, assuming that the CPU is at 45 MHz
	    }
	  else if (strcmp(Word[0],"STATE") == 0)
	    {
	      if (strcmp(Word[1],"RESET") == 0) Emergency(0,0,ShiftData);
	    }
	  else if (strcmp(Word[0],"ENDIR") == 0)
	    {
	      if (strcmp(Word[1],"IDLE") ==0 )
		{
		  extrType = 0;
		  printf(" ExtraType: %d \n",extrType);
		}
	      else if (strcmp(Word[1],"IRPAUSE") ==0)
		{
		  extrType = 2;
		  printf(" ExtraType: %d \n",extrType);
		}
	      else
		{
		  printf(" Unknown ENDIR type %s\n",Word[1]);
		}
	    }
	  else
	    {
	      printf(" Command type ignored: %s \n",Word[0]);
	    }

	}  //end of if (comment statement)
    } //end of while

  //close the file
  fclose(svfFile);

  // A24 address modifier reset
#ifdef VXWORKS
  sysTempeSetAM(2,0);
#else
  vmeSetA24AM(0);
#endif
  printf("\n A24 memory map is set back to its default \n");
}


