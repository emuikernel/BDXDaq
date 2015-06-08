h11775
s 00365/00291/00312
d D 1.2 04/03/04 17:49:56 boiarino 3 1
c new function l2_mlu_setup() - the only one we need
e
s 00000/00000/00000
d R 1.2 02/08/27 00:39:50 Codemgr 2 1
c SunPro Code Manager data about conflicts, renames, etc...
c Name history : 1 0 vxworks/level2/level2.c
e
s 00603/00000/00000
d D 1.1 02/08/27 00:39:49 boiarino 1 0
c date and time created 02/08/27 00:39:49 by boiarino
e
u
U
f e 0
t
T
I 1

D 3
/* level2.c - Sergey Boyarinov from Kelly's logic2_11.c and majority6_28.c */
E 3
I 3
/* level2.c - this code was designed to configure a Level 2 Majority Logic Unit  */
/*            (MLU) through remote control */
/*    modified by Sergey Boyarinov from Kelly Teague's logic2_11.c and majority6_28.c */
E 3

D 3

/* Kelly Teague */
/* logic2_11.c */
/*  This code was designed to configure a Level 2 Majority Logic Unit  */
/*  (MLU) through remote control. A user inputs select and opposite    */
/*  preferences for each sector, and they are then concatenated into a */
/*  4 bit control word. Data transfer occurs through file manipulation */
/*  Once opened, the file is scanned and the data is read from it.  As */
/*  the  data is read, it is sent to a VME CPU, which is inside the    */
/*  crate that houses the MLUs.  Note that this code has been written  */
/*  to configure, in order, all 6 MLUs needed for the Level 2 trigger  */ 


/* Kelly Teague, ext. 6056 as jul-30-2002 */
/* majority6_28.c */
/*  This code was designed to configure a Level 2 Majority Logic Unit  */
/*  (MLU) through remote control.  When testing a MLU in remote mode,  */
/*  it is necessary to firstconfigure a flex I/O unit to read back the */ 
/*  output produced by the MLU, and then each MLU. This is required    */ 
/*  because the control word is no longer selected through manual      */
/*  switches as in local mode.  A user inputs select line and opposite */
/*  preferences for each sector, and they are then concatenated into a */
/*  4 bit control word. Data transfer occurs through file manipulation */
/*  A user writes data to a filename which is later opened.  Once      */
/*  opened, the file is scanned and the data is read from it.  As the  */
/*  data is read, it is sent to a VME CPU, which is inside the crate   */
/*  that houses the MLUs.  			                       */


E 3
#include <stdio.h>
#include <stdlib.h>
#include <usrLib.h>
#include "level2.h"

struct majority_struct *Sectors1_2;
struct majority_struct *Sectors3_4;
struct majority_struct *Sectors5_6;
struct majority_struct *VMEflex_io;

I 3














E 3
/*********************************************************************/
I 3
/***** ????????????????????????????????????????????????????????? *****/
E 3

D 3
int
Check_Sector(int Sector)   
E 3
I 3
void
CheckPort1_Input(struct majority_struct *VMEflex_io)
E 3
{
D 3
  if((Sector==1)||(Sector==2)||(Sector==3)||
     (Sector==4)||(Sector==5)||(Sector==6))   
    Assign_OutputBoard(Sector);
E 3
I 3
  if((VMEflex_io->csr1 & 0xC0) != 0x80)
  {
    puts("\nchecking the VME module for input type");	
    puts("\n--- Port 1 Card is NOT of type input ---\n");
    exit(0);	
  }
E 3
  else
D 3
    exit(0); 
 
E 3
I 3
  { 
    return;
  }

E 3
}

/*********************************************************************/

long
D 3
Assign_OutputBoard(int Sector)
E 3
I 3
Assign_VMEreadback()
E 3
{
D 3
  long SectorAddress;
E 3
I 3
  unsigned long ppc_offset;
E 3

D 3
  if((Sector == 1) || (Sector == 2))
  {
    /*printf("\nThe Sector assignment inside Assign_OutputBoard is: %d",Sector);*/
    SectorAddress = 0xFFFF6100;
    Sectors1_2 = (struct majority_struct*) SectorAddress;  
  }
  else if((Sector == 3) || (Sector == 4))
  {
    /*printf("\nThe Sector assignment inside Assign_OutputBoard is:  %d",Sector);*/
    SectorAddress = 0xFFFF6200;
    Sectors3_4 = (struct majority_struct*)SectorAddress; 
  }
  else if((Sector == 5) || (Sector == 6))
  {
    /*printf("\nThe Sector assignment inside Assign_OutputBoard is: %d",Sector);*/
    SectorAddress = 0xFFFF6300;
    Sectors5_6 = (struct majority_struct *)SectorAddress; 
  }
E 3
I 3
  sysBusToLocalAdrs(0x29,0,&ppc_offset);
E 3

D 3
  CheckSector_Output(Sectors1_2, Sectors3_4, Sectors5_6); 
E 3
I 3
  VMEflex_io = (struct majority_struct *) (ppc_offset+0x8100);/*0xFFFF8100*/
  printf("VMEflex_io -> 0x%08x\n",VMEflex_io);
  CheckPort1_Input(VMEflex_io);
E 3

}

I 3

E 3
/*********************************************************************/

D 3
void
CheckSector_Output(struct majority_struct *Sectors1_2,
                   struct majority_struct *Sectors3_4,
                   struct majority_struct *Sectors5_6)
E 3
I 3
int
VMEReadOutput(struct majority_struct *VMEflex_io)
E 3
{
D 3
  if((Sectors1_2->csr1 & 0xC0) != 0x40)
  {
    printf("\nSector 1CSR is %x", Sectors1_2->csr1& 0xC0);
    puts("\n--- Port 1 Card is NOT of type output ---\n");
    exit(0);
  }
  else
  {
    return;
  }
E 3
I 3
  int Level2_Or;
E 3

D 3
  if((Sectors1_2->csr2 & 0xC0) != 0x40)
  {
    printf("\nSector 2CSR is %x", Sectors1_2->csr2& 0xC0);
    puts("\n--- Port 2 Card is NOT of type output ---\n");
    exit(0);	
  }
  else
  {
    return;
  }
E 3
I 3
  VMEflex_io -> port1_data = Level2_Or; 
E 3

D 3
  if((Sectors3_4->csr1 & 0xC0) != 0x40)
  {
    /*printf("\nSector 3CSR is %x", Sectors3_4->csr1& 0xC0);*/
    puts("\n--- Port 1 Card is NOT of type output ---\n");
    exit(0);
E 3
I 3
  printf("The final output is %hu", Level2_Or);

}



/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/












/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/

void
VME_ReadData(int *Data, int Sector, 
              struct majority_struct *Sectors1_2,
              struct majority_struct *Sectors3_4,
		      struct majority_struct *Sectors5_6)
{
  if(Sector == 1)
  { 	   
    *Data = Sectors1_2 -> port1_data;
    /*printf("data read from sector1 was 0x%X\n", *Data);*/
E 3
  }
D 3
  else
E 3
I 3
  else if(Sector == 2)
E 3
  {
D 3
    return;
E 3
I 3
    *Data = Sectors1_2 -> port2_data;
    /*printf("data read from sector2 was 0x%X\n", *Data);*/
E 3
  }
D 3

  if((Sectors3_4->csr2 & 0xC0) != 0x40)
E 3
I 3
  else if(Sector == 3)
E 3
  {
D 3
    /*printf("\nSector 4CSR is %x", Sectors3_4->csr2& 0xC0);*/
    puts("\n--- Port 2 Card is NOT of type output ---\n");
    exit(0);
E 3
I 3
    *Data = Sectors3_4 -> port1_data;
    /*printf("data read from sector3 was 0x%X\n", *Data);*/
E 3
  }
D 3
  else
  { 
    return;
  }

  if((Sectors5_6->csr1 & 0xC0) != 0x40)
E 3
I 3
  else if(Sector == 4)
E 3
  {
D 3
    /*printf("\nSector 5CSR is %x", Sectors5_6->csr1& 0xC0);*/
    puts("\n--- Port 1 Card is NOT of type output ---\n");
    exit(0);
E 3
I 3
    *Data = Sectors3_4 -> port2_data;
    /*printf("data read from sector4 was 0x%X\n", *Data);*/
E 3
  }
D 3
  else
E 3
I 3
  else if(Sector == 5)
E 3
  {
D 3
    return;
E 3
I 3
    *Data = Sectors5_6 -> port1_data;
    /*printf("data read from sector5 was 0x%X\n", *Data);*/
E 3
  }
D 3

  if((Sectors5_6->csr2 & 0xC0) != 0x40)
E 3
I 3
  else if(Sector == 6)
E 3
  {
D 3
    /*printf("\nSector 6CSR is %x", Sectors5_6->csr2& 0xC0);*/
    puts("\n--- Port 2 Card is NOT of type output ---\n");
    exit(0);
E 3
I 3
    *Data = Sectors5_6 -> port2_data;
    /*printf("data read from sector6 was 0x%X\n", *Data);*/
E 3
  }
D 3
  else
E 3
I 3
  else if((Sector != 1) || (Sector != 2) || (Sector != 3) ||
  	      (Sector != 4) || (Sector != 5) || (Sector != 6))
E 3
  {
D 3
    return;
E 3
I 3
    puts("\n--- Data Error ---\n");
    puts("Invalid Sector Assignment");
E 3
  }

}

/*********************************************************************/

D 3
int
Make_DataWord(int Sector, int Select, int Opposite)
E 3
I 3
void
Decode_DataWord(int Sector, int Data, int *Select, int *Opposite)
E 3
{
D 3
  int Data;
E 3
I 3
  *Select = Data & 0x7;
  *Opposite = (Data>>3) & 0x1;
E 3

D 3
  Data = ((Opposite << 3) | Select);	

  return(Data);
E 3
I 3
  return;
E 3
}

/*********************************************************************/

int
D 3
Write_Data_call(int Sector, int Select, int Opposite)
E 3
I 3
Read_Data_call(int Sector, int *Select, int *Opposite)
E 3
{
  int Data;

D 3
  Data = Make_DataWord(Sector, Select, Opposite);
  VME_WriteData(Data, Sector, Sectors1_2, Sectors3_4, Sectors5_6);
E 3
I 3
  VME_ReadData(&Data, Sector, Sectors1_2, Sectors3_4, Sectors5_6);
  Decode_DataWord(Sector, Data, Select, Opposite);
E 3

}

I 3

E 3
/*********************************************************************/
I 3
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
E 3

void
D 3
VME_WriteData(int Data, int Sector, struct majority_struct *Sectors1_2,
E 3
I 3
VME_WriteData(int Data, int Sector, 
              struct majority_struct *Sectors1_2,
E 3
              struct majority_struct *Sectors3_4,
		      struct majority_struct *Sectors5_6)
{
  if(Sector == 1)
  { 	   
    Sectors1_2 -> port1_data = Data;
D 3
    printf("data sent to sector1 was 0x%X\n", Sectors1_2 -> port1_data = Data);
E 3
I 3
    /*printf("data sent to sector1 was 0x%X\n", Data);*/
E 3
  }
  else if(Sector == 2)
  {
    Sectors1_2 -> port2_data = Data;
D 3
    printf("data sent to sector2 was 0x%X\n", Sectors1_2 -> port2_data = Data);
E 3
I 3
    /*printf("data sent to sector2 was 0x%X\n", Data);*/
E 3
  }
  else if(Sector == 3)
  {
    Sectors3_4 -> port1_data = Data;
D 3
    printf("data sent to sector3 was 0x%X\n", Sectors3_4 -> port1_data = Data);
E 3
I 3
    /*printf("data sent to sector3 was 0x%X\n", Data);*/
E 3
  }
  else if(Sector == 4)
  {
    Sectors3_4 -> port2_data = Data;
D 3
    printf("data sent to sector4 was 0x%X\n", Sectors3_4 -> port2_data = Data);
E 3
I 3
    /*printf("data sent to sector4 was 0x%X\n", Data);*/
E 3
  }
  else if(Sector == 5)
  {
    Sectors5_6 -> port1_data = Data;
D 3
    printf("data sent to sector5 was 0x%X\n", Sectors5_6 -> port1_data = Data);
E 3
I 3
    /*printf("data sent to sector5 was 0x%X\n", Data);*/
E 3
  }
  else if(Sector == 6)
  {
    Sectors5_6 -> port2_data = Data;
D 3
    printf("data sent to sector6 was 0x%X\n", Sectors5_6 -> port1_data = Data);
E 3
I 3
    /*printf("data sent to sector6 was 0x%X\n", Data);*/
E 3
  }
  else if((Sector != 1) || (Sector != 2) || (Sector != 3) ||
  	         	(Sector != 4) || (Sector != 5) || (Sector != 6))
  {
    puts("\n--- Data Error ---\n");
    puts("Invalid Sector Assignment");
  }

}

/*********************************************************************/

D 3
void
char_clear(void)   
E 3
I 3
int
Make_DataWord(int Sector, int Select, int Opposite)
E 3
{
D 3
  char junk[80];
  gets(junk);
E 3
I 3
  int Data;

  Data = ((Opposite << 3) | Select);	

  return(Data);
E 3
}

/*********************************************************************/

int
D 3
Get_Choice(void)
E 3
I 3
Write_Data_call(int Sector, int Select, int Opposite)
E 3
{
D 3
  int selection = 0;
E 3
I 3
  int Data;
E 3

D 3
  do
  {
    puts("\n1 - Enter New Trigger");
    puts("2 - Load Trigger File");
    puts("3 - Save this File");
    puts("4 - Download to VME CPU");	
    puts("5 - Quit");	
    puts("\nChoose a trigger selection: ");
E 3
I 3
  Data = Make_DataWord(Sector, Select, Opposite);
  VME_WriteData(Data, Sector, Sectors1_2, Sectors3_4, Sectors5_6);
E 3

D 3
    scanf("%d", &selection);
    char_clear();
  }
  while ((selection < 1 || selection > 5));

  return selection;
E 3
}

I 3


E 3
/*********************************************************************/
I 3
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
/*********************************************************************/
E 3

D 3
int
Save_File(char *tempname, char *filename, FILE *fnew, FILE *ftemp)
E 3
I 3
void
CheckSector_Output(struct majority_struct *Sectors1_2,
                   struct majority_struct *Sectors3_4,
                   struct majority_struct *Sectors5_6)
E 3
{
D 3
  if(File_Copy(tempname, filename, fnew, ftemp) == 0)
E 3
I 3
  if((Sectors1_2->csr1 & 0xC0) != 0x40)
E 3
  {
D 3
    printf("\nFile saved as: %s", filename);
E 3
I 3
    printf("\nSector 1CSR is %x", Sectors1_2->csr1& 0xC0);
    puts("\n--- Port 1 Card is NOT of type output ---\n");
    exit(0);
E 3
  }
  else
  {
D 3
    fprintf(stderr, "Error copying file %s to filename %s",tempname, filename);
E 3
I 3
    return;
E 3
  }

D 3
}
E 3
I 3
  if((Sectors1_2->csr2 & 0xC0) != 0x40)
  {
    printf("\nSector 2CSR is %x", Sectors1_2->csr2& 0xC0);
    puts("\n--- Port 2 Card is NOT of type output ---\n");
    exit(0);	
  }
  else
  {
    return;
  }
E 3

D 3
/*********************************************************************/
E 3
I 3
  if((Sectors3_4->csr1 & 0xC0) != 0x40)
  {
    /*printf("\nSector 3CSR is %x", Sectors3_4->csr1& 0xC0);*/
    puts("\n--- Port 1 Card is NOT of type output ---\n");
    exit(0);
  }
  else
  {
    return;
  }
E 3

D 3
int
File_Copy(char *tempname, char *filename, FILE *fnew, FILE *ftemp)
{                        
  int b;
                   
  puts("\nSave file as: ");
  gets(filename);

  if((ftemp = fopen (tempname, "r")) == NULL)
E 3
I 3
  if((Sectors3_4->csr2 & 0xC0) != 0x40)
E 3
  {
D 3
    fprintf(stderr, "error opening tempfile %s", tempname);
    return(-1);
E 3
I 3
    /*printf("\nSector 4CSR is %x", Sectors3_4->csr2& 0xC0);*/
    puts("\n--- Port 2 Card is NOT of type output ---\n");
    exit(0);
E 3
  }
I 3
  else
  { 
    return;
  }
E 3

D 3
  if((fnew = fopen(filename, "w")) == NULL)
E 3
I 3
  if((Sectors5_6->csr1 & 0xC0) != 0x40)
E 3
  {
D 3
    fclose(ftemp);
    fprintf(stderr, "error opening file %s", filename);
    return -1;      
E 3
I 3
    /*printf("\nSector 5CSR is %x", Sectors5_6->csr1& 0xC0);*/
    puts("\n--- Port 1 Card is NOT of type output ---\n");
    exit(0);
E 3
  }
I 3
  else
  {
    return;
  }
E 3

D 3
  while(1)
  {                      
    b = fgetc(ftemp);

    if(!feof (ftemp))
    {
      fputc(b, fnew);
    }
    else
    {
      break;
    }
E 3
I 3
  if((Sectors5_6->csr2 & 0xC0) != 0x40)
  {
    /*printf("\nSector 6CSR is %x", Sectors5_6->csr2& 0xC0);*/
    puts("\n--- Port 2 Card is NOT of type output ---\n");
    exit(0);
E 3
  }
I 3
  else
  {
    return;
  }
E 3

D 3
  fclose(ftemp);                   
  remove(tempname);
  fclose(fnew);
E 3
I 3
}
E 3

D 3
  return(0);
}                  

E 3
/*********************************************************************/

long
D 3
Assign_VMEreadback()
E 3
I 3
Assign_OutputBoard(int Sector)
E 3
{
D 3
  long VMEAddress=0xFFFF8100;
E 3
I 3
  unsigned long ppc_offset;
E 3

D 3
  VMEflex_io=(struct majority_struct *)VMEAddress;
  CheckPort1_Input(VMEflex_io);
E 3
I 3
  sysBusToLocalAdrs(0x29,0,&ppc_offset);
E 3

I 3

E 3
}

/*********************************************************************/

D 3
void
CheckPort1_Input(struct majority_struct *VMEflex_io)
E 3
I 3
int
Check_Sector(int Sector)   
E 3
{
D 3
  if((VMEflex_io->csr1 & 0xC0) != 0x80)
  {
    puts("\nchecking the VME module for input type");	
    puts("\n--- Port 1 Card is NOT of type input ---\n");
    exit(0);	
  }
  else
  { 
    return;
  }
E 3
I 3
  unsigned long ppc_offset;
E 3

D 3
}
E 3
I 3
  sysBusToLocalAdrs(0x29,0,&ppc_offset);
E 3

D 3
/*********************************************************************/
E 3
I 3
  if((Sector==1)||(Sector==2)||(Sector==3)||
     (Sector==4)||(Sector==5)||(Sector==6))
  {   
    if((Sector == 1) || (Sector == 2))
    {
      /*printf("\nThe Sector assignment inside Assign_OutputBoard is: %d",Sector);*/
      Sectors1_2 = (struct majority_struct *) (ppc_offset+0x6100); /*0xFFFF6100*/
      /*printf("Sectors1_2 -> 0x%08x\n",Sectors1_2);*/
    }
    else if((Sector == 3) || (Sector == 4))
    {
      /*printf("\nThe Sector assignment inside Assign_OutputBoard is:  %d",Sector);*/
      Sectors3_4 = (struct majority_struct *) (ppc_offset+0x6200); /*0xFFFF6200*/
      /*printf("Sectors3_4 -> 0x%08x\n",Sectors3_4);*/
    }
    else if((Sector == 5) || (Sector == 6))
    {
      /*printf("\nThe Sector assignment inside Assign_OutputBoard is: %d",Sector);*/
      Sectors5_6 = (struct majority_struct *) (ppc_offset+0x6300); /*0xFFFF6300*/
      /*printf("Sectors5_6 -> 0x%08x\n",Sectors5_6);*/
    }
E 3

D 3
void
CheckPort2_Output (struct majority_struct *VMEflex_io)
{
  if((VMEflex_io->csr2 & 0xC0) != 0x40)
  {
    puts("\nchecking the VME module for output type");
    puts("\n--- Port 2 Card is NOT of type output ---\n");
    exit(0);	
E 3
I 3
    CheckSector_Output(Sectors1_2, Sectors3_4, Sectors5_6); 

E 3
  }
  else
  {
D 3
    return;
E 3
I 3
    printf("wrong sector # %d\n",Sector);
    return(-1);
E 3
  }
D 3
	
E 3
I 3

  return(0);
E 3
}

I 3

E 3
/*********************************************************************/

I 3







/*

E 3
int
Get_Data(int Sector, int Select, int Opposite, FILE *ftemp)
{
  int Data = 0;

  if(((Select == 0) || (Select == 1) || (Select == 2) ||
      (Select == 3) || (Select == 4) || (Select == 5) || 
      (Select == 6) || (Select == 7)) && ((Opposite == 0) ||
      (Opposite == 1)))
  {
    Make_DataWord(Sector, Select, Opposite);
    fprintf(ftemp,"\n%d %d %d",Sector,Select,Opposite); 
D 3
  } /* end if data correct */
E 3
I 3
  }
E 3
  else if(((Select != 0) || (Select != 1) || (Select != 2) || 
           (Select != 3) || (Select != 4) || (Select != 5) ||
           (Select != 6) || (Select != 7)) || ((Opposite != 0) ||
           (Opposite != 1)))
  {
    puts("\n\t ---Invalid Response--- \n");
    puts("Choose a valid logic level, N out of 6,");
    puts("and a valid Opposite slection.  Enter 1");
    puts("to enable or 0 to disable Opposite\n");
    printf("\nSector %d: \t", Sector);		
    scanf("%d%d", &Select, &Opposite);
    char_clear();

    if(((Select == 0) || (Select == 1) || (Select == 2)||
		(Select == 3) || (Select == 4) || (Select == 5)|| 
		(Select == 6) || (Select == 7)) &&
		((Opposite == 0) || (Opposite == 1)))
    {
      Make_DataWord(Sector, Select, Opposite);
      fprintf(ftemp,"\n%d %d %d",Sector,Select,Opposite);
D 3
    } /* end embedded if */
E 3
I 3
    }
E 3

    else
    {
      puts("\n\t ---Invalid Response--- \n");
      exit(1);
    }
D 3
  } /* end else if data !correct */
E 3
I 3
  }
E 3

}

D 3
/*********************************************************************/
E 3

D 3
int
VMEReadOutput(struct majority_struct *VMEflex_io)
E 3
I 3
void
char_clear(void)   
E 3
{
D 3
  int Level2_Or;

  VMEflex_io -> port1_data = Level2_Or; 

  printf("The final output is %hu", Level2_Or);

E 3
I 3
  char junk[80];
  gets(junk);
E 3
}


D 3

/*  Kelly Teague */
/*  mlu_main.o  */

/*  This is the modified driver which will be used to integrate the  */
/*  level2 Majority logic units into the Hall B data acquisition     */
/*  system. Modification of the original driver includes the removal */
/*  of its interface.  The only function required of this newly      */
/*  modifed driver is that it takes one parameter, which is the file */
/*  name of the MLU configuration file.  It then downloads the MLU   */
/*  configuration automatically, in order, to each of the six MLUs   */  

#define MAX 7
#define QUIT 5

E 3
int
D 3
KksDriver1()   
E 3
I 3
Get_Choice(void)
E 3
{
D 3
  FILE *fp;
  char *mystring=0;
  int Sector=0;
  char filename[50];
  int f1 = 0;
  int f2 = 0; 
  int f3 = 0;
  int crap;
	
  gets(filename);
 
  if ((fp = fopen(filename, "rb")) == NULL)   
   {
     fprintf(stderr, "error opening file %s.\n", filename);
     return -1;
   } /* end if fopen error */
E 3
I 3
  int selection = 0;
E 3

D 3
  while(!feof(fp))  
E 3
I 3
  do
E 3
  {
D 3
   for (Sector = 1; Sector < MAX; Sector++)  
   {
     fscanf(fp, "%d %d %d", &f1, &f2, &f3);
     Check_Sector(f1);	
     Write_Data_call(f1, f2, f3);
				
     if (Sector == MAX-1)   
	break;
     else  
	continue;
   } /* end for Sector++ */
  } /* end while !eof */	 
E 3
I 3
    puts("\n1 - Enter New Trigger");
    puts("2 - Load Trigger File");
    puts("3 - Save this File");
    puts("4 - Download to VME CPU");	
    puts("5 - Quit");	
    puts("\nChoose a trigger selection: ");
E 3

D 3
  if((fclose(fp))<0)
  {
    fprintf(stderr, "error closing file %s\n", filename);
    perror(mystring);
E 3
I 3
    scanf("%d", &selection);
    char_clear();
E 3
  }
I 3
  while ((selection < 1 || selection > 5));

  return selection;
E 3
}


int
KksDriver2()
{
  FILE *fnew, *ftemp;
  int Select, Opposite;
  int choice=0;
  int Sector=0;
  char filename[50], tempname[50];
  int f1 = 0;
  int f2 = 0;
  int f3 = 0;

  tmpnam(tempname);	

  choice = Get_Choice();

D 3
  while(choice == QUIT) break;
E 3
I 3
  while(choice == 5) break;
E 3

D 3
  while(choice != QUIT)
E 3
I 3
  while(choice != 5)
E 3
  {
    if(choice == 1)
    {
      if((ftemp = fopen(tempname, "w+")) == NULL)
      {
        fprintf(stderr, "error opening file.");
        return(-1);
      }

      puts("\n\tMajority Logic Remote Control\n");
      puts("For each Sector, select a Majority Logic, 0-7,");
      puts("and enable or disable the opposite function");
      puts("with either 0 or 1.  A logic selection of 0");
      puts("turns off a sector's output, while a 7 turns");	
      puts("on a sector's output.");
      puts("\n\t\tLogic  Opposite");

      for(Sector = 1; Sector<7; Sector++)
      {
        printf("\nSector %d: \t", Sector);
        scanf("%d%d", &Select, &Opposite);
        char_clear();
        Get_Data(Sector, Select, Opposite, ftemp);
      }

      Sector = 0;		
      fclose(ftemp);
      choice = Get_Choice();
    }
    else if(choice == 2)
    {
      puts("Enter the filename for the trigger: ");
      gets(filename);

      if((fnew = fopen(filename, "rb")) == NULL)
      {
        fprintf(stderr, "error opening file %s", filename);
        choice = Get_Choice();
      }
      else if((fnew = fopen(filename, "rb")) != NULL)
      {
        printf("\nSuccessfully opened file %s\n", filename);
        puts("Data to be downloaded is:");
        puts("\t\tLogic Opposite");

        while(!feof(fnew))
        {
          for(Sector = 1; Sector < 7; Sector++)
          {
            fscanf(fnew, "%d %d %d", &f1,&f2,&f3);
            printf("Sector %d:\t %d\t%d\n",f1,f2,f3);

            if(Sector == 7) break;
            else continue;
D 3
          } /* end for */
        } /* end while */
E 3
I 3
          }
        }
E 3

        choice = Get_Choice();
D 3
      } /* end else no fopen error */
    }  /* end if choice=2 */
E 3
I 3
      }
    }
E 3
    else if(choice == 3)
    {
D 3
      Save_File(tempname, filename, fnew, ftemp);
E 3
      choice = Get_Choice();
D 3
    } /* end if choice=3 */	
E 3
I 3
    }
E 3
    else if(choice == 4)
    {
      printf("\nFile to download is %s\n", filename);
      puts("Data is:");	
      puts("\t\tLogic Opposite");
D 3
      /*fseek(fnew, 0, SEEK_SET);*/
E 3

      while(!feof(fnew))
      {
        for(Sector = 1; Sector < 7; Sector++)
        {
          fscanf(fnew, "%d %d %d", &f1, &f2, &f3);
          printf("Sector %d:\t  %d\t %d\n",f1,f2,f3); 
          Check_Sector(f1);
          Write_Data_call(f1, f2, f3);

          if(Sector == 6) break;
          else continue;
D 3
        }/* end for Sector++ */
      } /* end while !eof */	 
E 3
I 3
        }
      } 
E 3
      fclose(fnew);  
      printf("\n\nClosed file %s\n", filename);

      choice = Get_Choice();

D 3
    } /* end if choice=4 */			
  } /* end while choice !quit */
} /* end main */
E 3
I 3
    }
  }
}
*/
E 3


I 3
/*
      Save_File(tempname, filename, fnew, ftemp);
*/




/* sets MLU using file $CLON_PARMS/trigger/current_level2_config.cfg,
   then uploads result into the file $CLON_PARMS/trigger/level2_config.out */

int
l2_mlu_setup()
{
  char *clonparms = "/usr/local/clas/parms";
  FILE *fd;
  char *mystring=0;
  int Sector=0;
  char filename[256];
  int f1 = 0;
  int f2 = 0; 
  int f3 = 0;
  int crap;
 
  /*if((clonparms = (char *)getenv("CLON_PARMS")) == NULL)
  {
    printf("ERROR: cannot get CLON_PARMS environment variable\n");
  }
  else*/


  /* DOWNLOAD */

  sprintf(filename,"%s/%s/%s",clonparms,"trigger","current_level2_config.cfg");
  printf("download l2 Majority logic configuration from >%s<\n",filename);
  if((fd = fopen(filename, "rb")) == NULL)   
  {
    printf("error opening file >%s<\n", filename);
    return(-1);
  }

  for(Sector=1; Sector<=6; Sector++)  
  {
    fscanf(fd, "%d %d %d", &f1, &f2, &f3);
    /*printf("read: %d %d %d\n",f1, f2, f3);*/

    Check_Sector(f1);	
    Write_Data_call(f1, f2, f3);
  }

  if((fclose(fd))<0)
  {
    printf("error closing file >%s<\n", filename);
  }
  else
  {
    printf("download completed.\n");
  }



  /* UPLOAD */

  sprintf(filename,"%s/%s/%s",clonparms,"trigger","level2_config.out");
  printf("upload l2 Majority logic configuration to >%s<\n",filename);
  if((fd = fopen(filename, "w")) == NULL)   
  {
    printf("error opening file >%s<\n", filename);
    return(-1);
  }

  for(Sector=1; Sector<=6; Sector++)  
  {
    Check_Sector(Sector);	
    Read_Data_call(Sector, &f2, &f3);

    /*printf("write: %d %d %d\n",Sector, f2, f3);*/
    fprintf(fd, "%d %d %d\n", Sector, f2, f3);
  }

  if((fclose(fd))<0)
  {
    printf("error closing file >%s<\n", filename);
  }
  else
  {
    printf("upload completed.\n");
  }


  return(0);
}
E 3
E 1
